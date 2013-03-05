#include "StdAfx.h"
#include ".\fccmdlayermerge.h"
#include "foobit.h"
#include "ObjLayer.h"
#include "ObjCanvas.h"
#include "FColor.h"
#include "ObjProgress.h"
#include "LayerCommand.h"
//=================================================================================
// 合并图层/merge layers (32 bit)
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

	// 先按从小到大排序，删除时要从大的删起
	fooBubbleSort (arIndex, m_LayerIndex.size()) ;

	// 计算合并后图层大小
	RECT		rcNewLayer = {0x7FFFFFFF, 0x7FFFFFFF, 0x80000000, 0x80000000} ; // 最大最小值
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

	// 制作合并图层
	FCObjLayer		* pNewLayer = new FCObjLayer ;
	if (!pNewLayer->Create (RECTWIDTH(rcNewLayer), RECTHEIGHT(rcNewLayer), 32))
	{
		delete[] arIndex ; delete pNewLayer ;
		return ;
	}

	// 绘制各个图层（从底层往上绘制）
	for (i=0 ; i < (int)m_LayerIndex.size() ; i++)
	{
		FCObjLayer	* pLayer = canvas.GetLayer(arIndex[i]) ;
		if (pLayer->ColorBits() < 32) // 只支持32bit图层
			continue ;
		// pLayer在新图层上的位置
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

	// 插入删除组和命令
	m_cmdList.push_back (new FCCmdLayerAdd (pNewLayer, arIndex[0])) ; // 插在最小index图层
	for (i = m_LayerIndex.size()-1 ; i >=0 ; i--)
		m_cmdList.push_back (new FCCmdLayerRemove (canvas.GetLayer(arIndex[i]))) ;
	FCCmdImgCmdComposite::Execute (canvas, Percent) ;

	delete[] arIndex ;
}
