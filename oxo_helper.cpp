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
//    // �õ��ļ�����
//    inFile.seekg (0, std::ios::end) ;
//    const int     nFileSize = inFile.tellg() ;
//    if (nFileSize != 54) // 27.elem * 2.bytes
//        return FALSE ;
//
//    // �Ȱ��ļ������ڴ�����
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
//    // �����ļ������ڼ�����
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
//        // �õ��ļ�����
//        inFile.seekg (0, std::ios::end) ;
//        if ((int)inFile.tellg() == sizeof(PHOXOSETTING))
//        {
//            inFile.seekg (0, std::ios::beg) ;
//            inFile.read ((char*)&oXoSetting, sizeof(PHOXOSETTING)) ;
//            return ;
//        }
//    }
//    // �ļ������ڻ򳤶Ȳ�����Ĭ������
//    oXoSetting.nUndoLevel = 20 ; // Undo����, default(20)
//    oXoSetting.nBkType = 2 ; // �����������ͣ�0(��ɫ)��1(����)��2(�ۺ�����)
//    oXoSetting.nNewWidth = 640 ; // �½�ͼ���
//    oXoSetting.nNewHeight = 480 ; // �½�ͼ���
//    oXoSetting.nResolution = 72 ; // �½�ͼ�������
//    oXoSetting.crFore.rgbBlue = oXoSetting.crFore.rgbGreen = oXoSetting.crFore.rgbRed=0 ; // ǰ��ɫ
//    oXoSetting.crBack.rgbBlue = oXoSetting.crBack.rgbGreen = oXoSetting.crBack.rgbRed=0xFF ; // ����ɫ
//    oXoSetting.nShadowOffsetX = oXoSetting.nShadowOffsetY = 5 ; // ��ӰX/Yƫ��
//    oXoSetting.nIsShowLogo = 1 ; // Ĭ����ʾlogo
//    oXoSetting.nAntStepTime = 300 ; // �����߼��ʱ�������
//}
//=============================================================================
//void  FLib_SavePhoXoSettingFile (PCSTR szFileName, const PHOXOSETTING & oXoSetting)
//{
//    //// �����ļ������ڼ�����
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
