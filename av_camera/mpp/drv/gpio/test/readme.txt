����˵����
2015-03-04
����������ƹ���



���ܣ������ⲿģ�����µ����Ƶ�л�
�ļ�˵����
commctl.c   ��Դ�ļ�
commctl.h   ͷ�ļ�
libcommctl.so ��̬���ļ�
libcommctl.a��̬���ļ�
audioctl_test.c������Ƶ�л����Գ���


ʹ��˵����
1.����COMMCTL_init()��ȡgpio�ļ����
2.����COMMCTL_Power��COMMCTL_Reset���µ��λ�ⲿģ��
  COMMCTL_Switch__IPSpkAndAudio��COMMCTL_Switch_MicAndAudioout�л���Ƶ
3.COMMCTL_exit()�ͷ�gpio�ļ����
����ʹ���뿴audioctl_test.c
4.������������Ҫ��ȡgpio�����ֱ�ӵ������ú�������

����˵����
������Щ����Կ������µ�
enum{
	POWER_STM32 = 0,
	POWER_PCM2407,
	POWER_LED1,
	POWER_LED2,
	POWER_LED3,
	POWER_LED4,
	POWER_InAudio,
	POWER_OutAudio,
	POWER_GSMAudio,
	POWER_HDD,
	POWER_SSD,
	POWER_3G,
	POWER_WIFI,
	POWER_SD,
	
}POWER_ITEM;

���������Կ��Ƹ�λ
enum{
	RESET_NVP6114_1 = 0,
	RESET_NVP6114_2,
	RESET_ETH0,
	RESET_ETH1,
	RESET_USBHUB,
	RESET_3G,
}RESET_ITEM;
