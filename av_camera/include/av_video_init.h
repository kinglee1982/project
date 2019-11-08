#ifndef __AV_VIDEO_INIT_H__
#define __AV_VIDEO_INIT_H__
#include "memfunc.h"
#define VENC_BUF_LEN 							(409600)	/*400k，单位BYTE*/
#define AENC_BUF_LEN							(1088)		/*单位BYTE*/
#define CHK_VENC_INTERVAL						(10000)		/*检查获取码流间隔，单位SEC*/

#define CHK_REC_STAT_INTL 					50

#define VPP_ONE_CHR_HEIGHT 					38  /* 字符所占的高度单位像素点 */
#define VPP_CHR_REAL_HEIGRH  					32 /* 字符实际所占的高度，单位像素点 */
#define VPP_ASCII_MAP_SIZE   					46  /* 常用ASCII表点阵数组中字符的个数 */
#define VPP_ONE_CHR_WIDTH  					17  /* 一个字符所占的宽度单位像素点 */
#define VPP_CHR_REAL_WIDTH 					16  /* 一个字符实际占用宽度，单位像素点 */
#define MAX_PIC_NUM     1000
#define SNAPINFO   "/mnt/nand/pic/snapinfo"
typedef struct FB_MARK_ST
{
	HI_S32 s32Fd;
	HI_CHAR *pcMemAddr;
	HI_U32 u32MemLen;
}FB_MARK_T;

HI_VOID APP_GetFbMark(FB_MARK_T **pFbMark);
HI_VOID APP_SetFbMark(FB_MARK_T **pFbMark);
void HI3521_SetCompany(HI_U8 u8Company,HI_U8 u8SubCompany);
HI_S32 HI3521_ViInit(HI_VOID);
HI_S32 HI3521_AVencInit(HI_VOID);
HI_S32 HI3521_StartAVEnc(HI_VOID);
HI_S32 HI3521_StartVencStream(HI_VOID);
HI_S32 APP_InitHiFb(HI_VOID);
HI_S32 HI3521_StopAVEnc(HI_VOID);
HI_S32 HI3521_VencExit(HI_VOID);
HI_S32 HI3521_ViExit(HI_VOID);
HI_S32 HI3521_StopAllVo(HI_VOID);
extern  HI_S32 HI3521_SnapShotByChn();
extern  HI_S32 HI3521_SetAutoBitrate(VENC_CHN VeChn,HI_S32 priority);
extern void HI3521_InitSysPtsBase();
extern HI_U64 HI3521_ConvertPts(HI_U64 hiPts);
extern HI_S32 HI3521_InitChnDataCenter();
extern HI_S32  HI3521_StartSubStreamVenc(VENC_CHN VeChn);
extern HI_S32  HI3521_StopSubStreamVenc(VENC_CHN VeChn);
#ifdef JHY_SDK
extern HI_S32 HI3521_JHY_Init(HI_VOID);
extern HI_S32 HI3521_StartJHY_Work(HI_VOID);
extern HI_S32 HI3521_JHY_GetStnInfo(HI_VOID *pStnInfo);
extern HI_S32 HI3521_JHY_SetStnInfo(HI_VOID *pStnInfo);

extern HI_S32 HI3521_JHY_GetDoorStnInfo(HI_VOID *pStnInfo);
extern HI_S32 HI3521_JHY_SetDoorStnInfo(HI_VOID *pStnInfo);

extern HI_BOOL HI3521_JHY_GetDoorTrigger();
extern HI_VOID HI3521_JHY_SetDoorTrigger(HI_BOOL bTrigger);

extern HI_S32 HI3521_JHY_GetLogicChn(HI_S32 s32Door);
#endif
extern HI_S32 HI3521_GetTw2968VideoType();
extern HI_VOID HI3521_InitTw2968VideoType();
extern HDC g_ChnDataCenter[25];
#endif
