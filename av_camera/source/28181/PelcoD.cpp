#include"PelcoD.h"
#include <string.h>
#include <stdio.h>


namespace ptz
{



CPelcoD::CPelcoD()
	:CPTZ(0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x3F,0X00,0X3F),
     CCamera(0X08,0x01,0x02,0x04,0x08,0x10),
	 STX(0xFF),
	 Sense(0x80)
{   
}

CPelcoD::~CPelcoD()
{   
}


//设置预置位，清除预置位，转到预置位
//preset :0x00-0x20
//清除全部预置位:preset=0x77

//lint -save -e525
void CPelcoD::Preset(void *proto,unsigned int deviceAddress, byte preset, byte action)
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
   GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_action,0x00,preset);
}

//lint -restore


//水平反转 
void CPelcoD::Flip(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x07,0x00,0x21);
}

//高速球水平复位
void CPelcoD::ZeroPanPosition(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x07,0x00,0x22);
}

//辅助功能
void CPelcoD::SetAuxiliary(void *proto,unsigned int deviceAddress,byte auxiliaryID, 
						   byte action)
{
    if(auxiliaryID<0x01)
    {
        auxiliaryID = 0x01;
    }
    if(auxiliaryID>0x08)
    {
       // auxiliaryID = 0x08;
    }

	//雨刷开 FF 1 0 9 0 1 B
	//雨刷关 FF 1 0 B 0 1 D
	// FF 1 0 7 0 1 3F 
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,action,0x00,auxiliaryID);

	// GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x07,0x00,preset);
	 
}

void CPelcoD::RemoteReset(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x0F,0x00,0x00);
}

void CPelcoD::Zone(void *proto,unsigned int deviceAddress,byte zone, byte action)
{
   
    byte m_action;
    if(action == ACTSTART)
    {
        m_action = 0x11;
    }
    else
    {
        m_action = 0x13;
    }
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_action,0x00,zone);
}

void CPelcoD::ClearScreen(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x17,0x00,0x00);
}

void CPelcoD::AlarmAcknowledge(void *proto,unsigned int deviceAddress, 
							   unsigned int alarmID)
{
    if((alarmID < 1) || (alarmID>8))
    {
        return;
    } 
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x19,0x00,byte(alarmID));
}

void CPelcoD::ZoneScan(void *proto,unsigned int deviceAddress,byte action)
{
    byte m_action;
    if(action == ACTSTART)
    {
        m_action = 0x1B;
    }
    else
    {
        m_action = 0x1D;  
    }
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_action,0x00,0x00);	

}

void CPelcoD::Pattern(void *proto,unsigned int deviceAddress,byte action)
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
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_action,0x00,0x00);
}

void CPelcoD::SetZoomLensSpeed(void *proto,unsigned int deviceAddress, 
							   unsigned int speed)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x25,0x00,(byte)speed);
}

void CPelcoD::SetFocusLensSpeed(void *proto,unsigned int deviceAddress, byte speed)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x27,0x00,(byte)speed); 
}

void CPelcoD::ResetCamera(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x29,0x00,0x00);
}

void CPelcoD::AutoFocus(void *proto,unsigned int deviceAddress, byte action)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x2B,0x00,(byte)action);
}

void CPelcoD::AutoIris(void *proto,unsigned int deviceAddress, byte action)
{  
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x2D,0x00,(byte)action);
}

void CPelcoD::AGC(void *proto,unsigned int deviceAddress, byte action)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x2F,0x00,(byte)action);
}

void CPelcoD::BackLightCompensation(void *proto,unsigned int deviceAddress, byte action)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x31,0x00,(byte)action);	
}

void CPelcoD::AutoWhiteBalance(void *proto,unsigned int deviceAddress, byte action)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x33,0x00,(byte)action);
}

void CPelcoD::EnableDevicePhaseDelayMode(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x35,0x00,0x00);
}

void CPelcoD::SetShutterSpeed(void *proto,unsigned int deviceAddress,byte speed)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x37,speed,speed);
}

void CPelcoD::AdjustLineLockPhaseDelay(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x39,0x00,0x00);
}

void CPelcoD::AdjustWhiteBalanceRB(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x3B,0x00,0x00);
}

void CPelcoD::AdjustWhiteBalanceMG(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x3D,0x00,0x00);
}

void CPelcoD::AdjustGain(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x3F,0x00,0x00);
}

void CPelcoD::AdjustAutoIrisLevel(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x41,0x00,0x00);
}

void CPelcoD::AdjustAutoIrisPeakValue(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x43,0x00,0x00);
}

void CPelcoD::Query(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x45,0x00,0x00);
}

void CPelcoD::CameraSwitch(void *proto,unsigned int deviceAddress,byte action)
{
    byte m_action = CameraOnOff;
    if(action == ON)
    {
        m_action = CameraOnOff + Sense;	
    } 
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_action,0x00,0x00);
}

//光圈变大/变小
void CPelcoD::CameraIrisSwitch(void *proto,unsigned int deviceAddress,byte action)
{
   GetMessage((Pelco_d_Stru *)proto,deviceAddress,(byte)action,0x00,0x00,0x00);
} 

//焦距拉近/拉远
void CPelcoD::CameraFocus(void *proto,unsigned int deviceAddress,byte action)
{
    if(action == FocusNear)
    {
        GetMessage((Pelco_d_Stru *)proto,deviceAddress,(byte)action,0x00,0x00,0x00);
    }
    else
    {
        GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,(byte)action,0x00,0x00);
    }
}
//视野变宽/变窄
void CPelcoD::CameraZoom(void *proto,unsigned int deviceAddress,byte action)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,(byte)action,0x00,0x00);
}

//上下移动速度:0x00-0x3F
void CPelcoD::CameraTilt(void *proto,unsigned int deviceAddress,byte action, 
						 unsigned int speed)
{
   // if(speed > DTILTSPEEDMAX)
   // speed = DTILTSPEEDMAX;
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,(byte)action,0x00,(byte)speed);
}

//平移速度 :0x00-0x3F,0xFF为最高速
void CPelcoD::CameraPan(void *proto,unsigned int deviceAddress,byte action,
						unsigned int speed)
{
    //if(speed > DTILTSPEEDMAX && speed < 0xFF)
    //speed = DTILTSPEEDMAX;
    //if(speed > 0xFF)
    //speed = 0xFF; 
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,(byte)action,(byte)speed,0x00);
}

void CPelcoD::CameraPanTilt(void *proto,unsigned int deviceAddress,byte panAction, 
							unsigned int panSpeed, byte tiltAction, 
							unsigned int tiltSpeed)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,panAction + tiltAction,
		panSpeed,tiltSpeed);
}

void CPelcoD::CameraStop(void *proto,unsigned int deviceAddress)
{
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,0x00,0x00,0x00);
}

void CPelcoD::CameraScan(void *proto,unsigned int deviceAddress,byte scan)
{
    byte m_byte = AutoManualScan;
    if(scan == AUTOSCAN)
    {
        m_byte = AutoManualScan+Sense;
    }
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,m_byte,0x00,0x00,0x00);
}

void CPelcoD::LightSwitch(void *proto ,unsigned int deviceAddress,byte action)
{
    byte m_byte;
    if(action == DLIGHTOPEN)
    {
        m_byte = 0x09;
    }
    else
    {
        m_byte = 0x0b;
    }
    GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,m_byte,0x00,0x01); 
}

void CPelcoD::QueryAngle(void *proto,unsigned int deviceAddress,byte action)
{
	GetMessage((Pelco_d_Stru *)proto,deviceAddress,0x00,action,0x00,0x00);
}


void CPelcoD::GetMessage(Pelco_d_Stru *pd,unsigned int address, byte command1, 
						 byte command2, byte data1, byte data2)
{
    
    if (address < 1 || address > 256)
    {
        return;
    }
	byte checkSum = (byte)(((byte)address + command1 + command2 + data1 + data2)%256);
	pd->STX = STX;
	pd->Address = address;
	pd->Command1 = command1;
	pd->Command2 = command2;
	pd->Data1 = data1;
	pd->Data2 = data2;
	pd->CheckSum = checkSum;
}

//获取控制信息
bool CPelcoD::GetBuffer(const Pelco_d_Stru &pp,byte *buffer,int &size)
{
	if (buffer == NULL || size < 7)
	{
		return false;
	}
	memcpy(buffer,(byte *)&pp,7);	
	size = 7;
	return true;
}


}


