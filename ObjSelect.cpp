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
// 根据内部的m_nZoom计算区域的边界点，放于m_ptEdge中
void  FCObjSelect::RecalculateEdge ()
{
	m_ptEdge.clear() ;
	if (m_nZoom == ZOOM_UNSCAN)
		return ;

	FCObjImage    EdgeImg (Width(), Height(), 8) ; // 存储边界点
	int           nEdgeNum = 0 ;

	// 先查找出区域的边界点
	const DWORD  dwPitch = GetPitch() ;
	int          x, y ;
	for (y=0 ; y < Height() ; y++)
	{
		BYTE	* pPixel = GetBits (y),
				* pEdge = EdgeImg.GetBits (y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++, pEdge++)
			if (*pPixel == 0xFF)
			{
				// 判断该点是否为edge（当前点为选中点，且四周有未选中(为0)的点）
				// 另：边缘选中点一定为边界点
				if ((y == 0) || (*(pPixel + dwPitch) == 0) || // 上
					(y == Height() - 1) || (*(pPixel - dwPitch) == 0) || // 下
					(x == 0) || (*(pPixel - 1) == 0) || // 左
					(x == Width() - 1) || (*(pPixel + 1) == 0) || // 右
					(*(pPixel + 1 + dwPitch) == 0) || // 右上
					(*(pPixel + 1 - dwPitch) == 0) || // 右下
					(*(pPixel - 1 - dwPitch) == 0) || // 左下
					(*(pPixel - 1 + dwPitch) == 0)) // 左上
				{
					*pEdge = 0xFF ;
					nEdgeNum++ ;
				}
			}
	}
	if (nEdgeNum == 0) // 靠
		return ;

	// 首先，缩小的时候是“除”运算，只是原边界点的一部分，并不需要特殊运算
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
					if (*pHash == 0) // 因为有可能几个点对应一个点
					{
						*pHash = 0xFF ;
						m_ptEdge.push_back (ptPush) ;
					}
				}
		}
		return ;
	}

    // 下面是放大部分
	// 要注意，放大时候（>=1）要插入一些新点
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
				// 继续判断缩放后是否还是边界点
				// 依据是判断周围点颜色是否一样
				int		cr = *GetBits (nSrcX,nSrcY) ;
				if ((y == 0) || (*GetBits(nSrcX,(y-1)/m_nZoom) != cr) || // 上
					(y == nNewH - 1) || (*GetBits(nSrcX,(y+1)/m_nZoom) != cr) || // 下
					(x == 0) || (*GetBits((x-1)/m_nZoom,nSrcY) != cr) || // 左
					(x == nNewW - 1) || (*GetBits((x+1)/m_nZoom,nSrcY) != cr)) // 右
//					(*(pPixel + 1 + dwPitch) == 0) || // 右上
//					(*(pPixel + 1 - dwPitch) == 0) || // 右下
//					(*(pPixel - 1 - dwPitch) == 0) || // 左下
//					(*(pPixel - 1 + dwPitch) == 0)) // 左上
				{
					POINT		ptPush = {x,y} ;
					m_ptEdge.push_back (ptPush) ;
				}
			}
		}
	}
}
//===================================================================
// rect坐标为canvas上的坐标
BOOL  FCObjSelect::SetRect (const RECT & rect, RGN_TYPE nType)
{
//	::NormalizeRect (rect) ;
	if (::IsRectEmpty(&rect))
		return FALSE ;

    // 创建新区域
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

    // 创建新区域
    FCObjSelect     selNew ;
    selNew.Create (RECTWIDTH(rcBound), RECTHEIGHT(rcBound), 8) ;
    selNew.SetGraphObjPos (rcBound.left, rcBound.top) ;

    // 画polygon
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
// 边界
void  FCObjSelect::SelectionBorder (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

    POINT       ptOldPos = GetGraphObjPos() ;
    this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
    this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;

	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0xFF) ;

	// 每个边界点画mask
	std::deque<POINT>   ptList ;
	this->GetEdgePointList (ptList) ;

	// 清空选择（与下面的函数唯一不同之处）
	ZeroMemory (GetMemStart(), GetPitch()*Height()) ;

	for (int i=0 ; i < (int)ptList.size() ; i++)
	{
		LogicalBlend (imgMask, LOGI_OR, ptList[i].x-nPixel, ptList[i].y-nPixel) ;
	}
	this->SelectionOptimize() ;
	this->ResetEdge() ;
}
//===================================================================
// 扩张
void  FCObjSelect::SelectionExpand (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

	POINT		ptOldPos = GetGraphObjPos() ;
	this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
	this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;

	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0xFF) ;

	// 每个边界点画mask
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
// 收缩
void  FCObjSelect::SelectionShrink (int nPixel)
{
	if (!HasSelected() || (nPixel <= 0))
		return ;

	POINT		ptOldPos = GetGraphObjPos() ;
	this->ExpandFrame (FALSE, nPixel, nPixel, nPixel, nPixel) ;
	this->SetGraphObjPos (ptOldPos.x-nPixel, ptOldPos.y-nPixel) ;
	
	FCObjImage    imgMask (2*nPixel+1, 2*nPixel+1, 8) ;
	FillMemory (imgMask.GetMemStart(), imgMask.GetPitch()*imgMask.Height(), 0) ;

	// 每个边界点画mask
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
// 去掉周围未选取的点
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
	if (EqualRect(&rcInit, &rcBound)) // 全是0（未选）
	{
		SetEmpty() ;
		return ;
	}

	rcBound.right++ ; rcBound.bottom++ ; // 因为是开区域
	int		nRight = Width()-rcBound.right, // 右擦除
			nBottom = Height()-rcBound.bottom ;// 下擦除
	if ((rcBound.left == 0) && (rcBound.top == 0) && (nRight == 0) && (nBottom == 0))
		return ; // 周围不能优化了

	POINT		ptOldPos = GetGraphObjPos() ;
	EraseFrame (rcBound.left, rcBound.top, nRight, nBottom) ;
	SetGraphObjPos (ptOldPos.x+rcBound.left, ptOldPos.y+rcBound.top) ;
	this->ResetEdge() ;
}
//===================================================================
// 反选区域
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
	this->SelectionOptimize() ; // 周围可能会有未选区域
	this->ResetEdge() ;
}
//===================================================================
// 平滑区域
void  FCObjSelect::SelectionSmooth (int nStep)
{

}
//===================================================================
// 种子填充（使用队列实现，绝不可用递归实现，否则会迅速溢出）(x,y)为img上的坐标
FCObjImage *  FCObjSelect::SeedFill (int x, int y, int nTolerance, const FCObjImage & img)
{
	if (!img.IsInside (x, y))
		return NULL ;

	FAssert (img.ColorBits() == 32) ;

	// img一定是32 bit色
	RGBQUAD			crOrigin = *(RGBQUAD*)img.GetBits(x,y) ; // 点中点颜色
	RECT			rcBound = {x,y,x,y} ; // 最大外围矩形
	FCObjImage		* pFillImg = new FCObjImage(img.Width(), img.Height(), 8) ; // 与img等大mask位图

	std::deque<POINT>	ptUnfilled ;
	POINT			ptCurr = {x, y}, ptPush ;
	ptUnfilled.push_back (ptCurr) ;
	while (!ptUnfilled.empty ())
	{
		ptCurr = ptUnfilled.back() ; ptUnfilled.pop_back() ; // 栈顶弹出

		*pFillImg->GetBits (ptCurr.x, ptCurr.y) = 0xFF ; // 填充flag
		// 计算最大外围矩形
		if (ptCurr.x < rcBound.left)	rcBound.left = ptCurr.x ;
		if (ptCurr.x > rcBound.right)	rcBound.right = ptCurr.x ;
		if (ptCurr.y < rcBound.top)		rcBound.top = ptCurr.y ;
		if (ptCurr.y > rcBound.bottom)	rcBound.bottom = ptCurr.y ;

		// 上
		if ((ptCurr.y > 0) && (*pFillImg->GetBits (ptCurr.x, ptCurr.y - 1) == 0))
			if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x, ptCurr.y - 1), crOrigin, nTolerance))
			{
				ptPush.x = ptCurr.x ; ptPush.y = ptCurr.y - 1 ;
				ptUnfilled.push_back (ptPush) ;
			}
			// 右
			if ((ptCurr.x < img.Width() - 1) && (*pFillImg->GetBits (ptCurr.x + 1, ptCurr.y) == 0))
				if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x + 1, ptCurr.y), crOrigin, nTolerance))
				{
					ptPush.x = ptCurr.x + 1 ; ptPush.y = ptCurr.y ;
					ptUnfilled.push_back (ptPush) ;
				}
				// 下
				if ((ptCurr.y < img.Height() - 1) && (*pFillImg->GetBits (ptCurr.x, ptCurr.y + 1) == 0))
					if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x, ptCurr.y + 1), crOrigin, nTolerance))
					{
						ptPush.x = ptCurr.x ; ptPush.y = ptCurr.y + 1 ;
						ptUnfilled.push_back (ptPush) ;
					}
					// 左
					if ((ptCurr.x > 0) && (*pFillImg->GetBits (ptCurr.x - 1, ptCurr.y) == 0))
						if (IsInTolerance (*(RGBQUAD*)img.GetBits (ptCurr.x - 1, ptCurr.y), crOrigin, nTolerance))
						{
							ptPush.x = ptCurr.x - 1 ; ptPush.y = ptCurr.y ;
							ptUnfilled.push_back (ptPush) ;
						}
	}
	rcBound.right++ ; rcBound.bottom++ ; // 因为是开区域

	// 裁剪到合适大小
	pFillImg->EraseFrame (rcBound.left, rcBound.top, pFillImg->Width()-rcBound.right, pFillImg->Height()-rcBound.bottom) ;
	POINT      ptImg = img.GetGraphObjPos() ;
	pFillImg->SetGraphObjPos (ptImg.x+rcBound.left, ptImg.y+rcBound.top) ; // 在canvas上的坐标
	
	return pFillImg ;
}
//===================================================================
// 魔术棒/Magic Wand
BOOL  FCObjSelect::SetMagicWand (int x, int y, int nTolerance, const FCObjImage & img, BOOL bContinuous, RGN_TYPE nType)
{
	// img一定为32bit色
	if (!img.IsValidImage() || !img.IsInside(x,y) || (img.ColorBits() != 32))
		return FALSE ;

	// 开始种子填充
	if (bContinuous)
	{
		// 仅连通区域
		FCObjImage    * imgFill = this->SeedFill (x, y, nTolerance, img/*, nFill*/) ;
		if (imgFill == NULL)
			return FALSE ; // 点出去啦
		imgFill->ConvertToGray();
		HandleSelection (*(FCObjSelect*)imgFill, nType) ;
	
		delete imgFill ;
	}
	else
	{
		FCObjImage	imgFill (img.Width(), img.Height(), 8) ; // 为了和原区域做运算，还不能破坏this
		RGBQUAD		crClick = *(RGBQUAD*)img.GetBits (x, y) ; // 点击点颜色
		RECT		rcBound = {x,y,x,y} ; // 最大外围矩形
		for (int y=0 ; y < img.Height() ; y++)
		{
			RGBQUAD		* pPixel = (RGBQUAD*)img.GetBits(y) ;
			BYTE		* pRgn = imgFill.GetBits(y) ;
			for (int x=0 ; x < img.Width() ; x++, pPixel++, pRgn++)
				if (IsInTolerance (*pPixel, crClick, nTolerance))
				{
					*pRgn = 0xFF ;
					// 计算最大外围矩形
					if (x < rcBound.left)   rcBound.left = x ;
					if (x > rcBound.right)  rcBound.right = x ;
					if (y < rcBound.top)    rcBound.top = y ;
					if (y > rcBound.bottom) rcBound.bottom = y ;
				}
				else
					*pRgn = 0 ;
		}
		rcBound.right++ ; rcBound.bottom++ ; // 因为是开区域

		// 裁剪到合适大小并计算新位置
		imgFill.EraseFrame (rcBound.left, rcBound.top, imgFill.Width()-rcBound.right, imgFill.Height()-rcBound.bottom) ;
		POINT      ptImg = img.GetGraphObjPos() ;
		imgFill.SetGraphObjPos (ptImg.x+rcBound.left, ptImg.y+rcBound.top) ; // 在canvas上的坐标
		imgFill.ConvertToGray();
		// 与原位图操作
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
// 处理新区域
void  FCObjSelect::HandleSelection (const FCObjSelect & selectionNew, RGN_TYPE nType)
{
	if (!HasSelected() || (nType == RGN_CREATE)) // 新建
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
// 加到当前区域
void  FCObjSelect::AddSelection (const FCObjSelect & sel)
{
	if (&sel == this)
		return ;
	if (!(HasSelected() && sel.HasSelected()))
		return ;

	// 当前选取在canvas上的位置
	RECT      rcSel, rcCurr ;
	GetRectInCanvas (&rcCurr) ;
	sel.GetRectInCanvas (&rcSel) ;
	
	// 合并
	if (!::IsRectInRect (rcCurr, rcSel))
	{
		// 这种情况得新建位图
		RECT			rcDest ;
		::UnionRect (&rcDest, &rcSel, &rcCurr) ;

		FCObjImage		imgOld(*this) ;
		imgOld.ConvertToGray();
		Create (RECTWIDTH(rcDest), RECTHEIGHT(rcDest), 8) ;
		CoverBlock (imgOld, rcCurr.left-rcDest.left, rcCurr.top-rcDest.top) ; // 覆盖原图
		SetGraphObjPos (rcDest.left, rcDest.top) ;
		rcCurr = rcDest ; // 给下面的过程用的
	}

	// 下面把sel覆盖上去，上面保证了sel一定在this内部
	int     nStartX = rcSel.left - rcCurr.left,
		nStartY = rcSel.top - rcCurr.top ;
	LogicalBlend (sel, LOGI_SEL_ADD, nStartX, nStartY) ;
}
//================================================================================
// 从当前区域中清除
void  FCObjSelect::SubSelection (const FCObjSelect & sel)
{
	if (&sel == this)
		return ;
	if (!(HasSelected() && sel.HasSelected()))
		return ;

	// 当前选取在canvas上的位置
	RECT      rcSel, rcCurr ;
	GetRectInCanvas (&rcCurr) ;
	sel.GetRectInCanvas (&rcSel) ;

	int     nStartX = rcSel.left - rcCurr.left,
		nStartY = rcSel.top - rcCurr.top ;
	LogicalBlend (sel, LOGI_SEL_SUB, nStartX, nStartY) ;
	//    SelectionOptimize() ;
}
//================================================================================
// 获得边界点list
void  FCObjSelect::GetEdgePointList (std::deque<POINT> & ptList) const
{
	ptList.clear() ;

	// 查找出区域的边界点
	DWORD		dwPitch = GetPitch() ;
	for (int y=0 ; y < Height() ; y++)
	{
		BYTE	* pPixel = GetBits (y) ;
		for (int x=0 ; x < Width() ; x++, pPixel++)
			if (*pPixel == 0xFF)
			{
				// 判断该点是否为edge（当前点为选中点，且四周有未选中(为0)的点）
				// 另：边缘选中点一定为边界点
				if ((y == 0) || (*(pPixel + dwPitch) == 0) || // 上
					(y == Height() - 1) || (*(pPixel - dwPitch) == 0) || // 下
					(x == 0) || (*(pPixel - 1) == 0) || // 左
					(x == Width() - 1) || (*(pPixel + 1) == 0) || // 右
					(*(pPixel + 1 + dwPitch) == 0) || // 右上
					(*(pPixel + 1 - dwPitch) == 0) || // 右下
					(*(pPixel - 1 - dwPitch) == 0) || // 左下
					(*(pPixel - 1 + dwPitch) == 0)) // 左上
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