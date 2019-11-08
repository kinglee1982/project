#include "av_public_head.h"
#include "av_point_font.h"
#include "av_osd.h"
#include "av_sys.h"
#include "av_video_init.h"
#include "av_capture.h"
#include "tinyxml2API.h"
static HI_S32 g_TimeLen = TIME_STRING_LEN;
OSD_TIMEPARAM g_osdTime[MAX_VI_CHN];
OSD_TITLEPARAM g_osdTitle[MAX_OSD_TITLE][MAX_VI_CHN*2 + 2];
//运行信息OSD缓存
OSD_TITLEPARAM g_osdRunInfoTitle_Cashe[MAX_OSD_TITLE][4];//高清，标清，主次码流
//通道、线路，车号缓存
OSD_TITLEPARAM g_osdBasicInfoTitle_Cashe[MAX_VI_CHN*2];//高清，标清，主次码流
OSD_TITLE_DATA_ST g_stOsdShowData;
HI_CHAR BUSID[BUS_ID_LEN] = {0};

HI_S32 g_s32AsciiMapCnt = (sizeof(fontmatrix_ASCIIMap)/sizeof(fontmatrix_ASCIIMap[0]));

/*****************************************************************************
 函 数 名  : HI3521_OsdInit
 功能描述  :
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_OsdInit(HI_VOID)
{
    HI_S32 s32OsdCnt = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Chn = 0;

    memset(g_osdTime, 0, sizeof(g_osdTime));

    for (s32Chn = 0; s32Chn < MAX_VI_CHN; s32Chn++)
    {
        g_osdTime[s32Chn].changed = 0;
        g_osdTime[s32Chn].imagesize[0] = 0;
        g_osdTime[s32Chn].imagesize[1] = 0;
        g_osdTime[s32Chn].jpegsize = 0;
        g_osdTime[s32Chn].osdHandle[0] = -1;
        g_osdTime[s32Chn].osdHandle[1] = -1;
        g_osdTime[s32Chn].snaposdHandle = -1;
        g_osdTime[s32Chn].StBufFont.pAddr = NULL;
        g_osdTime[s32Chn].StBufFont.nSize = 0;
        g_osdTime[s32Chn].StBufRGB.pAddr = NULL;
        g_osdTime[s32Chn].StBufRGB.nSize = 0;
        g_osdTime[s32Chn].osdTime.sysTime.year = 2014;
        g_osdTime[s32Chn].osdTime.sysTime.month = 11;
        g_osdTime[s32Chn].osdTime.sysTime.day = 22;
        g_osdTime[s32Chn].osdTime.sysTime.wday = 6;
        g_osdTime[s32Chn].osdTime.sysTime.hour = 13;
        g_osdTime[s32Chn].osdTime.sysTime.minute = 30;
        g_osdTime[s32Chn].osdTime.sysTime.second = 0;
        g_osdTime[s32Chn].osdTime.sysTime.isdst = 1;
    }

    for (s32OsdCnt = 0; s32OsdCnt < MAX_OSD_TITLE; s32OsdCnt++)
    {
        for (s32Chn = 0; s32Chn < (MAX_VI_CHN*2 + 2); s32Chn++)
        {
            //g_osdTitle[s32OsdCnt][s32Chn].TitleLen = 0;
            //g_osdTitle[s32OsdCnt][s32Chn].imagesize[0] = 0;
            //g_osdTitle[s32OsdCnt][s32Chn].imagesize[1] = 0;
            g_osdTitle[s32OsdCnt][s32Chn].osdHandle[0] = -1;
            g_osdTitle[s32OsdCnt][s32Chn].osdHandle[1] = -1;
            g_osdTitle[s32OsdCnt][s32Chn].snaposdHandle = -1;
            g_osdTitle[s32OsdCnt][s32Chn].StBufRGB.pAddr = NULL;
            g_osdTitle[s32OsdCnt][s32Chn].StBufRGB.nSize = 0;
        }
    }

    for (s32OsdCnt = 0; s32OsdCnt < MAX_OSD_TITLE; s32OsdCnt++)
    {
        for (s32Chn = 0; s32Chn < 4; s32Chn++)
        {
            //g_osdTitle_Cashe[s32OsdCnt][s32Chn].TitleLen = 0;
            //g_osdTitle_Cashe[s32OsdCnt][s32Chn].imagesize[0] = 0;
            //g_osdTitle_Cashe[s32OsdCnt][s32Chn].imagesize[1] = 0;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].osdHandle[0] = -1;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].osdHandle[1] = -1;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].snaposdHandle = -1;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].StBufRGB.pAddr = NULL;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].StBufRGB.nSize = 0;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].Param.width= 0;
            g_osdRunInfoTitle_Cashe[s32OsdCnt][s32Chn].Param.height= 0;
        }
    }
    for (s32Chn = 0; s32Chn < MAX_VI_CHN*2; s32Chn++)
    {
        //g_osdTitle_Cashe[s32OsdCnt][s32Chn].TitleLen = 0;
        //g_osdTitle_Cashe[s32OsdCnt][s32Chn].imagesize[0] = 0;
        //g_osdTitle_Cashe[s32OsdCnt][s32Chn].imagesize[1] = 0;
        g_osdBasicInfoTitle_Cashe[s32Chn].osdHandle[0] = -1;
        g_osdBasicInfoTitle_Cashe[s32Chn].osdHandle[1] = -1;
        g_osdBasicInfoTitle_Cashe[s32Chn].snaposdHandle = -1;
        g_osdBasicInfoTitle_Cashe[s32Chn].StBufRGB.pAddr = NULL;
        g_osdBasicInfoTitle_Cashe[s32Chn].StBufRGB.nSize = 0;
        g_osdBasicInfoTitle_Cashe[s32Chn].Param.width = 0;
        g_osdBasicInfoTitle_Cashe[s32Chn].Param.height= 0;
    }
    for (s32Chn = 0; s32Chn < TOTAL_VI_CHN_NUM; s32Chn++)
    {
        s32Ret = pthread_mutex_init(&(g_osdTime[s32Chn].lock), NULL);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "pthread_mutex_init viChn = %d failed %#X\n", s32Chn, s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*******************************************************************
 * 函数名称：HI3521_PointFont2RGB
 * 函数功能：将点阵数据转换成RGB数据
 * 输入参数：pParam		-- 点阵数据存放位置,指向CAPTURE_TITLE_PARAM的结构体指针
 * 输出参数：pRGBBuf	-- RGB数据存放区域
 * 返回值：  0	-- 成功
 *          -1	-- 失败
 ******************************************************************/
HI_S32 HI3521_PointFont2RGB(CAPTURE_TITLE_PARAM * pParam, HI_CHAR *pRGBBuf)
{
    HI_S32 i, j, k, rowAddr, colAddr;
    HI_S8  fgRGB[2];
    HI_S8  bgRGB[2];
    HI_U8  byteSrc;
    const HI_CHAR  rimRGB[2]= {0x08,0x91};

    if ((NULL == pParam) || (NULL == pParam->raster) || (NULL == pRGBBuf))
    {
        return -1;
    }
    PARSE_RGB1555(fgRGB, pParam->fg_color);
    PARSE_RGB1555(bgRGB, pParam->bg_color);
    /*防止前景色被透过去*/
    fgRGB[1] |= 0x80;
    for(i=0; i<pParam->height; i++)
    {
        HI_CHAR *tmp = NULL;
        HI_U16 i_width = i * pParam->width;
        for(j=0; j < pParam->width; j++)
        {
            //memcpy((pRGBBuf+(i*pParam->width+j)*2), bgRGB, 2);
            tmp=(pRGBBuf + ((i_width + j) << 1));
            *tmp = bgRGB[0];
            *(tmp + 1) = bgRGB[1];
        }
    }
    for(i = 0; i<pParam->height; i++)
    {
        HI_CHAR *tmp = NULL;
        rowAddr = i;
        for(j = 0; j < pParam->width>>3; j++)
        {
            byteSrc = *(pParam->raster + i * (pParam->width>>3) + j);
            for(k = 0; k < 8; k++)
            {
                //colAddr = j*8+k;
                colAddr = (j << 3) +k;
                if(byteSrc & (0x80 >> k))
                {
                    if(rowAddr > 0 )
                    {
                        tmp = pRGBBuf+(((rowAddr-1) * pParam->width + colAddr) * 2);
                        //memcpy(pRGBBuf+((rowAddr-1) * pParam->width + colAddr)*2, rimRGB, 2);
                    }
                    if(rowAddr < (pParam->height - 1))
                    {
                        tmp = pRGBBuf+(((rowAddr+1) * pParam->width + colAddr) * 2);
                        //memcpy(pRGBBuf+((rowAddr+1) * pParam->width + colAddr)*2, rimRGB, 2);
                    }
                    if(colAddr > 0)
                    {
                        tmp = pRGBBuf+((rowAddr * pParam->width + colAddr - 1) * 2);
                        //memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr - 1)*2, rimRGB, 3);
                    }
                    if(colAddr < (pParam->width - 1))
                    {
                        tmp = pRGBBuf+((rowAddr * pParam->width + colAddr + 1) * 2);
                        //memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr + 1)*2, rimRGB, 2);
                    }
                    if(rowAddr > 0 && colAddr > 0)
                    {
                        tmp = pRGBBuf+(((rowAddr-1) * pParam->width + colAddr - 1) * 2);
                        //memcpy(pRGBBuf+((rowAddr-1) * pParam->width + colAddr - 1)*2, rimRGB, 2);
                    }
                    if(rowAddr > 0 && colAddr < (pParam->width - 1))
                    {
                        tmp = pRGBBuf+(((rowAddr-1) * pParam->width + colAddr + 1) * 2);
                        //memcpy(pRGBBuf+((rowAddr-1) * pParam->width + colAddr + 1)*2, rimRGB, 2);
                    }
                    if(rowAddr < (pParam->height - 1) && colAddr > 0)
                    {
                        tmp = pRGBBuf+(((rowAddr+1) * pParam->width + colAddr - 1) * 2);
                        //memcpy(pRGBBuf+((rowAddr+1) * pParam->width + colAddr - 1)*2, rimRGB, 2);
                    }
                    if(rowAddr < (pParam->height - 1) && colAddr < (pParam->width - 1))
                    {
                        tmp = pRGBBuf+(((rowAddr+1) * pParam->width + colAddr + 1) * 2);
                        //memcpy(pRGBBuf+((rowAddr+1) * pParam->width + colAddr + 1)*2, rimRGB, 2);
                    }
                    *tmp = rimRGB[0];
                    *(tmp + 1) = rimRGB[1];
                }
            }
        }
    }
    for (i = 0; i < pParam->height; i++)
    {
        HI_CHAR *tmp = NULL;
        rowAddr = i;
        HI_U16 width = pParam->width>>3;
        HI_U16 i_width = width * i;
        for (j = 0; j < width; j++)
        {
            byteSrc = *(pParam->raster + i_width + j);
            int m = j << 3;
            for (k = 0; k < 8; k++)
            {
                colAddr = (m + k);
                if (byteSrc & (0x80 >> k))
                {
                    //memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr)*2, fgRGB, 2);
                    tmp = pRGBBuf+((rowAddr * pParam->width + colAddr) << 1);
                    *tmp = fgRGB[0];
                    *(tmp+1) = fgRGB[1];
                }
            }
        }
    }

    return HI_SUCCESS;
}

static HI_S32 OsdAttachToChn(HI_S32 channel,RGN_HANDLE osdHandle,CAPTURE_TITLE_PARAM *pParam)
{
    HI_S32			s32Ret;
    RGN_CHN_ATTR_S  stChnAttr;
    MPP_CHN_S 		stChn;

    stChn.enModId = HI_ID_VENC;//HI_ID_GROUP;
    stChn.s32DevId = 0;
    stChn.s32ChnId = channel;
    memset(&stChnAttr,0,sizeof(stChnAttr));
    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = pParam->x / 4 * 4;//16*channel;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = pParam->y / 4 * 4;//16*channel;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;

    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
    if(((pParam->bg_color >> 24)&0xFF) < 64)
    {
        stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
        stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    }
    else
    {
        stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = ((pParam->bg_color >> 24)&0xFF)/2;
        stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = ((pParam->fg_color >> 24)&0xFF)/2;
    }

    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn    = HI_FALSE;



    //AV_PRINT("u32BgAlpha= %d,u32FgAlpha=%d\n", stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha, stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha);
    s32Ret = HI_MPI_RGN_AttachToChn(osdHandle, &stChn, &stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        AV_PRINT("vencChn:%d HI_MPI_RGN_AttachToChn err:%#x\n", channel, s32Ret);
        return -1;
    }

    return 0;
}

static HI_S32 OsdCreateRgn(HI_S32 channel,RGN_HANDLE	osdHandle, CAPTURE_TITLE_PARAM *pParam)
{
    HI_S32			s32Ret;
    RGN_ATTR_S		stRgnAttr;

    stRgnAttr.enType = OVERLAY_RGN;
    stRgnAttr.unAttr.stOverlay.stSize.u32Height = pParam->height / 4 * 4;
    stRgnAttr.unAttr.stOverlay.stSize.u32Width	= pParam->width / 4 * 4;
    stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
    stRgnAttr.unAttr.stOverlay.u32BgColor = 0x0000;

#ifndef OSD_DEBUG
    AV_PRINT("OSD create rgn Set chn:%d <x:%d,y:%d,w:%d,h:%d>\n",
             channel,pParam->x,pParam->y,pParam->width,pParam->height);
#endif

    s32Ret = HI_MPI_RGN_Create(osdHandle,&stRgnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("vencChn:%d HI_MPI_RGN_Create error:%#x\n", channel, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

/*******************************************************************
 * 函数名称：HI3521_char_cpy
 * 函数功能：OSD数据拷贝
 * 输入参数：src	    -- 源内存区域地址
 			 pFont		-- OSD点阵参数,指向POINT_FONT_S的指针
 * 输出参数：dest		-- 目标内存区域地址
 * 返回值：  0	-- 成功
 *          -1	-- 失败
 ******************************************************************/
// OSD数据拷贝
static inline HI_VOID HI3521_char_cpy( HI_CHAR* dest, HI_CHAR* src, const POINT_FONT_S *pFont)
{
    HI_S32 i, j, width;

    width = (pFont->s32Width / 8);
    for (i = 0; i < pFont->s32Height; i++)
    {
        for (j = 0; j < width; j++)
        {
            *(dest+j) = *(src++);
        }
        dest +=  g_TimeLen*width;
    }
}

/*****************************************************************************
 函 数 名  : HI3521_GetCharIdxInMap
 功能描述  : 点阵字库匹配
 输入参数  : HI_CHAR onechar
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_GetCharIdxInMap(HI_CHAR onechar)
{
    int i;
    HI_S32 s32Index = 0;

    /* 在ASCII表中查找对应的字符，获取数组下标 */
    for (i = 0; i < g_s32AsciiMapCnt; i++)
    {
        if (onechar == fontmatrix_ASCIIMap[i])
        {
            s32Index = i;
            break;
        }
    }

    if (i >= g_s32AsciiMapCnt)
    {
        s32Index = 62;
    }

    return s32Index;

}

OSD_TITLE_DATA_ST *HI3521_GetTitleOsdData()
{
    return &g_stOsdShowData;
}

void DeCompressLineBit_2(unsigned char *des, unsigned char *ascl)
{
    des[0] = (ascl[0] & 0x3F);
    des[1] = (ascl[1] >> 2);
    des[2] = (((ascl[1] << 4) | (ascl[2] >> 4)) & 0x3F);
    des[3] = (((ascl[2] << 2) | (ascl[3] >> 6)) & 0x3F);
    des[4] = (ascl[3] & 0x3F);

}

void LineHex2Ascii_2(unsigned char *des, unsigned char *line)
{

    HI_U8 i, *pt;

    memset(des, 0x20, LINE_BIT_LEN);

    for (i=0; i<LINE_BIT_LEN; i++)
    {
        pt = &line[i];

        *pt &= 0x3F;
        if ((*pt >= 0x01) && (*pt <= 0x0A))
        {
            des[i] = (*pt + 0x2F);
        }
        else if ((*pt >= 0x10) && (*pt <= 0x29))
        {
            des[i] = (*pt + 0x31);
        }
    }
}
void HI3521_GetBusId(char *pBusId,int iLen)
{
    strncpy(pBusId,BUSID,iLen);
}
HI_VOID HI3521_ResetBusIdTitleCache(OSD_TITLE_DATA_ST *pstOsdShowData)
{
    HI_S32 s32Chn = 0;
    if(memcmp(pstOsdShowData->stOsdSchInfo.busId,g_stOsdShowData.stOsdSchInfo.busId,sizeof(g_stOsdShowData.stOsdSchInfo.busId)) !=0
            || memcmp(pstOsdShowData->stOsdSchInfo.lineId,g_stOsdShowData.stOsdSchInfo.lineId,sizeof(g_stOsdShowData.stOsdSchInfo.lineId)) != 0)
    {
        for (s32Chn = 0; s32Chn < MAX_VI_CHN*2; s32Chn++)
        {
            g_osdBasicInfoTitle_Cashe[s32Chn].StBufRGB.pAddr = NULL;
            g_osdBasicInfoTitle_Cashe[s32Chn].StBufRGB.nSize = 0;
            g_osdBasicInfoTitle_Cashe[s32Chn].Param.width = 0;
            g_osdBasicInfoTitle_Cashe[s32Chn].Param.height= 0;
        }
        //AV_PRINT("Reset BusID Title");
    }
}
HI_VOID HI3521_ResetPositionTitleCache(OSD_TITLE_DATA_ST *pstOsdShowData)
{
    HI_S32 s32Chn = 0;
    if(pstOsdShowData->stOsdSchInfo.latitude != g_stOsdShowData.stOsdSchInfo.latitude ||
            pstOsdShowData->stOsdSchInfo.longitude != g_stOsdShowData.stOsdSchInfo.longitude)
    {
        for (s32Chn = 0; s32Chn < 4; s32Chn++)
        {
            g_osdRunInfoTitle_Cashe[2][s32Chn].StBufRGB.pAddr = NULL;
            g_osdRunInfoTitle_Cashe[2][s32Chn].StBufRGB.nSize = 0;
            g_osdRunInfoTitle_Cashe[2][s32Chn].Param.height = 0;
            g_osdRunInfoTitle_Cashe[2][s32Chn].Param.width = 0;
        }
        //AV_PRINT("Reset Position Title");
    }
}
HI_VOID HI3521_ResetSpeedTitleCache(OSD_TITLE_DATA_ST *pstOsdShowData)
{
    HI_S32 s32Chn = 0;
    if(pstOsdShowData->stOsdSchInfo.speed!= g_stOsdShowData.stOsdSchInfo.speed||
            pstOsdShowData->stOsdSchInfo.ucLimitSpeed!= g_stOsdShowData.stOsdSchInfo.ucLimitSpeed)
    {
        for (s32Chn = 0; s32Chn < 4; s32Chn++)
        {
            g_osdRunInfoTitle_Cashe[3][s32Chn].StBufRGB.pAddr = NULL;
            g_osdRunInfoTitle_Cashe[3][s32Chn].StBufRGB.nSize = 0;
            g_osdRunInfoTitle_Cashe[3][s32Chn].Param.height = 0;
            g_osdRunInfoTitle_Cashe[3][s32Chn].Param.width = 0;
        }
        //AV_PRINT("Reset Speed Title");
    }
}
HI_VOID HI3521_ResetScheduleTitleCache(OSD_TITLE_DATA_ST *pstOsdShowData)
{
    HI_S32 s32Chn = 0;
    if(pstOsdShowData->stOsdSchInfo.ucOperate!= g_stOsdShowData.stOsdSchInfo.ucOperate||
            pstOsdShowData->stOsdSchInfo.ucStationNum!= g_stOsdShowData.stOsdSchInfo.ucStationNum ||
            pstOsdShowData->stOsdSchInfo.ucRegStatus!= g_stOsdShowData.stOsdSchInfo.ucRegStatus ||
            pstOsdShowData->stOsdSchInfo.bussignal!= g_stOsdShowData.stOsdSchInfo.bussignal ||
            pstOsdShowData->stOsdSchInfo.ucGpsStatus!= g_stOsdShowData.stOsdSchInfo.ucGpsStatus ||
			pstOsdShowData->stOsdSchInfo.ucStnDir!= g_stOsdShowData.stOsdSchInfo.ucStnDir||
            pstOsdShowData->stOsdSchInfo.ucIsInPark!= g_stOsdShowData.stOsdSchInfo.ucIsInPark||
            pstOsdShowData->stOsdSchInfo.ucParkDir!= g_stOsdShowData.stOsdSchInfo.ucParkDir)
    {
        for (s32Chn = 0; s32Chn < 4; s32Chn++)
        {
            g_osdRunInfoTitle_Cashe[4][s32Chn].StBufRGB.pAddr = NULL;
            g_osdRunInfoTitle_Cashe[4][s32Chn].StBufRGB.nSize = 0;
            g_osdRunInfoTitle_Cashe[4][s32Chn].Param.height = 0;
            g_osdRunInfoTitle_Cashe[4][s32Chn].Param.width = 0;
        }
        //AV_PRINT("Reset Schedule title");
    }
}
HI_VOID HI3521_ResetUITitleCache(OSD_TITLE_DATA_ST *pstOsdShowData)
{
    HI_S32 s32Chn = 0;
    if(pstOsdShowData->stOsdUiInfo.PowerStatus!= g_stOsdShowData.stOsdUiInfo.PowerStatus||
            pstOsdShowData->stOsdUiInfo.LockStatus!= g_stOsdShowData.stOsdUiInfo.LockStatus ||
            pstOsdShowData->stOsdUiInfo.CapStatus!= g_stOsdShowData.stOsdUiInfo.CapStatus ||
            pstOsdShowData->stOsdUiInfo.TemVal!= g_stOsdShowData.stOsdUiInfo.TemVal ||
            pstOsdShowData->stOsdUiInfo.telesig!= g_stOsdShowData.stOsdUiInfo.telesig)
    {
        for (s32Chn = 0; s32Chn < 4; s32Chn++)
        {
            g_osdRunInfoTitle_Cashe[5][s32Chn].StBufRGB.pAddr = NULL;
            g_osdRunInfoTitle_Cashe[5][s32Chn].StBufRGB.nSize = 0;
            g_osdRunInfoTitle_Cashe[5][s32Chn].Param.height = 0;
            g_osdRunInfoTitle_Cashe[5][s32Chn].Param.width = 0;
        }
        //AV_PRINT("Reset UI title");
    }
}
/*****************************************************************************
 函 数 名  : HI3521_BusidPointFont
 功能描述  : BUSID转换
 输入参数  : HI_S32 s32Chn
             HI_CHAR *pBuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_BusidPointFont(HI_S32 s32Chn, HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32  s32Index = 0;
    HI_CHAR id[8] = "ID:";
    HI_CHAR busid[8] = {0};
    HI_S32 s32busid;
    HI_CHAR chn[8] = "CHN:";
    HI_S32 channo = s32Chn+1;

    HI_CHAR line[8] = "LINE:";
    HI_CHAR lineno[8] = {0};
    HI_CHAR dstlineno[8] = {0};
    HI_CHAR ctmplineno[8] = {0};
    HI_CHAR strOSD[64];
    HI_S32 		 s32FW, s32FH;
    OSD_TITLE_DATA_ST *pstTitleData;
    OSD_SCH_DATA_ST *pstSchOsd = NULL;


    s32FW    = pFont->s32Width/8;
    s32FH    = pFont->s32Height * s32FW;

    pstTitleData = HI3521_GetTitleOsdData();
    pstSchOsd = &pstTitleData->stOsdSchInfo;
    DeCompressLineBit_2((HI_U8*)lineno, (HI_U8*)pstSchOsd->lineId);
    LineHex2Ascii_2((HI_U8*)ctmplineno, (HI_U8*)lineno);
    s32busid = pstSchOsd->busId[2]<< 16| pstSchOsd->busId[1]<< 8|pstSchOsd->busId[0];
    sprintf(busid, "%d", s32busid);
    memset(BUSID, 0, 16);
    sprintf(BUSID, "%d", s32busid);
#ifndef OSD_DEBUG
    AV_PRINT("line no is %s", ctmplineno);
    AV_PRINT("s32busid is %d", s32busid);
    AV_PRINT("busid is %s", busid);
#endif
    for (i = 0; i < 8; i++)
    {
        if (ctmplineno[i] != 0x20)
        {
            dstlineno[j] = ctmplineno[i];
            j++;
        }
    }

    snprintf(strOSD, sizeof(strOSD), "%s%s %s%d %s%s",id,busid,chn,channo,line,dstlineno);
#ifndef OSD_DEBUG
    AV_PRINT("strOSD BUSID is %s",strOSD);
#endif

    if (strlen(strOSD) > MAX_OSD_STRLEN)
    {
        AV_PRINT("strOSD overload");
    }

    j = 0;
    for(i = 0; i <(HI_S32)strlen(strOSD); i++)
    {
        s32Index = HI3521_GetCharIdxInMap(strOSD[i]);
        s32FW    = pFont->s32Width/8;
        s32FH    = pFont->s32Height * s32FW;
        // AV_PRINT("s32Index is %d.",s32Index);
        HI3521_char_cpy(pBuf+s32FW*j, pFont->pFont + (s32FH*s32Index), pFont);
        j++;
    }

}

/*****************************************************************************
 函 数 名  : HI3521_BusidPointFont
 功能描述  : 位置信息转换
 输入参数  : HI_S32 s32Chn
             HI_CHAR *pBuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_PosPointFont(HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32  s32Index = 0;

    HI_CHAR strOSD[64];
    HI_S32 		 s32FW, s32FH;
    OSD_TITLE_DATA_ST *pstTitleData;
    OSD_SCH_DATA_ST *pstSchOsd = NULL;

    s32FW    = pFont->s32Width/8;
    s32FH    = pFont->s32Height * s32FW;

    pstTitleData = HI3521_GetTitleOsdData();
    pstSchOsd = &pstTitleData->stOsdSchInfo;

    //memcpy(lontitude, "113.551276", 10);
    //memcpy(latitude, "34.823883", 9);
    snprintf(strOSD, sizeof(strOSD),"LON:%9.6f LAT:%9.6f ", (pstSchOsd->longitude*1.0)/60000, (pstSchOsd->latitude*1.0)/60000);

    //snprintf(strOSD, sizeof(strOSD), "%s%s %s%s",lon,lontitude,lat,latitude);
#ifndef OSD_DEBUG
    AV_PRINT("strOSD BUSID is %s",strOSD);
#endif


    if (strlen(strOSD) > MAX_OSD_STRLEN)
    {
        AV_PRINT("strOSD overload");
    }

    for(i = 0; i <(HI_S32)strlen(strOSD); i++)
    {
        s32Index = HI3521_GetCharIdxInMap(strOSD[i]);
        s32FW    = pFont->s32Width/8;
        s32FH    = pFont->s32Height * s32FW;
        // AV_PRINT("s32Index is %d.",s32Index);

        HI3521_char_cpy(pBuf+s32FW*j, pFont->pFont + (s32FH*s32Index), pFont);
        j++;
    }

}

/*****************************************************************************
 函 数 名  : HI3521_BusidPointFont
 功能描述  : 速度转换
 输入参数  : HI_S32 s32Chn
             HI_CHAR *pBuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_SpeedPointFont(HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32  s32Index = 0;
    HI_S32  s32len = 0;
    HI_CHAR speedno[8] = {0};
    HI_CHAR unit[8] = "KM/H";
    HI_CHAR strOSD[64];
    HI_CHAR ch = 0;
    HI_S32 		 s32FW, s32FH;
    OSD_TITLE_DATA_ST *pstTitleData;
    OSD_SCH_DATA_ST *pstSchOsd = NULL;

    pstTitleData = HI3521_GetTitleOsdData();
    pstSchOsd = &pstTitleData->stOsdSchInfo;

    s32FW    = pFont->s32Width/8;
    s32FH    = pFont->s32Height * s32FW;

    snprintf(speedno, sizeof(speedno), "%03d", pstSchOsd->speed);

    s32len = strlen(speedno);
    ch = speedno[s32len - 1];
    speedno[s32len] = ch;
    speedno[s32len - 1] = '.';
    speedno[s32len + 1] = '\0';

    snprintf(strOSD, sizeof(strOSD), "CS %s%s LS %d%s", speedno, unit, pstSchOsd->ucLimitSpeed, unit);

#ifndef OSD_DEBUG
    AV_PRINT("strOSD BUSID is %s",strOSD);
#endif

    if (strlen(strOSD) > MAX_OSD_STRLEN)
    {
        AV_PRINT("strOSD overload");
    }

    for(i = 0; i <(HI_S32)strlen(strOSD); i++)
    {
        s32Index = HI3521_GetCharIdxInMap(strOSD[i]);
        s32FW    = pFont->s32Width/8;
        s32FH    = pFont->s32Height * s32FW;
        HI3521_char_cpy(pBuf+s32FW*j, pFont->pFont + (s32FH*s32Index), pFont);
        j++;
    }

}

/*****************************************************************************
 函 数 名  : HI3521_SchedulePointFont
 功能描述  : 将调度信息及信号值叠加到OSD
 输入参数  : HI_S32 s32Chn
             HI_CHAR *pBuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_SchedulePointFont(HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32	s32Index = 0;
    const HI_CHAR *strinfo[4] = {"US", "DS", "UP", "DP"};
    HI_CHAR strOSD[64];
    HI_S32 num = 0;
    HI_CHAR maskcode;
    HI_S32 s32FW, s32FH;
    OSD_TITLE_DATA_ST *pstTitleData;
    OSD_SCH_DATA_ST *pstSchOsd = NULL;

    pstTitleData = HI3521_GetTitleOsdData();
    pstSchOsd = &pstTitleData->stOsdSchInfo;

    s32FW	 = pFont->s32Width/8;
    s32FH	 = pFont->s32Height * s32FW;
#if 0
    if ((pstSchOsd->ucStationNum & 0xC0) == 0xC0)
    {
        num = (pstSchOsd->ucStationNum & 0x20) == 0x20 ? 2 : 3;
        maskcode = 0x0F;
    }
    else
    {
        num = (pstSchOsd->ucStationNum & 0x80) == 0x80 ? 0 : 1;
        maskcode = 0x3F;
    }

    snprintf(strOSD, sizeof(strOSD), "W%d %s%d R%d S%x G%d",
             pstSchOsd->ucOperate,
             strinfo[num],
             (pstSchOsd->ucStationNum & maskcode),
             pstSchOsd->ucRegStatus,
             pstSchOsd->bussignal,
             pstSchOsd->ucGpsStatus);
#else
    if(pstSchOsd->ucIsInPark)
    {
        num= pstSchOsd->ucParkDir ? 3:2;    /* 在场区显示场区上下行方向*/
    }
    else
    {
        num= pstSchOsd->ucStnDir ? 1:0;
        /*不在场区显示站点上下行方向*/
    }
    snprintf(strOSD, sizeof(strOSD), "W%d %s%d R%d S%x G%d",
             pstSchOsd->ucOperate,
             strinfo[num],
             (num<2 ? pstSchOsd->ucStationNum : pstSchOsd->ucParkCode),
             pstSchOsd->ucRegStatus,
             pstSchOsd->bussignal,
             pstSchOsd->ucGpsStatus);
#endif
#ifndef OSD_DEBUG
    AV_PRINT("strOSD BUSID is %s",strOSD);
#endif

    if (strlen(strOSD) > MAX_OSD_STRLEN)
    {
        AV_PRINT("strOSD overload");
    }

    for(i = 0; i <(HI_S32)strlen(strOSD); i++)
    {
        s32Index = HI3521_GetCharIdxInMap(strOSD[i]);
        s32FW	 = pFont->s32Width/8;
        s32FH	 = pFont->s32Height * s32FW;
        HI3521_char_cpy(pBuf+s32FW*j, pFont->pFont + (s32FH*s32Index), pFont);
        j++;
    }

}
/*****************************************************************************
 函 数 名  : HI3521_UiPointFont
 功能描述  : 将UI发送的信息转点阵，如主电源状态，电容状态，温度，硬盘锁及sim信号
 输入参数  : HI_S32 s32Chn
             HI_CHAR *pBuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_UiPointFont(HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32	s32Index = 0;
    HI_CHAR strOSD[64];
    HI_S32 diskstatus = 0;
    const HI_CHAR *diskstr[10]= {"NR", "RD", "TM", "MG", "TU", "UG", "MS", "MF", "WK", "FG"};
    const HI_CHAR *reasonstr[5] = {"PWR", "LCK", "CAP", "TEM", "ACC"};
    HI_S32 s32FW, s32FH;
    OSD_TITLE_DATA_ST *pstTitleData;
    OSD_UI_DATA_ST *pstUiOsd = NULL;

    pstTitleData = HI3521_GetTitleOsdData();
    pstUiOsd = &pstTitleData->stOsdUiInfo;

    //diskstatus = HDD_GetDiskFSM();

    /*dis DISK_STAT_NOT_READY  add by zoulq */
    if (diskstatus == -1)
    {
        diskstatus = 0;  /*DISK_STAT_NOT_READY 的时候也写成 0*/
    }
    else
    {
        diskstatus += 1; /*其它偏移 1 */
    }

    s32FW	 = pFont->s32Width/8;
    s32FH	 = pFont->s32Height * s32FW;
    if(pstUiOsd->TemVal == -127)
        snprintf(strOSD, sizeof(strOSD), "P%d L%d C%d TN S%d D%s",
             pstUiOsd->PowerStatus,
             pstUiOsd->LockStatus,
             pstUiOsd->CapStatus,
             pstUiOsd->telesig,
             diskstr[diskstatus]);
    else
        snprintf(strOSD, sizeof(strOSD), "P%d L%d C%d T%d S%d D%s",
             pstUiOsd->PowerStatus,
             pstUiOsd->LockStatus,
             pstUiOsd->CapStatus,
             pstUiOsd->TemVal,
             pstUiOsd->telesig,
             diskstr[diskstatus]);
    if ((pstUiOsd->reason <= 5) && (pstUiOsd->reason > 0))
    {
        snprintf(strOSD + strlen(strOSD), sizeof(strOSD) - strlen(strOSD), " %s", reasonstr[pstUiOsd->reason - 1]);
    }


#ifndef OSD_DEBUG
    AV_PRINT("strOSD BUSID is %s",strOSD);
#endif

    if (strlen(strOSD) > MAX_OSD_STRLEN)
    {
        AV_PRINT("strOSD overload");
    }

    for(i = 0; i <(HI_S32)strlen(strOSD); i++)
    {
        s32Index = HI3521_GetCharIdxInMap(strOSD[i]);
        s32FW	 = pFont->s32Width/8;
        s32FH	 = pFont->s32Height * s32FW;
        HI3521_char_cpy(pBuf+s32FW*j, pFont->pFont + (s32FH*s32Index), pFont);
        j++;
    }

}


/*****************************************************************************
 函 数 名  : HI3521_SetOsdShowData
 功能描述  : 设置OSD刷新数据
 输入参数  : HI_S32 s32Chn
             HI_S32 s32Title
             HI_CHAR *pbuf
             const POINT_FONT_S *pFont
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_SetOsdShowData(HI_S32 s32Chn, HI_S32 s32Title, HI_CHAR *pbuf, const POINT_FONT_S *pFont)
{
    if (s32Title == 1)
    {
        HI3521_BusidPointFont(s32Chn, pbuf, pFont);
    }
    else if(s32Title == 2)
    {
        HI3521_PosPointFont(pbuf, pFont);
    }
    else if(s32Title == 3)
    {
        HI3521_SpeedPointFont(pbuf, pFont);
    }
    else if (s32Title == 4)
    {
        HI3521_SchedulePointFont(pbuf, pFont);
    }
    else if (s32Title == 5)
    {
        HI3521_UiPointFont(pbuf, pFont);
    }

    return;

}


/*****************************************************************************
 函 数 名  : HI3521_GetOsdFont
 功能描述  :
 输入参数  : HI_S32 s32VideoSize, POINT_FONT_S *pstPointFont
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_GetOsdFont(HI_U32 enMode, POINT_FONT_S *pstPointFont, HI_S32 s32Type)
{
    if (enMode == PIC_VGA)
    {
        pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w16_h24[0][0];
        pstPointFont->s32Width  = 16;
        pstPointFont->s32Height = 24;
        g_TimeLen = TIME_STRING_LEN;
    }
    else if((enMode == PIC_D1 )||(PIC_960H == enMode ))
    {
        /*mod by xzhang 20150825 begin*/
        if (OSD_TYPE_TIME == s32Type)
        {
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w8_h16[0][0];
        }
        else if (OSD_TYPE_TEXT == s32Type)
        {
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w8_h16_v2[0][0];
        }
        pstPointFont->s32Width  = 8;
        pstPointFont->s32Height = 16;
        g_TimeLen = TIME_STRING_LEN;
        /*mod by xzhang 20150825 end*/
    }
    else if(enMode == PIC_HD1)
    {
        pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w16_h24[0][0];
        pstPointFont->s32Width  = 16;
        pstPointFont->s32Height = 24;
        g_TimeLen = TIME_STRING_LEN;
    }
    else if(enMode == PIC_QCIF)
    {
        pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w8_h16_v2[0][0];
        pstPointFont->s32Width  = 8;
        pstPointFont->s32Height = 16;
        g_TimeLen = TIME_STRING_LEN;
    }
    else if(enMode == PIC_HD1080)
    {
        pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w16_h24[0][0];
        pstPointFont->s32Width  = 16;
        pstPointFont->s32Height = 24;
        g_TimeLen = TIME_STRING_LEN;
    }
    else if(enMode == PIC_HD720)
    {
        /*mod by xzhang 20150825 begin*/
        if (OSD_TYPE_TIME == s32Type)
        {
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w16_h24[0][0];
        }
        else if (OSD_TYPE_TEXT == s32Type)
        {
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_busid_w16_h24[0][0];
        }
        pstPointFont->s32Width  = 16;
        pstPointFont->s32Height = 24;
        g_TimeLen = TIME_STRING_LEN;
        /*mod by xzhang 20150825 end*/

    }
    else
    {
        if (OSD_TYPE_TIME == s32Type)
        {
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w8_h16[0][0];
        }
        else
            pstPointFont->pFont	   	= (HI_CHAR *)&fontmatrix_w8_h16_v2[0][0];
        pstPointFont->s32Width  = 8;
        pstPointFont->s32Height = 16;
        g_TimeLen = TIME_STRING_LEN;
    }

    return;
}
HI_VOID HI3521_GetOsdFont_Size(HI_U32 enMode,HI_S32 &s32Width,HI_S32 &s32Height)
{
    if (enMode == PIC_VGA)
    {
        s32Width  = 16;
        s32Height = 24;
    }
    else if((enMode == PIC_D1 )||(PIC_960H == enMode ))
    {
        s32Width  = 8;
        s32Height = 16;
    }
    else if(enMode == PIC_HD1)
    {
        s32Width  = 16;
        s32Height = 24;
    }
    else if(enMode == PIC_QCIF)
    {
        s32Width  = 8;
        s32Height = 16;
    }
    else if(enMode == PIC_HD1080)
    {
        s32Width  = 16;
        s32Height = 24;
    }
    else if(enMode == PIC_HD720)
    {
        s32Width  = 16;
        s32Height = 24;
    }
    else
    {
        s32Width  = 8;
        s32Height = 16;
    }
}
/*******************************************************************
 * 函数名称：CaptureSetTitle
 * 函数功能：设置标题,包括时间和通道名称标题的创建,刷新时间标题不在此处
 * 输入参数：channel	-- 输入通道号
 			 pParam		-- 标题参数存放区域,指向CAPTURE_TITLE_PARAM的指针
 * 输出参数：无
 * 返回值：  0	-- 成功
 *          -1	-- 失败
 ******************************************************************/

/*mod by xzhang for double stream support begin 20150825*/
HI_S32 HI3521_CaptureSetTitle(HI_S32 s32Title, HI_S32 s32Chn, CAPTURE_TITLE_PARAM * pTitleParam)
{
    HI_S32		s32Ret;
    BUFFER_S	*pStRGBBuf = NULL;
    RGN_HANDLE	osdHandle[2];
    //CAPTURE_TITLE_PARAM *posdParam;
    //CAPTURE_TITLE_PARAM Param;
    BITMAP_S stBitmap;
    HI_S32 index;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    SIZE_S stSize;
    HI_S32 video_size;
    HI_S32 j;

    if (((HI_U32)(pTitleParam->index) >= MAX_OSD_TITLE) || ((HI_U32)s32Chn >= VI_CHN_NUM_HOST_CHIP))
    {
        AV_PRINT("Invalid channel %d, index %d\n", s32Chn, pTitleParam->index);
        return -1;
    }

    for (j = 0; j < 2; j++)
    {
        if(pCaptureDev->vi_chn[s32Chn].venc_chn[j].started != HI_ENABLE)
        {
            continue;
        }
        HI3521_InitOsdTitle(s32Title, s32Chn, j);
        video_size = pCaptureDev->vi_chn[s32Chn].venc_chn[j].image_size;
        //memcpy(&Param, pTitleParam, sizeof(CAPTURE_TITLE_PARAM));
        //pTitleParam = &(g_osdTitle[s32Title][s32Chn].Param);
        //memcpy(&Param, pTitleParam, sizeof(CAPTURE_TITLE_PARAM));
        //AV_PRINT("title(%d), s32Chn(%d), j(%d), osd-size=%d", s32Title, s32Chn, j, video_size);

        if(pTitleParam->index != 0)
        {
            //修改通道名称标题
            index = pTitleParam->index;
            osdHandle[j] = g_osdTitle[index][s32Chn].osdHandle[j];
            if(pTitleParam->enable == 0)
            {
                AV_PRINT("pTitleParam->enable:%d!\n",pTitleParam->enable);
                if(j == 0)
                {
                    continue;
                }
                else
                {
                    return 0;
                }
            }

            //posdParam = &Param;
#ifndef OSD_DEBUG
            AV_PRINT("1OSD Title chn:%d <x:%d,y:%d,w:%d,h:%d>\n",s32Chn,pTitleParam->x,pTitleParam->y,pTitleParam->width,pTitleParam->height);
#endif
            HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd, (PIC_SIZE_E)video_size,&stSize);
            /*分配空间 */
            pStRGBBuf = &(g_osdTitle[index][s32Chn].StBufRGB);
            if (pStRGBBuf->nSize < ((pTitleParam->height * pTitleParam->width) << 1))
            {
                if(pStRGBBuf->pAddr != NULL)
                {
                    free(pStRGBBuf->pAddr);
                    pStRGBBuf->pAddr = NULL;
                }
                pStRGBBuf->nSize = ((pTitleParam->height * pTitleParam->width) << 1);
                pStRGBBuf->pAddr = (HI_CHAR *)malloc(pStRGBBuf->nSize);
            }

            /* Check the buffer address */
            if (NULL == pStRGBBuf->pAddr)
            {
                pStRGBBuf->nSize = 0;
                AV_PRINT("Buffer malloc error size:%d!\n",pStRGBBuf->nSize);
                return -1;
            }
            /* Prepate the RGB data */
            s32Ret = HI3521_PointFont2RGB(pTitleParam, (HI_CHAR *)pStRGBBuf->pAddr);

            if(HI_SUCCESS != s32Ret)
            {
                return s32Ret;
            }

            if(osdHandle[j] == (RGN_HANDLE)-1)
            {
                osdHandle[j] = (s32Chn << 1) + j + ((TOTAL_VI_CHN_NUM << 1) * index);
                //osdHandle[j] = s32Chn + ((TOTAL_VI_CHN_NUM << 1) * index);
#ifndef OSD_DEBUG
                AV_PRINT("OsdCreateRgn j(%d) osdHandle(%d)", j, osdHandle[j]);
#endif
                s32Ret = OsdCreateRgn(s32Chn,osdHandle[j],pTitleParam);
                if(HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("vencChn:%d OsdCreateRgn error:%#x\n",s32Chn,s32Ret);
                    return -1;
                }

                //s32Ret = OsdAttachToChn(s32Chn * 2 + j,osdHandle[j],posdParam);
                s32Ret = OsdAttachToChn((s32Chn << 1) + j,osdHandle[j],pTitleParam);
                if(HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("vencChn:%d OsdAttachToChn error:%#x\n",s32Chn,s32Ret);
                    return -1;
                }

                g_osdTitle[index][s32Chn].osdHandle[j] = osdHandle[j];
            }

            stBitmap.u32Width	   = pTitleParam->width;
            stBitmap.u32Height	   = pTitleParam->height;
            stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
            stBitmap.pData = (HI_VOID *)pStRGBBuf->pAddr;
            s32Ret = HI_MPI_RGN_SetBitMap(osdHandle[j],&stBitmap);
            if(s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
                return -1;
            }

        }
    }
    return 0;

}
HI_S32 HI3521_CaptureSetBasicInfoTitle(HI_S32 s32Title, HI_S32 s32Chn)
{
    HI_S32		s32Ret;
    BUFFER_S	*pStRGBBuf = NULL;
    RGN_HANDLE	osdHandle[2];
    BITMAP_S stBitmap;
    HI_S32 index;
    HI_S32 j;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    if (((HI_U32)s32Chn >= VI_CHN_NUM_HOST_CHIP))
    {
        AV_PRINT("Invalid channel %d", s32Chn);
        return -1;
    }
    HI_S32 s32Width = 0;
    HI_S32 s32Height = 0;
    CAPTURE_TITLE_PARAM * pTitleParam = NULL;
	HI_S32 s32ViCount = TOTAL_VI_CHN_NUM;
    for (j = 0; j < 2; j++)
    {
        if(pCaptureDev->vi_chn[s32Chn].venc_chn[j].started != HI_ENABLE)
        {
            continue;
        }
        HI_BOOL bNewBuf = HI_FALSE;
        pTitleParam = &(g_osdTitle[s32Title][s32Chn*2 + j].Param);
        if ((HI_U32)(pTitleParam->index) >= MAX_OSD_TITLE)
        {
            AV_PRINT("Invalid channel %d, index %d\n", s32Chn, pTitleParam->index);
            return -1;
        }
        //修改通道名称标题
        HI3521_GetOsdFont_Size(pCaptureDev->vi_chn[s32Chn].venc_chn[j].image_size,s32Width,s32Height);
        index = pTitleParam->index;
        osdHandle[j] = g_osdTitle[index][s32Chn*2 + j].osdHandle[j];
        if(pTitleParam->enable == 0)
        {
            AV_PRINT("pTitleParam->enable:%d!\n",pTitleParam->enable);
            if(j == 0)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
        if(g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].StBufRGB.pAddr != NULL &&
                g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].Param.width == s32Width &&
                g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].Param.height == s32Height)
        {
            pStRGBBuf = &(g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].StBufRGB);
        }
        else
        {
            HI3521_InitOsdTitle(s32Title, s32Chn, j);
            /*分配空间 */
            pStRGBBuf = &(g_osdTitle[index][s32Chn*2 + j].StBufRGB);
            if (pStRGBBuf->nSize < ((pTitleParam->height * pTitleParam->width) << 1))
            {
                if(pStRGBBuf->pAddr != NULL)
                {
                    free(pStRGBBuf->pAddr);
                    pStRGBBuf->pAddr = NULL;
                }
                pStRGBBuf->nSize = ((pTitleParam->height * pTitleParam->width) << 1);
                pStRGBBuf->pAddr = (HI_CHAR *)malloc(pStRGBBuf->nSize);
            }

            /* Check the buffer address */
            if (NULL == pStRGBBuf->pAddr)
            {
                pStRGBBuf->nSize = 0;
                AV_PRINT("Buffer malloc error size:%d!\n",pStRGBBuf->nSize);
                return -1;
            }
            s32Ret = HI3521_PointFont2RGB(pTitleParam, (HI_CHAR *)pStRGBBuf->pAddr);
            if(HI_SUCCESS != s32Ret)
            {
                return s32Ret;
            }
            bNewBuf = HI_TRUE;
        }
        if(osdHandle[j] == (RGN_HANDLE)-1)
        {
            osdHandle[j] = (s32Chn << 1) + j + ((s32ViCount << 1) * index);
            s32Ret = OsdCreateRgn(s32Chn,osdHandle[j],pTitleParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdCreateRgn error:%#x\n",s32Chn,s32Ret);
                return -1;
            }
            s32Ret = OsdAttachToChn((s32Chn << 1) + j,osdHandle[j],pTitleParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdAttachToChn error:%#x\n",s32Chn,s32Ret);
                return -1;
            }

            g_osdTitle[index][s32Chn*2 + j].osdHandle[j] = osdHandle[j];
            //AV_PRINT("CHN %d Stream %d Title %d New osdHandle",s32Chn,j,s32Title);
        }

        stBitmap.u32Width	   = pTitleParam->width;
        stBitmap.u32Height	   = pTitleParam->height;
        stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
        stBitmap.pData = (HI_VOID *)pStRGBBuf->pAddr;
        s32Ret = HI_MPI_RGN_SetBitMap(osdHandle[j],&stBitmap);
        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
            return -1;
        }
        if(bNewBuf == HI_TRUE)
        {
            g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].Param.height = s32Height;
            g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].Param.width = s32Width;
            g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].StBufRGB.nSize = pStRGBBuf->nSize;
            g_osdBasicInfoTitle_Cashe[s32Chn*2 + j].StBufRGB.pAddr = pStRGBBuf->pAddr;
        }
    }
    return 0;
}

HI_S32 HI3521_CaptureSetRunInfoTitle(HI_S32 s32Title, HI_S32 s32Chn)
{
    HI_S32		s32Ret;
    BUFFER_S	*pStRGBBuf = NULL;
    RGN_HANDLE	osdHandle[2];
    BITMAP_S stBitmap;
    HI_S32 index;
    HI_S32 j;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    if (((HI_U32)s32Chn >= VI_CHN_NUM_HOST_CHIP))
    {
        AV_PRINT("Invalid channel %d", s32Chn);
        return -1;
    }
    HI_S32 s32Width = 0;
    HI_S32 s32Height = 0;
    CAPTURE_TITLE_PARAM * pTitleParam = NULL;
	HI_S32 s32Index = (s32Chn/8)*2;
	HI_S32 s32ViCount = TOTAL_VI_CHN_NUM;
    for (j = 0; j < 2; j++)
    {
        if(pCaptureDev->vi_chn[s32Chn].venc_chn[j].started != HI_ENABLE)
        {
            continue;
        }
        pTitleParam = &(g_osdTitle[s32Title][s32Chn*2 + j].Param);
        if ((HI_U32)(pTitleParam->index) >= MAX_OSD_TITLE)
        {
            AV_PRINT("Invalid channel %d, index %d\n", s32Chn, pTitleParam->index);
            return -1;
        }
        HI_BOOL bNewBuf = HI_FALSE;
        HI3521_GetOsdFont_Size(pCaptureDev->vi_chn[s32Chn].venc_chn[j].image_size,s32Width,s32Height);
        index = pTitleParam->index;
        osdHandle[j] = g_osdTitle[index][s32Chn*2 + j].osdHandle[j];
        if(pTitleParam->enable == 0)
        {
            AV_PRINT("pTitleParam->enable:%d!\n",pTitleParam->enable);
            if(j == 0)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
        int iValue = (s32Width * MAX_OSD_STRLEN) * s32Height;
        if(g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].StBufRGB.pAddr != NULL &&
                g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].StBufRGB.nSize >= iValue &&
                g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].Param.width == s32Width &&
                g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].Param.height == s32Height)
        {
            pStRGBBuf = &(g_osdRunInfoTitle_Cashe[index][s32Index + j].StBufRGB);
        }
        else
        {
            HI3521_InitOsdTitle(s32Title, s32Chn, j);
            /*分配空间 */
            pStRGBBuf = &(g_osdTitle[index][s32Chn*2 + j].StBufRGB);
            if (pStRGBBuf->nSize < ((pTitleParam->height * pTitleParam->width) << 1))
            {
                if(pStRGBBuf->pAddr != NULL)
                {
                    free(pStRGBBuf->pAddr);
                    pStRGBBuf->pAddr = NULL;
                }
                pStRGBBuf->nSize = ((pTitleParam->height * pTitleParam->width) << 1);
                pStRGBBuf->pAddr = (HI_CHAR *)malloc(pStRGBBuf->nSize);
            }

            /* Check the buffer address */
            if (NULL == pStRGBBuf->pAddr)
            {
                pStRGBBuf->nSize = 0;
                AV_PRINT("Buffer malloc error size:%d!\n",pStRGBBuf->nSize);
                return -1;
            }
            s32Ret = HI3521_PointFont2RGB(pTitleParam, (HI_CHAR *)pStRGBBuf->pAddr);
            if(HI_SUCCESS != s32Ret)
            {
                return s32Ret;
            }
            bNewBuf = HI_TRUE;
        }
        if(osdHandle[j] == (RGN_HANDLE)-1)
        {
            osdHandle[j] = (s32Chn << 1) + j + ((s32ViCount << 1) * index);
#ifndef OSD_DEBUG
            AV_PRINT("OsdCreateRgn j(%d) osdHandle(%d)", j, osdHandle[j]);
#endif
            s32Ret = OsdCreateRgn(s32Chn,osdHandle[j],pTitleParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdCreateRgn error:%#x\n",s32Chn,s32Ret);
                return -1;
            }
            s32Ret = OsdAttachToChn((s32Chn << 1) + j,osdHandle[j],pTitleParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdAttachToChn error:%#x\n",s32Chn,s32Ret);
                return -1;
            }

            g_osdTitle[index][s32Chn*2 + j].osdHandle[j] = osdHandle[j];
            //AV_PRINT("CHN %d Stream %d Title %d New osdHandle",s32Chn,j,s32Title);
        }

        stBitmap.u32Width	   = pTitleParam->width;
        stBitmap.u32Height	   = pTitleParam->height;
        stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
        stBitmap.pData = (HI_VOID *)pStRGBBuf->pAddr;
        s32Ret = HI_MPI_RGN_SetBitMap(osdHandle[j],&stBitmap);
        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
            return -1;
        }
        if(bNewBuf == HI_TRUE)
        {
            g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].Param.height = s32Height;
            g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].Param.width = s32Width;
            g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].StBufRGB.nSize = pStRGBBuf->nSize;
            g_osdRunInfoTitle_Cashe[s32Title][s32Index + j].StBufRGB.pAddr = pStRGBBuf->pAddr;
        }
    }
    return 0;
}
/*****************************************************************************
 函 数 名  : HI3521_InitOsdTitle
 功能描述  : 初始化OSD
 输入参数  : HI_S32 s32Title, HI_S32 s32Chn, HI_S32 HI_S32 s32StreamType
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年8月25日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_InitOsdTitle(HI_S32 s32Title, HI_S32 s32Chn, HI_S32 s32StreamType)
{
    POINT_FONT_S stPointFont;
    HI_S32 s32VideoSize = 0;
    HI_CHAR *pucBuffer = NULL;
    HI_S32 size = 0;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    pCaptureDev->vi_chn[s32Chn].venc_chn[CHL_2END_T].enable = HI_ENABLE;
    if ((s32StreamType != HI_MAIN_STREAM) && (s32StreamType != HI_SUB_STREAM))
    {
        AV_PRINT("the type of venc stream is out of range.");
        return;
    }
    s32VideoSize = pCaptureDev->vi_chn[s32Chn].venc_chn[s32StreamType].image_size;
    HI3521_GetOsdFont(s32VideoSize, &stPointFont, OSD_TYPE_TEXT);
    size = (stPointFont.s32Height*stPointFont.s32Width * (MAX_OSD_STRLEN / 8));
    if (g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster == NULL)
    {
        g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster = (HI_U8*)malloc(size);
        g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.rastersize = size;
        if (NULL == g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster)
        {
            return;
        }
    }
    else if(g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.rastersize<size)
    {
        free(g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster);
        g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster = NULL;
        g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster = (HI_U8*)malloc(size);
        g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.rastersize = size;
        if (NULL == g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster)
        {
            return;
        }
    }
    memset(g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster, 0, g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.rastersize);
    pucBuffer = (HI_CHAR*)g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.raster;
    HI3521_SetOsdShowData(s32Chn, s32Title, pucBuffer, &stPointFont);
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.index = s32Title;
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.enable = 1;
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.x = 0;          /*X坐标起始点*/
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.y= ((stPointFont.s32Height * s32Title));/*Y起始点*/
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.width = (stPointFont.s32Width * MAX_OSD_STRLEN);
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.height = stPointFont.s32Height;
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.fg_color= 0x00000000;// 前景色为黑
    g_osdTitle[s32Title][s32Chn*2 + s32StreamType].Param.bg_color= 0x00FFFFFF;// 背景色为白
    g_osdTime[s32Chn].Param.enable = HI_ENABLE;
}

/*****************************************************************************
 函 数 名  : HI3521_EnableOsd
 功能描述  : 使能通道OSD刷新
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID	HI3521_EnableOsd(HI_S32 s32Chn)
{
#if 0
    /*mod by xzhang 20150825 for double stream support begin.*/
    HI_S32 s32Title = 1;

    for (s32Title = 0; s32Title < MAX_OSD_TITLE; s32Title++)
    {
#if 0
        HI3521_InitOsdTitle(s32Title, s32Chn, HI_MAIN_STREAM);
#endif
        HI3521_CaptureSetTitle(s32Title, s32Chn, &(g_osdTitle[s32Title][s32Chn].Param));
    }


    /*mod by xzhang 20150825 for double stream support end.*/

    return;
#else
    HI_S32 s32Title = 1;
    //通道、线路，车号
    HI3521_CaptureSetBasicInfoTitle(s32Title, s32Chn);
    //运行参数OSD
    for (s32Title = 2; s32Title < MAX_OSD_TITLE; s32Title++)
    {
        HI3521_CaptureSetRunInfoTitle(s32Title, s32Chn);
    }
#endif

}

/*****************************************************************************
 函 数 名  : HI3521_OsdShow
 功能描述  : OSD刷新显示
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_OsdShow()
{
    static time_t	oldosdtime1 = 0;
    time_t			curosdtime1 = time(NULL);
    if (oldosdtime1 != curosdtime1 || oldosdtime1 == 0)
    {
        static OSD_TITLE_DATA_ST stOsdInfo;
        HI3521_ResetBusIdTitleCache(&stOsdInfo);
        HI3521_ResetPositionTitleCache(&stOsdInfo);
        HI3521_ResetSpeedTitleCache(&stOsdInfo);
        HI3521_ResetScheduleTitleCache(&stOsdInfo);
        HI3521_ResetUITitleCache(&stOsdInfo);
        memcpy(&stOsdInfo,&g_stOsdShowData,sizeof(OSD_TITLE_DATA_ST));
        HI_S32 	s32Chn = 0;
		HI_S32 s32ChnTotalCnt = VI_CHN_NUM_HOST_CHIP;
        for (s32Chn = 0; s32Chn < s32ChnTotalCnt; s32Chn++)
        {
            HI3521_EnableOsd(s32Chn);
        }
        oldosdtime1 = curosdtime1;
    }
}

/*****************************************************************************
 函 数 名  : HI3521_ModOsdTitlePara
 功能描述  :
 输入参数  : CAPTURE_TITLE_PARAM *pstTitlePara, enum capture_size_t enMode, SIZE_S *pstSize, POINT_FONT_S *pstPointFont
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_ModOsdTitlePara(CAPTURE_TITLE_PARAM *pstTitlePara,PIC_SIZE_E enMode, SIZE_S *pstSize, POINT_FONT_S *pstPointFont)
{
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();

    pstTitlePara->x = pstSize->u32Width * pstTitlePara->x / 352;

    if (enMode == PIC_HD1080 || enMode == PIC_HD720)
    {
        pstTitlePara->x *= 2;
    }

    if ((pstTitlePara->x + (HI_U32)(pstPointFont->s32Width * g_TimeLen)) > pstSize->u32Width)
    {
        pstTitlePara->x = pstSize->u32Width - (HI_U32)(TIME_STRING_LEN * pstPointFont->s32Width);
    }

    if (pstTitlePara->x > pstSize->u32Width)
    {
        pstTitlePara->x = 0;
    }

    pstTitlePara->x = pstTitlePara->x - pstTitlePara->x % 8;

    pstTitlePara->y = pstTitlePara->y * pstSize->u32Height / (pCaptureDev->vstd == VIDEO_ENCODING_MODE_PAL?288:240);

    if((pstTitlePara->y + (HI_U32)pstPointFont->s32Height) > pstSize->u32Height )
    {
        pstTitlePara->y = 0;
    }

    pstTitlePara->y = pstTitlePara->y - pstTitlePara->y % 2;

    pstTitlePara->width	= pstPointFont->s32Width * g_TimeLen;
    pstTitlePara->height = pstPointFont->s32Height;

#ifndef OSD_DEBUG
    AV_PRINT("#####2222222222x(%d) y(%d) w(%d) h(%d) enMode(%d)",
             pstTitlePara->x, pstTitlePara->y, pstTitlePara->width, pstTitlePara->height, enMode);
#endif

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_ModOsdTitlePara
 功能描述  :
 输入参数  : BUFFER_S *pStBufFont, POINT_FONT_S *pstPointFont
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_MallocBufFont(BUFFER_S *pStBufFont, POINT_FONT_S *pstPointFont)
{
    /* Malloc the time stap buffer */
    if (pStBufFont->nSize < (pstPointFont->s32Height * pstPointFont->s32Width * g_TimeLen / 8))
    {
        if (pStBufFont->pAddr != NULL)
        {
            free(pStBufFont->pAddr);
            pStBufFont->pAddr = NULL;
        }

        pStBufFont->nSize = (pstPointFont->s32Height * pstPointFont->s32Width * g_TimeLen / 8);
        pStBufFont->pAddr = (HI_CHAR *)malloc(pStBufFont->nSize);
        AV_PRINT("TIME HI3521_MallocBufFont size(%d--%d--%d)",pstPointFont->s32Width,pstPointFont->s32Height,pStBufFont->nSize);
        memset(pStBufFont->pAddr, 0, pStBufFont->nSize);
    }

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_Time2PointFont
 功能描述  : 将具体时间转化为点阵数据
 输入参数  : pTm		-- 具体时间,指向OSD_TIME的结构体指针
 			 pFont		-- 点阵数据信息,指向POINT_FONT_S的指针
 输出参数  : pBuf		-- 点阵数据存放区域
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_Time2PointFont(OSD_TIME *pTm, HI_CHAR *pBuf, const POINT_FONT_S *pFont)
{
    HI_U32 		 u32Index, u32Tmp;;
    HI_S32 		 s32FW, s32FH;
    SYSTEM_TIME	*pSysTm = NULL;

    // 准备日期分隔符
    switch(pTm->dateSpec)
    {
    case DS_DASH:
        u32Index = 11;
        break;
    case DS_SLASH:
        u32Index = 12;
        break;
    case DS_DOT:
        u32Index = 13;
        break;
    default:
        u32Index = 13;
        break;
    }

    s32FW    = pFont->s32Width/8;
    s32FH    = pFont->s32Height * s32FW;
    pSysTm   = &(pTm->sysTime);

#ifndef OSD_DEBUG
    AV_PRINT("pFont->s32Width(%d),pFont->s32Height(%d),s32FW(%d), s32FH(%d),year(%d),mon(%d),day(%d),hour(%d),min(%d),sec(%d),wday(%d),isdst(%d)",
             pFont->s32Width,pFont->s32Height,s32FW, s32FH,
             pSysTm->year,pSysTm->month,
             pSysTm->day,pSysTm->hour,
             pSysTm->minute,pSysTm->second,
             pSysTm->wday,pSysTm->isdst);
#endif
    // 叠加年月日
    switch( pTm->dateFmt)
    {
    case DF_YYMMDD:
        HI3521_char_cpy(pBuf+s32FW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont);
        HI3521_char_cpy(pBuf+s32FW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont);
        HI3521_char_cpy(pBuf+s32FW*2, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*4, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*5, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*7, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont);
        break;
    case DF_MMDDYY:
        HI3521_char_cpy(pBuf+s32FW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*2, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*4, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*5, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont);
        HI3521_char_cpy(pBuf+s32FW*7, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont);
        HI3521_char_cpy(pBuf+s32FW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont);
        break;
    case DF_DDMMYY:
        HI3521_char_cpy(pBuf+s32FW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*2, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*4, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont);
        HI3521_char_cpy(pBuf+s32FW*5, pFont->pFont + (s32FH*u32Index), pFont);
        HI3521_char_cpy(pBuf+s32FW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont);
        HI3521_char_cpy(pBuf+s32FW*7, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont);
        HI3521_char_cpy(pBuf+s32FW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont);
        break;
    default:
        AV_PRINT("Unknown OSD time format\n");
        break;
    }

    // 叠加一个空格
    HI3521_char_cpy( pBuf+s32FW*10, pFont->pFont + (s32FH*17), pFont);

    // 叠加小时
    switch (pTm->timeFmt)
    {
    case TF_12:
        u32Tmp = pSysTm->hour % 12;
        if (u32Tmp != 0)
        {
            HI3521_char_cpy(pBuf+s32FW*11, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 10), pFont);
            HI3521_char_cpy(pBuf+s32FW*12, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 1), pFont);
        }
        else
        {
            HI3521_char_cpy(pBuf+s32FW*11, pFont->pFont + s32FH*1, pFont);
            HI3521_char_cpy(pBuf+s32FW*12, pFont->pFont + s32FH*2, pFont);
        }
        break;
    case TF_24:
        u32Tmp = pSysTm->hour;
        HI3521_char_cpy(pBuf+s32FW*11, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 10), pFont);
        HI3521_char_cpy(pBuf+s32FW*12, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 1), pFont);
        break;
    default:
        AV_PRINT("Unkown time format\n");
        break;
    }

    // 叠加分钟和秒钟
    HI3521_char_cpy(pBuf+s32FW*13, pFont->pFont + (s32FH*10), pFont);
    HI3521_char_cpy(pBuf+s32FW*14, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->minute, 10), pFont);
    HI3521_char_cpy(pBuf+s32FW*15, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->minute, 1), pFont);
    HI3521_char_cpy(pBuf+s32FW*16, pFont->pFont + (s32FH*10), pFont);
    HI3521_char_cpy(pBuf+s32FW*17, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->second, 10), pFont);
    HI3521_char_cpy(pBuf+s32FW*18, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->second, 1), pFont);

    // 叠加上下午提示
    switch (pTm->timeFmt)
    {
    case TF_12:

        // 叠加一个空格
        HI3521_char_cpy(pBuf+s32FW*19, pFont->pFont + (s32FH*17), pFont);
        if (pSysTm->hour < 12)
        {
            HI3521_char_cpy(pBuf+s32FW*20, pFont->pFont + (s32FH*14), pFont);
        }
        else
        {
            HI3521_char_cpy(pBuf+s32FW*20, pFont->pFont + (s32FH*15), pFont);
        }
        HI3521_char_cpy(pBuf+s32FW*21, pFont->pFont + (s32FH*16), pFont);
        HI3521_char_cpy(pBuf+s32FW*22, pFont->pFont + (s32FH*17), pFont);
        break;
    case TF_24:
    default:
        HI3521_char_cpy(pBuf+s32FW*(19), pFont->pFont + (s32FH*17), pFont);
        HI3521_char_cpy(pBuf+s32FW*(20), pFont->pFont + (s32FH*17), pFont);
        HI3521_char_cpy(pBuf+s32FW*(21), pFont->pFont + (s32FH*17), pFont);
        HI3521_char_cpy(pBuf+s32FW*(22), pFont->pFont + (s32FH*17), pFont);
        break;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3521_SetTimeStap
 功能描述  :
 输入参数  : BUFFER_S **pBufFont, POINT_FONT_S *pFont,
			 CAPTURE_TITLE_PARAM *pTitlePara, OSD_TIME *pstOsdTime
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数


*****************************************************************************/
HI_S32 HI3521_SetTimeStap(BUFFER_S **pBufFont, POINT_FONT_S *pFont,
                          CAPTURE_TITLE_PARAM *pTitlePara, OSD_TIME *pstOsdTime)
{
    BUFFER_S *pBuf = *pBufFont;
    HI_S32 s32Ret = HI_SUCCESS;

    HI3521_MallocBufFont(pBuf, pFont);

    /* Check the buffer address */
    if (NULL == pBuf->pAddr)
    {
        pBuf->nSize = 0;
        return HI_FAILURE;
    }

    /* Prepare the time stap raster */
    memset(pBuf->pAddr, 0, pBuf->nSize);
    s32Ret = HI3521_Time2PointFont(pstOsdTime, pBuf->pAddr, pFont);
    if(HI_SUCCESS != s32Ret)
    {
        AV_PRINT("#####HI_TimeToPoHI_S32Font error:%x\n", s32Ret);
        free(pBuf->pAddr);
        return HI_FAILURE;
    }

    /* Set time stap parament */
    pTitlePara->raster = (HI_U8 *)pBuf->pAddr;

    return HI_SUCCESS;
}

HI_VOID HI3521_DebugCapTitlePara(HI_S32 s32Chn, CAPTURE_TITLE_PARAM *pstCapTitlePara)
{
    AV_PRINT("s32Chn(%d) idx(%d) en(%d) x(%d) y(%d) w(%d) h(%d) fgc(%d) bgc(%d) raster(%s)",
             s32Chn,
             pstCapTitlePara->index,
             pstCapTitlePara->enable,
             pstCapTitlePara->x,
             pstCapTitlePara->y,
             pstCapTitlePara->width,
             pstCapTitlePara->height,
             pstCapTitlePara->fg_color,
             pstCapTitlePara->bg_color,
             pstCapTitlePara->raster);
    return;
}


/*****************************************************************************
 函 数 名  : HI3521_CaptureSetTime
 功能描述  :
 输入参数  : HI_S32 channel, const SYSTEM_TIME * pTime
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_CaptureSetTime(HI_S32 s32Chn, const SYSTEM_TIME * pTime)
{
    HI_S32 			i;
    HI_S32			s32Ret;
    OSD_TIME		osdTime;
    POINT_FONT_S	font;
    BUFFER_S		*pStBufFont=NULL;
    BUFFER_S		*pStBufRGB=NULL;
    BITMAP_S		stBitmap;
    RGN_HANDLE		osdHandle[2] ;
    CAPTURE_TITLE_PARAM *pParam = NULL;
    CAPTURE_TITLE_PARAM capTitlePara;
    SIZE_S stSize;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    HI_S32 s32VideoSize;
    HI3521_PthreadMutexLock(&(g_osdTime[s32Chn].lock));

    for (i = 0; i < 2; i++)
    {
        if(pCaptureDev->vi_chn[s32Chn].venc_chn[i].started != HI_ENABLE)
        {
            continue;
        }
        osdHandle[i] = g_osdTime[s32Chn].osdHandle[i];
        memcpy(&capTitlePara, &g_osdTime[s32Chn].Param, sizeof(CAPTURE_TITLE_PARAM));/*Param在那里初始化的.*/
        pParam = &capTitlePara;
#if 0
        if (pParam->enable == HI_DISABLE)
        {
            /*destroy*/
            if (osdHandle[i] != -1)
            {
                HI_MPI_RGN_Destroy(osdHandle[i]);
                g_osdTime[s32Chn].osdHandle[i] = -1;
                osdHandle[i] = -1;
            }

            /*unlock*/
            HI3531_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));

            if (i == 0)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
#endif
        /*init font and RGB*/
        pStBufFont = &(g_osdTime[s32Chn].StBufFont);
        pStBufRGB = &(g_osdTime[s32Chn].StBufRGB);

        /* Init time struct */
        osdTime.dateFmt = g_osdTime[s32Chn].osdTime.dateFmt;
        osdTime.dateSpec = g_osdTime[s32Chn].osdTime.dateSpec;
        osdTime.timeFmt  = g_osdTime[s32Chn].osdTime.timeFmt;
        memcpy(&(osdTime.sysTime), pTime, sizeof(osdTime.sysTime));

        if (osdHandle[i] != (RGN_HANDLE)-1)
        {
            if(g_osdTime[s32Chn].changed != 0
                    || g_osdTime[s32Chn].imagesize[i] != pCaptureDev->vi_chn[s32Chn].venc_chn[i].image_size )
            {
                HI_MPI_RGN_Destroy(osdHandle[i]);
                g_osdTime[s32Chn].osdHandle[i] = -1;
                osdHandle[i] = -1;
                //AV_PRINT("#####osd time changed");
                //g_osdTime[channel].changed = 0;
            }
        }

        g_osdTime[s32Chn].imagesize[i] = pCaptureDev->vi_chn[s32Chn].venc_chn[i].image_size;
        s32VideoSize = pCaptureDev->vi_chn[s32Chn].venc_chn[i].image_size;
        //AV_PRINT("time, s32Chn(%d),i(%d),osd-size=%d", s32Chn, i, s32VideoSize);
        HI3521_GetOsdFont(s32VideoSize, &font, OSD_TYPE_TIME);

        HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd, (PIC_SIZE_E)s32VideoSize, &stSize);
        HI3521_ModOsdTitlePara(pParam, (PIC_SIZE_E)s32VideoSize, &stSize, &font);

        s32Ret = HI3521_SetTimeStap(&pStBufFont, &font, pParam, &osdTime);
        if (HI_SUCCESS != s32Ret)
        {
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return s32Ret;
        }

        if (osdHandle[i] == (RGN_HANDLE)-1)
        {
            osdHandle[i] = s32Chn * 2 + i;
            //	AV_PRINT("#####osdHandle[%d](%d)", i, osdHandle[i]);
            s32Ret = OsdCreateRgn(s32Chn, osdHandle[i], pParam);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn(%d) OsdCreateRgn error(%#x)",s32Chn,s32Ret);
                HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
                return HI_FAILURE;
            }
            g_osdTime[s32Chn].osdHandle[i] = osdHandle[i];
            //HI3531_DebugCapTitlePara(pParam);
        }

        //s32Ret = OsdAttachToChn(s32Chn * 2 + i, osdHandle[i], pParam);
        s32Ret = OsdAttachToChn(s32Chn * 2 + i, osdHandle[i], pParam);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("vencChn(%d) OsdAttachToChn error(%#x)", s32Chn * 2 + i, s32Ret);
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return HI_FAILURE;
        }

#ifndef OSD_DEBUG
        HI3521_DebugCapTitlePara(s32Chn,pParam);
#endif

        if (pCaptureDev->vi_chn[s32Chn].venc_chn[i].started == HI_TRUE)
        {
            /* Malloc the RGB buffer */
            if (pStBufRGB->nSize < (pParam->height * pParam->width * 2))
            {
                if(pStBufRGB->pAddr != NULL)
                {
                    free(pStBufRGB->pAddr);
                    pStBufRGB->pAddr = NULL;
                }
                pStBufRGB->nSize = ((pParam->height * pParam->width * 2));
                pStBufRGB->pAddr = (HI_CHAR *)malloc(pStBufRGB->nSize);
            }

#ifndef OSD_DEBUG
            AV_PRINT("malloc rgb buf size(%d) addr(%p).", pStBufRGB->nSize, pStBufRGB->pAddr);
#endif

            /* Check the buffer address */
            if(NULL == pStBufRGB->pAddr)
            {
                pStBufRGB->nSize = 0;
                AV_PRINT("Buffer malloc error!\n");
                HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
                return HI_FAILURE;
            }

            /* Prepate the RGB data */
            memset(pStBufRGB->pAddr, 0, pStBufRGB->nSize);
            s32Ret = HI3521_PointFont2RGB(pParam, pStBufRGB->pAddr);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("HI3531_PointFont2RGB error:%x\n",s32Ret);
                HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
                return HI_FAILURE;
            }

            /* Set the bitmap parament */
            stBitmap.u32Width	   = pParam->width;
            stBitmap.u32Height	   = pParam->height;
            /* Only PIXEL_FORMAT_RGB_1555 is supported */
            stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
            stBitmap.pData = (HI_VOID *)pStBufRGB->pAddr;

            //HI3531_DebugCapTitlePara(pParam);
            //osdHandle = channel * 2 + index;
#ifndef OSD_DEBUG
            AV_PRINT("stBitmap.u32Width(%d), stBitmap.u32Height(%d)",stBitmap.u32Width, stBitmap.u32Height);
#endif
            s32Ret = HI_MPI_RGN_SetBitMap(osdHandle[i],&stBitmap);
            if(s32Ret != HI_SUCCESS)
            {
                AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
                HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
                return HI_FAILURE;
            }
        }
    }

    HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
    return s32Ret;
}

/*****************************************************************************
 函 数 名  : HI3521_SetTimeOsd
 功能描述  :
 输入参数  : HI_VOID
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID HI3521_SetTimeOsd(HI_VOID)
{
    SYSTEM_TIME 	osdtime;
    static time_t	oldosdtime = 0;
    time_t			curosdtime = time(NULL);
    struct tm		time;
    HI_S32 			s32Chn;

    if (oldosdtime != curosdtime || oldosdtime == 0)
    {
        localtime_r(&curosdtime, &time);

        osdtime.second	= time.tm_sec;			/* seconds (1 - 60) */
        osdtime.minute	= time.tm_min;			/* minutes (1 - 60) */
        osdtime.hour	= time.tm_hour; 		/* hours (1 - 24)	*/
        osdtime.wday	= time.tm_wday; 		/* day of week		*/
        osdtime.day 	= time.tm_mday; 		/* day of the month */
        osdtime.month	= time.tm_mon + 1;		/* month (1-12) 	*/
        osdtime.year	= time.tm_year + 1900;	/* year 			*/
		HI_S32 s32ChnTotalCnt = VI_CHN_NUM_HOST_CHIP;
        for (s32Chn = 0; s32Chn < s32ChnTotalCnt; s32Chn++)
        {
            HI3521_CaptureSetTime(s32Chn, &osdtime);
        }
        oldosdtime = curosdtime;
    }
    return;
}


/*****************************************************************************
 函 数 名  : HI3521_CaptureSetTime
 功能描述  :
 输入参数  : HI_S32 channel, const SYSTEM_TIME * pTime
             HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月19日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_CaptureSetTime_Snap(HI_S32 s32Chn, const SYSTEM_TIME * pTime)
{
    HI_S32			s32Ret;
    OSD_TIME		osdTime;
    POINT_FONT_S	font;
    BUFFER_S		*pStBufFont=NULL;
    BUFFER_S		*pStBufRGB=NULL;
    BITMAP_S		stBitmap;
    RGN_HANDLE	    snaposdHandle;
    CAPTURE_TITLE_PARAM *pParam = NULL;
    CAPTURE_TITLE_PARAM capTitlePara;
    SIZE_S stSize;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    HI_S32 s32VideoSize;
    HI3521_PthreadMutexLock(&(g_osdTime[s32Chn].lock));

    snaposdHandle = g_osdTime[s32Chn].snaposdHandle;
    memcpy(&capTitlePara, &g_osdTime[s32Chn].Param, sizeof(CAPTURE_TITLE_PARAM));/*Param在那里初始化的.*/
    pParam = &capTitlePara;
    /*init font and RGB*/
    pStBufFont = &(g_osdTime[s32Chn].StBufFont);
    pStBufRGB = &(g_osdTime[s32Chn].StBufRGB);

    /* Init time struct */
    osdTime.dateFmt = g_osdTime[s32Chn].osdTime.dateFmt;
    osdTime.dateSpec = g_osdTime[s32Chn].osdTime.dateSpec;
    osdTime.timeFmt  = g_osdTime[s32Chn].osdTime.timeFmt;
    memcpy(&(osdTime.sysTime), pTime, sizeof(osdTime.sysTime));

    if (snaposdHandle != (RGN_HANDLE)-1)
    {
        if(g_osdTime[s32Chn].changed != 0
                || g_osdTime[s32Chn].jpegsize!= pCaptureDev->vi_chn[s32Chn].jpeg_chn.image_size )
        {
            HI_MPI_RGN_Destroy(snaposdHandle);
            g_osdTime[s32Chn].snaposdHandle = -1;
            snaposdHandle = -1;
            //AV_PRINT("#####osd time changed");
            //g_osdTime[channel].changed = 0;
        }
    }

    g_osdTime[s32Chn].jpegsize= pCaptureDev->vi_chn[s32Chn].jpeg_chn.image_size;
    s32VideoSize = pCaptureDev->vi_chn[s32Chn].jpeg_chn.image_size;

    HI3521_GetOsdFont(s32VideoSize, &font, OSD_TYPE_TIME);

    HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd, (PIC_SIZE_E)s32VideoSize, &stSize);
    HI3521_ModOsdTitlePara(pParam, (PIC_SIZE_E)s32VideoSize, &stSize, &font);

    s32Ret = HI3521_SetTimeStap(&pStBufFont, &font, pParam, &osdTime);
    if (HI_SUCCESS != s32Ret)
    {
        HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
        return s32Ret;
    }

    if (snaposdHandle == (RGN_HANDLE)-1)
    {
        snaposdHandle = MAX_OSD_TITLE*s32Chn+1+TOTAL_VENC_CHN_NUM*MAX_OSD_TITLE;
        //	AV_PRINT("#####osdHandle[%d](%d)", i, osdHandle[i]);
        s32Ret = OsdCreateRgn(s32Chn, snaposdHandle, pParam);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("vencChn(%d) OsdCreateRgn error(%#x)",s32Chn,s32Ret);
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return HI_FAILURE;
        }
        g_osdTime[s32Chn].snaposdHandle = snaposdHandle;
        //HI3531_DebugCapTitlePara(pParam);
    }

    //s32Ret = OsdAttachToChn(s32Chn * 2 + i, osdHandle[i], pParam);
    s32Ret = OsdAttachToChn(pCaptureDev->vi_chn[s32Chn].jpeg_chn.jpegChn, snaposdHandle, pParam);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("vencChn(%d) OsdAttachToChn error(%#x)",pCaptureDev->vi_chn[s32Chn].jpeg_chn.jpegChn,s32Ret);
        HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
        return HI_FAILURE;
    }

#ifndef OSD_DEBUG
    HI3521_DebugCapTitlePara(s32Chn,pParam);
#endif

    //if (pCaptureDev->vi_chn[s32Chn].venc_chn[i].started == HI_TRUE)
    {
        /* Malloc the RGB buffer */
        if (pStBufRGB->nSize < (pParam->height * pParam->width * 2))
        {
            if(pStBufRGB->pAddr != NULL)
            {
                free(pStBufRGB->pAddr);
                pStBufRGB->pAddr = NULL;
            }
            pStBufRGB->nSize = ((pParam->height * pParam->width * 2));
            pStBufRGB->pAddr = (HI_CHAR *)malloc(pStBufRGB->nSize);
        }

#ifndef OSD_DEBUG
        AV_PRINT("malloc rgb buf size(%d) addr(%p).", pStBufRGB->nSize, pStBufRGB->pAddr);
#endif

        /* Check the buffer address */
        if(NULL == pStBufRGB->pAddr)
        {
            pStBufRGB->nSize = 0;
            AV_PRINT("Buffer malloc error!\n");
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return HI_FAILURE;
        }

        /* Prepate the RGB data */
        memset(pStBufRGB->pAddr, 0, pStBufRGB->nSize);
        s32Ret = HI3521_PointFont2RGB(pParam, pStBufRGB->pAddr);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI3531_PointFont2RGB error:%x\n",s32Ret);
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return HI_FAILURE;
        }

        /* Set the bitmap parament */
        stBitmap.u32Width	   = pParam->width;
        stBitmap.u32Height	   = pParam->height;
        /* Only PIXEL_FORMAT_RGB_1555 is supported */
        stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
        stBitmap.pData = (HI_VOID *)pStBufRGB->pAddr;

        //HI3531_DebugCapTitlePara(pParam);
        //osdHandle = channel * 2 + index;
#ifndef OSD_DEBUG
        AV_PRINT("stBitmap.u32Width(%d), stBitmap.u32Height(%d)",stBitmap.u32Width, stBitmap.u32Height);
#endif
        s32Ret = HI_MPI_RGN_SetBitMap(snaposdHandle,&stBitmap);
        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
            HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
            return HI_FAILURE;
        }
    }

    HI3521_PthreadMutexUnlock(&(g_osdTime[s32Chn].lock));
    return s32Ret;
}
HI_S32 HI3521_SnapSetTitle(HI_S32 channel, CAPTURE_TITLE_PARAM * pParam)
{
    HI_S32		s32Ret;
    BUFFER_S	*pStBuf = NULL;
    RGN_HANDLE	snaposdHandle;
    CAPTURE_TITLE_PARAM *posdParam;
    CAPTURE_TITLE_PARAM Param;
    BITMAP_S stBitmap;
    HI_S32 index;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    SIZE_S stSize;
    HI_S32 video_size ;//= pCaptureDev->vi_chn[channel].venc_chn[0].image_size;
    HI_S32 s32TmpChn = channel/8+MAX_VI_CHN*2;

    if(((HI_U32)(pParam->index) >= MAX_OSD_TITLE) || ((HI_U32)channel >= VI_CHN_NUM_HOST_CHIP))
    {
        AV_PRINT("Invalid channel %d, index %d\n", channel, pParam->index);
        return -1;
    }
    video_size = pCaptureDev->vi_chn[channel].jpeg_chn.image_size;
    memcpy(&Param, pParam, sizeof(CAPTURE_TITLE_PARAM));

    if(pParam->index != 0)
    {
        //修改通道名称标题
        index = pParam->index;
        snaposdHandle = g_osdTitle[index][channel].snaposdHandle;
        if(pParam->enable == 0)
        {
            return 0;
        }
        posdParam = &Param;
        AV_PRINT("video_size : %d App OSD Title chn:%d <x:%d,y:%d,w:%d,h:%d>",video_size,channel,posdParam->x,posdParam->y,posdParam->width,posdParam->height);
        HI3521_Comm_SYS_GetPicSize((VIDEO_NORM_E)pCaptureDev->vstd,(PIC_SIZE_E)video_size,&stSize);
        pStBuf = &(g_osdTitle[index][s32TmpChn].StBufRGB);
        /* Malloc the RGB buffer */
        if(pStBuf->nSize < (posdParam->height * posdParam->width * 2))
        {
            if(pStBuf->pAddr != NULL)
            {
                free(pStBuf->pAddr);
                pStBuf->pAddr = NULL;
            }
            pStBuf->nSize = ((posdParam->height * posdParam->width * 2));
            pStBuf->pAddr = (HI_CHAR*)malloc(pStBuf->nSize);
        }
        /* Check the buffer address */
        if(NULL == pStBuf->pAddr)
        {
            pStBuf->nSize = 0;
            AV_PRINT("Buffer malloc error size:%d!\n",pStBuf->nSize);
            return -1;
        }
        /* Prepate the RGB data */
        s32Ret = HI3521_PointFont2RGB(posdParam, pStBuf->pAddr);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
        if(snaposdHandle == (RGN_HANDLE)-1)
        {
            HI_S32 jpegChn = pCaptureDev->vi_chn[channel].jpeg_chn.jpegChn;

            snaposdHandle = MAX_OSD_TITLE*channel +TOTAL_VENC_CHN_NUM*MAX_OSD_TITLE*index;

            s32Ret = OsdCreateRgn(channel,snaposdHandle,posdParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdCreateRgn error:%#x\n",channel,s32Ret);
                return -1;
            }
            AV_PRINT("jpegChn to osd : %d",jpegChn);
            s32Ret = OsdAttachToChn(jpegChn,snaposdHandle,posdParam);
            if(HI_SUCCESS != s32Ret)
            {
                AV_PRINT("vencChn:%d OsdAttachToChn error:%#x\n",channel,s32Ret);
                return -1;
            }
            g_osdTitle[index][channel].snaposdHandle = snaposdHandle;
        }

        stBitmap.u32Width	   = posdParam->width;
        stBitmap.u32Height	   = posdParam->height;
        stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
        stBitmap.pData = (HI_VOID *)pStBuf->pAddr;

        s32Ret = HI_MPI_RGN_SetBitMap(snaposdHandle,&stBitmap);

        if(s32Ret != HI_SUCCESS)
        {
            AV_PRINT("HI_MPI_RGN_SetBitMaperror:%x\n",s32Ret);
            return -1;
        }
    }
    else if(pParam->index == 0)
    {
        posdParam = &(g_osdTime[channel].Param);
        memcpy(&(g_osdTime[channel].Param), &Param, sizeof(CAPTURE_TITLE_PARAM));
        g_osdTime[channel].changed = 1;
        AV_PRINT("App OSD Time chn:%d <x:%d,y:%d,w:%d,h:%d>\n",channel,posdParam->x,posdParam->y,posdParam->width,posdParam->height);
        SYSTEM_TIME 	osdtime;
        time_t			curosdtime = time(NULL);
        struct tm		time;
        localtime_r(&curosdtime, &time);
        osdtime.second	= time.tm_sec;			/* seconds (1 - 60) */
        osdtime.minute	= time.tm_min;			/* minutes (1 - 60) */
        osdtime.hour	= time.tm_hour; 		/* hours (1 - 24)	*/
        osdtime.wday	= time.tm_wday; 		/* day of week		*/
        osdtime.day 	= time.tm_mday; 		/* day of the month */
        osdtime.month	= time.tm_mon + 1;		/* month (1-12) 	*/
        osdtime.year	= time.tm_year + 1900;	/* year 			*/
        /*这里不会跑不修改 */
        HI3521_CaptureSetTime_Snap(channel,&osdtime);
        g_osdTime[channel].changed = 0;
    }
    return 0;
}


/*mod by xzhang for double stream support end 20150825*/

/*****************************************************************************
 函 数 名  : HI3531_SnapOsdShow
 功能描述  : 抓图OSD
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月16日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID	HI3531_SnapOsdShow(HI_S32 s32Chn)
{
    HI_S32 s32Title = 0;
    //CAPTURE_TITLE_PARAM *pCapOsdParam = NULL;
    POINT_FONT_S stPointFont;
    HI_CHAR *pucBuffer = NULL;
    HI_S32 size = 0;
    HI_S32 tmpchn = s32Chn/8+MAX_VI_CHN*2;
    Capture_Dev_t *pCaptureDev = HI3521_GetCaptureDev();
    HI_S32 s32VideoSize = pCaptureDev->vi_chn[s32Chn].jpeg_chn.image_size;
    HI3521_GetOsdFont(s32VideoSize, &stPointFont, OSD_TYPE_TEXT);
    for (s32Title = 1; s32Title < MAX_OSD_TITLE; s32Title++)
    {
        size = (stPointFont.s32Height*stPointFont.s32Width*(MAX_OSD_STRLEN / 8));
        if (g_osdTitle[s32Title][tmpchn].Param.raster == NULL)
        {
            AV_PRINT("alloc raster size is %d--%d--%d", size,stPointFont.s32Width,stPointFont.s32Height);
            g_osdTitle[s32Title][tmpchn].Param.raster = (HI_U8*)malloc(size);
            g_osdTitle[s32Title][tmpchn].Param.rastersize = size;
            if (NULL == g_osdTitle[s32Title][tmpchn].Param.raster)
            {
                AV_PRINT("alloc raster size failed");
                return;
            }
            memset(g_osdTitle[s32Title][tmpchn].Param.raster, 0, size);
        }
        else if(g_osdTitle[s32Title][tmpchn].Param.rastersize<size)
        {
            free(g_osdTitle[s32Title][tmpchn].Param.raster);
            g_osdTitle[s32Title][tmpchn].Param.raster = NULL;
            g_osdTitle[s32Title][tmpchn].Param.raster = (HI_U8*)malloc(size);
            g_osdTitle[s32Title][tmpchn].Param.rastersize = size;
            if (NULL == g_osdTitle[s32Title][tmpchn].Param.raster)
            {
                return;
            }
            memset(g_osdTitle[s32Title][tmpchn].Param.raster, 0, size);
        }
        memset(g_osdTitle[s32Title][tmpchn].Param.raster, 0,g_osdTitle[s32Title][tmpchn].Param.rastersize);
        pucBuffer = (HI_CHAR*)g_osdTitle[s32Title][tmpchn].Param.raster;
        HI3521_SetOsdShowData(s32Chn, s32Title, pucBuffer, &stPointFont);
        g_osdTitle[s32Title][tmpchn].Param.index = s32Title;
        g_osdTitle[s32Title][tmpchn].Param.enable = 1;
        g_osdTitle[s32Title][tmpchn].Param.x = 0;          /*X坐标起始点*/
        g_osdTitle[s32Title][tmpchn].Param.y= ((stPointFont.s32Height * s32Title));/*Y起始点*/
        g_osdTitle[s32Title][tmpchn].Param.width = (stPointFont.s32Width * MAX_OSD_STRLEN);;
        g_osdTitle[s32Title][tmpchn].Param.height = stPointFont.s32Height;
        g_osdTitle[s32Title][tmpchn].Param.fg_color= 0x00000000;
        g_osdTitle[s32Title][tmpchn].Param.bg_color= 0x00FFFFFF;
        AV_PRINT("HI3531_SnapOsdShow titile %d", s32Title);
        HI3521_SnapSetTitle(s32Chn, &(g_osdTitle[s32Title][tmpchn].Param));
    }

    g_osdTime[tmpchn].Param.enable = HI_ENABLE;

    return;
}


