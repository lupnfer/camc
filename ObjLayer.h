//////////////////////////////////////////////////////////////////
//																//
//		用途 : 图层类（canvas上的对象）							//
//		创建 : [Foolish] / 2003-3-30							//
//		更新 : 2004-2-1											//
//		主页 : http://www.crazy-bit.com/						//
//		邮箱 : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= 付黎	//
//																//
//		说明 :													//
//			1 . layer必须是32位色image							//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_OBJECT_LAYER_H__
#define	 __FOO_OBJECT_LAYER_H__
#include "ObjImage.h"


class FCObjCanvas ; // external class
class FCObjProgress;
class FCObjLayer ; // image layer
class FCObjTextLayer ; // text image layer

//=============================================================================
//	image layer class
//=============================================================================
class FCObjLayer : public FCObjImage
{
public :
	FCObjLayer () ;
	
    FCObjLayer (const FCObjLayer & layer) {*this = layer ;}
	virtual ~FCObjLayer(){}
	FCObjLayer & operator= (const FCObjLayer & layer) ;
	virtual int  Serialize (BOOL bSave, BYTE * pSave) ; // save/load layer

	// Parent canvas
    void	SetParentCanvas (FCObjCanvas * pCanvas) {pParentCanvas = pCanvas ;}
    FCObjCanvas *  GetParentCanvas () const {return pParentCanvas ;}

	// Layer visible property
    BOOL	GetLayerVisible () const {return m_bLayerVisible ;}
    void	SetLayerVisible (BOOL bVisible) {m_bLayerVisible = bVisible ;}
    void	FlipLayerVisible () {m_bLayerVisible = !m_bLayerVisible ;}

	// Layer transparent property
    void	SetLayerTransparent (int nNewTransparent) {m_nAlphaPercent = FClamp (nNewTransparent, 0, 100) ;}
    int		GetLayerTransparent () const {return m_nAlphaPercent ;}

	// is limited
    void	SetLayerLimitedFlag (BOOL bLimited) {m_bLayerLimited = bLimited ;}
    BOOL	IsLayerLimited () const {return m_bLayerLimited ;}

	
	
protected :
	FCObjCanvas     * pParentCanvas ; // 所属canvas
	BOOL            m_bLayerVisible ; // 图层是否可见
	BOOL            m_bLayerLimited ; // 图层是否有限，即alpha 0区域是否能着色
	int             m_nAlphaPercent ; // 图层透明度, 即乘这个系数[0,100]
private:

public:
	BOOL            m_rv;              //是否待删除
} ;
//=============================================================================
//	text image layer class
//=============================================================================
class FCObjTextLayer : public FCObjLayer
{
public :
	// 初始化为限制图层，不改变alpha
	FCObjTextLayer () : m_pszText(NULL) {m_bLayerLimited = TRUE; m_bToolEdited = FALSE;}
	~FCObjTextLayer () ;
	virtual int  Serialize (BOOL bSave, BYTE * pSave) ; // save/load text layer

	// 使用前要调用::_tsetlocale (LC_ALL, _T(".ACP")) ;
	void	Create (HFONT hFont, const TCHAR * szText, const SIZE & sizeText,
					RGBQUAD crFont, BOOL bAddShadow, const SHADOWDATA & ShadowData) ;
	PTSTR	GetDrawText() const {return m_pszText;}
	void	GetTextFont (LOGFONT * pFont) const ;
	BOOL	IsAddShadow () const {return m_bAddShadow;}
	void	GetShadowData (SHADOWDATA * pShadow) const ;

	void	SetShadowFlag (BOOL bFlag) {m_bAddShadow = bFlag;}
	void	SetShadowData (const SHADOWDATA & ShadowDa) ;
	void	SetTextFont (const LOGFONT & logFont) ;

	BOOL	HasToolEdited () const ;
	void	SetToolEditFlag (BOOL bFlag) ;
protected :
	TCHAR		* m_pszText ; // 指向0结尾的字符串
	LOGFONT		m_LogFont ; // 字体
	BOOL		m_bAddShadow ; // 是否加阴影
	SHADOWDATA	m_ShadowData ; // 阴影参数
	BOOL		m_bToolEdited ; // 被别的tool编辑过的图层不能通过create创建了，蜕变为普通图层
} ;

//=============================================================================
//	inline Implement
//=============================================================================
inline FCObjTextLayer::~FCObjTextLayer () {
	if (m_pszText != NULL)
		delete[] m_pszText ;
}
//=============================================================================
inline void FCObjTextLayer::GetTextFont (LOGFONT * pFont) const {
	if (pFont)
		CopyMemory (pFont, &m_LogFont, sizeof(LOGFONT)) ;
}
//=============================================================================
inline void FCObjTextLayer::GetShadowData (SHADOWDATA * pShadow) const {
	if (pShadow)
		*pShadow = m_ShadowData ;
}
//=============================================================================
inline void FCObjTextLayer::SetShadowData (const SHADOWDATA & ShadowDa) {
	CopyMemory (&m_ShadowData, &ShadowDa, sizeof(m_ShadowData)) ;
}
inline void FCObjTextLayer::SetTextFont (const LOGFONT & logFont) {
	CopyMemory (&m_LogFont, &logFont, sizeof(m_LogFont)) ;
}
//=============================================================================
inline BOOL FCObjTextLayer::HasToolEdited () const {
	return m_bToolEdited ;
}
inline void FCObjTextLayer::SetToolEditFlag (BOOL bFlag) {
	m_bToolEdited = bFlag ;
}
//=============================================================================

#endif
