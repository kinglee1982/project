// DevSearch.cpp : Defines the entry point for the console application.
//
#include "DevSearch.h"

#include "Nvs.h"
#include "tinyxml2API.h"
#include "TM_APPEvent.h"
#include "av_ocx_type.h"
#include "av_sys.h"

#define COMM_PORT 3000

#if defined(WIN32)
#else
#define DBG_ERR(x...)		perror(x)
#define NETCARDNAME		"eth0"
#define SOCKET int
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#define SOCKET_ERROR            (-1)

#define PROCNET_ROUTE_PATH		"/proc/net/route"
#define CONFIG_FILE				"/tmp/NET.TXT"


#define MSG_KEY			0x12345		///用于AV_Server流控制的消息，sysserver(approDrvMsg)<->avserver(stream)
#define SYS_MSG_KEY		0x36f9  	///< Message key used by system server.sysserver<->BOA SYS_MSG_*
#define FILE_MSG_KEY	0xc54be5 	///< File message key.文件管理和sysserver间,FILE_MSG_*
#define ALARM_KEY		0x542c7ae6 	///< Alarm message key.
#define WDT_MSG_KEY		0x410105 	///< Watchdog message key.	XQ
#define AUD_MSG_KEY     0x31       	///< Audio alarm message key

#define PROC_MEM_SIZE	4096*3
#define MAX_SHARE_PROC	9

enum {
	MSG_TYPE_START = 0,

	MSG_TYPE_MSG1, ///< Message type 1. Always reserved for server.
	MSG_TYPE_MSG2, ///< Message type 2.
	MSG_TYPE_MSG3, ///< Message type 3.
	MSG_TYPE_MSG4, ///< Message type 4.
	MSG_TYPE_MSG5, ///< Message type 5.
	MSG_TYPE_MSG6, ///< Message type 6.
	MSG_TYPE_MSG7, ///< Message type 7.
	MSG_TYPE_MSG8, ///< Message type 8.
	MSG_TYPE_MSG9, ///< Message type 9.
	MSG_TYPE_MSG10, ///< Message type 10.
	MSG_TYPE_MSG11, ///< Message type 11.
	MSG_TYPE_MSG12, ///< Message type 12.
	MSG_TYPE_MSG13, ///< Message type 13.
	MSG_TYPE_MSG14, ///< Message type 14.
	MSG_TYPE_MSG15, ///< Message type 15.
	MSG_TYPE_MSG16, ///< Message type 15.
	MSG_TYPE_END
};

#define SYS_BOA_MSG			MSG_TYPE_MSG2 ///< message ID used in BOA process to communicate with system server.
#define SYS_UPNP_MSG		MSG_TYPE_MSG3 ///< message ID used in UPNP process to communicate with system server.
#define SYS_FTP_MSG			MSG_TYPE_MSG4 ///< message ID used in FTP client to communicate with system server.
#define SYS_SMTP_MSG		MSG_TYPE_MSG5 ///< message ID used in SMTP client to communicate with system server.
#define SYS_BOOT_MSG		MSG_TYPE_MSG6 ///< message ID used in boot_proc to communicate with system server.
#define SYS_SIRP_MSG		MSG_TYPE_MSG7 ///< message ID used in seria protocols process to communicate with system server.

enum {
	SYS_MSG_GET_DHCP_EN, ///< Get DHCP status.
	SYS_MSG_GET_HTTP_PORT, ///< Get http port.
	SYS_MSG_SET_NETMASK, ///< Set new netmask of IP netcam.
	SYS_MSG_GET_SNTP_FQDN, ///< Get SNTP FQDN.
	SYS_MSG_SET_TIMEFORMAT, ///< Set display time format.
	SYS_MSG_GET_DEVICE_TYPE, ///< Get device type.
	SYS_MSG_GET_TITLE, ///< Get IP netcam title.
	SYS_MSG_GET_REGUSR, ///< Get IP netcam title.
	SYS_MSG_GET_OSDTEXT, ///< Get IP netcam title.
	SYS_MSG_GET_MAC, ///< Get ethernet MAC address.
	SYS_MSG_GET_IP, ///< Get IP netcam IP address.
	SYS_MSG_GET_GATEWAY, ///< Get IP netcam gateway.
	SYS_MSG_GET_NETMASK, ///< Get IP netcam netmask.
	SYS_MSG_GET_FTP_SERVER_IP, ///< Get FTP FQDN.
	SYS_MSG_GET_FTP_USERNAME, ///< Get FTP user name.
	SYS_MSG_GET_FTP_PASSWORD, ///< Get FTP user password.
	SYS_MSG_GET_FTP_FOLDNAME, ///< Get FTP foleder.
	SYS_MSG_GET_FTP_IMAGEAMOUNT, ///< Get JPEG count at each FTP connection.
	SYS_MSG_GET_FTP_PID, ///< Get FTP process ID.
	SYS_MSG_GET_FTP_PORT, ///< Get FTP connection port.
	SYS_MSG_GET_DHCP_CONFIG, ///< Get DHCP status.
	//SYS_MSG_GET_UPNP_PORT,
	SYS_MSG_SET_IP, ///< Set static IP address.
	SYS_MSG_SET_REGUSR, ///< Set IP netcam title.
	SYS_MSG_SET_OSDTEXT, ///< Set IP netcam title.
	SYS_MSG_SET_GATEWAY, ///< Set gateway in static IP mode.
	SYS_MSG_SET_DHCPC_ENABLE, ///< Set DHCP enable.
	SYS_MSG_SET_FTP_FQDN, ///< Set FTP FQDN.
	SYS_MSG_SET_FTP_USERNAME, ///< Set FTP username.
	SYS_MSG_SET_FTP_PASSWORD, ///< Set FTP password.
	SYS_MSG_SET_FTP_FOLDNAME, ///< Set FTP folder.
	SYS_MSG_SET_FTP_IMAGEAMOUNT, ///< Set JPEG count at each FTP connection.
	SYS_MSG_SET_FTP_PID, ///< Save FTP process ID.
	SYS_MSG_SET_FTP_PORT, ///< Set FTP port.
	SYS_MSG_GET_DNS, ///< Get DNS address.
	SYS_MSG_SET_DNS, ///< Set DNS address.
	SYS_MSG_SET_HOSTNAME,    ///<set hostname.
	SYS_MSG_SET_SNTP_SERVER_IP, ///< Set SNTP FQDN.
	SYS_MSG_SET_ADVMODE,
	SYS_MSG_SET_M41SFEATURE,
	SYS_MSG_SET_M42SFEATURE, ///< Set Noise Filter.
	SYS_MSG_SET_JPGSFEATURE,
	SYS_MSG_SET_DEMOCFG,
	SYS_MSG_SET_OSDWIN,
	SYS_MSG_SET_OSDWINNUM,
	SYS_MSG_SET_OSDSTREAM,
	SYS_MSG_SET_IMAGE_FORMAT,
	SYS_MSG_SET_RESOLUTION, ///< Set JPEG resolution.
	SYS_MSG_SET_MPEG4_RES, ///< Set MPEG1 resolution.
	SYS_MSG_SET_MPEG42_RES, ///< Set MPEG2 resolution.
	SYS_MSG_SET_LOSTALARM, ///< Enable ethernet lost alarm or not.
	SYS_MSG_SET_SDAENABLE, ///< Enable save file to SD card when alarm happen or not.
	SYS_MSG_SET_FTPAENABLE, ///< Enable send file to FTP server when alarm happen or not.
	SYS_MSG_SET_ALARMDURATION, ///< Set how long will one alarm last.
	SYS_MSG_SET_AVIDURATION, ///< Set length of one AVI file(in seconds).
	SYS_MSG_SET_AVIFORMAT, ///< Set format of AVI file.
	SYS_MSG_SET_FTPFILEFORMAT, ///< Sent which kind of files to FTP.
	SYS_MSG_SET_SDFILEFORMAT, ///< Save which kind of files to SD card.
	SYS_MSG_SET_FTP_RFTPENABLE, ///< Enable send file to FTP server at schedule.
	SYS_MSG_SET_SD_REENABLE, ///< Enable save file to SD card at schedule.
	SYS_MSG_SET_IMAGE_SOURCE, ///< Select NTSC/PAL mode.
	/* Live Video Page */
	/* Video & Image */
	SYS_MSG_SET_TITLE, ///< Set IP netcam title.
	SYS_MSG_SET_VIDEOCODEC_MODE,
	SYS_MSG_SET_VIDEOCODEC_RES, ///< Set video resolution. This operation will take several time.
	SYS_MSG_SET_MPEG41_BITRATE, ///< Set MPEG1 bitrate.
	SYS_MSG_SET_MPEG42_BITRATE, ///< Set MPEG2 bitrate.
	SYS_MSG_SET_JPEG_QUALITY, ///< Set JPEG encode quality.
	SYS_MSG_SET_FRAMERATE1, ///< Set MPEG1 frame rate.
	SYS_MSG_SET_FRAMERATE2, ///< Set MPEG2 frame rate.
	SYS_MSG_SET_FRAMERATE3, ///< Set JPEG frame rate.
	SYS_MSG_SET_RATE_CONTROL1,
	SYS_MSG_SET_RATE_CONTROL2,
	SYS_MSG_SET_DATE_STAMP_ENABLE1,
	SYS_MSG_SET_DATE_STAMP_ENABLE2,
	SYS_MSG_SET_DATE_STAMP_ENABLE3,
	SYS_MSG_SET_TIME_STAMP_ENABLE1,
	SYS_MSG_SET_TIME_STAMP_ENABLE2,
	SYS_MSG_SET_TIME_STAMP_ENABLE3,
	SYS_MSG_SET_LOGO_ENABLE1,
	SYS_MSG_SET_LOGO_ENABLE2,
	SYS_MSG_SET_LOGO_ENABLE3,
	SYS_MSG_SET_LOGO_POSITION1,
	SYS_MSG_SET_LOGO_POSITION2,
	SYS_MSG_SET_LOGO_POSITION3,
	SYS_MSG_SET_TEXT_ENABLE1,
	SYS_MSG_SET_TEXT_ENABLE2,
	SYS_MSG_SET_TEXT_ENABLE3,
	SYS_MSG_SET_TEXT_POSITION1,
	SYS_MSG_SET_TEXT_POSITION2,
	SYS_MSG_SET_TEXT_POSITION3,
	SYS_MSG_SET_OVERLAY_TEXT1,
	SYS_MSG_SET_OVERLAY_TEXT2,
	SYS_MSG_SET_OVERLAY_TEXT3,
	SYS_MSG_SET_DETAIL_INFO1,
	SYS_MSG_SET_DETAIL_INFO2,
	SYS_MSG_SET_DETAIL_INFO3,
	SYS_MSG_SET_LOCAL_DISPLAY,
	SYS_MSG_SET_MIRROR, ///< Set Rotation.

	/* Video advanced */
	SYS_MSG_SET_IPRATIO1,
	SYS_MSG_SET_IPRATIO2,
	SYS_MSG_SET_IPRATIO3,
	SYS_MSG_SET_FORCE_IFRAME1,
	SYS_MSG_SET_FORCE_IFRAME2,
	SYS_MSG_SET_FORCE_IFRAME3,
	SYS_MSG_SET_QPINIT1,
	SYS_MSG_SET_QPINIT2,
	SYS_MSG_SET_QPINIT3,
	SYS_MSG_SET_QPMIN1,
	SYS_MSG_SET_QPMIN2,
	SYS_MSG_SET_QPMIN3,
	SYS_MSG_SET_QPMAX1,
	SYS_MSG_SET_QPMAX2,
	SYS_MSG_SET_QPMAX3,
	SYS_MSG_SET_MECONFIG1,
	SYS_MSG_SET_MECONFIG2,
	SYS_MSG_SET_MECONFIG3,
	SYS_MSG_SET_PACKET_SIZE1,
	SYS_MSG_SET_PACKET_SIZE2,
	SYS_MSG_SET_PACKET_SIZE3,
	SYS_MSG_SET_ROI_ENABLE1,
	SYS_MSG_SET_ROI_ENABLE2,
	SYS_MSG_SET_ROI_ENABLE3,
	SYS_MSG_SET_STR1X1,
	SYS_MSG_SET_STR1Y1,
	SYS_MSG_SET_STR1W1,
	SYS_MSG_SET_STR1H1,
	SYS_MSG_SET_STR1X2,
	SYS_MSG_SET_STR1Y2,
	SYS_MSG_SET_STR1W2,
	SYS_MSG_SET_STR1H2,
	SYS_MSG_SET_STR1X3,
	SYS_MSG_SET_STR1Y3,
	SYS_MSG_SET_STR1W3,
	SYS_MSG_SET_STR1H3,
	SYS_MSG_SET_STR2X1,
	SYS_MSG_SET_STR2Y1,
	SYS_MSG_SET_STR2W1,
	SYS_MSG_SET_STR2H1,
	SYS_MSG_SET_STR2X2,
	SYS_MSG_SET_STR2Y2,
	SYS_MSG_SET_STR2W2,
	SYS_MSG_SET_STR2H2,
	SYS_MSG_SET_STR2X3,
	SYS_MSG_SET_STR2Y3,
	SYS_MSG_SET_STR2W3,
	SYS_MSG_SET_STR2H3,
	SYS_MSG_SET_STR3X1,
	SYS_MSG_SET_STR3Y1,
	SYS_MSG_SET_STR3W1,
	SYS_MSG_SET_STR3H1,
	SYS_MSG_SET_STR3X2,
	SYS_MSG_SET_STR3Y2,
	SYS_MSG_SET_STR3W2,
	SYS_MSG_SET_STR3H2,
	SYS_MSG_SET_STR3X3,
	SYS_MSG_SET_STR3Y3,
	SYS_MSG_SET_STR3W3,
	SYS_MSG_SET_STR3H3,
	/* Video Analytics */
	SYS_MSG_SET_MOTIONENABLE, ///< Enable motion detection or not.
	SYS_MSG_SET_MOTIONCENABLE, ///< Use customer define or normal mode.
	SYS_MSG_SET_MOTIONLEVEL, ///< Set motion level in normal mode.
	SYS_MSG_SET_MOTIONCVALUE, ///< Set customer value at customer mode.
	SYS_MSG_SET_MOTIONBLOCK, ///< Define motion blocks.
//	SYS_MSG_SET_FDETECT,
//	SYS_MSG_SET_FDX,
//	SYS_MSG_SET_FDY,
//	SYS_MSG_SET_FDW,
//	SYS_MSG_SET_FDH,
//	SYS_MSG_SET_FDCONF_LEVEL,
//	SYS_MSG_SET_FD_DIRECTION,
//	SYS_MSG_SET_FRECOGNITION,
//	SYS_MSG_SET_FRCONF_LEVEL,
//	SYS_MSG_SET_FR_DATABASE,
//	SYS_MSG_SET_PRIVACY_MASK,
//	SYS_MSG_SET_MASK_OPTIONS,
	/* Camera Page */
	//SYS_MSG_SET_UILANGUAGE, ///< Set 界面语言.	XQ
	//SYS_MSG_SET_IRCUTAUTO, ///< Set IRCUT工作状态.	XQ
	//SYS_MSG_SET_IRCUTLEAVE, ///< Set IRCUT切换门限值.
	//SYS_MSG_SET_IRIS,		///< Set 自动光圈.
	SYS_MSG_SET_BRIGHTNESS, ///< Set brightness.
	SYS_MSG_SET_CONTRAST, ///< Set contrast.
	SYS_MSG_SET_SATURATION, ///< Set saturation.
	SYS_MSG_SET_SHARPNESS, ///< Set sharpness.
	SYS_MSG_SET_BLC,
	SYS_MSG_SET_BACKLIGHT, ///< Set backlight mode of IP netcam.
	SYS_MSG_SET_EXPOSUREROI,	////XQ 曝光
	SYS_MSG_SET_DYNRANGE,
	SYS_MSG_SET_WHITE_BALANCE, ///< Set whitebalance moe of IP netcam.
	SYS_MSG_SET_DAY_NIGHT, ///< Set day/night mode of IP netcam.
	SYS_MSG_SET_HISTOGRAM,
	SYS_MSG_SET_VSTAB,
	SYS_MSG_SET_LDC,
	SYS_MSG_SET_BINNING, ///< Set binning mode.
	SYS_MSG_SET_2A_SWITCH,
	SYS_MSG_SET_2A_TYPE,
	SYS_MSG_SET_MAXEXPOSURE, ///< Set TV cable connect or not.
	SYS_MSG_SET_MAXGAIN, ///< Set TV cable connect or not.
	SYS_MSG_SET_EXPPRIORITY,
	SYS_MSG_SET_SNF,
	SYS_MSG_SET_TNF,
	/* Audio Page */
	SYS_MSG_SET_AUDIOENABLE, ///< Enable audio send to web site or not.
	SYS_MSG_SET_AUDIOMODE,
	SYS_MSG_SET_AUDIOINVOLUME, ///< Set alarm audio volume
	SYS_MSG_SET_AUDIO_ENCODE,
	SYS_MSG_SET_AUDIO_SAMPLERATE,
	SYS_MSG_SET_AUDIO_BITRATE,
	SYS_MSG_SET_AUDIO_ALARMLEVEL,
	SYS_MSG_SET_AUDIOOUTVOLUME,
	//SYS_MSG_SET_AUDIORECEIVER_ENABLE,
	//SYS_MSG_SET_AUDIOSERVER_IP,
    /* Date Time Page */
	SYS_MSG_SET_DAYLIGHT, ///< Set system time to use daylight or not.
	SYS_MSG_SET_TIMEZONE, ///< Set system time zone.
	SYS_MSG_SET_DATEFORMAT,
	SYS_MSG_SET_TSTAMPFORMAT, ///< Set timestamp format.
	SYS_MSG_SET_DATEPPOS,
	SYS_MSG_SET_TIMEPOS,
    /* Network & Port Page */
	SYS_MSG_SET_MULTICAST,///< Set Multicast fuction enable / disable.
	SYS_MSG_SET_HTTPPORT, ///< set web site port. This operation may take serveral time.
	SYS_MSG_SET_HTTPSPORT,
	SYS_MSG_SCHEDULE_STOP,
	SYS_MSG_SCHEDULE_RESUME,
	//SYS_MSG_SET_GIOINTYPE, ///< Set GIO input alarm triger type(high triger or low triger).
	//SYS_MSG_SET_GIOOUTTYPE, ///< Set GIO output high or low.
	//SYS_MSG_SET_RS485,
	/* Alarm & Storage */
	SYS_MSG_SET_ALARM_ENABLE,
	SYS_MSG_SET_AUDIOALARM,
	SYS_MSG_SET_G_ALARM,
	SYS_MSG_SET_HT_ALARM,
	SYS_MSG_SET_ADC_ALARM,
	SYS_MSG_SET_DIO_ALARM,
	SYS_MSG_SET_DIR_ALARM,
	/* Maintainence */
	SYS_MSG_SET_DEFAULT,
	SYS_MSG_BACKUP_SYSINFO,	//XQ
	SYS_MSG_RESTORE_SYSINFO,	//XQ
	/**********************/
	SYS_MSG_SET_RATE_CONTROL, ///< Set rate control.
	SYS_MSG_SET_IMAGE_DEFAULT, ///< Set all image setting to default.
	SYS_MSG_DO_LOGIN, ///< Do user login.
	SYS_MSG_GET_USER_AUTHORITY, ///< Get user authority by name.
	SYS_MSG_ADD_USER, ///< Add/change user acount.
	SYS_MSG_DEL_USER, ///< Delete user acount.
	SYS_MSG_SET_TSTAMPENABLE, ///< Enable timestamp or not.
	SYS_MSG_QUIT, ///< Make system server quit processing.
	//SYS_MSG_IPNCPTZ, ///< Contorl PTZ.
	//SYS_MSG_INIT_IPNCPTZ,///Init PTZ(4X).
	SYS_MSG_SD_FORMAT, ///< Format SD card.
	SYS_MSG_SD_UNMOUNT, ///< Unmount SD card.
	SYS_MSG_GET_SD_FILENAME,///< Get current recording file in SD card.
	SYS_MSG_DO_BOOT_PROC,///< Tell system server to do something about boot control.

	SYS_MSG_SET_G_THRESHOLD, ///加速度门限.
	SYS_MSG_SET_HTT_THRESHOLD, ///温度门限.
	SYS_MSG_SET_HTH_THRESHOLD, ///湿度门限.
	SYS_MSG_SET_C_THRESHOLD, ///电流门限.
	SYS_MSG_SET_P_THRESHOLD, ///压力门限.
	SYS_MSG_SET_H_THRESHOLD, ///液位门限.
	SYS_MSG_SET_DATA_DURATION, ///数据记录长度.

	SYS_MSG_SET_MEMS_CONFIG,	///传感器采样配置
	SYS_MSG_GET_REAL_DATA,	///传感器采样实时数据
	SYS_MSG_GET_AVG_DATA,	///传感器采样均值数据
	SYS_MSG_GET_CACHE_DATA,	///传感器采样缓存数据
	SYS_MSG_GET_SAVED_DATA_LIST,///传感器采样存储数据列表
	SYS_MSG_GET_SAVED_DATA,	///传感器采样存储数据
	SYS_MSG_GET_LAST_ALARM_LOG,	///最后的报警日志
	SYS_MSG_SET_MANUAL_SNAP,	///人工触发报警
	SYS_MSG_SERIAL_COMM,
	SYS_MSG_SERIAL_CMD,
	/* 添加串口传输功能 junyong 20130913 start*/
	#ifdef ENALBE_TTRAN
	SYS_MSG_READ_SERIAL_DATA,
	SYS_MSG_WRITE_SERIAL_DATA,
	#endif
	/* 添加串口传输功能 junyong 20130913 end*/
	SYS_MSG_WDT_POOLING_ALIVE,	//XQ
	SYS_MSG_MAX
};

#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)

int gMsgQueueId = 0;
int gSysProcId = MSG_TYPE_MSG10;
int gMid = 0;
SemHandl_t hndlDrvSem = NULL;

SemHandl_t MakeSem()
{
	SemHandl_t hndlSem = (SemHandl_t)malloc(sizeof(pthread_mutex_t));
	if(hndlSem == NULL){
    	__E("Not enough memory!!\n");
		return NULL;
	}
	/* Initialize the mutex which protects the global data */
    if( pthread_mutex_init(hndlSem, NULL) != 0 )
    {
    	__E("Sem_Creat init faill!!\n");
		free(hndlSem);
		return NULL;
    }
	return hndlSem;
}

int SemRelease(SemHandl_t hndlSem)
{
	if(hndlSem == NULL){
		__E("SemRelease: Invalid Semaphore handler\n");
		return -1;
	}
	return pthread_mutex_unlock(hndlSem);
}

int SemWait(SemHandl_t hndlSem)
{
	if(hndlSem == NULL){
		__E("SemWait: Invalid Semaphore handler\n");
		return -1;
	}
	return pthread_mutex_lock(hndlSem);
}

int DestroySem(SemHandl_t hndlSem)
{
	if(hndlSem == NULL){
		__E("DestroySem: Invalid Semaphore handler\n");
		return -1;
	}
	pthread_mutex_lock(hndlSem);
	pthread_mutex_unlock(hndlSem);
	if( pthread_mutex_destroy(hndlSem)!= 0 )
	{
		__E("Sem_kill faill!!\n");
	}
	free(hndlSem);
	return 0;
}

struct sockaddr_in sa =
{
sin_family:	PF_INET,
sin_port:	0
};

int pShareMemInit(int key)
{
	int mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | 0660);
	printf("shared memory size %d\n", PROC_MEM_SIZE * MAX_SHARE_PROC);
	if(mid < 0)
		mid = shmget(key, 0, 0);
	printf("shared memory id:%d\n",mid);
	if(mid < 0)
		return -1;
	return mid;
}

void ShareMemWrite(int offset,void *buf, int length)
{
	char *pDst = (char *)shmat(gMid,0,0);
	memcpy(pDst + offset, buf, length);
	shmdt(pDst);
}

int GetNetMacAddress(const char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
	{
		DBG_ERR("net_get_hwaddr: ioctl SIOCGIFHWADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}

in_addr_t GetNetIP(const char *ifname)
{
	struct ifreq ifr;

	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		DBG_ERR("socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
	{
		DBG_ERR("net_get_ip: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}
int SetNetIP(const char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0)
	{
		DBG_ERR("net_set_ifaddr: ioctl SIOCSIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);


	////进行保存
	//int len = sizeof(addr);

	//SYS_MSG_BUF msgbuf;
	//memset(&msgbuf,0,sizeof(msgbuf));
	//msgbuf.Des = MSG_TYPE_MSG1;
	//msgbuf.src = gSysProcId;
	//msgbuf.cmd = SYS_MSG_SET_IP;
	//msgbuf.length = len;
	//msgbuf.offset = (gSysProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
	//SemWait(hndlDrvSem);
	//ShareMemWrite(msgbuf.offset, &addr, len);
	//msgsnd( gMsgQueueId,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	//msgrcv( gMsgQueueId, &msgbuf,sizeof(msgbuf)-sizeof(long), gSysProcId, 0);
	//SemRelease(hndlDrvSem);
	//return msgbuf.length;
	return 0;
}
in_addr_t GetNetmask(const char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0)
	{
		DBG_ERR("net_get_netmask: ioctl SIOCGIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);
	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}
int SetNetMask(const char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0)
	{
		DBG_ERR("net_set_netmask: ioctl SIOCSIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);
	////进行保存
	//int len = sizeof(addr);
	//SYS_MSG_BUF msgbuf;
	//memset(&msgbuf,0,sizeof(msgbuf));
	//msgbuf.Des = MSG_TYPE_MSG1;
	//msgbuf.src = gSysProcId;
	//msgbuf.cmd = SYS_MSG_SET_NETMASK;
	//msgbuf.length = len;
	//msgbuf.offset = (gSysProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
	//SemWait(hndlDrvSem);
	//ShareMemWrite(msgbuf.offset, &addr, len);
	//msgsnd( gMsgQueueId,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	//msgrcv( gMsgQueueId, &msgbuf,sizeof(msgbuf)-sizeof(long), gSysProcId, 0);
	//SemRelease(hndlDrvSem);
	//return msgbuf.length;
	return 0;
}
int NetSearchGateway(char *buf, in_addr_t *gate_addr)
{
	char iface[16] = { 0 };
	in_addr_t dest, gate;
	int iflags;

	sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, (unsigned long *)&dest, (unsigned long *)&gate, &iflags);
	const char *pETH0 = "eth0";
	if ( memcmp(iface,pETH0,strlen(pETH0)) != 0 )
	{
		return -1;
	}
	if ( (iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY) )
	{
		*gate_addr = gate;
		return 0;
	}
	return -1;
}
int NetDelGateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;
	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		DBG_ERR("socket error");
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCDELRT, &rt) < 0)
	{
		DBG_ERR("net_del_gateway: ioctl SIOCDELRT");
		close(skfd);
		return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}
int NetAddGateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;

	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		DBG_ERR("socket error");
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCADDRT, &rt) < 0)
	{
		DBG_ERR("net_add_gateway: ioctl SIOCADDRT");
		close(skfd);
		return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}
in_addr_t NetGetGateway(void)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp)
	{
		DBG_ERR("INET (IPv4) not configured in this system.\n");
		return (INADDR_ANY);
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL)
	{
		if ( NetSearchGateway(buff, &gate_addr) == 0)
		{
			fclose(fp);
			return gate_addr;
		}
	}
	fclose(fp);
	return (INADDR_ANY);
}
int NetSetGateway(in_addr_t addr)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp)
	{
		DBG_ERR("INET (IPv4) not configured in this system.\n");
		return -1;
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL)
	{
		if (NetSearchGateway(buff, &gate_addr) == 0)
		{
			NetDelGateway(gate_addr);
		}
	}
	fclose(fp);
	NetAddGateway(addr);

	////进行保存
	//int len = sizeof(addr);
	//SYS_MSG_BUF msgbuf;
	//memset(&msgbuf,0,sizeof(msgbuf));
	//msgbuf.Des = MSG_TYPE_MSG1;
	//msgbuf.src = gSysProcId;
	//msgbuf.cmd = SYS_MSG_SET_GATEWAY;
	//msgbuf.length = len;
	//msgbuf.offset = (gSysProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
	//SemWait(hndlDrvSem);
	//ShareMemWrite(msgbuf.offset, &addr, len);
	//msgsnd( gMsgQueueId,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	//msgrcv( gMsgQueueId, &msgbuf,sizeof(msgbuf)-sizeof(long), gSysProcId, 0);
	//SemRelease(hndlDrvSem);
	//if ( -1 == nRet || -1 == msgbuf.length )
	//	return -1;
	//return msgbuf.length;
	return 0;
}

int GetNetCardInfo(NetCardInfo &netcardinfo)
{
	memset(&netcardinfo,0,sizeof(netcardinfo));
	//获取IP
	struct in_addr ipaddr;
	ipaddr.s_addr = GetNetIP(NETCARDNAME);
	if ( -1 == (int)ipaddr.s_addr )
	{
		return -1;
	}
	const char *pIP = inet_ntoa(ipaddr);
	memmove(netcardinfo.szIP,pIP,strlen(pIP));

	//获取子网掩码
	struct in_addr Maskaddr;
	Maskaddr.s_addr = GetNetmask(NETCARDNAME);
	if ( -1 == (int)Maskaddr.s_addr )
	{
		return -1;
	}
	const char *pMask = inet_ntoa(Maskaddr);
	memmove(netcardinfo.szMask,pMask,strlen(pMask));

	//获取网关
	struct in_addr gatewayaddr;
	gatewayaddr.s_addr = NetGetGateway();
	if ( -1 == (int)gatewayaddr.s_addr )
	{
		return -1;
	}
	const char *pGateWay = inet_ntoa(gatewayaddr);
	memmove(netcardinfo.szGateway,pGateWay,strlen(pGateWay));
	return 0;
}
int SetNetCardInfo(NetCardInfo &netcardinfo)
{
	//设置IP
	struct in_addr ipaddr;
	ipaddr.s_addr = inet_addr(netcardinfo.szIP);
	int nRet = SetNetIP(NETCARDNAME,ipaddr.s_addr);
	printf("nRet = %d----->\n",nRet);
	if ( -1 == nRet )
	{
		return -1;
	}

	//设置子网掩码
	struct in_addr Maskaddr;
	Maskaddr.s_addr = inet_addr(netcardinfo.szMask);
	nRet = SetNetMask(NETCARDNAME,Maskaddr.s_addr);
	printf("nRet = %d----->\n",nRet);

	if ( -1 == nRet )
	{
		return -1;
	}

	//设置网关
	struct in_addr gatewayaddr;
	gatewayaddr.s_addr = inet_addr(netcardinfo.szGateway);
	nRet = NetSetGateway(gatewayaddr.s_addr);
	printf("nRet = %d----->\n",nRet);

	if ( -1 == nRet )
	{
		return -1;
	}
	return 0;
}

int Msg_Init( int msgKey )
{
	int qid;
	key_t key = msgKey;

	qid = msgget(key,0);
	if(  qid < 0 )
	{
		qid = msgget(key,IPC_CREAT|0666);
		printf("Creat queue id:%d\n",qid);
	}

	printf("queue id:%d\n",qid);

	return qid;
}

int GetSoftwareVer(char *cur_version)
{
	char version[128] = { 0 };
    strcpy(version, "cat /mnt/ipnc/modules/ver/software_version|cut -c12-19");
	FILE *ptr = popen(version, "r");
    if( ptr != NULL)
    {
        if( fgets(cur_version, 32, ptr) != NULL)
        {
            pclose(ptr);
            return 0;
        }
		pclose(ptr);
    }
    return -1;
}
int DevParaRead(struct COMPACKET* pPack)
{
	for(int i=0; i< TOTAL_VI_CHN_NUM;i++)
	{
		memcpy(pPack->body.AddSearchResp.chnPara[i].szChannelID,\
			devicecamparam[i].PlatformDevId,\
			sizeof(pPack->body.AddSearchResp.chnPara[i].szChannelID));
		sprintf(pPack->body.AddSearchResp.chnPara[i].szChannelName,"%s%2d","Camera",i+1);
	}
	nvs::PlatformRegInfo  devicereginfo;
	GetXmlPlatformRegInfo(&devicereginfo);

	//tm 服务器设置；
	memcpy(pPack->body.AddSearchResp.szServerIP,devicereginfo.serverAddr.ip,sizeof(devicereginfo.serverAddr.ip));
	memcpy(pPack->body.AddSearchResp.szServerID,devicereginfo.serverAddr.id,sizeof(pPack->body.AddSearchResp.szServerID));
	pPack->body.AddSearchResp.uServerPort = devicereginfo.serverAddr.port;
	return 0;
}

int DevParaWrite(struct COMPACKET* pPack)
{
	for(int i=0; i< TOTAL_VI_CHN_NUM;i++)
	{
		memcpy(devicecamparam[i].PlatformDevId,\
			pPack->body.AddSetReq.chnPara[i].szChannelID,\
			sizeof(pPack->body.AddSetReq.chnPara[i].szChannelID));
		memcpy(devicecamparam[i].ChnName,\
			pPack->body.AddSetReq.chnPara[i].szChannelName,\
			sizeof(pPack->body.AddSetReq.chnPara[i].szChannelName));
		printf("------>[devicecamparam %d]. [PlatformDevId %s]  [ CHnname %s]\n ",
			i,devicecamparam[i].PlatformDevId,devicecamparam[i].ChnName);
	}
	SetXmlCamParam(CHNNUM,devicecamparam);

	nvs::PlatformRegInfo  devicereginfo;
	GetXmlPlatformRegInfo(&devicereginfo);

	//tm 服务器设置；
	memcpy(devicereginfo.serverAddr.ip,pPack->body.AddSetReq.szServerIP,sizeof(devicereginfo.serverAddr.ip));
	memcpy(devicereginfo.serverAddr.id,pPack->body.AddSetReq.szServerID,sizeof(pPack->body.AddSetReq.szServerID));
	devicereginfo.serverAddr.port = pPack->body.AddSetReq.uServerPort;
	printf("-------------->[server Ip %s]. [server ID %s]  [ port %d]\n ",
		devicereginfo.serverAddr.ip,devicereginfo.serverAddr.id,devicereginfo.serverAddr.port);
	printf("-------------->[server Ip %s]. [server ID %s]  [ port %d]\n ",
		devicereginfo.serverAddr.ip,devicereginfo.serverAddr.id,devicereginfo.serverAddr.port);

	SetXmlPlatformRegInfo(&devicereginfo);

	return 0;
}


#endif


//#define ADDETH0ROUTE	"route add default gw 192.168.0.1"
#define ADDETH0ROUTE	"route add 192.168.0.0 dev eth0 gw 192.168.0.1"
#define DELETH0ROUTE	"route del default gw 192.168.0.1"

void* DevSearchEntry(void*)
{
#ifdef _WIN32
#else
/*
	gSysProcId = MSG_TYPE_MSG10;
	gMsgQueueId = Msg_Init(SYS_MSG_KEY);
	gMid = pShareMemInit(SYS_MSG_KEY);
	printf("Control MsgQueueID = %d gMid=%d\n",gMsgQueueId,gMid);
	hndlDrvSem = MakeSem();
*/
	SOCKET sockListener;
	SOCKADDR_IN sin,saClient;                          //设置两个地址，sin用来绑定
	SOCKADDR_IN broascast;
	size_t nSize;
	int nbSize;
	sockListener=socket(AF_INET, SOCK_DGRAM,0);
	int bBroadCast = 1;
	setsockopt( sockListener,SOL_SOCKET,SO_REUSEADDR |SO_BROADCAST,&bBroadCast,sizeof(bBroadCast));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(COMM_PORT);             //发送端使用的发送端口，可以根据需要更改
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	broascast.sin_family = AF_INET;
	broascast.sin_port = htons(COMM_PORT);             //发送端使用的发送端口，可以根据需要更改
	broascast.sin_addr.s_addr = inet_addr("255.255.255.255");

	if(bind( sockListener, (SOCKADDR  *)&sin, sizeof(sin))!=0)
	{
		DBG_ERR("Can't bind socket to local port!Program stop.");//初始化失败返回-1
		return NULL;
	}
	int nHeadrSize = sizeof(FrameHead);
	nSize = sizeof ( SOCKADDR_IN );
	NetCardInfo netcardinfo;

	unsigned char uMAC[32] = { 0 };
	char szSelfMAC[32] = { 0 };
	if ( GetNetMacAddress(NETCARDNAME,uMAC) != -1 )
	{
		sprintf(szSelfMAC,"%02X-%02X-%02X-%02X-%02X-%02X",uMAC[0],
		uMAC[1],uMAC[2],uMAC[3],uMAC[4],uMAC[5] );
	}
	COMPACKET compacket;
	int PacketLen = sizeof(COMPACKET);
	while(1)
	{
		memset(&compacket,0,PacketLen);
		if((nbSize=recvfrom (sockListener,(char *)&compacket,PacketLen,0,
			(SOCKADDR  *) &saClient,&nSize))==SOCKET_ERROR) //若接收失败则提示错误
		{
			DBG_ERR("Recive Error");
			break;
		}
		switch ( compacket.header.bFrameType )
		{
			case ADDRESSINFOSEARCH_REQ:
			{
				//地址信息查询请求
				printf("Get Address Info Req!\r\n");
				if ( GetNetCardInfo(netcardinfo) != -1 )
				{
					printf("Get-->IP:%s mask:%s GateWay:%s MAC:%s\n",netcardinfo.szIP,netcardinfo.szMask,netcardinfo.szGateway,szSelfMAC);
				}

				int nBodyLen = sizeof(HHAddSearchResp);
				compacket.header.bFrameType = ADDRESSINFOSEARCH_RESP;
				compacket.header.uFrameTotalLen = nHeadrSize+nBodyLen;

				memmove(compacket.body.AddSearchResp.szMAC,szSelfMAC,strlen(szSelfMAC));
				memmove(compacket.body.AddSearchResp.szIP,netcardinfo.szIP,strlen(netcardinfo.szIP));
				memmove(compacket.body.AddSearchResp.szMask,netcardinfo.szMask,strlen(netcardinfo.szMask));
				memmove(compacket.body.AddSearchResp.szGateway,netcardinfo.szGateway,strlen(netcardinfo.szGateway));
				strcpy(compacket.body.AddSearchResp.szSoftWareVer,"Ver 2017-02-06 09:20:00");
				compacket.body.AddSearchResp.bChannelNum = TOTAL_VI_CHN_NUM;
				DevParaRead(&compacket);
				if ( SOCKET_ERROR == sendto ( sockListener,(const char *)&compacket, compacket.header.uFrameTotalLen, 0,
					(SOCKADDR *) &broascast,sizeof ( SOCKADDR_IN )) )
				{
					DBG_ERR("sendto SOCKETERROR");
				}
			}
			break;
			case ADDRESSINFOSET_REQ:
			{
				//地址信息设定请求
				//如果设定的MAC和自己不匹配，则不处理
				if ( 0 == memcmp(compacket.body.AddSetReq.szMAC,szSelfMAC,strlen(szSelfMAC)) )
				{
					printf("Set Address Info Req!\r\n");
					memset(&netcardinfo,0,sizeof(NetCardInfo));
					memmove(netcardinfo.szMAC,compacket.body.AddSetReq.szMAC,strlen(compacket.body.AddSetReq.szMAC));
					memmove(netcardinfo.szIP,compacket.body.AddSetReq.szIP,strlen(compacket.body.AddSetReq.szIP));
					memmove(netcardinfo.szMask,compacket.body.AddSetReq.szMask,strlen(compacket.body.AddSetReq.szMask));
					memmove(netcardinfo.szGateway,compacket.body.AddSetReq.szGateway,strlen(compacket.body.AddSetReq.szGateway));

					DevParaWrite(&compacket);

					int nBodyLen = sizeof(HHAddSetResp);
					memset(&compacket.body,0,sizeof(compacket.body));

					compacket.header.bFrameType = ADDRESSINFOSET_RESP;
					compacket.header.uFrameTotalLen = nHeadrSize+nBodyLen;
					memmove(compacket.body.AddSetResp.szMAC,szSelfMAC,strlen(szSelfMAC));
					if ( -1 == SetNetCardInfo(netcardinfo) )
					{
						//设置失败
						compacket.body.AddSetResp.uRet = 0;
					}
					else
					{
						//设置成功
						printf("Set-->IP:%s mask:%s GateWay:%s MAC:%s\n",netcardinfo.szIP,netcardinfo.szMask,netcardinfo.szGateway,szSelfMAC);
						memmove(compacket.body.AddSetResp.szIP,netcardinfo.szIP,strlen(netcardinfo.szIP));
						memmove(compacket.body.AddSetResp.szMask,netcardinfo.szMask,strlen(netcardinfo.szMask));
						memmove(compacket.body.AddSetResp.szGateway,netcardinfo.szGateway,strlen(netcardinfo.szGateway));
						compacket.body.AddSetResp.uRet = 1;
					}
					if ( SOCKET_ERROR == sendto ( sockListener,(const char *)&compacket, compacket.header.uFrameTotalLen, 0,
						(SOCKADDR *) &broascast,sizeof ( SOCKADDR_IN )) )
					{
						DBG_ERR("sendto SOCKETERROR");
					}
					reLoginDvs(NULL);
				}
			}
			break;
		}
		sleep(1);
		//TM_USleep(1,0);
	}
#endif
	return NULL;
}
