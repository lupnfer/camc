#include "StdAfx.h"
#include "BgSub.h"
#include "cv.h"
#include "highgui.h"
#include "StdDefine.h"
#include "MoonCake.h"
CBgSub::CBgSub()
{
	m_first=1;
	m_vLow=0.4;
	m_vHi=0.6;
	m_sT=4;
	m_hT=9;
	m_IavgF  = NULL;
	m_IdiffF = NULL;
	m_IprevF = NULL;
	m_IhiF   = NULL;
	m_IlowF  = NULL;

	m_Ilow1  = NULL;
	m_Ilow2  = NULL;
	m_Ilow3  = NULL;

	m_Ihi1   = NULL;
	m_Ihi2   = NULL;
	m_Ihi3   = NULL;

	m_Iscratch   = NULL;
	m_Isscratch2 = NULL;

	m_Igray1 = NULL;
	m_Igray2 = NULL;
	m_Igray3 = NULL;
	m_Imaskt = NULL;
	m_cB=NULL;


	
	m_model=NULL;
	m_bg_model=NULL;
}
CBgSub::~CBgSub(void)
{
	ReleaseImages();
	if(m_cB){
		for(int i=0;i<m_imageLen;++i){
			for(int j=0;j<m_cB[i].numEntries;++j){
				code_element * cdElem=m_cB[i].cb[j];
				delete cdElem;
			}
			delete[] (m_cB[i].cb);
		}
		delete [] m_cB;
	}	
}
void CBgSub::AllocateImages(CvSize sz)
{
	sz.height=sz.height/2;
	sz.width=sz.width/2;
	ReleaseImages();
	m_IavgF  = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_IdiffF = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_IprevF = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_IhiF   = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_IlowF  = cvCreateImage(sz,IPL_DEPTH_32F,3);

	m_Ilow1  = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Ilow2  = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Ilow3  = cvCreateImage(sz,IPL_DEPTH_32F,1);

	m_Ihi1   =  cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Ihi2   =  cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Ihi3   =  cvCreateImage(sz,IPL_DEPTH_32F,1);

	cvZero(m_IavgF);
	cvZero(m_IdiffF);
	cvZero(m_IprevF);
	cvZero(m_IhiF);
	cvZero(m_IlowF);
	m_Icount = 0.00001;

	m_Iscratch   = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_Isscratch2 = cvCreateImage(sz,IPL_DEPTH_32F,3);

	m_Igray1 = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Igray2 = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Igray3 = cvCreateImage(sz,IPL_DEPTH_32F,1);

	m_Imaskt = cvCreateImage(sz,IPL_DEPTH_8U,1);
	cvZero(m_Iscratch);
	cvZero(m_Isscratch2);

}
void CBgSub::AccumulateBackground(IplImage *I)
{
	if(I==NULL)
		return;
	cvCvtScale(I,m_Iscratch,1,0);
	if(!m_first){
		cvAcc(m_Iscratch,m_IavgF);
		cvAbsDiff(m_Iscratch,m_IprevF,m_Isscratch2);
		cvAcc(m_Isscratch2,m_IdiffF);
		m_Icount+=1.0;
	}
	m_first=0;
	cvCopy(m_Iscratch,m_IprevF);
}
void CBgSub::CreateModelsfromStats()
{
	cvConvertScale(m_IavgF,m_IavgF,(double)(1.0/m_Icount));
	cvConvertScale(m_IdiffF,m_IdiffF,(double)(1.0/m_Icount));
	//cvSaveImage("D:\\Test.jpg",m_IavgF);
	cvAddS(m_IdiffF,cvScalar(1.0,1.0,1.0),m_IdiffF);
	//cvSaveImage("D:\\diff.jpg",m_IdiffF);
	SetHighThreshold(30.0);
	SetLowThreshold(20.0);
	char buffer[1024];
	GetCurrentDirectory(1024, buffer);
	std::string strPath;
	strPath.append(buffer);
	strPath.append("\\");
	std::string str=strPath+"MeanBackgroudModel.xml";
	CvFileStorage* fs=cvOpenFileStorage(str.c_str(),0,CV_STORAGE_WRITE);
	cvWrite(fs,"low1",m_Ilow1);
	cvWrite(fs,"low2",m_Ilow2);
	cvWrite(fs,"low3",m_Ilow3);
	cvWrite(fs,"hi1",m_Ihi1);
	cvWrite(fs,"hi2",m_Ihi2);
	cvWrite(fs,"hi3",m_Ihi3);
	cvReleaseFileStorage(&fs);

}
void CBgSub::LoadMeanModel()
{	
	char buffer[1024];
	GetCurrentDirectory(1024, buffer);
	std::string strPath;
	strPath.append(buffer);
	strPath.append("\\");
	std::string str=strPath+"MeanBackgroudModel.xml";
	CvFileStorage* fs=cvOpenFileStorage(str.c_str(),0,CV_STORAGE_READ);
	if(!fs)
		return;
	m_Ilow1=(IplImage*)cvReadByName(fs,0,"low1");
	m_Ilow2=(IplImage*)cvReadByName(fs,0,"low2");
	m_Ilow3=(IplImage*)cvReadByName(fs,0,"low3");
	m_Ihi1=(IplImage*)cvReadByName(fs,0,"hi1");
	m_Ihi2=(IplImage*)cvReadByName(fs,0,"hi2");
	m_Ihi3=(IplImage*)cvReadByName(fs,0,"hi3");
	cvReleaseFileStorage(&fs);
	CvSize sz=cvGetSize(m_Ilow1);
	m_Iscratch   = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_Isscratch2 = cvCreateImage(sz,IPL_DEPTH_32F,3);
	m_Igray1 = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Igray2 = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Igray3 = cvCreateImage(sz,IPL_DEPTH_32F,1);
	m_Imaskt = cvCreateImage(sz,IPL_DEPTH_8U,1);
}
void CBgSub::SetHighThreshold(float scale)
{
	cvConvertScale(m_IdiffF,m_Iscratch,scale);
	cvAdd(m_Iscratch,m_IavgF,m_IhiF);
	cvSplit(m_IhiF,m_Ihi1,m_Ihi2,m_Ihi3,0);
}
void CBgSub::SetLowThreshold(float scale)
{
	cvConvertScale(m_IdiffF,m_Iscratch,scale);
	cvSub(m_IavgF,m_Iscratch,m_IlowF);
	cvSplit(m_IlowF,m_Ilow1,m_Ilow2,m_Ilow3,0);
}
BOOL CBgSub::IsMeanModelReady()
{
	if(m_Ilow1&&m_Ilow2&&m_Ilow3&&m_Ihi1&&m_Ihi2&&m_Ihi3)
		return TRUE;
	return FALSE;
}
IplImage* CBgSub::BackgroundDiffOfMean(IplImage *pImageRead)
{
	if(pImageRead==NULL||IsMeanModelReady()==FALSE)
		return NULL;
	IplImage* pI=NULL;
	BOOL bState=FALSE;
	if(pImageRead->width!=m_Ilow1->width||pImageRead->height!=m_Ilow1->height)
	{
		pI=AutoStorage->CreateIplImage(m_Ilow1->width,m_Ilow1->height,pImageRead->depth,pImageRead->nChannels);
		cvZero(pI);
		cvResize(pImageRead,pI);
		bState=TRUE;
		/*pI=MV_NEW_IMG(pImageRead->width,pImageRead->height,m_Ilow1->depth,m_Ilow1->nChannels);
		cvResize(m_Ilow1,pI);*/

	}else{
		pI=pImageRead;
	}
	IplImage* pEdge=AutoStorage->CreateIplImage(pI->width,pI->height,8,1);
	cvCvtScale(pI,m_Iscratch,1,0);
	cvSplit(m_Iscratch,m_Igray1,m_Igray2,m_Igray3,0);
	cvInRange(m_Igray1,m_Ilow1,m_Ihi1,pEdge);
	cvInRange(m_Igray2,m_Ilow2,m_Ihi2,m_Imaskt);
	cvOr(pEdge,m_Imaskt,pEdge);
	cvInRange(m_Igray3,m_Ilow3,m_Ihi3,m_Imaskt);
	cvOr(pEdge,m_Imaskt,pEdge);
	cvSubRS(pEdge,cvScalar(255),pEdge);
	
	IplConvKernel* pElement=cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_ELLIPSE);
	cvMorphologyEx( pEdge, pEdge, NULL, pElement, CV_MOP_OPEN);
	//cvMorphologyEx( pEdge, pEdge, NULL, pElement, CV_MOP_CLOSE);	
	cvReleaseStructuringElement(&pElement);
	if(bState)
		AutoStorage->ReleaseIplImage(pI);
	return pEdge;	
	
}
//把采集的图像转化为需要的大小
IplImage* CBgSub::PrepareImage(IplImage* pImgSrc)
{	
	if(pImgSrc==NULL)
		return NULL;
	CvSize dstSize={pImgSrc->width/2,pImgSrc->height/2};
	IplImage* pImgDst=AutoStorage->CreateIplImage(dstSize.width,dstSize.height,pImgSrc->depth,pImgSrc->nChannels);
	pImgDst->origin=1;
	cvResize(pImgSrc,pImgDst,CV_INTER_AREA);
	return pImgDst;
}
void CBgSub::ReleaseImages()
{
	if(m_IavgF)
		cvReleaseImage(&m_IavgF);
	m_IavgF=NULL;
	if(m_IdiffF)
		cvReleaseImage(&m_IdiffF);
	m_IdiffF=NULL;
	if(m_IprevF)
		cvReleaseImage(&m_IprevF);
	m_IprevF=NULL;
	if(m_IhiF)
		cvReleaseImage(&m_IhiF);
	m_IhiF=NULL;
	if(m_IlowF)
		cvReleaseImage(&m_IlowF);
	m_IlowF=NULL;
	if(m_Ilow1)
		cvReleaseImage(&m_Ilow1);
	m_Ilow1=NULL;
	if(m_Ilow2)
		cvReleaseImage(&m_Ilow2);
	m_Ilow2=NULL;
	if(m_Ilow3)
		cvReleaseImage(&m_Ilow3);
	m_Ilow3=NULL;
	if(m_Ihi1)
		cvReleaseImage(&m_Ihi1);
	m_Ihi1=NULL;
	if(m_Ihi2)
		cvReleaseImage(&m_Ihi2);
	m_Ihi2=NULL;
	if(m_Ihi3)
		cvReleaseImage(&m_Ihi3);
	m_Ihi3=NULL;
	
	if(m_Iscratch)
		cvReleaseImage(&m_Iscratch);
	m_Iscratch=NULL;
	if(m_Isscratch2)
		cvReleaseImage(&m_Isscratch2);
	m_Isscratch2=NULL;
	if(m_Igray1)
		cvReleaseImage(&m_Igray1);
	m_Igray1=NULL;
	if(m_Igray2)
		cvReleaseImage(&m_Igray2);
	m_Igray2=NULL;
	if(m_Igray3)
		cvReleaseImage(&m_Igray3);
	m_Igray3=NULL;
	if(m_Imaskt)
		cvReleaseImage(&m_Imaskt);
	m_Imaskt=NULL;	
}
void CBgSub::InitCodebook()
{
	if(m_model)
		cvReleaseBGCodeBookModel(&m_model);
	m_model = cvCreateBGCodeBookModel();
	//Set color thresholds to default values
	m_model->modMin[0] = 3;
	m_model->modMin[1] = m_model->modMin[2] = 3;
	m_model->modMax[0] = 10;
	m_model->modMax[1] = m_model->modMax[2] = 10;
	m_model->cbBounds[0] = m_model->cbBounds[1] = m_model->cbBounds[2] = 10;
}
//采用了自动内存管理
void CBgSub::LearningCodeBookModel(IplImage* pI)
{
	if(pI==NULL)
		return;
	if(!m_model)
		return;
	IplImage* pYuvImage=AutoStorage->CreateIplImage(pI->width,pI->height,pI->depth,3);
	cvCvtColor( pI,pYuvImage, CV_BGR2YCrCb );
	cvBGCodeBookUpdate( m_model, pYuvImage);
	AutoStorage->ReleaseIplImage(pYuvImage);
}
void CBgSub::EndCodeBookModelLearning()
{
	if(!m_model)
		return;
	cvBGCodeBookClearStale( m_model, m_model->t/2 );
}
//采用了内存管理
IplImage* CBgSub::BackgroundDiffOfCodebook(IplImage *pI)
{
	if(pI==NULL)
		return NULL;
	IplImage* pYuvImage=AutoStorage->CreateIplImage(pI->width,pI->height,pI->depth,3);
	cvCvtColor( pI,pYuvImage, CV_BGR2YCrCb );
	IplImage* pMaskCodeBook=AutoStorage->CreateIplImage(pI->width,pI->height,IPL_DEPTH_8U, 1);
	cvSet(pMaskCodeBook,cvScalar(255));
	cvBGCodeBookDiff(m_model, pYuvImage, pMaskCodeBook);
	AutoStorage->ReleaseIplImage(pYuvImage);
	cvSegmentFGMask(pMaskCodeBook);
	return pMaskCodeBook;
}
//采用了内存管理
void CBgSub::DoShadowRemoval(IplImage* pBI,IplImage* pFI,IplImage* pMask)
{	
	if(pBI==NULL||pFI==NULL||pMask==NULL)
		return;
	IplImage* pHSV_B,*pHSV_F;
	pHSV_B=AutoStorage->CreateIplImage(pBI->width,pBI->height,8,3);
	pHSV_F=AutoStorage->CreateIplImage(pFI->width,pFI->height,8,3);
	if(pBI->depth!=8){
		IplImage* pDst=AutoStorage->CreateIplImage(pBI->width,pBI->height,8,3);
		cvCvtScale(pBI,pDst,1,0);
		cvCvtColor(pDst,pHSV_B,CV_BGR2HSV_FULL);
		AutoStorage->ReleaseIplImage(pDst);
	}else{
		cvCvtColor(pBI,pHSV_B,CV_BGR2HSV_FULL);
	}
	if(pFI->depth!=8){
		IplImage* pDst=AutoStorage->CreateIplImage(pFI->width,pFI->height,8,3);
		cvCvtScale(pFI,pDst,1,0);
		cvCvtColor(pDst,pHSV_F,CV_BGR2HSV_FULL);
		AutoStorage->ReleaseIplImage(pDst);
	}else{
		cvCvtColor(pFI,pHSV_F,CV_BGR2HSV_FULL);
	}	
	DWORD imageLen=pBI->width*pBI->height;
	uchar* pB=(uchar*)(pHSV_B->imageData);
	uchar* pF=(uchar*)(pHSV_F->imageData);
	uchar* pM=(uchar*)(pMask->imageData);
	for(int i=0;i<imageLen;++i){
		if((*pM)==0xFF){
			double vTmpB=pB[2];
			double vTmpF=pF[2];
			double vTmp=vTmpF/vTmpB;
			double sTmpB=pB[1];
			double sTmpF=pF[1];
			double hTmpB=pB[0];
			double hTmpF=pF[0];
			double sTmp=abs((double)(sTmpF-sTmpB));
			double hTmp=abs((double)(hTmpF-hTmpB));
			if(vTmp>=m_vLow&&vTmp<=m_vHi&&sTmp<=m_sT&&hTmp<=m_hT){
				*pM=0x00;
			}	
		}
		pM++;
		pB=pB+3;
		pF=pF+3;
	}
	IplConvKernel* pElement=cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_ELLIPSE);
	cvMorphologyEx( pMask, pMask, NULL, pElement, CV_MOP_OPEN);
	cvMorphologyEx( pMask, pMask, NULL, pElement, CV_MOP_CLOSE);	
	cvReleaseStructuringElement(&pElement);
	AutoStorage->ReleaseIplImage(pHSV_B);
	AutoStorage->ReleaseIplImage(pHSV_F);
}
void CBgSub::EqualizeHist_1(const IplImage* pSrc,IplImage* pDst)
{
	if(!pSrc||!pDst||pSrc->nChannels!=1||pDst->nChannels!=1)
		return;
	cvEqualizeHist(pSrc,pDst);
}
void CBgSub::EqualizeHist_3(const IplImage* pSrc,IplImage* pDst)
{
	if(!pSrc||!pDst||pSrc->nChannels!=3||pDst->nChannels!=3)
		return;
	IplImage* pB1=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pG1=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pR1=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);

	cvSplit(pSrc,pB1,pG1,pR1,NULL);
	
	IplImage* pB2=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pG2=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pR2=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	cvEqualizeHist(pR1,pR2);
	cvEqualizeHist(pB1,pB2);
	cvEqualizeHist(pG1,pG2);
	cvZero(pDst);
	cvMerge(pB2,pG2,pR2,NULL,pDst);
	AutoStorage->ReleaseIplImage(pR1);
	AutoStorage->ReleaseIplImage(pR2);
	AutoStorage->ReleaseIplImage(pB1);
	AutoStorage->ReleaseIplImage(pB2);
	AutoStorage->ReleaseIplImage(pG1);
	AutoStorage->ReleaseIplImage(pG2);
}

void CBgSub::EqualizeContrast_1(IplImage* pSrc,IplImage* pDst,int low,int high)
{
	if(!pSrc||!pDst||pSrc->nChannels!=1||pDst->nChannels!=1)
		return;
	for(int y=0;y<pSrc->height;++y){
		BYTE* pSdata=(BYTE*)(pSrc->imageData+y*pSrc->widthStep);
		BYTE* pDdata=(BYTE*)(pDst->imageData+y*pSrc->widthStep);
		for(int x=0;x<pSrc->width;++x){
			if(*pSdata==255){
				(*pDdata)=255;
			}else if(*pSdata<=low){
				*pDdata=0;
			}else{
				int tmp=(*pSdata);
				int g=255*(tmp-low)/(high-low);
				*pDdata=FMin<int>(255,g);
			}
			pSdata++;
			pDdata++;
		}
	}
}
void CBgSub::EqualizeContrast_3(IplImage* pSrc)
{
	if(!pSrc||pSrc->nChannels!=3)
		return;
	IplImage* pB=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pG=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);
	IplImage* pR=AutoStorage->CreateIplImage(pSrc->width,pSrc->height,pSrc->depth,1);

	cvSplit(pSrc,pB,pG,pR,NULL);
	EqualizeContrast_1(pB,pB,m_bLow,m_bHigh);
	EqualizeContrast_1(pG,pG,m_gLow,m_gHigh);
	EqualizeContrast_1(pR,pR,m_rLow,m_rHigh);
	cvZero(pSrc);
	cvMerge(pB,pG,pR,NULL,pSrc);
	AutoStorage->ReleaseIplImage(pR);
	AutoStorage->ReleaseIplImage(pB);
	AutoStorage->ReleaseIplImage(pG);
	
}

IplImage* CBgSub::MakeChessBoard(int imgW,int imgH,int cW,int cH)
{
	IplImage* pImgBoard=cvCreateImage(cvSize(imgW,imgH),8,1);
	for(int y=0;y<imgH;++y){
		BYTE* pData=(BYTE*)(pImgBoard->imageData+y*pImgBoard->widthStep);
		for(int x=0;x<imgW;++x){
			*pData=0xFF;
			pData++;
		}
	}
	int cNum=(imgW)/cW-1;
	int rNum=(imgH)/cH-1;
	CvRect rc;
	rc.width=cW;
	rc.height=cH;
	for(int r=1;r<rNum;++r){
		for(int c=1;c<cNum;++c){
			if((r%2==1&&c%2==0)||(r%2==0&&c%2==1)){
				rc.x=c*cW;
				rc.y=r*cH;
				cvSetImageROI(pImgBoard,rc);
				cvZero(pImgBoard);
			}
		}
	}
	rc.x=rc.y=0;
	rc.height=imgH;
	rc.width=imgW;
	cvSetImageROI(pImgBoard,rc);
	/*cvShowImage("棋盘格图像",pImgBoard);
	cvWaitKey(0);*/
	return pImgBoard;
}
void CBgSub::InitBGStatModel(IplImage* pI,int type){
	if(m_bg_model)
		cvReleaseBGStatModel(&m_bg_model);
	if(type==CV_BG_MODEL_FGD)
		m_bg_model=cvCreateFGDStatModel(pI);
	else m_bg_model = cvCreateGaussianBGModel(pI);
}
void CBgSub::UpdateBGStatModel(IplImage* pI){
	cvUpdateBGStatModel(pI,m_bg_model);
}
IplImage*  CBgSub::BackgroundDiffOfStatModel(IplImage* pI){
	cvUpdateBGStatModel(pI,m_bg_model,0);
	/*cvShowImage("foreground",m_bg_model->foreground);
	cvWaitKey(0);*/
	IplImage *fgMask=AutoStorage->CreateIplImage(pI->width,pI->height,IPL_DEPTH_8U, 1);
	cvCopy(m_bg_model->foreground,fgMask);
	cvSegmentFGMask(fgMask);
	return fgMask;
}
IplImage* CBgSub::SxfBGDiffModel(IplImage* pI)
{
	return SXF::ProcessBMP(pI);
}