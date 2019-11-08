#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//#include "gpio_i2c.h"
#include "nvp6114.h"
#include "video.h"
#include "coax_protocol.h"
#include "motion.h"
#include "common.h"

static struct i2c_board_info hi_info =
{
    I2C_BOARD_INFO("nvp6114", 0x60),
};

static struct i2c_client* nvp6114_client;
void __I2CWriteByte8(unsigned char chip_addr, unsigned char reg_addr, unsigned char value)
{
    int ret;
    unsigned char buf[2];
    struct i2c_client* client = nvp6114_client;

    nvp6114_client->addr = chip_addr;

    buf[0] = reg_addr;
    buf[1] = value;

    ret = i2c_master_send(client, buf, 2);
	udelay(300);
    //return ret;
}

unsigned char __I2CReadByte8(unsigned char chip_addr, unsigned char reg_addr)
{
    int ret_data = 0xFF;
    int ret;
    struct i2c_client* client = nvp6114_client;
    unsigned char buf[2];

    nvp6114_client->addr = chip_addr;

    buf[0] = reg_addr;
    ret = i2c_master_recv(client, buf, 1);
    if (ret >= 0)
    {
        ret_data = buf[0];
    }
    return ret_data;
}
#define HI_CHIPID_BASE 0x20050000
#define HI_CHIPID0 IO_ADDRESS(HI_CHIPID_BASE + 0xEEC)
#define HI_CHIPID1 IO_ADDRESS(HI_CHIPID_BASE + 0xEE8)
#define HW_REG(reg)         *((volatile unsigned int *)(reg))

int g_soc_chiptype=0x3531;

unsigned int vdec_mode = 1;  //0:ntsc, 1: pal
unsigned int vdec_mode_bak = 0xFF;
unsigned int chmode = 0x00;  //1 port 1ch mode
unsigned int vdec_cnt = 0;
unsigned int vdec_slave_addr[4] = {0x60, 0x62, 0x64, 0x66};
unsigned int outmode=NVP6114_OUT_ALL_720P;
module_param(outmode, uint, S_IRUGO);
module_param(chmode, uint, S_IRUGO);
module_param(vdec_mode, uint, S_IRUGO);
spinlock_t  nvp6114_lock;
extern void nvp6114_datareverse(void);

int check_id(unsigned int dec)
{
	int ret;
	I2CWriteByte(dec, 0xFF, 0x01);
	ret = I2CReadByte(dec, 0xf4);
	return ret;
}

int nvp6114_open(struct inode * inode, struct file * file)
{
	return 0;
}

int nvp6114_close(struct inode * inode, struct file * file)
{
	return 0;
}
int get_hichipid(void)
{
	g_soc_chiptype = (HW_REG(HI_CHIPID0)&0xFF)<<8 | (HW_REG(HI_CHIPID1)&0xFF);
	printk("g_soc_chiptype ==> %x\n", g_soc_chiptype);
	return g_soc_chiptype;
}
//int vdec_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
int nvp6114_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned int __user *argp = (unsigned int __user *)arg;
	unsigned int on;
	int i;
	unsigned char vloss=0;
	unsigned long ptz_ch;
	unsigned int value;
	unsigned int sens[16];
	vdec_video_mode vmode;
	vdec_video_adjust v_adj;


	switch (cmd)
	{
			case IOC_VDEC_GET_VIDEO_LOSS:
			spin_lock(&nvp6114_lock);
			{
				I2CWriteByte(vdec_slave_addr[1], 0xFF, 0x01);
				vloss=I2CReadByte(vdec_slave_addr[1], 0xD8)<<4;
				I2CWriteByte(vdec_slave_addr[0], 0xFF, 0x01);
				vloss|=I2CReadByte(vdec_slave_addr[0], 0xD8);
			}
			spin_unlock(&nvp6114_lock);
			//printk("IOC_VDEC_GET_VIDEO_LOSS ret[%x]\n", ret);
			if(copy_to_user(argp, &vloss, sizeof(unsigned char)))
				printk("IOC_VDEC_GET_VIDEO_LOSS error\n");
			break;
        case IOC_VDEC_SET_VIDEO_MODE:
            if (copy_from_user(&vmode, argp, sizeof(vdec_video_mode)))
				return -1;
			spin_lock(&nvp6114_lock);

			vdec_mode = vmode.mode;
			if(vdec_mode_bak != (vdec_mode%2))
			{
				/*第一次启动的时候初始化一次6114*/
				if(vdec_mode%2 == NTSC)
					nvp6114_720p_30fps();
				else
					nvp6114_720p_25fps();
				vdec_mode_bak = vdec_mode%2;
				printk("%%%%%%%%%%%%%%%%%%%%%%%%%init once is needed\n");
			}
			#if 1  //for customer
			if(outmode == NVP6114_OUT_ALL_960H)
			{
				nvp6114_960H_setting(vdec_mode&0x01, 0x0F);
				printk("NVP6114_OUT_ALL_960H\n");
			}
			else if(outmode == NVP6114_OUT_2X960_2X720P)
			{
				nvp6114_960H_setting(vdec_mode&0x01, 0x03);
				printk("NVP6114_OUT_2X960_2X720P\n");
			}
			else if(outmode == NVP6114_OUT_2X720P_2X960H)
			{
				nvp6114_960H_setting(vdec_mode&0x01, 0x0C);
				printk("NVP6114_OUT_2X720P_2X960H\n");
			}
			else
			{
				nvp6114_960H_setting(vdec_mode&0x01, 0x00);
				printk("NVP6114_OUT_4X720P\n");
			}
			#else //nextchip app use only....
			if(vmode.mode == VIDEO_NTSC_960_4CH)
			{
				outmode = NVP6114_OUT_ALL_960H;
				nvp6114_960H_setting(0, 0x0F);
				/////////////////4D1 mode output//////////////
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x32);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x88);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x02);
					I2CWriteByte(vdec_slave_addr[i], 0xCC, 0x66);
					I2CWriteByte(vdec_slave_addr[i], 0xCD, 0x66);
				}
			}
			else if(vmode.mode == VIDEO_PAL_960_4CH)
			{
				outmode = NVP6114_OUT_ALL_960H;
				nvp6114_960H_setting(1, 0x0F);
				/////////////////4D1 mode output//////////////
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x32);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x88);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x02);
					I2CWriteByte(vdec_slave_addr[i], 0xCC, 0x66);
					I2CWriteByte(vdec_slave_addr[i], 0xCD, 0x66);
				}
			}
			else if(vmode.mode == VIDEO_NTSC_960_720P)
			{
				outmode = NVP6114_OUT_2X960_2X720P;
				nvp6114_960H_setting(0, 0x03);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
				}
			}
			else if(vmode.mode == VIDEO_PAL_960_720P)
			{
				outmode = NVP6114_OUT_2X960_2X720P;
				nvp6114_960H_setting(1, 0x03);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
				}
			}
			else if(vmode.mode == VIDEO_NTSC_720P_960)
			{
				outmode = NVP6114_OUT_2X720P_2X960H;
				nvp6114_960H_setting(0, 0x0C);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
				}
			}
			else if(vmode.mode == VIDEO_PAL_720P_960)
			{
				outmode = NVP6114_OUT_2X720P_2X960H;
				nvp6114_960H_setting(1, 0x0C);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
				}
			}
			else if(vmode.mode == VIDEO_NTSC_720P_2CH)
			{
				outmode = NVP6114_OUT_ALL_720P;

				nvp6114_960H_setting(0, 0x00);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
					//I2CWriteByte(vdec_slave_addr[i], 0xCC, 0x66);
					//I2CWriteByte(vdec_slave_addr[i], 0xCD, 0x66);
				}

			}
			else if(vmode.mode == VIDEO_PAL_720P_2CH)
			{
				outmode = NVP6114_OUT_ALL_720P;

				nvp6114_960H_setting(1, 0x00);
				for(i=0;i<vdec_cnt;i++)
				{
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x00);
					I2CWriteByte(vdec_slave_addr[i], 0x56, 0x10);  //channel iD reset
					I2CWriteByte(vdec_slave_addr[i], 0xFF, 0x01);
					I2CWriteByte(vdec_slave_addr[i], 0xC0, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC1, 0x32);
					I2CWriteByte(vdec_slave_addr[i], 0xC2, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC3, 0x10);
					I2CWriteByte(vdec_slave_addr[i], 0xC8, 0x22);
					I2CWriteByte(vdec_slave_addr[i], 0xCA, 0x03);
				}
			}
			#endif
			spin_unlock(&nvp6114_lock);
			break;
#if 1
		case IOC_VDEC_SET_BRIGHTNESS:
            if(copy_from_user(&v_adj, argp, sizeof(vdec_video_adjust))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_video_set_brightness(v_adj.ch, v_adj.value, vdec_mode&1);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_SET_CONTRAST:
			if(copy_from_user(&v_adj, argp, sizeof(vdec_video_adjust))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_video_set_contrast(v_adj.ch, v_adj.value, vdec_mode&1);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_SET_HUE:
			if(copy_from_user(&v_adj, argp, sizeof(vdec_video_adjust))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_video_set_hue(v_adj.ch, v_adj.value, vdec_mode&1);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_SET_SATURATION:
			if(copy_from_user(&v_adj, argp, sizeof(vdec_video_adjust))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_video_set_saturation(v_adj.ch, v_adj.value, vdec_mode&1);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_SET_SHARPNESS:
            if(copy_from_user(&v_adj, argp, sizeof(vdec_video_adjust))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_video_set_saturation(v_adj.ch, v_adj.value, vdec_mode&1);
			spin_unlock(&nvp6114_lock);
			break;
#endif
		case IOC_VDEC_PTZ_PELCO_INIT:
			spin_lock(&nvp6114_lock);
			vdec_coaxial_init();
			pelco_coax_mode();
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_PTZ_PELCO_RESET:
			spin_lock(&nvp6114_lock);
			pelco_reset();
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_PTZ_PELCO_SET:
			spin_lock(&nvp6114_lock);
			pelco_set();
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_PTZ_CHANNEL_SEL:
            if (copy_from_user(&ptz_ch, argp, sizeof(ptz_ch)))
			{
				return -1;
			}
			extern unsigned char g_coax_ch;
			spin_lock(&nvp6114_lock);
			vdec_coaxial_select_ch(ptz_ch);
			#ifdef ADG708BRUZ
			g_coax_ch = 3;
			#else //74LV125
			g_coax_ch = ptz_ch;
			#endif
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_PTZ_PELCO_UP:
			spin_lock(&nvp6114_lock);
		 	pelco_up();
			spin_unlock(&nvp6114_lock);
		 	break;
		case IOC_VDEC_PTZ_PELCO_DOWN:
			spin_lock(&nvp6114_lock);
		 	pelco_down();
			spin_unlock(&nvp6114_lock);
		 	break;
		case IOC_VDEC_PTZ_PELCO_LEFT:
			spin_lock(&nvp6114_lock);
		 	pelco_left();
			spin_unlock(&nvp6114_lock);
		 	break;
		case IOC_VDEC_PTZ_PELCO_RIGHT:
			spin_lock(&nvp6114_lock);
			pelco_right();
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_PTZ_PELCO_FOCUS_NEAR:
			//FIXME
			spin_lock(&nvp6114_lock);
			pelco_osd();
			spin_unlock(&nvp6114_lock);
			//pelco_focus_near();
		 	break;
		case IOC_VDEC_PTZ_PELCO_FOCUS_FAR:
			//FIXME
			spin_lock(&nvp6114_lock);
			pelco_set();
			spin_unlock(&nvp6114_lock);
			//pelco_focus_far();
		 	break;
		case IOC_VDEC_PTZ_PELCO_ZOOM_WIDE:
			//pelco_zoom_wide();
			spin_lock(&nvp6114_lock);
			pelco_iris_open();
			spin_unlock(&nvp6114_lock);
		 	break;
		case IOC_VDEC_PTZ_PELCO_ZOOM_TELE:
			//pelco_zoom_tele();
			spin_lock(&nvp6114_lock);
			pelco_iris_close();
			spin_unlock(&nvp6114_lock);
			break;

		case IOC_VDEC_INIT_MOTION:
			spin_lock(&nvp6114_lock);
			vdec_motion_init();
			hi3520_init_blank_data(0);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_ENABLE_MOTION:
			break;
		case IOC_VDEC_DISABLE_MOTION:
			break;
		case IOC_VDEC_SET_MOTION_AREA:
			break;
		case IOC_VDEC_GET_MOTION_INFO:
			vdec_get_motion_info(0);
			break;
		case IOC_VDEC_SET_MOTION_DISPLAY:
            if(copy_from_user(&on, argp, sizeof(unsigned int))) return -1;
			spin_lock(&nvp6114_lock);
			vdec_motion_display(0,on);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_SET_MOTION_SENS:
            if(copy_from_user(&sens, argp, sizeof(unsigned int)*16)) return -1;
			spin_lock(&nvp6114_lock);
			vdec_motion_sensitivity(sens);
			spin_unlock(&nvp6114_lock);
			break;
		case IOC_VDEC_ENABLE_LOW_RES:
			//vdec_low_resoultion_enable(0xff);
			break;
		case IOC_VDEC_DISABLE_LOW_RES:
			//vdec_low_resoultion_disable(0xff);
			break;
		case IOC_VDEC_ENABLE_BW:
			//vdec_bw_detection_enable(0xff);
			break;
		case IOC_VDEC_DISABLE_BW:
			//vdec_bw_detection_disable(0xff);
			break;
		case IOC_VDEC_READ_BALCK_COUNT:
			//value = vdec_bw_black_count_read(0);
			//copy_to_user(arg,&value, sizeof(int));
			break;
		case IOC_VDEC_READ_WHITE_COUNT:
			break;
		case IOC_VDEC_4CH_VIDEO_SEQUENCE:
			break;


#if 0
        case IOC_VIDEO_GET_VIDEO_MODE:
		case IOC_VIDEO_SET_MOTION:
        case IOC_VIDEO_GET_MOTION:
		case IOC_VIDEO_SET_MOTION_EN:
		case IOC_VIDEO_SET_MOTION_SENS:
		case IOC_VIDEO_SET_MOTION_TRACE:
        case IOC_VIDEO_GET_VIDEO_LOSS:
		case IOC_VIDEO_GET_IMAGE_ADJUST:
        case IOC_AUDIO_SET_SAMPLE_RATE:
        case IOC_AUDIO_SET_AUDIO_PLAYBACK:
        case IOC_AUDIO_SET_AUDIO_DA_VOLUME:
		case IOC_AUDIO_SET_BRIGHTNESS:
		case IOC_AUDIO_SET_CONTRAST:
		case IOC_AUDIO_SET_HUE:
		case IOC_AUDIO_SET_SATURATION:
		case IOC_AUDIO_SET_SHARPNESS:
		case IOC_AUDIO_SET_AUDIO_MUTE:
		case IOC_AUDIO_SET_LIVE_CH:
		case IOC_AUDIO_SET_PB_CH:
#endif
		default:
            //printk("drv:invalid nc decoder ioctl cmd[%x]\n", cmd);
			break;
	}
	return 0;
}

static int i2c_client_init(void)
{
    struct i2c_adapter* i2c_adap;

    // use i2c2
    //i2c_adap = i2c_get_adapter(2);
    i2c_adap = i2c_get_adapter(0);
    nvp6114_client = i2c_new_device(i2c_adap, &hi_info);
    i2c_put_adapter(i2c_adap);

    return 0;
}

static void i2c_client_exit(void)
{
    i2c_unregister_device(nvp6114_client);
}


static struct file_operations nvp6114_fops = {
	.owner      = THIS_MODULE,
    .unlocked_ioctl	= nvp6114_ioctl,
	.open       = nvp6114_open,
	.release    = nvp6114_close
};

static struct miscdevice nvp6114_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "nvp6114",
	.fops  		= &nvp6114_fops,
};

int chip_id[4];


static int __init nvp6114_module_init(void)
{
	int ret = 0, i = 0;
    unsigned char i2c_addr;

	ret = misc_register(&nvp6114_dev);

   	if (ret)
	{
		printk("ERROR: could not register NC Decoder devices:%#x \n",i);
	}
	///nvp6114_reset();
	printk("NVP6114 Test Driver 2014.09.15\n");
	i2c_client_init();
	for(i=0;i<4;i++)
	{
		chip_id[i] = check_id(vdec_slave_addr[i]);
		if( (chip_id[i] != NVP6114_R0_ID ) )
		{
			printk("nvp6114 Device ID Error... %x\n", chip_id[i]);
		}
		else
		{
			printk("nvp6114 Device (0x%x) ID OK... %x\n", vdec_slave_addr[i], chip_id[i]);
			vdec_cnt++;
		}
	}


	printk("NVP6114 Count = %x\n", vdec_cnt);

	#if 1
	if(outmode < 4)
	{
		if(vdec_mode_bak != (vdec_mode%2))
		{
			/*第一次启动的时候初始化一次6114*/
			if(vdec_mode%2 == NTSC)
				nvp6114_720p_30fps();
			else
				nvp6114_720p_25fps();
			vdec_mode_bak = vdec_mode%2;
		}

		if(outmode == NVP6114_OUT_ALL_960H)
		{
			nvp6114_960H_setting(vdec_mode&0x01, 0x0F);
			printk("NVP6114_OUT_ALL_960H\n");
		}
		else if(outmode == NVP6114_OUT_2X960_2X720P)
		{
			nvp6114_960H_setting(vdec_mode&0x01, 0x03);
			printk("NVP6114_OUT_2X960_2X720P\n");
		}
		else if(outmode == NVP6114_OUT_2X720P_2X960H)
		{
			nvp6114_960H_setting(vdec_mode&0x01, 0x0C);
			printk("NVP6114_OUT_2X720P_2X960H\n");
		}
		else
		{
			nvp6114_960H_setting(vdec_mode&0x01, 0x00);
			printk("NVP6114_OUT_ALL_720P\n");
		}
	}
	//if(get_hichipid() == 0x3520)  //hi3520d
		//nvp6114_datareverse();
	#endif
	spin_lock_init(&nvp6114_lock);
	return 0;
}



static void __exit nvp6114_module_exit(void)
{
	misc_deregister(&nvp6114_dev);
	i2c_client_exit();
}

module_init(nvp6114_module_init);
module_exit(nvp6114_module_exit);

MODULE_LICENSE("GPL");

