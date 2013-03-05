#include "stdafx.h"
#include "oxo_helper.h"
#include "Win32_Func.h"
#include "ObjImage.h"


//=============================================================================
static int  PS_WORD2INT (const BYTE * pWord)
{
    BYTE	bySwap[2] = {pWord[0], pWord[1]} ;
    FSwap(bySwap[0], bySwap[1]) ;
    return *(short*)bySwap ;
}
////=============================================================================
//BOOL  FLib_LoadPhotoshopACF (PCSTR szFileName, std::deque<int> & listElem)
//{
//    listElem.clear() ;
//
//    std::ifstream    inFile (szFileName, std::ios::in|std::ios::binary) ;
//    if (!inFile.is_open())
//        return FALSE ;
//
//    // 得到文件长度
//    inFile.seekg (0, std::ios::end) ;
//    const int     nFileSize = inFile.tellg() ;
//    if (nFileSize != 54) // 27.elem * 2.bytes
//        return FALSE ;
//
//    // 先把文件读到内存中来
//    short      * pStart = (short*)new BYTE[nFileSize] ;
//    inFile.seekg (0, std::ios::beg) ;
//    inFile.read ((char*)pStart, nFileSize) ;
//
//    // 5 x 5 filter & scale & offset
//    for (int i=0 ; i < 25 ; i++)
//    {
//        listElem.push_back (PS_WORD2INT((BYTE*)&pStart[i])) ;
//    }
//    listElem.push_back (PS_WORD2INT((BYTE*)&pStart[25])) ;
//    listElem.push_back (PS_WORD2INT((BYTE*)&pStart[26])) ;
//
//    delete[] pStart ;
//    return TRUE ;
//}
////=============================================================================
//BOOL  FLib_SavePhotoshopACF (PCSTR szFileName, const std::deque<int> & listElem)
//{
//    if (listElem.size() != 27)
//        return FALSE ;
//
//    // 生成文件，存在即覆盖
//    FLib_SetFileNormal (szFileName) ;
//    std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
//    if (!outFile.is_open())
//        return FALSE ;
//
//    BYTE    pElem[2] ;
//    for (int i=0 ; i < (int)listElem.size() ; i++)
//    {
//        *(short*)pElem = (short)listElem[i] ;
//        FSwap (pElem[0], pElem[1]) ;
//        outFile.write ((char*)pElem, 2) ;
//    }
//    return TRUE ;
//}
////=============================================================================
//void  FLib_LoadPhoXoSettingFile (PCSTR szFileName, PHOXOSETTING & oXoSetting)
//{
//    std::ifstream    inFile (szFileName, std::ios::in|std::ios::binary) ;
//    if (inFile.is_open())
//    {
//        // 得到文件长度
//        inFile.seekg (0, std::ios::end) ;
//        if ((int)inFile.tellg() == sizeof(PHOXOSETTING))
//        {
//            inFile.seekg (0, std::ios::beg) ;
//            inFile.read ((char*)&oXoSetting, sizeof(PHOXOSETTING)) ;
//            return ;
//        }
//    }
//    // 文件不存在或长度不符，默认设置
//    oXoSetting.nUndoLevel = 20 ; // Undo级数, default(20)
//    oXoSetting.nBkType = 2 ; // 画布背景类型，0(颜色)，1(检测板)，2(粉红棋盘)
//    oXoSetting.nNewWidth = 640 ; // 新建图像宽
//    oXoSetting.nNewHeight = 480 ; // 新建图像高
//    oXoSetting.nResolution = 72 ; // 新建图像解析度
//    oXoSetting.crFore.rgbBlue = oXoSetting.crFore.rgbGreen = oXoSetting.crFore.rgbRed=0 ; // 前景色
//    oXoSetting.crBack.rgbBlue = oXoSetting.crBack.rgbGreen = oXoSetting.crBack.rgbRed=0xFF ; // 背景色
//    oXoSetting.nShadowOffsetX = oXoSetting.nShadowOffsetY = 5 ; // 阴影X/Y偏移
//    oXoSetting.nIsShowLogo = 1 ; // 默认显示logo
//    oXoSetting.nAntStepTime = 300 ; // 蚂蚁线间隔时间毫秒数
//}
//=============================================================================
//void  FLib_SavePhoXoSettingFile (PCSTR szFileName, const PHOXOSETTING & oXoSetting)
//{
//    //// 生成文件，存在即覆盖
//    //FLib_SetFileNormal (szFileName) ;
//    //std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
//    //if (outFile.is_open())
//    //    outFile.write ((char*)&oXoSetting, sizeof(oXoSetting)) ;
//}
//=============================================================================
BITMAPINFOHEADER *  FLib_NewImgInfoWithPalette (const FCObjImage & img)
{
    // prepare info
    const int           nColorNum = 1 << img.ColorBits() ;
    const int           nPalBytes = ((img.ColorBits() <= 8) ? (4*nColorNum) : 0) ;
    BITMAPINFOHEADER    * pBmfh = (BITMAPINFOHEADER *) new BYTE[16 + sizeof(BITMAPINFOHEADER) + nPalBytes] ;
    img.GetImageInfo (pBmfh) ;

    if (img.ColorBits() <= 8)
        img.GetColorTable (0, nColorNum, (RGBQUAD*)(pBmfh + 1)) ;
    else
        img.GetBitFields (pBmfh + 1) ;
    return pBmfh ;
}
//=============================================================================
BITMAPINFOHEADER *  FLib_NewImgInfoNoPalette (const FCObjImage & img)
{
    // prepare info
    BITMAPINFOHEADER    * pBmfh = (BITMAPINFOHEADER *) new BYTE[16 + sizeof(BITMAPINFOHEADER)] ;
    img.GetImageInfo (pBmfh) ;
    img.GetBitFields (pBmfh + 1) ;
    return pBmfh ;
}
//=============================================================================
