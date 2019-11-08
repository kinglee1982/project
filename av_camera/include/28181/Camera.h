#ifndef CAMERA_H_
#define CAMERA_H_

namespace ptz
{


enum BasePTZCmd
{
	PRESET    = 0x01,
    PRECLEAR  = 0x02,
    PREGOTO   = 0x03,
    ACTSTART  = 0x04,
    ACTSTOP   = 0x05,

	LOW       = 0x06,
	MEDIUM    = 0x07,
    HIGH      = 0x08,
	TURBO     = 0x0A,
    AUXSET    = 0x09,
    AUXCLEAR  = 0x0B,

    PATTERNSTART = 0x0C,
    PATTERNSTOP  = 0x0D,
    PATTERNRUN   = 0x0E,

    AUTO         = 0x0F,
    ON           = 0x10,
    OFF          = 0x11,

    AUTOSCAN     = 0x12,
    MANUALSCAN   = 0x13
};


enum PelcoDCmd
{
	DPANRIGHT 	  = 0x02,
    DPANLEFT	  = 0x04,
    DTILTUP 	  = 0x08,
    DTILTDOWN 	  = 0x10,
    DZOOMTELE	  = 0x20,
    DZOOMWIDE 	  = 0x40,
    DPANSPEEDMIN  = 0x00,
    DPANSPEEDMAX  = 0x3F,
    DTILTSPEEDMIN = 0x00,
    DTILTSPEEDMAX = 0x3F,

    DFOCUSFAR 	  = 0x80,
    DFOCUSNEAR 	  = 0x01,
    DIRISOPEN 	  = 0x02,
    DIRISCLOSE	  = 0x04,
    DCAMERAONOFF  = 0x08,
    DAUTOMANUALSCAN  = 0x10,
    DLIGHTOPEN       = 0x11,
    DLIGHTCLOSE      = 0x12,
	QUERRYANGLE      = 0x51
};


enum PelcoPCmd
{
	PPANRIGHT	= 0x02,
	PPANLEFT	= 0x04,
	PTILTUP	    = 0x08,
	PTILTDOWN   = 0x10,
	PZOOMTELE 	= 0x20,
	PZOOMWIDE	= 0x40,
	PPANSPEEDMIN = 0x00,
	PPANSPEEDMAX = 0x3F,
	PTILTSPEEDMIN = 0x00,
	PTILTSPEEDMAX = 0x3F,
	PFOCUSFAR	  = 0x01,
	PFOCUSNEAR 	  = 0x02,
	PIRISOPEN	  = 0x04,
	PIRISCLOSE	  = 0x08,
	PCAMERAONOFF  = 0x10,
	PAUTOMANUALSCAN = 0x20
};



#ifndef _BYTE_
#define _BYTE_
typedef unsigned char byte;
#endif

#pragma pack( 1 )
typedef struct Pelco_d_Stru
{
	byte STX;
	byte Address;
	byte Command1;
	byte Command2;
	byte Data1;
	byte Data2;
	byte CheckSum;
	void init()
	{
		STX = 0;
		Address = 0;
		Command1 = 0;
		Command2 = 0;
		Data1 = 0;
		Data2 = 0;
		CheckSum = 0;
	}
}PELCO_D_S;

typedef struct Pelco_p_Stru
{
	byte STX;
	byte Address;
	byte Data1;
	byte Data2;
	byte Data3;
	byte Data4;
	byte ETX;
	byte CheckSum;
	void init()
	{
		STX = 0;
		Address = 0;
		Data1 = 0;
		Data2 = 0;
		Data1 = 0;
		Data2 = 0;
		CheckSum = 0;
	}
}PELCO_P_S;
#pragma pack ()


class CPTZ
{
public:
	CPTZ(){}
	CPTZ(byte pRight,byte pLeft,
		byte tUp,byte tDown,
		byte zTele,byte zWide,
		byte pSpeedMin,byte pSpeedMax,
		byte tSpeedMin,byte tSpeedMax);
	virtual ~CPTZ(){};
	virtual void Preset(void *proto,unsigned int deviceAddress,
		byte preset, byte action) = 0;

	virtual void Flip(void *proto,unsigned int deviceAddress) = 0;
	virtual void ZeroPanPosition(void *proto,unsigned int deviceAddress) = 0;
	virtual void SetAuxiliary(void *proto,unsigned int deviceAddress,
		byte auxiliaryID, byte action) = 0;
	virtual void RemoteReset(void *proto,unsigned int deviceAddress) = 0;
	virtual void Zone(void *proto,unsigned int deviceAddress,byte zone,
		byte action) = 0;
	virtual void ClearScreen(void *proto,unsigned int deviceAddress) = 0;
	virtual void AlarmAcknowledge(void *proto,unsigned int deviceAddress,
		unsigned int alarmID) = 0;
	virtual void ZoneScan(void *proto,unsigned int deviceAddress,byte action) = 0;
	virtual void Pattern(void *proto,unsigned int deviceAddress,byte action) = 0;
	virtual void SetZoomLensSpeed(void *proto,unsigned int deviceAddress,
		unsigned int speed) =0;
	virtual void CameraZoom(void *proto,unsigned int deviceAddress,byte action) = 0;
	virtual void CameraTilt(void *proto,unsigned int deviceAddress,byte action,
		unsigned int speed) = 0;
	virtual void CameraPan(void *proto,unsigned int deviceAddress,byte action,
		unsigned int speed) = 0;
	virtual void CameraPanTilt(void *proto,unsigned int deviceAddress,
		byte panAction, unsigned int panSpeed,
		byte tiltAction, unsigned int tiltSpeed) = 0;

protected:
	byte PanRight;
	byte PanLeft;
	byte TiltUp;
	byte TiltDown;
	byte ZoomTele;
	byte ZoomWide;
	byte PanSpeedMin;
	byte PanSpeedMax;
	byte TiltSpeedMin;
	byte TiltSpeedMax;
};

class CCamera
{
public:
		CCamera(){}
		CCamera(byte fFar,byte fNear,byte iOpen,byte iClose,byte cOnOff,byte amScan);
		virtual ~CCamera(){};
		virtual void SetFocusLensSpeed(void *proto,
			unsigned int deviceAddress, byte speed) = 0;
		virtual void CameraSwitch(void *proto,
			unsigned int deviceAddress,byte action) = 0;
		virtual void CameraIrisSwitch(void *proto,
			unsigned int deviceAddress,byte action) = 0;
		virtual void CameraFocus(void *proto,
			unsigned int deviceAddress,byte action) = 0;
		virtual void CameraStop(void *proto,
			unsigned int deviceAddress) = 0;
		virtual void CameraScan(void *proto ,
			unsigned int deviceAddress,byte scan) = 0;
		virtual void LightSwitch(void *proto ,
			unsigned int deviceAddress,byte action) = 0;

protected:
	byte FocusFar;
	byte FocusNear;
	byte IrisOpen;
	byte IrisClose;
	byte CameraOnOff;
	byte AutoManualScan;
};
class CPelco : public CPTZ,public CCamera
{

};

}

#endif


