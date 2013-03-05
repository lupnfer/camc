#pragma once
#include "cmdimgeffect.h"

class CCmdImgHueSat :
	public CCmdImgEffect
{
public :
	CCmdImgHueSat (int nPercentHue, int nPercentSat) : m_nPercentHue(nPercentHue), m_nPercentSat(nPercentSat) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int		m_nPercentHue, m_nPercentSat ; // 当前色调,饱和度百分比 (>=0)
};
