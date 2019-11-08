#include "Camera.h"

namespace ptz
{


CCamera::CCamera(byte fFar,byte fNear,byte iOpen,byte iClose,byte cOnOff,byte amScan)
{
    FocusFar = fFar;
    FocusNear = fNear;
    IrisOpen = iOpen;
    IrisClose = iClose;
    CameraOnOff = cOnOff;
    AutoManualScan = amScan;
}


CPTZ::CPTZ(byte pRight,byte pLeft,
		   byte tUp,byte tDown,
		   byte zTele,byte zWide,byte pSpeedMin,
		   byte pSpeedMax,byte tSpeedMin,byte tSpeedMax)
{
	PanRight = pRight;
	PanLeft = pLeft;
	TiltUp = tUp;
	TiltDown = tDown;
	ZoomTele = zTele;
	ZoomWide = zWide;
	PanSpeedMin = pSpeedMin;
	PanSpeedMax = pSpeedMax;
	TiltSpeedMin = tSpeedMin;
	TiltSpeedMax = tSpeedMax;
}


}


