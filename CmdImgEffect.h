#pragma once
#include "objbase.h"
#include "ObjLayer.h"
#include <deque>
class CCmdImgEffect :public FCCmdArtPrider
{
public :
	CCmdImgEffect () : m_pLayer(NULL), m_pCanvas(NULL) {}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) =0 ;
public :
	// 底层访问函数
	FCObjCanvas *	GetCanvasPtr () const {return m_pCanvas ;}
	FCObjLayer *	GetLayerPtr () const {return m_pLayer ;}
protected :
	FCObjCanvas		* m_pCanvas ;
	FCObjLayer		* m_pLayer ;
	FCObjImage		m_Undo ;
	FCObjImage		m_Redo ;
};
//=============================================================================
// 图象卷积运算/image convolute (>= 24 bit)
class FCCmdImgConvolute : public CCmdImgEffect
{
public :
	// nElements为从矩阵左上角开始的数组, iBlockLen为矩阵宽度（正方形）
	FCCmdImgConvolute () {}
	FCCmdImgConvolute (int * nElements, int iBlockLen, int iDivisor, int nOffset = 0) ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	std::deque<int>    m_Element ;
	int                m_iBlock, m_iDivisor, m_nOffset ;
} ;
//=============================================================================