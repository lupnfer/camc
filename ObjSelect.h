//////////////////////////////////////////////////////////////////
//																//
//		用途 : 区域选择对象										//
//		创建 : [Foolish] / 2003-4-8								//
//		更新 : 2003-12-17										//
//		主页 : http://www.crazy-bit.com/						//
//		邮箱 : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= 付黎	//
//		说明 :													//
//      1) 区域位图：0xFF代表选中，0代表未选					//
//      2) 原先采用Win32 HRGN实现，在实现magic wand时遇阻，		//
//         于2003-6-6改为用位图mask实现							//
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
	// 魔术棒/Magic Wand，(x,y)为点中img上的坐标
	BOOL	SetMagicWand (int x, int y, int nTolerance, const FCObjImage & img, BOOL bContinuous, RGN_TYPE nType = FCObjSelect::RGN_CREATE) ;

	// Property
	bool	HasSelected () const ;
	bool	PtInSelection (const POINT & ptTest) const ; // ptTest为canvas坐标
	bool	PtInSelection (int x, int y) const ; // (x,y)为canvas坐标
	bool	PtInEdge(int x,int y);
	// Selection operation
	void	SelectionExpand (int nPixel) ; // 扩张selection边界
	void	SelectionShrink (int nPixel) ; // 收缩selection边界
	void	SelectionBorder (int nSpan) ; // 边界
	void	SelectionInvert (const SIZE & sizeCanvas) ; // 反选区域
	void	SelectionSmooth (int nStep) ; // 平滑区域
	void	SelectionOptimize () ; // 去掉周围未选取的点

	// Draw the region
	void	DrawStep (HDC hdc, const FCObjCanvas &canvas, const RECT &rcViewCanvas) ;

public :
	void	ResetEdge () ; // 清空边界点list
	void	GetEdgePointList (std::deque<POINT> & ptList) const ; // 获得边界点list

    void    HandleSelection (const FCObjSelect & selectionNew, RGN_TYPE nType) ; // 处理新区域
	void    AddSelection (const FCObjSelect & sel) ; // 加到当前区域
	void    SubSelection (const FCObjSelect & sel) ; // 从当前区域中清除

	// 根据内部的m_nZoom计算区域的边界点，放于m_ptEdge中
	void	RecalculateEdge () ;

	// 种子填充（使用队列实现，绝不可用递归实现）,(x,y)为img上的坐标
	FCObjImage *  SeedFill (int x, int y, int nTolerance, const FCObjImage & img) ;
	BOOL	IsInTolerance (RGBQUAD crCurr, RGBQUAD crOrigin, int nTolerance) const ;
protected :
	int			m_nZoom ;
	int			m_nCurAnt ; // [0..7]
	std::deque<POINT>   m_ptEdge ; // 经过缩放的边界点
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
