#include "StdAfx.h"
#include "ImageHandle.h"
#include <memory.h>

//===================================================================
BOOL  FCImageHandle::LoadImageHeaderInfo (const BYTE * pStart, int nFileSize)
{
    memset (&m_ImageInfo, 0, sizeof(m_ImageInfo)) ;
    memset (m_dwBitfields, 0, 16) ;
    memset (m_Palette, 0, sizeof(RGBQUAD)*256) ;
    m_ImageInfo.biSize = sizeof(m_ImageInfo) ;
    m_ImageInfo.biPlanes = 1 ;
    return TRUE ;
}
//===================================================================
