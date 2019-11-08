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

#define MCU485TRANS     0x13    /* 485数据透传命令 */
#define MCUIOREPORT     0x14    /* IO上报 */
#define MCUIOCTRONL     0x15    /* IO控制 */
#define MCUREBOOTARM    0x16    /* 重启命令 */

#define IO_STATUE_ON    1   /* IO、继电器状态 开  */
#define IO_STATUE_OFF   0   /* IO、继电器状态 关 */

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
