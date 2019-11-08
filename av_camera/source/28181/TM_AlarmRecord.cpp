#include "TM_AlarmRecord.h"


using namespace std;
using namespace tinyxml2;




XMLDocument AlarmDocument;


XMLElement *RootElementAlarmXml()
{
	if(AlarmDocument.LoadFile(ALARM_XML_NAME) != XML_NO_ERROR)
	{
		return NULL;
	}
	//得到跟节点
	XMLElement *alarm_scene = AlarmDocument.RootElement();
	if(alarm_scene == NULL)
	{
		printf("alarm_scene == NULL\n");
		return NULL;
	}
	cout<< alarm_scene->Name() << ":" << alarm_scene->Value() << endl;

	return alarm_scene;
}
int NewAlarmXmlText(Alarm_record_param *alarmparam)
{
	XMLElement *scene = RootElementAlarmXml();

	char buffer[128] = {0};

	if(scene == NULL)
	{
		printf("NewAlarmXmlText scene is NULL\n");
		return -1;
	}

	XMLElement *FirstData = scene->FirstChildElement();

	while(FirstData != NULL)
	{
		printf("FirstData->FirstChildElement() NOT NULL\n");
		printf("FirstData->name:%s\n",FirstData->Name());

		FirstData = FirstData->NextSiblingElement();
	}

	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"chn%d",alarmparam->chnnum);

	XMLElement *sub = AlarmDocument.NewElement(buffer);
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",alarmparam->alarmbegntime);
	printf("alarmparam->alarmbegntime buffer:%s\n",buffer);
	sub->SetAttribute("begintime",buffer);
	printf("sub->Name():sub->Value()%s,%s\n",sub->Name(),sub->Value());

	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",alarmparam->alarmendtime);
	printf("alarmparam->alarmendtime buffer:%s\n",buffer);
	sub->SetAttribute("endtime",buffer);
/*
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",alarmparam->alarmbegntime+MAGICNUM);
	printf("recordid buffer:%s\n",buffer);
	sub->SetAttribute("recordid",buffer);
*/
	printf("scene->LinkEndChild(sub):%d\n",__LINE__);
	scene->LinkEndChild(sub);
	printf("scene->LinkEndChild(sub):%d\n",__LINE__);


	AlarmDocument.SaveFile(ALARM_XML_NAME);
	printf("AlarmDocument.SaveFile:%d\n",__LINE__);

	return 0;
}


int GetAlarmRecXmlText(Alarm_record_param *alarmparam,nvs::QueryRecordResponse& resp)
{
	XMLElement *scene = RootElementAlarmXml();

	if(scene == NULL)
	{
		printf("GetAlarmXmlText scene is NULL\n");
		return -1;
	}
	nvs::RecordItem  Items;
	XMLElement *FirstData = scene->FirstChildElement();

	printf("Find CHN %d\n",alarmparam->chnnum);
	int count = 0;
	while(FirstData != NULL)
	{
		printf("FirstData->name:%s!!! \n",FirstData->Name());

		if(FirstData->Name()==NULL)
		{
			FirstData = FirstData->NextSiblingElement();
			continue;
		}
		if(alarmparam->chnnum == atoi(FirstData->Name()+3))
		{
			if(FirstData->FindAttribute("begintime")->Value()!=NULL)
			{
				Items.beginTime=atoi(FirstData->FindAttribute("begintime")->Value());
			}
			else
			{
				FirstData = FirstData->NextSiblingElement();
				continue;
			}

			if(FirstData->FindAttribute("endtime")->Value()!=NULL)
			{
				Items.endTime=atoi(FirstData->FindAttribute("endtime")->Value());
			}
			else
			{
				FirstData = FirstData->NextSiblingElement();
				continue;
			}

			if((alarmparam->alarmendtime < Items.beginTime)||\
				( alarmparam->alarmbegntime > Items.endTime))
			{

			}
			else
			{
				printf("FirstData->begintime:%s\n",FirstData->FindAttribute("begintime")->Value());
				printf("FirstData->endtime:%s\n",FirstData->FindAttribute("endtime")->Value());
				sprintf(Items.recordID,"%d%ld",alarmparam->chnnum,Items.beginTime+RECIDMAGICNUM);
				Items.secrecy = alarmparam->alarmflag ;
				printf("FirstData->recordID:%s\n",Items.recordID);
				memcpy(Items.type,"alarm",sizeof(Items.type));
				memcpy(&resp.pItems[count],&Items,sizeof(nvs::RecordItem));
				count++;
				if(count>=resp.sum)
				{
					break;
				}
			}
		}
		FirstData = FirstData->NextSiblingElement();
	}
	resp.sum = count;
	return 0;
}



