#include "av_public_head.h"
#include "av_socket.h"
#include "28181_AV_API.h"
#include <sys/time.h>
#include "av_video_init.h"
#include "commctl.h"
TM_APP_VoipCache_st g_VoipStream = {TASK_STOP_E,100,0,0,{0}};
pthread_mutex_t g_PhoneMutex;

#ifdef TIME_TAGS
HI_U64 g_SetPts_28281[APP_VI_CHNCNT*2] = {0};
HI_BOOL bFindFirstIFrame_28281[APP_VI_CHNCNT*2] = {HI_FALSE};
time_t tGetVencStartTime_28281[APP_VI_CHNCNT*2] = {0};
#endif
//sem_t
void init_28181()
{
    pthread_mutex_init(&g_PhoneMutex,NULL);
}
#ifdef TIME_TAGS
HI_U64 GetVideoU64Framets(VENC_STREAM_S *pstStream,int iCh)
{
    HI_U64 U64Framets = 0;
    HI_CHAR cFrameType;
    cFrameType = *((HI_U8 *)(pstStream->pstPack[0].pu8Addr + 4));
    if (cFrameType==0x67)
    {
        if (bFindFirstIFrame_28281[iCh] == HI_FALSE)
        {
            bFindFirstIFrame_28281[iCh] = HI_TRUE;
            tGetVencStartTime_28281[iCh] = time((time_t *)NULL);
        }
        U64Framets = (HI_U64)tGetVencStartTime_28281[iCh]*1000000;
        tGetVencStartTime_28281[iCh]++;
        g_SetPts_28281[iCh] = U64Framets;
    }
    else
    {
        U64Framets = g_SetPts_28281[iCh]+40000;
        g_SetPts_28281[iCh] = U64Framets;
    }
    return U64Framets;
}
#endif
unsigned long long mygetLocalTime()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int  m_second = tp.tv_sec;
    int  m_millisecond = tp.tv_usec / 1000;
    return (unsigned long long  )m_second*1000+m_millisecond;
}

/*****************************************************************************
 Prototype    : DOMAIN_SndBZQCtrlVOIP
 Description  : 向报站器发送IP对讲控制命令
 Input        : HI_U8 pktCmd
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/9/2
    Author       : mabuyun
    Modification : Created function

*****************************************************************************/
HI_S32 AVSndBZQCtrlVOIP(HI_U8 pktCmd )
{
    /* 发送控制消息 */
    DOMAIN_PackageData(ID_BZQ, AV_VOIP_CTRLAOAI,&pktCmd, sizeof(pktCmd));

    return HI_SUCCESS;
}

HI_S32 SndUIOpenVOIP(HI_VOID)
{
    VOIP_STAT_T stvoipStat;

    /* nvs中连接上是0，av协议连接上是1，非链接为0 */
    stvoipStat.s32Cmd =  1;
    stvoipStat.limittime = 0;
    DOMAIN_PackageData(ID_UI, AV_OPEN_VOIP, (HI_U8 *)&stvoipStat, sizeof(VOIP_STAT_T));

    return HI_SUCCESS;
}

HI_S32 SndUIStopVOIP(HI_VOID)
{
    VOIP_STAT_T stvoipStat;

    /* nvs中连接上是0，av协议连接上是1，非链接为0 */
    stvoipStat.s32Cmd =  0;
    stvoipStat.limittime = 0;
    DOMAIN_PackageData(ID_UI, AV_CLOSE_VOIP, (HI_U8 *)&stvoipStat, sizeof(VOIP_STAT_T));

    return HI_SUCCESS;
}
#define DEBUG_P printf("-------------->line [%d]\n",__LINE__);
void SetVoipAdecStop()
{
    g_VoipStream.ucRun = TASK_STOP_E;
    g_VoipStream.rdIndex = 0;
    g_VoipStream.wrIndex = 0;
}

char GetVoipBufCount()
{
    return (g_VoipStream.wrIndex-g_VoipStream.rdIndex+VOIPMAXBUFSIZE)%VOIPMAXBUFSIZE;
}

void CpyVoipDataToBuf(HI_U8* buf,int len)
{
    if(GetVoipBufCount() == VOIPMAXBUFSIZE)
    {
        g_VoipStream.rdIndex = (1+g_VoipStream.rdIndex) % VOIPMAXBUFSIZE;
    }
    memcpy(g_VoipStream.buf[(int)g_VoipStream.wrIndex],buf,AUDIOFREAMSIZE);
    g_VoipStream.wrIndex = (1+g_VoipStream.wrIndex) % VOIPMAXBUFSIZE;
}

#define DELAYFREAMNUM       10
void* StartVoipAdecPthread(void* arg)
{
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    bool playstate = false;
    char count = 0;

    /* 4字节海思头 */
    HI_U8 audioBuf[AUDIOFREAMSIZE+4] = {0};
    audioBuf[0] = 0x00;
    audioBuf[1] = 0x01;
    audioBuf[2] = 0xf0;
    audioBuf[3] = 0x00;
    HI_U8* pBuf = audioBuf+4;
    COMMCTL_SetMicVolume(g_VoipStream.ucMicVolume);
    while(g_VoipStream.ucRun == TASK_WORK_E)
    {
        /* 25 = VOIPMAXBUFSIZE / 2 */
        count = GetVoipBufCount();
        if(count == 0)
        {
            playstate = false;
        }
        if((count>= DELAYFREAMNUM) || (playstate == true))
        {
            AUDIO_STREAM_S stAudioStream;
            memcpy(pBuf,g_VoipStream.buf[(int)g_VoipStream.rdIndex],AUDIOFREAMSIZE);
            g_VoipStream.rdIndex = (1+g_VoipStream.rdIndex) % VOIPMAXBUFSIZE;
            stAudioStream.pStream = audioBuf;
            stAudioStream.u32Len = AUDIOFREAMSIZE+4;
            stAudioStream.u64TimeStamp = 1;
            stAudioStream.u32Seq = 1;

            HI_MPI_ADEC_SendStream(0, &stAudioStream, HI_IO_BLOCK);
            playstate = true;
        }
        if(playstate)
        {
            //usleep(1000);
            TM_USleep(0,1000);
        }
        else
        {
            //usleep(500000);
            TM_USleep(0,500000);
        }
    }

    return NULL;
}
void SetVoipMicVolume(HI_U8 u8Volume)
{
    g_VoipStream.ucMicVolume = u8Volume;
}
HI_S32 VoipAdecEntry()
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid;

    if(g_VoipStream.ucRun != TASK_WORK_E)
    {

        g_VoipStream.ucRun = TASK_WORK_E;
        g_VoipStream.rdIndex = 0;
        g_VoipStream.wrIndex = 0;

        s32Ret = pthread_create(&tid, NULL, StartVoipAdecPthread, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "create GetVencStream thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

        s32Ret = pthread_detach(tid);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "detach GetVencStream thread err(%s)", ERR_STR);
            return HI_FAILURE;
        }

    }
    return HI_SUCCESS;
}

void initPhoneMutex()
{
    pthread_mutex_init(&g_PhoneMutex,NULL);
}

void LockPhoneMutex()
{
    pthread_mutex_lock(&g_PhoneMutex);
}

void UnlockPhoneMutex()
{
    pthread_mutex_unlock(&g_PhoneMutex);
}

