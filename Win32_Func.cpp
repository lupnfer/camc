#include "stdafx.h"
#include "Win32_Func.h"
#include "ObjImage.h"
#include "ObjSelect.h"
#include "oxo_helper.h"


#include "FColor.h"
extern IMAGE_TYPE  GetImageType_FromName (PCSTR szFileName) ;

#ifdef WIN32

//=============================================================================
HBITMAP  FLib_CreateDDBHandle (const FCObjImage & img)
{
    if (!img.IsValidImage())
    {
        FAssert(FALSE) ; return NULL ;
    }

    // 准备info
    BITMAPINFOHEADER   * pBmfh = FLib_NewImgInfoWithPalette (img) ;

    HDC         hdc = ::CreateDC (TEXT("DISPLAY"), NULL, NULL, NULL) ;
	HBITMAP     hBitmap = CreateDIBitmap (hdc, (BITMAPINFOHEADER *)pBmfh, CBM_INIT,
                                          (VOID *)img.GetMemStart(), (BITMAPINFO *)pBmfh, DIB_RGB_COLORS) ;
    ::DeleteDC (hdc) ;
	delete[] pBmfh ;
    return hBitmap ;
}
CBitmap* FLib_CreateBitmapHandle(const FCObjImage & img)
{
	HBITMAP bmp=FLib_CreateDDBHandle(img);
	CBitmap* bitmap=NULL;
	if(bmp!=NULL){
		bitmap=CBitmap::FromHandle(bmp);
	}
	return bitmap;
}
//=============================================================================
void  FLib_DrawImage (const FCObjImage & img, HDC hdc, int x, int y)
{
    if (!img.IsValidImage())
        return ;
//    HDC         hdcSrc = CreateCompatibleDC (hdc) ;
//    HBITMAP     hBmpOld = (HBITMAP)SelectObject (hdcSrc, img.GetHandle()) ;
//    ::BitBlt (hdc, x, y, img.Width(), img.Height(), hdcSrc, 0, 0, SRCCOPY) ;
//    SelectObject (hdcSrc, hBmpOld) ;
//    DeleteDC (hdcSrc) ;

    // 准备info
    BITMAPINFOHEADER   * pBmfh = FLib_NewImgInfoWithPalette (img) ;

    // start draw
    const int    nOldMode = ::SetStretchBltMode (hdc, COLORONCOLOR) ;
    ::SetDIBitsToDevice (hdc, x, y, img.Width(), img.Height(), 0, 0, 0, img.Height(),
                         img.GetMemStart(), (BITMAPINFO*)pBmfh, DIB_RGB_COLORS) ;
    ::SetStretchBltMode (hdc, nOldMode) ;
    delete[] pBmfh ;
}
//=============================================================================
void  FLib_DrawImage (const FCObjImage & img, HDC hdc, const RECT & rcDrawDC)
{
    if (!img.IsValidImage())
        return ;

    // 准备info
    BITMAPINFOHEADER   * pBmfh = FLib_NewImgInfoWithPalette (img) ;

    // start draw
    const int    nOldMode = ::SetStretchBltMode (hdc, COLORONCOLOR) ;
    ::StretchDIBits (hdc, rcDrawDC.left, rcDrawDC.top, RECTWIDTH(rcDrawDC), RECTHEIGHT(rcDrawDC),
                     0, 0, img.Width(), img.Height(),
                     img.GetMemStart(), (BITMAPINFO*)pBmfh, DIB_RGB_COLORS, SRCCOPY) ;
    ::SetStretchBltMode (hdc, nOldMode) ;
    delete[] pBmfh ;
}
//=============================================================================
void  FLib_CopyToClipboard (const FCObjImage & img)
{
    if (img.IsValidImage() && (img.ColorBits() >= 24))
        if (::OpenClipboard (NULL))
        {
            // 通过DIB拷贝
            HGLOBAL		hMem = GlobalAlloc (GMEM_MOVEABLE, img.GetPitch()*img.Height() + sizeof(BITMAPINFOHEADER) + 16) ;
            BITMAPINFOHEADER  * pClipData = (BITMAPINFOHEADER*)GlobalLock(hMem) ;
            img.GetImageInfo (pClipData) ;
            pClipData++ ;
            CopyMemory (pClipData, img.GetMemStart(), img.GetPitch()*img.Height()) ;
            GlobalUnlock (hMem) ;
            if (::EmptyClipboard())
                ::SetClipboardData (CF_DIB, hMem) ; 
            ::CloseClipboard () ;
             GlobalFree (hMem) ; // 想死就释放它吧，反正我是死过一次了:)
        }
}
//=============================================================================
BOOL  FLib_IsPasteAvailable ()
{
    return (IsClipboardFormatAvailable(CF_BITMAP) || IsClipboardFormatAvailable(CF_DIB)) ;
}
//=============================================================================
// 获得的位图为32bit
void  FLib_GetClipboardImage (FCObjImage & img)
{
    if (FLib_IsPasteAvailable())
        if (::OpenClipboard (NULL))
        {
            // 获得位图
            BITMAPINFOHEADER    * bmif = (BITMAPINFOHEADER *)::GetClipboardData(CF_DIB) ;
            if (img.Create (bmif))
            {
                // 象素值
                BYTE    * pPixel = (BYTE*)(bmif + 1) ;
                if (bmif->biCompression == BI_BITFIELDS)
                    pPixel += 12 ;
                CopyMemory (img.GetMemStart(), pPixel, img.GetPitch()*img.Height()) ;

                if (img.ColorBits() == 32)
                {
                    if (bmif->biCompression == BI_BITFIELDS)
                        img.SetAlphaChannelValue (0xFF) ;
                }
                else
                    img.ConvertTo32Bit() ;
            }
            ::CloseClipboard() ;
        }
}
//=============================================================================
void  FLib_SetPixel (HDC hdc, int X, int Y, COLORREF crColor)
{
    ::SetPixel (hdc, X, Y, crColor) ;
}
//=============================================================================
void  FLib_SetFileNormal (PCSTR szFileName)
{
    ::SetFileAttributesA (szFileName, FILE_ATTRIBUTE_NORMAL) ;
}
//=============================================================================
BYTE *  FLib_MallocPixel (int nBytes)
{
    return (BYTE*)VirtualAlloc (NULL, nBytes, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE) ;
}
//=============================================================================
void  FLib_FreePixel (void * pPixel)
{
    if (pPixel != NULL)
        ::VirtualFree (pPixel, 0, MEM_RELEASE) ;
}
//=============================================================================
void  FLib_LoadImageRes (FCObjImage & img, LPCTSTR resName, LPCTSTR resType, IMAGE_TYPE imgType)
{
    HRSRC     res = ::FindResource (NULL, resName, resType) ;
    HGLOBAL   gol = ::LoadResource (NULL, res) ;
    BYTE      * pImgData = (BYTE *) ::LockResource (gol) ;
    /*if (pImgData != NULL)
        img.Load (pImgData, ::SizeofResource (NULL, res), imgType) ;*/
}
//=============================================================================
// the received image object is 24-bit 
BOOL  FLib_CaptureScreen (FCObjImage & img, const RECT & rcCap)
{
    HDC     hSrcDC = CreateDC (_T("DISPLAY"), NULL, NULL, NULL) ;
    if (GetDeviceCaps (hSrcDC, BITSPIXEL) <= 8) // palette format
    {
        DeleteDC (hSrcDC) ;
        FAssert(FALSE) ; return FALSE ;
    }

    HDC       hMemDC = CreateCompatibleDC (hSrcDC) ;
    HBITMAP   hDDB = CreateCompatibleBitmap (hSrcDC, RECTWIDTH(rcCap), RECTHEIGHT(rcCap)) ;
    if (hDDB == NULL)
    {
        DeleteDC (hMemDC) ; DeleteDC (hSrcDC) ;
        FAssert(FALSE) ; return FALSE ;
    }

    // capture
    HBITMAP   hTemp = (HBITMAP) SelectObject (hMemDC, hDDB) ;
    BOOL      bRet = ::BitBlt (hMemDC, 0, 0, RECTWIDTH(rcCap), RECTHEIGHT(rcCap),
                               hSrcDC, rcCap.left, rcCap.top, SRCCOPY) ;
    SelectObject (hMemDC, hTemp) ; // 置换 hDDB

    // attach FCObjImage to DDB
    BITMAP    bm ;
    GetObject (hDDB, sizeof(BITMAP), &bm) ;
    if (img.Create (bm.bmWidth, bm.bmHeight, 24))
    {
        BITMAPINFOHEADER   bmif ;
        ZeroMemory (&bmif, sizeof(bmif)) ;
        bmif.biSize        = sizeof(BITMAPINFOHEADER) ;
        bmif.biWidth       = bm.bmWidth ;
        bmif.biHeight      = bm.bmHeight ;
        bmif.biPlanes      = 1 ;
        bmif.biBitCount    = 24 ;
        ::GetDIBits (hSrcDC, hDDB, 0, bm.bmHeight, img.GetMemStart(), (BITMAPINFO *)&bmif, DIB_RGB_COLORS) ;
    }

    DeleteObject (hDDB) ;
    DeleteDC (hMemDC) ;
    DeleteDC (hSrcDC) ;
    return TRUE ;
}
//=============================================================================



//=================================================================================
#else

HBITMAP  FLib_CreateDDBHandle (const FCObjImage & img)
{ FAssert(FALSE) ; return NULL ; }
void  FLib_DrawImage (const FCObjImage & img, HDC hdc, int x, int y)
{ FAssert(FALSE) ; }
void  FLib_CopyToClipboard (const FCObjImage & img)
{ FAssert(FALSE) ; }
BOOL  FLib_IsPasteAvailable ()
{ FAssert(FALSE) ; return FALSE ; }
void  FLib_GetClipboardImage (FCObjImage & img)
{ FAssert(FALSE) ; }
void  FLib_SetPixel (HDC hdc, int X, int Y, COLORREF crColor)
{ FAssert(FALSE) ; }
void  FLib_SetFileNormal (PCSTR szFileName)
{ FAssert(FALSE) ; }

BYTE  * FLib_MallocPixel (int nBytes)
{
    BYTE    * pByte = (BYTE*)malloc (nBytes) ;
    ZeroMemory (pByte, nBytes) ;
    return pByte ;
}
void  FLib_FreePixel (void * pPixel)
{
    if (pPixel != NULL)
        free (pPixel) ;
}

void  FLib_LoadImageRes (FCObjImage & img, PCTSTR resName, PCTSTR resType, IMAGE_TYPE imgType)
{ FAssert(FALSE) ; }
BOOL  FLib_CaptureScreen (FCObjImage & img, const RECT & rcCap)
{ FAssert(FALSE) ; return FALSE;}

#endif
