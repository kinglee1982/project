#ifndef __AV_SPLIT_H__
#define __AV_SPLIT_H__

#include "av_capture.h"
#include "av_sys.h"

#define MAX_ANALOG_VO_CHN 12
#define MIN_ZOOM_WIDTH	128

#define DATA_ALIGN(data,bit)	( ((data) >> (bit)) << (bit) )
#ifndef MAX
#define MAX(a,b)				(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)				(((a) < (b)) ? (a) : (b))
#endif

/*画面预览分割的几个模式 */
typedef enum preview_split_mode
{
	PREVIEW_CHN1 = 1,
	PREVIEW_CHN2 ,
	PREVIEW_CHN3 ,
	PREVIEW_CHN4 ,
	PREVIEW_CHN5 ,
	PREVIEW_CHN6 ,
	PREVIEW_CHN7 ,
	PREVIEW_CHN8 ,
	PREVIEW_CHN9,
	PREVIEW_CHN10,
	PREVIEW_CHN11,
	PREVIEW_CHN12 = 12,
	PREVIEW_4_CHN_SCREEN1 = 13,		/*4画面 */
	PREVIEW_4_CHN_SCREEN2 = 14,		/*4画面 */
	PREVIEW_4_CHN_SCREEN3 = 15,
	PREVIEW_9_CHN_SCREEN = 16,       /*9画面 */
	PREVIEW_NR = -1,
}preview_split_mode_e;

/*画面分割设置配置的结构*/
    /*
    MODE:PREVIEW_4_CHN_SCREEN       INDEX:0(chn[0~3]),1(chn[4~7])
    MODE:PREVIEW_9_CHN_SCREEN       INDEX:0(chn[0~8])
    MODE:单                         INDEX:x(chnx(x[0])
    */
typedef struct tagSPLIT_MODE
{
	HI_U32 dwMode;
	HI_U32 dwIndex;
	HI_U32 Rev[4];
}SPLIT_MODE;

typedef struct tagSPLIT_CHN
{
	HI_S32 binded;
	HI_S32 audioen;
	HI_S32 vichn;
	HI_S32 videv;
	HI_S32 vochn;
	HI_S32 vdechn;
	HI_S32 vpsschn;
	HI_S32 zoombinded;
	//VIDEO_RECT rect;
	VIDEO_RECT saverect;
	//RECT_S zoom_rect[MAX_VO_DEV];
	PIC_SIZE_E videosize[2];
}SPLIT_CHN;

typedef struct tagSPLIT_STATUS
{
	HI_S32 mode;
	HI_S32 index;
}SPLIT_STATUS;

typedef struct tagSPLIT_DEV
{
	HI_S32 vstd;
	HI_S32 enable;
	HI_S32	deflick;
	SPLIT_STATUS status;
	SPLIT_CHN split_chn[MAX_ANALOG_VO_CHN];
	pthread_mutex_t lock;
}SPLIT_DEV;


extern SPLIT_DEV *HI3521_GetSplitDev(HI_VOID);


#if 0
HI_VOID HI3531_SplitDisable(HI_VOID);
extern HI_S32 HI3531_SplitChnDisable(HI_S32 s32Chn);
extern enum capture_size_t HI3531_GetCapVideoSize(HI_S32 s32Chn,HI_S32 s32Mode);
extern HI_S32 HI3531_SplitDev_Init(HI_VOID);
extern HI_S32 HI3531_SplitChnSet(SPLIT_MODE* pstSplitMode);
extern HI_S32 HI3531_GetSplitRectCfg(SPLIT_MODE * pstSplitMode, RECT_S * pstRect, VO_CHN_ATTR_S * pstVoChnAttr);
extern HI_VOID HI3531_StartDefaultPreview(HI_VOID);
extern HI_S32 HiSplitResetPlayZoom(HI_VOID);
extern HI_VOID Hi3531_SwitchPreview(HI_S32 s32PreviewType);
#endif
PIC_SIZE_E HI3521_GetCapVideoSize(HI_S32 s32Chn,HI_S32 s32Mode);
HI_S32 Hi3521_SwitchPreview(HI_S32 s32PreviewType);
DISTIBUTE_VPSS *HI3521_GetVpssInfo(HI_VOID);
HI_S32 HI3521_SplitDev_Init(HI_VOID);
HI_VOID HI3521_StartDefaultPreview(HI_VOID);
HI_VOID HI3521_Split_ViDisable(HI_VOID);
HI_S32 HI3521_VdecChnInit(SPLIT_MODE* pstSplitMode);
HI_S32 HI3521_VdecChnDisable();
HI_VOID Hi3521_SwitchVdecChn(HI_S32 chn);
#endif	//__AVDEC_H__
