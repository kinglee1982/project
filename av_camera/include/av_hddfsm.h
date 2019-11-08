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

#define DISK_EVENT_FLAG_REC  0x1 /*录像*/
#define DISK_EVENT_FLAG_REC_PB 0x2  /*录像回放 */
#define DISK_EVENT_FLAG_DOWNLOAD 0x4 /*录像下载*/
#define DISK_EVENT_FLAG_RPB 0x8     /*远程回放 */
#define DISK_EVENT_FLAG_USB_PB 0x10 /*USB备份 */


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
	DISK_STAT_WORKING,     /*进入该状态WORK后开始进行时间处理 */
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
  	HI_BOOL bStart;			/*事件是否使能*/
	HI_S32 s32EventCnt;		/*多个相同事件时，表示事件的个数；只有一个事件时，事件个数为1*/
}HDD_EVENT_THD_ST;

typedef struct _HDD_FSM_T_
{
    HI_S32 s32HddStat;
	HI_S32 s32MountFailTimes; /*挂载失败次数 */
	HI_S32 s32HddUnLockEvent; /*Unlock 初始化为0 有开锁事件为 1*/
	HI_S32 s32HddAbnormal;	/*单片机给UI的状态，硬盘准备好为0没好为1
                              UI在开机的时候会发一次 */
    HI_S32 s32FormatFlag;   /*1格式化开启 0格式化完成 */
    HI_S32 s32RebootFlag;   //重启系统标记
	HI_S32 s32LPBFLag;		/*0未回放 1开始回放 2停止回放*/


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

