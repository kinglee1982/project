#include"PelcoP.h"
#include <string.h>
#include <stdio.h>

namespace ptz
{

CPelcoP::CPelcoP()
	:CPTZ(0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x40,0X00,0X3F),
     CCamera(0x01,0x02,0x04,0x08,0x10,0x20),
	 STX(0xA0),
	 ETX(0xAF),
	 CameraOn(0x80)
{
}

CPelcoP::~CPelcoP()
{
}

//设置预置位，清除预置位，转到预置位
void CPelcoP::Preset(void *proto,unsigned int deviceAddress, byte preset, byte action)

{
    byte m_action;
    switch (action)
    {
        case PRESET:
            m_action = 0x03;
            break;
        case PRECLEAR:
            m_action = 0x05;
            break;
        case PREGOTO:
            m_action = 0x07;
            break;
        default:
            m_action = 0x03;
            break;
    }
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,m_action,0x00,preset);

}

//巡航
void CPelcoP::Scout(void *proto,unsigned int deviceAddress,
					byte preset1,byte preset2,int times)
{
}

//巡航停止
void CPelcoP::ScoutStop()
{
}

void CPelcoP::Flip(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x07,0x00,0x21);
}

void CPelcoP::ZeroPanPosition(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x07,0x00,0x22);
}

void CPelcoP::SetAuxiliary(void *proto,
						   unsigned int deviceAddress,byte auxiliaryID, byte action)
{
    if(auxiliaryID<0x01)
    {
        auxiliaryID = 0x01;
    }
    if(auxiliaryID>0x08)
    {
        auxiliaryID = 0x08;
    }
    GetMessage((Pelco_p_Stru *)proto,deviceAddress,0x00,(byte)action,0x00,auxiliaryID);
}

void CPelcoP::RemoteReset(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x0F,0x00,0x00);
}

void CPelcoP::Zone(void *proto,unsigned int deviceAddress,byte zone, byte action)
{
    byte m_action;
    if(action == ACTSTART)
        m_action = 0x11;
    else
        m_action = 0x13;
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,m_action,0x00,zone);
}

void CPelcoP::ClearScreen(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x17,0x00,0x00);
}

void CPelcoP::AlarmAcknowledge(void *proto,unsigned int deviceAddress, unsigned int alarmID)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x19,0x00,byte(alarmID));
}

void CPelcoP::ZoneScan(void *proto,unsigned int deviceAddress,byte action)
{
    byte m_action;
    if(action == ACTSTART)
        m_action = 0x1B;
    else
        m_action = 0x1D;
    GetMessage((Pelco_p_Stru*)proto, deviceAddress,0x00,m_action,0x00,0x00);
}

void CPelcoP::Pattern(void *proto,unsigned int deviceAddress,byte action)
{
    byte m_action;
    switch (action)
    {
        case PATTERNSTART:
        m_action = 0x1F;
        break;
        case PATTERNSTOP:
        m_action = 0x21;
        break;
        case PATTERNRUN:
        m_action = 0x23;
        break;
        default:
        m_action = 0x23;
        break;
    }

    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,m_action,0x00,0x00);

}

void CPelcoP::SetZoomLensSpeed(void *proto,unsigned int deviceAddress, unsigned int speed)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x25,0x00,speed);
}

void CPelcoP::SetFocusLensSpeed(void *proto,unsigned int deviceAddress, byte speed)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x27,0x00,speed);
}

void CPelcoP::CameraSwitch(void *proto ,unsigned int deviceAddress,byte action)
{
    byte m_action = CameraOnOff;
    if(action == ON)
        m_action += CameraOn; //Maybe wrong !!!
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,m_action,0x00,0x00);

}
void CPelcoP::CameraIrisSwitch(void *proto ,unsigned int deviceAddress,byte action)
{
    return GetMessage((Pelco_p_Stru*)proto,deviceAddress,(byte)action,0x00,0x00,0x00);
}
void CPelcoP::CameraFocus(void *proto ,unsigned int deviceAddress,byte action)
{
    return GetMessage((Pelco_p_Stru*)proto,deviceAddress,(byte)action,0x00,0x00,0x00);
}
void CPelcoP::CameraZoom(void *proto ,unsigned int deviceAddress,byte action)
{
    return GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,(byte)action,0x00,0x00);
}

void CPelcoP::CameraTilt(void *proto ,unsigned int deviceAddress,
						 byte action, unsigned int speed)
{
    if(speed<PTILTSPEEDMIN)
        speed = PTILTSPEEDMIN;
    if(speed>PTILTSPEEDMAX)
        speed = PTILTSPEEDMAX;
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,(byte)action,0x00,(byte)speed);
}

void CPelcoP::CameraPan(void *proto ,unsigned int deviceAddress,
						byte action, unsigned int speed)
{
    if(speed < PTILTSPEEDMIN)
       speed = PTILTSPEEDMIN;
    if(speed > PTILTSPEEDMAX && speed < 0xFF)
       speed = PTILTSPEEDMAX;
    if(speed > 0xFF)
        speed = 0xFF;
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,(byte)action,(byte)speed,0x00);
}

void CPelcoP::CameraPanTilt(void *proto ,unsigned int deviceAddress,byte panAction, unsigned int panSpeed, byte tiltAction, unsigned int tiltSpeed)
{
    GetMessage((Pelco_p_Stru *)proto,deviceAddress,0x00,panAction + tiltAction,panSpeed,tiltSpeed);
}

void CPelcoP::CameraStop(void *proto ,unsigned int deviceAddress)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x00,0x00,0x00);
}

void CPelcoP::CameraScan(void *proto ,unsigned int deviceAddress,byte action)
{
    byte m_action;
    if(action == ACTSTART)
        m_action = 0x09;
    else
        m_action = 0x0B;
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,m_action,0x00,0x00);
}

void CPelcoP::LightSwitch(void *proto ,unsigned int deviceAddress,byte action)
{
    GetMessage((Pelco_p_Stru*)proto,deviceAddress,0x00,0x00,0x00,0x00);
}

void CPelcoP::GetMessage(Pelco_p_Stru *pp,unsigned int address,
						 byte data1, byte data2, byte data3, byte data4)
{
    if(0 == pp)
    {
        return;
    }

    byte checkSum = (byte)(STX ^ (byte)address ^ data1 ^ data2 ^ data3 ^ data4 ^ ETX);
    pp->STX = STX;
    pp->Address = address;
    pp->Data1 = data1;
    pp->Data2 = data2;
    pp->Data3 = data3;
    pp->Data4 = data4;
    pp->ETX = ETX;
    pp->CheckSum = checkSum;
}

//获取控制信息
bool CPelcoP::GetBuffer(const Pelco_p_Stru &pp,byte *buffer,int &size)
{
	if (buffer == NULL || size < 8)
	{
		return false;
	}
	memcpy(buffer,(byte *)&pp,8);
	size = 8;
	return true;
}


}


