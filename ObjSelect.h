//////////////////////////////////////////////////////////////////
//																//
//		��; : ����ѡ�����										//
//		���� : [Foolish] / 2003-4-8								//
//		���� : 2003-12-17										//
//		��ҳ : http://www.crazy-bit.com/						//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= ����	//
//		˵�� :													//
//      1) ����λͼ��0xFF����ѡ�У�0����δѡ					//
//      2) ԭ�Ȳ���Win32 HRGNʵ�֣���ʵ��magic wandʱ���裬		//
//         ��2003-6-6��Ϊ��λͼmaskʵ��							//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_OBJECT_GRAPH_SELECT_H__
#define	 __FOO_OBJECT_GRAPH_SELECT_H__
#include "ObjImage.h"
#include <deque>
class FCObjCanvas ; // external class

//===================================================================
//	Declaration
//===================================================================
class FCObjSelect : public FCObjImage
{
public :
	enum RGN_TYPE {RGN_CREATE, RGN_ADD, RGN_SUB} ; // handle between current and new selection
public :
	// Initialization
	FCObjSelect () ;
	FCObjSelect (const FCObjSelect & sel) ;
	//FCObjSelect & operator= (const FCObjSelect & sel) ;

	// Set Operations
	void	SetEmpty () ;
	BOOL	SetRect (const RECT & rect, RGN_TYPE nType = FCObjSelect::RGN_CREATE) ;
	BOOL	SetElliptic (const RECT & rect, const int angle,RGN_TYPE nType = FCObjSelect::RGN_CREATE) ;
	//BOOL    Set
	BOOL	SetPolygon (const POINT * ppt, int cNumPoint, RGN_TYPE nType = FCObjSelect::RGN_CREATE) ;
	// ħ����/Magic Wand��(x,y)Ϊ����img�ϵ�����
	BOOL	SetMagicWand (int x, int y, int nTolerance, const FCObjImage & img, BOOL bContinuous, RGN_TYPE nType = FCObjSelect::RGN_CREATE) ;

	// Property
	bool	HasSelected () const ;
	bool	PtInSelection (const POINT & ptTest) const ; // ptTestΪcanvas����
	bool	PtInSelection (int x, int y) const ; // (x,y)Ϊcanvas����
	bool	PtInEdge(int x,int y);
	// Selection operation
	void	SelectionExpand (int nPixel) ; // ����selection�߽�
	void	SelectionShrink (int nPixel) ; // ����selection�߽�
	void	SelectionBorder (int nSpan) ; // �߽�
	void	SelectionInvert (const SIZE & sizeCanvas) ; // ��ѡ����
	void	SelectionSmooth (int nStep) ; // ƽ������
	void	SelectionOptimize () ; // ȥ����Χδѡȡ�ĵ�

	// Draw the region
	void	DrawStep (HDC hdc, const FCObjCanvas &canvas, const RECT &rcViewCanvas) ;

public :
	void	ResetEdge () ; // ��ձ߽��list
	void	GetEdgePointList (std::deque<POINT> & ptList) const ; // ��ñ߽��list

    void    HandleSelection (const FCObjSelect & selectionNew, RGN_TYPE nType) ; // ����������
	void    AddSelection (const FCObjSelect & sel) ; // �ӵ���ǰ����
	void    SubSelection (const FCObjSelect & sel) ; // �ӵ�ǰ���������

	// �����ڲ���m_nZoom��������ı߽�㣬����m_ptEdge��
	void	RecalculateEdge () ;

	// ������䣨ʹ�ö���ʵ�֣��������õݹ�ʵ�֣�,(x,y)Ϊimg�ϵ�����
	FCObjImage *  SeedFill (int x, int y, int nTolerance, const FCObjImage & img) ;
	BOOL	IsInTolerance (RGBQUAD crCurr, RGBQUAD crOrigin, int nTolerance) const ;
protected :
	int			m_nZoom ;
	int			m_nCurAnt ; // [0..7]
	std::deque<POINT>   m_ptEdge ; // �������ŵı߽��
} ;

//===================================================================
// inline implement
//===================================================================
inline bool  FCObjSelect::HasSelected () const {
	return IsValidImage() ? true : false ;
}
inline bool	 FCObjSelect::PtInSelection (int x, int y) const {
	POINT    ptTest = {x, y} ;
	return PtInSelection(ptTest) ;
}
inline BOOL  FCObjSelect::IsInTolerance (RGBQUAD crCurr, RGBQUAD crOrigin, int nTolerance) const {
	return ((abs(crCurr.rgbBlue - crOrigin.rgbBlue) <= nTolerance) &&
			(abs(crCurr.rgbGreen - crOrigin.rgbGreen) <= nTolerance) &&
			(abs(crCurr.rgbRed - crOrigin.rgbRed) <= nTolerance) &&
			(crCurr.rgbReserved == crOrigin.rgbReserved)) ;
}

#endif
