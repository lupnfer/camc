#include "StdAfx.h"
#include ".\cmdimgeffect.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
#include "CmdImgSetAlpha.h"
#include "CAMCDoc.h"
//=================================================================================
void  CCmdImgEffect::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	
	//m_pLayer = canvas.GetCurrentLayer() ;
//////////////////////////////////////////////////////////////////////////
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	m_pLayer = canvas.GetLayer(i);


	if ((m_pLayer == NULL) || !m_pLayer->IsValidImage())
	{
		m_pLayer = NULL ;
		FAssert(FALSE) ; return ;
	}
	m_pCanvas = &canvas ;

/*
	BOOL		bHoldAlpha = m_pLayer->IsLayerLimited() ;
	if (dynamic_cast<CCmdImgSetAlpha*>(this) != NULL)
		bHoldAlpha = FALSE ; // �������ʱ����������
*/
	if (Percent != NULL)
		Percent->ResetProgress () ; // percent set to 0%
	if (!canvas.HasSelected()) // ��ͼ
	{
		m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;
		this->Implement (*m_pLayer, Percent) ;

		// ����ͼ�����⴦�����ı�alphaͨ��ֵ
	//	if (bHoldAlpha)
	//		m_pLayer->CopyAlphaChannel (m_Undo) ;
	}
	else
	{
		// ���ڳ�һ��rect����
		RECT		rcSel ;
		canvas.GetSelectLayerRect (*m_pLayer, &rcSel) ;
		if (IsRectEmpty (&rcSel))
		{
			m_pLayer = NULL ; m_pCanvas = NULL ;
			FAssert(FALSE) ; return ;
		}

		// ����ԭ����
		m_pLayer->GetSubBlock (&m_Undo, rcSel) ;

		// ������֮��alpha��0���������Ҫ�������Ӱ���硰box blur������Ҫalpha������㷨
		int				x, y ;
		FCObjImage		block(m_Undo), imgSelection ;
		canvas.GetSelectLayerMask (*m_pLayer, &imgSelection) ;
		for (y=0 ; y < block.Height() ; y++)
		{
			BYTE	* pBlock = block.GetBits(y),
				* pSel = imgSelection.GetBits(y) ;
			for (x=0 ; x < block.Width() ; x++, pBlock+=4, pSel++)
				if (*pSel == 0)
					pBlock[3] = 0 ;
		}

		// �����㷨����֮
		this->Implement (block, Percent) ;

		// ���ڣ������Ż�ȥ��ע������֮�ⲻҪ����
		for (y=0 ; y < imgSelection.Height() ; y++)
		{
			BYTE	* pSel = imgSelection.GetBits(y),
				* pBlock = block.GetBits(y),
				* pWrite = m_pLayer->GetBits (rcSel.left, rcSel.top + y) ;
			for (x=0 ; x < imgSelection.Width() ; x++)
			{
				if (*pSel != 0)
				{
				/*	
					if (bHoldAlpha)
					{
						if (pWrite[3] != 0)
							FCColor::AlphaBlendPixel ((RGBQUAD*)pWrite, (RGBQUAD*)pBlock) ;
					}
					else
					*/
						*(RGBQUAD*)pWrite = *(RGBQUAD*)pBlock ;
				}
				pSel++ ; pBlock+=4 ; pWrite+=4 ;
			}
		}
	}
	if (Percent != NULL)
		Percent->SetProgress (100) ; // set 100%

}

//=================================================================================
void  CCmdImgEffect::Undo ()
{
	
	if (!m_Undo.IsValidImage() || (m_pCanvas == NULL))
		return ;

	if (!m_pCanvas->HasSelected()) // whole image
	{
		m_Redo = * static_cast<FCObjImage *>(m_pLayer) ;
		* static_cast<FCObjImage *>(m_pLayer) = m_Undo ;
	}
	else
	{
		RECT	rcBlock ;
		m_pCanvas->GetSelectLayerRect (*m_pLayer, &rcBlock) ;
		rcBlock.right = rcBlock.left + m_Undo.Width() ;
		rcBlock.bottom = rcBlock.top + m_Undo.Height() ;
		m_pLayer->GetSubBlock (&m_Redo, rcBlock) ;
		m_pLayer->CoverBlock (m_Undo, rcBlock.left, rcBlock.top) ;
	}
	m_Undo.Destroy () ;

}
void  CCmdImgEffect::Redo ()
{
	
	if (!m_Redo.IsValidImage() || (m_pCanvas == NULL))
		return ;

	if (!m_pCanvas->HasSelected()) // whole image
	{
		m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;
		* static_cast<FCObjImage *>(m_pLayer) = m_Redo ;
	}
	else
	{
		RECT	rcBlock ;
		m_pCanvas->GetSelectLayerRect (*m_pLayer, &rcBlock) ;
		rcBlock.right = rcBlock.left + m_Redo.Width() ;
		rcBlock.bottom = rcBlock.top + m_Redo.Height() ;
		m_pLayer->GetSubBlock (&m_Undo, rcBlock) ;
		m_pLayer->CoverBlock (m_Redo, rcBlock.left, rcBlock.top) ;
	}
	m_Redo.Destroy () ;
	
}