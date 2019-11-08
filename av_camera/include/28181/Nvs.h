/********************************************************************
created:	2014/12/08
modify :    2014/12/08
author:
purpose:    NVS��װ�ӿ�,����28181�淶
*********************************************************************/
#ifndef NETVIDEOSYSTEM_H_
#define NETVIDEOSYSTEM_H_


#ifdef WIN32
#include <stddef.h>
#include <windef.h>
#else
#endif

#include <time.h>

#ifdef WIN32
	#ifdef NET_VIDEO_SDK_EXPORT
		#define NET_VIDEO_API __declspec(dllexport)
	#else
		#define NET_VIDEO_API __declspec(dllimport)
	#endif //!NET_VIDEO_SDK_EXPORTS

	#ifndef NET_VIDEO_CALLBACK
		#define NET_VIDEO_CALLBACK CALLBACK
	#endif

	#ifndef NET_VIDEO_METHOD
		#define NET_VIDEO_METHOD __stdcall
	#endif
#else	//!WIN32
	#define NET_VIDEO_API

	#ifndef NET_VIDEO_CALLBACK
		#define NET_VIDEO_CALLBACK
	#endif

	#ifndef NET_VIDEO_METHOD
		#define NET_VIDEO_METHOD
	#endif
#endif	//WIN32

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif


namespace nvs
{

////������������
typedef unsigned char            uint8_t;
typedef unsigned short           uint16_t;
typedef long long                int64_t;
typedef unsigned long long       uint64_t;


///����
enum Constants
{
	MAX_IPV4_LEN = 16,
    MAX_ID       = 32,
    MAX_NAME     = 128,
    MAX_DESC     = 256,
    MAX_SPROP    = 512
};



///����jitterֵ��������,Ӧ�ò����QOS��������QOS���ߣ���̬������Ƶ�ֱ��ʡ����ʡ�֡�ʡ�qp��ֵ
enum QOSJitter
{
	QOS_LEVEL_NORMAL = 0,     ///��������Ŀǰ�����ʿ����������ͣ�����ı�
	QOS_LEVEL_BETTER = 1,     ///�Ϻã������ʵ�������Ƶ�������ṩ�Ϻ÷���
	QOS_LEVEL_BAD    = 2      ///�ϲ����ӵ���������ʵ��ϵ���Ƶ����������������
};

//�������Ͷ���
enum NvsErrorCode
{
	NVS_ERR_FAIL              = -1,      //ʧ��
	NVS_ERR_OK                = 0,       //�ɹ�
	NVS_ERR_NO_INIT           = -100,   //δ��ʼ��
	NVS_ERR_AUTH_FAIL         = -102,   //��Ȩʧ��
	NVS_ERR_OUT_OF_SERVER     = -103,   //�����ṩ����
	NVS_ERR_CONNT_AGU_FAIL    = -104,   //���ӽ������ط�����ʧ��
	NVS_ERR_SOCKET_ERR        = -105,   //SOCKET����
	NVS_ERR_RECV_TIMEOUT      = -106,   //������ճ�ʱ
	NVS_ERR_MEM_ALLOC_FAIL    = -107,   //�����ڴ����
	NVS_ERR_REPORT_RES_FAIL   = -108,   //�ϱ���Դʧ��
	NVS_ERR_INVALID_RES       = -109,   //��Ч����Դ
	NVS_ERR_SEND_RTP_FAIL     = -110,   //������ý��ʧ��
	NVS_ERR_INVAID_CHANNEL    = -111,   //�����ͨ����
	NVS_ERR_RES_NO_EXIST      = -112,   //�������Դ������
	NVS_ERR_RES_NO_ONLINE     = -114,   //�������Դ������
	NVS_ERR_R_OVER_MAXLINK    = -115    //��Դ�������������

};

///�ƾ����ƶ���
enum PTZCommand
{
	PTZ_CMD_START   = 0,   ///������
	PTZ_CMD_STOP    = 1,   ///ֹͣ

	PTZ_CMD_RIGHT   = 2,   ///��
	PTZ_CMD_LEFT    = 4,   ///��
	PTZ_CMD_DOWN    = 6,   ///��
	PTZ_CMD_UP      = 8,   ///��

	PTZ_CMD_SMALL   = 10,    ///��Ȧ��С
	PTZ_CMD_LARGE   = 12,    ///��Ȧ���

	PTZ_CMD_ZOOM_IN  = 14,   ///������
	PTZ_CMD_ZOOM_OUT = 16,   ///�����С

	PTZ_CMD_FOCUS_IN  = 18,  ///����ǰ��
	PTZ_CMD_FOCUS_OUT = 20,  ///������

	PTZ_CMD_ASSIST_FUNC_1 = 30,	 ///��ˢ
	PTZ_CMD_ASSIST_FUNC_2 = 32,	 ///��������2
	PTZ_CMD_ASSIST_FUNC_3 = 34,	 ///��������3
	PTZ_CMD_ASSIST_FUNC_4 = 36,	 ///��������4

	PTZ_RIGHT_APPLY   = 40,    ///Ȩ������
	PTZ_RIGHT_RELEASE = 42,    ///Ȩ���ͷ�

	PTZ_PRESET_ADD    = 110,   ///���Ԥ��λ
	PTZ_PRESET_MODIDY = 112,   ///�޸�Ԥ��λ
	PTZ_PRESET_DELETE = 114,   ///ɾ��Ԥ��λ
	PTZ_PRESET_GOTO   = 116,   ///ת��Ԥ��λ

	PTZ_CMD_SCAN = 120,        ///�Զ�ɨ�迪
	PTZ_CMD_LENGTH
};


///Ԥ��λ����
enum PresetFlag
{
	PRESET_FLAG_NORM  = 0,   ///��ͨԤ��λ
	PRESET_FLAG_DAY   = 1,   ///��������λ
	PRESET_FLAG_NIGHT = 2    ///ҹ������λ
};




///�澯����������
enum AlarmType
{
	ALARM_INTRUSION = 1,/// ����
	ALARM_LOITER,       /// �ǻ�
	ALARM_MOVING,       /// �ƶ����
	ALARM_STAY,         /// ����
	ALARM_LEFT_TAKE,    /// ��������
	ALARM_LINE,         /// ��Խ����
	ALARM_DISK_FULL,    /// Ӳ����
	ALARM_DISK_FAULT,   /// Ӳ�̹���
	ALARM_SERVER_OFF,   /// �������Ͽ�
	ALARM_VIDEO_LOST,   /// ��Ƶ��ʧ
	ALARM_ENCODER_OFF,  /// �������Ͽ�
	ALARM_INPUT,        /// �澯����澯
	ALARM_USER_PLAY_ERROR     /// �û�ʵʱԤ���쳣
};


///�澯��״̬֪ͨ��״̬
enum StatusType
{
	STATUS_BEGIN = 0,   ///״̬��ʼ
	STATUS_END = 1      ///״̬����
};

///ý������֡����
enum FrameType
{
	FRAME_PCM      = 0x00,
	FRAME_G711A    = 0x08,
	FRAME_OPUS     = 0x09,

	FRAME_VIDEO_I  = 0x10,
	FRAME_VIDEO_P  = 0x11
};


///ý��ѹ������
enum CompressionCodec
{
	G711A_CODEC = 0x08,

	STANDARD_H264_CODEC  = 98,
	STANDARD_MPEG4_CODEC = 97
};


struct SipAddrInfo
{
	char     id[MAX_ID];       //sip��ַ����ID��
	char     ip[MAX_IPV4_LEN]; //sip������IP��ַ
	uint16_t port;             //sip�����Ķ˿�
};

///ƽ̨ע����Ϣ
struct PlatformRegInfo
{
	SipAddrInfo  nvsAddr;      //nvs�Լ���sip��Ϣ
	SipAddrInfo  serverAddr;   //Ҫע��ķ�������sip��Ϣ

	char  userName[MAX_NAME];  //�û���
	char  password[MAX_NAME];  //����

	uint16_t expires;         //sipˢ��ע���ʱ����
};


///�ϱ���Դ��Ϣ
struct ResNode
{
    char    id[MAX_ID];        /// ͳһ��Դ���루����ͷ��Ŀ¼�ṹ��
    char    name[MAX_NAME];    /// ��Դ����
    char    parentID[MAX_ID];  /// ��Դ���ڵ��ͳһ��Դ����
    int     subNum;            /// �ӽڵ���Ŀ
    int     codec;             /// �����ǩ
    int     status;            /// ״̬��0�����ã�1����
	int     resType;           /// ��Դ���ͣ�0��Ŀ¼��1������
	char    address[MAX_NAME]; /// ��Դ���ڵ��ͳһ��Դ����
	int     hasGuard;
};


///Ԥ��λ��Ϣ
struct Preset
{
    int  num;            ///Ԥ��λ���
    char name[MAX_NAME]; ///Ԥ��λ����
    int  flag;           ///Ԥ��λ����
};


///¼����Ϣ
struct RecordItem
{
	char   id[MAX_ID];         ///��Դ����
	time_t beginTime;          ///ʵ�ʿ�ʼʱ��
	time_t endTime;            ///ʵ�ʽ���ʱ��
	int    secrecy;            ///0�����ܣ�1����
	char   type[MAX_ID];       ///¼������,time�ճ�  alarm�澯  manual�ֶ�  allȫ��
	char   recordID[MAX_NAME]; ///¼�񴥷���ID
};

///��ѯ¼����Ϣ����
struct QueryRecordRequest
{
	char   id[MAX_ID];    ///��Դ����
	time_t beginTime;     ///��ʼʱ��
	time_t endTime;       ///����ʱ��
	int    secrecy;       ///0�����ܣ�1����
	char   type[MAX_ID];  ///¼������,time�ճ�  alarm�澯  manual�ֶ�  allȫ��
	char   recordID[MAX_NAME]; ///¼�񴥷���ID
};


///��ѯ¼����Ϣ��Ӧ
struct QueryRecordResponse
{
	int  sum;               ///�����������������pItems���ڴ泤�ȣ�ʵ�ʷ��ص�¼����Ϣ����
	RecordItem* pItems;     ///¼����Ϣ�ṹ
};



///״̬֪ͨ
struct StatusNotification
{
    char    id[MAX_ID];  ///��Դ����
    time_t  time;        ///״̬����ʱ��
    int     status;      ///״̬
};


///�澯֪ͨ
struct AlarmNotification
{
    char    id[MAX_ID];     ///��Դ����
    char    name[MAX_NAME]; ///��Դ����
    int     type;           ///�澯����
    int     priority;       ///�澯����
    time_t  time;           ///�澯ʱ��
	int     status;         ///�澯״̬
    double  longitude;      ///�澯����
    double  latitude;       ///�澯γ��
    char    description[MAX_DESC]; ///�澯����
};





/////////////////////////////////////////////////////////////////////////////
///ý����Ϣ����
struct StreamFormat
{
    double duration;    ///ý����ʱ�����룩����ʷ��Ƶ��Ч

    int codec;          ///�����ǩ����Ӧ�����
    int width;          ///��Ƶ���
    int height;         ///��Ƶ�߶�
    int fps;            ///֡��
    int bitrate;        ///����
    int iFrame;         ///�ؼ�֡���
    int propSize;             ///ý��������Ϣ���ȣ���propʵ�ʳ���
    uint8_t prop[MAX_SPROP];  ///ý��������Ϣ�����纣��40�ֽڽ���ͷ����
    char mediaName[MAX_NAME];       ///ý������

    int audioCodec;         ///��Ƶ����
    int audioChannel;       ///��Ƶͨ��
    int audioSampleRate;    ///������sample rate
    int audioPropSize;      ///��Ƶ������Ϣ���ȣ���audioPropʵ�ʳ���
    uint8_t audioProp[MAX_DESC]; ///��Ƶ������Ϣ

	char  redirectIp[MAX_IPV4_LEN];
	int   redirectPort;
};





///ý�����ݷ��ͽṹ
struct StreamPacket
{
    uint8_t* data;      ///ý������
    int      size;      ///ý�����ݳ���
	int      flag;      ///ý��֡����
    uint64_t timestamp; ///����֡������ʱ�䣬milliseconds
};


///�豸��Ϣ����
struct DeviceInfoParam
{
	char  deviceType[MAX_NAME];
	char  manufacturer[MAX_NAME];
	char  model[MAX_NAME];
	char  firmware[MAX_NAME];
	char  maxCamera[MAX_ID];
	char  maxAlarm[MAX_ID];
};


struct DeviceStatusParam
{
	char  onLine[MAX_ID];
	char  status[MAX_ID];
	char  encode[MAX_ID];
	char  record[MAX_ID];
	char  deviceTime[MAX_NAME];
	char  alarmstatus[MAX_ID];
};

///ý�����ݴ���
class  StreamSink
{
public:
    virtual ~StreamSink() {}

	//�ڵ�һ��д����ǰ��Ҫ����SDKý�����ݵĸ�ʽ������Ҫ����һ�Σ�
    virtual void setFormat(IN const StreamFormat& fmt) =0;

	//������ý����ʱ�����øú�������ý������д��SDK����SDK������
    virtual int writePacket(IN const StreamPacket& packet) =0;

	///֪ͨ����ý�����ݽ���
    virtual void endOfStream() =0;
};




///ý����ƣ�ʵʱ��Ƶֻ��ʵ�ֲ��֣���ʷ��Ƶ����ʵ��
class  MediaStream
{
public:
    virtual ~MediaStream() {}

	///����
    virtual int mplay() =0;

    ///��ͣ
    virtual int mpause() =0;

	///ֹͣ
    virtual int mstop() =0;

	///��֡����֡
	virtual int mstep() =0;

	///��ק,����0-100
    virtual int mseek(IN int offset) =0;

	///���٣�1/4.0 ---- 4.0
    virtual int mscale(IN double fscale) =0;

	///����QOS����
	virtual int mjitter(IN int jitterLevel) =0;

	///�������ص�
    virtual int setSink(IN StreamSink* pSink) =0;
};


///��Ϣ֪ͨ
class  NotificationSink
{
public:
    virtual ~NotificationSink() {}

	/**
     * ע��ɹ�
     */
    virtual int OnRegisterOk(IN struct tm timeNow) =0;

	/**
     * �Ͽ��������������
     */
    virtual int OnDisConnect() =0;

    /**
     * �����ƾ�
	 * @param id    [in] ��Դ����
	 * @param cmd   [in] ����
	 * @param speed [in] �ٶ�
	 * @param data  [in] ��������ʼ��ֹͣ��
	 * @param para  [in] ��������Ϊ�գ�Ԥ��λʱΪ����
     */
    virtual int OnControlPtz(IN const char* id, IN int cmd,
		IN int speed, IN int data,IN const char *para) =0;


	 /**
     * ǿ�ƹؼ�֡
	 * @param id          [in] ��Դ����
	 * @param streamIdx   [in] �������ͣ�0��������1������
     */
	virtual int OnMakeKeyFrame (IN const char *id,IN int streamIdx) =0;


	 /**
     * ��ʵʱ��Ƶ
	 * @param id          [in] ��Դ����
	 * @param streamIdx   [in] �������ͣ�0��������1������
	 * @param ppStream    [in] ����������ָ������ʹ��������ʵ��
     */
    virtual int OnOpenLiveStream(IN const char* id, IN int streamIdx,
		OUT MediaStream** ppStream) =0;


	/**
     * ����ʷ��Ƶ��
	 * @param rec          [in] ��Դ����
	 * @param ppStream     [in] ����������ָ������ʹ��������ʵ��
     */
    virtual int OnOpenRecordStream(IN const RecordItem& rec,
		OUT MediaStream** ppStream) =0;

	/**
     * �ر���Ƶ
	 * @param ppStream    [in] ����������ָ����
     */
    virtual int OnCloseStream(IN MediaStream* pStream) =0;


	/**
     * ����Ƶ�Խ�
	 * @param id           [in]   ��Դ����
	 * @param pSink        [out]  ���ݽ���ͨ��
	 * @param ppStream     [out]  ���ݷ���ͨ����ʹ��������ʵ��
     */
    virtual int OnOpenPhoneStream(IN const char* id, OUT StreamSink** ppSink,
		OUT MediaStream** ppStream) =0;


	/**
     * ��ѯ��ʷ��Ƶ
	 * @param req         [in] ��ѯ����
	 * @param resp        [in] ��Ӧ��Ϣ
     */
    virtual int OnQueryRecord(IN const QueryRecordRequest& req,
		OUT QueryRecordResponse& resp) =0;

	/**
     * �ֶ�¼��
	 * @param id         [in] ��Դ����
	 * @param isStart    [in] 1������0ֹͣ
     */
	virtual int OnRecord(IN const char* id,IN int isStart) =0;


	/**
     * ����
	 * @param id         [in] ��Դ����
	 * @param guardFlag  [in] 0������1����
     */
	virtual int OnSetGuard(IN const char* id, IN int guardFlag) =0;

	/**
     * ������λ
	 * @param id         [in] ��Դ����
	 * @param alarmType  [in] �澯����
     */
	virtual int OnResetAlarm(IN const char* id, IN int alarmType) =0;


	/**
     * �豸��Ϣ��ѯ
	 * @param id         [in]  ��Դ����
	 * @param info       [out] �豸��Ϣ
     */
	virtual int OnGetDeviceInfo(IN const char* id,OUT DeviceInfoParam &info) =0;


	/**
     * �豸״̬��ѯ
	 * @param id         [in] ��Դ����
     */
	virtual int OnGetDeviceStatus(IN const char* id, OUT DeviceStatusParam &status) =0;


	/**
     * �豸Զ������
	 * @param id         [in] ��Դ����
     */
	virtual int OnBootDevice(IN const char* id) =0;

};


}


/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////////////

/**
 * ʹ�ܵ�����Ϣ
 * @param enable  [in] 1ʹ�ܴ�ӡ������Ϣ��0��ʹ��
 */
NET_VIDEO_API int NET_VIDEO_METHOD enableDebug(IN int enable);

/**
 * ��¼
 * @param pInfo  [in] ��¼��Ϣ
 */
NET_VIDEO_API int NET_VIDEO_METHOD login(IN const nvs::PlatformRegInfo *pInfo);

/**
 * �ǳ�
 */
NET_VIDEO_API int NET_VIDEO_METHOD logout();

/**
 * ���µ�¼
 * @param pInfo  [in] ��¼��Ϣ
 */
NET_VIDEO_API int NET_VIDEO_METHOD reLogin(IN const nvs::PlatformRegInfo *pInfo);
/**
 * �Ƿ��¼
 */
NET_VIDEO_API int NET_VIDEO_METHOD hasLogin();

/**
 * �����ҶϺͽ���IP�Խ�
 */
extern NET_VIDEO_API int NET_VIDEO_METHOD DevStopVoip();
extern NET_VIDEO_API int NET_VIDEO_METHOD DevStartVoip();
extern NET_VIDEO_API int NET_VIDEO_METHOD DevStartDialing();
extern NET_VIDEO_API int NET_VIDEO_METHOD DevGetNewPlatInfo(void* para);
#ifdef PLT_PARA_BK
extern NET_VIDEO_API void NET_VIDEO_METHOD DevGetPlatInfo(char *pBuf);
extern NET_VIDEO_API void NET_VIDEO_METHOD DevSndPlatInfo2UI();
#endif
extern int reLoginDvs( const nvs::PlatformRegInfo *pInfo = NULL);

/**
 * ��Դ�ϱ�
 * @param pNodes  [in] ��Դ����
 * @param length  [in] ���鳤��
 * @return NVS_ERR_OK ��ʾ�ɹ���
           NVS_ERR_FAILʧ�ܣ�
 */
NET_VIDEO_API int NET_VIDEO_METHOD pushResNode(IN nvs::ResNode *pNodes,IN size_t length);


/**
 * ״̬֪ͨ
 * @param status  [in] ״̬��Ϣ
 */
NET_VIDEO_API int NET_VIDEO_METHOD notifyStatus(IN const nvs::StatusNotification* pStatus);


/**
 * �澯֪ͨ
 * @param alarm  [in] �澯��Ϣ
 */
NET_VIDEO_API int NET_VIDEO_METHOD notifyAlarm(IN const nvs::AlarmNotification* pAlarm);


/**
 * ����֪ͨ�ص�
 * @param pSink  [in] ֪ͨ������ָ��
 */
NET_VIDEO_API int NET_VIDEO_METHOD setNotificationSink(IN nvs::NotificationSink* pSink);


/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////

#endif /* NETVIDEOSYSTEM_H_ */


