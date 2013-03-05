#include "StdAfx.h"
#include ".\zoomout.h"


CZoomOut* CZoomOut::_instance = NULL;

CZoomOut* CZoomOut::Instance()
{
	if(_instance==NULL){
		_instance=new CZoomOut();
	}
	_instance->Initial();
	return _instance;
}
CZoomOut::CZoomOut(void)
{
}

CZoomOut::~CZoomOut(void)
{
}
void CZoomOut::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	CCAMCDoc* pDoc = pView->GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;	
	pDoc->OnZoomout();
	pDoc->UpdateAllViews(NULL);


}
