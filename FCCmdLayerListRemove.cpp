#include "StdAfx.h"
#include ".\fccmdlayerlistremove.h"
#include "fooBit.h"
//=================================================================================
// 删除多个图层/remove multi layers from canvas
FCCmdLayerListRemove::FCCmdLayerListRemove (const FCObjCanvas & canvas, const int LayerIndex[], int nNum)
{
	if ((nNum >= 1) && LayerIndex)
	{
		int		* pIndex = new int[nNum] ;
		CopyMemory (pIndex, LayerIndex, nNum*sizeof(int)) ;
		fooBubbleSort (pIndex, nNum) ; // 因为必须从后往前删除
		while (--nNum >= 0)
		{
			m_cmdList.push_back (new FCCmdLayerRemove(canvas.GetLayer(pIndex[nNum]))) ;
		}
		delete[] pIndex ;
	}
}
