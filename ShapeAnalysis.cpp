#include "StdAfx.h"
#include "ShapeAnalysis.h"
#include "cv_yuv_codebook.h"
#include "MainFrm.h"
#include "FColor.h"
#include "CameraCalibration.h"
#include <iostream>
#include <vector>


using namespace std;
using namespace cv;

#define     NO_OBJECT       0  
#define     MIN(x, y)       (((x) < (y)) ? (x) : (y))  
#define     ELEM(img, r, c) (CV_IMAGE_ELEM(img, unsigned char, r, c))  
#define     ONETWO(L, r, c, col) (L[(r) * (col) + c])  

CShapeAnalysis::CShapeAnalysis(void)
{
	g_storage=NULL;
	m_pContour=NULL;
	m_ctLineWidth=1; 
	
}

CShapeAnalysis::~CShapeAnalysis(void)
{	
	if(g_storage)
		cvReleaseMemStorage(&g_storage);
}
CvSeq*  CShapeAnalysis::FindContour(IplImage *mask,BOOL approxPoly)
{
	if(mask==NULL)
		return NULL;
	if( g_storage==NULL ) g_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(g_storage);
	m_contourLength=0;
	CvContourScanner scanner = cvStartFindContours(mask,g_storage,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	CvSeq* c;
	while( (c = cvFindNextContour( scanner )) != NULL )
	{
		double len= cvContourPerimeter( c );
		double q = (mask->height + mask->width) /perimScale;   //calculate perimeter len threshold
		if( len < q ) //Get rid of blob if it's perimeter is too small
		{
			cvSubstituteContour( scanner, NULL );
		}
		else //Smooth it's edges if it's large enough
		{
			CvSeq* c_new;
			if(approxPoly) //Polygonal approximation of the segmentation
				c_new = cvApproxPoly(c,sizeof(CvContour),g_storage,CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL,0);
			cvSubstituteContour( scanner, c_new );
			len= cvContourPerimeter( c_new );
		}
		m_contourLength=m_contourLength+len;
	}
	m_pContour= cvEndFindContours( &scanner );	
	cvZero(mask);
	for( c=m_pContour; c != NULL; c = c->h_next )
	{
		cvDrawContours(mask,c,CV_CVX_WHITE, CV_CVX_WHITE,100,m_ctLineWidth,8);
	}
	//cvDrawContours(mask,m_pContour,CV_CVX_WHITE, CV_CVX_BLAC K,-1,1,8);
	return m_pContour;
}
//返回孔的个数
int CShapeAnalysis::CheckContourHoles()
{	
	if(m_pContour==NULL)
		return -1;
	CvSeq* ch,*cv;
	int num=0;
	for(ch=m_pContour;ch!=NULL;ch=ch->h_next){
		for(cv=ch->v_next;cv!=NULL;cv=cv->h_next)
			num++;
	}
	return num;
}
double CShapeAnalysis::CalcArea(IplImage* pMask)
{
	if(!pMask)
		return 0;
	double area=0;
	for(int y=0;y<pMask->height;++y){
		BYTE* pData=(BYTE*)(pMask->imageData+y*pMask->widthStep);
		for(int x=0;x<pMask->height;++x,++pData){
			if(*pData==0xFF)
				area++;
		}
	}
	return area;
}
CvBox2D CShapeAnalysis::PartsAngleDetection(CvSeq* pContour)
{
	if(pContour!=NULL){
		return cvMinAreaRect2(pContour,g_storage);
	}	
}
CvBox2D CShapeAnalysis::PartsAngleDetection(IplImage* pMask)
{
	CvBox2D box;
	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	
	/*cvShowImage("afsa",pMask);
	cvWaitKey(0);*/

	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	if(br.GetNumBlobs()==0){
		//AfxMessageBox("训练样本有误，连通检测出的Blob 没有或不止一个！");
		return box;
	}
	CBlob* pBlob=br.GetBlob(0);
	box=pBlob->GetMinBoundingRect();
	return box;
}
CvRect  CShapeAnalysis::PartsBoundingRect(IplImage* pMask)
{
	CvRect rc;
	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	if(br.GetNumBlobs()==0){
		//AfxMessageBox("训练样本有误，连通检测出的Blob 没有或不止一个！");
		return rc;
	}
	CBlob* pBlob=br.GetBlob(0);
	rc=pBlob->GetBoundingBox();
	return rc;
}

void  CShapeAnalysis::DrawMinBoundingRect(CvBox2D box2d,IplImage* pImgShow,BOOL flag)
{
	if(pImgShow==NULL)
		return;
	CvPoint2D32f pt2d[4];
	int npts[1]={4};
	cvBoxPoints(box2d,pt2d);
	CvPoint *pt=new CvPoint[4];
	for(int i=0;i<4;++i){
		pt[i].x=pt2d[i].x;
		pt[i].y=pt2d[i].y;
	}
	cvDrawPolyLine(pImgShow,&pt,npts,1,1,CV_RGB(255,0,0));
	delete pt;
}
void CShapeAnalysis::CalcPGH(CvHistogram* hist)
{
	//cvCalcpgh()
}
double
CShapeAnalysis::CompareSURFDescriptors( const float* d1, const float* d2, double best, int length )
{
	double total_cost = 0;
	assert( length % 4 == 0 );
	for( int i = 0; i < length; i += 4 )
	{
		double t0 = d1[i] - d2[i];
		double t1 = d1[i+1] - d2[i+1];
		double t2 = d1[i+2] - d2[i+2];
		double t3 = d1[i+3] - d2[i+3];
		total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
		if( total_cost > best )
			break;
	}
	return total_cost;
}


int 
CShapeAnalysis::naiveNearestNeighbor( const float* vec, int laplacian,
					 const CvSeq* model_keypoints,
					 const CvSeq* model_descriptors )
{
	int length = (int)(model_descriptors->elem_size/sizeof(float));
	int i, neighbor = -1;
	double d, dist1 = 1e6, dist2 = 1e6;
	CvSeqReader reader, kreader;
	cvStartReadSeq( model_keypoints, &kreader, 0 );
	cvStartReadSeq( model_descriptors, &reader, 0 );

	for( i = 0; i < model_descriptors->total; i++ )
	{
		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
		const float* mvec = (const float*)reader.ptr;
		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
		CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
		if( laplacian != kp->laplacian )
			continue;
		d = CompareSURFDescriptors( vec, mvec, dist2, length );
		if( d < dist1 )
		{
			dist2 = dist1;
			dist1 = d;
			neighbor = i;
		}
		else if ( d < dist2 )
			dist2 = d;
	}
	if ( dist1 < 0.6*dist2 )
		return neighbor;
	return -1;
}

void
CShapeAnalysis::flannFindPairs( const CvSeq*, const CvSeq* objectDescriptors,
			   const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs )
{
	int length = (int)(objectDescriptors->elem_size/sizeof(float));

	cv::Mat m_object(objectDescriptors->total, length, CV_32F);
	cv::Mat m_image(imageDescriptors->total, length, CV_32F);


	// copy descriptors
	CvSeqReader obj_reader;
	float* obj_ptr = m_object.ptr<float>(0);
	cvStartReadSeq( objectDescriptors, &obj_reader );
	for(int i = 0; i < objectDescriptors->total; i++ )
	{
		const float* descriptor = (const float*)obj_reader.ptr;
		CV_NEXT_SEQ_ELEM( obj_reader.seq->elem_size, obj_reader );
		memcpy(obj_ptr, descriptor, length*sizeof(float));
		obj_ptr += length;
	}
	CvSeqReader img_reader;
	float* img_ptr = m_image.ptr<float>(0);
	cvStartReadSeq( imageDescriptors, &img_reader );
	for(int i = 0; i < imageDescriptors->total; i++ )
	{
		const float* descriptor = (const float*)img_reader.ptr;
		CV_NEXT_SEQ_ELEM( img_reader.seq->elem_size, img_reader );
		memcpy(img_ptr, descriptor, length*sizeof(float));
		img_ptr += length;
	}

	// find nearest neighbors using FLANN
	cv::Mat m_indices(objectDescriptors->total, 2, CV_32S);
	cv::Mat m_dists(objectDescriptors->total, 2, CV_32F);
	cv::flann::Index flann_index(m_image, cv::flann::KDTreeIndexParams(4));  // using 4 randomized kdtrees
	flann_index.knnSearch(m_object, m_indices, m_dists, 2, cv::flann::SearchParams(64) ); // maximum number of leafs checked

	int* indices_ptr = m_indices.ptr<int>(0);
	float* dists_ptr = m_dists.ptr<float>(0);
	for (int i=0;i<m_indices.rows;++i) {
		if (dists_ptr[2*i]<0.6*dists_ptr[2*i+1]) {
			ptpairs.push_back(i);
			ptpairs.push_back(indices_ptr[2*i]);
		}
	}
}

void CShapeAnalysis::DrawContour(IplImage* pMask)
{
	cvZero(pMask);
	for( CvSeq* c=m_pContour; c != NULL; c = c->h_next )
	{
		cvDrawContours(pMask,c,CV_CVX_WHITE, CV_CVX_WHITE,100,m_ctLineWidth,8);
	}
}
IplImage* CShapeAnalysis::Rotate(IplImage* pSrcMask){
	
	CvBox2D box2d=PartsAngleDetection(pSrcMask);
	CvPoint2D32f pt2d[4];
	cvBoxPoints(box2d,pt2d);
	float len=sqrt(float((pt2d[0].x-pt2d[1].x)*(pt2d[0].x-pt2d[1].x)+(pt2d[0].y-pt2d[1].y)*(pt2d[0].y-pt2d[1].y)));
	
	double bZero=len-FMax<float>(box2d.size.width,box2d.size.height);
	double angle=0.0;
	if(abs(bZero)<0.001)
		angle=-(90-abs(box2d.angle));
	else angle=-box2d.angle;
	
	double scale=FMax<float>(box2d.size.height,box2d.size.width)/140.00;
	float m[6];
	m[0] = (float)(scale*cos(angle*CV_PI/180.));
	m[1] = (float)(scale*sin(angle*CV_PI/180.));
	m[2] = pSrcMask->width*0.5f;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = pSrcMask->height*0.5f;
	CvMat M = cvMat (2, 3, CV_32F, m);
	IplImage* pDstMask=AutoStorage->CreateIplImage(pSrcMask->width,pSrcMask->height,pSrcMask->depth,pSrcMask->nChannels);
	cvZero(pDstMask);
	IplImage* pMask=OffsetObject(pSrcMask,box2d.center);
	cvGetQuadrangleSubPix(pMask, pDstMask, &M);
	//cvThreshold(pDstMask,pDstMask,64,255,CV_THRESH_BINARY);
	AutoStorage->ReleaseIplImage(pMask);
	return pDstMask;
}
IplImage* CShapeAnalysis::OffsetObject(IplImage* pMask,CvPoint2D32f objCenter)
{
	if(pMask==NULL)
		return NULL;
	CvPoint2D32f center={pMask->width/2,pMask->height/2};
	IplImage* pTmpMask=AutoStorage->CreateIplImage(pMask->width,pMask->height,pMask->depth,pMask->nChannels);
	cvZero(pTmpMask);
	for(int y=0;y<pMask->height;++y){
		BYTE* pSrc=(BYTE*)(pMask->imageData+y*pMask->widthStep);
		for(int x=0;x<pMask->width;++x,pSrc++){
			if(*pSrc==0xFF){
				int new_y=y-objCenter.y+center.y;
				int new_x=x-objCenter.x+center.x;
				if(new_y<0||new_y>=pTmpMask->height||new_x<0||new_x>=pTmpMask->width){
					continue;
				}
				BYTE* pDst=cvPtr2D(pTmpMask,new_y,new_x);
				*pDst=0xFF;
			}
		}
	}
	return pTmpMask;
}
void CShapeAnalysis::Rotate(IplImage* pImg,IplImage* pSrcMask,IplImage* pImgDst,IplImage* pMskDst)
{
	CvBox2D box2d=PartsAngleDetection(pSrcMask);
	double angle=GetRealAngle(box2d);

	double scale=1;
	float m[6];
	m[0] = (float)(scale*cos(angle*CV_PI/180.));
	m[1] = (float)(scale*sin(angle*CV_PI/180.));
	m[2] = pSrcMask->width*0.5f;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = pSrcMask->height*0.5f;
	CvMat M = cvMat (2, 3, CV_32F, m);
	cvZero(pMskDst);
	IplImage* pMaskTmp=MV_NEW_IMG(pSrcMask->width,pSrcMask->height,pSrcMask->depth,pSrcMask->nChannels);
	IplImage* pDstTmp=MV_NEW_IMG(pImg->width,pImg->height,pImg->depth,pImg->nChannels);
	OffsetObject(pImg,pSrcMask,box2d,pDstTmp,pMaskTmp);
	/*cvShowImage("sf",pDstTmp);
	cvWaitKey(0);*/

	cvGetQuadrangleSubPix(pMaskTmp,pMskDst, &M);
	cvGetQuadrangleSubPix(pDstTmp, pImgDst, &M);
	MV_DEL_IMG(pDstTmp);
	MV_DEL_IMG(pMaskTmp);
}
void CShapeAnalysis::Rotate(IplImage* pImg,IplImage* pSrcMask,CvBox2D box2d,IplImage* pImgDst,double sz)
{	
	double angle=GetRealAngle(box2d);
	double scale=FMax<float>(box2d.size.height,box2d.size.width)/sz;
	float m[6];
	m[0] = (float)(scale*cos(angle*CV_PI/180.));
	m[1] = (float)(scale*sin(angle*CV_PI/180.));
	m[2] = pImg->width*0.5f;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = pImg->height*0.5f;
	CvMat M = cvMat (2, 3, CV_32F, m);

	//IplImage* pMaskTmp=AutoStorage->CreateIplImage(pSrcMask->width,pSrcMask->height,pSrcMask->depth,pSrcMask->nChannels);
	IplImage* pDstTmp=AutoStorage->CreateIplImage(pImg->width,pImg->height,pImg->depth,pImg->nChannels);
	OffsetObject(pImg,pSrcMask,box2d,pDstTmp,NULL);
	cvGetQuadrangleSubPix(pDstTmp, pImgDst, &M);
	AutoStorage->ReleaseIplImage(pDstTmp);
}
void CShapeAnalysis::OffsetObject(IplImage* pImgSrc,IplImage* pMaskSrc,CvBox2D objBox,IplImage* pImgDst,IplImage* pMskDst)
{
	if(pMaskSrc==NULL)
		return;
	CvPoint2D32f pt[4];
	cvBoxPoints(objBox,pt);
	CvMat* B=MV_NEW_MAT(4,1,CV_32FC2);
	cvSetData(B,pt,B->step);

	CvPoint2D32f center={pMaskSrc->width/2,pMaskSrc->height/2};
	cvZero(pMskDst);
	for(int y=0;y<pMaskSrc->height;++y){
		//BYTE* pMSrc=(BYTE*)(pMaskSrc->imageData+(pMaskSrc->height-1-y)*pMaskSrc->widthStep);
		for(int x=0;x<pMaskSrc->width;++x){
			CvPoint2D32f p={x,y};
			if(cvPointPolygonTest(B,p,0)>=0){
				int new_y=y-objBox.center.y+center.y;
				int new_x=x-objBox.center.x+center.x;
				if(new_y<0||new_y>=pImgDst->height||new_x<0||new_x>=pImgDst->width){
					continue;
				}
				if(pMskDst!=NULL){
					BYTE* pMDst=cvPtr2D(pMskDst,new_y,new_x);
					*pMDst=*(cvPtr2D(pMaskSrc,y,x));
				}
				BYTE* pIDst=cvPtr2D(pImgDst,new_y,new_x);
				BYTE* pISrc=cvPtr2D(pImgSrc,y,x);
				FCColor::CopyPixel(pIDst,pISrc,pImgDst->nChannels);
			}
		}
	}
	MV_DEL_MAT(B);
}
int CShapeAnalysis::find( int set[], int x )  
{  
	int r = x;  
	while ( set[r] != r )  
		r = set[r];  
	return r;  
}
//input:img     --  gray image  
//      n       --  n connectedness  
//      labels  --  label of each pixel, labels[row * col]  
//output:  number of connected regions  

int CShapeAnalysis::bwlabel(IplImage* img, int n, int* labels)  
{  
	if(n != 4 && n != 8)  
		n = 4;  
	int nr = img->height;  
	int nc = img->width;  
	int total = nr * nc;  
	// results  
	memset(labels, 0, total * sizeof(int));  
	int nobj = 0;                               // number of objects found in image  
	// other variables                               
	int* lset = new int[total];   // label table  
	memset(lset, 0, total * sizeof(int));  
	int ntable = 0;  
	for( int r = 0; r < nr; r++ )   
	{  
		for( int c = 0; c < nc; c++ )   
		{              
			if ( ELEM(img, r, c) )   // if A is an object  
			{                 
				// get the neighboring pixels B, C, D, and E  
				int B, C, D, E;  
				if ( c == 0 )   
					B = 0;   
				else   
					B = find( lset, ONETWO(labels, r, c - 1, nc) );  
				if ( r == 0 )   
					C = 0;   
				else   
					C = find( lset, ONETWO(labels, r - 1, c, nc) );  
				if ( r == 0 || c == 0 )   
					D = 0;   
				else   
					D = find( lset, ONETWO(labels, r - 1, c - 1, nc) );  
				if ( r == 0 || c == nc - 1 )   
					E = 0;  
				else   
					E = find( lset, ONETWO(labels, r - 1, c + 1, nc) );  
				if ( n == 4 )   
				{  
					// apply 4 connectedness  
					if ( B && C )   
					{        // B and C are labeled  
						if ( B == C )  
							ONETWO(labels, r, c, nc) = B;  
						else {  
							lset[C] = B;  
							ONETWO(labels, r, c, nc) = B;  
						}  
					}   
					else if ( B )             // B is object but C is not  
						ONETWO(labels, r, c, nc) = B;  
					else if ( C )               // C is object but B is not  
						ONETWO(labels, r, c, nc) = C;  
					else   
					{                      // B, C, D not object - new object  
						//   label and put into table  
						ntable++;  
						ONETWO(labels, r, c, nc) = lset[ ntable ] = ntable;  
					}  
				}   
				else if ( n == 6 )   
				{  
					// apply 6 connected ness  
					if ( D )                    // D object, copy label and move on  
						ONETWO(labels, r, c, nc) = D;  
					else if ( B && C )   
					{        // B and C are labeled  
						if ( B == C )  
							ONETWO(labels, r, c, nc) = B;  
						else   
						{  
							int tlabel = MIN(B,C);  
							lset[B] = tlabel;  
							lset[C] = tlabel;  
							ONETWO(labels, r, c, nc) = tlabel;  
						}  
					}   
					else if ( B )             // B is object but C is not  
						ONETWO(labels, r, c, nc) = B;  
					else if ( C )               // C is object but B is not  
						ONETWO(labels, r, c, nc) = C;  
					else   
					{                      // B, C, D not object - new object  
						//   label and put into table  
						ntable++;  
						ONETWO(labels, r, c, nc) = lset[ ntable ] = ntable;  
					}  
				}  
				else if ( n == 8 )   
				{  
					// apply 8 connectedness  
					if ( B || C || D || E )   
					{  
						int tlabel = B;  
						if ( B )   
							tlabel = B;  
						else if ( C )   
							tlabel = C;  
						else if ( D )   
							tlabel = D;  
						else if ( E )   
							tlabel = E;  
						ONETWO(labels, r, c, nc) = tlabel;  
						if ( B && B != tlabel )   
							lset[B] = tlabel;  
						if ( C && C != tlabel )   
							lset[C] = tlabel;  
						if ( D && D != tlabel )   
							lset[D] = tlabel;  
						if ( E && E != tlabel )   
							lset[E] = tlabel;  
					}   
					else   
					{  
						//   label and put into table  
						ntable++;  
						ONETWO(labels, r, c, nc) = lset[ ntable ] = ntable;  
					}  
				}  
			}   
			else   
			{  
				ONETWO(labels, r, c, nc) = NO_OBJECT;      // A is not an object so leave it  
			}  
		}  
	}  
	// consolidate component table  
	for( int i = 0; i <= ntable; i++ )  
		lset[i] = find( lset, i );                                                                                                   
	// run image through the look-up table  
	for( int r = 0; r < nr; r++ )  
		for( int c = 0; c < nc; c++ )  
			ONETWO(labels, r, c, nc) = lset[ ONETWO(labels, r, c, nc) ];  
	// count up the objects in the image  
	for( int i = 0; i <= ntable; i++ )  
		lset[i] = 0;  
	for( int r = 0; r < nr; r++ )  
		for( int c = 0; c < nc; c++ )  
			lset[ ONETWO(labels, r, c, nc) ]++;  
	// number the objects from 1 through n objects  
	nobj = 0;  
	lset[0] = 0;  
	for( int i = 1; i <= ntable; i++ )  
		if ( lset[i] > 0 )  
			lset[i] = ++nobj;  
	// run through the look-up table again  
	for( int r = 0; r < nr; r++ )  
		for( int c = 0; c < nc; c++ )  
			ONETWO(labels, r, c, nc) = lset[ ONETWO(labels, r, c, nc) ];  
	//  
	delete[] lset;  
	return nobj;  
}
void CShapeAnalysis::CptInCicles(IplImage* pMask,IplImage* &out)
{
	if(pMask==NULL||pMask->nChannels!=1)
		return;
	out=AutoStorage->CreateIplImage(pMask->width,pMask->height,8,1);
	cvZero(out);
	for(int y=0;y<pMask->height;++y){
		BYTE* pB=NULL;
		if(pMask->origin){
			pB=(BYTE*)(pMask->imageData+(pMask->height-1-y)*pMask->widthStep);
		}else{
			pB=(BYTE*)(pMask->imageData+y*pMask->widthStep);
		}
		BYTE* pData=(BYTE*)(out->imageData+(out->height-1-y)*out->widthStep);
		for(int x=0;x<pMask->width;++x,pB++,pData++){
			if((*pB)==0xFF){
				BYTE c=CptInCicleRadius(pMask,x,y,200);
				*pData=c*2;
			}
		}
	}
	
}
int  CShapeAnalysis::CptInCicleRadius(IplImage* pMask,int x,int y,int max_radius,double eps)
{
	double preNum=0;
	for(int r=1;r<=m_idxVec.size();++r){
		int beginIdx,endIdx;
		if(r==1)
			beginIdx=0;
		else beginIdx=m_idxVec[r-2];
		endIdx=m_idxVec[r-1];
		double area=endIdx-beginIdx;
		double num=0;
		for(int i=beginIdx;i<endIdx;++i){
			CvPoint pt={m_lookupTable[i].x+x,m_lookupTable[i].y+y};
			if(pt.x<0||pt.y<0||pt.x>=pMask->width||pt.y>=pMask->height)
				continue;
			BYTE* pData=cvPtr2D(pMask,pt.y,pt.x);
			if(!pData)
				continue;
			if((*pData)==0){
				return r;
			}
		}			
	}
	return max_radius;
}
void CShapeAnalysis::GenLookupTable(int radius)
{
	IplImage* pMask=cvCreateImage(cvSize(radius*2+1,radius*2+1),8,1);
	pMask->origin=1;
	cvZero(pMask);
	CvPoint center=cvPoint(pMask->width/2,pMask->height/2);
	for(int i=1;i<=radius;++i){
		cvCircle(pMask,center,i,CV_RGB(i,i,i),-1);
		if(i>1){
			cvCircle(pMask,center,i-1,CV_RGB(0,0,0),-1);
		}
		for(int y=0;y<pMask->height;++y){
			BYTE* pB=(BYTE*)(pMask->imageData+(pMask->height-1-y)*pMask->widthStep);
			for(int x=0;x<pMask->width;++x,pB++){
				if((*pB)>0){
					CvPoint pt=cvPoint(x-center.x,y-center.y);
					m_lookupTable.push_back(pt);
				}
			}
		}
		m_idxVec.push_back(m_lookupTable.size());
	}
	cvShowImage("test",pMask);
	cvWaitKey(0);

	cvReleaseImage(&pMask);
	SaveLUT();
}
void CShapeAnalysis::SaveLUT()
{
	std::string str(PartsDB->m_curDir);
	str.append("LUT.xml");
	FileStorage fs(str,CV_STORAGE_WRITE);
	if(!fs.isOpened())
		return;
	{
		WriteStructContext ws(fs, "points", CV_NODE_SEQ);
		vector<CvPoint>::iterator it=m_lookupTable.begin();
		for( ;it!=m_lookupTable.end();++it)
		{
			write(fs, it->x);
			write(fs, it->y);
		}	
	}
	{
		WriteStructContext ws(fs, "indexs", CV_NODE_SEQ);
		vector<int>::iterator it=m_idxVec.begin();
		for( ;it!=m_idxVec.end();++it)
		{
			write(fs, *it);
		}	
	}
}
void CShapeAnalysis::LoadLUT()
{	
	std::string str(PartsDB->m_curDir);
	str.append("LUT.xml");
	FileStorage fs(str,CV_STORAGE_READ);
	if(!fs.isOpened()){
		GenLookupTable();
		return;
	}
	m_lookupTable.clear();
	m_idxVec.clear();
	{
		FileNode node=fs["points"];
		FileNodeIterator it = node.begin(), it_end = node.end();
		for(int i=0; it != it_end; i=i+2)
		{
			CvPoint pt;
			it>>pt.x;
			it>>pt.y;
			m_lookupTable.push_back(pt);
		}
	}
	{
		FileNode node=fs["indexs"];
		FileNodeIterator it = node.begin(), it_end = node.end();
		for(int i=0; it != it_end; i=i+2)
		{
			int idx;
			it>>idx;
			m_idxVec.push_back(idx);
		}
	}

}
void CShapeAnalysis::CptContourTriangle(std::vector<CvPoint2D32f>& dpList,std::vector<Triangle>& triLstOut)
{
	if(dpList.size()<=2)
		return;
	std::vector<CvPoint2D32f>::iterator it=dpList.begin();//当前元素指针
	std::vector<CvPoint2D32f>::iterator pit=dpList.end()-1;
	std::vector<CvPoint2D32f>::iterator nit=it+1;//后一个元素指针
	
	
	Triangle tri;
	tri.a=sqrt(double((it->x-pit->x)*(it->x-pit->x)+(it->y-pit->y)*(it->y-pit->y)));
	tri.b=sqrt((double)((it->x-nit->x)*(it->x-nit->x)+(it->y-nit->y)*(it->y-nit->y)));
	tri.c=sqrt((double)((pit->x-nit->x)*(pit->x-nit->x)+(pit->y-nit->y)*(pit->y-nit->y)));
	tri.pos=(*it);
	tri.left=*nit;
	tri.right=*pit;
	triLstOut.push_back(tri);
	pit=it;
	for(it++,nit++;nit!=dpList.end();++it,++nit,++pit){		
		Triangle tri;
		tri.a=sqrt(double((it->x-pit->x)*(it->x-pit->x)+(it->y-pit->y)*(it->y-pit->y)));
		tri.b=sqrt((double)((it->x-nit->x)*(it->x-nit->x)+(it->y-nit->y)*(it->y-nit->y)));
		tri.c=sqrt((double)((pit->x-nit->x)*(pit->x-nit->x)+(pit->y-nit->y)*(pit->y-nit->y)));
		tri.pos=(*it);
		tri.left=*nit;
		tri.right=*pit;
		triLstOut.push_back(tri);
	}
	nit=dpList.begin();
	//处理到底列表底部时情况
	Triangle tri2;
	tri2.a=sqrt(double((it->x-pit->x)*(it->x-pit->x)+(it->y-pit->y)*(it->y-pit->y)));
	tri2.b=sqrt((double)((it->x-nit->x)*(it->x-nit->x)+(it->y-nit->y)*(it->y-nit->y)));
	tri2.c=sqrt((double)((pit->x-nit->x)*(pit->x-nit->x)+(pit->y-nit->y)*(pit->y-nit->y)));
	tri2.pos=(*it);
	tri2.left=*nit;
	tri2.right=*pit;
	triLstOut.push_back(tri2);
}
double  CShapeAnalysis::MatchTriangle(Triangle& src,Triangle& dst)
{
	double cDiff=(src.c-dst.c)*(src.c-dst.c);
	double diff1=(src.a-dst.a)*(src.a-dst.a)+(src.b-dst.b)*(src.b-dst.b)+cDiff;
	double diff2=(src.a-dst.b)*(src.a-dst.b)+(src.b-dst.a)*(src.b-dst.a)+cDiff;
	return FMin<double>(diff1,diff2);
}
void CShapeAnalysis::MatchTriangle(Triangle& tri,std::vector<Triangle>& triLst,std::vector<double>& distList)
{
	std::vector<Triangle>::iterator it=triLst.begin();
	for (;it!=triLst.end();++it)
	{
		double diff1=(tri.a-it->a)*(tri.a-it->a)+(tri.b-it->b)*(tri.b-it->b)+(tri.c-it->c)*(tri.c-it->c);
		double diff2=(tri.a-it->b)*(tri.a-it->b)+(tri.b-it->a)*(tri.b-it->a)+(tri.c-it->c)*(tri.c-it->c);
		distList.push_back(FMin<double>(diff1,diff2));
	}
}
double CShapeAnalysis::MatchTriangle(std::vector<Triangle>& queryTriLst,
					            std::vector<Triangle>& dbTriLst,Triangle& out)
{
	std::vector<Triangle>::iterator it=queryTriLst.begin();
	

	return 0;
}
CvMat*  CShapeAnalysis::GetAffineTransform(Triangle& src,Triangle& dst)
{
	double diff1=(src.a-dst.a)*(src.a-dst.a)+(src.b-dst.b)*(src.b-dst.b)+(src.c-dst.c)*(src.c-dst.c);
	double diff2=(src.a-dst.b)*(src.a-dst.b)+(src.b-dst.a)*(src.b-dst.a)+(src.c-dst.c)*(src.c-dst.c);
	CvMat* warp_mat=NULL;
	if(diff1<=diff2){
		CvPoint2D32f pts_src[3]={src.right,src.pos,src.left};
		CvPoint2D32f pts_dst[3]={dst.right,dst.pos,dst.left};
		warp_mat=cvCreateMat(2,3,CV_32FC1);
		cvGetAffineTransform(pts_src,pts_dst,warp_mat);
	}else{
		CvPoint2D32f pts_src[3]={src.right,src.pos,src.left};
		CvPoint2D32f pts_dst[3]={dst.left, dst.pos,dst.right};
		warp_mat=cvCreateMat(2,3,CV_32FC1);
		cvGetAffineTransform(pts_src,pts_dst,warp_mat);

		/*CvPoint2D32f ptDst;
		Transform(pts_src[0],ptDst,warp_mat);
		double diff=(ptDst.x-pts_dst[0].x)*(ptDst.x-pts_dst[0].x)+(ptDst.y-pts_dst[0].y)*(ptDst.y-pts_dst[0].y);
		diff=sqrt(diff);*/
	}	
	return warp_mat;
}

CvBox2D CShapeAnalysis::CptMinBoundingRect(Triangle& src,Triangle& dst,CvBox2D& box,IplImage* pMask)
{
	//DrawMinBoundingRect(box,pMask);
	CvMat* warp_mat=GetAffineTransform(src,dst);
	CvPoint2D32f pt2d[4];
	cvBoxPoints(box,pt2d);
	for(int i=0;i<4;++i){
		CvPoint2D32f dstPt,icsPt;
		Transform(pt2d[i],dstPt,warp_mat);
		CamCalib->CptICSPoint(dstPt,icsPt);
		pt2d[i]=icsPt;
	}
	if(pMask){
		for (int i=0;i<4;++i)
		{
			int nxt=(i+1)%4;
			CvPoint pt1={pt2d[i].x,pt2d[i].y};
			CvPoint pt2={pt2d[nxt].x,pt2d[nxt].y};
			cvLine(pMask,pt1,pt2,CV_RGB(0,0,255));
		}
	}	
	return GetBox(pt2d);
}
//
double CShapeAnalysis::MatchDominantPoints(TriVec& src,Pt32fVec& dpLst,
							    CvMat* warp_mat,double threshold)
{
	if(src.empty())
		return DBL_MAX;
	TriVec::iterator it=src.begin();
	double diffSum=0.0;
	double count=src.size();
	for(;it!=src.end();++it){
		CvPoint2D32f ptDst;
		Transform(it->pos,ptDst,warp_mat);
		double diff=BestMatch(ptDst,dpLst);
		diffSum+=diff;
	}
	return diffSum/(count*count);
}
double CShapeAnalysis::BestMatch(CvPoint2D32f pt,Pt32fVec& dpLst)
{
	Pt32fVec::iterator it=dpLst.begin();
	double diff=DBL_MAX;
	for(;it!=dpLst.end();++it){
		double diffTmp=(it->x-pt.x)*(it->x-pt.x);
		if(diff>diffTmp){
			diffTmp=diffTmp+(it->y-pt.y)*(it->y-pt.y);
			if(diff>diffTmp){
				diff=diffTmp;
			}
		}
	}
	return sqrt(diff);
}
CvMat*  CShapeAnalysis::ToMatrix(TriVec& triLst)
{
	if(triLst.empty())
		return NULL;
	CvMat* data=cvCreateMat(triLst.size(),9,CV_32FC1);
	TriVec::iterator it=triLst.begin();
	for(int r=0;it!=triLst.end();++it,++r){
		CV_MAT_ELEM(*data,float,r,0)=it->left.x;
		CV_MAT_ELEM(*data,float,r,1)=it->left.y;
		CV_MAT_ELEM(*data,float,r,2)=it->pos.x;
		CV_MAT_ELEM(*data,float,r,3)=it->pos.y;
		CV_MAT_ELEM(*data,float,r,4)=it->right.x;
		CV_MAT_ELEM(*data,float,r,5)=it->right.y;
		CV_MAT_ELEM(*data,float,r,6)=it->a;
		CV_MAT_ELEM(*data,float,r,7)=it->b;
		CV_MAT_ELEM(*data,float,r,8)=it->c;
	}
	return data;
}
void    CShapeAnalysis::FromMatrix(CvMat* data,TriVec& triLst)
{
	if(!data)
		return;
	for(int r=0;r<data->rows;++r){
		Triangle tri;
		tri.left.x=CV_MAT_ELEM(*data,float,r,0);
		tri.left.y=CV_MAT_ELEM(*data,float,r,1);
		tri.pos.x=CV_MAT_ELEM(*data,float,r,2);
		tri.pos.y=CV_MAT_ELEM(*data,float,r,3);
		tri.right.x=CV_MAT_ELEM(*data,float,r,4);
		tri.right.y=CV_MAT_ELEM(*data,float,r,5);
		tri.a=CV_MAT_ELEM(*data,float,r,6);
		tri.b=CV_MAT_ELEM(*data,float,r,7);
		tri.c=CV_MAT_ELEM(*data,float,r,8);
		triLst.push_back(tri);
	}
}
Triangle CShapeAnalysis::GetMaxLengthTriangle(TriVec& triLst)
{
	Triangle triRet;
	if(triLst.empty())
		return triRet;
	TriVec::iterator it=triLst.begin();
	double maxC=0.0;
	int    idx=0;
	for (int i=0;it!=triLst.end();++it,++i)
	{
		if(maxC<it->c){
			idx=i;
			maxC=it->c;
		}
	}
	triRet=triLst[idx];
	triRet.diff=1.0;
	return triRet;
}
void CShapeAnalysis::DrawTrianglePoints(Triangle tri,CvScalar color,IplImage* pMask,int style)
{
	CvPoint2D32f pt[3];
	CamCalib->CptICSPoint(tri.right,pt[0]);
	CamCalib->CptICSPoint(tri.pos,pt[1]);
	CamCalib->CptICSPoint(tri.left,pt[2]);
	for(int i=0;i<3;++i){
		CvPoint p={pt[i].x,pt[i].y};
		if(style==0)
			cvDrawCircle(pMask,p,2,color,CV_FILLED);
		else if(style==1){
			CvPoint pt1={p.x-5,p.y-5};
			CvPoint pt2={p.x+5,p.y+5};
			cvDrawRect(pMask,pt1,pt2,color,CV_FILLED);
		}			
	}
}
void CShapeAnalysis::DrawTrianglePoints(Triangle triQry,TMatch& tm,TriVec& trainTriLst,CvScalar color,IplImage* pMask)
{
	CvMat* warp_mat=GetAffineTransform(tm.tri,triQry);
	TriVec::iterator it=trainTriLst.begin();
	for(;it!=trainTriLst.end();++it){
		CvPoint2D32f pt32f,ptICS;
		//Transform(it->pos,pt32f,warp_mat);
		CamCalib->CptICSPoint(it->pos,ptICS);
		CvPoint p={ptICS.x,ptICS.y};
		cvDrawCircle(pMask,p,5,color,CV_FILLED);
	}
	cvReleaseMat(&warp_mat);
}
void CShapeAnalysis::Transform(CvPoint2D32f ptSrc,CvPoint2D32f& ptDst,CvMat* warp_mat)
{
	CvMat* srcPts=MV_NEW_MAT(1,1,CV_32FC3);
	CvMat* dstPts=MV_NEW_MAT(1,1,CV_32FC2);
	CV_MAT_ELEM(*srcPts,float,0,0)=ptSrc.x;
	CV_MAT_ELEM(*srcPts,float,0,1)=ptSrc.y;
	CV_MAT_ELEM(*srcPts,float,0,2)=1;
	cvTransform(srcPts,dstPts,warp_mat);
	ptDst.x=CV_MAT_ELEM(*dstPts,float,0,0);
	ptDst.y=CV_MAT_ELEM(*dstPts,float,0,1);
	MV_DEL_MAT(srcPts);
	MV_DEL_MAT(dstPts);
}
void  CShapeAnalysis::Transform(Triangle triSrc,Triangle& triDst,CvMat* warp_mat)
{
	Transform(triSrc.left,triDst.left,warp_mat);
	Transform(triSrc.pos,triDst.pos,warp_mat);
	Transform(triSrc.right,triDst.right,warp_mat);
}
double CShapeAnalysis::GetRealAngle(CvBox2D box2d)
{
	
	CvPoint2D32f pt2d[4];
	cvBoxPoints(box2d,pt2d);
	float len=sqrt(float((pt2d[0].x-pt2d[1].x)*(pt2d[0].x-pt2d[1].x)+(pt2d[0].y-pt2d[1].y)*(pt2d[0].y-pt2d[1].y)));
	double bZero=len-FMax<float>(box2d.size.width,box2d.size.height);
	double angle=0.0;
	if(abs(bZero)<0.0001)
		angle=90+abs(box2d.angle);
	else angle=-box2d.angle;
	return angle;
}
CvBox2D CShapeAnalysis::GetRealMinBox(CvBox2D box2d)
{
	CvPoint2D32f pt2d[4];
	cvBoxPoints(box2d,pt2d);
	for(int i=0;i<4;++i){
		CvPoint2D32f wcsPt;
		CamCalib->CptWCSPoint(pt2d[i],wcsPt);
		pt2d[i]=wcsPt;
	}
	return GetBox(pt2d);
}
CvBox2D CShapeAnalysis::GetBox(CvPoint2D32f* pts)
{
	CvMat* B=MV_NEW_MAT(4,1,CV_32FC2);
	cvSetData(B,pts,B->step);
	CvBox2D bbox=cvMinAreaRect2(B);
	MV_DEL_MAT(B);
	return bbox;
}