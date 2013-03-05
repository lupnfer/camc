#pragma once

class FCCmdArtPrider;
//***************************************************************************/
/*								 ͼ��Ĳ���									*/
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
// ����ͼ��/stretch layer
class FCCmdLayerStretch : public FCCmdLayerOperation
{
public :
	FCCmdLayerStretch (FCObjLayer * pLayer, int nNewW, int nNewH) : m_nNewWidth(nNewW), m_nNewHeight(nNewH) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int		m_nNewWidth, m_nNewHeight ;
} ;
//=============================================================================
// ��תͼ��/rotate layer
class FCCmdLayerRotate : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate (FCObjLayer * pLayer, int nAngle) : m_nAngle(nAngle) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	int		m_nAngle ;
} ;
//=============================================================================
// ���б���/slope transform
class FCCmdLayerSlope : public FCCmdLayerOperation
{
public :
	FCCmdLayerSlope (FCObjLayer * pLayer, const POINT ptPos[4]) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	POINT		m_ptNewPos[4] ; // ���ϡ����ϡ����¡�����
} ;
//=============================================================================
// ͸������/lens transform
class FCCmdLayerLens : public FCCmdLayerOperation
{
public :
	FCCmdLayerLens (FCObjLayer * pLayer, const POINT ptPos[4]) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	POINT		m_ptNewPos[4] ; // ���ϡ����ϡ����¡�����
} ;
//=============================================================================
// ˳ʱ����ת90��/clockwise rotate 90'
class FCCmdLayerRotate90 : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate90 (FCObjLayer * pLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
} ;
//=============================================================================
// ��ʱ����ת90��/clockwise rotate 270'
class FCCmdLayerRotate270 : public FCCmdLayerOperation
{
public :
	FCCmdLayerRotate270 (FCObjLayer * pLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
} ;
//=============================================================================
// �ü�ͼ��/crop layer
class FCCmdLayerCrop : public FCCmdLayerOperation
{
public :
	FCCmdLayerCrop (FCObjLayer * pLayer, FCObjImage * pImgMask8, const RECT & rcLayer) : m_pImgMask(pImgMask8), m_rcLayer(rcLayer) {m_pLayer = pLayer;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	FCObjImage		* m_pImgMask ; // 8λɫalphaλͼ
	RECT			m_rcLayer ; // ���ü���layer����
} ;
//=============================================================================
// ��FCCmdCanvasCrop�õĲü������߼�
class __FCCmdCanvasCrop_Layer : public FCCmdLayerOperation
{
public :
	__FCCmdCanvasCrop_Layer (FCObjLayer * pLayer, const RECT & rcCanvas) : m_rcCanvas(rcCanvas) {m_pLayer = pLayer; m_bSaveSelection = FALSE;}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	RECT		m_rcCanvas ; // ���ü�canvas����
} ;
//=============================================================================
// �ƶ�ͼ��/move layer
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
// ���ͼ��/add new layer to canvas
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
// ɾ��ͼ��/remove layer from canvas
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
// ����ͼ������/set layer property
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
// �ı�ͼ���˳��/change layer's sequence in canvas
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
// ����ͼ������/save layer's rect region
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
// ����ͼ������/set layer property
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