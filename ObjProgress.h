//////////////////////////////////////////////////////////////////
//																//
//		用途 : 进度												//
//		创建 : [Foolish] / 2003-4-10							//
//		更新 : 2003-6-27										//
//		主页 : http://www.crazy-bit.com/						//
//		邮箱 : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= 付黎	//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_OBJECT_PROGRESS_H__
#define	 __FOO_OBJECT_PROGRESS_H__
#include "ObjBase.h"
#include "ProgressBar.h"
//=============================================================================
//	Declaration
//=============================================================================
class FCObjProgress : public FCObject
{
public :
	FCObjProgress () : m_nProgress(0) {
		m_pbar.Create(_T("进度"),20,100);
	} ;

	// operations (override)
	virtual void   ResetProgress () ;
	virtual int    GetProgress () const ;
	virtual void   SetProgress (int nNew) ;
protected :
	int		m_nProgress ;
public:
	CProgressBar m_pbar;
} ;

//=============================================================================
//	inline implement
//=============================================================================
inline void  FCObjProgress::ResetProgress () {
	this->SetProgress (0) ;
}
inline int  FCObjProgress::GetProgress () const {
	return m_nProgress ;
}
inline void  FCObjProgress::SetProgress (int nNew) {
//    return ::InterlockedExchange ((LONG *)&m_nProgress, nNew) ;
	
    m_nProgress = nNew ;
	m_pbar.SetPos(m_nProgress);

}

#endif
