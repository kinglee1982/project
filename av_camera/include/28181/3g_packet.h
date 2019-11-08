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
	u8	protocolver;					//Э��汾
	u8	byConnType;						//�������ͣ�0x1���������0x2���������䣻
	                                    //0x3���㲥����, 0x04: ͼƬ����
	u8	reserved[2];					//�����ֶ�
}PACK_NO_PADDING ifly_ProtocolHead_3g_t;
//��ctrlprotocol.h�е� lifly_ProtocolHead_t�ṹ��ȫһ��;

typedef struct
{
	unsigned int length;	                  // ��Ϣ����
	unsigned short type;                      // ��Ϣ����
	unsigned short event;                     // ��Ϣ��
	unsigned short number;	                  // ��ˮ��
	unsigned short version;                   // �汾��
	char srcID[32];                           // ԴID,�ַ���
	char destID[32];                          // Ŀ��ID���ַ���
}PACK_NO_PADDING tm_svr_header;

//�л�DVRԤ��ͨ��2012-08-15
typedef struct
{
	unsigned int   length;	      // ��Ϣ����
	unsigned short type;          // ��Ϣ����
	unsigned short event;         // ��Ϣ��
	unsigned char  chn;	          //�л�DVRԤ����ͨ����
}__attribute__ ((__packed__)) switch_preview_chn;

//������ add by wu 2012-05-22
typedef struct
{
	unsigned int length;	                   // ��Ϣ����
	unsigned short type;                      // ��Ϣ����
	unsigned short event;                     // ��Ϣ��
	unsigned short number;	                   // ��ˮ��
	unsigned short version;                   // �汾��
}PACK_NO_PADDING tmkj_heard_cmd;

typedef struct
{
	char device_ID[32];                    // ID���ַ���
	char device_name[32];                  // ���ƣ��ַ���
	char group[32];                        // �飬�ַ���
	char user[16];                         // �û������ַ���
	char pass[10];                         // ���룬�ַ���

	u8	maxChnNum;						//���ͨ����
	u8	maxAudioNum;					//�����Ƶ��
	u8	maxSubstreamNum;				//�����������
	u8	maxPlaybackNum;					//���ط���
	u8	maxAlarmInNum;					//��󱨾�������
	u8	maxAlarmOutNum;					//��󱨾������
	u8	maxHddNum;						//���Ӳ����

	u8  mode;                          // TCP����UDP����
	u8 reserved[64];
}PACK_NO_PADDING ifly_devcice_login_req_3g_t;


//������������ṹ
typedef struct
{
	u8 command;		//0��Ԥ�� 1���ļ��ط� 2��ʱ��ط� 3���ļ����� 4������
						//5 VOIP 6 �ļ���֡���� 7 Ԥ����ʱ�䰴֡���أ� 8 ͸��ͨ��
						//9 Զ�̸�ʽ��Ӳ��
	union
	{
		struct
		{
			u8		chn;				//Ԥ��ͨ��
			u8		type;				//0:��������Ƶ 1����������Ƶ 2����������Ƶ
		}PACK_NO_PADDING Monitor_t;						//Ԥ�� command = 0

		struct
		{
			char	filename[64];		//�طŵ��ļ�
			u32		offset;				//�ļ�ƫ��
		}PACK_NO_PADDING FilePlayBack_t; 				//�ļ��ط� command = 1,�� command = 7

		struct
		{
			u8		channel;			//ͨ����
			u16		type;				//����
			u32		start_time;			//��ʼʱ��
			u32		end_time;			//��ֹʱ��
		}PACK_NO_PADDING TimePlayBack_t ;				//ʱ��ط� command = 2

		struct
		{
			char	filename[64];		//���ص��ļ�
			u32		offset;				//�ļ�ƫ��
			u32		size;				//�ļ���С
		}PACK_NO_PADDING FileDownLoad_t;				//�ļ����� command = 3

		struct
		{
			u32		size;				//�ļ�����
			u32		verify;				//У���
			u16		version;			//�ļ��汾��
			u16		updateType;			//0:�������� 1���������
		}PACK_NO_PADDING Update_t;						//Զ������  command = 4��

		//VOIP command
		u8 voipreserved;				//VOIPԤ�� command = 5��

		//͸��ͨ��
		u8 SerialPort;					//͸��ͨ�� 1-232����  2-485����	command = 8��

		//Զ�̸�ʽ��Ӳ��
		u8 formatHddIndex;				//Ҫ��ʽ����Ӳ�̺� 0��ʼ
	} ;
	// u8	reserved[7];					//Ԥ��
	u32 linkID;                      // ����ID
	u8 reserved[3];
}PACK_NO_PADDING ifly_TCP_Stream_Req_3g;

//�������������˻�Ӧ�ṹ
typedef struct
{
	u32	errcode;						//���ӳɹ�����0�� ���򷵻�����������
	u32	ackid;						//�����ļ��طţ�ʱ��طš� ���ں����ͻ��˿��ƻطŲ��������ֽ���, Ԥ������
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
    u32    interval;    // ʱ�������Ժ���Ϊ��λ
    u8     reserved[64];
}PACK_NO_PADDING ifly_device_state_interval_t;
// lwj add end

// lwj add 100823
typedef struct
{
	char utc_position[16];		//utc ʱ�䣬��ʽΪhhmmss.sss������ʱ�����ڴ˻����ϼ�8Сʱ
	u8 Status;					//A:��λ V:δ��λ
	char latitude[16];			//γ�ȣ��ȷָ�ʽ��Ϊddmm.mmmm ����3723.2475
	u8 n_s_indicator;			//�ϱ�γָʾ N: north ; S: south
	char longitude[16];			//���ȣ��ȷָ�ʽ��Ϊdddmm.mmmm ����12158.3416
	u8 e_w_indicator;			//������ָʾ E: east ; W: west
	char speed_over_ground[16];	//����ٶȣ���λΪ�ڣ������ǧ��/Сʱ�����1.852
	char course_over_ground[16];	//��Է��򣬵�λΪ��
	int date;						//���ڣ���ʽΪ�����꣬��2010��8��20��Ϊ200810
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
	char	ID[16];						// ID���ַ���
	char name[32];						// ���ƣ��ַ���
	char group[16];					// �飬�ַ���
	char user[16];						// �û������ַ���
	char pass[10];						// ���룬�ַ���

	unsigned char mainChnNum;				// ������ͨ����
	unsigned char subChnNum;				// ������ͨ����
	unsigned char audioChnNum;				// ��Ƶͨ����
	unsigned char alarmInNum;				// ��������ͨ����
	unsigned char alarmOutNum;				// �������ͨ����
	unsigned char reserved[64];
}PACK_NO_PADDING ifly_devcice_login_req_t;

// ���ֽ��������õ�
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
#define VIDEO_FRAM_SPEED        (BYTE)25  //֡��

#endif
