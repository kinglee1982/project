
#include "av_public_head.h"
#include "av_common_31.h"
#include "commctl.h"
#include "data_buf.h"
#include "av_socket.h"
#include "av_hddfsm.h"

#ifdef HDD_FSM

HDD_FSM_ST g_HddFsm;
HI_BOOL bAlarmTrigger = HI_FALSE;
EVENT_INFO stEventInfo;
HI_S32 HDD_ChkDevRemoveable(HI_CHAR *pDevPath)
{
    HI_S32 s32RdLen = 0;
    FILE *pFile = NULL;
    HI_CHAR file[128] = {0};
    HI_CHAR content[32] = {0};

    if (NULL == pDevPath)
    {
        AV_PRINT("input para is invalid.");
        return HI_FAILURE;
    }

    snprintf(file, sizeof(file), "%s/removable", pDevPath);
    pFile = fopen(file, "r");
    if (NULL == pFile)
    {
        AV_PRINT("open(%s) err(%s)", file, ERR_STR);
        return HI_FAILURE;
    }

    s32RdLen = fread(content, sizeof(content), 1, pFile);
    if (0 > s32RdLen)
    {
        fclose(pFile);
        AV_PRINT("fread(%s) err(%s)", file, ERR_STR);
        return HI_FAILURE;
    }

    fclose(pFile);
    //AV_PRINT("dev(%s) removable(%d)", pDevPath, atoi(content));

	return atoi(content);
}

HI_S32 HDD_ChkDevSize(HI_CHAR *pDevPath)
{
	HI_S32 s32RdLen = 0;
	FILE *pFile = NULL;
	HI_CHAR file[64] = {0};
	HI_CHAR content[32] = {0};

	if (NULL == pDevPath)
	{
		AV_PRINT("input para is invalid.");
		return HI_FAILURE;
	}

	snprintf(file, sizeof(file), "%s/size", pDevPath);
	pFile = fopen(file, "r");
	if (NULL == pFile)
	{
		AV_PRINT("open(%s) err(%s)", file, ERR_STR);
		return HI_FAILURE;
	}

    s32RdLen = fread(content, sizeof(content), 1, pFile);
	if (0 > s32RdLen)
	{
		fclose(pFile);
		AV_PRINT("fread(%s) err(%s)", file, ERR_STR);
		return HI_FAILURE;
	}

	fclose(pFile);
	//AV_PRINT("dev(%s) size(%d)", pDevPath, atoi(content));

	return (atoi(content) / 1048576);
}

HI_S32 HDD_CheckSSD()
{
    FILE *fp = NULL;
	HI_CHAR ReadBuf[2048] = {0};
    HI_CHAR dir[32] = {0};
	HI_S32 s32Ret = -1;
	HI_CHAR *p = NULL;

	fp = fopen(HDD_MOUNT_FILE, "r");
	if (fp == NULL)
	{
        AV_PRINT("in detect_hdd_mount, fopen error!");
		return HI_FAILURE;
	}

    s32Ret = fread(ReadBuf, sizeof(ReadBuf), 1, fp);
	if (s32Ret < 0)
	{
		fclose(fp);
		return HI_FAILURE;
	}

	snprintf(dir, sizeof(dir), "/mnt/ssd");
	p = strstr(ReadBuf, dir);
	if (p != NULL)
	{
	    fclose(fp);
	    return HI_SUCCESS;
	}
    else
    {
        fclose(fp);
		AV_PRINT("dir(%s)-not find mounted",dir);
        return HI_FAILURE;
    }

	fclose(fp);
    fp = NULL;

}

HI_S32 HDD_MountSSD(HI_CHAR *psrcdir)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR dir[32] = "/mnt/ssd";

	s32Ret = mount(psrcdir, dir, "vfat", 0, NULL);
	if (HI_SUCCESS != s32Ret)
	{
		AV_PRINT("mount(%s) to(%s) err(%s).", psrcdir, dir, ERR_STR);
		return HI_FAILURE;
	}
	else
	{
		AV_PRINT("mount(%s) to(%s) suc.", psrcdir, dir);
	}

    return s32Ret;
}

HI_S32 HDD_FindDev(HI_VOID)
{
	HI_S32 i = 0;	/*dev partiton var*/
	HI_S32 j = 0;	/*dev range from 'a' to 'p'*/
	HI_S32 s32DevRange = (HI_S32)('p' - 'a');
	HI_S32 s32PrtRange = 9;
	HI_CHAR szFileName[32] = {0};
	HI_S32 s32Ret = 0;

	for (j = 0; j < s32DevRange; j++)
	{
		/* find dev file, note:97='a' */
		HI_CHAR fileDir[32] = {0};
		snprintf(fileDir, sizeof(fileDir), "/sys/block/sd%c", (HI_CHAR)(j + 97));
		if (0 != access(fileDir, F_OK))
		{
			continue;
		}


		s32Ret = HDD_ChkDevRemoveable(fileDir);

//		AV_PRINT("removable(%d)................dir(%s)", s32Ret, fileDir);
		if (HARD_DISK_UNREMOVABLE != s32Ret)
		{
			continue;
		}

		s32Ret = HDD_ChkDevSize(fileDir);
		AV_PRINT("size(%d)..................dir(%s)", s32Ret, fileDir);
		if (s32Ret < HARD_DISK_MIN_SIZE)
		{
            continue;
		}

		/* find dev-partiton file, range:[a1,a2,a5-a8] */
		for (i = 1; i < s32PrtRange; i++)
		{
			if ((i == 3 )|| (i == 4))
			{
				continue;
			}

			snprintf(szFileName, sizeof(szFileName), "%s/sd%c%d", fileDir, (HI_CHAR)(j + 97), i);
			if (0 != access(szFileName, F_OK))
			{
				AV_PRINT("NOT FOUND DEV-PARTION(%s)", szFileName);
				return HI_FAILURE;
			}
			else
			{
				AV_PRINT("FOUND DEV-PARTION(%s)", szFileName);
			}
		}

		return HI_SUCCESS;
	}

	return HI_FAILURE;
}

HI_S32 HDD_DetectHddMount(HI_VOID)
{
    FILE *fp = NULL;
	HI_CHAR ReadBuf[2048] = {0};
	HI_S32 s32Ret = -1;
	HI_S32 i = -1;
	HI_CHAR *p = NULL;
    HI_S32 s32HddMountStat = 0;
	fp = fopen(HDD_MOUNT_FILE, "r");
	if (fp == NULL)
	{
        AV_PRINT("in detect_hdd_mount, fopen error!");
		return -1;
	}
    s32Ret = fread(ReadBuf, sizeof(ReadBuf), 1, fp);
	if (s32Ret < 0)
	{
		fclose(fp);
		return -1;
	}
	for (i = 1; i < 9; i++)
	{
		HI_CHAR dir[32] = {0};
		if ((i == 2) || (i == 3 ) || (i == 4))
		{
			continue;
		}

		snprintf(dir, sizeof(dir), "/mnt/a%d", i);
		p = strstr(ReadBuf, dir);
		if (p != NULL)
		{
			s32HddMountStat = (s32HddMountStat | (0x01 << (i - 1)));
			//AV_PRINT("dir(%s)-find mounted stat(%x) ", dir, s32HddMountStat);
		}
        else
        {
			//AV_PRINT("dir(%s)-not find mounted stat(%x) ", dir, s32HddMountStat);
        }
	}

	fclose(fp);
	return s32HddMountStat;
}

HI_S32 HDD_MountDisk(HI_VOID)
{
	HI_S32 i = 0;
	HI_S32 j = 0;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_CHAR dev[64] = {0};
	HI_CHAR dir[64] = {0};
	HI_S32 s32HDDMountStat = 0;
	#define FS_TYPE			"vfat"
	HI_S32 s32DevRange = (HI_S32)('p' - 'a');

	for (j = 0; j < s32DevRange; j++)
	{
		HI_CHAR fileDir[32] = {0};
		snprintf(fileDir, sizeof(fileDir), "/dev/sd%c", (HI_CHAR)(j + 97));
		if (0 != access(fileDir, F_OK))
		{
			continue;
		}

		HI_CHAR blockDir[32] = {0};
		snprintf(blockDir, sizeof(blockDir), "/sys/block/sd%c", (HI_CHAR)(j + 97));
		if (0 != access(blockDir, F_OK))
		{
			continue;
		}

		s32Ret = HDD_ChkDevRemoveable(blockDir);
		if (HARD_DISK_UNREMOVABLE != s32Ret)
		{
			continue;
		}

		s32Ret = HDD_ChkDevSize(blockDir);
		if (s32Ret < HARD_DISK_MIN_SIZE)
		{
		    /*挂SSD */
            if(HDD_CheckSSD()!= HI_SUCCESS)
            {
 			    snprintf(dev, sizeof(dev), "%s%d", fileDir, 1);
                HDD_MountSSD(dev);
            }
            sync();

            continue;
		}

		for (i = 1; i < 9; i++)
		{
			if ((i == 2) || (i == 3) || (i == 4))
			{
				continue;
			}
			AV_PRINT("mount dev-prt(%d)", i);
			s32HDDMountStat = HDD_DetectHddMount();
			if (s32HDDMountStat & (1 << (i - 1)))
			{
				AV_PRINT("not exit(%s) mount stat(%x)...............", fileDir, s32HDDMountStat);
				continue;
			}

			snprintf(dev, sizeof(dev), "%s%d", fileDir, i);
			snprintf(dir, sizeof(dir), "/mnt/a%d", i);
			s32Ret = mount(dev, dir, FS_TYPE, 0, NULL);
			if (HI_SUCCESS != s32Ret)
			{
				AV_PRINT("mount(%s) to(%s) err(%s).", dev, dir, ERR_STR);
				return HI_FAILURE;
			}
			else
			{
				AV_PRINT("mount(%s) to(%s) suc.", dev, dir);
			}
		}

		if (i == 9)
		{
			break;
		}
	}

	return HI_SUCCESS;
}

HI_S32 HDD_UmountDisk(HI_S32 s32Times)
{
	HI_S32 i = 0;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_CHAR dir[64] = {0};
	HI_S32 s32UmountRet = HI_SUCCESS;
	HI_S32 s32UmountFlags = MNT_DETACH;

	if (s32Times == 3)
	{
		s32UmountFlags |= MNT_FORCE;
	}

    /*ssd卸载 */
	umount2("/mnt/ssd", s32UmountFlags);

	for (i = 1; i < 9; i++)
	{
		if ((i >= 2) && (i <= 4))
		{
			continue;
		}

		snprintf(dir, sizeof(dir), "/mnt/a%d", i);
		s32Ret = umount2(dir, s32UmountFlags);
		if (HI_SUCCESS != s32Ret)
		{
			AV_PRINT("umount (%s) err(%s).", dir, ERR_STR);
			s32UmountRet = HI_FAILURE;
		}
		else
		{
			AV_PRINT("umount (%s) suc.", dir);
		}
	}

	return s32UmountRet;
}

HDD_FSM_ST *HDD_GetHddFsm()
{
	return &g_HddFsm;
}

HI_S32 HDD_GetDiskFSM(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->s32HddStat;
}

HI_S32 HDD_GetHddUnLockEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->s32HddUnLockEvent;
}

HI_VOID HDD_SetHddUnLockEvent(HI_BOOL bUnLock)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	AV_PRINT("HDD_SetHddUnLockEvent : %d",bUnLock);
	pHddFsm->s32HddUnLockEvent = bUnLock;
}


HI_VOID HDD_SetHddRecEvent(HI_BOOL bRecStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();

	pthread_mutex_lock(&pHddFsm->stEventThd[DISK_EVENT_REC].mutex);
	if (bRecStart)
	{
		pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt++;
		if (pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt > 0)
		{
			pHddFsm->stEventThd[DISK_EVENT_REC].bStart = HI_TRUE;
		}
	}
	else
	{
		pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt--;
		if (pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt <= 0)
		{
			pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt = 0;
			pHddFsm->stEventThd[DISK_EVENT_REC].bStart = HI_FALSE;
		}
        AV_PRINT("DISK_EVENT_REC stop rec (%d)",pHddFsm->stEventThd[DISK_EVENT_REC].s32EventCnt)
	}
	pthread_mutex_unlock(&pHddFsm->stEventThd[DISK_EVENT_REC].mutex);

	return;
}

HI_BOOL HDD_GetHddRecEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return	pHddFsm->stEventThd[DISK_EVENT_REC].bStart;
}

HI_VOID HDD_SetHddLPBEvent(HI_BOOL bRecStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();

	pthread_mutex_lock(&pHddFsm->stEventThd[DISK_EVENT_REC_LPB].mutex);
	if (bRecStart)
	{
		pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt++;
		//AV_PRINT("event->PlusPlus%d",pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt)
		if (pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt > 0)
		{
			pHddFsm->stEventThd[DISK_EVENT_REC_LPB].bStart = HI_TRUE;
		}
	}
	else
	{
		pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt--;
		//AV_PRINT("event->ReduceReduce%d",pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt)
		if (pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt <= 0)
		{
			pHddFsm->stEventThd[DISK_EVENT_REC_LPB].s32EventCnt = 0;
			pHddFsm->stEventThd[DISK_EVENT_REC_LPB].bStart = HI_FALSE;
		}
	}
	pthread_mutex_unlock(&pHddFsm->stEventThd[DISK_EVENT_REC_LPB].mutex);

	return;
}

HI_BOOL HDD_GetHddLPBEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return	pHddFsm->stEventThd[DISK_EVENT_REC_LPB].bStart;
}

HI_VOID HDD_SetHddDownLoadEvent(HI_BOOL bStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->stEventThd[DISK_EVENT_REC_DOWNLOAD].bStart = bStart;
	return;
}

HI_BOOL HDD_GetHddDownLoadEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->stEventThd[DISK_EVENT_REC_DOWNLOAD].bStart;
}

HI_VOID HDD_SetHddRPBEvent(HI_BOOL bStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->stEventThd[DISK_EVENT_REC_RPB].bStart = bStart;
	return;
}

HI_BOOL HDD_GetHddRPBEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->stEventThd[DISK_EVENT_REC_RPB].bStart;
}

HI_VOID HDD_SetHddBKEvent(HI_BOOL bStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->stEventThd[DISK_EVENT_USB_BK].bStart = bStart;
	return;
}

HI_BOOL HDD_GetHddBKEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->stEventThd[DISK_EVENT_USB_BK].bStart;
}

HI_VOID HDD_SetAlarmRecbkEvent(HI_BOOL bStart)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->stEventThd[DISK_EVENT_ALARMREC_BK].bStart = bStart;
	return;
}

HI_BOOL HDD_GetAlarmRecbkEvent(HI_VOID)
{
	HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	return pHddFsm->stEventThd[DISK_EVENT_ALARMREC_BK].bStart;
}


HI_VOID HDD_SetHddEvent(DISK_EVENT_E eDiskEvent, HI_BOOL bStart)
{
	switch(eDiskEvent)
	{
		case DISK_EVENT_REC:
			HDD_SetHddRecEvent(bStart);
			break;
		case DISK_EVENT_REC_LPB:
			HDD_SetHddLPBEvent(bStart);
			break;
		case DISK_EVENT_REC_DOWNLOAD:
			HDD_SetHddDownLoadEvent(bStart);
			break;
		case DISK_EVENT_REC_RPB:
			HDD_SetHddRPBEvent(bStart);
			break;
		case DISK_EVENT_USB_BK:
			HDD_SetHddBKEvent(bStart);
			break;
		case DISK_EVENT_ALARMREC_BK:
			HDD_SetAlarmRecbkEvent(bStart);
			break;
		default:
			break;
	}

	return;
}

HI_BOOL HDD_GetHddEvent(DISK_EVENT_E eDiskEvent)
{
	HI_BOOL bStart = HI_FALSE;

	switch(eDiskEvent)
	{
		case DISK_EVENT_REC:
			bStart = HDD_GetHddRecEvent();
			break;
		case DISK_EVENT_REC_LPB:
			bStart = HDD_GetHddLPBEvent();
			break;
		case DISK_EVENT_REC_DOWNLOAD:
			bStart = HDD_GetHddDownLoadEvent();
			break;
		case DISK_EVENT_REC_RPB:
			bStart = HDD_GetHddRPBEvent();
			break;
		case DISK_EVENT_USB_BK:
			bStart = HDD_GetHddBKEvent();
			break;
		case DISK_EVENT_ALARMREC_BK:
			bStart = HDD_GetAlarmRecbkEvent();
			break;
			default:
				break;
	}

	return bStart;
}



/*ui */
HI_S32 HDD_GetDiskAbnormal()
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
    return pHddFsm->s32HddAbnormal;
}

HI_VOID HDD_SetDiskAbnormal(HI_S32 s32Abnormal)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
//	AV_PRINT("HDD_SetDiskAbnormal : %d",s32Abnormal);
	pHddFsm->s32HddAbnormal = s32Abnormal;
}
HI_VOID HDD_SetRebootFlag(HI_S32 s32RebootFlag)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	AV_PRINT("HDD_SetRebootFlag : %d",s32RebootFlag);
	pHddFsm->s32RebootFlag = s32RebootFlag;
}

HI_VOID HDD_SetAlarmTrigger(HI_BOOL bTrigger)
{
    if (bTrigger)
    {
        stEventInfo.s32StartTime = time(NULL);
        stEventInfo.s32EndTime = stEventInfo.s32StartTime; /*结束时间和开始时间一致*/
        bAlarmTrigger = HI_TRUE;
    }
    /*点触发，只判断触发OK */
    #if 0
    else
    {
        stEventInfo.s32EndTime = time(NULL);
        bAlarm = HI_FALSE;
    }
    #endif
}

HI_S32 HDD_GetDiskStat(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	if (DISK_STAT_WORKING == pHddFsm->s32HddStat)
    {
        return HARD_DISK_OK;
    }
    else
    {
        return HARD_DISK_ERROR;
    }
}
HI_S32 HDD_GetDevSize(HI_CHAR *pDevPath)
{

    HI_S32 s32RdLen = 0;
    FILE *pFile = NULL;
    HI_CHAR file[64] = {0};
    HI_CHAR content[32] = {0};
    if (NULL == pDevPath)
    {
        AV_PRINT("input para is invalid.");
        return HI_FAILURE;
    }
    snprintf(file, sizeof(file), "%s/size", pDevPath);
    pFile = fopen(file, "r");
    if (NULL == pFile)
    {
        AV_PRINT("open(%s) err(%s)", file, ERR_STR);
        return HI_FAILURE;
    }
    s32RdLen = fread(content, sizeof(content), 1, pFile);
    if (0 > s32RdLen)
    {
        fclose(pFile);
        AV_PRINT("fread(%s) err(%s)", file, ERR_STR);
        return HI_FAILURE;
    }
    fclose(pFile);
    return atoi(content);
}
HI_S32 HDD_GetSdStat(HI_VOID)
{
    int j = 0;	/*dev range from 'a' to 'p'*/
    int s32DevRange = (int)('p' - 'a');
    int s32Ret = 0;
    char cDevicePath[64] = {0};
    char linkpath[256] = {0};
    for (j = 0; j < s32DevRange; j++)
    {
        char fileDir[32] = {0};
        snprintf(fileDir, sizeof(fileDir), "/sys/block/sd%c", (char)(j + 97));
        if (0 != access(fileDir, F_OK))
        {
            continue;
        }
        s32Ret = HDD_ChkDevRemoveable(fileDir);
        if(s32Ret == 0)
        {
            continue;
        }
        snprintf(cDevicePath, sizeof(cDevicePath), "%s/device", fileDir);
        if(realpath(cDevicePath,linkpath) == NULL)
        {
            continue;
        }
        if(strstr(linkpath,"1-1.3") != NULL)
        {
            s32Ret = HDD_GetDevSize(fileDir);
            if(s32Ret > 0)
            return 1;
        }
    }
    return 0;
}


HI_S32 HDD_StartLPB(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->s32LPBFLag = LPB_START;
    return HI_SUCCESS;
}

HI_S32 HDD_StopLPB(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->s32LPBFLag = LPB_STOP;
    return HI_SUCCESS;
}

HI_S32 HDD_ExitLPB(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	pHddFsm->s32LPBFLag = LPB_EXIT;
    return HI_SUCCESS;
}
HI_BOOL HDD_IsLPBRunning(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	if (LPB_START == pHddFsm->s32LPBFLag)
	{
		return HI_TRUE;
	}
	return HI_FALSE;
}

HI_VOID HDD_WaitExitLPB(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
	if (LPB_STOP == pHddFsm->s32LPBFLag)
	{
		while (LPB_EXIT != pHddFsm->s32LPBFLag)
		{
			//usleep (100*1000);
			TM_USleep(0,100*1000);
		}
	}
	return;
}


HI_S32 HDD_StopRec(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
    pHddFsm->s32FormatFlag = 1;

    return HI_SUCCESS;
}

HI_S32 HDD_StartRec(HI_VOID)
{
    HDD_FSM_ST *pHddFsm = HDD_GetHddFsm();
    pHddFsm->s32FormatFlag = 0;

    return HI_SUCCESS;
}


HI_VOID HDD_HddNotReadyHandle(HI_VOID)
{
	HI_S32 s32FindDev = -1;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();
    if ((0 == pHddFsm->s32HddUnLockEvent)
        && (0 == pHddFsm->s32HddAbnormal)
        &&(0==pHddFsm->s32RebootFlag))
    {
		sync();
		sync();
		HDD_UmountDisk(3);
		sync();
		sync();

		COMMCTL_Power(POWER_HDD,POWER_OFF);
        //usleep(500000);
        TM_USleep(0,500000);
		COMMCTL_Power(POWER_HDD,POWER_OFF);
        AV_PRINT("hdd power off");
		while (1)
		{
			s32FindDev = HDD_FindDev();
			if (HI_FAILURE == s32FindDev)
			{
				AV_PRINT("hdd no found");
				break;
			}

			//sleep(1);
			TM_USleep(1,0);
		}
        AV_PRINT("FIND DEV DISK READY");
		pHddFsm->s32HddStat = DISK_STAT_READY;

    }

	return;
}

HI_VOID HDD_HddReadyHandle(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

    if (0 == pHddFsm->s32HddUnLockEvent)
    {
        COMMCTL_Power(POWER_HDD, POWER_ON);
        //usleep(500000);
        TM_USleep(0,500000);
        COMMCTL_Power(POWER_HDD, POWER_ON);
        AV_PRINT("hdd power On");
        pHddFsm->s32HddStat = DISK_STAT_TOMOUNT;
    }

	return;
}

HI_VOID HDD_HddToMountHandle(HI_VOID)
{
	HI_S32 s32FindDev = 0;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	while (1)
	{
		s32FindDev = HDD_FindDev();
		if ((1 == pHddFsm->s32HddUnLockEvent)
            || (1 == pHddFsm->s32HddAbnormal)
            || (1==pHddFsm->s32RebootFlag))
		{
			pHddFsm->s32HddStat = DISK_STAT_NOT_READY;
			break;
		}

        if(1 == pHddFsm->s32FormatFlag)
        {
            pHddFsm->s32HddStat = DISK_STAT_FORMATING;
			break;
        }

		if (HI_SUCCESS == s32FindDev)
		{
			pHddFsm->s32HddStat = DISK_STAT_MOUNTING;
			break;
		}
		//usleep(200000);
		TM_USleep(0,200000);
	}

	return;
}

HI_VOID HDD_HddMountingHandle(HI_VOID)
{
	HI_S32 s32Ret = HI_FAILURE;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	s32Ret = HDD_MountDisk();
	if (HI_SUCCESS == s32Ret)
	{
		pHddFsm->s32HddStat = DISK_STAT_MOUNT_SUCC;
		pHddFsm->s32MountFailTimes = 0;
	}
	else
	{
		pHddFsm->s32MountFailTimes++;
		pHddFsm->s32HddStat = DISK_STAT_MOUNT_FAILED;
	}

	sync();
	sync();

}

HI_VOID HDD_HddMountFailedHandle(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	/*unlock, disk abnormal, mount 3 times*/
	if ((3 == pHddFsm->s32MountFailTimes)
			||(1 == pHddFsm->s32HddUnLockEvent)
			||(1 == pHddFsm->s32HddAbnormal)
			||(1 == pHddFsm->s32RebootFlag))
	{
		pHddFsm->s32HddStat = DISK_STAT_UMOUNTING;
		pHddFsm->s32MountFailTimes = 0;
	}
	else
	{
		pHddFsm->s32HddStat = DISK_STAT_TOMOUNT;
	}

	return;
}

HI_VOID HDD_HddMountSuccHandle(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();
	AV_PRINT("#####%d  %d %d#######",pHddFsm->s32HddUnLockEvent, pHddFsm->s32HddAbnormal,pHddFsm->s32RebootFlag);
	/*unlock, disk abnormal, */
	if ((1 == pHddFsm->s32HddUnLockEvent)
        ||(1 == pHddFsm->s32HddAbnormal)
        ||(1==pHddFsm->s32RebootFlag))
	{
		pHddFsm->s32HddStat = DISK_STAT_UMOUNTING;
	}
	else
	{
		pHddFsm->s32HddStat = DISK_STAT_WORKING;
	}

	return;
}

HI_VOID HDD_HddWorkingHandle(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	/*unlock, disk abnormal, format*/
	if ((1 == pHddFsm->s32HddUnLockEvent)
		|| (1 == pHddFsm->s32HddAbnormal)
		|| (1 == pHddFsm->s32FormatFlag)
		|| (1 == pHddFsm->s32RebootFlag))
	{
		pHddFsm->s32HddStat = DISK_STAT_TOUMOUNT;
	}

	return;
}

HI_VOID HDD_HddToUmountHandle(HI_VOID)
{
	HI_S32 i = 0;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	for(i = 0; i < MAX_EVENT_NUMS; i++)
	{
        AV_PRINT("event(%d) thread bstart(%d)", i, pHddFsm->stEventThd[i].bStart);
		if (pHddFsm->stEventThd[i].bStart == HI_TRUE)
		{
			AV_PRINT("event(%d) thread running.", i);
			return;
		}
	}

	pHddFsm->s32HddStat = DISK_STAT_UMOUNTING;
	return;
}

HI_VOID HDD_HddUmountingHandle(HI_VOID)
{
	HI_S32 s32Ret = -1;
	HI_S32 s32UmountTimes = 0;
	HI_S32 s32FindDev = 0;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	/*umounting hard disk only*/
	sync();
	sync();
	for (s32UmountTimes = 0; s32UmountTimes < 3; s32UmountTimes++)
	{
		s32Ret = HDD_UmountDisk(s32UmountTimes);
		if (HI_SUCCESS == s32Ret)
		{
			break;
		}
	}
	sync();
	sync();

	/*triggered by "format" event*/
    if (1 == pHddFsm->s32FormatFlag)
    {
		pHddFsm->s32HddStat = DISK_STAT_FORMATING;
		return;
    }

	/*triggered by "unlock, disk abnormal"  event*/
	COMMCTL_Power(POWER_HDD, POWER_OFF);
    //usleep(500000);
    TM_USleep(0,500000);
	COMMCTL_Power(POWER_HDD, POWER_OFF);
    AV_PRINT("hdd power off");
    DOMAIN_PackageData(ID_UI, AV_GET_DISK_STAT, NULL, 0);

	while (1)
	{
		s32FindDev = HDD_FindDev();
		if (HI_FAILURE == s32FindDev)
		{
			AV_PRINT("hdd no found");
			break;
		}

		//sleep(1);
		TM_USleep(1,0);
	}

	pHddFsm->s32HddStat = DISK_STAT_NOT_READY;

	return;
}


HI_VOID HDD_HddFormatingHandle(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

    //DOMAIN_PackageData(ID_UI, AV_GET_DISK_STAT, NULL, 0);
    while(1)
    {
        if(pHddFsm->s32FormatFlag == 0)
        {
            break;
        }
        //sleep(2);
        TM_USleep(2,0);
    }
    pHddFsm->s32HddStat = DISK_STAT_TOMOUNT;

	return;
}

/*UI*/
HI_VOID HDD_SetDiskUnWork(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;

	pHddFsm =  HDD_GetHddFsm();
	pHddFsm->s32HddStat = DISK_STAT_NOT_READY;

	return;
}

HI_VOID HDD_UnlockDiskKey(HI_VOID)
{
	HDD_FSM_ST *pHddFsm;

	pHddFsm =  HDD_GetHddFsm();
	pHddFsm->s32HddStat = DISK_STAT_NOT_READY;

	return;
}


HI_VOID HDD_Init(HI_VOID)
{
	HI_S32 i = 0;
	HI_S32 s32Ret = HI_FAILURE;
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();

	pHddFsm->s32HddStat = DISK_STAT_NOT_READY;
	pHddFsm->s32MountFailTimes = 0;
	pHddFsm->s32HddUnLockEvent = 0;
	pHddFsm->s32HddAbnormal = 1;
    pHddFsm->s32RebootFlag = 0;
	pHddFsm->s32FormatFlag = 0;
	pHddFsm->s32LPBFLag = LPB_UNSTART;

	for (i = 0; i < MAX_EVENT_NUMS; i++)
	{
		pHddFsm->stEventThd[i].tid = -1;
		pHddFsm->stEventThd[i].bStart = HI_FALSE;
		pHddFsm->stEventThd[i].s32EventCnt = 0;

		s32Ret = pthread_mutex_init(&pHddFsm->stEventThd[i].mutex, NULL);
		if(s32Ret != HI_SUCCESS)
		{
			AV_PRINT("mutex init error");
			return;
		}
	}

	return;
}

HI_VOID AV_PRINT_ONE(HI_CHAR *pMsg, HI_S32 s32MsgId)
{
	static HI_S32 s32Id = -2;


	if (s32MsgId != s32Id)
	{
		AV_PRINT(pMsg);
		s32Id = s32MsgId;
	}
	return;
}

HI_VOID* HDD_Check(HI_VOID *)
{
	HDD_FSM_ST *pHddFsm;
	pHddFsm =  HDD_GetHddFsm();
	COMMCTL_init();
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    while(1)
    {
	    switch(pHddFsm->s32HddStat)
        {
            case DISK_STAT_NOT_READY:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_NOT_READY", 1);
				HDD_HddNotReadyHandle();
                break;
            case DISK_STAT_READY:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_READY", 2);
				HDD_HddReadyHandle();
                break;
            case DISK_STAT_TOMOUNT:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_TOMOUNT", 3);
				HDD_HddToMountHandle();
                break;
            case DISK_STAT_TOUMOUNT:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_TOUMOUNT", 4);
				HDD_HddToUmountHandle();
                break;
            case DISK_STAT_MOUNTING:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_MOUNTING", 5);
				HDD_HddMountingHandle();
                break;
            case DISK_STAT_UMOUNTING:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_UMOUNTING", 6);
				HDD_HddUmountingHandle();
                break;
            case DISK_STAT_MOUNT_SUCC:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_MOUNT_SUCC", 7);
				HDD_HddMountSuccHandle();
                break;
			case DISK_STAT_MOUNT_FAILED:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_MOUNT_FAILED", 8);
				HDD_HddMountFailedHandle();
				break;
			case DISK_STAT_WORKING:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_WORKING", 9);
				HDD_HddWorkingHandle();
				break;
            case DISK_STAT_FORMATING:
				AV_PRINT_ONE((HI_CHAR *)"DISK_STAT_FORMATING", 10);
                HDD_HddFormatingHandle();
                break;
			case DISK_STAT_PART_ERROR:
				break;
        }
	  	//usleep(500000);
	  	TM_USleep(0,500000);
    }

	COMMCTL_exit();
	return NULL;
}
#endif

