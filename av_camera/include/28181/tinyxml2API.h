#ifndef __TINYXML2API_H_
#define __TINYXML2API_H_


#include "Nvs.h"
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "tinyxml2.h"
#include "av_ocx_def.h"
#include "av_ocx_type.h"
/* BEGIN: Added by duzhanhui, 2016/12/8   PN:16120301090 */
#include "28181_defaultConfig.h"
/* END:   Added by duzhanhui, 2016/12/8   PN:16120301090 */


#define XML_NAME  "/mnt/flash/SIP.xml"
#define LABELLEN 256
#define XML_DEBUG(format, ...) 	//printf(format, ##__VA_ARGS__)




int SetXmlDeviceParam(WEBOCX_DEVPARA_ST *deviceparam);
int GetXmlDeviceParam(WEBOCX_DEVPARA_ST *deviceparam);



int SetXmlNetParam(WEBOCX_NETWORK_PARA_ST *deviceparam);
int GetXmlNetParam(WEBOCX_NETWORK_PARA_ST *deviceparam);


int SetXmlCamParam(int chnnum,WEBOCX_CHN_INFO_ST *deviceparam);
int GetXmlCamParam(int chnnum,WEBOCX_CHN_INFO_ST *deviceparam);


int SetXmlCamPTZParam(int chnnum,WEBOCX_CAMERA_PARA_ST *deviceparam);
int GetXmlCamPTZParam(int chnnum,WEBOCX_CAMERA_PARA_ST *pdevicecamPTZparam);



int SetXmlNodeParam(int chnnum,WEBOCX_NODE_SET_ST *deviceparam);
int GetXmlNodeParam(int chnnum,WEBOCX_NODE_SET_ST *deviceparam);

int SetXmlMDParam(int chnnum,WEBOCX_MD_SET_ST *deviceparam);
int GetXmlMDParam(int chnnum,WEBOCX_MD_SET_ST *deviceparam);


int GetXmlVoideLoseParam(WEBOCX_LOSTFRAM_ST *deviceparam);
int SetXmlVoideLoseParam(WEBOCX_LOSTFRAM_ST *deviceparam);

/*摄像头ID数据结构 */
typedef struct device_camer
{
    char CameraSIPID[32];
    int CamerAddSIP;
    int CameraPDSIP;
    int CamerBuzzDuration;
    char CamerName[128];
}device_camer_id;

int GetXmlcamerid(device_camer_id *camerid);


extern const char *cameranumstr[];
extern int Getcameranumstr();



tinyxml2::XMLElement *RootElementXml();
int SetXmlText(const char *label,char *text);
int SetXmlText(const char *first_label,const char *second_label,char *text);

char *GetXmlText(const char *label);
char *GetXmlText(const char *first_label,const char *second_label);

void PrintXML(const char * text);
bool XmlError(const char * text);
/*设备信息读取*/
#ifdef PLT_PARA_BK
int GetXmlPlatformRegInfo2(nvs::PlatformRegInfo *devicereginfo);
#endif
int GetXmlPlatformRegInfo(nvs::PlatformRegInfo *devicereginfo);
#ifdef PLT_PARA_BK
int SetXmlPlatformRegInfo2(nvs::PlatformRegInfo *devicereginfo);
#endif
int SetXmlPlatformRegInfo(nvs::PlatformRegInfo *devicereginfo);

int	GetXmlPlatformRegInfo(int num,nvs::ResNode *deviceresreport);
int GetXmlDeviceStatus(nvs::DeviceStatusParam *devicestatus);
int	GetXmlDeviceInfo(nvs::DeviceInfoParam *deviceinfo);
#endif
