#ifndef __TM_ALARM_RECORD_H_
#define __TM_ALARM_RECORD_H_

#include "Nvs.h"
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "tinyxml2.h"



//#define ALARM_XML_NAME  "/mnt/nand/Alarm.xml"
#define ALARM_XML_NAME  "/mnt/flash/SIPAlarm.xml"
#define LABELNAME 256
#define RECIDMAGICNUM	0x00123456

typedef struct Alarm_record
{
    unsigned char alarmflag;
    int chnnum;
    int alarmbegntime;
    int alarmendtime;
}Alarm_record_param;


tinyxml2::XMLElement *RootElementAlarmXml();
int NewAlarmXmlText(Alarm_record_param *alarmparam);
//char *GetAlarmXmlText(const char *first_label,const char *second_label);
int GetAlarmRecXmlText(Alarm_record_param *alarmparam,nvs::QueryRecordResponse& resp);


#endif
