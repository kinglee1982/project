
/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : tm_app_mp3play.c
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2012��8��7�����ڶ�
  ����޸�   :
  ��������   : MP3���Ź���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��8��7�����ڶ�
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/

#include "av_public_head.h"
#include "av_common_31.h"

#include "mp3dec.h"
#include "av_public_ctrl_cmd.h"
#include "av_video_proc.h"
#include "av_audio_init.h"
#include "av_mem.h"
#include "av_mp3_play.h"


#define MP3_MAINBUF_SIZE	16*1024                      /* min. size. of input buffer. UNIT:bytes */
#define MP3_MAX_NCHANS		2	                      /* max number of channels�� dont supoort multichannel  */
#define MP3_MAX_OUT_NSAMPS	1152                      /* max output samples per-frame, per-channel */
HI_S32 g_Mp3FileStat = 0;
extern HI_S32 g_iVideoStatus;
extern ADEC_CHN g_iAdChn;   /* ����ͨ�� */

#if defined(OUTPUT_DEBUG_INFO)
	#define MP3_DEBUG_INFO(info) do{AV_PRINT info;}while(0)
#else
	#define MP3_DEBUG_INFO(info) do{}while(0)
#endif
#define APP_MP3PLAY_VOL_MAX   31
/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

extern HI_S32 g_iDevErrStatue;          /* �豸����״̬ */

/*****************************************************************************
 �� �� ��  : MP3_SetGlobalMemData
 ��������  : дȫ�ֱ����ڴ�
 �������  : HI_VOID *piGlobMem
             HI_VOID *pData
             pthread_rwlock_t *pstLock
             HI_S32 s32Len
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��7�����ڶ�
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 MP3_SetGlobalMemData(HI_VOID *piGlobMem, const HI_VOID *pData,pthread_rwlock_t *pstLock, HI_S32 s32Len)
{
    HI_S32 s32Ret;

    if ((NULL == piGlobMem) ||
        (NULL == pData) ||
        (NULL == pstLock) ||
        (0 == s32Len))
    {
		AV_PRINT( "Input param error! p1=0x%x, p2=0x%x, p3=0x%x, p4=%d",
                   (HI_U32 )piGlobMem, (HI_U32 )pData,
                   (HI_U32 )pstLock, s32Len);

        return HI_FAILURE;
    }
    s32Ret = pthread_rwlock_wrlock(pstLock);
    if(s32Ret)
	{
        AV_PRINT( "err pthread_rwlock_rdlock,exit");
		goto ERROUT;
	}

    memcpy(piGlobMem, pData, s32Len);
    s32Ret = pthread_rwlock_unlock(pstLock);
    if(s32Ret)
	{
        AV_PRINT( "err pthread_unrwlock_rdlock,exit");
		goto ERROUT;
	}

    return HI_SUCCESS;

    ERROUT:
    AV_PRINT( "err happen,exit");
	exit(EXIT_FAILURE);
}

/*****************************************************************************
 �� �� ��  : MP3_GetGlobalMemData
 ��������  : ��ȡȫ�ֱ�������
 �������  : const HI_VOID *piGlobMem
             HI_VOID *pData
             pthread_rwlock_t *pstLock
             HI_S32 s32Len
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��7�����ڶ�
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 MP3_GetGlobalMemData(const HI_VOID *piGlobMem, HI_VOID *pData,pthread_rwlock_t *pstLock, HI_S32 s32Len)
{
    HI_S32 s32Ret;

    if ((NULL == piGlobMem) ||
        (NULL == pData) ||
        (NULL == pstLock) ||
        (0 == s32Len))
    {
		AV_PRINT( "Input param error! p1=0x%x, p2=0x%x, p3=0x%x, p4=%d",
                   (HI_U32 )piGlobMem, (HI_U32 )pData,
                   (HI_U32 )pstLock, s32Len);

        return HI_FAILURE;
    }
    s32Ret = pthread_rwlock_rdlock(pstLock);
    if(s32Ret)
	{
        AV_PRINT( "err pthread_rwlock_rdlock,exit");
		goto ERROUT;
	}

    memcpy(pData, piGlobMem, s32Len);
    s32Ret = pthread_rwlock_unlock(pstLock);
    if(s32Ret)
	{
        AV_PRINT( "err pthread_unrwlock_rdlock,exit");
		goto ERROUT;
	}

    return HI_SUCCESS;

    ERROUT:
	AV_PRINT( "err happen,exit");
	exit(EXIT_FAILURE);
}
/*****************************************************************************
 Prototype    : FillReadBuffer
 Description  : ���mp3���ݻ�����
 Input        : HI_U8 *readBuf----------���ڱ����ȡ��������
                HI_U8 *readPtr----------ָ��ǰ��ȡ������λ��-���涼��δ��������
                HI_S32 bufSize----------readBuf�ĳ���
                HI_S32 bytesLeft--------readPtrָ���δ��������ݳ���
                FILE *infile------------mp3�ļ����
 Output       : None
 Return Value : static

  History        :
  1.Date         : 2015/3/10
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
static HI_S32 FillReadBuffer(HI_U8 *readBuf, HI_U8 *readPtr, HI_S32 bufSize, HI_S32 bytesLeft, FILE *infile)
{
	HI_S32 nRead = 0;
	//HI_S32 nRead2 = 0; /* (L40186) */

	/* move last, small chunk from end of buffer to start, then fill with new data */
    /* ����û�н�������� */
	memmove(readBuf, readPtr, bytesLeft);
	nRead = fread(readBuf + bytesLeft, 1, bufSize - bytesLeft, infile);

	/* zero-pad to aHI_VOID finding false sync word after last frame (from old data in readBuf) */
    if (nRead < bufSize - bytesLeft)
    {
		//AV_PRINT("***FillReadBuffer! nRead=%d, bytesLeft=%d", nRead, bytesLeft);

        memset(readBuf + bytesLeft + nRead, 0, bufSize - bytesLeft - nRead);
    }

    return nRead;
}
/************************************************************
��������: ����mp3ԭʼ����
�������:Chan -- ͨ����
			pInfo--mp3 ֡ͷ��Ϣ
			pBuf--����ԭʼ����
�������:��
���ز���:�ɹ�����HI_SUCCESS�����򷵻�����ֵ
**********************************************************/
static inline HI_S32 PlayRawData(AO_CHN Chan, MP3FrameInfo *pInfo, HI_U8 *pBuf, HI_S32 DataLen)
{
	HI_S32 Retval = -1;
    HI_S32 i = 0;
    short *pusTmpData = NULL;
    HI_U8 ucSTmp[2];
    float fTmpData = 0.0;

	AUDIO_FRAME_S stAudioFrame;
    HI_U8 *pucTemp =NULL;
	stAudioFrame.enBitwidth = AUDIO_BIT_WIDTH_16;//AUDIO_BIT_WIDTH_16;
	stAudioFrame.enSoundmode = AUDIO_SOUND_MODE_MONO;//(pInfo->nChans == 1) ? AUDIO_SOUND_MODE_MONO : AUDIO_SOUND_MODE_STEREO;
	HI_S32 s32Mp3Vol;
	RW_LOCK_T *pMp3VolLock = NULL;
	HI3531_GetLock(LOCK_MP3_VOL_E, &pMp3VolLock);

	HI3531_GetLockStat(pMp3VolLock, &s32Mp3Vol);
    //������������ʱ�����ʧ��-����
    pucTemp = (HI_U8 *)pBuf;
    for (i = 0; i < DataLen; )
    {
        ucSTmp[1] = *(HI_U8 *)(pucTemp + 1);
        ucSTmp[0] = *(HI_U8 *)(pucTemp);
        pusTmpData = (short *)ucSTmp;
        fTmpData = *pusTmpData;
        fTmpData = (fTmpData*s32Mp3Vol)/16;
        *pusTmpData = (short)fTmpData;
        //printf("***********************fTmpData = %f, s32Mp3Vol = %d\n",fTmpData,s32Mp3Vol);

        if ( *pusTmpData >= 0x7FFF )
        {
            *pusTmpData = 0x7FFF;
        }
        else
        {
            if ( *pusTmpData < -32767 )
            {
                *pusTmpData = -32767;
            }
        }

        memcpy(pucTemp, &ucSTmp[0], sizeof(HI_CHAR));
        memcpy(pucTemp+1, &ucSTmp[1], sizeof(HI_CHAR));

        i = i+2;
        pucTemp = pucTemp + 2;
    }

    stAudioFrame.pVirAddr[0] = pBuf;
	stAudioFrame.u32Len = DataLen;

    Retval = HI_MPI_AO_SendFrame(Chan, 0, &stAudioFrame, (HI_BOOL)-1);/* send audio frme to ao */
	if(Retval != 0)
	{
		if(Retval == (HI_S32)0xa016800e)
		{
			MP3_DEBUG_INFO(("HI_MPI_AO_SendFrame buffer is NULL.\n"));
		}
		else
		{
			MP3_DEBUG_INFO(("FUNCTION(%s),LINE(%d), err 0x%x\n",__FUNCTION__, __LINE__,Retval));
		}
		AV_PRINT("HI_MPI_AO_SendFrame DataLen:%d %#x",DataLen,Retval);
	}

    return 0;
}

#define	SYNCWORDH			0xff
#define	SYNCWORDL			0xf0
static int MP3FindSyncWord_NEW(unsigned char *buf, int nBytes)
{
	int i;

	/* find byte-aligned syncword - need 12 (MPEG 1,2) or 11 (MPEG 2.5) matching bits */
	for (i = 0; i < nBytes - 1; i++) {
		if ( (buf[i+0] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL && buf[i+1]!= 0xfe)
			return i;
	}

	return -1;
}
/*****************************************************************************
 Prototype    : MP3_PlayMp3File
 Description  : ����һ��mp3�ļ�
 Input        : const char *pMp3file------�ļ���(����ȫ·��)
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/3/10
    Author       : wuquanwei
    Modification : Created function

*****************************************************************************/
int MP3_PlayMp3File(const char *pMp3file)
{
	int bytesLeft = 0, nRead = 0, err = -1, offset = 0;
	unsigned char readBuf[MP3_MAINBUF_SIZE] = {0}, *readPtr=NULL;
	short outBuf[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP] = {0};
    short *psDecData = outBuf;//psDecData����ָ����������ݻ�����
    short stereoOutBuf[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP] = {0};    //������������������ݱ���
    int   iDataLen = 0;    //��������ݳ���
	FILE  *infile;         //mp3�ļ����
	MP3FrameInfo mp3FrameInfo;
	HMP3Decoder hMP3Decoder;
    int sampsNum = 0;

    HI_S32 s32Mp3PlayCtrl = 0;
	RW_LOCK_T *pMp3PlayCtrlLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_CTRL_E, &pMp3PlayCtrlLock);
    /*���ڱ���mp3����״̬*/
	RW_LOCK_T *pMp3PlayStatLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_STAT_E, &pMp3PlayStatLock);


	infile = fopen(pMp3file, "rb");
	if (!infile)
    {
		printf("#[av]: in playMp3File, file open error\n");
		return -1;
	}

	if ( ( hMP3Decoder = MP3InitDecoder() ) == 0 )
    {
        printf("[av]: in playMp3File, MP3FindSyncWord ret: %d",offset);
    	fclose(infile);
    	return -1;
    }

	readPtr = readBuf;

	while ( 1 )
	{
        /*��Ͽ���*/
        HI3531_GetLockStat(pMp3PlayCtrlLock, &s32Mp3PlayCtrl);
        if ( APP_MP3PLAYCTLRUN != s32Mp3PlayCtrl )
        {
            printf("[av]: in playMp3File, ctrl play cmd: %d\n",s32Mp3PlayCtrl);
            AV_PRINT("MP3 play status=%d!", pMp3PlayStatLock->stat);
            HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);

        	MP3FreeDecoder(hMP3Decoder);
        	fclose(infile);
            return MP3_RET_INTERRUPT;
        }

		/* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */
		if ( bytesLeft < 2 * MAINBUF_SIZE )
		{
			nRead = FillReadBuffer(readBuf, readPtr, MP3_MAINBUF_SIZE, bytesLeft, infile);
			bytesLeft += nRead;
			readPtr    = readBuf;
		}
		/* find start of next MP3 frame - assume EOF if no sync found */
		offset = MP3FindSyncWord_NEW(readPtr, bytesLeft);
		if (offset < 0)
		{
            printf("MP3FindSyncWord ret: %d\n",offset);
        	fclose(infile);
            MP3FreeDecoder(hMP3Decoder);
        	return MP3_RET_ERROR;
		}
		readPtr += offset;
		bytesLeft -= offset;

        err = MP3Decode(hMP3Decoder, &readPtr, &bytesLeft, outBuf, 0);
		if (err)
		{
		    printf("MP3Decode return error! ret = %d\n",err);
        	fclose(infile);
            MP3FreeDecoder(hMP3Decoder);
        	return MP3_RET_ERROR;
		}

		/* no error */
		MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);

        int i = 0;
		if ( mp3FrameInfo.outputSamps == 576 ) //������
		{
			sampsNum = (mp3FrameInfo.outputSamps);
            psDecData = outBuf;
		}
		else if ( mp3FrameInfo.outputSamps == 1152 )//˫����
		{
			sampsNum = (mp3FrameInfo.outputSamps / 2);
			for (i = 0; i < mp3FrameInfo.outputSamps; i += 2)
			{
				memcpy(stereoOutBuf + i / 2, outBuf + i + 1, 2);
			}
            psDecData = stereoOutBuf;
		}
        iDataLen = ( mp3FrameInfo.bitsPerSample / 8 ) * sampsNum;

        PlayRawData(0, &mp3FrameInfo, (HI_U8 *)psDecData, iDataLen);

	    if ( 0 == bytesLeft ) /*�ж��Ƿ񲥷Ž���*/
	    {
            break;
	    }
        TM_USleep(0,1000 *10);
	} ;

	MP3FreeDecoder(hMP3Decoder);
	fclose(infile);

	return MP3_RET_SUCCESS;

}
/*****************************************************************************
 �� �� ��  : MP3_PlayThread
 ��������  : MP3�����߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��7��30������һ
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *MP3_PlayThread(HI_VOID *pArg)
{
	AV_PRINT( "PID@%s pid is %d,tid %d", __FUNCTION__,getpid(),gettid());

    HI_S32 iRet = -1;
    HI_CHAR cTmpMp3PlayList[10][64];   /* MP3�ļ��б� */
    IFLY_MP3_PLAY_T stMp3PlayStruct; /* MP3���Žṹ�� */
    HI_U32 i = 0;
    HI_U32 iFileIndex = 0;   /* �ļ�����ֵ */

    /*���ڱ���mp3����״̬*/
	RW_LOCK_T *pMp3PlayStatLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_STAT_E, &pMp3PlayStatLock);

	HI_S32 s32Mp3PlayCtrl = 0;
    /*���ڱ���mp3���ſ���*/
	RW_LOCK_T *pMp3PlayCtrlLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_CTRL_E, &pMp3PlayCtrlLock);

    /*���ڱ���mp3�ļ�������ͷ�Ľṹ��*/
	RW_LOCK_T *pMp3PlayStructLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_STRUCT_E, &pMp3PlayStructLock);

    /*���ڱ���mp3�ļ�������*/
	RW_LOCK_T *pMp3PlayListLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_LIST_E, &pMp3PlayListLock);
    while(1)
    {
        /* ��ʱ200ms */
        //usleep(100000);
        TM_USleep(0,1000 *150);
		HI3531_GetLockStat(pMp3PlayCtrlLock, &s32Mp3PlayCtrl);
        /* ��ʼ����MP3 */
        if ( CTRL_CMD_PLAYMP3FILE_START == s32Mp3PlayCtrl )
        {
			HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYRUN);
    		AV_PRINT("MP3 play status=%d!", APP_MP3PLAYRUN);

            memset(cTmpMp3PlayList,  0, sizeof(cTmpMp3PlayList));
            memset(&stMp3PlayStruct, 0, sizeof(stMp3PlayStruct));

			HI3531_GetLockData(pMp3PlayStructLock, (HI_VOID *)&stMp3PlayStruct, sizeof(IFLY_MP3_PLAY_T));
            if (stMp3PlayStruct.mp3FileNum > APP_MP3PLAYNAMENUM)
            {
        		AV_PRINT( "Mp3 play name number error, stTmpMp3Struct.mp3FileNum = %d",
                                            stMp3PlayStruct.mp3FileNum);
				HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);
        		AV_PRINT( "MP3 play status=%d!", APP_MP3PLAYIDLE);
				HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_STOP);
                continue;
            }
            else
            {
				HI3531_GetLockData(pMp3PlayStructLock, (HI_VOID *)&stMp3PlayStruct, sizeof(IFLY_MP3_PLAY_T));
				HI3531_GetLockData(pMp3PlayListLock, (HI_VOID *)cTmpMp3PlayList,
										stMp3PlayStruct.mp3FileNum*APP_MP3PLAYFILELEN);

                stMp3PlayStruct.mp3_name = (HI_CHAR *)cTmpMp3PlayList;

                for(i = 0; i < stMp3PlayStruct.mp3FileNum; i++)            // ��ӡMP3�ļ��б�
            	{
            		AV_PRINT("--mp3 file %s\n", cTmpMp3PlayList[i]);
            	}

        		AV_PRINT("---mp3file.mp3FileNum = %d", stMp3PlayStruct.mp3FileNum);
				HI3531_SetLockStat(pMp3PlayCtrlLock, APP_MP3PLAYCTLRUN);
            }
        }
        else
        {
            if ( CTRL_CMD_PLAYMP3FILE_STOP == s32Mp3PlayCtrl )
            {
				HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);
                continue;
            }
            else if ( CTRL_CMD_PLAYMP3FILE_PAUSE == s32Mp3PlayCtrl )
            {
				HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);
        		AV_PRINT("MP3 play status=%d!", APP_MP3PLAYIDLE);
                continue;
            }
        }

		HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYRUN);

        for(iFileIndex = 0; iFileIndex < stMp3PlayStruct.mp3FileNum; iFileIndex++)
        {
    		AV_PRINT("***input file: %s***", cTmpMp3PlayList[iFileIndex]);
            //call mp3 play function
            iRet = MP3_PlayMp3File(cTmpMp3PlayList[iFileIndex]);
            TM_USleep(0,10*1000);
            if ( MP3_RET_INTERRUPT == iRet ) //��Ӵ�ϴ���
			{
				break;
			}
    		//AV_PRINT("***input file: %s***", cTmpMp3PlayList[iFileIndex]);
            //call mp3 play function
        }

        if ( MP3_RET_SUCCESS ==  iRet )
        {
            //usleep(15*1000*100);
            TM_USleep(1,500*1000);
            //AV_PRINT("MP3 play status=%d!", pMp3PlayStatLock->stat);
        }

        AV_PRINT("MP3 play status=%d!", pMp3PlayStatLock->stat);
		HI3531_GetLockStat(pMp3PlayCtrlLock, &s32Mp3PlayCtrl);
        if (s32Mp3PlayCtrl == CTRL_CMD_PLAYMP3FILE_PAUSE)
    	{
    		stMp3PlayStruct.mp3_name    = (HI_CHAR *)cTmpMp3PlayList[iFileIndex];
    		stMp3PlayStruct.mp3FileNum -= iFileIndex;

			HI3531_SetLockData(pMp3PlayListLock, (HI_VOID *)stMp3PlayStruct.mp3_name,
									stMp3PlayStruct.mp3FileNum * APP_MP3PLAYFILELEN);
			HI3531_SetLockData(pMp3PlayStructLock, (HI_VOID *)&stMp3PlayStruct, sizeof(IFLY_MP3_PLAY_T));
    	}

        AV_PRINT("MP3 play status=%d!", pMp3PlayStatLock->stat);
		HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_STOP);
        HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);

    }

    AV_PRINT( "err happen,exit");
	exit(EXIT_FAILURE);
}
/*****************************************************************************
 �� �� ��  : MP3_OpenPlayBack
 ��������  : ��MP3���Żط�ͨ��
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��22��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 MP3_OpenPlayBack(HI_VOID)
{
    HI_S32 s32Ret = 0;

    /* ����ƵԤ�� */
//    s32Ret = HI3531_SetAudioPreview(HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "close audio preview failed!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : MP3_ClosePlayBack
 ��������  : �ر�MP3�ط�ͨ��
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��22��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 MP3_ClosePlayBack(HI_VOID)
{
    HI_S32 s32Ret = 0;
    /* �ر���ƵԤ�� */
//    s32Ret = HI3531_SetAudioPreview(HI_FALSE);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "open audio preview failed!");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#if 0
/*****************************************************************************
 �� �� ��  : APP_IpSpkAdecThread
 ��������  : IP�Խ����봦���߳�
 �������  : HI_VOID *pArg
 �������  : ��
 �� �� ֵ  : HI_VOID



 �޸���ʷ      :
  1.��    ��   : 2012��8��23��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_VOID *APP_IpSpkAdecThread(HI_VOID *pArg)
{
    HI_S32 s32ret;
    AUDIO_STREAM_S stAudioStream;
    HI_S32 s32Len = 640;      /* 320*16bit/8bit=640B */
    HI_U8 *pu8AudioStream = NULL;
    HI_S32 s32Ret = 0;
    HI_S32 u32Len = 0;
    HI_S32 s32Counter = 0;
    IP_BIG_PKT_HEAD_T *pstBigPackHead = NULL;  /* ���֡�İ�ͷ���� */
    SHM_HEAD_T *pstShareMemHead = NULL; /* �����ڴ��ͷ������ */
    HI_U8 *pucTmp = NULL;
    HI_S32 i = 0;

	HI_S32 s32IpSpeak;
	RW_LOCK_T *pIpSpeakLock = NULL;

	SHM_SEM_MNG_T *pstShmSemMng = NULL;
	SEM_MNG_T *pstSemMng = NULL;
	SHM_MNG_T *pstShmMng = NULL;

	HI3531_GetShmSemMng(IPC_IPSPK_SND_E, &pstShmSemMng);
	if (NULL == pstShmSemMng)
	{
		AV_PRINT( "HI3531_GetShmSemMng err! ");
        return NULL;
	}

	pstShmMng = &pstShmSemMng->shmMng;
	pstSemMng = &pstShmSemMng->semMng;
    s32Ret = MEM_ShmDataInit(&pstShmMng->shmAttr, pstSemMng->sysSemId);
    if (HI_SUCCESS != s32Ret)
    {
		AV_PRINT( "MEM_ShmDataInit err! s32Ret = %d", s32Ret);
        return NULL;
    }

	HI3531_GetLock(LOCK_IP_SPEAK_E, &pIpSpeakLock);

    pu8AudioStream = (HI_U8*)malloc(sizeof(HI_U8)*MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        AV_PRINT( "Read mem share error!");
        return NULL;
    }

	AV_PRINT("PID@%s pid is %d", __FUNCTION__,getpid());

    u32Len = 168;
    usleep(10000);
    while (1)
    {
    	HI3531_GetLockStat(pIpSpeakLock, &s32IpSpeak);
		if (APP_IPSPEAKRUN != s32IpSpeak)
		{
			break;
		}

        /* ��ȡ640�ֽ� */
        s32Ret = MEM_RdShm(&pstShmMng->shmAttr, pu8AudioStream,
                                     955 + sizeof(SHM_HEAD_T), pstSemMng->sysSemId, &s32Counter);
        if (HI_SUCCESS != s32Ret)
        {
            if (HI_NO_DATA == s32Ret)
            {
                usleep(2000);
            }
            else
            {
                AV_PRINT( "Read mem share error!");
            }
            continue;
        }

        pstShareMemHead = (SHM_HEAD_T *)pu8AudioStream;
        if (pstShareMemHead->dataSize != (sizeof(SHM_HEAD_T) + 955))
        {
    		AV_PRINT( "Read mem share error!DataSize=%d", pstBigPackHead->m_dwDataSize);
            continue;
        }

        for (i = 0; i< 5; i++)
        {
            pucTmp = (HI_U8 *)((HI_U8 *)pu8AudioStream + sizeof(SHM_HEAD_T)
                                       + sizeof(VOIP_HEAD_T) + i*(sizeof(VOIP_HEAD_T) + 168));
            s32Len = 168;
            /* here only demo adec streaming sending mode, but pack sending mode is commended */
            /* read from file */
            stAudioStream.pStream = pucTmp;
            stAudioStream.u32Len = s32Len;

            s32ret = HI_MPI_ADEC_SendStream(g_iAdChn, &stAudioStream, HI_IO_NOBLOCK);
            if (s32ret)
            {
        		AV_PRINT( "HI_MPI_ADEC_SendStream chn:%d err:0x%x", g_iAdChn, s32ret);
                free(pu8AudioStream);
                pu8AudioStream = NULL;
                APP_StopIPSpeak();
				HI3531_SetLockStat(pIpSpeakLock, APP_IPSPEAKIDLE);
                return NULL;
            }
            usleep(12000);
        }
    }

    free(pu8AudioStream);
    pu8AudioStream = NULL;
	HI3531_SetLockStat(pIpSpeakLock, APP_IPSPEAKIDLE);
    return NULL;
}


/*****************************************************************************
 �� �� ��  : APP_StopIPSpeak
 ��������  : ֹͣIP�Խ�
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��23��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_StopIPSpeak(HI_VOID)
{
    HI_S32 s32Ret = 0;

    /* ֹͣIP�Խ������� */
    //s32Ret = APP_StopAdec();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "Enable mike failed!");

        return HI_FAILURE;
    }

    /* BEGIN: Added by wuquanwei, 2015/3/5   PN:�޸�mp3����.201503001 */
    /*ֹͣip�Խ�ʱ�ָ�ao��ÿ֡��������*/
    HI3521A_AoExit();
    HI3521A_AoInit(DEFAULT_DATA_PER_FRAM);
    /* END:   Added by wuquanwei, 2015/3/5   PN:�޸�mp3����.201503001 */

    return HI_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : APP_StartIPSpeak
 ��������  : ��ʼIP�Խ�
 �������  : HI_VOID
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��8��22��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_StartIPSpeak(HI_VOID)
{
    HI_S32 s32Ret = 0;
	pthread_t ipSpkWrTid;
    pthread_attr_t stIpSpkWrAttr;
	SHM_SEM_MNG_T *pstShmSemSndMng = NULL;
	SEM_MNG_T *pstSemSndMng = NULL;
	SHM_MNG_T *pstShmSndMng = NULL;

    SHM_SEM_MNG_T *pstShmSemRcvMng = NULL;
	SEM_MNG_T *pstSemRcvMng = NULL;
	SHM_MNG_T *pstShmRcvMng = NULL;

    /* BEGIN: Added by wuquanwei, 2015/3/5   PN:�޸�mp3����.201503001 */
    /*ip�Խ�Ҫ��������ao��ÿ֡��������*/
    HI3531_AoExit();
    HI3531_AoInit(VOIP_DATA_PER_FRAM);
    /* END:   Added by wuquanwei, 2015/3/5   PN:�޸�mp3����.201503001 */

    /* IP�Խ�ǰ��ʼ������ */
    //s32Ret = APP_StartAdec();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "able mike failed!");
        return HI_FAILURE;
    }

	HI3531_GetShmSemMng(IPC_IPSPK_SND_E, &pstShmSemSndMng);
	if (NULL == pstShmSemSndMng)
	{
		AV_PRINT( "HI3531_GetShmSemMng err! ");
        return HI_FAILURE;
	}

	pstShmSndMng = &pstShmSemSndMng->shmMng;
	pstSemSndMng = &pstShmSemSndMng->semMng;
    s32Ret = MEM_ShmDataInit(&pstShmSndMng->shmAttr, pstSemSndMng->sysSemId);
    if (HI_SUCCESS != s32Ret)
    {
		AV_PRINT( "MEM_ShmDataInit err! s32Ret = %d", s32Ret);
        return HI_FAILURE;
    }

	HI3531_GetShmSemMng(IPC_IPSPK_RCV_E, &pstShmSemRcvMng);
	if (NULL == pstShmSemRcvMng)
	{
		AV_PRINT( "HI3531_GetShmSemMng err! ");
        return HI_FAILURE;
	}

	pstShmRcvMng = &pstShmSemRcvMng->shmMng;
	pstSemRcvMng = &pstShmSemRcvMng->semMng;
    s32Ret = MEM_ShmDataInit(&pstShmRcvMng->shmAttr, pstSemRcvMng->sysSemId);
    if (HI_SUCCESS != s32Ret)
    {
		AV_PRINT( "MEM_ShmDataInit err! s32Ret = %d", s32Ret);
        return HI_FAILURE;
    }

    /* ����IP�Խ���ȡ�����ڴ����ݣ����н����߳� */
    pthread_attr_init(&stIpSpkWrAttr);
    pthread_attr_setdetachstate(&stIpSpkWrAttr, PTHREAD_CREATE_DETACHED);
    s32Ret = pthread_create(&ipSpkWrTid, &stIpSpkWrAttr, APP_IpSpkAdecThread, NULL);
    if (s32Ret < 0)
    {
        AV_PRINT( "err creat APP_IpSpkAdecThread !");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif
/*****************************************************************************
 �� �� ��  : APP_AudioMsgProc
 ��������  : ��Ƶ��Ϣ������
 �������  : const MSG_QUE_BUF_DATA_T *pstRcvMsg
             MSG_QUE_T *pstSendMsg
 �������  : ��
 �� �� ֵ  :



 �޸���ʷ      :
  1.��    ��   : 2012��7��5��������
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
HI_S32 APP_AudioMsgProc(MSG_QUE_BUF_DATA_T *pstRcvMsg, MSG_QUE_T *pstSendMsg)
{
    MSG_QUE_BUF_DATA_T *pstTempMsg = NULL;        /* �ӻ�������ȡ��Ϣ���� */
    MSG_QUE_T *pstSendTmpMsg = NULL;            /* ������Ϣָ�� */
    MSG_QUE_BUF_DATA_T *pstSendBufferMsg = NULL;  /* ������Ϣ��bufferָ�� */
    IFLY_MP3_PLAY_T *pstTmpMp3Struct; /* MP3���Žṹ��ָ�� */
    HI_U16 u16GetCrc = 0;                        /* �����ȡ��CRCֵ */

    HI_S32 *piMp3PlayStatus = NULL;  /* mp3����״̬ */
    HI_S32 s32Ret = 0;
//    IFLY_AUDIO_ATTR_T *pstAudioPro = NULL;  /* ��Ƶ����ָ�� */
    MSG_RESULT_T *pMsgBackResult = NULL;  /* ��Ϣ���з������ݽṹ */
    HI_U16 *uspTmpCRC = NULL;
    HI_U16 usTmpCRC = 0;
    HI_U16 usNewCrc = 0;
    HI_S32 *piMp3FileStatue = NULL;

	HI_S32 s32Mp3PlayStat;
	RW_LOCK_T *pMp3PlayStatLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_STAT_E, &pMp3PlayStatLock);

	RW_LOCK_T *pMp3PlayCtrlLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_CTRL_E, &pMp3PlayCtrlLock);

	IFLY_MP3_PLAY_T stMp3PlayStruct;
	RW_LOCK_T *pMp3PlayStructLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_STRUCT_E, &pMp3PlayStructLock);

	RW_LOCK_T *pMp3PlayListLock = NULL;
	HI3531_GetLock(LOCK_MP3_PLAY_LIST_E, &pMp3PlayListLock);
	HI_CHAR mMp3PlayList[APP_MP3PLAYNAMENUM][APP_MP3PLAYFILELEN];

//	HI_S32 s32IpSpeak;
//	RW_LOCK_T *pIpSpeakLock = NULL;
//	HI3531_GetLock(LOCK_IP_SPEAK_E, &pIpSpeakLock);

	HI_S32 s32Mp3Vol;
	RW_LOCK_T *pMp3VolLock = NULL;
	HI3531_GetLock(LOCK_MP3_VOL_E, &pMp3VolLock);

    /* ������� */
    if ((NULL == pstRcvMsg) || (NULL == pstSendMsg))
    {
		AV_PRINT( "input error pointer,pstRcvMsg=0x%x, pstSendMsg=0x%x",
                    (HI_S32)pstRcvMsg, (HI_S32)pstSendMsg);

        return HI_FAILURE;
    }

    pstTempMsg = pstRcvMsg;
    pstSendTmpMsg = pstSendMsg;

    /* ����CRCУ�� */
    if (pstTempMsg->s32Len < 16)
    {
		AV_PRINT( "crc16  pstTempMsg->s32Len=0x%x", pstTempMsg->s32Len);

        return HI_FAILURE;
    }

	//AV_PRINT("crc16  pstTempMsg->s32Len=0x%x,CMD=%d", pstTempMsg->s32Len, pstTempMsg->s32Cmd);

    u16GetCrc = HI3531_CRC16((HI_U8 *)pstTempMsg, pstTempMsg->s32Len - 2);
    uspTmpCRC = (HI_U16 *)((HI_U8 *)pstTempMsg + pstTempMsg->s32Len - 2);
    usTmpCRC = *uspTmpCRC;
    if (usTmpCRC != u16GetCrc)
    {
    	AV_PRINT( "crc16 error crc16=0x%x, u16GetCrc=0x%x",
                    pstTempMsg->u16Crc, u16GetCrc);

        return HI_FAILURE;
    }

    pstSendTmpMsg->s32MsgType = pstTempMsg->s32RspType;
    pstSendBufferMsg = (MSG_QUE_BUF_DATA_T *)(pstSendTmpMsg->cBuffer);
    pstSendBufferMsg->s32Prio = pstTempMsg->s32Prio;

    switch (pstTempMsg->s32Cmd)
    {
        /* ��ʼ����MP3�ļ��б� */
        case CTRL_CMD_PLAYMP3FILE_START :
            /* ������Ϣ���� */
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_START;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            /* ��ȡMP3�����б���Ϣ */
            pstTmpMp3Struct = (IFLY_MP3_PLAY_T *)(pstTempMsg->cData);
            if (pstTmpMp3Struct->mp3FileNum >= APP_MP3PLAYNAMENUM)
            {
                pMsgBackResult->s32Result = HI_FAILURE;
            	AV_PRINT( "mp3 file number error mp3FileNum=%d",pstTmpMp3Struct->mp3FileNum);
            }
            else
            {
                /* ��ǰMP3����״̬ */
				HI3531_GetLockStat(pMp3PlayStatLock, &s32Mp3PlayStat);
                if (s32Mp3PlayStat)
                {
	                HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_STOP);

                    memset(mMp3PlayList, 0, sizeof(mMp3PlayList));
					HI3531_SetLockData(pMp3PlayListLock, (HI_VOID *)mMp3PlayList, sizeof(mMp3PlayList));
                    //usleep(100000);
                    TM_USleep(0,100000);
                }
				HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYRUN);

                pMsgBackResult->s32Result = HI_SUCCESS;
            }
            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));
			HI3531_SetLockData(pMp3PlayStructLock, (HI_VOID *)pstTmpMp3Struct, sizeof(IFLY_MP3_PLAY_T));
			HI3531_SetLockData(pMp3PlayListLock,
								(HI_VOID *)((char *)pstTmpMp3Struct + sizeof(IFLY_MP3_PLAY_T)),
								pstTmpMp3Struct->mp3FileNum * APP_MP3PLAYFILELEN);
			HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_START);
            break;

        /* ֹͣ����MP3�ļ� */
        case CTRL_CMD_PLAYMP3FILE_STOP :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_STOP;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            pMsgBackResult->s32Result = HI_SUCCESS;
            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);


            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));
			HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_STOP);
            /* BEGIN: Added by wuquanwei, 2015/4/8   PN:�������.20150408 */
			//HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);
			/* END:   Added by wuquanwei, 2015/4/8   PN:�������.20150408 */
            break;

        /* ��ͣ����MP3�ļ� */
        case CTRL_CMD_PLAYMP3FILE_PAUSE :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_PAUSE;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            pMsgBackResult->s32Result = HI_SUCCESS;
			HI3531_GetLockData(pMp3PlayStructLock, (HI_VOID *)pMsgBackResult->pData, sizeof(IFLY_MP3_PLAY_T));


			HI3531_GetLockData(pMp3PlayStructLock, (HI_VOID *)&stMp3PlayStruct, sizeof(IFLY_MP3_PLAY_T));

			HI3531_GetLockData(pMp3PlayListLock,
								(HI_VOID *)((IFLY_MP3_PLAY_T *)pMsgBackResult->pData+sizeof(IFLY_MP3_PLAY_T)),
								stMp3PlayStruct.mp3FileNum * APP_MP3PLAYFILELEN);

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

			HI3531_SetLockStat(pMp3PlayCtrlLock, CTRL_CMD_PLAYMP3FILE_PAUSE);

            /* BEGIN: Added by wuquanwei, 2015/4/8   PN:�������.20150408 */
			//HI3531_SetLockStat(pMp3PlayStatLock, APP_MP3PLAYIDLE);
            /* END:   Added by wuquanwei, 2015/4/8   PN:�������.20150408 */
            break;

        /* ��������ֵ */
        case CTRL_CMD_PLAYMP3FILE_SETVOLUME :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_SETVOLUME;
            s32Mp3Vol = (HI_S32)(*(HI_U16 *)(pstTempMsg->cData));
            if (s32Mp3Vol > APP_MP3PLAY_VOL_MAX)
            {
                s32Mp3Vol = APP_MP3PLAY_VOL_MAX;
            }
			AV_PRINT("HI3531_SetLockStat up");
			HI3531_SetLockStat(pMp3VolLock, s32Mp3Vol);
			AV_PRINT("HI3531_SetLockStat down");
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            piMp3PlayStatus= (HI_S32 *)pMsgBackResult->pData;

            pMsgBackResult->s32Result = HI_SUCCESS;

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));
            break;

        /* ��ȡMP3����״̬ */
        case CTRL_CMD_PLAYMP3FILE_GETSTATUS :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_GETSTATUS;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;

            pMsgBackResult->s32Result = HI_SUCCESS;
            pMsgBackResult->pData = (HI_U8 *)(pstSendBufferMsg->cData + sizeof(pMsgBackResult->s32Result));
            piMp3PlayStatus = (HI_S32 *)pMsgBackResult->pData;
			HI3531_GetLockStat(pMp3PlayStatLock, piMp3PlayStatus);

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) + 4 -  sizeof(pstSendBufferMsg->cData);


            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));
            break;

        /* ��ȡ����ֵ */
        case CTRL_CMD_PLAYMP3FILE_GETVOLUME :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_PLAYMP3FILE_GETVOLUME;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            pMsgBackResult->pData = (HI_CHAR *)(pstSendBufferMsg->cData + sizeof(pMsgBackResult->s32Result));
            piMp3PlayStatus = (HI_S32 *)pMsgBackResult->pData;
			HI3531_GetLockStat(pMp3VolLock, piMp3PlayStatus);

            pMsgBackResult->s32Result = HI_SUCCESS;

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) + 4 -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            break;

        /* ����Ƶ�ط�ͨ�� */
        case CTRL_CMD_AOUT_PLAYBACK_ON :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_AOUT_PLAYBACK_ON;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            s32Ret = MP3_OpenPlayBack();
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "open mp3 play chn failed!");
                pMsgBackResult->s32Result = HI_FAILURE;
            }
            else
            {
                pMsgBackResult->s32Result = HI_SUCCESS;
            }

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            break;

        /* �ر���Ƶ�ط�ͨ�� */
        case CTRL_CMD_AOUT_PLAYBACK_OFF :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_AOUT_PLAYBACK_OFF;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            s32Ret = MP3_ClosePlayBack();
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( "close mp3 play chn failed!");
                pMsgBackResult->s32Result = HI_FAILURE;
            }
            else
            {
                pMsgBackResult->s32Result = HI_SUCCESS;
            }

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            break;
#if 0
        /* ֹͣIP�Խ� */
        case CTRL_CMD_STOPVOIP :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_STOPVOIP;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;

			HI3531_GetLockStat(pIpSpeakLock, &s32IpSpeak);
            if (APP_IPSPEAKRUN == s32IpSpeak)
            {
				HI3531_SetLockStat(pIpSpeakLock, APP_IPSPEAKIDLE);
                usleep(100000);

                s32Ret = APP_StopIPSpeak();
                if (HI_SUCCESS != s32Ret)
                {
                    pMsgBackResult->s32Result = HI_FAILURE;
                }
                else
                {
                    pMsgBackResult->s32Result = HI_SUCCESS;
                }
            }
            else
            {
                pMsgBackResult->s32Result = HI_SUCCESS;
            }

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            AV_PRINT("stop IP speaker!");
            break;

        /* ��ʼIP�Խ� */
        case CTRL_CMD_STARTVOIP :
            pstSendBufferMsg->s32Cmd = CTRL_CMD_STARTVOIP;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
			HI3531_GetLockStat(pIpSpeakLock, &s32IpSpeak);
            if (APP_IPSPEAKIDLE == s32IpSpeak)
            {
				HI3531_SetLockStat(pIpSpeakLock, APP_IPSPEAKRUN);

                s32Ret = APP_StartIPSpeak();
                if (HI_SUCCESS != s32Ret)
                {
                    pMsgBackResult->s32Result = HI_FAILURE;
                }
                else
                {
                    pMsgBackResult->s32Result = HI_SUCCESS;
                }
            }
            else
            {
                pMsgBackResult->s32Result = HI_SUCCESS;
            }

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            AV_PRINT("start IP speaker!");
            break;

        /* ��ȡ��Ƶ���� */
        case CTRL_CMD_GETAUDIOPROPERTY:
            pstSendBufferMsg->s32Cmd = CTRL_CMD_GETAUDIOPROPERTY;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            pstAudioPro = (IFLY_AUDIO_ATTR_T *)pMsgBackResult->pData;
            s32Ret = APP_GetAudioPro(pstAudioPro);
            if (HI_SUCCESS != s32Ret)
            {
                pMsgBackResult->s32Result = HI_FAILURE;
            }
            else
            {
                pMsgBackResult->s32Result = HI_SUCCESS;
            }

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            break;
#endif
        /* ��ȡMP3�ļ�״̬ */
        case CTRL_CMD_SET_MP3FILE_STATUE:
            pstSendBufferMsg->s32Cmd = CTRL_CMD_SET_MP3FILE_STATUE;
            pMsgBackResult = (MSG_RESULT_T *)pstSendBufferMsg->cData;
            piMp3FileStatue = (HI_S32 *)pstTempMsg->cData;
            g_Mp3FileStat = *piMp3FileStatue;
            #if 0
            if (*piMp3FileStatue)
            {
                g_iDevErrStatue = (g_iDevErrStatue |(0x01 << 9));
            }
            else
            {
                g_iDevErrStatue = (g_iDevErrStatue &(~(0x01 << 9)));
            }
            #endif

            //AV_PRINT( "g_iDevErrStatue:%d, *piMp3FileStatue=%d!", g_iDevErrStatue,*piMp3FileStatue);

            pMsgBackResult->s32Result = HI_SUCCESS;

            pstSendBufferMsg->s32Len = sizeof(MSG_QUE_BUF_DATA_T) + sizeof(pMsgBackResult->s32Result) -  sizeof(pstSendBufferMsg->cData);
            usNewCrc = HI3531_CRC16((HI_U8 *)pstSendBufferMsg, pstSendBufferMsg->s32Len - 2);
            memcpy((HI_U8 *)pstSendBufferMsg + pstSendBufferMsg->s32Len - 2, (HI_U8 *)&usNewCrc, sizeof(usNewCrc));

            break;

        default:
    		AV_PRINT( "not support this type:%d!", pstTempMsg->s32Cmd);

            return HI_FAILURE;
    }

    return HI_SUCCESS;
}



