#include <BaseClass.h>

/*****************************************************************************
 函 数 名  : FunctionBaseClass.FunctionBaseClass
 功能描述  : 构造函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
FunctionBaseClass::FunctionBaseClass()
{
	FunctionStat = false;   /*初始化为未启动*/
	FunctionSwitchFlag = 1; /*模块功能开关-初始化为打开*/
}
/*****************************************************************************
 函 数 名  : FunctionBaseClass.~FunctionBaseClass
 功能描述  : 析构函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
FunctionBaseClass::~FunctionBaseClass()
{

}
/*****************************************************************************
 函 数 名  : FunctionBaseClass.GetFunctionStat
 功能描述  : 获取模块工作状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
bool FunctionBaseClass::getFunctionStat()
{
	return FunctionStat;
}

/*****************************************************************************
 函 数 名  : FunctionBaseClass.StartFunction
 功能描述  : 启动模块-设置状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
int FunctionBaseClass::StartFunction()
{
	FunctionStat = true;
	return 0;
}

/*****************************************************************************
 函 数 名  : FunctionBaseClass.StopFunction
 功能描述  : 结束模块状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
int FunctionBaseClass::StopFunction()
{
	FunctionStat = false;
	return 0;
}
/*****************************************************************************
 函 数 名  : FunctionBaseClass.GetFunctionSwitchFlag
 功能描述  : 获取功能开关
 输入参数  : int &iFlag
 输出参数  : 无
 返 回 值  : int

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
int FunctionBaseClass::GetFunctionSwitchFlag(int &iFlag)
{
    if (iFlag)
	  iFlag = FunctionSwitchFlag;
	return FunctionSwitchFlag;
}
/*****************************************************************************
 函 数 名  : FunctionBaseClass.SetFunctionSwitchFlag
 功能描述  : 设置功能开关
 输入参数  : int iFlag
 输出参数  : 无
 返 回 值  : int

 修改历史      :
  1.日    期   : 2012年12月8日 星期六
    作    者   : wuquanwei
    修改内容   : 新生成函数

*****************************************************************************/
int FunctionBaseClass::SetFunctionSwitchFlag(int iFlag)
{
	FunctionSwitchFlag = iFlag;
	return 0;
}




























