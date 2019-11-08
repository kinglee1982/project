#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <pthread.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <arpa/inet.h>
#include "TM_Alarm.h"
#include "Nvs.h"
#include "TM_AlarmRecord.h"
#include "tm_app_movedetect.h"



#if 0
nvs::AlarmNotification pAlarm;

COMM_ALARM_ST g_stCommAlarmMng;
_ALARM_EVENT_LOG_ST g_stEventLogDisp[4] =
{
		{ALARM_TYPE_TRIGGER, 	"io alarm stat: "},

		{ALARM_TYPE_MD, 		"move detect stat: "},
};

void convertTime2Str(time_t t, char *pTime, COMM_TIME_ST *pstCommTime)
{
    struct tm *tmp;

    if (NULL == pTime)
    {
        return;
    }
    tmp = localtime(&t);
    sprintf(pTime,"%04d-%02d-%02d %02d:%02d:%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	pstCommTime->year = tmp->tm_year+1900;
	pstCommTime->mon = tmp->tm_mon+1;
	pstCommTime->day = tmp->tm_mday;
	pstCommTime->hour = tmp->tm_hour;
	pstCommTime->min = tmp->tm_min;
	pstCommTime->sec = tmp->tm_sec;
	pstCommTime->week = tmp->tm_wday;

}



/*将XML里读到的结构全部填到报警需要的结构中 */
void readAlarmParam(WEBOCX_NODE_SET_ST *pstAlarmNodeSet)
{
	int i = 0;
	int s32Ret = -1;
    TRIGGER_ALARM_ST stTriggerAlarm[MAX_TRIGGER_ALARM_NUMS];
    TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	for (i = 0; i < MAX_TRIGGER_ALARM_NUMS; i++)
	{
		pstTriggerAlarm = &g_stCommAlarmMng.stTriggerAlarm[i];
		pstTriggerAlarm->bEnable = pstAlarmNodeSet->alarmstat;
		pstTriggerAlarm->s32BeepTime = pstAlarmNodeSet->BeepOutDuration;
		pstTriggerAlarm->s32BeepEnable = pstAlarmNodeSet->BeepEnable;
		/*联动相关数据 */
		pstTriggerAlarm->LinkOutput = pstAlarmNodeSet->LinkOutput;
		pstTriggerAlarm->LinkRecDuration = pstAlarmNodeSet->LinkRecDuration;
		pstTriggerAlarm->AlarmOutDuration = pstAlarmNodeSet->AlarmOutDuration;
		pstTriggerAlarm->LinkSnap = pstAlarmNodeSet->LinkSnap;
		pstTriggerAlarm->LinkSnap = pstAlarmNodeSet->LinkRecord;

		/*时间*/
		memcpy(pstTriggerAlarm->stTriggerTimeSeg, pstAlarmNodeSet->AlertTime,
			sizeof(ALARM_TIME_SEG_ST)*ALARM_TIME_SEG_DAYS*ALARM_TIME_SEG_NUMS);
	}

	return;
}

/*将XML里读到的结构全部填到报警需要的结构中 */
void readMDParam(WEBOCX_MD_SET_ST *pstMdSet)
{
	int i = 0;
	int s32Ret = -1;
    MD_ChnAttr_st    stMdAttr[VIDEOINPUT_MAX_CHANNEL_NUM];
    MD_ChnAttr_st    *pMdChnAttr = NULL;

	for (i = 0; i < VIDEOINPUT_MAX_CHANNEL_NUM; i++)
	{
		pMdChnAttr = &g_stCommAlarmMng.stMdAttr[i];
		pMdChnAttr->areaNum = 4;
		/*联动相关数据 */

		/*时间*/
	}

	return;
}

/*使能布防*/
void SetMdEnable(int chnno)
{
    MD_ChnAttr_st    stMdAttr[VIDEOINPUT_MAX_CHANNEL_NUM];
    MD_ChnAttr_st    *pMdChnAttr = NULL;

	pMdChnAttr = &g_stCommAlarmMng.stMdAttr[chnno-1];
	pMdChnAttr->enableflag = 1;
	return;
}

/*取消布防*/
void SetMdDisable(int chnno)
{
    MD_ChnAttr_st    stMdAttr[VIDEOINPUT_MAX_CHANNEL_NUM];
    MD_ChnAttr_st    *pMdChnAttr = NULL;

	pMdChnAttr = &g_stCommAlarmMng.stMdAttr[chnno];
	pMdChnAttr->enableflag = 0;
	return;
}

/*设置处理失效 */
void setAlarmHandleDisable(int alarmno)
{
    TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	pstTriggerAlarm = &g_stCommAlarmMng.stTriggerAlarm[alarmno - 1];
	memset(&pstTriggerAlarm->stAlarmHandle, 0, sizeof(ALARM_HANDLE_T));

	return;
}

/*报警复位 */
void SetAlarmReset(int alarmno)
{
	_COMM_ALARM_STAT_ST *pstCommAlarmStat = NULL;
	pstCommAlarmStat = &g_stCommAlarmMng.stCommAlarmStat;
	pstCommAlarmStat->s32IoAlarmStat = 0;

	setAlarmHandleDisable(alarmno);
	return;
}

/*使能报警检测 */
void SetAlarmEnable(int alarmno)
{
    TRIGGER_ALARM_ST stTriggerAlarm[MAX_TRIGGER_ALARM_NUMS];
    TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	pstTriggerAlarm = &g_stCommAlarmMng.stTriggerAlarm[alarmno - 1];
	pstTriggerAlarm->bEnable = 1;

	return;
}

/*取消报警检测 */
void SetAlarmDisable(int alarmno)
{
    TRIGGER_ALARM_ST stTriggerAlarm[MAX_TRIGGER_ALARM_NUMS];
    TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	pstTriggerAlarm = &g_stCommAlarmMng.stTriggerAlarm[alarmno - 1];
	pstTriggerAlarm->bEnable = 0;

	return;
}

/*获取报警状态 */
int GetCurrIoAlarmStat()
{
	_COMM_ALARM_STAT_ST *pstCommAlarmStat = NULL;
	pstCommAlarmStat = &g_stCommAlarmMng.stCommAlarmStat;

	return pstCommAlarmStat->s32IoAlarmStat;
}

/*获取MD状态 */
int GetCurrMdStat()
{
	_COMM_ALARM_STAT_ST *pstCommAlarmStat = NULL;
	pstCommAlarmStat = &g_stCommAlarmMng.stCommAlarmStat;

	XML_DEBUG("GetCurrMdStat is 0x%x.\n", pstCommAlarmStat->s32MdStat);
	return (pstCommAlarmStat->s32MdStat);
}

/*设置 MD STAT  通道 1-8 */
void SetCurrMdStat(int channo)
{
	int iTmpStat = GetCurrMdStat();

	_COMM_ALARM_STAT_ST *pstCommAlarmStat = NULL;
	pstCommAlarmStat = &g_stCommAlarmMng.stCommAlarmStat;

	iTmpStat |= (1>>channo);

	pstCommAlarmStat->s32MdStat = iTmpStat;
	XML_DEBUG("SetCurrMdStat is 0x%x.\n", pstCommAlarmStat->s32MdStat);

	return;
}



/*获取报警通道号  暂时先返回  1*/
int GetCurrIoAlarmNo()
{
	int s32Alarmno = 0;
	int s32AlarmStat = 0;

	s32AlarmStat = GetCurrIoAlarmStat();
	s32Alarmno = 1;
	return (s32Alarmno -1);
}

/*写报警事件给XML
  */
void WriteAlarmStat2Xml(ALARM_XML_LOG_ST *pAlarmXmlLog , bool bTrigger)
{

	/*触发 */
	if(bTrigger)
	{
		/*组装数据 */

	}
	else
	{
		/*组装数据 */

	}

	return;
}


/*发送报警状态给AV-OCX*/
void SendAlarmStat2Av(int s32Alarmno, bool bAlarmTrigger)
{
	return;
}

/*发送报警状态给SIP协议*/
void SendAlarmStat2Sip(int s32Alarmno, bool bAlarmTrigger)
{

	return;
}


/*发送移动侦测状态给AV-OCX*/
void SendMDStat2Av(int channo, bool bMDTrigger)
{
	return;
}

/*发送移动侦测状态给SIP协议*/
void SendMDStat2Sip(int channo, bool bMDTrigger)
{

	return;
}
/*BEEP */
void Alarmbeep(bool bStart)
{
	return;
}

/*alarm 时间比较 */
bool AlarmTimeCheck(COMM_TIME_ST *pstSrcCommTime, int s32AlarmNo, ALARM_TYPE_EM emAlarmType)
{
	int i = 0;
	int iTmpWeek = 0;
	TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	pstTriggerAlarm = &(g_stCommAlarmMng.stTriggerAlarm[s32AlarmNo]);
	ALARM_TIME_SEG_ST stTimeSeg;

	memset(&stTimeSeg, 0, sizeof(ALARM_TIME_SEG_ST));
	/*周和平台的进行一个转换 */
	switch(pstSrcCommTime->week)
		{
			case 0:
				iTmpWeek = 6;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				iTmpWeek = pstSrcCommTime->week -1;
			case 6:
				iTmpWeek = 0;
				default:
					XML_DEBUG("AlarmTimeCheck input week error.%d\n", pstSrcCommTime->week);
					break;

		}

	if (ALARM_TYPE_TRIGGER == emAlarmType)
	{
		for(i = 0; i < ALARM_TIME_SEG_NUMS; i++)
		{
			memcpy(&stTimeSeg, &(pstTriggerAlarm->stTriggerTimeSeg[iTmpWeek][i]), sizeof(ALARM_TIME_SEG_ST));

			if((pstSrcCommTime->hour >= stTimeSeg.startHour && pstSrcCommTime->hour <= stTimeSeg.endHour)
				&&(pstSrcCommTime->min >= stTimeSeg.startMin && pstSrcCommTime->min <= stTimeSeg.endMin)
				&&(pstSrcCommTime->sec >= stTimeSeg.startSec && pstSrcCommTime->sec <= stTimeSeg.endSec))
			{
				return true;
			}
		}
	}
	else if (ALARM_TYPE_MD == emAlarmType)
	{

	}

	return false;
}

void *TM_Device_alarm(void *arg)
{
	int socketfd = *((int*)arg);
	int s32Chn = 0;
	int s32IoAlarmTicks = 0;
	int s32IoAlarminStat = 0;
	int s32MdAlarmStat = 0;    /*位的方式过来的数据 */
	int s32AlarmNo = 0;
	int s32MdChn = 0;
	time_t currIoAlarmTime = 0;
	time_t currMdAlarmTime = 0;


	char szTime[64] = {0};
	char szAlarmEvenvt[64] = {0};
	ALARM_XML_LOG_ST stAlarmXmlLog;
	TRIGGER_ALARM_ST *pstTriggerAlarm = NULL;
	ALARM_HANDLE_T   *pstMdHandle = NULL;
	XML_DEBUG("TM_Device_alarm:%d\n",socketfd);

	while(1)
	{
		//报警处理获，取报警状态
		s32IoAlarminStat = GetCurrIoAlarmStat();
		if (s32IoAlarminStat == IO_ALARM_TRIGGER)
		{
			s32IoAlarmTicks++;
			if (s32IoAlarmTicks >= ALARM_CHECK_TIMES)
			{
				//记录报警事件
				currIoAlarmTime = time(NULL);
				s32AlarmNo = GetCurrIoAlarmNo();
				stAlarmXmlLog.s32IoAlarmNo = s32AlarmNo;
				stAlarmXmlLog.bTrigger = ALARM_TRIGGER_START;
				stAlarmXmlLog.emAlarmType = ALARM_TYPE_TRIGGER;
				sprintf(szAlarmEvenvt, "%s%02d:%02d",g_stEventLogDisp[stAlarmXmlLog.emAlarmType].evenvDisp
						,stAlarmXmlLog.emAlarmType, s32AlarmNo);
				convertTime2Str(currIoAlarmTime, szTime, &stAlarmXmlLog.stStartTime);

				memcpy(stAlarmXmlLog.szEventName, szAlarmEvenvt, LOG_EVENV_DISP_LEN);
				if(AlarmTimeCheck(&stAlarmXmlLog.stStartTime, s32AlarmNo, stAlarmXmlLog.emAlarmType))
				{
					pstTriggerAlarm = &(g_stCommAlarmMng.stTriggerAlarm[s32AlarmNo]);

					/*beep start*/
					Alarmbeep(ALARM_TRIGGER_START);

					/*记录报警*/
					if(pstTriggerAlarm->stAlarmHandle.bWriteLog == 0)
					{
						WriteAlarmStat2Xml(&stAlarmXmlLog, ALARM_TRIGGER_START);
						pstTriggerAlarm->stAlarmHandle.bWriteLog = 1;
					}

					//上传给SIP
					if(pstTriggerAlarm->stAlarmHandle.bSendSip == 0)
					{
						SendAlarmStat2Sip(s32AlarmNo, ALARM_TRIGGER_START);
						pstTriggerAlarm->stAlarmHandle.bSendSip = 1;
					}

					//发给AV
					if(pstTriggerAlarm->stAlarmHandle.bSendAv == 0)
					{
						SendAlarmStat2Av(s32AlarmNo, ALARM_TRIGGER_START);
						pstTriggerAlarm->stAlarmHandle.bSendAv = 1;
					}
				}

			}

		}
		else
		{
			s32IoAlarmTicks = 0;
			/*beep over*/
			Alarmbeep(ALARM_TRIGGER_END);

			/*报警关闭的处理 */
			/*写日志处理 */
			s32IoAlarmTicks = 0;
			currIoAlarmTime = time(NULL);
			s32AlarmNo = GetCurrIoAlarmNo();
			stAlarmXmlLog.s32IoAlarmNo = s32AlarmNo;
			stAlarmXmlLog.bTrigger = ALARM_TRIGGER_END;
			convertTime2Str(currIoAlarmTime, szTime, &stAlarmXmlLog.stStartTime);
			pstTriggerAlarm = &(g_stCommAlarmMng.stTriggerAlarm[s32AlarmNo]);
			sprintf(szAlarmEvenvt, "%s%02d:%02d",g_stEventLogDisp[stAlarmXmlLog.emAlarmType].evenvDisp
						,stAlarmXmlLog.emAlarmType, s32AlarmNo);
			memcpy(stAlarmXmlLog.szEventName, szAlarmEvenvt, LOG_EVENV_DISP_LEN);

			/*之前记录了报警，再记录一条报警消除*/
			if(pstTriggerAlarm->stAlarmHandle.bWriteLog == 1)
			{
				WriteAlarmStat2Xml(&stAlarmXmlLog, ALARM_TRIGGER_END);
				pstTriggerAlarm->stAlarmHandle.bWriteLog = 0;
			}

			//上传给SIP
			if(pstTriggerAlarm->stAlarmHandle.bSendSip == 1)
			{
				SendAlarmStat2Sip(s32AlarmNo, ALARM_TRIGGER_END);
				pstTriggerAlarm->stAlarmHandle.bSendSip = 0;
			}

			//发给AV
			if(pstTriggerAlarm->stAlarmHandle.bSendAv == 1)
			{
				SendAlarmStat2Av(s32AlarmNo, ALARM_TRIGGER_END);
				pstTriggerAlarm->stAlarmHandle.bSendAv = 0;
			}

		}

		//移动侦测处理的处理
		for (s32Chn = 0; s32Chn < ALL_CHN_NUMS; s32Chn++)
		{
			s32MdAlarmStat = GetCurrMdStat();
			if ((s32MdAlarmStat >> s32Chn)&0x1 == 0x1)
			{
				//记录报警事件
				currMdAlarmTime = time(NULL);
				s32MdChn = s32Chn;
				stAlarmXmlLog.s32IoAlarmNo = s32AlarmNo;
				stAlarmXmlLog.bTrigger = ALARM_TRIGGER_START;
				stAlarmXmlLog.emAlarmType = ALARM_TYPE_MD;
				sprintf(szAlarmEvenvt, "%s%02d:%02d",g_stEventLogDisp[stAlarmXmlLog.emAlarmType].evenvDisp
						 ,stAlarmXmlLog.emAlarmType, s32MdChn+1);
				convertTime2Str(currMdAlarmTime, szTime, &stAlarmXmlLog.stStartTime);

				memcpy(stAlarmXmlLog.szEventName, szAlarmEvenvt, LOG_EVENV_DISP_LEN);
				if(AlarmTimeCheck(&stAlarmXmlLog.stStartTime, s32MdChn, stAlarmXmlLog.emAlarmType))
				{
					pstMdHandle = &(g_stCommAlarmMng.stMdHandle[s32MdChn]);

					/*beep start*/
					Alarmbeep(ALARM_TRIGGER_START);

					/*记录报警*/
					if(pstMdHandle->bWriteLog == 0)
					{
						WriteAlarmStat2Xml(&stAlarmXmlLog, ALARM_TRIGGER_START);
						pstMdHandle->bWriteLog = 1;
					}

					//上传给SIP
					if(pstMdHandle->bSendSip == 0)
					{
						SendAlarmStat2Sip(s32AlarmNo, ALARM_TRIGGER_START);
						pstMdHandle->bSendSip = 1;
					}

					//发给AV
					if(pstMdHandle->bSendAv == 0)
					{
						SendAlarmStat2Av(s32AlarmNo, ALARM_TRIGGER_START);
						pstMdHandle->bSendAv = 1;
					}
				}			}
			else
			{
				/*写日志处理 */
				s32IoAlarmTicks = 0;
				currMdAlarmTime = time(NULL);
				s32AlarmNo = GetCurrIoAlarmNo();
				stAlarmXmlLog.s32IoAlarmNo = s32AlarmNo;
				stAlarmXmlLog.bTrigger = ALARM_TRIGGER_END;
				convertTime2Str(currMdAlarmTime, szTime, &stAlarmXmlLog.stStartTime);

				sprintf(szAlarmEvenvt, "%s%02d:%02d",g_stEventLogDisp[stAlarmXmlLog.emAlarmType].evenvDisp
							,stAlarmXmlLog.emAlarmType, s32AlarmNo);
				memcpy(stAlarmXmlLog.szEventName, szAlarmEvenvt, LOG_EVENV_DISP_LEN);
				/*之前记录了报警，再记录一条报警消除*/

				pstMdHandle = &(g_stCommAlarmMng.stMdHandle[s32MdChn]);

				if(pstMdHandle->bWriteLog == 1)
				{
					WriteAlarmStat2Xml(&stAlarmXmlLog, ALARM_TRIGGER_END);
					pstMdHandle->bWriteLog = 0;
				}

				//上传给SIP
				if(pstMdHandle->bSendSip == 1)
				{
					SendAlarmStat2Sip(s32AlarmNo, ALARM_TRIGGER_END);
					pstMdHandle->bSendSip = 0;
				}

				//发给AV
				if(pstMdHandle->bSendAv == 1)
				{
					SendAlarmStat2Av(s32AlarmNo, ALARM_TRIGGER_END);
					pstMdHandle->bSendAv = 0;
				}

				/*beep over*/
				Alarmbeep(ALARM_TRIGGER_END);
			}

		}

		usleep(1000*20); //20ms

	}

}

#endif


nvs::AlarmNotification alarminfo;

int cmd_sockfd = 0;
int bootflag = 0;
void restdevice()
{
	bootflag = 1;
}

device_camer_id sipcamerid[CHNNUM];
int restalarmflag[CHNNUM];
int avalarmflag[CHNNUM];

/*视频丢失配置参数*/
int chnumlsoeflag[32] = {0};
VOID_LOSE_SEND_AV_STAT alarmvoidestat;


/*移动侦测配置参数*/
ALARM_SEND_AV_STAT alarmMDstat;
MD_ChnAttr_st alarmMDparam[CHNNUM];
int chnumMDflag[32] = {0};
Alarm_record_param md_alarm_record[CHNNUM];
time_t md_time;

/*SIP报警配置参数*/
char alarmenableflag = 0x00;

#if 0
void PrintalarmMDparam()
{
	XML_DEBUG("11111111111@@@@@@@@@@@@@@@@@@@PrintalarmMDparam \n");

	int i = 0;
	int j = 0;
	int m = 0;
	int n = 0;

	for(i=0;i<4;i++)
	{

		XML_DEBUG("alarmMDparam[%d].enableflag:%d\n",i,alarmMDparam[i].enableflag);
		XML_DEBUG("alarmMDparam[%d].macro_threshold:%d\n",i,alarmMDparam[i].macro_threshold);
		XML_DEBUG("alarmMDparam[%d].interval:%d\n",i,alarmMDparam[i].interval);
		XML_DEBUG("alarmMDparam[%d].areaNum:%d\n",i,alarmMDparam[i].areaNum);

		for(j=0;j<4;j++)
		{
			XML_DEBUG("alarmMDparam[%d].recattr[%d].x:%d\n",i,j,alarmMDparam[i].recattr[j].x);
			XML_DEBUG("alarmMDparam[%d].recattr[%d].y:%d\n",i,j,alarmMDparam[i].recattr[j].y);
			XML_DEBUG("alarmMDparam[%d].recattr[%d].higth:%d\n",i,j,alarmMDparam[i].recattr[j].higth);
			XML_DEBUG("alarmMDparam[%d].recattr[%d].width:%d\n",i,j,alarmMDparam[i].recattr[j].width);
		}
	}
	XML_DEBUG("2222222222@@@@@@@@@@@@@@@@@@@PrintalarmMDparam \n");

}
#endif

int Setalarmenableflag(const char* id, int flag)
{
	int i = 0;

	/*布防通道上报设置(0撤防，1布防)*/
	for(i=0;i<Getcameranumstr();i++)
	{
		//if((strcmp(id,GetXmlText(cameranumstr[i],"Id"))) == 0)
		XML_DEBUG("sipcamerid[%d].CameraSIPID:%s\n",i,sipcamerid[i].CameraSIPID);
		if((strcmp(id,sipcamerid[i].CameraSIPID)) == 0)
		{
			#if 0
			XML_DEBUG("____++++++\n%d",__LINE__);
			XML_DEBUG("alarmMDparam[%d].enableflag:%d\n",i,alarmMDparam[i].enableflag);
			XML_DEBUG("alarmMDparam[%d].macro_threshold:%d\n",i,alarmMDparam[i].macro_threshold);
			XML_DEBUG("alarmMDparam[%d].interval:%d\n",i,alarmMDparam[i].interval);
			XML_DEBUG("alarmMDparam[%d].areaNum:%d\n",i,alarmMDparam[i].areaNum);
			#endif

			if(flag == 1)
			{
				alarmMDparam[i].enableflag = 1;
				XML_DEBUG("TM_Start_Alarm alarmenableflag:%x\n",alarmenableflag);
				//TM_APP_UpdateMdPara(i,&alarmMDparam[i]);

				alarmenableflag |= (0x01<<i);
				XML_DEBUG("TM_Start_Alarm alarmenableflag:%x\n",alarmenableflag);
				break;
			}
			else if(flag == 0)
			{
				XML_DEBUG("TM_Start_Alarm alarmflag:%x\n",alarmenableflag);
				alarmMDparam[i].enableflag = 0;
				//TM_APP_UpdateMdPara(i,&alarmMDparam[i]);
				alarmenableflag &= (0x0E<<i);
				XML_DEBUG("TM_Start_Alarm alarmflag:%x\n",alarmenableflag);
				break;
			}

		}
	}
	//TM_APP_SetMdParaUpdate();

	return 0;
}



/*设置设备视频丢失参数*/
int SetDeviceViodeLoseParam(WEBOCX_LOSTFRAM_ST *voideparam)
{
#if 0
	/* 非0摄像头存在，0视频丢失 */
	int TM_APP_CheckCameraStatue(VENC_CHN chn);
#endif
	int i = 0;
	for(i=0;i<4;i++)
	{
		XML_DEBUG("SetDeviceViodeLoseParam:%d\n",__LINE__);
		if(((voideparam->ChnBit>>i)&0x01)  == 1)
		{
			XML_DEBUG("voideparam->ChnBit:%d\n",voideparam->ChnBit);
			XML_DEBUG("ViodeLose chnum is:%d\n",i);
			//chnumlsoeflag[i] = TM_APP_CheckCameraStatue(i);
			XML_DEBUG("chnumlsoeflag[%d]is:%d\n",i,chnumlsoeflag[i]);
		}
	}

	return 0;
}


int SetDeviceMD()
{
	int i = 0;

	for(i=0;i<4;i++)
	{
		//TM_APP_UpdateMdPara(i,&alarmMDparam[i]);

		#if 0
		PrintalarmMDparam();
		#endif
	}
	//TM_APP_SetMdParaUpdate();

	return 0;
}


/*设置设备移动侦测参数*/
int SetDeviceMDParam()
{
	int i = 0;
	int j = 0;
	int m = 0;
	int n = 0;

	for(i=0;i<4;i++)
	{
		alarmMDparam[i].enableflag = voidemdparam[i].AlertEnable;
		XML_DEBUG("alarmMDparam[%d].enableflag:%d\n",i,alarmMDparam[i].enableflag);
		alarmMDparam[i].macro_threshold =(4-voidemdparam[i].Senibility/3)*20+\
			(3-voidemdparam[i].Senibility%3)*6;
		XML_DEBUG("alarmMDparam[%d].macro_threshold:%d\n",i,alarmMDparam[i].macro_threshold);
 		alarmMDparam[i].alarm_threshold = voidemdparam[i].Senibility;
		alarmMDparam[i].interval = voidemdparam[i].DetectInterval;
		XML_DEBUG("alarmMDparam[%d].interval:%d\n",i,alarmMDparam[i].interval);
		alarmMDparam[i].areaNum = voidemdparam[i].DetectAreaNum;
		XML_DEBUG("alarmMDparam[%d].areaNum:%d\n",i,alarmMDparam[i].areaNum);

		for(j=0;j<4;j++)
		{
			alarmMDparam[i].recattr[j].x = voidemdparam[i].stMdArea[j].TopLeft.x;
			XML_DEBUG("alarmMDparam[%d].recattr[%d].x:%d\n",i,j,alarmMDparam[i].recattr[j].x);
			alarmMDparam[i].recattr[j].y = voidemdparam[i].stMdArea[j].TopLeft.y;
			XML_DEBUG("alarmMDparam[%d].recattr[%d].y:%d\n",i,j,alarmMDparam[i].recattr[j].y);
			alarmMDparam[i].recattr[j].higth = voidemdparam[i].stMdArea[j].BottomRight.x - voidemdparam[i].stMdArea[j].TopLeft.x;
			XML_DEBUG("alarmMDparam[%d].recattr[%d].higth:%d\n",i,j,alarmMDparam[i].recattr[j].higth);
			alarmMDparam[i].recattr[j].width = voidemdparam[i].stMdArea[j].BottomRight.y - voidemdparam[i].stMdArea[j].TopLeft.y;
			XML_DEBUG("alarmMDparam[%d].recattr[%d].width:%d\n",i,j,alarmMDparam[i].recattr[j].width);
		}

		for(m=0;m<7;m++)
		{
			for(n=0;n<4;n++)
			{
				alarmMDparam[i].timeattr[m][n].hourbeg = voidemdparam[i].AlertTime[m][n].AlertStartHour;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].hourbeg:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].hourbeg);
				alarmMDparam[i].timeattr[m][n].minbeg = voidemdparam[i].AlertTime[m][n].AlertStartMinu;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].minbeg:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].minbeg);
				alarmMDparam[i].timeattr[m][n].secbeg = voidemdparam[i].AlertTime[m][n].AlertStartSecond;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].secbeg:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].secbeg);
				alarmMDparam[i].timeattr[m][n].hourend = voidemdparam[i].AlertTime[m][n].AlertEndHour;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].hourend:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].hourend);
				alarmMDparam[i].timeattr[m][n].minend = voidemdparam[i].AlertTime[m][n].AlertEndMinu;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].minend:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].minend);
				alarmMDparam[i].timeattr[m][n].secend = voidemdparam[i].AlertTime[m][n].AlertEndSecond;
				XML_DEBUG("alarmMDparam[%d].timeattr[%d][%d].secend:%d\n",i,m,n,alarmMDparam[i].timeattr[m][n].secend);
			}

		}

	}

	return 0;
}



/*读出视频丢失参数(0不报警，1报警)*/
int readXmlViodeLoseParam()
{
	//GetXmlViodeLoseParam(&alarmvoideparam);
	memset(chnumlsoeflag,1,sizeof(chnumlsoeflag));
	voideloseparam.Enable = 0;
	voideloseparam.ChnBit = 0;
	XML_DEBUG("alarmvoideparam->Enable:%d\n",voideloseparam.Enable);
	XML_DEBUG("alarmvoideparam->ChnBit:%d\n",voideloseparam.ChnBit);

	return 0;
}

/*读出移动侦测参数 */
int readXmlMDParam(WEBOCX_MD_SET_ST *alarmdevicemd)
{
	int i = 0;
	GetXmlMDParam(CHNNUM,voidemdparam);

	for(i=0;i<4;i++)
	{
		XML_DEBUG("alarmdevicemd[%d]->ChnNum:%d\n",i,alarmdevicemd[i].ChnId);
		XML_DEBUG("alarmdevicemd[%d]->AlertEnable:%d\n",i,alarmdevicemd[i].AlertEnable);
		XML_DEBUG("alarmdevicemd[%d]->AlarmEnable:%d\n",i,alarmdevicemd[i].AlarmEnable);
		XML_DEBUG("alarmdevicemd[%d]->Senibility:%d\n",i,alarmdevicemd[i].Senibility);
		XML_DEBUG("alarmdevicemd[%d]->DetectAreaNum:%d\n",i,alarmdevicemd[i].DetectAreaNum);
		XML_DEBUG("alarmdevicemd[%d]->BuzzEnable:%d\n",i,alarmdevicemd[i].BuzzEnable);
		XML_DEBUG("alarmdevicemd[%d]->LinkBuzzDuration:%d\n",i,alarmdevicemd[i].LinkBuzzDuration);
		XML_DEBUG("alarmdevicemd[%d]->DetectInterval:%d\n",i,alarmdevicemd[i].DetectInterval);

		XML_DEBUG("sipcamerid[i]:%s\n",sipcamerid[i].CameraSIPID);
	}


	SetDeviceMDParam();

	SetDeviceMD();

	memset(md_alarm_record,0,sizeof(md_alarm_record));

	memset(restalarmflag,0,sizeof(int)*CHNNUM);

	//TM_APP_SetMdEnable();

	return 0;
}


void *TM_Device_alarm(void *arg)
{
	//int avsocketfd = *((int*)arg);
	int i = 0;
	int m = 0;
	int k = 0;
	char reboot = 0;

	char sipalarmflag = 0x00;

	while(1)
	{
		if(bootflag == 1)
		{
			sleep(3);
			logout();
			XML_DEBUG("logout===%d\n",ret);
			sleep(3);
			XML_DEBUG("device reboot\n");
			writeCrc16Data(cmd_sockfd,TM_mcu_addr,TM_reboot,&reboot,1);
		}

		/*视频丢失检查*/
		SetDeviceViodeLoseParam(&voideloseparam);
		XML_DEBUG("alarmvoideparam->Enable:%d\n",voideloseparam.Enable);

		if(voideloseparam.Enable == 1)
		{
			for(i=0;i<4;i++)
			{
				if(chnumlsoeflag[i] == 0)
				{
					//发给AV

					//发给SIP

					//蜂鸣器报警
					TM_Start_Alarm_Beep();
				}
			}
		}


		/*移动侦测检查*/
		for(m=0;m<4;m++)
		{
			//chnumMDflag[m] = TM_APP_MD_GetChnMDResult(m);
			XML_DEBUG("chnumMDflag[%d]:%d\n",m,chnumMDflag[m]);
		}
		for(k=0;k<4;k++)
		{
			if(chnumMDflag[k] != 0)
			{
				//发给AV
				alarmMDstat.channo = k;
				alarmMDstat.voipstat = 0;
				alarmMDstat.mdstat = 1;
				alarmMDstat.alarmstat = 1;
				#if 0
				if(avalarmflag[k] == 0)
				{
					avalarmflag[k] = 1;
					writeCrc16Data(cmd_sockfd,AVADDER,APP_MD_ALARM,(char *)&alarmMDstat,sizeof(alarmMDstat));
				}
				#endif

				//记录报警事件
				if(md_alarm_record[k].alarmflag == 0)
				{
					md_alarm_record[k].alarmflag = 1;
					md_alarm_record[k].chnnum = k;
					time(&md_time);
					XML_DEBUG("TM_Reset_Alarmalarmstarttime():%d\n",(int)md_time);
					md_alarm_record[k].alarmbegntime = (int)md_time;
					XML_DEBUG("md_alarm_record[%d].alarmbegntime():%d\n",k,md_alarm_record[k].alarmbegntime);
				}


				#if 1
				//蜂鸣器报警
				if(voidemdparam[k].BuzzEnable)
				{
					alarminfo.status = 1;
					TM_Start_Alarm(sipcamerid[k].CameraSIPID,0);
				}
				#endif
				//发给SIP
				memset(&alarminfo,0,sizeof(alarminfo));
				//strcpy(sipid,GetXmlText(cameranumstr[k],"Id"));
				strcpy(alarminfo.id,sipcamerid[k].CameraSIPID);
				XML_DEBUG("alarminfo.id:%s\n",alarminfo.id);

				strcpy(alarminfo.name,sipcamerid[k].CamerName);
				alarminfo.priority = onePriorit;
				alarminfo.type = fiveMethod;
				time(&alarminfo.time);
				XML_DEBUG("alarmenableflag:%d\n",alarmenableflag);
				sipalarmflag = alarmenableflag;
				if((sipalarmflag &= (0x01 << k)) == ((int )pow(2.0, k)))
				{

					if(restalarmflag[k] == 0)
					{
						restalarmflag[k] = 1;
						notifyAlarm(&alarminfo);
						XML_DEBUG("Send SIP\n");
					}
				}
			}
		}


		sleep(1); //1s
	}

	return 0;
}



int DeviceAlarmStart(int Sockfd)
{
	XML_DEBUG("DeviceAlzrmStart===%d\n",Sockfd);
	int ret = 0;
	pthread_t Alzrmpth;

	/*设置系统IP信息*/
 	GetXmlNetParam(&devicenetparam);
	//netConfig(&tmnetparam);
	/*获取PTZ信息*/
	GetXmlCamPTZParam(CHNNUM,devicecamPTZparam);
	/*获取设备信息*/
	GetXmlDeviceParam(&deviceparam);
	/*获取摄像头信息*/
	GetXmlCamParam(CHNNUM,devicecamparam);
	/*获取节点信息*/
	GetXmlNodeParam(1,devicenodeparam);
	/*读出视频丢失参数*/
	//GetXmlNetParam(&devicenetparam);
	readXmlViodeLoseParam();
	/*读出移动侦测参数 */
	//GetXmlMDParam(CHNNUM,voidemdparam);
	readXmlMDParam(voidemdparam);
	/*读取PTZ参数*/
	GetXmlcamerid(sipcamerid);

	/*创建报警处理线程*/
	ret = pthread_create(&Alzrmpth,NULL,TM_Device_alarm,(void *)&Sockfd);
	if(ret < 0)
	{
		XML_DEBUG("DeviceAlzrmStart error:%d\n",ret);
		pthread_exit(0);
	}

	return ret;
}



/*录像控制(recordflag:1启动，0停止)*/
int TM_Record_Control(const char* id,int recordflag)
{
	TM_Stop_Record device_record_control;
	int i = 0;

	/*通道选择*/
	for(i=0;i<Getcameranumstr();i++)
	{
		//if((strcmp(id,GetXmlText(cameranumstr[i],"Id"))) == 0)
		if((strcmp(id,sipcamerid[i].CameraSIPID)) == 0)
		{
			device_record_control.chnnum = i;
			XML_DEBUG("device_record_control.chnnum:%d\n",device_record_control.chnnum);
			break;
		}
	}

	XML_DEBUG("recordflag:%d\n",recordflag);
	if(recordflag == 1)
	{
		alarmMDstat.Recstat = 1;
		SetXmlText((const char*)"Record",(char*)"ON");
		device_record_control.rcord_flag = 0x01;
	}
	else if(recordflag == 0)
	{
		alarmMDstat.Recstat = 0;
		SetXmlText((const char*)"Record",(char*)"OFF");
		device_record_control.rcord_flag = 0x02;
	}

	/*具体通道录像控制*/
	switch(device_record_control.chnnum)
	{
		case 0:
			writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_CMD,(char *)&device_record_control,sizeof(device_record_control));
			break;
		case 1:
			writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_CMD,(char *)&device_record_control,sizeof(device_record_control));
			break;
		case 2:
			writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_CMD,(char *)&device_record_control,sizeof(device_record_control));
			break;
		case 3:
			writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_CMD,(char *)&device_record_control,sizeof(device_record_control));
			break;
		default:
			XML_DEBUG("device_record_control.chnnum:%d",device_record_control.chnnum);
			return -1;
	}

	return 0;
}

/*蜂鸣器控制*/
int TM_Start_Alarm_Beep()
{
	char buffer[10] = {0};
	unsigned short int alarmlong = 5;

	buffer[0] = alarmlong >> 8;
	buffer[1]  = alarmlong & 0xFF;
	XML_DEBUG("TM_Start_Alarm alarmlong:%s\n",buffer);

	writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_BEEP_START,buffer,2);

	return 0;

}
int TM_Start_Alarm(const char* id,int alarmType)
{
	unsigned short int alarmlong = 5;
	int i;
	char buffer[16] = {0};

	/*时间选择*/
	for(i=0;i<Getcameranumstr();i++)
	{
		if((strcmp(id,sipcamerid[i].CameraSIPID)) == 0)
		{
			if(sipcamerid[i].CamerBuzzDuration > 5)
			{
				alarmlong = sipcamerid[i].CamerBuzzDuration;
			}
			XML_DEBUG("TM_Start_Alarm alarmlong:%d\n",alarmlong);
			break;
		}
	}

	buffer[0] = alarmlong >> 8;
	buffer[1]  = alarmlong & 0xFF;

	writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_BEEP_START,buffer,2);

	return 0;
}
int TM_Reset_Alarm(const char* id,int alarmType)
{
	int i = 0;

	#if 0
	writeCrc16Data(cmd_sockfd,TM_UI_ADDER,TM_UI_BEEP_STOP,&buffer,1);
	#endif

	for(i=0;i<Getcameranumstr();i++)
	{
		//if((strcmp(id,GetXmlText(cameranumstr[i],"Id"))) == 0)
		if((strcmp(id,sipcamerid[i].CameraSIPID)) == 0)
		{
			restalarmflag[i] = 0;
			if(md_alarm_record[i].alarmflag == 1)
			{
				md_alarm_record[i].alarmflag = 0;
				time(&md_time);
				XML_DEBUG("TM_Reset_Alarmalarmendtime():%d\n",(int)md_time);
				md_alarm_record[i].alarmendtime = md_time;
				XML_DEBUG("md_alarm_record[%d].alarmendtime:%d\n",i,md_alarm_record[i].alarmendtime);
				//保存报警信息
				NewAlarmXmlText(&md_alarm_record[i]);
				//清除缓存
				memset(&md_alarm_record[i],0,sizeof(md_alarm_record));
			}

			break;
		}
	}


	return 0;
}


int netConfig(WEBOCX_NETWORK_PARA_ST *pstNetArgs)
{
#if 1
	char cCmdBuf[ 512 ] = {0};

	snprintf(cCmdBuf, sizeof(cCmdBuf), "/mnt/nand/net_config.sh %s %s %s %s %s %s %s " \
				,pstNetArgs->IpAddr, pstNetArgs->NetMask, pstNetArgs->NetGate\
				,pstNetArgs->MultAddr,pstNetArgs->MacAddr,pstNetArgs->DnsAddr1\
				,pstNetArgs->DnsAddr2);

	XML_DEBUG("cmd list: %s\n", cCmdBuf);

	system(cCmdBuf);
#endif
	return 0;
}




