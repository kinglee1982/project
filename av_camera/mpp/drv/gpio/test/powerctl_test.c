#include<stdio.h>
#include<stdlib.h>
#include"commctl.h"

int main(int argc,char *argv[])
{
	int power= 1;
	if(argc != 2)
	{
		printf("Usage:%s 1/0",argv[0]);
		return 0;
	}

	power = atoi(argv[1]);
	COMMCTL_init();
	COMMCTL_Power(POWER_InAudio,POWER_ON);
	
	if(power == 1)
	{
		COMMCTL_Power(POWER_LED1,POWER_ON);
		COMMCTL_Power(POWER_LED2,POWER_ON);
		COMMCTL_Power(POWER_LED3,POWER_ON);
		COMMCTL_Power_WIFI(POWER_ON);
		COMMCTL_Power_3G(POWER_ON);
		//COMMCTL_Reset_3G();
	}
	else if(power == 0)
	{
		COMMCTL_Power(POWER_LED1,POWER_OFF);
		COMMCTL_Power(POWER_LED2,POWER_OFF);
		COMMCTL_Power(POWER_LED3,POWER_OFF);
		COMMCTL_Power_WIFI(POWER_OFF);
		COMMCTL_Power_3G(POWER_OFF);
	}
	COMMCTL_exit();
	return 0;
}
