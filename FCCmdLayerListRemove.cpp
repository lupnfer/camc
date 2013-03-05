#include "StdAfx.h"
#include ".\fccmdlayerlistremove.h"
#include "fooBit.h"
//=================================================================================
// ɾ�����ͼ��/remove multi layers from canvas
FCCmdLayerListRemove::FCCmdLayerListRemove (const FCObjCanvas & canvas, const int LayerIndex[], int nNum)
{
	if ((nNum >= 1) && LayerIndex)
	{
		int		* pIndex = new int[nNum] ;
		CopyMemory (pIndex, LayerIndex, nNum*sizeof(int)) ;
		fooBubbleSort (pIndex, nNum) ; // ��Ϊ����Ӻ���ǰɾ��
		while (--nNum >= 0)
		{
			m_cmdList.push_back (new FCCmdLayerRemove(canvas.GetLayer(pIndex[nNum]))) ;
		}
		delete[] pIndex ;
	}
}
