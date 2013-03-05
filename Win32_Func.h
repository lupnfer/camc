
#ifndef	 __FOO_WIN32_FUNC_H__
#define	 __FOO_WIN32_FUNC_H__
#include "StdDefine.h"


class FCObjImage ; // external class
class FCObjSelect;

//-----------------------------------------------------------------------------
CBitmap* FLib_CreateBitmapHandle(const FCObjImage & img);
//=============================================================================
HBITMAP  FLib_CreateDDBHandle (const FCObjImage & img) ;
//=============================================================================
void  FLib_DrawImage (const FCObjImage & img, HDC hdc, int x, int y) ;
void  FLib_DrawImage (const FCObjImage & img, HDC hdc, const RECT & rcDrawDC) ;
//=============================================================================
void  FLib_CopyToClipboard (const FCObjImage & img) ;
//=============================================================================
BOOL  FLib_IsPasteAvailable () ;
//=============================================================================
void  FLib_GetClipboardImage (FCObjImage & img) ;
//=============================================================================
void  FLib_SetPixel (HDC hdc, int X, int Y, COLORREF crColor) ;
//=============================================================================
void  FLib_SetFileNormal (PCSTR szFileName) ;
//=============================================================================
BYTE *  FLib_MallocPixel (int nBytes) ;
//=============================================================================
void  FLib_FreePixel (void * pPixel) ;
//=============================================================================
// 从资源中读取
void  FLib_LoadImageRes (FCObjImage & img, LPCTSTR resName, LPCTSTR resType, IMAGE_TYPE imgType) ;
//=============================================================================
// the received image object is 24-bit 
BOOL  FLib_CaptureScreen (FCObjImage & img, const RECT & rcCap) ;
//=============================================================================

// OpenCv Iplimage to our FCObjImage

void DisplayImage(FCObjImage& imgObj);
// 基于GPU的样本块匹配

#endif
