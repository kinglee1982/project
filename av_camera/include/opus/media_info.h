#ifndef MEDIA_INFO_H
#define MEDIA_INFO_H

#include <string.h>



////������������
typedef unsigned char            uint8_t;
typedef unsigned short           uint16_t;
typedef long long                int64_t;
typedef unsigned long long       uint64_t;
typedef unsigned int             uint32_t;

typedef enum
{
	JPLAYER_SOFT_DECODER = 0,
	JPLAYER_OMX_DECODER,
	JPLAYER_XBMC_DECODER
}JPlayer_Decoder_TYPE;


///ý�����ݷ��ͽṹ
typedef struct
{
	uint8_t* data;      ///ý������
	int      size;      ///ý�����ݳ���
	int      flag;      ///ý��֡����
	int      seq;       ///֡���
	int      isKey;     ///�Ƿ��ǹؼ�֡
	uint64_t timestamp; ///����֡������ʱ�䣬milliseconds
}StreamPacket;




///ý������֡����
typedef enum
{
	JPLAYER_FRAME_PCM      = 1,
	JPLAYER_FRAME_YUV420P  = 2,
	JPLAYER_FRAME_YUV12    = 4,
	JPLAYER_FRAME_AUDIO    = 8,
	JPLAYER_FRAME_H264     = 16,

	JPLAYER_FRAME_LENGTH
}JPlayer_FrameType;







#endif

