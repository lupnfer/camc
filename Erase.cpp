#include "StdAfx.h"
#include ".\erase.h"

CErase* CErase::_instance = NULL;

CErase* CErase::Instance()
{
	if(_instance==NULL){
		_instance=new CErase();
	}
	_instance->Initial();
	return _instance;
}
CErase::CErase(void)
{
}

CErase::~CErase(void)
{
}
void CErase::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	

}
