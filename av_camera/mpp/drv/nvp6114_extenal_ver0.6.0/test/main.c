#include<stdio.h>
#include"nvp6114.h"

int main(int argc,char *argv[])
{
	int chn=0;
	int val=0;
	chn = atoi(argv[1]);
	val = atoi(argv[2]);
	printf("set_brightness chn %d val %d\n",chn,val);
	set_brightness(chn,val);
	set_video_mode(0,PAL);

	return 0;
}
