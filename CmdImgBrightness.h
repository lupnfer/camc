#pragma once
#include "cmdimgeffect.h"

class CCmdImgBrightness :public CCmdImgEffect
{
public :
	CCmdImgBrightness (int nPercent, IMAGE_CHANNEL nChannel = CHANNEL_RGB) : m_nPercent(nPercent), m_nChannel(nChannel) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
	void SetBrightness(int nPercent){m_nPercent = nPercent;}
protected :
	IMAGE_CHANNEL	m_nChannel ;
	int				m_nPercent ; // 当前亮度百分比 (>=0)

};
