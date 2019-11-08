#ifndef PELCO_P_H_
#define PELCO_P_H_

#include "Camera.h"

namespace ptz
{


class CPelcoP : public CPTZ,public CCamera
{
public:
    CPelcoP();
    virtual ~CPelcoP();

    //����Ԥ��λPreSet�����Ԥ��λPreClear ��ת��Ԥ��λPreGoto
    //preset: 01-FF
    virtual void Preset(void *proto ,unsigned int deviceAddress,
		byte preset, byte action);

	//Ѳ��
	virtual void Scout(void *proto,unsigned int deviceAddress,
		byte preset1,byte preset2,int times);
	//Ѳ��ֹͣ
		virtual void ScoutStop();

	//��ת180
    virtual void Flip(void *proto,unsigned int deviceAddress);

	//ת�����pan
    virtual void ZeroPanPosition(void *proto ,unsigned int deviceAddress);
    //���ø�����AuxSet ���������AuxClear

	//auxiliaryID:01-08
    virtual void SetAuxiliary(void *proto,unsigned int deviceAddress,
		byte auxiliaryID, byte action);

    //Զ�����°���
    virtual void RemoteReset(void *proto ,unsigned int deviceAddress);

	//zone:01-08
    virtual void Zone(void *proto ,unsigned int deviceAddress,byte zone, byte action);
    //�����Ļ

    virtual void ClearScreen(void *proto,unsigned int deviceAddress);

    //������Ϣ
    virtual void AlarmAcknowledge(void *proto ,unsigned int deviceAddress,
		unsigned int alarmID);

    //���������ActStart�����������ActStop
    virtual void ZoneScan(void *proto,unsigned int deviceAddress,byte action);

    //���ÿ�ʼģʽPatternStart����ֹͣPatternStopģʽ����ģʽPatternRun
    virtual void Pattern(void *proto ,unsigned int deviceAddress,byte action);

	//���ñ䱶�ٶ�
    //speed:00-03
    virtual void SetZoomLensSpeed(void *proto ,unsigned int deviceAddress,
		unsigned int speed);

	//���þ۽��ٶ�
    //speed:00-03
    virtual void SetFocusLensSpeed(void *proto ,unsigned int deviceAddress, byte speed);

	//������򿪣��ر�	on/off
    virtual void CameraSwitch(void *proto ,unsigned int deviceAddress,byte action);
    //��Ȧ�򿪣��ر� pIrisOpen/pIrisClose

    virtual void CameraIrisSwitch(void *proto ,unsigned int deviceAddress,byte action);

	//������������Զ pFocusFar/pFocusNear
    virtual void CameraFocus(void *proto ,unsigned int deviceAddress,byte action);
    //��Ұ�����խpZoomTele/pZoomWide

    virtual void CameraZoom(void *proto ,unsigned int deviceAddress,byte action);
    //��̨��ֱ�����ƶ�pTiltUp/pTiltDown   pTiltSpeedMin/pTiltSpeedMax

    virtual void CameraTilt(void *proto ,unsigned int deviceAddress,
		byte action, unsigned int speed);

    //��̨ˮƽ�����ƶ�pPanLeft/pPanRight	 pPanSpeedMin/pPanSpeedMax
    virtual void CameraPan(void *proto ,unsigned int deviceAddress,
		byte action, unsigned int speed);

	//��̨���ϣ����£����ϣ������ƶ�
    virtual void CameraPanTilt(void *proto ,unsigned int deviceAddress,
		byte panAction, unsigned int panSpeed, byte tiltAction, unsigned int tiltSpeed);

	//��ֹ̨ͣ�ƶ�
    virtual void CameraStop(void *proto ,unsigned int deviceAddress);

	//�Զ�ɨ�� ActStart/ActStop
	virtual void CameraScan(void *proto ,unsigned int deviceAddress,byte action);

	//�ƹ��/�ر�
    virtual void LightSwitch(void *proto ,unsigned int deviceAddress,byte action);


	//��ȡ������Ϣ
	bool GetBuffer(const Pelco_p_Stru &pp,byte *buffer,int &size);

private:
	//��ȡ��Ϣ
	void GetMessage(Pelco_p_Stru *pp,unsigned int address,
		byte data1, byte data2, byte data3, byte data4);

private:
	byte STX;
	byte ETX;
	byte CameraOn;
};

}

#endif


