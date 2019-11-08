
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

#define MCP4531_WRITE (0 << 2)
#define MCP4531_INCR  (1 << 2)
#define MCP4531_DECR  (2 << 2)
#define MCP4531_READ  (3 << 2)

spinlock_t  i2c_lock;
static struct i2c_client* ds3231_client;
static struct i2c_client* mcp4531_mic_client;
static struct i2c_client* mcp4531_audio_client;
static struct i2c_board_info ds3231_info =
{
    I2C_BOARD_INFO("ds3231", 0xD0),
};
static struct i2c_board_info mcp4531_mic_info =
{
    I2C_BOARD_INFO("mcp4531_mic", 0x5E),
};
static struct i2c_board_info mcp4531_audio_info =
{
    I2C_BOARD_INFO("mcp4531_audio", 0x5C),
};
void i2c_write(unsigned char chip_addr, unsigned char reg_addr, unsigned char value)
{
	int ret;
	struct i2c_client* client;
	if(chip_addr==ds3231_info.addr)
	{
		client = ds3231_client;
	}
	else if(chip_addr==mcp4531_mic_info.addr)
	{
		client = mcp4531_mic_client;
	}
	else if(chip_addr==mcp4531_audio_info.addr)
	{
		client = mcp4531_audio_client;
	}
	else
	{
		return;
	}
	unsigned char buf[2];
	spin_lock(&i2c_lock);
	//client->addr = chip_addr;

	buf[0] = reg_addr;
	buf[1] = value;

	ret = i2c_master_send(client, buf, 2);
	udelay(300);
	spin_unlock(&i2c_lock);
    //return ret;
}
EXPORT_SYMBOL(i2c_write);
unsigned char i2c_read(unsigned char chip_addr, unsigned char reg_addr)
{
    	int ret_data = 0xFF;
    	int ret;
    	struct i2c_client* client;
	if(chip_addr==ds3231_info.addr)
	{
		client = ds3231_client;
	}
	else if(chip_addr==mcp4531_mic_info.addr)
	{
		client = mcp4531_mic_client;
	}
	else if(chip_addr==mcp4531_audio_info.addr)
	{
		client = mcp4531_audio_client;
	}
	else
	{
		return 0;
	}
    	unsigned char buf[2];

    	//client->addr = chip_addr;

    	buf[0] = reg_addr;
    	ret = i2c_master_recv(client, buf, 1);
    	if (ret >= 0)
    	{
        	ret_data = buf[0];
    	}
    	return ret_data;
}

EXPORT_SYMBOL(i2c_read);

/*********************************************************
名称：SetMicDigitalPotentiometer
功能：设置麦克数字电位器音量值
输入：
    volume:    设定音量值，范围0 ~ 127
输出：
    =0：操作失败
    =1：操作成功
*/
unsigned char SetMicDigitalPotentiometer(unsigned int volume)
{

	struct i2c_client* client = mcp4531_mic_client;
	unsigned char buf[2];
	buf[0] = 0x00;
	buf[1] = volume;
	int ret = i2c_master_send(client, buf, 2);

    printk("in SetMicDigitalPotentiometer, send ret = %d\n",ret);

    return(1);
}

EXPORT_SYMBOL(SetMicDigitalPotentiometer);

static int _i2c_master_recv(const struct i2c_client *client, char *buf, int count)
{
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	int ret;

	msg.addr = client->addr;
#ifdef CONFIG_HI_I2C
	msg.flags = client->flags;
#else
	msg.flags = client->flags & I2C_M_TEN;
#endif
	msg.flags |= I2C_M_RD;

    msg.flags &= ~I2C_M_NOSTART;
    printk("i2c_transfer msg.flags = %x, I2C_M_NOSTART: %x\n",msg.flags,I2C_M_NOSTART);

	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(adap, &msg, count);
    printk("i2c_transfer ret = %d\n",ret);
	/*
	 * If everything went ok (i.e. 1 msg received), return #bytes received,
	 * else error code.
	 */
	return ret;
}
/*********************************************************
名称：ReadDigitalPotentiometer
功能：读取麦克数字电位器音量值
输入：
    *volume:    读取音量值放置单元指针，范围0 ~ 31
输出：
    =0：操作失败
    =1：操作成功
*/
unsigned char ReadMicDigitalPotentiometer(unsigned int *volume)
{

	struct i2c_client* client = mcp4531_mic_client;
    int ret = -1;

	unsigned char buf[2];
	buf[0] = 0x00;
    //buf[1] = 0x5F; //;0x00;
	//ret = i2c_master_send(client, buf, 1);
    //printk("in ReadMicDigitalPotentiometer, send ret = %d\n",ret);
	buf[0] = 0x00;
	buf[1] = 0;
	ret = _i2c_master_recv(client, buf, 2);
    //ret = i2c_smbus_read_i2c_block_data(client,0x00,2, buf);
    //ret = i2c_smbus_read_word_data(client,0x00);
	*volume = buf[1];

    printk("in ReadMicDigitalPotentiometer, recv ret = %d,buf[0]=%d,1=%d\n",ret, buf[0], buf[1]);

    return(1);
}




EXPORT_SYMBOL(ReadMicDigitalPotentiometer);




/*********************************************************
名称：SetAudioOutDigitalPotentiometer
功能：设置音频输出数字电位器音量值
输入：
    volume:    设定音量值，范围0 ~ 127
    发送给数字电位器的是反码
输出：
    =0：操作失败
    =1：操作成功
*/
unsigned char SetAudioOutDigitalPotentiometer(unsigned int volume)
{
   	struct i2c_client* client = mcp4531_audio_client;
	unsigned char buf[2];
	buf[0] = 0x00;
	buf[1] = volume;
	int ret = i2c_master_send(client, buf, 2);
    printk("in SetAudioOutDigitalPotentiometer, send ret = %d\n",ret);

    return(1);
}

EXPORT_SYMBOL(SetAudioOutDigitalPotentiometer);



/*********************************************************
名称：ReadAudioOutDigitalPotentiometer
功能：读取数字电位器音量值
输入：
    *volume:    读取音频输出电位器值放置单元指针，范围0 ~ 31
    *读取最后访问的存储地址命令
输出：
    =0：操作失败
    =1：操作成功
*/
unsigned char ReadAudioOutDigitalPotentiometer(unsigned int *volume)
{
    	struct i2c_client *client = mcp4531_audio_client;
   	unsigned char buf[2] = {0};
    s32 ret;
    buf[0] = 0x0C;
    buf[1] = 0x5D;
	ret = i2c_master_send(client, buf, 2);
    printk("in ReadAudioOutDigitalPotentiometer, send ret = %d\n",ret);

	buf[0] = 0x0C;
    buf[1] = 0x0;
	ret = i2c_master_recv(client, buf, 2);
	*volume = buf[1];

	//ret = i2c_smbus_read_word_swapped(client,
	//ret = i2c_smbus_read_block_data(client,
	//				  0x0C, buf);
	//*volume = ret;

    printk("in ReadAudioOutDigitalPotentiometer, recv ret = %d\n",ret);

    	return(1);
}


EXPORT_SYMBOL(ReadAudioOutDigitalPotentiometer);

static long i2c_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    unsigned int val;

	char device_addr, reg_addr;
	short reg_val;


	switch(cmd)
	{
		case I2C_READ:
			val = *(unsigned int *)arg;
			device_addr = (val&0xff000000)>>24;
			reg_addr = (val&0xff0000)>>16;

			reg_val = i2c_read(device_addr, reg_addr);
			*(unsigned int *)arg = (val&0xffff0000)|reg_val;
			break;

		case I2C_WRITE:
			val = *(unsigned int *)arg;
			device_addr = (val&0xff000000)>>24;
			reg_addr = (val&0xff0000)>>16;

			reg_val = val&0xffff;
			i2c_write(device_addr, reg_addr, reg_val);
			break;

		default:
			return -1;
	}
    return 0;
}

static int i2c_open(struct inode * inode, struct file * file)
{
    return 0;
}
static int i2c_close(struct inode * inode, struct file * file)
{
    return 0;
}

static int i2c_client_init(void)
{
    	struct i2c_adapter* i2c_adap;
    	i2c_adap = i2c_get_adapter(0);
    	ds3231_client = i2c_new_device(i2c_adap, &ds3231_info);
	mcp4531_mic_client = i2c_new_device(i2c_adap, &mcp4531_mic_info);
	mcp4531_audio_client = i2c_new_device(i2c_adap, &mcp4531_audio_info);
    	i2c_put_adapter(i2c_adap);

    return 0;
}

static void i2c_client_exit(void)
{
    	i2c_unregister_device(ds3231_client);
	i2c_unregister_device(mcp4531_mic_client);
	i2c_unregister_device(mcp4531_audio_client);
}

static struct file_operations i2c_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = i2c_ioctl,
    .open       = i2c_open,
    .release    = i2c_close
};


static struct miscdevice i2c_dev = {
   .minor		= MISC_DYNAMIC_MINOR,
   .name		= "i2c",
   .fops  = &i2c_fops,
};

static int __init i2c_init(void)
{
    int ret;
    //unsigned int reg;

    ret = misc_register(&i2c_dev);
    if(0 != ret)
    	return -1;
	i2c_client_init();
      spin_lock_init(&i2c_lock);
    return 0;
}

static void __exit i2c_exit(void)
{
    misc_deregister(&i2c_dev);
	i2c_client_exit();
}


module_init(i2c_init);
module_exit(i2c_exit);

MODULE_LICENSE("GPL");




