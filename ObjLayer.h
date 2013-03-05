//////////////////////////////////////////////////////////////////
//																//
//		��; : ͼ���ࣨcanvas�ϵĶ���							//
//		���� : [Foolish] / 2003-3-30							//
//		���� : 2004-2-1											//
//		��ҳ : http://www.crazy-bit.com/						//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= ����	//
//																//
//		˵�� :													//
//			1 . layer������32λɫimage							//
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
	FCObjCanvas     * pParentCanvas ; // ����canvas
	BOOL            m_bLayerVisible ; // ͼ���Ƿ�ɼ�
	BOOL            m_bLayerLimited ; // ͼ���Ƿ����ޣ���alpha 0�����Ƿ�����ɫ
	int             m_nAlphaPercent ; // ͼ��͸����, �������ϵ��[0,100]
private:

public:
	BOOL            m_rv;              //�Ƿ��ɾ��
} ;
//=============================================================================
//	text image layer class
//=============================================================================
class FCObjTextLayer : public FCObjLayer
{
public :
	// ��ʼ��Ϊ����ͼ�㣬���ı�alpha
	FCObjTextLayer () : m_pszText(NULL) {m_bLayerLimited = TRUE; m_bToolEdited = FALSE;}
	~FCObjTextLayer () ;
	virtual int  Serialize (BOOL bSave, BYTE * pSave) ; // save/load text layer

	// ʹ��ǰҪ����::_tsetlocale (LC_ALL, _T(".ACP")) ;
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
	TCHAR		* m_pszText ; // ָ��0��β���ַ���
	LOGFONT		m_LogFont ; // ����
	BOOL		m_bAddShadow ; // �Ƿ����Ӱ
	SHADOWDATA	m_ShadowData ; // ��Ӱ����
	BOOL		m_bToolEdited ; // �����tool�༭����ͼ�㲻��ͨ��create�����ˣ��ɱ�Ϊ��ͨͼ��
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
