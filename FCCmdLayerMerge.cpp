#include "StdAfx.h"
#include ".\fccmdlayermerge.h"
#include "foobit.h"
#include "ObjLayer.h"
#include "ObjCanvas.h"
#include "FColor.h"
#include "ObjProgress.h"
#include "LayerCommand.h"
//=================================================================================
// �ϲ�ͼ��/merge layers (32 bit)
FCCmdLayerMerge::FCCmdLayerMerge (const int nIndex[], int nNum)
{
	for (int i=0 ; i < nNum ; i++)
		m_LayerIndex.push_back (nIndex[i]) ;
}
void  FCCmdLayerMerge::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	if (m_LayerIndex.size() <= 1)
		return ;

	int		* arIndex = new int[m_LayerIndex.size()], i ;
	for (i=0 ; i < (int)m_LayerIndex.size() ; i++)
		arIndex[i] = m_LayerIndex[i] ;

	// �Ȱ���С��������ɾ��ʱҪ�Ӵ��ɾ��
	fooBubbleSort (arIndex, m_LayerIndex.size()) ;

	// ����ϲ���ͼ���С
	RECT		rcNewLayer = {0x7FFFFFFF, 0x7FFFFFFF, 0x80000000, 0x80000000} ; // �����Сֵ
	for (i=0 ; i < (int)m_LayerIndex.size() ; i++)
	{
		FCObjLayer	* pLayer = canvas.GetLayer(arIndex[i]) ;
		if (pLayer == NULL)
		{
			delete[] arIndex ;
			return ;
		}
		POINT		ptLayer = pLayer->GetGraphObjPos() ;
		if (ptLayer.x < rcNewLayer.left)	rcNewLayer.left = ptLayer.x ;

		if (ptLayer.x + (int)pLayer->Width() > rcNewLayer.right)	rcNewLayer.right = ptLayer.x + (int)pLayer->Width() ;
		if (ptLayer.y < rcNewLayer.top)		rcNewLayer.top = ptLayer.y ;
		if (ptLayer.y + (int)pLayer->Height() > rcNewLayer.bottom)	rcNewLayer.bottom = ptLayer.y + (int)pLayer->Height() ;
	}

	// �����ϲ�ͼ��
	FCObjLayer		* pNewLayer = new FCObjLayer ;
	if (!pNewLayer->Create (RECTWIDTH(rcNewLayer), RECTHEIGHT(rcNewLayer), 32))
	{
		delete[] arIndex ; delete pNewLayer ;
		return ;
	}

	// ���Ƹ���ͼ�㣨�ӵײ����ϻ��ƣ�
	for (i=0 ; i < (int)m_LayerIndex.size() ; i++)
	{
		FCObjLayer	* pLayer = canvas.GetLayer(arIndex[i]) ;
		if (pLayer->ColorBits() < 32) // ֻ֧��32bitͼ��
			continue ;
		// pLayer����ͼ���ϵ�λ��
		POINT		ptLayer = pLayer->GetGraphObjPos() ;
		ptLayer.x -= rcNewLayer.left ; ptLayer.y -= rcNewLayer.top ; 

		for (int y=0 ; y < pLayer->Height() ; y++)
		{
			RGBQUAD	* pDest = (RGBQUAD*)pNewLayer->GetBits (ptLayer.x, y+ptLayer.y),
				* pSrc = (RGBQUAD*)pLayer->GetBits (y) ;
			for (int x=0 ; x < pLayer->Width() ; x++, pDest++, pSrc++)
			{
				int     nSrcA = pSrc->rgbReserved * pLayer->GetLayerTransparent() / 100 ;
				FCColor::CombineAlphaPixel (pDest, *pDest, pDest->rgbReserved, *pSrc, nSrcA) ;
			}
		}
		if (Percent != NULL)
			Percent->SetProgress (100 * (i + 1) / m_LayerIndex.size()) ;
	}
	pNewLayer->SetGraphObjPos (rcNewLayer.left, rcNewLayer.top) ;

	// ����ɾ���������
	m_cmdList.push_back (new FCCmdLayerAdd (pNewLayer, arIndex[0])) ; // ������Сindexͼ��
	for (i = m_LayerIndex.size()-1 ; i >=0 ; i--)
		m_cmdList.push_back (new FCCmdLayerRemove (canvas.GetLayer(arIndex[i]))) ;
	FCCmdImgCmdComposite::Execute (canvas, Percent) ;

	delete[] arIndex ;
}
