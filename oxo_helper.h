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
    INT32       nUndoLevel ; // Undo����, default(20)
    INT32       nBkType ; // �����������ͣ�0(��ɫ)��1(����)��2(�ۺ�����)
    RGBQUAD     crCanvasBk ; // canvas����ɫ
    INT32       nNewWidth ; // �½�ͼ���
    INT32       nNewHeight ; // �½�ͼ���
    INT32       nResolution ; // �½�ͼ�������
    RGBQUAD     crFore ; // ǰ��ɫ
    RGBQUAD     crBack ; // ����ɫ
    INT32       nShadowOffsetX ; // ��ӰXƫ��
    INT32       nShadowOffsetY ; // ��ӰYƫ��
    INT32       nIsShowLogo ; // ����ʱ�Ƿ���ʾlogo
    INT32       nAntStepTime ; // �����߼��ʱ�������
} PHOXOSETTING ;
//=============================================================================
//void  FLib_LoadPhoXoSettingFile (PCSTR szFileName, PHOXOSETTING & oXoSetting) ;
//=============================================================================
//void  FLib_SavePhoXoSettingFile (PCSTR szFileName, const PHOXOSETTING & oXoSetting) ;
//=============================================================================
BITMAPINFOHEADER *  FLib_NewImgInfoWithPalette (const FCObjImage & img) ;
BITMAPINFOHEADER *  FLib_NewImgInfoNoPalette (const FCObjImage & img) ;
//=============================================================================


