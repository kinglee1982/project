#ifndef __AV_CAPTURE_H__
#define __AV_CAPTURE_H__

#define MAX_VI_DEV								(4)
#define MAX_VI_CHN								(24)
#define MAX_VENC_CHN							(32)
#define MAX_AVJENC_CHN							(50)
#define VENC_NTSC_GOP							(60)	// N������֡ʱI֡���
#define VENC_PAL_GOP							(50)	// P������֡ʱI֡���
#define VIVIRT_DEV_OFFSET					    (4)
#define COVER_MAX_WIDTH                         (352)
#define PAL_COVER_MAX_HEIGHT                    (288)
#define NTSC_COVER_MAX_HEIGHT                   (240)
#define OSD_MAX_NUM	                            (8)

/* RGB format is 1888. */
#define VO_BKGRD_RED      0xFF0000    /* red back groud color */
#define VO_BKGRD_GREEN    0x00FF00    /* green back groud color */
#define VO_BKGRD_BLUE     0x0000FF    /* blue back groud color */
#define VO_BKGRD_BLACK    0x000000    /* black back groud color */



enum channel_t
{
	CHL_MAIN_T = 0,	//	��ͨ��	��	������1
	CHL_2END_T = 1,	//	��ͨ��	��	��������2
	CHL_3IRD_T = 2,	//	��ͨ��	��	��������3
	CHL_4RTH_T = 3,	//	��ͨ��	��	��������4
	CHL_JPEG_T = 4,	//	��ͨ��	��	��JPEGץͼ
	CHL_FUNCTION_NUM
};

enum video_profile_e
{
	BS = 0,
	MP,
	HP,
};

enum capture_start_t
{
	CAPTURE_START_VIDEO = 1,
	CAPTURE_START_AUDIO = 2
};

enum capture_comp_t
{
	CAPTURE_COMP_DIVX_MPEG4,
	CAPTURE_COMP_MS_MPEG4,
	CAPTURE_COMP_MPEG2,
	CAPTURE_COMP_MPEG1,
	CAPTURE_COMP_H263,
	CAPTURE_COMP_MJPG,
	CAPTURE_COMP_FCC_MPEG4,
	CAPTURE_COMP_H264,
	CAPTURE_COMP_NR
};

enum capture_brc_t
{
	CAPTURE_BRC_CBR,
	CAPTURE_BRC_VBR,
	CAPTURE_BRC_MBR,
	CAPTURE_BRC_NR
};

enum capture_size_t
{
	CAPTURE_SIZE_D1,		///< 720*576(PAL)	720*480(NTSC)
	CAPTURE_SIZE_HD1,		///< 352*576(PAL)	352*480(NTSC)
	CAPTURE_SIZE_BCIF,		///< 720*288(PAL)	720*240(NTSC)
	CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	CAPTURE_SIZE_SVCD,		///< 480*480(PAL)	480*480(NTSC)
	CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	CAPTURE_SIZE_NR = 9,	///< ����ʱ����ȵײ�һ���޸�
	CAPTURE_SIZE_ND1 = 9,   ///< 240*192
	CAPTURE_SIZE_960H,    ///< 928*576
	CAPTURE_SIZE_720P,      ///< 1280*720
	CAPTURE_SIZE_1_3M,      ///< 1280*960
	CAPTURE_SIZE_UXGA ,     ///< 1600*1200
	CAPTURE_SIZE_1080P,     ///< 1920*1080
	CAPTURE_SIZE_WUXGA,     ///< 1920*1200
	CAPTURE_SIZE_2_5M,      ///< 1872*1408
	CAPTURE_SIZE_3M,        ///< 2048*1536
	CAPTURE_SIZE_5M,        ///< 3744*1408
	CAPTURE_SIZE_EXT_NR,    ///< ö�ٵ�ͼ�δ�С�������Ŀ��
};



typedef enum capture_mode
{
	CAPTURE_VI = 0,
	CAPTURE_VENC_MAIN = 0,
	CAPTURE_VENC_SECOND = 1,
}CAPTURE_MODE;

typedef enum vi_mode_e
{
    VI_MODE_1_D1 = 0,
    VI_MODE_16_D1,
    VI_MODE_16_960H,
    VI_MODE_4_720P,
    VI_MODE_4_1080P,
    VI_MODE_8_720P
}VI_MODE_E;

typedef struct video_rect
{
	HI_U16	left;			///< ���Ͻ�x���ꡣ
	HI_U16	top;			///< ���Ͻ�y���ꡣ
	HI_U16	right;			///< ���½�x���ꡣ
	HI_U16	bottom;			///< ���½�y���ꡣ
}VIDEO_RECT;

/// ��Ƶ���ǲ���
typedef struct VIDEO_COVER_PARAM
{
	/// ���ǵľ������򣬲������������ϵ��
	VIDEO_RECT	rect;

	/// ������������ʾ����ɫ��16���Ʊ�ʾΪ0x00BBGGRR������ֽڱ�����
	HI_U32		color;

	/// ������������ͣ� ȡvideo_cover_t���͵�ֵ�����ں�ȫ������Ӧ���Ƕ����ģ�
	/// �������ڵĲ�����Ӱ��ȫ����ȫ���ڵĲ���Ҳ��Ӱ�촰�ڡ� �������ʱ������
	/// ���������ԡ�
	HI_S32			enable;
}VIDEO_COVER_PARAM;

typedef struct tagVOCSC
{
	HI_U8 lum;// ���� 0 -100
	HI_U8 con;//�Աȶ� 0-100
	HI_U8 hue;//ɫ�� 0-100
	HI_U8 sat;//���Ͷ� 0 -100
	HI_S32 resv[3];
}VOCSC;

typedef struct tagSTREAM_CHN_INFO
{
	HI_S32 enc_chn;
	HI_S32 channel;
	HI_S32 dwtype;
	HI_S32 frame_rate;
	HI_S32 pic_width;
	HI_S32 pic_height;
	HI_S32 image_size;

	unsigned long long oldpts;			// ����PS����ʱ����
	unsigned long long scr;				// ����PS����ʱ����
	unsigned long long vpts;
	unsigned long long apts;

}STREAM_CHN_INFO;

// ��Ƶ����ͨ����Ϣ�ṹ
typedef struct tagVENC_CHN_INFO
{
	HI_S32 vencChn;
	HI_S32 created;						// ����ͨ���Ƿ񱻴���
	HI_S32 started;						// ����ͨ���Ƿ񱻿���
	HI_S32 enable;							// ��Ƶ����ʹ��
	HI_S32 pic_width;						// ͼ����
	HI_S32 pic_height;						// ͼ��߶�
	HI_S32 frame_rate;						// ֡��
	HI_S32 image_size;                     //
	HI_S32 venc_fd;
	HI_S32 gop;							// ֡���
	HI_S32 bitrate;
	HI_S32 cbrvbr;
	HI_S32 preframe;
	HI_S32 cur_frame_rate;
	HI_S32 frame_count;
	HI_S32 error_count;
	unsigned long long oldpts;			// ����PS����ʱ����
	unsigned long long scr;				// ����PS����ʱ����
	pthread_t threadid;
	pthread_mutex_t lock;				// ���ڷ�ֹ�������úͲɼ��ĳ�ͻ
	struct timeval oldtime;

	STREAM_CHN_INFO info;

} VENC_CHN_INFO_t;

typedef struct tagJPEG_CHN_INFO
{
	HI_S32 create;
	HI_S32 grp;
	HI_S32 viChn;
	HI_S32 jpegChn;
	HI_S32 ready;
	HI_S32 started;
	HI_S32 fps;
	HI_S32 Qval;
	HI_S32 image_size;
	HI_S32 width;
	HI_S32 height;
	HI_S32 venc_fd;
	HI_U64 last_frame_time;
	HI_U8 *ptr;
    HI_S32 len;
	pthread_t threadid;
	STREAM_CHN_INFO info;
}JPEG_CHN_INFO_t;



#ifndef REGION_HANDLE
typedef HI_U32 REGION_HANDLE;
#endif

typedef struct tagCOVER_INFO
{
	REGION_HANDLE handle[4];
	VIDEO_COVER_PARAM Param[4];
}COVER_INFO_t;

// ��Ƶ������Ϣ�ṹ
typedef struct tagAENC_CHN_INFO
{
	HI_S32 HiAencChn;
	HI_S32 aenc_fd;
	HI_S32 created;						// ����ͨ���Ƿ񱻴���
	HI_S32 started;						// ����ͨ���Ƿ񱻿���
	HI_S32 regular_enable;					// ����������Ƶ����ʹ��
	HI_S32 extra_enable;					// ����������Ƶ����ʹ��
	pthread_t threadid;
	HI_S32 Channel;
	HI_S32 AiDev;
	HI_S32 AiChn;
	unsigned long long oldpts;		    // ����PS����ʱ����
	unsigned long long scr;				// ����PS����ʱ����
	pthread_mutex_t lock;
	STREAM_CHN_INFO info[2];
} AENC_CHN_INFO_t;



// ��Ƶͨ�����Խṹ
typedef struct tagVI_CHN_INFO
{
    /*
	HI_S32 binded;
	HI_S32 vi_picsize;
	HI_S32 vi_width;
	HI_S32 vi_height;
	HI_S32 vi_capsel;
	HI_S32 vi_downscale;
	HI_S32 vi_framerate;
	HI_S32 split_picsize;
	HI_S32 split_framerate;

	HI_S32 venc_picsize;
	HI_S32 venc_framerate;

	HI_S32 vi_capenable;
	*/

	HI_S32 vi_chn;
	VENC_CHN_INFO_t venc_chn[2];		    // ������Ƶ����ͨ������������
	AENC_CHN_INFO_t aenc_chn;				// һ����Ƶ����ͨ��
	JPEG_CHN_INFO_t jpeg_chn;				// һ����Ƶ����ͨ��
	COVER_INFO_t cover;						// �����ڵ�
} VI_CHN_INFO_t;

// �豸��Ϣ�ṹ
typedef struct tagCapture_Dev
{
	HI_S32 sys_init;
	HI_S32 venc_opened;					    // ��Ƶ�������Ƿ�򿪣�0Ϊ�رգ�1Ϊ��
	HI_S32 aenc_opened;					    // ��Ƶ�������Ƿ�򿪣�0Ϊ�رգ�1Ϊ��
	HI_S32 vstd;							    // ��Ƶ��ʽ
	HI_S32 video_codec_format;		            // ��Ƶ������ʽ
	HI_S32 audio_codec_format;		            // ��Ƶ������ʽ
	time_t basetime;					    //pts�Ļ�׼ʱ��
	VI_CHN_INFO_t  vi_chn[16];		// ��Ƶ����ͨ��
	pthread_mutex_t lock;
	pthread_mutex_t vilock;
	fd_set read_fds;
	fd_set aenc_rfds;
} Capture_Dev_t;






//��Ƶ��ؽṹ
enum audio_encode_type
{
	NONE = 0,
	G729_8KBIT,
	G726_16KBIT,
	G726_24KBIT,
	G726_32KBIT,
	G726_40KBIT,
	PCM_8TO16BIT,
	PCM_ALAW,
	PCM_ULAW,
	ADPCM8K16BIT,
	ADPCM16K16BIT,
	G711_ALAW,   //zhuangke
	MPEG2_LAYER1,
	AMR8K16BIT,
	G711_ULAW
};

enum audio_codec_format_t {
	AUDIO_PCM8,
	AUDIO_G729,
	AUDIO_ADPCM,
	AUDIO_PCM_MULAW,
	AUDIO_G721,
	AUDIO_PCM8_VWIS,
	AUDIO_MS_ADPCM,
	AUDIO_G711A,
	AUDIO_AVI_AUDIO,
	AUDIO_NR,
};

typedef struct hiAUDIO_RAW_INFO_S
{
    AUDIO_BIT_WIDTH_E   enBitwidth;     /*audio frame bitwidth*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /*audio frame momo or stereo mode*/
    HI_U64 u64TimeStamp;                /*audio frame timestamp*/
    HI_U32 u32Seq;                      /*audio frame seq*/
    HI_U32 u32Len;                      /*data lenth per channel in frame*/
}AUDIO_RAW_INFO_S;



typedef struct tagAENC_CHN_ATTR
{
	HI_S32 pic_width;						// ͼ����
	HI_S32 pic_height;						// ͼ��߶�
	HI_S32 frame_rate;						// ֡��
	HI_S32 image_size;
	HI_S32 gop;							// ֡���
	HI_S32 bitrate;
	HI_S32 cbrvbr;
}AENC_CHN_ATTR;


typedef struct tagH264_Param
{
	HI_S32 vstd;
	HI_U32 bitrate;
	PAYLOAD_TYPE_E type;
	VENC_RC_MODE_E rcmode;
	PIC_SIZE_E size;
	HI_U32 frmrate;
	HI_U32 fluctuatelevel;
	HI_S32 mainstream;
	HI_U32 gop;
	HI_BOOL vifield;
	HI_U32 maxQp;
	HI_U32 minQp;
}H264_Param;


extern HI_S32 HI3521_CapDevInit(HI_S32 s32Chn);
extern Capture_Dev_t *HI3521_GetCaptureDev(HI_VOID);
extern HI_S32 HI3521_EnableAVencChn(HI_S32 s32Chn);
extern VI_MODE_E HI3521_GetViMode(HI_VOID);

#endif	//__AVDEC_H__

