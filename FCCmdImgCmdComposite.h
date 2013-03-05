#pragma once
#include "objbase.h"
#include <deque>

class FCCmdImgCmdComposite :
	public FCCmdArtPrider
{
public :
	~FCCmdImgCmdComposite() ;
	void  PushImgCommand (FCCmdArtPrider * cmd) ;
	virtual BOOL  IsNeedComposite (FCCmdArtPrider * cmd) {return FALSE ;}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	std::deque<FCCmdArtPrider*>     m_cmdList ;

};
