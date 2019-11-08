#include "av_public_head.h"
#include "av_split.h"
#include "av_sys.h"
#include "av_vivo.h"
//#include "av_dec.h"
#include "av_capture.h"
#include "av_common_21.h"
#include "av_video_proc.h"


VI_DEV_ATTR_S DEV_ATTR_BT656D1_4MUX =
{
    /*接口模式*/
    VI_MODE_BT656,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

    /* 双沿输入时必须设置 */
    VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_YVYU,
    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

        /*timing信息，对应reg手册的如下配置*/
        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            0,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            0,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    /*使用内部ISP*/
    VI_PATH_BYPASS,
    /*输入数据类型*/
    VI_DATA_TYPE_YUV
};

VI_DEV_ATTR_S DEV_ATTR_6114_720P_2MUX_BASE =
{
    /*接口模式*/
    VI_MODE_BT656,                             /* 这个确认是正确 */
    /*1、2、4路工作模式*/
    VI_WORK_MODE_2Multiplex,                   /* 这个确认是正确 */
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

    /* 双沿输入时必须设置 */
    VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_YVYU,
    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

        /*timing信息，对应reg手册的如下配置*/
        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            0,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            0,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    /*使用内部ISP*/
    VI_PATH_BYPASS,
    /*输入数据类型*/
    VI_DATA_TYPE_YUV
};

/******************************************************************************
* function : venc bind vpss
******************************************************************************/
HI_S32 HI3521_Comm_VENC_BindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 HI3521_Comm_VPSS_Start(HI_S32 s32ChnStart,HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr;
    VPSS_CHN_ATTR_S stChnAttr;
    VPSS_GRP_PARAM_S stVpssParam;
    HI_S32 s32Ret;
    HI_S32 i, j;
    VPSS_CHN_MODE_S stVpssMode;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

        stGrpAttr.bIeEn = HI_FALSE;
        stGrpAttr.bNrEn = HI_TRUE;
        stGrpAttr.bDciEn = HI_FALSE;
        stGrpAttr.bHistEn = HI_FALSE;
        stGrpAttr.bEsEn = HI_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;//VPSS_DIE_MODE_NODIE;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }


    for(i=s32ChnStart; i<s32GrpCnt+s32ChnStart; i++)
    {
        VpssGrp = i;
        /*** create vpss group ***/
        s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** set vpss param ***/
        s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        stVpssParam.u32IeStrength = 0;
        s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** enable vpss chn, with frame ***/
        /* BEGIN: Modified by duzhanhui, 2016/5/24   PN:16051601075 */
        for(j=0; j<s32ChnCnt; j++)// 1
            /* END:   Modified by duzhanhui, 2016/5/24   PN:16051601075 */
        {
            VpssChn = j;
            //        SAMPLE_PRT("********VpssChn = %d,  VpssGrp=%d*******!\n", VpssChn,VpssGrp);
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bUVInvert = HI_FALSE;
            stChnAttr.bBorderEn = HI_FALSE;
            stChnAttr.stBorder.u32Color = 0;
            stChnAttr.stBorder.u32LeftWidth = 0;
            stChnAttr.stBorder.u32RightWidth = 0;
            stChnAttr.stBorder.u32TopWidth = 0;
            stChnAttr.stBorder.u32BottomWidth = 0;

            s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }

            /* BEGIN: Modified by zhangjinlei, 2016/4/23  DES:调试3521A视频VIVO功能   PN:16042001001 */
            //stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
            stVpssMode.enChnMode = VPSS_CHN_MODE_AUTO;
            /* END:   Modified by zhangjinlei, 2016/4/23   PN:16042001001 */
            /* BEGIN: Modified by zhangjinlei, 2016/4/23  DES:调试3521A视频VIVO功能   PN:16042001001 */
            //stVpssMode.u32Width  = 720/3;
            //stVpssMode.u32Height = 480/3;
            stVpssMode.u32Width  = pstSize->u32Width;
            stVpssMode.u32Height = pstSize->u32Height;
            stVpssMode.stFrameRate.s32DstFrmRate = -1;
            stVpssMode.stFrameRate.s32SrcFrmRate = -1;
            /* END:   Modified by zhangjinlei, 2016/4/23   PN:16042001001 */
            stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

            /* BEGIN: Modified by zhangjinlei, 2016/4/23  DES:调试3521A视频VIVO功能   PN:16042001001 */
            s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssMode);
            /* END:   Modified by zhangjinlei, 2016/4/23   PN:16042001001 */
            if (s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_VPSS_SetChnMode failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
        }

        /*** start vpss group ***/
        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        AV_PRINT("start vpss grp %d",VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_ViChnConfig
 功能描述  : 设置VI通道的属性且使能
 输入参数  : VI_CHN ViChn, HI_U16 mode, VI_CHN_SET_E enViChnSet,HI_U16 framerate
			 HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 成功， HI_FAILURE 失败
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2014年11月17日
	作	  者   : xzhang
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_ViChnConfig(VI_CHN ViChn, VI_DEV ViDev, HI_U16 mode, SAMPLE_VI_CHN_SET_E enViChnSet, HI_U16 framerate)
{
#if 0
    HI_S32 s32Ret;
    VI_CHN_ATTR_S stChnAttr;
    VI_CHN_BIND_ATTR_S chnBindAttr;
    SIZE_S stSize;
    SPLIT_DEV *pSplitDev = HI3521_GetSplitDev();

    chnBindAttr.ViWay = 1;
    stChnAttr.stCapRect.s32X = 0;
    stChnAttr.stCapRect.s32Y = 0;
    stChnAttr.enCapSel = VI_CAPSEL_BOTH;
    HI3521_Comm_SysGetPicSize((VIDEO_NORM_E)pSplitDev->vstd, (enum capture_size_t)mode, &stSize);
    stChnAttr.stCapRect.u32Width = stSize.u32Width;
    stChnAttr.stCapRect.u32Height = stSize.u32Height;
    stChnAttr.stDestSize.u32Width = stSize.u32Width;
    stChnAttr.stDestSize.u32Height = stSize.u32Height;
    stChnAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stChnAttr.bMirror = (VI_CHN_SET_MIRROR == enViChnSet)?HI_TRUE:HI_FALSE;
    stChnAttr.bFlip = (VI_CHN_SET_FILP == enViChnSet)?HI_TRUE:HI_FALSE;
    //stChnAttr.bChromaResample = HI_FALSE;
    stChnAttr.s32SrcFrameRate = framerate;
    stChnAttr.s32DstFrameRate = framerate;

    if ((ViChn / 2) % 2)
    {
        chnBindAttr.ViDev = ViDev;
        s32Ret = HI_MPI_VI_ChnBind(ViChn, &chnBindAttr);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI_MPI_VI_ChnBind err! (%x)", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with (%#x)!", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with (%#x)!", s32Ret);
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_ViSetMask
 功能描述  : VI设置输入掩码
 输入参数  : VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月18日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_Comm_ViSetMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{
    switch (ViDev % 4)
    {
    case 0:
        pstDevAttr->au32CompMask[0] = 0xFF;
        if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0x00FF0000;
        }
        else if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0x0;
        }
        break;
    case 1:
        pstDevAttr->au32CompMask[0] = 0xFF00;
        if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0x0;
        }
        break;
    case 2:
        pstDevAttr->au32CompMask[0] = 0xFF0000;
        if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0xFF;
        }
        else if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0x0;
        }
        break;
    case 3:
        pstDevAttr->au32CompMask[0] = 0xFF000000;
        if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
        {
            pstDevAttr->au32CompMask[1] = 0x0;
        }
        break;
    default:
        HI_ASSERT(0);
    }
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_VI_StartDev
 功能描述  : 设置VI设备的属性且使能
 输入参数  : VO_DEV VoDev,VO_CHN VoChn,VO_CHN_ATTR_S *pstChnAttr
			 HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 成功， HI_FAILURE 失败
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2014年11月17日
	作	  者   : duzhanhui
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 s32Ret;
    VI_DEV_ATTR_S stViDevAttr;
    memset(&stViDevAttr,0,sizeof(stViDevAttr));

    switch (enViMode)
    {
    case SAMPLE_VI_MODE_16_D1:
        memcpy(&stViDevAttr,&DEV_ATTR_BT656D1_4MUX,sizeof(stViDevAttr));
        HI3521_Comm_ViSetMask(ViDev,&stViDevAttr);
        break;
    case SAMPLE_VI_MODE_16_960H:
        memcpy(&stViDevAttr,&DEV_ATTR_BT656D1_4MUX,sizeof(stViDevAttr));
        HI3521_Comm_ViSetMask(ViDev,&stViDevAttr);
        break;
    case SAMPLE_VI_MODE_8_720P:
    case SAMPLE_VI_MODE_16_720P:
    case SAMPLE_VI_MODE_8_1080P:
        memcpy(&stViDevAttr,&DEV_ATTR_6114_720P_2MUX_BASE,sizeof(stViDevAttr));
        HI3521_Comm_ViSetMask(ViDev,&stViDevAttr);
        printf("***********set vi attr\n");
        break;
    default:
        AV_PRINT("vi input type[%d] is invalid!\n", enViMode);
        return HI_FAILURE;
    }

    if(SAMPLE_VI_MODE_8_1080P == enViMode)
    {
        stViDevAttr.enClkEdge = VI_CLK_EDGE_DOUBLE;
    }
    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableDev(ViDev);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}

HI_S32 HI3521_Comm_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32 *pu32W,HI_U32 *pu32H, HI_U32 *pu32Frm)
{
    switch (enIntfSync)
    {
    case VO_OUTPUT_PAL       :
        *pu32W = 720;
        *pu32H = 576;
        *pu32Frm = 25;
        break;
    case VO_OUTPUT_NTSC      :
        *pu32W = 720;
        *pu32H = 480;
        *pu32Frm = 30;
        break;
    case VO_OUTPUT_576P50    :
        *pu32W = 720;
        *pu32H = 576;
        *pu32Frm = 50;
        break;
    case VO_OUTPUT_480P60    :
        *pu32W = 720;
        *pu32H = 480;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_800x600_60:
        *pu32W = 800;
        *pu32H = 600;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_720P50    :
        *pu32W = 1280;
        *pu32H = 720;
        *pu32Frm = 50;
        break;
    case VO_OUTPUT_720P60    :
        *pu32W = 1280;
        *pu32H = 720;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1080I50   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 50;
        break;
    case VO_OUTPUT_1080I60   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1080P24   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 24;
        break;
    case VO_OUTPUT_1080P25   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 25;
        break;
    case VO_OUTPUT_1080P30   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 30;
        break;
    case VO_OUTPUT_1080P50   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 50;
        break;
    case VO_OUTPUT_1080P60   :
        *pu32W = 1920;
        *pu32H = 1080;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1024x768_60:
        *pu32W = 1024;
        *pu32H = 768;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1280x1024_60:
        *pu32W = 1280;
        *pu32H = 1024;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1366x768_60:
        *pu32W = 1366;
        *pu32H = 768;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1440x900_60:
        *pu32W = 1440;
        *pu32H = 900;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1280x800_60:
        *pu32W = 1280;
        *pu32H = 800;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1600x1200_60:
        *pu32W = 1600;
        *pu32H = 1200;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1680x1050_60:
        *pu32W = 1680;
        *pu32H = 1050;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_1920x1200_60:
        *pu32W = 1920;
        *pu32H = 1200;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_3840x2160_30:
        *pu32W = 3840;
        *pu32H = 2160;
        *pu32Frm = 30;
        break;
    case VO_OUTPUT_3840x2160_60:
        *pu32W = 3840;
        *pu32H = 2160;
        *pu32Frm = 60;
        break;
    case VO_OUTPUT_USER    :
        *pu32W = 800;
        *pu32H = 480;
        *pu32Frm = 25;
        break;
    default:
        AV_PRINT("vo enIntfSync not support!");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    HI_U32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;
    HI_U32 u32Square = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    HI_S32 s32ChnFrmRate;

    switch (enMode)
    {
    case VO_MODE_1MUX:
        u32WndNum = 1;
        u32Square = 1;
        break;
    case VO_MODE_4MUX:
        u32WndNum = 4;
        u32Square = 2;
        break;
    case VO_MODE_9MUX:
        u32WndNum = 9;
        u32Square = 3;
        break;
    case VO_MODE_16MUX:
        u32WndNum = 16;
        u32Square = 4;
        break;
    default:
        AV_PRINT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    u32Width = stLayerAttr.stImageSize.u32Width;
    u32Height = stLayerAttr.stImageSize.u32Height;
    printf("u32Width:%d, u32Square:%d,u32WndNum:%d\n", u32Width, u32Square,u32WndNum);

    if (stLayerAttr.u32DispFrmRt <= 0)
    {
        s32ChnFrmRate = 30;
    }
    else if (stLayerAttr.u32DispFrmRt > 30)
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt / 2;
    }
    else
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt;
    }

    for (i=0; i<u32WndNum; i++)
    {
        stChnAttr.stRect.s32X       = ALIGN_BACK((u32Width/u32Square) * (i%u32Square), 2);
        stChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height/u32Square) * (i/u32Square), 2);
        stChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width/u32Square, 2);
        stChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height/u32Square, 2);
        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = HI_FALSE;

        s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_VO_SetChnFrameRate(VoLayer, i, s32ChnFrmRate);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT(" i = %d failed with %#x!\n",i, s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    HI_U32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;

    switch (enMode)
    {
    case VO_MODE_1MUX:
    {
        u32WndNum = 1;
        break;
    }
    case VO_MODE_4MUX:
    {
        u32WndNum = 4;
        break;
    }
    case VO_MODE_9MUX:
    {
        u32WndNum = 9;
        break;
    }
    case VO_MODE_16MUX:
    {
        u32WndNum = 16;
        break;
    }

    default:
        AV_PRINT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    for (i=0; i<u32WndNum; i++)
    {
        s32Ret = HI_MPI_VO_DisableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return s32Ret;
}

HI_S32 HI3521_Comm_VDEC_BindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VDEC_BindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VDEC_UnBindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind");

    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VDEC_UnBindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind");

    return HI_SUCCESS;
}


HI_S32 HI3521_Comm_VI_BindVpss(HI_S32 s32ChnStart,SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 j, s32Ret,VIstart,Viend;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN ViChn;

    s32Ret = HI3521_Comm_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        AV_PRINT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }

    if(enViMode == SAMPLE_VI_MODE_16_D1 || enViMode == SAMPLE_VI_MODE_16_960H)
    {
        VIstart = 8;
        VpssGrp = 0;
        Viend = 8;
    }
    else
    {
        VIstart = 0;
        VpssGrp = 8;
        Viend = 4;
    }
    for (j=0; j<Viend; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval + VIstart;
        AV_PRINT("********ViChn = %d,  VpssGrp=%d*******!\n", ViChn,VpssGrp);

        stSrcChn.enModId = HI_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;

        stDestChn.enModId = HI_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

        s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        VpssGrp ++;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_VpssConfig
 功能描述  :
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_VpssConfig(HI_S32 s32ChnStart,HI_S32 s32VpssGrpCnt,VIDEO_NORM_E enNorm,SAMPLE_VI_MODE_E enViMode)
{
    int iRet = 0;
    VPSS_GRP_ATTR_S stGrpAttr;
    SIZE_S stSize;
    PIC_SIZE_E enPicSize;
    //HI_S32 s32VpssGrpCnt = 4;
    //VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;//VIDEO_ENCODING_MODE_PAL;//
    //SAMPLE_VI_MODE_E enViMode = SAMPLE_VI_MODE_8_720P;//SAMPLE_VI_MODE_8_1080P;

    if(SAMPLE_VI_MODE_8_720P == enViMode)
    {
        enPicSize = PIC_HD720;
    }
    else if(SAMPLE_VI_MODE_16_960H == enViMode)
    {
        enPicSize = PIC_960H;
    }
    else if(SAMPLE_VI_MODE_16_D1== enViMode)
    {
        enPicSize = PIC_D1;
    }
    else if(SAMPLE_VI_MODE_4_1080P== enViMode)
    {
        enPicSize = PIC_HD1080;
    }



    iRet = HI3521_Comm_SYS_GetPicSize(enNorm, enPicSize, &stSize);
    if (HI_SUCCESS != iRet)
    {
        AV_PRINT("SAMPLE_COMM_SYS_GetPicSize failed!");
    }

    memset(&stGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
    stGrpAttr.u32MaxW = stSize.u32Width;
    stGrpAttr.u32MaxH = stSize.u32Height;
    stGrpAttr.bNrEn = HI_TRUE;
    stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    iRet = HI3521_Comm_VPSS_Start(s32ChnStart,s32VpssGrpCnt, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != iRet)
    {
        AV_PRINT("Start Vpss failed!");
        return HI_FAILURE;
    }
    if(s32ChnStart < 12)
    {
        iRet = HI3521_Comm_VI_BindVpss(s32ChnStart,enViMode);
        if (HI_SUCCESS != iRet)
        {
            AV_PRINT("Vi bind Vpss failed!");
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_VoPubConfig
 功能描述  : 设置VO设备的属性，且使能VO，图像层
 输入参数  : VO_DEV VoDev
			 HI_VOID
 输出参数  : 无
 返 回 值  : HI_SUCCESS 成功， HI_FAILURE 失败
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2016年6月6日
	作	  者   : duzhanhui
	修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_VoPubConfig(VO_DEV s32VoDev, HI_S32 s32priority)
{
    HI_S32 s32Ret = HI_SUCCESS;
    //VO_CSC_S stVoCsc;
    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    //HI_S32 s32Result;
    VO_LAYER VoLayer;
    //VO_CHN VoChn;

    AV_PRINT("HI3521_Comm_VoPubConfig dev(%d) enter.", s32VoDev);

    s32Ret = HI_MPI_VO_Disable(s32VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_Disable failed with (%#x!)", s32Ret);
        return HI_FAILURE;
    }

    if (SAMPLE_VO_DEV_DHD0 == s32VoDev)
    {
#ifdef RUIMINGPING
        stPubAttr.enIntfType = VO_INTF_VGA;
        stPubAttr.u32BgColor = VO_BKGRD_BLACK;
        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        VoLayer = SAMPLE_VO_LAYER_VHD0;

        stPubAttr.stSyncInfo.bIdv = HI_FALSE;
        stPubAttr.stSyncInfo.bIhs = HI_FALSE;
        stPubAttr.stSyncInfo.bIop = HI_FALSE;

        stPubAttr.stSyncInfo.bIvs = HI_FALSE;
        stPubAttr.stSyncInfo.bSynm = HI_FALSE;

        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb = 0;
        stPubAttr.stSyncInfo.u16Bvfb = 0;

        stPubAttr.stSyncInfo.u16Hact = 800;
        stPubAttr.stSyncInfo.u16Hbb = 2;

        stPubAttr.stSyncInfo.u16Hfb = 4;
        stPubAttr.stSyncInfo.u16Hmid = 0;


        stPubAttr.stSyncInfo.u16Vact = 240;
        stPubAttr.stSyncInfo.u16Vbb = 0;
        stPubAttr.stSyncInfo.u16Vfb = 0;

        stPubAttr.stSyncInfo.u16Vpw = 0;
        stPubAttr.stSyncInfo.u16Hpw = 0;

        stPubAttr.stSyncInfo.u8Intfb = 0;
#else
        stPubAttr.enIntfType = VO_INTF_VGA;
        stPubAttr.u32BgColor = VO_BKGRD_BLACK;
        stPubAttr.enIntfSync = VO_OUTPUT_800x600_60;
        VoLayer = SAMPLE_VO_LAYER_VHD0;
#endif
    }
    else if(SAMPLE_VO_DEV_DSD0 == s32VoDev)
    {
        stPubAttr.enIntfType = VO_INTF_CVBS;
        stPubAttr.u32BgColor = VO_BKGRD_BLACK;
#ifdef RUIMINGPING
        stPubAttr.enIntfSync = VO_OUTPUT_NTSC;
#else
        stPubAttr.enIntfSync = VO_OUTPUT_PAL;
#endif
        VoLayer = SAMPLE_VO_LAYER_VSD0;
    }
    else
    {
        stPubAttr.enIntfType = VO_INTF_BT656;
        stPubAttr.enIntfSync = VO_OUTPUT_PAL;
        s32Ret = HI_MPI_VO_SetPubAttr(s32VoDev, &stPubAttr);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VO_SetPubAttr other, failed with (%#x)", s32Ret);
            return HI_FAILURE;
        }
        s32Ret = HI_MPI_VO_Enable(s32VoDev);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VO_Enable other, failed with (%#x)", s32Ret);
            return HI_FAILURE;
        }

        return s32Ret;
    }
    s32Ret = HI_MPI_VO_SetPubAttr(s32VoDev, &stPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_SetPubAttr, failed with (%#x)!", s32Ret);
        return HI_FAILURE;
    }
#ifdef RUIMINGPING
    if(SAMPLE_VO_DEV_DHD0 == s32VoDev)
    {
        s32Ret = HI_MPI_VO_SetDevFrameRate(SAMPLE_VO_DEV_DHD0,30);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VO_Enable, failed with (%#x)!", s32Ret);
            return HI_FAILURE;
        }
    }
#endif
    s32Ret = HI_MPI_VO_Enable(s32VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VO_Enable, failed with (%#x)!", s32Ret);
        return HI_FAILURE;
    }

    memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
    s32Ret = HI3521_Comm_VO_GetWH(stPubAttr.enIntfSync, \
                                  &stLayerAttr.stImageSize.u32Width, \
                                  &stLayerAttr.stImageSize.u32Height, \
                                  &stLayerAttr.u32DispFrmRt);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("Start SAMPLE_COMM_VO_GetWH failed!");
    }

    stLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;
    stLayerAttr.stDispRect.s32X 	  = 0;
    stLayerAttr.stDispRect.s32Y 	  = 0;
    stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
    stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;
	stLayerAttr.u32DispFrmRt          = 25;

    s32Ret = HI_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI_MPI_VO_SetVideoLayerAttr failed with (%#x)", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT( "HI_MPI_VO_EnableVideoLayer failed with (%#x)", s32Ret);
        return HI_FAILURE;
    }

    if(SAMPLE_VO_DEV_DSD0 == s32VoDev)
    {
        VO_CSC_S stVideoOutPara;
        VIDEO_CFG_T stVideoCfg;
        s32Ret = HI_MPI_VO_GetVideoLayerCSC(2, &stVideoOutPara);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT( "HI_MPI_VO_GetDevCSC failed with (%#x)", s32Ret);
            return HI_FAILURE;
        }
        s32Ret = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " Read app cfg failed!");
            stVideoOutPara.u32Luma = 50;
            stVideoOutPara.u32Contrast = 50;
            stVideoOutPara.u32Hue = 50;
            stVideoOutPara.u32Saturation = 50;
        }
        else
        {
            stVideoOutPara.u32Luma = stVideoCfg.stOutputCfg.s32CscLuma;
            stVideoOutPara.u32Contrast = stVideoCfg.stOutputCfg.s32CscContr;
            stVideoOutPara.u32Hue = stVideoCfg.stOutputCfg.s32CscHue;
            stVideoOutPara.u32Saturation = stVideoCfg.stOutputCfg.s32CscSatu;
        }
        AV_PRINT("set vo para , u32Luma=%d\n", stVideoOutPara.u32Luma);
        AV_PRINT("set vo para , u32Contrast=%d\n",stVideoOutPara.u32Contrast);
        AV_PRINT("set vo para , u32Hue=%d\n",stVideoOutPara.u32Hue);
        AV_PRINT("set vo para , u32Satuature=%d\n",stVideoOutPara.u32Saturation);
        stVideoOutPara.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
        s32Ret = HI_MPI_VO_SetVideoLayerCSC(2, &stVideoOutPara);
        if (s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_VO_SetDevCSC HI_MPI_VO_SetDevCSC failed with (%#x)!", s32Ret);
            return HI_FAILURE;
        }
    }
    return s32Ret;
}

HI_S32 HI3521_Comm_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S *pstViParam)
{
    switch (enViMode)
    {
    case SAMPLE_VI_MODE_16_D1:
    case SAMPLE_VI_MODE_16_960H:
    case SAMPLE_VI_MODE_16_1280H:
    case SAMPLE_VI_MODE_16_HALF720P:
        pstViParam->s32ViDevCnt = 2;
        pstViParam->s32ViDevInterval = 1;
        pstViParam->s32ViChnCnt = 8;
        pstViParam->s32ViChnInterval = 1;
        break;
    case SAMPLE_VI_MODE_8_720P:
    case SAMPLE_VI_MODE_8_1080P:
        /* use chn 0,2,4,6,8,10,12,14 */
        /* BEGIN: Modified by zhangjinlei, 2016/4/23  DES:调试3521A视频VIVO功能   PN:16042001001 */
        //pstViParam->s32ViDevCnt = 4;
        //pstViParam->s32ViDevInterval = 1;
        //pstViParam->s32ViChnCnt = 8;
        //pstViParam->s32ViChnInterval = 2;

        pstViParam->s32ViDevCnt = 2;
        pstViParam->s32ViDevInterval = 1;
        pstViParam->s32ViChnCnt = 4;
        pstViParam->s32ViChnInterval = 2;
        /* END:   Modified by zhangjinlei, 2016/4/23   PN:16042001001 */
        break;
    case SAMPLE_VI_MODE_4_1080P:
    case SAMPLE_VI_MODE_4_3M:
        /* use chn 0,4,8,12 */
        pstViParam->s32ViDevCnt = 4;
        pstViParam->s32ViDevInterval = 1;
        pstViParam->s32ViChnCnt = 4;
        pstViParam->s32ViChnInterval = 4;
        break;
    default:
        SAMPLE_PRT("ViMode invaild!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, RECT_S *pstCapRect, SIZE_S *pstDestSize)
{
    pstCapRect->s32X = 0;
    pstCapRect->s32Y = 0;
    switch (enViMode)
    {
    case SAMPLE_VI_MODE_16_D1:
        pstDestSize->u32Width = D1_WIDTH;
        pstDestSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        pstCapRect->u32Width = D1_WIDTH;
        pstCapRect->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        break;
    case SAMPLE_VI_MODE_16_960H:
        pstDestSize->u32Width = _960H_WIDTH;
        pstDestSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        pstCapRect->u32Width = _960H_WIDTH;
        pstCapRect->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
        break;
    case SAMPLE_VI_MODE_8_720P:
    case SAMPLE_VI_MODE_16_720P:
        pstDestSize->u32Width  = _720P_WIDTH;
        pstDestSize->u32Height = _720P_HEIGHT;
        pstCapRect->u32Width  = _720P_WIDTH;
        pstCapRect->u32Height = _720P_HEIGHT;
        break;

    case SAMPLE_VI_MODE_8_1080P:
        pstDestSize->u32Width  = HD_WIDTH;
        pstDestSize->u32Height = HD_HEIGHT;
        pstCapRect->u32Width  = HD_WIDTH;
        pstCapRect->u32Height = HD_HEIGHT;
        break;

    default:
        SAMPLE_PRT("vi mode invaild!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HI3521_Comm_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize,
                               SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_CHN_SET_E enViChnSet)
{
    HI_S32 s32Ret;
    VI_CHN_ATTR_S stChnAttr;

    /* step  5: config & start vicap dev */
    memcpy(&stChnAttr.stCapRect, pstCapRect, sizeof(RECT_S));
    /* to show scale. this is a sample only, we want to show dist_size = D1 only */
    stChnAttr.stDestSize.u32Width = pstTarSize->u32Width;
    stChnAttr.stDestSize.u32Height = pstTarSize->u32Height;
    stChnAttr.enCapSel = VI_CAPSEL_BOTH;
    stChnAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;   /* sp420 or sp422 */
    stChnAttr.bMirror = (VI_CHN_SET_MIRROR == enViChnSet)?HI_TRUE:HI_FALSE;
    stChnAttr.bFlip = (VI_CHN_SET_FILP == enViChnSet)?HI_TRUE:HI_FALSE;
    stChnAttr.s32SrcFrameRate = 25;
    stChnAttr.s32DstFrameRate = 25;
    switch (enViMode)
    {
    case SAMPLE_VI_MODE_16_D1:
    case SAMPLE_VI_MODE_16_960H:
    case SAMPLE_VI_MODE_16_1280H:
        stChnAttr.enScanMode = VI_SCAN_INTERLACED;
        break;
    case SAMPLE_VI_MODE_16_HALF720P:
    case SAMPLE_VI_MODE_8_720P:
    case SAMPLE_VI_MODE_16_720P:
    case SAMPLE_VI_MODE_4_1080P:
    case SAMPLE_VI_MODE_8_1080P:
    case SAMPLE_VI_MODE_16_1080P:
    case SAMPLE_VI_MODE_4_3M:
        stChnAttr.enScanMode = VI_SCAN_PROGRESSIVE;
        break;
    default:
        AV_PRINT("ViMode invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VI_SetChnAttr CHN[%d] failed with %#x!\n",ViChn,s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI_MPI_VI_EnableChn CHN[%d] failed with %#x!\n",ViChn,s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_Comm_ViInit
 功能描述  : HI3521 VI初始化
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_ViInit(SAMPLE_VI_MODE_E enViMode,VIDEO_NORM_E enNorm)
{
    VI_DEV ViDev = 0;
    VI_CHN ViChn = 0;
    HI_S32 i = 0;
    HI_S32 s32Ret = 0;
    SAMPLE_VI_PARAM_S stViParam;
    SIZE_S stTargetSize;
    RECT_S stCapRect;
    VI_CHN_BIND_ATTR_S stChnBindAttr;
    VI_CHN iChnStart = 0;
    VI_DEV iDevStart = 0;
    if(SAMPLE_VI_MODE_8_720P == enViMode)
    {
        iChnStart = 0;
        iDevStart = 0;
    }
    else if((SAMPLE_VI_MODE_16_960H== enViMode)||(SAMPLE_VI_MODE_16_D1== enViMode))
    {
        iChnStart = 8;
        iDevStart = 2;
    }

    /*** get parameter from Sample_Vi_Mode ***/
    s32Ret = HI3521_Comm_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        AV_PRINT("vi get param failed!\n");
        return HI_FAILURE;
    }
    s32Ret = HI3521_Comm_VI_Mode2Size(enViMode, enNorm, &stCapRect, &stTargetSize);
    if (HI_SUCCESS !=s32Ret)
    {
        AV_PRINT("vi get size failed!\n");
        return HI_FAILURE;
    }

    /*** Start VI Dev ***/
    for(i=iDevStart; i<stViParam.s32ViDevCnt+iDevStart; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = HI3521_Comm_VI_StartDev(ViDev, enViMode);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("SAMPLE_COMM_VI_StartDev(%d,%d,%d) failed with %#x\n"
                    ,iDevStart,i,enViMode,s32Ret);
            return HI_FAILURE;
        }
    }

    /*** Start VI Chn ***/
    SAMPLE_PRT("iChnStart = %d!\n",iChnStart);
    for(i=iChnStart; i<stViParam.s32ViChnCnt + iChnStart; i++)
    {
        ViChn = i * stViParam.s32ViChnInterval;
        /* BEGIN: Modified by duzhanhui, 2016/5/25   PN:16051601075 */
        if (SAMPLE_VI_MODE_8_1080P == enViMode
                || SAMPLE_VI_MODE_8_720P == enViMode)
        {
            /* When in the 8x1080p mode, bind chn 2,6,10,14 to way1 is needed */
            if (ViChn%4 != 0)
            {
                s32Ret = HI_MPI_VI_GetChnBind(ViChn, &stChnBindAttr);
                printf("-------chn: %d, s32Ret = %#x, %#x\n", ViChn,s32Ret,HI_ERR_VI_FAILED_NOTBIND);
                if (HI_ERR_VI_FAILED_NOTBIND == s32Ret)
                {
                    printf("HI_ERR_VI_FAILED_NOTBIND\n");
                    stChnBindAttr.ViDev = ViChn/4;
                    stChnBindAttr.ViWay = 1;
                    s32Ret = HI_MPI_VI_BindChn(ViChn, &stChnBindAttr);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("call HI_MPI_VI_BindChn failed with %#x\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
            }
        }
        /* END:   Modified by duzhanhui, 2016/5/25   PN:16051601075 */

//       printf("ViChn = %d\n",ViChn);
        s32Ret = HI3521_Comm_VI_StartChn(ViChn, &stCapRect, &stTargetSize, enViMode, VI_CHN_SET_NORMAL);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("call SAMPLE_COMM_VI_StarChn failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }
    AV_PRINT("HI3521_Comm_ViInit suc.");
    return HI_SUCCESS;
}

/*****************************************************************************
* function : stop vi accroding to product type
*****************************************************************************/
HI_S32 HI3521_Comm_ViStop(SAMPLE_VI_MODE_E enViMode)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_S32 i;
    HI_S32 s32Ret;
    SAMPLE_VI_PARAM_S stViParam;

    /*** get parameter from Sample_Vi_Mode ***/
    s32Ret = HI3521_Comm_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }

    /*** Stop VI Chn ***/
    for(i=0; i<stViParam.s32ViChnCnt; i++)
    {
        /* Stop vi phy-chn */
        ViChn = i * stViParam.s32ViChnInterval;
        s32Ret = HI_MPI_VI_DisableChn(ViChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopChn failed with %#x\n",s32Ret);
            return HI_FAILURE;
        }
    }

    /*** Stop VI Dev ***/
    for(i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = HI_MPI_VI_DisableDev(ViDev);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopDev failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}


HI_S32 HI3521_Comm_VO_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    AV_PRINT("VoLayer:%d VoChnId:%d Bind VpssGrp:%d VpssChn:%d"
            ,VoLayer,VoChn,VpssGrp,VpssChn);
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 HI3521_Comm_VO_UnBindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}


/******************************************************************************
* function : venc unbind vpss
******************************************************************************/
HI_S32 HI3521_COMM_VENC_UnBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}


/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
HI_S32 HI3521_Comm_VENC_Stop(VENC_CHN VencChn)
{
    HI_S32 s32Ret;

    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",\
                   VencChn, s32Ret);
        return HI_FAILURE;
    }

    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n",\
                   VencChn, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 HI3521_Comm_WBC_BindVo(VO_WBC VoWbc,VO_WBC_SOURCE_S *pstWbcSource)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VO_SetWbcSource(VoWbc, pstWbcSource);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 HI3521_Comm_VO_StartWbc(VO_WBC VoWbc,const VO_WBC_ATTR_S *pstWbcAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VO_SetWbcAttr(VoWbc, pstWbcAttr);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableWbc(VoWbc);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 HI3521_Comm_VO_BindVoWbc(VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = HI_ID_VOU;
    stSrcChn.s32DevId   = VoWbcDev;
    stSrcChn.s32ChnId   = 0;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

HI_S32 HI3521_Comm_VO_UnBindVoWbc(VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stDestChn;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;

    return HI_MPI_SYS_UnBind(NULL, &stDestChn);
}

HI_S32 HI3521_Comm_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = HI_ID_VIU;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = ViChn;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

HI_S32 HI3521_Comm_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = HI_ID_VIU;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = ViChn;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

HI_S32 HI3521_Comm_VO_StopWbc(VO_WBC VoWbc)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableWbc(VoWbc);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/*****************************************************************************
 函 数 名  : HI3521_WBC_Start
 功能描述  :
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_WBC_Start(HI_VOID)
{
    VO_WBC VoWbc;
    VO_WBC_ATTR_S stWbcAttr;
    VO_WBC_SOURCE_S stWbcSource;
    int iRet = 0;

    VoWbc = SAMPLE_VO_WBC_BASE;
    /******Wbc bind source*******/
    stWbcSource.enSourceType = VO_WBC_SOURCE_DEV;
    stWbcSource.u32SourceId = SAMPLE_VO_DEV_DHD0;

    iRet = HI3521_Comm_WBC_BindVo(VoWbc,&stWbcSource);
    if (iRet != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", iRet);
    }
    /* BEGIN: Added by duzhanhui, 2016/5/7   PN:16030801070 */
    AV_PRINT("SAMPLE_COMM_VO_GetWH!");
    /* END:   Added by duzhanhui, 2016/5/7   PN:16030801070 */
    /*******start Wbc*********/
#ifdef RUIMINGPING
    iRet = HI3521_Comm_VO_GetWH(VO_OUTPUT_NTSC, \
                                &stWbcAttr.stTargetSize.u32Width, \
                                &stWbcAttr.stTargetSize.u32Height, \
                                &stWbcAttr.u32FrameRate);
#else
    iRet = HI3521_Comm_VO_GetWH(VO_OUTPUT_PAL, \
                                &stWbcAttr.stTargetSize.u32Width, \
                                &stWbcAttr.stTargetSize.u32Height, \
                                &stWbcAttr.u32FrameRate);
#endif
    if (iRet != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", iRet);
    }
    stWbcAttr.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    iRet = HI3521_Comm_VO_StartWbc(VoWbc,&stWbcAttr);
    if (iRet != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", iRet);
    }
    /* BEGIN: Added by duzhanhui, 2016/5/7   PN:16030801070 */
    AV_PRINT("SAMPLE_COMM_VO_BindVoWbc!\n");
    /* END:   Added by duzhanhui, 2016/5/7   PN:16030801070 */
    /*****bind wbc to target***/
    iRet = HI3521_Comm_VO_BindVoWbc(VoWbc,SAMPLE_VO_LAYER_VSD0,0);
    if (iRet != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", iRet);
    }

    return HI_SUCCESS;

}
/*****************************************************************************
 函 数 名  : HI3521_COMM_WBC_End
 功能描述  :
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_WBC_End(HI_VOID)
{
    int iRet = 0;
    VO_WBC VoWbc;
    VoWbc = SAMPLE_VO_WBC_BASE;

    iRet = HI3521_Comm_VO_UnBindVoWbc(SAMPLE_VO_LAYER_VSD0,0);
    if (iRet != HI_SUCCESS)
    {
        AV_PRINT("failed with %#x!", iRet);
    }

    HI3521_Comm_VO_StopWbc(VoWbc);

    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : HI3521_COMM_Vo_SwitchMode
 功能描述  :
 输入参数  : HI_VOID

 输出参数  : HI_VOID
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Comm_Vo_SwitchMode(VO_LAYER VoLayer,SAMPLE_VO_MODE_E enPreVoMode,SAMPLE_VO_MODE_E enVoMode)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret= HI_MPI_VO_SetAttrBegin(VoLayer);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VO failed!\n");
    }

    s32Ret = HI3521_Comm_VO_StopChn(VoLayer, enPreVoMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VO failed!\n");
    }

    s32Ret = HI3521_Comm_VO_StartChn(VoLayer, enVoMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VO failed!\n");
    }
    s32Ret= HI_MPI_VO_SetAttrEnd(VoLayer);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start VO failed!\n");
    }

    return HI_SUCCESS;
}
