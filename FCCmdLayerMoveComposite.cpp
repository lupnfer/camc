#include "StdAfx.h"
#include ".\fccmdlayermovecomposite.h"

//=================================================================================
BOOL  FCCmdLayerMoveComposite::IsNeedComposite (FCCmdArtPrider * cmd)
{
	if ((dynamic_cast<FCCmdLayerMove*>(cmd) != NULL) &&
		(dynamic_cast<FCCmdLayerMove*>(cmd)->GetLayerPtr() == m_pLayer))
		return TRUE ;
	if ((dynamic_cast<FCCmdLayerMoveComposite*>(cmd) != NULL) &&
		(dynamic_cast<FCCmdLayerMoveComposite*>(cmd)->GetLayerPtr() == m_pLayer))
		return TRUE ;
	return FALSE ;
}
