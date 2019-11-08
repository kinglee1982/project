#include "av_public_head.h"

HI_S32 gettid(HI_VOID)
{
	return syscall(SYS_gettid);
}
HI_CHAR *AV_GetTime(HI_CHAR *pTimeBuf, HI_S32 s32Len)
{
	HI_CHAR timeBuf[32] = {0};
	time_t nowTime = 0;
	struct tm *pTm = NULL;

	if (s32Len < (HI_S32)sizeof(timeBuf))
	{
		return NULL;
	}

	nowTime = time((time_t *)NULL);
	pTm = localtime(&nowTime);
	//if (strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", pTm) == 0)
	if (strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", pTm) == 0)
    {
		perror("strftime returned 0");
		return NULL;
	}

	if(pTimeBuf != NULL)
	{
		memcpy(pTimeBuf, timeBuf, sizeof(timeBuf));
	}
	else
	{
		return NULL;
	}

    return pTimeBuf;
}

int debug_print(const char *cTitle,char *buf, int len)
{
	int i = 0;
    printf("#[av]:%s debug_print:\n*****************************\n",cTitle);
	for(i = 0; i < len; i++)
	{
		printf("%02x ", buf[i] & 0xff);
		if(!((i+1) % 10)) printf("\n");
	}
	printf("#[av]: size = %d\n*****************************\n", len);
    return 0;
}


void TM_USleep(int iSec,int iUsec)
{
    struct timeval delay;
    delay.tv_sec = iSec;
    delay.tv_usec = iUsec;
    select(0,NULL,NULL,NULL,&delay);
}