#include "StdAfx.h"
#include ".\cmdimgdrawlinebase.h"
#include "FCSinglePixelProcessBase.h"
//=================================================================================
// 画线基类/base class of drawline (32 bit)
void  CCmdImgDrawlineBase::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() != 32))
		return ;
	m_nAlpha = FClamp0255 (m_nAlpha) ;

	// 制作大小一样的alpha位图，注意：位图数据初始化为0（也即黑色）
	FCObjImage		imgAlpha (img.Width(), img.Height(), 8) ;
	
	// draw lines
	
	this->DrawAlphaMask (imgAlpha) ;
	//imgAlpha.Save("D:\\a.bmp");
	// 制作纯色位图
	FCObjImage          imgCover (img.Width(), img.Height(), 32) ;
	FCPixelFillColor    cmdFillCr(m_cr) ;
	imgCover.SinglePixelProcessProc (cmdFillCr) ;
	
	imgCover.AppendAlphaChannel (imgAlpha) ;
	//imgCover.Save("D:\\te.bmp");
	img.CombineAlphaImg (imgCover) ;
	//img.Save("D:\\te.bmp");
}

