#pragma once

class FCCmdArtPrider;
//***************************************************************************/
/*								 图层的操作									*/
//***************************************************************************/
class FCCmdLayerOperation : public FCCmdArtPrider
{
public :
	FCCmdLayerOperation () {m_bSaveSelection = TRUE; m_pLayer = NULL;}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) {}
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	FCObjLayer		m_UndoLayer, m_RedoLayer ;
	FCObjSelect		m_Selection ;
	BOOL			m_bSaveSelection ;
	FCObjLayer		* m_pLayer ;
	FCObjCanvas		* m_pCanvas ;
} ;
//=============================================================================
// 缩放图层/stretch layer
class FCCmdLayerStretch : public FCCmdLayerOperation
{
public :
	FCCmdLayerStretch (FCObjLayer * pLayer, int nNewW, int nNewH) : m_nNewWidth(nNewW), m_nNewHeight(nNewH) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int		m_nNewWidth, m_nNewHeight ;
} ;
//=============================================================================
// 旋转图层/rotate layer
class FCCmdLayerRotate : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate (FCObjLayer * pLayer, int nAngle) : m_nAngle(nAngle) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int		m_nAngle ;
} ;
//=============================================================================
// 错切变形/slope transform
class FCCmdLayerSlope : public FCCmdLayerOperation
{
public :
	FCCmdLayerSlope (FCObjLayer * pLayer, const POINT ptPos[4]) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	POINT		m_ptNewPos[4] ; // 左上、右上、右下、左下
} ;
//=============================================================================
// 透镜变形/lens transform
class FCCmdLayerLens : public FCCmdLayerOperation
{
public :
	FCCmdLayerLens (FCObjLayer * pLayer, const POINT ptPos[4]) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	POINT		m_ptNewPos[4] ; // 左上、右上、右下、左下
} ;
//=============================================================================
// 顺时针旋转90度/clockwise rotate 90'
class FCCmdLayerRotate90 : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate90 (FCObjLayer * pLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
} ;
//=============================================================================
// 逆时针旋转90度/clockwise rotate 270'
class FCCmdLayerRotate270 : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate270 (FCObjLayer * pLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
} ;
//=============================================================================
// 裁减图层/crop layer
class FCCmdLayerCrop : public FCCmdLayerOperation
{
public :
	FCCmdLayerCrop (FCObjLayer * pLayer, FCObjImage * pImgMask8, const RECT & rcLayer) : m_pImgMask(pImgMask8), m_rcLayer(rcLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	FCObjImage		* m_pImgMask ; // 8位色alpha位图
	RECT			m_rcLayer ; // 欲裁减的layer区域
} ;
//=============================================================================
// 给FCCmdCanvasCrop用的裁剪单层逻辑
class __FCCmdCanvasCrop_Layer : public FCCmdLayerOperation
{
public :
	__FCCmdCanvasCrop_Layer (FCObjLayer * pLayer, const RECT & rcCanvas) : m_rcCanvas(rcCanvas) {m_pLayer = pLayer; m_bSaveSelection = FALSE;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	RECT		m_rcCanvas ; // 欲裁剪canvas区域
} ;
//=============================================================================
// 移动图层/move layer
class FCCmdLayerMove : public FCCmdArtPrider
{
public :
	FCCmdLayerMove (FCObjLayer * pLayer, const POINT & NewPoint) : m_ptOld(NewPoint) {m_pLayer = pLayer;}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
public :
	FCObjLayer   * GetLayerPtr () const {return m_pLayer ;}
protected :
	POINT		m_ptOld ;
	FCObjLayer	* m_pLayer ;
} ;
//=============================================================================
// 添加图层/add new layer to canvas
class FCCmdLayerAdd : public FCCmdArtPrider
{
public :
	FCCmdLayerAdd (FCObjLayer * pAddLayer, int nPos) : m_nPos(nPos) {
		m_pAddLayer = pAddLayer;
	}
	~FCCmdLayerAdd () ;
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	int			m_nPos ;
	FCObjLayer	* m_pAddLayer ;
	FCObjCanvas	* m_pCanvas ;
} ;
//=============================================================================
// 删除图层/remove layer from canvas
class FCCmdLayerRemove : public FCCmdArtPrider
{
public :
	FCCmdLayerRemove (FCObjLayer * pRemoveLayer) {m_pRemoveLayer = pRemoveLayer;}
	~FCCmdLayerRemove () ;
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	int			m_nPos ;
	FCObjLayer	* m_pRemoveLayer ;
	FCObjCanvas	* m_pCanvas ;
} ;
//=============================================================================
// 设置图层属性/set layer property
class FCCmdLayerSetProperty : public FCCmdArtPrider
{
public :
	FCCmdLayerSetProperty (FCObjLayer * pLayer, int nNewTransparent, BOOL bVisible, BOOL bLimited) : m_nNewTransparent(nNewTransparent), m_bVisible(bVisible), m_bLimited(bLimited) {m_pLayer = pLayer;}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	int		m_nNewTransparent ;
	BOOL	m_bVisible ;
	BOOL	m_bLimited ;
	FCObjLayer	* m_pLayer ;
} ;
//=============================================================================
// 改变图层的顺序/change layer's sequence in canvas
class FCCmdLayerExchange : public FCCmdArtPrider
{
public :
    FCCmdLayerExchange (const std::deque<FCObjLayer *> & NewList) : m_LayerList(NewList) {}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
    std::deque<FCObjLayer *>    m_LayerList ;
	FCObjCanvas                 * m_pCanvas ;
} ;
//=============================================================================
// 保存图层区域/save layer's rect region
class FCCmdLayerPenSaveRect : public FCCmdArtPrider
{
public :
	FCCmdLayerPenSaveRect (FCObjLayer * pLayer, const FCObjImage &ImgUndo, const RECT &rcSave) : m_rcSave(rcSave), m_pLayer(pLayer), m_imgUndo(ImgUndo) {}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	FCObjLayer	* m_pLayer ;
	RECT		m_rcSave ;
	FCObjImage	m_imgUndo, m_imgRedo ;
} ;
//=============================================================================
// 设置图层属性/set layer property
class FCCmdContourLayerSetProperty : public FCCmdArtPrider
{
public :
	FCCmdContourLayerSetProperty (FCObjLayer * pLayer, int nNewTransparent, BOOL bVisible, BOOL bLimited) : m_nNewTransparent(nNewTransparent), m_bVisible(bVisible), m_bLimited(bLimited) {m_pLayer = pLayer;}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	int		m_nNewTransparent ;
	BOOL	m_bVisible ;
	BOOL	m_bLimited ;
	FCObjLayer	* m_pLayer ;
} ;