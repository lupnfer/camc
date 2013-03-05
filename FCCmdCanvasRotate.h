#pragma once
#include "fccmdcanvasoperation.h"

class FCCmdCanvasRotate :
	public FCCmdCanvasOperation
{
public :
	FCCmdCanvasRotate (const FCObjCanvas &rCanvas, int nAngle) ;
};
