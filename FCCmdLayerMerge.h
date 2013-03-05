#pragma once
#include "fccmdimgcmdcomposite.h"
#include <deque>
class FCCmdLayerMerge :
	public FCCmdImgCmdComposite
{
public :
	FCCmdLayerMerge (const int nIndex[], int nNum) ;
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
protected :
	std::deque<int>     m_LayerIndex ;

};
