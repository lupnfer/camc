#include "StdAfx.h"
#include "ImageHandle.h"
#include "compress/Rle.h"
#include <fstream>

//=============================================================================
//	TGA ͼ���ļ�ͷ ( ��BMP�ļ�һ�������ش����½Ǵ洢 )
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
    BYTE	byID_Length ;	// ͼ��ʶ����Ϣ��С
    BYTE	byPalType ;		// 00 : �޵�ɫ��	01 : �е�ɫ��
    BYTE	byImageType ;	// ����
    WORD	wPalFirstNdx ;	// ��ɫ����ʼ����
    WORD	wPalLength ;	// ��ɫ�峤��
    BYTE	byPalBits ;		// ��ɫ����ÿһ��ɫ��ռλ��
    WORD	wLeft ;			// �������Ļ���½�X����
    WORD	wBottom ;		// �������Ļ���½�Y����
    WORD	wWidth ;		// ��
    WORD	wHeight ;		// ��
    BYTE	byColorBits ;	// ɫ��λ��
    struct 
    {//����ṹ
        BYTE	AlphaBits : 4 ;	// ÿ����Alpha Channelλ��
        BYTE	HorMirror : 1 ;	// Ϊ1��ʾͼ���������ҵߵ��洢
        BYTE	VerMirror : 1 ;	// Ϊ1��ʾͼ���������µߵ��洢
        BYTE	Reserved : 2 ;
    } Descriptor ;
} TGAHEAD, * PTGAHEAD ; // 18 - Bytes
#pragma pack()

//===================================================================
BOOL  FCTgaHandle::LoadImageHeaderInfo (const BYTE * pStart, int nFileSize)
{
    // ���û���ķ�����ʼ����Ա
    FCImageHandle::LoadImageHeaderInfo (pStart, nFileSize) ;

    const TGAHEAD   * pTga = (TGAHEAD *) pStart ;

    if (pStart == NULL)
        return FALSE ;
    if ((pTga->byPalBits == 15) || (pTga->byPalBits == 16))
        return FALSE ; // ��֧��15, 16λɫ��ɫ��

    // ͼ����Ϣ
    m_ImageInfo.biWidth = pTga->wWidth ;
    m_ImageInfo.biHeight = pTga->wHeight ;
    m_ImageInfo.biBitCount = ((pTga->byColorBits == 15) ? 16 : pTga->byColorBits) ;
    m_ImageInfo.biCompression = BI_RGB ;

    // 24,32λ��ɫ������
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

    // �Ҷ�ͼ��ɫ�壬TGA��С֧��8bit
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

    // ��������
    const DWORD     dwPitch = pTga->wWidth * nColorBit / 8 ;
    BYTE            * pCurr = pStart + sizeof(TGAHEAD) + pTga->byID_Length ;
    if (pTga->byPalType == 1)
        pCurr += pTga->wPalLength * pTga->byPalBits / 8 ;

    if ((pTga->byImageType == TGA_RLEPAL) || (pTga->byImageType == TGA_RLERGB) || (pTga->byImageType == TGA_RLEMONO)) // ѹ��
        if (pStart[0x11] & 0x20) // ���µߵ�
            for (int i=0 ; i < pTga->wHeight ; i++)
                pCurr = ::RLE_TGA_DecodeLine (pCurr, nColorBit, pTga->wWidth, LinePtr[i]) ;
        else // same as BMP
            for (int i=pTga->wHeight - 1 ; i >= 0 ; i--)
                pCurr = ::RLE_TGA_DecodeLine (pCurr, nColorBit, pTga->wWidth, LinePtr[i]) ;
    else // δѹ��
        if (pStart[0x11] & 0x20) // ���µߵ�
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

    // �����ļ������ڼ�����
    std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
    if (!outFile.is_open())
        return FALSE ;

    // Initialize TGA Header
    const BYTE      fTgaInfo[] = {"foolish -- TGA"} ;
    TGAHEAD         TgaHead ;
    ZeroMemory (&TgaHead, sizeof(TGAHEAD)) ;
    TgaHead.byID_Length     = 14 ; // ������Ϣ
    TgaHead.byPalType       = (nColorBit == 8) ? 1 : 0 ;
    TgaHead.byImageType     = (nColorBit == 8) ? TGA_UCPAL : TGA_UCRGB ;
    TgaHead.wPalFirstNdx    = 0 ;
    TgaHead.wPalLength      = 256 ;
    TgaHead.byPalBits       = 24 ; // ��ɫ����ÿһ��ɫ��ռλ��
    TgaHead.wWidth          = nWidth ; // ��
    TgaHead.wHeight         = nHeight ; // ��
    TgaHead.byColorBits     = nColorBit ; // ɫ��λ��
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
