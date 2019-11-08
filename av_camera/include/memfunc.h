#ifndef _MEM_FUNC_H_
#define _MEM_FUNC_H_
#include "memcommon.h"
//data package
HDP mem_CreatePackage(int iSize,int _iPriType = PKG_PRIORITY_NORMAL);

char * mem_GetPackageData(HDP hDP);

int mem_GetPackageDataLen(HDP hDP);

void mem_ReleasePackage(HDP hPkg);

void mem_ReferPackage(HDP hPkg);

void mem_PrintMemPool();

//data center
int mem_DataCenterEmpty(HBC hDC);

HDC mem_CreateDataCenter();

void mem_Bind(HBC hBC,HDC hDC);

void mem_UnBind(HBC hBC,HDC hDC);

void mem_PushPackage(HDP hPkg,HDC hDC);
//data container
HBC mem_CreateContainer(int iType,int iPri,int iSize);

void mem_Enable(HBC hBC);

void mem_DisEnable(HBC hBC);

void mem_Clear(HBC hBC);

void mem_Destory(HBC hBC);

int mem_GetPackageNum(HBC hBC);

void mem_ChangeContainerType(HBC hBC,int iPri);

HDP mem_PopPackageData(HBC hBC);

HDP mem_PopPackageDataTmp(HBC hBC);

int mem_ReleaseTopPackage(HBC hBC);
#endif
