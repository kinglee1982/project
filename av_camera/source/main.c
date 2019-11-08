/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

#include "av_public_head.h"
#include "av_sys.h"
#include "av_init.h"
#include "av_video_init.h"
#include "av_audio_init.h"
#include "av_vivo.h"
#include "av_osd.h"
#include "av_video_proc.h"
#include "av_socket.h"
#include "av_msgque_proc.h"
#include "av_mem.h"
#include "av_store.h"
#include "av_hddfsm.h"
#include "av_split.h"
#include "memfunc.h"
extern int Start_28181main();
HI_BOOL g_bExit = HI_FALSE;


HI_BOOL Get_Exit()
{
    return g_bExit;
}

HI_BOOL Set_Exit(HI_BOOL bExit)
{
    g_bExit = bExit;
    return g_bExit;
}

HI_VOID HI3521_SignalExit()
{
    HDD_UmountDisk(3);
    /*停止音视频编码*/
    HI3521_StopAVEnc();
    AV_PRINT("HI3521_StopAVEnc success");
    /*停止视频编码通道 */
    HI3521_VencExit();
    AV_PRINT("HI3521_VencExit success");
    /*停止音频编码通道 */
    HI3521A_AencExit();
    AV_PRINT("HI3521A_AencExit success");
    /*停止AI*/
    HI3521A_AiExit();
    AV_PRINT("HI3521A_AiExit success");
    /*停止AO*/
    HI3521A_AoExit();
    AV_PRINT("HI3521A_AoExit success");
    /*停止VI*/
    HI3521_ViExit();
    AV_PRINT("HI3521_ViExit success");
    /*停止VO*/
    HI3521_StopAllVo();
    AV_PRINT("HI3521_StopAllVo success");

    HI_MPI_SYS_Exit();

    HI_MPI_VB_Exit();

    AV_PRINT("SignalExit success");
}

HI_VOID SignalHander(HI_S32 signal)
{
    static HI_S32 s32Cnt = 0;
    s32Cnt++;
    int i;
    AV_PRINT("bExit(%d) err(%s) signal(%d) pid(%d) tid(%d) invoke_cnt(%d)", Get_Exit(), ERR_STR, signal, getpid(), gettid(), s32Cnt);
    Set_Exit(HI_TRUE);

    for (i = 0; i < 3; i ++)
    {
        HI3521_SignalExit();
        system("killall -9 av");
    }

    if (signal == SIGPIPE)
    {
        AV_PRINT("get an SIGPIPE");
    }

    AV_PRINT("SignalHander(signal:%d)(pid:%d)\n", signal, getpid());

    exit(1);
}

HI_VOID AV_SYS_INIT(HI_VOID)
{
    return;
}

HI_VOID CaptureAllSignal(HI_VOID)
{
    int i;
    for (i = 1; i < 32; i ++)
    {
        if ((i == SIGPIPE) || (i == SIGCHLD))
        {
            //忽略
            signal(SIGPIPE, SIG_IGN);
        }
        else
        {
            signal(i, SignalHander);
        }
    }
    return;
}
HI_S32 main(HI_S32 argc, HI_CHAR **argv)
{
    HI_S32 s32Ret = HI_SUCCESS;

    AV_PRINT("VERSION");
    AV_PRINT("main enter!\n");

    CaptureAllSignal();

    HI3521_InitTw2968VideoType();
    /*系统初始化*/
    HI3521_Sys_Init();

    /*MSG QUE*/
    HI3521_MsgQueModeInit();

    /*MSG DOMAIN*/
    DOMAIN_InitCycleBuf();

    /*SHARE MEM*/
    HI3531_ShmSemMngInit();

    /*初始化状态锁*/
    s32Ret = HI3521_InitRWLock();
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI3531_InitRWLock s32Ret = %d", s32Ret);
        return s32Ret;
    }

    //HI3531_ShmInit();


    //HI3531_SemInit();

    /*获取相关能力集配置一些全局参数*/
    /*设置一些常用的全局参数*/
//    HI3521_DefCfg();
    HI3521_InitChnDataCenter();

    HI3521_InitSysPtsBase();

    /*启动硬盘管理*/
    HI3531_StartHddMng();

    /*OSD初始化*/
    HI3521_OsdInit();

    /*视频输入初始化*/
    HI3521_ViInit();
#ifdef JHY_SDK
    if(HI3521_GetTw2968VideoType() == 0)
    {
        HI3521_JHY_Init();

        HI3521_StartJHY_Work();
    }
    else
    {
        AV_PRINT("video size isn't D1,init keliu error");
    
}
#endif
    /*音频输入初始化*/
    HI3521A_AiInit();

    /*音频输出初始化*/
    HI3521A_AoInit(DEFAULT_DATA_PER_FRAM);

    /*音视频编码初始化*/
    HI3521_AVencInit();

    /*启动音视频编码*/
    HI3521_StartAVEnc();

    /*启动视频码流*/
    HI3521_StartVencStream();

    /*启动音频码流*/
    HI3521A_StartAencStream();

    /*启动默认的预览9画面预览*/
    HI3521_StartDefaultPreview();
#ifdef STORE_MULTI_THD
    //HI3521_StartAlarmEvtRecBk();

    HI3521_StartAvencRec();
#endif

    /*用于搜索提供时间片完整性，后续联调 */

    DOMAIN_StartRcvDataThd();

    DOMAIN_StartSndDataThd();

    s32Ret = APP_CreatThread();

    Start_28181main();
    /*退出检查*/
    for (; ;)
    {
        if (Get_Exit())
        {
            break;
        }

        pause();
    }

    HI3521_SignalExit();
    return s32Ret;
}
