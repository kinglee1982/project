/******************************************************************************

                  版权所有 (C), 2001-2015, 郑州天迈科技有限公司

*******************************************************************************
  文 件 名   : av_socket.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2015年1月26日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2015年1月26日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#include "av_public_head.h"
#include "av_video_init.h"
#include "av_common_31.h"
#include "av_video_proc.h"
#include "av_split.h"
#include "data_buf.h"
#include "av_socket.h"
#include "av_crc16.h"
#include "av_video_proc.h"
#include "av_hddfsm.h"
#include "av_osd.h"
#include "Nvs.h"
#include "28181_AV_API.h"
HI_BOOL bDevLoginRecvOk = HI_FALSE;
HI_BOOL bDevSvrRecvOk   = HI_FALSE;
HI_BOOL bDevStatRecvOk = HI_FALSE;
/*****************************************************************************
定义AV通信全局BUF对象指针,用于收发数据
*****************************************************************************/
static DataBuf *gCycleBuf = NULL;
/*****************************************************************************
定义AV通信全局SOCKET
*****************************************************************************/
static HI_S32 gSocketFd = -1;
//ACC状态
static HI_U8  g_u8AccEnable = 1;
//硬件状态
static AV_HARD_STA g_HardWareState = {0};
static AV_TEMP_STA g_HardWareTemp = {0};
static HW_TEMP_ABNORMAL_T g_HardWareTempSta = {0};
TM_APP_DEVSTAT_S *g_DevErrInfoList = NULL;
/*****************************************************************************
定义DOMAIN通信处理INVOKE函数
注:可以把这个结构分成二级结构来处理
*****************************************************************************/
MSG_MAP_T gMsgMap[] =
{

    /*消息源ID*/	/*命令ID*/				/*消息处理函数*/
    {ID_UI, 		AV_TAKE_PHOTO,			DOMAIN_Snapshot},	//抓拍
    {ID_UI, 		AV_SWITCH_SCREEN,		DOMAIN_Preview},	//本地预览
    {ID_UI, 		UI_REC_DISPLAY,			DOMAIN_RecDisplay},	//查询月录像
    {ID_UI, 		UI_REC_QUREY,			DOMAIN_RecQuery},	//查询日录像
    {ID_UI, 		AV_DEC_CHN_SWITCH,		DOMAIN_DecChnSwitch},//切换回放通道
    {ID_UI, 		AV_OPEN_L_PLAY_BACK, 	DOMAIN_StartLPB},	 //开启本地回放
    {ID_UI, 		AV_CLOSE_L_PLAY_BACK,	DOMAIN_StopLPB},//关闭本地回放
    {ID_UI, 		AV_PLAY_CTRL, 		    DOMAIN_localPlayCtrlStat},	//开锁硬盘
    {ID_UI, 		AV_PLAY_CTRL_SEEK, 		DOMAIN_localPlayCtrl_Seek},	//开锁硬盘
    //备份相关
    {ID_UI, 		AV_START_BK, 	        DOMAIN_StartLBK},	 //开启备份
    {ID_UI,         AV_CANCLE_BK,           DOMAIN_StopLBK},
    {ID_LOCAL, 		AV_START_BK,	        DOMAIN_SndUIStartLBK},
    {ID_LOCAL,      AV_BK_STAT,             DOMAIN_SndUiBkStat},    /*发送备份状态到UI*/
    {ID_UI, 		AV_FORMAT_DISK, 		DOMAIN_FormatDisk},	//格式化硬盘
    {ID_UI, 		AV_FORMAT_DISK_END,     DOMAIN_FormatDiskEnd},	//格式化硬盘结束
    {ID_UI,         AV_UMOUNT_DISK,         DOMAIN_RebootSystem},//硬盘开锁事件
    {ID_UI, 		UI_AV_LOGIN,		    DOMAIN_RcvUiLoginRsp},	//查询与服务器连接状态
    {ID_UI, 		AV_OPEN_VOIP,			DOMAIN_RcvUIStartVOIP}, //开启VOIP
    {ID_UI, 		AV_CLOSE_VOIP,			DOMAIN_RcvUIStopVOIP}, 	//关闭VOIP
    {ID_UI,         AV_VOIP_DIALING,        DOMAIN_RcvUIStartDialing},
    {ID_UI, 		AV_DEVERR_RSP, 		    DOMAIN_RcvUiDevStatRsp},
    {ID_LOCAL, 		AV_DEVERR_STAT,	        DOMAIN_SendDevErrStat},
    {ID_UI, 		AV_HDD_COND,		    DOMAIN_SetUiOsd},
#if 0
    {ID_UI, 		AV_OPEN_REC,			DOMAIN_OpenRec},	//打开录像
    {ID_UI, 		AV_CLOSE_REC,			DOMAIN_CloseRec},	//关闭录像
    /* BEGIN: Added by mabuyun, 2015/9/14  DES:添加IP对讲功能  PN:20150902 */

    /* END:   Added by mabuyun, 2015/9/14   PN:20150902 */
    {ID_UI, 		AV_SET_SUB_PARA,		DOMAIN_SetSubPara}, //设置子码流参数
    {ID_UI, 		AV_GET_SUB_PARA,		DOMAIN_GetSubPara}, //查询子码流参数
    /* BEGIN: Modified by mabuyun, 2015/9/6  DES:添加IP对讲功能   PN:20150902 */
    //{ID_UI, 		AV_GET_NET_STAT,		DOMAIN_GetNetStat},	//查询与服务器连接状态
    {ID_UI, 		UI_AV_LOGIN,		    DOMAIN_RcvUiLoginRsp},	//查询与服务器连接状态
    /* END:   Modified by mabuyun, 2015/9/6   PN:20150902 */
    {ID_UI, 		AV_SET_REC_CHN,			DOMAIN_SetRecChn},	 // 设置录像通道
//	{ID_UI, 		AV_GET_REC_CHN,			DOMAIN_GetRecChn},	 // 获取录像通道
    {ID_UI, 		AV_ALARM_EVENT,			NULL},	             // 报警事件通知(为了报警录像)
    {ID_UI, 		AV_OSD_SET,		        DOMAIN_ExtOsdSet},	//扩展osd设置
    {ID_UI, 		UI_AV_VER,		        DOMAIN_GetAvVersion},	    //获取av版本
    {ID_UI, 		AV_BUS_ID,		        DOMAIN_OsdSet},	    //osd设置
    {ID_UI, 		AV_HDD_COND,		    DOMAIN_SetUiOsd},




    {ID_UI, 		AV_UNLOCK_DISK, 		DOMAIN_UnlockKey},	//开锁硬盘
    {ID_UI, 		AV_DEVERR_RSP, 		    DOMAIN_RcvUiDevStatRsp},	//开锁硬盘
    {ID_BZQ,		NULL,					NULL},
    {ID_BZQ, 		AV_VOIP_CTRLAOAI,       DOMAIN_RcvBZQCtrlVOIP},	//收到报站器VOIP应答
#endif
#ifdef PLT_PARA_BK
    {ID_UI, 		AV_PLT_ADDRESS,			DOMAIN_RcvUIQueryPlatInfo}, //查询AV连接的视频服务器
#endif
#ifdef PLT_PARA_BK
    {ID_LOCAL, 		AV_PLT_ADDRESS,         DOMAIN_SndUIPlatInfo},
    {ID_LOCAL, 		SCH_SET_PLT_INFO,       DOMAIN_GetPlatformInfo},
#endif
    {ID_LOCAL, 		AV_GET_DISK_STAT,		DOMAIN_LocalDiskStat},
    {ID_UI, 		AV_GET_DISK_STAT,		DOMAIN_DiskStat},	//硬盘状态

    {ID_UI, 		AV_GET_VO_PARA, 		DOMAIN_GetVoPara},	//获取VO参数
    {ID_UI, 		AV_SET_VO_PARA, 		DOMAIN_SetVoPara},	//设置VO参数
    {ID_UI, 		AV_GET_VI_PARA, 		DOMAIN_GetViPara},	//获取VI参数
    {ID_UI, 		AV_SET_VI_PARA, 		DOMAIN_SetViPara},	//设置VI参数
    {ID_UI,         AV_SET_CHN_VI_PARA,     DOMAIN_SetChnViPara},
//	{ID_PHONE,		NULL,					NULL},

    {ID_LOCAL,      BASIC_GPIO_CMD,             DOMAIN_SndGpioReqToBasic},
    {ID_LOCAL, 		BASIC_HARDWARE_STATE_REQ,	DOMAIN_SndRequstToBasic},
    {ID_BASIC,		BASIC_HARDWARE_STATE_RSP,	DOMAIN_HandleHardWareSta},
    {ID_BASIC,      BASIC_TEMP_ACK,             DOMAIN_RecvHardWareTemp},
    {ID_3GGATE,		GATE_MOUDLE_REG_CMD,	DOMAIN_RecvRegAckFromGate},
//	{ID_MONITOR, 	AV_HEART_BEAT,			NULL},	//连接维持命令

    {ID_PARAM,      PARA_CMD_GET_PARA_COM,  DOMAIN_GetCommonPara},
    {ID_LOCAL, 		PARA_CMD_GET_PARA_COM,	DOMAIN_SndRequstToPara},
    {ID_PARAM,		AV_UPDATE_PARA,		    DOMAIN_UpdatePara},	//更新抓拍通道，默认视频通道，录像标记

    {ID_UI,      AV_HARDWARE_STAT_REQUEST,  DOMAIN_QueryDeviceState},  //单片机获取的设备状态
//	{ID_MCU,		NULL,					NULL},
    {ID_MCU,		AV_SET_IP,				NULL},  //设置ip地址+端口+通话时长+车辆编号

    {ID_SRV,		AV_CLOSE_VOIP,			NULL},  //关闭VOIP
    {ID_SRV,		AV_OPEN_VOIP,			NULL},	//打开VOIP
    {ID_SRV,		AV_CLOSE_R_PREVIEW,		NULL},	//关闭远程预览
    {ID_SRV,		AV_OPEN_R_PREVIEW,		NULL},	//打开远程预览
    {ID_SRV,		AV_SEARCH_REC,			NULL},	//搜索录像
    {ID_SRV,		AV_CLOSE_R_PLAYBACK,	NULL},	//关闭远程录像回放
    {ID_SRV,		AV_OPEN_R_PLAYBACK,		NULL},	//打开远程录像回放
    {ID_SRV,		AV_OPEN_R_4PLAYBACK,	NULL},	//打开远程4通道录像回放
    {ID_LOCAL, 		UI_REC_RESP, 			DOMAIN_RecTimeResp},
    {ID_LOCAL, 		AV_VOIP_CTRLAOAI,       	DOMAIN_SndBZQCtrlVOIP},	//给报站器VOIP状态
    {ID_LOCAL, 		AV_CLOSE_L_PLAY_BACK,	DOMAIN_StopLPB},
    {ID_LOCAL,		AV_LPB_AO_SWITCH,		DOMAIN_SwitchLPBAo},
    {ID_LOCAL,		SCH_DEV_SVR_STAT,		DOMAIN_SendDevSvrStat},
    {ID_LOCAL, 		AV_PLAYBACK_OK,	DOMAIN_LPBOK},//关闭本地回放
    {ID_SCH, 		SCH_DEV_SVR_STAT,	DOMAIN_RcvSvrDevStatRsp},
    {ID_SCH,        SCH_GET_OSD_DATA,       DOMAIN_GetOsdData},		/*osd*/
    {ID_SCH,		SCH_SET_PLT_INFO,	    DOMAIN_RcvSchPlatInfoRsp},  /* 视频服务器相关参数 */
    {ID_UI, 		AV_FUNC_CONFIG,		    DOMAIN_GetFuncConfig},
    {ID_LOCAL, 		AV_FUNC_CONFIG,		    DOMAIN_GetFuncConfig},
#ifdef JHY_SDK
    {ID_BASIC,		BASIC_IO_SIG_NOTIFY,	DOMAIN_HandleIoSingle},
    {ID_SCH, 		SCH_KL_STA_INFO,	    DOMAIN_RcvSchStaInfo},
    {ID_SCH,        SCH_KL_STA_RST_NUM,     DOMAIN_RetsetKlNum},		/*osd*/
    {ID_LOCAL, 		SCH_KL_STA_TOTAL_NUM,   DOMAIN_SndKeliuInfoToSch},
    {ID_LOCAL,      SCH_KL_RTINFO_RA,   DOMAIN_SndKlRealTimeInfoToSch},
    {ID_UI, 		AV_JHY_PARA_SET,		    DOMAIN_SetCameraPara},
    {ID_UI, 		AV_JHY_PARA_GET,		    DOMAIN_GetCameraPara},
    {ID_UI, 		AV_JHY_PARA_LEARN_BK,		    DOMAIN_LearnBKGround},
    {ID_UI, 		AV_JHY_PARA_UPDATE_BIN,		    DOMAIN_UpdateCameraPara},
    {ID_UI, 		AV_JHY_PARA_GET_CAM_ID,		    DOMAIN_GetCameraId},
#endif
    {ID_LOCAL, 		UI_AV_LOGIN,            DOMAIN_SndSVRLoginStat},	//给UI发送注册状态
    {ID_LOCAL, 		SCH_RSP_PLT_INFO,          DOMAIN_SndSchPlatInfoReq},
    {ID_LOCAL, 		AV_OPEN_VOIP,           DOMAIN_SndUIOpenVOIP},
    {ID_LOCAL, 		AV_CLOSE_VOIP,          DOMAIN_SndUIStopVOIP},

#if 0
    {ID_SCH,		SCH_ALARM_TRIGGER,	    DOMAIN_McuAlarmTrigger},  /*mcu alarm trigger*/




    /*以下消息为LOCAL  AV 主动给别的模块发送消息的接口 */
    {ID_LOCAL, 		AV_CLOSE_L_PLAY_BACK,	DOMAIN_StopLPB},
    {ID_LOCAL, 		AV_START_BK,	        DOMAIN_SndUIStartLBK},
    //{ID_LOCAL, 		AV_OPEN_L_PLAY_BACK,	DOMAIN_SndUIStartLBK},  /*U盘导录像回应UI*/
    //{ID_LOCAL, 		AV_START_BK,	        DOMAIN_SndUIStartLBK},  /*U盘导录像回应UI*/

    {ID_LOCAL,      AV_BK_STAT,             DOMAIN_SndUiBkStat},    /*发送备份状态到UI*/
    {ID_LOCAL,		AV_LPB_AO_SWITCH,		DOMAIN_SwitchLPBAo},	//LPB提示音输出开关

    {ID_LOCAL,		SCH_ALARM_TRIGGER,		DOMAIN_SndMcuAlarmRsp},	//LPB提示音输出开关
    {ID_LOCAL, 		AV_PLAY_CTRL, 		    DOMAIN_SndUiLPBCtrlStat},	//开锁硬盘

    {ID_LOCAL, 		MCU_SND_DISKLED_DATA,   DOMAIN_SndMcuDiskLed},	//开锁硬盘

    /* BEGIN: Added by mabuyun, 2015/9/6  DES:添加IP对讲功能  PN:20150902 */
    {ID_LOCAL, 		AV_VOIP_CTRLAOAI,       DOMAIN_SndBZQCtrlVOIP},	//给报站器VOIP状态


    {ID_LOCAL, 		SCH_RSP_PLT_INFO,          DOMAIN_SndSchPlatInfoReq},
#endif
    /* END:   Added by mabuyun, 2015/9/6   PN:20150902 */
};


extern HI_VOID HI3531_GetVer(HI_U8 * ver);
/*****************************************************************************
 函 数 名  : DOMAIN_InitCycleBuf
 功能描述  :
 输入参数  :
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID	DOMAIN_InitCycleBuf(HI_VOID)
{
    DataBuf *pCycleBuf = NULL;

    /*初始化循环队列*/
    pCycleBuf = new DataBuf((char *)"####", 4, DOMAIN_BUF_LEN);
    DOMAIN_SetCycleBuf(pCycleBuf);
}


/*****************************************************************************
 函 数 名  : DOMAIN_GetMsgFunc
 功能描述  :
 输入参数  : HI_S32 s32CmdId
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
MSG_PF DOMAIN_GetMsgFunc(HI_S32 s32MsgId, HI_S32 s32CmdId)
{
    HI_S32 i = 0;
    HI_S32 s32TotalNum = sizeof(gMsgMap) / sizeof(MSG_MAP_T);

    for (; i < s32TotalNum; i++)
    {
        if ((gMsgMap[i].s32MsgId == s32MsgId) && (gMsgMap[i].s32CmdId == s32CmdId))
        {
            return gMsgMap[i].pMsgFunc;
        }
    }

    return NULL;
}



/*****************************************************************************
 函 数 名  : DOMAIN_SetSocket
 功能描述  :
 输入参数  : HI_S32 Socketfd
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID DOMAIN_SetSocket(HI_S32 socketFd)
{
    gSocketFd = socketFd;
    return;
}

/*****************************************************************************
 函 数 名  : DOMAIN_GetSocket
 功能描述  :
 输入参数  : HI_S32 Socketfd
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID DOMAIN_GetSocket(HI_S32 *pSocketFd)
{
    if (NULL == pSocketFd)
    {
        return;
    }

    *pSocketFd = gSocketFd;
    return;
}


/*****************************************************************************
 函 数 名  : DOMAIN_SetCycleBuf
 功能描述  :
 输入参数  : HI_S32 Socketfd
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID DOMAIN_SetCycleBuf(DataBuf *pCycleBuf)
{
    gCycleBuf = pCycleBuf;
    return;
}


/*****************************************************************************
 函 数 名  : DOMAIN_GetCycleBuf
 功能描述  :
 输入参数  : HI_S32 Socketfd
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID DOMAIN_GetCycleBuf(DataBuf **pCycleBuf)
{
    *pCycleBuf = gCycleBuf;
    return;
}


/*****************************************************************************
 函 数 名  : DOMAIN_PushData2CycleBuf
 功能描述  :
 输入参数  : HI_CHAR *pBuf, HI_S32 s32BufLen
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID DOMAIN_PushData2CycleBuf(HI_CHAR *pBuf, HI_S32 s32BufLen)
{
    DataBuf *pCycleBuf = NULL;
    if (NULL == pBuf)
    {
        return;
    }

    DOMAIN_GetCycleBuf(&pCycleBuf);

    pCycleBuf->PushData(pBuf, s32BufLen);

    return;
}



#if 0


/*****************************************************************************
 函 数 名  : DOMAIN_OpenRec
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_OpenRec(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;

    HDD_SetDiskAbnormal(0);
    HDD_SetHddUnLockEvent(HI_FALSE);

    *ps32SndLen = sizeof(REC_STAT_T);
    pstSndPkt->stRecStat.s32Stat = 0;
    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("UI open record.");

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_CloseRec
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_CloseRec(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    OSD_UI_DATA_ST *pstOsd = NULL;


    s32Ret = s32Ret;
    pstOsd = (OSD_UI_DATA_ST *)pstRcvPkt->buff;
    if (pstOsd->reason == 2)
    {
        DOMAIN_DiskKeyOpen(pRcvPkt, pSndPkt, ps32SndLen);
        return HI_SUCCESS;
    }

    HDD_SetDiskAbnormal(1);

    *ps32SndLen = sizeof(REC_STAT_T);
    pstSndPkt->stRecStat.s32Stat = 0;
    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("UI close record");

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_DiskKeyOpen
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_DiskKeyOpen(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;


    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;

    HDD_SetHddUnLockEvent(HI_TRUE);

    *ps32SndLen = sizeof(REC_STAT_T);
    pstSndPkt->stRecStat.s32Stat = 0;
    pstSndPkt->head.dst_id = ID_UI;

    AV_PRINT("DOMAIN_DiskKeyOpen");

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_CloseVoip
 Description  : 关闭IP对讲
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_CloseVoip(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    //DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("close voip , cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //process close voip
    //AV_CloseVoip();

    *ps32SndLen = 0;

    return HI_SUCCESS;
}
/*****************************************************************************
 Prototype    : DOMAIN_GetSubPara
 Description  : 获取子码流参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_GetSubPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    //DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("set sub para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //process
    //AV_GetSubPara();
    //*ps32SndLen = sizeof();

    return HI_SUCCESS;
}
/*****************************************************************************
 Prototype    : DOMAIN_GetNetStat
 Description  : 获取视频服务器注册状态
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_GetNetStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("get video reg stat, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);
    pstSndPkt = pstSndPkt;

    //process
    //AV_GetNetStat(pstSndPkt->stSvrRegStat.s32Stat);

    *ps32SndLen = sizeof(SVR_REG_STAT_T);

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_SetSubPara
 Description  : 设置子码流参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SetSubPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    //DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("set sub para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);
    //process
    //AV_SetSubPara(pstSndPkt->);

    *ps32SndLen = 0;

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : DOMAIN_DiskStat
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_DiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32MountStat = 0;

    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;

    s32MountStat = HDD_DetectHddMount();

    if (0 == s32MountStat)
    {
        AV_PRINT("hdd no found");
        s32DiskStat = 0;
    }
    else
    {
        //	AV_PRINT("hdd found");
        s32DiskStat = 1;
    }

    *ps32SndLen = sizeof(DISK_STAT_T);
    pstSndPkt->stDiskStat.s32Stat = s32DiskStat;
    pstSndPkt->head.dst_id = ID_UI;

    //AV_PRINT("disk stat(%d)", pstSndPkt->stDiskStat.s32Stat);
    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : DOMAIN_SwitchLPBAo
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_SwitchLPBAo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->stLPBSwitchAo);

    pstSndPkt->stLPBSwitchAo = pstRcvPkt->stLPBSwitchAo;
    pstSndPkt->head.dst_id = ID_BZQ;

    AV_PRINT("switch LPB ao switch+++++++++++++++++++++(0x%02x)", pstRcvPkt->stLPBSwitchAo.cSwitchAo);
    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndMcuAlarmRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    *ps32SndLen = sizeof(HI_S32);

    pstSndPkt->s32Ret = 1;
    pstSndPkt->head.dst_id = ID_SCH;
    AV_PRINT("send mcu alarm rsp now");
    return HI_SUCCESS;
}
#endif
#ifdef PLT_PARA_BK
HI_S32 DOMAIN_RcvUIQueryPlatInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    DevGetPlatInfo((char *)pstSndPkt->buff);

    *ps32SndLen = sizeof(pstSndPkt->stPlatSipInfo);

    AV_PRINT("RcvUIQueryPlatInfo(%s:%d)"
             ,pstSndPkt->stPlatSipInfo.ip
             ,pstSndPkt->stPlatSipInfo.port[0]|(pstSndPkt->stPlatSipInfo.port[1]<<8));
    return HI_SUCCESS;
}
HI_S32 DOMAIN_GetPlatformInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_GetPlatformInfo  now");

    *ps32SndLen = 0;

    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndUIPlatInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    memcpy(pstSndPkt->buff,pstRcvPkt->buff,sizeof(pstRcvPkt->stPlatSipInfo));

    *ps32SndLen = sizeof(pstSndPkt->stPlatSipInfo);

    AV_PRINT("Snd ui Platform info(%s:%d)"
             ,pstSndPkt->stPlatSipInfo.ip
             ,pstSndPkt->stPlatSipInfo.port[1]|(pstSndPkt->stPlatSipInfo.port[0]<<8));

    return HI_SUCCESS;
}
#endif
HI_S32 DOMAIN_LocalDiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{

    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32FindDev = 0;
    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;
    s32FindDev = HDD_FindDev();
    if (HI_FAILURE == s32FindDev)
    {
        AV_PRINT("hdd no found");
        s32DiskStat = 0;
    }
    else
    {
        AV_PRINT("hdd found");
        s32DiskStat = 1;
    }

    *ps32SndLen = sizeof(DISK_STAT_T);
    pstSndPkt->stDiskStat.s32Stat = s32DiskStat;
    pstSndPkt->head.dst_id = ID_UI;

    AV_PRINT("av send disk stat(%d)", pstSndPkt->stDiskStat.s32Stat);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RebootSystem(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
//    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
//    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HDD_SetRebootFlag(HI_TRUE);
    AV_PRINT("DOMAIN_RebootSystem");
    return s32Ret;
}
HI_S32 DOMAIN_DiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32MountStat = 0;
    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;
    s32MountStat = HDD_DetectHddMount();
    if (0 == s32MountStat)
    {
        if(DISK_STAT_FORMATING == HDD_GetDiskFSM())
        {
            AV_PRINT("hdd no found,format");
            s32DiskStat = 2;
        }
        else if(!HDD_GetHddEvent(DISK_EVENT_REC))
        {
            s32DiskStat = 3;
        }
        else
        {
            s32DiskStat = 0;
        }
    }
    else
    {
        s32DiskStat = 1;
    }
    *ps32SndLen = sizeof(HI_S32)*2;
    pstSndPkt->stDiskStat.s32Stat = s32DiskStat;
    memcpy(pstSndPkt->buff,&s32DiskStat,sizeof(HI_S32));
    HI_S32 s32CurrloginStat = (hasLogin()==0?1:0);
    memcpy(pstSndPkt->buff+4,&s32CurrloginStat,sizeof(HI_S32));
    pstSndPkt->head.dst_id = ID_UI;
    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndGpioReqToBasic(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_SndGpioReqToBasic");
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    memcpy(pstSndPkt->buff,pstRcvPkt->buff,sizeof(TGPIOCmd));
    pstSndPkt->head.dst_id = ID_BASIC;
    *ps32SndLen = sizeof(TGPIOCmd);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndRequstToBasic(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_SndRequstToBasic======");
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    pstSndPkt->head.dst_id = ID_BASIC;
    *ps32SndLen = 0;
    return HI_SUCCESS;
}
#ifdef JHY_SDK
HI_S32 DOMAIN_HandleIoSingle(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    char cIoSingle = pstRcvPkt->buff[0];
    //后门
    if (cIoSingle & BUS_BACK_DOOR_OPEN)/*开*/
    {
        AV_PRINT("\033[0;32;31mback door open\033[m");
        SetDoorState(BACK_DOOR,DOOR_OPEN);
    }
    else if (cIoSingle & BUS_BACK_DOOR_CLOSE)/*关*/
    {
        AV_PRINT("\033[0;32;32mback door close\033[m");
        SetDoorState(BACK_DOOR,DOOR_CLOSE);
    }
    /*前门*/
    if (cIoSingle & BUS_FRONT_DOOR_OPEN)/*开*/
    {
        AV_PRINT("\033[0;32;31mfront door open\033[m");
        SetDoorState(FRONT_DOOR,DOOR_OPEN);
    }
    else if (cIoSingle & BUS_FRONT_DOOR_CLOSE)/*关*/
    {
        AV_PRINT("\033[0;32;32mfront door close\033[m");
        SetDoorState(FRONT_DOOR,DOOR_CLOSE);
    }
    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndKlRealTimeInfoToSch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    memcpy(pstSndPkt->buff,pstRecvPkt->buff,sizeof(SCH_KLSTN_REAL_DATA_T));
    pstSndPkt->head.dst_id = ID_SCH;
    *ps32SndLen = sizeof(SCH_KLSTN_REAL_DATA_T);
    return HI_SUCCESS;

}
HI_S32 DOMAIN_SndKeliuInfoToSch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    memcpy(pstSndPkt->buff,pstRecvPkt->buff,sizeof(SCH_KLSTN_DATA_REQ_T));
    pstSndPkt->head.dst_id = ID_SCH;
    *ps32SndLen = sizeof(SCH_KLSTN_DATA_REQ_T);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvSchStaInfo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    SCH_KLSTN_DATA_T *pKlSta = (SCH_KLSTN_DATA_T *)&(pstRecvPkt->buff[0]);
    SCH_KLSTN_DATA_T stLastSta;
    SCH_KLSTN_DATA_T stDoorSta;
    HI3521_JHY_GetStnInfo((HI_VOID*)&stLastSta);
    HI3521_JHY_GetDoorStnInfo((HI_VOID*)&stDoorSta);
    APCCOUNTER stApccount;
    GetCounter(&stApccount);
    //HI_U8 u8StnInfo = pKlSta->in_out_flag & 0x01;
    //HI_BOOL bSendKlData = HI_FALSE;
    AV_PRINT("Recv Stn from schedule.direct:%d sta_num:%d lineno:%s bus_id:%d_%d_%d in_out_flag:%d"
             ,pKlSta->direct,pKlSta->sta_num,pKlSta->line_no,pKlSta->bus_id[0],pKlSta->bus_id[1]
             ,pKlSta->bus_id[2],pKlSta->in_out_flag);
    SCH_KLSTN_DATA_REQ_T stKlReqAck;
    memset(&stKlReqAck,0,sizeof(SCH_KLSTN_DATA_REQ_T));
    stKlReqAck.cmd = SCH_KL_STA_TOTAL_NUM;
    stKlReqAck.bagid = pKlSta->bagid;
    HI_BOOL bOpenDoor = HI3521_JHY_GetDoorTrigger();
    //if(stApccount.doorS[0] == 1 || stApccount.doorS[1] == 1
    //        || stApccount.doorS[2] == 1 || stApccount.doorS[3] == 1)
    //    bOpenDoor = HI_TRUE;
    /* 关门触发 */
    if((pKlSta->in_out_flag&0x04)==0x04)
    {
        //不定位时，关门触发人数上传
        //定位时，需要站序号不同才能触发上传
        if(((pKlSta->in_out_flag&0x08)==0x00)||\
                (((pKlSta->in_out_flag&0x08)==0x08)&&\
                 ((stDoorSta.sta_num!=stLastSta.sta_num)||bOpenDoor)))
        {
            HI3521_JHY_SetDoorTrigger(HI_TRUE);
            AV_PRINT("door close,id=%d,station_flag=%d",pKlSta->sta_num,pKlSta->in_out_flag);
            //if(pKlSta->sta_num!=0x80) //64站场区
            //{
            /* 备份本站信息 */
            stDoorSta.sta_num = pKlSta->sta_num;
            stDoorSta.direct= pKlSta->direct;
            stDoorSta.in_out_flag= pKlSta->in_out_flag;
            // }
            stKlReqAck.direct=pKlSta->direct;
            stKlReqAck.sta_num=pKlSta->sta_num;
            //g_doorent.sendBuf.klnum_sendrevbuf.station_flag=station_flag&0xF3;
            /* 强制改成 0x02 。否则车载机不解析
               车载机只认出站标志，才上传客流
            */
            stKlReqAck.in_out_flag= 0x02;
            stKlReqAck.up_num = stApccount.stationInNum;
            stKlReqAck.down_num = stApccount.stationOutNum;
            HI3521_JHY_SetDoorStnInfo((HI_VOID*)(&stDoorSta));
            DOMAIN_PackageData(ID_SCH, SCH_KL_STA_TOTAL_NUM,(HI_U8*)(&stKlReqAck),sizeof(SCH_KLSTN_DATA_REQ_T));
        }
    }
    /* 进站 */
    else if((pKlSta->in_out_flag&0x03)==0x01)
    {
        pKlSta->in_out_flag &= 0xf3;
        if(pKlSta->in_out_flag==0x01)
        {
            AV_PRINT("enter station,id=%d",pKlSta->sta_num);
        }
        else if(pKlSta->in_out_flag==0x41)
        {
            AV_PRINT("The last station enter,id=%d",pKlSta->sta_num);
        }
        else if(pKlSta->in_out_flag==0x81)
        {
            AV_PRINT("The first station enter,id=%d",pKlSta->sta_num);
        }
        /* 上站没有收到出站信息 */
        if((stLastSta.in_out_flag&0x03)==0x01)
        {
            /*
            	当前收到的站序号与上次的站序号不同
            	而且也没有关门上传过
            	才进行进站上传
            */
            if((pKlSta->sta_num!=stLastSta.sta_num)&&(stLastSta.sta_num!=stDoorSta.sta_num))
            {
                stDoorSta.sta_num=stLastSta.sta_num;
                stDoorSta.direct=stLastSta.direct;
                stDoorSta.in_out_flag=stLastSta.in_out_flag;

                stKlReqAck.direct=stLastSta.direct;
                stKlReqAck.sta_num=stLastSta.sta_num;
                //g_doorent.sendBuf.klnum_sendrevbuf.station_flag=station_flag&0xF3;
                /* 强制改成 0x02 。否则车载机不解析
                   车载机只认出站标志，才上传客流
                */
                stKlReqAck.in_out_flag= 0x02;
                stKlReqAck.up_num = stApccount.stationInNum;
                stKlReqAck.down_num = stApccount.stationOutNum;
                HI3521_JHY_SetDoorStnInfo((HI_VOID*)(&stDoorSta));
                DOMAIN_PackageData(ID_SCH, SCH_KL_STA_TOTAL_NUM,(HI_U8*)(&stKlReqAck),sizeof(SCH_KLSTN_DATA_REQ_T));
                AV_PRINT("%d station enter,lost out station %d",pKlSta->sta_num,stLastSta.sta_num);
            }
        }
        /* 备份本站信息 */
        stLastSta.sta_num = pKlSta->sta_num;
        stLastSta.direct= pKlSta->direct;
        stLastSta.in_out_flag= pKlSta->in_out_flag;
        HI3521_JHY_SetStnInfo((HI_VOID*)(&stLastSta));
    }
    /* 出站 */
    else if((pKlSta->in_out_flag&0x03)==0x02||(pKlSta->in_out_flag&0x03)==0x03)
    {
        pKlSta->in_out_flag &= 0xf3;
        if(pKlSta->in_out_flag==0x02)
        {
            AV_PRINT("leave station,id=%d",pKlSta->sta_num);
        }
        /* BEGIN: Modified by mabuyun, 2014/8/13  DES:修改优化   PN:10000 */
        else if(pKlSta->in_out_flag==0x42)
            /* END:   Modified by mabuyun, 2014/8/13   PN:10000 */
        {
            AV_PRINT("The last station leave,id=%d",pKlSta->sta_num);
        }
        /* BEGIN: Modified by mabuyun, 2014/8/13  DES:修改优化   PN:10000 */
        else if(pKlSta->in_out_flag==0x82)
            /* END:   Modified by mabuyun, 2014/8/13   PN:10000 */
        {
            AV_PRINT("The first station leave,id=%d",pKlSta->sta_num);
        }
        else if(pKlSta->in_out_flag==0x43)
        {
            AV_PRINT("The last station leave,id=%d",pKlSta->sta_num);
        }
        if((bOpenDoor == HI_FALSE)&&(stDoorSta.sta_num==pKlSta->sta_num))
        {
            //已经传过了本站数据
            stLastSta.sta_num=pKlSta->sta_num;
            stLastSta.direct=pKlSta->direct;
            stLastSta.in_out_flag=pKlSta->in_out_flag;
        }
        else
        {
            if(bOpenDoor == HI_TRUE)
            {
                HI3521_JHY_SetDoorTrigger(HI_FALSE);
            }
            /* 备份出站信息 */
            /* 备份本站信息 */
            stLastSta.sta_num=pKlSta->sta_num;
            stLastSta.direct=pKlSta->direct;
            stLastSta.in_out_flag=pKlSta->in_out_flag;
            //if(pKlSta->sta_num!=0x80) //64站场区
            //{
            /* 备份本站信息 */
            stDoorSta.sta_num = pKlSta->sta_num;
            stDoorSta.direct= pKlSta->direct;
            stDoorSta.in_out_flag= pKlSta->in_out_flag;
            // }
            /* 出站，发送人数统计数据 */
            stKlReqAck.direct=stLastSta.direct;
            stKlReqAck.sta_num=stLastSta.sta_num;
            stKlReqAck.in_out_flag= stLastSta.in_out_flag;
            stKlReqAck.up_num = stApccount.stationInNum;
            stKlReqAck.down_num = stApccount.stationOutNum;
            AV_PRINT("send station out ID:%d",stLastSta.sta_num);
            DOMAIN_PackageData(ID_SCH, SCH_KL_STA_TOTAL_NUM,(HI_U8*)(&stKlReqAck),sizeof(SCH_KLSTN_DATA_REQ_T));
        }
        HI3521_JHY_SetStnInfo((HI_VOID*)(&stLastSta));
    }
    *ps32SndLen = 0;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RetsetKlNum(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    APCCOUNTER stApccount;
    GetCounter(&stApccount);
    AV_PRINT("Reset Kl Info(IN:%d %d %d %d OUT:%d %d %d %d DOOR:%d %d %d %d STA:%d %d)"
             ,stApccount.inNum[0],stApccount.inNum[1],stApccount.inNum[2],stApccount.inNum[3]
             ,stApccount.outNum[0],stApccount.outNum[1],stApccount.outNum[2],stApccount.outNum[3]
             ,stApccount.doorS[0],stApccount.doorS[1],stApccount.doorS[2],stApccount.doorS[3]
             ,stApccount.stationInNum,stApccount.stationOutNum);
    CounterReset(FRONT_DOOR);
    CounterReset(BACK_DOOR);
    *ps32SndLen = 0;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_GetCameraId(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    CAMERA_ID_T *pCameraId = (CAMERA_ID_T *)(pstSndPkt->buff);
    HI_S32 s32Camera = pstRecvPkt->s32Ret;
    if(s32Camera != FRONT_DOOR && s32Camera != BACK_DOOR)
    {
        pCameraId->cError = 0;
        *ps32SndLen = sizeof(CAMERA_ID_T);
    }
    else
    {
        pCameraId->cError = 1;
        GetApcCameraID(s32Camera,pCameraId->cameraid);
        *ps32SndLen = sizeof(CAMERA_ID_T);
    }
    return HI_SUCCESS;

}
HI_S32 DOMAIN_UpdateCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    UPDATE_CAMERA_BIN_T* pCameraBin = (UPDATE_CAMERA_BIN_T*)(pstRecvPkt->buff);
    HI_S32 s32JhyRet =0;
    if(pCameraBin->door != FRONT_DOOR && pCameraBin->door != BACK_DOOR)
    {
        pstSndPkt->s32Ret = 1;
    }
    else
    {
        char cFilePath[256] = {0};
        sprintf(cFilePath,"/mnt/usb/tmkj_update/plugins/%s",pCameraBin->cameraid);
        AV_PRINT("update camera para door:%d file:%s",pCameraBin->door,cFilePath);
        if(!access(cFilePath, F_OK))  //文件存在
        {
            FILE *cfg_fp = NULL;
            HI_S32 s32Ret = 0;
            cfg_fp = fopen(cFilePath, "r+");
            if(cfg_fp == NULL)
            {
                AV_PRINT( "DOMAIN_UpdateCameraPara fopen filename=%s error",cFilePath);
                pstSndPkt->s32Ret = 3;
            }
            else
            {
                fseek(cfg_fp, SEEK_SET, SEEK_END);
                long lFilelen = ftell(cfg_fp);
                s32Ret = fseek(cfg_fp,0,SEEK_SET);
                char cBuffer[512*1024] = {0};
                s32Ret = fread(cBuffer,sizeof(char),lFilelen, cfg_fp);
                if(s32Ret<0 || s32Ret != lFilelen)
                {
                    AV_PRINT("read camera bin file err!%d %ld",s32Ret,lFilelen);
                    pstSndPkt->s32Ret = 4;
                }
                else
                {
                    s32JhyRet = SetApcCameraParam(pCameraBin->door,cBuffer,s32Ret);
                    AV_PRINT("read camera bin file length:%d.return value:%d",s32Ret,s32JhyRet);
                    pstSndPkt->s32Ret = s32JhyRet;
                }
                fclose(cfg_fp);
            }
        }
        else
        {
            pstSndPkt->s32Ret = 2;
            AV_PRINT("camera bin file no exist.%s",cFilePath);
        }
    }
    *ps32SndLen = sizeof(HI_S32);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_LearnBKGround(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HI_S32 s32Camera = pstRecvPkt->s32Ret;
    APCCOUNTER conbInfo;
    GetCounter(&conbInfo);
    HI_S32 s32JhyRet =0;
    if(s32Camera == FRONT_DOOR)
    {
        if(conbInfo.doorS[FRONT_DOOR] == DOOR_OPEN)
        {
            s32JhyRet = ApcLearnBKGround(FRONT_DOOR);
            pstSndPkt->s32Ret = s32JhyRet;
        }
        else
        {
            pstSndPkt->s32Ret = RET_LEARN_BK_OPEN_FRONT;
        }
    }
    else if(s32Camera == BACK_DOOR)
    {
        if(conbInfo.doorS[BACK_DOOR] == DOOR_OPEN)
        {
            s32JhyRet = ApcLearnBKGround(FRONT_DOOR);
            pstSndPkt->s32Ret = s32JhyRet;
        }
        else
        {
            pstSndPkt->s32Ret = RET_LEARN_BK_OPEN_BACK;
        }
    }
    else
        pstSndPkt->s32Ret = RET_LEARN_BK_INDEX;
    AV_PRINT("Learn back ground camera index is %d.FrontDoor[%d][%d] BackDoor[%d][%d]"
             ,s32Camera,conbInfo.doorS[0],conbInfo.doorS[1],conbInfo.doorS[2],conbInfo.doorS[3]);
    *ps32SndLen = sizeof(HI_S32);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SetCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    CAMERA_PARA_T *pCamPara = (CAMERA_PARA_T *)(pstRecvPkt->buff);
    APCPARAM apcParam;
    HI_S32 s32JhyRet =0;
    apcParam.installH = pCamPara->installH;
    apcParam.noiseFilter = pCamPara->noiseFilter;
    apcParam.leftLimit = pCamPara->leftLimit;
    apcParam.rightLimit = pCamPara->rightLimit;
    apcParam.topLimit = pCamPara->topLimit;
    apcParam.bottomLimit = pCamPara->bottomLimit;
    apcParam.enableC = pCamPara->enableC;
    apcParam.inoutType = pCamPara->inoutType;
    apcParam.doorDelay = pCamPara->doorDelay;
    apcParam.filterHeight = pCamPara->filterHeight;
    apcParam.counterType = pCamPara->counterType;
    memcpy(&apcParam.sensitivity,pCamPara->sensitivity,sizeof(char)*4);
    apcParam.beeline[0] = pCamPara->beeline[0];
    apcParam.beeline[1] = pCamPara->beeline[1];
    apcParam.headFilter = pCamPara->headFilter;
    apcParam.forgTh = pCamPara->forgTh;
    s32JhyRet = SetApcParam(pCamPara->door,apcParam);
    AV_PRINT("Set Camera[%d][%d] Para(installH[%d] noiseFilter[%d] leftLimit[%d] rightLimit[%d] \
                topLimit[%d] bottomLimit[%d] enableC[%d] inoutType[%d] doorDelay[%d]    \
                filterHeight[%d] counterType[%d] sensitivity[%f] beeline[%d][%d] headFilter[%d] forgTh[%d])"
             ,pCamPara->door,pCamPara->camerach,pCamPara->installH,pCamPara->noiseFilter,pCamPara->leftLimit,pCamPara->rightLimit
             ,pCamPara->topLimit,pCamPara->bottomLimit,pCamPara->enableC,pCamPara->inoutType
             ,pCamPara->doorDelay,pCamPara->filterHeight,pCamPara->counterType,apcParam.sensitivity
             ,pCamPara->beeline[0],pCamPara->beeline[1],pCamPara->headFilter,pCamPara->forgTh);
    pstSndPkt->s32Ret = s32JhyRet;
    *ps32SndLen = sizeof(HI_S32);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_GetCameraPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRecvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    CAMERA_PARA_T *pCamPara = (CAMERA_PARA_T *)(pstSndPkt->buff);
    HI_S32 s32Camera = FRONT_DOOR;
    APCPARAM apcParam;

    for(s32Camera = FRONT_DOOR; s32Camera <= BACK_DOOR; s32Camera++,pCamPara++)
    {
        GetApcParam(s32Camera,&apcParam);
        GetApcCameraID(s32Camera,pCamPara->cameraid);
        pCamPara->door = s32Camera;
        pCamPara->camerach = HI3521_JHY_GetLogicChn(s32Camera);
        pCamPara->installH = apcParam.installH;
        pCamPara->noiseFilter = apcParam.noiseFilter;
        pCamPara->leftLimit = apcParam.leftLimit;
        pCamPara->rightLimit = apcParam.rightLimit;
        pCamPara->topLimit = apcParam.topLimit;
        pCamPara->bottomLimit = apcParam.bottomLimit;
        pCamPara->enableC = apcParam.enableC;
        pCamPara->inoutType = apcParam.inoutType;
        pCamPara->doorDelay = apcParam.doorDelay;
        pCamPara->filterHeight = apcParam.filterHeight;
        pCamPara->counterType = apcParam.counterType;
        memcpy(pCamPara->sensitivity,&apcParam.sensitivity,sizeof(apcParam.sensitivity));
        pCamPara->beeline[0] = apcParam.beeline[0];
        pCamPara->beeline[1] = apcParam.beeline[1];
        pCamPara->headFilter = apcParam.headFilter;
        pCamPara->forgTh = apcParam.forgTh;
        AV_PRINT("Get Camera[%d][%d][%s] Para(installH[%d] noiseFilter[%d] leftLimit[%d] rightLimit[%d] \
                topLimit[%d] bottomLimit[%d] enableC[%d] inoutType[%d] doorDelay[%d]    \
                filterHeight[%d] counterType[%d] sensitivity[%f] beeline[%d][%d] headFilter[%d] forgTh[%d])"
                 ,s32Camera,pCamPara->camerach,pCamPara->cameraid,pCamPara->installH,pCamPara->noiseFilter,pCamPara->leftLimit,pCamPara->rightLimit
                 ,pCamPara->topLimit,pCamPara->bottomLimit,pCamPara->enableC,pCamPara->inoutType
                 ,pCamPara->doorDelay,pCamPara->filterHeight,pCamPara->counterType,apcParam.sensitivity
                 ,pCamPara->beeline[0],pCamPara->beeline[1],pCamPara->headFilter,pCamPara->forgTh);
    }
    debug_print("Camera Para",(char *)pstSndPkt->buff,sizeof(CAMERA_PARA_T)*2);
    *ps32SndLen = sizeof(CAMERA_PARA_T)*2;
    return HI_SUCCESS;
}
#endif
HI_S32 DOMAIN_GetFuncConfig(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    AV_FUNC_CFG_T* pstFunc = (AV_FUNC_CFG_T*)(pstSndPkt->buff);
    memset(pstFunc,0,sizeof(AV_FUNC_CFG_T));
#ifdef JHY_SDK
    pstFunc->cEnableKl = 1;
#else
    pstFunc->cEnableKl = 0;
#endif
    pstFunc->cVideoInCount = 12;
    *ps32SndLen = sizeof(AV_FUNC_CFG_T);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RecvHardWareTemp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    static int iHeatIndex = 0;
    static int iCoolIndex = 0;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    AV_TEMP_STA stHardWareTemp = pstRcvPkt->stHardWareTemp;

    if(g_HardWareTemp.diskTemp != stHardWareTemp.diskTemp
            ||g_HardWareTemp.MainBoardTemp != stHardWareTemp.MainBoardTemp)
        AV_PRINT("Recv HardWare Temp--HDD[%d][%d] MainBorad[%d][%d]"
                 ,stHardWareTemp.diskTemp,stHardWareTemp.MainBoardTemp
                 ,g_HardWareTemp.diskTemp,g_HardWareTemp.MainBoardTemp);
    if(g_HardWareTempSta.iLowTempCount >= HW_TEMP_ABNORMAL_TIME)
    {
        if(g_HardWareTempSta.iLowTempCount >= 2*HW_TEMP_ABNORMAL_TIME
           && g_HardWareTempSta.iGpioHeatDisk == GPIO_ACTION_START
           && stHardWareTemp.diskTemp >=  DISKTMP_HEAT_MAX_THREHOLD)
        {
            //停止加热
            TGPIOCmd stGpio;
            stGpio.cCmd = GPIO_ACTION_STOP;
            stGpio.cFunc = GPIO_CMD_DISK_HEAT;
            DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
            AV_PRINT("stop heat hard disk!!!!!");
            g_HardWareTempSta.iGpioHeatDisk = GPIO_ACTION_STOP;
            g_HardWareTempSta.iLowTempCount=0;
            iHeatIndex = 0;
        }
        else
        {
            if(g_HardWareTempSta.iGpioHeatDisk == GPIO_ACTION_STOP
                || iHeatIndex >= 10)
            {
                TGPIOCmd stGpio;
                stGpio.cCmd = GPIO_ACTION_START;
                stGpio.cFunc = GPIO_CMD_DISK_HEAT;
                DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
                AV_PRINT("start heat hard disk!!!");
                g_HardWareTempSta.iGpioHeatDisk = GPIO_ACTION_START;
                iHeatIndex = 0;
            }
            if(stHardWareTemp.diskTemp >= DISKTMP_HEAT_MAX_THREHOLD)
            {
                g_HardWareTempSta.iLowTempCount++;
            }
            else
            {
                g_HardWareTempSta.iLowTempCount = HW_TEMP_ABNORMAL_TIME;
            }
            iHeatIndex++;
        }
    }
    //温度、ACC状态
    if (stHardWareTemp.diskTemp < DISKTMP_MAX_THREHOLD
            &&stHardWareTemp.diskTemp > DISKTMP_MIN_THREHOLD)
    {
        //使用ACC信号触发硬盘挂载
        if(g_u8AccEnable == 1)
        {
            if(g_HardWareState.otherHwStat&HW_ACC_STAT_BIT)
                HDD_SetDiskAbnormal(0);
            else
            {
                HDD_SetDiskAbnormal(1);
            }
        }
        else
            HDD_SetDiskAbnormal(0);
        //18->17度
        if(stHardWareTemp.diskTemp <= DISKTMP_HEAT_MAX_THREHOLD - 3
            &&g_HardWareTemp.diskTemp >= DISKTMP_HEAT_MAX_THREHOLD - 2)
        {
            g_HardWareTempSta.iLowTempCount = HW_TEMP_ABNORMAL_TIME;
        }
    }
    else
    {
        //温度过低
        if(stHardWareTemp.diskTemp!=HW_TEMP_ABNORMAL_VALUE
                && stHardWareTemp.diskTemp<=DISKTMP_HEAT_MIN_THREHOLD)
        {
            g_HardWareTempSta.iLowTempCount++;
        }
        else if(stHardWareTemp.diskTemp==HW_TEMP_ABNORMAL_VALUE
                && g_HardWareTempSta.iGpioHeatDisk == GPIO_ACTION_START)
        {
            g_HardWareTempSta.iLowTempCount++;
        }
        else
            g_HardWareTempSta.iLowTempCount=0;
		if(g_HardWareTempSta.iLowTempCount >= HW_TEMP_ABNORMAL_TIME)
			HDD_SetDiskAbnormal(1);
    }
#if 0
    //温度、ACC状态
    if (stHardWareTemp.diskTemp<DISKTMP_MAX_THREHOLD
            &&stHardWareTemp.diskTemp>DISKTMP_MIN_THREHOLD)
    {
        //使用ACC信号触发硬盘挂载
        if(g_u8AccEnable == 1)
        {
            if(g_HardWareState.otherHwStat&HW_ACC_STAT_BIT)
                HDD_SetDiskAbnormal(0);
            else
            {
                //AV_PRINT("enable acc power,acc state error");
                HDD_SetDiskAbnormal(1);
            }
        }
        else
            HDD_SetDiskAbnormal(0);
        //温度正常，停止加热
        if(g_HardWareTempSta.iGpioHeatDisk == GPIO_ACTION_START)
        {
            TGPIOCmd stGpio;
            stGpio.cCmd = GPIO_ACTION_STOP;
            stGpio.cFunc = GPIO_CMD_DISK_HEAT;
            DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
            AV_PRINT("stop heat hard disk!!!!!");
        }
        g_HardWareTempSta.iGpioHeatDisk = GPIO_ACTION_STOP;
        g_HardWareTempSta.iLowTempCount=0;
    }
    else if(g_HardWareTempSta.iLowTempCount >= HW_TEMP_ABNORMAL_TIME)
    {
        HDD_SetDiskAbnormal(1);
        //通知MUC加热硬盘
        if(g_HardWareTempSta.iLowTempCount >= HW_TEMP_ABNORMAL_TIME)
        {
            if(g_HardWareTempSta.iGpioHeatDisk == GPIO_ACTION_STOP)
            {
                TGPIOCmd stGpio;
                stGpio.cCmd = GPIO_ACTION_START;
                stGpio.cFunc = GPIO_CMD_DISK_HEAT;
                DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
                AV_PRINT("start heat hard disk!!!");
                g_HardWareTempSta.iGpioHeatDisk = GPIO_ACTION_START;
            }
        }
        else if(stHardWareTemp.diskTemp!=HW_TEMP_ABNORMAL_VALUE
                && stHardWareTemp.diskTemp<=DISKTMP_MIN_THREHOLD)
        {
            g_HardWareTempSta.iLowTempCount++;
        }
        else
        {
            g_HardWareTempSta.iLowTempCount=0;
        }
    }
    else
    {
        //温度过低
        if(stHardWareTemp.diskTemp!=HW_TEMP_ABNORMAL_VALUE
                && stHardWareTemp.diskTemp<=DISKTMP_MIN_THREHOLD)
        {
            g_HardWareTempSta.iLowTempCount++;
        }
        else
        {
            g_HardWareTempSta.iLowTempCount=0;
        }
    }
#endif
    //风扇降温
    if(g_HardWareTempSta.iEnableAssitDownTmep == 1)
    {
        if(g_HardWareTempSta.iHighTempCount >= HW_TEMP_ABNORMAL_TIME)
        {
            if(g_HardWareTempSta.iHighTempCount >= 2*HW_TEMP_ABNORMAL_TIME
                    && g_HardWareTempSta.iGpioCoolDownDisk == GPIO_ACTION_START)
            {
                //停止降温
                TGPIOCmd stGpio;
                stGpio.cCmd = GPIO_ACTION_STOP;
                stGpio.cFunc = GPIO_CMD_DISK_COOL_DOWN;
                DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
                g_HardWareTempSta.iGpioCoolDownDisk = GPIO_ACTION_STOP;
                g_HardWareTempSta.iHighTempCount=0;
                AV_PRINT("stop cool down disk");
                iCoolIndex = 0;
            }
            else
            {
                if(g_HardWareTempSta.iGpioCoolDownDisk == GPIO_ACTION_STOP
                    || iCoolIndex >= 10)
                {
                    TGPIOCmd stGpio;
                    stGpio.cCmd = GPIO_ACTION_START;
                    stGpio.cFunc = GPIO_CMD_DISK_COOL_DOWN;
                    DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
                    g_HardWareTempSta.iGpioCoolDownDisk = GPIO_ACTION_START;
                    AV_PRINT("start cool down disk");
                    iCoolIndex = 0;
                }
                if(stHardWareTemp.diskTemp<g_HardWareTempSta.iCoolDownTemp
                    ||stHardWareTemp.diskTemp==HW_TEMP_ABNORMAL_VALUE)
                {
                    g_HardWareTempSta.iHighTempCount++;
                }
                else
                {
                    g_HardWareTempSta.iHighTempCount = HW_TEMP_ABNORMAL_TIME;
                }
                iCoolIndex++;
            }

        }
        else
        {
            if(stHardWareTemp.diskTemp!=HW_TEMP_ABNORMAL_VALUE
                    && stHardWareTemp.diskTemp>g_HardWareTempSta.iCoolDownTemp)
            {
                g_HardWareTempSta.iHighTempCount++;
            }
            //硬盘温度异常
            else if(stHardWareTemp.diskTemp==HW_TEMP_ABNORMAL_VALUE
                    &&g_HardWareTempSta.iGpioCoolDownDisk == GPIO_ACTION_START)
            {
                //停止降温
                TGPIOCmd stGpio;
                stGpio.cCmd = GPIO_ACTION_STOP;
                stGpio.cFunc = GPIO_CMD_DISK_COOL_DOWN;
                DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
                g_HardWareTempSta.iGpioCoolDownDisk = GPIO_ACTION_STOP;
                g_HardWareTempSta.iHighTempCount=0;
                AV_PRINT("disk temp error!!!stop cool down disk");
            }
            else
                g_HardWareTempSta.iHighTempCount=0;
        }

    }
    //停止降温
    else if(g_HardWareTempSta.iGpioCoolDownDisk == GPIO_ACTION_START)
    {
        TGPIOCmd stGpio;
        stGpio.cCmd = GPIO_ACTION_STOP;
        stGpio.cFunc = GPIO_CMD_DISK_COOL_DOWN;
        DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
        AV_PRINT("unenable flag!!!stop cool down disk");
        g_HardWareTempSta.iHighTempCount = 0;
        g_HardWareTempSta.iGpioCoolDownDisk = GPIO_ACTION_STOP;
    }
    else
    {
        g_HardWareTempSta.iHighTempCount = 0;
    }
    g_HardWareTemp.diskTemp = stHardWareTemp.diskTemp;
    g_HardWareTemp.MainBoardTemp = stHardWareTemp.MainBoardTemp;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_HandleHardWareSta(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    AV_HARD_STA stDeviceState = pstRcvPkt->stDeviceState;
    AV_PRINT("DOMAIN_HandleHardWareSta HWSTAT: %#x PowerStat:%d"
             ,stDeviceState.otherHwStat,stDeviceState.ucPowerStat);
    if(stDeviceState.ucPowerStat == PW_STAT_MAIN_AND_BAT_ERR
            ||(stDeviceState.powerstatus == STAT_ONOFF_OFF))
    {
        AV_PRINT("Main and back power down,stop record!!!!");
        HDD_SetHddUnLockEvent(HI_TRUE);

        return HI_SUCCESS;
    }
    //开锁事件
    if(stDeviceState.otherHwStat&HW_DISK_LOCK_STAT_BIT)
    {
        AV_PRINT("unlock disk stop record!!");
        HDD_SetHddUnLockEvent(HI_TRUE);
    }
    else
    {
        HDD_SetHddUnLockEvent(HI_FALSE);
    }

#ifdef JHY_SDK
    if((g_HardWareState.cIoSingle & 0x0F) != (stDeviceState.cIoSingle & 0x0F))
    {
        //后门
        if (stDeviceState.cIoSingle & BUS_BACK_DOOR_OPEN)/*开*/
        {
            AV_PRINT("\033[0;32;31mback door open\033[m");
            SetDoorState(BACK_DOOR,DOOR_OPEN);
        }
        else if (stDeviceState.cIoSingle & BUS_BACK_DOOR_CLOSE)/*关*/
        {
            AV_PRINT("\033[0;32;32mback door close\033[m");
            SetDoorState(BACK_DOOR,DOOR_CLOSE);
        }
        /*前门*/
        if (stDeviceState.cIoSingle & BUS_FRONT_DOOR_OPEN)/*开*/
        {
            AV_PRINT("\033[0;32;31mfront door open\033[m");
            SetDoorState(FRONT_DOOR,DOOR_OPEN);
        }
        else if (stDeviceState.cIoSingle & BUS_FRONT_DOOR_CLOSE)/*关*/
        {
            AV_PRINT("\033[0;32;32mfront door close\033[m");
            SetDoorState(FRONT_DOOR,DOOR_CLOSE);
        }
    }

#endif
    memcpy(&g_HardWareState,&stDeviceState,sizeof(AV_HARD_STA));
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RecvRegAckFromGate(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("Reg gate success!!!!");
    //向参数模块获取视频通道相关参数
    DOMAIN_PackageData(ID_PARAM, PARA_CMD_GET_PARA_COM,NULL,0);
    //获取硬件状态
    DOMAIN_PackageData(ID_BASIC, BASIC_HARDWARE_STATE_REQ,NULL,0);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_GetCommonPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_GetCommonPara======");
    short usSetParaId = 0;
    ParaHeadStruct *pstParaHead = NULL;
    short usParaLen = 0;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    int iPackageLen = ((pstRcvPkt->head.pkt_len_h << 8 ) | pstRcvPkt->head.pkt_len_l);
    int iRealDataLen = iPackageLen - sizeof(pstRcvPkt->head) - 2;
    int iRecvBufIndex = 0;
    VO_PARA_T stVideoOutPara;

    while ( iRecvBufIndex < iRealDataLen )
    {
        if ( ( iRealDataLen - iRecvBufIndex ) < 4 )
        {
            AV_PRINT("SetPara Error!!!iRecvBufIndex = %d, num_i = %d",iRecvBufIndex,iRealDataLen);
            break;
        }
        pstParaHead = (ParaHeadStruct *)&pstRcvPkt->buff[iRecvBufIndex];
        usSetParaId = ((pstParaHead->ucParaId_h << 8 ) | pstParaHead->ucParaId_l);
        usParaLen   = ((pstParaHead->ucParalen_h << 8) | pstParaHead->ucParalen_l);
        iRecvBufIndex += 4;
        HI_U16 *u16Para = (HI_U16*)&pstRcvPkt->buff[iRecvBufIndex];
        if(usSetParaId==PARAID_RECORD_CHN_CFG)
        {
            AV_SetVideoRecChn(*u16Para);
        }
        else if(usSetParaId==PARAID_PHOTO_CHN)
        {
            AV_SetVideoSnapShotChn(*u16Para);
        }
        else if(usSetParaId==PARAID_DEFAULT_CHN)
        {
            Hi3521_SwitchPreview(*u16Para);
        }
        else if(usSetParaId==PARAID_COOL_DOWN_ENABLE)
        {
            g_HardWareTempSta.iEnableAssitDownTmep=pstRcvPkt->buff[iRecvBufIndex];
            AV_PRINT("Enable cool down hard disk:%d",g_HardWareTempSta.iEnableAssitDownTmep);
        }
        else if(usSetParaId==PARAID_COOL_DOWN_TEMP)
        {
            g_HardWareTempSta.iCoolDownTemp= pstRcvPkt->buff[iRecvBufIndex];
            AV_PRINT("cool down hard disk temp:%d",g_HardWareTempSta.iCoolDownTemp);
        }
        else if(usSetParaId==PARAID_ACC_ENABLE)
        {
            g_u8AccEnable = pstRcvPkt->buff[iRecvBufIndex];
            //温度、ACC状态
            if (g_HardWareTemp.diskTemp<DISKTMP_MAX_THREHOLD
                    &&g_HardWareTemp.diskTemp>DISKTMP_MIN_THREHOLD)
            {
                //使用ACC信号触发硬盘挂载
                if(g_u8AccEnable == 1)
                {
                    if(g_HardWareState.otherHwStat&HW_ACC_STAT_BIT)
                        HDD_SetDiskAbnormal(0);
                    else
                    {
                        AV_PRINT("enable acc power,acc state error");
                        HDD_SetDiskAbnormal(1);
                    }
                }
                else
                    HDD_SetDiskAbnormal(0);
            }
#if 0
            else
            {
                AV_PRINT("disk tempratrue abnomal!");
                HDD_SetDiskAbnormal(1);
            }
#endif
            AV_PRINT("ACC enable:%d",g_u8AccEnable);
        }
        else if(usSetParaId==PARAID_LUMINANCE)
        {
            APP_GetVideoOutputPara(&stVideoOutPara);
            stVideoOutPara.s32CscLuma = pstRcvPkt->buff[iRecvBufIndex];
            APP_SetVideoOutputPara(&stVideoOutPara);
            AV_PRINT("Video Out s32CscLuma:%d",stVideoOutPara.s32CscLuma);
        }
        else if(usSetParaId==PARAID_CONTRAST)
        {
            APP_GetVideoOutputPara(&stVideoOutPara);
            stVideoOutPara.s32CscContr = pstRcvPkt->buff[iRecvBufIndex];
            APP_SetVideoOutputPara(&stVideoOutPara);
            AV_PRINT("Video Out s32CscContr:%d",stVideoOutPara.s32CscContr);
        }
        else if(usSetParaId==PARAID_HUE)
        {
            APP_GetVideoOutputPara(&stVideoOutPara);
            stVideoOutPara.s32CscHue = pstRcvPkt->buff[iRecvBufIndex];
            APP_SetVideoOutputPara(&stVideoOutPara);
            AV_PRINT("Video Out s32CscHue:%d",stVideoOutPara.s32CscHue);
        }
        else if(usSetParaId==PARAID_SATURATION)
        {
            APP_GetVideoOutputPara(&stVideoOutPara);
            stVideoOutPara.s32CscSatu = pstRcvPkt->buff[iRecvBufIndex];
            APP_SetVideoOutputPara(&stVideoOutPara);
            AV_PRINT("Video Out s32CscSatu:%d",stVideoOutPara.s32CscSatu);
        }
        else if(usSetParaId==PARAID_IP_MIC_POWER)
        {
            unsigned char ucMicVolume = pstRcvPkt->buff[iRecvBufIndex];
            SetVoipMicVolume(ucMicVolume);
            AV_PRINT("IP talk volume:%d",ucMicVolume);
        }
        else
        {
            AV_PRINT("Error para id:%x",usSetParaId);
        }
        iRecvBufIndex += usParaLen;
    }
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndRequstToPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_SndRequstToPara======");
//    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //调用函数，在函数中启动回放线程
    memset(pstSndPkt->buff,0,sizeof(pstSndPkt->buff));
    pstSndPkt->head.dst_id = ID_PARAM;
    pstSndPkt->buff[1] = PARAID_RECORD_CHN_CFG;//录像状态
    pstSndPkt->buff[3] = PARAID_PHOTO_CHN;//抓拍通道
    pstSndPkt->buff[5] = PARAID_DEFAULT_CHN;//抓拍通道
    pstSndPkt->buff[7] = PARAID_ACC_ENABLE;//是否启用ACC信号控制硬盘挂载
    pstSndPkt->buff[9] = PARAID_LUMINANCE;//亮度
    pstSndPkt->buff[11] = PARAID_CONTRAST;//对比度
    pstSndPkt->buff[13] = PARAID_HUE;//色调
    pstSndPkt->buff[15] = PARAID_SATURATION;//饱和度
    pstSndPkt->buff[17] = PARAID_IP_MIC_POWER;//IP对讲MIC音量
    pstSndPkt->buff[19] = PARAID_COOL_DOWN_TEMP;//饱和度
    pstSndPkt->buff[21] = PARAID_COOL_DOWN_ENABLE;//饱和度
    *ps32SndLen = 22;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_UpdatePara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("DOMAIN_UpdatePara======");
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    *ps32SndLen = sizeof(HI_S32);
    pstSndPkt->s32Ret = 0;
    DOMAIN_PackageData(ID_PARAM, PARA_CMD_GET_PARA_COM,NULL,0);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_QueryDeviceState(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    pstSndPkt->stCameraState.cameraStat = 0;
    HI3521_GetChnStoreStat(&(pstSndPkt->stCameraState.recordVideoStat));
    HI_S32 s32HdVideoStat = HI3521_HdGetVideoLostStat();
    tw2968_video_loss video_loss;
    video_loss.chip = 0;
    video_loss.ch = 0;
    video_loss.is_lost = 0;
    HI3521_SdGetVideoLostStat(&video_loss);
    pstSndPkt->stCameraState.cameraStat = video_loss.is_lost;
    pstSndPkt->stCameraState.cameraStat|= (~s32HdVideoStat)<<8;
    *ps32SndLen = sizeof(AV_CAMERA_STA);
    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : DOMAIN_StartLPB
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_StartLPB(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
//    LPB_SWITCH_AO_T stLPBAoSwitch;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    PLAYBACK_T stPlayBack;
    stPlayBack = pstRcvPkt->stPlayBack;
    *ps32SndLen = sizeof(HI_S32);
    HI_CHAR timeBufFrom[32] = {0};
    HI_CHAR timeBufTo[32] = {0};
    struct tm *pTm = NULL;
    pTm = localtime((time_t*)(&pstRcvPkt->stPlayBack.sTime));
    strftime(timeBufFrom, sizeof(timeBufFrom), "%Y-%m-%d %H:%M:%S", pTm);
    pTm = localtime((time_t*)(&pstRcvPkt->stPlayBack.eTime));
    strftime(timeBufTo, sizeof(timeBufTo), "%Y-%m-%d %H:%M:%S", pTm);
    AV_PRINT("DOMAIN_StartLPB(%d) From %s(%d) to %s(%d)",pstRcvPkt->stPlayBack.s32ChnSel, timeBufFrom,pstRcvPkt->stPlayBack.sTime,timeBufTo,pstRcvPkt->stPlayBack.eTime);
    pstSndPkt->s32Ret = 0;
    if (HDD_IsLPBRunning())
    {
        pstSndPkt->s32Ret = 0;
        return HI_SUCCESS;
    }

    HDD_StartLPB();
    s32Ret = HI3521_StartPlayback((REC_PLAYBACK_T *)&stPlayBack);
    if (s32Ret == HI_FAILURE)
    {
        pstSndPkt->s32Ret = 1;
        HDD_StopLPB();
    }
    else
    {
        AV_PRINT("HI3531_LoaclPlayback success");
        pstSndPkt->s32Ret = 0;
//    	stLPBAoSwitch.cSwitchAo = 1;
//        DOMAIN_PackageData(ID_BZQ, AV_LPB_AO_SWITCH, (HI_U8 *)&stLPBAoSwitch, sizeof(stLPBAoSwitch));
    }

    AV_PRINT("start local play back cmd+++++++++++++++++++++(0x%02x)", stPlayBack.s32ChnSel);
    return HI_SUCCESS;
}
/*****************************************************************************
 Prototype    : DOMAIN_StopLPB
 Description  : 停止回放
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_StopLPB(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    *ps32SndLen = sizeof(HI_S32);

    if (HDD_IsLPBRunning())
    {
        AV_PRINT("DOMAIN_StopLPB===============");
        s32Ret = HDD_StopLPB();
        if (s32Ret == HI_FAILURE)
        {
            pstSndPkt->s32Ret = 1;
        }
        else
        {
            pstSndPkt->s32Ret = 0;
        }
        /*等待退出*/
        HDD_WaitExitLPB();
    }

    return HI_SUCCESS;
}
HI_S32 DOMAIN_LPBOK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    *ps32SndLen = sizeof(HI_S32);
    pstSndPkt->s32Ret = 0;
    pstSndPkt->head.dst_id = ID_UI;
    //usleep(50*1000);
    TM_USleep(0,50*1000);
    return s32Ret;
}

HI_S32 DOMAIN_localPlayCtrl_Seek(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    PLAY_CTRL_ST stPlayctrlstat;
    stPlayctrlstat = pstRcvPkt->stPlayctrl;
    pstSndPkt->s32Ret = 1;
    *ps32SndLen = sizeof(HI_S32);
    AV_PRINT("DOMAIN_localPlayCtrl_Seek===============%d",pstRcvPkt->stPlayctrl.s32playstat);
    if (HDD_IsLPBRunning())
    {
        AV_PRINT("s32playstat (%d)", stPlayctrlstat.s32playstat);
        HI3521_SetAllPlayCtrl(stPlayctrlstat.s32playstat);

        DOMAIN_PackageData(ID_UI, AV_PLAY_CTRL, NULL, 0);
        pstSndPkt->s32Ret = 0;
    }
    return HI_SUCCESS;
}
HI_S32 DOMAIN_localPlayCtrlStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    PLAY_CTRL_ST stPlayctrlstat;

    stPlayctrlstat = pstRcvPkt->stPlayctrl;
    pstSndPkt->s32Ret = 1;
    *ps32SndLen = sizeof(HI_S32);
    AV_PRINT("DOMAIN_localPlayCtrlStat===============%d",pstRcvPkt->stPlayctrl.s32playstat);
    debug_print("DOMAIN_localPlayCtrlStat",(char *)&(pstRcvPkt->stPlayctrl),sizeof(PLAY_CTRL_ST));
    if (HDD_IsLPBRunning())
    {
        AV_PRINT("s32playstat (%d)", stPlayctrlstat.s32playstat);
        HI3521_SetAllPlayCtrl(stPlayctrlstat.s32playstat);

        /*回应收到的消息 */
        DOMAIN_PackageData(ID_UI, AV_PLAY_CTRL, NULL, 0);

        pstSndPkt->s32Ret = 0;
    }
    return HI_SUCCESS;
}

HI_S32 DOMAIN_StartLBK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
//	DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    /*调用回放测试 */
    LOCAL_BK_T stLocalBk;
    stLocalBk = pstRcvPkt->stLocalBk;
    //调用函数，在函数中启动回放线程
    //*ps32SndLen = sizeof(HI_S32);
    *ps32SndLen = 0;
    AV_PRINT("start usb bk cmd+++++++++++++++++++++");
    HI3521_LocalBK((REC_COPY_T *)&stLocalBk);
#if 0
    /*收到开始备份，立即发送应答消息 */
    //DOMAIN_PackageData(ID_UI, AV_START_BK, NULL, 0);
    s32Ret = HI3521_LocalBK((REC_COPY_T *)&stLocalBk);
    if (s32Ret == HI_FAILURE)
    {
        pstSndPkt->s32Ret = 1;
    }
    else
    {
        pstSndPkt->s32Ret = 0;
    }
#endif
    return s32Ret;
}

HI_S32 DOMAIN_StopLBK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    //调用函数，在函数中启动回放线程
    *ps32SndLen = sizeof(HI_S32);
    printf("===========STOP LBK NOW===========");
    HDD_SetHddEvent(DISK_EVENT_USB_BK, HI_FALSE);
    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndUIStartLBK(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    *ps32SndLen = sizeof(HI_S32);
    pstSndPkt->s32Ret = 0;
    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("AV send DOMAIN_StartLBK NOW");

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_SndUiBkStat
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_SndUiBkStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->stLBKStat);

    pstSndPkt->stLBKStat = pstRcvPkt->stLBKStat;
    pstSndPkt->head.dst_id = ID_UI;

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_FormatDisk
 Description  : 格式化磁盘
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/

HI_S32 DOMAIN_FormatDisk(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    PLAYBACK_T stPlayBack;
    stPlayBack = pstRcvPkt->stPlayBack;
    *ps32SndLen = sizeof(HI_S32);

    s32Ret = HDD_StopRec();
    if (s32Ret == HI_FAILURE)
    {
        pstSndPkt->s32Ret = 1;
    }
    else
    {
        pstSndPkt->s32Ret = 0;
    }

    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("format start(0x%02x)", pstSndPkt->s32Ret);
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_FormatDiskEnd
 Description  : 格式化磁盘结束
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_FormatDiskEnd(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    PLAYBACK_T stPlayBack;
    stPlayBack = pstRcvPkt->stPlayBack;


    //调用函数，在函数中启动回放线程
    *ps32SndLen = sizeof(HI_S32);

    s32Ret = HDD_StartRec();
    if (s32Ret == HI_FAILURE)
    {
        pstSndPkt->s32Ret = 1;
    }
    else
    {
        pstSndPkt->s32Ret = 0;
    }
    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("format over!!!!!!(0x%02x)", pstSndPkt->s32Ret);
    return HI_SUCCESS;
}

HI_S32 DOMAIN_GetOsdData(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    OSD_TITLE_DATA_ST *pstOsdTiltle = NULL;
    s32Ret = s32Ret;
    OSD_SHOW_DATA_ST stOsdData;
    stOsdData = pstRcvPkt->stOsdData;
    pstOsdTiltle = HI3521_GetTitleOsdData();
    memset(&pstOsdTiltle->stOsdSchInfo, 0, sizeof(OSD_SHOW_DATA_ST));
    memcpy(&pstOsdTiltle->stOsdSchInfo, &stOsdData, sizeof(OSD_SHOW_DATA_ST));
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvSchPlatInfoRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("sch recv PlatInfo now");
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    AV_VIDEOPLTPARA_INFO_ST *pstplatinfo = NULL;

    pstplatinfo = &pstRcvPkt->stPlatinfo;
    char ip[16] = {0};
    memcpy(ip,pstplatinfo->ip,15);
#ifdef PLT_PARA_BK
    char ipbk[16] = {0};
    memcpy(ipbk,pstplatinfo->ipbk,15);
    AV_PRINT("ip[M:%s][B:%s]  port[M:0x%x%x][B:0x%x%x] id[%x%x%x]",
             ip,ipbk,
             pstplatinfo->port[0],pstplatinfo->port[1],
             pstplatinfo->portbk[0],pstplatinfo->portbk[1],
             pstplatinfo->machineId[0],pstplatinfo->machineId[1],pstplatinfo->machineId[2]);
#else
    AV_PRINT("[ip %s]  [port 0x%x%x] [id %x%x%x]",ip,pstplatinfo->port[0],pstplatinfo->port[1],
             pstplatinfo->machineId[0],pstplatinfo->machineId[1],pstplatinfo->machineId[2]);
#endif
    DevGetNewPlatInfo((void*)pstplatinfo);
    HI3521_SetCompany(pstplatinfo->cCompany,pstplatinfo->cSubCompany);
    DOMAIN_PackageData(ID_SCH, SCH_RSP_PLT_INFO, (HI_U8 *)&s32Ret, 0);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndSchPlatInfoReq(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    //DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    //DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    AV_PRINT("DOMAIN_SndSchPlatInfoReq  now");

    *ps32SndLen = 0;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SVRLoginStat(HI_VOID)
{
    HI_S32 s32CurrloginStat = 0;
    static HI_S32 s32staloginStat = 2;
    SVR_REG_STAT_T stDevloginStat;
    /*dev err stat*/
    s32CurrloginStat = hasLogin();
    if ((s32staloginStat != s32CurrloginStat) || (!bDevLoginRecvOk))
    {
        s32staloginStat = s32CurrloginStat;
        /* nvs中连接上是0，av协议连接上是1，非链接为0 */
        stDevloginStat.s32Stat = (s32staloginStat==0?1:0);
        bDevLoginRecvOk = HI_FALSE;
        DOMAIN_PackageData(ID_UI, UI_AV_LOGIN, (HI_U8 *)&stDevloginStat, sizeof(SVR_REG_STAT_T));
    }

    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndSVRLoginStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //pstRcvPkt->head.pkt_cmd = AV_VOIP_CTRLAOAI;
    pstSndPkt->stLoginstat = pstRcvPkt->stLoginstat;

    *ps32SndLen = sizeof(SVR_REG_STAT_T);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvUiLoginRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("ui recv svr login Rsp now");
    bDevLoginRecvOk = HI_TRUE;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvUIStopVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("recv ui Stop VOIP now");
    DevStopVoip();
    return HI_SUCCESS;
}

HI_S32 DOMAIN_RcvUIStartDialing(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("recv ui Start Dialing now");
    DevStartDialing();
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvUIStartVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("recv ui Start VOIP now");
    DevStartVoip();
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndUIOpenVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //pstRcvPkt->head.pkt_cmd = AV_VOIP_CTRLAOAI;
    pstSndPkt->stVoipCtrl = pstRcvPkt->stVoipCtrl;
    *ps32SndLen = sizeof(VOIP_STAT_T);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndUIStopVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //pstRcvPkt->head.pkt_cmd = AV_VOIP_CTRLAOAI;
    pstSndPkt->stVoipCtrl = pstRcvPkt->stVoipCtrl;
    *ps32SndLen = sizeof(VOIP_STAT_T);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SetUiOsd(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    OSD_TITLE_DATA_ST *pstOsdTiltle = NULL;
    OSD_UI_SHOW_ST *pstOsdInfo = NULL;
    s32Ret = s32Ret;
    pstOsdInfo = (OSD_UI_SHOW_ST *)pstRcvPkt->buff;
    pstOsdTiltle = HI3521_GetTitleOsdData();
    memcpy(&pstOsdTiltle->stOsdUiInfo, pstOsdInfo, sizeof(OSD_UI_SHOW_ST));
    return HI_SUCCESS;
}
#if 0
HI_S32 DOMAIN_SndUiLPBCtrlStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    *ps32SndLen = sizeof(HI_S32);

    pstSndPkt->s32Ret = 0;
    pstSndPkt->head.dst_id = ID_UI;
    AV_PRINT("AV send DOMAIN_SndUiLPBCtrlStat NOW");
    return HI_SUCCESS;
}

HI_S32 DOMAIN_SndMcuDiskLed(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->s32diskled);

    pstSndPkt->s32diskled = pstRcvPkt->s32diskled;
    pstSndPkt->head.dst_id = ID_MCU;

    AV_PRINT("disk led stat");
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_UnlockKey
 Description  : 打开硬盘锁
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_UnlockKey(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;

    AV_PRINT("open disk key, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //open hdd key, write cache to hdd and stop recoder
    //AV_StopVideoRec();

    *ps32SndLen = 0; //ack data len

    return HI_SUCCESS;

}
/*****************************************************************************
 函 数 名  : DOMAIN_LocalDiskStat
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_LocalDiskStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32FindDev = 0;

    s32Ret = s32Ret;
    pstRcvPkt = pstRcvPkt;

    s32FindDev = HDD_FindDev();
    if (HI_FAILURE == s32FindDev)
    {
        AV_PRINT("hdd no found");
        s32DiskStat = 0;
    }
    else
    {
        AV_PRINT("hdd found");
        s32DiskStat = 1;
    }

    *ps32SndLen = sizeof(DISK_STAT_T);
    pstSndPkt->stDiskStat.s32Stat = s32DiskStat;
    pstSndPkt->head.dst_id = ID_UI;

    AV_PRINT("av send disk stat(%d)", pstSndPkt->stDiskStat.s32Stat);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_SendDevErrStat
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_SendDevErrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    //调用函数，在函数中启动回放线程
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->s32DevErrStat);

    pstSndPkt->s32DevErrStat = pstRcvPkt->s32DevErrStat;
    pstSndPkt->head.dst_id = ID_UI;

    //AV_PRINT("send dev stat(0x%02x)", pstRcvPkt->s32DevErrStat.s32DevStat);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_UiDevStatRsp
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_RcvUiDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("ui recv dev stat now");
    bDevStatRecvOk = HI_TRUE;
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_SetRecChn
 Description  : 设置录像通道状态-按位表示
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SetRecChn(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;

    AV_PRINT("set rec chn stat(0x%02x)", pstRcvPkt->stRecStat.s32Stat);

    //save recoder chn and set recoder chn
    AV_SetVideoRecChn(pstRcvPkt->stRecStat.s32Stat);

    *ps32SndLen = 0;

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_ExtOsdSet
 Description  : 设置扩展osd
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/3
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_ExtOsdSet(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;

    AV_PRINT("set ext osd cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //set ext osd EXT_OSD_T
    //AV_SetExtOsd(pstRcvPkt->stExtOsd);

    *ps32SndLen = 0;

    return HI_SUCCESS;
}
HI_S32 DOMAIN_GetAvVersion(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("get av version ,cmd =(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //set osd
    //AV_SetOsd(pstRcvPkt->stOsd);
    HI3531_GetVer((HI_U8 *)&pstSndPkt->stVersion);
    *ps32SndLen = 24;
    bDevLoginRecvOk = HI_FALSE;

    return HI_SUCCESS;
}

/* BEGIN: Added by mabuyun, 2015/9/2  DES:添加IP对讲功能  PN:20150902 */

/*****************************************************************************
 Prototype    : DOMAIN_RcvBZQCtrlVOIP
 Description  : 接收报站器对讲控制应答
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/9/2
    Author       : mabuyun
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SndBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    //HI_BOOL bAlarmTrigger = HI_FALSE;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //pstRcvPkt->head.pkt_cmd = AV_VOIP_CTRLAOAI;
    pstSndPkt->buff[0] = pstRcvPkt->buff[0];
    pstSndPkt->buff[1] = pstRcvPkt->buff[1];

    AV_PRINT("DOMAIN_SndBZQCtrlVOIP here (%d)", pstSndPkt->buff[0]);
    *ps32SndLen = 2;
    return HI_SUCCESS;
}





HI_S32 DOMAIN_SndSchPlatInfoReq(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    //DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    //DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    AV_PRINT("DOMAIN_SndSchPlatInfoReq  now");

    *ps32SndLen = 0;
    return HI_SUCCESS;
}


HI_S32 DOMAIN_RcvUiLoginRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("ui recv Login Rsp now");
    bDevLoginRecvOk = HI_TRUE;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("bzq recv Voip cmd now");

    return HI_SUCCESS;
}




/* END:   Added by mabuyun, 2015/9/2   PN:20150902 */

/*****************************************************************************
 Prototype    : DOMAIN_ExtOsdSet
 Description  : 设置OSD显示内容--这里是车辆编号
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32  *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/3
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_OsdSet(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;

    AV_PRINT("set osd cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //set osd
    //AV_SetOsd(pstRcvPkt->stOsd);

    *ps32SndLen = 0;

    return HI_SUCCESS;
}

//调用函数，在函数中启动回放线程



//调用函数，在函数中启动回放线程






HI_S32 DOMAIN_McuAlarmTrigger(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_BOOL bAlarmTrigger = HI_FALSE;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    ALARM_STAT_T stAlarmStat;

    stAlarmStat = pstRcvPkt->stAlarmStat;
    AV_PRINT("alarm trigger here (%d)", stAlarmStat.alarmStat);

    if(stAlarmStat.alarmStat & ALARM_TRRIGER_BIT)
    {
        bAlarmTrigger = HI_TRUE;
        AV_PRINT("alarm trigger now");
    }
    else
    {
        bAlarmTrigger = HI_FALSE;
    }

    HDD_SetAlarmTrigger(bAlarmTrigger);

    /*收到，立即发送应答消息 */
    DOMAIN_PackageData(ID_MCU, SCH_ALARM_TRIGGER, NULL, 0);

    *ps32SndLen = sizeof(ALARM_STAT_T);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_SndDevStat
 功能描述  : 发送设备故障状态
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月22日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_SndDevStat(HI_VOID)
{
    HI_S32 s32CurrDevStat = 0;
    DEVERROR_STAT_T stDevErrStat;

    /*dev err stat*/
    s32CurrDevStat = HI3531_GetDevErrStat();
    stDevErrStat.s32DevStat = s32CurrDevStat;
    if ((HI3531_GetGobalDevErrStat() != s32CurrDevStat) || (!bDevStatRecvOk))
    {
        HI3531_SetGobalDevErrStat(s32CurrDevStat);
        bDevStatRecvOk = HI_FALSE;
        DOMAIN_PackageData(ID_UI, AV_DEVERR_STAT, (HI_U8 *)&stDevErrStat, sizeof(DEVERROR_STAT_T));
    }

    return HI_SUCCESS;
}

#endif
/*****************************************************************************
 Prototype    : DOMAIN_GetVoPara
 Description  : 获取vo参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_GetVoPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("get vo para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //get vo para copy to pSndPkt
    APP_GetVideoOutputPara(&pstRcvPkt->stVideoOutPara);
    memcpy(&pstSndPkt->stVideoOutPara,&pstRcvPkt->stVideoOutPara,sizeof(VO_PARA_T));
    AV_PRINT("get vo para, cmd(0x%02x)", pstSndPkt->head.pkt_cmd);
    AV_PRINT("get vo para, s32CscContr=%d\n",pstSndPkt->stVideoOutPara.s32CscContr);
    AV_PRINT("get vo para, s32CscHue=%d\n",pstSndPkt->stVideoOutPara.s32CscHue);
    AV_PRINT("get vo para, s32CscLuma=%d\n",pstSndPkt->stVideoOutPara.s32CscLuma);
    AV_PRINT("get vo para, s32CscSatu=%d\n",pstSndPkt->stVideoOutPara.s32CscSatu);
    debug_print("DOMAIN_GetVoPara",(char *)(&pstSndPkt->stVideoOutPara),sizeof(VO_PARA_T));
    *ps32SndLen = sizeof(VO_PARA_T); //ack data len

    return HI_SUCCESS;

}
HI_S32 DOMAIN_SwitchLPBAo(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->stLPBSwitchAo);
    pstSndPkt->stLPBSwitchAo = pstRcvPkt->stLPBSwitchAo;
    pstSndPkt->head.dst_id = ID_BZQ;
    AV_PRINT("switch LPB ao switch+++++++++++++++++++++(0x%02x)", pstRcvPkt->stLPBSwitchAo.cSwitchAo);
    return HI_SUCCESS;
}
/*****************************************************************************
 Prototype    : DOMAIN_SetVoPara
 Description  : 设置vo参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SetVoPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    AV_PRINT("set vo para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);
    AV_PRINT("set vo para, s32CscContr=%d\n",pstRcvPkt->stVideoOutPara.s32CscContr);
    AV_PRINT("set vo para, s32CscHue=%d\n",pstRcvPkt->stVideoOutPara.s32CscHue);
    AV_PRINT("set vo para, s32CscLuma=%d\n",pstRcvPkt->stVideoOutPara.s32CscLuma);
    AV_PRINT("set vo para, s32CscSatu=%d\n",pstRcvPkt->stVideoOutPara.s32CscSatu);
    //set vo para copy to local
    s32Ret = APP_SetVideoOutputPara(&pstRcvPkt->stVideoOutPara);
    if(s32Ret!=0)
        s32Ret = 1;
    AV_PRINT("set video out ret %d",s32Ret);
    *ps32SndLen = 4; //ack data len
    pstSndPkt->head.dst_id = ID_UI;
    pstSndPkt->s32Ret = s32Ret;
    return HI_SUCCESS;

}
/*****************************************************************************
 Prototype    : DOMAIN_GetViPara
 Description  : 获取vi参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_GetViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("get vi para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);

    //get vi para copy to local
    APP_GetVideoInputPara(&pstRcvPkt->stVideoInPara);
    memcpy(&pstSndPkt->stVideoInPara,&pstRcvPkt->stVideoInPara,sizeof(VI_PARA_T));
    int i = 0;
    for(i=0; i<APP_VI_CHNCNT; i++)
    {
        AV_PRINT("get ch%d vi para, s32CscLuma=%d\n",i+1,pstSndPkt->stVideoInPara.s32CscLuma[i]);
        AV_PRINT("get ch%d vi para, s32CscContr=%d\n",i+1,pstSndPkt->stVideoInPara.s32CscContr[i]);
        AV_PRINT("get ch%d vi para, s32CscSatu=%d\n",i+1,pstSndPkt->stVideoInPara.s32CscSatu[i]);
        AV_PRINT("get ch%d vi para, s32CscHue=%d\n",i+1,pstSndPkt->stVideoInPara.s32CscHue[i]);
        AV_PRINT("get ch%d vi para, s32Sharpness=%d\n",i+1,pstSndPkt->stVideoInPara.s32Sharpness[i]);
    }
    *ps32SndLen = sizeof(VI_PARA_T); //ack data len
    return HI_SUCCESS;

}
/*****************************************************************************
 Prototype    : DOMAIN_SetViPara
 Description  : 设置视频输入参数
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/2
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SetViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    VI_PARA_T stViPara;
    HI_S32 s32Ret = HI_SUCCESS;
    AV_PRINT("set vi para, cmd(0x%02x)", pstRcvPkt->head.pkt_cmd);
    int i = 0;
    for(i=0; i<APP_VI_CHNCNT; i++)
    {
        AV_PRINT("get ch%d vi para, s32CscLuma=%d\n",i+1,pstRcvPkt->stVideoInPara.s32CscLuma[i]);
        AV_PRINT("get ch%d vi para, s32CscContr=%d\n",i+1,pstRcvPkt->stVideoInPara.s32CscContr[i]);
        AV_PRINT("get ch%d vi para, s32CscSatu=%d\n",i+1,pstRcvPkt->stVideoInPara.s32CscSatu[i]);
        AV_PRINT("get ch%d vi para, s32CscHue=%d\n",i+1,pstRcvPkt->stVideoInPara.s32CscHue[i]);
        AV_PRINT("get ch%d vi para, s32Sharpness=%d\n",i+1,pstRcvPkt->stVideoInPara.s32Sharpness[i]);
    }
    //get vi para copy to local
    memcpy(&stViPara, &pstRcvPkt->stVideoInPara , sizeof(VI_PARA_T));
    s32Ret =  APP_SetVideoInPara(&stViPara);
    if(s32Ret!=0)
        s32Ret = 1;
    pstSndPkt->head.dst_id = ID_UI;
    *ps32SndLen = 4; //ack data len

    return s32Ret;

}
HI_S32 DOMAIN_SetChnViPara(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    VI_PARA_CHN_T stViPara;
    AV_PRINT("set chn%d vi para, s32CscLuma=%d\n",pstRcvPkt->stVideoInChnPara.s32Chn,pstRcvPkt->stVideoInChnPara.s32CscLuma);
    AV_PRINT("set chn%d vi para, s32CscContr=%d\n",pstRcvPkt->stVideoInChnPara.s32Chn,pstRcvPkt->stVideoInChnPara.s32CscContr);
    AV_PRINT("set chn%d vi para, s32CscSatu=%d\n",pstRcvPkt->stVideoInChnPara.s32Chn,pstRcvPkt->stVideoInChnPara.s32CscSatu);
    AV_PRINT("set chn%d vi para, s32CscHue=%d\n",pstRcvPkt->stVideoInChnPara.s32Chn,pstRcvPkt->stVideoInChnPara.s32CscHue);
    AV_PRINT("set chn%d vi para, s32Sharpness=%d\n",pstRcvPkt->stVideoInChnPara.s32Chn,pstRcvPkt->stVideoInChnPara.s32Sharpness);
    memcpy(&stViPara, &pstRcvPkt->stVideoInChnPara , sizeof(VI_PARA_CHN_T));
    APP_SetVideoInPara_Chn(&stViPara);
    *ps32SndLen = 0; //ack data len
    return HI_SUCCESS;
}

/****************************************************************************
AV_SOCKET_MSG
*****************************************************************************/
/*****************************************************************************
 函 数 名  : DOMAIN_Snapshot
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_Snapshot(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
//    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    AV_PRINT("snapshot ");
    s32Ret = HI3521_SnapShotByChn();
    if (s32Ret == HI_SUCCESS)
    {
        pstSndPkt->stSnapshot.s32Ret = TAKE_PHOTO_OK;
    }
    else
    {
        pstSndPkt->stSnapshot.s32Ret = TAKE_PHOTO_ERROR;
    }

    pstSndPkt->head.dst_id = ID_UI;
    *ps32SndLen = sizeof(SNAPSHOT_T);
    //上传线程
    return HI_SUCCESS;
}


/****************************************************************************
AV_SOCKET_MSG
*****************************************************************************/
/*****************************************************************************
 函 数 名  : DOMAIN_RecDisplay
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_RecDisplay(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    HI_CHAR timeBufFrom[32] = {0};
    HI_CHAR timeBufTo[32] = {0};
    struct tm *pTm = NULL;
    pTm = localtime((time_t*)(&pstRcvPkt->stRecinfo.iStartTime));
    strftime(timeBufFrom, sizeof(timeBufFrom), "%Y-%m-%d %H:%M:%S", pTm);
    pTm = localtime((time_t*)(&pstRcvPkt->stRecinfo.iEndTime));
    strftime(timeBufTo, sizeof(timeBufTo), "%Y-%m-%d %H:%M:%S", pTm);
    AV_PRINT("DOMAIN_RecDisplay From %s to %s", timeBufFrom,timeBufTo);
    memset(&pstSndPkt->stRecinfo,0,sizeof(pstSndPkt->stRecinfo));
    pstSndPkt->stRecinfo.iStartTime = pstRcvPkt->stRecinfo.iStartTime;
    pstSndPkt->stRecinfo.iEndTime = pstRcvPkt->stRecinfo.iEndTime;
    HI3521_SFByTime2((void *)(&pstSndPkt->stRecinfo));
    AV_PRINT("DOMAIN_RecDisplay end");
    debug_print("DOMAIN_RecDisplay",(char*)(&pstSndPkt->stRecinfo),sizeof(pstSndPkt->stRecinfo));
    pstSndPkt->head.dst_id = ID_UI;
    *ps32SndLen = sizeof(REC_MONTH_ST);
    return s32Ret;
}


/*****************************************************************************
 函 数 名  : DOMAIN_Snapshot
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_RecQuery(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;

    HI_CHAR timeBufFrom[32] = {0};
    HI_CHAR timeBufTo[32] = {0};
    struct tm *pTm = NULL;
    pTm = localtime((time_t*)(&pstRcvPkt->stRecreq.req_time.uiStartTime));
    strftime(timeBufFrom, sizeof(timeBufFrom), "%Y-%m-%d %H:%M:%S", pTm);
    pTm = localtime((time_t*)(&pstRcvPkt->stRecreq.req_time.uiEndTime));
    strftime(timeBufTo, sizeof(timeBufTo), "%Y-%m-%d %H:%M:%S", pTm);
    AV_PRINT("DOMAIN_RecQuery From %s to %s", timeBufFrom,timeBufTo);

    pstSndPkt->head.dst_id = ID_UI;
    HI3521_LoaclRecQuery(pstRcvPkt->stRecreq.req_time);
    *ps32SndLen = sizeof(REC_QUREY_REQ_T);
    return s32Ret;
}

HI_S32 DOMAIN_DecChnSwitch(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    LPB_SWITCH_AO_T stLPBAoSwitch;
    AV_PRINT("DOMAIN_DecChnSwitch chn : %d", pstRcvPkt->stDecchn.chn);
    pstSndPkt->head.dst_id = ID_UI;
    Hi3521_SwitchVdecChn(pstRcvPkt->stDecchn.chn);
    HI3521_Setdecchn(pstRcvPkt->stDecchn.chn);
    if(pstRcvPkt->stDecchn.chn == 4)
    {
        stLPBAoSwitch.cSwitchAo = 0;
    }
    else
    {
        stLPBAoSwitch.cSwitchAo = 1;
    }
    DOMAIN_PackageData(ID_BZQ, AV_LPB_AO_SWITCH, (HI_U8 *)&stLPBAoSwitch, sizeof(stLPBAoSwitch));
    *ps32SndLen = sizeof(REC_QUREY_REQ_T);
    return s32Ret;
}

/*****************************************************************************
 函 数 名  : DOMAIN_Preview
 功能描述  :
 输入参数  : HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen
 输出参数  : 无
 返 回 值  :
 修改历史	   :
  1.日	  期   :
	作	  者   :
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_Preview(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    PREVIEW_T stPreview;
    stPreview = pstRcvPkt->stPreview;
    *ps32SndLen = 1;
    pstSndPkt->buff[0] = 0;
    if (HDD_IsLPBRunning())
    {
        AV_PRINT("play back running!!!");
    }
    else if(Hi3521_SwitchPreview(stPreview.u8type)==HI_SUCCESS)
        pstSndPkt->buff[0]= stPreview.u8type;
    AV_PRINT("preview(0x%02x)", stPreview.u8type);
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_RcvBZQCtrlVOIP
 Description  : 接收报站器对讲控制应答
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/9/2
    Author       : mabuyun
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_SndBZQCtrlVOIP(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    //HI_BOOL bAlarmTrigger = HI_FALSE;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    //pstRcvPkt->head.pkt_cmd = AV_VOIP_CTRLAOAI;
    pstSndPkt->buff[0] = pstRcvPkt->buff[0];
    AV_PRINT("DOMAIN_SndBZQCtrlVOIP here (%d)", pstSndPkt->buff[0]);
    *ps32SndLen = 2;
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : DOMAIN_RecTimeResp
 Description  :查询录像时间返回消息
 Input        : HI_VOID *pRcvPkt
                HI_VOID *pSndPkt
                HI_S32 *ps32SndLen
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/9/2
    Author       : mabuyun
    Modification : Created function

*****************************************************************************/
HI_S32 DOMAIN_RecTimeResp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    memcpy(&pstSndPkt->stRecrsp,&pstRcvPkt->stRecrsp,sizeof(REC_QUREY_RSP_T));
    pstSndPkt->stRecrsp.chn++;
    AV_PRINT("@@@@@@@@@@@@@DOMAIN_RecTimeResp  chn : %d Sum:%d",pstSndPkt->stRecrsp.chn,pstSndPkt->stRecrsp.sum);
    int i = 0;
    HI_CHAR timeBufFrom[32] = {0};
    HI_CHAR timeBufTo[32] = {0};
    struct tm *pTm = NULL;
    pTm = localtime((time_t*)(&pstRcvPkt->stRecreq.req_time.uiStartTime));
    strftime(timeBufFrom, sizeof(timeBufFrom), "%Y-%m-%d %H:%M:%S", pTm);
    pTm = localtime((time_t*)(&pstRcvPkt->stRecreq.req_time.uiEndTime));
    strftime(timeBufTo, sizeof(timeBufTo), "%Y-%m-%d %H:%M:%S", pTm);

    for(i = 0; i<pstSndPkt->stRecrsp.sum; i++)
    {
        pTm = localtime((time_t*)(&pstSndPkt->stRecrsp.rec_items[i].rsp_time.uiStartTime));
        strftime(timeBufFrom, sizeof(timeBufFrom), "%Y-%m-%d %H:%M:%S", pTm);
        pTm = localtime((time_t*)(&pstSndPkt->stRecrsp.rec_items[i].rsp_time.uiEndTime));
        strftime(timeBufTo, sizeof(timeBufTo), "%Y-%m-%d %H:%M:%S", pTm);
        AV_PRINT("DOMAIN_RecTimeResp File(%d) From %s to %s",i,timeBufFrom,timeBufTo);
    }
    *ps32SndLen =sizeof(REC_QUREY_RSP_T);
    return HI_SUCCESS;
}

int DOMAIN_SetDevInfo(TM_APP_DEVSTATINFO_S *pstDevInfo)
{
    int iWritePosition = 0;
    if(NULL == pstDevInfo)
    {
        AV_PRINT("#[app] pstDevInfo NULL pointer ");
        return HI_FAILURE;
    }
    pthread_rwlock_wrlock(&g_DevErrInfoList->m_Mutex);
    if(MAX_DEVINFO_COUNT > g_DevErrInfoList->iCount)
    {
        iWritePosition = g_DevErrInfoList->iWritePos;
        g_DevErrInfoList->stDevInfo[iWritePosition].iDevStat = pstDevInfo->iDevStat;
        g_DevErrInfoList->stDevInfo[iWritePosition].iTime = pstDevInfo->iTime;
        g_DevErrInfoList->iCount++;
        g_DevErrInfoList->iWritePos = (g_DevErrInfoList->iWritePos+1)%MAX_DEVINFO_COUNT;
    }
    else
    {
        AV_PRINT("#[app]g_DevErrInfoList FULL iCount = %d", g_DevErrInfoList->iCount);
        return HI_FAILURE;
    }
    pthread_rwlock_unlock(&g_DevErrInfoList->m_Mutex);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SendDevErrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->s32DevErrStat);

    pstSndPkt->s32DevErrStat = pstRcvPkt->s32DevErrStat;
    pstSndPkt->head.dst_id = ID_UI;

    AV_PRINT("send dev stat(0x%02x) to ui", pstRcvPkt->s32DevErrStat.s32DevStat);
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SendDevSvrStat(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    DOMAIN_PACKET_T *pstRcvPkt = (DOMAIN_PACKET_T *)pRcvPkt;
    DOMAIN_PACKET_T *pstSndPkt = (DOMAIN_PACKET_T *)pSndPkt;
    s32Ret = s32Ret;
    *ps32SndLen = sizeof(pstRcvPkt->s32DevErrStat);
    pstSndPkt->s32DevErrStat = pstRcvPkt->s32DevErrStat;
    pstSndPkt->head.dst_id = ID_SCH;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_SndDevStat(HI_VOID)
{
    HI_S32 s32CurrDevStat = 0;
    static HI_S32 s32CurrDevStat_old = 0;
    static HI_S32 s32CurrDevStat1_old = 0;
    DEVERROR_STAT_T stDevErrStat;
    TM_APP_DEVSTATINFO_S stDevInfo;
    s32CurrDevStat = HI3521_GetDevErrStat();
    stDevErrStat.s32DevStat = s32CurrDevStat;
    if ((s32CurrDevStat1_old != s32CurrDevStat) || (!bDevStatRecvOk))
    {
        bDevStatRecvOk = HI_FALSE;
        DOMAIN_PackageData(ID_UI, AV_DEVERR_STAT, (HI_U8 *)&stDevErrStat, sizeof(DEVERROR_STAT_T));
    }
    if((!bDevSvrRecvOk)||(s32CurrDevStat_old!=s32CurrDevStat))
    {
        AV_PRINT("send dev stat to schedule (%#x)", s32CurrDevStat);
        stDevInfo.iDevStat = stDevErrStat.s32DevStat;
        if(!bDevSvrRecvOk)
        {
            stDevInfo.iTime = time((time_t *)NULL)-70;
        }
        else
        {
            stDevInfo.iTime = time((time_t *)NULL);
        }
        DOMAIN_SetDevInfo(&stDevInfo);
    }
    s32CurrDevStat_old = s32CurrDevStat;
    s32CurrDevStat1_old = s32CurrDevStat;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvUiDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("ui recv dev stat now");
    bDevStatRecvOk = HI_TRUE;
    return HI_SUCCESS;
}
HI_S32 DOMAIN_RcvSvrDevStatRsp(HI_VOID *pRcvPkt, HI_VOID *pSndPkt, HI_S32 *ps32SndLen)
{
    AV_PRINT("sch recv dev stat now");
    bDevSvrRecvOk = HI_TRUE;
    return HI_SUCCESS;
}
int DOMAIN_CheckDevInfoTime(time_t iTime)
{
    time_t iLocalTime = 0;
    iLocalTime = time((time_t *)NULL);
    if((iLocalTime - iTime) < 60)
    {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
int DOMAIN_GetDevInfo(TM_APP_DEVSTATINFO_S *pstDevInfo)
{
    int iRet = 0;
    int iReadPosition = 0;
    pthread_rwlock_wrlock(&g_DevErrInfoList->m_Mutex);
    iReadPosition = g_DevErrInfoList->iReadPos;
    if(0 < g_DevErrInfoList->iCount)
    {
        iRet = DOMAIN_CheckDevInfoTime(g_DevErrInfoList->stDevInfo[iReadPosition].iTime);
        if(HI_FAILURE == iRet)
        {
            pthread_rwlock_unlock(&g_DevErrInfoList->m_Mutex);
            return HI_FAILURE;
        }
        pstDevInfo->iDevStat = g_DevErrInfoList->stDevInfo[iReadPosition].iDevStat;
        pstDevInfo->iTime = g_DevErrInfoList->stDevInfo[iReadPosition].iTime;
        g_DevErrInfoList->iReadPos = (g_DevErrInfoList->iReadPos+1)%MAX_DEVINFO_COUNT;
        g_DevErrInfoList->iCount--;
    }
    else
    {
        pthread_rwlock_unlock(&g_DevErrInfoList->m_Mutex);
        return HI_FAILURE;
    }
    pthread_rwlock_unlock(&g_DevErrInfoList->m_Mutex);
    return HI_SUCCESS;
}
int DOMAIN_SendDevInfo(void)
{
    int iRet = 0;
    TM_APP_DEVSTATINFO_S stDevInfo;
    DEVERROR_STAT_T stDevErrStat;
    iRet = DOMAIN_GetDevInfo(&stDevInfo);
    if(HI_SUCCESS != iRet)
    {
        return HI_FAILURE;
    }
    //AV_PRINT("#[app]%s stDevInfo.iDevStat=0x%x ",__FUNCTION__,stDevInfo.iDevStat);
    stDevErrStat.s32DevStat = stDevInfo.iDevStat;
    iRet = DOMAIN_PackageData(ID_SCH, SCH_DEV_SVR_STAT, (HI_U8 *)&stDevErrStat, sizeof(DEVERROR_STAT_T));
    if (HI_SUCCESS != iRet)
    {
        AV_PRINT("#[app]DOMAIN_PackageData err 0x%x", iRet);
    }
    return HI_SUCCESS;
}
HI_VOID DOMAIN_ResetGPIO()
{
    TGPIOCmd stGpio;
    stGpio.cCmd = GPIO_ACTION_STOP;
    stGpio.cFunc = GPIO_CMD_DISK_HEAT;
    DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
    stGpio.cCmd = GPIO_ACTION_STOP;
    stGpio.cFunc = GPIO_CMD_DISK_COOL_DOWN;
    DOMAIN_PackageData(ID_BASIC, BASIC_GPIO_CMD,(HI_U8*)(&stGpio),sizeof(TGPIOCmd));
    g_HardWareTempSta.iGpioCoolDownDisk = GPIO_ACTION_STOP;
    g_HardWareTempSta.iGpioHeatDisk = GPIO_ACTION_STOP;
}
/*****************************************************************************
 函 数 名  : DOMAIN_ProcData
 功能描述  :
 输入参数  : HI_S32 Socketfd, HI_CHAR *buf, HI_S32 len
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_ProcData(HI_S32 Socketfd, HI_CHAR *buf, HI_S32 len)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MSG_PF pMsgFunc = NULL;
    DOMAIN_PACKET_T stRcvPkt;
    DOMAIN_PACKET_T stSndPkt;
    HI_S32 s32SndBufLen = -1;
    HI_S32 sDestId = 0;
    static HI_BOOL bRecvMsgFromBasic = HI_FALSE;
    if (len <= (HI_S32)(sizeof(DOMAIN_PACKET_T)))
    {
        memmove(&stRcvPkt, buf, len);
    }
    else
    {
        AV_PRINT("domain proc cmd, data is out of range.");
    }

//	debug_print((HI_CHAR *)buf, len);

    //AV_PRINT("stRcvPkt.head.src_id(0x%x) stRcvPkt.head.pkt_cmd(0x%x)!",stRcvPkt.head.src_id,stRcvPkt.head.pkt_cmd);
    pMsgFunc = DOMAIN_GetMsgFunc(stRcvPkt.head.src_id, stRcvPkt.head.pkt_cmd);
    if (NULL != pMsgFunc)
    {
        //第一次收到Basic消息，重置MCU的GPIO
        if(stRcvPkt.head.src_id == ID_BASIC
                && bRecvMsgFromBasic == HI_FALSE)
        {
            bRecvMsgFromBasic = HI_TRUE;
            DOMAIN_ResetGPIO();
        }
        s32Ret = pMsgFunc((HI_VOID *)&stRcvPkt, (HI_VOID *)&stSndPkt, &s32SndBufLen);
        if (s32Ret < 0)
        {
            // AV_PRINT("pMsgFunc ret err !");
            return s32Ret;
        }

        if ( -1 == s32SndBufLen )
        {
            //	AV_PRINT("domain no snd data !");
            return s32Ret;
        }
//		AV_PRINT("stRcvPkt.head.dst_id(0x%02x), stRcvPkt.head.src_id(0x%02x)", stRcvPkt.head.dst_id, stRcvPkt.head.src_id);

        /*LOCAL主动发送的 DEST ID 要等于要发送的目的ID*/
        if (stRcvPkt.head.src_id == ID_LOCAL)
        {
            sDestId = stRcvPkt.head.dst_id;
        }
        else
        {
            sDestId = stRcvPkt.head.src_id;
        }
        s32Ret = DOMAIN_SndData(Socketfd, sDestId, stRcvPkt.head.pkt_cmd, stSndPkt.buff, s32SndBufLen);
        if (s32Ret < 0)
        {
            AV_PRINT("domain snd data err.");
            return HI_FAILURE;
        }
    }
    else
    {
        AV_PRINT("srcId(%x) cmdId(%x) is out of range.", stRcvPkt.head.src_id, stRcvPkt.head.pkt_cmd);
        debug_print("Packet buffer",(HI_CHAR *)buf, len);
    }


    return s32Ret;
}


/*****************************************************************************
 函 数 名  : DOMAIN_CreateSocket
 功能描述  : 创建本地套接字
 输入参数  : char *path
             SOCKET_TYPE_E socketType---服务器用或者客户端用标志
             	SOCKET_LISTEN_E = 0,
				SOCKET_CONNECT_E = 1,
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_CreateSocket(const HI_CHAR *path, SOCKET_TYPE_E socketType)
{
    struct sockaddr_un  servAdd;
    socklen_t   server_len;
    HI_S32 sockfd;

    struct timeval tv;
    tv.tv_sec  = 30;
    tv.tv_usec = 0;

    bzero(&servAdd, sizeof(servAdd));
    servAdd.sun_family = PF_UNIX;
    strcpy(servAdd.sun_path, path);
    server_len = sizeof(servAdd);

    if ((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        AV_PRINT("socket err(%s)", ERR_STR);
    }

    //set send time out
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

    if (socketType == SOCKET_LISTEN_E)
    {
        unlink(path);
        if (bind(sockfd, (const struct sockaddr *)&servAdd, sizeof(servAdd)) == -1)
        {
            AV_PRINT("bind err(%s)", ERR_STR);
            exit(-1);
        }

        listen(sockfd, 5);
        return sockfd; 			//返回服务器用的fd
    }
    else if (socketType == SOCKET_CONNECT_E)
    {
        if (connect(sockfd, (const struct sockaddr *)&servAdd, server_len) == -1)
        {
            close(sockfd);
            sockfd = -1;
        }
    }
    return sockfd;
}

/*****************************************************************************
 函 数 名  : DOMAIN_RcvTimeout
 功能描述  : 读超时
 输入参数  : HI_S32 fd, HI_S32 sec
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_RcvTimeout(HI_S32 fd, HI_S32 sec)
{
    fd_set rset;
    struct timeval	tv;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    /*> 0 有文件描述符可读， =0 超时， < 0 无描述符可读*/
    return (select(fd + 1, &rset, NULL, NULL, &tv));
}


/*****************************************************************************
 函 数 名  : DOMAIN_RcvData
 功能描述  : 读超时
 输入参数  : HI_S32 *fd, DataBuf *CycleBuf, HI_CHAR *ReadBuf, HI_S32 ReadLen
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_RcvData(HI_S32 *pSocketFd, DataBuf *CycleBuf, HI_CHAR *ReadBuf, HI_S32 ReadLen)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == pSocketFd) || (NULL == CycleBuf) || (NULL == ReadBuf) || (DOMAIN_BUF_LEN != ReadLen))
    {
        AV_PRINT("domain rcv data paras is invalid.");
        return DOMAIN_PARA_ERR;
    }

    bzero(ReadBuf, DOMAIN_BUF_LEN);
    s32Ret = recv(*pSocketFd, ReadBuf, DOMAIN_BUF_LEN, 0);
    if (s32Ret <= 0)	/*如果连接断开或异常，则跳出循环*/
    {
        close(*pSocketFd);
        *pSocketFd = -1;
        AV_PRINT("recv err(%s)", ERR_STR);
        return DOMAIN_RCV_ERR;
    }
//	AV_PRINT("recv %d bytes", s32Ret);

    s32Ret = CycleBuf->PushData(ReadBuf, s32Ret);
    if (s32Ret < 0)
    {
        CycleBuf->PrintErrMsg("[av][DOMAIN_RcvData]: ", s32Ret);
        return DOMAIN_PUSH_ERR;
    }

    return s32Ret;
}

/*****************************************************************************
 函 数 名  : DOMAIN_ChkData
 功能描述  : 读超时
 输入参数  : DataBuf *CycleBuf, HI_CHAR *ReadBuf, HI_S32 ReadLen
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_ChkData(DataBuf *CycleBuf, HI_CHAR *ReadBuf, HI_S32 ReadLen)
{
    HI_U16 u16DataCrc = 0;
    HI_U16 u16RcvCrc = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == CycleBuf) || (NULL == ReadBuf) || (DOMAIN_BUF_LEN != ReadLen))
    {
        AV_PRINT("domain chk data paras is invalid.");
        return DOMAIN_PARA_ERR;
    }

    /*取数据并处理数据*/
    s32Ret = CycleBuf->PopData(ReadBuf, ReadLen);
    if (s32Ret <= 0)
    {
        if (s32Ret != BUF_EMPUTY)
        {
            CycleBuf->PrintErrMsg("[av][DOMAIN_ChkData]: ", s32Ret);
        }
        return DOMAIN_POP_ERR;
    }
//	AV_PRINT("domain pop data len(%d)", s32Ret);
    /*打印数据*/
    //CycleBuf->DebugPrintHex(ReadBuf, s32Ret);

    /*校验数据*/
    u16DataCrc = crc16((HI_U8 *)ReadBuf, s32Ret - 2);
    u16RcvCrc = ((ReadBuf[s32Ret - 2] << 8) | (ReadBuf[s32Ret - 1] & 0xff));
    if (u16RcvCrc != u16DataCrc)
    {
        AV_PRINT("CRC Err!");
        return DOMAIN_CRC_ERR;
    }

    return s32Ret;
}


/*****************************************************************************
 函 数 名  : DOMAIN_SndData
 功能描述  :
 输入参数  : HI_S32 socketFd, HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_SndData(HI_S32 socketFd, HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen)
{
    HI_S16 totalLen;
    HI_S32 ret = -1;
    HI_S32 id_len = 4;
    HI_U16 u16Crc16;
    const char *id = "####";
    DOMAIN_PACKET_T stSndPkt;

    if (strlen(id) > 4)
    {
        id_len = 4;
    }

    /*
    *	+------+------+------------+----------+------+------+------+-----+
    *	|	4  |  2   |      1     |     1    |  1   |  1   |  X   |  2  |
    *	+------+------+------------+----------+------+------+------+-----+
    *	+------+------+------------+----------+------+------+------+-----+
    *	| $$$$ | 长度 | 目标模块ID | 源模块ID | 包ID | 命令 | DATA | CRC |
    *	+------+------+------------+----------+------+------+------+-----+
    *
    */

    totalLen = id_len + 2 + 4 + dataLen + 2;

    strncpy((HI_CHAR *)stSndPkt.head.pkt_label, "####", id_len);
    stSndPkt.head.pkt_len_h = totalLen >> 8;
    stSndPkt.head.pkt_len_l = totalLen & 0xff;
    stSndPkt.head.dst_id = pktDstId;
    stSndPkt.head.src_id = ID_AV;
    stSndPkt.head.pkt_seq = 00;
    stSndPkt.head.pkt_cmd = pktCmd;
    memcpy(stSndPkt.buff, dataBuf, dataLen);

    u16Crc16 = crc16((HI_U8 *)&stSndPkt, totalLen - 2);
    stSndPkt.buff[dataLen] = u16Crc16 >> 8;
    stSndPkt.buff[dataLen + 1] = u16Crc16 & 0x00FF;
//	stSndPkt.buff[dataLen] = u16Crc16 & 0x00FF;
//	stSndPkt.buff[dataLen + 1] = u16Crc16 >> 8;
    if (0 > socketFd)
    {

    }
    else
    {
        ret = (send(socketFd, &stSndPkt, totalLen, 0) == totalLen);
        if (ret != 1)
        {
            AV_PRINT("send err(%s)", ERR_STR);
            return HI_FAILURE;
        }
    }

    //AV_PRINT("send len(%d), fd(%d)", ret, socketFd);
//	debug_print((HI_CHAR *)&stSndPkt, totalLen);

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : DOMAIN_PackageData
 功能描述  : 把需要从本模块发出去的数据，插入组包且发送
 输入参数  : HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen
 输出参数  : 无
 返 回 值  :
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_PackageData(HI_S32 pktDstId, HI_U8 pktCmd, HI_U8 *dataBuf, HI_S16 dataLen)
{
    HI_S16 totalLen = 0;
    HI_S32 id_len = 4;
    HI_U16 u16Crc16;
    const char *id = "####";
    DOMAIN_PACKET_T stSndPkt;

    if (strlen(id) > 4)
    {
        id_len = 4;
    }

    /*
    *	+------+------+------------+----------+------+------+------+-----+
    *	|	4  |  2   |      1     |     1    |  1   |  1   |  X   |  2  |
    *	+------+------+------------+----------+------+------+------+-----+
    *	+------+------+------------+----------+------+------+------+-----+
    *	| $$$$ | 长度 | 目标模块ID | 源模块ID | 包ID | 命令 | DATA | CRC |
    *	+------+------+------------+----------+------+------+------+-----+
    *
    */

    totalLen = id_len + 2 + 4 + dataLen + 2;

    strncpy((HI_CHAR *)stSndPkt.head.pkt_label, "####", id_len);
    stSndPkt.head.pkt_len_h = totalLen >> 8;
    stSndPkt.head.pkt_len_l = totalLen & 0xff;
    stSndPkt.head.dst_id = pktDstId;
    stSndPkt.head.src_id = ID_LOCAL;
    stSndPkt.head.pkt_seq = 00;
    stSndPkt.head.pkt_cmd = pktCmd;
    if (0 != dataLen)
    {
        memcpy(stSndPkt.buff, dataBuf, dataLen);
    }

    u16Crc16 = crc16((HI_U8 *)&stSndPkt, totalLen - 2);
    stSndPkt.buff[dataLen] = u16Crc16 >> 8;
    stSndPkt.buff[dataLen + 1] = u16Crc16 & 0x00FF;

    //AV_PRINT("package data len(%d)", totalLen);
    //HI3531_DebugPrint((HI_CHAR *)&stSndPkt, totalLen);

    /*把生成的数据包，插入循环队列*/
    DOMAIN_PushData2CycleBuf((HI_CHAR *)&stSndPkt, totalLen);

    return HI_SUCCESS;
}



/****************************************************************/


/*****************************************************************************
 Prototype    : TM_Socket_RegToGate
 Description  : 向gate发送注册包
 Input        : None
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/9/11
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
int DOMAIN_Socket_RegToGate()
{
    HI_U16 u16Crc16;
    HI_S32 socketFd =-1;
    DOMAIN_GetSocket(&socketFd);
    unsigned char regData[12] = {0x23,0x23,0x23,0x23,0x00,0x0C,0x31,0x32,0x01,0x01,0xB3,0xF8};
    u16Crc16 = crc16((HI_U8 *)regData, 10);
    regData[10] = u16Crc16 >> 8;
    regData[11] = u16Crc16 & 0x00FF;
    return send(socketFd, regData, sizeof(regData), 0);
}

/*****************************************************************************
 Prototype    : TM_Socket_SendHeartToGate
 Description  : 向gate发心跳包
 Input        : None
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/11/27
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
int TM_Socket_SendHeartToGate()
{
    HI_U16 u16Crc16;
    HI_S32 socketFd =-1;
    DOMAIN_GetSocket(&socketFd);
    unsigned char heartData[] = {0x23,0x23,0x23,0x23,0x00,0x0C,0x31,0x32,0x01,0x02,0xF3,0xF9};
    u16Crc16 = crc16((HI_U8 *)heartData, 10);
    heartData[10] = u16Crc16 >> 8;
    heartData[11] = u16Crc16 & 0x00FF;
    return send(socketFd, heartData, sizeof(heartData), 0);
}

/*****************************************************************************
 Prototype    : TM_Socket_Connect
 Description  : 连接函数
 Input        : const char *path
 Output       : None
 Return Value : 成功--返回描述符

  History        :
  1.Date         : 2015/9/11
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
int DOMAIN_Socket_Connect(const char *path)
{
    struct sockaddr_un  servAdd;
    socklen_t   server_len;
    HI_S32 SocketFd = -1;
    signal(SIGPIPE, SIG_IGN);  //忽略网络连接断开的信号,防止服务器程序因网络断开而退出

    bzero(&servAdd, sizeof(servAdd));
    servAdd.sun_family = PF_UNIX;
    strcpy(servAdd.sun_path, path);
    server_len = sizeof(servAdd);

    if((SocketFd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        AV_PRINT("socket gate error");
        return -1;
    }
    if(connect(SocketFd, (struct sockaddr *)&servAdd, server_len) == -1)
    {
        //	AV_PRINT(" connect error, fd: %d\n", SocketFd);
        close(SocketFd);
        return -1;
    }
    return SocketFd;
}


/*****************************************************************************
 函 数 名  : DOMAIN_Init_DevErrInfoList
 功能描述  :
 输入参数  :
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2016年4月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
int DOMAIN_Init_DevErrInfoList(void)
{

    g_DevErrInfoList = (TM_APP_DEVSTAT_S *)malloc(sizeof(TM_APP_DEVSTAT_S));

    if (NULL == g_DevErrInfoList)
    {
        AV_PRINT("#[app]malloc memory failed");

        return HI_FAILURE;
    }

    memset(g_DevErrInfoList,0,sizeof(TM_APP_DEVSTAT_S));

    AV_PRINT("#[app] g_DevErrInfoList->iCount = %d,rp=%d,wp=%d ",
             g_DevErrInfoList->iCount,
             g_DevErrInfoList->iReadPos,
             g_DevErrInfoList->iWritePos);

    g_DevErrInfoList->iCount = 0;
    g_DevErrInfoList->iReadPos = 0;
    g_DevErrInfoList->iWritePos = 0;

    if(0 != pthread_rwlock_init(&g_DevErrInfoList->m_Mutex, NULL))
    {
        free(g_DevErrInfoList);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DOMAIN_ProcDataThd
 功能描述  : 处理3ggatesocket消息线程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *DOMAIN_RcvDataThd(HI_VOID *)
{
//    HI_S32 Localfd = -1;
    HI_S32 sockfd_3g = -1;
    HI_S32 s32Ret = -1 ;
    HI_S32 s32Len = 0;
    DataBuf *pCycleBuf = NULL;			   	/*循环队列*/
    char ReadBuf[DOMAIN_BUF_LEN]= {'\0'};	/*接收缓冲区*/
//    socklen_t	peerLen;
//   struct sockaddr_un peerAdd;

//    peerLen = sizeof(peerAdd);
//	Localfd = DOMAIN_CreateSocket(DOMAIN_PATH, SOCKET_LISTEN_E);
    DOMAIN_GetCycleBuf(&pCycleBuf);
    s32Ret = DOMAIN_Init_DevErrInfoList();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("#[app]TM_APP_Init_DevErrInfoList err 0x%x", s32Ret);
    }
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    while (1)
    {
        sockfd_3g = DOMAIN_Socket_Connect(TM_UNIX_SOCKE_PATH);
        if (sockfd_3g < 0)
        {
//			AV_PRINT("accept err(%s)", ERR_STR);
            //sleep(DOMAIN_TIMEOUT);/*当有问题时，等待一段时间后，再次ACCEPT*/
            TM_USleep(DOMAIN_TIMEOUT,0);
            continue;
        }
        else
        {
            DOMAIN_SetSocket(sockfd_3g);
            DOMAIN_Socket_RegToGate();
            AV_PRINT("accept client connect, sockfd(%d)", sockfd_3g);
        }
#ifdef  PLT_PARA_BK
        //向调度获取视频后台主副IP地址
        DOMAIN_PackageData(ID_SCH, SCH_SET_PLT_INFO,NULL,0);
        //向UI主动发送当前链接视频后台的IP地址
        DevSndPlatInfo2UI();
#endif
        //向UI发送AV功能配置
        DOMAIN_PackageData(ID_UI, AV_FUNC_CONFIG,NULL,0);
        while (1)   /*循环接收*/
        {
            /*根据变化发送状态*/
            DOMAIN_SndDevStat();
            /* BEGIN: Added by duzhanhui, 2016/11/8  DES:自检包延迟发送 PN:16110101044 */
            DOMAIN_SendDevInfo();
            /* END:   Added by duzhanhui, 2016/11/8   PN:16110101044 */
            /* BEGIN: Added by mabuyun, 2015/9/6  DES:添加IP对讲功能  PN:20150902 */
            DOMAIN_SVRLoginStat();
            /* END:   Added by mabuyun, 2015/9/6   PN:20150902 */

            s32Ret = DOMAIN_RcvTimeout(sockfd_3g, DOMAIN_TIMEOUT);
            if (0 == s32Ret )
            {
                //如果有报警事件，就一秒重发一次; 没有报警事件的话该函数直接返回
//    			AV_PRINT("select rd time out");
                continue;
            }

            s32Len = DOMAIN_RcvData(&sockfd_3g, pCycleBuf, ReadBuf, sizeof(ReadBuf));
            if ((DOMAIN_PARA_ERR == s32Len) || (DOMAIN_RCV_ERR == s32Len))
            {
                break;
            }
            TM_USleep(0,20*1000);
        }
    }

    return NULL;
}

/*****************************************************************************
 函 数 名  : DOMAIN_ProcDataThd
 功能描述  : 处理3ggatesocket消息线程
 输入参数  : 无
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *DOMAIN_SndDataThd(HI_VOID *)
{
    HI_S32 socketFd = -1;
    HI_S32 s32Ret = -1 ,num;
    DataBuf *pCycleBuf = NULL;			   	/*循环队列*/
    char ReadBuf[DOMAIN_BUF_LEN] = {0};		/*接收缓冲区*/

    DOMAIN_GetCycleBuf(&pCycleBuf);
    DOMAIN_GetSocket(&socketFd);
    AV_PRINT("snd sockfd(%d)", socketFd);
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    while (1)
    {
        s32Ret = DOMAIN_ChkData(pCycleBuf, ReadBuf, sizeof(ReadBuf));
        if (HI_SUCCESS > s32Ret)
        {
            if(num++ == 6000)
            {
                num = 0;
                TM_Socket_SendHeartToGate();
            }
            //usleep(5000);	/*NO DATA，WAIT FOR SCHEDULE AGAIN.*/
            TM_USleep(0,10000);
            continue;
        }

        DOMAIN_GetSocket(&socketFd);
        if (0 > socketFd)
        {
            //usleep(200000);/*NO VALID SOCKET，WAIT FOR SCHEDULE AGAIN.*/
            TM_USleep(0,200000);
            continue;
        }

        s32Ret = DOMAIN_ProcData(socketFd, ReadBuf, s32Ret - 2);
        if (s32Ret < 0)
        {
            AV_PRINT("snd data err");
        }
        TM_USleep(0,10000);
    }

    return NULL;
}






/*****************************************************************************
 函 数 名  : HI3531_StartDomainMsgThd
 功能描述  : 启动视频存储
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_StartRcvDataThd(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;

    s32Ret = pthread_create(&tid, NULL, DOMAIN_RcvDataThd, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("create DOMAIN_StartRcvDataThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach DOMAIN_StartRcvDataThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : DOMAIN_StartRcvDataThd
 功能描述  : 启动视频存储
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 DOMAIN_StartSndDataThd(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;

    s32Ret = pthread_create(&tid, NULL, DOMAIN_SndDataThd, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("create DOMAIN_StartSndDataThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach DOMAIN_StartSndDataThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


