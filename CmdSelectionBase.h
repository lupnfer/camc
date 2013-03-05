#pragma once
#include "objbase.h"

class CCmdSelectionBase :
	public FCCmdArtPrider
{
public :
	CCmdSelectionBase () : m_pCanvas(NULL) {}
protected :
	FCObjCanvas		* m_pCanvas ;
};
