/******************************************************************************

  Copyright (C), 2001-2011, zhengzhou Tiamaes Technology Co., Ltd.

 ******************************************************************************
  File Name     : TM_MCUSocket.h
  Version       : Initial Draft
  Author        : mabuyun
  Created       : 2014/12/29
  Last Modified :
  Description   : TM_MCUSocket.cpp header file
  Function List :
  History       :
  1.Date        : 2014/12/29
    Author      : mabuyun
    Modification: Created file

******************************************************************************/

#ifndef __TM_MCUSOCKET_H__
#define __TM_MCUSOCKET_H__

#define MCU485TRANS     0x13    /* 485����͸������ */
#define MCUIOREPORT     0x14    /* IO�ϱ� */
#define MCUIOCTRONL     0x15    /* IO���� */
#define MCUREBOOTARM    0x16    /* �������� */

#define IO_STATUE_ON    1   /* IO���̵���״̬ ��  */
#define IO_STATUE_OFF   0   /* IO���̵���״̬ �� */

typedef enum
{
    AlarmIORelaySleep = 0,
    AlarmIORelayRest ,
    AlarmIOACC ,
    AlarmIOReverse ,
    AlarmIOBackdoorClose ,
    AlarmIOBackdoorOpen ,
    AlarmIOFrontdoorClose ,
    AlarmIOFrontdoorOpen ,
}AlarmIO_e;

typedef enum
{
    CtrlIORelaySleep = 6,
    CtrlIORelayRest = 7,
}CtrlIO_e;

extern int CtrlIOStatue(CtrlIO_e iobit,int flag);
extern int IOMsgProc(int Socketfd,char *buf,int len);
extern int RebootArm();
extern int SendPtzMsgToMcu( char *buf,int len);
extern int TmMCUMsg( int Socketfd,char *buf,int len);
extern void setMcuIOAlarmCallback(int (* pFunc)(char*,int));
extern unsigned char GetMcuAlarmIO();

#endif /* __TM_MCUSOCKET_H__ */
