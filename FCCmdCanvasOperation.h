#pragma once
#include "fccmdimgcmdcomposite.h"
#include "ObjCanvas.h"

class FCCmdCanvasOperation :
	public FCCmdImgCmdComposite
{
public :
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	SIZE		m_OldCanvasSize, m_NewCanvasSize ; // 必须在构造时初始化m_NewCanvasSize
	FCObjCanvas	* m_pCanvas ;
};
