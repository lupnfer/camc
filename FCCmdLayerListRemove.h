#pragma once
#include "fccmdimgcmdcomposite.h"

class FCCmdLayerListRemove :
	public FCCmdImgCmdComposite
{
public :
	FCCmdLayerListRemove (const FCObjCanvas & canvas, const int LayerIndex[], int nNum) ;

};
