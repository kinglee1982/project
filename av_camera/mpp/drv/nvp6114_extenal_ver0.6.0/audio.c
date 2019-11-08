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

#include "nvp6114.h"

/*
param:
dec[4]= {0x60,0x62,0x64,0x66};
ch_num: audio channel number
samplerate: 0[8k], 1[16k]
bits: 0[16bits], 1[8bits]
*/

void audio_init(unsigned char dec, unsigned char ch_num, unsigned char samplerate, unsigned char bits)
{
	I2CWriteByte(dec, 0xff, 0x01);
	if( (dec == 0x60) || (dec == 0x64))
	{
		I2CWriteByte(dec, 0x06, 0x0a);
		I2CWriteByte(dec, 0x07, 0x80|(samplerate<<3)|(bits<<2));	//Rec I2C 8K 16bit : master
		if(8 == ch_num)
		{
		//	I2CWriteByte(dec, 0x06, 0x1b);
			I2CWriteByte(dec, 0x08, 0x03);
			I2CWriteByte(dec, 0x0c, 0x98);    //set I2S right sequence
			I2CWriteByte(dec, 0x0d, 0xba);
			I2CWriteByte(dec, 0x0e, 0x03);
			I2CWriteByte(dec, 0x0f, 0x10);    //set I2S right sequence
		}
		else if(4 == ch_num)
		{
			I2CWriteByte(dec, 0x06, 0x0b);
			I2CWriteByte(dec, 0x08, 0x01);
			I2CWriteByte(dec, 0x0f, 0x32);   //set I2S right sequence
		}
		I2CWriteByte(dec, 0x13, 0x80|(samplerate<<3)|(bits<<2));	// PB I2C 8k 16bit : master
		I2CWriteByte(dec, 0x23, 0x10);  // Audio playback out
		//I2CWriteByte(dec, 0x23, 0x18);  // Audio mix out
	}
	else
	{
		I2CWriteByte(dec, 0x06, 0x19);
		I2CWriteByte(dec, 0x07, 0x00|(samplerate<<3)|(bits<<2));	//Rec I2C 16K 16bit : slave
		I2CWriteByte(dec, 0x13, 0x00|(samplerate<<3)|(bits<<2));	// PB I2C 8k 16bit : slave
	}
	//I2CWriteByte(dec, 0x3C, 0x10);
	//I2CWriteByte(dec, 0x3D, 0x11);

//	I2CWriteByte(dec, 0x24, 0x14); //set mic_1's data to i2s_sp left channel
//	I2CWriteByte(dec, 0x25, 0x15); //set mic_2's data to i2s_sp right channel
}

