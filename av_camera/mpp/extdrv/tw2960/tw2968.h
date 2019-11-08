#ifndef __TW2968_H__
#define __TW2968_H__

#include <linux/ioctl.h>
//#include "hi_type.h"


unsigned char __I2CReadByte8(unsigned char chip_addr, unsigned char reg_addr);
void __I2CWriteByte8(unsigned char chip_addr, unsigned char reg_addr, unsigned char value);


#define  I2CReadByte   __I2CReadByte8
#define  I2CWriteByte  __I2CWriteByte8



#define TW2968_NTSC	        1
#define TW2968_PAL	        2
#define TW2968_AUTO	        3
#define TW2968_INVALID_MODE	4

typedef struct _tw2968_video_norm
{
    unsigned int chip;
    unsigned int mode;
} tw2968_video_norm;


#define TW2968_1D1_MODE     0
#define TW2968_2D1_MODE     1
#define TW2968_4CIF_MODE    2
#define TW2968_4D1_MODE     3

typedef struct _tw2968_work_mode
{
    unsigned int chip;
    unsigned int mode;
} tw2968_work_mode;


typedef struct _tw2968_video_loss
{
    unsigned int chip;
    unsigned int ch;
    unsigned int is_lost;
} tw2968_video_loss;


#define TW2968_SET_HUE          0x01
#define TW2968_SET_CONTRAST     0x02
#define TW2968_SET_BRIGHT       0x04
#define TW2968_SET_SATURATION   0x08

typedef struct _tw2968_image_adjust
{
    unsigned char chip;
    unsigned char chn;
	unsigned char hue;
	unsigned char contrast;
	unsigned char brightness;
	unsigned char saturation;
	unsigned long item_sel;/* use such as "TW2968_SET_HUE|TW2968_SET_CONTRAST" */
} tw2968_image_adjust;


typedef struct _tw2968_audio_format
{
	unsigned char format;   /* 0:i2s; 1:dsp */
} tw2968_audio_format;

typedef struct _tw2968_audio_output
{
	unsigned char PlaybackOrLoop;   /*0:playback; 1:loop*/
	unsigned char channel;          /*[0, 15]*/
	unsigned char reserve[6];
} tw2968_audio_output;


typedef enum _tw2968_audio_samplerate
{
	TW2968_SAMPLE_RATE_8000,
	TW2968_SAMPLE_RATE_16000,
	TW2968_SAMPLE_RATE_32000,
	TW2968_SAMPLE_RATE_44100,
	TW2968_SAMPLE_RATE_48000,
	TW2968_SAMPLE_RATE_BUTT
} tw2968_audio_samplerate;

typedef struct _tw2968_audio_playback
{
    unsigned int chip;
    unsigned int chn;
} tw2968_audio_playback;

typedef struct _tw2968_audio_da_mute
{
    unsigned int chip;
} tw2968_audio_da_mute;

typedef struct _ttw2968_audio_da_volume
{
    unsigned int chip;
    unsigned int volume;
} tw2968_audio_da_volume;

typedef struct _tw2968_reg_val
{
	unsigned int RegVal;   /* 0xff--regVal; 0xff<<8:regAddr */
} tw2968_reg_val;

//芯片信息结构-用于提供给应用层检测芯片存在与否
typedef struct _tw2968_Chip_Info
{
	unsigned int chipNum;   /* 0--not find tw2968 chip; else return chip num detected */
} tw2968_Chip_Info;


#define TW2968_IOC_MAGIC            't'

#define TW2968CMD_READ_REG		      _IOWR(TW2968_IOC_MAGIC, 1, int)
#define TW2968CMD_WRITE_REG	        _IOW(TW2968_IOC_MAGIC, 2, int)

#define TW2968_GET_VIDEO_LOSS		    _IOWR(TW2968_IOC_MAGIC, 4, tw2968_video_loss)
#define TW2968_SET_IMAGE_ADJUST	    _IOW(TW2968_IOC_MAGIC, 5, tw2968_image_adjust)
#define TW2968_GET_IMAGE_ADJUST	    _IOWR(TW2968_IOC_MAGIC, 6, tw2968_image_adjust)
#define TW2968_SET_VIDEO_NORM	      _IOW(TW2968_IOC_MAGIC, 7, tw2968_video_norm)
#define TW2968_GET_VIDEO_NORM	      _IOWR(TW2968_IOC_MAGIC, 8, tw2968_video_norm)
#define TW2968_SET_WORK_MODE	      _IOW(TW2968_IOC_MAGIC, 9, tw2968_work_mode)

#define TW2968_SET_SAMPLE_RATE	    _IOW(TW2968_IOC_MAGIC, 10, tw2968_audio_samplerate)
#define TW2968_SET_AUDIO_PLAYBACK   _IOW(TW2968_IOC_MAGIC, 11, tw2968_audio_playback)
#define TW2968_SET_AUDIO_DA_MUTE    _IOW(TW2968_IOC_MAGIC, 12, tw2968_audio_da_mute)
#define TW2968_SET_AUDIO_DA_VOLUME  _IOW(TW2968_IOC_MAGIC, 13, tw2968_audio_da_volume)
#define TW2968_SET_AUDIO_FORMAT     _IOW(TW2968_IOC_MAGIC, 14, tw2968_audio_format)

#define TW2968_READ_REG_VAL     _IOW(TW2968_IOC_MAGIC, 15, tw2968_reg_val)
#define TW2968_WRITE_REG_VAL    _IOW(TW2968_IOC_MAGIC, 16, tw2968_reg_val)
#define TW2968_CHIP_INFO        _IOW(TW2968_IOC_MAGIC, 17, tw2968_Chip_Info)

int tw2968_set_audio_samplerate(tw2968_audio_samplerate samplerate);
int tw2968_set_audio_format(tw2968_audio_format audio_format);
#endif


