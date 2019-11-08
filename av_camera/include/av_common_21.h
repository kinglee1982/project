#ifndef __AV_COMMON_21_H__
#define __AV_COMMON_21_H__
extern "C"
{

/*******************************************************
    macro define
*******************************************************/
#define CHECK_CHN_RET(express,Chn,name)\
	do{\
		HI_S32 Ret;\
		Ret = express;\
		if (HI_SUCCESS != Ret)\
		{\
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
			fflush(stdout);\
			return Ret;\
		}\
	}while(0)

#define CHECK_RET(express,name)\
    do{\
        HI_S32 Ret;\
        Ret = express;\
        if (HI_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)

//#define SAMPLE_GLOBAL_NORM	    VIDEO_ENCODING_MODE_PAL
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define TW2865_FILE     "/dev/tw2865dev"
#define TW2960_FILE     "/dev/tw2960dev"
#define TLV320_FILE     "/dev/tlv320aic31"
#define NVP6124_FILE    "/dev/nvp6124"
/* BEGIN: Added by zhangjinlei, 2016/4/21  DES:调试3521A视频VIVO功能  PN:16042001001 */
#define NVP6114_FILE    "/dev/nvp6114"
/* END:   Added by zhangjinlei, 2016/4/21   PN:16042001001 */

#define ERR_STR		strerror(errno)			/*错误信息*/

#define SAMPLE_VO_DEV_DHD0 0
#define SAMPLE_VO_DEV_DSD0 1
#define SAMPLE_VO_DEV_VIRT0 2
#define SAMPLE_VO_DEV_DSD1 -1

#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VSD0 2
#define SAMPLE_VO_LAYER_VIRT0 3
#define SAMPLE_VO_LAYER_VPIP 1

#define VO_LAYER_PIP      1
#define VO_LAYER_PIP_STA	1
#define VO_LAYER_PIP_END	1
#define VO_DEV_HD_END	0


#define SAMPLE_VO_WBC_BASE  0
#define SAMPLE_VO_LAYER_PRIORITY_BASE 0
#define SAMPLE_VO_LAYER_PRIORITY_PIP 1

#define GRAPHICS_LAYER_G0  0
#define GRAPHICS_LAYER_G1  1
#define GRAPHICS_LAYER_HC0 2

#define SAMPLE_AUDIO_PTNUMPERFRM   320
#define SAMPLE_AUDIO_TLV320_DEV 1
#define SAMPLE_AUDIO_TW2865_DEV 0
#define SAMPLE_AUDIO_HDMI_AO_DEV 1
#define SAMPLE_AUDIO_AI_DEV 0
#define SAMPLE_AUDIO_AO_DEV 0

#define HDD_MOUNT_FILE 					    "/proc/mounts"

#define SAMPLE_CIF_H264_PATH "../common/CIF.h264"
#define SAMPLE_1080P_H264_PATH "./stream_chn0.h264"
#define SAMPLE_1080P_H265_PATH "../common/1080P.h265"
#define SAMPLE_4K_H264_PATH "../common/tmp1"
#define SAMPLE_4K_H265_PATH "../common/tmp2"
#define SAMPLE_1080P_MPEG4_PATH "../common/1080P.mpeg4"
#define SAMPLE_FIELD_H264_PATH "../common/D1_field.h264"
#define SAMPLE_1080P_JPEG_PATH "../common/1080P.jpg"
#define SAMPLE_4K_JPEG_PATH "../common/tmp3"
#define SAMPLE_720P_PATH "./av0.h264"

#define SAMPLE_MAX_VDEC_CHN_CNT 8



#define ALIGN_UP(x, a)              ((x+a-1)&(~(a-1)))
#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))

#define SAMPLE_SYS_ALIGN_WIDTH  16
#define VO_BKGRD_BLUE           0x0000FF

#define HD_WIDTH                1920
#define HD_HEIGHT               1080
#define D1_WIDTH                704
#define D1_HEIGHT               576
#define _960H_WIDTH             928
#define _720P_WIDTH              1280
#define _720P_HEIGHT             720

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)


typedef enum sample_vi_mode_e
{
    SAMPLE_VI_MODE_16_D1,
    SAMPLE_VI_MODE_8_960H,
    SAMPLE_VI_MODE_16_960H,
    SAMPLE_VI_MODE_16_1280H,
    SAMPLE_VI_MODE_16_HALF720P,

    SAMPLE_VI_MODE_8_720P,
    SAMPLE_VI_MODE_16_720P,

    SAMPLE_VI_MODE_4_1080P,
    SAMPLE_VI_MODE_8_1080P,
    SAMPLE_VI_MODE_16_1080P,

    SAMPLE_VI_MODE_4_3M,
}SAMPLE_VI_MODE_E;

typedef enum sample_vi_6124_mode_e
{
    /*nextchip nvp6124/6114a*/
	SAMPLE_VI_MODE_6124_960H,
	SAMPLE_VI_MODE_6124_HDX,   //640x720
	SAMPLE_VI_MODE_6124_HD,    //720p
	SAMPLE_VI_MODE_6124_FHDX,  //960x1080
	SAMPLE_VI_MODE_6124_FHD,   //1080p
    SAMPLE_VI_MODE_960H_720P_2MUX,
    SAMPLE_VI_MODE_6124_2MUX_FHD,   //1080p
    SAMPLE_VI_MODE_6124_4MUX_HD,   //4mux 720p

}SAMPLE_VI_6124_MODE_E;

enum
{
	NVP6124_VI_SD = 0,
	NVP6124_VI_720P_2530,
	NVP6124_VI_720P_5060,
	NVP6124_VI_1080P_2530,
	NVP6124_VI_1920H,
	NVP6124_VI_BUTT
};

typedef enum sample_vi_6124_mux_e
{
	NVP6124_OUTMODE_1MUX_SD = 0,
	NVP6124_OUTMODE_1MUX_HD,
	NVP6124_OUTMODE_1MUX_HD5060,
	NVP6124_OUTMODE_1MUX_FHD,
	NVP6124_OUTMODE_2MUX_SD,
	NVP6124_OUTMODE_2MUX_HD_X,
	NVP6124_OUTMODE_2MUX_HD,
	NVP6124_OUTMODE_2MUX_FHD_X,
	NVP6124_OUTMODE_4MUX_SD,
	NVP6124_OUTMODE_4MUX_HD_X,
	NVP6124_OUTMODE_4MUX_HD,
	NVP6124_OUTMODE_2MUX_FHD,
	NVP6124_OUTMODE_BUTT
}SAMPLE_VI_6124_MUX_E;

typedef struct sample_vi_param_s
{
    HI_S32 s32ViDevCnt;        // VI Dev Total Count
    HI_S32 s32ViDevInterval;   // Vi Dev Interval
    HI_S32 s32ViChnCnt;        // Vi Chn Total Count
    HI_S32 s32ViChnInterval;   // VI Chn Interval
}SAMPLE_VI_PARAM_S;

typedef enum sample_vi_chn_set_e
{
    VI_CHN_SET_NORMAL = 0, /* mirror, filp close */
    VI_CHN_SET_MIRROR,      /* open MIRROR */
    VI_CHN_SET_FILP        /* open filp */
}SAMPLE_VI_CHN_SET_E;

typedef enum sample_vo_mode_e
{
    VO_MODE_1MUX  = 0,
    VO_MODE_4MUX = 1,
    VO_MODE_9MUX = 2,
    VO_MODE_16MUX = 3,
    VO_MODE_BUTT
}SAMPLE_VO_MODE_E;

typedef struct hisample_MEMBUF_S
{
    VB_BLK  hBlock;
    VB_POOL hPool;
    HI_U32  u32PoolId;

    HI_U32  u32PhyAddr;
    HI_U8   *pVirAddr;
    HI_S32  s32Mdev;
} SAMPLE_MEMBUF_S;

typedef enum sample_rc_e
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_FIXQP
}SAMPLE_RC_E;

typedef enum sample_rgn_change_type_e
{
    RGN_CHANGE_TYPE_FGALPHA = 0,
    RGN_CHANGE_TYPE_BGALPHA,
    RGN_CHANGE_TYPE_LAYER
}SAMPLE_RGN_CHANGE_TYPE_EN;

typedef struct sample_vo_param_s
{
    VO_DEV VoDev;
    HI_CHAR acMmzName[20];
    HI_U32 u32WndNum;
    SAMPLE_VO_MODE_E enVoMode;
    VO_PUB_ATTR_S stVoPubAttr;
    HI_BOOL bVpssBind;
}SAMPLE_VO_PARAM_S;

typedef struct sample_venc_getstream_s
{
     HI_BOOL bThreadStart;
     HI_S32  s32Cnt;
}SAMPLE_VENC_GETSTREAM_PARA_S;



typedef enum
{
    HIFB_LAYER_0 = 0x0,
    HIFB_LAYER_1,
    HIFB_LAYER_2,
    HIFB_LAYER_CURSOR_0,
    HIFB_LAYER_ID_BUTT
} HIFB_LAYER_ID_E;

typedef enum hiVdecThreadCtrlSignal_E
{
	VDEC_CTRL_START,
	VDEC_CTRL_PAUSE,
	VDEC_CTRL_STOP,
}VdecThreadCtrlSignal_E;

typedef struct hiVdecThreadParam
{
	HI_S32 s32ChnId;
	PAYLOAD_TYPE_E enType;
	HI_CHAR cFileName[100];
	HI_S32 s32StreamMode;
	HI_S32 s32MilliSec;
	HI_S32 s32MinBufSize;
	HI_S32 s32IntervalTime;
	VdecThreadCtrlSignal_E eCtrlSinal;
	HI_U64	u64PtsInit;
	HI_U64	u64PtsIncrease;
    HI_BOOL bLoopSend;
    HI_BOOL bManuSend;
    HI_CHAR cUserCmd;
}VdecThreadParam;

typedef enum hiAudioCodecType
{
    AUDIO_CODEC_INNER = 0,
    AUDIO_CODEC_TLV320,
    AUDIO_CODEC_HDMI,
    AUDIO_CODEC_TW2865,
    AUDIO_CODEC_BUTT
}AudioCodecType;

typedef struct hiHDMI_CALLBACK_ARGS_S
{
    HI_HDMI_ID_E		enHdmi;
	HI_HDMI_VIDEO_FMT_E eForceFmt;
}HDMI_CALLBACK_ARGS_S;




HI_VOID HI3521_CovTime2Str(time_t t, HI_CHAR *pTime);

}
#endif /* __APP_COMMON_H__ */
