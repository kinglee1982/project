#ifndef PELCO_D_H_
#define PELCO_D_H_

#include "Camera.h"

namespace ptz
{



class CPelcoD : public CPTZ,public CCamera
{

public:
    CPelcoD();
    virtual ~CPelcoD();

	//����Ԥ��λPreSet�����Ԥ��λPreClear ��ת��Ԥ��λPreGoto
	//preset: 01-20
	virtual void Preset(void *proto,unsigned int deviceAddress,
		byte preset,byte action);



	//��ת180
	virtual void Flip(void *proto,unsigned int deviceAddress);
	//ת�����pan
	virtual void ZeroPanPosition(void *proto,unsigned int deviceAddress);
	//���ø�����AuxSet ���������AuxClear
	//auxiliaryID:01-08
	virtual void SetAuxiliary(void *proto,unsigned int deviceAddress,
		byte auxiliaryID, byte action);
	//Զ�����°���
	virtual void RemoteReset(void *proto,unsigned int deviceAddress);
	//���ÿ�ʼ����ActStart�����ý�������ActStop
	//zone:01-08
	virtual void Zone(void *proto ,unsigned int deviceAddress,byte zone, byte action);
	//�����Ļ
	virtual void ClearScreen(void *proto,unsigned int deviceAddress);
	//������Ϣ
	virtual void AlarmAcknowledge(void *proto,unsigned int deviceAddress,
		unsigned int alarmID);
	//���������ActStart�����������ActStop
	virtual void ZoneScan(void *proto ,unsigned int deviceAddress,byte action);
	//���ÿ�ʼģʽPatternStart����ֹͣPatternStopģʽ����ģʽPatternRun
	virtual void Pattern(void *proto,unsigned int deviceAddress, byte action);
	//���ñ䱶�ٶ�
	//speed:00-03
	virtual void SetZoomLensSpeed(void *proto ,
		unsigned int deviceAddress, unsigned int speed);
	//���þ۽��ٶ�
	//speed:00-03
	virtual void SetFocusLensSpeed(void *proto ,unsigned int deviceAddress, byte speed);
	//������򿪣��ر�	on/off
	virtual void CameraSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//��Ȧ�򿪣��ر� dIrisOpen/dIrisClose
	virtual void CameraIrisSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//������������Զ dFocusFar/dFocusNear
	virtual void CameraFocus(void *proto ,unsigned int deviceAddress,byte action);
	//��Ұ�����խdZoomTele/dZoomWide
	virtual void CameraZoom(void *proto ,unsigned int deviceAddress,byte action);
	//��̨��ֱ�����ƶ�dTiltUp/dTiltDown   dTiltSpeedMin/dTiltSpeedMax
	virtual void CameraTilt(void *proto ,unsigned int deviceAddress,byte action,
		unsigned int speed);
	 //��̨ˮƽ�����ƶ�dPanLeft/dPanRight	 dPanSpeedMin/dPanSpeedMax
	virtual void CameraPan(void *proto,unsigned int deviceAddress,byte action,
		unsigned int speed);
	 //��̨���ϣ����£����ϣ������ƶ�
	virtual void CameraPanTilt(void *proto ,unsigned int deviceAddress,
		byte panAction,unsigned  int panSpeed, byte tiltAction, unsigned int tiltSpeed);
	//��ֹ̨ͣ�ƶ�
	virtual void CameraStop(void *proto,unsigned int deviceAddress);
	//�Զ����ֶ�ɨ�� ScanAuto/ScanManual
	virtual void CameraScan(void *proto ,unsigned int deviceAddress,byte scan);
	//�ƹ⿪/��
	void LightSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//���ô��ڲ�����
	//speed{38400,19200,9600,4800,2400,1200,300,38400,19200,9600,4800,2400,1200,300}
	void ResetCamera(void *proto,unsigned int deviceAddress);
	void AutoFocus(void *proto,unsigned int deviceAddress, byte action);
	void AutoIris(void *proto,unsigned int deviceAddress, byte action);
	void AGC(void *proto,unsigned int deviceAddress, byte action);
	void BackLightCompensation(void *proto,unsigned int deviceAddress, byte action);
	void AutoWhiteBalance(void *proto,unsigned int deviceAddress, byte action);
	void EnableDevicePhaseDelayMode(void *proto,unsigned int deviceAddress);
	void SetShutterSpeed(void *proto,unsigned int deviceAddress,byte speed);
	void AdjustLineLockPhaseDelay(void *proto,unsigned int deviceAddress);
	void AdjustWhiteBalanceRB(void *proto,unsigned int deviceAddress);
	void AdjustWhiteBalanceMG(void *proto,unsigned int deviceAddress);
	void AdjustGain(void *proto,unsigned int deviceAddress);
	void AdjustAutoIrisLevel(void *proto,unsigned int deviceAddress);
	void AdjustAutoIrisPeakValue(void *proto,unsigned int deviceAddress);
	void Query(void *proto,unsigned int deviceAddress);
	void QueryAngle(void *proto,unsigned int deviceAddress,byte action);

	//��ȡ������Ϣ
	bool GetBuffer(const Pelco_d_Stru &pp,byte *buffer,int &size);

private:
	//��ȡ��Ϣ
	void GetMessage(Pelco_d_Stru *pb,unsigned int address,
		byte command1, byte command2, byte data1, byte data2);

private:
	byte STX;
	byte Sense;

};


}

#endif


