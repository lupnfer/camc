#include "StdAfx.h"
#include "ImageHandle.h"
#include "compress/rle.h"
#include <fstream>

//=============================================================================
//	PCX 图像文件头
//=============================================================================
#pragma pack(1)
typedef struct tagPCXHEAD
{
    BYTE	signal ;		// 00 - always 0x0A
    BYTE	version ;		// 01 - version number (0,2,3,4,5)
    BYTE	encoding ;		// 02 - always 1 (RLE)
    BYTE	bit_per_pixel ;	// 03 - 每平面像素bit数 (1 or 4 or 8)
    WORD	xmin ;			// 04 
    WORD	ymin ;			// 06   此四个域指出
    WORD	xmax ;			// 08     图像大小
    WORD	ymax ;			// 0A  
    WORD	Xresolution ;	// 0C - 屏幕X解析度
    WORD	Yresolution ;	// 0E - 屏幕Y解析度
    BYTE	palette[48] ;	// 10 - 1 or 4 Bits 图像调色板
    BYTE	reserved ;		// 40 - Set 0
    BYTE	color_planes ;	// 41 - 色彩平面数目 (1 or 3 or 4)
    WORD	byte_per_line ;	// 42 - 每行字节宽度 (每色彩plane)
    WORD	palette_type ;	// 44 - 1 = color/BW, 2 = grayscale
    BYTE	filler[58] ;	// 46 - Set 0
} PCXHEAD, * PPCXHEAD ; // 128 - Bytes
#pragma pack()

//===================================================================
BOOL  FCPcxHandle::LoadImageHeaderInfo (const BYTE * pStart, int nFileSize)
{
    // 调用基类的方法初始化成员
    FCImageHandle::LoadImageHeaderInfo (pStart, nFileSize) ;

    PCXHEAD   * pPcx = (PCXHEAD *) pStart ;
    if ((pStart == NULL) || (nFileSize <= sizeof(PCXHEAD)))
        return FALSE ;
    if ((pPcx->signal != 0x0A) || (pPcx->encoding != 1) || (pPcx->version > 5))
        return FALSE ;

    // 图像信息
    m_ImageInfo.biWidth = pPcx->xmax - pPcx->xmin + 1 ;
    m_ImageInfo.biHeight = pPcx->ymax - pPcx->ymin + 1 ;
    m_ImageInfo.biBitCount = (WORD)(pPcx->bit_per_pixel * pPcx->color_planes) ;
    m_ImageInfo.biCompression = BI_RGB ;
    m_ImageInfo.biXPelsPerMeter = pPcx->Xresolution ;
    m_ImageInfo.biYPelsPerMeter = pPcx->Yresolution ;

    // 填充调色板
    const int     nColorBit = m_ImageInfo.biBitCount,
                  nColorNum = 1 << nColorBit ;
    if (nColorBit <= 8) // 1, 4, 8
        if (pPcx->palette_type == 2) // grayscale palette
        {
            for (int i=0 ; i < nColorNum ; i++)
            {
                m_Palette[i].rgbRed =
                m_Palette[i].rgbGreen =
                m_Palette[i].rgbBlue = (BYTE)(i * 255 / (nColorNum - 1)) ;
            }
        }
        else
        {
            const BYTE   * pCurr = ((nColorBit <= 4) ? pPcx->palette : (pStart + nFileSize - 768)) ;
            for (int i=0 ; i < nColorNum ; i++)
            {
                m_Palette[i].rgbRed = *pCurr++ ; // R
                m_Palette[i].rgbGreen = *pCurr++ ; // G
                m_Palette[i].rgbBlue = *pCurr++ ; // B
            }
        }
    return TRUE ;
}
//===================================================================
static void  __Pcx_PlanesToPixelsLine (BYTE * pixels, BYTE * bitplanes, int bytesperline)
{
    BYTE	mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01} ;
    BYTE	* pPlane4 = bitplanes,
            * pPlane3 = bitplanes + bytesperline,
            * pPlane2 = bitplanes + 2 * bytesperline,
            * pPlane1 = bitplanes + 3 * bytesperline ;
    for (int i=0 ; i < bytesperline ; i++)
    {
        int		nCount = 0 ;
        for (int j=0 ; j < 8 ; j++)
        {
            if (mask[j] & *pPlane1)		*pixels |= mask[nCount]  ;
            nCount++ ;
            if (mask[j] & *pPlane2)		*pixels |= mask[nCount]  ;
            nCount++ ;
            if (mask[j] & *pPlane3)		*pixels |= mask[nCount]  ;
            nCount++ ;
            if (mask[j] & *pPlane4)		*pixels |= mask[nCount]  ;
            nCount++ ;
            if (nCount == 8)
            {
                pixels++ ;
                nCount = 0 ;
            }
        }
        pPlane1++ ; pPlane2++ ; pPlane3++ ; pPlane4++ ;
    }
}
//===================================================================
BOOL  FCPcxHandle::LoadImagePixelData (BYTE * pStart, int nFileSize, const std::deque<BYTE*> & LinePtr)
{
    const PCXHEAD   * pPcx = (PCXHEAD *) pStart ;

    // 先解压图像数据到内存中，附加的32Kb是为了调色板
    const int   nOutBufSize = m_ImageInfo.biHeight * 4 * ((m_ImageInfo.biWidth * m_ImageInfo.biBitCount + 31) / 32) + 1024*32 ;
    BYTE        * pOutBuf = new BYTE[nOutBufSize] ;

    // 解压数据
    ::RLE_PCX_Decode (pStart + sizeof(PCXHEAD), nFileSize - sizeof(PCXHEAD), pOutBuf) ;

    // 像素数据对行入座
    BYTE        * pCurr = pOutBuf ;
    for (int y=0 ; y < (int)LinePtr.size() ; y++)
    {
        if (m_ImageInfo.biBitCount <= 8)
            if ((pPcx->bit_per_pixel == 1) && (pPcx->color_planes == 4))
            {
                __Pcx_PlanesToPixelsLine (LinePtr[y], pCurr, pPcx->byte_per_line) ;
                pCurr += 4 * pPcx->byte_per_line ;
            }
            else
            {
                ::CopyMemory (LinePtr[y], pCurr, pPcx->byte_per_line) ;
                pCurr += pPcx->byte_per_line ;
            }
        else // 24 bits color
        {
            BYTE    * pRed = pCurr,
                    * pGreen = pCurr + pPcx->byte_per_line,
                    * pBlue = pCurr + 2 * pPcx->byte_per_line ;
            BYTE    * pPixel = LinePtr[y] ;
            for (int x=0 ; x < m_ImageInfo.biWidth ; x++, pPixel += 3)
            {
                pPixel[0] = *pBlue++ ;
                pPixel[1] = *pGreen++ ;
                pPixel[2] = *pRed++ ;
            }
            pCurr += 3 * pPcx->byte_per_line ;
        }
    }
    delete[] pOutBuf ;
    return TRUE ;
}
//===================================================================
// 1,4,8,24 bit PCX文件写
BOOL  FCPcxHandle::SaveImage (PCSTR szFileName, const std::deque<BYTE*> & LinePtr, int nFlag)
{
    const int   nWidth = m_ImageInfo.biWidth,
                nHeight = m_ImageInfo.biHeight,
                nColorBit = m_ImageInfo.biBitCount ;

    if ((nColorBit != 1) && (nColorBit != 4) && (nColorBit != 8) && (nColorBit != 24))
        return FALSE ;

    // 生成文件，存在即覆盖
    std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
    if (!outFile.is_open())
        return FALSE ;

    // Initialize PCX Header
    PCXHEAD			PcxHead ;
    ZeroMemory (&PcxHead, sizeof(PcxHead)) ;
    PcxHead.signal			= 0x0A ;
    PcxHead.version			= 5 ;
    PcxHead.encoding		= 1 ;
    PcxHead.bit_per_pixel	= nColorBit % 16 ;
    PcxHead.xmax			= nWidth - 1 ;
    PcxHead.ymax			= nHeight - 1 ;
    PcxHead.Xresolution		= (WORD)m_ImageInfo.biXPelsPerMeter ;
    PcxHead.Yresolution		= (WORD)m_ImageInfo.biYPelsPerMeter ;
    PcxHead.color_planes	= (nColorBit == 24) ? 3 : 1 ;
    PcxHead.byte_per_line	= 2 * ((nWidth * PcxHead.bit_per_pixel + 15) / 16) ; // 每平面每行字节数
    PcxHead.palette_type	= 1 ;

    // Set 1-or-4 bit color table
    if (nColorBit <= 4)
        for (int i=0 ; i < (1 << nColorBit) ; i++)
        {
            PcxHead.palette[i * 3    ] = m_Palette[i].rgbRed ;
            PcxHead.palette[i * 3 + 1] = m_Palette[i].rgbGreen ;
            PcxHead.palette[i * 3 + 2] = m_Palette[i].rgbBlue ;
        }

    // write PCX file header
    outFile.write ((char*)&PcxHead, sizeof(PCXHEAD)) ;

    // prepare for input buffer
    const int   nInBufSize = nHeight * 4 * ((nWidth * nColorBit + 31) / 32) ;
    BYTE        * pInPtr = new BYTE[nInBufSize] ;
    BYTE        * pInStart = pInPtr ;
    if (nColorBit <= 8)
    {
        for (int y=0 ; y < nHeight ; y++, pInPtr+=PcxHead.byte_per_line)
            ::CopyMemory (pInPtr, LinePtr[y], PcxHead.byte_per_line) ;
    }
    else // 24 bit color
    {
        for (int y=0 ; y < nHeight ; y++)
        {
            BYTE    * pRed = pInPtr,
                    * pGreen = pInPtr + PcxHead.byte_per_line,
                    * pBlue = pInPtr + 2 * PcxHead.byte_per_line ;
            BYTE    * pPixel = LinePtr[y] ;
            for (int x=0 ; x < nWidth ; x++, pPixel += 3)
            {
                pRed[x]   = pPixel[2] ;
                pGreen[x] = pPixel[1] ;
                pBlue[x]  = pPixel[0] ;
            }
            pInPtr += 3 * PcxHead.byte_per_line ;
        }
    }

    // 编码
    BYTE    * pOutBuf = new BYTE[nInBufSize * 2 + 4096] ;
    int     nWrite = ::RLE_PCX_Encode (pInStart, PcxHead.byte_per_line * nHeight * PcxHead.color_planes, pOutBuf) ;
    outFile.write ((char*)pOutBuf, nWrite) ;
    delete[] pOutBuf ; delete[] pInStart ;

    // 8-bit color
    BYTE        byPalFlag = 0x0C ; // 调色板写在文件尾部
    outFile.write ((char*)&byPalFlag, 1) ;
    for (int i=0 ; i < 256 ; i ++)
        if (nColorBit == 8)
        {
            outFile.write ((char*)&m_Palette[i].rgbRed, 1) ;
            outFile.write ((char*)&m_Palette[i].rgbGreen, 1) ;
            outFile.write ((char*)&m_Palette[i].rgbBlue, 1) ;
        }
        else
        {
            int     nDummy = 0 ;
            outFile.write ((char*)&nDummy, 3) ;
        }
    return TRUE ;
}
//===================================================================
