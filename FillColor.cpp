#include "StdAfx.h"
#include ".\fillcolor.h"
#include "CmdImgFillColor.h"


CFillColor* CFillColor::_instance = NULL;

CFillColor* CFillColor::Instance()
{
	if(_instance==NULL){
		_instance=new CFillColor();
	}
	_instance->Initial();
	return _instance;
}

CFillColor::CFillColor(void)
{
}

CFillColor::~CFillColor(void)
{
}
void CFillColor::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
}
void CFillColor::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	
}

