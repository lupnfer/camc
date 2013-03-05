#pragma once
#include "objbase.h"
#include "cxcore.h"

const float perimScale=100;
class CCAMCDoc;
class CEdgeDetection :
	public FCObject
{
public:
	CEdgeDetection(void);
	~CEdgeDetection(void);

private:
	CvMemStorage* g_storage;
public:
	CvScalar	  m_ctColor;
	int           m_ctLineWidth;
public:
	//void AllocateImages(IplImage* pImg);
	IplImage* EdgeDetectionByCanny(IplImage* pImg);
	IplImage* EdgeDetectionBySegment(IplImage* pImg);
	CvSeq*	  FindContour(IplImage *mask);
public:
	
};
