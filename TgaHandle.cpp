#include "StdAfx.h"
#include "ImageHandle.h"
#include "compress/Rle.h"
#include <fstream>

//=============================================================================
//	TGA 图像文件头 ( 与BMP文件一样，像素从左下角存储 )
//=============================================================================
// Definitions for TGA image types.
#define		TGA_NULL		0
#define		TGA_UCPAL		1
#define		TGA_UCRGB		2
#define		TGA_UCMONO		3
#define		TGA_RLEPAL		9
#define		TGA_RLERGB		10
#define		TGA_RLEMONO		11
#pragma pack(1)
typedef struct tagTGAHEAD
{
    BYTE	byID_Length ;	// 图像识别信息大小
    BYTE	byPalType ;		// 00 : 无调色板	01 : 有调色板
    BYTE	byImageType ;	// 见上
    WORD	wPalFirstNdx ;	// 调色板起始索引
    WORD	wPalLength ;	// 调色板长度
    BYTE	byPalBits ;		// 调色板中每一颜色所占位数
    WORD	wLeft ;			// 相对于屏幕左下角X坐标
    WORD	wBottom ;		// 相对于屏幕左下角Y坐标
    WORD	wWidth ;		// 宽
    WORD	wHeight ;		// 高
    BYTE	byColorBits ;	// 色彩位数
    struct 
    {//试验结构
        BYTE	AlphaBits : 4 ;	// 每像素Alpha Channel位数
        BYTE	HorMirror : 1 ;	// 为1表示图像数据左右颠倒存储
        BYTE	VerMirror : 1 ;	// 为1表示图像数据上下颠倒存储
        BYTE	Reserved : 2 ;
    } Descriptor ;
} TGAHEAD, * PTGAHEAD ; // 18 - Bytes
#pragma pack()

//===================================================================
BOOL  FCTgaHandle::LoadImageHeaderInfo (const BYTE * pStart, int nFileSize)
{
    // 调用基类的方法初始化成员
    FCImageHandle::LoadImageHeaderInfo (pStart, nFileSize) ;

    const TGAHEAD   * pTga = (TGAHEAD *) pStart ;

    if (pStart == NULL)
        return FALSE ;
    if ((pTga->byPalBits == 15) || (pTga->byPalBits == 16))
        return FALSE ; // 不支持15, 16位色调色板

    // 图像信息
    m_ImageInfo.biWidth = pTga->wWidth ;
    m_ImageInfo.biHeight = pTga->wHeight ;
    m_ImageInfo.biBitCount = ((pTga->byColorBits == 15) ? 16 : pTga->byColorBits) ;
    m_ImageInfo.biCompression = BI_RGB ;

    // 24,32位调色板设置
    const BYTE     * pCurr = pStart + sizeof(TGAHEAD) + pTga->byID_Length ;
    if (pTga->byPalType == 1)
    {
        RGBQUAD     * pPalette = new RGBQUAD [pTga->wPalLength] ;
        for (int i=0 ; i < pTga->wPalLength ; i++)
        {
            pPalette[i].rgbBlue = *pCurr++ ;
            pPalette[i].rgbGreen = *pCurr++ ;
            pPalette[i].rgbRed = *pCurr++ ;
            if (pTga->byPalBits == 32)
                pPalette[i].rgbReserved = *pCurr++ ;
        }
        CopyMemory (&m_Palette[pTga->wPalFirstNdx], pPalette, 4*pTga->wPalLength) ;
        delete[] pPalette ;
    }

    // 灰度图调色板，TGA最小支持8bit
    if ((pTga->byImageType == TGA_UCMONO) || (pTga->byImageType == TGA_RLEMONO))
    {
        for (int i=0 ; i < 256 ; i++)
        {
            m_Palette[i].rgbRed =
            m_Palette[i].rgbGreen =
            m_Palette[i].rgbBlue = i ;
        }
    }
    return TRUE ;
}
//===================================================================
BOOL  FCTgaHandle::LoadImagePixelData (BYTE * pStart, int nFileSize, const std::deque<BYTE*> & LinePtr)
{
    const TGAHEAD   * pTga = (TGAHEAD *) pStart ;
    const int       nColorBit = m_ImageInfo.biBitCount ;

    // 解码像素
    const DWORD     dwPitch = pTga->wWidth * nColorBit / 8 ;
    BYTE            * pCurr = pStart + sizeof(TGAHEAD) + pTga->byID_Length ;
    if (pTga->byPalType == 1)
        pCurr += pTga->wPalLength * pTga->byPalBits / 8 ;

    if ((pTga->byImageType == TGA_RLEPAL) || (pTga->byImageType == TGA_RLERGB) || (pTga->byImageType == TGA_RLEMONO)) // 压缩
        if (pStart[0x11] & 0x20) // 上下颠倒
            for (int i=0 ; i < pTga->wHeight ; i++)
                pCurr = ::RLE_TGA_DecodeLine (pCurr, nColorBit, pTga->wWidth, LinePtr[i]) ;
        else // same as BMP
            for (int i=pTga->wHeight - 1 ; i >= 0 ; i--)
                pCurr = ::RLE_TGA_DecodeLine (pCurr, nColorBit, pTga->wWidth, LinePtr[i]) ;
    else // 未压缩
        if (pStart[0x11] & 0x20) // 上下颠倒
            for (int i=0 ; i < pTga->wHeight ; i++, pCurr += dwPitch)
                CopyMemory (LinePtr[i], pCurr, dwPitch) ;
        else // same as BMP
            for (int i=pTga->wHeight - 1 ; i >= 0 ; i--, pCurr += dwPitch)
                CopyMemory (LinePtr[i], pCurr, dwPitch) ;
    return TRUE ;
}
//===================================================================
BOOL  FCTgaHandle::SaveImage (PCSTR szFileName, const std::deque<BYTE*> & LinePtr, int nFlag)
{
    const int   nWidth = m_ImageInfo.biWidth,
                nHeight = m_ImageInfo.biHeight,
                nColorBit = m_ImageInfo.biBitCount ;

    if ((nColorBit != 8) && (nColorBit != 16) && (nColorBit != 24) && (nColorBit != 32))
        return FALSE ;

    // 生成文件，存在即覆盖
    std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
    if (!outFile.is_open())
        return FALSE ;

    // Initialize TGA Header
    const BYTE      fTgaInfo[] = {"foolish -- TGA"} ;
    TGAHEAD         TgaHead ;
    ZeroMemory (&TgaHead, sizeof(TGAHEAD)) ;
    TgaHead.byID_Length     = 14 ; // 附加信息
    TgaHead.byPalType       = (nColorBit == 8) ? 1 : 0 ;
    TgaHead.byImageType     = (nColorBit == 8) ? TGA_UCPAL : TGA_UCRGB ;
    TgaHead.wPalFirstNdx    = 0 ;
    TgaHead.wPalLength      = 256 ;
    TgaHead.byPalBits       = 24 ; // 调色板中每一颜色所占位数
    TgaHead.wWidth          = nWidth ; // 宽
    TgaHead.wHeight         = nHeight ; // 高
    TgaHead.byColorBits     = nColorBit ; // 色彩位数
    ((BYTE*)&TgaHead)[17]   = 0x20 ;
    outFile.write ((char*)&TgaHead, sizeof(TGAHEAD)) ;
    outFile.write ((char*)fTgaInfo, 14) ;

    // write palette
    if (nColorBit == 8)
    {
        for (int i=0 ; i < 256 ; i++)
            outFile.write ((char*)&m_Palette[i], 3) ;
    }

    // write pixels
    DWORD    dwLineByte = nColorBit * nWidth / 8 ;
    for (int Y=0 ; Y < nHeight ; Y++)
        outFile.write ((char*)LinePtr[Y], dwLineByte) ;

    return TRUE ;
}
//===================================================================
