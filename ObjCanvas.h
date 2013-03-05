//////////////////////////////////////////////////////////////////
//																//
//		用途 : 画布对象（layer的容器）							//
							//										//	//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_OBJECT_CANVAS_H__
#define	 __FOO_OBJECT_CANVAS_H__
#include "ObjLayer.h"
#include "ObjSelect.h"

#include <deque>
#include <algorithm>

using namespace std ;
class FCCmdArtPrider ; // external class
class FCObjProgress ; // external class

//=============================================================================
//	Declaration
//=============================================================================
class FCObjCanvas : public FCObject
{
public :
	FCObjCanvas (int nUndoLevel = 20) ;
	virtual ~FCObjCanvas ()  ;

    // misc
    RGBQUAD GetCanvasPixelColor (const POINT & ptCanvas) ; // the coordinate is actual

    // DPI resolution
    void    SetCanvasResolution (int nResX, int nResY) ;
    void    GetCanvasResolution (int & nResX, int & nResY) const ;

	// Size
	void	SetCanvasDimension (const SIZE & size) ;
	SIZE	GetCanvasDimension () const ;
	SIZE	GetCanvasScaledDimension () const ; // 缩小时舍掉小数位
    SIZE_F  GetCanvasScaledDimension_F () const ;
	BOOL	IsCanvasEmpty () const ;
	BOOL	IsPtInCanvas (const POINT & pt) const ;

	// Layer operations
	int     GetLayerNumber () const ;
	BOOL	IsValidLayerIndex (int nIndex) const ;
	FCObjLayer * GetLayer (int nIndex) const ;
	FCObjLayer * GetCurrentLayer () const ;
	int		GetCurrentLayerIndex () const ;
	void	SetCurrentLayer (int nIndex) ;
	void	SetCurrentLayer (const FCObjLayer * pLayer) ;
	void	AddLayer (FCObjLayer * pNewLay, int nIndex = -1) ; // 加进去了就不要自己释放，canvas析构时会delete
	void	RemoveLayer (const FCObjLayer * pLayer, BOOL bDelLayer = TRUE) ; // 只有一个时不能删
    int		FindLayer (const FCObjLayer * pNewLay) const ;
	void	DeleteLayerFromRemoveList (FCObjLayer * pLayer) ;
	deque<FCObjLayer *>*  __GetLayerListPtr () ; // ...后门，能不用就不用

	// Undo/Redo manage
	// cmd命令必须用new创建，不要自己删除cmd对象
	void	ExecuteEffect (FCCmdArtPrider * cmd, FCObjProgress * Percent = NULL) ;
	void	Undo () ;
	void	Redo () ;
	bool	IsUndoEnable () const ;
	bool	IsRedoEnable () const ;
	void	ClearRedoList () ;
	void	ClearUndoList () ;
	void	ClearUndoRedoList () ;
	void	SetUndoLevel (int nLevel) ; // 0为无undo，执行cmd后删除。（会清空多出的cmd）
	int		GetUndoLevel () const ;

	// Region select
	FCObjSelect & GetSelection () ;
	bool	HasSelected () const ;
	BOOL	PtInSelection (int x, int y) const ; // (x,y)为canvas坐标
	void	GetSelectLayerAlphaBlock (FCObjImage * imgBlock32) const ; // imgBlock32得到区域位图
	void	GetSelectLayerRect (const FCObjLayer & layer, RECT * rcLayer) const ; // 选择区域对应layer上的区域外围矩形
	void	GetSelectLayerMask (const FCObjLayer & layer, FCObjImage * imgMask8) const ; // imgMask8区域内为0xFF，外为0
	void	__ClearSelection () ;
	void	__SetRegionMember (const FCObjSelect & select) ; // 直接设置m_CurrSel成员

	// Clipboard
	void	CopyToClipboard () const ; // copy selection to clipboard
	BOOL	PasteFromClipboard () ; // add new layer on paste
	BOOL	IsPasteAvailable () const ;

	// Zoom scale
	void	SetZoomScale (int iScale) ;
	void    SetZoomIn();//在现有基础上放大2倍；
	void    SetZoomOut();//在现有基础上缩小2倍；
	void    SetIteratedZoomScale(float fScale);
	int		GetZoomScale () const ;

	// Coordinate transport
	void	BoundRect (RECT & rect) const ; // rect为真实canvas坐标
	void	MapRealRect (RECT & rect) const ; // scaled ==> actual
	void	MapScaledRect (RECT & rect) const ; // actual ==> scaled
	void	MapRealPoint (POINT & pt) const; // scaled ==> actual
	void	MapScaledPoint (POINT & pt) const ; // actual ==> scaled
	void	CanvasToLayerRect (const FCObjGraph & layer, RECT & rcCanvas) const ;
	void	CanvasToLayerPoint (const FCObjGraph & layer, POINT & ptCanvas) const ;
	void	LayerToCanvasRect (const FCObjGraph & layer, RECT & rcLayer) const ;
	void	LayerToCanvasPoint (const FCObjGraph & layer, POINT & ptLayer) const ;
	// Coordinate transport (float)
	void	BoundRect (RECT_F & rect) const ; // rect为真实canvas坐标
	void	MapRealRect (RECT_F & rect) const ; // scaled ==> actual
	void	MapScaledRect (RECT_F & rect) const ; // actual ==> scaled
	void	MapRealPoint (POINT_F & pt) const ; // scaled ==> actual
	void	MapScaledPoint (POINT_F & pt) const ; // actual ==> scaled
	void	CanvasToLayerRect (const FCObjGraph & layer, RECT_F & rcCanvas) const ;
	void	CanvasToLayerPoint (const FCObjGraph & layer, POINT_F & ptCanvas) const ;
	void	LayerToCanvasRect (const FCObjGraph & layer, RECT_F & rcLayer) const ;
	void	LayerToCanvasPoint (const FCObjGraph & layer, POINT_F & ptLayer) const ;

	// 为图层管理器做指定layer的缩略图/Make Thumbnail (for layer manager)
    void    MakeLayerThumbnail (FCObjImage * pImgThumb, int nWidth, int nHeight,
                                const FCObjLayer & fLayer,
                                const FCObjImage & imgBack,
                                RECT * pCanvasPos = NULL) const ;
    // 为View显示制图
    void    MakeViewImage (int nScrollX, int nScrollY, const SIZE & sizeView,
                           const FCObjImage & imgBack,
                           FCObjImage * pImgView) const ;

    void    MakeUpdateImage (int nScrollX, int nScrollY, const SIZE & sizeView,
                             const FCObjImage & imgBack,
                             const RECT & rcCanvas,
                             FCObjImage * pImgUpdate,
                             RECT * rcImgOnView) const ;
	// draw
	POINT	GetViewOffset (const SIZE & sizeView) const ; // 当图像居中时，左上角的偏移
	void	GetCanvasImage (FCObjImage * imgCanvas) const ;



protected :
	int		FindRemovedLayer (const FCObjLayer * pLayer) const ;

protected :
	SIZE				m_sizeCanvas ;
	FCObjLayer			* m_pCurrentLayer ;
	deque<FCObjLayer *>	m_LayerList ;
	deque<FCObjLayer *>	m_RemovedLayerList ; // 被移除的layer，和m_LayerList一起释放
	int					m_iScale ;

    int                 m_nResX, m_nResY ; // image's DPI resolution (Dot Per Inch)

	FCObjSelect				m_CurrSel ; // 当前选取的区域对象

	deque<FCCmdArtPrider *>	m_UndoList, m_RedoList ; // undo/redo sequence
	int						m_nUndoLevel ; // the level of undo operation. the ZERO means disable undo

} ;

//=============================================================================
//	Implement
//=============================================================================
inline FCObjCanvas::FCObjCanvas (int nUndoLevel) {
	m_sizeCanvas.cx = m_sizeCanvas.cy = 0 ;
	m_pCurrentLayer = NULL ;
	m_iScale = 1 ;
	m_nUndoLevel = nUndoLevel ;
    m_nResX = m_nResY = 72 ;
}
inline void  FCObjCanvas::SetCanvasDimension (const SIZE & size) {
	m_sizeCanvas = size ;
}
inline SIZE  FCObjCanvas::GetCanvasDimension () const {
	return m_sizeCanvas ;
}
inline BOOL  FCObjCanvas::IsCanvasEmpty () const {
	return ((m_sizeCanvas.cx <= 0) || (m_sizeCanvas.cy <= 0)) ;
}
inline BOOL  FCObjCanvas::IsPtInCanvas (const POINT & pt) const {
	return ((pt.x >= 0) && (pt.x < m_sizeCanvas.cx) && (pt.y >= 0) && (pt.y < m_sizeCanvas.cy)) ;
}
//=============================================================================
// DPI resolution
inline void  FCObjCanvas::SetCanvasResolution (int nResX, int nResY) {
    m_nResX = nResX ; m_nResY = nResY ;
}
inline void  FCObjCanvas::GetCanvasResolution (int & nResX, int & nResY) const {
    nResX = m_nResX ; nResY = m_nResY ;
}
//=============================================================================
inline BOOL  FCObjCanvas::IsValidLayerIndex (int nIndex) const {
	return ((nIndex >= 0) && (nIndex < GetLayerNumber())) ;
}
inline FCObjLayer *  FCObjCanvas::GetLayer (int nIndex) const {
	return (IsValidLayerIndex(nIndex) ? m_LayerList[nIndex] : NULL) ;
}
inline void  FCObjCanvas::SetCurrentLayer (int nIndex) {
	if (IsValidLayerIndex (nIndex))
		m_pCurrentLayer = m_LayerList[nIndex] ;
}
inline void  FCObjCanvas::SetCurrentLayer (const FCObjLayer * pLayer) {
	if (m_pCurrentLayer != pLayer)
		this->SetCurrentLayer (FindLayer (pLayer)) ;
}
inline FCObjLayer * FCObjCanvas::GetCurrentLayer () const {
	return m_pCurrentLayer ;
}
inline int  FCObjCanvas::GetLayerNumber () const {
	return (int)m_LayerList.size () ;
}
inline int	 FCObjCanvas::GetCurrentLayerIndex () const {
	return this->FindLayer (m_pCurrentLayer) ;
}
//=============================================================================
inline void  FCObjCanvas::SetZoomScale (int iScale) {
	m_iScale = iScale ;
}
inline void FCObjCanvas::SetZoomIn()
{
	if(m_iScale>0){
		m_iScale=m_iScale*2;
	}else {
		m_iScale=m_iScale/2;
		if(m_iScale==-1)
			m_iScale=1;
	}
}
inline void FCObjCanvas::SetZoomOut()
{
	if(m_iScale>0){
		m_iScale=m_iScale/2;
		if(m_iScale==0)
			m_iScale=-2;
	}else {
		m_iScale=m_iScale*2;
	}
}
inline int  FCObjCanvas::GetZoomScale () const {
	return m_iScale ;
}
//=============================================================================
inline void  FCObjCanvas::CanvasToLayerRect (const FCObjGraph & layer, RECT & rcCanvas) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	::OffsetRect (&rcCanvas, -ptLayer.x, -ptLayer.y) ;
}
inline void  FCObjCanvas::LayerToCanvasRect (const FCObjGraph & layer, RECT & rcLayer) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	::OffsetRect (&rcLayer, ptLayer.x, ptLayer.y) ;
}
inline void  FCObjCanvas::CanvasToLayerPoint (const FCObjGraph & layer, POINT & ptCanvas) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	ptCanvas.x -= ptLayer.x ; ptCanvas.y -= ptLayer.y ;
}
inline void  FCObjCanvas::LayerToCanvasPoint (const FCObjGraph & layer, POINT & ptLayer) const {
	POINT	ptLayerPos = layer.GetGraphObjPos() ;
	ptLayer.x += ptLayerPos.x ; ptLayer.y += ptLayerPos.y ;
}

inline void  FCObjCanvas::CanvasToLayerRect (const FCObjGraph & layer, RECT_F & rcCanvas) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	::OffsetRect_F (&rcCanvas, -ptLayer.x, -ptLayer.y) ;
}
inline void  FCObjCanvas::LayerToCanvasRect (const FCObjGraph & layer, RECT_F & rcLayer) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	::OffsetRect_F (&rcLayer, ptLayer.x, ptLayer.y) ;
}
inline void  FCObjCanvas::CanvasToLayerPoint (const FCObjGraph & layer, POINT_F & ptCanvas) const {
	POINT	ptLayer = layer.GetGraphObjPos() ;
	ptCanvas.x -= ptLayer.x ; ptCanvas.y -= ptLayer.y ;
}
inline void  FCObjCanvas::LayerToCanvasPoint (const FCObjGraph & layer, POINT_F & ptLayer) const {
	POINT	ptLayerPos = layer.GetGraphObjPos() ;
	ptLayer.x += ptLayerPos.x ; ptLayer.y += ptLayerPos.y ;
}
//=============================================================================
inline  FCObjSelect & FCObjCanvas::GetSelection () {
	return m_CurrSel ;
}
inline bool  FCObjCanvas::HasSelected () const {
	return m_CurrSel.HasSelected() ;
}
inline BOOL  FCObjCanvas::PtInSelection (int x, int y) const {
	return (BOOL)m_CurrSel.PtInSelection (x, y) ;
}
inline void  FCObjCanvas::__ClearSelection () {
	m_CurrSel.SetEmpty () ;
}
inline void  FCObjCanvas::__SetRegionMember (const FCObjSelect & select) {
	m_CurrSel = select ;
}
//=============================================================================
inline void  FCObjCanvas::ClearUndoRedoList () {
	this->ClearUndoList () ; this->ClearRedoList () ;
}
inline bool  FCObjCanvas::IsRedoEnable () const {
	return !m_RedoList.empty () ;
}
inline bool  FCObjCanvas::IsUndoEnable () const {
	return !m_UndoList.empty () ;
}
inline int  FCObjCanvas::GetUndoLevel () const {
	return m_nUndoLevel ;
}
//=============================================================================
inline deque<FCObjLayer *>*  FCObjCanvas::__GetLayerListPtr () {
	return &m_LayerList ;
}

#endif
