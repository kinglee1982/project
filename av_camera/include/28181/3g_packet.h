#ifndef _3G_HISENSE_PACKET_H
#define _3G_HISENSE_PACKET_H

#include "iflytype.h"
#include "ctrlprotocol.h"

#define TIMEOUTLINK3G	(60)

// lwj add 100609
#define PACK_NO_PADDING  __attribute__ ((__packed__))

#pragma pack( push, 1 )
//12-28
typedef struct{
	int channel;
	unsigned char *data;
	unsigned int len;
	unsigned long long time_stamp;
	int is_raw_data;
}audio_frame_t;

// lwj modify 100713
#if 1
typedef struct
{
	u8	safeguid[16];					// PROTOGUID
	u8	protocolver;					//协议版本
	u8	byConnType;						//连接类型，0x1：控制信令；0x2：码流传输；
	                                    //0x3：广播搜索, 0x04: 图片链接
	u8	reserved[2];					//保留字段
}PACK_NO_PADDING ifly_ProtocolHead_3g_t;
//与ctrlprotocol.h中的 lifly_ProtocolHead_t结构完全一样;

typedef struct
{
	unsigned int length;	                  // 消息长度
	unsigned short type;                      // 消息类型
	unsigned short event;                     // 消息名
	unsigned short number;	                  // 流水号
	unsigned short version;                   // 版本号
	char srcID[32];                           // 源ID,字符串
	char destID[32];                          // 目的ID，字符串
}PACK_NO_PADDING tm_svr_header;

//切换DVR预览通道2012-08-15
typedef struct
{
	unsigned int   length;	      // 消息长度
	unsigned short type;          // 消息类型
	unsigned short event;         // 消息名
	unsigned char  chn;	          //切换DVR预览的通道号
}__attribute__ ((__packed__)) switch_preview_chn;

//心跳包 add by wu 2012-05-22
typedef struct
{
	unsigned int length;	                   // 消息长度
	unsigned short type;                      // 消息类型
	unsigned short event;                     // 消息名
	unsigned short number;	                   // 流水号
	unsigned short version;                   // 版本号
}PACK_NO_PADDING tmkj_heard_cmd;

typedef struct
{
	char device_ID[32];                    // ID，字符串
	char device_name[32];                  // 名称，字符串
	char group[32];                        // 组，字符串
	char user[16];                         // 用户名，字符串
	char pass[10];                         // 密码，字符串

	u8	maxChnNum;						//最大通道数
	u8	maxAudioNum;					//最大音频数
	u8	maxSubstreamNum;				//最大子码流数
	u8	maxPlaybackNum;					//最大回放数
	u8	maxAlarmInNum;					//最大报警输入数
	u8	maxAlarmOutNum;					//最大报警输出数
	u8	maxHddNum;						//最大硬盘数

	u8  mode;                          // TCP还是UDP传输
	u8 reserved[64];
}PACK_NO_PADDING ifly_devcice_login_req_3g_t;


//码流传输请求结构
typedef struct
{
	u8 command;		//0：预览 1：文件回放 2：时间回放 3：文件下载 4：升级
						//5 VOIP 6 文件按帧下载 7 预留（时间按帧下载） 8 透明通道
						//9 远程格式化硬盘
	union
	{
		struct
		{
			u8		chn;				//预览通道
			u8		type;				//0:主码流视频 1：主码流音频 2：子码流视频
		}PACK_NO_PADDING Monitor_t;						//预览 command = 0

		struct
		{
			char	filename[64];		//回放的文件
			u32		offset;				//文件偏移
		}PACK_NO_PADDING FilePlayBack_t; 				//文件回放 command = 1,或 command = 7

		struct
		{
			u8		channel;			//通道数
			u16		type;				//类型
			u32		start_time;			//开始时间
			u32		end_time;			//终止时间
		}PACK_NO_PADDING TimePlayBack_t ;				//时间回放 command = 2

		struct
		{
			char	filename[64];		//下载的文件
			u32		offset;				//文件偏移
			u32		size;				//文件大小
		}PACK_NO_PADDING FileDownLoad_t;				//文件下载 command = 3

		struct
		{
			u32		size;				//文件长度
			u32		verify;				//校验和
			u16		version;			//文件版本号
			u16		updateType;			//0:主板升级 1：面板升级
		}PACK_NO_PADDING Update_t;						//远程升级  command = 4；

		//VOIP command
		u8 voipreserved;				//VOIP预留 command = 5；

		//透明通道
		u8 SerialPort;					//透明通道 1-232串口  2-485串口	command = 8；

		//远程格式化硬盘
		u8 formatHddIndex;				//要格式化的硬盘号 0开始
	} ;
	// u8	reserved[7];					//预留
	u32 linkID;                      // 连接ID
	u8 reserved[3];
}PACK_NO_PADDING ifly_TCP_Stream_Req_3g;

//码流传输主机端回应结构
typedef struct
{
	u32	errcode;						//连接成功返回0， 否则返回其他错误码
	u32	ackid;						//对于文件回放，时间回放。 用于后续客户端控制回放操作和区分进度, 预览控制
}PACK_NO_PADDING ifly_TCP_Stream_Ack_3g;

typedef struct
{
	u8 m_byMediaType;
	u8 m_byFrameRate;
	u8 m_bKeyFrame;
	int m_wVideoWidth;
	int m_wVideoHeight;
	u32 m_dwDataSize;
	u32 m_dwFrameID;
	u32 m_dwTimeStamp;

}PACK_NO_PADDING IFLY_MediaFRAMEHDR_3g_t;

// lwj add 100814
typedef struct
{
	u8 HDD_state;	// 0:no hdd; 1:hdd ok; 2:hdd err

	u8 Channel_num;	// total channel num
	u8 Rec_state[16];	// 0:no rec; 1:timer rec; 2:MD rec; 3:alarm rec; 4:hand rec
	u8 Video_lost[16];	// 0:ok; 1:lost
	u8 reserved[64];
}PACK_NO_PADDING ifly_device_state_3g_t;
typedef struct
{
    u32    interval;    // 时间间隔，以毫秒为单位
    u8     reserved[64];
}PACK_NO_PADDING ifly_device_state_interval_t;
// lwj add end

// lwj add 100823
typedef struct
{
	char utc_position[16];		//utc 时间，格式为hhmmss.sss，北京时间须在此基础上加8小时
	u8 Status;					//A:定位 V:未定位
	char latitude[16];			//纬度，度分格式，为ddmm.mmmm ，例3723.2475
	u8 n_s_indicator;			//南北纬指示 N: north ; S: south
	char longitude[16];			//经度，度分格式，为dddmm.mmmm ，例12158.3416
	u8 e_w_indicator;			//东西经指示 E: east ; W: west
	char speed_over_ground[16];	//相对速度，单位为节，换算成千米/小时须乘以1.852
	char course_over_ground[16];	//相对方向，单位为度
	int date;						//日期，格式为日月年，例2010年8月20日为200810
}PACK_NO_PADDING ifly_serial_gpsrmc_info_t;
// lwj add end
#else

#endif

typedef struct
{
	struct timeval oldtime;
	struct timeval newtime;
	unsigned char first_time_cal;
}PACK_NO_PADDING period_cal;

typedef struct
{
	unsigned int stream_bytecount;
	unsigned int seconds_passed;
	unsigned int bps_avg;
}PACK_NO_PADDING bps_cal;
// lwj add end

// lwj add 100630
typedef struct
{
	char	ID[16];						// ID，字符串
	char name[32];						// 名称，字符串
	char group[16];					// 组，字符串
	char user[16];						// 用户名，字符串
	char pass[10];						// 密码，字符串

	unsigned char mainChnNum;				// 主码流通道数
	unsigned char subChnNum;				// 子码流通道数
	unsigned char audioChnNum;				// 音频通道数
	unsigned char alarmInNum;				// 报警输入通道数
	unsigned char alarmOutNum;				// 报警输出通道数
	unsigned char reserved[64];
}PACK_NO_PADDING ifly_devcice_login_req_t;

// 各种结束命令用到
typedef struct
{
	u32 linkID;
	u8 reserved[64];

}ifly_linkID_t;
// lwj add end

//add by wu-20140822
typedef struct
{
	u32 linkID;
    u8  chn;
	u8  reserved[63];
}PACK_NO_PADDING svr_close_st;


enum NETDVR_MEDIA_TYPE {
	NETDVR_MEDIA_VIDEO = 0,
	NETDVR_MEDIA_AUDIO,
	NETDVR_MEDIA_VOIP,
	NETDVR_MEDIA_SUBVIDEO,
	NETDVR_MEDIA_PLAYCB,
};

typedef struct
{
	FILE *fp;
	int Handle;
	int chn;
	int id;
	enum NETDVR_MEDIA_TYPE media_type;
}PACK_NO_PADDING MyFrmCBContext;


#pragma pack( pop )
// lwj add end
#define MEDIA_TYPE_DOWNLOAD     (BYTE)3   //add by wu 2014-08-13
#define MEDIA_TYPE_H264		    (BYTE)98
#define AUDIO_SAMPLE_BITWIDTH		  16
#define MEDIA_TYPE_ADPCM		(BYTE)20
#define VIDEO_FRAM_SPEED        (BYTE)25  //帧率

#endif
