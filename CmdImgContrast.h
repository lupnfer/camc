#pragma once
#include "cmdimgeffect.h"

class CCmdImgContrast :
	public CCmdImgEffect
{
public :
	CCmdImgContrast (int nPercent, IMAGE_CHANNEL nChannel = CHANNEL_RGB) : m_nPercent(nPercent), m_nChannel(nChannel) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
	void SetContrast(int nPercent){m_nPercent = nPercent;}
protected :
	IMAGE_CHANNEL	m_nChannel ;
	int				m_nPercent ; // 当前对比度百分比 (>=0)
};
