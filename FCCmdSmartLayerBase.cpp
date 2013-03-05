#include "StdAfx.h"
#include ".\fccmdsmartlayerbase.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
#include "VirtualPaper.h"
#include "VBDefine.h"
//=================================================================================
void  FCCmdSmartLayerBase::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	m_pLayer = canvas.GetCurrentLayer() ;
	if ((m_pLayer == NULL) || !m_pLayer->IsValidImage())
	{
		m_pLayer = NULL ;
		return ;
	}
	m_pCanvas = &canvas ;
	FCObjSelect		& rSelect = canvas.GetSelection() ;

	// 计算并保存undo区域
	m_bSaveAll = FALSE ;
	if (!m_pLayer->IsLayerLimited())
	{
		// 非限制图层，计算扩充大小（填满整个canvas）
		RECT	rcLayer ;
		m_pLayer->GetRectInCanvas (&rcLayer) ;
		SIZE	sizeCanvas = m_pCanvas->GetCanvasDimension() ;
		int		nExpL = FMax (0L, rcLayer.left),
			nExpT = FMax (0L, rcLayer.top),
			nExpR = FMax (0L, sizeCanvas.cx-rcLayer.right),
			nExpB = FMax (0L, sizeCanvas.cy-rcLayer.bottom) ;
		if (nExpL || nExpT || nExpR || nExpB)
		{
			m_bSaveAll = TRUE ; // 如果扩充就保存整个位图
			m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;
			POINT		ptOldPos = m_pLayer->GetGraphObjPos() ;
			m_pLayer->ExpandFrame (FALSE, nExpL, nExpT, nExpR, nExpB) ;
			m_pLayer->SetGraphObjPos (ptOldPos.x-nExpL, ptOldPos.y-nExpT) ; // 图层新的位置
			this->OnAfterGrowLayer (rcLayer.left,rcLayer.top, rcLayer.right,rcLayer.bottom) ; // 问问派生类还要干啥			
		}
		else
		{
			goto NoneSaveAll ; // 不用扩充，只保存一部分即可
		}
	}
	else
	{
NoneSaveAll :
		// 有限的图层
		this->QuerySaveRect (&m_rcSave) ; // 询问派生类要保存的layer区域
		if (IsRectEmpty(&m_rcSave))
			return ;
		m_pLayer->GetSubBlock (&m_Undo, m_rcSave) ; // 保存此区域
	}

	// 取出目标区域进行处理
	if (Percent != NULL)
		Percent->ResetProgress () ; // percent set to 0%
	FCObjImage		imgBlock ;
	RECT			rcBlock ;
	this->QuerySaveRect (&rcBlock) ;
	m_pLayer->GetSubBlock (&imgBlock, rcBlock) ;
	this->OnPrepareBlockRect (rcBlock) ; // 给派生类一个转换参数坐标系的机会
	this->Implement (imgBlock, Percent) ; // 具体算法处理之

	// 处理完成，现在把它放回去，注意区域之外不要覆盖
	RECT	rcInSel = rcBlock ;
	if (m_pCanvas->HasSelected())
	{
		m_pCanvas->LayerToCanvasRect (*m_pLayer, rcInSel) ;
		m_pCanvas->CanvasToLayerRect (rSelect, rcInSel) ; // rcBlock位于sel上的区域
	}
	for (int y=0 ; y < imgBlock.Height() ; y++)
	{
		BYTE	* pBlock = imgBlock.GetBits (y),
			* pLayer = m_pLayer->GetBits (rcBlock.left, rcBlock.top + y),
			* pSel = m_pCanvas->HasSelected() ? rSelect.GetBits(rcInSel.left, rcInSel.top + y) : NULL ;
		for (int x=0 ; x < imgBlock.Width() ; x++, pBlock+=4, pLayer+=4)
		{
			if ((pSel == NULL) || (*pSel == 0xFF))
			{
				if (m_pLayer->IsLayerLimited())
					*(RGBTRIPLE*)pLayer = *(RGBTRIPLE*)pBlock ; // 有限图层不要破坏alpha
				else
					*(RGBQUAD*)pLayer = *(RGBQUAD*)pBlock ;
			}
			if (pSel)
				pSel++ ;
		}
	}
	if (Percent != NULL)
		Percent->SetProgress (100) ; // set 100%
}
//=================================================================================
void  FCCmdSmartLayerBase::QuerySaveRect (RECT * prcSave) const
{
	m_pCanvas->GetSelectLayerRect (*m_pLayer, prcSave) ;
}
//=================================================================================
void  FCCmdSmartLayerBase::Undo ()
{
	if (!m_Undo.IsValidImage() || (m_pLayer == NULL))
		return ;

	if (m_bSaveAll)
	{
		m_Redo = * static_cast<FCObjImage *>(m_pLayer) ;
		* static_cast<FCObjImage *>(m_pLayer) = m_Undo ;
	}
	else
		if (!IsRectEmpty(&m_rcSave))
		{
			m_pLayer->GetSubBlock (&m_Redo, m_rcSave) ;
			m_pLayer->CoverBlock (m_Undo, m_rcSave.left, m_rcSave.top) ;
		}
		m_Undo.Destroy() ;
}
void  FCCmdSmartLayerBase::Redo ()
{
	if (!m_Redo.IsValidImage() || (m_pLayer == NULL))
		return ;

	if (m_bSaveAll)
	{
		m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;
		* static_cast<FCObjImage *>(m_pLayer) = m_Redo ;
	}
	else
		if (!IsRectEmpty(&m_rcSave))
		{
			m_pLayer->GetSubBlock (&m_Undo, m_rcSave) ;
			m_pLayer->CoverBlock (m_Redo, m_rcSave.left, m_rcSave.top) ;
		}
		m_Redo.Destroy() ;
}


