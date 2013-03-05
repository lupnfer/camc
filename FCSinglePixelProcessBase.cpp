#include "StdAfx.h"
#include ".\fcsinglepixelprocessbase.h"
#include "FColor.h"
#include "ObjImage.h"
//=================================================================================
FCSinglePixelProcessBase::FCSinglePixelProcessBase ()
{
	m_prcImg = NULL ;
	m_pImgOld = NULL ;
}
FCSinglePixelProcessBase::~FCSinglePixelProcessBase ()
{
	if (m_prcImg != NULL)
		delete m_prcImg ;
	if (m_pImgOld != NULL)
		delete m_pImgOld ;
}
BOOL  FCSinglePixelProcessBase::ValidateColorBits (const FCObjImage * pImg)
{
	return (pImg->IsValidImage() && (pImg->ColorBits() >= 24)) ;
}
void  FCSinglePixelProcessBase::SetProcessRect (const RECT * prcImg)
{
	if (prcImg == NULL)
	{
		// 置区域为空
		if (m_prcImg != NULL)
			delete m_prcImg ;
		m_prcImg = NULL ;
	}
	else
	{
		// 设置区域
		if (m_prcImg == NULL)
			m_prcImg = new RECT ;
		::CopyRect (m_prcImg, prcImg) ;
	}
}
void  FCSinglePixelProcessBase::SetBackupImage (const FCObjImage * pImg)
{
	if (pImg != NULL)
	{
		if (m_pImgOld != NULL)
			delete m_pImgOld ;
		m_pImgOld = new FCObjImage(*pImg) ;
	}
}
//=================================================================================
// 填充颜色/fill color (>=24 bit)
FCPixelFillColor::FCPixelFillColor (RGBQUAD crFill, int nAlpha, RECT * prcFill)
{
	m_crFill = crFill ;
	m_nAlpha = nAlpha ;
	SetProcessRect (prcFill) ;
	m_bIsFillAlpha = FALSE ;
}
BOOL  FCPixelFillColor::ValidateColorBits (const FCObjImage * pImg)
{
	if (pImg->IsValidImage() && (pImg->ColorBits() >= 24))
	{
		m_bIsFillAlpha = ((m_nAlpha != -1) && (pImg->ColorBits() == 32)) ;
		return TRUE ;
	}
	return FALSE ;
}
void  FCPixelFillColor::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
    FCColor::CopyPixel (pPixel, &m_crFill, 3) ;
	if (m_bIsFillAlpha)
		pPixel[3] = m_nAlpha ;
}
//=================================================================================
// 设置alpha通道值/set alpha channel value (32 bit)
BOOL  FCPixelSetAlphaValue::ValidateColorBits (const FCObjImage * pImg)
{
	return (pImg->IsValidImage() && (pImg->ColorBits() == 32)) ;
}
void  FCPixelSetAlphaValue::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
	pPixel[3] = m_nAlpha ;
}
//=================================================================================
// 组合颜色/combine color (32 bit)
BOOL  FCPixelCombineColor::ValidateColorBits (const FCObjImage * pImg)
{
	return (pImg->IsValidImage() && (pImg->ColorBits() == 32)) ;
}
void  FCPixelCombineColor::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
	RGBQUAD		* pCurr = (RGBQUAD*)pPixel ;
	FCColor::CombineAlphaPixel (pCurr, *pCurr, pCurr->rgbReserved, m_crFill, m_crFill.rgbReserved) ;
}
//=================================================================================
// 反片/negate (>=24 bit)
void  FCPixelInvert::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
	for (int i=0 ; i < 3 ; i++)
		pPixel[i] = ~pPixel[i] ;
}
//=================================================================================
// 灰化/gray scale (>=24 bit)
void  FCPixelGrayscale::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
	pPixel[2] = pPixel[1] = pPixel[0] = FCColor::GetGrayscale ((RGBQUAD*)pPixel) ;
}
//=================================================================================
// 调节RGB值/adjust RGB (>=24 bit)
void  FCPixelAdjustRGB::ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
{
	pPixel[0] = FClamp0255 (pPixel[0] + m_iDeltaB) ;
	pPixel[1] = FClamp0255 (pPixel[1] + m_iDeltaG) ;
	pPixel[2] = FClamp0255 (pPixel[2] + m_iDeltaR) ;
}
