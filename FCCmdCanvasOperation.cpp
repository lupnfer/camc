#include "StdAfx.h"
#include ".\fccmdcanvasoperation.h"

//=================================================================================
// ������������
void  FCCmdCanvasOperation::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	// ע�������������ѹջ˳��
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