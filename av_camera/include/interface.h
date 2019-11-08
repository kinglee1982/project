#ifndef __ERFACE_H__

#define __ERFACE_H__


#ifdef __cplusplus
extern "C"{
#endif

#define CHANNUM        4

typedef struct apccounter
{
    int inNum[CHANNUM];				//上车人数
    int outNum[CHANNUM];			//下车人数
    int doorS[CHANNUM];             //开关门状态，  1为开门 0为关门

    int stationInNum;        		//站点统计上车人数
    int stationOutNum;				//站点统计下车人数
}APCCOUNTER;

typedef enum
{
	FRONT_DOOR = 0,
	BACK_DOOR = 1,
}BUS_DOOR;

typedef enum
{
	DOOR_CLOSE = 0,
	DOOR_OPEN = 1,
}BUS_DOOR_STA;

typedef struct ApcParam
{
	int installH;			//高度过滤暂时未用				范围0				默认:0
	int noiseFilter;		//噪声过滤							范围120~200		默认:120
	int leftLimit;			//左边界								范围0~100			默认:0
	int rightLimit;			//右边界								范围260~360		默认:360
	int topLimit;			//上边界								范围0~30			默认:0
	int bottomLimit;		//下边界								范围258~288		默认:288
	int enableC;			//使能通道							范围0~1			默认:1
	int inoutType;			//0为上进下出 1为下进上出		范围0~1			默认:0
	int doorDelay;			//开关门延迟统计					范围0~5			默认:3
	int filterHeight;		//高度过滤							范围0~3			默认:0
	int counterType;		//计数类型							范围8或12或14		默认:8
	float sensitivity;		//灵敏度								范围10				默认:10
	int beeline[2];			//计数线								范围100~260		默认:0:106 1:174
	int headFilter;			//人头大小过滤						范围80~150			默认:80
	int forgTh;				//前景阈值							范围15~20			默认:15
}APCPARAM;

//接口反馈0为成功 -1 为失败

//客流算法初始化接口
int ApcIniterface();
//获取客流统计数据接口
int GetCounter(APCCOUNTER *conbInfo);
//客流算法主接口
int AlgDeal(int width, int height, unsigned char *img);
//设置通道开关门接口
int SetDoorState(int door, int state);
//通道清零接口
int CounterReset(int door);
//开启关闭算法保存日志接口
int EnableLog(int flag);
//获取算法参数接口
int GetApcParam(int door, APCPARAM *apcParam);
//设置算法参数接口
int SetApcParam(int door, APCPARAM apcParam);
//保存背景接口
int ApcLearnBKGround(int door);
//更新摄像机参数接口.读取摄像机参数二进制文件到内存中，在把这段数据通过这个接口更新。
int SetApcCameraParam(int door, char *buf, int bufsize);
//获取摄像机ID，id即摄像机编号8个字节保存
int GetApcCameraID(int door, char *id);


#ifdef __cplusplus
}
#endif


#endif

