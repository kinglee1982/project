#include<stdio.h>
#include"commctl.h"


int main(void)
{
	COMMCTL_init();
	while(1)
	{
		COMMCTL_Power(POWER_LED1,POWER_ON);
		usleep(100000);
		COMMCTL_Power(POWER_LED1,POWER_OFF);
		usleep(100000);
		COMMCTL_Power(POWER_LED2,POWER_ON);
		usleep(100000);
		COMMCTL_Power(POWER_LED2,POWER_OFF);
		usleep(100000);
		COMMCTL_Power(POWER_LED3,POWER_ON);
		usleep(100000);
		COMMCTL_Power(POWER_LED3,POWER_OFF);
		usleep(100000);
	}
	COMMCTL_exit();
}
