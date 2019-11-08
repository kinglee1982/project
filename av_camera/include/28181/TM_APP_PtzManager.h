/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : TM_APP_pelcomanager.h
  Version       : Initial Draft
  Author        : mabuyun
  Created       : 2014/12/25
  Last Modified :
  Description   : TM_APP_pelcomanager.cpp header file
  Function List :
  History       :
  1.Date        : 2014/12/25
    Author      : mabuyun
    Modification: Created file

******************************************************************************/

#ifndef __TM_APP_PTZMANAGER_H__
#define __TM_APP_PTZMANAGER_H__

#include "Camera.h"
#include "PelcoD.h"
#include "PelcoP.h"

typedef int (*DataCallback_t)(char *data,int length );	//数据到达的回调函数

class CPTZManager
{
public:
    CPTZManager();
    int onPtzHandle(char procotol,int cmd, int chn,int speed, int data,const char *para);
    int pelcodHandle(int cmd, int paddr,int speed, int data,const char *para);
    int pelcopHandle(int cmd, int paddr,int speed, int data,const char *para);
    int SetSendDataHandle(DataCallback_t proc);
private:
    DataCallback_t  m_cbDataSend;
	ptz::CPelcoP    m_pelcop;
    ptz::Pelco_p_Stru  m_pstroto;
	ptz::CPelcoD    m_pelcod;
    ptz::Pelco_d_Stru  m_dstroto;
};



#endif /* __TM_APP_PELCOMANAGER_H__ */
