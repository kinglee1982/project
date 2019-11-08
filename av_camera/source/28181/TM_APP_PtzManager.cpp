#include "Nvs.h"
#include "Camera.h"
#include "PelcoD.h"
#include "PelcoP.h"
#include "TM_APP_PtzManager.h"
#include <unistd.h>
#include <stdio.h>
CPTZManager::CPTZManager()
{
	m_cbDataSend = NULL;
}

int CPTZManager::SetSendDataHandle(DataCallback_t proc)
{
	m_cbDataSend = proc;
	return 0;
}

int CPTZManager::pelcopHandle(int cmd, int paddr,int speed, int data,const char *para)
{
	unsigned char ptzBuffer[12] = {0};
	int ptzLength = 12;
	m_pstroto.init();     ///初始化Pelcod协议

	switch(cmd)
	{
		case nvs::PTZ_CMD_RIGHT:// 右
		{
			m_pelcop.CameraPan(&m_pstroto,paddr,ptz::DPANRIGHT, speed);
			break;
		}
		case nvs::PTZ_CMD_LEFT: // 左
		{
			m_pelcop.CameraPan(&m_pstroto,paddr,ptz::DPANLEFT, speed);
			break;
		}
		case nvs::PTZ_CMD_UP:   // 上
		{
			m_pelcop.CameraTilt(&m_pstroto,paddr,ptz::DTILTUP, speed);
			break;
		}
		case nvs::PTZ_CMD_DOWN:	// 下
		{
			m_pelcop.CameraTilt(&m_pstroto,paddr,ptz::DTILTDOWN, speed);
			break;
		}
		case nvs::PTZ_CMD_ZOOM_IN:	// zoom in
		{
			m_pelcop.CameraZoom(&m_pstroto,paddr,ptz::DZOOMWIDE);
			break;
		}
		case nvs::PTZ_CMD_ZOOM_OUT:	// zoom out
			m_pelcop.CameraZoom(&m_pstroto,paddr,ptz::DZOOMTELE);
			break;
		case nvs::PTZ_CMD_FOCUS_IN:	// focus in
		{
			m_pelcop.CameraFocus(&m_pstroto,paddr,ptz::DFOCUSNEAR);
			break;
		}
		case nvs::PTZ_CMD_FOCUS_OUT:// focus out
		{
			m_pelcop.CameraFocus(&m_pstroto,paddr,ptz::DFOCUSFAR);
			break;
		}
		case nvs::PTZ_CMD_SMALL:         // small
		{
			m_pelcop.CameraIrisSwitch(&m_pstroto,paddr,ptz::DIRISCLOSE);
			break;
		}
		case nvs::PTZ_CMD_LARGE:	        // large
		{
			m_pelcop.CameraIrisSwitch(&m_pstroto,paddr,ptz::DIRISOPEN);
			break;
		}
		case nvs::PTZ_PRESET_ADD://预置位添加
		{
			m_pelcop.Preset(&m_pstroto,paddr, data,ptz::PRESET);
			break;
		}
		case nvs::PTZ_PRESET_GOTO://预置位调用
		{
			m_pelcop.Preset(&m_pstroto,paddr, data,ptz::PREGOTO);
			break;
		}
		case nvs::PTZ_PRESET_DELETE://预置位删除
		{
			m_pelcop.Preset(&m_pstroto,paddr, data,ptz::PRECLEAR);
			break;
		}
		default:// 停止
		{
			m_pelcop.CameraTilt(&m_pstroto,paddr,0,0);
			break;
		}
	}
	m_pelcop.GetBuffer(m_pstroto,ptzBuffer,ptzLength);
	m_cbDataSend((char*)ptzBuffer, ptzLength);
#if 0
		//发送停止命令
	usleep(200*1000);
	m_pelcop.CameraTilt(&m_pstroto,paddr,0,0);
	m_pelcop.GetBuffer(m_pstroto,ptzBuffer,ptzLength);
	m_cbDataSend((char*)ptzBuffer, ptzLength);
#endif
	return 0;
}

int CPTZManager::pelcodHandle(int cmd, int paddr,int speed, int data,const char *para)
{
	unsigned char ptzBuffer[12] = {0};
	int ptzLength = 12;
	m_dstroto.init();     ///初始化Pelcod协议

	switch(cmd)
	{
		case nvs::PTZ_CMD_RIGHT:// 右
		{
			m_pelcod.CameraPan(&m_dstroto,paddr,ptz::DPANRIGHT, speed);
			break;
		}
		case nvs::PTZ_CMD_LEFT: // 左
		{
			m_pelcod.CameraPan(&m_dstroto,paddr,ptz::DPANLEFT, speed);
			break;
		}
		case nvs::PTZ_CMD_UP:   // 上
		{
			m_pelcod.CameraTilt(&m_dstroto,paddr,ptz::DTILTUP, speed);
			break;
		}
		case nvs::PTZ_CMD_DOWN:	// 下
		{
			m_pelcod.CameraTilt(&m_dstroto,paddr,ptz::DTILTDOWN, speed);
			break;
		}
		case nvs::PTZ_CMD_ZOOM_IN:	// zoom in
		{
			m_pelcod.CameraZoom(&m_dstroto,paddr,ptz::DZOOMWIDE);
			break;
		}
		case nvs::PTZ_CMD_ZOOM_OUT:	// zoom out
			m_pelcod.CameraZoom(&m_dstroto,paddr,ptz::DZOOMTELE);
			break;
		case nvs::PTZ_CMD_FOCUS_IN:	// focus in
		{
			m_pelcod.CameraFocus(&m_dstroto,paddr,ptz::DFOCUSNEAR);
			break;
		}
		case nvs::PTZ_CMD_FOCUS_OUT:// focus out
		{
			m_pelcod.CameraFocus(&m_dstroto,paddr,ptz::DFOCUSFAR);
			break;
		}
		case nvs::PTZ_CMD_SMALL:         // small
		{
			m_pelcod.CameraIrisSwitch(&m_dstroto,paddr,ptz::DIRISCLOSE);
			break;
		}
		case nvs::PTZ_CMD_LARGE:	        // large
		{
			m_pelcod.CameraIrisSwitch(&m_dstroto,paddr,ptz::DIRISOPEN);
			break;
		}
		case nvs::PTZ_PRESET_ADD://预置位添加
		{
			m_pelcod.Preset(&m_dstroto,paddr, data,ptz::PRESET);
			break;
		}
		case nvs::PTZ_PRESET_GOTO://预置位调用
		{
			m_pelcod.Preset(&m_dstroto,paddr, data,ptz::PREGOTO);
			break;
		}
		case nvs::PTZ_PRESET_DELETE://预置位删除
		{
			m_pelcod.Preset(&m_dstroto,paddr, data,ptz::PRECLEAR);
			break;
		}
		default:// 停止
		{
			m_pelcod.CameraTilt(&m_dstroto,paddr,0,0);
			break;
		}
	}

	m_pelcod.GetBuffer(m_dstroto,ptzBuffer,ptzLength);
	if (NULL != m_cbDataSend)
	{
		m_cbDataSend((char*)ptzBuffer, ptzLength);
	}

#if 0
	//发送停止命令
	usleep(200*1000);
	m_pelcod.CameraTilt(&m_dstroto,paddr,0,0);
	m_pelcod.GetBuffer(m_dstroto,ptzBuffer,ptzLength);
	if (NULL != m_cbDataSend)
	{
		m_cbDataSend((char*)ptzBuffer, ptzLength);
	}
	printf("return before\n");
#endif

	return 0;
}

int CPTZManager::onPtzHandle(char procotol,int cmd, int paddr,int speed, int data,const char *para)
{
	switch(procotol)
	{
		/*pelco-d*/
		case 1:
		{
			//printf("%d  %d:\n",__LINE__,procotol);
			pelcodHandle( cmd, paddr, speed, data, para);
			break;
		}
		/* pelco-p */
		case 2:
		{
			printf("%d  %d:\n",__LINE__,procotol);
			pelcopHandle( cmd, paddr, speed, data, para);
			break;
		}
		default:
		{
			/*  */
			break;
		}
	}

	return 0;
}
