#pragma once
#include "fccmdsmartlayerbase.h"
#include "CAMCView.h"
class CCmdImgDrawlineBase :public FCCmdSmartLayerBase
{
public :
		CCmdImgDrawlineBase (POINT offset,int nAlpha, RGBQUAD cr, int nPenWidth, LINE_STYLE LineStyle = LINE_STYLE_SOLID) : m_offset(offset),m_nAlpha(nAlpha), m_cr(cr), m_nPenWidth(nPenWidth), m_LineStyle(LineStyle) {}
		virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
		virtual void  DrawAlphaMask (FCObjImage & imgAlpha) const =0 ;
protected :
		int          m_nAlpha ; // Í¸Ã÷¶È
		RGBQUAD      m_cr ;
		int          m_nPenWidth ;
		LINE_STYLE   m_LineStyle ;

		POINT   m_offset;

};
