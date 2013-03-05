#include "stdafx.h"
#include "ObjLayer.h"
#include "StdDefine.h"
#include "ObjProgress.h"
#include "CAMCView.h"
#include "MainFrm.h"
//=============================================================================
FCObjLayer::FCObjLayer ()
{
    pParentCanvas = NULL ;
    m_bLayerVisible = TRUE ;
    m_bLayerLimited = FALSE ; // alpha为0区域可着色
    m_nAlphaPercent = 100 ;
	m_rv = FALSE;
	//m_paper.Init()
}
//=============================================================================
FCObjLayer & FCObjLayer::operator= (const FCObjLayer & layer)
{
	this->SetParentCanvas (layer.GetParentCanvas()) ;
	this->SetLayerVisible (layer.GetLayerVisible()) ;
	this->SetLayerLimitedFlag (layer.IsLayerLimited()) ;
	this->SetLayerTransparent (layer.GetLayerTransparent()) ;
	FCObjImage::operator= (layer) ;
	return *this ;
}
//=============================================================================
int  FCObjLayer::Serialize (BOOL bSave, BYTE * pSave)
{
	const BYTE   * pStartBak = pSave ;
	if (bSave)
	{
		*(BOOL*)pSave = m_bLayerVisible ; pSave += sizeof(BOOL) ;
		*(BOOL*)pSave = m_bLayerLimited ; pSave += sizeof(BOOL) ;
		*(int*)pSave = m_nAlphaPercent ; pSave += sizeof(int) ;
	}
	else
	{
		m_bLayerVisible = *(BOOL*)pSave ; pSave += sizeof(BOOL) ;
		m_bLayerLimited = *(BOOL*)pSave ; pSave += sizeof(BOOL) ;
		m_nAlphaPercent = *(int*)pSave ; pSave += sizeof(int) ;
	}
	pSave += FCObjImage::Serialize (bSave, pSave) ;
	return pSave - pStartBak ;
}

int  FCObjTextLayer::Serialize (BOOL bSave, BYTE * pSave)
{
#ifdef WIN32
	const BYTE   * pBak = pSave ;
	if (bSave)
	{
		int		nStrBytes = (lstrlen(m_pszText) + 1) * sizeof(TCHAR) ;
		CopyMemory (pSave, m_pszText, nStrBytes) ;
		pSave += nStrBytes ;
		CopyMemory (pSave, &m_LogFont, sizeof(m_LogFont)) ;
		pSave += sizeof(m_LogFont) ;
		*(BOOL*)pSave = m_bAddShadow ; pSave += sizeof(BOOL) ;
		CopyMemory (pSave, &m_ShadowData, sizeof(m_ShadowData)) ;
		pSave += sizeof(m_ShadowData) ;
	}
	else
	{
		int		nStrBytes = (lstrlen((TCHAR*)pSave) + 1) * sizeof(TCHAR) ;
		if (m_pszText != NULL)
			delete[] m_pszText ;
		m_pszText = (TCHAR*)new BYTE[nStrBytes] ;
		CopyMemory (m_pszText, pSave, nStrBytes) ;
		pSave += nStrBytes ;
		CopyMemory (&m_LogFont, pSave, sizeof(m_LogFont)) ;
		pSave += sizeof(m_LogFont) ;
		m_bAddShadow = *(BOOL*)pSave ; pSave += sizeof(BOOL) ;
		CopyMemory (&m_ShadowData, pSave, sizeof(m_ShadowData)) ;
		pSave += sizeof(m_ShadowData) ;
		SetToolEditFlag (TRUE) ; // 不能create生成
	}
	pSave += FCObjLayer::Serialize (bSave, pSave) ;
	return pSave - pBak ;
#else
    FAssert(FALSE) ;
    return 0 ;
#endif
}
//=============================================================================
/*
// 调用FreeType创建位图
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma comment (lib, "freetype218MT_D.lib")

void  fooFT_CreateStringImage (PCSTR szFontFile , PCSTR szText, FCObjImage & imgText)
{
    FT_Library     fft_library ;

    FT_Error       fft_error = FT_Init_FreeType (&fft_library) ;
    if (fft_error != 0)
        return ;

    FT_Face     fft_face ;
    fft_error = FT_New_Face (fft_library, szFontFile, 0, &fft_face) ;
    if (fft_error != 0)
    {
        FT_Done_FreeType (fft_library) ;
        return ;
    }

    fft_error = FT_Set_Char_Size (fft_face, 128 * 64, 0, 0, 0) ;
    if (fft_error != 0)
        return ;

//    fft_error = FT_Select_CharMap (fft_face, FT_ENCODING_UNICODE) ;
    if (fft_error != 0)
        return ;

    const FT_Bool    fft_use_kerning = FT_HAS_KERNING (fft_face) ;
    FT_UInt          fft_previous = 0 ;

    for (int n=0 ; n < strlen(szText) ; n++)
    {
        // convert character code to glyph index
        FT_UInt    fft_glyph_index = FT_Get_Char_Index (fft_face, szText[n]) ;

        // retrieve kerning distance and move pen position
        if (fft_use_kerning && fft_previous && fft_glyph_index)
        {
            FT_Vector  delta;
            
            
            FT_Get_Kerning( fft_face, fft_previous, fft_glyph_index,
                FT_KERNING_DEFAULT, &delta );
            
      //      pen_x += delta.x >> 6;
            ::Sleep(00);
        }

        fft_error = FT_Load_Glyph (fft_face, fft_glyph_index, FT_LOAD_DEFAULT) ;
        if (fft_error != 0)
            continue ; // ignore errors, jump to next glyph

        FT_GlyphSlot     fft_slot = fft_face->glyph ;
        fft_previous = fft_glyph_index ;
    }

    FT_Load_Char (fft_face, szText[0], FT_LOAD_RENDER) ;

    imgText.Create (slot->bitmap.width, slot->bitmap.rows, 8) ;
    imgText.SetGrayPalette() ;
    for (int y=0 ; y < imgText.Height() ; y++)
        CopyMemory (imgText.GetBits(y), slot->bitmap.buffer+slot->bitmap.pitch*y, slot->bitmap.pitch);

    FT_Done_FreeType (fft_library) ;
}*/
//=============================================================================
void  FCObjTextLayer::Create (HFONT hFont, const TCHAR * szText, const SIZE & sizeText, RGBQUAD crFont,
							  BOOL bAddShadow, const SHADOWDATA & ShadowData)
{

}
//=============================================================================

