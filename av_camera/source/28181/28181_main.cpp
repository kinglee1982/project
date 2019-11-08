// test.cpp : 定义控制台应用程序的入口点。
//

#ifdef WIN32
#include "stdafx.h"
#endif

#include "Nvs.h"
#include <map>
#include <string>
#include "TThread.h"
#include "NaluAnalyzer.h"
#include "TEvent.h"
#include "TTime.h"
#include "TM_APP_PtzManager.h"
#include "28181_AV_API.h"
#include "tinyxml2API.h"
#include "av_public_head.h"
#include "TM_APP_PtzManager.h"
#include "TM_Alarm.h"

#include "TM_MCUSocket.h"
#include "TM_Alarm.h"
#include "TM_AlarmRecord.h"
#include "av_video_proc.h"
#include "av_video_init.h"
#include "av_hddfsm.h"
#include "av_store.h"
#include "av_sys.h"
#include "av_socket.h"
#include "av_ocx_process.h"
#include "av_audio_init.h"
#include "opus.h"
#include "opus_encode.h"
#include "opus_decode.h"

#include "memfunc.h"
#include "DevSearch.h"


bool GetChnByResId(const char* id,int* pchn);
#ifdef PLAT_QUERY_REC_TIME
int GetSeekTimeFileIndex(SF_LIST_ST* _pFileList,REC_PLAYBACK_T *_pstPB,int _seekpos,time_t *_seektime);
#endif

#define DEBUG_P
//AV_PRINT("\033[32m -------->%d \033[0m\n",__LINE__)
#define HOUR8SEC	28800   //以前需要+8个小时的时间，时区有问题
#define HOUR8MSEC	28800000   //以前需要+8个小时的时间，时区有问题
#define HOUR8USEC   28800000000LL
#define RESERVETIME	0          // 时间预留量


HI_VOID ConvertTime2LocaltimeStr(time_t t)
{
    struct tm tmm;
    memset(&tmm, 0, sizeof(struct tm));
    localtime_r(&t, &tmm);
    AV_PRINT( "----> %04d-%02d-%02d %02d:%02d:%02d\n",
              tmm.tm_year+1900, tmm.tm_mon+1, tmm.tm_mday, tmm.tm_hour, tmm.tm_min, tmm.tm_sec);
}


int ModifyRspTime2UTC(nvs::QueryRecordResponse& resp);

struct FrameInfo
{
    int len;
    int flag;
    unsigned long long timestamp;
};
#ifdef PLT_PARA_BK

#define FLAG_MAIN_IP 0
#define	FLAG_SUB_IP  1
#define PLT_TRY_CONNECT_FAIL_TIME 	24		//重连失败次数,每次重连等待10s
#define PLT_CONNECT_SLEEP_ARG 		5	    //已连接的休眠系数
#define PLT_NORMAL_SLEEP_ARG 		1	    //正常休眠系数
#define PLT_SLEEP_10S				10*1000*1000


enum PLATFORM_FLAG
{
    FLAG_SWITCH_TO_MAIN,	//切换IP
    FLAG_SWITCH_TO_SUB,		//切换IP
    FLAG_CHECKPARA,			//参数是否有变动
    FLAG_CHEKCONNECT,		//检测是否链接上
    FLAG_CONNECTFAIL,		//链接失败
    FLAG_CONNECTED,			//已经链接上
    FLAG_WAIT_SIM_ONLINE	//等待SIM卡拨号
};


struct PlatAddr
{
    int flag;
    int iaddress;//0:主IP 1:备用IP
    //sip双ip，port
    char     id[nvs::MAX_ID];       //sip地址编码ID号
    char     ip[2][nvs::MAX_IPV4_LEN]; //sip监听的IP地址
    uint16_t port[2];             //sip监听的端口
};
static pthread_mutex_t g_PltSipDatlock;
static struct PlatAddr g_PltSipData =
{
    FLAG_WAIT_SIM_ONLINE,
    FLAG_MAIN_IP,
    "2100001205000008",
    {"116.255.198.211","116.255.198.211"},
    {10002,10002}
};
#endif
class DvrMediaPlayer : public nvs::MediaStream,public comn::Thread,public NaluAnalyzerSink
{
public:
    explicit DvrMediaPlayer(const char* id, int streamIdx)
        :m_pSink(NULL),
         m_id(id),
         m_streamIdx(streamIdx),
         m_isLive(true)
    {
        //m_naluAnalyzer.setSink(this);

        m_preFrameMilliseconds = 0;
        m_scale = 1.0;
        m_basespeed = 1.0;
        m_isPause = false;
        m_chn = -1;
        m_canSend = false;

        m_bseek = false;
        m_seekpos = 0;
        m_bstep = false;
        m_bsndstep = false;

        m_hFiFo = mem_CreateContainer(CONTAINER_FIFO,CONTAINER_PRI_CYC,750);
        AV_PRINT("DvrMediaPlayer fifo (%p)",m_hFiFo);
    }

    nvs::uint64_t getTimestamp()
    {
        comn::Time t;
        return (nvs::uint64_t)t.getSecond() * 1000 + t.getMillisecond();
    }

    explicit DvrMediaPlayer(const nvs::RecordItem &rec)
        :m_pSink(NULL),
         m_id(rec.id),
         m_streamIdx(0),
         m_isLive(false),
         m_rec(rec)
    {
        //m_naluAnalyzer.setSink(this);

        m_preFrameMilliseconds = 0;

        m_isPause = false;

        m_canSend = false;
        m_chn = -1;

        m_scale = 1.0;
        m_basespeed = 1.0;

        m_bseek = false;
        m_seekpos = 0;
        m_bstep = false;
        m_bsndstep = false;

        m_hFiFo = NULL;
    }
    virtual ~DvrMediaPlayer()
    {
        mstop();

        if(m_hFiFo != NULL)
        {
            //去使能Fifo
            mem_DisEnable(m_hFiFo);
            mem_UnBind(m_hFiFo,g_ChnDataCenter[m_chn]);
            mem_Destory(m_hFiFo);
            m_hFiFo = NULL;
            AV_PRINT("free fifo");
        }
        if(m_streamIdx && mem_DataCenterEmpty(g_ChnDataCenter[m_chn]) == 1)
        {
            AV_PRINT("data center empty stop sub stream!!!!");
            HI3521_SetAutoBitrate(m_chn,1);
            HI3521_StopSubStreamVenc((m_chn - 1)/2);
        }
    }

    int GetRateFmt(IN int jitterLevel)
    {
        int rate = 0;

        switch(jitterLevel)
        {
        case nvs::QOS_LEVEL_BAD:
        {
            rate = 8;
            break;
        }
        case nvs::QOS_LEVEL_NORMAL:
        {
            rate = 15;
            break;
        }
        case nvs::QOS_LEVEL_BETTER:
        {
            rate = 25;
            break;
        }
        default:
        {
            rate = 25;
            break;
        }
        }

        return rate;
    }

    ///播放
    virtual int mplay()
    {
        if (!isRunning())
        {
            start();
        }

        m_isPause = false;
        m_scale = 1.0*m_basespeed;
        m_bstep = false;

        AV_PRINT("play\n");
        return nvs::NVS_ERR_OK;
    }

    ///暂停
    virtual int mpause()
    {
        AV_PRINT("pause\n");
        m_isPause = true;

        return nvs::NVS_ERR_OK;
    }

    ///停止
    virtual int mstop()
    {
        m_canExit = true;
        m_timeWaiter.post();

        stop();

        AV_PRINT("stop chn:%d stream:%d\n",m_chn,m_streamIdx);
        return nvs::NVS_ERR_OK;
    }

    ///逐帧，单帧
    virtual int mstep()
    {
        AV_PRINT("step\n");
        m_bstep = true;
        m_bsndstep = true;
        return nvs::NVS_ERR_OK;
    }

    ///拖拽,进度0-100
    virtual int mseek(int offset)
    {
        AV_PRINT("seek:%d\n",offset);
        m_bseek = true;
        m_seekpos = offset;

        return nvs::NVS_ERR_OK;
    }

    virtual int mjitter(IN int jitterLevel)
    {
        AV_PRINT("--------->mjitter:%d\n",jitterLevel);
        if(m_isLive)
        {
            if((jitterLevel==0)||(jitterLevel==1)||(jitterLevel==2))
            {
                HI3521_SetAutoBitrate(m_chn,jitterLevel);
                //g_mapRealStream[m_chn].rate = jitterLevel;
                m_fmt.fps = GetRateFmt(jitterLevel);
                AV_PRINT("  fps-------->%d",m_fmt.fps);
                if (m_pSink != NULL)
                {
                    m_pSink->setFormat(m_fmt);
                }
            }
        }

        return nvs::NVS_ERR_OK;
    }


    ///倍速，1/4.0 ---- 4.0
    virtual int mscale(double fscale)
    {
        AV_PRINT("scale:%lf\n",fscale);
        m_scale = fscale*m_basespeed;
        return nvs::NVS_ERR_OK;
    }

    ///设置流回调
    virtual int setSink(nvs::StreamSink* pSink)
    {
        AV_PRINT("setSink\n");
        m_pSink = pSink;
        m_scale = 1.0*m_basespeed;

        memset(&m_fmt,0,sizeof(m_fmt));
        m_fmt.codec = nvs::STANDARD_H264_CODEC;
        m_fmt.audioCodec = nvs::G711A_CODEC;

        if(m_streamIdx)
        {
            m_fmt.width = 704;
            m_fmt.height = 576;
        }
        else
        {
            m_fmt.width = 1280;
            m_fmt.height = 720;
        }
        m_fmt.fps = 25;
        m_fmt.audioSampleRate = 16000;

        if (m_pSink != NULL)
        {
            m_pSink->setFormat(m_fmt);
        }

        return nvs::NVS_ERR_OK;
    }

    virtual void naluAnalyPacket(NaluPacket& nalu)
    {
        nvs::StreamPacket packet;
        packet.data = nalu.data;
        packet.size = nalu.length;
        packet.flag = nvs::FRAME_VIDEO_P;
        packet.timestamp = nalu.nTimestamp;
        //AV_PRINT("----->[%llu]\n",nalu.nTimestamp);
        if (nalu.type == NaluPacket::NALU_SPS)
        {
            packet.flag = nvs::FRAME_VIDEO_I;
            m_canSend = true;
        }

        if (!m_canSend)
        {
            return;
        }


        if (m_pSink != NULL)
        {

            m_pSink->writePacket(packet);
        }
    }

    void doAudio(unsigned char *pData,int len,unsigned long long timestamp)
    {
        if (!m_canSend)
        {
            return;
        }

        nvs::StreamPacket packet;
        packet.data = pData;
        packet.size = len;
        /*
        		if(m_isLive)
        		{
        			packet.flag = nvs::FRAME_PCM;
        		}
        		else
        		{
        			packet.flag = nvs::FRAME_OPUS;
        		}
        */
#if (AENC_TYPE == AENC_G711)
        packet.flag = nvs::FRAME_G711A;
#else
        packet.flag = nvs::FRAME_PCM;
#endif
        packet.timestamp = timestamp ;


        if (m_pSink != NULL)
        {
#ifdef VOIP_DEBUG
            static int numPacket = 0;
            static nvs::uint64_t dwBeginTime = getTimestamp();
            nvs::uint64_t dwNow = getTimestamp();
            numPacket++;
            int dwSpan = dwNow-dwBeginTime;
            if ( dwSpan > 1000 )
            {
                AV_PRINT( "playpcm ---------1 sec -------->%d packet \r\n",numPacket);
                numPacket = 0;
                dwBeginTime = dwNow;
            }

            static FILE* fp = NULL;
            if(fp == NULL)
            {
                fp = fopen("/mnt/nand/pcm11","w+");
                AV_PRINT(" ------------>fopen %p\n",fp);
            }
            if(fp != NULL)
                fwrite(pData ,len,1,fp);
#endif
            m_pSink->writePacket(packet);
        }
    }

    void caleRate(unsigned long long curFrameMilliseconds)
    {

        ///实际保存时候的帧间隔
        unsigned int Milliseconds = curFrameMilliseconds - m_preFrameMilliseconds;
        ///上一帧比当前帧时间戳还大或者帧间隔时间戳过大
        if (Milliseconds >= 2000) ///2秒
        {
            Milliseconds = 40;
        }

        m_preFrameMilliseconds = curFrameMilliseconds;

        if (m_scale > 0.0)
        {
            ///倍速
            Milliseconds *= (1.0/m_scale);
        }

        m_timeWaiter.timedwait(Milliseconds);
    }

    void caleRate2(unsigned long long curFrameMilliseconds)
    {
        ///实际保存时候的帧间隔
        //unsigned int Milliseconds = curFrameMilliseconds - m_preFrameMilliseconds2;

        int Milliseconds = 30;
        if (m_scale > 0.0)
        {
            ///倍速
            Milliseconds *= (1.0/m_scale);
        }

        m_timeWaiter.timedwait(Milliseconds);
    }
    int run()
    {
        if(m_isLive)
        {
            TransLiveStream();
        }
        else
        {
            TransHistoryFile();
        }
        return 0;

    }

    int TransHistoryFile()
    {
        if(!GetChnByResId(m_id.c_str(), &m_chn))
        {
            sinkEnd();
            AV_PRINT("#[av]Trans History Stream CHN error, NO this id %s \n",m_id.c_str());
            return 0;
        }

        SF_LIST_ST  pstSFList = {0};
        HI_BOOL bLocal = HI_FALSE;
        HI_S32 s32RecType = 0;
        HI_S32 s32FileCnt = 0;
        SF_NAME_ST stSFName = {0};
        FILE *pFile = NULL;
        HI_U32 u32FrameSeek = 0;
        HI_U32 u32FrameEnd = 0;
        HI_U32 s32Ret = 0;
        STOR_FILE_BLK_HEAD_T stFileBlkHead;
        FRAME_HEAD_T stFrameHead = {0};
        FRAME_HEAD_T *pstFrameHead = &stFrameHead;
        HI_S32 s32RdLen = 0;
        HI_U32 u32CurFrameTime = 0;
        HI_U32 u32LastFrameTime = 0;
        HI_U8 u8VdecBuf[307200] = {0};	/*300KB*/
        HI_U64 Milltime = 0;
        REC_PLAYBACK_T  pstPB = {0};
        time_t ctrlTime = 0;

        pstPB.s32Cmd = m_chn;
        pstPB.tStartTime = m_rec.beginTime+HOUR8SEC-RESERVETIME;
        pstPB.tEndTime = m_rec.endTime+HOUR8SEC+RESERVETIME;

        AV_PRINT("-------->#[av]Trans History File [chn %d] beginTime =%d,endTime=%d\n",
                 m_chn,(int)m_rec.beginTime, (int)m_rec.endTime);
        /* search file by time */
        if(HI_SUCCESS != HI3531_SFByTime_V28181((SF_REQ_CMD_ST*)&pstPB,&pstSFList,bLocal,s32RecType))
        {

            sinkEnd();

            return 0;
        }

        HDD_SetHddEvent(DISK_EVENT_REC_RPB, HI_TRUE);
#ifdef PLAT_QUERY_REC_TIME
        int iSwitchFileIndex = -1;
        for (s32FileCnt = 0; s32FileCnt < pstSFList.s32FileSegNum; )
#else
        /*一段录像可能有N个时间段组成*/
        for (s32FileCnt = 0; s32FileCnt < pstSFList.s32FileSegNum; s32FileCnt++)
#endif
        {
            memset(&stSFName, 0, sizeof(stSFName));
            memcpy(&stSFName.szFileName, pstSFList.stFileName[s32FileCnt].szFileName, FILE_NAME_LEN);
            u32FrameSeek = 0;
            AV_PRINT("-------->s32Chn(%d),start HI3531_OpenSF(%s) \n",pstSFList.iChnno,stSFName.szFileName);
            /*打开文件*/
            pFile = HI3521_OpenSF(&stSFName);
            if (NULL == pFile)
            {
#ifdef PLAT_QUERY_REC_TIME
                s32FileCnt++;
#endif
                continue;
            }

            AV_PRINT("-------->s32Chn(%d), HI3531_OpenSF(%s) success\n",m_chn,stSFName.szFileName);

            /*获取帧开始位置*/
            s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, pstPB.tStartTime);
            if (HI_SUCCESS != s32Ret)
            {
                s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, pstPB.tStartTime + 1);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("-------->STOR_GetIFrameSeek err\n");
                }
            }

            /*获取帧结束位置*/
            s32Ret = STOR_GetVideoFileHeadInfo(pFile, &stFileBlkHead, PART_INDEX_SEEK_HEAD);
            if (HI_SUCCESS != s32Ret)
            {
                if(NULL != pFile)
                {
                    fclose(pFile);
                    pFile = NULL;
                }
#ifdef PLAT_QUERY_REC_TIME
                s32FileCnt++;
#endif
                continue;
            }

            u32FrameEnd = stFileBlkHead.frame_data_end_pos;
            /*播放数据*/
            u32CurFrameTime = 0;
            u32LastFrameTime = 0;
            ctrlTime = pstPB.tStartTime;
            while (!m_canExit)
            {
                if (DISK_STAT_WORKING != HDD_GetDiskFSM())
                {
                    AV_PRINT("DISK STAT %d ",HDD_GetDiskFSM());
                    break;
                }

                if (u32FrameSeek >= u32FrameEnd)
                {
                    AV_PRINT("[%d] u32FrameSeek %d,u32FrameEnd %d\n",__LINE__,u32FrameSeek,u32FrameEnd);
                    break;
                }

                if(m_bseek)
                {
#ifdef PLAT_QUERY_REC_TIME
                    time_t seektime = 0;
                    int ifileindex = GetSeekTimeFileIndex(&pstSFList,&pstPB,m_seekpos,&seektime);
                    if(ifileindex==s32FileCnt)
                    {
                        iSwitchFileIndex = -1;
                        m_bseek = false;
                        s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, seektime);
                        if (HI_SUCCESS != s32Ret)
                        {
                            s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, seektime + 1);
                            if (HI_SUCCESS != s32Ret)
                            {
                                AV_PRINT("[%d] STOR_GetIFrameSeek %d\n",__LINE__,s32Ret);
                                break;
                            }
                        }
                        u32CurFrameTime = seektime;
                    }
                    else if(ifileindex<pstSFList.s32FileSegNum)
                    {
                        AV_PRINT("Need change Now(File%d:S(%d)E(%d)) TO(File%d:S(%d)E(%d))\n"
                                 ,s32FileCnt,(int)pstSFList.stFileTimeSeg[s32FileCnt].tStartTime,(int)pstSFList.stFileTimeSeg[s32FileCnt].tEndTime
                                 ,ifileindex,(int)pstSFList.stFileTimeSeg[ifileindex].tStartTime,(int)pstSFList.stFileTimeSeg[ifileindex].tEndTime);
                        iSwitchFileIndex = ifileindex;
                        break;
                    }
                    else
                    {
                        iSwitchFileIndex = -1;
                        m_bseek = false;
                        AV_PRINT("No match file.SeekTime(%d)Query(S:%d;E:%d)"
                                 ,(int)seektime,(int)pstSFList.stFileTimeSeg[0].tStartTime
                                 ,(int)pstSFList.stFileTimeSeg[pstSFList.s32FileSegNum-1].tEndTime);
                        break;
                    }
#else
                    m_bseek = false;
                    time_t seektime = ((pstPB.tEndTime - pstPB.tStartTime)*m_seekpos/100)+pstPB.tStartTime;
                    AV_PRINT("------> [seektime %d] [start %d]  [end %d] ,rec[start %d] [end %d]",
                             (int)seektime,(int)pstPB.tStartTime,(int)pstPB.tEndTime,
                             (int)m_rec.beginTime-HOUR8SEC,(int)m_rec.endTime-HOUR8SEC);

                    s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, seektime);
                    if (HI_SUCCESS != s32Ret)
                    {

                        s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, seektime + 1);
                        if (HI_SUCCESS != s32Ret)
                        {
                            AV_PRINT("[%d] STOR_GetIFrameSeek %d\n",__LINE__,s32Ret);
                            break;
                        }
                    }
                    u32CurFrameTime = seektime;
#endif
                }
                if(m_bstep)
                {
                    if(!m_bsndstep)
                    {
                        m_timeWaiter.timedwait(1000);
                        continue;
                    }
                }

                if (m_isPause)
                {
                    m_timeWaiter.timedwait(1000);
                    continue;
                }

                fseek(pFile, u32FrameSeek ,SEEK_SET);
                s32RdLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pFile);
                if (sizeof(FRAME_HEAD_T) != s32RdLen)
                {
                    AV_PRINT("[%d] sizeof(FRAME_HEAD_T) %d,s32RdLen %d\n",__LINE__,sizeof(FRAME_HEAD_T),s32RdLen);
                    break;
                }

                /*录像有问题，长度过大 */
                if (pstFrameHead->len > 1024*1024)
                {
                    u32CurFrameTime = STOR_CalTime2(pstFrameHead);
                    s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, u32CurFrameTime+1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT("[%d] STOR_GetIFrameSeek %d \n",__LINE__,s32Ret );
                        break;
                    }
                }

                u32FrameSeek = u32FrameSeek+pstFrameHead->len+sizeof(FRAME_HEAD_T);

                /*根据协议要求，和回放区别开来，发送数据 帧数据 */
                s32RdLen = fread(u8VdecBuf  , 1, pstFrameHead->len, pFile);
                if (pstFrameHead->len != (HI_U32)s32RdLen)
                {
                    continue;
                }

                /*ts已经是一个UTC 0的微秒时间 */
                Milltime = (pstFrameHead->ts - HOUR8USEC)/1000;
                //ConvertTime2LocaltimeStr(Milltime/1000);
                /* 将获取音视频数据进行解码 */
                if((pstFrameHead->type == DATA_VIDEO_IFRAME)||
                        (pstFrameHead->type == DATA_VIDEO_PFRAME))
                {
                    NaluPacket nalu;
                    nalu.data = u8VdecBuf;
                    nalu.length = s32RdLen;
                    nalu.nTimestamp = Milltime;
                    nalu.type = (pstFrameHead->type == DATA_VIDEO_IFRAME)?NaluPacket::NALU_SPS:NaluPacket::NALU_NULL;
                    naluAnalyPacket(nalu);
                    m_bsndstep = false;

                    caleRate2(Milltime);
                }
                else if(pstFrameHead->type == DATA_AUDIO)
                {
#if (AENC_TYPE == AENC_G711)
                    doAudio((uint8_t*)u8VdecBuf+4 ,s32RdLen-4 , Milltime);
#else
                    doAudio((uint8_t*)u8VdecBuf ,s32RdLen , Milltime);
#endif
                }

                /* 判断该文件是否结束 */
                if (u32CurFrameTime > (HI_U32)pstPB.tEndTime)
                {
                    AV_PRINT("[%d]  u32CurFrameTime %d,(HI_U32)pstPB.tEndTime %d\n",\
                             __LINE__,u32CurFrameTime,(HI_U32)pstPB.tEndTime);
                    break;
                }

                /*更新时间*/
                u32CurFrameTime = STOR_CalTime2(pstFrameHead);
                if (u32CurFrameTime >= u32LastFrameTime)
                {
                    u32LastFrameTime = u32CurFrameTime;
                    ctrlTime = u32CurFrameTime;
                }
                else
                {
                    u32CurFrameTime = ctrlTime;
                }
            }

#ifdef PLAT_QUERY_REC_TIME
            if(iSwitchFileIndex!=-1)
            {
                s32FileCnt = iSwitchFileIndex;
                iSwitchFileIndex = -1;
                continue;
            }
            s32FileCnt++;
#endif
            /*关闭文件*/
            if (NULL != pFile)
            {
                fclose(pFile);
                pFile = NULL;
            }
        }

        HDD_SetHddEvent(DISK_EVENT_REC_RPB, HI_FALSE);
        sinkEnd();
        return 0;
    }


    int testTransHistoryFile()
    {
        FILE *f = fopen("28181VA.dat","rb");
        if (f == NULL)
        {
            if (m_pSink != NULL)
            {
                m_pSink->endOfStream();
            }
            return -1;
        }

        FrameInfo info;
        int len = fread(&info,1,sizeof(info),f);

        while(!m_canExit)
        {
            if (m_isPause)
            {
                m_timeWaiter.timedwait(1000);
                continue;
            }

            unsigned char *pBuf = new unsigned char[info.len];
            len = fread(pBuf,1,info.len,f);
            if (len == info.len)
            {
                caleRate(info.timestamp);

                if (info.flag == 0)
                {
                    //m_naluAnalyzer.inputData(pBuf,len,info.timestamp);
                }
                else
                {
                    doAudio((unsigned char *)pBuf,len,info.timestamp);
                }

            }

            delete []pBuf;

            len = fread(&info,1,sizeof(info),f);

            if (feof(f))
            {
                break;
                fseek(f,0,SEEK_SET);
                len = fread(&info,1,sizeof(info),f);
            }
        }

        fclose(f);
        sinkEnd();

        return 0;
    }

    int TransLiveStream()
    {
        if(!GetChnByResId(m_id.c_str(), &m_chn))
        {
            sinkEnd();
            return nvs::NVS_ERR_FAIL;
        }
        HI3521_StartSubStreamVenc(m_chn);
        AV_PRINT("----------->#[app]Trans Live Stream   begin [AUDIO %d] m_streamIdx:%d\n",m_chn,m_streamIdx);
        m_chn = 2*m_chn + m_streamIdx;
        mem_Bind(m_hFiFo,g_ChnDataCenter[m_chn]);
        FRAME_HEAD_T *pstFrameHead = NULL;
        while(!m_canExit)
        {
            if (m_isPause)
            {
                m_timeWaiter.timedwait(1000);
                continue;
            }
            int iCount = mem_GetPackageNum(m_hFiFo);
            while(iCount > 25 && !m_canExit)
            {
                HDP hPkg = mem_PopPackageData(m_hFiFo);
                if(hPkg)
                {
                    char *pData = mem_GetPackageData(hPkg);
                    int iFrameLen = mem_GetPackageDataLen(hPkg);
                    int iFrameSeek = 0;
                    while(iFrameSeek < iFrameLen)
                    {
                        pstFrameHead = (FRAME_HEAD_T *)(pData + iFrameSeek);
                        if(pstFrameHead->type == DATA_VIDEO_PFRAME|| pstFrameHead->type == DATA_VIDEO_IFRAME)
                        {
                            NaluPacket nalu;
                            nalu.data = (uint8_t *)(pData+sizeof(FRAME_HEAD_T));
                            nalu.length = pstFrameHead->len;
                            nalu.nTimestamp = (long long)(pstFrameHead->ts - HOUR8USEC)/1000;
                            if(pstFrameHead->type==DATA_VIDEO_IFRAME)
                                nalu.type = NaluPacket::NALU_SPS;
                            else
                                nalu.type = NaluPacket::NALU_NULL;
                            naluAnalyPacket(nalu);
                        }
                        else if(pstFrameHead->type == DATA_AUDIO)
                        {
                            doAudio((unsigned char*)(pData + sizeof(FRAME_HEAD_T) + 4),\
                                    pstFrameHead->len - 4,\
                                    (pstFrameHead->ts - HOUR8USEC)/1000);
                        }
                        iFrameSeek = iFrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
                    }
                    mem_ReleasePackage(hPkg);
                }
                iCount--;
            }
            TM_USleep(0,10*1000);
        }
        sinkEnd();
        return 0;
    }

    inline void sinkEnd()
    {

        AV_PRINT("----------->#[app]sinkEnd ------------------>>\n");
        if (m_pSink != NULL)
        {
            m_pSink->endOfStream();
        }
    }

private:
    nvs::StreamSink *m_pSink;
    std::string m_id;
    int m_streamIdx;

    //NaluAnalyzer m_naluAnalyzer;

    bool m_isLive;

    nvs::RecordItem m_rec;
    int m_chn;

    comn::Event m_timeWaiter;

    unsigned long long m_preFrameMilliseconds;
    unsigned long long m_preFrameMilliseconds2;

    nvs::StreamFormat m_fmt;

    bool m_isPause;

    bool m_canSend;

    float m_scale;
    float m_basespeed;

    bool m_bseek;	/* enable seek */
    int  m_seekpos;	/* seek pos */

    bool m_bstep;	/* enable step */
    bool m_bsndstep;/* should send one fream? */

    HBC m_hFiFo;
};

class PhoneMediaPlayer* g_iPhone = NULL;
#define OPUS 0
class PhoneMediaPlayer : public nvs::MediaStream,public nvs::StreamSink,public comn::Thread
{
public:
    explicit PhoneMediaPlayer(const char* id)
        :m_pSink(NULL),
         m_id(id)
    {
        m_preFrameMilliseconds = 0;
#if OPUS
        m_decoder = NULL;
        m_encoder = NULL;
#endif
        m_run = false;
        AV_PRINT("=====open PhoneMediaPlayer id====%s  \n",m_id.c_str());

        m_hFiFo = mem_CreateContainer(CONTAINER_FIFO,CONTAINER_PRI_CYC,300);
    }

    virtual ~PhoneMediaPlayer()
    {
        LockPhoneMutex();
        g_iPhone=NULL;
        UnlockPhoneMutex();
        stop();
        m_run = false;
        SetVoipAdecStop();
        AVSndBZQCtrlVOIP(BZQ_CLOSE_VOICE_CHN);
        SndUIStopVOIP();
        mem_DisEnable(m_hFiFo);
        mem_UnBind(m_hFiFo,g_ChnDataCenter[TM_APP_AUDIO_MIC_INDEX]);
        mem_Destory(m_hFiFo);
        m_hFiFo = NULL;
        HI3521A_VoIPDisable();
        AV_PRINT("=====close PhoneMediaPlayer id====%s  \n",m_id.c_str());
    }

    nvs::uint64_t getTimestamp()
    {
        comn::Time t;
        return (nvs::uint64_t)t.getSecond() * 1000 + t.getMillisecond();
    }


    ///播放
    virtual int mplay()
    {
        //CHANNEL
        if (!isRunning())
        {
            HI3521A_VoIPEnable();
            VoipAdecEntry();
            m_run = true;
            start();
        }

        AV_PRINT("play\n");
        return nvs::NVS_ERR_OK;
    }

    ///暂停
    virtual int mpause()
    {
        AV_PRINT("pause\n");
        return nvs::NVS_ERR_OK;
    }

    ///停止
    virtual int mstop()
    {
        stop();
        AV_PRINT("stop\n");
        return nvs::NVS_ERR_OK;
    }

    ///逐帧，单帧
    virtual int mstep()
    {
        AV_PRINT("step\n");
        return nvs::NVS_ERR_OK;
    }

    ///拖拽,进度0-100
    virtual int mseek(int offset)
    {
        AV_PRINT("seek:%d\n",offset);
        return nvs::NVS_ERR_OK;
    }
    virtual int mjitter(IN int jitterLevel)
    {
        AV_PRINT("mjitter:%d\n",jitterLevel);

        return nvs::NVS_ERR_OK;
    }

    ///倍速，1/4.0 ---- 4.0
    virtual int mscale(double fscale)
    {
        AV_PRINT("scale:%lf\n",fscale);
        return nvs::NVS_ERR_OK;
    }
    virtual void setFormat(IN const nvs::StreamFormat& fmt)
    {
        return;
    }

    //在请求媒体流时，调用该函数将流媒体数据写入SDK，由SDK负责发送
    virtual int writePacket(IN const nvs::StreamPacket& packet)
    {
        if(!m_run)
        {
            return nvs::NVS_ERR_OK;
        }
#ifdef VOIP_DEBUG
        int ret = 0;
        unsigned char pcmBuf[1024*4] = {0};
        ret = decodeOpus(m_decoder, packet.data, packet.size,  pcmBuf);

        /* send data to AO */
        if(ret > 0)
        {
            AUDIO_STREAM_S stAudioStream;

            stAudioStream.pStream = pcmBuf;;
            stAudioStream.u32Len = ret;
            stAudioStream.u64TimeStamp = 1;
            stAudioStream.u32Seq = 1;
            AV_PRINT("writePacket pstFrameHead->len(%d)", ret);
            HI_MPI_ADEC_SendStream(0, &stAudioStream, HI_IO_BLOCK);
        }
        AUDIO_STREAM_S stAudioStream;
        /*
        	    static FILE* fp = NULL;
        	    if(fp == NULL)
        	    {
        	        fp = fopen("/mnt/nfs/G711A","rb");
        	        //AV_PRINT(" ------------>fopen %p\n",fp);
        	    }
        */
        //fwrite(&g_AudioStream[chn].uLen, 4,1,fp);
        // fread(m_buffer+4,480,1,fp);

        memcpy(m_buffer+4,packet.data,packet.size);

        stAudioStream.pStream = m_buffer;
        stAudioStream.u32Len = packet.size+4;
        //stAudioStream.u32Len = 484;
        stAudioStream.u64TimeStamp = 1;
        stAudioStream.u32Seq = 1;

        HI_MPI_ADEC_SendStream(0, &stAudioStream, HI_IO_BLOCK);


        //AV_PRINT("--------------> recv voip data len is [%d]",packet.size);
        static FILE* fp = NULL;
        if(fp == NULL)
        {
            fp = fopen("/mnt/nand/rcvG711","w+");
            AV_PRINT(" ------------>fopen %p\n",fp);
        }
        else
        {
            fwrite(packet.data,packet.size,1,fp);
        }
#endif

        if(packet.size != AUDIOFREAMSIZE)
        {
            AV_PRINT("--------------> some error happend, voip data len is error [%d]",packet.size);
        }
        else
        {
            CpyVoipDataToBuf(packet.data,packet.size);
        }
        return nvs::NVS_ERR_OK;
    }

    ///通知发送媒体数据结束
    virtual void endOfStream()
    {
        return;
    }

    int inputAudioData(IN const nvs::StreamPacket& packet)
    {
        if (m_pSink != NULL)
        {
            m_pSink->writePacket(packet);
        }

        return 0;
    }

    ///设置流回调
    virtual int setSink(nvs::StreamSink* pSink)
    {
        AV_PRINT("setSink\n");
        m_pSink = pSink;

        if (m_pSink != NULL)
        {
            nvs::StreamFormat fmt;
            memset(&fmt,0,sizeof(fmt));
            //fmt.audioCodec = nvs::G711A_CODEC;
            //fmt.audioSampleRate = 16000;
            m_pSink->setFormat(fmt);

            /*
            VOIP的启动和编解码的开始由UI进行控制
            该类只负责框架流程的搭建
            */
            //mplay();
#if OPUS
            int sampleRate = 8000;
            int channel = 1;
            int bitrates = 16000;
            int fps = 25;

            m_decoder = openDecoder(  sampleRate,   channel);
            if(m_decoder==NULL)
            {
                sinkEnd();
            }
            m_encoder = openEncoder(  sampleRate,   channel,   bitrates,   fps);
            if(m_encoder==NULL)
            {
                sinkEnd();
            }
#endif
        }

        return nvs::NVS_ERR_OK;
    }

    void doAudio(unsigned char *pData,int len,unsigned long long timestamp)
    {
        nvs::StreamPacket packet;
        packet.data = pData;
        packet.size = len;

#if (AENC_TYPE == AENC_G711)
        packet.flag = nvs::FRAME_G711A;
#else
        packet.flag = nvs::FRAME_PCM;
#endif
        packet.timestamp = timestamp ;

        if (m_pSink != NULL)
        {
#ifdef VOIP_DEBUG
            AV_PRINT("----------->#[app]VOIP   writePacket  -->  [%d] \n",len);
            static int numPacket = 0;
            static nvs::uint64_t dwBeginTime = getTimestamp();
            nvs::uint64_t dwNow = getTimestamp();
            numPacket++;
            int dwSpan = dwNow-dwBeginTime;
            if ( dwSpan > 1000 )
            {

                AV_PRINT( "playpcm ---------1 sec -------->%d packet \r\n",numPacket);

                numPacket = 0;
                dwBeginTime = dwNow;
            }

            static FILE* fp = NULL;
            if(fp == NULL)
            {
                fp = fopen("/mnt/nand/sndG711","w+");
                AV_PRINT(" ------------>fopen %p\n",fp);
            }
            else
            {
                fwrite(pData,len,1,fp);
            }
#endif
            m_pSink->writePacket(packet);
        }
    }


    void caleRate(unsigned long long curFrameMilliseconds)
    {
        ///实际保存时候的帧间隔
        unsigned int Milliseconds = curFrameMilliseconds - m_preFrameMilliseconds;
        ///上一帧比当前帧时间戳还大或者帧间隔时间戳过大
        if (Milliseconds >= 2000) ///2秒
        {
            Milliseconds = 40;
        }

        m_preFrameMilliseconds = curFrameMilliseconds;
        m_timeWaiter.timedwait(Milliseconds);
    }
#if OPUS
    int run()
    {
        FILE *pFile = fopen("./test.opus","rb");

        if ( pFile != NULL )
        {
            int nlen = 0;
            do
            {

                fread(&nlen,sizeof(int),1,pFile);
                unsigned char *pBuff = new unsigned char[nlen+1];
                nlen = fread(pBuff,1,nlen,pFile);
                if ( nlen )
                {

                    doAudio(  pBuff,nlen,getTimestamp());
                }
                delete []pBuff;
                if ( 0 == nlen )
                {
                    break;
                }
                //usleep(40*1000);
                TM_USleep(0,40*1000);
            }
            while (true );
            fclose(pFile);
        }
    }
#else
    int run()
    {
        AVSndBZQCtrlVOIP(BZQ_OPEN_VOICE_CHN);
#if OPUS
        int ret = 0;
        unsigned char opusBuf[1024*40] = {0};
#endif
        mem_Bind(m_hFiFo,g_ChnDataCenter[TM_APP_AUDIO_MIC_INDEX]);
        FRAME_HEAD_T *pstFrameHead = NULL;
        while(!m_canExit)
        {
            int iCount = mem_GetPackageNum(m_hFiFo);
            while(iCount > 30 && !m_canExit)
            {
                HDP hPkg = mem_PopPackageData(m_hFiFo);
                if(hPkg)
                {
                    char *pData = mem_GetPackageData(hPkg);
                    int iFrameLen = mem_GetPackageDataLen(hPkg);
                    int iFrameSeek = 0;
                    while(iFrameSeek<iFrameLen)
                    {
                        pstFrameHead = (FRAME_HEAD_T *)(pData + iFrameSeek);
                        if(pstFrameHead->type==DATA_AUDIO)
                        {
                            doAudio((unsigned char*)(pData+sizeof(FRAME_HEAD_T)+4),\
                                    pstFrameHead->len - 4,\
                                    (pstFrameHead->ts - HOUR8USEC)/1000);
                        }
                        iFrameSeek = iFrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
                    }
                    mem_ReleasePackage(hPkg);
                }
                iCount--;
            }
            //usleep(20*1000);
            TM_USleep(0,10*1000);
        }
        sinkEnd();
        return 0;
    }
#endif
    inline void sinkEnd()
    {
        AV_PRINT("----------->#[app]VOIP   sinkEnd !!! \n" );
        SetVoipAdecStop();
        m_run = false;
        if (m_pSink != NULL)
        {

            m_pSink->endOfStream();

        }

    }

private:
    nvs::StreamSink *m_pSink;
    std::string m_id;

    bool  m_run;
    comn::Event m_timeWaiter;

#if OPUS
    OpusDecoderHandle m_decoder;
    OpusEncoderHandle m_encoder;
#endif
    unsigned long long m_preFrameMilliseconds;
    HBC m_hFiFo;
};

class MsgNotificationSink : public nvs::NotificationSink
{
public:

    /**
     * 注册成功
     */
    virtual int OnRegisterOk(struct tm timeNow)
    {
        AV_PRINT("=====OnRegisterOk=====\n");
#if 0
        struct tm timeSYS;
        timeSYS.tm_year = timeNow.tm_year;
        timeSYS.tm_mon = timeNow.tm_mon;
        timeSYS.tm_mday = timeNow.tm_mday;
        timeSYS.tm_hour = timeNow.tm_hour;
        timeSYS.tm_min = timeNow.tm_min;
        timeSYS.tm_sec = timeNow.tm_sec;
#endif
        //timeSYS.tm_wday = (timeNow.tm_mday+1 + 2*(timeNow.tm_mon+1) + 3*((timeNow.tm_mon+1)+1)/5 + (timeNow.tm_year+1900) + (timeNow.tm_year+1900)/4 - (timeNow.tm_year+1900)/100 + (timeNow.tm_year+1900)/400)%7;
        AV_PRINT("today is [%4d:%02d:%02d %02d:%02d:%02d]\n",
                 timeNow.tm_year+1900,timeNow.tm_mon+1, timeNow.tm_mday,
                 timeNow.tm_hour, timeNow.tm_min, timeNow.tm_sec);

        return nvs::NVS_ERR_OK;
    }
    /**
    * 注册成功
    */
    virtual int OnDisConnect()
    {
        AV_PRINT("=====OnDisConnect=====\n");

        return nvs::NVS_ERR_OK;
    }

    /**
     * 控制云镜
     * @param id    [in] 资源编码
     * @param cmd   [in] 命令
     * @param speed [in] 速度
     * @param data  [in] 动作（开始、停止）
     * @param para  [in] 参数，可为空，预置位时为名字
     */
    virtual int OnControlPtz(const char* id, int cmd, int speed, int data,const char *para)
    {
        AV_PRINT("\n=====OnControlPtz===== id:[%s] cmd:[%d] speed:[%d] data:[%d]\n",id,cmd,speed,data);
        int i = 0;
        if(speed == 0)
        {
            cmd = 0;
            data = 0;
        }

        for(i=0; i<4; i++)
        {
            if((strcmp(id,sipcamerid[i].CameraSIPID)) == 0)
            {
                AV_PRINT("\n=====OnControlPtz===== CameraPDSIP pro:[%d] addr:[%d] \n",\
                         sipcamerid[i].CameraPDSIP,sipcamerid[i].CamerAddSIP);
                m_cptz.onPtzHandle(sipcamerid[i].CameraPDSIP,cmd,sipcamerid[i].CamerAddSIP,speed,data,para);
                break;
            }

        }

        return nvs::NVS_ERR_OK;
    }


    /**
    * 强制关键帧
    * @param id          [in] 资源编码
    * @param streamIdx   [in] 码流类型，0主码流，1子码流
    */
    virtual int OnMakeKeyFrame ( const char *id,IN int streamIdx)
    {
        return nvs::NVS_ERR_OK;
    }


    /**
    * 打开实时视频
    * @param id          [in] 资源编码
    * @param streamIdx   [in] 码流类型，0主码流，1子码流
    * @param ppStream    [in] 码流控制类指针句柄，使用者派生实现
    */
    int OnOpenLiveStream(const char* id, int streamIdx,
                         nvs::MediaStream** ppStream)
    {
        DvrMediaPlayer *pPlayer =  new DvrMediaPlayer(id,streamIdx);
        *ppStream = pPlayer;


        AV_PRINT("=====OnOpenLiveStream===== id:[%s] streamIdx:[%d] \n",id,streamIdx);
        return nvs::NVS_ERR_OK;
    }

    /**
     * 打开音频对讲
     * @param id           [in]   资源编码
     * @param pSink        [out]  数据接收通道
     * @param ppStream     [out]  数据发送通道，使用者派生实现
     */
    virtual int OnOpenPhoneStream(IN const char* id, OUT nvs::StreamSink** ppSink,
                                  OUT nvs::MediaStream** ppStream)
    {
        if (g_iPhone != NULL)
        {
            AV_PRINT("=====OnOpenPhoneStream id====%s [%p] exist!!!!!!!!!!!!!!!!\n",id,g_iPhone);
            return nvs::NVS_ERR_R_OVER_MAXLINK;
        }

        //AVSndBZQCtrlVOIP(2);
        SndUIOpenVOIP();

        PhoneMediaPlayer *pPlayer =  new PhoneMediaPlayer(id);
        LockPhoneMutex();
        g_iPhone=pPlayer;
        UnlockPhoneMutex();
        *ppStream = pPlayer;
        *ppSink = pPlayer;
        AV_PRINT("=====OnOpenPhoneStream id====%s [%p] \n",id,g_iPhone);
        return nvs::NVS_ERR_OK;
    }


    /**
     * 打开历史视频流
     * @param rec          [in] 资源描述
     * @param ppStream     [in] 码流控制类指针句柄，使用者派生实现
     */
    virtual int OnOpenRecordStream(const nvs::RecordItem& rec,
                                   nvs::MediaStream** ppStream)
    {
        AV_PRINT("=====openRecordStream=====  start [%d] end [%d]\n",(unsigned int)rec.beginTime,(unsigned int)rec.endTime);
        DvrMediaPlayer *pPlayer =  new DvrMediaPlayer(rec);
        *ppStream = pPlayer;
        AV_PRINT("=====openRecordStream=====  \n");
        return nvs::NVS_ERR_OK;
    }

    /**
     * 关闭视频
     * @param ppStream    [in] 码流控制类指针句柄
     */
    virtual int OnCloseStream(nvs::MediaStream* pStream)
    {
        AV_PRINT("=====OnCloseStream id==== 111 [%p] \n", pStream);

        delete pStream;
        AV_PRINT("=====OnCloseStream id==== 222 [%p] \n", pStream);
        return nvs::NVS_ERR_OK;
    }


    /**
     * 查询历史视频
     * @param req         [in] 查询参数
     * @param resp        [in] 响应信息
     */
    virtual int OnQueryRecord(const nvs::QueryRecordRequest& req,
                              nvs::QueryRecordResponse& resp)
    {
        if(req.beginTime > req.endTime)
        {
            AV_PRINT("#[av]OnQueryRecord error, time falta beginTime =%d,endTime=%d \n",\
                     (int)req.beginTime, (int)req.endTime);
            return nvs::NVS_ERR_FAIL;
        }
        int chn = 0;
        if(!GetChnByResId(req.id , &chn))
        {
            AV_PRINT("#[av]OnQueryRecord error, NO this id [%s] \n",req.id);
            return nvs::NVS_ERR_FAIL;
        }

        REC_PLAYBACK_T  pstPB = {0};

        pstPB.s32Cmd = chn;
        pstPB.tStartTime = req.beginTime+HOUR8SEC-RESERVETIME;
        pstPB.tEndTime = req.endTime+HOUR8SEC+RESERVETIME;

        AV_PRINT("-------->#[av]OnQueryRecord [chn %d] beginTime =%d,endTime=%d\n",
                 chn,(int)req.beginTime, (int)req.endTime);

        SF_LIST_ST  pstSFList = {0};
        HI_BOOL bLocal = HI_FALSE;
        HI_S32 s32RecType = 0;
        //int iRet = -1;
        int maxbuf = resp.sum;



        resp.sum = 0;
        if(HI_SUCCESS != HI3531_SFByTime_V28181((SF_REQ_CMD_ST*)&pstPB,&pstSFList,bLocal,s32RecType))
        {

            return nvs::NVS_ERR_FAIL;
        }

        nvs::RecordItem  Items;
#ifdef PLAT_QUERY_REC_TIME
        int iRecItemCount = 0;
        for(int i = 0; i<pstSFList.s32FileSegNum; i++)
        {
            if(i==0)
            {
                Items.beginTime= pstSFList.stFileTimeSeg[i].tStartTime;
                Items.endTime= pstSFList.stFileTimeSeg[i].tEndTime;
                if(Items.beginTime<pstPB.tStartTime)
                    Items.beginTime=pstPB.tStartTime;
                if(Items.endTime>pstPB.tEndTime)
                    Items.endTime=pstPB.tEndTime;
                sprintf(Items.recordID,"%d%ld",pstSFList.iChnno,Items.beginTime+RECIDMAGICNUM);
                Items.secrecy = req.secrecy;
                memcpy(Items.type,"time",sizeof(Items.type));
                memcpy(&resp.pItems[(iRecItemCount+resp.sum)%maxbuf],&Items,sizeof(nvs::RecordItem));
                AV_PRINT("-------->#[av]OnQueryRecord %d beginTime =%d,endTime=%d\n",iRecItemCount,(int)Items.beginTime,(int)Items.endTime);
                iRecItemCount++;
            }
            else
            {
                if((Items.endTime+1)==pstSFList.stFileTimeSeg[i].tStartTime)
                {
                    Items.endTime= pstSFList.stFileTimeSeg[i].tEndTime;
                    if(Items.endTime>pstPB.tEndTime)
                        Items.endTime=pstPB.tEndTime;
                    AV_PRINT("-------->#[av]OnQueryRecord Update %d endTime=%d\n",iRecItemCount-1,(int)Items.endTime);
                    memcpy(&resp.pItems[((iRecItemCount-1)+resp.sum)%maxbuf],&Items,sizeof(nvs::RecordItem));
                }
                else
                {
                    Items.beginTime= pstSFList.stFileTimeSeg[i].tStartTime;
                    Items.endTime= pstSFList.stFileTimeSeg[i].tEndTime;
                    if(Items.beginTime<pstPB.tStartTime)
                        Items.beginTime=pstPB.tStartTime;
                    if(Items.endTime>pstPB.tEndTime)
                        Items.endTime=pstPB.tEndTime;
                    sprintf(Items.recordID,"%d%ld",pstSFList.iChnno,Items.beginTime+RECIDMAGICNUM);
                    Items.secrecy = req.secrecy;
                    memcpy(Items.type,"time",sizeof(Items.type));
                    memcpy(&resp.pItems[(iRecItemCount+resp.sum)%maxbuf],&Items,sizeof(nvs::RecordItem));
                    AV_PRINT("-------->#[av]OnQueryRecord %d beginTime =%d,endTime=%d\n",iRecItemCount,(int)Items.beginTime,(int)Items.endTime);
                    iRecItemCount++;
                }
            }
        }
        resp.sum = iRecItemCount;
        resp.sum = resp.sum>maxbuf?maxbuf:resp.sum;
        ModifyRspTime2UTC(resp);
#else
        for(int i = 0; i<pstSFList.s32FileSegNum; i++)
        {
            Items.beginTime= pstSFList.stFileTimeSeg[i].tStartTime;
            Items.endTime= pstSFList.stFileTimeSeg[i].tEndTime;

            sprintf(Items.recordID,"%d%ld",pstSFList.iChnno,Items.beginTime+RECIDMAGICNUM);
            Items.secrecy = req.secrecy;
            memcpy(Items.type,"time",sizeof(Items.type));
            memcpy(&resp.pItems[(i+resp.sum)%maxbuf],&Items,sizeof(nvs::RecordItem));
        }
        resp.sum = pstSFList.s32FileSegNum;
        resp.sum = resp.sum>maxbuf?maxbuf:resp.sum;
        ModifyRspTime2UTC(resp);
#endif
        return nvs::NVS_ERR_OK;
    }

    /**
     * 手动录像
     * @param id         [in] 资源编码
     * @param isStart    [in] 1启动，0停止
     */
    virtual int OnRecord(const char* id,int isStart)
    {
        return nvs::NVS_ERR_OK;
    }

    /**
     * 布防
     * @param id         [in] 资源编码
     * @param guardFlag  [in] 0撤防，1布防
     */
    virtual int OnSetGuard(const char* id, int guardFlag)
    {

        Setalarmenableflag(id,guardFlag);
        AV_PRINT("====OnSetGuard===[%s] guardFlag:[%d]\n",id,guardFlag);
        return nvs::NVS_ERR_OK;
    }

    /**
     * 报警复位
     * @param id         [in] 资源编码
     * @param alarmType  [in] 告警类型
     */
    virtual int OnResetAlarm(const char* id, int alarmType)
    {
        /*报警复位*/
        TM_Reset_Alarm(id,alarmType);

        AV_PRINT("====OnResetAlarm===id:[%s],[%d]\n",id,alarmType);
        return nvs::NVS_ERR_OK;
    }


    /**
     * 设备信息查询
     * @param id         [in]  资源编码
     * @param info       [out] 设备信息
     */
    virtual int OnGetDeviceInfo(const char* id,nvs::DeviceInfoParam &info)
    {

        GetXmlDeviceInfo(&info);

        AV_PRINT("====OnGetDeviceInfo===[%s]\n",id);
        return nvs::NVS_ERR_OK;
    }


    /**
     * 设备状态查询
     * @param id         [in] 资源编码
     */
    virtual int OnGetDeviceStatus(const char* id,nvs::DeviceStatusParam &status)
    {
        struct timeval tv;
        char mytime[20] = "";

        GetXmlDeviceStatus(&status);
        gettimeofday(&tv,NULL);
        strftime(mytime,sizeof(mytime),"%Y:%m:%dT%T",localtime(&tv.tv_sec));
        AV_PRINT("Time:%s\n",mytime);
        strcpy(status.deviceTime,mytime);

        AV_PRINT("====OnGetDeviceStatus===[%s]\n",id);
        return nvs::NVS_ERR_OK;
    }


    /**
     * 设备远程启动
     * @param id         [in] 资源编码
     */
    virtual int OnBootDevice(const char* id)
    {
        //system("/sbin/reboot");
        //RebootArm();
        restdevice();
        AV_PRINT("====OnBootDevice===[%s]\n",id);
        return nvs::NVS_ERR_OK;
    }
    int setPtzDataSendcb(DataCallback_t proc)
    {
        return m_cptz.SetSendDataHandle( proc);
    }
private:
    CPTZManager m_cptz;
};

int ModifyRspTime2UTC(nvs::QueryRecordResponse& resp)
{
    for(int i = 0; i<resp.sum; i++)
    {
        resp.pItems[i].beginTime -= HOUR8SEC;
        resp.pItems[i].endTime -= HOUR8SEC;
    }
    return 0;
}


NET_VIDEO_API int NET_VIDEO_METHOD DevStopVoip()
{
    if(g_iPhone != NULL)
    {
        LockPhoneMutex();
        g_iPhone->sinkEnd();
        UnlockPhoneMutex();
    }

    return nvs::NVS_ERR_OK;
}
NET_VIDEO_API int NET_VIDEO_METHOD DevStartDialing()
{
    LockPhoneMutex();
    if(g_iPhone != NULL)
    {
        AVSndBZQCtrlVOIP(BZQ_VOIP_RING_CMD);
    }
    UnlockPhoneMutex();
    return nvs::NVS_ERR_OK;
}
NET_VIDEO_API int NET_VIDEO_METHOD DevStartVoip()
{
    if(g_iPhone != NULL)
    {
        LockPhoneMutex();
        g_iPhone->mplay();
        UnlockPhoneMutex();
    }
    return nvs::NVS_ERR_OK;
}
#ifdef PLAT_QUERY_REC_TIME
int GetSeekTimeFileIndex(SF_LIST_ST* _pFileList,REC_PLAYBACK_T *_pstPB,int _seekpos,time_t *_seektime)
{
    int i = 0;
    time_t seektime = ((_pstPB->tEndTime - _pstPB->tStartTime)*_seekpos/100)+_pstPB->tStartTime;
    AV_PRINT("------> [seektime %d] [start %d]  [end %d]",
             (int)seektime,(int)_pstPB->tStartTime,(int)_pstPB->tEndTime);
    if(seektime<_pFileList->stFileTimeSeg[0].tStartTime
            ||seektime>_pFileList->stFileTimeSeg[_pFileList->s32FileSegNum-1].tEndTime)
        return _pFileList->s32FileSegNum;
    for(i = 0; i<_pFileList->s32FileSegNum; i++)
    {
        if(_pFileList->stFileTimeSeg[i].tStartTime<=seektime
                &&seektime<=_pFileList->stFileTimeSeg[i].tEndTime)
        {
            *_seektime = seektime;
            break;
        }
    }
    return i;
}
#endif
bool GetChnByResId(const char* id,int* pchn)
{
    bool ret = false;

    for (int i = 0; i < TOTAL_VI_CHN_NUM; i++)
    {
        if((strcmp(id, devicecamparam[i].PlatformDevId)) == 0)
        {
            *pchn = i;
            ret = true;
        }
    }


    return ret;
}
void testResReport()
{
    nvs::ResNode pNodes[12];

    GetXmlPlatformRegInfo(12,pNodes);

    int ret = pushResNode(pNodes,12);

    AV_PRINT("pushResNode===[%d][%d]\n",ret,sizeof(nvs::ResNode));
}

void Init28181Para()
{
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
    //XmlError("Init28181Para");
}

/* BEGIN: Added by mabuyun, 2015/7/24  DES:20150724  PN:无线对接代码 */

int GetIpAddr(const char *InterfaceName,unsigned int *IpAddr)
{
    int s;

    if((InterfaceName == NULL) || (IpAddr == NULL))
    {
        //参数非法
        return -2;
    }

    //创建SOCKET
    if((s = socket(PF_INET,SOCK_STREAM,0)) < 0)
    {
        return -1;
    }

    struct ifreq ifr;

    strcpy(ifr.ifr_name,InterfaceName);
    //获取IP地址
    if(ioctl(s,SIOCGIFADDR,&ifr) < 0)
    {
        //失败
        close(s);
        return -1;
    }

    //IP地址
    *IpAddr = ((struct sockaddr_in *)&ifr.ifr_ifru.ifru_addr)->sin_addr.s_addr;

    *IpAddr = ntohl(*IpAddr);//字节序转换

    close(s);
    s = -1;
    return 0;
}

/*****************************************
 *函数名    ：getipintrface
 *功能描述	：发送命令
 *入口参数	：neti 查找网络类型，ipaddr 当前网络地址
 *出口参数	：
 *返回值    ：成功返回1，失败返回相应的负值
 *最后修改  ：2011-09-15
 *****************************************/
int getipintrface(const char* neti,char* ipaddr)
{
    unsigned int IpAddr = 0;

    if(GetIpAddr(neti,&IpAddr) != 0)
    {
        return -1;
    }

    sprintf(ipaddr,"%d.%d.%d.%d",(IpAddr >> 24) & 0xff,(IpAddr >> 16) & 0xff,\
            (IpAddr >> 8) & 0xff,(IpAddr >> 0) & 0xff);

    return 1;
}

/*****************************************************************************
 函 数 名  : process_Ip_add_zero
 功能描述  : 给ip地址不足三个数字的字段填充0
 输入参数  : unsigned char *ip
             int len
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年3月25日
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
void process_Ip_add_zero(unsigned char *ip,int len)
{
    char *p = NULL;
    int ip_i[4] = {0};
    int i = 0;
    char buf[16] = {'\0'};

    if ( !ip )
    {
        AV_PRINT("#[para]: in process_Ip_add_zero, in ip = NULL!\n");
        return;
    }

    memcpy(buf, ip, len);

    ip_i[0] = atoi(buf);
    for(i = 1; i < 4; i++)
    {
        p = strstr(buf, ".");
        if(p != NULL)
        {
            ip_i[i] = atoi(p+1);
            p[0] = '0';/*清除已处理的'.'*/
        }
    }
    memset(buf,0,sizeof(buf));
    snprintf(buf, sizeof(buf), "%03d.%03d.%03d.%03d", ip_i[0],ip_i[1],ip_i[2],ip_i[3]);
    memcpy(ip, buf, len);
    //return 0;

}
/*****************************************************************************
 函 数 名  : process_Ip
 功能描述  : 去掉ip地址中多余的0
 输入参数  : unsigned char *ip
             int len
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年3月25日
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
void process_Ip(unsigned char *ip,int len)
{
    char *p = NULL;
    int ip_i[4] = {0};
    int i = 0;
    char buf[16] = {'\0'};

    memcpy(buf, ip, len);

    if ( !ip )
    {
        AV_PRINT("#[para]: in process_Ip, in ip = NULL!\n");
        return ;
    }

    ip_i[0] = atoi(buf);
    for(i = 1; i < 4; i++)
    {
        p = strstr(buf, ".");
        if(p != NULL)
        {
            ip_i[i] = atoi(p+1);
            p[0] = '0';/*清除已处理的'.'*/
        }
    }
    memset(buf,0,sizeof(buf));
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip_i[0],ip_i[1],ip_i[2],ip_i[3]);
    memcpy(ip, buf, len);
    //return 0;

}
#ifdef PLT_PARA_BK
void PlatForm_SendPlatInfo2UI()
{
    //ip:16 port:2
    char cSendBuf[nvs::MAX_IPV4_LEN+2] = {0};
    memcpy(cSendBuf,g_PltSipData.ip[g_PltSipData.iaddress],nvs::MAX_IPV4_LEN);
    cSendBuf[nvs::MAX_IPV4_LEN] = g_PltSipData.port[g_PltSipData.iaddress]&0xFF;
    cSendBuf[nvs::MAX_IPV4_LEN + 1] = g_PltSipData.port[g_PltSipData.iaddress]>>8&0xFF;
    DOMAIN_PackageData(ID_UI, AV_PLT_ADDRESS, (HI_U8 *)cSendBuf, sizeof(cSendBuf));
}

void DevGetPlatInfo(char *pBuf)
{
    memcpy(pBuf,g_PltSipData.ip[g_PltSipData.iaddress],nvs::MAX_IPV4_LEN);
    pBuf[nvs::MAX_IPV4_LEN] = g_PltSipData.port[g_PltSipData.iaddress]&0xFF;
    pBuf[nvs::MAX_IPV4_LEN + 1] = g_PltSipData.port[g_PltSipData.iaddress]>>8&0xFF;
}
void DevSndPlatInfo2UI()
{
    PlatForm_SendPlatInfo2UI();
}

void PlatForm_SetSipFlag(int iflag)
{
    pthread_mutex_lock(&g_PltSipDatlock);
    g_PltSipData.flag = iflag;
    pthread_mutex_unlock(&g_PltSipDatlock);
}

int PlatForm_GetSipAddressFlag()
{
    return g_PltSipData.iaddress;
}
int PlatForm_CheckNetWork()
{
    char SimIP[16] = {0};//SIM卡IP
    if(getipintrface("ppp0",SimIP) != -1)
    {
        AV_PRINT("#####SIM Card Online#####[%s]",SimIP);
        return 0;
    }
    return -1;
}

void* SwitchNetThread(void*)
{
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    int iFailTimes = 0;
    int iSleep = PLT_NORMAL_SLEEP_ARG;
    static int iPrint = -1;
    //XmlError("SwitchNetThread");
    while(1)
    {
        switch(g_PltSipData.flag)
        {
        case FLAG_WAIT_SIM_ONLINE:
            //XmlError("FLAG_WAIT_SIM_ONLINE");
            if(iPrint!=FLAG_WAIT_SIM_ONLINE)
            {
                AV_PRINT("#####Wait SIM Card Online#####");
                iPrint = FLAG_WAIT_SIM_ONLINE;
            }
            //网络是否畅通
            if(PlatForm_CheckNetWork()==0)
                PlatForm_SetSipFlag(FLAG_CHEKCONNECT);
            break;
        case FLAG_CHEKCONNECT:
        {
            //XmlError("FLAG_CHEKCONNECT");
            if(iPrint!=FLAG_CHEKCONNECT)
            {
                AV_PRINT("#####Check Login state#####");
                iPrint = FLAG_CHEKCONNECT;
            }
            if(hasLogin()==0)
            {
                iFailTimes = 0;
                PlatForm_SetSipFlag(FLAG_CONNECTED);
            }
            else
            {
                if(iFailTimes>PLT_TRY_CONNECT_FAIL_TIME)
                {
                    iFailTimes = 0;
                    if(g_PltSipData.iaddress==FLAG_MAIN_IP)
                        PlatForm_SetSipFlag(FLAG_SWITCH_TO_SUB);
                    else
                        PlatForm_SetSipFlag(FLAG_SWITCH_TO_MAIN);
                    AV_PRINT("#####Login Fail#####");
                }
                else
                    iFailTimes++;
            }
            iSleep = PLT_NORMAL_SLEEP_ARG;
        }
        break;
        case FLAG_CONNECTED:
        {
            if(iPrint!=FLAG_CONNECTED)
            {
                AV_PRINT("#####Login Success#####");
                iPrint = FLAG_CONNECTED;
            }
            if(hasLogin()==0)
                iSleep = PLT_CONNECT_SLEEP_ARG;
            else
            {
                PlatForm_SetSipFlag(FLAG_WAIT_SIM_ONLINE);
                iSleep = PLT_NORMAL_SLEEP_ARG;
            }

            iFailTimes = 0;
        }
        break;
        case FLAG_SWITCH_TO_MAIN:
        {
            g_PltSipData.iaddress = FLAG_MAIN_IP;

            nvs::PlatformRegInfo reg;


            GetXmlPlatformRegInfo(&reg);

            if((memcmp(g_PltSipData.ip[FLAG_MAIN_IP],reg.serverAddr.ip,nvs::MAX_IPV4_LEN) != 0)||\
                    (g_PltSipData.port[FLAG_MAIN_IP] != reg.serverAddr.port)||\
                    (memcmp(g_PltSipData.id,reg.serverAddr.id,sizeof(g_PltSipData.id)) != 0))
            {
                memcpy(reg.serverAddr.ip,g_PltSipData.ip[FLAG_MAIN_IP],nvs::MAX_IPV4_LEN);
                reg.serverAddr.port = g_PltSipData.port[FLAG_MAIN_IP];
                memcpy(reg.serverAddr.id,g_PltSipData.id,sizeof(g_PltSipData.id));

                SetXmlPlatformRegInfo(&reg);
            }


            reLoginDvs(&reg);

            PlatForm_SetSipFlag(FLAG_WAIT_SIM_ONLINE);

            iFailTimes = 0;

            iSleep = PLT_NORMAL_SLEEP_ARG;

            if(iPrint!=FLAG_SWITCH_TO_MAIN)
            {
                AV_PRINT("#####Switch to main address#####[%s][%d]"
                         ,reg.serverAddr.ip,reg.serverAddr.port);
                iPrint = FLAG_SWITCH_TO_MAIN;
            }

            PlatForm_SendPlatInfo2UI();
        }
        break;
        case FLAG_SWITCH_TO_SUB:
        {
            g_PltSipData.iaddress = FLAG_SUB_IP;
            nvs::PlatformRegInfo reg;
            GetXmlPlatformRegInfo2(&reg);
            if((memcmp(g_PltSipData.ip[FLAG_SUB_IP],reg.serverAddr.ip,nvs::MAX_IPV4_LEN) != 0)||\
                    (g_PltSipData.port[FLAG_SUB_IP] != reg.serverAddr.port)||\
                    (memcmp(g_PltSipData.id,reg.serverAddr.id,sizeof(g_PltSipData.id)) != 0))
            {
                memcpy(reg.serverAddr.ip,g_PltSipData.ip[FLAG_SUB_IP],nvs::MAX_IPV4_LEN);
                reg.serverAddr.port = g_PltSipData.port[FLAG_SUB_IP];
                memcpy(reg.serverAddr.id,g_PltSipData.id,sizeof(g_PltSipData.id));
                SetXmlPlatformRegInfo2(&reg);
            }
            reLoginDvs(&reg);
            PlatForm_SetSipFlag(FLAG_WAIT_SIM_ONLINE);

            iFailTimes = 0;

            iSleep = PLT_NORMAL_SLEEP_ARG;

            if(iPrint!=FLAG_SWITCH_TO_SUB)
            {
                AV_PRINT("#####Switch to sub address#####[%s][%d]"
                         ,reg.serverAddr.ip,reg.serverAddr.port);
                iPrint = FLAG_SWITCH_TO_SUB;
            }

            PlatForm_SendPlatInfo2UI();
        }
        break;
        }
        //usleep(iSleep*PLT_SLEEP_10S);
        TM_USleep(iSleep*10,0);
    }
    return NULL;
}
void PlatForm_InitSipData()
{
    nvs::PlatformRegInfo reg;

    GetXmlPlatformRegInfo(&reg);
    memcpy(g_PltSipData.id,reg.serverAddr.id,sizeof(reg.serverAddr.id));
    memcpy(g_PltSipData.ip[0],reg.serverAddr.ip,sizeof(reg.serverAddr.ip));
    g_PltSipData.port[0] = reg.serverAddr.port;

    GetXmlPlatformRegInfo2(&reg);
    memcpy(g_PltSipData.ip[1],reg.serverAddr.ip,sizeof(reg.serverAddr.ip));
    g_PltSipData.port[1] = reg.serverAddr.port;

    AV_PRINT("#####PlatForm Data[%d][%d][%s][%s][%s][%d][%d]#####"
             ,g_PltSipData.flag,g_PltSipData.iaddress
             ,g_PltSipData.id,g_PltSipData.ip[0]
             ,g_PltSipData.ip[1],g_PltSipData.port[0]
             ,g_PltSipData.port[1]);
    //XmlError("PlatForm_InitSipData");
}
int PlatForm_StartCheckThread()
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;

    s32Ret = pthread_mutex_init(&g_PltSipDatlock, NULL);

    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "pthread_mutex_init failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    PlatForm_InitSipData();

    s32Ret = pthread_create(&tid, NULL, SwitchNetThread, NULL);

    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "create SwitchNetThread thread err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "detach SwitchNetThread thread err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
#endif
NET_VIDEO_API int NET_VIDEO_METHOD DevGetNewPlatInfo(void* para)
{
    nvs::PlatformRegInfo reg;
    memset(&reg,0,sizeof(nvs::PlatformRegInfo));
    GetXmlPlatformRegInfo(&reg);
    AV_VIDEOPLTPARA_INFO_ST *pstplatinfo = (AV_VIDEOPLTPARA_INFO_ST *)para;
#ifdef PLT_PARA_BK
    char ip[16] = {0};
    unsigned short port = 0;
    char machineID[9] = {0};
    unsigned int macid = 0;
    //主IP
    memcpy(ip,pstplatinfo->ip,15);
    process_Ip((unsigned char*)ip,15);
    port = pstplatinfo->port[1]|(pstplatinfo->port[0]<<8);
    macid = (pstplatinfo->machineId[0])|(pstplatinfo->machineId[1]<<8)|(pstplatinfo->machineId[2]<<16);
    sprintf(machineID,"%d",macid);

    //修改主IP信息
    memcpy(g_PltSipData.ip[FLAG_MAIN_IP],ip,nvs::MAX_IPV4_LEN);
    g_PltSipData.port[FLAG_MAIN_IP] = port;
    memcpy(g_PltSipData.id,machineID,sizeof(machineID));


    BOOL bMainChange = FALSE;
    BOOL bSubChange = FALSE;
    //检测主IP是否变化
    if((memcmp(ip,reg.serverAddr.ip,nvs::MAX_IPV4_LEN) != 0)||\
            (port != reg.serverAddr.port)||\
            (memcmp(machineID,reg.serverAddr.id,sizeof(machineID)) != 0))
    {

        debug_print("machineID from schedule",machineID,sizeof(machineID));
        debug_print("machineID from reg",reg.serverAddr.id,sizeof(machineID));
        debug_print("serverIp from schedule",g_PltSipData.ip[FLAG_MAIN_IP],nvs::MAX_IPV4_LEN);
        debug_print("serverIp from reg",reg.serverAddr.ip,nvs::MAX_IPV4_LEN);
        memcpy(reg.serverAddr.ip,g_PltSipData.ip[FLAG_MAIN_IP],nvs::MAX_IPV4_LEN);
        reg.serverAddr.port = g_PltSipData.port[FLAG_MAIN_IP];
        memcpy(reg.serverAddr.id,g_PltSipData.id,sizeof(g_PltSipData.id));

        SetXmlPlatformRegInfo(&reg);

        bMainChange = TRUE;

        AV_PRINT("Main Address Change!!!!");
    }

    AV_PRINT("Main Address:[%s][%d][%s]",g_PltSipData.ip[FLAG_MAIN_IP],g_PltSipData.port[FLAG_MAIN_IP],g_PltSipData.id);
    //备用IP
    memset(ip,0,sizeof(ip));

    memset(&reg,0,sizeof(reg));

    GetXmlPlatformRegInfo2(&reg);
    memcpy(ip,pstplatinfo->ipbk,15);
    process_Ip((unsigned char*)ip,15);
    port = pstplatinfo->portbk[1]|(pstplatinfo->portbk[0]<<8);

    memcpy(g_PltSipData.ip[FLAG_SUB_IP],ip,nvs::MAX_IPV4_LEN);
    g_PltSipData.port[FLAG_SUB_IP] = port;

    if((memcmp(ip,reg.serverAddr.ip,nvs::MAX_IPV4_LEN) != 0)||\
            (port != reg.serverAddr.port)||\
            (memcmp(machineID,reg.serverAddr.id,sizeof(machineID)) != 0))
    {

        memcpy(reg.serverAddr.ip,g_PltSipData.ip[FLAG_SUB_IP],nvs::MAX_IPV4_LEN);

        reg.serverAddr.port = g_PltSipData.port[FLAG_SUB_IP];

        memcpy(reg.serverAddr.id,g_PltSipData.id,sizeof(g_PltSipData.id));

        SetXmlPlatformRegInfo2(&reg);

        bSubChange = TRUE;

        AV_PRINT("Sub Address Change!!!!");
    }

    if(bMainChange || (bSubChange&&PlatForm_GetSipAddressFlag()==FLAG_MAIN_IP))
    {
        PlatForm_SetSipFlag(FLAG_SWITCH_TO_MAIN);
    }
    else if(bSubChange)
    {
        PlatForm_SetSipFlag(FLAG_SWITCH_TO_SUB);
    }

    AV_PRINT("Sub Address:[%s][%d][%s]",g_PltSipData.ip[FLAG_SUB_IP],g_PltSipData.port[FLAG_SUB_IP],g_PltSipData.id);
#else
    char ip[16] = {0};
    unsigned short port = 0;
    char machineID[8] = {0};
    unsigned int macid = 0;

    memcpy(ip,pstplatinfo->ip,15);
    process_Ip((unsigned char*)ip,15);
    port = pstplatinfo->port[1]|(pstplatinfo->port[0]<<8);
    macid = (pstplatinfo->machineId[0])|(pstplatinfo->machineId[1]<<8)|(pstplatinfo->machineId[2]<<16);
    sprintf(machineID,"%d",macid);

    if((memcmp(ip,reg.serverAddr.ip,nvs::MAX_IPV4_LEN) != 0)||\
            (port != reg.serverAddr.port)||\
            (memcmp(machineID,reg.serverAddr.id,sizeof(machineID)) != 0))
    {
        memcpy(reg.serverAddr.ip,ip,nvs::MAX_IPV4_LEN);
        reg.serverAddr.port = port;
        memcpy(reg.serverAddr.id,machineID,sizeof(machineID));
        SetXmlPlatformRegInfo(&reg);
        reLoginDvs(&reg);
    }
#endif
    return 0;
}

/*****************************************************************************
 Prototype    : Entry28181Thread
 Description  : 28181通讯线程入口
 Input        : void*
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/7/24
    Author       : mabuyun
    Modification : Created function

*****************************************************************************/
void* Entry28181Thread(void*)
{
#ifndef	PLT_PARA_BK
    init_28181();
    Init28181Para();
    enableDebug(1);
#endif
    AV_PRINT("Entry28181ThreadPID@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());

    MsgNotificationSink* pSinkObj = new MsgNotificationSink;
    pSinkObj->setPtzDataSendcb(SendPtzMsgToMcu);

    int ret = setNotificationSink(pSinkObj);
    AV_PRINT("\033[42;37m setNotificationSink===%d \033[0m\n",ret);


    nvs::PlatformRegInfo reg;
    GetXmlPlatformRegInfo(&reg);
    //strcpy(reg.serverAddr.id,"12345678");
    ret = login(&reg);
    //AV_PRINT("\033[42;37m  login===%d\033[0m\n",ret);
    testResReport();
    //XmlError("testResReport\n");
    return NULL;
}
/* END:   Added by mabuyun, 2015/7/24   PN:无线对接代码 */

int DevSearchStart()
{
    int iRet = -1;
    pthread_t p_id;

    iRet = pthread_create(&p_id, NULL, DevSearchEntry, NULL);
    if ( iRet < 0 )
    {
        AV_PRINT( "DevSearchStartm thread err(%s)", ERR_STR);
    }

    return iRet;
}
int Start_28181main()
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;
    static HI_BOOL bThdStart = HI_FALSE;

    if (HI_FALSE == bThdStart)
    {
#ifdef	PLT_PARA_BK
        init_28181();

        Init28181Para();

        enableDebug(1);

        PlatForm_StartCheckThread();
#endif
        s32Ret = pthread_create(&tid, NULL, Entry28181Thread, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "create Entry28181Thread thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        s32Ret = pthread_detach(tid);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "detach Entry28181Thread thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }
        //XmlError("Start_28181main");
        bThdStart = HI_TRUE;
    }

    return HI_SUCCESS;
}

int reLoginDvs( const nvs::PlatformRegInfo *pInfo)
{
    AV_PRINT("------------> reLoginDvs\n");
    int ret =  0;
    if(pInfo == NULL)
    {
        nvs::PlatformRegInfo reg;
        GetXmlPlatformRegInfo(&reg);
        reLogin(&reg);
    }
    else
    {
        reLogin( pInfo);
    }
    testResReport();

    return ret;
}

