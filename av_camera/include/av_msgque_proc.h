/******************************************************************************

                  ��Ȩ���� (C), 2001-2024, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : av_msgque_proc.h
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2012��6��1��������
  ����޸�   :
  ��������   : drv_app_msgqueproc.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��1��������
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __AV_MSGQUE_PROC_H__
#define __AV_MSGQUE_PROC_H__


#define  APP_MSG_QUE_PRIORITY     3       /* ���ȼ����� */
#define  APP_MSG_QUE_CNT          16      /* ÿ����Ϣ�������ɵ������Ϣ���� */
#define  APP_MSG_QUE_DATA_SIZE    1044    /* ������Ϣ���ݳ��� */

typedef struct APP_MSG_QUE_CTLDATA
{
    HI_S32 s32Counter;               /* ��Ϣ���� */
    HI_S32 s32NewestIndex;           /* ���½��յ�����Ϣ�±�ֵ */
    HI_S32 s32OldestIndex;           /* ��ɽ��յ�����Ϣ�±�ֵ */
}APP_MSG_CTLDATA_S;

extern HI_S32 APP_MsgQueModeInit(HI_VOID);
extern HI_VOID *APP_RcvAudioMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_SendAudioMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_RcvVideoMsgQueThread(HI_VOID *pArg);
extern HI_VOID *APP_SendVideoMsgQueThread(HI_VOID *pArg);
extern HI_S32 HI3521_MsgQueModeInit(HI_VOID);
extern HI_S32 HI3521_CreateAiMsgQueRcvThread(HI_VOID);
extern HI_S32 HI3521_CreateViMsgQueRcvThread(HI_VOID);
extern HI_S32 HI3521_CreateAVMsgQueSndThread(HI_VOID);

#define TEST_MSGQ
#ifdef TEST_MSGQ
/*������Ϣ���в��Խӿ� */
extern HI_VOID  HI3531_SndMsgQueData(HI_S32 s32MainType , HI_S32 s32MinorType);
#endif

#endif /* __AV_MSGQUE_PROC_H__ */

