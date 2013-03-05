#include "StdAfx.h"
#include ".\fccmdcanvasrotate.h"
#include "math.h"
#include "LayerCommand.h"
//=================================================================================
// 旋转画布/rotate canvas
FCCmdCanvasRotate::FCCmdCanvasRotate (const FCObjCanvas &rCanvas, int nAngle)
{
	// 计算画布新尺寸
	SIZE		OldSize = rCanvas.GetCanvasDimension() ;
	int			nTmpAng = FMax (0, nAngle % 180) ;
	double		fSin = sin(AngleToRadian(nTmpAng % 90)),
		fCos = cos(AngleToRadian(nTmpAng % 90)) ;
	// 注意这里的宽高计算
	if (nTmpAng >= 90)
	{
		m_NewCanvasSize.cx = FRound (OldSize.cx * fSin + OldSize.cy * fCos) ;
		m_NewCanvasSize.cy = FRound (OldSize.cy * fSin + OldSize.cx * fCos) ;
	}
	else
	{
		m_NewCanvasSize.cx = FRound (OldSize.cx * fCos + OldSize.cy * fSin) ;
		m_NewCanvasSize.cy = FRound (OldSize.cy * fCos + OldSize.cx * fSin) ;
	}
	for (int i=0 ; i < rCanvas.GetLayerNumber() ; i++)
	{
		FCObjLayer    * pLayer = rCanvas.GetLayer(i) ;
		if (pLayer == NULL)
			continue ;

		// 计算新位置
		// 这里特殊点，先移动，后旋转，因为旋转时会绕中心点旋转
		POINT		ptNew = pLayer->GetGraphObjPos() ;
		ptNew.x += (m_NewCanvasSize.cx-OldSize.cx)/2 ;
		ptNew.y += (m_NewCanvasSize.cy-OldSize.cy)/2 ;
		PushImgCommand (new FCCmdLayerMove (pLayer, ptNew)) ;

		PushImgCommand (new FCCmdLayerRotate (pLayer, nAngle)) ;
	}
}