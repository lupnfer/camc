#include "StdAfx.h"
#include ".\cmdimgfillcolor.h"
#include "ObjSelect.h"
#include "ObjCanvas.h"
#include "CmdSelectionSet.h"
#include "fcolor.h"
CCmdImgFillColor::CCmdImgFillColor(void)
{
}

CCmdImgFillColor::~CCmdImgFillColor(void)
{
}
void CCmdImgFillColor::Implement (FCObjImage & img, FCObjProgress * Percent) 
{
	
	FCObjSelect objselect;
	POINT ptLayer = img.GetGraphObjPos() ;
	objselect.SetMagicWand(m_point.x-ptLayer.x,m_point.y-ptLayer.y,30,img,TRUE,FCObjSelect::RGN_CREATE);
	
	POINT	ptSel = objselect.GetGraphObjPos();
	int		nStartX = ptSel.x-ptLayer.x,
		nStartY = ptSel.y-ptLayer.y;
	int nPan=img.ColorBits()/8;
	
	RGBQUAD rgb;
	rgb.rgbBlue=GetBValue(m_color);
	rgb.rgbGreen=GetGValue(m_color);
	rgb.rgbRed=GetRValue(m_color);
	rgb.rgbReserved=0xFF;

	
	for (int y=0 ; y < objselect.Height() ; y++)
	{
		BYTE	* pPixel = objselect.GetBits (y),
			* pRgnPx = img.GetBits(nStartX,nStartY+y);
		for (int x=0 ; x < objselect.Width() ; x++, pPixel++,pRgnPx+=nPan)
		{
			if(*pPixel!=0)
				FCColor::CopyPixel(pRgnPx,&rgb,nPan);
		}
	}
}

