/******************************************************************************
*
*  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.
*
*******************************************************************************
*  File Name     : hi_comm_region.h
*  Version       : Initial Draft
*  Author        : j00169368
*  Created       : 2010/12/13
*  Last Modified :
*  Description   : include RGN struct , Marco and Error information
*  Function List :
*
*
*  History:
* 
*       1.  Date         : 2010/12/13
*           Author       : j00169368
*           Modification : Created file
*
******************************************************************************/


#ifndef __HI_COMM_REGION_H__
#define __HI_COMM_REGION_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_errno.h"
#include "hi_defines.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


typedef HI_U32 RGN_HANDLE;

/* type of video regions */
typedef enum hiRGN_TYPE_E
{
    OVERLAY_RGN = 0,     /* video overlay region */
    COVER_RGN,
    COVEREX_RGN,
    OVERLAYEX_RGN,
    LINE_RGN,
    MOSAIC_RGN,
    RGN_BUTT
} RGN_TYPE_E;

typedef enum hiINVERT_COLOR_MODE_E
{
    LESSTHAN_LUM_THRESH = 0,   /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */ 
    MORETHAN_LUM_THRESH,       /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
    INVERT_COLOR_BUTT
}INVERT_COLOR_MODE_E;

typedef struct hiOVERLAY_QP_INFO_S
{
	HI_BOOL	 bQpDisable;
    HI_BOOL  bAbsQp;
    HI_S32   s32Qp;
}OVERLAY_QP_INFO_S;

typedef struct hiOVERLAY_INVERT_COLOR_S
{
    SIZE_S              stInvColArea;                //it must be multipe of 16 but not more than 64.
    HI_U32              u32LumThresh;                //the threshold to decide whether invert the OSD's color or not.
    INVERT_COLOR_MODE_E enChgMod;      
    HI_BOOL             bInvColEn;                   //the switch of inverting color.
}OVERLAY_INVERT_COLOR_S;

typedef struct hiOVERLAY_ATTR_S
{
    /* bitmap pixel format*/
    PIXEL_FORMAT_E enPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    HI_U32 u32BgColor;

    /* region size */
    SIZE_S stSize;
}OVERLAY_ATTR_S;

typedef struct hiOVERLAY_CHN_ATTR_S
{
    /* start point */
    POINT_S stPoint;
    
    /* foreground transparence */

    HI_U32 u32FgAlpha;

	/* background transparence */
    HI_U32 u32BgAlpha;

	/* OVERLAY region layer*/
    HI_U32 u32Layer;   

	/* QP infomation when venc*/
    OVERLAY_QP_INFO_S stQpInfo;

	/* invertColor infomation*/
    OVERLAY_INVERT_COLOR_S stInvertColor;
}OVERLAY_CHN_ATTR_S;

typedef enum hiRGN_AREA_TYPE_E
{
    AREA_RECT = 0,
    AREA_QUAD_RANGLE,
    AREA_BUTT
} RGN_AREA_TYPE_E;

typedef struct hiRGN_QUADRANGLE_S
{
    HI_BOOL bSolid;            /* whether solid or dashed framework */
    HI_U32 u32LineWidth;       /* Line Width of framework, valid when dashed framework */
    POINT_S stPoint[4];        /* points of quadrilateral*/
} RGN_QUADRANGLE_S;

typedef struct hiCOVER_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;        /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        	/* config of rect*/
        RGN_QUADRANGLE_S    stQuadRangle;  	/* config of arbitary quadrilateral COVER */
    };
    HI_U32 u32Color;
    HI_U32 u32Layer;   						/* COVER region layer range:[0,3] */
}COVER_CHN_ATTR_S;

typedef struct hiCOVEREX_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;       /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        /* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;  /* config of arbitary quadrilateral COVER */
    };
    HI_U32 u32Color;
    HI_U32 u32Layer;   /* COVEREX region layer range:[0,7] */
}COVEREX_CHN_ATTR_S;

typedef enum hiMOSAIC_BLK_SIZE_E
{
    MOSAIC_BLK_SIZE_8 = 0,    /*block size 8*8 of MOSAIC*/
    MOSAIC_BLK_SIZE_16,       /*block size 16*16 of MOSAIC*/
    MOSAIC_BLK_SIZE_32,       /*block size 32*32 of MOSAIC*/
    MOSAIC_BLK_SIZE_64,       /*block size 64*64 of MOSAIC*/
    MOSAIC_BLK_SIZE_BUTT
} MOSAIC_BLK_SIZE_E;

typedef struct hiMOSAIC_CHN_ATTR_S
{
    RECT_S stRect;                 /*location of MOSAIC*/
    MOSAIC_BLK_SIZE_E enBlkSize;   /*block size of MOSAIC*/
    HI_U32 u32Layer;               /*MOSAIC region layer range:[0,3] */
}MOSAIC_CHN_ATTR_S;

typedef struct hiOVERLAYEX_ATTR_S
{
    PIXEL_FORMAT_E enPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    HI_U32 u32BgColor;

    /* region size */
    SIZE_S stSize;
}OVERLAYEX_ATTR_S;

typedef struct hiOVERLAYEX_CHN_ATTR_S
{
    /* start point */
    POINT_S stPoint;
    
    /* foreground transparence */
    HI_U32 u32FgAlpha;

	/* background transparence */
    HI_U32 u32BgAlpha;

	/* OVERLAYEX region layer */
    HI_U32 u32Layer;   
    
}OVERLAYEX_CHN_ATTR_S;


typedef struct hiLINE_CHN_ATTR_S
{
	HI_U32  				u32LineWidth;       /* width of line */
	HI_U32  				u32LineColor;       /* color of line */
	POINT_S                 stLinePoints[2];    /* startpoint and endpoint of line */
}LINE_CHN_ATTR_S;

typedef union hiRGN_ATTR_U
{
    OVERLAY_ATTR_S      stOverlay;      /* attribute of overlay region */
    OVERLAYEX_ATTR_S    stOverlayEx;    /* attribute of overlayex region */ 
} RGN_ATTR_U;

typedef union hiRGN_CHN_ATTR_U
{
    OVERLAY_CHN_ATTR_S      stOverlayChn;      /* attribute of overlay region */
    COVER_CHN_ATTR_S        stCoverChn;        /* attribute of cover region */
    COVEREX_CHN_ATTR_S      stCoverExChn;      /* attribute of coverex region */
    OVERLAYEX_CHN_ATTR_S    stOverlayExChn;    /* attribute of overlayex region */
	LINE_CHN_ATTR_S    	    stLineChn;         /* attribute of drawline region */
    MOSAIC_CHN_ATTR_S       stMosaicChn;       /* attribute of mosic region */
} RGN_CHN_ATTR_U;

/* attribute of a region */
typedef struct hiRGN_ATTR_S
{
    RGN_TYPE_E enType;  /* region type */
    RGN_ATTR_U unAttr;  /* region attribute */
} RGN_ATTR_S;

/* attribute of a region */
typedef struct hiRGN_CHN_ATTR_S
{
    HI_BOOL           bShow;
    RGN_TYPE_E        enType;     /* region type */
    RGN_CHN_ATTR_U    unChnAttr;  /* region attribute */
} RGN_CHN_ATTR_S;


#define RGN_MAX_BMP_UPDATE_NUM 16

typedef struct hiRGN_BMP_UPDATE_S
{
    POINT_S             stPoint;
    BITMAP_S            stBmp;
    HI_U32              u32Stride;
} RGN_BMP_UPDATE_S;

typedef struct hiRGN_BMP_UPDATE_CFG_S
{
    HI_U32              u32BmpCnt;
    RGN_BMP_UPDATE_S    astBmpUpdate[RGN_MAX_BMP_UPDATE_NUM];
} RGN_BMP_UPDATE_CFG_S;

typedef struct hiRGN_CANVAS_INFO_S
{
    HI_U32         u32PhyAddr;
    HI_U32         u32VirtAddr;
    SIZE_S         stSize;              
    HI_U32         u32Stride;
    PIXEL_FORMAT_E enPixelFmt;  
} RGN_CANVAS_INFO_S;

/* PingPong buffer change when set attr, it needs to remap memory in mpi interface */
#define HI_NOTICE_RGN_BUFFER_CHANGE  HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_NOTICE, HI_SUCCESS)

/* invlalid device ID */
#define HI_ERR_RGN_INVALID_DEVID     HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HI_ERR_RGN_INVALID_CHNID     HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_RGN_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_RGN_EXIST             HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define HI_ERR_RGN_UNEXIST           HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_RGN_NULL_PTR          HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_RGN_NOT_CONFIG        HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_RGN_NOT_SUPPORT       HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_RGN_NOT_PERM          HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_RGN_NOMEM             HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_RGN_NOBUF             HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_RGN_BUF_EMPTY         HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_RGN_BUF_FULL          HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define HI_ERR_RGN_BADADDR           HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define HI_ERR_RGN_BUSY              HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define HI_ERR_RGN_NOTREADY          HI_DEF_ERR(HI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_REGION_H__ */



