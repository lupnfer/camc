#include "stdafx.h"
#include "ObjCanvas.h"
#include "ObjLayer.h"
#include "FCSinglePixelProcessBase.h"
#include "FColor.h"
#include "Win32_Func.h"
#include "FCCmdImgCmdComposite.h"
#include "LayerCommand.h"
#include "CAMCDoc.h"

const RGBQUAD    g_crBackground = {0xC0,0xC0,0xC0} ; // view canvas����ɫ
const RGBQUAD    g_crThumbBack = {251, 203, 191} ; // thumb����ɫ

//===================================================================
FCObjCanvas::~FCObjCanvas ()
{
    this->ClearUndoRedoList () ; // ���ܺ������˳��ߵ�����Ϊundo�����п��ܱ�����ָ��

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
	if (nRemove > 0) // ���һ����undo����
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
// �ӽ�ȥ�˾Ͳ�Ҫ�Լ��ͷţ�canvas����ʱ��delete
// Ҳ�п��ܱ�remove�ˣ���command���������ʱdelete
// nIndex == -1�����뵽ĩβ
void  FCObjCanvas::AddLayer (FCObjLayer * pNewLay, int nIndex)
{
	if (pNewLay != NULL)
	{
        FAssert (pNewLay->ColorBits() == 32) ;

		// �ȱ�֤������û�д�layer
		std::deque<FCObjLayer *>::iterator    pt ;
		pt = find (m_LayerList.begin(), m_LayerList.end(), pNewLay) ;
		if (pt == m_LayerList.end()) // ���ڶ�����
		{
			// ���뵽LayerList��
			if (nIndex == -1)
				m_LayerList.push_back (pNewLay) ;
			else
				m_LayerList.insert (m_LayerList.begin() + nIndex, pNewLay) ;
			m_pCurrentLayer = pNewLay ;
			m_pCurrentLayer->SetParentCanvas (this) ; // ����

			// ��Removed LayerList���Ƴ�
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
		// ���µ�ǰlayer
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
// Ҫע��rcView, rcCanvas, rcClient֮��Ĺ�ϵ
// ����ʱ�����𻭱߿�
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
    RECT           rcView = {0, 0, sizeView.cx, sizeView.cy} ; // canvas��������λͼ�ϵ�����
    RECT_F         rcCanvas ; // canvas�϶�Ӧ������

    // ������ʾ������canvas����С��ʱ��֤���У�
    // ����ʱcanvas������������
    if ((int)sizeScaled.cx > sizeView.cx) // X coordinate
    {
        rcCanvas.left = nScrollX ;
        rcCanvas.right = nScrollX + sizeView.cx ;
    }
    else
    {
        rcCanvas.left = 0.0 ;
        rcCanvas.right = sizeScaled.cx ;
        rcView.left = (sizeView.cx - (int)sizeScaled.cx) / 2 ; // ������ʾ
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
        rcView.top = (sizeView.cy - (int)sizeScaled.cy) / 2 ; // ������ʾ
        rcView.bottom = rcView.top + (int)sizeScaled.cy ;
    }
    // ������ϣ�����ʱ��rcCanvas�е����껹�Ǿ������ŵģ�ӳ�䵽��ʵ����
    this->MapRealRect (rcCanvas) ;

    // 1.��仭����ɫ����ͼ����������򣨽���ͼ��X,Y�������ʾʱ��
    // 2.��canvas����
    const RECT    rcClient = {0, 0, sizeView.cx, sizeView.cy} ;
    const int     nBkPosX = -nScrollX % imgBack.Width(),
                  nBkPosY = -nScrollY % imgBack.Height() ;
    if (!EqualRect(&rcClient, &rcView))
    {
        // ��һ���߿�ͻ�����ɫһ������
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

    // ok�����ڻ���rcView�ڲ��ĸ���ͼ��
    for (int i=0 ; i < GetLayerNumber() ; i++)
    {
        FCObjLayer      * pLayer = this->GetLayer(i) ;
        if ((pLayer == NULL) || !pLayer->GetLayerVisible())
            continue ;

        RECT_F      rcLayerF = rcCanvas ; // ����layer��Ӧ����
        CanvasToLayerRect (*pLayer, rcLayerF) ;
        // ���ڷŴ�ʱ���ܻ���ָ�����������Ҫȡ������Сʱ�����У�
        RECT        rcLayer = {(LONG)floor(rcLayerF.left), (LONG)floor(rcLayerF.top),
                               (LONG)ceil(rcLayerF.right), (LONG)ceil(rcLayerF.bottom)} ; // ����layer��Ӧ����
        pLayer->BoundRect (rcLayer) ; // layer������
        if (IsRectEmpty(&rcLayer))
            continue ;
        
        RECT		rcInCanvas = rcLayer ;
        LayerToCanvasRect (*pLayer, rcInCanvas) ;
        MapScaledRect (rcInCanvas) ;
        // ������������ƫ�Ƽ����Ͼ�����ʾʱ��ƫ�Ƽ�Ϊ����λͼ�ϵ�λ��
        ::OffsetRect (&rcInCanvas, -nScrollX + rcView.left, -nScrollY + rcView.top) ;

        // �����մ���λͼ����ʾ
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

    // ���ȼ������ո�������
    const POINT   ptOffset = GetViewOffset (sizeView) ;
    RECT          rcBlock = rcCanvas ;
    RECT_F		  rcView_F = {nScrollX-ptOffset.x, nScrollY-ptOffset.y, 0.0, 0.0} ; // canvas��������λͼ�ϵ�����
    rcView_F.right = rcView_F.left + sizeView.cx ;
    rcView_F.bottom = rcView_F.top + sizeView.cy ;
    MapRealRect (rcView_F) ;
    RECT    rcView = {(LONG)floor(rcView_F.left), (LONG)floor(rcView_F.top),
                      (LONG)ceil(rcView_F.right), (LONG)ceil(rcView_F.bottom)} ;
    if (::IntersectRect (&rcBlock, &rcBlock, &rcView) == 0)
        return ;
    BoundRect (rcBlock) ;
    if (IsRectEmpty(&rcBlock)) // rcBlock��canvas��������
        return ;

    // ����rcBlock����Ӧ��rcView������
    rcView = rcBlock ;
    MapScaledRect (rcView) ; // �������ȥ�����Ӧ�õ�

    // ��������λͼ
    if (!pImgUpdate->Create (RECTWIDTH(rcView), RECTHEIGHT(rcView), 32))
    {
        FAssert (FALSE) ;
        return ;
    }

    // ���Ͼ�����ʾ��ƫ�Ʋ���view�ϵ�rect
    CopyRect (rcImgOnView, &rcView) ;
    OffsetRect (rcImgOnView, ptOffset.x-nScrollX, ptOffset.y-nScrollY) ;

    // ��canvas����
    const int     nBkPosX = -rcView.left % imgBack.Width(),
                  nBkPosY = -rcView.top % imgBack.Height() ;
    pImgUpdate->TileBlock (imgBack, nBkPosX, nBkPosY) ;

    // ���Ƹ���ͼ��
    for (int i=0 ; i < GetLayerNumber() ; i++)
    {
        FCObjLayer      * pLayer = this->GetLayer(i) ;
        if ((pLayer == NULL) || !pLayer->GetLayerVisible())
            continue ;
        
        // ����layer��Ӧ����
        RECT        rcLayer = rcBlock ;
        CanvasToLayerRect (*pLayer, rcLayer) ;
        pLayer->BoundRect (rcLayer) ; // layer������
        if (IsRectEmpty(&rcLayer))
            continue ;

        // ӳ�䵽����λͼ�ϵ�λ��
        RECT        rcInCanvas = rcLayer ;
        LayerToCanvasRect (*pLayer, rcInCanvas) ;
        MapScaledRect (rcInCanvas) ;
        OffsetRect (&rcInCanvas, -rcView.left, -rcView.top) ;
        
        // ������λͼ����ʾ
        pImgUpdate->AlphaBlend (*pLayer, rcInCanvas, rcLayer, pLayer->GetLayerTransparent()) ;
    }
}
//===================================================================
// ��canvas������λͼ
void  FCObjCanvas::GetCanvasImage (FCObjImage * imgCanvas) const
{

}
//===================================================================
// Ϊͼ�������������ͼ/Make Thumbnail (for layer manager)
void  FCObjCanvas::MakeLayerThumbnail (FCObjImage * pImgThumb, int nWidth, int nHeight, const FCObjLayer & fLayer, const FCObjImage & imgBack, RECT * pCanvasPos) const
{
    if (!fLayer.IsValidImage() || (imgBack.ColorBits() != 32) || (pImgThumb == NULL) || !imgBack.IsValidImage())
        return ;

    // ��������ͼ
    if (!pImgThumb->Create (nWidth, nHeight, 32))
        return ;

    // �����Χ��ɫ
    FCPixelFillColor     cmdFillCr(g_crThumbBack) ;
    pImgThumb->SinglePixelProcessProc (cmdFillCr) ;

    // ����canvas��thumbλͼ�ϵ���ʾ����
    SIZE        sizeCanvas = GetCanvasDimension() ;
    RECT        rcCanvasShow ; // canvas��ʾ����
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

    // ���canvas������ע�ⱳ����32bit��alphaΪ0xFF
    FCObjImage    imgCanvas (RECTWIDTH(rcCanvasShow), RECTHEIGHT(rcCanvasShow), 32) ;
    imgCanvas.TileBlock (imgBack, 0, 0) ;
    pImgThumb->CoverBlock (imgCanvas, rcCanvasShow.left, rcCanvasShow.top) ;

    // layer����canvas�е������Լ���canvas�ϻ��Ƶ�����
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

    // ����canvas��thumb�ϵ�ƫ��
    ::OffsetRect (&rcInCanvas, rcCanvasShow.left, rcCanvasShow.top) ;

    // ���ˣ�����rcInCanvas�зŵ���������ʾ��λ�ã�λ��thumb�ϣ�����ʼ��ͼ��
    if (!IsRectEmpty(&rcInCanvas) && !IsRectEmpty(&rcLayer))
        pImgThumb->AlphaBlend (fLayer, rcInCanvas, rcLayer, 100) ; // ����ͼ�Ͳ�͸����
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
		FCCmdArtPrider	* pUndo = m_UndoList.back () ; // ȡ��Undo����ĩβ��command obj
		pUndo->Undo () ;
		m_UndoList.pop_back () ;
		m_RedoList.push_front (pUndo) ;
	}
}
void  FCObjCanvas::Redo ()
{
	if (!m_RedoList.empty())
	{
		FCCmdArtPrider	* pRedo = m_RedoList.front () ; // ȡ��Redo�����׵�command obj
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

			// �����Ƿ��������
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
// �õ�ѡ�е�����λͼ
// ���������alpha 0
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

		// �ѷ�������alpha 0
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
// ����ѡ���layer����
void  FCObjCanvas::GetSelectLayerRect (const FCObjLayer & layer, RECT * rcLayer) const
{
	if ((rcLayer != NULL) && layer.IsValidImage())
		if (!HasSelected())
		{
			::SetRect (rcLayer, 0, 0, layer.Width(), layer.Height()) ;
		}
		else
		{
			// layer��canvas����
			layer.GetRectInCanvas (rcLayer) ;

			// select��canvas����
			RECT	rcSel ;
			m_CurrSel.GetRectInCanvas (&rcSel) ;

			::IntersectRect (rcLayer, &rcSel, rcLayer) ;
			CanvasToLayerRect (layer, *rcLayer) ; // layer������
		}
}
//===================================================================
// imgMask8������Ϊ0xFF����Ϊ0
void  FCObjCanvas::GetSelectLayerMask (const FCObjLayer & layer, FCObjImage * imgMask8) const
{
	if ((imgMask8 == NULL) || !layer.IsValidImage())
		return ;

	RECT		rcSel ;
	GetSelectLayerRect (layer, &rcSel) ; // layerѡȡ����
	if (!imgMask8->Create (RECTWIDTH(rcSel), RECTHEIGHT(rcSel), 8))
		return ;
	imgMask8->SetGraphObjPos (rcSel.left, rcSel.top) ;
	
	if (!HasSelected())
	{
		FillMemory (imgMask8->GetMemStart(), imgMask8->GetPitch()*imgMask8->Height(), 0xFF) ;
	}
	else
	{
		// �õ�������m_CurrSelλͼ���Ͻǵ����
		POINT	ptSel = m_CurrSel.GetGraphObjPos(),
				ptLayer = layer.GetGraphObjPos() ;
		int		nStartX = rcSel.left + ptLayer.x - ptSel.x,
				nStartY = rcSel.top + ptLayer.y - ptSel.y ;

		// ����m_CurrSel�ѷ�������alpha 0
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
// ��ͼ�������ʾʱ�����Ͻǵ�ƫ��
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
    // ���λͼ
    FCObjLayer    * pLayer = new FCObjLayer ;
    FLib_GetClipboardImage (*pLayer) ;
    if (!pLayer->IsValidImage())
    {
        delete pLayer ;
        FAssert(FALSE) ;
        return FALSE ;
    }

    // ���뵽canvas��
    // ������ͼ���λ�ã�λ���м�
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
    GetSelectLayerAlphaBlock (&imgSel) ; // imgSelΪ32 bitɫ
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

        // �ϲ�����ͼ���Ӧ��
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
