/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
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
    /*ֹͣ����Ƶ����*/
    HI3521_StopAVEnc();
    AV_PRINT("HI3521_StopAVEnc success");
    /*ֹͣ��Ƶ����ͨ�� */
    HI3521_VencExit();
    AV_PRINT("HI3521_VencExit success");
    /*ֹͣ��Ƶ����ͨ�� */
    HI3521A_AencExit();
    AV_PRINT("HI3521A_AencExit success");
    /*ֹͣAI*/
    HI3521A_AiExit();
    AV_PRINT("HI3521A_AiExit success");
    /*ֹͣAO*/
    HI3521A_AoExit();
    AV_PRINT("HI3521A_AoExit success");
    /*ֹͣVI*/
    HI3521_ViExit();
    AV_PRINT("HI3521_ViExit success");
    /*ֹͣVO*/
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
            //����
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
    /*ϵͳ��ʼ��*/
    HI3521_Sys_Init();

    /*MSG QUE*/
    HI3521_MsgQueModeInit();

    /*MSG DOMAIN*/
    DOMAIN_InitCycleBuf();

    /*SHARE MEM*/
    HI3531_ShmSemMngInit();

    /*��ʼ��״̬��*/
    s32Ret = HI3521_InitRWLock();
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI3531_InitRWLock s32Ret = %d", s32Ret);
        return s32Ret;
    }

    //HI3531_ShmInit();


    //HI3531_SemInit();

    /*��ȡ�������������һЩȫ�ֲ���*/
    /*����һЩ���õ�ȫ�ֲ���*/
//    HI3521_DefCfg();
    HI3521_InitChnDataCenter();

    HI3521_InitSysPtsBase();

    /*����Ӳ�̹���*/
    HI3531_StartHddMng();

    /*OSD��ʼ��*/
    HI3521_OsdInit();

    /*��Ƶ�����ʼ��*/
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
    /*��Ƶ�����ʼ��*/
    HI3521A_AiInit();

    /*��Ƶ�����ʼ��*/
    HI3521A_AoInit(DEFAULT_DATA_PER_FRAM);

    /*����Ƶ�����ʼ��*/
    HI3521_AVencInit();

    /*��������Ƶ����*/
    HI3521_StartAVEnc();

    /*������Ƶ����*/
    HI3521_StartVencStream();

    /*������Ƶ����*/
    HI3521A_StartAencStream();

    /*����Ĭ�ϵ�Ԥ��9����Ԥ��*/
    HI3521_StartDefaultPreview();
#ifdef STORE_MULTI_THD
    //HI3521_StartAlarmEvtRecBk();

    HI3521_StartAvencRec();
#endif

    /*���������ṩʱ��Ƭ�����ԣ��������� */

    DOMAIN_StartRcvDataThd();

    DOMAIN_StartSndDataThd();

    s32Ret = APP_CreatThread();

    Start_28181main();
    /*�˳����*/
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
