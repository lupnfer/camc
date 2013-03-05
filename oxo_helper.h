#pragma once


#include "StdDefine.h"

class FCObjImage ; // external class

//=============================================================================
//BOOL  FLib_LoadPhotoshopACF (PCSTR szFileName, std::deque<int> & listElem) ;
//=============================================================================
//BOOL  FLib_SavePhotoshopACF (PCSTR szFileName, const std::deque<int> & listElem) ;
//=============================================================================
// PhoXo setting file format
typedef struct tagPHOXOSETTING
{
    INT32       nUndoLevel ; // Undo级数, default(20)
    INT32       nBkType ; // 画布背景类型，0(颜色)，1(检测板)，2(粉红棋盘)
    RGBQUAD     crCanvasBk ; // canvas背景色
    INT32       nNewWidth ; // 新建图像宽
    INT32       nNewHeight ; // 新建图像高
    INT32       nResolution ; // 新建图像解析度
    RGBQUAD     crFore ; // 前景色
    RGBQUAD     crBack ; // 背景色
    INT32       nShadowOffsetX ; // 阴影X偏移
    INT32       nShadowOffsetY ; // 阴影Y偏移
    INT32       nIsShowLogo ; // 启动时是否显示logo
    INT32       nAntStepTime ; // 蚂蚁线间隔时间毫秒数
} PHOXOSETTING ;
//=============================================================================
//void  FLib_LoadPhoXoSettingFile (PCSTR szFileName, PHOXOSETTING & oXoSetting) ;
//=============================================================================
//void  FLib_SavePhoXoSettingFile (PCSTR szFileName, const PHOXOSETTING & oXoSetting) ;
//=============================================================================
BITMAPINFOHEADER *  FLib_NewImgInfoWithPalette (const FCObjImage & img) ;
BITMAPINFOHEADER *  FLib_NewImgInfoNoPalette (const FCObjImage & img) ;
//=============================================================================


