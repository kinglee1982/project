#include <linux/module.h>
//#include <asm/hardware.h>
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


#include "i2c.h"
#include "mcp4531.h"

int mcp4531_open(struct inode * inode, struct file * file)
{
	return 0;
}

int mcp4531_close(struct inode * inode, struct file * file)
{
	return 0;
}

int mcp4531_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned int __user *argp = (unsigned int __user *)arg;
	unsigned int volume = 0;


	printk("[mcp4531]:cmd =%d\n",cmd);
	switch (cmd)
	{
			case IOC_MCP4531_SET_MIC_VOLUME:
				copy_from_user(&volume, argp, sizeof(unsigned int));
				if(volume > 127)
					volume = 127;
				printk("[mcp4531]:set mic volume =%d\n",volume);

				SetMicDigitalPotentiometer(volume);
			break;
       		case IOC_MCP4531_SET_AUDIO_VOLUME:
				copy_from_user(&volume, argp, sizeof(unsigned int));
				if(volume > 127)
					volume = 127;
				printk("[mcp4531]:set audio volume =%d\n",volume);
				SetAudioOutDigitalPotentiometer(volume);
			break;
			case IOC_MCP4531_GET_MIC_VOLUME:
				ReadMicDigitalPotentiometer(&volume);
				printk("[mcp4531]:get mic volume =%d\n",volume);
				copy_to_user(argp, &volume, sizeof(unsigned int));
			break;
			case IOC_MCP4531_GET_AUDIO_VOLUME:
				ReadAudioOutDigitalPotentiometer(&volume);
				printk("[mcp4531]:get audio volume =%d\n",volume);
				copy_to_user(argp, &volume, sizeof(unsigned int));
			break;
		default:
			break;
	}
	return 0;
}

static struct file_operations mcp4531_fops = {
	.owner      = THIS_MODULE,
    //.ioctl      = mcp4531_ioctl,
    .unlocked_ioctl	= mcp4531_ioctl,
	.open       = mcp4531_open,
	.release    = mcp4531_close
};

static struct miscdevice mcp4531_dev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "mcp4531",
	.fops  		= &mcp4531_fops,
};
static int __init mcp4531_module_init(void)
{
	int ret = 0;

	ret = misc_register(&mcp4531_dev);
   	if (ret)
	{
		printk("ERROR: could not register mcp4531 devices");
	}

	return 0;
}



static void __exit mcp4531_module_exit(void)
{
	misc_deregister(&mcp4531_dev);
}

module_init(mcp4531_module_init);
module_exit(mcp4531_module_exit);

MODULE_LICENSE("GPL");

