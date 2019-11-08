#include<stdio.h>
#include<stdlib.h>
#include"commctl.h"

int main(int argc,char *argv[])
{
	int power= 1;
	if(argc != 2)
	{
		printf("Usage:%s 1/2/3\n 1:InAudio  2:OutAudio   3:GSMAudio\n",argv[0]);
		return 0;
	}
	COMMCTL_init();
	power = atoi(argv[1]);
	if(power == 1)
	{
		COMMCTL_Power(POWER_InAudio,POWER_ON);
		COMMCTL_Power(POWER_OutAudio,POWER_OFF);
		COMMCTL_Power(POWER_TSAudio,POWER_OFF);
	}
	else if(power == 2)
	{
		COMMCTL_Power(POWER_InAudio,POWER_OFF);
		COMMCTL_Power(POWER_OutAudio,POWER_ON);
		COMMCTL_Power(POWER_TSAudio,POWER_OFF);
	}
	else if(power == 3)
	{
		COMMCTL_Power(POWER_InAudio,POWER_OFF);
		COMMCTL_Power(POWER_OutAudio,POWER_OFF);
		COMMCTL_Power(POWER_TSAudio,POWER_ON);
	}
	COMMCTL_exit();
	return 0;
}
