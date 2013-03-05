#pragma once
#include "fccmdimgcmdcomposite.h"

class FCCmdLayerMoveComposite :
	public FCCmdImgCmdComposite
{
public :
	FCCmdLayerMoveComposite (FCObjLayer * pLayer) {m_pLayer = pLayer ;}
	virtual BOOL  IsNeedComposite (FCCmdArtPrider * cmd) ;
	FCObjLayer *  GetLayerPtr () const {return m_pLayer ;}
protected :
	FCObjLayer		* m_pLayer ;
};
