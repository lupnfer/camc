#include "StdAfx.h"
#include ".\fccmdcanvasoperation.h"

//=================================================================================
// 画布操作基类
void  FCCmdCanvasOperation::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	// 注意派生类的命令压栈顺序
	m_pCanvas = &canvas ;
	m_OldCanvasSize = canvas.GetCanvasDimension() ;
	FCCmdImgCmdComposite::Execute (canvas, Percent) ;
	m_pCanvas->SetCanvasDimension (m_NewCanvasSize) ;
}
void  FCCmdCanvasOperation::Undo ()
{
	FCCmdImgCmdComposite::Undo() ;
	m_pCanvas->SetCanvasDimension (m_OldCanvasSize) ;
}
void  FCCmdCanvasOperation::Redo ()
{
	FCCmdImgCmdComposite::Redo() ;
	m_pCanvas->SetCanvasDimension (m_NewCanvasSize) ;
}