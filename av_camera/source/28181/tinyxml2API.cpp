#include "tinyxml2API.h"
#include <unistd.h>
#include <assert.h>
using namespace std;
using namespace tinyxml2;


XMLDocument myDocument;
static bool xmlOK = false;

const char *netparam[] = {"IpAddr","NetMask","NetGate",
                          "MultAddr","MacAddr","DnsAddr1",
                          "DnsAddr2"
                         };

const char *cameranumstr[] = {"CameraFirst","CameraSecond","CameraThird",
                              "CameraFourth","CameraFifth","CameraSixth",
                              "CameraSeventh","CameraEighth","CameraNinth",
                              "CameraTenth","CameraEleventh","CameraTwelfth"
                             };

const char *camerparam[] = {"Id","Name","CameraAdd","CameraPD"};

const char *alarmumnumstr[] = {"AlarmNodeOne","AlarmNodeTwo"};


int Getcameranumstr()
{
    int chnnum = 0;
    chnnum = sizeof(cameranumstr)/sizeof(int);
    return chnnum;
}
#ifdef PLT_PARA_BK
/* BEGIN: Modified by duzhanhui, 2016/12/8 DES:SIP.xml异常处理  PN:16120301090 */
int CheckSubAddressParam()
{
    bool bFlag = false;
    XMLElement *scene = myDocument.RootElement();
    if(NULL == scene)
    {
        printf( "*********function=%s, line = %d,readfile erro!!**********\n", __FUNCTION__,__LINE__);
        return -1;
    }
    XMLElement *SvrSurface = scene->FirstChildElement("ServerAddr");
    XMLElement *surface = SvrSurface->FirstChildElement("IpSub");
    XMLElement *newsurface = NULL;
    if(surface==NULL)
    {
        bFlag = true;
        newsurface = myDocument.NewElement("IpSub");
        newsurface->SetText("116.255.198.211");
        SvrSurface->InsertEndChild(newsurface);
    }
    surface = SvrSurface->FirstChildElement("PortSub");
    if(surface==NULL)
    {
        bFlag = true;
        newsurface = myDocument.NewElement("PortSub");
        newsurface->SetText("10002");
        SvrSurface->InsertEndChild(newsurface);
    }
    if(bFlag)
    {
        printf( "*********function=%s, line = %d ,write backup ip and port!**********\n", __FUNCTION__,__LINE__);
        myDocument.SaveFile(XML_NAME);
        bFlag = false;
    }
    return 0;
}
/* END:   Modified by duzhanhui, 2016/12/8   PN:16120301090 */
#endif
/* BEGIN: Modified by duzhanhui, 2016/12/8  DES:SIP.xml异常处理 PN:16120301090 */
bool LoadXMLPara()
{
    if(!xmlOK)
    {
        if(0!=access(XML_NAME,F_OK))
        {
            printf( "*********function=%s, line = %d file not exist!**********\n", __FUNCTION__,__LINE__);
            CreateDefaultCfg(XML_NAME);
        }
        if(myDocument.LoadFile(XML_NAME)!=XML_NO_ERROR)
        {
            XML_DEBUG("LoadFile error \n");
            xmlOK = false;
        }
        else
        {
            xmlOK = true;
#ifdef PLT_PARA_BK
            if(CheckSubAddressParam())
            {
                CreateDefaultCfg(XML_NAME);
                printf( "*********function=%s, line = %d,check**********\n", __FUNCTION__,__LINE__);
                if(myDocument.LoadFile(XML_NAME)!=XML_NO_ERROR)
                {
                    XML_DEBUG("LoadFile error \n");
                    xmlOK = false;
                }
                else
                {
                    xmlOK = true;
                }
            }
#endif
        }
    }
    return xmlOK;
}
/* END:   Modified by duzhanhui, 2016/12/8   PN:16120301090 */
void PrintXML(const char * text)
{
    printf(text);
    myDocument.Print();
}
bool XmlError(const char * text)
{
    if(!xmlOK)
        return false;
    printf("%s %p\n",text,&myDocument);
    XMLElement *scene = myDocument.RootElement();
    if(scene->FirstChildElement("NvsAddr")==NULL)
    {
        myDocument.Print();
        exit(0);
    }
    return true;
}
XMLElement *RootElementXml()
{
    if(!LoadXMLPara())
    {
        return NULL;
    }

    //得到跟节点
    XMLElement *scene = myDocument.RootElement();
    if(scene == NULL)
    {
        XML_DEBUG("RootElementXml is NULL \n");
        return NULL;
    }
    cout<< scene->Name() << ":" << scene->Value()<<scene<< endl;
    return scene;
}

int SetXmlText(const char *label,char *text)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }

    XMLElement *surface;
    if((surface = scene->FirstChildElement(label)) != NULL)
    {
        //XML_DEBUG("%s\n",surface->Name());
        if((strcmp(surface->Name(),label)) == 0);
        {
            if((text != NULL))
            {
                surface->SetText(text);
                XML_DEBUG("%s\n",surface->GetText());
                myDocument.SaveFile(XML_NAME);
            }
        }

    }
    return 0;
}

int SetXmlText(const char *first_label,const char *second_label,char *text)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    if((surface = scene->FirstChildElement(first_label)->FirstChildElement(second_label)) != NULL)
    {
        //XML_DEBUG("%s\n",surface->Name());
        if((strcmp(surface->Name(),second_label)) == 0);
        {
            if((text != NULL))
            {
                surface->SetText(text);
                XML_DEBUG("%s\n",surface->GetText());
            }
        }

    }
    myDocument.SaveFile(XML_NAME);
    return 0;
}

char *GetXmlText(const char *label)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return NULL;
    }
    XMLElement *surface;

    static char labelname[LABELLEN] = {0};
    memset(labelname,0,sizeof(labelname));

    if((surface = scene->FirstChildElement(label)) != NULL)
    {
        //XML_DEBUG("%s\n",surface->Name());
        if((strcmp(surface->Name(),label)) == 0);
        {
            if((surface->GetText() != NULL))
            {
                const char* title = surface->GetText();
                XML_DEBUG("%s\n",title);
                strcpy(labelname,title);
                XML_DEBUG("%s\n",labelname);
            }
        }

    }

    return labelname;
}

char *GetXmlText(const char *first_label,const char *second_label)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return NULL;
    }

    XMLElement *surface;

    static char labelname[LABELLEN];
    memset(labelname,0,sizeof(labelname));

    if((surface = scene->FirstChildElement(first_label)->FirstChildElement(second_label)) != NULL)
    {
        //XML_DEBUG("%s\n",surface->Name());
        if((strcmp(surface->Name(),second_label)) == 0);
        {
            if((surface->GetText() != NULL))
            {
                const char* title = surface->GetText();
                XML_DEBUG("%s\n",title);
                strcpy(labelname,title);
                XML_DEBUG("%s\n",labelname);
            }
        }

    }

    return labelname;
}


/*设置系统IP信息*/
int SetXmlNetParam(WEBOCX_NETWORK_PARA_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }

    XMLElement *surface;
    //char buffer[16] = {0};
    int i = 0;

    if((surface = scene->FirstChildElement()) != NULL)
    {
        XML_DEBUG("FirstChildElement:%s\n",surface->Name());
        while(surface)
        {
            if(surface->Name() != NULL)
            {
                XML_DEBUG("surface->Name:%s\n",surface->Name());
                if(strcmp(netparam[0],surface->Name()) == 0)
                {
                    if(deviceparam->IpAddr != NULL)
                    {
                        surface->SetText(deviceparam->IpAddr);
                        XML_DEBUG("deviceparam->IpAddr:%s\n",deviceparam->IpAddr);
                    }
                    ++i;
                }
                else if(strcmp(netparam[1],surface->Name()) == 0)
                {
                    if(deviceparam->NetMask != NULL)
                    {
                        surface->SetText(deviceparam->NetMask);
                        XML_DEBUG("deviceparam->NetMask:%s\n",deviceparam->NetMask);
                    }
                    ++i;
                }
                else if(strcmp(netparam[2],surface->Name()) == 0)
                {
                    if(deviceparam->NetGate != NULL)
                    {
                        surface->SetText(deviceparam->NetGate);
                        XML_DEBUG("deviceparam->NetGate:%s\n",deviceparam->NetGate);
                    }
                    ++i;
                }
                else if(strcmp(netparam[3],surface->Name()) == 0)
                {
                    if(deviceparam->MultAddr != NULL)
                    {
                        surface->SetText(deviceparam->MultAddr);
                        XML_DEBUG("deviceparam->NetGate:%s\n",deviceparam->NetGate);
                    }
                    ++i;
                }
                else if(strcmp(netparam[4],surface->Name()) == 0)
                {
                    if(deviceparam->MacAddr != NULL)
                    {
                        surface->SetText(deviceparam->MacAddr);
                        XML_DEBUG("deviceparam->MacAddr:%s\n",deviceparam->MacAddr);
                    }
                    ++i;
                }
                else if(strcmp(netparam[5],surface->Name()) == 0)
                {
                    if(deviceparam->DnsAddr1 != NULL)
                    {
                        surface->SetText(deviceparam->DnsAddr1);
                        XML_DEBUG("deviceparam->DnsAddr1:%s\n",deviceparam->DnsAddr1);
                    }
                    ++i;
                }
                else if(strcmp(netparam[6],surface->Name()) == 0)
                {
                    if(deviceparam->DnsAddr2 != NULL)
                    {
                        surface->SetText(deviceparam->DnsAddr2);
                        XML_DEBUG("deviceparam->DnsAddr2:%s\n",deviceparam->DnsAddr2);
                    }
                    ++i;
                }
            }

            surface = surface->NextSiblingElement();
            XML_DEBUG("%d\n",i);
            if(i == 7)
                break;
        }

    }

    myDocument.SaveFile(XML_NAME);

    return 0;
}


/*获取系统IP信息*/
int GetXmlNetParam(WEBOCX_NETWORK_PARA_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }

    XMLElement *surface;
    //char buffer[16] = {0};
    int i = 0;

    if((surface = scene->FirstChildElement()) != NULL)
    {
        XML_DEBUG("FirstChildElement:%s\n",surface->Name());
        while(surface)
        {
            if(surface->Name() != NULL)
            {
                XML_DEBUG("surface->Name:%s\n",surface->Name());
                if(strcmp(netparam[0],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->IpAddr,surface->GetText());
                        XML_DEBUG("deviceparam->IpAddr:%s\n",deviceparam->IpAddr);
                    }
                    ++i;
                }
                else if(strcmp(netparam[1],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->NetMask,surface->GetText());
                        XML_DEBUG("deviceparam->NetMask:%s\n",deviceparam->NetMask);
                    }
                    ++i;
                }
                else if(strcmp(netparam[2],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->NetGate,surface->GetText());
                        XML_DEBUG("deviceparam->NetGate:%s\n",deviceparam->NetGate);
                    }
                    ++i;
                }
                else if(strcmp(netparam[3],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->MultAddr,surface->GetText());
                        XML_DEBUG("deviceparam->MultAddr:%s\n",deviceparam->MultAddr);
                    }
                    ++i;
                }
                else if(strcmp(netparam[4],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->MacAddr,surface->GetText());
                        XML_DEBUG("deviceparam->MacAddr:%s\n",deviceparam->MacAddr);
                    }
                    ++i;
                }
                else if(strcmp(netparam[5],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->DnsAddr1,surface->GetText());
                        XML_DEBUG("deviceparam->DnsAddr1:%s\n",deviceparam->DnsAddr1);
                    }
                    ++i;
                }
                else if(strcmp(netparam[6],surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(deviceparam->DnsAddr2,surface->GetText());
                        XML_DEBUG("deviceparam->DnsAddr2:%s\n",deviceparam->DnsAddr2);
                    }
                    ++i;
                }
            }

            surface = surface->NextSiblingElement();
            if(i == 7)
                break;
        }

    }

    return 0;
}

/*设置PTZ信息*/
int SetXmlCamPTZParam(int chnnum,WEBOCX_CAMERA_PARA_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }

    XMLElement *surface;
    char info[16] = {0};

    XML_DEBUG("deviceparam->chunum:%d\n",chnnum);
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("CameraAdd")) != NULL)
    {
        if(strcmp("CameraAdd",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->CameraAddr);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }

    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("CameraPD")) != NULL)
    {
        if(strcmp("CameraPD",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->CameraProto);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("SerialId")) != NULL)
    {
        if(strcmp("SerialId",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->SerialId);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("DataBit")) != NULL)
    {
        if(strcmp("DataBit",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->DataBit);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("StopBit")) != NULL)
    {
        if(strcmp("StopBit",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->StopBit);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("AuthenRule")) != NULL)
    {
        if(strcmp("AuthenRule",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->AuthenRule);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("CameraSpeed")) != NULL)
    {
        if(strcmp("CameraSpeed",surface->Name()) == 0)
        {
            sprintf(info,"%d",deviceparam->Speed);
            surface->SetText(info);
            XML_DEBUG("info:%s\n",info);
        }
    }

    myDocument.SaveFile(XML_NAME);
    return 0;
}


/*获取PTZ信息*/
int GetXmlCamPTZParam(int chnnum,WEBOCX_CAMERA_PARA_ST *pdevicecamPTZparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int i = 0;

    for(i=0; i<chnnum; i++)
    {
        XML_DEBUG("pdevicecamPTZparam->chunum:%d[index:%d]\n",chnnum,i);
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraAdd")) != NULL)
        {
            if(strcmp("CameraAdd",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->CameraAddr = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->CameraAddr:%d\n",i,(pdevicecamPTZparam+i)->CameraAddr);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraPD")) != NULL)
        {
            if(strcmp("CameraPD",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->CameraProto = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->ppd:%d\n",i,(pdevicecamPTZparam+i)->CameraProto);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("SerialId")) != NULL)
        {
            if(strcmp("SerialId",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->SerialId = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->SerialId:%d\n",i,(pdevicecamPTZparam+i)->SerialId);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("DataBit")) != NULL)
        {
            if(strcmp("DataBit",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->DataBit = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->DataBit:%d\n",i,(pdevicecamPTZparam+i)->DataBit);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("StopBit")) != NULL)
        {
            if(strcmp("StopBit",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->StopBit = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->StopBit:%d\n",i,(pdevicecamPTZparam+i)->StopBit);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("AuthenRule")) != NULL)
        {
            if(strcmp("AuthenRule",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->AuthenRule = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->AuthenRule:%d\n",i,(pdevicecamPTZparam+i)->AuthenRule);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraSpeed")) != NULL)
        {
            if(strcmp("CameraSpeed",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (pdevicecamPTZparam+i)->Speed = atoi(surface->GetText());
                    XML_DEBUG("(pdevicecamPTZparam+%d)->Speed:%d\n",i,(pdevicecamPTZparam+i)->Speed);
                }
            }
        }

    }


    return 0;
}


/*获取设备信息*/
int GetXmlDeviceParam(WEBOCX_DEVPARA_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    if((surface = scene->FirstChildElement("DeviceId")) != NULL)
    {
        XML_DEBUG("DeviceId:%s\n",surface->Name());
        if(strcmp("DeviceId",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->DevId,surface->GetText());
                XML_DEBUG("deviceparam->DevId:%s\n",deviceparam->DevId);
            }
        }
    }
    if((surface = scene->FirstChildElement("Model")) != NULL)
    {
        XML_DEBUG("Model:%s\n",surface->Name());
        if(strcmp("Model",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->DevName,surface->GetText());
                XML_DEBUG("deviceparam->DevName:%s\n",deviceparam->DevName);
            }
        }
    }
    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("NvsAddr-Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->PlatformDevId,surface->GetText());
                XML_DEBUG("deviceparam->PlatformDevId:%s\n",deviceparam->PlatformDevId);
            }
        }
    }
    if((surface = scene->FirstChildElement("ArarmName")) != NULL)
    {
        XML_DEBUG("ArarmName:%s\n",surface->Name());
        if(strcmp("ArarmName",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->Domain,surface->GetText());
                XML_DEBUG("deviceparam->Domain:%s\n",deviceparam->Domain);
            }
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->PassWd,surface->GetText());
                XML_DEBUG("deviceparam->PassWd:%s\n",deviceparam->PassWd);
            }
        }
    }
    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        XML_DEBUG("Expires:%s\n",surface->Name());
        if(strcmp("Expires",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                deviceparam->TimeOut = atoi(surface->GetText());
                XML_DEBUG("deviceparam->TimeOut:%d\n",deviceparam->TimeOut);
            }
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Ip")) != NULL)
    {
        XML_DEBUG("ServerAddr-Ip:%s\n",surface->Name());
        if(strcmp("Ip",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->SipSvrIp,surface->GetText());
                XML_DEBUG("deviceparam->SipSvrIp:%s\n",deviceparam->SipSvrIp);
            }
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("ServerAddr-Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                deviceparam->SipSvrPort = atoi(surface->GetText());
                XML_DEBUG("deviceparam->SipSvrPort:%d\n",deviceparam->SipSvrPort);
            }
        }
    }

    if((surface = scene->FirstChildElement("IpAddr")) != NULL)
    {
        XML_DEBUG("IpAddr-Ip:%s\n",surface->Name());
        if(strcmp("IpAddr",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceparam->LocalIp,surface->GetText());
                XML_DEBUG("deviceparam->LocalIp:%s\n",deviceparam->LocalIp);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("NvsAddr-Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                deviceparam->LocalPort = atoi(surface->GetText());
                XML_DEBUG("deviceparam->LocalPort:%d\n",deviceparam->LocalPort);
            }
        }
    }

    return 0;
}

/*设置设备信息*/
int SetXmlDeviceParam(WEBOCX_DEVPARA_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    char buffer[16] = {0};

    if((surface = scene->FirstChildElement("DeviceId")) != NULL)
    {
        XML_DEBUG("DeviceId:%s\n",surface->Name());
        if(strcmp("DeviceId",surface->Name()) == 0)
        {
            if(deviceparam->DevId != NULL)
            {
                surface->SetText(deviceparam->DevId);
                XML_DEBUG("deviceparam->DevId:%s\n",deviceparam->DevId);
            }
        }
    }
    if((surface = scene->FirstChildElement("Model")) != NULL)
    {
        XML_DEBUG("Model:%s\n",surface->Name());
        if(strcmp("Model",surface->Name()) == 0)
        {
            if(deviceparam->DevName != NULL)
            {
                surface->SetText(deviceparam->DevName);
                XML_DEBUG("deviceparam->DevName:%s\n",deviceparam->DevName);
            }
        }
    }
    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("NvsAddr-Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(deviceparam->PlatformDevId != NULL)
            {
                surface->SetText(deviceparam->PlatformDevId);
                XML_DEBUG("deviceparam->PlatformDevId:%s\n",deviceparam->PlatformDevId);
            }
        }
    }
    if((surface = scene->FirstChildElement("ArarmName")) != NULL)
    {
        XML_DEBUG("ArarmName:%s\n",surface->Name());
        if(strcmp("ArarmName",surface->Name()) == 0)
        {
            if(deviceparam->Domain != NULL)
            {
                surface->SetText(deviceparam->Domain);
                XML_DEBUG("deviceparam->Domain:%s\n",deviceparam->Domain);
            }
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            if(deviceparam->PassWd != NULL)
            {
                surface->SetText(deviceparam->PassWd);
                XML_DEBUG("deviceparam->PassWd:%s\n",deviceparam->PassWd);
            }
        }
    }

    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        if(strcmp("Expires",surface->Name()) == 0)
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"%d",deviceparam->TimeOut);
            if(buffer != NULL)
            {
                surface->SetText(buffer);
                XML_DEBUG("deviceparam->TimeOut:%d\n",deviceparam->TimeOut);
            }
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Ip")) != NULL)
    {
        XML_DEBUG("deviceparam->SipSvrIp:%s\n",deviceparam->SipSvrIp);
        if(strcmp("Ip",surface->Name()) == 0)
        {
            if(deviceparam->SipSvrIp != NULL)
            {
                surface->SetText(deviceparam->SipSvrIp);
                XML_DEBUG("deviceparam->SipSvrIp:%s\n",deviceparam->SipSvrIp);
            }
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("deviceparam->SipSvrPort:%d\n",deviceparam->SipSvrPort);
        if(strcmp("Port",surface->Name()) == 0)
        {
            sprintf(buffer,"%d",deviceparam->SipSvrPort);
            if(buffer != NULL)
            {
                surface->SetText(buffer);
                XML_DEBUG("deviceparam->SipSvrPort:%d\n",deviceparam->SipSvrPort);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Ip")) != NULL)
    {
        if(strcmp("Ip",surface->Name()) == 0)
        {
            if(deviceparam->LocalIp != NULL)
            {
                surface->SetText(deviceparam->LocalIp);
                XML_DEBUG("deviceparam->LocalIp:%s\n",deviceparam->LocalIp);
            }
        }
    }
    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Port")) != NULL)
    {
        if(strcmp("Port",surface->Name()) == 0)
        {
            sprintf(buffer,"%d",deviceparam->LocalPort);
            if(buffer != NULL)
            {
                surface->SetText(buffer);
                XML_DEBUG("deviceparam->LocalPort:%d\n",deviceparam->LocalPort);
            }

        }
    }

    myDocument.SaveFile(XML_NAME);

    return 0;
}

/*获取摄像头信息*/
int GetXmlCamParam(int chnnum,WEBOCX_CHN_INFO_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }

    XMLElement *surface;
    int i = 0;

    for(i=0; i<chnnum; i++)
    {
        (deviceparam+i)->ChnID = i;
        XML_DEBUG("deviceparam->Chnnum:%d\n",(deviceparam+i)->ChnID);
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Id")) != NULL)
        {
            if(strcmp("Id",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceparam+i)->PlatformDevId,surface->GetText());
                    XML_DEBUG("deviceparam->PlatformDevId:%s\n",(deviceparam+i)->PlatformDevId);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Name")) != NULL)
        {
            if(strcmp("Name",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceparam+i)->ChnName,surface->GetText());
                    XML_DEBUG("deviceparam->ChnName:%s\n",(deviceparam+i)->ChnName);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraAdd")) != NULL)
        {
            if(strcmp("CameraAdd",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->CameraAddr = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->CameraAddr:%d\n",(deviceparam+i)->CameraAddr);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraPD")) != NULL)
        {
            if(strcmp("CameraPD",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->ProtType = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->ProtType:%d\n",(deviceparam+i)->ProtType);
                }
            }
        }
    }

    return 0;
}

/*设置摄像头信息*/
int SetXmlCamParam(int chnnum,WEBOCX_CHN_INFO_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    //char buffer[16] = {0};
    int i = 0;

    for(i=0; i<chnnum; i++)
    {
        if((deviceparam+i)->ChnID ==i)
        {
            XML_DEBUG("deviceparam+%d->Chnnum:%d\n",i,(deviceparam+i)->ChnID);
            if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Id")) != NULL)
            {
                if(strcmp("Id",surface->Name()) == 0)
                {
                    if((deviceparam+i)->PlatformDevId != NULL)
                    {
                        surface->SetText((deviceparam+i)->PlatformDevId);
                        XML_DEBUG("(deviceparam+%d)->PlatformDevId:%s\n",i,(deviceparam+i)->PlatformDevId);
                    }
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Name")) != NULL)
            {
                if(strcmp("Name",surface->Name()) == 0)
                {
                    if((deviceparam+i)->ChnName != NULL)
                    {
                        surface->SetText((deviceparam+i)->ChnName);
                        XML_DEBUG("(deviceparam+%d)->ChnName:%s\n",i,(deviceparam+i)->ChnName);
                    }
                }
            }
#if 0
            if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Address")) != NULL)
            {
                if(strcmp("Address",surface->Name()) == 0)
                {
                    memset(buffer,0,sizeof(buffer));
                    sprintf(buffer,"tm-%s",(deviceparam+i)->ChnName);
                    if(buffer != NULL)
                    {
                        surface->SetText(buffer);
                        XML_DEBUG("(deviceparam+%d)->CameraAddr:%s\n",i,buffer);
                    }
                }
            }
#endif
        }

    }

#if 0
    XML_DEBUG("deviceparam->Chnnum:%d\n",deviceparam->Chnnum);
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("Id")) != NULL)
    {
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(deviceparam->PlatformDevId != NULL)
            {
                surface->SetText(deviceparam->PlatformDevId);
                XML_DEBUG("deviceparam->PlatformDevId:%s\n",deviceparam->PlatformDevId);
            }
        }
    }

    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("Name")) != NULL)
    {
        if(strcmp("Name",surface->Name()) == 0)
        {
            if(deviceparam->ChnName != NULL)
            {
                surface->SetText(deviceparam->ChnName);
                XML_DEBUG("deviceparam->ChnName:%s\n",deviceparam->ChnName);
            }
        }
    }
#if 1
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("Address")) != NULL)
    {
        if(strcmp("Address",surface->Name()) == 0)
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"tm-%s",deviceparam->ChnName);
            if(buffer != NULL)
            {
                surface->SetText(buffer);
                XML_DEBUG("deviceparam->CameraAddr:%s\n",buffer);
            }
        }
    }

#else
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("CameraAdd")) != NULL)
    {
        if(strcmp("CameraAdd",surface->Name()) == 0)
        {
            if(deviceparam->CameraAddr >= 0)
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"%d",deviceparam->CameraAddr);
                if(buffer != NULL)
                {
                    surface->SetText(buffer);
                    XML_DEBUG("deviceparam->CameraAddr:%d\n",deviceparam->CameraAddr);
                }
            }
        }
    }
    if((surface = scene->FirstChildElement(cameranumstr[chnnum])->FirstChildElement("CameraPD")) != NULL)
    {
        if(strcmp("CameraPD",surface->Name()) == 0)
        {
            if(deviceparam->ProtType >= 1)
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"%d",deviceparam->CameraAddr);
                if(buffer != NULL)
                {
                    surface->SetText(buffer);
                    XML_DEBUG("deviceparam->ProtType:%d\n",deviceparam->ProtType);
                }
            }
        }
    }
#endif
#endif
    myDocument.SaveFile(XML_NAME);
    return 0;
}


/*获取节点信息*/
int GetXmlNodeParam(int chnnum,WEBOCX_NODE_SET_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int i = 0,j = 0;
    int m =0;
    char alarmxmldata[128] = {0} ;
    char alarmtime[128] = {0};
    int hour, min, sec, ehour, emin, esec;
    WEBOCX_ALERT_TIME_ST *pstAlarmTime = NULL;

    for(i=0; i<chnnum; i++)
    {
        (deviceparam+i)->NodeId = (i+1);
        XML_DEBUG("deviceparam->NodeId:%d\n",(deviceparam+i)->NodeId);
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlarmStat")) != NULL)
        {
            if(strcmp("AlarmStat",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->AlarmStatus = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->alarmstat:%d\n",(deviceparam+i)->AlarmStatus);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlarmEvent")) != NULL)
        {
            if(strcmp("AlarmEvent",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceparam+i)->AlarmEvent,surface->GetText());
                    XML_DEBUG("deviceparam->AlarmEvent:%s\n",(deviceparam+i)->AlarmEvent);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlertEnable")) != NULL)
        {
            if(strcmp("AlertEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->AlertEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlertEnable:%d\n",(deviceparam+i)->AlertEnable);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlarmEnable")) != NULL)
        {
            if(strcmp("AlarmEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->AlarmEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlarmEnable:%d\n",(deviceparam+i)->AlarmEnable);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("BeepEnable")) != NULL)
        {
            if(strcmp("BeepEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->BuzzerEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->BeepEnable:%d\n",(deviceparam+i)->BuzzerEnable);
                }
            }
        }

        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlarmSpaceTime")) != NULL)
        {
            if(strcmp("AlarmSpaceTime",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->DetectInterval = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlarmSpaceTime:%d\n",(deviceparam+i)->DetectInterval);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("LinkSnap")) != NULL)
        {
            if(strcmp("LinkSnap",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->LinkSnap = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkSnap:%d\n",(deviceparam+i)->LinkSnap);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("LinkRecord")) != NULL)
        {
            if(strcmp("LinkRecord",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->LinkRecord = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkRecord:%d\n",(deviceparam+i)->LinkRecord);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("LinkOutput")) != NULL)
        {
            if(strcmp("LinkOutput",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->LinkOutput = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkOutput:%d\n",(deviceparam+i)->LinkOutput);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("LinkRecDuration")) != NULL)
        {
            if(strcmp("LinkRecDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->LinkRecDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkRecDuration:%d\n",(deviceparam+i)->LinkRecDuration);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("AlarmOutDuration")) != NULL)
        {
            if(strcmp("AlarmOutDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->AlarmOutDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlarmOutDuration:%d\n",(deviceparam+i)->AlarmOutDuration);
                }
            }
        }
        if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement("BeepOutDuration")) != NULL)
        {
            if(strcmp("BeepOutDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+i)->BuzzerDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->BeepOutDuration:%d\n",(deviceparam+i)->BuzzerDuration);
                }
            }
        }


        for(m=0; m<7; m++)
        {
            for(j=0; j<4; j++)
            {
                pstAlarmTime = &(deviceparam+i)->AlertTime[m][j];
                memset(alarmxmldata,0,sizeof(alarmxmldata));
                sprintf(alarmxmldata,"Data%dTime%d",m,j);
                if((surface = scene->FirstChildElement(alarmumnumstr[i])->FirstChildElement(alarmxmldata)) != NULL)
                {
                    if(strcmp(alarmxmldata,surface->Name()) == 0)
                    {
                        if(surface->GetText() != NULL)
                        {
                            strcpy(alarmtime,surface->GetText());
                            XML_DEBUG("alarmtime:%s\n",alarmtime);
                        }
                    }
                }

                sscanf(alarmtime,"%2d:%2d:%2d,%2d:%2d:%2d",
                       &hour,&min,&sec,&ehour,&emin,&esec);

                pstAlarmTime->AlertStartHour = hour;
                pstAlarmTime->AlertStartMinu = min;
                pstAlarmTime->AlertStartSecond = sec;
                pstAlarmTime->AlertEndHour = ehour;
                pstAlarmTime->AlertEndMinu = emin;
                pstAlarmTime->AlertEndSecond = esec;

                XML_DEBUG("alarmtime:%s,i:%d,j:%d\n",alarmtime,m,j);

            }

        }

    }

    return 0;
}

/*设置节点信息*/
int SetXmlNodeParam(int chnnum,WEBOCX_NODE_SET_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int j = 0;
    int m = 0;
    char enable[128] = {0};
    char alarmxmldata[128] = {0} ;

    XML_DEBUG("deviceparam->NodeId:%d\n",deviceparam->NodeId);
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlarmStat")) != NULL)
    {
        if(strcmp("AlarmStat",surface->Name()) == 0)
        {
            if((deviceparam->AlarmStatus == 0) ||(deviceparam->AlarmStatus == 1))
            {
                sprintf(enable,"%d",deviceparam->AlarmStatus);
                surface->SetText(deviceparam->AlarmStatus);
                XML_DEBUG("deviceparam->alarmstat:%d\n",deviceparam->AlarmStatus);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlarmEvent")) != NULL)
    {
        if(strcmp("AlarmEvent",surface->Name()) == 0)
        {
            if(deviceparam->AlarmEvent != NULL)
            {
                surface->SetText(deviceparam->AlarmEvent);
                XML_DEBUG("deviceparam->AlarmEvent:%s\n",deviceparam->AlarmEvent);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlertEnable")) != NULL)
    {
        if(strcmp("AlertEnable",surface->Name()) == 0)
        {
            if((deviceparam->AlertEnable == 0) ||(deviceparam->AlarmEnable == 1))
            {
                sprintf(enable,"%d",deviceparam->AlertEnable);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->AlarmEnable:%d\n",deviceparam->AlertEnable);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlarmEnable")) != NULL)
    {
        if(strcmp("AlarmEnable",surface->Name()) == 0)
        {
            if((deviceparam->AlarmEnable == 0) ||(deviceparam->AlarmEnable == 1))
            {
                sprintf(enable,"%d",deviceparam->AlarmEnable);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->AlarmEnable:%d\n",deviceparam->AlarmEnable);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("BeepEnable")) != NULL)
    {
        if(strcmp("BeepEnable",surface->Name()) == 0)
        {
            if((deviceparam->BuzzerEnable == 0) ||(deviceparam->BuzzerEnable == 1))
            {
                sprintf(enable,"%d",deviceparam->BuzzerEnable);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->BeepEnable:%d\n",deviceparam->BuzzerEnable);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlarmSpaceTime")) != NULL)
    {
        if(strcmp("AlarmSpaceTime",surface->Name()) == 0)
        {
            if((0 <= deviceparam->DetectInterval) &&(deviceparam->DetectInterval < 255))
            {
                sprintf(enable,"%d",deviceparam->DetectInterval);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->DetectInterval:%d\n",deviceparam->DetectInterval);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("LinkSnap")) != NULL)
    {
        if(strcmp("LinkSnap",surface->Name()) == 0)
        {
            sprintf(enable,"%d",deviceparam->LinkSnap);
            surface->SetText(enable);
            XML_DEBUG("deviceparam->LinkSnap:%d\n",deviceparam->LinkSnap);
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("LinkRecord")) != NULL)
    {
        if(strcmp("LinkRecord",surface->Name()) == 0)
        {
            sprintf(enable,"%d",deviceparam->LinkRecord);
            surface->SetText(enable);
            XML_DEBUG("deviceparam->LinkRecord:%d\n",deviceparam->LinkRecord);
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("LinkOutput")) != NULL)
    {
        if(strcmp("LinkOutput",surface->Name()) == 0)
        {
            sprintf(enable,"%d",deviceparam->LinkOutput);
            surface->SetText(enable);
            XML_DEBUG("deviceparam->LinkOutput:%d\n",deviceparam->LinkOutput);
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("LinkRecDuration")) != NULL)
    {
        if(strcmp("LinkRecDuration",surface->Name()) == 0)
        {
            if((0 <= deviceparam->LinkRecDuration) &&(deviceparam->LinkRecDuration < 255))
            {
                sprintf(enable,"%d",deviceparam->LinkRecDuration);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->LinkRecDuration:%d\n",deviceparam->LinkRecDuration);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("AlarmOutDuration")) != NULL)
    {
        if(strcmp("AlarmOutDuration",surface->Name()) == 0)
        {
            if((0 <= deviceparam->AlarmOutDuration) &&(deviceparam->AlarmOutDuration < 255))
            {
                sprintf(enable,"%d",deviceparam->AlarmOutDuration);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->AlarmOutDuration:%d\n",deviceparam->AlarmOutDuration);
            }
        }
    }
    if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement("BeepOutDuration")) != NULL)
    {
        if(strcmp("BeepOutDuration",surface->Name()) == 0)
        {
            if((0 <= deviceparam->BuzzerDuration) &&(deviceparam->BuzzerDuration < 255))
            {
                sprintf(enable,"%d",deviceparam->BuzzerDuration);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->BeepOutDuration:%d\n",deviceparam->BuzzerDuration);
            }
        }
    }

    for(m=0; m<7; m++)
    {
        for(j=0; j<4; j++)
        {
            memset(alarmxmldata,0,sizeof(alarmxmldata));
            sprintf(alarmxmldata,"Data%dTime%d",m,j);
            sprintf(enable,"%2d:%2d:%2d,%2d:%2d:%2d",deviceparam->AlertTime[m][j].AlertStartHour,
                    deviceparam->AlertTime[m][j].AlertStartMinu,deviceparam->AlertTime[m][j].AlertStartSecond,
                    deviceparam->AlertTime[m][j].AlertEndHour,deviceparam->AlertTime[m][j].AlertEndMinu,
                    deviceparam->AlertTime[m][j].AlertEndSecond);
            XML_DEBUG("enable:%s j:%d\n",enable,j);
            if((surface = scene->FirstChildElement(alarmumnumstr[chnnum-1])->FirstChildElement(alarmxmldata)) != NULL)
            {
                if(strcmp(alarmxmldata,surface->Name()) == 0)
                {
                    if(enable != NULL)
                    {
                        surface->SetText(enable);
                        XML_DEBUG("(deviceparam+i)->AlertTime[m][j]:%s\n",enable);
                    }
                }
            }
        }

    }


    myDocument.SaveFile(XML_NAME);
    return 0;
}

/*获取视频丢失信息*/
int GetXmlVoideLoseParam(WEBOCX_LOSTFRAM_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    if((surface = scene->FirstChildElement("ViodeLoseControl")->FirstChildElement("Enable")) != NULL)
    {
        if(strcmp("Enable",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                deviceparam->Enable = atoi(surface->GetText());
                XML_DEBUG("deviceparam->Enable:%d\n",deviceparam->Enable);
            }
        }
    }
    if((surface = scene->FirstChildElement("ViodeLoseControl")->FirstChildElement("CameraNum")) != NULL)
    {
        if(strcmp("CameraNum",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                deviceparam->ChnBit = atoi(surface->GetText());
                XML_DEBUG("deviceparam->ChnBit:%d\n",deviceparam->ChnBit);
            }
        }
    }

    return 0;
}

/*设置视频丢失信息*/
int SetXmlVoideLoseParam(WEBOCX_LOSTFRAM_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    char enable[128] = {0};

    if((surface = scene->FirstChildElement("ViodeLoseControl")->FirstChildElement("Enable")) != NULL)
    {
        if(strcmp("Enable",surface->Name()) == 0)
        {
            if((deviceparam->Enable == 0) || (deviceparam->Enable == 1))
            {
                sprintf(enable,"%d",deviceparam->Enable);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->Enable:%d\n",deviceparam->Enable);
            }
        }
    }
    if((surface = scene->FirstChildElement("ViodeLoseControl")->FirstChildElement("CameraNum")) != NULL)
    {
        if(strcmp("CameraNum",surface->Name()) == 0)
        {
            if(deviceparam->ChnBit >= 0)
            {
                sprintf(enable,"%d",deviceparam->ChnBit);
                surface->SetText(enable);
                XML_DEBUG("deviceparam->ChnBit:%d\n",deviceparam->ChnBit);
            }
        }
    }

    myDocument.SaveFile(XML_NAME);

    return 0;
}

/*获取移动侦测信息*/
int GetXmlMDParam(int chnnum,WEBOCX_MD_SET_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int i = 0;
    int j = 0;
    int m = 0;
    char alarmxmldata[128] = {0} ;
    char alarmtime[128] = {0};
    WEBOCX_ALERT_TIME_ST *pstAlarmTime = NULL;
    WEBOCX_DETECT_AREA_ST *pAarmarea = NULL;
    int hour, min, sec, ehour, emin, esec;
    int leftx,lefty,rightx,righty;

    for(m=0; m<chnnum; m++)
    {
        (deviceparam+m)->ChnId = m;
        XML_DEBUG("deviceparam->ChnNum:%d\n",(deviceparam+m)->ChnId);

        for(i=0; i<7; i++)
        {
            for(j=0; j<4; j++)
            {
                pstAlarmTime = &(deviceparam+m)->AlertTime[i][j];
                memset(alarmxmldata,0,sizeof(alarmxmldata));
                sprintf(alarmxmldata,"Data%dTime%d",i,j);
                if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement(alarmxmldata)) != NULL)
                {
                    if(strcmp(alarmxmldata,surface->Name()) == 0)
                    {
                        if(surface->GetText() != NULL)
                        {
                            strcpy(alarmtime,surface->GetText());
                            XML_DEBUG("alarmtime:%s\n",alarmtime);
                        }
                    }
                }

                sscanf(alarmtime,"%2d:%2d:%2d,%2d:%2d:%2d",
                       &hour,&min,&sec,&ehour,&emin,&esec);

                pstAlarmTime->AlertStartHour = hour;
                pstAlarmTime->AlertStartMinu = min;
                pstAlarmTime->AlertStartSecond = sec;
                pstAlarmTime->AlertEndHour = ehour;
                pstAlarmTime->AlertEndMinu = emin;
                pstAlarmTime->AlertEndSecond = esec;
            }
        }



        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlertEnable")) != NULL)
        {
            if(strcmp("AlertEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->AlertEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlertEnable:%d\n",(deviceparam+m)->AlertEnable);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlarmEnable")) != NULL)
        {
            if(strcmp("AlarmEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->AlarmEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlarmEnable:%d\n",(deviceparam+m)->AlarmEnable);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("Senibility")) != NULL)
        {
            if(strcmp("Senibility",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->Senibility = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->Senibility:%d\n",(deviceparam+m)->Senibility);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("BuzzEnable")) != NULL)
        {
            if(strcmp("BuzzEnable",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->BuzzEnable = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->BuzzEnable:%d\n",(deviceparam+m)->BuzzEnable);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("DetectInterval")) != NULL)
        {
            if(strcmp("DetectInterval",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->DetectInterval = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->DetectInterval:%d\n",(deviceparam+m)->DetectInterval);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkSnap")) != NULL)
        {
            if(strcmp("LinkSnap",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->LinkSnap = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkSnap:%d\n",(deviceparam+m)->LinkSnap);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkRecord")) != NULL)
        {
            if(strcmp("LinkRecord",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->LinkRecord = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkRecord:%d\n",(deviceparam+m)->LinkRecord);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkOutput")) != NULL)
        {
            if(strcmp("LinkOutput",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->LinkOutput = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkOutput:%d\n",(deviceparam+m)->LinkOutput);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkRecDuration")) != NULL)
        {
            if(strcmp("LinkRecDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->LinkRecDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkRecDuration:%d\n",(deviceparam+m)->LinkRecDuration);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkBuzzDuration")) != NULL)
        {
            if(strcmp("LinkBuzzDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->LinkBuzzDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->LinkBuzzDuration:%d\n",(deviceparam+m)->LinkBuzzDuration);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlarmOutDuration")) != NULL)
        {
            if(strcmp("AlarmOutDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->AlarmOutDuration = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->AlarmOutDuration:%d\n",(deviceparam+m)->AlarmOutDuration);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("MdAreaNums")) != NULL)
        {
            if(strcmp("MdAreaNums",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceparam+m)->DetectAreaNum = atoi(surface->GetText());
                    XML_DEBUG("deviceparam->DetectAreaNum:%d\n",(deviceparam+m)->DetectAreaNum);
                }
            }
        }

        for(j=0; j<(deviceparam+m)->DetectAreaNum; j++)
        {
            pAarmarea = &(deviceparam+m)->stMdArea[j];
            memset(alarmxmldata,0,sizeof(alarmxmldata));
            sprintf(alarmxmldata,"Area%d",j);
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement(alarmxmldata)) != NULL)
            {
                if(strcmp(alarmxmldata,surface->Name()) == 0)
                {
                    if(surface->GetText() != NULL)
                    {
                        strcpy(alarmtime,surface->GetText());
                        XML_DEBUG("alarmtime:%s\n",alarmtime);
                    }
                }
            }

            sscanf(alarmtime,"%3d,%3d,%3d,%3d",&leftx,&lefty,&rightx,&righty);

            pAarmarea->TopLeft.x = leftx;
            XML_DEBUG("pAarmarea->TopLeft.x:%d\n",pAarmarea->TopLeft.x);
            pAarmarea->TopLeft.y = lefty;
            XML_DEBUG("pAarmarea->TopLeft.y:%d\n",pAarmarea->TopLeft.y);
            pAarmarea->BottomRight.x = rightx;
            XML_DEBUG("pAarmarea->BottomRight.x:%d\n",pAarmarea->BottomRight.x);
            pAarmarea->BottomRight.y = righty;
            XML_DEBUG("pAarmarea->BottomRight.y:%d\n",pAarmarea->BottomRight.y);


        }

    }

    return 0;
}

/*设置移动侦测信息*/
int SetXmlMDParam(int chnnum,WEBOCX_MD_SET_ST *deviceparam)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    char alarmxmldata[128] = {0};
    char enable[128] = {0};
    int m = 0;
    int i = 0;
    int j = 0;


    for(m=0; m<chnnum; m++)
    {
        XML_DEBUG("deviceparam->ChnNum:%d\n",(deviceparam+m)->ChnId);
        if((deviceparam+m)->ChnId == m)
        {
            /*布防时间段*/
            for(i=0; i<7; i++)
            {
                for(j=0; j<4; j++)
                {
                    memset(alarmxmldata,0,sizeof(alarmxmldata));
                    sprintf(alarmxmldata,"Data%dTime%d",i,j);
                    sprintf(enable,"%2d:%2d:%2d,%2d:%2d:%2d",(deviceparam+m)->AlertTime[i][j].AlertStartHour,
                            (deviceparam+m)->AlertTime[i][j].AlertStartMinu,(deviceparam+m)->AlertTime[i][j].AlertStartSecond,
                            (deviceparam+m)->AlertTime[i][j].AlertEndHour,(deviceparam+m)->AlertTime[i][j].AlertEndMinu,
                            (deviceparam+m)->AlertTime[i][j].AlertEndSecond);
                    XML_DEBUG("enable:%s,i:%d,j:%d\n",enable,i,j);
                    if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement(alarmxmldata)) != NULL)
                    {
                        if(strcmp(alarmxmldata,surface->Name()) == 0)
                        {
                            if(surface->Name()!= NULL)
                            {
                                surface->SetText(enable);
                                XML_DEBUG("alarmtime:%s\n",enable);
                            }
                        }
                    }

                }

            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlertEnable")) != NULL)
            {
                if(strcmp("AlertEnable",surface->Name()) == 0)
                {
                    if((0 == (deviceparam+m)->AlertEnable) || ((deviceparam+m)->AlertEnable == 1))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->AlertEnable);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->AlertEnable:%d\n",(deviceparam+m)->AlertEnable);
                    }
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlarmEnable")) != NULL)
            {
                if(strcmp("AlarmEnable",surface->Name()) == 0)
                {
                    if((0 == (deviceparam+m)->AlarmEnable) || ((deviceparam+m)->AlarmEnable == 1))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->AlertEnable);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->AlarmEnable:%d\n",(deviceparam+m)->AlarmEnable);
                    }
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("Senibility")) != NULL)
            {
                if(strcmp("Senibility",surface->Name()) == 0)
                {
                    if(((deviceparam+m)->Senibility >= 0) && ((deviceparam+m)->Senibility < 10))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->Senibility);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->Senibility:%d\n",(deviceparam+m)->Senibility);
                    }
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("BuzzEnable")) != NULL)
            {
                if(strcmp("BuzzEnable",surface->Name()) == 0)
                {
                    if((0 == (deviceparam+m)->BuzzEnable) || ((deviceparam+m)->BuzzEnable == 1 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->BuzzEnable);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->BuzzEnable:%d\n",(deviceparam+m)->BuzzEnable);
                    }
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("DetectInterval")) != NULL)
            {
                if(strcmp("DetectInterval",surface->Name()) == 0)
                {
                    if((0 <= (deviceparam+m)->DetectInterval) && ((deviceparam+m)->DetectInterval <= 255 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->DetectInterval);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->DetectInterval:%d\n",(deviceparam+m)->DetectInterval);
                    }
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkSnap")) != NULL)
            {
                if(strcmp("LinkSnap",surface->Name()) == 0)
                {
                    sprintf(enable,"%d",(deviceparam+m)->LinkSnap);
                    surface->SetText(enable);
                    XML_DEBUG("deviceparam->LinkSnap:%d\n",(deviceparam+m)->LinkSnap);
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkRecord")) != NULL)
            {
                if(strcmp("LinkRecord",surface->Name()) == 0)
                {
                    sprintf(enable,"%d",(deviceparam+m)->LinkRecord);
                    surface->SetText(enable);
                    XML_DEBUG("deviceparam->LinkRecord:%d\n",(deviceparam+m)->LinkRecord);
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkOutput")) != NULL)
            {
                if(strcmp("LinkOutput",surface->Name()) == 0)
                {
                    sprintf(enable,"%d",(deviceparam+m)->LinkOutput);
                    surface->SetText(enable);
                    XML_DEBUG("deviceparam->LinkOutput:%d\n",(deviceparam+m)->LinkOutput);
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkRecDuration")) != NULL)
            {
                if(strcmp("LinkRecDuration",surface->Name()) == 0)
                {
                    if((0 <= (deviceparam+m)->LinkRecDuration) && ((deviceparam+m)->LinkRecDuration <= 255 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->LinkRecDuration);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->LinkRecDuration:%d\n",(deviceparam+m)->LinkRecDuration);
                    }
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("LinkBuzzDuration")) != NULL)
            {
                if(strcmp("LinkBuzzDuration",surface->Name()) == 0)
                {
                    if((0 <= (deviceparam+m)->LinkBuzzDuration) && ((deviceparam+m)->LinkBuzzDuration <= 255 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->LinkBuzzDuration);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->LinkBuzzDuration:%d\n",(deviceparam+m)->LinkBuzzDuration);
                    }
                }
            }
            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("AlarmOutDuration")) != NULL)
            {
                if(strcmp("AlarmOutDuration",surface->Name()) == 0)
                {
                    if((0 <= (deviceparam+m)->AlarmOutDuration) && ((deviceparam+m)->AlarmOutDuration <= 255 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->AlarmOutDuration);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->AlarmOutDuration:%d\n",(deviceparam+m)->AlarmOutDuration);
                    }
                }
            }

            if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement("MdAreaNums")) != NULL)
            {
                if(strcmp("MdAreaNums",surface->Name()) == 0)
                {
                    if((0 <= (deviceparam+m)->DetectAreaNum) && ((deviceparam+m)->DetectAreaNum <= 4 ))
                    {
                        sprintf(enable,"%d",(deviceparam+m)->DetectAreaNum);
                        surface->SetText(enable);
                        XML_DEBUG("deviceparam->DetectAreaNum:%d\n",(deviceparam+m)->DetectAreaNum);
                    }
                }
            }

            for(j=0; j<(deviceparam+m)->DetectAreaNum; j++)
            {
                memset(alarmxmldata,0,sizeof(alarmxmldata));
                sprintf(alarmxmldata,"Area%d",j);
                sprintf(enable,"%3d,%3d,%3d,%3d",(deviceparam+m)->stMdArea[j].TopLeft.x,
                        (deviceparam+m)->stMdArea[j].TopLeft.y,(deviceparam+m)->stMdArea[j].BottomRight.x,
                        (deviceparam+m)->stMdArea[j].BottomRight.y);
                XML_DEBUG("enable:%s,j:%d\n",enable,j);
                if((surface = scene->FirstChildElement(cameranumstr[m])->FirstChildElement(alarmxmldata)) != NULL)
                {
                    if(strcmp(alarmxmldata,surface->Name()) == 0)
                    {
                        if(enable != NULL)
                        {
                            surface->SetText(enable);
                            XML_DEBUG("(deviceparam+i)->stMdArea[i][j]:%s\n",enable);
                        }
                    }
                }

            }


        }

    }


    myDocument.SaveFile(XML_NAME);

    return 0;
}



int GetXmlcamerid(device_camer_id *camerid)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    int i = 0;
    XMLElement *surface;

    for(i=0; i<4; i++)
    {
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Id")) != NULL)
        {
            if(strcmp("Id",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy(camerid[i].CameraSIPID,surface->GetText());
                    XML_DEBUG("camerid[i]->CameraSIPID:%s\n",camerid[i].CameraSIPID);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraAdd")) != NULL)
        {
            if(strcmp("CameraAdd",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (camerid+i)->CamerAddSIP = atoi(surface->GetText());
                    XML_DEBUG("camerid->CamerAddSIP:%d\n",(camerid+i)->CamerAddSIP);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("CameraPD")) != NULL)
        {
            if(strcmp("CameraPD",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (camerid+i)->CameraPDSIP = atoi(surface->GetText());
                    XML_DEBUG("camerid->CameraPDSIP:%d\n",(camerid+i)->CameraPDSIP);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("LinkBuzzDuration")) != NULL)
        {
            if(strcmp("LinkBuzzDuration",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (camerid+i)->CamerBuzzDuration = atoi(surface->GetText());
                    XML_DEBUG("camerid->CamerBuzzDuration:%d\n",(camerid+i)->CamerBuzzDuration);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Name")) != NULL)
        {
            if(strcmp("Name",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy(camerid[i].CamerName,surface->GetText());
                    XML_DEBUG("camerid[i]->CamerName:%s\n",camerid[i].CamerName);
                }
            }
        }

    }


    return 0;
}
#ifdef PLT_PARA_BK
int GetXmlPlatformRegInfo2(nvs::PlatformRegInfo *devicereginfo)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int reti = 3600;

    if((surface = scene->FirstChildElement("UserName")) != NULL)
    {
        XML_DEBUG("UserName:%s\n",surface->Name());
        if(strcmp("UserName",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->userName,surface->GetText());
                XML_DEBUG("devicereginfo->userName:%s\n",devicereginfo->userName);
            }
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->password,surface->GetText());
                XML_DEBUG("devicereginfo->password:%s\n",devicereginfo->password);
            }
        }
    }

    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        XML_DEBUG("Expires:%s\n",surface->Name());
        if(strcmp("Expires",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                reti = atoi(surface->GetText());
                devicereginfo->expires = reti;
                XML_DEBUG("devicereginfo->expires:%d\n",devicereginfo->expires);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->nvsAddr.id,surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.id:%s\n",devicereginfo->nvsAddr.id);
            }
        }
    }
    if((surface = scene->FirstChildElement("IpAddr")) != NULL)
    {
        XML_DEBUG("IpAddr:%s\n",surface->Name());
        if(strcmp("IpAddr",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->nvsAddr.ip,surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.ip:%s\n",devicereginfo->nvsAddr.ip);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                devicereginfo->nvsAddr.port = atoi(surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.port:%d\n",devicereginfo->nvsAddr.port);
            }
        }
    }


    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->serverAddr.id,surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.id:%s\n",devicereginfo->serverAddr.id);
            }
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("IpSub")) != NULL)
    {
        XML_DEBUG("IpSub:%s\n",surface->Name());
        if(strcmp("IpSub",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->serverAddr.ip,surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.ipSub:%s\n",devicereginfo->serverAddr.ip);
            }
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("PortSub")) != NULL)
    {
        XML_DEBUG("PortSub:%s\n",surface->Name());
        if(strcmp("PortSub",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                devicereginfo->serverAddr.port = atoi(surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.portSub:%d\n",devicereginfo->serverAddr.port);
            }
        }
    }

    return 0;
}
#endif
int GetXmlPlatformRegInfo(nvs::PlatformRegInfo *devicereginfo)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int reti = 3600;

    if((surface = scene->FirstChildElement("UserName")) != NULL)
    {
        XML_DEBUG("UserName:%s\n",surface->Name());
        if(strcmp("UserName",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->userName,surface->GetText());
                XML_DEBUG("devicereginfo->userName:%s\n",devicereginfo->userName);
            }
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->password,surface->GetText());
                XML_DEBUG("devicereginfo->password:%s\n",devicereginfo->password);
            }
        }
    }

    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        XML_DEBUG("Expires:%s\n",surface->Name());
        if(strcmp("Expires",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                reti = atoi(surface->GetText());
                devicereginfo->expires = reti;
                XML_DEBUG("devicereginfo->expires:%d\n",devicereginfo->expires);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->nvsAddr.id,surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.id:%s\n",devicereginfo->nvsAddr.id);
            }
        }
    }
    if((surface = scene->FirstChildElement("IpAddr")) != NULL)
    {
        XML_DEBUG("IpAddr:%s\n",surface->Name());
        if(strcmp("IpAddr",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->nvsAddr.ip,surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.ip:%s\n",devicereginfo->nvsAddr.ip);
            }
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                devicereginfo->nvsAddr.port = atoi(surface->GetText());
                XML_DEBUG("devicereginfo->nvsAddr.port:%d\n",devicereginfo->nvsAddr.port);
            }
        }
    }


    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->serverAddr.id,surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.id:%s\n",devicereginfo->serverAddr.id);
            }
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Ip")) != NULL)
    {
        XML_DEBUG("Ip:%s\n",surface->Name());
        if(strcmp("Ip",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicereginfo->serverAddr.ip,surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.ip:%s\n",devicereginfo->serverAddr.ip);
            }
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                devicereginfo->serverAddr.port = atoi(surface->GetText());
                XML_DEBUG("devicereginfo->serverAddr.port:%d\n",devicereginfo->serverAddr.port);
            }
        }
    }

    return 0;
}

#ifdef PLT_PARA_BK
int SetXmlPlatformRegInfo2(nvs::PlatformRegInfo *devicereginfo)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    char text[64] = {0};

    if((surface = scene->FirstChildElement("UserName")) != NULL)
    {
        XML_DEBUG("UserName:%s\n",surface->Name());
        if(strcmp("UserName",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->userName);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->userName);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        XML_DEBUG("Expires:%s\n",surface->Name());
        if(strcmp("Expires",surface->Name()) == 0)
        {
            sprintf(text,"%d",devicereginfo->expires);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->nvsAddr.id);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->serverAddr.id);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("IpSub")) != NULL)
    {
        XML_DEBUG("IpSub:%s \n",surface->Name());
        if(strcmp("IpSub",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->serverAddr.ip);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("PortSub")) != NULL)
    {
        XML_DEBUG("PortSub:%s\n",surface->Name());
        if(strcmp("PortSub",surface->Name()) == 0)
        {
            sprintf(text,"%d",devicereginfo->serverAddr.port);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    myDocument.SaveFile(XML_NAME);
    return 0;
}
#endif
int SetXmlPlatformRegInfo(nvs::PlatformRegInfo *devicereginfo)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    char text[64] = {0};

    if((surface = scene->FirstChildElement("UserName")) != NULL)
    {
        XML_DEBUG("UserName:%s\n",surface->Name());
        if(strcmp("UserName",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->userName);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    if((surface = scene->FirstChildElement("PassWord")) != NULL)
    {
        XML_DEBUG("PassWord:%s\n",surface->Name());
        if(strcmp("PassWord",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->userName);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("Expires")) != NULL)
    {
        XML_DEBUG("Expires:%s\n",surface->Name());
        if(strcmp("Expires",surface->Name()) == 0)
        {
            sprintf(text,"%d",devicereginfo->expires);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("NvsAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->nvsAddr.id);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Id")) != NULL)
    {
        XML_DEBUG("Id:%s\n",surface->Name());
        if(strcmp("Id",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->serverAddr.id);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Ip")) != NULL)
    {
        XML_DEBUG("Ip:%s \n",surface->Name());
        if(strcmp("Ip",surface->Name()) == 0)
        {
            sprintf(text,"%s",devicereginfo->serverAddr.ip);
            surface->SetText(text);
            bzero(text,64);
        }
    }

    if((surface = scene->FirstChildElement("ServerAddr")->FirstChildElement("Port")) != NULL)
    {
        XML_DEBUG("Port:%s\n",surface->Name());
        if(strcmp("Port",surface->Name()) == 0)
        {
            sprintf(text,"%d",devicereginfo->serverAddr.port);
            surface->SetText(text);
            bzero(text,64);
        }
    }
    myDocument.SaveFile(XML_NAME);

    return 0;
}


int	GetXmlPlatformRegInfo(int num,nvs::ResNode *deviceresreport)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;
    int i = 0;

    for(i=0; i<num; i++)
    {
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Id")) != NULL)
        {
            if(strcmp("Id",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceresreport+i)->id,surface->GetText());
                    XML_DEBUG("(deviceresreport+i)->id:%s\n",(deviceresreport+i)->id);
                }
            }
        }
        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Name")) != NULL)
        {
            if(strcmp("Name",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceresreport+i)->name,surface->GetText());
                    XML_DEBUG("(deviceresreport+i)->name:%s\n",(deviceresreport+i)->name);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Address")) != NULL)
        {
            if(strcmp("Address",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    strcpy((deviceresreport+i)->address,surface->GetText());
                    XML_DEBUG("(deviceresreport+i)->address:%s\n",(deviceresreport+i)->address);
                }
            }
        }

        if((surface = scene->FirstChildElement(cameranumstr[i])->FirstChildElement("Codec")) != NULL)
        {
            if(strcmp("Codec",surface->Name()) == 0)
            {
                if(surface->GetText() != NULL)
                {
                    (deviceresreport+i)->codec = atoi(surface->GetText());
                    XML_DEBUG("(deviceresreport+i)->codec:%d\n",(deviceresreport+i)->codec);
                }
            }
        }

    }

    return 0;
}

int GetXmlDeviceStatus(nvs::DeviceStatusParam *devicestatus)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    if((surface = scene->FirstChildElement("OnLine")) != NULL)
    {
        XML_DEBUG("OnLine:%s\n",surface->Name());
        if(strcmp("OnLine",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicestatus->onLine,surface->GetText());
                XML_DEBUG("devicestatus->onLine:%s\n",devicestatus->onLine);
            }
        }
    }

    if((surface = scene->FirstChildElement("Status")) != NULL)
    {
        XML_DEBUG("Status:%s\n",surface->Name());
        if(strcmp("Status",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicestatus->status,surface->GetText());
                XML_DEBUG("devicestatus->status:%s\n",devicestatus->status);
            }
        }
    }
    if((surface = scene->FirstChildElement("Encode")) != NULL)
    {
        XML_DEBUG("Encode:%s\n",surface->Name());
        if(strcmp("Encode",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicestatus->encode,surface->GetText());
                XML_DEBUG("devicestatus->encode:%s\n",devicestatus->encode);
            }
        }
    }
    if((surface = scene->FirstChildElement("Record")) != NULL)
    {
        XML_DEBUG("Record:%s\n",surface->Name());
        if(strcmp("Record",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicestatus->record,surface->GetText());
                XML_DEBUG("devicestatus->record:%s\n",devicestatus->record);
            }
        }
    }

    if((surface = scene->FirstChildElement("AlarmStatus")) != NULL)
    {
        XML_DEBUG("AlarmStatus:%s\n",surface->Name());
        if(strcmp("AlarmStatus",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(devicestatus->alarmstatus,surface->GetText());
                XML_DEBUG("devicestatus->alarmstatus:%s\n",devicestatus->alarmstatus);
            }
        }
    }

    return 0;
}


int	GetXmlDeviceInfo(nvs::DeviceInfoParam *deviceinfo)
{
    XMLElement *scene = RootElementXml();
    if(scene == NULL)
    {
        return -1;
    }
    XMLElement *surface;

    if((surface = scene->FirstChildElement("DeviceType")) != NULL)
    {
        XML_DEBUG("DeviceType:%s\n",surface->Name());
        if(strcmp("DeviceType",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->deviceType,surface->GetText());
                XML_DEBUG("deviceinfo->deviceType:%s\n",deviceinfo->deviceType);
            }
        }
    }

    if((surface = scene->FirstChildElement("ManuFacturer")) != NULL)
    {
        XML_DEBUG("ManuFacturer:%s\n",surface->Name());
        if(strcmp("ManuFacturer",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->manufacturer,surface->GetText());
                XML_DEBUG("deviceinfo->manufacturer:%s\n",deviceinfo->manufacturer);
            }
        }
    }

    if((surface = scene->FirstChildElement("Model")) != NULL)
    {
        XML_DEBUG("Model:%s\n",surface->Name());
        if(strcmp("Model",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->model,surface->GetText());
                XML_DEBUG("deviceinfo->model:%s\n",deviceinfo->model);
            }
        }
    }

    if((surface = scene->FirstChildElement("Firmware")) != NULL)
    {
        XML_DEBUG("Firmware:%s\n",surface->Name());
        if(strcmp("Firmware",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->firmware,surface->GetText());
                XML_DEBUG("deviceinfo->firmware:%s\n",deviceinfo->firmware);
            }
        }
    }

    if((surface = scene->FirstChildElement("MaxCamera")) != NULL)
    {
        XML_DEBUG("MaxCamera:%s\n",surface->Name());
        if(strcmp("MaxCamera",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->maxCamera,surface->GetText());
                XML_DEBUG("deviceinfo->maxCamera:%s\n",deviceinfo->maxCamera);
            }
        }
    }

    if((surface = scene->FirstChildElement("MaxAlarm")) != NULL)
    {
        XML_DEBUG("MaxAlarm:%s\n",surface->Name());
        if(strcmp("MaxAlarm",surface->Name()) == 0)
        {
            if(surface->GetText() != NULL)
            {
                strcpy(deviceinfo->maxAlarm,surface->GetText());
                XML_DEBUG("deviceinfo->maxAlarm:%s\n",deviceinfo->maxAlarm);
            }
        }
    }

    return 0;
}


