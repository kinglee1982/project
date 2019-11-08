
/******************************************************************************

                  版权所有 (C), 2001-2024, 郑州天迈科技股份有限公司

 ******************************************************************************
  文 件 名   : av_store.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2014年12月1日
  最近修改   :
  功能描述   : 录像
  函数列表   :
  修改历史   :
  1.日    期   : 2014年12月01日
    作    者   :
    修改内容   :

******************************************************************************/
#include "av_public_head.h"
#include "av_common_31.h"
#include "av_store.h"
#include "av_video_proc.h"
#include "av_mem.h"
#include "av_hddfsm.h"
#include "av_video_init.h"
#include "av_audio_init.h"
#include "av_split.h"
#include "av_osd.h"
#include "commctl.h"
#include "memfunc.h"

extern HI_BOOL bAlarmTrigger;
#if 0
HI_BOOL bRecUpdateidx[8] = {HI_FALSE};
#endif
extern EVENT_INFO stEventInfo;
extern HI_U16 Cam_En_Flag;
extern HI_U8 g_iVideoFrameRate;
extern OSD_TITLE_DATA_ST g_stOsdShowData;
#ifdef STORE_MULTI_THD
FILE_IFRAME_LIST_T *g_pstFileIFrameList[APP_VENC_CHNCNT]= {NULL}; /* I帧列表信息 */
FILE_GPS_LIST_ST *g_pstGpsFrameList[APP_VENC_CHNCNT]= {NULL};       /* GPS列表信息，按理来说GPS只一个通道保存即可 */

/*存储相关全局结构 暂时没有将所有的合并进来 */
HDD_PART_ST g_HddPart;
CHN_STOR_MGMT_ST g_stChnStoreMgmt[APP_VENC_CHNCNT];
#endif

#define MEM_1M 1024*1024
#define MEM_128K    128*1024
HI_S32 g_s32RecCacheLength[APP_VENC_CHNCNT] = {2*MEM_1M ,2*MEM_1M + MEM_128K,
                                               2*MEM_1M + 2*MEM_128K,2*MEM_1M + 3*MEM_128K,
                                               3*MEM_1M,3*MEM_1M + MEM_128K,
                                               3*MEM_1M + 2*MEM_128K,3*MEM_1M + 3*MEM_128K,
                                               4*MEM_1M,4*MEM_1M +MEM_128K,
                                               4*MEM_1M + 2*MEM_128K,4*MEM_1M + 3*MEM_128K,
                                              };
/*****************************************************************************
 函 数 名  : STOR_DebugSaveH264
 功能描述  : 测试保留存储函数
 输入参数  : FILE* fpH264File
             char *pstVencStream
             HI_S32 s32Len
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_DebugSaveH264(FILE* fpH264File, char *pstVencStream, HI_S32 s32Len)
{
    if ((fpH264File == NULL) || (pstVencStream == NULL))
    {
        AV_PRINT("HI3531_VencSaveH264 paras is invalid.\n");
        return HI_FAILURE;
    }

    fwrite(pstVencStream,s32Len, 1, fpH264File);
    fflush(fpH264File);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_DebugWRStream
 功能描述  : 测试保留存储函数
 输入参数  : HI_S32 s32Chn
             char *pdata
             HI_S32 s32Len
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_DebugWRStream(HI_S32 s32Chn, char *pdata, HI_S32 s32Len)
{
    HI_S32 s32Ret = HI_SUCCESS;
    time_t nowTime;
    char timeStr[32] = {0};
    static FILE *fp[16] = {NULL};
    HI_CHAR name[64] = {0};

    if (NULL == fp[s32Chn])
    {
        time(&nowTime);
        HI3521_CovTime2Str(nowTime, timeStr);
        sprintf(name, "/mnt/a1/StoreSaveStream%d_%s.h264", s32Chn, timeStr);

        fp[s32Chn] = fopen(name, "wb");
        if (NULL == fp[s32Chn])
        {
            AV_PRINT("fopen %d err[%d:%s].\n", s32Chn, errno, ERR_STR);
        }
    }
    else
    {
        s32Ret = STOR_DebugSaveH264(fp[s32Chn], pdata, s32Len);
    }

    return s32Ret;
}

/*****************************************************************************
 函 数 名  : STOR_GetChnStoreMgmt
 功能描述  : 获取通道全局参数
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  : CHN_STOR_MGMT_ST
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
CHN_STOR_MGMT_ST *STOR_GetChnStoreMgmt(HI_S32 s32Chn)
{
    return &g_stChnStoreMgmt[s32Chn];
}

/*****************************************************************************
 函 数 名  : STOR_GetHddPart
 功能描述  : 获取分区参数
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  : HDD_PART_ST
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HDD_PART_ST *STOR_GetHddPart(HI_VOID)
{
    return &g_HddPart;
}

/*****************************************************************************
 函 数 名  : STOR_IsRecOverWrite
 功能描述  : 获取分区覆盖状态
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年2月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_BOOL STOR_IsRecOverWrite(HI_VOID)
{
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    return pstPart->bOverWrite;
}

/*****************************************************************************
 函 数 名  : STOR_CheckRecAllStartOK
 功能描述  : 检查所有通道录像是否都已经启动
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月17日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_BOOL STOR_CheckRecAllStartOK()
{
    HI_S32 s32Chn = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        if(pstChnStorMgmt->bEnable)
        {
            if(!pstChnStorMgmt->bRecording)
            {
                return HI_FALSE;
            }
        }
    }

    return HI_TRUE;
}
/*****************************************************************************
 函 数 名  : STOR_GetChnRecStartFlag
 功能描述  : 获取开启录像标志
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年04月14日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID STOR_GetChnRecStartFlag()
{
    HI_S32 s32Chn = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        if((Cam_En_Flag&(0x01<<s32Chn)))
        {
            pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
            pstChnStorMgmt->bEnable = HI_TRUE;
        }
        else
        {
            pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
            pstChnStorMgmt->bEnable = HI_FALSE;
        }

    }

    return;
}
/*****************************************************************************
 Prototype    : STOR_SetBlkIdxHead
 Description  : 设置索引文件信息
 Input        : FILE* pFile
                STOR_BLK_IDX_HEAD_T *pstBlkIdxHead
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/8/12
    Author       :
    Modification : Created function

*****************************************************************************/
HI_S32 STOR_SetBlkIdxHead(FILE* pFile, STOR_BLK_IDX_HEAD_T *pstBlkIdxHead)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == pFile) || (NULL == pstBlkIdxHead))
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }

    s32Ret = fseek(pFile, STOR_IDX_BLK_HEAD_SEEK, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fwrite(pstBlkIdxHead, 1, sizeof(STOR_BLK_IDX_HEAD_T), pFile);
    if (s32Len != sizeof(STOR_BLK_IDX_HEAD_T))
    {
        AV_PRINT("write file block head info,s32Len=%d, sizeof=%d", s32Len, sizeof(STOR_BLK_IDX_HEAD_T));
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : STOR_GetBlkIdxHead
 Description  : 设置视频索引文件头部信息
 Input        : FILE* pFile
                STOR_BLK_IDX_HEAD_T *pstBlkIdxHead
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/8/12
    Author       :
    Modification : Created function

*****************************************************************************/
HI_S32 STOR_GetBlkIdxHead(FILE* pFile, STOR_BLK_IDX_HEAD_T *pstBlkIdxHead)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pFile) || (NULL == pstBlkIdxHead))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

    s32Ret = fseek(pFile, STOR_IDX_BLK_HEAD_SEEK, SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("lseek recode config err!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    s32Len = fread(pstBlkIdxHead, 1, sizeof(STOR_BLK_IDX_HEAD_T), pFile);
    if(s32Len != sizeof(STOR_BLK_IDX_HEAD_T))
    {
        AV_PRINT("read file block head info");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SetVideoFileHeadInfo
 功能描述  : 设置视频块头部信息
 输入参数  : FILE* pFile
             STOR_FILE_BLK_HEAD_T *pstBlkIdxHead
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetVideoFileHeadInfo(FILE* pFile, STOR_FILE_BLK_HEAD_T *pstFileBlkHead, HI_U32 u32Addr)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pFile) || (NULL == pstFileBlkHead))
    {
        AV_PRINT("input para invalid.");
        return HI_FAILURE;
    }

    s32Len = fseek(pFile, u32Addr, SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR)
        return HI_FAILURE;
    }

    s32Len = fwrite(pstFileBlkHead, 1, sizeof(STOR_FILE_BLK_HEAD_T), pFile);
    if(s32Len != sizeof(STOR_FILE_BLK_HEAD_T))
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    fflush(pFile);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_GetVideoFileHeadInfo
 功能描述  : 获取视频块文件信息头部信息
 输入参数  : FILE* pFile
             STOR_FILE_BLK_HEAD_T *pstBlkIdxHead
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月21日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetVideoFileHeadInfo(FILE* pFile, STOR_FILE_BLK_HEAD_T *pstBlkIdxHead, HI_U32 u32Addr)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pFile) || (NULL == pstBlkIdxHead))
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }

    s32Ret = fseek(pFile, u32Addr, SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("lseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fread(pstBlkIdxHead, 1, sizeof(STOR_FILE_BLK_HEAD_T), pFile);
    if(s32Len != sizeof(STOR_FILE_BLK_HEAD_T))
    {
        AV_PRINT("fread err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    AV_PRINT("lfrm_offset(%u) finfo_spos(%u) frm_spos(%u) frm_epos(%u) frm_idx_spos(%d) frm_idx_size(%u) frm_stime(%u)",
             pstBlkIdxHead->last_frame_ts_offset,pstBlkIdxHead->file_info_start_pos,
             pstBlkIdxHead->frame_data_start_pos,pstBlkIdxHead->frame_data_end_pos,
             pstBlkIdxHead->frame_index_start_pos,pstBlkIdxHead->frame_index_size,
             pstBlkIdxHead->reserve);

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : STOR_GetVideoFileInfo
 功能描述  : 设置视频块文件信息
 输入参数  : FILE* pFile
             STOR_FILE_INFO_T *pstFileInfo
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月8日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetVideoFileInfo(FILE* pFile, STOR_FILE_INFO_T *pstFileInfo)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pFile) || (NULL == pstFileInfo))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

    s32Ret = fseek(pFile, STOR_FILE_INFO_START_ADDR +sizeof(FRAME_HEAD_T), SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("lseek recode config err!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    s32Len = fread(pstFileInfo, 1, sizeof(STOR_FILE_INFO_T), pFile);
    if(s32Len != sizeof(STOR_FILE_INFO_T))
    {
        AV_PRINT("STOR_GetVideoFileInfo");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SetVideoFileInfo
 功能描述  : 设置视频块文件信息
 输入参数  : FILE* pFile
             STOR_FILE_INFO_T *pstFileInfo
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月8日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetVideoFileInfo(FILE* pFile, STOR_FILE_INFO_T *pstFileInfo)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pFile) || (NULL == pstFileInfo))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }
    /*其实存放在STOR_FILE_INFO_START_ADDR +sizeof(FRAME_HEAD_T)*/
    s32Ret = fseek(pFile, STOR_FILE_INFO_START_ADDR +sizeof(FRAME_HEAD_T), SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("lseek recode config err!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    s32Len = fwrite(pstFileInfo, 1, sizeof(STOR_FILE_INFO_T), pFile);
    if(s32Len != sizeof(STOR_FILE_INFO_T))
    {
        AV_PRINT("write error (%s)",ERR_STR);
        return HI_FAILURE;
    }


    fflush(pFile);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_ChkIdxFileVer
 功能描述  : 录像索引文件版本检测
 输入参数  : FILE *pIdxBlkFp
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月8日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_ChkIdxFileVer(FILE *pIdxBlkFp)
{
    STOR_FILE_BLK_HEAD_T stFileHead;
    HI_S32 s32Ret = 0;
    FILE *pIdxBlkFpTmp = NULL;
    HI_CHAR cIdxPath[64] = {0};
    HI_U32 u32PartId = 0;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }

    /*从索引文件中将头部信息读取出来，主要用来判断版本信息 */
    s32Ret = STOR_GetVideoFileHeadInfo(pIdxBlkFp, &stFileHead, STOR_IDX_BLK_VER_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetVideoFileHeadInfo failed!");
        return HI_FAILURE;
    }

    /*这里有风险 ，如果索引出现问题，整个录像文件的索引都会搞没 录像是不是全部丢失乱套 */
    if (STOR_IDX_VER != stFileHead.version)
    {
        for (u32PartId = 0 ; u32PartId < PART_MAX_NUMS; u32PartId++)
        {
            /* 切换分区，重新打开新的索引文件 */
            memset(cIdxPath, 0, sizeof(cIdxPath));
            GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(u32PartId));
            pIdxBlkFpTmp = fopen(cIdxPath, "r+");
            if (NULL == pIdxBlkFpTmp)
            {
                AV_PRINT("fopen %s err(%s)", cIdxPath, ERR_STR);
                return HI_FAILURE;
            }

            /* 格式化索引文件 */
            s32Ret = STOR_InitIdxBlk(pIdxBlkFpTmp);
            if (HI_SUCCESS != s32Ret)
            {
                fclose(pIdxBlkFpTmp);
                pIdxBlkFpTmp = NULL;
                AV_PRINT("STOR_InitIdxBlk error! ");
                return HI_FAILURE;
            }

            fclose(pIdxBlkFpTmp);
            pIdxBlkFpTmp = NULL;
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SaveRecPartId
 功能描述  : 设置正常使用的录像分区
 输入参数  : HI_S32 iPart
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月24日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SaveRecPartId(HI_S32 s32CurPart)
{
    FILE *pRecLogFp = NULL;
    HI_S32 s32Ret = 0;
    STOR_PARAM_ST stStoreParam;
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    stStoreParam.bOverWrite = pstPart->bOverWrite;
    stStoreParam.iPartId = s32CurPart;

    if (!IS_PART_ID_VAILD(stStoreParam.iPartId))
    {
        AV_PRINT("rec part id out of range, 32CurPart = %d", stStoreParam.iPartId);
        stStoreParam.iPartId = 0;
    }

    pRecLogFp = fopen(APP_REC_ERR_LOG, "r+");
    if (NULL == pRecLogFp)
    {
        AV_PRINT("fopen %s err(%s)", APP_REC_ERR_LOG, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fseek(pRecLogFp, 0, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("lseek recode config err!s32Ret=%d",s32Ret);
        fflush(pRecLogFp);
        fclose(pRecLogFp);
        return HI_FAILURE;
    }

    s32Ret=fwrite(&stStoreParam, sizeof(STOR_PARAM_ST), 1, pRecLogFp);
    if (s32Ret < 0)
    {
        AV_PRINT("write recode log config err!");
        fflush(pRecLogFp);
        fclose(pRecLogFp);
        return HI_FAILURE;
    }

    fflush(pRecLogFp);
    fclose(pRecLogFp);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SaveRecOverWriteFlag
 功能描述  : 保存覆盖标志
 输入参数  : bool bOverWrite
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年2月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SaveRecOverWriteFlag(HI_BOOL bOverWrite)
{
    FILE *pRecLogFp = NULL;
    HI_S32 s32Ret = 0;
    STOR_PARAM_ST stStoreParam;
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    stStoreParam.bOverWrite = bOverWrite;
    stStoreParam.iPartId = pstPart->iPartId;

    pRecLogFp = fopen(APP_REC_ERR_LOG, "r+");
    if (NULL == pRecLogFp)
    {
        AV_PRINT("fopen %s err(%s)", APP_REC_ERR_LOG, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fseek(pRecLogFp, 0, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("lseek recode config err!s32Ret=%d",s32Ret);
        fflush(pRecLogFp);
        fclose(pRecLogFp);
        return HI_FAILURE;
    }

    s32Ret=fwrite(&stStoreParam, sizeof(STOR_PARAM_ST), 1, pRecLogFp);
    if (s32Ret < 0)
    {
        AV_PRINT("write recode log config err!");
        fflush(pRecLogFp);
        fclose(pRecLogFp);
        return HI_FAILURE;
    }

    fflush(pRecLogFp);
    fclose(pRecLogFp);
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : STOR_CreateRecLogFile
 功能描述  :
 输入参数  :
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_CreateRecLogFile(HI_VOID)
{
    HI_S32 s32Ret = 0;
    FILE *recLogFp = NULL;
    STOR_PARAM_ST stStoreParam;
    STOR_REC_LOG_T *pstRecLog = NULL;

    s32Ret = access(APP_REC_ERR_LOG, F_OK);
    if (0 != s32Ret)
    {
        recLogFp = fopen(APP_REC_ERR_LOG, "w");
        if (NULL == recLogFp)
        {
            AV_PRINT("open %s err(%s)", APP_REC_ERR_LOG, ERR_STR);
            return HI_FAILURE;
        }

        memset(&stStoreParam, 0, sizeof(STOR_PARAM_ST));
        s32Ret = fwrite(&stStoreParam, sizeof(STOR_PARAM_ST), 1, recLogFp);
        if (s32Ret < 0)
        {
            AV_PRINT("write %s err(%s)", APP_REC_ERR_LOG, ERR_STR);
            fclose(recLogFp);
            return HI_FAILURE;
        }

        pstRecLog = (STOR_REC_LOG_T *)malloc(sizeof(STOR_REC_LOG_T));
        if (NULL == pstRecLog)
        {
            AV_PRINT("malloc memory err(%s)", ERR_STR);
            fclose(recLogFp);
            return HI_FAILURE;
        }

        s32Ret = fwrite(pstRecLog, sizeof(STOR_REC_LOG_T), 1, recLogFp);
        if (s32Ret < 0)
        {
            AV_PRINT("write err(%s)", ERR_STR);
            fclose(recLogFp);
            free(pstRecLog);
            pstRecLog = NULL;
            return HI_FAILURE;
        }

        free(pstRecLog);
        pstRecLog = NULL;
        fflush(recLogFp);
        fclose(recLogFp);
    }

    return HI_SUCCESS;
}

#ifdef STORE_MULTI_THD
/*****************************************************************************
 函 数 名  : STOR_GetRecPartId
 功能描述  : 获取录像分区ID
 输入参数  : HI_S32 *pS32PartId
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月24日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetRecPartId(HI_VOID)
{
    HI_S32 s32Ret = 0;
    FILE *recLogFp = NULL;
    HI_S32 s32CurPart = 0;
    STOR_PARAM_ST stStoreParam;
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    s32Ret = STOR_CreateRecLogFile();
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    recLogFp = fopen(APP_REC_ERR_LOG, "r+");
    if (NULL == recLogFp)
    {
        AV_PRINT("open %s err(%s)", APP_REC_ERR_LOG, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fseek(recLogFp, 0, SEEK_SET);
    if (s32Ret < 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        fclose(recLogFp);
        return HI_FAILURE;
    }

    s32Ret = fread(&stStoreParam, 1, sizeof(STOR_PARAM_ST), recLogFp);
    if (s32Ret < 0)
    {
        AV_PRINT("fread err(%s)", ERR_STR);
        fclose(recLogFp);
        return HI_FAILURE;
    }

    s32CurPart = stStoreParam.iPartId;
    if (s32CurPart > MAX_PART_ID || s32CurPart < MIN_PART_ID)
    {
        s32CurPart = 0;
        s32Ret = fwrite(&stStoreParam, sizeof(STOR_PARAM_ST), 1, recLogFp);
        if (s32Ret < 0)
        {
            AV_PRINT("write err(%s)", ERR_STR);
            fclose(recLogFp);
            return HI_FAILURE;
        }
    }

    pstPart->iPartId = s32CurPart;
    pstPart->bOverWrite = stStoreParam.bOverWrite;

    if (!IS_PART_ID_VAILD(pstPart->iPartId))
    {
        AV_PRINT("rec part id out of range, 32CurPart = %d", pstPart->iPartId);
        pstPart->iPartId = 0;
    }

    printf("::::::STOR_GetRecPartId PartId(%d) bOverWrite(%d)\n", pstPart->iPartId, pstPart->bOverWrite);
    fflush(recLogFp);
    fclose(recLogFp);

    return HI_SUCCESS;
}

HI_S32 STOR_OnePartFileNum(HI_VOID)
{
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    return (pstPart->partFileNums);
}

#endif
HI_U32 STOR_CalTime1(STOR_REC_IFRAME_INFO *pstIFRAMEInfo)
{
    return ((pstIFRAMEInfo->nmk_time4 <<24) | (pstIFRAMEInfo->nmk_time3 <<16)
            | (pstIFRAMEInfo->nmk_time2 <<8) | (pstIFRAMEInfo->nmk_time1));
}

HI_U32 STOR_CalTime2(FRAME_HEAD_T *pstFrameHead)
{
    return ((pstFrameHead->nmk_time4 <<24) | (pstFrameHead->nmk_time3 <<16)
            | (pstFrameHead->nmk_time2 <<8) | (pstFrameHead->nmk_time1));
}
HI_U32 STOR_CalTime3(STOR_GPS_INFO_ST *pstGpsInfo)
{
    return ((pstGpsInfo->nmk_time4 <<24) | (pstGpsInfo->nmk_time3 <<16)
            | (pstGpsInfo->nmk_time2 <<8) | (pstGpsInfo->nmk_time1));
}

/*****************************************************************************
 函 数 名  : STOR_GetIFrameSeek
 功能描述  : 获取录像文件该时间段之后的I帧地址
 输入参数  : FILE* pFile
             HI_U32 *pU32StartAddr
             HI_U32 uiSecond
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2013年10月30日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetIFrameSeek(FILE* pFile, HI_U32 *pU32FrameSeek, time_t tFrameStartTime)
{
    HI_U32 u32IFrameTime = 0;
    HI_S32 s32IFrameCnt = 0;
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32FrameCnt = 0;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

    if ((NULL == pFile) || (NULL == pU32FrameSeek))
    {
        AV_PRINT("Input para invalid.");
        goto ERR_LABLE;
    }

    pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        goto ERR_LABLE;
    }

    s32Ret = STOR_GetFileIFrameList(pFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetFileIFrameList failed.");
        goto ERR_LABLE;
    }

    s32FrameCnt = pstIFrameList->s32Counter;

    AV_PRINT("s32FrameCnt(%d)", s32FrameCnt);
    if (s32FrameCnt > STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32FrameCnt(%d), idxNew(%d), idxOld(%d)",
                 s32FrameCnt, pstIFrameList->s32NewestIndex, pstIFrameList->s32OldestIndex);
        goto ERR_LABLE;
    }


    for (s32IFrameCnt = pstIFrameList->s32OldestIndex; s32IFrameCnt < s32FrameCnt; s32IFrameCnt++)
    {
        pstIFRAMEInfo = &(pstIFrameList->stRecIFrameInfo[s32IFrameCnt]);
        u32IFrameTime = STOR_CalTime1(pstIFRAMEInfo);
        //	AV_PRINT("s32IFrameCnt=%d, u32IFrameTime=%d, tFrameStartTime=%d", s32IFrameCnt, u32IFrameTime, (HI_U32)tFrameStartTime);
        if ((u32IFrameTime >= (HI_U32)tFrameStartTime) && (u32IFrameTime < (HI_U32)(tFrameStartTime + 3600*24)))
        {
            *pU32FrameSeek = pstIFRAMEInfo->frame_data_pos;
            AV_PRINT("*pU32FrameSeek=%d, u32IFrameTime=%d, tFrameStartTime=%d", *pU32FrameSeek, u32IFrameTime, (HI_U32)tFrameStartTime);
            s32Ret = HI_SUCCESS;
            goto ERR_LABLE;
        }
        else
        {
            s32Ret = HI_FAILURE;
            //		AV_PRINT("u32IFrameTime(%d), tFrameStartTime(%d)", u32IFrameTime, (HI_U32)tFrameStartTime);
        }
    }

ERR_LABLE:
    if (NULL != pstIFrameList)
    {
        free(pstIFrameList);
        pstIFrameList = NULL;
    }
    return s32Ret;
}

#ifdef STORE_MULTI_THD
HI_VOID STOR_GetOnePartBlkNum(HI_CHAR *pcPartName)
{
    struct statfs diskInfo;
    unsigned long long blocksize;
    unsigned long long totalsize;
    HI_U32  uS32BlockNum = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
#ifdef HDD_SIZE_CHECK
    HI_U32  storflie_maxsize = 0;
    if(pstPart ->hddinfo)
        storflie_maxsize = 512*1024*1024;
    else
        storflie_maxsize = 256*1024*1024;
#endif
    statfs(pcPartName, &diskInfo);

    blocksize = diskInfo.f_bsize;	//每个block里包含的字节数
    totalsize = blocksize * diskInfo.f_blocks; 	//总的字节数，f_blocks为block的数目
#ifdef HDD_SIZE_CHECK
    uS32BlockNum = (totalsize) / storflie_maxsize;
#else
    uS32BlockNum = (totalsize) / STOR_FILE_MAX_SIZE;
#endif
    uS32BlockNum = uS32BlockNum - 5;   /*拥有的文件个数做一个保护 */
    /*测试*/
#ifndef STORE_DEBUG
    uS32BlockNum = 51;
#endif
    AV_PRINT( "STOR_GetOnePartBlkNum = %d", uS32BlockNum);
    pstPart->partFileNums = uS32BlockNum;

    return;
}

#endif
/*****************************************************************************
 函 数 名  : STOR_GetBlkIdxByChn
 功能描述  : 获取每个通道的使用的块索引
 输入参数  : FILE *pIdxBlkFp
             STOR_CHN_IDX_INFO_T *pstChnIdxInfo
             HI_S32 iChn
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetBlkIdxByChn(FILE *pIdxBlkFp, STOR_CHN_IDX_INFO_T *pstChnIdxInfo, HI_S32 s32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("input para null, pIdxBlkFp");
        return HI_FAILURE;
    }

    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("input para invalid, pstChnIdxInfo");
    }

    if (s32Chn >= APP_VENC_CHNCNT)
    {
        AV_PRINT("input para invalid, chn = %d", s32Chn);
        return HI_FAILURE;
    }

    s32Ret = fseek(pIdxBlkFp, STOR_IDX_BLK_CHN_SEEK(s32Chn), SEEK_SET);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fread(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pIdxBlkFp);
    if (s32Ret < 0)
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SetBlkIdxByChn
 功能描述  : 设置每个通道的使用的块索引
 输入参数  : FILE *pRecLogFp
             STOR_CHN_IDX_INFO_T *pstChnIdxInfo
             HI_S32 iChn
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetBlkIdxByChn(FILE *pIdxBlkFp, STOR_CHN_IDX_INFO_T *pstChnIdxInfo, HI_S32 s32Chn)
{
    HI_S32 s32Ret = 0;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("input para invalid");
        return HI_FAILURE;
    }

    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("input para invalid");
        return HI_FAILURE;
    }

    if (s32Chn >= APP_VENC_CHNCNT)
    {
        AV_PRINT("input para invalid, chn = %d", s32Chn);
        return HI_FAILURE;
    }

    s32Ret = fseek(pIdxBlkFp, STOR_IDX_BLK_CHN_SEEK(s32Chn), SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fwrite(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1 ,pIdxBlkFp);
    if(s32Ret < 0)
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    fflush(pIdxBlkFp);
    return HI_SUCCESS;
}

HI_VOID STOR_DebugIdxEntry(FILE_IFRAME_LIST_T *pstIFrameList)
{

    HI_S32 i = 0;
    AV_PRINT("file iframe list======================================print");
    AV_PRINT("cnt(%d) newIdx(%d) oldIdx(%d)",
             pstIFrameList->s32Counter,
             pstIFrameList->s32NewestIndex,
             pstIFrameList->s32OldestIndex);
    if (pstIFrameList == NULL)
    {
        return;
    }

    for (i = 0; i < STOR_FILE_IFRAME_MAX_NUMS; i++)
    {
        AV_PRINT("type(%d) offset(%u) intl(%u) frm_pos(%u) frm_time(%u)",
                 pstIFrameList->stRecIFrameInfo[i].type,
                 pstIFrameList->stRecIFrameInfo[i].ts_offset,
                 pstIFrameList->stRecIFrameInfo[i].ts_interval,
                 pstIFrameList->stRecIFrameInfo[i].frame_data_pos,
                 STOR_CalTime1(&pstIFrameList->stRecIFrameInfo[i]));

    }

    return;
}


HI_VOID STOR_DebugFileBlkHead(STOR_FILE_BLK_HEAD_T *pstFileHead)
{
    AV_PRINT("file blk head======================================print");
    AV_PRINT("lfrm_offset(%u) finfo_spos(%u) frm_spos(%u) frm_epos(%u) frm_idx_spos(%d) frm_idx_size(%u) frm_stime(%u)",
             pstFileHead->last_frame_ts_offset,pstFileHead->file_info_start_pos,
             pstFileHead->frame_data_start_pos,pstFileHead->frame_data_end_pos,
             pstFileHead->frame_index_start_pos,pstFileHead->frame_index_size,
             pstFileHead->reserve);
    return;
}

#ifdef ALARM_REC
HI_S32 STOR_UpdateEventRecFileInfo(FILE *pFile, EVENT_REC_TYPE_EM eEventType)
{
    HI_S32 s32Ret = HI_FAILURE;
    STOR_FILE_INFO_T stVideoFileInfo;

    memset(&stVideoFileInfo, 0, sizeof(STOR_FILE_INFO_T));
    STOR_GetVideoFileInfo(pFile, &stVideoFileInfo);

    stVideoFileInfo.stRecEventInfo[eEventType].s32Trigger = 1;
    stVideoFileInfo.stRecEventInfo[eEventType].s32StartTime = stEventInfo.s32StartTime;
    stVideoFileInfo.stRecEventInfo[eEventType].s32EndTime = stEventInfo.s32EndTime;

    s32Ret = STOR_SetVideoFileInfo(pFile, &stVideoFileInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoFileInfo FAILED");
        return HI_FAILURE;
    }

    fflush(pFile);
    return HI_SUCCESS;
}

HI_S32 STOR_UpdateEventRecBkFlag(FILE *pFile, EVENT_REC_TYPE_EM eEventType)
{
    HI_S32 s32Ret = HI_FAILURE;
    STOR_FILE_INFO_T stVideoFileInfo;

    memset(&stVideoFileInfo, 0, sizeof(STOR_FILE_INFO_T));
    STOR_GetVideoFileInfo(pFile, &stVideoFileInfo);

    /*报警录像事件触发了则把备份标志进行标志 */
    if (stVideoFileInfo.stRecEventInfo[eEventType].s32Trigger == 1)
    {
        stVideoFileInfo.stRecEventInfo[eEventType].s32BkOK = 1;
        AV_PRINT("STOR_UpdateEventRecBkFlag success");
        s32Ret = STOR_SetVideoFileInfo(pFile, &stVideoFileInfo);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_SetVideoFileInfo error");
            return s32Ret;
        }
    }

    STOR_GetVideoFileInfo(pFile, &stVideoFileInfo);
    AV_PRINT("STOR_GetVideoFileInfo %d", stVideoFileInfo.stRecEventInfo[eEventType].s32BkOK);
    fflush(pFile);
    return s32Ret;
}

HI_S32 STOR_UpdateEventIdxHead(FILE *pIdxBlkFp, HI_S32 s32Chn)
{
    HI_S32 s32Ret = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = STOR_GetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetBlkIdxByChn err! chn=%d", s32Chn);
        return HI_FAILURE;
    }


    pstBlkIdxEntry = &(pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32NewestIndex]);

    pstBlkIdxEntry->lock_flag = 1;
    s32Ret = STOR_SetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxByChn err! chn=%d", s32Chn);
        return HI_FAILURE;
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;

}

#endif

/*****************************************************************************
 函 数 名  : STOR_UpdateBlockFileInfo
 功能描述  : 更新视频块文件信息
 输入参数  : FILE *pRecLogFp
             FILE_IFRAME_LIST_T *pstIFrameList
             STOR_FILE_BLK_HEAD_T *pstFileHead
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月1日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
#ifdef REC_SAVE_GPS
HI_S32 STOR_UpdateBlockFileInfo(FILE *pRecFileFp, FILE_IFRAME_LIST_T *pstIFrameList,
                                FILE_GPS_LIST_ST *pstGpsList,STOR_FILE_BLK_HEAD_T *pstFileHead)
#else
HI_S32 STOR_UpdateBlockFileInfo(FILE *pRecFileFp, FILE_IFRAME_LIST_T *pstIFrameList,
                                STOR_FILE_BLK_HEAD_T *pstFileHead)
#endif
{
    HI_S32 s32Ret = 0;
    time_t nowTime = 0;
    HI_U32 timeInterval = 0;

    if (NULL == pRecFileFp)
    {
        AV_PRINT("pRecFileFp para invalid.");
        return HI_FAILURE;
    }

    if (NULL == pstIFrameList)
    {
        AV_PRINT("pstIFrameList para invalid.");
        return HI_FAILURE;
    }

    if (NULL == pstFileHead)
    {
        AV_PRINT("pstFileHead para invalid.");
        return HI_FAILURE;
    }

    nowTime = time((time_t *)NULL);
    timeInterval = nowTime - pstFileHead->reserve;
    pstFileHead->last_frame_ts_offset = timeInterval * 1000;

#ifndef STORE_DEBUG
    STOR_DebugIdxEntry(pstIFrameList);
#endif

    s32Ret = STOR_SetFileIFrameList(pRecFileFp, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetFileIFrameList err!");
        return HI_FAILURE;
    }

#ifdef REC_SAVE_GPS
    s32Ret = STOR_SetGpsInfoList(pRecFileFp, pstGpsList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetGpsInfoList err!");
        return HI_FAILURE;
    }
#endif

#ifndef STORE_DEBUG
    STOR_DebugFileBlkHead(pstFileHead);
#endif


    s32Ret = STOR_SetVideoFileHeadInfo(pRecFileFp, pstFileHead, STOR_IDX_BLK_HEAD_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoFileHeadInfo err!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 STOR_GetRecFileIFrameTime(FILE_IFRAME_LIST_T *pstIFrameList, time_t *startTime, time_t *endTime)
{
    HI_S32 s32NewIndex = 0;
    HI_S32 s32OldIndex = 0;
    STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

    s32NewIndex = pstIFrameList->s32NewestIndex;
    s32OldIndex = pstIFrameList->s32OldestIndex;

    if (s32NewIndex >= STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32Counter=%d,s32NewIndex=%d", pstIFrameList->s32Counter, s32NewIndex);
        return HI_FAILURE;
    }

    pstIFRAMEInfo = &pstIFrameList->stRecIFrameInfo[s32OldIndex];

    *startTime = ((pstIFRAMEInfo->nmk_time4 <<24) | (pstIFRAMEInfo->nmk_time3 <<16)
                  | (pstIFRAMEInfo->nmk_time2 <<8) | pstIFRAMEInfo->nmk_time1);


    pstIFRAMEInfo = &pstIFrameList->stRecIFrameInfo[s32NewIndex - 1];

    *endTime = ((pstIFRAMEInfo->nmk_time4 <<24) | (pstIFRAMEInfo->nmk_time3 <<16)
                | (pstIFRAMEInfo->nmk_time2 <<8) | pstIFRAMEInfo->nmk_time1);
#ifndef STORE_DEBUG
//   HI3531_ConvertTime2LocaltimeStr2((HI_CHAR*)"IFRAME", *startTime, *endTime);
#endif
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_UpdateChnListEndTime
 功能描述  : 更新视频索引文件文件结束时间
 输入参数  :
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月26日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_UpdateChnListEndTime(FILE *pIdxBlkFp, FILE_IFRAME_LIST_T * pstIFrameList,HI_U32 u32Chn, HI_U32 u32DataLen)
{
    HI_S32 s32Ret = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;
    time_t startTime;
    time_t endTime;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("pIdxBlkFp para invalid.");
        return HI_FAILURE;
    }

    if (NULL == pstIFrameList)
    {
        AV_PRINT("pstIFrameList para invalid.");
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = STOR_GetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, u32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetBlkIdxByChn err! chn=%d", u32Chn);
        goto ERR_LABLE;
    }


    pstBlkIdxEntry = &(pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32NewestIndex]);
    pstBlkIdxEntry->end_time = time((time_t *)NULL);

    /*根据I帧时间来更新最早和最新的结束时间 */
    s32Ret =  STOR_GetRecFileIFrameTime(pstIFrameList, &startTime, &endTime);
    if (HI_FAILURE != s32Ret)
    {
        pstBlkIdxEntry->end_time = endTime;
        pstBlkIdxEntry->start_time = startTime;
    }

    pstBlkIdxEntry->size = u32DataLen;
#ifndef STORE_DEBUG
    AV_PRINT("start_time=%u,end_time=%u,CHN=%d,s32NewestIndex =%d",
             pstBlkIdxEntry->start_time,
             pstBlkIdxEntry->end_time,
             u32Chn,
             pstChnIdxInfo->s32NewestIndex);


    AV_PRINT("s32NewestIndex=%u,s32OldestIndex=%u,s32Counter=%u,CHN=%d",
             pstChnIdxInfo->s32NewestIndex,
             pstChnIdxInfo->s32OldestIndex,
             pstChnIdxInfo->s32Counter,
             u32Chn);
#endif
    s32Ret = STOR_SetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, u32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxByChn err! chn=%d", u32Chn);
        goto ERR_LABLE;
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;

ERR_LABLE:
    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    fclose(pIdxBlkFp);
    pIdxBlkFp = NULL;
    return HI_FAILURE;

}

/*****************************************************************************
 函 数 名  : STOR_GetVideoBlkStat
 功能描述  : 获取视频块列表信息
 输入参数  : FILE *pRecLogFp
             STOR_BLK_STAT_INFO_T *pstBlkStatInfo
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月26日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetVideoBlkStat(FILE *pIdxBlkFp, STOR_BLK_STAT_INFO_T *pstBlkStatInfo)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32BlkMax = 0;

    if ((NULL == pIdxBlkFp) || (NULL == pstBlkStatInfo))
    {
        AV_PRINT("input para invalid.");
        return HI_FAILURE;
    }

    s32Ret = fseek(pIdxBlkFp, STOR_IDX_BLK_STAT_INFO_SEEK, SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fread(pstBlkStatInfo, sizeof(STOR_BLK_STAT_INFO_T), 1, pIdxBlkFp);
    if(s32Ret < 0)
    {
        AV_PRINT("fread err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    u32BlkMax = STOR_OnePartFileNum();

    if ((pstBlkStatInfo->s32BlkOldestIndex >= u32BlkMax) ||
            (pstBlkStatInfo->s32BlkNewestIndex >= u32BlkMax) ||
            (pstBlkStatInfo->s32BlkCounter > u32BlkMax))
    {
        AV_PRINT("Storage block error! s32OldestIndex=%u,s32NewestIndex=%u,s32Counter=%u",
                 pstBlkStatInfo->s32BlkOldestIndex,
                 pstBlkStatInfo->s32BlkNewestIndex,
                 pstBlkStatInfo->s32BlkCounter);
        pstBlkStatInfo->s32BlkOldestIndex = 0;
        pstBlkStatInfo->s32BlkNewestIndex = 0;
        pstBlkStatInfo->s32BlkCounter = 0;

        /* 初始化视频索引文件 */
        s32Ret = STOR_InitIdxBlk(pIdxBlkFp);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_InitIdxBlk error! ");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SetVideoBlkStat
 功能描述  : 设置视频块列表信息
 输入参数  : FILE *pRecLogFp
             STOR_BLK_STAT_INFO_T *pstBlkStatInfo
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月26日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetVideoBlkStat(FILE *pIdxBlkFp, STOR_BLK_STAT_INFO_T *pstBlkStatInfo)
{
    HI_S32 s32Ret = 0;

    if ((NULL == pIdxBlkFp) || (NULL == pstBlkStatInfo))
    {
        AV_PRINT("input para invalid.");
        return HI_FAILURE;
    }

    s32Ret = fseek(pIdxBlkFp, STOR_IDX_BLK_STAT_INFO_SEEK, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fwrite(pstBlkStatInfo, sizeof(STOR_BLK_STAT_INFO_T), 1, pIdxBlkFp);
    if (s32Ret < 0)
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : STOR_GetDiskNewestOldestTime
 功能描述  : 获取指定通道最老的和最新的视频块起始时间
 输入参数  : HI_CHAR *pcIndexName
             HI_S32 iChn
             time_t *pNewestTime
             time_t *pOldestTime
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月16日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetDiskNewestOldestTime(HI_CHAR *pcIndexName, HI_S32 iChn,
                                    time_t *pNewestTime, time_t *pOldestTime)
{
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    FILE* file = NULL;
    HI_S32 s32Ret = 0;

    if ((NULL == pcIndexName) || (NULL == pNewestTime) || (NULL == pOldestTime))
    {
        AV_PRINT("input para invalid");
        return HI_FAILURE;
    }

    file = fopen(pcIndexName, "r+");
    if (NULL == file)
    {
        AV_PRINT("cFileName(%s) err(%s)", pcIndexName, ERR_STR);
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc memory err(%s)", ERR_STR);
        if (NULL != file)
        {
            fclose(file);
        }
        return HI_FAILURE;
    }

    s32Ret = STOR_GetBlkIdxByChn(file, pstChnIdxInfo, iChn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetBlkIdxByChn err! CHN(%d)",iChn);
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;

        if (NULL != file)
        {
            fclose(file);
        }
        return HI_FAILURE;
    }

    if (pstChnIdxInfo->s32Counter == 0)
    {
        if (NULL != file)
        {
            fclose(file);
        }

        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
        *pNewestTime = 0;
        *pOldestTime = 0;

        return HI_SUCCESS;
    }

    AV_PRINT("@@@@s32NewestIndex(%d),s32OldestIndex(%d)", pstChnIdxInfo->s32NewestIndex,pstChnIdxInfo->s32OldestIndex);
    *pNewestTime = pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32NewestIndex].end_time;
    *pOldestTime = pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32OldestIndex].start_time;

    if (NULL != file)
    {
        fclose(file);
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_InitIdxBlk
 功能描述  : 初始化索引文件
 输入参数  : FILE *pIdxBlkFp
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月24日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InitIdxBlk(FILE *pIdxBlkFp)
{
    HI_S32 s32Ret = 0;
    STOR_BLK_STAT_INFO_T stBlkStatInfo;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    HI_S32 i = 0;
    STOR_FILE_BLK_HEAD_T stFileHead;
    STOR_BLK_IDX_HEAD_T stBlkIdxHead;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }

    memset(&stBlkIdxHead, 0, sizeof(STOR_BLK_IDX_HEAD_T));
    stBlkIdxHead.en_chns = APP_VENC_CHNCNT;
    stBlkIdxHead.all_blocks = STOR_OnePartFileNum();

    s32Ret = STOR_SetBlkIdxHead(pIdxBlkFp, &stBlkIdxHead);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxHead error! ");
        return HI_FAILURE;
    }

    memset(&stFileHead, 0, sizeof(stFileHead));
    stFileHead.version = STOR_IDX_VER;
    s32Ret = STOR_SetVideoFileHeadInfo(pIdxBlkFp, &stFileHead, STOR_IDX_BLK_VER_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoFileHeadInfo error! ");
        return HI_FAILURE;
    }

    memset(&stBlkStatInfo, 0, sizeof(stBlkStatInfo));
    s32Ret = STOR_SetVideoBlkStat(pIdxBlkFp, &stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error! ");
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    memset(pstChnIdxInfo, 0, sizeof(STOR_CHN_IDX_INFO_T));

    for (i = 0; i < APP_VENC_CHNCNT; i++)
    {
        s32Ret = STOR_SetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_SetBlkIdxByChn error! chn=%d", i);
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_AddNewBlockID
 功能描述  : 添加一个新的视频块到视频索引文件
 输入参数  : HI_U32 uiBlockID
             FILE *pRecLogFp
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_AddNewBlockID(HI_U32 uiBlockID, FILE *pRecLogFp, HI_U32 iCHn,time_t tStartTime)
{
    HI_S32 s32Ret = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntryEntry = NULL;
    HI_U32 u32StartAddr = 0;
    HI_U32 uiNewBlockIndex = 0;
    time_t ulSecond = 0;
    struct tm *tmp;
    HI_CHAR time_buf[32] = {0};

    if (NULL == pRecLogFp)
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    u32StartAddr = STOR_IDX_BLK_CHN_SEEK(iCHn);
    s32Ret = fseek(pRecLogFp,u32StartAddr,SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("fseek recode config err(%s)", ERR_STR);
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
        return HI_FAILURE;
    }

    s32Ret = fread(pstChnIdxInfo, 1, sizeof(STOR_CHN_IDX_INFO_T), pRecLogFp);
    if(s32Ret<0)
    {
        AV_PRINT("fread stor chn idx info err(%s)", ERR_STR);
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
        return HI_FAILURE;
    }

    pstBlkIdxEntryEntry = pstChnIdxInfo->stBlkIdxList;
    if (0 == pstChnIdxInfo->s32Counter)
    {
        uiNewBlockIndex = (pstChnIdxInfo->s32NewestIndex + 0)%STOR_RECFILE_MAX_NUMS;
    }
    else
    {
        uiNewBlockIndex = (pstChnIdxInfo->s32NewestIndex + 1)%STOR_RECFILE_MAX_NUMS;
    }
    pstBlkIdxEntryEntry = pstBlkIdxEntryEntry + uiNewBlockIndex;
    pstChnIdxInfo->s32Counter++;


    pstBlkIdxEntryEntry->blockid = uiBlockID;
    pstBlkIdxEntryEntry->chn = iCHn;
    if(tStartTime == 0)
        ulSecond = time((time_t *)NULL);
    else
        ulSecond = tStartTime;
    pstBlkIdxEntryEntry->start_time = ulSecond;
    pstBlkIdxEntryEntry->end_time = ulSecond;
    pstBlkIdxEntryEntry->status = STOR_FILE_BLK_STAT_USING;

    tmp = localtime(&ulSecond);
    strftime(time_buf,sizeof(time_buf),"%Y%m%d%H%M%S",tmp);
    snprintf(pstBlkIdxEntryEntry->filename, sizeof(pstBlkIdxEntryEntry->filename) - 1,
             "chn%d_%s.tmm", iCHn, time_buf);
    snprintf(pstBlkIdxEntryEntry->blockname, sizeof(pstBlkIdxEntryEntry->blockname),
             "rec%04d", pstBlkIdxEntryEntry->blockid);

    pstChnIdxInfo->s32NewestIndex = uiNewBlockIndex;

    AV_PRINT("CHN(%02d) new block name:%s file:%s",iCHn,pstBlkIdxEntryEntry->blockname,pstBlkIdxEntryEntry->filename);

    s32Ret = fseek(pRecLogFp, u32StartAddr, SEEK_SET);
    if(s32Ret != 0)
    {
        AV_PRINT("fseek recode config err(%s), s32Ret(%d)", ERR_STR, s32Ret);
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
        return HI_FAILURE;
    }

    s32Ret = fwrite(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pRecLogFp);
    if(s32Ret < 0)
    {
        AV_PRINT("fwrite stor chn idx info err(%s)!", ERR_STR);
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
        return HI_FAILURE;
    }

    if(NULL != pstChnIdxInfo)
    {
        free(pstChnIdxInfo);
        pstChnIdxInfo = NULL;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_DelOldBlkId
 功能描述  : 删除最老的视频块
 输入参数  : HI_U32 uiBlockID
             FILE *pRecLogFp
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
#ifdef HDD_SIZE_CHECK
HI_S32 STOR_DelOldBlkId(HI_U32 u32BlkId, FILE *pIdxBlkFp)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32FileCnt = 0;
    HI_S32 s32ChnCnt = 0;
    HI_S32 s32FileNums = 0;
    HI_S32 s32TmpOldestest = 0;
    HI_S32 s32TmpFileNums = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;
    HI_U32 u32Addr = 0;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("input para invalid.");
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    AV_PRINT("START STOR_DelOldBlkId");
    for (s32ChnCnt = 0; s32ChnCnt < APP_VENC_CHNCNT; s32ChnCnt++)
    {
        u32Addr = STOR_IDX_BLK_CHN_SEEK(s32ChnCnt);
        s32Ret = fseek(pIdxBlkFp, u32Addr, SEEK_SET);
        if (s32Ret != 0)
        {
            AV_PRINT("fseek err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }

        s32Ret = fread(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pIdxBlkFp);
        if (s32Ret < 0)
        {
            AV_PRINT("fread err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }

        if ((pstChnIdxInfo->s32Counter > STOR_RECFILE_MAX_NUMS) ||
                (pstChnIdxInfo->s32NewestIndex >= STOR_RECFILE_MAX_NUMS) ||
                (pstChnIdxInfo->s32OldestIndex >= STOR_RECFILE_MAX_NUMS))
        {
            AV_PRINT("read chn data err, s32Counter=%d, s32NewestIndex=%d, s32OldestIndex=%d",
                     pstChnIdxInfo->s32Counter, pstChnIdxInfo->s32NewestIndex, pstChnIdxInfo->s32OldestIndex);
            pstChnIdxInfo->s32Counter = 0;
            pstChnIdxInfo->s32NewestIndex = 0;
            pstChnIdxInfo->s32OldestIndex = 0;
        }

        if (0 == pstChnIdxInfo->s32Counter)
        {
            continue;
        }

        s32FileNums = pstChnIdxInfo->s32Counter;
        s32FileCnt = 0;
        for (s32FileCnt = 0; s32FileCnt <s32FileNums; s32FileCnt++)
        {
            pstBlkIdxEntry = &(pstChnIdxInfo->stBlkIdxList[(pstChnIdxInfo->s32OldestIndex+s32FileCnt) % STOR_RECFILE_MAX_NUMS]);
            //     AV_PRINT("s32ChnCnt= %d,s32FileCnt=%d, s32Counter=%d, s32NewestIndex=%d, s32OldestIndex=%d",
            //         		s32ChnCnt,s32FileCnt,pstChnIdxInfo->s32Counter, pstChnIdxInfo->s32NewestIndex, pstChnIdxInfo->s32OldestIndex);

            if (u32BlkId == pstBlkIdxEntry->blockid)
            {
#ifdef STORE_DEBUG
                printf("s32ChnCnt = %d,s32FileCnt = %d,STOR_DelOldBlkId uiBlockID=%d, blockid = %d, addr=0x%x, file=%s\n",
                       s32ChnCnt,s32FileCnt,u32BlkId, pstBlkIdxEntry->blockid,
                       (HI_U32)pstBlkIdxEntry,
                       pstBlkIdxEntry->filename);
#endif

                if (pstChnIdxInfo->s32Counter > 0)
                {
                    s32TmpFileNums = pstChnIdxInfo->s32Counter - s32FileCnt;
                    pstChnIdxInfo->s32Counter = s32TmpFileNums - 1;
                    s32TmpOldestest = pstChnIdxInfo->s32OldestIndex + s32FileCnt;
                    pstChnIdxInfo->s32OldestIndex = (s32TmpOldestest + 1) % STOR_RECFILE_MAX_NUMS;
                    /*个数都没了直接标为0 */
                    if (0 == pstChnIdxInfo->s32Counter)
                    {
                        pstChnIdxInfo->s32OldestIndex = 0;
                        pstChnIdxInfo->s32NewestIndex = 0;
                    }
                }
                else
                {
                    pstChnIdxInfo->s32Counter = 0;
                }

                AV_PRINT("chn (%d) delblk ok s32Counter=%d, s32NewestIndex=%d, s32OldestIndex=%d",
                         s32ChnCnt, pstChnIdxInfo->s32Counter, pstChnIdxInfo->s32NewestIndex, pstChnIdxInfo->s32OldestIndex);

                s32Ret = fseek(pIdxBlkFp, u32Addr, SEEK_SET);
                if(s32Ret != 0)
                {
                    AV_PRINT("fseek err(%s)", ERR_STR)
                    free(pstChnIdxInfo);
                    pstChnIdxInfo = NULL;
                    return HI_FAILURE;
                }

                s32Ret = fwrite(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pIdxBlkFp);
                if(s32Ret < 0)
                {
                    AV_PRINT("fwrite err(%s)", ERR_STR)
                    free(pstChnIdxInfo);
                    pstChnIdxInfo = NULL;
                    return HI_FAILURE;
                }

                free(pstChnIdxInfo);
                pstChnIdxInfo = NULL;
                return HI_SUCCESS;
            }
        }
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;

}
#else
HI_S32 STOR_DelOldBlkId(HI_U32 u32BlkId, FILE *pIdxBlkFp)
{
    HI_S32 s32Ret = 0;
    HI_S32 i = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;
    HI_U32 u32Addr = 0;

    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("input para invalid.");
        return HI_FAILURE;
    }

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    for (i = 0; i < APP_VENC_CHNCNT; i++)
    {
        u32Addr = STOR_IDX_BLK_CHN_SEEK(i);
        s32Ret = fseek(pIdxBlkFp, u32Addr, SEEK_SET);
        if (s32Ret != 0)
        {
            AV_PRINT("fseek err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }

        s32Ret = fread(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pIdxBlkFp);
        if (s32Ret < 0)
        {
            AV_PRINT("fread err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }

        if ((pstChnIdxInfo->s32Counter > STOR_RECFILE_MAX_NUMS) ||
                (pstChnIdxInfo->s32NewestIndex >= STOR_RECFILE_MAX_NUMS) ||
                (pstChnIdxInfo->s32OldestIndex >= STOR_RECFILE_MAX_NUMS))
        {
            AV_PRINT("read chn data err, s32Counter=%d, s32NewestIndex=%d, s32OldestIndex=%d",
                     pstChnIdxInfo->s32Counter, pstChnIdxInfo->s32NewestIndex, pstChnIdxInfo->s32OldestIndex);
            pstChnIdxInfo->s32Counter = 0;
            pstChnIdxInfo->s32NewestIndex = 0;
            pstChnIdxInfo->s32OldestIndex = 0;
        }

        if (0 == pstChnIdxInfo->s32Counter)
        {
            continue;
        }

        pstBlkIdxEntry = &(pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32OldestIndex]);
        //if (0 == i)
        {
#ifndef STORE_DEBUG
            AV_PRINT("uiBlockID=%d, blockid = %d, addr=0x%x, file=%s",
                     u32BlkId, pstBlkIdxEntry->blockid, (HI_U32)pstBlkIdxEntry,
                     pstBlkIdxEntry->filename);
#endif
        }

        if (u32BlkId == pstBlkIdxEntry->blockid)
        {
#ifndef STORE_DEBUG
            printf("STOR_DelOldBlkId uiBlockID=%d, blockid = %d, addr=0x%x, file=%s\n",
                   u32BlkId, pstBlkIdxEntry->blockid,
                   (HI_U32)pstBlkIdxEntry,
                   pstBlkIdxEntry->filename);
#endif

            if (pstChnIdxInfo->s32Counter > 0)
            {
                pstChnIdxInfo->s32Counter--;
                pstChnIdxInfo->s32OldestIndex = (pstChnIdxInfo->s32OldestIndex + 1) % STOR_RECFILE_MAX_NUMS;
            }
            else
            {
                pstChnIdxInfo->s32Counter = 0;
            }
        }

        s32Ret = fseek(pIdxBlkFp, u32Addr, SEEK_SET);
        if(s32Ret != 0)
        {
            AV_PRINT("fseek err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }

        s32Ret = fwrite(pstChnIdxInfo, sizeof(STOR_CHN_IDX_INFO_T), 1, pIdxBlkFp);
        if(s32Ret < 0)
        {
            AV_PRINT("fwrite err(%s)", ERR_STR)
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;
            return HI_FAILURE;
        }
    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
    return HI_SUCCESS;
}
#endif

/*****************************************************************************
 函 数 名  : STOR_GetIdleBlkId
 功能描述  : 获取空闲的视频块
 输入参数  : STOR_BLK_STAT_INFO_T *pstBlkStat
             HI_S32 *pS32NewBlkId
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月26日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetIdleBlkId(STOR_BLK_STAT_INFO_T *pstBlkStat, HI_S32 *pS32NewBlkId, FILE *pIdxBlkFp)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32BlkMax = 0;
    HI_S32 i = 0;
    HI_S32 s32NewestIndex = 0;

    if ((NULL == pstBlkStat) || (NULL == pS32NewBlkId))
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }

    /* 获取该分区视频块最大值 */
    s32BlkMax = STOR_OnePartFileNum();
    s32NewestIndex = pstBlkStat->s32BlkNewestIndex;

#ifdef STORE_DEBUG
    STORE_DBG_PRT("s32NewestIndex(%d),s32BlkMax(%d)", s32NewestIndex,s32BlkMax);
#endif

    for (i = s32NewestIndex; i < (s32BlkMax - 0);)
    {
        /* 如果counter值为0，则newestindex值不再增加 */
        if (0 != pstBlkStat->s32BlkCounter)
        {
            i = (i + 1) % STOR_RECFILE_MAX_NUMS;
        }
        pstBlkStat->s32BlkCounter++;

        /* 在覆盖的情况下 需要删除最老的一条记录 */
        if (STOR_IsRecOverWrite())
        {
            s32Ret = STOR_DelOldBlkId(i, pIdxBlkFp);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("STOR_DelOldBlkId failed! s32OldestIndex=%u",i);
                return HI_FAILURE;
            }
        }

        if (STOR_FILE_BLK_STAT_BAD != pstBlkStat->cBlkStat[i])
        {
            pstBlkStat->s32BlkNewestIndex = i;
            *pS32NewBlkId = i;
            return HI_SUCCESS;
        }
    }

    return HI_BLOCK_FULL;
}

#ifdef STORE_MULTI_THD
HI_VOID STOR_SetMemFullDefault(HI_S32 s32Chn)
{
    REC_MEM_HEAD_T stRecMemHead;
    SHM_MNG_T *pstShmMng = NULL;
    SHM_ATTR_T *pstShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
    pstShmAttr = &pstShmMng->shmAttr;

    pstChnStorMgmt->bRecording = HI_FALSE;

    /*清空数据*/
    memset(pstShmAttr->pShmAddr, 0, SHM_RECBUF_MAX_SIZE);

    /*初始化头部数据 */
    memset(&stRecMemHead, 0, sizeof(REC_MEM_HEAD_T));
    stRecMemHead.cWrChnIdx = 0;
    stRecMemHead.s32MemFullStat = 0x0;
    stRecMemHead.s32MemEmptyStat = 12; /*1100*/
    memcpy(pstShmAttr->pShmAddr, &stRecMemHead, sizeof(REC_MEM_HEAD_T));

    return;
}



/*****************************************************************************
 函 数 名  : STOR_OpenCurrChnIdx
 功能描述  : 打开当前通道索引文件
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_OpenCurrChnIdx(HI_S32 s32Chn)
{
    HI_CHAR cIdxPath[64] = {0};
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    //HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(NULL != pstChnStorMgmt->pIdxFile)
    {
        fclose(pstChnStorMgmt->pIdxFile);
        pstChnStorMgmt->pIdxFile = NULL;

    }

    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstChnStorMgmt->iCurrWRId));
    pstChnStorMgmt->pIdxFile = fopen(cIdxPath, "r+");
    if (NULL == pstChnStorMgmt->pIdxFile)
    {
        AV_PRINT("open %s err(%s)", cIdxPath, ERR_STR);
        return HI_FAILURE;
    }
    AV_PRINT("open chan(%d)idxfile(%s) success", s32Chn, cIdxPath);
    return HI_SUCCESS;
}

HI_VOID STOR_SetSwitchDefalt()
{
    HI_S32 s32Chn = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for(s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        if (STOR_GetChnRecStat(s32Chn))
        {
            pstChnStorMgmt->bSwitchOK = HI_FALSE;
        }
    }

}


HI_VOID STOR_BzeroSwitchEndIdxFileSata(HI_S32 s32Chn)
{
    HI_S32 s32ChnCnt = 0;
    HI_S32 s32Ret = HI_FAILURE;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;

    for(s32ChnCnt = 0; s32ChnCnt < APP_VENC_CHNCNT; s32ChnCnt++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32ChnCnt);
        pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];
        if (STOR_GetChnRecStat(s32ChnCnt))
        {
            if(!pstChnStorMgmt->bSwitchOK)
            {
                return;
            }
        }
    }

    AV_PRINT("set video blkstat default\n");

    /*关闭当前通道索引，不会出现跨三个分区的情况下，这个分区是切换的最后一个索引 */
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];
    /**/
    memset(&pstStorePart->stBlkStatInfo, 0, sizeof(STOR_BLK_STAT_INFO_T));
    s32Ret = STOR_SetVideoBlkStat(pstChnStorMgmt->pIdxFile, &pstStorePart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return;
    }

    STOR_SetSwitchDefalt();
    return;
}

/*获取新录像文件ID RECXXX */
HI_S32 STOR_GetNewRecBlkID(STOR_BLK_STAT_INFO_T *pstBlkStat, FILE *pIdxBlkFp,
                           HI_S32 *pS32NewBlkId, HI_S32 s32Chn,time_t tStartTime)
{
    HI_S32 s32Ret = 0;
    STOR_BLK_STAT_INFO_T stBlkStat;

    if ((NULL == pstBlkStat) || (NULL == pIdxBlkFp) || (NULL == pS32NewBlkId))
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }

    /* 如果是该分区最后一个视频块，则直接更换下一个分区 */
    s32Ret = STOR_GetIdleBlkId(pstBlkStat, pS32NewBlkId, pIdxBlkFp);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetIdleBlkId error! ");
        if (HI_BLOCK_FULL == s32Ret)
        {
            /* 将该分区数据清空 */
            memset(&stBlkStat, 0, sizeof(stBlkStat));
            s32Ret = STOR_SetVideoBlkStat(pIdxBlkFp, &stBlkStat);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("STOR_InitIdxBlk error! ");
                return HI_FAILURE;
            }

            AV_PRINT("This part is full! ");
            return HI_BLOCK_FULL;
        }
        else
        {
            AV_PRINT("STOR_GetIdleBlkId failed! ");
            return HI_FAILURE;
        }
    }
    else
    {
        /* 添加一个新视频块到索引 */
        s32Ret = STOR_AddNewBlockID(*pS32NewBlkId ,pIdxBlkFp, s32Chn,tStartTime);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_AddNewBlockID failed! s32BlkOldestIndex=%u",stBlkStat.s32BlkOldestIndex);
            return HI_FAILURE;
        }

#ifndef STORE_DEBUG
        AV_PRINT("pstBlkStat->s32BlkCounter (%d)",pstBlkStat->s32BlkCounter);
        AV_PRINT("pstBlkStat->s32BlkNewestIndex (%d)",pstBlkStat->s32BlkNewestIndex);
        AV_PRINT("pstBlkStat->s32BlkOldestIndex (%d)",pstBlkStat->s32BlkOldestIndex);
#endif

        /* 将视频块列表进行更新 */
        s32Ret = STOR_SetVideoBlkStat(pIdxBlkFp, pstBlkStat);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_SetVideoBlkStat error! ");
            return HI_FAILURE;
        }
        return HI_SUCCESS;
    }
}

/*****************************************************************************
 函 数 名  : STOR_InitVideoIndexInfo
 功能描述  : 初始化视频索引信息
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月28日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InitVideoIndexInfo(HI_VOID)
{
    HI_S32 s32Ret = 0;
    HI_CHAR cPartPath[64] = {0};
    FILE *pRecLogFp = NULL;
    HI_S32 i = 0;

    for ( i = 0; i < PART_MAX_NUMS; i++ )
    {
        memset(cPartPath, 0, sizeof(cPartPath));
        GET_PART_INDEX_NAME(cPartPath, GET_PART_ID(i));
        pRecLogFp = fopen(cPartPath, "r+");
        if (pRecLogFp == NULL)
        {
            AV_PRINT("open file error failed! parth = %s", cPartPath);
            return HI_FAILURE;
        }

        /* 初始化视频索引文件 */
        s32Ret = STOR_InitIdxBlk(pRecLogFp);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_InitIdxBlk error! ");
            fflush(pRecLogFp);
            fclose(pRecLogFp);
            return HI_FAILURE;
        }

        fflush(pRecLogFp);
        fclose(pRecLogFp);
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_SetFileIFrameList
 功能描述  : 设置视频块帧索引信息
 输入参数  : FILE* pFile
             FILE_IFRAME_LIST_T *pstIFrameList
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetFileIFrameList(FILE* pFile, FILE_IFRAME_LIST_T *pstIFrameList)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;
#ifdef HDD_SIZE_CHECK
    HI_S32 storeframe_startpos = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(pstPart ->hddinfo)
        storeframe_startpos = 510*1024*1024;
    else
        storeframe_startpos = 255*1024*1024;
#endif
    if ((NULL == pFile) || (NULL == pstIFrameList))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }
#ifdef HDD_SIZE_CHECK
    s32Ret = fseek(pFile, storeframe_startpos, SEEK_SET);
#else
    s32Ret = fseek(pFile, STOR_FRAME_IDX_START_POS, SEEK_SET);
#endif
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fwrite(pstIFrameList, 1, sizeof(FILE_IFRAME_LIST_T), pFile);
    if (s32Len != sizeof(FILE_IFRAME_LIST_T))
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    fflush(pFile);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_GetFileIFrameList
 功能描述  : 获取视频块I帧索引列表
 输入参数  : FILE* pFile
             FILE_IFRAME_LIST_T *pstIFrameList
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetFileIFrameList(FILE* pFile, FILE_IFRAME_LIST_T *pstIFrameList)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;
#ifdef HDD_SIZE_CHECK
    HI_S32 storeframe_startpos = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(pstPart ->hddinfo)
        storeframe_startpos = 510*1024*1024;
    else
        storeframe_startpos = 255*1024*1024;
#endif
    if ((NULL == pFile) || (NULL == pstIFrameList))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }
#ifdef HDD_SIZE_CHECK
    s32Ret = fseek(pFile, storeframe_startpos, SEEK_SET);
#else
    s32Ret = fseek(pFile, STOR_FRAME_IDX_START_POS, SEEK_SET);
#endif
    if(s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fread(pstIFrameList, 1, sizeof(FILE_IFRAME_LIST_T), pFile);
    if(s32Len != sizeof(FILE_IFRAME_LIST_T))
    {
        AV_PRINT("read file block head info err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef REC_SAVE_GPS
/*****************************************************************************
 函 数 名  : STOR_SetGpsInfoList
 功能描述  : 写GPS信息
 输入参数  : FILE* pFile
             FILE_GPS_LIST_ST *pstGpsList
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SetGpsInfoList(FILE* pFile, FILE_GPS_LIST_ST *pstGpsList)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;
#ifdef HDD_SIZE_CHECK
    HI_S32 storegps_startpos = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(pstPart ->hddinfo)
        storegps_startpos = 508*1024*1024;
    else
        storegps_startpos = 254*1024*1024;
#endif
    if ((NULL == pFile) || (NULL == pstGpsList))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }
#ifdef HDD_SIZE_CHECK
    s32Ret = fseek(pFile, storegps_startpos, SEEK_SET);
#else
    s32Ret = fseek(pFile, STOR_GPS_IDX_START_POS, SEEK_SET);
#endif
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fwrite(pstGpsList, 1, sizeof(FILE_GPS_LIST_ST), pFile);
    if (s32Len != sizeof(FILE_GPS_LIST_ST))
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    fflush(pFile);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_GetGpsInfoList
 功能描述  : 读GPS列表信息
 输入参数  : FILE* pFile
             FILE_GPS_LIST_ST *pstGpsList
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetGpsInfoList(FILE* pFile, FILE_GPS_LIST_ST *pstGpsList)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;
#ifdef HDD_SIZE_CHECK
    HI_S32 storegps_startpos = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(pstPart ->hddinfo)
        storegps_startpos = 508*1024*1024;
    else
        storegps_startpos = 254*1024*1024;
#endif
    if ((NULL == pFile) || (NULL == pstGpsList))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }

#ifdef HDD_SIZE_CHECK
    s32Ret = fseek(pFile, storegps_startpos, SEEK_SET);
#else
    s32Ret = fseek(pFile, STOR_GPS_IDX_START_POS, SEEK_SET);
#endif
    if(s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Len = fread(pstGpsList, 1, sizeof(FILE_GPS_LIST_ST), pFile);
    if(s32Len != sizeof(FILE_GPS_LIST_ST))
    {
        AV_PRINT("read file block head info err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_UpdateIFrameGps
 功能描述  : 更新GPS信息
 输入参数  : STOR_GPS_FRAME_DATA_ST *pGpsFrameHead
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID STOR_UpdateIFrameGps(STOR_GPS_FRAME_DATA_ST *pGpsFrameHead)
{
    pGpsFrameHead->latitude = g_stOsdShowData.stOsdSchInfo.latitude;
    pGpsFrameHead->longitude = g_stOsdShowData.stOsdSchInfo.longitude;
}

/*****************************************************************************
 函 数 名  : STOR_InsertGpsInfoByIFrame
 功能描述  : I帧后插入GPS信息
 输入参数  : FRAME_HEAD_T *pIFrameHead
             HI_U8 *pVencData
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InsertGpsInfoByIFrame(FRAME_HEAD_T *pIFrameHead, HI_U8 *pVencData)
{
    FRAME_HEAD_T stGpsFrameHead;
    FRAME_HEAD_T *pstGpsFrameHead = &stGpsFrameHead;
    STOR_GPS_FRAME_DATA_ST stGpsFrameData;

    memcpy(pstGpsFrameHead, pIFrameHead, sizeof(FRAME_HEAD_T));

    pstGpsFrameHead->len = sizeof(STOR_GPS_FRAME_DATA_ST);
    pstGpsFrameHead->type = DATA_GPS;

    memcpy(pVencData, (HI_U8*)pstGpsFrameHead, sizeof(FRAME_HEAD_T));
    pVencData += sizeof(FRAME_HEAD_T);

    memset(&stGpsFrameData, 0, sizeof(STOR_GPS_FRAME_DATA_ST));
    stGpsFrameData.nmk_time1 = pIFrameHead->nmk_time1;
    stGpsFrameData.nmk_time2 = pIFrameHead->nmk_time2;
    stGpsFrameData.nmk_time3 = pIFrameHead->nmk_time3;
    stGpsFrameData.nmk_time4 = pIFrameHead->nmk_time4;

    STOR_UpdateIFrameGps(&stGpsFrameData);

    memcpy(pVencData, (HI_U8*)&stGpsFrameData, sizeof(STOR_GPS_FRAME_DATA_ST));
    pVencData += sizeof(STOR_GPS_FRAME_DATA_ST);

    return pstGpsFrameHead->len;
}

/*****************************************************************************
 函 数 名  : STOR_UpdateGpsInfoIdx
 功能描述  : 更新GPS索引信息
 输入参数  : FILE_GPS_LIST_ST *pstGpsList
             FRAME_HEAD_T *pstFrameHead
             HI_S32 s32CurrWrFilePos
             HI_U64 s32FrameSeek
             HI_S32 u32ChnId
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_UpdateGpsInfoIdx(FILE_GPS_LIST_ST *pstGpsList, FRAME_HEAD_T *pstFrameHead,
                             HI_S32 s32CurrWrFilePos, HI_U64 s32FrameSeek, HI_S32 u32ChnId)
{
    HI_S32 s32NewIndex = 0;
    STOR_GPS_INFO_ST *pstGpsInfo = NULL;

    s32NewIndex = pstGpsList->s32NewestIndex;
    if (s32NewIndex >= STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32Counter=%d,s32NewIndex=%d", pstGpsList->s32Counter, s32NewIndex);
        s32NewIndex = s32NewIndex % STOR_FILE_IFRAME_MAX_NUMS;
        pstGpsList->s32NewestIndex = s32NewIndex;
    }

    pstGpsList->s32Counter++;
    pstGpsList->s32NewestIndex++;

    pstGpsInfo = &(pstGpsList->stGpsInfo[s32NewIndex]);
    pstGpsInfo->type = DATA_GPS;
    pstGpsInfo->frame_data_pos = s32FrameSeek + s32CurrWrFilePos;
    pstGpsInfo->nmk_time1 = pstFrameHead->nmk_time1;
    pstGpsInfo->nmk_time2 = pstFrameHead->nmk_time2;
    pstGpsInfo->nmk_time3 = pstFrameHead->nmk_time3;
    pstGpsInfo->nmk_time4 = pstFrameHead->nmk_time4;

#ifndef STORE_DEBUG
    STORE_DBG_PRT("s32Chn(%d),Update GpsIdxTime(%u),Current pstGpsInfo->frame_data_pos(%u)",
                  u32ChnId, STOR_CalTime3(pstGpsInfo),
                  pstGpsInfo->frame_data_pos);
#endif

    return HI_SUCCESS;
}
#endif

/*****************************************************************************
 函 数 名  : STOR_UpdateIFrameIdx
 功能描述  :
 输入参数  :

 输出参数  : 无
 返 回 值  : 新I帧IDX


 修改历史      :
  1.日    期   : 2014年12月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/

HI_S32 STOR_UpdateIFrameIdx(FILE_IFRAME_LIST_T *pstIFrameList, FRAME_HEAD_T *pstFrameHead,
                            HI_S32 s32CurrWrFilePos, HI_U64 s32FrameSeek,
                            HI_S32 u32ChnId, HI_S32 *s32ChnStat)
{
    HI_S32 s32NewIndex = 0;
    HI_U32 u32TmpDate = 0;
    HI_S32 s32Ret = 0;
    STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

    /*静态变量用于存储，两个相邻I帧的时间*/
    static HI_U32 u32IFrameNowTime[APP_VENC_CHNCNT] = {0};
    static HI_U32 u32IFrameLastTime[APP_VENC_CHNCNT] = {0};

    s32NewIndex = pstIFrameList->s32NewestIndex;
    if (s32NewIndex >= STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32Counter=%d,s32NewIndex=%d", pstIFrameList->s32Counter, s32NewIndex);
        s32NewIndex = s32NewIndex % STOR_FILE_IFRAME_MAX_NUMS;
        pstIFrameList->s32NewestIndex = s32NewIndex;
    }

    pstIFrameList->s32Counter++;
    pstIFrameList->s32NewestIndex++;
    u32IFrameNowTime[u32ChnId] = ((pstFrameHead->nmk_time4 <<24) | (pstFrameHead->nmk_time3 <<16)
                                  | (pstFrameHead->nmk_time2 <<8) | pstFrameHead->nmk_time1);

    u32TmpDate = u32IFrameNowTime[u32ChnId] - u32IFrameLastTime[u32ChnId];
    if (u32TmpDate >= 3)
    {
        s32Ret = HDD_WrRecStat(u32IFrameLastTime[u32ChnId], u32IFrameNowTime[u32ChnId], u32ChnId, (HI_CHAR *)APP_REC_ERR_LOG);
        if (0 != s32Ret)
        {
            AV_PRINT("HDD_WrRecStat err 0x%x", s32Ret);
        }
    }
    u32IFrameLastTime[u32ChnId] = u32IFrameNowTime[u32ChnId];

    pstIFRAMEInfo = &(pstIFrameList->stRecIFrameInfo[s32NewIndex]);
    pstIFRAMEInfo->type = DATA_VIDEO_IFRAME;
    pstIFRAMEInfo->frame_data_pos = s32FrameSeek + s32CurrWrFilePos;
    pstIFRAMEInfo->nmk_time1 = pstFrameHead->nmk_time1;
    pstIFRAMEInfo->nmk_time2 = pstFrameHead->nmk_time2;
    pstIFRAMEInfo->nmk_time3 = pstFrameHead->nmk_time3;
    pstIFRAMEInfo->nmk_time4 = pstFrameHead->nmk_time4;

#ifndef STORE_DEBUG
    STORE_DBG_PRT("s32Chn(%d),Update IframeIdxTime(%u),Current pstIFRAMEInfo->frame_data_pos(%u) PTS (%lld)",
                  u32ChnId, STOR_CalTime1(pstIFRAMEInfo),
                  pstIFRAMEInfo->frame_data_pos,
                  pstFrameHead->ts);
#endif
    if (s32NewIndex >= STOR_FILE_IFRAME_FULL)
    {
        *s32ChnStat = STOR_FILE_FULL;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_UpdateIdxEntry
 功能描述  :
 输入参数  :

 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月25日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/

HI_VOID STOR_UpdateIdxEntry(STOR_REC_IFRAME_INFO *pstIFRAMEInfo, HI_S32 s32CurrWrFilePos, HI_U64 s32FrameSeek, FRAME_HEAD_T *pstFrameHead)
{
    pstIFRAMEInfo->type = DATA_VIDEO_IFRAME;
    pstIFRAMEInfo->frame_data_pos = s32FrameSeek + s32CurrWrFilePos;
    pstIFRAMEInfo->nmk_time1 = pstFrameHead->nmk_time1;
    pstIFRAMEInfo->nmk_time2 = pstFrameHead->nmk_time2;
    pstIFRAMEInfo->nmk_time3 = pstFrameHead->nmk_time3;
    pstIFRAMEInfo->nmk_time4 = pstFrameHead->nmk_time4;
    return;
}

HI_S32 STOR_GetRecCacheLength(HI_S32 s32Chn)
{
    if(s32Chn>=0 && s32Chn<APP_VENC_CHNCNT)
        return g_s32RecCacheLength[s32Chn];
    else
        return 2*MEM_1M;
}

/*****************************************************************************
 函 数 名  : STOR_WrFrameData
 功能描述  :
 输入参数  :

 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_WrFrameData(FILE *pRecFilefp, HI_CHAR *pDataBuf, HI_S32 s32Buflen, HI_S32 s32CurrWrFilePos)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = fseek(pRecFilefp, s32CurrWrFilePos, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fwrite(pDataBuf + sizeof(REC_CHN_MEM_HEAD_T), s32Buflen, 1, pRecFilefp);
    if (s32Ret != 1)
    {
        AV_PRINT("fwrite expect len(%d), real len(%d), err(%s)", s32Buflen, s32Ret, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
HI_S32 STOR_WrFrameData_2(FILE *pRecFilefp, HI_CHAR *pDataBuf, HI_S32 s32Buflen, HI_S32 s32CurrWrFilePos)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = fseek(pRecFilefp, s32CurrWrFilePos, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = fwrite(pDataBuf, s32Buflen, 1, pRecFilefp);
    if (s32Ret != 1)
    {
        AV_PRINT("fwrite expect len(%d), real len(%d), err(%s)", s32Buflen, s32Ret, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
HI_S32 STOR_CopyFrameDataToFile(HI_S32 s32Chn,char * pRecBuf,int iFrameLen
                                ,FILE_IFRAME_LIST_T **ppstFileIFrameList
                                ,FILE_GPS_LIST_ST **ppstGpsList
                               )
{
    HI_S32 s32Ret = 0;
    HI_S32 s32FuncRet = HI_SUCCESS;
    HI_S32 s32FrameSeek = 0;
    STOR_FILE_BLK_HEAD_T *pstFileBlkHead = NULL;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    FILE_GPS_LIST_ST *pstGpsList = NULL;
    FRAME_HEAD_T *pstFrameHead = NULL;
    HI_S32 s32ChnStat[APP_VENC_CHNCNT] = {0};
    HI_U32 u32ChnID = s32Chn;
    HI_U32 u32DataLen = 0;
    HI_U32 framedata_endaddr = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if (NULL == ppstFileIFrameList || NULL == ppstGpsList)
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }
#ifdef HDD_SIZE_CHECK
    if(pstPart->hddinfo)
        framedata_endaddr = 506*1024*1024;
    else
        framedata_endaddr = 250*1024*1024;
#endif
    pstIFrameList = ppstFileIFrameList[u32ChnID];
    pstGpsList = ppstGpsList[u32ChnID];
    pstFileBlkHead = &pstChnStorMgmt->stFileBlockHead;
    if ((NULL == pstFileBlkHead)||(NULL == pstIFrameList) || (NULL == pstGpsList))
    {
        AV_PRINT("pstFileBlkHead(%p) pstIFrameList(%p) pstGpsList(%p) is NULL!",pstFileBlkHead,pstIFrameList,pstGpsList);
        return HI_FAILURE;
    }
    int iRecCacheLen = STOR_GetRecCacheLength(s32Chn);
    if(pRecBuf == NULL || iFrameLen > iRecCacheLen)
    {
        AV_PRINT("pRecBuf(%p) is NULL! iFrameLen(%d) error",pRecBuf,iFrameLen);
        return HI_FAILURE;
    }

    s32Ret = STOR_WrFrameData_2(pstChnStorMgmt->pRecFile,(HI_CHAR*)pRecBuf,
                                iFrameLen,pstFileBlkHead->frame_data_end_pos);
    if (s32Ret < 0)
    {
        free(ppstFileIFrameList[u32ChnID]);
        ppstFileIFrameList[u32ChnID] = NULL;
        free(ppstGpsList[u32ChnID]);
        ppstGpsList[u32ChnID] = NULL;
        HDD_WrRecStat(HDD_GetDiskFSM(), HDD_GetHddEvent(DISK_EVENT_REC)
                      , __LINE__,  (HI_CHAR *)APP_REC_ERR_LOG);
        AV_PRINT("CHN %d STOR_WrCachDataToRecFile error.free ppstFileIFrameList,ppstGpsList",u32ChnID);
        return HI_FAILURE;
    }

    /*更新IFRAME索引*/
    s32FrameSeek = 0;
    while (s32FrameSeek < iFrameLen)
    {
        pstFrameHead = (FRAME_HEAD_T *)(pRecBuf + s32FrameSeek);
        if (DATA_VIDEO_IFRAME == pstFrameHead->type)
        {
            STOR_UpdateIFrameIdx(pstIFrameList, pstFrameHead,
                                 pstFileBlkHead->frame_data_end_pos,
                                 s32FrameSeek, u32ChnID, &s32ChnStat[u32ChnID]);
        }

#ifdef REC_SAVE_GPS
        else if (DATA_GPS == pstFrameHead->type)
        {
            STOR_UpdateGpsInfoIdx(pstGpsList, pstFrameHead, pstFileBlkHead->frame_data_end_pos,
                                  s32FrameSeek, u32ChnID);
        }
#endif
        s32FrameSeek = s32FrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
    }

    pstFileBlkHead->frame_data_end_pos = pstFileBlkHead->frame_data_end_pos + iFrameLen;

    /* 判断是否应该切换文件 */
#ifdef HDD_SIZE_CHECK
    if ((pstFileBlkHead->frame_data_end_pos >= framedata_endaddr)||
#else
    if ((pstFileBlkHead->frame_data_end_pos >= STOR_FRAME_DATA_END_ADDR)||
#endif
            (STOR_FILE_FULL == s32ChnStat[u32ChnID]))
    {
        STORE_DBG_PRT("s32Chn(%d),frame_data_end_pos=%u, endadd=%u",
                      s32Chn, pstFileBlkHead->frame_data_end_pos,
                      framedata_endaddr);

        AV_PRINT("chn=%d,IFrame: Counter=%d,OldestIndex=%d,NewestIndex=%d,ChnStat[u32ChnID]=%d",
                 u32ChnID,
                 pstIFrameList->s32Counter,
                 pstIFrameList->s32OldestIndex,
                 pstIFrameList->s32NewestIndex,
                 s32ChnStat[u32ChnID]);

        /* 更新视频块文件 */
#ifdef REC_SAVE_GPS
        s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, pstGpsList, pstFileBlkHead);
#else
        s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, pstFileBlkHead);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "STOR_UpdateBlockFileInfo err!");
        }
#endif
        //fsync(fileno(pstChnStorMgmt->pRecFile));
        u32DataLen = pstFileBlkHead->frame_data_end_pos - STOR_FRAME_DATA_START_ADDR;

        /*更新索引文件 */
        pthread_mutex_lock(&pstPart->partMutexLock);
        s32Ret = STOR_UpdateChnListEndTime(pstChnStorMgmt->pIdxFile, pstIFrameList, u32ChnID, u32DataLen);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_UpdateChnListEndTime err!");
        }
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_BLOCK_FULL;
    }
    return s32FuncRet;
}
HI_S32 STOR_WrCachDataToRecFile(HI_S32 s32Chn,HBC hList,
                                FILE_IFRAME_LIST_T **ppstFileIFrameList,
                                FILE_GPS_LIST_ST **ppstGpsList)
{
    HI_S32 s32Ret = 0;
    int iFrameLen  = 0;
    char * pRecBuf = NULL;
    while(1)
    {
        HDP hPkg = mem_PopPackageData(hList);
        if(hPkg == NULL)
        {
            return HI_NO_DATA;
        }
        pRecBuf = mem_GetPackageData(hPkg);
        iFrameLen = mem_GetPackageDataLen(hPkg);
        s32Ret = STOR_CopyFrameDataToFile(s32Chn,pRecBuf,iFrameLen \
                                          ,ppstFileIFrameList,ppstGpsList);
        mem_ReleasePackage(hPkg);
        if(s32Ret!=HI_SUCCESS)
        {
            return HI_FAILURE;

        }
    }
    return HI_SUCCESS;
}
HI_S32 STOR_WrRecFile_2(HI_S32 s32Chn,HBC hList,
                        FILE_IFRAME_LIST_T **ppstFileIFrameList,
                        FILE_GPS_LIST_ST **ppstGpsList,
                        STOR_REC_CACHE_ST *pCache)
{
    HI_S32 s32Ret = 0;
    int iFrameLen  = 0;
    char * pRecBuf = NULL;
    HDP hPkg = mem_PopPackageDataTmp(hList);
    if(hPkg!=NULL)
    {
        pRecBuf = mem_GetPackageData(hPkg);
        iFrameLen = mem_GetPackageDataLen(hPkg);
    }
    int iRecCacheLen = STOR_GetRecCacheLength(s32Chn);
    int iLeftLen = iRecCacheLen - pCache->m_iBufferPos;
    if(iLeftLen==0||iLeftLen<iFrameLen)
    {
        AV_PRINT("CHN(%02d) Cache full(%d) FIFO(%d)",s32Chn,pCache->m_iBufferPos
                 ,mem_GetPackageNum(hList));
        s32Ret = STOR_CopyFrameDataToFile(s32Chn,(HI_CHAR *)pCache->m_pcBuffer,pCache->m_iBufferPos \
                                          ,ppstFileIFrameList,ppstGpsList);
        pCache->m_iBufferPos = 0;
        if(s32Ret!=HI_SUCCESS)
            return s32Ret;
    }
    if(hPkg!=NULL)
    {
        memcpy(pCache->m_pcBuffer + pCache->m_iBufferPos,pRecBuf,iFrameLen);
        pCache->m_iBufferPos+= iFrameLen;
        mem_ReleaseTopPackage(hList);
    }
    int iCount = mem_GetPackageNum(hList);

    while(iCount>0)
    {
        hPkg = mem_PopPackageDataTmp(hList);
        if(hPkg == NULL)
            break;
        pRecBuf = mem_GetPackageData(hPkg);
        iFrameLen = mem_GetPackageDataLen(hPkg);
        iRecCacheLen = STOR_GetRecCacheLength(s32Chn);
        iLeftLen = iRecCacheLen - pCache->m_iBufferPos;
        if(iLeftLen<iFrameLen)
        {
            break;
        }
        memcpy(pCache->m_pcBuffer + pCache->m_iBufferPos,pRecBuf,iFrameLen);
        pCache->m_iBufferPos+= iFrameLen;
        mem_ReleaseTopPackage(hList);
        iCount--;
    }
    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : STOR_WrRecFile
 功能描述  : 写录像文件
 输入参数  : HI_S32 s32Chn
             HI_S32 s32MemStat
             FILE_IFRAME_LIST_T **ppstFileIFrameList
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月4日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_WrRecFile(HI_S32 s32Chn, HI_S32 s32MemStat,
                      FILE_IFRAME_LIST_T **ppstFileIFrameList,
                      FILE_GPS_LIST_ST **ppstGpsList)
{
    HI_S32 s32RecFullStat = 0;
    HI_S32 s32Ret = 0;
    HI_S32 iItem = 0;
    HI_S32 s32FrameSeek = 0;
    //HI_U64 u64FrameSeek=0;
    HI_CHAR *p;
    REC_CHN_MEM_HEAD_T *pstRecChnMemHead = NULL;
    STOR_FILE_BLK_HEAD_T *pstFileBlkHead = NULL;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    FILE_GPS_LIST_ST *pstGpsList = NULL;
    FRAME_HEAD_T *pstFrameHead = NULL;
    HI_S32 s32ChnStat[APP_VENC_CHNCNT] = {0};
    HI_U32 u32FileFullStat = 0;
    HI_U32 u32ChnID = 0;
    HI_U32 u32DataLen = 0;
    HI_U32 framedata_endaddr = 0;
    //time_t nowTime;
//    ALARM_REC_BK_ST *pstAlarmRecBk = HI3531_GetAlarmRecBkInfo();
#ifdef STORE_DEBUG
    long pos = 0;
    pos = pos;
#endif

    SHM_MNG_T *pstShmMng = NULL;
    SHM_ATTR_T *pstRecShmAttr = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if (NULL == ppstFileIFrameList)
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }
#ifdef HDD_SIZE_CHECK
    if(pstPart->hddinfo)
        framedata_endaddr = 506*1024*1024;
    else
        framedata_endaddr = 250*1024*1024;
#endif
    pstShmMng = &pstChnStorMgmt->stShmSemMng.shmMng;
    pstRecShmAttr = &pstShmMng->shmAttr;

    /*获取到满的标志 */
    s32RecFullStat = s32MemStat;
    for (iItem = 0; iItem < pstRecShmAttr->s32ItemMaxNum; iItem++)
    {
        if (s32RecFullStat & (0x01 << iItem))
        {
            /*读取满的里面的数据放到缓存中，标志还原回去 */
            s32Ret = MEM_GetRecFromOneMem(pstRecShmAttr, &pstChnStorMgmt->RecMemRWLock, iItem, pstChnStorMgmt->pRecbuf);
            if (HI_SUCCESS != s32Ret)
            {
#ifdef STORE_DEBUG
                STORE_DBG_PRT("iItem(%d),have not get data\n", iItem);
#endif
                continue;
            }

            pstRecChnMemHead = (REC_CHN_MEM_HEAD_T *)pstChnStorMgmt->pRecbuf;
            u32ChnID = pstRecChnMemHead->s32WrChn;
            pstFileBlkHead = &pstChnStorMgmt->stFileBlockHead;

            pstIFrameList = ppstFileIFrameList[u32ChnID];
            pstGpsList = ppstGpsList[u32ChnID];
            if ((NULL == pstFileBlkHead)||(NULL == pstIFrameList) || (NULL == pstGpsList))
            {
                continue;
            }

            p = pstChnStorMgmt->pRecbuf + sizeof(REC_CHN_MEM_HEAD_T);

#ifndef STORE_DEBUG
            STORE_DBG_PRT("s32Chn(%d),iItem(%d),Write s32RecDataLen(%d),Current frame_data_end_pos(%d)",
                          s32Chn, iItem,pstRecChnMemHead->s32RecDataLen,
                          pstFileBlkHead->frame_data_end_pos);
            //STORE_DebugWRStream(s32Chn, pstChnStorMgmt->pRecbuf, pstRecChnMemHead->s32RecDataLen);
#endif

            /*将读出来的buf数据写到文件中 */
            s32Ret = STOR_WrFrameData(pstChnStorMgmt->pRecFile, pstChnStorMgmt->pRecbuf,
                                      pstRecChnMemHead->s32RecDataLen,
                                      pstFileBlkHead->frame_data_end_pos);
            if (s32Ret < 0)
            {
                free(ppstFileIFrameList[u32ChnID]);
                ppstFileIFrameList[u32ChnID] = NULL;
                free(ppstGpsList[u32ChnID]);
                ppstGpsList[u32ChnID] = NULL;
                HDD_WrRecStat(HDD_GetDiskFSM(), HDD_GetHddEvent(DISK_EVENT_REC), __LINE__,  (HI_CHAR *)APP_REC_ERR_LOG);
                return HI_FAILURE;
            }

#ifndef STORE_DEBUG
            pos = ftell(pstChnStorMgmt->pRecFile);
            AV_PRINT( "s32Chn(%d),STOR_WrFrameData pos is %ld.\n!",s32Chn,pos);
#endif

            /*更新IFRAME索引*/
            s32FrameSeek = 0;
            while (s32FrameSeek < pstRecChnMemHead->s32RecDataLen)
            {
                pstFrameHead = (FRAME_HEAD_T *)(p + s32FrameSeek);
                if (DATA_VIDEO_IFRAME == pstFrameHead->type)
                {
                    STOR_UpdateIFrameIdx(pstIFrameList, pstFrameHead, pstFileBlkHead->frame_data_end_pos,
                                         s32FrameSeek, u32ChnID, &s32ChnStat[u32ChnID]);
                }

#ifdef REC_SAVE_GPS
                else if (DATA_GPS == pstFrameHead->type)
                {
                    STOR_UpdateGpsInfoIdx(pstGpsList, pstFrameHead, pstFileBlkHead->frame_data_end_pos,
                                          s32FrameSeek, u32ChnID);
                }
#endif
                s32FrameSeek = s32FrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
            }

            pstFileBlkHead->frame_data_end_pos = pstFileBlkHead->frame_data_end_pos + pstRecChnMemHead->s32RecDataLen;

#if 0
            if (bAlarmTrigger)
            {
                if (!bRecUpdateidx[s32Chn])
                {
                    pstAlarmRecBk->alarmTriggStart = HI_TRUE;
                    pstAlarmRecBk->alarmStartTime = time(NULL);
                    /*更新录像文件事件*/
                    s32Ret =  STOR_UpdateEventRecFileInfo(pstChnStorMgmt->pRecFile, EVENT_ALARM_REC_E);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT("STOR_UpdateEventRecFileInfo FAILED");
                    }
                    /*更新到索引文件 */
                    s32Ret =  STOR_UpdateEventIdxHead(pstChnStorMgmt->pIdxFile, s32Chn);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT("STOR_UpdateEventIdxHead FAILED");
                    }

                    AV_PRINT("bAlarm STOR_UpdateEventRecFileInfo ok");
                    bRecUpdateidx[s32Chn] = HI_TRUE;
                }

                nowTime = time(NULL);
                if (nowTime >= pstAlarmRecBk->alarmStartTime + pstAlarmRecBk->s32BkDowntimesec)
                {
                    /*一次报警结束之后才开始接受别的报警处理 */
                    pstAlarmRecBk->alarmEndTime = time(NULL);
                    pstAlarmRecBk->alarmTriggEnd = HI_TRUE;
                    bRecUpdateidx[s32Chn] = HI_FALSE;
                    AV_PRINT("alarm over now");
                }
            }
#endif

            /* 判断是否应该切换文件 */
#ifdef HDD_SIZE_CHECK
            if ((pstFileBlkHead->frame_data_end_pos >= framedata_endaddr)||
#else
            if ((pstFileBlkHead->frame_data_end_pos >= STOR_FRAME_DATA_END_ADDR)||
#endif
                    (STOR_FILE_FULL == s32ChnStat[u32ChnID]))
            {
                STORE_DBG_PRT("s32Chn(%d),frame_data_end_pos=%u, endadd=%u",
                              s32Chn, pstFileBlkHead->frame_data_end_pos,
                              STOR_FRAME_DATA_END_ADDR);

                AV_PRINT("chn=%d,IFrame: Counter=%d,OldestIndex=%d,NewestIndex=%d,ChnStat[u32ChnID]=%d",
                         u32ChnID,
                         pstIFrameList->s32Counter,
                         pstIFrameList->s32OldestIndex,
                         pstIFrameList->s32NewestIndex,
                         s32ChnStat[u32ChnID]);

                /* 更新视频块文件 */
#ifdef REC_SAVE_GPS
                s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, pstGpsList, pstFileBlkHead);
#else
                s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, pstFileBlkHead);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT( "STOR_UpdateBlockFileInfo err!");
                }
#endif
                u32DataLen = pstFileBlkHead->frame_data_end_pos - STOR_FRAME_DATA_START_ADDR;

                /*更新索引文件 */
                pthread_mutex_lock(&pstPart->partMutexLock);
                s32Ret = STOR_UpdateChnListEndTime(pstChnStorMgmt->pIdxFile, pstIFrameList, u32ChnID, u32DataLen);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("STOR_UpdateChnListEndTime err!");
                }
                pthread_mutex_unlock(&pstPart->partMutexLock);

                u32FileFullStat = ((0x01 << u32ChnID) | u32FileFullStat);
                STORE_DBG_PRT("u32FileFullStat =0x%x,u32ChnID=%d!", u32FileFullStat, u32ChnID);
            }
        }
    }

    if (0 != u32FileFullStat)
    {
        return HI_BLOCK_FULL;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : STOR_ShowVideoIndexList
 功能描述  : 显示视频索引文件信息
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月27日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_ShowVideoIndexList(FILE *pIdxBlkFp)
{
    HI_S32 s32Ret = 0;
//    HI_S32 i = 0;
//    HI_S32 j = 0;
    HI_CHAR cPartPath[64]= {0};
    STOR_BLK_STAT_INFO_T stRecodeBlock;
    //  STOR_CHN_IDX_INFO_T *pstChnIdxInfo = NULL;
    //  STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;

    if (pIdxBlkFp == NULL)
    {
        AV_PRINT("open file error failed! parth = %s", cPartPath);
        return HI_FAILURE;
    }

    s32Ret = STOR_GetVideoBlkStat(pIdxBlkFp, &stRecodeBlock);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    AV_PRINT(": STOR_GetVideoBlkStat:start\n*****************************\n");
    AV_PRINT(": s32BlkCounter=%d\n", stRecodeBlock.s32BlkCounter);
    AV_PRINT(": s32BlkNewestIndex=%d\n", stRecodeBlock.s32BlkNewestIndex);
    AV_PRINT(": s32BlkOldestIndex=%d\n", stRecodeBlock.s32BlkOldestIndex);
    AV_PRINT(": STOR_GetVideoBlkStat:end\n*****************************\n");
#if 0

    pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    if (NULL == pstChnIdxInfo)
    {
        AV_PRINT("malloc memory failed");
        return HI_FAILURE;
    }
    memset(pstChnIdxInfo, 0, sizeof(STOR_CHN_IDX_INFO_T));

    for ( i = 0; i < APP_VENC_CHNCNT; i++ )
    {

        s32Ret = STOR_GetBlkIdxByChn(pIdxBlkFp, pstChnIdxInfo, i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_SetBlkIdxByChn error!chn=%d",i);
            free(pstChnIdxInfo);
            pstChnIdxInfo = NULL;

            return HI_FAILURE;
        }
        AV_PRINT(": STOR_GetBlkIdxByChn:start\n*****************************\n");
        AV_PRINT(": CHN=%4d\n*****************************\n", i);
        AV_PRINT(": s32Counter=%d\n", pstChnIdxInfo->s32Counter);
        AV_PRINT(": s32NewestIndex=%d\n", pstChnIdxInfo->s32NewestIndex);
        AV_PRINT(": s32OldestIndex=%d\n", pstChnIdxInfo->s32OldestIndex);

#if 1
        for ( j = 0; j < pstChnIdxInfo->s32Counter; j++ )
        {
            pstBlkIdxEntry = &(pstChnIdxInfo->stBlkIdxList[pstChnIdxInfo->s32OldestIndex + j]);
            AV_PRINT(": filename=%s,blockname=%s,blockid=%d, index=%d,start=%u,end=%u\n",
                     pstBlkIdxEntry->filename,
                     pstBlkIdxEntry->blockname,
                     pstBlkIdxEntry->blockid,
                     j,
                     pstBlkIdxEntry->start_time,
                     pstBlkIdxEntry->end_time);
        }
#endif

        AV_PRINT(": STOR_GetBlkIdxByChn:end\n*****************************\n");

    }

    free(pstChnIdxInfo);
    pstChnIdxInfo = NULL;
#endif
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_InitVideoBlkFile
 功能描述  : 视频块数据初始化
 输入参数  : FILE *pRecLogFp
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月28日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InitVideoBlkFile(FILE *pRecFile, STOR_FILE_BLK_HEAD_T *pstFileBlkHead,
                             FILE_IFRAME_LIST_T **ppstIFrameList, FILE_GPS_LIST_ST **ppstGpsList,
                             STOR_FILE_INFO_T *pstVideoFileInfo)
{
    HI_S32 s32Ret = 0;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    FILE_GPS_LIST_ST *pstGpsList = NULL;
#ifdef HDD_SIZE_CHECK
    HI_S32 storeframe_startpos = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    if(pstPart ->hddinfo)
        storeframe_startpos = 510*1024*1024;
    else
        storeframe_startpos = 255*1024*1024;
#endif
    if ((NULL == pRecFile) || (NULL == ppstIFrameList))
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }

    memset(pstFileBlkHead, 0, sizeof(STOR_FILE_BLK_HEAD_T));
#ifdef HDD_SIZE_CHECK
    pstFileBlkHead->frame_index_start_pos = storeframe_startpos;
#else
    pstFileBlkHead->frame_index_start_pos = STOR_FRAME_IDX_START_POS;
#endif
    pstFileBlkHead->frame_data_start_pos = STOR_FRAME_DATA_START_ADDR;
    pstFileBlkHead->frame_data_end_pos = STOR_FRAME_DATA_START_ADDR;
    pstFileBlkHead->file_info_start_pos = STOR_FILE_INFO_START_ADDR;
    pstFileBlkHead->reserve = time((time_t *)NULL);

    s32Ret = STOR_SetVideoFileHeadInfo(pRecFile, pstFileBlkHead, STOR_IDX_BLK_HEAD_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    s32Ret = STOR_SetVideoFileInfo(pRecFile, pstVideoFileInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    memset(pstIFrameList, 0, sizeof(FILE_IFRAME_LIST_T));

    s32Ret = STOR_SetFileIFrameList(pRecFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetFileIFrameList error!");
        free(pstIFrameList);
        return HI_FAILURE;
    }
    *ppstIFrameList = pstIFrameList;

#ifdef REC_SAVE_GPS
    pstGpsList = (FILE_GPS_LIST_ST *)malloc(sizeof(FILE_GPS_LIST_ST));
    if (NULL == pstGpsList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        free(pstIFrameList);
        return HI_FAILURE;
    }
    memset(pstGpsList, 0, sizeof(FILE_GPS_LIST_ST));

    s32Ret = STOR_SetFileIFrameList(pRecFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("MEM_GetRecFromOneMem err!");
        free(pstIFrameList);
        free(pstGpsList);
        return HI_FAILURE;
    }
    *ppstGpsList = pstGpsList;
#endif

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_GetNewRecFileBlkNums
 功能描述  : 分配文件个数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetNewRecFileBlkNums()
{
    return 1;
}

/*****************************************************************************
 函 数 名  : STOR_GetPartSwitchNeedNums
 功能描述  : 分区切换要的文件个数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_GetPartSwitchNeedNums()
{
    return APP_VENC_CHNCNT;
}

/*****************************************************************************
 函 数 名  : STOR_CheckSwitchPart
 功能描述  : 检查是否要切换分区
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_CheckSwitchPart()
{
    HI_U32 u32TotalBlks = 0;
    HI_U32 u32leftBlks = 0;
    HI_U32 u32NeedBlks = 0;
    HI_U32 u32UsedBlks = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;
    pstStorePart = &pstPart->stStorePartInfo[pstPart->iPartId];

    u32TotalBlks = STOR_OnePartFileNum();
    u32NeedBlks = STOR_GetPartSwitchNeedNums();

    pthread_mutex_lock(&pstPart->partMutexLock);
    u32UsedBlks = pstStorePart->stBlkStatInfo.s32BlkCounter;
    u32leftBlks = u32TotalBlks - u32UsedBlks;

#ifdef STORE_DEBUG
    STORE_DBG_PRT("u32TotalBlks(%d),u32UsedBlks(%d), u32NeedBlks(%d)", \
                  u32TotalBlks,u32UsedBlks,u32NeedBlks);
#endif

    if (u32leftBlks >= u32NeedBlks)
    {
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return STOR_NO_SWITCH_PART;
    }
    pthread_mutex_unlock(&pstPart->partMutexLock);

    AV_PRINT("Start Need Switch Part!!!");
    return STOR_SWITCH_PART;
}


/*****************************************************************************
 函 数 名  : STOR_IsSwitchPartByChn
 功能描述  : 按通道进行切换
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_IsSwitchPartByChn(HI_S32 s32Chn)
{
    HI_U32 u32TotalBlks = 0;
    HI_U32 u32leftBlks = 0;
    HI_U32 u32NeedBlks = 0;
    HI_U32 u32UsedBlks = 0;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];

    u32TotalBlks = STOR_OnePartFileNum();
    u32NeedBlks = STOR_GetNewRecFileBlkNums();

    pthread_mutex_lock(&pstPart->partMutexLock);
    u32UsedBlks = pstStorePart->stBlkStatInfo.s32BlkCounter;
    u32leftBlks = u32TotalBlks - u32UsedBlks;

#ifdef STORE_DEBUG
    STORE_DBG_PRT("u32TotalBlks(%d),u32UsedBlks(%d), u32NeedBlks(%d)", \
                  u32TotalBlks,u32UsedBlks,u32NeedBlks);
#endif

    if (u32leftBlks >= u32NeedBlks)
    {
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return STOR_NO_SWITCH_PART;
    }
    pthread_mutex_unlock(&pstPart->partMutexLock);

    AV_PRINT("now s32Chn(%d) Need Switch Part!!!",s32Chn);
    return STOR_SWITCH_PART;
}


/*****************************************************************************
 Prototype    : STOR_PartEndProc
 Description  : 结束当前分区
 Input        : HI_U32 uiPart
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/8/9
    Author       :
    Modification : Created function

*****************************************************************************/
HI_S32 STOR_PartEndProc(FILE *pIdxBlkFp, STOR_BLK_STAT_INFO_T *pstBlkStatInfo)
{
    HI_S32 s32BlkMax = 0;
    HI_S32 i = 0;
    HI_S32 s32NewestIndex = 0;
    HI_S32 s32Ret = 0;

    if ((NULL == pstBlkStatInfo) || (NULL == pIdxBlkFp))
    {
        AV_PRINT( "Input NULL pointer!");
        return HI_FAILURE;
    }

    /* 获取该分区视频块最大值 */
    s32BlkMax = STOR_OnePartFileNum();
    s32NewestIndex = pstBlkStatInfo->s32BlkNewestIndex;

    for (i = s32NewestIndex; i < (s32BlkMax - 1); i++)
    {
        /* 如果counter值为0，则newestindex值不再增加 */
        if (0 != pstBlkStatInfo->s32BlkCounter)
        {
            i = (i + 1) % s32BlkMax;
        }

        pstBlkStatInfo->s32BlkCounter++;

        /* 需要删除最老的一条记录 */
        s32Ret = STOR_DelOldBlkId(i, pIdxBlkFp);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_DelOldBlkId failed! s32OldestIndex=%u",i);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_StartStore
 功能描述  : 初始化录像文件
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月9日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_RecFileInit()
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Chn = 0;
    HI_CHAR cIdxPath[64] = {0};
    STOR_BLK_IDX_HEAD_T stBlkIdxHead;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstStorePart = &pstPart->stStorePartInfo[pstPart->iPartId];

    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstPart->iPartId));
    pstStorePart->pIdxFile = fopen(cIdxPath, "r+");
    if (NULL == pstStorePart->pIdxFile)
    {
        AV_PRINT("open %s err(%s)", cIdxPath, ERR_STR);
        return HI_FAILURE;
    }

    stBlkIdxHead.en_chns = APP_VENC_CHNCNT;
    stBlkIdxHead.all_blocks = STOR_OnePartFileNum();
    s32Ret = STOR_SetBlkIdxHead(pstStorePart->pIdxFile, &stBlkIdxHead);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxHead error! ");
        return HI_FAILURE;
    }

    /*获取当前分区索引中的读写状态 */
    s32Ret = STOR_GetVideoBlkStat(pstStorePart->pIdxFile, &pstStorePart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error!s32Ret=%d", s32Ret);
        return HI_FAILURE;
    }

    for (s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        pstChnStorMgmt->iCurrWRId = pstPart->iPartId;
    }

    AV_PRINT("open %s success", cIdxPath);
    if (NULL != pstStorePart->pIdxFile)
    {
        fclose(pstStorePart->pIdxFile);
        pstStorePart->pIdxFile = NULL;
    }

    AV_PRINT("STOR_RecFileInit !s32BlkCounter=%d", pstStorePart->stBlkStatInfo.s32BlkCounter);
    AV_PRINT("STOR_RecFileInit !s32BlkNewestIndex=%d", pstStorePart->stBlkStatInfo.s32BlkNewestIndex);
    AV_PRINT("STOR_RecFileInit !s32BlkOldestIndex=%d", pstStorePart->stBlkStatInfo.s32BlkOldestIndex);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_StartSwitchPart
 功能描述  : 录像刚启动时候检测是否切换分区
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_StartSwitchPart(HI_VOID)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Chn = 0;
    HI_CHAR cIdxPath[64] = {0};
    STOR_BLK_IDX_HEAD_T stBlkIdxHead;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstStorePart = &pstPart->stStorePartInfo[pstPart->iPartId];

    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstPart->iPartId));
    pstStorePart->pIdxFile = fopen(cIdxPath, "r+");
    if (NULL == pstStorePart->pIdxFile)
    {
        AV_PRINT("open %s err(%s)", cIdxPath, ERR_STR);
        return HI_FAILURE;
    }

#if 0
    /* 结束当前分区 */
    s32Ret = STOR_PartEndProc(pstPart->pIdxFile, &pstPart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "#[app]STOR_PartEndProc failed ");
        return HI_FAILURE;
    }
#endif

    /* 添加调试信息 */
    //STOR_ShowVideoIndexList(pIdxBlkFp);

    /* 将该分区数据清空 */
    memset(&pstStorePart->stBlkStatInfo, 0, sizeof(STOR_BLK_STAT_INFO_T));
    s32Ret = STOR_SetVideoBlkStat(pstStorePart->pIdxFile, &pstStorePart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error! ");
        return HI_FAILURE;
    }

    /*关闭之前操作的索引文件 */
    if (NULL != pstStorePart->pIdxFile)
    {
        fclose(pstStorePart->pIdxFile);
        pstStorePart->pIdxFile = NULL;
    }

    /* 切换分区，重新打开新的索引文件 */
    pstPart->iPartId = (pstPart->iPartId + 1) % PART_MAX_NUMS;

    printf("STOR_StartSwitchPart pstPart->iPartId is %d.\n",pstPart->iPartId);
    /*如果iPartId = 0 说明开始进行覆盖 */
    if (0 == pstPart->iPartId)
    {
        printf("STOR_StartSwitchPart over load now\n");
        pstPart->bOverWrite = HI_TRUE;
    }

    memset(cIdxPath, 0, sizeof(cIdxPath));
    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstPart->iPartId));
    pstStorePart->pIdxFile = fopen(cIdxPath, "r+");
    if (pstStorePart->pIdxFile == NULL)
    {
        AV_PRINT("fopen %s err(%s)", cIdxPath, ERR_STR);
        return HI_FAILURE;
    }

    stBlkIdxHead.en_chns = APP_VENC_CHNCNT;
    stBlkIdxHead.all_blocks = STOR_OnePartFileNum();

    s32Ret = STOR_SetBlkIdxHead(pstStorePart->pIdxFile, &stBlkIdxHead);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxHead error! ");
        return HI_FAILURE;
    }

    s32Ret = STOR_ChkIdxFileVer(pstStorePart->pIdxFile);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_ChkIdxFileVer error! ");
        return HI_FAILURE;
    }

#if 0
    /* 需要删除最老的一条记录 */
    s32Ret = STOR_DelOldBlkId(0, pstPart->pIdxFile);
    if (HI_SUCCESS != s32Ret)
    {
        fflush(pstPart->pIdxFile);
        fclose(pstPart->pIdxFile);
        AV_PRINT("STOR_DelOldBlkId failed!");
        pthread_mutex_unlock(&pstStorePart->partMutexLock);
        return HI_FAILURE;
    }
#endif
    s32Ret = STOR_GetVideoBlkStat(pstStorePart->pIdxFile, &pstStorePart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    /* 设置当前使用的硬盘分区 */
    s32Ret = STOR_SaveRecPartId(pstPart->iPartId);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SaveRecPartId failed ");
        return HI_FAILURE;
    }

    for (s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        pstChnStorMgmt->iCurrWRId = pstPart->iPartId;
    }

    if(NULL != pstStorePart->pIdxFile)
    {
        fclose(pstStorePart->pIdxFile);
        pstStorePart->pIdxFile = NULL;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : STOR_SwitchPartByChn
 功能描述  : 按通道进行切换分区
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_SwitchPartByChn(HI_S32 s32Chn)
{
    HI_S32 s32Ret = 0;
    HI_CHAR cIdxPath[64] = {0};
    STOR_BLK_IDX_HEAD_T stBlkIdxHead;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    CHN_STOR_MGMT_ST *pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    STOR_PART_INFO_ST *pstStorePart = NULL;
    pthread_mutex_lock(&pstPart->partMutexLock);

#if 0
    /* 结束当前分区 */
    s32Ret = STOR_PartEndProc(pstPart->pIdxFile, &pstPart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "#[app]STOR_PartEndProc failed ");
        return HI_FAILURE;
    }
#endif

    /* 添加调试信息 */
    //STOR_ShowVideoIndexList(pIdxBlkFp);

    /* 将该分区数据清空 */
    STOR_BzeroSwitchEndIdxFileSata(s32Chn);
#if 0
    memset(&pstPart->stBlkStatInfo, 0, sizeof(STOR_BLK_STAT_INFO_T));
    s32Ret = STOR_SetVideoBlkStat(pstChnStorMgmt->pIdxFile, &pstPart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error! ");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
#endif

    /*关闭之前操作的索引文件 */
    if (NULL != pstChnStorMgmt->pIdxFile)
    {
        fclose(pstChnStorMgmt->pIdxFile);
        pstChnStorMgmt->pIdxFile = NULL;
    }
    AV_PRINT("STOR_SwitchPartByChn err!");
    /* 切换分区，重新打开新的索引文件 */
    pstChnStorMgmt->iCurrWRId = (pstChnStorMgmt->iCurrWRId+ 1) % PART_MAX_NUMS;

    pstPart->iPartId = pstChnStorMgmt->iCurrWRId;

    /*如果iPartId = 0 说明开始进行覆盖 */
    if (0 == pstChnStorMgmt->iCurrWRId)
    {
        printf("################## over load now##############\n");
        pstPart->bOverWrite = HI_TRUE;
        /*记录覆盖参数 */
    }

    AV_PRINT("STOR_SwitchPartByChn err %d\n",pstChnStorMgmt->iCurrWRId);

    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];
    memset(cIdxPath, 0, sizeof(cIdxPath));
    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstChnStorMgmt->iCurrWRId));
    pstChnStorMgmt->pIdxFile = fopen(cIdxPath, "r+");
    if (pstChnStorMgmt->pIdxFile == NULL)
    {
        AV_PRINT("fopen %s err(%s)", cIdxPath, ERR_STR);
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    stBlkIdxHead.en_chns = APP_VENC_CHNCNT;
    stBlkIdxHead.all_blocks = STOR_OnePartFileNum();

    s32Ret = STOR_SetBlkIdxHead(pstChnStorMgmt->pIdxFile, &stBlkIdxHead);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetBlkIdxHead error! ");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    s32Ret = STOR_ChkIdxFileVer(pstChnStorMgmt->pIdxFile);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_ChkIdxFileVer error! ");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
#if 0
    /* 需要删除最老的一条记录 */
    s32Ret = STOR_DelOldBlkId(0, pstPart->pIdxFile);
    if (HI_SUCCESS != s32Ret)
    {
        fflush(pstPart->pIdxFile);
        fclose(pstPart->pIdxFile);
        AV_PRINT("STOR_DelOldBlkId failed!");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    s32Ret = STOR_GetVideoBlkStat(pstChnStorMgmt->pIdxFile, &pstStorePart->stBlkStatInfo);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoBlkStat error!s32Ret=%d",s32Ret);
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    AV_PRINT("STOR_SwitchPartByChn err!");
#endif
    /* 设置当前使用的硬盘分区 */
    s32Ret = STOR_SaveRecPartId(pstPart->iPartId);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SaveRecPartId failed ");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }

    if(NULL != pstStorePart->pIdxFile)
    {
        fclose(pstStorePart->pIdxFile);
        pstStorePart->pIdxFile = NULL;
    }

    if(NULL != pstChnStorMgmt->pIdxFile)
    {
        fclose(pstChnStorMgmt->pIdxFile);
        pstChnStorMgmt->pIdxFile = NULL;
    }

    AV_PRINT("STOR_SwitchPartByChn : s32Chn (%d),s32NewestIndex=%d,s32OldestIndex=%d,s32Counter=%d",
             s32Chn,pstStorePart->stBlkStatInfo.s32BlkNewestIndex,
             pstStorePart->stBlkStatInfo.s32BlkOldestIndex,
             pstStorePart->stBlkStatInfo.s32BlkCounter);
    pthread_mutex_unlock(&pstPart->partMutexLock);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_UpdateIndexfile
 功能描述  : 更新录像文件索引信息
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_UpdateIndexfile(HI_S32 s32Chn)
{
    HI_U32 s32Ret = 0;
    HI_U32 u32DataLen = 0;
    STOR_FILE_BLK_HEAD_T *pstFileBlock = NULL;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
#ifdef REC_SAVE_GPS
    FILE_GPS_LIST_ST *pstGpsList = NULL;
#endif
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    pstIFrameList = g_pstFileIFrameList[s32Chn];
#ifdef REC_SAVE_GPS
    pstGpsList = g_pstGpsFrameList[s32Chn];
#endif
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;

    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];

#ifndef STORE_DEBUG
    STORE_DBG_PRT("s32Chn=%d,s32Counter=%d,s32OldestIndex=%d,s32NewestIndex=%d,pstIFrameList=0x%x",
                  s32Chn,pstIFrameList->s32Counter,
                  pstIFrameList->s32OldestIndex,pstIFrameList->s32NewestIndex,
                  (HI_U32)pstIFrameList);
#endif

    /* 更新视频块文件 */
#ifdef REC_SAVE_GPS
    s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, pstGpsList, &pstChnStorMgmt->stFileBlockHead);
#else
    s32Ret = STOR_UpdateBlockFileInfo(pstChnStorMgmt->pRecFile, pstIFrameList, &pstChnStorMgmt->stFileBlockHead);
#endif
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_UpdateBlockFileInfo err!");
    }

    pstFileBlock = &pstChnStorMgmt->stFileBlockHead;
    u32DataLen = pstFileBlock->frame_data_end_pos - STOR_FRAME_DATA_START_ADDR;

    /* 更新视频索引文件 */
    pthread_mutex_lock(&pstPart->partMutexLock);
    s32Ret = STOR_UpdateChnListEndTime(pstChnStorMgmt->pIdxFile, pstIFrameList, s32Chn, u32DataLen);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_UpdateChnListEndTime err!");
    }
    STOR_SetVideoBlkStat(pstChnStorMgmt->pIdxFile, &pstStorePart->stBlkStatInfo);
    pthread_mutex_unlock(&pstPart->partMutexLock);

    sync();
    //usleep(150000);
    TM_USleep(0,10000);
    sync();
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_AllocOneRecBlkFile
 功能描述  : 分配一个录像文件
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_AllocOneRecBlkFile(HI_S32 s32Chn,time_t tStartTime)
{
    HI_S32 s32VideoBlkId = 0;
    HI_CHAR cRecPath[64] = {0};
    HI_S32 s32Ret = 0;
    STOR_FILE_INFO_T stVideoFileInfo;
    time_t tSec = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;

    if (!IS_PART_ID_VAILD(pstChnStorMgmt->iCurrWRId))
    {
        AV_PRINT("rec part id out of range, 32CurPart = %d", pstChnStorMgmt->iCurrWRId);
        pstChnStorMgmt->iCurrWRId = 0;
    }

    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];

#ifdef STORE_DEBUG
    AV_PRINT("start malloc file here channo is = %d part %d.\n", s32Chn, pstChnStorMgmt->iCurrWRId);
#endif

    pthread_mutex_lock(&pstPart->partMutexLock);
    s32Ret = STOR_OpenCurrChnIdx(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "STOR_OpenCurrChnIdx failed");
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    /*获取申请的新的录像文件ID */
    s32Ret = STOR_GetNewRecBlkID(&pstStorePart->stBlkStatInfo, pstChnStorMgmt->pIdxFile \
                                    , &s32VideoBlkId, s32Chn,tStartTime);
    if (HI_SUCCESS == s32Ret)
    {
        memset(cRecPath, 0, sizeof(cRecPath));
        GET_PART_RECFILE_NAME(cRecPath, GET_PART_ID(pstPart->iPartId), s32VideoBlkId);
        pstChnStorMgmt->pRecFile = fopen(cRecPath, "r+");
        if (pstChnStorMgmt->pRecFile == NULL)
        {
            AV_PRINT("fopen %s err(%s)", cRecPath, ERR_STR);
            pthread_mutex_unlock(&pstPart->partMutexLock);
            return HI_FAILURE;
        }

        AV_PRINT(":::s32Chn(%d), openfile =%s.", s32Chn, cRecPath);
        memset(&stVideoFileInfo, 0, sizeof(stVideoFileInfo));
        stVideoFileInfo.frame_rate = 25;
        if(s32Chn > 7)
        {
            stVideoFileInfo.pixelwidth = 1280;
            stVideoFileInfo.pixelheight = 720;
        }
        else
        {
            stVideoFileInfo.pixelwidth = 720;
            stVideoFileInfo.pixelheight = 576;
        }
        tSec = time((time_t *)NULL);
        stVideoFileInfo.tv_sec = tSec;
        strncpy(stVideoFileInfo.company, "tiamaes", 7);
        strncpy(stVideoFileInfo.device_name, "TM8721", 6);
        s32Ret = STOR_InitVideoBlkFile(pstChnStorMgmt->pRecFile, &pstChnStorMgmt->stFileBlockHead,
                                       &g_pstFileIFrameList[s32Chn], &g_pstGpsFrameList[s32Chn]
                                       , &stVideoFileInfo);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_InitVideoBlkFile failed");
            pthread_mutex_unlock(&pstPart->partMutexLock);
            return HI_FAILURE;
        }
    }

    //STOR_ShowVideoIndexList(pstChnStorMgmt->pIdxFile);
    fflush(pstChnStorMgmt->pIdxFile);
    pthread_mutex_unlock(&pstPart->partMutexLock);
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_CloseRecFile
 功能描述  : 关闭打开的录像文件
 输入参数  : HI_S32 s32Chn
             FILE_IFRAME_LIST_T **ppstIFrameList
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年2月10日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID STOR_CloseRecFile(HI_S32 s32Chn, FILE_IFRAME_LIST_T **ppstIFrameList, FILE_GPS_LIST_ST **ppstGpsList)
{
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);

    if ((STOR_GetChnRecStat(s32Chn)))
    {
        if (NULL != pstChnStorMgmt->pRecFile)
        {
            fclose(pstChnStorMgmt->pRecFile);
            pstChnStorMgmt->pRecFile = NULL;
        }

        AV_PRINT("s32Chn (%d) iframelist free", s32Chn);
        /*I帧信息指针也释放 */
        if (NULL != ppstIFrameList[s32Chn])
        {
            free(ppstIFrameList[s32Chn]);
            ppstIFrameList[s32Chn] = NULL;
        }
        /*I帧信息指针也释放 */
        if (NULL != ppstGpsList[s32Chn])
        {
            free(ppstGpsList[s32Chn]);
            ppstGpsList[s32Chn] = NULL;
        }

    }

    return;
}

/*****************************************************************************
 函 数 名  : STOR_CloseIdxFile
 功能描述  : 关闭通道索引文件
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID STOR_CloseIdxFile(HI_VOID)
{
    HI_S32 s32Chn = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    for (s32Chn = 0; s32Chn < APP_VENC_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        if (NULL != pstChnStorMgmt->pIdxFile)
        {
            AV_PRINT("close chn(%d) av files.",s32Chn);
            fclose(pstChnStorMgmt->pIdxFile);
            pstChnStorMgmt->pIdxFile = NULL;
        }
    }

    return;
}

/*****************************************************************************
 函 数 名  : STOR_WrMemData2File
 功能描述  : 写内存中的码流数据到录像文件
 输入参数  : HI_S32 s32Chn
             SHM_ATTR_T *pstShmAttr
             bool bFullMem
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_WrMemData2File(HI_S32 s32Chn, SHM_ATTR_T *pstShmAttr, bool bFullMem)
{
    HI_S32 s32Ret;
    HI_S32 s32MemFullStat = 0;
    HI_S32 s32MemEmptyStat = 0;
    HI_S32 s32TemStat = 0;

    if (bFullMem)
    {
        //共享内存满数据
        MEM_GetShmFullStat(s32Chn, &s32MemFullStat);
        s32TemStat = s32MemFullStat;
        /*DEBUG*/
#ifdef STORE_DEBUG
        if(0 != s32MemFullStat)
        {
            // STORE_DBG_PRT("now s32Chn(%d), Full!!!!!!= 0x%x", s32Chn, s32MemFullStat);
        }
#else
        AV_PRINT("s32Chn(%d), s32RecFullStat = 0x%x", s32Chn, s32MemFullStat);
#endif
    }
    else
    {
        MEM_GetShmEmptyStat(s32Chn, &s32MemEmptyStat);
        AV_PRINT("s32Chn(%d), s32MemEmptyStat = 0x%x, s32MemEmptyStat^0xF = 0x%x",
                 s32Chn, s32MemEmptyStat, s32MemEmptyStat^0xF);
        s32TemStat = (s32MemEmptyStat ^ 0xF);
    }

    s32Ret = STOR_WrRecFile(s32Chn, s32TemStat, g_pstFileIFrameList, g_pstGpsFrameList);
    return s32Ret;
}

/*****************************************************************************
 函 数 名  : STOR_ChkPartIdxFile
 功能描述  : 检查索引文件
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_ChkPartIdxFile(HI_VOID)
{
    HI_S32 s32Ret = 0;
    HI_CHAR cIdxPath[64] = {0};
    FILE *pIdxBlkFp = NULL;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    /* 获取上一次录像停止时所在硬盘分区 */
    pthread_mutex_lock(&pstPart->partMutexLock);
    s32Ret = STOR_GetRecPartId();
    if (HI_SUCCESS != s32Ret)
    {
        pthread_mutex_unlock(&pstPart->partMutexLock);
        return HI_FAILURE;
    }
    /* 获取上一次录像停止时所在硬盘分区 */
    GET_PART_INDEX_NAME(cIdxPath, GET_PART_ID(pstPart->iPartId));
    pthread_mutex_unlock(&pstPart->partMutexLock);

    pIdxBlkFp = fopen(cIdxPath, "r+");
    if (NULL == pIdxBlkFp)
    {
        AV_PRINT("open(%s) err(%s)", cIdxPath, ERR_STR);
        return HI_FAILURE;
    }

    /* 获取指定分区里面视频块数 每个分区块数一样  */
    if (0 == STOR_OnePartFileNum())
    {
        STOR_GetOnePartBlkNum(cIdxPath);
        //分区异常
        if(pstPart->partFileNums > STOR_ONE_PART_FILES_MAX)
        {
            AV_PRINT("one part own rec file error.%d",pstPart->partFileNums);
            return HI_FAILURE;
        }
    }

    /*检查索引文件的版本信息 */
    s32Ret = STOR_ChkIdxFileVer(pIdxBlkFp);
    if (HI_SUCCESS != s32Ret)
    {
        if (NULL != pIdxBlkFp)
        {
            fclose(pIdxBlkFp);
            pIdxBlkFp = NULL;
        }
        AV_PRINT("STOR_ChkIdxFileVer error! ");
        return HI_FAILURE;
    }

    if (NULL != pIdxBlkFp)
    {
        fclose(pIdxBlkFp);
        pIdxBlkFp = NULL;
    }

    return HI_SUCCESS;
}
#ifdef HDD_SIZE_CHECK
HI_U32 GetHddFileSize()
{
    HI_U32 len = 0,ret = 0;
    FILE *fp  = fopen(FILE_BLOCK_SIZE_TEST,"rb");
    if(fp != NULL)
    {
        fseek( fp, SEEK_SET, SEEK_END);
        len = ftell(fp);
        fclose(fp);
        AV_PRINT("GetHddFileSize: %d\n", len);
        if(len == 512*1024*1024)
            ret = 1;
        else
            ret = 0;
    }
    return ret;
}
#endif

/*****************************************************************************
 函 数 名  : STOR_InitPartInfo
 功能描述  : 初始化分区信息
 输入参数  : HDD_PART_ST *pstPart
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InitPartInfo(HDD_PART_ST *pstPart)
{
    HI_S32 s32Ret = -1;

    memset(pstPart, 0, sizeof(HDD_PART_ST));

    s32Ret = pthread_mutex_init(&(pstPart->partMutexLock), NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_init failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = pthread_mutex_init(&(pstPart->partSwitchLock), NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_init failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = pthread_mutex_init(&(pstPart->blkStatLock), NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_init failed %#X\n", s32Ret);
        return HI_FAILURE;
    }


    /*Partid 初始化 */
    pstPart->iPartId = -1;
    pstPart->bOverWrite = HI_FALSE;       /*这个参数需要保存在参数表里面，*/
#ifdef HDD_SIZE_CHECK
    pstPart->hddinfo = GetHddFileSize();
#endif
    return HI_SUCCESS;
}

/*去初始化分区信息  */
/*****************************************************************************
 函 数 名  : STOR_ExitPartInfo
 功能描述  : 去初始化分区信息
 输入参数  : HDD_PART_ST *pstPart
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_ExitPartInfo(HDD_PART_ST *pstPart)
{
    HI_S32 s32Ret = -1;

    s32Ret = pthread_mutex_destroy(&(pstPart->partMutexLock));
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_destory failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = pthread_mutex_destroy(&(pstPart->partSwitchLock));
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_destory failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = pthread_mutex_destroy(&(pstPart->blkStatLock));
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("pthread_mutex_destory failed %#X\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_InitChnStoreMgmt
 功能描述  : 初始化通道存储结构
 输入参数  : CHN_STOR_MGMT_ST *pstChnStorMgmt
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_InitChnStoreMgmt(CHN_STOR_MGMT_ST *pstChnStorMgmt)
{
#if 0
    if (pstChnStorMgmt->pRecbuf == NULL)
    {
        pstChnStorMgmt->pRecbuf = (HI_CHAR *)malloc(SHM_RECBUF_ONE_SIZE);
        if (NULL == pstChnStorMgmt->pRecbuf)
        {
            AV_PRINT("malloc error");
            return HI_FAILURE;
        }

        memset(pstChnStorMgmt->pRecbuf, 0, SHM_RECBUF_ONE_SIZE);
    }
    memset(pstChnStorMgmt->pRecbuf, 0, SHM_RECBUF_ONE_SIZE);
#endif
    pstChnStorMgmt->pRecFile = NULL;

    memset(&pstChnStorMgmt->stFileBlockHead, 0, sizeof(STOR_FILE_BLK_HEAD_T));
    return HI_SUCCESS;
}

HI_VOID STOR_SetChnRecStat(HI_S32 s32Chn, HI_BOOL bFlag)
{
    CHN_STOR_MGMT_ST *pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);

    pstChnStorMgmt->bRecording = bFlag;
    return;
}

HI_BOOL STOR_GetChnRecStat(HI_S32 s32Chn)
{
    CHN_STOR_MGMT_ST *pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);

    return pstChnStorMgmt->bRecording;
}

/*****************************************************************************
 函 数 名  : STOR_DestroyRecbuf
 功能描述  : 销毁录像临时缓冲
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID STOR_DestroyRecbuf(HI_S32 s32Chn)
{
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;

    pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);

    if (NULL != pstChnStorMgmt->pRecbuf)
    {
        free(pstChnStorMgmt->pRecbuf);
        pstChnStorMgmt->pRecbuf = NULL;
    }

    return;
}

/*****************************************************************************
 函 数 名  : STOR_RecThdExitbyChn
 功能描述  : 录像线程退出
 输入参数  : HI_S32 s32Chn
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_RecThdExitbyChn(HI_S32 s32Chn)
{
    HDD_SetHddEvent(DISK_EVENT_REC, HI_FALSE);

    STOR_CloseRecFile(s32Chn, g_pstFileIFrameList, g_pstGpsFrameList);
#if 0
    STOR_DestroyRecbuf(s32Chn);
#endif
    STOR_SetChnRecStat(s32Chn, HI_FALSE);

    AV_PRINT("STOR_RecThdExitbyChn s32Chn is %d",s32Chn);

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_StoreThdByChn
 功能描述  : 按通道启动线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年5月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *STOR_StoreThdByChn(HI_VOID *pArg)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Chn = 0;
    HI_S32 s32IndexUpdateCnt = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = (CHN_STOR_MGMT_ST *)pArg;
    HDD_PART_ST *pstPart = STOR_GetHddPart();
    STOR_PART_INFO_ST *pstStorePart = NULL;

    pstStorePart = &pstPart->stStorePartInfo[pstChnStorMgmt->iCurrWRId];

    s32Chn = pstChnStorMgmt->iChan;

    s32Ret = STOR_InitChnStoreMgmt(pstChnStorMgmt);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_InitChnStoreMgmt failed");
        return NULL;
    }

    /* 打开视频文件 */
    s32Ret = STOR_AllocOneRecBlkFile(s32Chn,0);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_AllocOneRecBlkFile failed");
        return NULL;
    }

    /* 将录像开始事件写入到日志文件 */
#ifdef NAND_LOG
    s32Ret = HDD_WrRecStat(DISK_STAT_WORKING, HDD_GetHddEvent(DISK_EVENT_REC), s32Chn, (HI_CHAR *)APP_RECODE_LOG);
    if (0 != s32Ret)
    {
        AV_PRINT("HDD_WrRecStat err(%d)", s32Ret);
    }
#endif
    s32Ret = HDD_WrRecStat(DISK_STAT_WORKING, HDD_GetHddEvent(DISK_EVENT_REC), s32Chn,  (HI_CHAR *)APP_REC_ERR_LOG);
    if (0 != s32Ret)
    {
        AV_PRINT("HDD_WrRecStat err(%d)", s32Ret);
    }

#ifdef STORE_DEBUG
    AV_PRINT("start STOR_StoreThdByChn pthread s32Chn(%d)" ,s32Chn);
#endif
    STOR_SetChnRecStat(s32Chn, HI_TRUE);

    HBC hList = mem_CreateContainer(CONTAINER_FIFO,CONTAINER_PRI_CYC,1400);
    mem_Bind(hList,g_ChnDataCenter[2*s32Chn]);
    STOR_REC_CACHE_ST stRecCache;
    int iRecCacheLen = STOR_GetRecCacheLength(s32Chn);
    stRecCache.m_pcBuffer = (HI_U8*)malloc(iRecCacheLen*sizeof(HI_U8));
    stRecCache.m_iBufferPos = 0;
    AV_PRINT("@%s(%02d) pid is %d,tid%d,list(%p)", __FUNCTION__,s32Chn,getpid(),gettid(),hList);
    while (HDD_GetDiskStat())
    {
        /* 将录像数据写入到录像文件 */
        s32Ret = STOR_WrRecFile_2(s32Chn, hList, g_pstFileIFrameList
                                  , g_pstGpsFrameList,&stRecCache);
        //s32Ret = STOR_WrMemData2File(s32Chn, pstShmAttr, TRUE);
        if (HI_SUCCESS != s32Ret)
        {
            if (HI_BLOCK_FULL == s32Ret)
            {
                time_t ulSecond = time((time_t *)NULL);
                AV_PRINT("now s32Chn(%d) rec file block full!!!", s32Chn);
                /* 是否需要切换分区 */
                pthread_mutex_lock(&pstPart->partSwitchLock);
                s32Ret = STOR_IsSwitchPartByChn(s32Chn);
                if (STOR_SWITCH_PART == s32Ret)
                {
                    pstChnStorMgmt->bSwitchOK = HI_TRUE;
                    s32Ret = STOR_SwitchPartByChn(s32Chn);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT( "STOR_SwitchPart failed");
                        pthread_mutex_unlock(&pstPart->partSwitchLock);
                        return NULL;
                    }

                    AV_PRINT("::::::STOR_SwitchPart OK: s32NewestIndex=%d,s32OldestIndex=%d,s32Counter=%d",
                             pstStorePart->stBlkStatInfo.s32BlkNewestIndex,
                             pstStorePart->stBlkStatInfo.s32BlkOldestIndex,
                             pstStorePart->stBlkStatInfo.s32BlkCounter);
                }

                /*关闭当前操作的录像文件指针 */
                STOR_CloseRecFile(s32Chn, g_pstFileIFrameList, g_pstGpsFrameList);
                s32IndexUpdateCnt = 0;
                /* 重新开始分配一个新录像文件 */
                s32Ret = STOR_AllocOneRecBlkFile(s32Chn,ulSecond);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("STOR_AllocOneRecBlkFile failed");
                    pthread_mutex_unlock(&pstPart->partSwitchLock);
                    return NULL;
                }
                //更新文件开始时间
                pthread_mutex_unlock(&pstPart->partSwitchLock);

            }
            else
            {
                AV_PRINT("CHN %d STOR_WrRecFile_2 err",s32Chn);
                break;
            }
        }

        /*更新索引文件 */
        if (s32IndexUpdateCnt++>=5000)
        {
            STOR_UpdateIndexfile(s32Chn);
            AV_PRINT("s32Chn(%d),STOR_UpdateIndexfile OK", s32Chn);
            s32IndexUpdateCnt = 0;
        }
        else
            TM_USleep(0,10*1000);
    }
    //去使能Fifo
    mem_DisEnable(hList);
    //解除绑定，FIFO不在接收音视频帧
    mem_UnBind(hList,g_ChnDataCenter[2*s32Chn]);
    s32Ret = HI_SUCCESS;
    //将缓冲里的数据写入录像文件
    if(stRecCache.m_iBufferPos != 0)
    {
        AV_PRINT("CHN(%d) flush cache to file.total %d",s32Chn,stRecCache.m_iBufferPos);
        s32Ret = STOR_CopyFrameDataToFile(s32Chn,(HI_CHAR *)(stRecCache.m_pcBuffer),stRecCache.m_iBufferPos \
                                          ,g_pstFileIFrameList, g_pstGpsFrameList);
    }
    //将FIFO中缓存的帧写入录像文件
    if(s32Ret == HI_SUCCESS)
    {
        AV_PRINT("CHN(%d) flush list data to file,frame count :%d",s32Chn,mem_GetPackageNum(hList));
        STOR_WrCachDataToRecFile(s32Chn, hList, g_pstFileIFrameList, g_pstGpsFrameList);
    }
    //销毁FIFO
    mem_Destory(hList);

    free(stRecCache.m_pcBuffer);
    AV_PRINT("%d STOR_UpdateIndexfile up...",s32Chn);
    /* 更新视频索引文件和视频块文件 */
    s32Ret = STOR_UpdateIndexfile(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("%d STOR_UpdateRecFile failed",s32Chn);
    }
    sync();
    sleep(2);
    sync();
    sleep(2);
    AV_PRINT("%d STOR_UpdateIndexfile over...",s32Chn);
    s32Ret = STOR_RecThdExitbyChn(s32Chn);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_RecThdExitbyChn failed");
    }

    /* 将录像开始事件写入到日志文件 */
#ifdef NAND_LOG
    s32Ret = HDD_WrRecStat(HDD_GetDiskFSM(), HDD_GetHddEvent(DISK_EVENT_REC), s32Chn, (HI_CHAR *)APP_RECODE_LOG);
    if (0 != s32Ret)
    {
        AV_PRINT("HDD_WrRecStat err(%d)", s32Ret);
    }
#endif

    s32Ret = HDD_WrRecStat(HDD_GetDiskFSM(), HDD_GetHddEvent(DISK_EVENT_REC), s32Chn,  (HI_CHAR *)APP_REC_ERR_LOG);
    if (0 != s32Ret)
    {
        AV_PRINT("HDD_WrRecStat err(%d)", s32Ret);
    }
    return NULL;
}

/*****************************************************************************
 函 数 名  : STOR_HddPartCheck
 功能描述  : 分区检查
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_HddPartCheck()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HDD_PART_ST *pstPart = STOR_GetHddPart();

    STOR_ExitPartInfo(pstPart);

    /*分区信息初始化*/
    s32Ret = STOR_InitPartInfo(pstPart);
    if (HI_SUCCESS != s32Ret )
    {
        return HI_FAILURE;
    }

    /*分区索引检查*/
    s32Ret = STOR_ChkPartIdxFile();
    if (HI_SUCCESS != s32Ret )
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : STOR_StartStoreThd
 功能描述  : 按通道启动存储线程
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_StartStoreThd(HI_VOID)
{
    HI_S32 s32Chn = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid[APP_VENC_CHNCNT];
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;  /*单通道录像文件管理 */
	HI_S32 s32ViCnt = APP_VENC_CHNCNT;
    for (s32Chn = 0; s32Chn < s32ViCnt; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        /*开启录像*/
        if (pstChnStorMgmt->bEnable)
        {
            pstChnStorMgmt->iChan = s32Chn;

            HDD_SetHddEvent(DISK_EVENT_REC, HI_TRUE);
            s32Ret = pthread_create(&tid[s32Chn], NULL, STOR_StoreThdByChn, (HI_VOID*)pstChnStorMgmt);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("create STOR_StoreRecThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
                return HI_FAILURE;
            }
            s32Ret = pthread_detach(tid[s32Chn]);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("detach STOR_StoreRecThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
                return HI_FAILURE;
            }
        }
        else
            AV_PRINT("CHN(%d) disenable record",s32Chn);
    }
    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : STOR_StartStore
 功能描述  : 存储线程
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月14日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 STOR_StartStore(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    /*分区检查*/
    s32Ret = STOR_HddPartCheck();
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    /*初始化录像*/
    s32Ret = STOR_RecFileInit();
    if (HI_SUCCESS != s32Ret )
    {
        AV_PRINT("STOR_StartStore failed");
        return HI_FAILURE;
    }

    /* 是否需要切换分区 */
    s32Ret = STOR_CheckSwitchPart();
    if (STOR_SWITCH_PART == s32Ret)
    {
        AV_PRINT("STOR_CheckSwitchPart: STOR_SWITCH_PART");
        s32Ret = STOR_StartSwitchPart();
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_SwitchPart failed");
            return HI_FAILURE;
        }
    }

    AV_PRINT("::::::All CheckFile Ok, STOR_StoreThdByChn Start Now::::::");

    s32Ret = STOR_StartStoreThd();
    if (HI_SUCCESS != s32Ret )
    {
        AV_PRINT("STOR_StartStoreThd failed");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_VOID STOR_SetDiskLED(int flag)
{
    static int led = 0;
    if(flag)
    {
        if(led)
            COMMCTL_Power(POWER_LED2,POWER_ON);
        else
            COMMCTL_Power(POWER_LED2,POWER_OFF);
        led = 1-led;
    }
    else
        COMMCTL_Power(POWER_LED2,POWER_OFF);
}
/*****************************************************************************
 函 数 名  : STOR_StoreRecThd
 功能描述  : 视频存储线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *STOR_StoreRecThd(HI_VOID *pArg)
{
    HI_S32 s32Ret = 0;
    AV_PRINT("STOR_StoreRecThd PID@%s pid is %d", __FUNCTION__, getpid());
    while (1)
    {
        /*通过参数获取通道录像是否开启 */
        STOR_GetChnRecStartFlag();
        if (HDD_GetDiskStat())
        {
            if (!Cam_En_Flag)
            {
                /* 如果没有通道录像，则停止录像 */
                AV_PRINT("Stop record, Cam_En_Flag = 0x%x", Cam_En_Flag);
                //sleep(1);
                TM_USleep(1,0);
                continue;
            }

            /*控制录像重复启动 */
            if (HDD_GetHddEvent(DISK_EVENT_REC))
            {
                //usleep(500*1000);
                TM_USleep(0,500*1000);
                /*开启LED灯闪烁 */
                STOR_SetDiskLED(1);
                continue;
            }

            s32Ret = STOR_StartStore();
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("STOR_VideoStore failed");
#ifdef NAND_LOG
                s32Ret = HDD_WrRecStat(DISK_STAT_WORKING, HDD_GetHddEvent(DISK_EVENT_REC), __LINE__, (HI_CHAR *)APP_RECODE_LOG);
                if (0 != s32Ret)
                {
                    AV_PRINT("HDD_WrRecStat err 0x%x", s32Ret);
                }
#endif
            }
            else
            {
                AV_PRINT("STOR_VideoStore success");
            }
        }
        else
        {
            if (!HDD_GetHddEvent(DISK_EVENT_REC))/*所有的录像线程已退出*/
            {
                AV_PRINT_ONE((HI_CHAR *)"REC THREADS EXIT, WAIT UNTIL DISK_STAT_WORKING AGAIN.", 1);
                STOR_CloseIdxFile();
                /*关闭LED灯 */
                STOR_SetDiskLED(0);
            }
        }
        //sleep(1);//以后用检测代替等待
        TM_USleep(1,500000);
    }

    return NULL;
}
#endif

/*****************************************************************************
 函 数 名  : HI3531_StartHddMng
 功能描述  : 启动硬盘管理
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_StartHddMng(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t hddMngTid;

    //初始化硬盘状态
    HDD_Init();

    s32Ret = pthread_create(&hddMngTid, NULL, HDD_Check, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("create HddManage[tid-%d] failed, err-%s!", (HI_S32)hddMngTid, ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = pthread_detach(hddMngTid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach HddManage[tid-%d] failed, err-%s!", (HI_S32)hddMngTid, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI3531_StartAvencRec
 功能描述  : 启动视频存储
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_StartAvencRec(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_t tid = -1;

    s32Ret = pthread_create(&tid, NULL, STOR_StoreRecThd, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("create STOR_StoreRecThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }
    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("detach STOR_StoreRecThd[tid-%d] failed, err-%s!", (HI_S32)tid, ERR_STR);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}




