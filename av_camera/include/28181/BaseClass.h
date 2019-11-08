/******************************************************************************

                  版权所有 (C), 2001-2011, 郑州天迈科技有限公司

 ******************************************************************************
  文 件 名   : BaseClass.h
  版 本 号   : 初稿
  作    者   : wuquanwei
  生成日期   : 2012年12月1日 星期六
  最近修改   :
  功能描述   : BaseClass.cpp 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年12月1日 星期六
    作    者   : wuquanwei
    修改内容   : 创建文件

******************************************************************************/

#ifndef __BASECLASS_H__
#define __BASECLASS_H__

#include <iostream>
#include <string.h>


#define OPEN_FUN_SWITCH     1 /*开启该功能模块*/
#define CLOSE_FUN_SWITCH    2 /*关闭该功能模块*/

using namespace std;

class FunctionBaseClass
{

private:

    bool FunctionStat;      /*功能状态-true--已经启动; false-未启动；*/
    int FunctionSwitchFlag;/*功能开关标志:1-开启；2-关闭-从配置文件中读取*/
    //struct av_config_args Para_p;       /*参数-从配置文件中获取*/


public:

    FunctionBaseClass();
    virtual ~FunctionBaseClass();

    bool getFunctionStat();/*获取当前的状态*/
    virtual int StartFunction();
    virtual int StopFunction();
    int GetFunctionSwitchFlag(int &iFlag);
    int SetFunctionSwitchFlag(int iFlag);
    virtual int getModulePara(char *Para){return 0;};
    virtual int setModulePara(char *Para){return 0;};
};

























#endif /* __BASECLASS_H__ */
