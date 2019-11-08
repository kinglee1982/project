/*************************************************************************
	> File Name: include/audio_init.h
	> Author:
	> Mail:
	> Created Time: Tue 24 May 2016 02:32:29 PM UTC
 ************************************************************************/
#ifndef __AUDIO_INIT_H_
#define __AUDIO_INIT_H_

#include "av_audio_public_define.h"

//使用adpcm音频编码格式
#define AENC_PCM    0
#define AENC_ADPCM  1
#define AENC_G711 2

#define AENC_TYPE   AENC_G711

#define AUDIO_PREVIEW_START 1
#define AUDIO_PREVIEW_STOP  0

#define DEFAULT_DATA_PER_FRAM   (576)

//音频码流获取回调函数
typedef void (*AudioFrameInvocation) (AUDIO_STREAM_S *, HI_S32);






HI_VOID HI3521A_AiPubAttrInit(AIO_ATTR_S * pstAttr);
HI_VOID HI3521A_AoPubAttrInit(AIO_ATTR_S * pstAttr);
HI_U32  HI3521A_AiInit(HI_VOID);
HI_S32  HI3521A_AiExit(HI_VOID);
HI_U32  HI3521A_AoInit(HI_U32 u32PtNumPerFrm);
HI_S32  HI3521A_AoExit(HI_VOID);
HI_S32  HI3521A_SetAudioPreview(AI_CHN AiNum,HI_BOOL bIsPreview);
HI_S32  HI3521A_Comm_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32  HI3521A_Comm_AoUnBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32  HI3521A_Comm_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32  HI3521A_Comm_AencUnBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32  HI3521A_AencInit(HI_S32 s32Chn, HI_S32 s32AeChn,HI_S32 sAiDev);
HI_S32  HI3521A_AencExit(HI_VOID);
HI_S32  HI3521A_AdecInit(HI_VOID);
HI_S32  HI3521A_AdecExit(HI_VOID);
HI_S32  HI3521A_StartAencStream( );
HI_VOID HI3521A_GetAencFd(HI_VOID);
HI_BOOL HI3521A_IsAencFdValid(HI_VOID);
HI_S32  HI3521A_GetAencStream(HI_VOID);
HI_VOID HI3521A_SetAudioInvocation(AudioFrameInvocation sFun);
HI_VOID*HI3521A_GetAencStreamThd(HI_VOID *arg);
HI_VOID HI3521A_VoIPEnable();
HI_VOID HI3521A_VoIPDisable();

#endif

