#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include"tw2968.h"

int main(int argc,char *argv[])
{
	int chn=0;
	int val=0;
	//chn = atoi(argv[1]);
	//val = atoi(argv[2]);
	//printf("set_brightness chn %d val %d\n",chn,val);
	//set_brightness(chn,val);
	//set_video_mode(0,PAL);

	int fd = open("/dev/tw2968dev", O_RDWR);
	if(fd < 0)
	{
		perror("open /dec/tw2968dev error!");
		return -1;
	}

	tw2968_Chip_Info chipInfo;
    ioctl(fd, TW2968_CHIP_INFO, &chipInfo);

    printf("get chip_num: %d\n", chipInfo.chipNum);

    close(fd);

	return 0;
}
