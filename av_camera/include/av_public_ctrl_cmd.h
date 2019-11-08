/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : av_public_ctrl_cmd.h
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2012年7月5日星期四
  最近修改   :
  功能描述   : av_public_ctrl_cmd.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年11月26日星期四
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/
#ifndef __AV_PUBLIC_CTRL_CMD_H__
#define __AV_PUBLIC_CTRL_CMD_H__

//命令类型
#define CTRL_CMD_BASE					10000
#define CTRL_CMD_LOGIN					CTRL_CMD_BASE+1			//远程登录命令
#define CTRL_CMD_LOGOFF					CTRL_CMD_BASE+2			//注销用户登录
#define CTRL_CMD_GETDEVICEINFO			CTRL_CMD_BASE+3			//获得设备信息
#define CTRL_CMD_GETVIDEOPROPERTY		CTRL_CMD_BASE+4			//视频属性信息
#define CTRL_CMD_GETAUDIOPROPERTY		CTRL_CMD_BASE+5			//音频属性信息
#define CTRL_CMD_GETVOIPPROPERTY		CTRL_CMD_BASE+6			//voip属性信息
#define CTRL_CMD_STARTVIDEOMONITOR		CTRL_CMD_BASE+7			//开启视频预览
#define CTRL_CMD_STOPVIDEOMONITOR		CTRL_CMD_BASE+8			//停止视频预览
#define CTRL_CMD_STOPAUDIOMONITOR		CTRL_CMD_BASE+9			//停止音频预览
#define CTRL_CMD_STOPVOIP				CTRL_CMD_BASE+10		//停止voip
//#define CTRL_CMD_STOPDOWNLOAD			CTRL_CMD_BASE+11		//停止下载
#define CTRL_CMD_STOPFILEPLAY			CTRL_CMD_BASE+12		//停止文件回放
#define CTRL_CMD_STOPTIMEPLAY			CTRL_CMD_BASE+13		//停止时间回放
#define CTRL_CMD_FASTPLAY				CTRL_CMD_BASE+14		//快速播放
#define CTRL_CMD_SLOWPLAY				CTRL_CMD_BASE+15		//慢速播放
#define CTRL_CMD_SETPLAYRATE			CTRL_CMD_BASE+16		//播放速率
#define CTRL_CMD_PAUSEPLAY				CTRL_CMD_BASE+17		//暂停播放
#define CTRL_CMD_RESUMEPLAY				CTRL_CMD_BASE+18		//恢复播放
#define CTRL_CMD_SINGLEPLAY				CTRL_CMD_BASE+19		//帧进
#define CTRL_CMD_FASTBACKPLAY			CTRL_CMD_BASE+20		//快退
#define CTRL_CMD_PLAYPREV				CTRL_CMD_BASE+21		//上一段
#define CTRL_CMD_PLAYNEXT				CTRL_CMD_BASE+22		//下一段
#define CTRL_CMD_PLAYSEEK				CTRL_CMD_BASE+23		//定位
#define CTRL_CMD_PLAYMUTE				CTRL_CMD_BASE+24		//回放静音
#define CTRL_CMD_PLAYPROGRESS 			CTRL_CMD_BASE+25		//播放进度
#define CTRL_CMD_GETNETWORK				CTRL_CMD_BASE+26		//获得网络参数
#define CTRL_CMD_SETNETWORK				CTRL_CMD_BASE+27		//设置网络参数
#define CTRL_CMD_GETVGASOLLIST			CTRL_CMD_BASE+28		//获得VGA分辨率
#define CTRL_CMD_GETSYSPARAM			CTRL_CMD_BASE+29		//获得系统参数
#define CTRL_CMD_SETSYSPARAM 			CTRL_CMD_BASE+30		//设置系统参数
#define CTRL_CMD_GETRECPARAM			CTRL_CMD_BASE+31		//获得录像参数
#define CTRL_CMD_SETRECPARAM 			CTRL_CMD_BASE+32		//设置录像参数
#define	CTRL_CMD_GETSUBSTREAMPARAM		CTRL_CMD_BASE+33		//获得子码流参数
#define	CTRL_CMD_SETSUBSTREAMPARAM		CTRL_CMD_BASE+34		//设置子码流参数
#define CTRL_CMD_GETIMGPARAM			CTRL_CMD_BASE+35		//获得图像参数
#define CTRL_CMD_SETIMGPARAM 			CTRL_CMD_BASE+36		//设置图像参数
#define CTRL_CMD_GETPICADJ 				CTRL_CMD_BASE+37		//获得画面参数
#define CTRL_CMD_SETPICADJ 				CTRL_CMD_BASE+38		//设置画面参数
#define CTRL_CMD_GETALARMINPARAM 		CTRL_CMD_BASE+39		//获得报警输入参数
#define CTRL_CMD_SETALARMINPARAM 		CTRL_CMD_BASE+40		//设置报警输入参数
#define CTRL_CMD_GETALARMOUTPARAM 		CTRL_CMD_BASE+41		//获得报警输出参数
#define CTRL_CMD_SETALARMOUTPARAM 		CTRL_CMD_BASE+42		//设置报警输出参数
#define	CTRL_CMD_GETALARMNOTICYPARAM	CTRL_CMD_BASE+43		//获得报警通知参数
#define	CTRL_CMD_SETALARMNOTICYPARAM	CTRL_CMD_BASE+44		//设置报警通知参数
#define CTRL_CMD_GETRECSCHPARAM 		CTRL_CMD_BASE+45		//获得录像布防参数
#define CTRL_CMD_SETRECSCHPARAM 		CTRL_CMD_BASE+46		//设置录像布防参数
#define CTRL_CMD_GETMDPARAM 			CTRL_CMD_BASE+47		//获得移动侦测参数
#define CTRL_CMD_SETMDPARAM 			CTRL_CMD_BASE+48		//设置移动侦测参数
#define CTRL_CMD_GETVIDEOLOSTPARAM		CTRL_CMD_BASE+49		//获得视频丢失参数
#define CTRL_CMD_SETVIDEOLOSTPARAM 		CTRL_CMD_BASE+50		//设置视频丢失参数
#define CTRL_CMD_GETVIDEOBLOCKPARAM 	CTRL_CMD_BASE+51		//获得视频遮挡参数
#define CTRL_CMD_SETVIDEOBLOCKPARAM 	CTRL_CMD_BASE+52		//设置视频遮挡参数
#define CTRL_CMD_GETPTZPARAM			CTRL_CMD_BASE+53		//获得云台参数
#define CTRL_CMD_SETPTZPARAM 			CTRL_CMD_BASE+54		//设置云台参数
#define CTRL_CMD_SETPRESET 				CTRL_CMD_BASE+55		//设置预置点参数
#define CTRL_CMD_GETCRUISEPARAM 		CTRL_CMD_BASE+56		//获得巡航路径参数
#define CTRL_CMD_SETCRUISEPARAM 		CTRL_CMD_BASE+57		//设置巡航路径参数
#define CTRL_CMD_CTRLCRUISEPATH 		CTRL_CMD_BASE+58		//巡航路径控制
#define CTRL_CMD_CTRLPTZTRACK 			CTRL_CMD_BASE+59		//轨迹控制
#define CTRL_CMD_FORMAT_SATA_DISK 		CTRL_CMD_BASE+60		//格式化磁盘
#define CTRL_CMD_GETUSERINFO			CTRL_CMD_BASE+61		//获得用户信息
#define CTRL_CMD_SETUSERINFO 			CTRL_CMD_BASE+62		//设置用户信息
#define CTRL_CMD_SETRESTORE				CTRL_CMD_BASE+63		//恢复默认
#define CTRL_CMD_CLEARALARM				CTRL_CMD_BASE+64		//清除报警
#define CTRL_CMD_GETSYSTIME				CTRL_CMD_BASE+65		//获得系统时间
#define CTRL_CMD_SETSYSTIME				CTRL_CMD_BASE+66		//设置系统时间
#define CTRL_CMD_GETSYSINFO				CTRL_CMD_BASE+67		//获得系统信息
#define CTRL_CMD_SHUTDOWN				CTRL_CMD_BASE+68		//关闭系统
#define CTRL_CMD_REBOOT					CTRL_CMD_BASE+69		//重启系统
#define CTRL_CMD_PTZCTRL				CTRL_CMD_BASE+70		//云台控制
#define CTRL_CMD_GETMANUALREC			CTRL_CMD_BASE+71		//获得手动录像状态
#define CTRL_CMD_SETMANUALREC			CTRL_CMD_BASE+72		//设置手动录像状态
#define CTRL_CMD_LOGSEARCH				CTRL_CMD_BASE+73		//日志文件搜索
#define CTRL_CMD_LOCAL_REC_PLAY	            CTRL_CMD_BASE+74		//本地回放
#define CTRL_CMD_GETSPSPPS				CTRL_CMD_BASE+75		//获得spspps数据
#define CTRL_CMD_ALARMUPLOADCENTER		CTRL_CMD_BASE+76		//报警上传中心
#define CTRL_CMD_CDROM					CTRL_CMD_BASE+77		//光驱控制

#define CTRL_CMD_COM_PROTOCOL_GET		CTRL_CMD_BASE+78		//获得串口参数
#define CTRL_CMD_COM_PROTOCOL_SET		CTRL_CMD_BASE+79		//设置串口参数
#define CTRL_CMD_SERIALSTOP				CTRL_CMD_BASE+80		//停止透明串口

#define CTRL_CMD_XWSERVER_GET			CTRL_CMD_BASE+85		//csp modify 20100419
#define CTRL_CMD_XWSERVER_SET			CTRL_CMD_BASE+86		//csp modify 20100419

#define CTRL_CMD_GETRECFILEINDEX		CTRL_CMD_BASE+87		//csp modify 20100419
// lihg add 20110808
#define  CTRL_CMD_3GSERVER 				CTRL_CMD_BASE+90 		//设置3G服务器
// lihg add end
#define CTRL_CMD_SETMAC					CTRL_CMD_BASE+92		//设置MAC地址


#define CTRL_CMD_GET_ALARMIN_VAL		CTRL_CMD_BASE+103		//获取报警输入值
#define CTRL_CMD_SET_ALARMOUT_VAL		CTRL_CMD_BASE+104		//设置报警输出值

#define CTRL_CMD_STREAM_REQ       		CTRL_CMD_BASE+105

#define CTRL_CMD_AUTO_BITRATE   		CTRL_CMD_BASE+107        // 动态码流

#define CTRL_CMD_PLAYMP3FILE_START      CTRL_CMD_BASE+108        	// 播放MP3文件
#define CTRL_CMD_PLAYMP3FILE_STOP       CTRL_CMD_BASE+109        	// 停止播放MP3文件
#define CTRL_CMD_PLAYMP3FILE_PAUSE      CTRL_CMD_BASE+110       	// 播放MP3文件
#define CTRL_CMD_PLAYMP3FILE_SETVOLUME  CTRL_CMD_BASE+111       	// 设置音量
#define CTRL_CMD_PLAYMP3FILE_GETSTATUS  CTRL_CMD_BASE+112       	// 获取MP3播放状态
#define CTRL_CMD_JPEGCAPTURE			CTRL_CMD_BASE+113       	// JPEG抓图
#define CTRL_CMD_GETDEVICESTATUS		CTRL_CMD_BASE+114       	// 获取设备状态

//add pfli 20120213
#define CTRL_CMD_GETBUSINFO				CTRL_CMD_BASE+115       	// 获取车辆信息
#define CTRL_CMD_SETBUSINFO				CTRL_CMD_BASE+116       	// 设置车辆信息
// lipf add 20111125
#define CTRL_CMD_UPDATE					CTRL_CMD_BASE+117		//升级
#define CTRL_CMD_GET_UPDATE_POS			CTRL_CMD_BASE+118		//获得升级进度
#define CTRL_CMD_GET_UPDATE_STATE		CTRL_CMD_BASE+119		//获得升级状态
// lipf add end
//#define CTRL_CMD_STOPVOIP				CTRL_CMD_BASE+200		//停止voip
//#define CTRL_CMD_STARTVOIP				CTRL_CMD_BASE+201		//开始voip
#define CTRL_CMD_STARTVOIP				CTRL_CMD_BASE+11		//开始voip

#define CTRL_CMD_PLAYMP3FILE_GETVOLUME  CTRL_CMD_BASE+150       	// 获取音量值
#define CTRL_CMD_SETOSD_BUSNO  			CTRL_CMD_BASE+151       	// 设置设备号osd
#define CTRL_CMD_GETOSD_BUSNO  			CTRL_CMD_BASE+152       	// 获取设备号osd
#define CTRL_CMD_SHOWOSD_BUSNO  		CTRL_CMD_BASE+153       	// 显示或隐藏设备号osd
#define CTRL_CMD_SETOSD_EXTERN  		CTRL_CMD_BASE+154       	// 设置扩展osd
#define CTRL_CMD_GETOSD_EXTERN   		CTRL_CMD_BASE+155       	// 获取扩展osd
#define CTRL_CMD_SHOWOSD_EXTERN			CTRL_CMD_BASE+156      		// 显示或隐藏扩展osd
//pfli add end

// lihg add 20120321
#define CTRL_CMD_AOUT_PLAYBACK_ON  		CTRL_CMD_BASE+157       	// 打开回放通道
#define CTRL_CMD_AOUT_PLAYBACK_OFF  	CTRL_CMD_BASE+158       	// 关闭回放通道
// lihg add end

/* BEGIN: Added by  , 2013/6/3   PN:调试视频输出.89001012 */
#define CTRL_CMD_VOUT_GETDEV_CSC  		CTRL_CMD_BASE+159       	// 获取视频输出效果
#define CTRL_CMD_VOUT_SETDEV_CSC      	CTRL_CMD_BASE+160       	// 设置视频输出效果
/* END:   Added by  , 2013/6/3   PN:调试视频输出.89001012 */
#define CTRL_CMD_UMOUNT_DISK  		    CTRL_CMD_BASE+161       	// 卸载硬盘
//#define CTRL_CMD_VOUT_SETDEV_CSC      	CTRL_CMD_BASE+160       	// 设置视频输出效果
#define CTRL_CMD_GET_DISK_STATUE        CTRL_CMD_BASE+162           /* 获取硬盘挂载状态 */
#define CTRL_CMD_SET_REC_CHN         CTRL_CMD_BASE+163           /* 设置录像通道 */
#define CTRL_CMD_GET_REC_CHN         CTRL_CMD_BASE+164           /* 获取录像通道状态 */
/* BEGIN: Added by  , 2013/12/30   PN:添加故障上传功能.1027001014 */
#define CTRL_CMD_SET_MP3FILE_STATUE     CTRL_CMD_BASE+165           /* 设置MP3文件状态 */
#define CTRL_CMD_SET_ERROR_STATUE       CTRL_CMD_BASE+166           /* 设备故障状态 */
/* END:   Added by  , 2013/12/30   PN:添加故障上传功能.1027001014 */

/* BEGIN: Added by xiaomengmeng, 2014/1/15  DES:添加U盘导录像  PN:4431 */
#define CTRL_CMD_RECORD_EXPORT_START     CTRL_CMD_BASE+167           /* U盘导录像启动命令 */
#define CTRL_CMD_RECORD_EXPORT_CANCLE    CTRL_CMD_BASE+168           /* U盘导录像取消命令 */
#define CTRL_CMD_RECORD_EXPORT_STATE     CTRL_CMD_BASE+169           /* 到录像过程状态 */
/* END:   Added by xiaomengmeng, 2014/1/15   PN:4431 */
#define CTRL_CMD_REC_SEARCH           CTRL_CMD_BASE + 170  /* 录像搜索 */
#define CTRL_CMD_START_REC_PLAY       CTRL_CMD_BASE + 171  /* 录像回放 */
#define CTRL_CMD_STOP_REC_PLAY        CTRL_CMD_BASE + 172  /* 停止录像回放 */
#define CTRL_CMD_FOUR_CHAN_PLAY       CTRL_CMD_BASE + 178  /*4通道回放 */
/* BEGIN: Added by  , 2014/10/11  DES:添加调整视频输入参数  PN:14100801008 */
#define CTRL_CMD_SET_VIDEO_INPUT      CTRL_CMD_BASE + 174  /* 设置视频输入参数 */
#define CTRL_CMD_GET_VIDEO_INPUT      CTRL_CMD_BASE + 173  /* 获取视频输入参数 */
/* END:   Added by  , 2014/10/11   PN:14100801008 */
/* BEGIN: Added by  , 2014/10/24  DES:添加获取视频输出默认参数命令  PN:14102401010 */
#define CTRL_CMD_GET_VIDEO_OUTPUT_ATUO      CTRL_CMD_BASE + 174  /* 获取视频输出参数默认值 */
/* END:   Added by  , 2014/10/24   PN:14102401010 */
// lihg add 20110729
#define CTRL_CMD_BASE_EX 				10120
#define CTRL_CMD_GETPARAM_EX			CTRL_CMD_BASE_EX+1
#define CTRL_CMD_SETPARAM_EX			CTRL_CMD_BASE_EX+2
// lihg add end
#define CTRL_CMD_PREVIEWSWITCH_EX		CTRL_CMD_BASE_EX+3          //单画面与4画面预览切换//add pfli 20120213
#define CTRL_CMD_SDRESET_EX				CTRL_CMD_BASE_EX+4       //sd卡重启

//协议
#define CTRL_PROTOCOL_SERVERPORT		6630					//服务器端口 6610
#define CTRL_PROTOCOL_MAXLINKNUM		256
#define CTRL_PROTOCOL_CONNECT_BLOCK		INFINITE				//连接方式:阻塞
#define CTRL_PROTOCOL_CONNECT_DEFAULT	3000					//缺省连接时间:3秒
#define CTRL_VERSION					0x0100

#define CTRL_COMMAND					0
#define CTRL_NOTIFY						1
#define CTRL_ACK						2

//事件通知类型
#define CTRL_NOTIFY_BASE				20000
#define	CTRL_NOTIFY_CONNLOST			CTRL_NOTIFY_BASE+0		//断链消息
#define	CTRL_NOTIFY_HEARTBEAT_REQ		CTRL_NOTIFY_BASE+1		//心跳请求
#define	CTRL_NOTIFY_HEARTBEAT_RESP		CTRL_NOTIFY_BASE+2		//心跳回复
#define CTRL_NOTIFY_PLAYEND				CTRL_NOTIFY_BASE+3		//放像结束
#define CTRL_NOTIFY_PLAYPROGRESS		CTRL_NOTIFY_BASE+4		//放像进度通知
#define CTRL_NOTIFY_HASAUDIO			CTRL_NOTIFY_BASE+5		//回放文件是否有音频
#define CTRL_NOTIFY_UPDATEPROGRESS		CTRL_NOTIFY_BASE+6		//升级进度
#define CTRL_NOTIFY_ALARMINFO			CTRL_NOTIFY_BASE+7		//异步报警信息

//应答类型
#define CTRL_SUCCESS					0						//成功
#define CTRL_FAILED_BASE				30000					//错误码偏移量
#define CTRL_FAILED_USER				CTRL_FAILED_BASE+1		//不存在的用户名
#define CTRL_FAILED_PASSWORD			CTRL_FAILED_BASE+2		//密码错误
#define CTRL_FAILED_COMMAND				CTRL_FAILED_BASE+3		//未认可的命令
#define CTRL_FAILED_PARAM				CTRL_FAILED_BASE+4		//无效参数
#define CTRL_FAILED_OUTOFMEMORY			CTRL_FAILED_BASE+5		//内存不足
#define CTRL_FAILED_RESOURCE			CTRL_FAILED_BASE+6		//资源不足
#define CTRL_FAILED_FILENOTEXIST		CTRL_FAILED_BASE+7		//文件不存在
#define CTRL_FAILED_DATABASE			CTRL_FAILED_BASE+8		//数据库错误
#define CTRL_FAILED_RELOGIN				CTRL_FAILED_BASE+9		//重复登录
#define CTRL_FAILED_BAUDLIMIT			CTRL_FAILED_BASE+10		//每一路通道最多支持5路实时监控
#define CTRL_FAILED_CREATESOCKET		CTRL_FAILED_BASE+11		//创建套结字失败
#define CTRL_FAILED_CONNECT				CTRL_FAILED_BASE+12		//网络连接失败
#define CTRL_FAILED_BIND				CTRL_FAILED_BASE+13		//绑定失败
#define CTRL_FAILED_LISTEN				CTRL_FAILED_BASE+14		//侦听失败
#define CTRL_FAILED_NETSND				CTRL_FAILED_BASE+15		//网络发送出错
#define CTRL_FAILED_NETRCV				CTRL_FAILED_BASE+16		//网络接收出错
#define CTRL_FAILED_TIMEOUT				CTRL_FAILED_BASE+17		//网络连接超时
#define CTRL_FAILED_CHNERROR			CTRL_FAILED_BASE+18		//超出通道限制
#define CTRL_FAILED_DEVICEBUSY			CTRL_FAILED_BASE+19		//设备正在忙
#define CTRL_FAILED_WRITEFLASH			CTRL_FAILED_BASE+20		//烧写flash出错
#define CTRL_FAILED_VERIFY				CTRL_FAILED_BASE+21		//校验错
#define CTRL_FAILED_CONFLICT			CTRL_FAILED_BASE+22		//系统资源冲突
#define CTRL_FAILED_BUSY				CTRL_FAILED_BASE+23		//系统正在忙
#define CTRL_FAILED_LINKLIMIT			CTRL_FAILED_BASE+24		//已达到连接上限
#define CTRL_FAILED_USER_SAME			CTRL_FAILED_BASE+25		//用户名相同07-08-02
#define CTRL_FAILED_MACADDR				CTRL_FAILED_BASE+26		//远程访问的pc mac地址错误
#define CTRL_FAILED_NOINIT				CTRL_FAILED_BASE+27		//模块未初始化
#define CTRL_FAILED_USER_MAX			CTRL_FAILED_BASE+28		//用户数最多//wrchen 080529


#define CTRL_FAILED_UNKNOWN				CTRL_FAILED_BASE+9999	//未知错误

//add pfli 20120416
#define SERVER_CTRL_NOTIFY_BASE                   50000
#define SERVER_CTRL_ALARMUPLOADCENTER             SERVER_CTRL_NOTIFY_BASE+6 // 报警上传
//pfli  add end

//连接
#define CTRL_CONNECTION_NULL			0x0
#define CTRL_CONNECTION_TCPCLIENT		0x1
#define CTRL_CONNECTION_TCPSERVER		0x2

//组播搜索设备
#define CTRL_DEVICESEARCH_ACKCLIENT		0X1
#define CTRL_DEVICESEARCH_ACKSERVER		0x2
#define SEARCHPORT						6666
#define ACKSEARCHPORT					SEARCHPORT+1
#define MULTICASTGROUP					"224.0.1.2"

//码流传输
#define VIDEO_CHANNELS		16
#define AUDIO_CHANNELS		16
#define EACH_STREAM_TCP_LINKS	5
#define MAX_REMOTE_PLAYER_NUM	8
#define VOIP_NUM				1
#define DOWNLOAD_NUM			1
#define	UPDATE_NUM				1
#define MEDIA_LINKMAX_SVR			(VIDEO_CHANNELS+AUDIO_CHANNELS)*(EACH_STREAM_TCP_LINKS)+MAX_REMOTE_PLAYER_NUM+VOIP_NUM
#define	STREAM_LINK_MAXNUM		MEDIA_LINKMAX_SVR+DOWNLOAD_NUM+UPDATE_NUM

/* App模块回应的命令字 */
#define CTRL_BACK_CMD_BASE					15000
/* 回应MP3开始播放命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_START      CTRL_BACK_CMD_BASE+108
/* 回应MP3停止播放命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_STOP       CTRL_BACK_CMD_BASE+109
/* 回应MP3暂停命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_PAUSE      CTRL_BACK_CMD_BASE+110       	// 播放MP3文件
/* 回应MP3播放设置音量命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_SETVOLUME  CTRL_BACK_CMD_BASE+111       	// 设置音量
/* 回应MP3播放获取音量值命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_GETSTATUS  CTRL_BACK_CMD_BASE+112       	// 获取MP3播放状态
/* 回应获取音量值命令字 */
#define CTRL_BACK_CMD_PLAYMP3FILE_GETVOLUME  CTRL_BACK_CMD_BASE+150       	// 获取音量值
/* 打开回放通道 */
#define CTRL_BACK_CMD_AOUT_PLAYBACK_ON       CTRL_BACK_CMD_BASE+157       	// 打开回放通道
/* 关闭回放通道 */
#define CTRL_BACK_CMD_AOUT_PLAYBACK_OFF  	 CTRL_BACK_CMD_BASE+158       	// 关闭回放通道
/* 停止IP对讲 */
#define CTRL_BACK_CMD_STOPVOIP				CTRL_BACK_CMD_BASE+200
/* 开启IP对讲 */
#define CTRL_BACK_CMD_STARTVOIP				CTRL_BACK_CMD_BASE+201
/* 获取手动录像状态 */
#define CTRL_BACK_CMD_GETMANUALREC			CTRL_BACK_CMD_BASE+71
/* 设置手动录像状态 */
#define CTRL_BACK_CMD_SETMANUALREC			CTRL_BACK_CMD_BASE+72
/* 画面切换 */
#define CTRL_BACK_CMD_PREVIEWSWITCH_EX      CTRL_BACK_CMD_BASE+123
/* 视频抓图 */
#define CTRL_BACK_CMD_JPEGCAPTURE           CTRL_BACK_CMD_BASE+113       	// JPEG抓图
/* 获取音频属性 */
#define CTRL_BACK_CMD_GETAUDIOPROPERTY      CTRL_BACK_CMD_BASE+5			//音频属性信息
/* 获取系统参数 */
#define CTRL_BACK_CMD_GETSYSINFO			CTRL_BACK_CMD_BASE+67		//获得系统信息
/* 开启视频预览 */
#define CTRL_BACK_CMD_STARTVIDEOMONITOR		CTRL_BACK_CMD_BASE+7			//开启视频预览
/* 停止视频预览 */
#define CTRL_BACK_CMD_STOPVIDEOMONITOR		CTRL_BACK_CMD_BASE+8			//停止视频预览
/* 获取车辆信息 */
#define CTRL_BACK_CMD_GETBUSINFO		    CTRL_BACK_CMD_BASE+115       	// 获取车辆信息
/* 设置车辆信息 */
#define CTRL_BACK_CMD_SETBUSINFO			CTRL_BACK_CMD_BASE+116       	// 设置车辆信息
/* 获取视频属性 */
#define CTRL_BACK_CMD_GETVIDEOPROPERTY		CTRL_BACK_CMD_BASE+4			//视频属性信息
/* 设置设备号OSD */
#define CTRL_BACK_CMD_SETOSD_BUSNO  		CTRL_BACK_CMD_BASE+151       	// 设置设备号osd
/* 获取设备号OSD */
#define CTRL_BACK_CMD_GETOSD_BUSNO  		CTRL_BACK_CMD_BASE+152       	// 获取设备号osd
/* 显示设备号OSD */
#define CTRL_BACK_CMD_SHOWOSD_BUSNO  		CTRL_BACK_CMD_BASE+153       	// 显示或隐藏设备号osd
/* 扩展OSD数据显示 */
#define CTRL_BACK_CMD_SETOSD_EXTERN  		CTRL_BACK_CMD_BASE+154       	// 设置扩展osd
/* 获取扩展OSD数据显示 */
#define CTRL_BACK_CMD_GETOSD_EXTERN   		CTRL_BACK_CMD_BASE+155       	// 获取扩展osd
/* 显示或隐藏扩展OSD数据显示 */
#define CTRL_BACK_CMD_SHOWOSD_EXTERN		CTRL_BACK_CMD_BASE+156      		// 显示或隐藏扩展osd
/* 获取视频输出参数 */
#define CTRL_BACK_CMD_VOUT_GETDEV_CSC  		CTRL_BACK_CMD_BASE+159       	// 获取视频输出效果
/* 设置视频输出参数 */
#define CTRL_BACK_CMD_VOUT_SETDEV_CSC      	CTRL_BACK_CMD_BASE+160       	// 设置视频输出效果


#endif /* __AV_PUBLIC_CTRL_CMD_H__ */

