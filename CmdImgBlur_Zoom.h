#pragma once
#include "cmdimgeffect.h"

class CCmdImgBlur_Zoom :
	public CCmdImgEffect
{
public :
	CCmdImgBlur_Zoom (int nLength) : m_nLength(nLength) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int			m_nLength ;
};
