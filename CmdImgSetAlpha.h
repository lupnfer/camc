#pragma once
#include "cmdimgeffect.h"

class CCmdImgSetAlpha :public CCmdImgEffect
{
public :
	CCmdImgSetAlpha (int nAlpha) : m_nAlpha(nAlpha) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int			m_nAlpha ;
};
