
#ifndef	 __FOO_OBJECT_BASE_H__
#define	 __FOO_OBJECT_BASE_H__
#include "StdDefine.h"

class FCObject ;
class FCObjGraph ;
class FCCommand ;
class FCObjCanvas;
class FCObjProgress;
//=============================================================================
//	the root of all objects classes
//=============================================================================
class FCObject
{
public :
	virtual ~FCObject () {}
	// save/load object's property, return written/loaded bytes
	virtual int  Serialize (BOOL bSave, BYTE * pSave) {return 0;}
} ;
//=============================================================================
//	base class of graphic objects
//=============================================================================
class FCObjGraph : public FCObject
{
public :
	FCObjGraph () ;
	FCObjGraph & operator= (const FCObjGraph & GraphObj) ;
	virtual int  Serialize (BOOL bSave, BYTE * pSave) ; // save/load position

	void  SetGraphObjPos (int xPos, int yPos) ;
	void  SetGraphObjPos (const POINT & pos) ;
	POINT  GetGraphObjPos () const ;
	void  OffsetGraphObj (int xPos, int yPos) ;

protected :
	POINT		m_ObjPos ; // object position on canvas
} ;
//=============================================================================
//	base command class
//=============================================================================
class FCCommand
{
public :
	virtual ~FCCommand () {}
	virtual void  Undo () = 0 ; // derived class must implement it
	virtual void  Redo () = 0 ; // derived class must implement it
} ;
class FCCmdArtPrider : public FCCommand
{
public :
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) =0 ;
	virtual void  Undo () =0 ;
	virtual void  Redo () =0 ;
} ;
//=============================================================================
//	inline implement
//=============================================================================
inline  FCObjGraph::FCObjGraph () {
	SetGraphObjPos (0, 0) ;
}
inline void  FCObjGraph::SetGraphObjPos (int xPos, int yPos) {
	m_ObjPos.x = xPos ; m_ObjPos.y = yPos ;
}
inline void  FCObjGraph::SetGraphObjPos (const POINT &pos) {
	m_ObjPos = pos ;
}
inline POINT  FCObjGraph::GetGraphObjPos () const {
	return m_ObjPos ;
}
inline void  FCObjGraph::OffsetGraphObj (int xPos, int yPos) {
	m_ObjPos.x += xPos ; m_ObjPos.y += yPos ;
}
inline FCObjGraph & FCObjGraph::operator= (const FCObjGraph &GraphObj) {
	SetGraphObjPos (GraphObj.GetGraphObjPos()) ;
	return *this ;
}
inline int  FCObjGraph::Serialize (BOOL bSave, BYTE * pSave) {
	if (bSave)
		*(POINT*)pSave = m_ObjPos ;
	else
		m_ObjPos = *(POINT*)pSave ;
	return sizeof(m_ObjPos) ;
}

#endif
