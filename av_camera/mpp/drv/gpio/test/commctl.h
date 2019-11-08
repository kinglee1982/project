#ifndef __COMMCTL_H__
#define __COMMCTL_H__
//ETH_RST1  GPIO11_0  phy1 复位 	低复位
//ETH_RST2  GPIO11_1  phy0 复位		低复位
//USB_RST	GPIO4_5   USB HUB 复位 低复位	高正常
//SD_POW	GPIO8_5	SD卡电源		低正常
//NVP6114_RSB1  GPIO10_6		NVP6114_1 复位	低复位 	高正常
//NVP6114_RSB1  GPIO10_7		NVP6114_2 复位	低复位 	高正常
#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1
#define GPIO_HIGH	1
#define GPIO_LOW	0
#define GPIO_SET_DIR 0x1
#define GPIO_GET_DIR 0x2
#define GPIO_READ_BIT 0x3
#define GPIO_WRITE_BIT 0x4
#define POWER_ON	1
#define POWER_OFF	0

typedef struct {
	unsigned int  groupnumber;
	unsigned int  bitnumber;
	unsigned int  value;
}gpio_groupbit_info;

enum{
	GPIO0 = 0,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO4,
	GPIO5,
	GPIO6,
	GPIO7,
	GPIO8,
	GPIO9,
	GPIO10,
	GPIO11,
	GPIO12,
	GPIO13,
}GPIO_GROUP;
enum{
	BIT0 = 0,
	BIT1,
	BIT2,
	BIT3,
	BIT4,
	BIT5,
	BIT6,
	BIT7,
}GPIO_BIT;
enum{
	POWER_STM32 = 0,
	POWER_LED1,
	POWER_LED2,
	POWER_LED3,
	POWER_InAudio,
	POWER_OutAudio,
	POWER_TSAudio,
	POWER_3G,
	POWER_WIFI,
}POWER_ITEM;
enum{
	RESET_3G,
}RESET_ITEM;
enum{
	ENAOUT0 = 0,
	ENAOUT1,
	ENGSM0,
	ENGSM1,
}AUDIO_SWITCH;

#define GPIO_DEV	"/dev/hi_gpio"
#define MCP4531DEV "/dev/mcp4531"
#define IOC_MCP4531_SET_MIC_VOLUME  0x01
#define IOC_MCP4531_SET_AUDIO_VOLUME  0x03
#define IOC_MCP4531_GET_MIC_VOLUME  0x05
#define IOC_MCP4531_GET_AUDIO_VOLUME  0x07


#define SLEEP_TIME 100000
extern int COMMCTL_init(void);
extern int COMMCTL_exit(void);
extern int COMMCTL_Power(int item,int power);
extern int COMMCTL_Reset(int item);
extern int COMMCTL_SetMicVolume(unsigned char volume);
extern int COMMCTL_GetMicVolume(unsigned char *volume);
extern int COMMCTL_SetAudioVolume(unsigned char volume);
extern int COMMCTL_GetAudioVolume(unsigned char *volume);
extern int COMMCTL_Switch_Aout(int audio);
extern int COMMCTL_Switch_Gsm(int audio);
#endif
