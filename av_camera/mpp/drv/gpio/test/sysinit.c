#include<stdio.h>
#include<stdlib.h>
#include"commctl.h"


int main(int argc,char *argv[])
{
	COMMCTL_init();
	//COMMCTL_Reset(RESET_USBHUB);
	//COMMCTL_Power(POWER_3G,POWER_ON);
	COMMCTL_Reset(RESET_3G);
	//COMMCTL_Reset(RESET_USBHUB);
	//printf("power on\n\n");
	COMMCTL_exit();
	return 0;
}
