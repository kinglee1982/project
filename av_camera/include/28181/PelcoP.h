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

    //设置预置位PreSet，清除预置位PreClear ，转到预置位PreGoto
    //preset: 01-FF
    virtual void Preset(void *proto ,unsigned int deviceAddress,
		byte preset, byte action);

	//巡航
	virtual void Scout(void *proto,unsigned int deviceAddress,
		byte preset1,byte preset2,int times);
	//巡航停止
		virtual void ScoutStop();

	//翻转180
    virtual void Flip(void *proto,unsigned int deviceAddress);

	//转到零点pan
    virtual void ZeroPanPosition(void *proto ,unsigned int deviceAddress);
    //设置辅助的AuxSet 清除辅助的AuxClear

	//auxiliaryID:01-08
    virtual void SetAuxiliary(void *proto,unsigned int deviceAddress,
		byte auxiliaryID, byte action);

    //远程重新安排
    virtual void RemoteReset(void *proto ,unsigned int deviceAddress);

	//zone:01-08
    virtual void Zone(void *proto ,unsigned int deviceAddress,byte zone, byte action);
    //清除屏幕

    virtual void ClearScreen(void *proto,unsigned int deviceAddress);

    //警报信息
    virtual void AlarmAcknowledge(void *proto ,unsigned int deviceAddress,
		unsigned int alarmID);

    //区域浏览开ActStart，区域浏览关ActStop
    virtual void ZoneScan(void *proto,unsigned int deviceAddress,byte action);

    //设置开始模式PatternStart设置停止PatternStop模式运行模式PatternRun
    virtual void Pattern(void *proto ,unsigned int deviceAddress,byte action);

	//设置变倍速度
    //speed:00-03
    virtual void SetZoomLensSpeed(void *proto ,unsigned int deviceAddress,
		unsigned int speed);

	//设置聚焦速度
    //speed:00-03
    virtual void SetFocusLensSpeed(void *proto ,unsigned int deviceAddress, byte speed);

	//摄象机打开，关闭	on/off
    virtual void CameraSwitch(void *proto ,unsigned int deviceAddress,byte action);
    //光圈打开，关闭 pIrisOpen/pIrisClose

    virtual void CameraIrisSwitch(void *proto ,unsigned int deviceAddress,byte action);

	//焦距拉近，拉远 pFocusFar/pFocusNear
    virtual void CameraFocus(void *proto ,unsigned int deviceAddress,byte action);
    //视野变宽，变窄pZoomTele/pZoomWide

    virtual void CameraZoom(void *proto ,unsigned int deviceAddress,byte action);
    //云台垂直上下移动pTiltUp/pTiltDown   pTiltSpeedMin/pTiltSpeedMax

    virtual void CameraTilt(void *proto ,unsigned int deviceAddress,
		byte action, unsigned int speed);

    //云台水平左右移动pPanLeft/pPanRight	 pPanSpeedMin/pPanSpeedMax
    virtual void CameraPan(void *proto ,unsigned int deviceAddress,
		byte action, unsigned int speed);

	//云台左上，左下，右上，右下移动
    virtual void CameraPanTilt(void *proto ,unsigned int deviceAddress,
		byte panAction, unsigned int panSpeed, byte tiltAction, unsigned int tiltSpeed);

	//云台停止移动
    virtual void CameraStop(void *proto ,unsigned int deviceAddress);

	//自动扫描 ActStart/ActStop
	virtual void CameraScan(void *proto ,unsigned int deviceAddress,byte action);

	//灯光打开/关闭
    virtual void LightSwitch(void *proto ,unsigned int deviceAddress,byte action);


	//获取控制信息
	bool GetBuffer(const Pelco_p_Stru &pp,byte *buffer,int &size);

private:
	//获取信息
	void GetMessage(Pelco_p_Stru *pp,unsigned int address,
		byte data1, byte data2, byte data3, byte data4);

private:
	byte STX;
	byte ETX;
	byte CameraOn;
};

}

#endif


