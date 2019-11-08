#ifndef _CTRLPROTOCOL_H_
#define _CTRLPROTOCOL_H_

#include "iflytype.h"

#define MAX_HDD_NUM			2 // 4		//fpga:8 davinci:2



#define MAX_ALARM_IN_NUM	4

#define MAX_ALARM_OUT_NUM	1

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#ifdef WIN32
#define SOCKHANDLE		SOCKET
#else
#define SOCKHANDLE		int
#define INVALID_SOCKET	(-1)
#define SOCKET_ERROR	(-1)
#endif

#ifdef WIN32
typedef HANDLE MutexHandle;
typedef HANDLE SemHandle;
#else
typedef pthread_mutex_t* MutexHandle;
typedef sem_t* SemHandle;

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#define INFINITE						0xffffffff
#endif

//Э��
#define CTRL_PROTOCOL_SERVERPORT		6630					//�������˿� 6610
#define CTRL_PROTOCOL_MAXLINKNUM		256

#define CTRL_PROTOCOL_CONNECT_BLOCK		INFINITE				//���ӷ�ʽ:����
#define CTRL_PROTOCOL_CONNECT_DEFAULT	3000					//ȱʡ����ʱ��:3��

#define CTRL_VERSION					0x0100

#define CTRL_COMMAND					0
#define CTRL_NOTIFY						1
#define CTRL_ACK						2

//��������
#define CTRL_CMD_BASE					10000

#define CTRL_CMD_STRAT_VOIP_ACK         CTRL_CMD_BASE-2        //
#define CTRL_CMD_STOP_VOIP_ACK          CTRL_CMD_BASE-1


#define CTRL_CMD_LOGIN					CTRL_CMD_BASE+1			//Զ�̵�¼����
#define CTRL_CMD_LOGOFF					CTRL_CMD_BASE+2			//ע���û���¼
#define CTRL_CMD_GETDEVICEINFO			CTRL_CMD_BASE+3			//����豸��Ϣ
#define CTRL_CMD_GETVIDEOPROPERTY		CTRL_CMD_BASE+4			//��Ƶ������Ϣ
#define CTRL_CMD_GETAUDIOPROPERTY		CTRL_CMD_BASE+5			//��Ƶ������Ϣ
#define CTRL_CMD_GETVOIPPROPERTY		CTRL_CMD_BASE+6			//voip������Ϣ
#define CTRL_CMD_STARTVIDEOMONITOR		CTRL_CMD_BASE+7			//��ʼ��ƵԤ��
#define CTRL_CMD_STOPVIDEOMONITOR		CTRL_CMD_BASE+8			//ֹͣ��ƵԤ��
#define CTRL_CMD_STOPAUDIOMONITOR		CTRL_CMD_BASE+9			//ֹͣ��ƵԤ��
#define CTRL_CMD_STOPVOIP				CTRL_CMD_BASE+10		//ֹͣvoip
#define CTRL_CMD_STARTVOIP		        CTRL_CMD_BASE+11		//��ʼvoip ��appͨ��ʱʹ��

//#define CTRL_CMD_STOPDOWNLOAD			CTRL_CMD_BASE+11		//
#define CTRL_CMD_STOPDOWNLOAD			CTRL_CMD_BASE+12		//ֹͣ���� ���̨ͨ��ʹ��
#define CTRL_CMD_STOPTIMEPLAY			CTRL_CMD_BASE+13		//ֹͣʱ��ط�
#define CTRL_CMD_FASTPLAY				CTRL_CMD_BASE+14		//���ٲ���
#define CTRL_CMD_SLOWPLAY				CTRL_CMD_BASE+15		//���ٲ���
#define CTRL_CMD_SETPLAYRATE			CTRL_CMD_BASE+16		//��������
#define CTRL_CMD_PAUSEPLAY				CTRL_CMD_BASE+17		//��ͣ����
#define CTRL_CMD_RESUMEPLAY				CTRL_CMD_BASE+18		//�ָ�����
#define CTRL_CMD_SINGLEPLAY				CTRL_CMD_BASE+19		//֡��
#define CTRL_CMD_FASTBACKPLAY			CTRL_CMD_BASE+20		//����
#define CTRL_CMD_PLAYPREV				CTRL_CMD_BASE+21		//��һ��
#define CTRL_CMD_PLAYNEXT				CTRL_CMD_BASE+22		//��һ��
#define CTRL_CMD_PLAYSEEK				CTRL_CMD_BASE+23		//��λ
#define CTRL_CMD_PLAYMUTE				CTRL_CMD_BASE+24		//�طž���
#define CTRL_CMD_PLAYPROGRESS 			CTRL_CMD_BASE+25		//���Ž���
#define CTRL_CMD_GETNETWORK				CTRL_CMD_BASE+26		//����������
#define CTRL_CMD_SETNETWORK				CTRL_CMD_BASE+27		//�����������
#define CTRL_CMD_GETVGASOLLIST			CTRL_CMD_BASE+28		//���VGA�ֱ���
#define CTRL_CMD_GETSYSPARAM			CTRL_CMD_BASE+29		//���ϵͳ����
#define CTRL_CMD_SETSYSPARAM 			CTRL_CMD_BASE+30		//����ϵͳ����
#define CTRL_CMD_GETRECPARAM			CTRL_CMD_BASE+31		//���¼�����
#define CTRL_CMD_SETRECPARAM 			CTRL_CMD_BASE+32		//����¼�����
#define	CTRL_CMD_GETSUBSTREAMPARAM		CTRL_CMD_BASE+33		//�������������
#define	CTRL_CMD_SETSUBSTREAMPARAM		CTRL_CMD_BASE+34		//��������������
#define CTRL_CMD_GETIMGPARAM			CTRL_CMD_BASE+35		//���ͼ�����
#define CTRL_CMD_SETIMGPARAM 			CTRL_CMD_BASE+36		//����ͼ�����
#define CTRL_CMD_GETPICADJ 				CTRL_CMD_BASE+37		//��û������
#define CTRL_CMD_SETPICADJ 				CTRL_CMD_BASE+38		//���û������
#define CTRL_CMD_GETALARMINPARAM 		CTRL_CMD_BASE+39		//��ñ����������
#define CTRL_CMD_SETALARMINPARAM 		CTRL_CMD_BASE+40		//���ñ����������
#define CTRL_CMD_GETALARMOUTPARAM 		CTRL_CMD_BASE+41		//��ñ����������
#define CTRL_CMD_SETALARMOUTPARAM 		CTRL_CMD_BASE+42		//���ñ����������
#define	CTRL_CMD_GETALARMNOTICYPARAM	CTRL_CMD_BASE+43		//��ñ���֪ͨ����
#define	CTRL_CMD_SETALARMNOTICYPARAM	CTRL_CMD_BASE+44		//���ñ���֪ͨ����
#define CTRL_CMD_GETRECSCHPARAM 		CTRL_CMD_BASE+45		//���¼�񲼷�����
#define CTRL_CMD_SETRECSCHPARAM 		CTRL_CMD_BASE+46		//����¼�񲼷�����
#define CTRL_CMD_GETMDPARAM 			CTRL_CMD_BASE+47		//����ƶ�������
#define CTRL_CMD_SETMDPARAM 			CTRL_CMD_BASE+48		//�����ƶ�������
#define CTRL_CMD_GETVIDEOLOSTPARAM		CTRL_CMD_BASE+49		//�����Ƶ��ʧ����
#define CTRL_CMD_SETVIDEOLOSTPARAM 		CTRL_CMD_BASE+50		//������Ƶ��ʧ����
#define CTRL_CMD_GETVIDEOBLOCKPARAM 	CTRL_CMD_BASE+51		//�����Ƶ�ڵ�����
#define CTRL_CMD_SETVIDEOBLOCKPARAM 	CTRL_CMD_BASE+52		//������Ƶ�ڵ�����
#define CTRL_CMD_GETPTZPARAM			CTRL_CMD_BASE+53		//�����̨����
#define CTRL_CMD_SETPTZPARAM 			CTRL_CMD_BASE+54		//������̨����
#define CTRL_CMD_SETPRESET 				CTRL_CMD_BASE+55		//����Ԥ�õ����
#define CTRL_CMD_GETCRUISEPARAM 		CTRL_CMD_BASE+56		//���Ѳ��·������
#define CTRL_CMD_SETCRUISEPARAM 		CTRL_CMD_BASE+57		//����Ѳ��·������
#define CTRL_CMD_CTRLCRUISEPATH 		CTRL_CMD_BASE+58		//Ѳ��·������
#define CTRL_CMD_CTRLPTZTRACK 			CTRL_CMD_BASE+59		//�켣����
#define CTRL_CMD_GETHDDINFO 			CTRL_CMD_BASE+60		//��ô�����Ϣ
#define CTRL_CMD_GETUSERINFO			CTRL_CMD_BASE+61		//����û���Ϣ
#define CTRL_CMD_SETUSERINFO 			CTRL_CMD_BASE+62		//�����û���Ϣ
#define CTRL_CMD_SETRESTORE				CTRL_CMD_BASE+63		//�ָ�Ĭ��
#define CTRL_CMD_CLEARALARM				CTRL_CMD_BASE+64		//�������
#define CTRL_CMD_GETSYSTIME				CTRL_CMD_BASE+65		//���ϵͳʱ��
#define CTRL_CMD_SETSYSTIME				CTRL_CMD_BASE+66		//����ϵͳʱ��
#define CTRL_CMD_GETSYSINFO				CTRL_CMD_BASE+67		//���ϵͳ��Ϣ
#define CTRL_CMD_SHUTDOWN				CTRL_CMD_BASE+68		//�ر�ϵͳ
#define CTRL_CMD_REBOOT					CTRL_CMD_BASE+69		//����ϵͳ
#define CTRL_CMD_PTZCTRL				CTRL_CMD_BASE+70		//��̨����
#define CTRL_CMD_GETMANUALREC			CTRL_CMD_BASE+71		//����ֶ�¼��״̬
#define CTRL_CMD_SETMANUALREC			CTRL_CMD_BASE+72		//�����ֶ�¼��״̬
#define CTRL_CMD_LOGSEARCH				CTRL_CMD_BASE+73		//��־�ļ�����
#define CTRL_CMD_RECFILESEARCH			CTRL_CMD_BASE+74		//�ط��ļ�����
#define CTRL_CMD_GETSPSPPS				CTRL_CMD_BASE+75		//���spspps����
#define CTRL_CMD_ALARMUPLOADCENTER		CTRL_CMD_BASE+76		//�����ϴ�����
#define CTRL_CMD_CDROM					CTRL_CMD_BASE+77		//��������

#define CTRL_CMD_COM_PROTOCOL_GET		CTRL_CMD_BASE+78		//��ô��ڲ���
#define CTRL_CMD_COM_PROTOCOL_SET		CTRL_CMD_BASE+79		//���ô��ڲ���
#define CTRL_CMD_SERIALSTOP				CTRL_CMD_BASE+80		//ֹͣ͸������
#define CTRL_CMD_GETSYSLANGLIST   		CTRL_CMD_BASE+83  		//���ϵͳ�����б�				//dong	100426
#define CTRL_CMD_GETBITRATELIST 		CTRL_CMD_BASE+84 		//�������λ���б�
#define CTRL_CMD_XWSERVER_GET			CTRL_CMD_BASE+85		//csp modify 20100419
#define CTRL_CMD_XWSERVER_SET			CTRL_CMD_BASE+86		//csp modify 20100419

#define CTRL_CMD_GETRECFILEINDEX		CTRL_CMD_BASE+87		//csp modify 20100419
// lihg add 20110808
#define  CTRL_CMD_3GSERVER 				CTRL_CMD_BASE+90 		//����3G������
// lihg add end
#define CTRL_CMD_SETMAC					CTRL_CMD_BASE+92		//����MAC��ַ
#define CTRL_CMD_NRSERVER_GET			CTRL_CMD_BASE+101		//��ȡ�������������
#define CTRL_CMD_NRSERVER_SET			CTRL_CMD_BASE+102		//�����������������
#define CTRL_CMD_GET_ALARMIN_VAL		CTRL_CMD_BASE+103		//��ȡ��������ֵ
#define CTRL_CMD_SET_ALARMOUT_VAL		CTRL_CMD_BASE+104		//���ñ������ֵ

#define CTRL_CMD_STREAM_REQ       		CTRL_CMD_BASE+105

#define CTRL_CMD_PLAYMP3FILE           CTRL_CMD_BASE+106                    // ����mp3�ļ�

#define CTRL_CMD_AUTO_BITRATE        CTRL_CMD_BASE+107                 // ��̬����

#define CTRL_CMD_PLAYMP3FILE_START      CTRL_CMD_BASE+108        	// ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_STOP       CTRL_CMD_BASE+109        	// ֹͣ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_PAUSE      CTRL_CMD_BASE+110       	// ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_SETVOLUME  CTRL_CMD_BASE+111       	// ��������
#define CTRL_CMD_PLAYMP3FILE_GETSTATUS  CTRL_CMD_BASE+112       	// ��ȡMP3����״̬
#define CTRL_CMD_JPEGCAPTURE			CTRL_CMD_BASE+113       	// JPEGץͼ
#define CTRL_CMD_GETDEVICESTATUS		CTRL_CMD_BASE+114       	// ��ȡ�豸״̬

#define CTRL_CMD_GETBUSINFO				CTRL_CMD_BASE+115       	// ��ȡ������Ϣ
#define CTRL_CMD_SETBUSINFO				CTRL_CMD_BASE+116       	// ���ó�����Ϣ
#define CTRL_CMD_UPDATE					CTRL_CMD_BASE+117		//����
#define CTRL_CMD_GET_UPDATE_POS			CTRL_CMD_BASE+118		//�����������
#define CTRL_CMD_GET_UPDATE_STATE		CTRL_CMD_BASE+119		//�������״̬
#define CTRL_CMD_PREVIEWSWITCH_EX		CTRL_CMD_BASE+123           //��������4����Ԥ���л�

#define CTRL_CMD_PLAYMP3FILE_GETVOLUME  CTRL_CMD_BASE+150       	// ��ȡ����ֵ
#define CTRL_CMD_SETOSD_BUSNO  			CTRL_CMD_BASE+151       	// �����豸��osd
#define CTRL_CMD_GETOSD_BUSNO  			CTRL_CMD_BASE+152       	// ��ȡ�豸��osd
#define CTRL_CMD_SHOWOSD_BUSNO  		CTRL_CMD_BASE+153       	// ��ʾ�������豸��osd
#define CTRL_CMD_SETOSD_EXTERN  		CTRL_CMD_BASE+154       	// ������չosd
#define CTRL_CMD_GETOSD_EXTERN   		CTRL_CMD_BASE+155       	// ��ȡ��չosd
#define CTRL_CMD_SHOWOSD_EXTERN			CTRL_CMD_BASE+156      		// ��ʾ��������չosd

#define CTRL_CMD_AOUT_PLAYBACK_ON  			CTRL_CMD_BASE+157       	// �򿪻ط�ͨ��
#define CTRL_CMD_AOUT_PLAYBACK_OFF  		CTRL_CMD_BASE+158       	// �رջط�ͨ��

#define CTRL_BACK_CMD_VOUT_GETDEV_CSC       CTRL_CMD_BASE+159   // ��ȡ��Ƶ���Ч��
#define CTRL_BACK_CMD_VOUT_SETDEV_CSC       CTRL_CMD_BASE+160   // ������Ƶ���Ч��

#define CTRL_CMD_REC_SEARCH             CTRL_CMD_BASE + 170  //¼������
#define CTRL_CMD_START_REC_PLAY         CTRL_CMD_BASE + 171  //¼��ط�
#define CTRL_CMD_STOP_REC_PLAY          CTRL_CMD_BASE + 172  //ֹͣ¼��ط�


#define CTRL_CMD_BASE_EX 				10120
#define CTRL_CMD_GETPARAM_EX			CTRL_CMD_BASE_EX+1
#define CTRL_CMD_SETPARAM_EX			CTRL_CMD_BASE_EX+2


enum TM_APP_CMD
{
    CTRL_CMD_REC_PLAY_CTRL = CTRL_CMD_BASE + 175,       //10175¼��ط��϶�����
    CTRL_CMD_SD_FORMAT,                                 //10176
    CTRL_CMD_INTEGRITY_REFER,                           //10177¼�������Լ��
    CTRL_CMD_START_4CHN_PLAY,                           //10178��ͨ��¼��ط�
    CTRL_CMD_START_TIME_DOWNLOAD,                       //10179��ʱ��¼������
    CTRL_CMD_STOP_TIME_DOWNLOAD,                        //10180ֹͣ¼������
};


#define CTRL_CMD_PPPOE_PREUP			CTRL_CMD_BASE+9999
#define CTRL_CMD_PPPOE_UP				CTRL_CMD_BASE+9998
#define CTRL_CMD_PPPOE_DOWN				CTRL_CMD_BASE+9997
#define CTRL_CMD_PPPOE_DISCONNECT		CTRL_CMD_BASE+9996

#define CTRL_CMD_DHCP_DECONFIG			CTRL_CMD_BASE+9989
#define CTRL_CMD_DHCP_BOUND				CTRL_CMD_BASE+9988
#define CTRL_CMD_DHCP_RENEW				CTRL_CMD_BASE+9987
#define CTRL_CMD_DHCP_NAK				CTRL_CMD_BASE+9986


//�¼�֪ͨ����
#define CTRL_NOTIFY_BASE				20000
#define	CTRL_NOTIFY_CONNLOST			CTRL_NOTIFY_BASE+0		//������Ϣ
#define	CTRL_NOTIFY_HEARTBEAT_REQ		CTRL_NOTIFY_BASE+1		//��������
#define	CTRL_NOTIFY_HEARTBEAT_RESP		CTRL_NOTIFY_BASE+2		//�����ظ�
#define CTRL_NOTIFY_PLAYEND				CTRL_NOTIFY_BASE+3		//�������
#define CTRL_NOTIFY_PLAYPROGRESS		CTRL_NOTIFY_BASE+4		//�������֪ͨ
#define CTRL_NOTIFY_HASAUDIO			CTRL_NOTIFY_BASE+5		//�ط��ļ��Ƿ�����Ƶ
#define CTRL_NOTIFY_UPDATEPROGRESS		CTRL_NOTIFY_BASE+6		//��������
#define CTRL_NOTIFY_ALARMINFO			CTRL_NOTIFY_BASE+7		//�첽������Ϣ
#define CTRL_NOTIFY_DEVICE_STATE			CTRL_NOTIFY_BASE+8		// �豸״̬��Ϣ

//Ӧ������
#define CTRL_SUCCESS					0						//�ɹ�
#define CTRL_FAILED_BASE				30000					//������ƫ����
#define CTRL_FAILED_USER				CTRL_FAILED_BASE+1		//�����ڵ��û���
#define CTRL_FAILED_PASSWORD			CTRL_FAILED_BASE+2		//�������
#define CTRL_FAILED_COMMAND				CTRL_FAILED_BASE+3		//δ�Ͽɵ�����
#define CTRL_FAILED_PARAM				CTRL_FAILED_BASE+4		//��Ч����
#define CTRL_FAILED_OUTOFMEMORY			CTRL_FAILED_BASE+5		//�ڴ治��
#define CTRL_FAILED_RESOURCE			CTRL_FAILED_BASE+6		//��Դ����
#define CTRL_FAILED_FILENOTEXIST		CTRL_FAILED_BASE+7		//�ļ�������
#define CTRL_FAILED_DATABASE			CTRL_FAILED_BASE+8		//���ݿ����
#define CTRL_FAILED_RELOGIN				CTRL_FAILED_BASE+9		//�ظ���¼
#define CTRL_FAILED_BAUDLIMIT			CTRL_FAILED_BASE+10		//ÿһ·ͨ�����֧��5·ʵʱ���
#define CTRL_FAILED_CREATESOCKET		CTRL_FAILED_BASE+11		//�����׽���ʧ��
#define CTRL_FAILED_CONNECT				CTRL_FAILED_BASE+12		//��������ʧ��
#define CTRL_FAILED_BIND				CTRL_FAILED_BASE+13		//��ʧ��
#define CTRL_FAILED_LISTEN				CTRL_FAILED_BASE+14		//����ʧ��
#define CTRL_FAILED_NETSND				CTRL_FAILED_BASE+15		//���緢�ͳ���
#define CTRL_FAILED_NETRCV				CTRL_FAILED_BASE+16		//������ճ���
#define CTRL_FAILED_TIMEOUT				CTRL_FAILED_BASE+17		//�������ӳ�ʱ
#define CTRL_FAILED_CHNERROR			CTRL_FAILED_BASE+18		//����ͨ������
#define CTRL_FAILED_DEVICEBUSY			CTRL_FAILED_BASE+19		//�豸����æ
#define CTRL_FAILED_WRITEFLASH			CTRL_FAILED_BASE+20		//��дflash����
#define CTRL_FAILED_VERIFY				CTRL_FAILED_BASE+21		//У���
#define CTRL_FAILED_CONFLICT			CTRL_FAILED_BASE+22		//ϵͳ��Դ��ͻ
#define CTRL_FAILED_BUSY				CTRL_FAILED_BASE+23		//ϵͳ����æ
#define CTRL_FAILED_LINKLIMIT			CTRL_FAILED_BASE+24		//�Ѵﵽ��������
#define CTRL_FAILED_USER_SAME			CTRL_FAILED_BASE+25		//�û�����ͬ07-08-02
#define CTRL_FAILED_MACADDR				CTRL_FAILED_BASE+26		//Զ�̷��ʵ�pc mac��ַ����
#define CTRL_FAILED_NOINIT				CTRL_FAILED_BASE+27		//ģ��δ��ʼ��
#define CTRL_FAILED_USER_MAX			CTRL_FAILED_BASE+28		//�û������//wrchen 080529

#define CTRL_FAILED_NOT_START_VIDEO		CTRL_FAILED_BASE+31	// ��Ƶ����û�д�
#define CTRL_FAILED_NO_CAMERA		    CTRL_FAILED_BASE+32		// �����ͨ��û�н�����ͷ
#define CTRL_FAILED_CALL_API		    CTRL_FAILED_BASE+33		// ����ƵԤ���ӿڵ���ʧ��

#define CTRL_FAILED_UNKNOWN				CTRL_FAILED_BASE+9999	//δ֪����
//����
#define CTRL_CONNECTION_NULL			0x0
#define CTRL_CONNECTION_TCPCLIENT		0x1
#define CTRL_CONNECTION_TCPSERVER		0x2

//�鲥�����豸
#define CTRL_DEVICESEARCH_ACKCLIENT		0X1
#define CTRL_DEVICESEARCH_ACKSERVER		0x2
#define SEARCHPORT						6666
#define ACKSEARCHPORT					SEARCHPORT+1
#define MULTICASTGROUP					"224.0.1.2"

//��������
#define VIDEO_CHANNELS		16
#define AUDIO_CHANNELS		16
#define EACH_STREAM_TCP_LINKS	5
#define MAX_REMOTE_PLAYER_NUM	8
#define VOIP_NUM				1
#define DOWNLOAD_NUM			1
#define	UPDATE_NUM				1
#define MEDIA_LINKMAX_SVR			(VIDEO_CHANNELS+AUDIO_CHANNELS)*(EACH_STREAM_TCP_LINKS)+MAX_REMOTE_PLAYER_NUM+VOIP_NUM
#define	STREAM_LINK_MAXNUM		MEDIA_LINKMAX_SVR+DOWNLOAD_NUM+UPDATE_NUM

// ����������ģʽ�µ�Э�鶨��
// ��������
#define SERVER_CTRL_CMD_BASE                      40000
#define SERVER_CTRL_CMD_DEVICE_LOGIN              SERVER_CTRL_CMD_BASE+1   // �豸��½
#define SERVER_CTRL_CMD_CLIENT_LOGIN              SERVER_CTRL_CMD_BASE+2   // �ͻ��˵�½
#define SERVER_CTRL_CMD_GET_DEVICE                SERVER_CTRL_CMD_BASE+3   // ��ȡ�豸
#define SERVER_CTRL_CMD_MEDIA_STREAM              SERVER_CTRL_CMD_BASE+4   // ������
#define SERVER_CTRL_CMD_HEARTBEAT			      SERVER_CTRL_CMD_BASE+5

#define SERVER_CTRL_CMD_DEVICE_LOGOUT   SERVER_CTRL_CMD_BASE+6   // �豸����

#define SERVER_CTRL_CMD_GET_DEVICE_STATE		SERVER_CTRL_CMD_BASE+8   // �����豸״̬��Ϣ
#define SERVER_CTRL_CMD_STOP_DEVICE_STATE	SERVER_CTRL_CMD_BASE+9   // ֹͣ�ϴ��豸״̬��Ϣ

#define SERVER_CTRL_CMD_GET_GPS_INFO			SERVER_CTRL_CMD_BASE+10   // ����GPS��Ϣ
#define SERVER_CTRL_CMD_STOP_GPS_INFO		SERVER_CTRL_CMD_BASE+11   // ֹͣ�ϴ�GPS��Ϣ

#define SERVER_CTRL_CMD_DEVICE_WIFI_LOGIN              SERVER_CTRL_CMD_BASE+13   // �豸��½

#define SERVER_CTRL_NOTIFY_BASE                   50000
#define SERVER_CTRL_NOTIFY_DEVICE_UPDATE          SERVER_CTRL_NOTIFY_BASE+1 // �豸����֪ͨ

#define SERVER_CTRL_NOTIFY_DEVICE_STATE          SERVER_CTRL_NOTIFY_BASE+2 // �豸״̬֪ͨ

#define SERVER_CTRL_NOTIFY_DEVICE_GPS          SERVER_CTRL_NOTIFY_BASE+3 // �豸gps֪ͨ


#pragma pack( push, 1 )

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
}PACK_NO_PADDING IFLY_MediaFRAMEHDR_t;


typedef struct
{
	u8			data[6];
}PACK_NO_PADDING CPGuid;

typedef struct
{
	u32         ip;
	u16			port;
	CPGuid		guid;
	SOCKHANDLE	sockfd;
	u8			conntype;
	u8          newmsgcome;
	u8          nolnkcount;
}PACK_NO_PADDING ifly_cp_t, *CPHandle;

typedef struct
{
	u32		length;								//��Ϣ����
	u16		type;								//��Ϣ����
	u16		event;								//��Ϣ��
	u16		number;								//��ˮ��
	u16		version;								//�汾��
}PACK_NO_PADDING ifly_cp_header_t;

typedef struct
{
	char	ifrname[16];
	u32		ip;
	u32		subnet;
	u32		broadcast;
	u32		gateway;
	u32		dns;
}PACK_NO_PADDING ifly_cp_dhcp_t;

/************************/
//07-09-03
typedef struct
{
 	u16		width;
 	u16		height;
}PACK_NO_PADDING ifly_pic_resolution_t;							//�������͸��ͻ��˵�ͼ��ֱ���

typedef struct
{
 	u8		channel_no;
}PACK_NO_PADDING ifly_need_resolution_t;						//��ͨ����Ҫ��������ͼ��ֱ���
/*************************/

typedef struct
{
	char	usrname[12];
}PACK_NO_PADDING ifly_usrname_header;

typedef struct
{
	u16		startID;
	u16		max_return;
}PACK_NO_PADDING ifly_remusr_send_t;


//Զ���ֶ�¼��ṹ
typedef struct
{
	u8		chan_rec[16];
	u8		allrec;
	u8		allstop;
}PACK_NO_PADDING ifly_hand_rec_t;

//Զ����־��ѯ
typedef struct
{
	u8		query_mode;							//��ѯ��ʽ
	u8		main_type;							//������
	u8		slave_type;							//������
	u16		max_return;							//��󷵻���
	u16		startID;							//���شӵ�һ����¼��ʼ
	u32		start_time;							//��ʼʱ��
	u32		end_time;							//����ʱ��
	u8 langID;									//��ѯ�����ʾ����
}PACK_NO_PADDING ifly_research_log_t;

typedef struct
{
	u32		startTime;
	char	main[64];							//������
	char	slave[64];							//������
	char	loginfo[32];						//������Ϣ07-10-09
}PACK_NO_PADDING ifly_log_back_t;

//Զ�̵�¼
typedef struct
{
	char	username[12];
	char	loginpass[12];
	char	macAddr[18];						//���ӿͻ���mac��ַ�ֶ�
	u32		ipAddr;
}PACK_NO_PADDING ifly_loginpara_t;

typedef struct
{
	char	username[12];
	char	loginpass[12];
}PACK_NO_PADDING ifly_logininfo_t;

typedef struct
{
	char	usrname[12];
	char	remote_privilege[16];
}PACK_NO_PADDING ifly_remusr_limit;

typedef struct
{
	int		link_online;
}PACK_NO_PADDING ifly_link_online;

//Զ���û�������Ϣ�ṹ
typedef struct
{
	char	name[12];
	char	password[12];
	char	local_privilege[16];
	char	remote_privilege[16];
	char	mac_address[18];
}PACK_NO_PADDING ifly_userinfo_t;

//Զ���û��༭�����ṹ
typedef struct
{
	char	name[12];							//�û���
	char	passwd[12];							//����
	char	confirm[12];						//У������
	char	macaddr[18];						//�����ַ
	u32		index;								//�û�Ȩ��
	u32		rcamer;								//Զ����̨Ȩ��
	u32		rrec;								//Զ��¼��Ȩ��
	u32		rplay;								//Զ�̻ط�Ȩ��
	u32		rsetpara;							//Զ�����ò���
	u32		rlog;								//Զ����־Ȩ��
	u32		rtool;								//Զ�̹���Ȩ��
	u32		rpreview;							//Զ��Ԥ��Ȩ��
	u32		ralarm;								//Զ�̱���Ȩ��
	u32		rvoip;								//�����Խ�Ȩ��
	u32		lcamer;								//������̨Ȩ��
	u32		lrec;								//����¼��Ȩ��
	u32		lplay;								//���ػط�Ȩ��
	u32		lsetpara;							//�������ò���
	u32		llog;								//������־Ȩ��
	u32		ltool;								//���ع���Ȩ��
}PACK_NO_PADDING ifly_useredit_t;

//Զ���û�ɾ�������ṹ
typedef struct
{
	u32		user_index;							//��ǰɾ���û���¼
	char	name[12];							//��ǰ��ɾ���û���
}PACK_NO_PADDING ifly_userdel_t;

//Զ���û������޸Ĳ���
typedef struct
{
	char	name[12];
	char	oldpass[12];
	char	newpass[12];
	char	confirm[12];
}PACK_NO_PADDING ifly_editpass_t;

//Զ�̷���Ӳ����Ϣ�ṹ
typedef	struct
{
	s64		capability;//Byte
	s64		freesize;//Byte
}PACK_NO_PADDING ifly_remhdd_t;

typedef struct
{
	u8		hdd_exist[8];
}PACK_NO_PADDING ifly_hdd_desc_t;

//Զ��ϵͳ��Ϣ����
typedef struct
{
	char	devicename[32];
	char	devicemodel[32];
	char	deviceser[32];
	char	version[64];
}PACK_NO_PADDING ifly_recsysinfo_t;

//Զ��ϵͳ�����ṹ added by kmli
typedef struct
{
	u16		device_id;
	char	device_name[32];
	u8		flag_overlap;
	u32		time;
	u8		lock_time;
	u8		switch_time;
	u8		flag_statusdisp;
	u8		video_format;
	u8		vga_solution;
	u8		transparency;//07-10-16
	u8		language;//07-10-16
	u8		preview;
	u8 		disk_delay;
	u8  	shut_delay;
}PACK_NO_PADDING ifly_syspara_send_t;

//Զ����̨����
typedef struct
{
	u8		channel_no;
	u16		address;
	u8		baud_ratesel;
	u8		data_bitsel;
	u8		stop_bitsel;
	u8		crccheck;
	u8		flow_control;
	u8		protocol;
}PACK_NO_PADDING ifly_yuntai_send_t;

typedef struct
{
	CPGuid	guid;								//����mac��ַ
	u32		dwIp;								//����ip
	u16		wPort;								//���ؽ������ݵĶ˿�
	u8		byChn;								//ͨ����,��ַΪ0
	u8		byFlag;								//0:audio;1:video
}PACK_NO_PADDING ifly_nat_t;

//Զ�̼��
typedef struct
{
	CPGuid	guid;								//����mac��ַ
	u32		dwIp;								//����ip
	u16		wPort;								//���ؽ������ݵĶ˿�
	u8		byChn;								//ͨ����,��ַΪ0
	u8 		byFlagSubcode;						//�Ƿ���������
}PACK_NO_PADDING ifly_monitor_param_t;

//���ļ�Զ�̻ط�
typedef struct
{
	u32		dwIp;								//����ý�����ݵ�ip
	u16		wVideoPort;							//������Ƶ���ݵĶ˿�
	u16		wAudioPort;							//������Ƶ���ݵĶ˿�
	char	filename[64];						//�طŵ��ļ�
	u32		offset;								//�ļ�ƫ��
}PACK_NO_PADDING ifly_remote_playback_t;

typedef struct
{
	u32		id;
}PACK_NO_PADDING ifly_play_handle_t;

typedef struct
{
	u32		curpos;
	u32		totaltime;
}PACK_NO_PADDING ifly_play_progress_t;

typedef struct
{
	u32		dwIp;								//�����ļ����ݵ�ip
	u16		wPort;								//�����ļ����ݵĶ˿�
	char	filename[64];						//���ص��ļ�
	u32		offset;								//�ļ�ƫ��
	u32		size;								//�ļ���С
}PACK_NO_PADDING ifly_remote_download_t;

typedef struct
{
	u32		id;
}PACK_NO_PADDING ifly_download_handle_t;

/*********************/
typedef struct
{
	u32		dwIp;
	u16		wPort;
}PACK_NO_PADDING ifly_remote_layout_head;

typedef struct
{
	char	filename[64];
	u32		size;
	u16		reserved;
}PACK_NO_PADDING ifly_remote_layout_t;

typedef struct
{
	u8		channel_no;							//ͨ����
	u8		flag_mask;							//�Ƿ���ʾ
	u8		mask_index;							//�ڸ���������
	u32		x;									//�ڸ���������x����
	u32		y;									//�ڸ���������y����
	u32   	width;								//�ڸ�������
	u32   	height;								//�ڸ�����߶�
}PACK_NO_PADDING ifly_maskset_t;

typedef	struct
{
	u8		channel_no;							//ͨ����
	u8		flag;								//���ڱ�־:0-3
	u8		val;								//����ֵ
}PACK_NO_PADDING ifly_picset_t;
/*********************/

typedef struct
{
	char	filename[64];						//�ļ���
	u32		size;								//�ļ�����
	u32		verify;								//У���
	u16		version;							//�ļ��汾��
	u16		reserved;
}PACK_NO_PADDING ifly_remote_upload_t;

typedef struct
{
	u32		dwIp;
	u16		wPort;
}PACK_NO_PADDING ifly_voip_t;

//�ط��ļ�����
typedef struct
{
	u16		channel_mask;				//ͨ�� ��λ���
	u16		type_mask;					//���� ��λ���
	u32		start_time;					//��ʼʱ��
	u32		end_time;					//��ֹʱ��
	u16		startID;					//���صĵ�һ����¼,��1��ʼ
	u16		max_return;					//ÿ�η��ص�����¼��
	u8		reserved[4];				//Ԥ��
}PACK_NO_PADDING ifly_recsearch_param_t;

//Զ�̻طŷ��ص���Ϣ�ṹ
typedef struct
{
	u8		channel_no;
	u8		type;
	u32		start_time;
	u32		end_time;
	u8		image_format;						//3:cif;4:4cif
	u8		stream_flag;						//0:��Ƶ��;1:��Ƶ��
	u32		size;
	u32		offset;
	char	filename[64];
}PACK_NO_PADDING ifly_recfileinfo_t;

typedef struct
{
	u16		sum;								//�ܼ�¼��
	u16		startID;							//��ʼ�ļ�¼,��ַΪ1.���޷�������ʱ,startID��ֵΪ0
	u16		endID;								//�����ļ�¼,��ַΪ1.��endID��startID���ʱ,��ʾ֮����һ����¼
}PACK_NO_PADDING ifly_recfile_desc_t;

typedef struct
{
	char	mac_address[18];
	u32		ip_address;
	u16		wifimask;		//wifi mask
	u32		net_mask;
	u32		net_gateway;
	u32		dns;
	u32		wifi_address;
	u32		admin_host;
	u16		host_port;
	u16		http_port;
	u8		flag_pppoe;
	char	user_name[64];
	char	passwd[64];
	char	pwdcheck[64];
	u8      flag_dhcp;
	u8		flag_ddns;
	u8		ddns_type;
	u8 		flag_wifi;		//for wifi flag	bit   0: wifi flag 1: static/danamic ip  2: password flag
	char	ddns_domain[64];
	char	ddns_user[64];
	char	ddns_passwd[64];

	u8 auto_adjust_frame_bit_rate;
	u16 ctrl_port;
	u16 data_port;

	u8 ServerDomainEnable;
	char ServerDomainname[64];

	u32 gpsctrl_ip;
	u16 gps_signaling_port;

}PACK_NO_PADDING ifly_network_t;

//�������������Ϣ
//WIFI�����Ϣ
typedef struct
{
	u8 		flag;			//�Ƿ�����WIFI
	u8 		ip_type;		//IP��ַ����:��̬/��̬
	u8		need_passwd;	//�Ƿ���������
	u8		reserved;		//����λ
	u32		ipaddr;			//ip address
	u32		netmask;		//��������
	u32		gateway;		//����
	u8		essid[32];		//ESSID
	u8		passwd[32];		//����
}PACK_NO_PADDING ifly_wifi_t;

//3G�����Ϣ
typedef struct
{
	u8		type;		//0:none 1:evdo189* 2:evdo���� 3:wcdma
	u8		connect;	//���ӷ�ʽ: ������; �ֻ�����
	u8		bittype;	//���ö�̬������񣻸��ݴ�������Ƿ��������������֡��
	u8		reserved;	//����λ
	u8		user[32];	//�û���
	u8		passwd[32];	//����
	u8		phone[32];	//�绰����,WCDMA:*99***1#, EVDO:#777
}PACK_NO_PADDING ifly_3g_t;


//Զ��ͼ����������ṹ
typedef struct
{
	u8		channel_no;							//ͨ����
	char	channelname[32];					//ͨ����
	//��λ����ص�
	u8		flag_name;							//����λ����ʾ
	//ʱ����ص�
	u8		flag_time;							//ʱ��λ����ʾ
	u8		flag_mask;							//�ڸ�
	u8		overlay;							//�ڵ�����
	u8		lost;								//��Ƶ��ʧ
	u8		motion;								//�ƶ����
	//����ʽ
	char	record_channel[16];					//������¼��ͨ��
	u8		flag_buzz;							//��������
	u8		flag_send;							//�ϴ�����
	char	alarm_out[16];						//�����������
}PACK_NO_PADDING ifly_imagset_t;

//OSDλ������ 07-07-27
typedef struct
{
	u8		channel_no;							//ͨ����
	u8		flag_name;							//����λ���Ƿ���ʾ
	u16		x;									//����λ�����Ͻ�x����
	u16		y;									//����λ�����Ͻ�y����
}PACK_NO_PADDING ifly_channelset_t;

typedef struct
{
	u8		channel_no;							//ͨ����
	u8		flag_time;							//ʱ��λ���Ƿ���ʾ
	u16		x;									//ʱ��λ�����Ͻ�x����
	u16		y;									//ʱ��λ�����Ͻ�y����
}PACK_NO_PADDING ifly_timeset_t;

typedef struct									//�ڵ�����Ƶ��ʧ���ƶ���Ⲽ�������ṹ
{
	u8		channel_no;							//ͨ����
	u8		week;								//����
	u8		flag_alltime;						//�Ƿ�ȫ�첼��
	u32		starttime[4];						//ʱ���1~4����ʼʱ��
	u32		endtime[4];							//ʱ���1~4����ֹʱ��
}PACK_NO_PADDING ifly_valid_t;

typedef struct									//Զ���ƶ���ⱨ����������ṹ
{
	u8		channel_no;							//ͨ����
	u8		recchan[16];						//������¼��ͨ��
	u8		flag_buzz;							//�Ƿ���������
	u8		flag_send;							//�Ƿ��ϴ�����
	u8		alarmout;							//�Ƿ񴥷��������
	u8		alarm_out[4];						//����ĳ·�ı������
}PACK_NO_PADDING ifly_motion_t;

typedef struct									//Զ���쳣����ҳ������ṹ
{
	u8		exceptiontype;						//�쳣����
	u8		flag_display;						//�Ƿ�������ʾ
	u8		flag_buzz;							//�Ƿ���������
	u8		flag_send;							//�Ƿ��ϴ�����
	u8		alarmout;							//�Ƿ񴥷��������
	u8		alarm_out[4];						//����ĳ·�ı������
}PACK_NO_PADDING ifly_exceptioninfo_t;

typedef struct									//Զ��¼������ҳ������ṹ
{
	u8		channelno;							//ͨ����
	char	channelname[32];					//ͨ����
	u8		code_type;							//��Ƶ����
	u8		video_resolution;					//�ֱ���
	u8		bit_type;							//λ������
	u32		bit_max;							//λ������
	u8		quality;							//ͼ������
	u8		frame_rate;							//��Ƶ֡��
	u16		pre_record;							//Ԥ¼ʱ��
	u16		post_record;						//¼����ʱ
	u8		flag_record;						//�Ƿ�����¼��
	//wrchen 090210
	u8 		yfactor;							//�жϵ�ǰ��Ƶ��ʽ05-13

	u8 		sub_size;									//�������Ĵ�С qcif cif
	u8 		sub_framerate;								//��������֡��
	int 	sub_bitrate;								//��������λ��
}PACK_NO_PADDING ifly_recordinfo_t;

typedef struct									//Զ��¼������ҳ������ṹ
{
	u8		channelno;							//ͨ����
	u8		week;								//����
	u8		flag_alltime;						//�Ƿ�ȫ�첼��
	u8		alltype;							//ȫ�첼��ʱ��¼������
	u32		start_time[4];						//ʱ���1~4����ʼʱ��
	u32		end_time[4];						//ʱ���1~4����ֹʱ��
	u8		type[4];							//ʱ���1~4��¼������
}PACK_NO_PADDING ifly_recordtime_t;

typedef struct									//Զ�̱�������ҳ��
{
	u8		inid;								//����������
	u8		typein;								//������������
	u8		flag_deal;							//�Ƿ񴥷��������봦��
	u8		copyin;
	u8		in_copy;							//�Ƿ�ѡ�񱨾����ĸ���
	u8		outid;								//ѡ�񱨾����
	u8		typeout;							//�����������
	u8		out_copy;							//�����������
	u8		copyout;
	u16		delay;								//���������ʱ
}PACK_NO_PADDING ifly_alarmset_t;

struct _alarmset_in_
{
	u8		typein;								//������������
	u8		flag_deal;							//�Ƿ񴥷��������봦��
}PACK_NO_PADDING;

struct _alarmset_out_
{
	u8		typeout;							//�����������
	u16		delay;								//���������ʱ
}PACK_NO_PADDING;

typedef struct									//Զ��ptz����
{
	u8		inid;								//����������
	u8		channo;								//ͨ����
	u8		flag_preset;						//Ԥ�õ�
	u8		preset;
	u8		flag_cruise;						//Ѳ����
	u8		cruise;
	u8		flag_track;							//�켣
}PACK_NO_PADDING ifly_alarmin_ptz;

typedef struct									//��������
{
	u8		id;									//ѡ�񱨾���
	u8		week;								//����
	u8		flag_alltime;						//ȫ�첼��
	u32		starttime[4];						//ʱ���1~4����ʼʱ��
	u32		endtime[4];							//ʱ���1~4����ֹʱ��
}PACK_NO_PADDING ifly_alarm_settime;

#define MD_ROW			18
#define MD_COL			22
#define MD_MIN_SENSE	0						//�����
#define MD_MAX_SENSE	5						//������

typedef struct
{
	u8	channel_no;
	u8	flag;									//�Ƿ����ƶ����
	u8	sense;									//������ MD_MIN_SENSE-MD_MAX_SENSE
	u8	block[MD_ROW*MD_COL];					//��0ѡ�� 0��ѡ��
}PACK_NO_PADDING ifly_md_param_t;

#ifdef TONGLI_PROTOCOL
typedef struct
{
	u8 s_filepath[32];			//�ļ���
	u8 send_pic_buff[10240];		//ͼƬ���Ͱ�
	u16 pic_buff_to_send_size;	//ͼƬ���ʹ�С
	u16 picsize;					//�ļ���С
}PACK_NO_PADDING ifly_serial_picinfo_t;

typedef struct
{
	u8 send_buf[352*288*2];
	u16 send_size;
}PACK_NO_PADDING ifly_serial_Send_t;

#endif
#ifdef RS232_HISENSE
typedef struct _DHTIME
{
 DWORD second  :6;          // �� 1-60
 DWORD minute  :6;          // �� 1-60
 DWORD hour   :5;            // ʱ 1-24
 DWORD day   :5;             // �� 1-31
 DWORD month   :4;          // �� 1-12
 DWORD year   :6;            // �� 2000-2063
}PACK_NO_PADDING DHTIME,*pDHTIME;

typedef struct
{
	u8		dst_addr;
	u8		src_addr;
	u8		pkg_type;
	u8		serial_num;			//������ˮ��
	u16		pkg_long;			//�������ݵ��ֽ���
}PACK_NO_PADDING ifly_serial_header_t;

typedef struct
{
	u8		son_pkg_type;
	u16		son_pkg_long;
	u8		*son_buf;
}PACK_NO_PADDING ifly_serial_son_t;

typedef struct
{
	u8 chn_no;
	u8 type;
	u8 image_quality;
	DHTIME start_time;
	DHTIME end_time;
	u32 file_size;
	u32 disk_no;
	char file_name[24];
	char extern_name[4];
}PACK_NO_PADDING ifly_serial_recfile_info_t;

typedef struct
{
	u8 send_buf[352*288*2];
	u16 send_size;

}PACK_NO_PADDING ifly_serial_Send_t;

typedef struct
{
	u8 in_tem;
	u8 out_tem;
}PACK_NO_PADDING ifly_serial_Temperature_t;

typedef struct
{
	char longitude[10]; //����
	char latitude[10];	  //γ��
	u8 speed;
}PACK_NO_PADDING ifly_serial_GPS_t;

typedef struct
{
	u8 get_on;
	u8 get_off;
	u8 passenger_num;
}PACK_NO_PADDING ifly_serial_Passengerflow_t;

typedef struct
{
	u8 state0;
	u8 state1;
	u8 state2;
}PACK_NO_PADDING ifly_serial_Carstate_t;
#endif

#ifdef OSD_FROM_232
typedef struct
{
	u8 state[5];
	u8 speed[16];
	u8 accinfo[16];
	u8 brakeinfo[16];
	u8 left[16];
	u8 right[16];
}PACK_NO_PADDING ifly_serial_osdinfo_t;
#endif
#define TIM_SEG_NUM 4
#define TIM_WEEK_DAY 7
typedef struct
{
	u8 flag;
	u8 hour;
	u8 min;
	u8 sec;
}PACK_NO_PADDING ifly_week_day;

typedef struct
{
	u32 from_mon;
	u32 from_day;
	u32 to_mon;
	u32 to_day;
	ifly_week_day week_day[TIM_WEEK_DAY];
}PACK_NO_PADDING ifly_Time_Segment;

typedef struct
{
	u8 flag[TIM_SEG_NUM];
	ifly_Time_Segment time_seg_t[TIM_SEG_NUM];
}PACK_NO_PADDING ifly_Startup_Close;

// lwj add 100715

//�豸��Ϣ
typedef struct
{
	u32	deviceIP; 						//�豸IP
	u16	devicePort;					//�豸�˿�
	char device_name[32];				//�豸����
	char device_mode[32];				//�豸�ͺ�
	u8	maxChnNum;						//���ͨ����
	u8	maxAduioNum;					//�����Ƶ��
	u8	maxSubstreamNum;				//�����������
	u8	maxPlaybackNum;					//���ط���
	u8	maxAlarmInNum;					//��󱨾�������
	u8	maxAlarmOutNum;					//��󱨾������
	u8	maxHddNum;						//���Ӳ����
	u8	reserved[16];					//Ԥ��
}PACK_NO_PADDING ifly_DeviceInfo_t;

//��Ƶ����
typedef struct
{
	u8	videoEncType;					//��Ƶ��������
	u16	max_videowidth;					//
	u16	max_videoheight;
	u8	reserved[3];					//Ԥ��
}PACK_NO_PADDING ifly_Video_Property_t;

//��Ƶ����
typedef struct
{
	u8	audioEnctype;						//Ԥ����Ƶ��������
	u8	audioBitPerSample;				//Ԥ����Ƶλ��
	u16 audioSamplePerSec;				//Ԥ����Ƶ������
	u16 audioFrameSize;					//Ԥ����Ƶÿ֡��С
	u16	audioFrameDurTime;				//Ԥ����Ƶÿ֡���
	u8	reserved[4];						//Ԥ��
}PACK_NO_PADDING ifly_Audio_Property_t;

//VOIP����
typedef struct
{
	u8	VOIPBitPerSample;				//�����Խ�λ��
	u16 VOIPSamplePerSec;				//�����Խ�������
	u16 VOIPFrameSize;					//�����Խ�ÿ֡��С
	u8	reserved[3];					//Ԥ��
}PACK_NO_PADDING ifly_VOIP_Property_t;

//�ƶ��������
typedef struct
{
	u8	MDCol;							//�ƶ��������
	u8	MDRow;							//�ƶ��������
	u8	reserved[4];					//Ԥ��
}PACK_NO_PADDING ifly_MD_Property_t;

//�������
typedef struct
{
	char	mac_address[18];			//mac��ַ
	u32		ip_address;					//ip��ַ
	u16		server_port;				//�豸�˿�
	u32		net_mask;					//����
	u32		net_gateway;				//����
	u32		dns;						//dns
	u8		flag_multicast;				//�鲥���ñ��
	u32		multicast_address;			//�鲥��ַ
	u16		http_port;					//http�˿�
	u8		flag_pppoe;					//pppoe���ñ��
	char	pppoe_user_name[64];		//pppoe�û���
	char	pppoe_passwd[64];			//pppoe����
	u8      flag_dhcp;					//dhcp���ñ�־
	u8		ddnsserver;					//ddns������
	u8		flag_ddns;					//ddns���ñ�־
	char	ddns_domain[64];			//ddns����
	char	ddns_user[64];				//ddns�û���
	char	ddns_passwd[64];			//ddns����
	u32		centerhost_address;			//���ķ�������ַ
	u16		centerhost_port;			//���ķ������˿�
	u16		mobile_port;				//�ֻ���ض˿�
	u8		reserved[14];				//�����ֶ�
}PACK_NO_PADDING ifly_NetWork_t;

//�ֱ������֧��16�ֲ�֧�ֵ�����0
typedef struct
{
	u16		width;						//�ֱ��ʿ�
	u16		height;						//�ֱ��ʸ�
	u8		flashrate;					//�ֱ���ˢ����
}PACK_NO_PADDING ifly_VGA_Pro;

//¼�����
typedef struct
{
	u8		channelno;					//ͨ����
	u8		code_type;					//��������
	u8		bit_type;					//λ������
	u32		bit_max;					//λ������
	u16		intraRate;					//�ؼ�֡���
	u8		qi;							//�ؼ�֡��������
	u8		minQ;						//��С��������
	u8		maxQ;						//�����������
	u8		quality;					//ͼ������
	u8		frame_rate;					//��Ƶ֡��
	u16		pre_record;					//Ԥ¼ʱ��
	u16		post_record;				//¼����ʱ
	u32		copy2chnmask;				//���Ƶ�����ͨ����ÿһλһ��ͨ��
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_RecordParam_t;

//����������
typedef struct
{
	u8		channelno;					//ͨ����
	u8		sub_flag;					//���ֶ������������� 0 cif 1 qcif
	u8		sub_bit_type;				//������λ������
	u16		sub_intraRate;				//�ؼ�֡���
	u8		sub_qi;						//�ؼ�֡��������
	u8		sub_minQ;					//��С��������
	u8		sub_maxQ;					//�����������
	u8		sub_quality;				//������ͼ������
	u8 		sub_framerate;				//��������֡��
	int 	sub_bitrate;				//��������λ��
	u32		copy2chnmask;				//���Ƶ�����ͨ����ÿһλһ��ͨ��
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_SubStreamParam_t;

//ͼ�����
typedef struct
{
	u8		channel_no;					//ͨ����
	char	channelname[32];			//ͨ����
	u8		flag_name;					//����λ����ʾ
	u16		chnpos_x;					//����x����
	u16		chnpos_y;					//����y����
	u8		flag_time;					//ʱ��λ����ʾ
	u16		timepos_x;					//ʱ��x����
	u16		timepos_y;					//ʱ��y����
	u8		flag_mask;					//�ڸ�
	struct  Mask_t
	{
		u16	 	x;
		u16		y;
		u16		width;
		u16		height;
	}PACK_NO_PADDING MaskInfo[4];
	u8		flag_safechn;				//��ȫͨ�����
	u32		copy2chnmask;				//���Ƶ�����ͨ����ÿһλһ��ͨ��
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_ImgParam_t;

//�������
typedef	struct
{
	u8		channel_no;					//ͨ����
	u8		flag;						//���ڱ�־:0-3
	u8		val;						//����ֵ
	u32		copy2chnmask;				//���Ƶ�����ͨ����ÿһλһ��ͨ��
}PACK_NO_PADDING ifly_PicAdjust_t;

//�����������
typedef struct
{
	u8		inid;						//����������
	u8		typein;						//������������
	u8		flag_deal;					//�Ƿ񴥷��������봦��
	u32		triRecChn;					//����¼��ͨ����ÿһλһͨ��
	u32		triAlarmoutid;				//���������������λ
	u8		flag_buzz;					//����������
	u8		flag_email;					//����emaill
	u8		flag_mobile;				//�����ֻ�����
	u16		delay;						//���������ʱ
	struct  AlarmInPtz_t				//����ptz
	{
		u8		flag_preset;			//Ԥ�õ�
		u8		preset;
		u8		flag_cruise;			//Ѳ����
		u8		cruise;
		u8		flag_track;				//�켣
	} PACK_NO_PADDING AlarmInPtz [32];
	u32		copy2AlarmInmask;			//���Ƶ������������롣��λ
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_AlarmInParam_t;


//��Ƶ��ʧ
typedef struct
{
	u8 		chn;						//ͨ��
	u32 	trigRecChn;					//����¼��ͨ�� ��λ
	u32 	trigAlarmOut;				//����������� ��λ
	u8 		flag_buzz;					//������
	u8		flag_email;					//����emaill
	u8		flag_mobile;				//�����ֻ�����
	u16 	delay;						//��ʱ
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_VideoLostParam_t;

//��Ƶ�ڵ�
typedef struct
{
	u8 		chn;						//ͨ��
	u32 	trigRecChn;					//����¼��ͨ�� ��λ
	u32 	trigAlarmOut;				//����������� ��λ
	u8 		flag_buzz;					//������
	u8		flag_email;					//����emaill
	u8		flag_mobile;				//�����ֻ�����
	u16 	delay;						//��ʱ
	u8		reserved[16];				//�����ֶ�
}PACK_NO_PADDING ifly_VideoBlockParam_t;


//Ԥ�õ����
typedef struct
{
	u8		chn;						//ͨ��
	u8		presetpoint;				//Ԥ�õ��
	u8		option;						//���� 0��� 1ɾ�� 2 ��Ԥ�õ�
}PACK_NO_PADDING ifly_PtzPresetCtr_t;

//Ѳ��·������
typedef struct
{
	u8		chn;						//ͨ��
	u8		cruise_path;				//Ѳ��·��
	struct Cruise_point_t
	{
		u8  preset;						//Ԥ�õ�
		u8  weeltime;					//ͣ��ʱ��
		u8  rate;						//�ٶ�
		u8  flag_add;					//0ɾ��  1���
	}PACK_NO_PADDING Cruise_point[16];
}PACK_NO_PADDING ifly_PtzCruisePathParam_t;

//Ѳ��·������
typedef struct
{
	u8		chn;						//ͨ��
	u8		cruisepath;					//Ѳ��·����
	u8		flagoption;					//���� 0ֹͣ 1��ʼ
}PACK_NO_PADDING ifly_CruisePathCtr_t;

//�켣����
typedef struct
{
	u8		chn;						//ͨ��
	u8		flagoption;					//���� 0��¼ 1ֹͣ��¼  2��ʼ�켣 3ֹͣ�켣
}PACK_NO_PADDING ifly_TrackCtr_t;

//Ӳ�̹���
typedef struct
{
	u8		hdd_index;					//Ӳ�����
	u8		hdd_exist;  				//1 exist; 0 not exist
	u32		capability;					//MB
	u32		freesize;					//MB
	u8		reserved[2];				//Ԥ��
}PACK_NO_PADDING  ifly_hddInfo_t;

//�û���Ϣ
typedef struct
{
	char	name[12];					//�û���
	char	passwd[12];					//����
	char	macaddr[18];				//�����ַ
	u8		rcamer;						//Զ����̨Ȩ��
	u8		rrec;						//Զ��¼��Ȩ��
	u8		rplay;						//Զ�̻ط�Ȩ��
	u8		rsetpara;					//Զ�����ò���
	u8		rlog;						//Զ����־Ȩ��
	u8		rtool;						//Զ�̹���Ȩ��
	u8		rpreview;					//Զ��Ԥ��Ȩ��
	u8		ralarm;						//Զ�̱���Ȩ��
	u8		rvoip;						//�����Խ�Ȩ��
	u8		lcamer;						//������̨Ȩ��
	u8		lrec;						//����¼��Ȩ��
	u8		lplay;						//���ػط�Ȩ��
	u8		lsetpara;					//�������ò���
	u8		llog;						//������־Ȩ��
	u8		ltool;						//���ع���Ȩ��
}PACK_NO_PADDING ifly_user_t;

typedef struct
{
	ifly_user_t	userNum[8];		//���8 ���û�
}PACK_NO_PADDING ifly_userNumber_t;

typedef struct
{
	ifly_user_t	userInfo;		//�û���Ϣ
	u8		flagOption;					//0����û� 1�༭�û� 2ɾ���û�
}PACK_NO_PADDING ifly_userMgr_t;

//ϵͳʱ��
typedef struct
{
	u32		systemtime;					//ϵͳʱ��
	u8		format;						//ʱ���ʽ ѡ��ֵ
	u8		flag_time;					//Ԥ��ʱ��λ����ʾ
	u16		timepos_x;					//Ԥ��ʱ��x����
	u16		timepos_y;					//Ԥ��ʱ��y����
}PACK_NO_PADDING ifly_sysTime_t;

//ϵͳ��Ϣ
typedef struct
{
	char	devicename[32];				//�豸��
	char	devicemodel[32];			//�豸�ͺ�
	char	deviceser[32];				//�豸���к�
	char	version[64];				//����汾
}PACK_NO_PADDING ifly_sysinfo_t;

//��̨����
//��������:
//0-ֹͣ 1�� 2�� 3 �� 4�� 5��ת
//6�䱶�� 7�䱶�� 8����� 9����� 10��Ȧ��
//11��Ȧ�� 12�ƹ⿪ 13�ƹ�� 14��ˢ�� 15��ˢ��
//16 ���� 17���� 18����
typedef struct
{
	u8  chn;							//ͨ��
	u8  cmd;
	u8	reserved;
}PACK_NO_PADDING ifly_PtzCtrl_t;

//�ֶ�¼��
typedef struct
{
	u32  chnRecState;					//ͨ���ֶ�¼��״̬ ��λ
}PACK_NO_PADDING ifly_ManualRecord_t;

//��־����
typedef struct
{
	u8		query_mode;					//��ѯ��ʽ
	u8		main_type;					//������
	u8		slave_type;					//������
	u16		max_return;					//��󷵻���
	u16		startID;					//���شӵ�һ����¼��ʼ
	u32		start_time;					//��ʼʱ��
	u32		end_time;					//����ʱ��
	u8      reserved[16];				//Ԥ��
}PACK_NO_PADDING ifly_search_log_t;

typedef struct
{
	u32		startTime;
	u8		main_type;					//������
	u8		slave_type;					//������
	char	loginfo[32];				//������Ϣ
}PACK_NO_PADDING ifly_logInfo_t;

typedef struct
{
	u16		sum;						//�ܼ�¼��
	u16		startID;					//��ʼ�ļ�¼,��ַΪ1.���޷�������ʱ,startID��ֵΪ0
	u16		endID;						//�����ļ�¼,��ַΪ1.��endID��startID���ʱ,��ʾ֮����һ����¼
}PACK_NO_PADDING ifly_search_desc_t;


//�������֪ͨ
typedef struct
{
	u32	 id;							//�ط�id
	u32  currPos;						//��ǰ����
	u32  totallen;						//��ʱ��
}PACK_NO_PADDING ifly_progress_t;

//�ط��ļ��Ƿ�����Ƶ
typedef struct
{
	u8 bHasAudio;					 	// 1����Ƶ��0����Ƶ
}PACK_NO_PADDING ifly_play_hasaudio_t;

//�첽������Ϣ
typedef struct
{
	u32	 time;							//��������ʱ��
	u32  alarmInfo;						//����ֵ
	u32  reserved;						//Ԥ��
}PACK_NO_PADDING ifly_alarminfo_t;

typedef struct
{
	CPHandle g_cph;
	u8 g_state;
}PACK_NO_PADDING ifly_AlarmUploadCenter_t;

//λ���б�											//dong	100424
typedef struct
{
 u8 chn;    //ͨ��
 u8 videotype;   //0 ������, 1 ������
 u32 bitratelist[16]; //λ���б�, ��λkb, δʹ�õ���0
 u8 reserved[16];  //Ԥ��
}PACK_NO_PADDING ifly_bitRateList_t;

typedef struct
{
	u16 errcode;
	u8 pos;
}PACK_NO_PADDING ifly_TCP_Pos;
// lwj add end

//������������ṹ
typedef struct
{
	u8 command;							//0��Ԥ�� 1���ļ��ط� 2��ʱ��ط� 3���ļ����� 4������ 5 VOIP 6 �ļ���֡����
										// 7 Ԥ����ʱ�䰴֡���أ� 8 ͸��ͨ��
	union
	{
		struct
		{
			u8		chn;				//Ԥ��ͨ��
			u8		type;				//0:��������Ƶ 1����������Ƶ 2:   ��������Ƶ
		}Monitor_t;						//Ԥ�� command = 0

		struct
		{
			char	filename[64];		//�طŵ��ļ�
			u32		offset;				//�ļ�ƫ��
		}FilePlayBack_t; 				//�ļ��ط� command = 1

		struct
		{
			u8		channel;			//ͨ����
			u16		type;				//����
			u32		start_time;			//��ʼʱ��
			u32		end_time;			//��ֹʱ��
		}TimePlayBack_t ;				//ʱ��ط� command = 2

		struct
		{
			char	filename[64];		//���ص��ļ�
			u32		offset;				//�ļ�ƫ��
			u32		size;				//�ļ���С
		}FileDownLoad_t;				//�ļ����� command = 3

		struct
		{
			u32		size;				//�ļ�����
			u32		verify;				//У���
			u16		version;			//�ļ��汾��
			u16		updateType;			//0:�������� 1���������
		}Update_t;						//Զ������  command = 4��

		//VOIP command
		u8 voipreserved;				//VOIPԤ��

		//͸��ͨ��
		u8 SerialPort;					//͸��ͨ�� 1-232����  2-485����
	};
	u8	reserved[7];					//Ԥ��
}PACK_NO_PADDING ifly_TCP_Stream_Req;

//�������������˻�Ӧ�ṹ
typedef struct
{
	u32	errcode;						//���ӳɹ�����0�� ���򷵻�����������
	u32	ackid;							//�����ļ��طţ�ʱ��طš� ���ں����ͻ��˿��ƻطŲ��������ֽ���, Ԥ������
}PACK_NO_PADDING ifly_TCP_Stream_Ack;

typedef enum									//��������ö��
{
	PLAYRATE_1 = 1,								//��������
		PLAYRATE_2 = 2,								//2x���
		PLAYRATE_4 = 4,								//4x���
		PLAYRATE_MAX = 8,							//8x���
		PLAYRATE_1_2 = -2,							//1/2x����
		PLAYRATE_1_4 = -4,							//1/4x����
		PLAYRATE_MIN = -8,							//1/8x����
		PLAYRATE_SINGLE = 0,						//֡��
		PLAYRATE_BACK_1 = -1						//1x�˷�
}re_em_play_rate;

typedef struct
{
	SOCKHANDLE	sockfd;
	u8	bStart;
	u32 linkid;
	ifly_TCP_Stream_Req req;
	union
	{
		struct
		{
			u32 frameid;
		}PACK_NO_PADDING Monitor_t;

		struct
		{
			CPHandle cph;
			u8   byFlag;//0:���ļ�����;1:��ʱ��ط�
			u8   byUse;
			u8   byStop;
			u8   byPause;
			u8   byMute;
			u8   byStatus;
			u8   bySkip;
			re_em_play_rate rate;
			u32  curPos;
			u32  totalTime;
			u32  refTime;
			u32  seekPos;
		}PACK_NO_PADDING PlayBack_t;

		struct
		{
			u8   byUse;
			u8   byStop;
		}PACK_NO_PADDING DonwLoad_t;

		struct
		{
			CPHandle cph;
		}PACK_NO_PADDING Update_t;
	};

}PACK_NO_PADDING ifly_stearmsnd_t;

//Э��ͷ�ṹ
typedef struct
{
	u8	safeguid[16];					// PROTOGUID
	u8  protocolver;					//Э��汾
	u8	byConnType;						//�������ͣ�0x1���������0x2���������䣻0x3���㲥����
	u8	reserved[2];					//�����ֶ�
}PACK_NO_PADDING ifly_ProtocolHead_t;

typedef struct
{
	u16		channel_mask;				//ͨ�� ��λ���
	u16		type_mask;					//���� ��λ���
	u32		start_time;					//��ʼʱ��
	u32		end_time;					//��ֹʱ��
	u16		startID;					//���صĵ�һ����¼,��1��ʼ
	u16		max_return;					//ÿ�η��ص�����¼��
	u8		reserved[4];				//Ԥ��
}PACK_NO_PADDING ifly_recsearch_param_wifi_t;

typedef struct
{
	u8 chn;							//ͨ��
	u8 type;						//0-�������� 1-������
	u8 spsbuf[20];					//sps buf
	u8 spslen;						//sps����
	u8 ppsbuf[20];					//pps buf
	u8 ppslen;						//pps����
}PACK_NO_PADDING ifly_spspps_t;

typedef struct
{
	u8 		flag_pingtai;
	u32 	ip_pingtai;
	u16 	port_pingtai;
	u16 	port_download;
	char	device_name[32];
	char	device_passwd[32];
	u8		flag_verifytime;
}PACK_NO_PADDING ifly_pingtai_xingwang_t;
//͸�����ڷ�������ͷ
typedef struct
{
	u8	byChannel;		//����ͨ����ʹ��485����ʱ��Ч����1��ʼ��232������Ϊ͸��ͨ��ʱ��ֵ����Ϊ0
	u32 dwDataSize;		//���͵����ݳ���
	u8	reserved[16];	//Ԥ��
}PACK_NO_PADDING ifly_serial_hdr;
//Ԥ�����طź� VOIP֡�ṹ
typedef struct
{
	u8	m_byMediaType;					//����
	u8	m_byFrameRate;					//֡��
	u8	m_bKeyFrame;					//�Ƿ�ؼ�֡
	int m_nVideoWidth;					//��
	int m_nVideoHeight;					//��
	u32 m_dwDataSize;					//���ݴ�С
	u32 m_dwFrameID;					//֡ID
	u32 m_dwTimeStamp;					//ʱ���
}PACK_NO_PADDING ifly_MediaFRAMEHDR_t;

typedef struct
{
	u32 nrserverip; //��������ַ
	u16 serverport; //�������˿�
	u16 databindport; //���ݰ󶨶˿�
	u8 reserved[16]; //Ԥ��
}PACK_NO_PADDING ifly_nrserver_t; //���������


//����
typedef struct
{
	u8 alarmid; //alarmid 0��ʼ
	u8  val; //ȡֵ 0δ���� 1����
	u8 reserved[2]; //Ԥ��
}PACK_NO_PADDING ifly_alarm_val_t;

typedef struct
{
	//0-�ź�������,1-Ӳ����,2-�źŶ�ʧ,3���ƶ����,4��Ӳ��δ��ʽ��,
	//5-��дӲ�̳���,6-�ڵ�����,7-��ʽ��ƥ��, 8-�Ƿ�����
	u8 type;
	u8 state;       //1���� 2�ָ�
	u8 id;			//ͨ��,Ӳ�̺�,���������,ȡ����type
	u16 reserved1;  //Ԥ��
	u32  reserved2; //Ԥ��
}PACK_NO_PADDING ifly_alarmstate_t;


#pragma pack( pop )
typedef void (*pFindDeivce_CB)(ifly_DeviceInfo_t deviceinfo, void* pContext);   // lihg add 20110411
typedef u16 (*pMESSAGE_CB)(CPHandle cph,u16 event,u8 *pbyMsgBuf,int msgLen,u8 *pbyAckBuf,int *pAckLen,void* pContext);
typedef int (*pAddStreamTCPLink)(SOCKHANDLE hSock, ifly_TCP_Stream_Req req, void* pContext);



#ifdef __cplusplus
}
#endif

#endif
