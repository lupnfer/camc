#include "StdAfx.h"
#include ".\cmdimgdrawlinebase.h"
#include "FCSinglePixelProcessBase.h"
//=================================================================================
// ���߻���/base class of drawline (32 bit)
void  CCmdImgDrawlineBase::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() != 32))
		return ;
	m_nAlpha = FClamp0255 (m_nAlpha) ;

	// ������Сһ����alphaλͼ��ע�⣺λͼ���ݳ�ʼ��Ϊ0��Ҳ����ɫ��
	FCObjImage		imgAlpha (img.Width(), img.Height(), 8) ;
	
	// draw lines
	
	this->DrawAlphaMask (imgAlpha) ;
	//imgAlpha.Save("D:\\a.bmp");
	// ������ɫλͼ
	FCObjImage          imgCover (img.Width(), img.Height(), 32) ;
	FCPixelFillColor    cmdFillCr(m_cr) ;
	imgCover.SinglePixelProcessProc (cmdFillCr) ;
	
	imgCover.AppendAlphaChannel (imgAlpha) ;
	//imgCover.Save("D:\\te.bmp");
	img.CombineAlphaImg (imgCover) ;
	//img.Save("D:\\te.bmp");
}

