#ifndef __AV_SYS_H__
#define __AV_SYS_H__
#include "hi_type.h"
#include "hi_comm_video.h"
#define		NO_CHIP		    0
#define		HI3511_CHIP	    1
#define		HI3512_CHIP	    2
#define		HI3520_CHIP	    3

#define		ENCODE_HI3511	1
#define		ENCODE_HI3512	2
#define		ENCODE_HI3520	3
#define		ENCODE_HI3515   4
#define		ENCODE_HI3531   5
#define		ENCODE_HI3520a  6
#define		ENCODE_HI3521   7

#define    PRODUCT_ID                   0
#define    SLAVE_CHIP_NUM               2
#define    TOTAL_VI_CHN_NUM             12
#define    TOTAL_VDEC_CHN_NUM             4
#define    VI_CHN_NUM_PER_SLAVE_CHIP    8
#define    VI_CHN_NUM_HOST_CHIP         12
#define    TOTAL_VENC_CHN_NUM           24
#define    VENC_CHN_NUM_PER_SLAVE_CHIP  0
#define    VENC_CHN_NUM_HOST_CHIP       24
#define    TOTAL_AI_CHN_NUM             16
#define    AI_CHN_NUM_PER_SLAVE_CHIP    0
#define    AI_CHN_NUM_HOST_CHIP         8
#define    TOTAL_AENC_CHN_NUM           16
#define    AENC_CHN_NUM_PER_SLAVE_CHIP  0
#define    AENC_CHN_NUM_HOST_CHIP       16
#define    VIDEO_AD_MODE                0
#define    HOST_CHIP                    ENCODE_HI3515
#define    SLAVE_CHIP                   ENCODE_HI3515
#define    AENC_CHN_OFFSET              0
#define    JPEG_CHN_OFFSET              16
#define    TOTAL_JPEG_CHN_NUM           0
#define    JPEG_CHN_NUM_HOST_CHIP       8
#define    MOSAIC_ENC_CHN_OFFSET_HOST_CHIP      24
#define    MAX_VIDEO_SIZE               CAPTURE_SIZE_720P //debug use D1 20141117, by xzhang
//#define    MAX_VIDEO_SIZE   			CAPTURE_SIZE_D1
#define    VDEC_CHN_NUM                 4                   //实际的解码通道数
#define    MOSAIC_VENC_ENABLE           0
#define    MOSAIC_VENC_CHN              0
#define    EXT_VENC_ENABLE              0
#define    TOTAL_VO_CHN_NUM             16
#define    SAMPLE_CHIP_CON_MODE         0
#define    OEM_TYPE                     0
#define    VENC_MODE                    0
#define    HDEC_CHN_NUM                 0			// 应用上设计的总的解码通道数，包括数字通道加回放通道，实际库里用同一份解码通道，节省性能
#define    HVR_TOTAL_CHN_NUM            0	        // 本地數字通道+本地模拟通道
#define    HVR_PLAY_DECODE_NUM          0           //回放通道数
#define    HVR_DIGITAL_DECODE_NUM       0           //数字通道数
#define HI_AI_MAX_CHN (13)/*(16)*/
#define HI_AENC_MAX_CHN (13)       /*AENC 13 */
#define HI_VI_MAX_CHN (12)           /*最大视频输入通道数 */
#define HI_ENABLE (1)               /*已初始化*/
#define HI_DISABLE (0)              /*未初始化*/
#define HI_MAIN_STREAM (0)          /**/
#define HI_SUB_STREAM (1)           /**/
#define HI_STREAM_NUMS_PER_CHN (2)  /*每个通道的编码码流数目 */

#define SAMPLE_SYS_ALIGN_WIDTH  16
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define VO_BKGRD_BLUE           0x0000FF

#define HD_WIDTH                1920
#define HD_HEIGHT               1080
#define D1_WIDTH                704//720
#define _960H_WIDTH            928
#define D1_HEIGHT               576

#define _720P_WIDTH              1280
#define _720P_HEIGHT             720

#define ZOOM_VPSS(chn)				(HI3521_GetVpssInfo()->mix_vpss.start + (chn))
#define ZOOM_VPSS_CHN()				(HI3521_GetVpssInfo()->zoom_vpss_chn)

#define PREVIEW_VPSS(chn)			(HI3521_GetVpssInfo()->vi_vpss.start + (chn))
#define PREVIEW_VPSS_CHN()			(HI3521_GetVpssInfo()->preview_vpss_chn)
#define DEC_VPSS_CHN()			(HI3521_GetVpssInfo()->vdec_vpss_chn)
#define PIP_VPSS_CHN()				(HI3521_GetVpssInfo()->pip_vpss_chn)
#define JPEG_VPSS_CHN()				(HI3521_GetVpssInfo()->jpeg_vpss_chn)

#define CAPTURE_VPSS(chn)			(HI3521_GetVpssInfo()->vi_vpss.start + (chn))
#define CAPTURE_MAIN_VPSS_CHN()		(HI3521_GetVpssInfo()->capture_main_vpss_chn)
#define CAPTURE_SECOND_VPSS_CHN()	(HI3521_GetVpssInfo()->capture_second_vpss_chn)

#define MIX_VPSS(chn)				(HI3521_GetVpssInfo()->mix_vpss.start + (chn))
#define PLAY_VPSS_CHN()				(HI3521_GetVpssInfo()->play_vpss_chn)

enum video_standard_t
{
	VIDEO_STANDARD_PAL = 0,	///< PAL制式。
	VIDEO_STANDARD_NTSC,	///< NTSC制式。
	VIDEO_STANDARD_SECAM	///< SECAM制式。
};

struct resource_vpss
{
	HI_S32 start;
	HI_S32 end;
};

typedef struct distribute_vpss
{
	struct resource_vpss vi_vpss;
	HI_S32 capture_main_vpss_chn;
	HI_S32 capture_second_vpss_chn;
	HI_S32 jpeg_vpss_chn;
	HI_S32 preview_vpss_chn;
	HI_S32 pip_vpss_chn;
	struct resource_vpss mix_vpss;
	HI_S32 zoom_vpss_chn;
	HI_S32 play_vpss_chn;
	struct resource_vpss digit_vpss;
	HI_S32 vdec_vpss_chn;
}DISTIBUTE_VPSS;

typedef enum dev_type_en
{
	DEV_MIX_D1_DVR = 0,
	DEV_MIX_960H_DVR = 1,
	DEV_ONLY_HVR = 2,
	DEV_NONE_HVR = 3,
	DEV_1080P_4CHN = 4,
	DEV_720P_8CHN = 5,
	DEV_MIX_4960H_HVR = 6,
	DEV_NR = 7,
}DEV_TYPE_E;

extern HI_S32 HI3521_Sys_Init(HI_VOID);
HI_S32 HI3521_Comm_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);
extern HI_S32 HI3521_PthreadMutexUnlock(pthread_mutex_t *pMutex);
extern HI_S32 HI3521_PthreadMutexLock(pthread_mutex_t *pMutex);
#endif
