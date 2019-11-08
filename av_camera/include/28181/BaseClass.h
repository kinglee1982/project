/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ֣�������Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : BaseClass.h
  �� �� ��   : ����
  ��    ��   : wuquanwei
  ��������   : 2012��12��1�� ������
  ����޸�   :
  ��������   : BaseClass.cpp ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��12��1�� ������
    ��    ��   : wuquanwei
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __BASECLASS_H__
#define __BASECLASS_H__

#include <iostream>
#include <string.h>


#define OPEN_FUN_SWITCH     1 /*�����ù���ģ��*/
#define CLOSE_FUN_SWITCH    2 /*�رոù���ģ��*/

using namespace std;

class FunctionBaseClass
{

private:

    bool FunctionStat;      /*����״̬-true--�Ѿ�����; false-δ������*/
    int FunctionSwitchFlag;/*���ܿ��ر�־:1-������2-�ر�-�������ļ��ж�ȡ*/
    //struct av_config_args Para_p;       /*����-�������ļ��л�ȡ*/


public:

    FunctionBaseClass();
    virtual ~FunctionBaseClass();

    bool getFunctionStat();/*��ȡ��ǰ��״̬*/
    virtual int StartFunction();
    virtual int StopFunction();
    int GetFunctionSwitchFlag(int &iFlag);
    int SetFunctionSwitchFlag(int iFlag);
    virtual int getModulePara(char *Para){return 0;};
    virtual int setModulePara(char *Para){return 0;};
};

























#endif /* __BASECLASS_H__ */
