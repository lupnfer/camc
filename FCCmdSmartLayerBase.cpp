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

	// ���㲢����undo����
	m_bSaveAll = FALSE ;
	if (!m_pLayer->IsLayerLimited())
	{
		// ������ͼ�㣬���������С����������canvas��
		RECT	rcLayer ;
		m_pLayer->GetRectInCanvas (&rcLayer) ;
		SIZE	sizeCanvas = m_pCanvas->GetCanvasDimension() ;
		int		nExpL = FMax (0L, rcLayer.left),
			nExpT = FMax (0L, rcLayer.top),
			nExpR = FMax (0L, sizeCanvas.cx-rcLayer.right),
			nExpB = FMax (0L, sizeCanvas.cy-rcLayer.bottom) ;
		if (nExpL || nExpT || nExpR || nExpB)
		{
			m_bSaveAll = TRUE ; // �������ͱ�������λͼ
			m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;
			POINT		ptOldPos = m_pLayer->GetGraphObjPos() ;
			m_pLayer->ExpandFrame (FALSE, nExpL, nExpT, nExpR, nExpB) ;
			m_pLayer->SetGraphObjPos (ptOldPos.x-nExpL, ptOldPos.y-nExpT) ; // ͼ���µ�λ��
			this->OnAfterGrowLayer (rcLayer.left,rcLayer.top, rcLayer.right,rcLayer.bottom) ; // ���������໹Ҫ��ɶ			
		}
		else
		{
			goto NoneSaveAll ; // �������䣬ֻ����һ���ּ���
		}
	}
	else
	{
NoneSaveAll :
		// ���޵�ͼ��
		this->QuerySaveRect (&m_rcSave) ; // ѯ��������Ҫ�����layer����
		if (IsRectEmpty(&m_rcSave))
			return ;
		m_pLayer->GetSubBlock (&m_Undo, m_rcSave) ; // ���������
	}

	// ȡ��Ŀ��������д���
	if (Percent != NULL)
		Percent->ResetProgress () ; // percent set to 0%
	FCObjImage		imgBlock ;
	RECT			rcBlock ;
	this->QuerySaveRect (&rcBlock) ;
	m_pLayer->GetSubBlock (&imgBlock, rcBlock) ;
	this->OnPrepareBlockRect (rcBlock) ; // ��������һ��ת����������ϵ�Ļ���
	this->Implement (imgBlock, Percent) ; // �����㷨����֮

	// ������ɣ����ڰ����Ż�ȥ��ע������֮�ⲻҪ����
	RECT	rcInSel = rcBlock ;
	if (m_pCanvas->HasSelected())
	{
		m_pCanvas->LayerToCanvasRect (*m_pLayer, rcInSel) ;
		m_pCanvas->CanvasToLayerRect (rSelect, rcInSel) ; // rcBlockλ��sel�ϵ�����
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
					*(RGBTRIPLE*)pLayer = *(RGBTRIPLE*)pBlock ; // ����ͼ�㲻Ҫ�ƻ�alpha
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


