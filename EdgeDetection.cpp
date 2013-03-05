#include "StdAfx.h"
#include "EdgeDetection.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "cv_yuv_codebook.h"
#include "StorageManage.h"

CEdgeDetection::CEdgeDetection(void)
{
	g_storage=NULL;
	m_ctColor=CV_CVX_WHITE;
	m_ctLineWidth=1; 
}

CEdgeDetection::~CEdgeDetection(void)
{	
	if(g_storage)
		cvReleaseMemStorage(&g_storage);
}
IplImage* CEdgeDetection::EdgeDetectionByCanny(IplImage* pImg)
{
	if(pImg==NULL)
		return NULL;	
	IplImage* pEdge=AutoStorage->CreateIplImage(pImg->width,pImg->height,IPL_DEPTH_8U,1);
	IplImage* pImgDst=AutoStorage->CreateIplImage(pImg->width,pImg->height,IPL_DEPTH_8U,1);
	cvCvtColor(pImg,pImgDst,CV_BGR2GRAY);
	cvCanny(pImgDst,pEdge,10,50);
	AutoStorage->ReleaseIplImage(pImgDst);
	return pEdge;
}
IplImage* CEdgeDetection::EdgeDetectionBySegment(IplImage* pImg)
{
	return NULL;
}
CvSeq* CEdgeDetection::FindContour(IplImage *mask)
{
	if(mask==NULL)
		return NULL;
	if( g_storage==NULL ) g_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(g_storage);
	
	CvContourScanner scanner = cvStartFindContours(mask,g_storage,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	CvSeq* c;
	int numCont = 0;
	while( (c = cvFindNextContour( scanner )) != NULL )
	{
		double len = cvContourPerimeter( c );
		double q = (mask->height + mask->width) /perimScale;   //calculate perimeter len threshold
		if( len < q ) //Get rid of blob if it's perimeter is too small
		{
			cvSubstituteContour( scanner, NULL );
		}
		else //Smooth it's edges if it's large enough
		{
			numCont++;
		}
	}
	CvSeq* pContour= cvEndFindContours( &scanner );	
	cvZero(mask);
	for( c=pContour; c != NULL; c = c->h_next )
	{
		cvDrawContours(mask,c,CV_CVX_WHITE, CV_CVX_WHITE,100,m_ctLineWidth,8);
	}
	//cvDrawContours(mask,m_pContour,CV_CVX_WHITE, CV_CVX_BLAC K,-1,1,8);
	return pContour;
}
