#ifndef __AV_MP3_PLAY_H__
#define __AV_MP3_PLAY_H__

//MP3播放返回值
#define MP3_RET_SUCCESS    0  //
#define MP3_RET_ERROR     -1
#define MP3_RET_INTERRUPT -2  //打断返回

extern HI_S32 APP_StopIPSpeak(HI_VOID);
extern HI_S32 APP_StartIPSpeak(HI_VOID);
extern HI_S32 APP_AudioMsgProc(MSG_QUE_BUF_DATA_T *pstRcvMsg, MSG_QUE_T *pstSendMsg);
extern HI_S32 APP_UnstartAO(AUDIO_DEV AoDevId, AO_CHN AoChnCnt);
extern HI_S32 APP_StartAo(AUDIO_DEV AoDevId, AO_CHN AoChnCnt, AIO_ATTR_S *pstAioAttr);
extern HI_S32 MP3_GetGlobalMemData(const HI_VOID *piGlobMem, HI_VOID *pData, pthread_rwlock_t *pstLock, HI_S32 s32Len);
extern HI_S32 MP3_SetGlobalMemData(HI_VOID *piGlobMem, const HI_VOID *pData, pthread_rwlock_t *pstLock, HI_S32 s32Len);



#endif

