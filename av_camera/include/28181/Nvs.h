/********************************************************************
created:	2014/12/08
modify :    2014/12/08
author:
purpose:    NVS封装接口,适配28181规范
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

////基础数据类型
typedef unsigned char            uint8_t;
typedef unsigned short           uint16_t;
typedef long long                int64_t;
typedef unsigned long long       uint64_t;


///常量
enum Constants
{
	MAX_IPV4_LEN = 16,
    MAX_ID       = 32,
    MAX_NAME     = 128,
    MAX_DESC     = 256,
    MAX_SPROP    = 512
};



///网络jitter值评估级别,应用层根据QOS评估做出QOS决策，动态调整视频分辨率、码率、帧率、qp等值
enum QOSJitter
{
	QOS_LEVEL_NORMAL = 0,     ///正常，以目前的码率可以正常发送，无需改变
	QOS_LEVEL_BETTER = 1,     ///较好，可以适当增加视频参数，提供较好服务
	QOS_LEVEL_BAD    = 2      ///较差，网络拥塞，可以适当较低视频参数，保持流畅性
};

//错误类型定义
enum NvsErrorCode
{
	NVS_ERR_FAIL              = -1,      //失败
	NVS_ERR_OK                = 0,       //成功
	NVS_ERR_NO_INIT           = -100,   //未初始化
	NVS_ERR_AUTH_FAIL         = -102,   //鉴权失败
	NVS_ERR_OUT_OF_SERVER     = -103,   //不能提供服务
	NVS_ERR_CONNT_AGU_FAIL    = -104,   //连接接入网关服务器失败
	NVS_ERR_SOCKET_ERR        = -105,   //SOCKET错误
	NVS_ERR_RECV_TIMEOUT      = -106,   //网络接收超时
	NVS_ERR_MEM_ALLOC_FAIL    = -107,   //分配内存错误
	NVS_ERR_REPORT_RES_FAIL   = -108,   //上报资源失败
	NVS_ERR_INVALID_RES       = -109,   //无效的资源
	NVS_ERR_SEND_RTP_FAIL     = -110,   //发送流媒体失败
	NVS_ERR_INVAID_CHANNEL    = -111,   //错误的通道号
	NVS_ERR_RES_NO_EXIST      = -112,   //请求的资源不存在
	NVS_ERR_RES_NO_ONLINE     = -114,   //请求的资源不在线
	NVS_ERR_R_OVER_MAXLINK    = -115    //资源超过最大连接数

};

///云镜控制动作
enum PTZCommand
{
	PTZ_CMD_START   = 0,   ///无意义
	PTZ_CMD_STOP    = 1,   ///停止

	PTZ_CMD_RIGHT   = 2,   ///右
	PTZ_CMD_LEFT    = 4,   ///左
	PTZ_CMD_DOWN    = 6,   ///下
	PTZ_CMD_UP      = 8,   ///上

	PTZ_CMD_SMALL   = 10,    ///光圈变小
	PTZ_CMD_LARGE   = 12,    ///光圈变大

	PTZ_CMD_ZOOM_IN  = 14,   ///焦距变大
	PTZ_CMD_ZOOM_OUT = 16,   ///焦距变小

	PTZ_CMD_FOCUS_IN  = 18,  ///焦点前调
	PTZ_CMD_FOCUS_OUT = 20,  ///焦点后调

	PTZ_CMD_ASSIST_FUNC_1 = 30,	 ///雨刷
	PTZ_CMD_ASSIST_FUNC_2 = 32,	 ///辅佐开关2
	PTZ_CMD_ASSIST_FUNC_3 = 34,	 ///辅佐开关3
	PTZ_CMD_ASSIST_FUNC_4 = 36,	 ///辅佐开关4

	PTZ_RIGHT_APPLY   = 40,    ///权限申请
	PTZ_RIGHT_RELEASE = 42,    ///权限释放

	PTZ_PRESET_ADD    = 110,   ///添加预置位
	PTZ_PRESET_MODIDY = 112,   ///修改预置位
	PTZ_PRESET_DELETE = 114,   ///删除预置位
	PTZ_PRESET_GOTO   = 116,   ///转到预置位

	PTZ_CMD_SCAN = 120,        ///自动扫描开
	PTZ_CMD_LENGTH
};


///预置位类型
enum PresetFlag
{
	PRESET_FLAG_NORM  = 0,   ///普通预置位
	PRESET_FLAG_DAY   = 1,   ///白天守望位
	PRESET_FLAG_NIGHT = 2    ///夜晚守望位
};




///告警、故障类型
enum AlarmType
{
	ALARM_INTRUSION = 1,/// 入侵
	ALARM_LOITER,       /// 徘徊
	ALARM_MOVING,       /// 移动侦测
	ALARM_STAY,         /// 逗留
	ALARM_LEFT_TAKE,    /// 遗留物检测
	ALARM_LINE,         /// 穿越绊线
	ALARM_DISK_FULL,    /// 硬盘满
	ALARM_DISK_FAULT,   /// 硬盘故障
	ALARM_SERVER_OFF,   /// 服务器断开
	ALARM_VIDEO_LOST,   /// 视频丢失
	ALARM_ENCODER_OFF,  /// 编码器断开
	ALARM_INPUT,        /// 告警输入告警
	ALARM_USER_PLAY_ERROR     /// 用户实时预览异常
};


///告警、状态通知的状态
enum StatusType
{
	STATUS_BEGIN = 0,   ///状态开始
	STATUS_END = 1      ///状态结束
};

///媒体数据帧类型
enum FrameType
{
	FRAME_PCM      = 0x00,
	FRAME_G711A    = 0x08,
	FRAME_OPUS     = 0x09,

	FRAME_VIDEO_I  = 0x10,
	FRAME_VIDEO_P  = 0x11
};


///媒体压缩类型
enum CompressionCodec
{
	G711A_CODEC = 0x08,

	STANDARD_H264_CODEC  = 98,
	STANDARD_MPEG4_CODEC = 97
};


struct SipAddrInfo
{
	char     id[MAX_ID];       //sip地址编码ID号
	char     ip[MAX_IPV4_LEN]; //sip监听的IP地址
	uint16_t port;             //sip监听的端口
};

///平台注册信息
struct PlatformRegInfo
{
	SipAddrInfo  nvsAddr;      //nvs自己的sip信息
	SipAddrInfo  serverAddr;   //要注册的服务器的sip信息

	char  userName[MAX_NAME];  //用户名
	char  password[MAX_NAME];  //密码

	uint16_t expires;         //sip刷新注册的时间间隔
};


///上报资源信息
struct ResNode
{
    char    id[MAX_ID];        /// 统一资源编码（摄像头或目录结构）
    char    name[MAX_NAME];    /// 资源名称
    char    parentID[MAX_ID];  /// 资源父节点的统一资源编码
    int     subNum;            /// 子节点数目
    int     codec;             /// 解码标签
    int     status;            /// 状态，0不可用，1可用
	int     resType;           /// 资源类型，0是目录，1是其它
	char    address[MAX_NAME]; /// 资源父节点的统一资源编码
	int     hasGuard;
};


///预置位信息
struct Preset
{
    int  num;            ///预置位编号
    char name[MAX_NAME]; ///预置位名字
    int  flag;           ///预置位类型
};


///录像信息
struct RecordItem
{
	char   id[MAX_ID];         ///资源编码
	time_t beginTime;          ///实际开始时间
	time_t endTime;            ///实际结束时间
	int    secrecy;            ///0不涉密，1涉密
	char   type[MAX_ID];       ///录像类型,time日常  alarm告警  manual手动  all全部
	char   recordID[MAX_NAME]; ///录像触发者ID
};

///查询录像信息请求
struct QueryRecordRequest
{
	char   id[MAX_ID];    ///资源编码
	time_t beginTime;     ///开始时间
	time_t endTime;       ///结束时间
	int    secrecy;       ///0不涉密，1涉密
	char   type[MAX_ID];  ///录像类型,time日常  alarm告警  manual手动  all全部
	char   recordID[MAX_NAME]; ///录像触发者ID
};


///查询录像信息响应
struct QueryRecordResponse
{
	int  sum;               ///输入输出参数，输入pItems的内存长度，实际返回的录像信息数量
	RecordItem* pItems;     ///录像信息结构
};



///状态通知
struct StatusNotification
{
    char    id[MAX_ID];  ///资源编码
    time_t  time;        ///状态发生时间
    int     status;      ///状态
};


///告警通知
struct AlarmNotification
{
    char    id[MAX_ID];     ///资源编码
    char    name[MAX_NAME]; ///资源名字
    int     type;           ///告警类型
    int     priority;       ///告警级别
    time_t  time;           ///告警时间
	int     status;         ///告警状态
    double  longitude;      ///告警经度
    double  latitude;       ///告警纬度
    char    description[MAX_DESC]; ///告警描述
};





/////////////////////////////////////////////////////////////////////////////
///媒体信息描述
struct StreamFormat
{
    double duration;    ///媒体总时长（秒），历史视频有效

    int codec;          ///解码标签，对应解码库
    int width;          ///视频宽度
    int height;         ///视频高度
    int fps;            ///帧率
    int bitrate;        ///码率
    int iFrame;         ///关键帧间隔
    int propSize;             ///媒体描述信息长度，即prop实际长度
    uint8_t prop[MAX_SPROP];  ///媒体描述信息，比如海康40字节解码头数据
    char mediaName[MAX_NAME];       ///媒体名字

    int audioCodec;         ///音频编码
    int audioChannel;       ///音频通道
    int audioSampleRate;    ///采样率sample rate
    int audioPropSize;      ///音频描述信息长度，即audioProp实际长度
    uint8_t audioProp[MAX_DESC]; ///音频描述信息

	char  redirectIp[MAX_IPV4_LEN];
	int   redirectPort;
};





///媒体数据发送结构
struct StreamPacket
{
    uint8_t* data;      ///媒体数据
    int      size;      ///媒体数据长度
	int      flag;      ///媒体帧类型
    uint64_t timestamp; ///数据帧产生的时间，milliseconds
};


///设备信息参数
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

///媒体数据处理
class  StreamSink
{
public:
    virtual ~StreamSink() {}

	//在第一次写数据前需要告诉SDK媒体数据的格式（仅需要设置一次）
    virtual void setFormat(IN const StreamFormat& fmt) =0;

	//在请求媒体流时，调用该函数将流媒体数据写入SDK，由SDK负责发送
    virtual int writePacket(IN const StreamPacket& packet) =0;

	///通知发送媒体数据结束
    virtual void endOfStream() =0;
};




///媒体控制，实时视频只需实现部分，历史视频都需实现
class  MediaStream
{
public:
    virtual ~MediaStream() {}

	///播放
    virtual int mplay() =0;

    ///暂停
    virtual int mpause() =0;

	///停止
    virtual int mstop() =0;

	///逐帧，单帧
	virtual int mstep() =0;

	///拖拽,进度0-100
    virtual int mseek(IN int offset) =0;

	///倍速，1/4.0 ---- 4.0
    virtual int mscale(IN double fscale) =0;

	///网络QOS反馈
	virtual int mjitter(IN int jitterLevel) =0;

	///设置流回调
    virtual int setSink(IN StreamSink* pSink) =0;
};


///消息通知
class  NotificationSink
{
public:
    virtual ~NotificationSink() {}

	/**
     * 注册成功
     */
    virtual int OnRegisterOk(IN struct tm timeNow) =0;

	/**
     * 断开与服务器的连接
     */
    virtual int OnDisConnect() =0;

    /**
     * 控制云镜
	 * @param id    [in] 资源编码
	 * @param cmd   [in] 命令
	 * @param speed [in] 速度
	 * @param data  [in] 动作（开始、停止）
	 * @param para  [in] 参数，可为空，预置位时为名字
     */
    virtual int OnControlPtz(IN const char* id, IN int cmd,
		IN int speed, IN int data,IN const char *para) =0;


	 /**
     * 强制关键帧
	 * @param id          [in] 资源编码
	 * @param streamIdx   [in] 码流类型，0主码流，1子码流
     */
	virtual int OnMakeKeyFrame (IN const char *id,IN int streamIdx) =0;


	 /**
     * 打开实时视频
	 * @param id          [in] 资源编码
	 * @param streamIdx   [in] 码流类型，0主码流，1子码流
	 * @param ppStream    [in] 码流控制类指针句柄，使用者派生实现
     */
    virtual int OnOpenLiveStream(IN const char* id, IN int streamIdx,
		OUT MediaStream** ppStream) =0;


	/**
     * 打开历史视频流
	 * @param rec          [in] 资源描述
	 * @param ppStream     [in] 码流控制类指针句柄，使用者派生实现
     */
    virtual int OnOpenRecordStream(IN const RecordItem& rec,
		OUT MediaStream** ppStream) =0;

	/**
     * 关闭视频
	 * @param ppStream    [in] 码流控制类指针句柄
     */
    virtual int OnCloseStream(IN MediaStream* pStream) =0;


	/**
     * 打开音频对讲
	 * @param id           [in]   资源编码
	 * @param pSink        [out]  数据接收通道
	 * @param ppStream     [out]  数据发送通道，使用者派生实现
     */
    virtual int OnOpenPhoneStream(IN const char* id, OUT StreamSink** ppSink,
		OUT MediaStream** ppStream) =0;


	/**
     * 查询历史视频
	 * @param req         [in] 查询参数
	 * @param resp        [in] 响应信息
     */
    virtual int OnQueryRecord(IN const QueryRecordRequest& req,
		OUT QueryRecordResponse& resp) =0;

	/**
     * 手动录像
	 * @param id         [in] 资源编码
	 * @param isStart    [in] 1启动，0停止
     */
	virtual int OnRecord(IN const char* id,IN int isStart) =0;


	/**
     * 布防
	 * @param id         [in] 资源编码
	 * @param guardFlag  [in] 0撤防，1布防
     */
	virtual int OnSetGuard(IN const char* id, IN int guardFlag) =0;

	/**
     * 报警复位
	 * @param id         [in] 资源编码
	 * @param alarmType  [in] 告警类型
     */
	virtual int OnResetAlarm(IN const char* id, IN int alarmType) =0;


	/**
     * 设备信息查询
	 * @param id         [in]  资源编码
	 * @param info       [out] 设备信息
     */
	virtual int OnGetDeviceInfo(IN const char* id,OUT DeviceInfoParam &info) =0;


	/**
     * 设备状态查询
	 * @param id         [in] 资源编码
     */
	virtual int OnGetDeviceStatus(IN const char* id, OUT DeviceStatusParam &status) =0;


	/**
     * 设备远程启动
	 * @param id         [in] 资源编码
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
 * 使能调试信息
 * @param enable  [in] 1使能打印调试信息，0不使能
 */
NET_VIDEO_API int NET_VIDEO_METHOD enableDebug(IN int enable);

/**
 * 登录
 * @param pInfo  [in] 登录信息
 */
NET_VIDEO_API int NET_VIDEO_METHOD login(IN const nvs::PlatformRegInfo *pInfo);

/**
 * 登出
 */
NET_VIDEO_API int NET_VIDEO_METHOD logout();

/**
 * 重新登录
 * @param pInfo  [in] 登录信息
 */
NET_VIDEO_API int NET_VIDEO_METHOD reLogin(IN const nvs::PlatformRegInfo *pInfo);
/**
 * 是否登录
 */
NET_VIDEO_API int NET_VIDEO_METHOD hasLogin();

/**
 * 主动挂断和接听IP对讲
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
 * 资源上报
 * @param pNodes  [in] 资源数组
 * @param length  [in] 数组长度
 * @return NVS_ERR_OK 表示成功；
           NVS_ERR_FAIL失败；
 */
NET_VIDEO_API int NET_VIDEO_METHOD pushResNode(IN nvs::ResNode *pNodes,IN size_t length);


/**
 * 状态通知
 * @param status  [in] 状态信息
 */
NET_VIDEO_API int NET_VIDEO_METHOD notifyStatus(IN const nvs::StatusNotification* pStatus);


/**
 * 告警通知
 * @param alarm  [in] 告警信息
 */
NET_VIDEO_API int NET_VIDEO_METHOD notifyAlarm(IN const nvs::AlarmNotification* pAlarm);


/**
 * 设置通知回调
 * @param pSink  [in] 通知派生类指针
 */
NET_VIDEO_API int NET_VIDEO_METHOD setNotificationSink(IN nvs::NotificationSink* pSink);


/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////

#endif /* NETVIDEOSYSTEM_H_ */


