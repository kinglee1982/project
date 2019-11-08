#include<stdio.h>
#include<stdlib.h>

#include"commctl.h"


/*
*测试内音输出及各音源切换音量调节
*/
void Test1_inaudio(void)
{
	unsigned char mic_vol = 50,audio_vol = 50,volume= 50;
	unsigned char c;
	unsigned char cmd =0;
	char mode =ENMIC;
	COMMCTL_init();

	COMMCTL_Switch_Aout(mode);
	COMMCTL_Power(POWER_OutAudio,POWER_ON);
	COMMCTL_SetMicVolume(100);
	COMMCTL_GetMicVolume(&mic_vol);
	printf("Mic volume =%d\n",mic_vol);
	printf("enter:'u'-up or 'd'-down or 's'-switch 'q'-quit\n");
	while(1)
	{
		c = getchar();
		if(c == 'd'||c=='u'||c=='s'||c=='q')
			cmd = c;
		if(cmd == 'd')
		{
			if(ENAOUT == mode )
			{
				if(audio_vol == 0)
				audio_vol = 127;
				else
				audio_vol--;
			}
			else
			{
				if(mic_vol == 0)
				mic_vol = 127;
				else
				mic_vol--;
			}
			if(mode == ENAOUT1)
			{
				COMMCTL_SetMicVolume(mic_vol);
				COMMCTL_GetMicVolume(&volume);
				printf("Mic volume =%d\n",volume);

			}
			else if(mode == ENAOUT0)
			{
				COMMCTL_SetAudioVolume(audio_vol);
				COMMCTL_GetAudioVolume(&volume);
				printf("Audio volume =%d\n",volume);
			}

		}
		else if(cmd == 'u')
		{
			if(ENAOUT == mode )
			{
				if(audio_vol == 127)
				audio_vol = 0;
				else
				audio_vol++;
			}
			else
			{
				if(mic_vol == 127)
				mic_vol = 0;
				else
				mic_vol++;
			}
			if(mode == ENAOUT1)
			{
				COMMCTL_SetMicVolume(mic_vol);
				COMMCTL_GetMicVolume(&volume);
				printf("Mic volume =%d\n",volume);

			}
			else if(mode == ENAOUT0)
			{
				COMMCTL_SetAudioVolume(audio_vol);
				COMMCTL_GetAudioVolume(&volume);
				printf("Audio volume =%d\n",volume);
			}
		}
		else if(cmd == 's')
		{
			if(ENAOUT0 == mode )
			{
				mode = ENAOUT1;
				COMMCTL_GetMicVolume(&volume);
				printf("Mic volume =%d\n",volume);
			}
			else
			{
				mode = ENAOUT0;
				COMMCTL_GetAudioVolume(&volume);
				printf("Audio volume =%d\n",volume);
			}
			COMMCTL_Switch_Aout(mode);


		}
		else if(cmd == 'q')
		{
			break;
		}

	}
    	COMMCTL_Power(POWER_OutAudio,POWER_OFF);
	COMMCTL_exit();
	return ;
}

int main(int argc,char ** argv)
{
	Test1_inaudio();
	return 0;
}
