#ifndef __HDD_FSM_H__
#define __HDD_FSM_H__

#define MAX_EVENT_NUMS 10
#define EVENT_START 1
#define EVENT_STOP 0

#define HARD_DISK_ERROR 0
#define HARD_DISK_OK 1
#define HARD_DISK_REMOVABLE	(1)
#define HARD_DISK_UNREMOVABLE (0)
#define HARD_DISK_MIN_SIZE		(100)

#define DISK_EVENT_FLAG_REC  0x1 /*¼��*/
#define DISK_EVENT_FLAG_REC_PB 0x2  /*¼��ط� */
#define DISK_EVENT_FLAG_DOWNLOAD 0x4 /*¼������*/
#define DISK_EVENT_FLAG_RPB 0x8     /*Զ�̻ط� */
#define DISK_EVENT_FLAG_USB_PB 0x10 /*USB���� */


typedef enum LPB_CTRL_EN
{
	LPB_UNSTART = 0,
	LPB_START,
	LPB_STOP,
	LPB_EXIT,
	LPB_BUFF = -1,
}LPB_CTRL_E;


/*DISK stat */
typedef enum _DISK_STAT_E_
{
	DISK_STAT_NOT_READY = -1,
	DISK_STAT_READY,
	DISK_STAT_TOMOUNT,
	DISK_STAT_MOUNTING,
	DISK_STAT_TOUMOUNT,
	DISK_STAT_UMOUNTING,
	DISK_STAT_MOUNT_SUCC,
	DISK_STAT_MOUNT_FAILED,
	DISK_STAT_WORKING,     /*�����״̬WORK��ʼ����ʱ�䴦�� */
	DISK_STAT_FORMATING, /*HDD FORMATING*/
	DISK_STAT_PART_ERROR = 0xFF,
	DISK_STAT_BUFF = -1,
}DISK_STAT_E;

/*DISK EVENT */
typedef enum _DISK_EVENT_E_
{
	DISK_EVENT_REC = 0,
	DISK_EVENT_REC_LPB,
	DISK_EVENT_REC_DOWNLOAD,
	DISK_EVENT_REC_RPB, /*REMOTE PALYBACK*/
	DISK_EVENT_USB_BK,
	DISK_EVENT_ALARMREC_BK,
	DISK_EVENT_BUFF = -1,
}DISK_EVENT_E;

typedef struct _HDD_EVENT_THD_T_
{
  	pthread_t tid;
	pthread_mutex_t mutex;
  	HI_BOOL bStart;			/*�¼��Ƿ�ʹ��*/
	HI_S32 s32EventCnt;		/*�����ͬ�¼�ʱ����ʾ�¼��ĸ�����ֻ��һ���¼�ʱ���¼�����Ϊ1*/
}HDD_EVENT_THD_ST;

typedef struct _HDD_FSM_T_
{
    HI_S32 s32HddStat;
	HI_S32 s32MountFailTimes; /*����ʧ�ܴ��� */
	HI_S32 s32HddUnLockEvent; /*Unlock ��ʼ��Ϊ0 �п����¼�Ϊ 1*/
	HI_S32 s32HddAbnormal;	/*��Ƭ����UI��״̬��Ӳ��׼����Ϊ0û��Ϊ1
                              UI�ڿ�����ʱ��ᷢһ�� */
    HI_S32 s32FormatFlag;   /*1��ʽ������ 0��ʽ����� */
    HI_S32 s32RebootFlag;   //����ϵͳ���
	HI_S32 s32LPBFLag;		/*0δ�ط� 1��ʼ�ط� 2ֹͣ�ط�*/


    HDD_EVENT_THD_ST stEventThd[MAX_EVENT_NUMS];
}HDD_FSM_ST;

typedef struct EVENT_INFO_T
{
    HI_S32 s32EventType;
    HI_U32 s32StartTime;
    HI_U32 s32EndTime;
    HI_U8  res[8];
}EVENT_INFO;

extern HI_S32 HDD_ChkDevRemoveable(HI_CHAR *pDevPath);
extern HI_VOID HDD_SetDiskAbnormal(HI_S32 s32Abnormal);
extern HI_S32 HDD_GetDiskAbnormal();
extern HI_VOID HDD_SetRebootFlag(HI_S32 s32RebootFlag);
extern HI_S32 HDD_GetDiskStat(HI_VOID);
extern HI_S32 HDD_StopLPB(HI_VOID);
extern HI_S32 HDD_StartLPB(HI_VOID);
extern HI_BOOL HDD_IsLPBRunning(HI_VOID);
extern HI_S32 HDD_StopRec(HI_VOID);
extern HI_S32 HDD_StartRec(HI_VOID);
extern HI_S32 HDD_GetDiskFSM(HI_VOID);
extern HI_S32 HDD_GetHddUnLockEvent(HI_VOID);
extern HI_VOID HDD_SetHddUnLockEvent(HI_BOOL bUnLock);
extern HI_VOID HDD_SetHddEvent(DISK_EVENT_E eDiskEvent, HI_BOOL bStart);
extern HI_BOOL HDD_GetHddEvent(DISK_EVENT_E eDiskEvent);
extern HI_S32 HDD_DetectHddMount(HI_VOID);
extern HI_VOID AV_PRINT_ONE(HI_CHAR *pMsg, HI_S32 s32MsgId);
extern HI_VOID HDD_WaitExitLPB(HI_VOID);
extern HI_S32 HDD_ExitLPB(HI_VOID);
extern HI_S32 HDD_GetSdStat(HI_VOID);
extern HI_VOID HDD_SetAlarmTrigger(HI_BOOL bTrigger);
extern HI_S32 HDD_FindDev(HI_VOID);
extern HI_S32 HDD_UmountDisk(HI_S32 s32Times);
extern HI_VOID HDD_Init(HI_VOID);
#endif

