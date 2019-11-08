更新说明：
2015-03-04
添加音量控制功能



功能：控制外部模块上下电和音频切换
文件说明：
commctl.c   库源文件
commctl.h   头文件
libcommctl.so 动态库文件
libcommctl.a静态库文件
audioctl_test.c控制音频切换测试程序


使用说明：
1.调用COMMCTL_init()获取gpio文件句柄
2.调用COMMCTL_Power或COMMCTL_Reset上下电或复位外部模块
  COMMCTL_Switch__IPSpkAndAudio和COMMCTL_Switch_MicAndAudioout切换音频
3.COMMCTL_exit()释放gpio文件句柄
具体使用请看audioctl_test.c
4.设置音量不需要获取gpio句柄，直接调用设置函数即可

参数说明：
下面这些项可以控制上下电
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

下面的项可以控制复位
enum{
	RESET_NVP6114_1 = 0,
	RESET_NVP6114_2,
	RESET_ETH0,
	RESET_ETH1,
	RESET_USBHUB,
	RESET_3G,
}RESET_ITEM;
