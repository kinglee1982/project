#include<stdio.h>
#include<stdlib.h>
#include <linux/fcntl.h>
#include"mcp4531.h"
#define MCP4531DEV "/dev/mcp4531"
int main(void)
{
	int fd = -1;
	unsigned int volume = 0;
	fd = open(MCP4531DEV,O_RDWR);
	if(fd < 0)
	{
		printf("open %s error\n",MCP4531DEV);
		return -1;
	}
    #if 1
	volume = 126;
	printf("set audio volume=%d\n",volume);
	ioctl(fd,IOC_MCP4531_SET_AUDIO_VOLUME,(void *)&volume);

	ioctl(fd,IOC_MCP4531_GET_AUDIO_VOLUME,(void *)&volume);
	printf("get audio volume=%d\n",volume);
    #endif
	volume = 126;
	ioctl(fd,IOC_MCP4531_SET_MIC_VOLUME,(void *)&volume);
	printf("set mic volume=%d\n",volume);
	ioctl(fd,IOC_MCP4531_GET_MIC_VOLUME,(void *)&volume);
	printf("get mic volume=%d\n",volume);

	close(fd);
	return 0;
}
