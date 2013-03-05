#include "StdAfx.h"
#include ".\zoomin.h"

CZoomIn* CZoomIn::_instance = NULL;

CZoomIn* CZoomIn::Instance()
{
	if(_instance==NULL){
		_instance=new CZoomIn();
	}
	_instance->Initial();
	return _instance;
}
CZoomIn::CZoomIn(void)
{

}

CZoomIn::~CZoomIn(void)
{

}
void CZoomIn::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	CCAMCDoc* pDoc = pView->GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;	



	pDoc->OnZoomin();
	pDoc->UpdateAllViews(NULL);
	

}





