#include "stdafx.h"
#include "ObjCanvas.h"
#include "ObjLayer.h"
#include "FCSinglePixelProcessBase.h"
#include "FColor.h"
#include "Win32_Func.h"
#include "FCCmdImgCmdComposite.h"
#include "LayerCommand.h"
#include "CAMCDoc.h"

const RGBQUAD    g_crBackground = {0xC0,0xC0,0xC0} ; // view canvas外颜色
const RGBQUAD    g_crThumbBack = {251, 203, 191} ; // thumb外颜色

//===================================================================
FCObjCanvas::~FCObjCanvas ()
{
    this->ClearUndoRedoList () ; // 不能和下面的顺序颠倒，因为undo对象中可能保存着指针

   	// layers
    while (!m_LayerList.empty())
    {
        FCObjLayer   * node = m_LayerList.back () ;
        m_LayerList.pop_back () ;
        delete node ;
    }

    // removed layers
    while (!m_RemovedLayerList.empty())
    {
        FCObjLayer   * node = m_RemovedLayerList.back () ;
        m_RemovedLayerList.pop_back () ;
        delete node ;
    }
    m_pCurrentLayer = NULL ;
}
//===================================================================
void  FCObjCanvas::BoundRect (RECT & rect) const
{
	RECT	rcCanvas = {0, 0, m_sizeCanvas.cx, m_sizeCanvas.cy} ;
	::IntersectRect (&rect, &rcCanvas, &rect) ;
}
//===================================================================
void  FCObjCanvas::BoundRect (RECT_F & rect) const
{
    rect.left = FMax ((double)0.0, rect.left) ;
	rect.top = FMax ((double)0.0, rect.top) ;
	rect.right = FMin ((double)m_sizeCanvas.cx, rect.right) ;
	rect.bottom = FMin ((double)m_sizeCanvas.cy, rect.bottom) ;
	::NormalizeRect_F (rect) ;
}
//===================================================================
void  FCObjCanvas::SetUndoLevel (int nLevel)
{
	nLevel = abs(nLevel) ;
	int		nRemove = (int)m_UndoList.size() - nLevel ;
	if (nRemove > 0) // 清空一部分undo队列
    {
		while (nRemove--)
		{
			FCCmdArtPrider   * pNode = m_UndoList.front() ;
			m_UndoList.pop_front () ;
			delete pNode ;
		}
    }
	m_nUndoLevel = nLevel ;
}
//===================================================================
// 加进去了就不要自己释放，canvas析构时会delete
// 也有可能被remove了，在command对象的析构时delete
// nIndex == -1，加入到末尾
void  FCObjCanvas::AddLayer (FCObjLayer * pNewLay, int nIndex)
{
	if (pNewLay != NULL)
	{
        FAssert (pNewLay->ColorBits() == 32) ;

		// 先保证队列中没有此layer
		std::deque<FCObjLayer *>::iterator    pt ;
		pt = find (m_LayerList.begin(), m_LayerList.end(), pNewLay) ;
		if (pt == m_LayerList.end()) // 不在队列中
		{
			// 加入到LayerList中
			if (nIndex == -1)
				m_LayerList.push_back (pNewLay) ;
			else
				m_LayerList.insert (m_LayerList.begin() + nIndex, pNewLay) ;
			m_pCurrentLayer = pNewLay ;
			m_pCurrentLayer->SetParentCanvas (this) ; // 关联

			// 从Removed LayerList中移除
			int		nRemovePos = this->FindRemovedLayer (pNewLay) ;
			if (nRemovePos != -1)
				m_RemovedLayerList.erase (m_RemovedLayerList.begin() + nRemovePos) ;
		}
	}
}
//===================================================================
void  FCObjCanvas::RemoveLayer (const FCObjLayer * pLayer, BOOL bDelLayer)
{
    if (pLayer == NULL)
        return ;

    int     nIndex = FindLayer (pLayer) ;
	if (IsValidLayerIndex(nIndex) && (GetLayerNumber() > 1))
	{
		FCObjLayer    * pLayer = GetLayer(nIndex) ;
		m_LayerList.erase (m_LayerList.begin() + nIndex) ;
		// 更新当前layer
		if (m_pCurrentLayer == pLayer)
		{
			int		nCurr = FMin (nIndex, GetLayerNumber() - 1) ;
			this->SetCurrentLayer (nCurr) ;
		}

		if (bDelLayer)
			delete pLayer ;
		else
			m_RemovedLayerList.push_back (pLayer) ;
	}
}
//===================================================================
void  FCObjCanvas::DeleteLayerFromRemoveList (FCObjLayer * pLayer)
{
	if (pLayer != NULL)
	{
		int		nIndex = this->FindRemovedLayer(pLayer) ;
		if (nIndex != -1)
		{
			m_RemovedLayerList.erase (m_RemovedLayerList.begin() + nIndex) ;
			delete pLayer ;
		}
	}
}
//===================================================================
int  FCObjCanvas::FindLayer (const FCObjLayer * pNewLay) const
{
	for (int i=0 ; i < GetLayerNumber() ; i++)
	{
		if (pNewLay == m_LayerList[i])
			return i ;
	}
	return -1 ;
}
//===================================================================
int  FCObjCanvas::FindRemovedLayer (const FCObjLayer * pNewLay) const
{
	for (int i=0 ; i < (int)m_RemovedLayerList.size() ; i++)
	{
		if (pNewLay == m_RemovedLayerList[i])
			return i ;
	}
	return -1 ;
}
//===================================================================
// 要注意rcView, rcCanvas, rcClient之间的关系
// 本临时对象负责画边框
class __FCPixelDrawSingleFrame : public FCSinglePixelProcessBase
{
public :
	__FCPixelDrawSingleFrame (const RECT & rcRect) : m_rcRect(rcRect) {}
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel)
	{
		RGBQUAD     crColor = g_crBackground ;
		if ((x == m_rcRect.left) || (x == m_rcRect.right))
			if ((y >= m_rcRect.top) && (y <= m_rcRect.bottom))
				crColor = FCColor::crBlack ;
		if ((y == m_rcRect.top) || (y == m_rcRect.bottom))
			if ((x >= m_rcRect.left) && (x <= m_rcRect.right))
				crColor = FCColor::crBlack ;
		FCColor::CopyPixel (pPixel, &crColor, 4) ;
	}
protected :
	RECT        m_rcRect ;
} ;
void  FCObjCanvas::MakeViewImage (int nScrollX, int nScrollY, const SIZE & sizeView,
                                  const FCObjImage & imgBack, FCObjImage * pImgView) const
{
    if (IsCanvasEmpty() || (sizeView.cx <= 0) || (sizeView.cy <= 0) || (pImgView == NULL))
        return ;

    if (!pImgView->Create (sizeView.cx, sizeView.cy, 32))
    {
        FAssert (FALSE) ;
        return ;
    }

    const SIZE_F   sizeScaled = GetCanvasScaledDimension_F() ;
    RECT           rcView = {0, 0, sizeView.cx, sizeView.cy} ; // canvas画到窗口位图上的区域
    RECT_F         rcCanvas ; // canvas上对应的区域

    // 计算显示的区域及canvas区域（小于时保证居中）
    // 大于时canvas填满整个窗口
    if ((int)sizeScaled.cx > sizeView.cx) // X coordinate
    {
        rcCanvas.left = nScrollX ;
        rcCanvas.right = nScrollX + sizeView.cx ;
    }
    else
    {
        rcCanvas.left = 0.0 ;
        rcCanvas.right = sizeScaled.cx ;
        rcView.left = (sizeView.cx - (int)sizeScaled.cx) / 2 ; // 居中显示
        rcView.right = rcView.left + (int)sizeScaled.cx ;
    }
    if ((int)sizeScaled.cy > sizeView.cy) // Y coordinate
    {
        rcCanvas.top = nScrollY ;
        rcCanvas.bottom = nScrollY + sizeView.cy ;
    }
    else
    {
        rcCanvas.top = 0.0 ;
        rcCanvas.bottom = sizeScaled.cy ;
        rcView.top = (sizeView.cy - (int)sizeScaled.cy) / 2 ; // 居中显示
        rcView.bottom = rcView.top + (int)sizeScaled.cy ;
    }
    // 计算完毕，但这时候rcCanvas中的坐标还是经过缩放的，映射到真实坐标
    this->MapRealRect (rcCanvas) ;

    // 1.填充画布颜色，即图像以外的区域（仅当图像X,Y轴居中显示时）
    // 2.画canvas背景
    const RECT    rcClient = {0, 0, sizeView.cx, sizeView.cy} ;
    const int     nBkPosX = -nScrollX % imgBack.Width(),
                  nBkPosY = -nScrollY % imgBack.Height() ;
    if (!EqualRect(&rcClient, &rcView))
    {
        // 加一个边框和画背景色一起做了
		RECT     rcFrame = rcView ;
		rcFrame.left-- ; rcFrame.top-- ;
		__FCPixelDrawSingleFrame      cmdDrawFr(rcFrame) ;
		pImgView->SinglePixelProcessProc (cmdDrawFr) ;

        FCObjImage     imgCanvasBk (RECTWIDTH(rcView), RECTHEIGHT(rcView), 32) ;
        imgCanvasBk.TileBlock (imgBack, nBkPosX, nBkPosY) ;
        pImgView->CoverBlock (imgCanvasBk, rcView.left, rcView.top) ;
    }
    else
    {
        pImgView->TileBlock (imgBack, nBkPosX, nBkPosY) ;
    }

    // ok，现在绘制rcView内部的各个图层
    for (int i=0 ; i < GetLayerNumber() ; i++)
    {
        FCObjLayer      * pLayer = this->GetLayer(i) ;
        if ((pLayer == NULL) || !pLayer->GetLayerVisible())
            continue ;

        RECT_F      rcLayerF = rcCanvas ; // 计算layer对应区域
        CanvasToLayerRect (*pLayer, rcLayerF) ;
        // 由于放大时可能会出现浮点数，所以要取整（缩小时不会有）
        RECT        rcLayer = {(LONG)floor(rcLayerF.left), (LONG)floor(rcLayerF.top),
                               (LONG)ceil(rcLayerF.right), (LONG)ceil(rcLayerF.bottom)} ; // 计算layer对应区域
        pLayer->BoundRect (rcLayer) ; // layer上区域
        if (IsRectEmpty(&rcLayer))
            continue ;
        
        RECT		rcInCanvas = rcLayer ;
        LayerToCanvasRect (*pLayer, rcInCanvas) ;
        MapScaledRect (rcInCanvas) ;
        // 消除滚动条的偏移及加上居中显示时的偏移即为窗口位图上的位置
        ::OffsetRect (&rcInCanvas, -nScrollX + rcView.left, -nScrollY + rcView.top) ;

        // 在最终窗口位图上显示
        if (IsRectEmpty(&rcInCanvas))
            continue ;
        pImgView->AlphaBlend (*pLayer, rcInCanvas, rcLayer, pLayer->GetLayerTransparent()) ;
    }
}
//===================================================================
void  FCObjCanvas::MakeUpdateImage (int nScrollX, int nScrollY, const SIZE & sizeView,
                                    const FCObjImage & imgBack,
                                    const RECT & rcCanvas,
                                    FCObjImage * pImgUpdate,
                                    RECT * rcImgOnView) const
{
    if (IsCanvasEmpty() || (sizeView.cx <= 0) || (sizeView.cy <= 0) ||
        (pImgUpdate == NULL) || (rcImgOnView == NULL))
        return ;

    // 首先计算最终更新区域
    const POINT   ptOffset = GetViewOffset (sizeView) ;
    RECT          rcBlock = rcCanvas ;
    RECT_F		  rcView_F = {nScrollX-ptOffset.x, nScrollY-ptOffset.y, 0.0, 0.0} ; // canvas画到窗口位图上的区域
    rcView_F.right = rcView_F.left + sizeView.cx ;
    rcView_F.bottom = rcView_F.top + sizeView.cy ;
    MapRealRect (rcView_F) ;
    RECT    rcView = {(LONG)floor(rcView_F.left), (LONG)floor(rcView_F.top),
                      (LONG)ceil(rcView_F.right), (LONG)ceil(rcView_F.bottom)} ;
    if (::IntersectRect (&rcBlock, &rcBlock, &rcView) == 0)
        return ;
    BoundRect (rcBlock) ;
    if (IsRectEmpty(&rcBlock)) // rcBlock放canvas更新区域
        return ;

    // 计算rcBlock所对应的rcView的区域
    rcView = rcBlock ;
    MapScaledRect (rcView) ; // 这里的舍去误差是应该的

    // 创建区域位图
    if (!pImgUpdate->Create (RECTWIDTH(rcView), RECTHEIGHT(rcView), 32))
    {
        FAssert (FALSE) ;
        return ;
    }

    // 加上居中显示的偏移才是view上的rect
    CopyRect (rcImgOnView, &rcView) ;
    OffsetRect (rcImgOnView, ptOffset.x-nScrollX, ptOffset.y-nScrollY) ;

    // 画canvas背景
    const int     nBkPosX = -rcView.left % imgBack.Width(),
                  nBkPosY = -rcView.top % imgBack.Height() ;
    pImgUpdate->TileBlock (imgBack, nBkPosX, nBkPosY) ;

    // 绘制各个图层
    for (int i=0 ; i < GetLayerNumber() ; i++)
    {
        FCObjLayer      * pLayer = this->GetLayer(i) ;
        if ((pLayer == NULL) || !pLayer->GetLayerVisible())
            continue ;
        
        // 计算layer对应区域
        RECT        rcLayer = rcBlock ;
        CanvasToLayerRect (*pLayer, rcLayer) ;
        pLayer->BoundRect (rcLayer) ; // layer上区域
        if (IsRectEmpty(&rcLayer))
            continue ;

        // 映射到窗口位图上的位置
        RECT        rcInCanvas = rcLayer ;
        LayerToCanvasRect (*pLayer, rcInCanvas) ;
        MapScaledRect (rcInCanvas) ;
        OffsetRect (&rcInCanvas, -rcView.left, -rcView.top) ;
        
        // 在最终位图上显示
        pImgUpdate->AlphaBlend (*pLayer, rcInCanvas, rcLayer, pLayer->GetLayerTransparent()) ;
    }
}
//===================================================================
// 从canvas中生成位图
void  FCObjCanvas::GetCanvasImage (FCObjImage * imgCanvas) const
{

}
//===================================================================
// 为图层管理器做缩略图/Make Thumbnail (for layer manager)
void  FCObjCanvas::MakeLayerThumbnail (FCObjImage * pImgThumb, int nWidth, int nHeight, const FCObjLayer & fLayer, const FCObjImage & imgBack, RECT * pCanvasPos) const
{
    if (!fLayer.IsValidImage() || (imgBack.ColorBits() != 32) || (pImgThumb == NULL) || !imgBack.IsValidImage())
        return ;

    // 制作缩略图
    if (!pImgThumb->Create (nWidth, nHeight, 32))
        return ;

    // 填充周围颜色
    FCPixelFillColor     cmdFillCr(g_crThumbBack) ;
    pImgThumb->SinglePixelProcessProc (cmdFillCr) ;

    // 计算canvas在thumb位图上的显示区域
    SIZE        sizeCanvas = GetCanvasDimension() ;
    RECT        rcCanvasShow ; // canvas显示区域
    double		duScaleX = pImgThumb->Width()/(double)sizeCanvas.cx,
                duScaleY = pImgThumb->Height()/(double)sizeCanvas.cy ;
    double      duScale = FMin (duScaleX, duScaleY) ;
    if (duScaleX < duScaleY)
    {
        int     nShowHeight = static_cast<int>(sizeCanvas.cy * duScale) ;
        rcCanvasShow.left = 0 ;
        rcCanvasShow.top = (pImgThumb->Height() - nShowHeight)/2 ;
        rcCanvasShow.right = pImgThumb->Width() ;
        rcCanvasShow.bottom = rcCanvasShow.top + nShowHeight ;
    }
    else
    {
        int     nShowWidth = static_cast<int>(sizeCanvas.cx * duScale) ;
        rcCanvasShow.left = (pImgThumb->Width() - nShowWidth)/2 ;
        rcCanvasShow.top = 0 ;
        rcCanvasShow.right = rcCanvasShow.left + nShowWidth ;
        rcCanvasShow.bottom = pImgThumb->Height() ;
    }
    pImgThumb->BoundRect (rcCanvasShow) ;
    if (IsRectEmpty (&rcCanvasShow))
        return ;

    // 填充canvas背景，注意背景是32bit且alpha为0xFF
    FCObjImage    imgCanvas (RECTWIDTH(rcCanvasShow), RECTHEIGHT(rcCanvasShow), 32) ;
    imgCanvas.TileBlock (imgBack, 0, 0) ;
    pImgThumb->CoverBlock (imgCanvas, rcCanvasShow.left, rcCanvasShow.top) ;

    // layer落在canvas中的区域以及在canvas上绘制的区域
    RECT		rcLayer = {0, 0, sizeCanvas.cx, sizeCanvas.cy} ;
    this->CanvasToLayerRect (fLayer, rcLayer) ;
    fLayer.BoundRect (rcLayer) ;
    if (IsRectEmpty (&rcLayer))
        return ;

    RECT		rcInCanvas = rcLayer ;
    this->LayerToCanvasRect (fLayer, rcInCanvas) ;
    rcInCanvas.left = (LONG)(rcInCanvas.left * duScale) ;
    rcInCanvas.top = (LONG)(rcInCanvas.top * duScale) ;
    rcInCanvas.right = (LONG)(rcInCanvas.right * duScale) ;
    rcInCanvas.bottom = (LONG)(rcInCanvas.bottom * duScale) ;

    // 加上canvas在thumb上的偏移
    ::OffsetRect (&rcInCanvas, rcCanvasShow.left, rcCanvasShow.top) ;

    // 行了，现在rcInCanvas中放的是最终显示的位置（位于thumb上），开始画图层
    if (!IsRectEmpty(&rcInCanvas) && !IsRectEmpty(&rcLayer))
        pImgThumb->AlphaBlend (fLayer, rcInCanvas, rcLayer, 100) ; // 缩略图就不透明了
}
//===================================================================
SIZE  FCObjCanvas::GetCanvasScaledDimension () const
{
	SIZE		sizeRet = {0,0} ;
	if (m_iScale > 0)
	{
		sizeRet.cx = m_sizeCanvas.cx * m_iScale ;
		sizeRet.cy = m_sizeCanvas.cy * m_iScale ;
	}
	else
	{
		if (m_iScale < 0)
		{
			sizeRet.cx = m_sizeCanvas.cx / (-m_iScale) ;
			sizeRet.cy = m_sizeCanvas.cy / (-m_iScale) ;
		}
	}
	return sizeRet ;
}
//===================================================================
SIZE_F  FCObjCanvas::GetCanvasScaledDimension_F () const
{
    SIZE_F      sizeRet = {0.0, 0.0} ;
    if (m_iScale > 0)
    {
        sizeRet.cx = m_sizeCanvas.cx * m_iScale ;
        sizeRet.cy = m_sizeCanvas.cy * m_iScale ;
    }
    else
    {
        if (m_iScale < 0)
        {
            sizeRet.cx = m_sizeCanvas.cx / (double)(-m_iScale) ;
            sizeRet.cy = m_sizeCanvas.cy / (double)(-m_iScale) ;
        }
    }
    return sizeRet ;
}
//===================================================================
void  FCObjCanvas::MapRealRect (RECT & rect) const
{
	if (m_iScale > 1)
	{
		rect.left /= m_iScale ;
		rect.top /= m_iScale ;
		rect.right /= m_iScale ;
		rect.bottom /= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			rect.left *= -m_iScale ;
			rect.top *= -m_iScale ;
			rect.right *= -m_iScale ;
			rect.bottom *= -m_iScale ;
		}
}
void  FCObjCanvas::MapScaledRect (RECT & rect) const
{
	if (m_iScale > 1)
	{
		rect.left *= m_iScale ;
		rect.top *= m_iScale ;
		rect.right *= m_iScale ;
		rect.bottom *= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			rect.left /= -m_iScale ;
			rect.top /= -m_iScale ;
			rect.right /= -m_iScale ;
			rect.bottom /= -m_iScale ;
		}
}
void  FCObjCanvas::MapRealPoint (POINT & pt) const
{
	if (m_iScale > 1)
	{
		pt.x /= m_iScale ;
		pt.y /= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			pt.x *= -m_iScale ;
			pt.y *= -m_iScale ;
		}
}
void  FCObjCanvas::MapScaledPoint (POINT & pt) const
{
	if (m_iScale > 1)
	{
		pt.x *= m_iScale ;
		pt.y *= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			pt.x /= -m_iScale ;
			pt.y /= -m_iScale ;
		}
}
//===================================================================
void  FCObjCanvas::MapRealRect (RECT_F & rect) const
{
	if (m_iScale > 1)
	{
		rect.left /= (double)m_iScale ;
		rect.top /= (double)m_iScale ;
		rect.right /= (double)m_iScale ;
		rect.bottom /= (double)m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			rect.left *= -(double)m_iScale ;
			rect.top *= -(double)m_iScale ;
			rect.right *= -(double)m_iScale ;
			rect.bottom *= -(double)m_iScale ;
		}
}
void  FCObjCanvas::MapScaledRect (RECT_F & rect) const
{
	if (m_iScale > 1)
	{
		rect.left *= m_iScale ;
		rect.top *= m_iScale ;
		rect.right *= m_iScale ;
		rect.bottom *= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			rect.left /= -m_iScale ;
			rect.top /= -m_iScale ;
			rect.right /= -m_iScale ;
			rect.bottom /= -m_iScale ;
		}
}
void  FCObjCanvas::MapRealPoint (POINT_F & pt) const
{
	if (m_iScale > 1)
	{
		pt.x /= (double)m_iScale ;
		pt.y /= (double)m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			pt.x *= -(double)m_iScale ;
			pt.y *= -(double)m_iScale ;
		}
}
void  FCObjCanvas::MapScaledPoint (POINT_F & pt) const
{
	if (m_iScale > 1)
	{
		pt.x *= m_iScale ;
		pt.y *= m_iScale ;
	}
	else
		if (m_iScale < -1)
		{
			pt.x /= -m_iScale ;
			pt.y /= -m_iScale ;
		}
}
//===================================================================
void  FCObjCanvas::Undo ()
{
	if (!m_UndoList.empty())
	{
		FCCmdArtPrider	* pUndo = m_UndoList.back () ; // 取出Undo队列末尾的command obj
		pUndo->Undo () ;
		m_UndoList.pop_back () ;
		m_RedoList.push_front (pUndo) ;
	}
}
void  FCObjCanvas::Redo ()
{
	if (!m_RedoList.empty())
	{
		FCCmdArtPrider	* pRedo = m_RedoList.front () ; // 取出Redo队列首的command obj
		pRedo->Redo () ;
		m_RedoList.pop_front () ;
		m_UndoList.push_back (pRedo) ;
	}
}
void  FCObjCanvas::ClearRedoList ()
{
	while (!m_RedoList.empty ())
	{
		FCCmdArtPrider   * pNode = m_RedoList.front () ;
		m_RedoList.pop_front () ;
		delete pNode ;
	}
}
void  FCObjCanvas::ClearUndoList ()
{
	while (!m_UndoList.empty ())
	{
		FCCmdArtPrider   * pNode = m_UndoList.back () ;
		m_UndoList.pop_back () ;
		delete pNode ;
	}
}
//===================================================================
void  FCObjCanvas::ExecuteEffect (FCCmdArtPrider * cmd, FCObjProgress * Percent)
{
/*	if (cmd != NULL){
		cmd->Execute (*this, Percent) ;
		delete cmd ;
	}
*/
	if (cmd != NULL)
		if (m_nUndoLevel > 0)
		{
			// clear redo-list, ennnnnnn. it like a bintree.
			this->ClearRedoList() ;

			// first test whether the 'cmd' need composite,
			//   if 'YES', push this cmd to previous command
			if (IsUndoEnable())
			{
				FCCmdImgCmdComposite  * cmdCompo = dynamic_cast<FCCmdImgCmdComposite *>(m_UndoList.back()) ;
				if (cmdCompo && cmdCompo->IsNeedComposite(cmd))
				{
					cmd->Execute (*this, Percent) ;
					cmdCompo->PushImgCommand(cmd) ;
					return ;
				}
			}

			// 下面是非组合命令
			if ((int)m_UndoList.size() >= m_nUndoLevel)
			{
				FCCmdArtPrider	* DiscardCmd = m_UndoList.front () ;
				m_UndoList.pop_front () ;
				delete DiscardCmd ;
			}
			cmd->Execute (*this, Percent) ;
			m_UndoList.push_back (cmd) ;
		}
		else
		{
			cmd->Execute (*this, Percent) ;
			delete cmd ;
		}
}
//===================================================================
// 得到选中的区域位图
// 区域外填充alpha 0
void  FCObjCanvas::GetSelectLayerAlphaBlock (FCObjImage * imgBlock) const
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	FCObjLayer * pLayer = NULL;
	if (pDoc->IsAuto)
		{
			 pLayer = this->GetLayer(0);
		}
	else
		{
			int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
				pLayer = this->GetLayer(i);
		}
	//const FCObjLayer   * pLayer = GetCurrentLayer() ;
	if ((imgBlock == NULL) || (pLayer == NULL) || !pLayer->IsValidImage())
		return ;

	if (!HasSelected())
		*imgBlock = *(FCObjImage *)pLayer ;
	else
	{
		RECT		rcBox ;
		GetSelectLayerRect (*pLayer, &rcBox) ;
		if (IsRectEmpty (&rcBox))
			return ;

		pLayer->GetSubBlock (imgBlock, rcBox) ;

		// 把非区域置alpha 0
		FCObjImage		imgSelection ;
		GetSelectLayerMask (*pLayer, &imgSelection) ;	
		for (int y=0 ; y < imgBlock->Height() ; y++)
		{
			BYTE    * pBlock = imgBlock->GetBits(y),
				* pSel = imgSelection.GetBits(y) ;
			for (int x=0 ; x < imgBlock->Width() ; x++, pBlock+=4, pSel++)
				if (*pSel == 0)
					pBlock[3] = 0 ;
		}
	}
}
//===================================================================
// 具体选择的layer区域
void  FCObjCanvas::GetSelectLayerRect (const FCObjLayer & layer, RECT * rcLayer) const
{
	if ((rcLayer != NULL) && layer.IsValidImage())
		if (!HasSelected())
		{
			::SetRect (rcLayer, 0, 0, layer.Width(), layer.Height()) ;
		}
		else
		{
			// layer的canvas区域
			layer.GetRectInCanvas (rcLayer) ;

			// select的canvas区域
			RECT	rcSel ;
			m_CurrSel.GetRectInCanvas (&rcSel) ;

			::IntersectRect (rcLayer, &rcSel, rcLayer) ;
			CanvasToLayerRect (layer, *rcLayer) ; // layer上坐标
		}
}
//===================================================================
// imgMask8区域内为0xFF，外为0
void  FCObjCanvas::GetSelectLayerMask (const FCObjLayer & layer, FCObjImage * imgMask8) const
{
	if ((imgMask8 == NULL) || !layer.IsValidImage())
		return ;

	RECT		rcSel ;
	GetSelectLayerRect (layer, &rcSel) ; // layer选取区域
	if (!imgMask8->Create (RECTWIDTH(rcSel), RECTHEIGHT(rcSel), 8))
		return ;
	imgMask8->SetGraphObjPos (rcSel.left, rcSel.top) ;
	
	if (!HasSelected())
	{
		FillMemory (imgMask8->GetMemStart(), imgMask8->GetPitch()*imgMask8->Height(), 0xFF) ;
	}
	else
	{
		// 得到区域在m_CurrSel位图左上角的起点
		POINT	ptSel = m_CurrSel.GetGraphObjPos(),
				ptLayer = layer.GetGraphObjPos() ;
		int		nStartX = rcSel.left + ptLayer.x - ptSel.x,
				nStartY = rcSel.top + ptLayer.y - ptSel.y ;

		// 根据m_CurrSel把非区域置alpha 0
		for (int y=0 ; y < imgMask8->Height() ; y++)
		{
			BYTE	* pPixel = imgMask8->GetBits (y),
					* pRgnPx = m_CurrSel.GetBits (nStartX, nStartY++) ;
			for (int x=0 ; x < imgMask8->Width() ; x++, pPixel++)
				*pPixel = ((*pRgnPx++ == 0) ? 0 : 0xFF) ;
		}
	}
}
//===================================================================
// 当图像居中显示时，左上角的偏移
POINT  FCObjCanvas::GetViewOffset (const SIZE &sizeView) const
{
	POINT       ptOffset = {0, 0} ;
	SIZE        sizeScaled = GetCanvasScaledDimension() ;

	if (sizeScaled.cx < sizeView.cx) // X coordinate
		ptOffset.x = FMax<int>((sizeView.cx - sizeScaled.cx) / 2 ,0);
	if (sizeScaled.cy < sizeView.cy) // Y coordinate
		ptOffset.y = FMax<int>((sizeView.cy - sizeScaled.cy) / 2,0);
	return ptOffset ;
}
//===================================================================
BOOL  FCObjCanvas::IsPasteAvailable () const
{
    return FLib_IsPasteAvailable() ;
}
//===================================================================
// paste add new layer
BOOL  FCObjCanvas::PasteFromClipboard ()
{
    // 获得位图
    FCObjLayer    * pLayer = new FCObjLayer ;
    FLib_GetClipboardImage (*pLayer) ;
    if (!pLayer->IsValidImage())
    {
        delete pLayer ;
        FAssert(FALSE) ;
        return FALSE ;
    }

    // 加入到canvas中
    // 设置新图层的位置，位于中间
	RECT      rcSel = {0, 0, m_sizeCanvas.cx, m_sizeCanvas.cy} ;
	if (HasSelected())
	{
		GetSelection().GetRectInCanvas (&rcSel) ;
	}
	
	POINT     ptLayer ;
	//ptLayer.x = (rcSel.left+rcSel.right-pLayer->Width())/2 ;
	//ptLayer.y = (rcSel.top+rcSel.bottom-pLayer->Height())/2 ;
	
	ptLayer.x = (rcSel.left) ;
	ptLayer.y = (rcSel.top) ;
	
	pLayer->SetGraphObjPos (ptLayer) ;
	this->ExecuteEffect (new FCCmdLayerAdd (pLayer, GetCurrentLayerIndex() + 1)) ;
	
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	pDoc->GetLayerBar()->Add(*pLayer);

	return TRUE ;
}
//===================================================================
// copy selection to clipboard
void  FCObjCanvas::CopyToClipboard () const
{
    FCObjImage      imgSel ;
    GetSelectLayerAlphaBlock (&imgSel) ; // imgSel为32 bit色
    FAssert (imgSel.IsValidImage()) ;
    FLib_CopyToClipboard (imgSel) ;
}
//===================================================================
RGBQUAD  FCObjCanvas::GetCanvasPixelColor (const POINT & ptCanvas) // the coordinate is actual
{
    RGBQUAD     crRet = FCColor::crWhite ; // default
    if (!IsPtInCanvas (ptCanvas))
        return crRet ;

    // alphablend the pixel
    for (int i=0 ; i < GetLayerNumber() ; i++)
    {
        FCObjLayer     * pLayer = this->GetLayer(i) ;
        if ((pLayer == NULL) || !pLayer->GetLayerVisible())
            continue ;

        // 合并各个图层对应点
        POINT       ptLayer (ptCanvas) ;
        CanvasToLayerPoint (*pLayer, ptLayer) ;
        if (pLayer->IsInside (ptLayer.x, ptLayer.y))
        {
            RGBQUAD   crSrc = *(RGBQUAD*)pLayer->GetBits (ptLayer.x, ptLayer.y) ;
            crSrc.rgbReserved = crSrc.rgbReserved * pLayer->GetLayerTransparent() / 100 ;
            FCColor::AlphaBlendPixel (&crRet, &crSrc) ;
        }
    }
    return crRet ;
}
//===================================================================
