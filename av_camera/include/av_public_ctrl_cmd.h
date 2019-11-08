/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : av_public_ctrl_cmd.h
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2012��7��5��������
  ����޸�   :
  ��������   : av_public_ctrl_cmd.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��11��26��������
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __AV_PUBLIC_CTRL_CMD_H__
#define __AV_PUBLIC_CTRL_CMD_H__

//��������
#define CTRL_CMD_BASE					10000
#define CTRL_CMD_LOGIN					CTRL_CMD_BASE+1			//Զ�̵�¼����
#define CTRL_CMD_LOGOFF					CTRL_CMD_BASE+2			//ע���û���¼
#define CTRL_CMD_GETDEVICEINFO			CTRL_CMD_BASE+3			//����豸��Ϣ
#define CTRL_CMD_GETVIDEOPROPERTY		CTRL_CMD_BASE+4			//��Ƶ������Ϣ
#define CTRL_CMD_GETAUDIOPROPERTY		CTRL_CMD_BASE+5			//��Ƶ������Ϣ
#define CTRL_CMD_GETVOIPPROPERTY		CTRL_CMD_BASE+6			//voip������Ϣ
#define CTRL_CMD_STARTVIDEOMONITOR		CTRL_CMD_BASE+7			//������ƵԤ��
#define CTRL_CMD_STOPVIDEOMONITOR		CTRL_CMD_BASE+8			//ֹͣ��ƵԤ��
#define CTRL_CMD_STOPAUDIOMONITOR		CTRL_CMD_BASE+9			//ֹͣ��ƵԤ��
#define CTRL_CMD_STOPVOIP				CTRL_CMD_BASE+10		//ֹͣvoip
//#define CTRL_CMD_STOPDOWNLOAD			CTRL_CMD_BASE+11		//ֹͣ����
#define CTRL_CMD_STOPFILEPLAY			CTRL_CMD_BASE+12		//ֹͣ�ļ��ط�
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
#define CTRL_CMD_FORMAT_SATA_DISK 		CTRL_CMD_BASE+60		//��ʽ������
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
#define CTRL_CMD_LOCAL_REC_PLAY	            CTRL_CMD_BASE+74		//���ػط�
#define CTRL_CMD_GETSPSPPS				CTRL_CMD_BASE+75		//���spspps����
#define CTRL_CMD_ALARMUPLOADCENTER		CTRL_CMD_BASE+76		//�����ϴ�����
#define CTRL_CMD_CDROM					CTRL_CMD_BASE+77		//��������

#define CTRL_CMD_COM_PROTOCOL_GET		CTRL_CMD_BASE+78		//��ô��ڲ���
#define CTRL_CMD_COM_PROTOCOL_SET		CTRL_CMD_BASE+79		//���ô��ڲ���
#define CTRL_CMD_SERIALSTOP				CTRL_CMD_BASE+80		//ֹͣ͸������

#define CTRL_CMD_XWSERVER_GET			CTRL_CMD_BASE+85		//csp modify 20100419
#define CTRL_CMD_XWSERVER_SET			CTRL_CMD_BASE+86		//csp modify 20100419

#define CTRL_CMD_GETRECFILEINDEX		CTRL_CMD_BASE+87		//csp modify 20100419
// lihg add 20110808
#define  CTRL_CMD_3GSERVER 				CTRL_CMD_BASE+90 		//����3G������
// lihg add end
#define CTRL_CMD_SETMAC					CTRL_CMD_BASE+92		//����MAC��ַ


#define CTRL_CMD_GET_ALARMIN_VAL		CTRL_CMD_BASE+103		//��ȡ��������ֵ
#define CTRL_CMD_SET_ALARMOUT_VAL		CTRL_CMD_BASE+104		//���ñ������ֵ

#define CTRL_CMD_STREAM_REQ       		CTRL_CMD_BASE+105

#define CTRL_CMD_AUTO_BITRATE   		CTRL_CMD_BASE+107        // ��̬����

#define CTRL_CMD_PLAYMP3FILE_START      CTRL_CMD_BASE+108        	// ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_STOP       CTRL_CMD_BASE+109        	// ֹͣ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_PAUSE      CTRL_CMD_BASE+110       	// ����MP3�ļ�
#define CTRL_CMD_PLAYMP3FILE_SETVOLUME  CTRL_CMD_BASE+111       	// ��������
#define CTRL_CMD_PLAYMP3FILE_GETSTATUS  CTRL_CMD_BASE+112       	// ��ȡMP3����״̬
#define CTRL_CMD_JPEGCAPTURE			CTRL_CMD_BASE+113       	// JPEGץͼ
#define CTRL_CMD_GETDEVICESTATUS		CTRL_CMD_BASE+114       	// ��ȡ�豸״̬

//add pfli 20120213
#define CTRL_CMD_GETBUSINFO				CTRL_CMD_BASE+115       	// ��ȡ������Ϣ
#define CTRL_CMD_SETBUSINFO				CTRL_CMD_BASE+116       	// ���ó�����Ϣ
// lipf add 20111125
#define CTRL_CMD_UPDATE					CTRL_CMD_BASE+117		//����
#define CTRL_CMD_GET_UPDATE_POS			CTRL_CMD_BASE+118		//�����������
#define CTRL_CMD_GET_UPDATE_STATE		CTRL_CMD_BASE+119		//�������״̬
// lipf add end
//#define CTRL_CMD_STOPVOIP				CTRL_CMD_BASE+200		//ֹͣvoip
//#define CTRL_CMD_STARTVOIP				CTRL_CMD_BASE+201		//��ʼvoip
#define CTRL_CMD_STARTVOIP				CTRL_CMD_BASE+11		//��ʼvoip

#define CTRL_CMD_PLAYMP3FILE_GETVOLUME  CTRL_CMD_BASE+150       	// ��ȡ����ֵ
#define CTRL_CMD_SETOSD_BUSNO  			CTRL_CMD_BASE+151       	// �����豸��osd
#define CTRL_CMD_GETOSD_BUSNO  			CTRL_CMD_BASE+152       	// ��ȡ�豸��osd
#define CTRL_CMD_SHOWOSD_BUSNO  		CTRL_CMD_BASE+153       	// ��ʾ�������豸��osd
#define CTRL_CMD_SETOSD_EXTERN  		CTRL_CMD_BASE+154       	// ������չosd
#define CTRL_CMD_GETOSD_EXTERN   		CTRL_CMD_BASE+155       	// ��ȡ��չosd
#define CTRL_CMD_SHOWOSD_EXTERN			CTRL_CMD_BASE+156      		// ��ʾ��������չosd
//pfli add end

// lihg add 20120321
#define CTRL_CMD_AOUT_PLAYBACK_ON  		CTRL_CMD_BASE+157       	// �򿪻ط�ͨ��
#define CTRL_CMD_AOUT_PLAYBACK_OFF  	CTRL_CMD_BASE+158       	// �رջط�ͨ��
// lihg add end

/* BEGIN: Added by  , 2013/6/3   PN:������Ƶ���.89001012 */
#define CTRL_CMD_VOUT_GETDEV_CSC  		CTRL_CMD_BASE+159       	// ��ȡ��Ƶ���Ч��
#define CTRL_CMD_VOUT_SETDEV_CSC      	CTRL_CMD_BASE+160       	// ������Ƶ���Ч��
/* END:   Added by  , 2013/6/3   PN:������Ƶ���.89001012 */
#define CTRL_CMD_UMOUNT_DISK  		    CTRL_CMD_BASE+161       	// ж��Ӳ��
//#define CTRL_CMD_VOUT_SETDEV_CSC      	CTRL_CMD_BASE+160       	// ������Ƶ���Ч��
#define CTRL_CMD_GET_DISK_STATUE        CTRL_CMD_BASE+162           /* ��ȡӲ�̹���״̬ */
#define CTRL_CMD_SET_REC_CHN         CTRL_CMD_BASE+163           /* ����¼��ͨ�� */
#define CTRL_CMD_GET_REC_CHN         CTRL_CMD_BASE+164           /* ��ȡ¼��ͨ��״̬ */
/* BEGIN: Added by  , 2013/12/30   PN:��ӹ����ϴ�����.1027001014 */
#define CTRL_CMD_SET_MP3FILE_STATUE     CTRL_CMD_BASE+165           /* ����MP3�ļ�״̬ */
#define CTRL_CMD_SET_ERROR_STATUE       CTRL_CMD_BASE+166           /* �豸����״̬ */
/* END:   Added by  , 2013/12/30   PN:��ӹ����ϴ�����.1027001014 */

/* BEGIN: Added by xiaomengmeng, 2014/1/15  DES:���U�̵�¼��  PN:4431 */
#define CTRL_CMD_RECORD_EXPORT_START     CTRL_CMD_BASE+167           /* U�̵�¼���������� */
#define CTRL_CMD_RECORD_EXPORT_CANCLE    CTRL_CMD_BASE+168           /* U�̵�¼��ȡ������ */
#define CTRL_CMD_RECORD_EXPORT_STATE     CTRL_CMD_BASE+169           /* ��¼�����״̬ */
/* END:   Added by xiaomengmeng, 2014/1/15   PN:4431 */
#define CTRL_CMD_REC_SEARCH           CTRL_CMD_BASE + 170  /* ¼������ */
#define CTRL_CMD_START_REC_PLAY       CTRL_CMD_BASE + 171  /* ¼��ط� */
#define CTRL_CMD_STOP_REC_PLAY        CTRL_CMD_BASE + 172  /* ֹͣ¼��ط� */
#define CTRL_CMD_FOUR_CHAN_PLAY       CTRL_CMD_BASE + 178  /*4ͨ���ط� */
/* BEGIN: Added by  , 2014/10/11  DES:��ӵ�����Ƶ�������  PN:14100801008 */
#define CTRL_CMD_SET_VIDEO_INPUT      CTRL_CMD_BASE + 174  /* ������Ƶ������� */
#define CTRL_CMD_GET_VIDEO_INPUT      CTRL_CMD_BASE + 173  /* ��ȡ��Ƶ������� */
/* END:   Added by  , 2014/10/11   PN:14100801008 */
/* BEGIN: Added by  , 2014/10/24  DES:��ӻ�ȡ��Ƶ���Ĭ�ϲ�������  PN:14102401010 */
#define CTRL_CMD_GET_VIDEO_OUTPUT_ATUO      CTRL_CMD_BASE + 174  /* ��ȡ��Ƶ�������Ĭ��ֵ */
/* END:   Added by  , 2014/10/24   PN:14102401010 */
// lihg add 20110729
#define CTRL_CMD_BASE_EX 				10120
#define CTRL_CMD_GETPARAM_EX			CTRL_CMD_BASE_EX+1
#define CTRL_CMD_SETPARAM_EX			CTRL_CMD_BASE_EX+2
// lihg add end
#define CTRL_CMD_PREVIEWSWITCH_EX		CTRL_CMD_BASE_EX+3          //��������4����Ԥ���л�//add pfli 20120213
#define CTRL_CMD_SDRESET_EX				CTRL_CMD_BASE_EX+4       //sd������

//Э��
#define CTRL_PROTOCOL_SERVERPORT		6630					//�������˿� 6610
#define CTRL_PROTOCOL_MAXLINKNUM		256
#define CTRL_PROTOCOL_CONNECT_BLOCK		INFINITE				//���ӷ�ʽ:����
#define CTRL_PROTOCOL_CONNECT_DEFAULT	3000					//ȱʡ����ʱ��:3��
#define CTRL_VERSION					0x0100

#define CTRL_COMMAND					0
#define CTRL_NOTIFY						1
#define CTRL_ACK						2

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


#define CTRL_FAILED_UNKNOWN				CTRL_FAILED_BASE+9999	//δ֪����

//add pfli 20120416
#define SERVER_CTRL_NOTIFY_BASE                   50000
#define SERVER_CTRL_ALARMUPLOADCENTER             SERVER_CTRL_NOTIFY_BASE+6 // �����ϴ�
//pfli  add end

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

/* Appģ���Ӧ�������� */
#define CTRL_BACK_CMD_BASE					15000
/* ��ӦMP3��ʼ���������� */
#define CTRL_BACK_CMD_PLAYMP3FILE_START      CTRL_BACK_CMD_BASE+108
/* ��ӦMP3ֹͣ���������� */
#define CTRL_BACK_CMD_PLAYMP3FILE_STOP       CTRL_BACK_CMD_BASE+109
/* ��ӦMP3��ͣ������ */
#define CTRL_BACK_CMD_PLAYMP3FILE_PAUSE      CTRL_BACK_CMD_BASE+110       	// ����MP3�ļ�
/* ��ӦMP3������������������ */
#define CTRL_BACK_CMD_PLAYMP3FILE_SETVOLUME  CTRL_BACK_CMD_BASE+111       	// ��������
/* ��ӦMP3���Ż�ȡ����ֵ������ */
#define CTRL_BACK_CMD_PLAYMP3FILE_GETSTATUS  CTRL_BACK_CMD_BASE+112       	// ��ȡMP3����״̬
/* ��Ӧ��ȡ����ֵ������ */
#define CTRL_BACK_CMD_PLAYMP3FILE_GETVOLUME  CTRL_BACK_CMD_BASE+150       	// ��ȡ����ֵ
/* �򿪻ط�ͨ�� */
#define CTRL_BACK_CMD_AOUT_PLAYBACK_ON       CTRL_BACK_CMD_BASE+157       	// �򿪻ط�ͨ��
/* �رջط�ͨ�� */
#define CTRL_BACK_CMD_AOUT_PLAYBACK_OFF  	 CTRL_BACK_CMD_BASE+158       	// �رջط�ͨ��
/* ֹͣIP�Խ� */
#define CTRL_BACK_CMD_STOPVOIP				CTRL_BACK_CMD_BASE+200
/* ����IP�Խ� */
#define CTRL_BACK_CMD_STARTVOIP				CTRL_BACK_CMD_BASE+201
/* ��ȡ�ֶ�¼��״̬ */
#define CTRL_BACK_CMD_GETMANUALREC			CTRL_BACK_CMD_BASE+71
/* �����ֶ�¼��״̬ */
#define CTRL_BACK_CMD_SETMANUALREC			CTRL_BACK_CMD_BASE+72
/* �����л� */
#define CTRL_BACK_CMD_PREVIEWSWITCH_EX      CTRL_BACK_CMD_BASE+123
/* ��Ƶץͼ */
#define CTRL_BACK_CMD_JPEGCAPTURE           CTRL_BACK_CMD_BASE+113       	// JPEGץͼ
/* ��ȡ��Ƶ���� */
#define CTRL_BACK_CMD_GETAUDIOPROPERTY      CTRL_BACK_CMD_BASE+5			//��Ƶ������Ϣ
/* ��ȡϵͳ���� */
#define CTRL_BACK_CMD_GETSYSINFO			CTRL_BACK_CMD_BASE+67		//���ϵͳ��Ϣ
/* ������ƵԤ�� */
#define CTRL_BACK_CMD_STARTVIDEOMONITOR		CTRL_BACK_CMD_BASE+7			//������ƵԤ��
/* ֹͣ��ƵԤ�� */
#define CTRL_BACK_CMD_STOPVIDEOMONITOR		CTRL_BACK_CMD_BASE+8			//ֹͣ��ƵԤ��
/* ��ȡ������Ϣ */
#define CTRL_BACK_CMD_GETBUSINFO		    CTRL_BACK_CMD_BASE+115       	// ��ȡ������Ϣ
/* ���ó�����Ϣ */
#define CTRL_BACK_CMD_SETBUSINFO			CTRL_BACK_CMD_BASE+116       	// ���ó�����Ϣ
/* ��ȡ��Ƶ���� */
#define CTRL_BACK_CMD_GETVIDEOPROPERTY		CTRL_BACK_CMD_BASE+4			//��Ƶ������Ϣ
/* �����豸��OSD */
#define CTRL_BACK_CMD_SETOSD_BUSNO  		CTRL_BACK_CMD_BASE+151       	// �����豸��osd
/* ��ȡ�豸��OSD */
#define CTRL_BACK_CMD_GETOSD_BUSNO  		CTRL_BACK_CMD_BASE+152       	// ��ȡ�豸��osd
/* ��ʾ�豸��OSD */
#define CTRL_BACK_CMD_SHOWOSD_BUSNO  		CTRL_BACK_CMD_BASE+153       	// ��ʾ�������豸��osd
/* ��չOSD������ʾ */
#define CTRL_BACK_CMD_SETOSD_EXTERN  		CTRL_BACK_CMD_BASE+154       	// ������չosd
/* ��ȡ��չOSD������ʾ */
#define CTRL_BACK_CMD_GETOSD_EXTERN   		CTRL_BACK_CMD_BASE+155       	// ��ȡ��չosd
/* ��ʾ��������չOSD������ʾ */
#define CTRL_BACK_CMD_SHOWOSD_EXTERN		CTRL_BACK_CMD_BASE+156      		// ��ʾ��������չosd
/* ��ȡ��Ƶ������� */
#define CTRL_BACK_CMD_VOUT_GETDEV_CSC  		CTRL_BACK_CMD_BASE+159       	// ��ȡ��Ƶ���Ч��
/* ������Ƶ������� */
#define CTRL_BACK_CMD_VOUT_SETDEV_CSC      	CTRL_BACK_CMD_BASE+160       	// ������Ƶ���Ч��


#endif /* __AV_PUBLIC_CTRL_CMD_H__ */

