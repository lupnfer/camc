#include "stdafx.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
#include "LayerCommand.h"
#include "FColor.h"
#include "fooBit.h"
#include "FCSinglePixelProcessBase.h"
#include "math.h"
#include "CAMCDoc.h"
//=================================================================================
// ͼ���������
void  FCCmdLayerOperation::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	m_pCanvas = &canvas ;
	if (m_pLayer)
	{

		m_UndoLayer = *m_pLayer ;
		this->Implement (*m_pLayer, Percent) ;
		if (m_bSaveSelection)
		{
			m_Selection = canvas.GetSelection() ;
			canvas.__ClearSelection() ;
		}
	}
}
void  FCCmdLayerOperation::Undo()
{
	if (m_pLayer && m_pCanvas)
	{
		m_RedoLayer = *m_pLayer ;
		*m_pLayer = m_UndoLayer ;
		if (m_bSaveSelection)
			m_pCanvas->__SetRegionMember (m_Selection) ; // �ָ�ѡȡ����
	}
}
void  FCCmdLayerOperation::Redo()
{
	if (m_pLayer && m_pCanvas)
	{
		m_UndoLayer = *m_pLayer ;
		*m_pLayer = m_RedoLayer ;
		if (m_bSaveSelection)
			m_pCanvas->__ClearSelection() ;
	}
}
//=================================================================================
// ����ͼ��/stretch layer
void  FCCmdLayerStretch::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage())
		return ;

	// ���ı�ͼ���λ��
	POINT    ptLayer = img.GetGraphObjPos() ;
    img.Stretch_Smooth (m_nNewWidth, m_nNewHeight) ;
	img.SetGraphObjPos (ptLayer) ;
}
//=================================================================================
// ��תͼ��/rotate layer
void  FCCmdLayerRotate::Implement (FCObjImage & img, FCObjProgress * Percent)
{
    if (!img.IsValidImage())
        return ;

	// ���ĵ��ǲ����
	POINT    ptCenter = img.GetGraphObjPos() ;
	ptCenter.x += img.Width()/2 ; ptCenter.y += img.Height()/2 ;

	// ��ת
	img.Rotate_Smooth (m_nAngle) ;

	
	// ����ͼ�����λ��
	ptCenter.x -= img.Width()/2 ; ptCenter.y -= img.Height()/2 ;
    img.SetGraphObjPos (ptCenter) ;

}
//=================================================================================
// ˳ʱ����ת90��/clockwise rotate 90'
void  FCCmdLayerRotate90::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() < 8))
		return ;

	// ���ĵ��ǲ����
	POINT    ptCenter = img.GetGraphObjPos() ;
	ptCenter.x += img.Width()/2 ; ptCenter.y += img.Height()/2 ;

//    FCPixelRotate90     cmdRotateImg ;
//    img.SinglePixelProcessProc (cmdRotateImg, Percent) ;

	// ����ͼ�����λ��
	ptCenter.x -= img.Width()/2 ; ptCenter.y -= img.Height()/2 ;
	img.SetGraphObjPos (ptCenter) ;
}
//=================================================================================
// ��ʱ����ת90��/clockwise rotate 270'
void  FCCmdLayerRotate270::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() < 8))
		return ;

	// ���ĵ��ǲ����
	POINT    ptCenter = img.GetGraphObjPos() ;
	ptCenter.x += img.Width()/2 ; ptCenter.y += img.Height()/2 ;

//    FCPixelRotate270     cmdRotateImg ;
//    img.SinglePixelProcessProc (cmdRotateImg, Percent) ;

	// ����ͼ�����λ��
	ptCenter.x -= img.Width()/2 ; ptCenter.y -= img.Height()/2 ;
	img.SetGraphObjPos (ptCenter) ;
}
//=================================================================================
// �ƶ�ͼ��/move layer
void  FCCmdLayerMove::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	this->Redo() ;
}
void  FCCmdLayerMove::Undo ()
{
	this->Redo() ;
}
void  FCCmdLayerMove::Redo ()
{
	if (m_pLayer == NULL)
		return ;
	POINT		ptTemp = m_pLayer->GetGraphObjPos() ;
	m_pLayer->SetGraphObjPos (m_ptOld) ;
	m_ptOld = ptTemp ;
}
//=================================================================================
// ���ͼ��/add new layer to canvas
void  FCCmdLayerAdd::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
    m_pCanvas = &canvas ;
    this->Redo() ;
}
void  FCCmdLayerAdd::Undo ()
{
    m_pCanvas->RemoveLayer (m_pAddLayer, FALSE) ; // ��delete
}
void  FCCmdLayerAdd::Redo ()
{
    if (m_pCanvas && m_pAddLayer)
        m_pCanvas->AddLayer (m_pAddLayer, m_nPos) ;
}
FCCmdLayerAdd::~FCCmdLayerAdd ()
{
    // ��ʵͼ��removeʱ�͸ñ�ɾ����ֻ��Ϊ�˽�ʡ�ڴ棬����ʱ�ŵ�remove�б���
    m_pCanvas->DeleteLayerFromRemoveList (m_pAddLayer) ;
}
//=================================================================================
// ɾ��ͼ��/remove layer from canvas
void  FCCmdLayerRemove::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
    m_pCanvas = &canvas ;
    this->Redo() ;
}
void  FCCmdLayerRemove::Undo ()
{
    if (m_pCanvas && m_pRemoveLayer)
        m_pCanvas->AddLayer (m_pRemoveLayer, m_nPos) ;
}
void  FCCmdLayerRemove::Redo ()
{
    m_nPos = m_pCanvas->FindLayer(m_pRemoveLayer) ;
    m_pCanvas->RemoveLayer (m_pRemoveLayer, FALSE) ; // ��delete
}
FCCmdLayerRemove::~FCCmdLayerRemove ()
{
    // ��ʵͼ��removeʱ�͸ñ�ɾ����ֻ��Ϊ�˽�ʡ�ڴ棬����ʱ�ŵ�remove�б���
    m_pCanvas->DeleteLayerFromRemoveList (m_pRemoveLayer) ;
}
//=================================================================================
// ����ͼ������/set layer property
void  FCCmdContourLayerSetProperty::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	this->Redo() ;
}
void  FCCmdContourLayerSetProperty::Undo ()
{
	if (m_pLayer == NULL)
		return ;


	int nWidth = m_pLayer->Width();
	int nHeight = m_pLayer->Height();

	for(int y =0;y<nHeight;y++)
		for(int x= 0;x<nWidth;x++)
		{

			BYTE* bpPixel = m_pLayer->GetBits(x,y);

			if((bpPixel[0]==255)&&(bpPixel[1]==255)&&(bpPixel[2]==255))
				bpPixel[3] = 255;
		}
}
void  FCCmdContourLayerSetProperty::Redo ()
{
	if (m_pLayer == NULL)
		return ;


	int nWidth = m_pLayer->Width();
	int nHeight = m_pLayer->Height();
	for(int y =0;y<nHeight;y++)
		for(int x= 0;x<nWidth;x++)
		{
			//�õ���ǰ���أ�Ȼ��õ����ص�RGBֵ��
			//�����ǰ�����ǰ�ɫ����������alphaֵ��
			//���򲻷����仯��
			BYTE* bpPixel = m_pLayer->GetBits(x,y);
			if((bpPixel[0]==255)&&(bpPixel[1]==255)&&(bpPixel[2]==255))
				bpPixel[3] = m_nNewTransparent;
		}

}
//=================================================================================
// ����ͼ������/set layer property
void  FCCmdLayerSetProperty::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	this->Redo() ;
}
void  FCCmdLayerSetProperty::Undo ()
{
	this->Redo() ;
}
void  FCCmdLayerSetProperty::Redo ()
{
	if (m_pLayer == NULL)
		return ;
	int		nTrans = m_pLayer->GetLayerTransparent() ;
	BOOL	bVisible = m_pLayer->GetLayerVisible() ;
	BOOL	bLimited = m_pLayer->IsLayerLimited() ;
	m_pLayer->SetLayerTransparent (m_nNewTransparent) ;
	m_pLayer->SetLayerVisible (m_bVisible) ;
	m_pLayer->SetLayerLimitedFlag (m_bLimited) ;
	m_nNewTransparent = nTrans ;
	m_bVisible = bVisible ;
	m_bLimited = bLimited ;
}
//=================================================================================
// �ı�ͼ���˳��/change layer's sequence in canvas
void  FCCmdLayerExchange::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	m_pCanvas = &canvas ;
	this->Redo() ;
}
void  FCCmdLayerExchange::Undo ()
{
	this->Redo() ;
}
void  FCCmdLayerExchange::Redo ()
{
	if (m_pCanvas)
	{
		FSwap (*m_pCanvas->__GetLayerListPtr(), m_LayerList) ;
	}
}
//=================================================================================
// ͼ����б���/layer slope transform
FCCmdLayerSlope::FCCmdLayerSlope (FCObjLayer *pLayer, const POINT ptPos[4])
{
	CopyMemory (m_ptNewPos, ptPos, sizeof(POINT) * 4) ;
	m_pLayer = pLayer ;
}
void  FCCmdLayerSlope::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	FCObjImage		Old(img) ; // ����ԭͼ
	if (m_ptNewPos[0].x != m_ptNewPos[3].x) // x�������
	{
		int		nDelta = m_ptNewPos[3].x - m_ptNewPos[0].x ;
		if (img.Create (img.Width() + abs(nDelta), img.Height(), Old.ColorBits()))
		{
			UINT32		dwPitch = Old.Width() * Old.ColorBits() / 8 ;
			double		duInc = nDelta / (double)(img.Height() - 1) ;
			for (int y=0 ; y < img.Height() ; y++) // ������
			{
				int    nXNewPos = FMax(0, FMin(img.Width()-1, (int)(m_ptNewPos[0].x + y*duInc))) ;
				CopyMemory (img.GetBits(nXNewPos, y), Old.GetBits(y), dwPitch) ;
				if (Percent != NULL)
					Percent->SetProgress (y * 100 / img.Height()) ;
			}
		}
	}
	if (m_ptNewPos[0].y != m_ptNewPos[1].y) // y�������
	{
		int		nDelta = m_ptNewPos[1].y - m_ptNewPos[0].y ;
		if (img.Create (img.Width(), img.Height() + abs(nDelta), Old.ColorBits()))
		{
			double		duInc = nDelta / (double)(img.Width() - 1) ;
			DWORD		dwPitchD = img.GetPitch(),
						dwPitchS = Old.GetPitch() ;
			int			nSpan = Old.ColorBits() / 8 ;
			for (int x=0 ; x < img.Width() ; x++)
			{
				int		nYNewPos = FMax(0, FMin(img.Height()-1, (int)(m_ptNewPos[0].y + x*duInc))) ;
				BYTE	* pDest = img.GetBits (x, nYNewPos),
						* pSrc = Old.GetBits (x, 0) ;
				for (int y=0 ; y < Old.Height() ; y++, pDest-=dwPitchD, pSrc-=dwPitchS)
				{
					FCColor::CopyPixel (pDest, pSrc, nSpan) ;
				}
				if (Percent != NULL)
					Percent->SetProgress (x * 100 / img.Width()) ;
			}
		}
	}
}
//=================================================================================
// ͼ��͸������/layer lens transform��<<GDI+>>
FCCmdLayerLens::FCCmdLayerLens (FCObjLayer *pLayer, const POINT ptPos[4])
{
	CopyMemory (m_ptNewPos, ptPos, sizeof(POINT) * 4) ;
	m_pLayer = pLayer ;
}
void  FCCmdLayerLens::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if ((m_ptNewPos[0].y == m_ptNewPos[1].y) && (m_ptNewPos[0].x == m_ptNewPos[3].x))
		return ;

	FCObjImage		Old(img) ; // ����ԭͼ

	// ����
	if (m_ptNewPos[0].y != m_ptNewPos[1].y) // x����͸��
		Old.Stretch_Smooth (m_ptNewPos[1].x-m_ptNewPos[0].x, FMax(m_ptNewPos[3].y-m_ptNewPos[0].y, m_ptNewPos[2].y-m_ptNewPos[1].y)) ;
	if (m_ptNewPos[0].x != m_ptNewPos[3].x) // y����͸��
		Old.Stretch_Smooth (FMax(m_ptNewPos[1].x-m_ptNewPos[0].x, m_ptNewPos[2].x-m_ptNewPos[3].x), m_ptNewPos[3].y-m_ptNewPos[0].y) ;

	if (!img.Create (Old.Width(), Old.Height(), Old.ColorBits()))
		return ;
    if ((img.Width() == 1) || (img.Height() == 1)) // ��ֹ0��
    {
        FAssert(FALSE) ; return ;
    }

	if (m_ptNewPos[0].y != m_ptNewPos[1].y) // x����͸��
	{
		// ����ÿ��
		double      duInc = (m_ptNewPos[1].y - m_ptNewPos[0].y) / (double)(img.Width() - 1) ;
		for (int x=0 ; x < img.Width() ; x++)
		{
            int     nCurrY = FClamp ((int)(m_ptNewPos[0].y + x*duInc), 0, img.Height()-1) ;
            int     nLenY = img.Height() - 2*nCurrY ;
            if (nLenY == 0)
                continue ;

            double    fScaleY = img.Height() / (double)nLenY ;
            BYTE      * pPixel = img.GetBits (x, nCurrY) ;
            int       nPixSpan = img.ColorBits() / 8 ;
            for (int y=0 ; y < nLenY ; y++, pPixel -= img.GetPitch())
                FCColor::CopyPixel (pPixel, Old.GetBits(x,(int)(fScaleY*y)), nPixSpan) ;
		}
	}
	if (m_ptNewPos[0].x != m_ptNewPos[3].x) // y����͸��
	{
		// ����ÿ��
		double		duInc = (m_ptNewPos[3].x - m_ptNewPos[0].x) / (double)(img.Height() - 1) ;
		for (int y=0 ; y < img.Height() ; y++)
		{
            int     nCurrX = FClamp ((int)(m_ptNewPos[0].x + y*duInc), 0, img.Width()-1) ;
            int     nLenX = img.Width() - 2*nCurrX ;
            if (nLenX == 0)
                continue ;

            double    fScaleX = img.Width() / (double)nLenX ;
            BYTE      * pPixel = img.GetBits (nCurrX, y) ;
            int       nPixSpan = img.ColorBits() / 8 ;
            for (int x=0 ; x < nLenX ; x++, pPixel += nPixSpan)
                FCColor::CopyPixel (pPixel, Old.GetBits((int)(fScaleX*x),y), nPixSpan) ;
		}
	}
}
//=================================================================================
// �ü�ͼ��/crop layer
void  FCCmdLayerCrop::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if ((m_pImgMask == NULL) || (m_pImgMask->ColorBits() != 8) || !img.IsValidImage() || (img.ColorBits() < 16))
		return ;

	// ��layer����
	RECT		rcSel = {0, 0, img.Width(), img.Height()} ;
	if (::IntersectRect (&rcSel, &rcSel, &m_rcLayer) == 0)
		return ;

	POINT		ptOldImg = img.GetGraphObjPos() ;

	// get selected region
	FCObjImage		imgBlock ;
	if (!img.GetSubBlock (&imgBlock, rcSel))
		return ;
	imgBlock.ConvertTo32Bit() ;

	// ����maskλͼ
    FAssert((m_pImgMask->Width() == imgBlock.Width()) && (m_pImgMask->Height() == imgBlock.Height())) ;
//	m_pImgMask->Stretch_Smooth (imgBlock.Width(), imgBlock.Height()) ;
	imgBlock.AppendAlphaChannel (*m_pImgMask) ;
	imgBlock.SetGraphObjPos (ptOldImg.x + rcSel.left, ptOldImg.y + rcSel.top) ;
	img = imgBlock ;
}
//=================================================================================
// �ü�ͼ��/crop layer
void  __FCCmdCanvasCrop_Layer::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage())
		return ;

	// ��layer����
	RECT		rcLayer = m_rcCanvas ;
	m_pCanvas->CanvasToLayerRect (img, rcLayer) ;
	img.BoundRect (rcLayer) ;
	if (IsRectEmpty (&rcLayer))
	{
		// ����һ����ͼ��
		img.Create (RECTWIDTH(m_rcCanvas), RECTHEIGHT(m_rcCanvas), 32) ;
        FCPixelFillColor     cmdFillCr(FCColor::crWhite, 0) ;
        img.SinglePixelProcessProc (cmdFillCr, NULL) ; // layer͸��
		img.SetGraphObjPos (0, 0) ;
	}
	else
	{
		FCObjImage		imgCrop ;
		FCObjLayer		* pOldCurLayer = m_pCanvas->GetCurrentLayer() ;
		m_pCanvas->SetCurrentLayer (m_pLayer) ;
		m_pCanvas->GetSelectLayerAlphaBlock (&imgCrop) ;
		m_pCanvas->SetCurrentLayer (pOldCurLayer) ;

		// �����µ�λ�ã�ע��˳��
		m_pCanvas->LayerToCanvasRect (*m_pLayer, rcLayer) ;
		img = imgCrop ;
		img.SetGraphObjPos (rcLayer.left-m_rcCanvas.left, rcLayer.top-m_rcCanvas.top) ;
	}
}
//=================================================================================
// ����ͼ������/save layer's rect region
void  FCCmdLayerPenSaveRect::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
}
void  FCCmdLayerPenSaveRect::Undo ()
{
	if (m_pLayer == NULL)
		return ;
	if (!IsRectEmpty(&m_rcSave))
	{
		m_pLayer->GetSubBlock (&m_imgRedo, m_rcSave) ;
		m_pLayer->CoverBlock (m_imgUndo, m_rcSave.left, m_rcSave.top) ;
	}
	else
	{
		m_imgRedo = *static_cast<FCObjImage*>(m_pLayer) ;
		*static_cast<FCObjImage*>(m_pLayer) = m_imgUndo ;
	}
}
void  FCCmdLayerPenSaveRect::Redo ()
{
	if (m_pLayer == NULL)
		return ;
	if (!IsRectEmpty(&m_rcSave))
	{
		m_pLayer->GetSubBlock (&m_imgUndo, m_rcSave) ;
		m_pLayer->CoverBlock (m_imgRedo, m_rcSave.left, m_rcSave.top) ;
	}
	else
	{
		m_imgUndo = *static_cast<FCObjImage*>(m_pLayer) ;
		*static_cast<FCObjImage*>(m_pLayer) = m_imgRedo ;
	}
}
//=================================================================================
