#ifndef __ERFACE_H__

#define __ERFACE_H__


#ifdef __cplusplus
extern "C"{
#endif

#define CHANNUM        4

typedef struct apccounter
{
    int inNum[CHANNUM];				//�ϳ�����
    int outNum[CHANNUM];			//�³�����
    int doorS[CHANNUM];             //������״̬��  1Ϊ���� 0Ϊ����

    int stationInNum;        		//վ��ͳ���ϳ�����
    int stationOutNum;				//վ��ͳ���³�����
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
	int installH;			//�߶ȹ�����ʱδ��				��Χ0				Ĭ��:0
	int noiseFilter;		//��������							��Χ120~200		Ĭ��:120
	int leftLimit;			//��߽�								��Χ0~100			Ĭ��:0
	int rightLimit;			//�ұ߽�								��Χ260~360		Ĭ��:360
	int topLimit;			//�ϱ߽�								��Χ0~30			Ĭ��:0
	int bottomLimit;		//�±߽�								��Χ258~288		Ĭ��:288
	int enableC;			//ʹ��ͨ��							��Χ0~1			Ĭ��:1
	int inoutType;			//0Ϊ�Ͻ��³� 1Ϊ�½��ϳ�		��Χ0~1			Ĭ��:0
	int doorDelay;			//�������ӳ�ͳ��					��Χ0~5			Ĭ��:3
	int filterHeight;		//�߶ȹ���							��Χ0~3			Ĭ��:0
	int counterType;		//��������							��Χ8��12��14		Ĭ��:8
	float sensitivity;		//������								��Χ10				Ĭ��:10
	int beeline[2];			//������								��Χ100~260		Ĭ��:0:106 1:174
	int headFilter;			//��ͷ��С����						��Χ80~150			Ĭ��:80
	int forgTh;				//ǰ����ֵ							��Χ15~20			Ĭ��:15
}APCPARAM;

//�ӿڷ���0Ϊ�ɹ� -1 Ϊʧ��

//�����㷨��ʼ���ӿ�
int ApcIniterface();
//��ȡ����ͳ�����ݽӿ�
int GetCounter(APCCOUNTER *conbInfo);
//�����㷨���ӿ�
int AlgDeal(int width, int height, unsigned char *img);
//����ͨ�������Žӿ�
int SetDoorState(int door, int state);
//ͨ������ӿ�
int CounterReset(int door);
//�����ر��㷨������־�ӿ�
int EnableLog(int flag);
//��ȡ�㷨�����ӿ�
int GetApcParam(int door, APCPARAM *apcParam);
//�����㷨�����ӿ�
int SetApcParam(int door, APCPARAM apcParam);
//���汳���ӿ�
int ApcLearnBKGround(int door);
//��������������ӿ�.��ȡ����������������ļ����ڴ��У��ڰ��������ͨ������ӿڸ��¡�
int SetApcCameraParam(int door, char *buf, int bufsize);
//��ȡ�����ID��id����������8���ֽڱ���
int GetApcCameraID(int door, char *id);


#ifdef __cplusplus
}
#endif


#endif

