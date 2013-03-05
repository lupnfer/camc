#pragma once
#include "cmdimgeffect.h"

class CCmdImgCurveAdjust :
	public CCmdImgEffect
{
public :
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
};
