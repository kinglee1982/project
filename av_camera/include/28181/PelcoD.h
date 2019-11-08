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

	//设置预置位PreSet，清除预置位PreClear ，转到预置位PreGoto
	//preset: 01-20
	virtual void Preset(void *proto,unsigned int deviceAddress,
		byte preset,byte action);



	//翻转180
	virtual void Flip(void *proto,unsigned int deviceAddress);
	//转到零点pan
	virtual void ZeroPanPosition(void *proto,unsigned int deviceAddress);
	//设置辅助的AuxSet 清除辅助的AuxClear
	//auxiliaryID:01-08
	virtual void SetAuxiliary(void *proto,unsigned int deviceAddress,
		byte auxiliaryID, byte action);
	//远程重新安排
	virtual void RemoteReset(void *proto,unsigned int deviceAddress);
	//设置开始区域ActStart，设置结束区域ActStop
	//zone:01-08
	virtual void Zone(void *proto ,unsigned int deviceAddress,byte zone, byte action);
	//清除屏幕
	virtual void ClearScreen(void *proto,unsigned int deviceAddress);
	//警报信息
	virtual void AlarmAcknowledge(void *proto,unsigned int deviceAddress,
		unsigned int alarmID);
	//区域浏览开ActStart，区域浏览关ActStop
	virtual void ZoneScan(void *proto ,unsigned int deviceAddress,byte action);
	//设置开始模式PatternStart设置停止PatternStop模式运行模式PatternRun
	virtual void Pattern(void *proto,unsigned int deviceAddress, byte action);
	//设置变倍速度
	//speed:00-03
	virtual void SetZoomLensSpeed(void *proto ,
		unsigned int deviceAddress, unsigned int speed);
	//设置聚焦速度
	//speed:00-03
	virtual void SetFocusLensSpeed(void *proto ,unsigned int deviceAddress, byte speed);
	//摄象机打开，关闭	on/off
	virtual void CameraSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//光圈打开，关闭 dIrisOpen/dIrisClose
	virtual void CameraIrisSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//焦距拉近，拉远 dFocusFar/dFocusNear
	virtual void CameraFocus(void *proto ,unsigned int deviceAddress,byte action);
	//视野变宽，变窄dZoomTele/dZoomWide
	virtual void CameraZoom(void *proto ,unsigned int deviceAddress,byte action);
	//云台垂直上下移动dTiltUp/dTiltDown   dTiltSpeedMin/dTiltSpeedMax
	virtual void CameraTilt(void *proto ,unsigned int deviceAddress,byte action,
		unsigned int speed);
	 //云台水平左右移动dPanLeft/dPanRight	 dPanSpeedMin/dPanSpeedMax
	virtual void CameraPan(void *proto,unsigned int deviceAddress,byte action,
		unsigned int speed);
	 //云台左上，左下，右上，右下移动
	virtual void CameraPanTilt(void *proto ,unsigned int deviceAddress,
		byte panAction,unsigned  int panSpeed, byte tiltAction, unsigned int tiltSpeed);
	//云台停止移动
	virtual void CameraStop(void *proto,unsigned int deviceAddress);
	//自动，手动扫描 ScanAuto/ScanManual
	virtual void CameraScan(void *proto ,unsigned int deviceAddress,byte scan);
	//灯光开/关
	void LightSwitch(void *proto ,unsigned int deviceAddress,byte action);
	//设置串口波特率
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

	//获取控制信息
	bool GetBuffer(const Pelco_d_Stru &pp,byte *buffer,int &size);

private:
	//获取信息
	void GetMessage(Pelco_d_Stru *pb,unsigned int address,
		byte command1, byte command2, byte data1, byte data2);

private:
	byte STX;
	byte Sense;

};


}

#endif


