#include "StdAfx.h"
#include ".\fccmdcanvasrotate.h"
#include "math.h"
#include "LayerCommand.h"
//=================================================================================
// ��ת����/rotate canvas
FCCmdCanvasRotate::FCCmdCanvasRotate (const FCObjCanvas &rCanvas, int nAngle)
{
	// ���㻭���³ߴ�
	SIZE		OldSize = rCanvas.GetCanvasDimension() ;
	int			nTmpAng = FMax (0, nAngle % 180) ;
	double		fSin = sin(AngleToRadian(nTmpAng % 90)),
		fCos = cos(AngleToRadian(nTmpAng % 90)) ;
	// ע������Ŀ�߼���
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

		// ������λ��
		// ��������㣬���ƶ�������ת����Ϊ��תʱ�������ĵ���ת
		POINT		ptNew = pLayer->GetGraphObjPos() ;
		ptNew.x += (m_NewCanvasSize.cx-OldSize.cx)/2 ;
		ptNew.y += (m_NewCanvasSize.cy-OldSize.cy)/2 ;
		PushImgCommand (new FCCmdLayerMove (pLayer, ptNew)) ;

		PushImgCommand (new FCCmdLayerRotate (pLayer, nAngle)) ;
	}
}