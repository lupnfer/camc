#include "StdAfx.h"
#include "CameraCalibration.h"
#include <string>
#include "PartsDatabase.h"
#include "Hvidicon.h"
#include <algorithm>
#include <functional>

typedef boost::mutex::scoped_lock scoped_lock;

CCameraCalibration* CCameraCalibration::_instance=NULL;
CCameraCalibration* CCameraCalibration::Instance(){
	if(_instance==NULL)
		_instance=new CCameraCalibration();
	return _instance;
}
CCameraCalibration::CCameraCalibration(void)
{
	m_homography=NULL;
	m_invhomography=NULL;
	m_corners=NULL;
	m_cornersWCS=NULL;
	m_cornersICS=NULL;
	m_cornerCount=0;
	m_originIdx=0;
	m_unitH=0.0;
	m_unitW=0.0;
	m_areaRate=-1;
	Load();
}

CCameraCalibration::~CCameraCalibration(void)
{
	if(m_homography)
		cvReleaseMat(&m_homography);
	if(m_invhomography)
		cvReleaseMat(&m_invhomography);
	
	if(m_cornersWCS)
		cvReleaseMat(&m_cornersWCS);
	if(m_cornersICS)
		cvReleaseMat(&m_cornersICS);

	if(m_corners)
		delete[] m_corners;
	
}
BOOL CCameraCalibration::FindChessBoardCorners(IplImage* image,
											  CvSize pattern_size,int flag,int nboard)
{
	scoped_lock lk(m_mutex);
	if(!image)
		return FALSE;
	if(m_corners)
		delete[] m_corners;
	m_width=image->width;
	m_height=image->height;
	m_boardSize=pattern_size;
	m_corners=new CvPoint2D32f[pattern_size.height*pattern_size.width];
	int found=cvFindChessboardCorners(image,pattern_size,m_corners,&m_cornerCount,flag);
	if(!found){
		AfxMessageBox("角点检测失败！可能配置文件中已知角点数输入有错！");
		return FALSE;
		
	}
	IplImage* gray_image=cvCreateImage(cvGetSize(image),8,1);
	cvCvtColor(image,gray_image,CV_RGB2GRAY);
	cvFindCornerSubPix(gray_image,m_corners,m_cornerCount,cvSize(5,5),cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,100,0.01));
	cvDrawChessboardCorners(image,pattern_size,m_corners,m_cornerCount,found);
	cvReleaseImage(&gray_image);
	if(m_cornerCount==nboard)
		return TRUE;
	return FALSE;
}
void CCameraCalibration::Findhomography()
{
	
	CptWCSCorners();
	{
		scoped_lock lk(m_mutex);
		if(m_homography)
			cvReleaseMat(&m_homography);
		m_homography=cvCreateMat(3,3,CV_32FC1);
		cvFindHomography(m_cornersICS,m_cornersWCS,m_homography);

		if(m_invhomography)
			cvReleaseMat(&m_invhomography);
		m_invhomography=cvCreateMat(3,3,CV_32FC1);
		cvFindHomography(m_cornersWCS,m_cornersICS,m_invhomography);
	}	
	Save();
}
void CCameraCalibration::SetOriginPoint(POINT p)
{
	scoped_lock lk(m_mutex);
	float minDiff=100000;
	for(int i=0;i<m_cornerCount;++i){
		CvPoint2D32f pt2D=m_corners[i];
		float diff=(pt2D.x-p.x)*(pt2D.x-p.x)+(pt2D.y-p.y)*(pt2D.y-p.y);
		if(minDiff>diff){
			minDiff=diff;
			m_originIdx=i;
		}
	}
	
	if(m_originIdx>=0){
		m_origin=m_corners[m_originIdx];
	}
}
void CCameraCalibration::CptWCSCorners()
{
	scoped_lock lk(m_mutex);
	if(m_cornersICS)
		cvReleaseMat(&m_cornersICS);
	if(m_cornersWCS)
		cvReleaseMat(&m_cornersWCS);
	

	int x_origin=m_originIdx%m_boardSize.width;
	int y_origin=m_originIdx/m_boardSize.width;

	m_cornersICS=cvCreateMat(m_cornerCount,3,CV_32FC1);
	m_cornersWCS=cvCreateMat(m_cornerCount,3,CV_32FC1);
	for(int i=0;i<m_cornerCount;++i){
		CV_MAT_ELEM(*m_cornersICS,float,i,0)=m_corners[i].x;
		CV_MAT_ELEM(*m_cornersICS,float,i,1)=m_corners[i].y;
		CV_MAT_ELEM(*m_cornersICS,float,i,2)=1.0f;
		int x=(i%m_boardSize.width-x_origin);
		int y=(i/m_boardSize.width-y_origin);
		CV_MAT_ELEM(*m_cornersWCS,float,i,0)=x*m_unitW;
		CV_MAT_ELEM(*m_cornersWCS,float,i,1)=y*m_unitH;
		CV_MAT_ELEM(*m_cornersWCS,float,i,2)=1.0f;
	}
	CptAreaRate();
}
void CCameraCalibration::Save()
{
	scoped_lock lk(m_mutex);
	if(!m_homography)
		return;
	std::string strDB(PartsDB->m_curDir);
	strDB.append("Homography.xml");
	FileStorage fs(strDB,CV_STORAGE_WRITE);
	if(!fs.isOpened())
		return;
	fs<<"origin_idx"<<m_originIdx;
	fs<<"board_width"<<m_boardSize.width;
	fs<<"board_height"<<m_boardSize.height;
	fs<<"corner_count"<<m_cornerCount;
	{
		WriteStructContext ws(fs,"corners", CV_NODE_SEQ);
		for(int i=0;i<m_cornerCount;++i)
		{
			write(fs, m_corners[i].x);
			write(fs, m_corners[i].y);
		}
	}	
	
	cvWrite(*fs,"homography",m_homography);
	cvWrite(*fs,"invhomography",m_invhomography);
}
void CCameraCalibration::Load()
{
	{
		scoped_lock lk(m_mutex);
		std::string strDB(PartsDB->m_curDir);
		strDB.append("Homography.xml");
		FileStorage fs(strDB,CV_STORAGE_READ);
		if(!fs.isOpened())
			return;
		if(m_homography)
			cvReleaseMat(&m_homography);
		if(m_invhomography)
			cvReleaseMat(&m_invhomography);
		fs["origin_idx"]>>m_originIdx;
		fs["board_width"]>>m_boardSize.width;
		fs["board_height"]>>m_boardSize.height;
		fs["corner_count"]>>m_cornerCount;
		if(m_corners)
			delete[] m_corners;
		m_corners=new CvPoint2D32f[m_cornerCount];
		FileNode node=fs["corners"];
		FileNodeIterator it = node.begin(), it_end = node.end();
		for(int i=0; it != it_end; i++)
		{
			if(i%2==0)
				it>>m_corners[i/2].x;
			else it>>m_corners[i/2].y;
		}

		m_homography=(CvMat*)cvReadByName(*fs,NULL,"homography");
		m_invhomography=(CvMat*)cvReadByName(*fs,NULL,"invhomography");

		m_origin=m_corners[m_originIdx];
	}	
	// TODO: 在此添加命令处理程序代码
	CHAR buffer[100];
	GetPrivateProfileString("Calibration Parameter", "unit_width","6",buffer,sizeof(buffer),HVidicom->m_configFile);
	float w;
	sscanf(buffer,"%f",&w);
	GetPrivateProfileString("Calibration Parameter", "unit_height","6",buffer,sizeof(buffer),HVidicom->m_configFile);
	float h;
	sscanf(buffer,"%f",&h);
	SetUnitWH(w,h);
	CptWCSCorners();
	
}
void CCameraCalibration::CptWCSPoint(CvPoint2D32f ptICS,CvPoint2D32f& ptWCS)
{
	scoped_lock lk(m_mutex);
	if(!m_homography)
		return;
	CvMat* src=MV_NEW_MAT(1,1,CV_32FC3);
	CV_MAT_ELEM(*src,float,0,0)=ptICS.x;
	CV_MAT_ELEM(*src,float,0,1)=ptICS.y;
	CV_MAT_ELEM(*src,float,0,2)=1.0f;
	CvMat* dst=MV_NEW_MAT(1,1,CV_32FC3);
	cvTransform(src,dst,m_homography);	
	float t=CV_MAT_ELEM(*dst,float,0,2);
	ptWCS.x=CV_MAT_ELEM(*dst,float,0,0)/t;
	ptWCS.y=CV_MAT_ELEM(*dst,float,0,1)/t;
	MV_DEL_MAT(src);
	MV_DEL_MAT(dst);
}
void CCameraCalibration::CptICSPoint(CvPoint2D32f ptWCS,CvPoint2D32f& ptICS)
{
	scoped_lock lk(m_mutex);
	if(!m_invhomography)
		return;
	CvMat* src=MV_NEW_MAT(1,1,CV_32FC3);
	CV_MAT_ELEM(*src,float,0,0)=ptWCS.x;
	CV_MAT_ELEM(*src,float,0,1)=ptWCS.y;
	CV_MAT_ELEM(*src,float,0,2)=1.0f;
	CvMat* dst=MV_NEW_MAT(1,1,CV_32FC3);
	cvTransform(src,dst,m_invhomography);	
	float t=CV_MAT_ELEM(*dst,float,0,2);
	ptICS.x=CV_MAT_ELEM(*dst,float,0,0)/t;
	ptICS.y=CV_MAT_ELEM(*dst,float,0,1)/t;
	MV_DEL_MAT(src);
	MV_DEL_MAT(dst);
}
double CCameraCalibration::UnitTest(std::vector<WCSPoint>& errorArray)
{
	double maxDiff=0;
	double meanDiff=0;
	for(int i=0;i<m_cornerCount;++i){
		CvPoint2D32f ptICS=m_corners[i];
		CvPoint2D32f ptWCS;
		CptWCSPoint(ptICS,ptWCS);
		float x=CV_MAT_ELEM(*m_cornersWCS,float,i,0);
		float y=CV_MAT_ELEM(*m_cornersWCS,float,i,1);
		double diff=(ptWCS.x-x)*(ptWCS.x-x)+(ptWCS.y-y)*(ptWCS.y-y);
		WCSPoint wcsPt;
		wcsPt.point=ptWCS;
		wcsPt.diff=diff;
		errorArray.push_back(wcsPt);
		if(maxDiff<diff)
			maxDiff=diff;
		meanDiff+=diff;
	}	
	maxDiff=cvSqrt(maxDiff);
	meanDiff=cvSqrt(meanDiff/m_cornerCount);
	CString strFormat;
	strFormat.Format("最大误差：%0.3f毫米,平均误差：%0.3f毫米",maxDiff,meanDiff);
	AfxMessageBox(strFormat);
	return maxDiff;
}
BOOL CCameraCalibration::IsValid()
{
	scoped_lock lk(m_mutex);
	if(m_homography&&m_invhomography) 
		return TRUE;
	return FALSE;
}
void CCameraCalibration::DrawWordCoordiateSystem(IplImage* img)
{
	scoped_lock lk(m_mutex);
	if(img==NULL||m_corners==NULL||m_homography==NULL)
		return;
	CvPoint origin={m_origin.x,m_origin.y};
	cvCircle(img,origin,5,CV_RGB(0,0,255),CV_FILLED);
	int x=m_originIdx%m_boardSize.width;		
	int y=m_originIdx/m_boardSize.width;
	CvPoint p0={origin.x,origin.y};
	for(int i=0;i<m_cornerCount;++i){
		int xTmp=i%m_boardSize.width;
		int yTmp=i/m_boardSize.width;
		if(xTmp==0&&yTmp==y){
			CvPoint p1={m_corners[i].x,m_corners[i].y};
			cvLine(img,p1,p0,CV_RGB(0,0,255),2,CV_AA);
		}else if(xTmp==m_boardSize.width-1&&yTmp==y){
			CvPoint p2={m_corners[i].x,m_corners[i].y};
			cvLine(img,p0,p2,CV_RGB(0,0,255),2,CV_AA);
			CvPoint p3={p2.x-13,p2.y+7};
			cvLine(img,p3,p2,CV_RGB(0,0,255),2,CV_AA);
			CvPoint p4={p2.x-13,p2.y-7};
			cvLine(img,p4,p2,CV_RGB(0,0,255),2,CV_AA);
		}else if(xTmp==x&&yTmp==m_boardSize.height-1){
			CvPoint p2={m_corners[i].x,m_corners[i].y};
			cvLine(img,p0,p2,CV_RGB(0,0,255),2,CV_AA);
			CvPoint p3={p2.x+7,p2.y-13};
			cvLine(img,p3,p2,CV_RGB(0,0,255),2,CV_AA);
			CvPoint p4={p2.x-7,p2.y-13};
			cvLine(img,p4,p2,CV_RGB(0,0,255),2,CV_AA);
		}else if(xTmp==x&&yTmp==0){
			CvPoint p1={m_corners[i].x,m_corners[i].y};
			cvLine(img,p1,p0,CV_RGB(0,0,255),2,CV_AA);
		}
	}
}

void CCameraCalibration::ICSpts2WCSpts(std::vector<CvPoint>& icsPts,
									        std::vector<CvPoint2D32f>& wcsPts)
{
	std::vector<CvPoint>::iterator it=icsPts.begin();
	for(;it!=icsPts.end();++it){
		CvPoint2D32f icsPt={it->x,it->y};
		CvPoint2D32f wcsPt;
		CptWCSPoint(icsPt,wcsPt);
	//	wcsPts.push_back(wcsPt);
		wcsPts.push_back(icsPt);
	}	
}
double CCameraCalibration::ICSArea2WCSArea(double icsArea)
{
	return (icsArea*m_areaRate);
}
void CCameraCalibration::CptAreaRate()
{
	if(!m_corners)
		return;
	double area=0.0;
	
	area=(m_corners[0].x *m_corners[1].y-m_corners[0].y*m_corners[1].x);
	double w=sqrt((m_corners[0].x-m_corners[1].x)*(m_corners[0].x-m_corners[1].x)+(m_corners[0].y-m_corners[1].y)*(m_corners[0].y-m_corners[1].y));
	double h=sqrt((m_corners[0].x-m_corners[m_boardSize.width].x)*(m_corners[0].x-m_corners[m_boardSize.width].x)+(m_corners[0].y-m_corners[m_boardSize.width].y)*(m_corners[0].y-m_corners[m_boardSize.width].y));
	area+=(m_corners[1].x *m_corners[m_boardSize.width+1].y-m_corners[1].y*m_corners[m_boardSize.width+1].x);
	area+=(m_corners[m_boardSize.width+1].x *m_corners[m_boardSize.width].y-m_corners[m_boardSize.width+1].y*m_corners[m_boardSize.width].x);
	area+=(m_corners[m_boardSize.width].x *m_corners[0].y-m_corners[m_boardSize.width].y*m_corners[0].x);	            
	area=area/2;
	if(area==0){
		m_areaRate=-1;
		return;
	}
	m_areaRate=(m_unitH*m_unitW)/area;
}