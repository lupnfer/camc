#include "stdafx.h"
#include "ObjSelect.h"
#include "ObjImage.h"
#include "ObjCanvas.h"
#include "Win32_Func.h"
#include "StdDefine.h"
#include <vector>
#include "GraphicsGems.h"

static const int ZOOM_UNSCAN = 0x7FFF ;

extern void  aggFillPolygon_Normal_8Bit (FCObjImage & img, const POINT * ppt, int cNumPoint) ;

//===================================================================
FCObjSelect::FCObjSelect ()
{
	ResetEdge() ;
}
FCObjSelect::FCObjSelect (const FCObjSelect & sel)
{
	ResetEdge() ;
	this->operator= (sel) ;
}
void  FCObjSelect::ResetEdge ()
{
	m_ptEdge.clear() ;
	m_nZoom = ZOOM_UNSCAN ;
	m_nCurAnt = 0 ;
}
//===================================================================
// �����ڲ���m_nZoom��������ı߽�㣬����m_ptEdge��
void  FCObjSelect::RecalculateEdge ()
{
	m_ptEdge.clear() ;
	if (m_nZoom == ZOOM_UNSCAN)
		return ;

	FCObjImage    EdgeImg (Width(), Height(), 8) ; // �洢�߽��
	int           nEdgeNum = 0 ;

	// �Ȳ��ҳ�����ı߽��
	const DWORD  dwPitch = GetPitch() ;
	int          x, y ;
	for (y=0 ; y < Height() ; y++)
	{
		BYTE	* pPixel = GetBits (y),
				* pEdge = EdgeImg.GetBits (y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++, pEdge++)
			if (*pPixel == 0xFF)
			{
				// �жϸõ��Ƿ�Ϊedge����ǰ��Ϊѡ�е㣬��������δѡ��(Ϊ0)�ĵ㣩
				// ����Եѡ�е�һ��Ϊ�߽��
				if ((y == 0) || (*(pPixel + dwPitch) == 0) || // ��
					(y == Height() - 1) || (*(pPixel - dwPitch) == 0) || // ��
					(x == 0) || (*(pPixel - 1) == 0) || // ��
					(x == Width() - 1) || (*(pPixel + 1) == 0) || // ��
					(*(pPixel + 1 + dwPitch) == 0) || // ����
					(*(pPixel + 1 - dwPitch) == 0) || // ����
					(*(pPixel - 1 - dwPitch) == 0) || // ����
					(*(pPixel - 1 + dwPitch) == 0)) // ����
				{
					*pEdge = 0xFF ;
					nEdgeNum++ ;
				}
			}
	}
	if (nEdgeNum == 0) // ��
		return ;

	// ���ȣ���С��ʱ���ǡ��������㣬ֻ��ԭ�߽���һ���֣�������Ҫ��������
	if (m_nZoom <= -1)
	{
		FCObjImage   imgHash (Width()/-m_nZoom+2, Height()/-m_nZoom+2, 8) ;
		for (int y=0 ; y < Height() ; y++)
		{
			BYTE	* pPixel = EdgeImg.GetBits(y) ;
			for (int x=0 ; x < Width() ; x++, pPixel++)
				if (*pPixel == 0xFF)
				{
					POINT    ptPush = {x/-m_nZoom, y/-m_nZoom} ;
					BYTE     * pHash = imgHash.GetBits (ptPush.x, ptPush.y) ;
					if (*pHash == 0) // ��Ϊ�п��ܼ������Ӧһ����
					{
						*pHash = 0xFF ;
						m_ptEdge.push_back (ptPush) ;
					}
				}
		}
		return ;
	}

    // �����ǷŴ󲿷�
	// Ҫע�⣬�Ŵ�ʱ��>=1��Ҫ����һЩ�µ�
	int     nNewW = Width() * m_nZoom,
            nNewH = Height() * m_nZoom ;
	for (y=0 ; y < nNewH ; y++)
	{
		int		nSrcY = y / m_nZoom ;
		for (x=0 ; x < nNewW ; x++)
		{
			int		nSrcX = x / m_nZoom ;
			if (*EdgeImg.GetBits (nSrcX,nSrcY) == 0xFF)
			{
				// �����ж����ź��Ƿ��Ǳ߽��
				// �������ж���Χ����ɫ�Ƿ�һ��
				int		cr = *GetBits (nSrcX,nSrcY) ;
				if ((y == 0) || (*GetBits(nSrcX,(y-1)/m_nZoom) != cr) || // ��
					(y == nNewH - 1) || (*GetBits(nSrcX,(y+1)/m_nZoom) != cr) || // ��
					(x == 0) || (*GetBits((x-1)/m_nZoom,nSrcY) != cr) || // ��
					(x == nNewW - 1) || (*GetBits((x+1)/m_nZoom,nSrcY) != cr)) // ��
//					(*(pPixel + 1 + dwPitch) == 0) || // ����
//					(*(pPixel + 1 - dwPitch) == 0) || // ����
//					(*(pPixel - 1 - dwPitch) == 0) || // ����
//					(*(pPixel - 1 + dwPitch) == 0)) // ����
				{
					POINT		ptPush = {x,y} ;
					m_ptEdge.push_back (ptPush) ;
				}
			}
		}
	}
}
//===================================================================
// rect����Ϊcanvas�ϵ�����
BOOL  FCObjSelect::SetRect (const RECT & rect, RGN_TYPE nType)
{
//	::NormalizeRect (rect) ;
	if (::IsRectEmpty(&rect))
		return FALSE ;

    // ����������
    FCObjSelect     selNew ;
    selNew.Create (RECTWIDTH(rect), RECTHEIGHT(rect), 8) ;
    ::FillMemory (selNew.GetMemStart(), selNew.GetPitch()*selNew.Height(), 0xFF) ;
    selNew.SetGraphObjPos (rect.left, rect.top) ;

    HandleSelection (selNew, nType) ;
	return TRUE ;
}
//===================================================================
BOOL  FCObjSelect::SetElliptic (const RECT & rect, const int angle, RGN_TYPE nType)
{

	return TRUE ;
}
//===================================================================
static void  FindPolygonBoundRect (const POINT * ppt, int cNumPoint, RECT & rcBound)
{
    SetRect (&rcBound, 0x7FFFFFFF, 0x7FFFFFFF, -0x7FFFFFF, -0x7FFFFFF) ;
    for (int i=0 ; i < cNumPoint ; i++)
    {
        if (ppt[i].x < rcBound.left)    rcBound.left = ppt[i].x ;
        if (ppt[i].x > rcBound.right)   rcBound.right = ppt[i].x ;
        if (ppt[i].y < rcBound.top)     rcBound.top = ppt[i].y ;
        if (ppt[i].y > rcBound.bottom)  rcBound.bottom = ppt[i].y ;
    }
}
BOOL  FCObjSelect::SetPolygon (const POINT * ppt, int cNumPoint, RGN_TYPE nType)
{
    if ((ppt == NULL) || (cNumPoint < 3))
        return FALSE ;

    RECT      rcBound ;
    FindPolygonBoundRect (ppt, cNumPoint, rcBound) ;
    if (::IsRectEmpty(&rcBound))
        return FALSE ;

    // ����������
    FCObjSelect     selNew ;
    selNew.Create (RECTWIDTH(rcBound), RECTHEIGHT(rcBound), 8) ;
    selNew.SetGraphObjPos (rcBound.left, rcBound.top) ;

    // ��polygon
    POINT   * pOffsetPT = new POINT[cNumPoint] ;
    for (int i=0 ; i < cNumPoint ; i++)
    {
        pOffsetPT[i].x = ppt[i].x - rcBound.left ;
        pOffsetPT[i].y = ppt[i].y - rcBound.top ;
    }
    //aggFillPolygon_Normal_8Bit (selNew, pOffsetPT, cNumPoint) ;
    delete[] pOffsetPT ;

    HandleSelection (selNew, nType) ;
	return TRUE ;
}
//===================================================================
// �߽�
void  FCObjSelect::SelectionBorder (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

    POINT       ptOldPos = GetGraphObjPos() ;
    this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
    this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;

	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0xFF) ;

	// ÿ���߽�㻭mask
	std::deque<POINT>   ptList ;
	this->GetEdgePointList (ptList) ;

	// ���ѡ��������ĺ���Ψһ��֮ͬ����
	ZeroMemory (GetMemStart(), GetPitch()*Height()) ;

	for (int i=0 ; i < (int)ptList.size() ; i++)
	{
		LogicalBlend (imgMask, LOGI_OR, ptList[i].x-nPixel, ptList[i].y-nPixel) ;
	}
	this->SelectionOptimize() ;
	this->ResetEdge() ;
}
//===================================================================
// ����
void  FCObjSelect::SelectionExpand (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

	POINT		ptOldPos = GetGraphObjPos() ;
	this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
	this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;

	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0xFF) ;

	// ÿ���߽�㻭mask
	std::deque<POINT>   ptList ;
	this->GetEdgePointList (ptList) ;
	for (int i=0 ; i < (int)ptList.size() ; i++)
	{
		LogicalBlend (imgMask, LOGI_OR, ptList[i].x-nPixel, ptList[i].y-nPixel) ;
	}
	this->SelectionOptimize() ;
	this->ResetEdge() ;
}
//===================================================================
// ����
void  FCObjSelect::SelectionShrink (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

	POINT		ptOldPos = GetGraphObjPos() ;
	this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
	this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;
	
	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0) ;

	// ÿ���߽�㻭mask
	std::deque<POINT>   ptList ;
	this->GetEdgePointList (ptList) ;
	for (int i=0 ; i < (int)ptList.size() ; i++)
	{
		LogicalBlend (imgMask, LOGI_AND, ptList[i].x-nPixel, ptList[i].y-nPixel) ;
	}
	this->SelectionOptimize() ;
	this->ResetEdge() ;
}
//===================================================================
// ȥ����Χδѡȡ�ĵ�
void  FCObjSelect::SelectionOptimize ()
{
    if (!HasSelected())
        return ;

	RECT	rcBound = {Width(), Height(), 0, 0}, rcInit={Width(),Height(),0,0} ;
	for (int y=0 ; y < Height() ; y++)
	{
		BYTE    * pPixel = GetBits(y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++)
			if (*pPixel == 0xFF)
			{
				if (x < rcBound.left)    rcBound.left = x ;
				if (x > rcBound.right)   rcBound.right = x ;
				if (y < rcBound.top)     rcBound.top = y ;
				if (y > rcBound.bottom)  rcBound.bottom = y ;
			}
	}
	if (EqualRect(&rcInit, &rcBound)) // ȫ��0��δѡ��
	{
		SetEmpty() ;
		return ;
	}

	rcBound.right++ ; rcBound.bottom++ ; // ��Ϊ�ǿ�����
	int		nRight = Width()-rcBound.right, // �Ҳ���
			nBottom = Height()-rcBound.bottom ;// �²���
	if ((rcBound.left == 0) && (rcBound.top == 0) && (nRight == 0) && (nBottom == 0))
		return ; // ��Χ�����Ż���

	POINT		ptOldPos = GetGraphObjPos() ;
	EraseFrame (rcBound.left, rcBound.top, nRight, nBottom) ;
	SetGraphObjPos (ptOldPos.x+rcBound.left, ptOldPos.y+rcBound.top) ;
	this->ResetEdge() ;
}
//===================================================================
// ��ѡ����
void  FCObjSelect::SelectionInvert (const SIZE & sizeCanvas)
{
	if (!HasSelected())
		return ;

	for (int y=0 ; y < Height() ; y++)
	{
		BYTE    * pPixel = GetBits (y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++)
			*pPixel = ~(*pPixel) ;
	}
	POINT		ptPos = GetGraphObjPos() ;
	FCObjImage	imgOld(*this) ;
	this->Create (sizeCanvas.cx, sizeCanvas.cy, 8) ;
	::FillMemory (GetMemStart(), GetPitch()*Height(), 0xFF) ;
	this->CoverBlock (imgOld, ptPos.x, ptPos.y) ;

	this->SetGraphObjPos (0,0) ;
	this->SelectionOptimize() ; // ��Χ���ܻ���δѡ����
	this->ResetEdge() ;
}
//===================================================================
// ƽ������
void  FCObjSelect::SelectionSmooth (int nStep)
{

}
//===================================================================
// ������䣨ʹ�ö���ʵ�֣��������õݹ�ʵ�֣������Ѹ�������(x,y)Ϊimg�ϵ�����
FCObjImage *  FCObjSelect::SeedFill (int x, int y, int nTolerance, const FCObjImage & img)
{
	if (!img.IsInside (x, y))
		return NULL ;

	FAssert (img.ColorBits() == 32) ;

	// imgһ����32 bitɫ
	RGBQUAD			crOrigin = *(RGBQUAD*)img.GetBits(x,y) ; // ���е���ɫ
	RECT			rcBound = {x,y,x,y} ; // �����Χ����
	FCObjImage		* pFillImg = new FCObjImage(img.Width(), img.Height(), 8) ; // ��img�ȴ�maskλͼ

	std::deque<POINT>	ptUnfilled ;
	POINT			ptCurr = {x, y}, ptPush ;
	ptUnfilled.push_back (ptCurr) ;
	while (!ptUnfilled.empty ())
	{
		ptCurr = ptUnfilled.back() ; ptUnfilled.pop_back() ; // ջ������

		*pFillImg->GetBits (ptCurr.x, ptCurr.y) = 0xFF ; // ���flag
		// ���������Χ����
		if (ptCurr.x < rcBound.left)	rcBound.left = ptCurr.x ;
		if (ptCurr.x > rcBound.right)	rcBound.right = ptCurr.x ;
		if (ptCurr.y < rcBound.top)		rcBound.top = ptCurr.y ;
		if (ptCurr.y > rcBound.bottom)	rcBound.bottom = ptCurr.y ;

		// ��
		if ((ptCurr.y > 0) && (*pFillImg->GetBits (ptCurr.x, ptCurr.y - 1) == 0))
			if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x, ptCurr.y - 1), crOrigin, nTolerance))
			{
				ptPush.x = ptCurr.x ; ptPush.y = ptCurr.y - 1 ;
				ptUnfilled.push_back (ptPush) ;
			}
			// ��
			if ((ptCurr.x < img.Width() - 1) && (*pFillImg->GetBits (ptCurr.x + 1, ptCurr.y) == 0))
				if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x + 1, ptCurr.y), crOrigin, nTolerance))
				{
					ptPush.x = ptCurr.x + 1 ; ptPush.y = ptCurr.y ;
					ptUnfilled.push_back (ptPush) ;
				}
				// ��
				if ((ptCurr.y < img.Height() - 1) && (*pFillImg->GetBits (ptCurr.x, ptCurr.y + 1) == 0))
					if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x, ptCurr.y + 1), crOrigin, nTolerance))
					{
						ptPush.x = ptCurr.x ; ptPush.y = ptCurr.y + 1 ;
						ptUnfilled.push_back (ptPush) ;
					}
					// ��
					if ((ptCurr.x > 0) && (*pFillImg->GetBits (ptCurr.x - 1, ptCurr.y) == 0))
						if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x - 1, ptCurr.y), crOrigin, nTolerance))
						{
							ptPush.x = ptCurr.x - 1 ; ptPush.y = ptCurr.y ;
							ptUnfilled.push_back (ptPush) ;
						}
	}
	rcBound.right++ ; rcBound.bottom++ ; // ��Ϊ�ǿ�����

	// �ü������ʴ�С
	pFillImg->EraseFrame (rcBound.left, rcBound.top, pFillImg->Width()-rcBound.right, pFillImg->Height()-rcBound.bottom) ;
	POINT      ptImg = img.GetGraphObjPos() ;
	pFillImg->SetGraphObjPos (ptImg.x+rcBound.left, ptImg.y+rcBound.top) ; // ��canvas�ϵ�����
	
	return pFillImg ;
}
//===================================================================
// ħ����/Magic Wand
BOOL  FCObjSelect::SetMagicWand (int x, int y, int nTolerance, const FCObjImage & img, BOOL bContinuous, RGN_TYPE nType)
{
	// imgһ��Ϊ32bitɫ
	if (!img.IsValidImage() || !img.IsInside(x,y) || (img.ColorBits() != 32))
		return FALSE ;

	// ��ʼ�������
	if (bContinuous)
	{
		// ����ͨ����
		FCObjImage    * imgFill = this->SeedFill (x, y, nTolerance, img/*, nFill*/) ;
		if (imgFill == NULL)
			return FALSE ; // ���ȥ��
		imgFill->ConvertToGray();
		HandleSelection (*(FCObjSelect*)imgFill, nType) ;
	
		delete imgFill ;
	}
	else
	{
		FCObjImage	imgFill (img.Width(), img.Height(), 8) ; // Ϊ�˺�ԭ���������㣬�������ƻ�this
		RGBQUAD		crClick = *(RGBQUAD*)img.GetBits (x, y) ; // �������ɫ
		RECT		rcBound = {x,y,x,y} ; // �����Χ����
		for (int y=0 ; y < img.Height() ; y++)
		{
			RGBQUAD		* pPixel = (RGBQUAD*)img.GetBits(y) ;
			BYTE		* pRgn = imgFill.GetBits(y) ;
			for (int x=0 ; x < img.Width() ; x++, pPixel++, pRgn++)
				if (IsInTolerance (*pPixel, crClick, nTolerance))
				{
					*pRgn = 0xFF ;
					// ���������Χ����
					if (x < rcBound.left)   rcBound.left = x ;
					if (x > rcBound.right)  rcBound.right = x ;
					if (y < rcBound.top)    rcBound.top = y ;
					if (y > rcBound.bottom) rcBound.bottom = y ;
				}
				else
					*pRgn = 0 ;
		}
		rcBound.right++ ; rcBound.bottom++ ; // ��Ϊ�ǿ�����

		// �ü������ʴ�С��������λ��
		imgFill.EraseFrame (rcBound.left, rcBound.top, imgFill.Width()-rcBound.right, imgFill.Height()-rcBound.bottom) ;
		POINT      ptImg = img.GetGraphObjPos() ;
		imgFill.SetGraphObjPos (ptImg.x+rcBound.left, ptImg.y+rcBound.top) ; // ��canvas�ϵ�����
		imgFill.ConvertToGray();
		// ��ԭλͼ����
		HandleSelection (*(FCObjSelect*)&imgFill, nType) ;
	}
	this->ResetEdge() ;
	return TRUE ;
}
//===================================================================
void  FCObjSelect::SetEmpty ()
{
	if (IsValidImage())
		this->Destroy() ;
	ResetEdge() ;
}
//===================================================================
bool  FCObjSelect::PtInSelection (const POINT & ptTest) const
{
	if (!HasSelected ())
		return false ;
	else
	{
		RECT    rcSel ;
		GetRectInCanvas (&rcSel) ;
		if (!::PtInRect(&rcSel, ptTest))
			return false ;
		return (GetPixelData (ptTest.x-rcSel.left, ptTest.y-rcSel.top) == 0xFF) ;
	}
}
//===================================================================
void  FCObjSelect::DrawStep (HDC hdc, const FCObjCanvas &canvas, const RECT &rcViewCanvas)
{

	if (canvas.GetZoomScale() != m_nZoom)
	{
		m_nZoom = canvas.GetZoomScale() ;
		this->RecalculateEdge() ;
	}

	POINT		ptPos = GetGraphObjPos() ;
	canvas.MapScaledPoint (ptPos) ;
	ptPos.x+=rcViewCanvas.left;
	ptPos.y+=rcViewCanvas.top;
	for (int i=0 ; i < (int)m_ptEdge.size() ; i++)
	{
		POINT    ptDraw = {ptPos.x + m_ptEdge[i].x, ptPos.y + m_ptEdge[i].y} ;
		if (!::PtInRect(&rcViewCanvas, ptDraw))
			continue ;

		if (g_ant_data[m_nCurAnt][m_ptEdge[i].y % 8] & g_ant_xmask[m_ptEdge[i].x % 8])
			FLib_SetPixel (hdc, ptDraw.x, ptDraw.y, RGB(255,255,255)) ;
		else
			FLib_SetPixel (hdc, ptDraw.x, ptDraw.y, RGB(0,0,0)) ;
	}
	m_nCurAnt = ++m_nCurAnt % 8 ;
	
}
//================================================================================
// ����������
void  FCObjSelect::HandleSelection (const FCObjSelect & selectionNew, RGN_TYPE nType)
{
	if (!HasSelected() || (nType == RGN_CREATE)) // �½�
	{
		SetEmpty ();
		*(FCObjImage*)this = *(FCObjImage*)&selectionNew ;
		SetGraphObjPos(selectionNew.GetGraphObjPos());
	}
	else if (nType == RGN_ADD)
		AddSelection (selectionNew) ;
	else if (nType == RGN_SUB)
		SubSelection (selectionNew) ;
	this->ResetEdge() ;
}
//================================================================================
// �ӵ���ǰ����
void  FCObjSelect::AddSelection (const FCObjSelect & sel)
{
	if (&sel == this)
		return ;
	if (!(HasSelected() && sel.HasSelected()))
		return ;

	// ��ǰѡȡ��canvas�ϵ�λ��
	RECT      rcSel, rcCurr ;
	GetRectInCanvas (&rcCurr) ;
	sel.GetRectInCanvas (&rcSel) ;
	
	// �ϲ�
	if (!::IsRectInRect (rcCurr, rcSel))
	{
		// ����������½�λͼ
		RECT			rcDest ;
		::UnionRect (&rcDest, &rcSel, &rcCurr) ;

		FCObjImage		imgOld(*this) ;
		imgOld.ConvertToGray();
		Create (RECTWIDTH(rcDest), RECTHEIGHT(rcDest), 8) ;
		CoverBlock (imgOld, rcCurr.left-rcDest.left, rcCurr.top-rcDest.top) ; // ����ԭͼ
		SetGraphObjPos (rcDest.left, rcDest.top) ;
		rcCurr = rcDest ; // ������Ĺ����õ�
	}

	// �����sel������ȥ�����汣֤��selһ����this�ڲ�
	int     nStartX = rcSel.left - rcCurr.left,
		nStartY = rcSel.top - rcCurr.top ;
	LogicalBlend (sel, LOGI_SEL_ADD, nStartX, nStartY) ;
}
//================================================================================
// �ӵ�ǰ���������
void  FCObjSelect::SubSelection (const FCObjSelect & sel)
{
	if (&sel == this)
		return ;
	if (!(HasSelected() && sel.HasSelected()))
		return ;

	// ��ǰѡȡ��canvas�ϵ�λ��
	RECT      rcSel, rcCurr ;
	GetRectInCanvas (&rcCurr) ;
	sel.GetRectInCanvas (&rcSel) ;

	int     nStartX = rcSel.left - rcCurr.left,
		nStartY = rcSel.top - rcCurr.top ;
	LogicalBlend (sel, LOGI_SEL_SUB, nStartX, nStartY) ;
	//    SelectionOptimize() ;
}
//================================================================================
// ��ñ߽��list
void  FCObjSelect::GetEdgePointList (std::deque<POINT> & ptList) const
{
	ptList.clear() ;

	// ���ҳ�����ı߽��
	DWORD		dwPitch = GetPitch() ;
	for (int y=0 ; y < Height() ; y++)
	{
		BYTE	* pPixel = GetBits (y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++)
			if (*pPixel == 0xFF)
			{
				// �жϸõ��Ƿ�Ϊedge����ǰ��Ϊѡ�е㣬��������δѡ��(Ϊ0)�ĵ㣩
				// ����Եѡ�е�һ��Ϊ�߽��
				if ((y == 0) || (*(pPixel + dwPitch) == 0) || // ��
					(y == Height() - 1) || (*(pPixel - dwPitch) == 0) || // ��
					(x == 0) || (*(pPixel - 1) == 0) || // ��
					(x == Width() - 1) || (*(pPixel + 1) == 0) || // ��
					(*(pPixel + 1 + dwPitch) == 0) || // ����
					(*(pPixel + 1 - dwPitch) == 0) || // ����
					(*(pPixel - 1 - dwPitch) == 0) || // ����
					(*(pPixel - 1 + dwPitch) == 0)) // ����
				{
					POINT   ptPush = {x, y} ;
					ptList.push_back (ptPush) ;
				}
			}
	}
}
//================================================================================
bool FCObjSelect::PtInEdge(int x,int y)
{
	return false;
}