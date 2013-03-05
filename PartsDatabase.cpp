#include "StdAfx.h"
#include "PartsDatabase.h"
#include "MainFrm.h"
#include <cxcore.h>
#include <ostream>
#include <opencv2\imgproc\imgproc_c.h>
#include <highgui.h>
#include "FColor.h"
#include "Hvidicon.h"

#define Sgn(num) (num<0?-1:1)
#define  MY_ZERO 0.000000001
typedef boost::mutex::scoped_lock scoped_lock;

CPartsDatabase* CPartsDatabase::_instance=NULL;
CPartsDatabase* CPartsDatabase::Instance()
{
	if(_instance==NULL){
		_instance=new CPartsDatabase();
		_instance->InitDatabase();	
	}	
	return _instance;
}
CPartsDatabase::CPartsDatabase(void)
{
	m_uuid=1;
	
	char buffer[1024];
	GetCurrentDirectory(1024, buffer);
	m_curDir.append(buffer);
	m_curDir.append("\\");
	m_bArea=1;
	
	m_bfID=1;
	m_cxID=1;
	m_dbName="Database.xml";
	m_areaRate=1.0;
	m_kpTemplate.CreateDetectorDescriptorMatcher();
	InputConfig();
}
CPartsDatabase::~CPartsDatabase(void)
{
	ClearDataBase();
}
void CPartsDatabase::InitDatabase()
{
	Load();
	m_bgSub.LoadMeanModel();
}
BOOL CPartsDatabase::CptShapeInfo(IplImage* pMask,PartsInfo& pf)
{
	//IplImage* pMask=AutoStorage->CreateIplImage(pMaskInput->width,pMaskInput->height,pMaskInput->depth,pMaskInput->nChannels);
	
	pf.area=m_spAnalysis.CalcArea(pMask);
	if(pf.area==0)
		return FALSE;
	CvBox2D box2d=m_spAnalysis.PartsAngleDetection(pMask);
	pf.rectangularity=pf.area/(box2d.size.height*box2d.size.width);
	pf.eolngatedness=FMin<float>(box2d.size.height,box2d.size.width)/FMax<float>(box2d.size.height,box2d.size.width);
	CptHuMoments(pMask,pf.huMmts);
	pf.pTemplate=CptTemplate(pMask);
	
	double arclen= m_spAnalysis.GetLength();
	pf.compactness=(arclen*arclen)/pf.area;
	return TRUE;
}
void CPartsDatabase::CptHuMoments(IplImage* pMask,CvHuMoments& huMoments)
{
	//CptHuMoments()
	if(pMask==NULL)
		return;
	CvMoments moments;
	
	cvMoments(pMask,&moments,1);                     
	/*　函数 cvMoments 计算最高达三阶的空间和中心矩，并且将结果存在结构 moments 中。
	矩用来计算形状的重心，面积，主轴和其它的形状特征，如 7 Hu 不变量等.*/
	cvGetHuMoments(&moments,&huMoments);       
	/*计算 7 Hu 不变量 　　void cvGetHuMoments( CvMoments* moments, CvHuMoments* hu_moments ); 
	　　moments：矩状态结构的指针 　　hu_momentsHu： 矩结构的指针. */
}
void CPartsDatabase::CptKeyPointsDescriptors(IplImage* pImg,CBlob* pBlob,PartsInfo& pf)
{
	Mat img(pImg);
	CvRect rc=pBlob->GetBoundingBox();
	rc.x=FMax<int>(0,rc.x-2);
	rc.y=FMax<int>(0,rc.y-2);
	rc.width=FMin<int>(rc.width+5,pImg->width-rc.x);
	rc.height=FMin<int>(rc.height+5,pImg->height-rc.y);

	Mat roi=Mat(img,Rect(rc));
	m_kpTemplate.DetectKeyPoints(roi,pf.keyPoints);
	//m_kpTemplate.DrawKeyPoints(roi,pf.keyPoints,Scalar(255,0,0));
	///*cvShowImage("surf",pImg);
	//cvWaitKey(0);*/
	
	m_kpTemplate.ComputeDescriptors(roi,pf.keyPoints,pf.kpDescriptors);
}
//利用hu矩特征进行检索
int CPartsDatabase::ShapeRetrivalByMoments(IplImage* pMask,PartsInfo& pf)
{
	
	if(!pMask)
		return -1;
	PartsInfo spInfo;
	if(!CptShapeInfo(pMask,spInfo))
		return -1;
	Iterator it=m_pfList.begin();
	double minDiff=100000000;
	int id=-1;
	for(;it!=m_pfList.end();++it){
		double diff=MatchShapesByMoments(spInfo.huMmts,it->huMmts,CV_CONTOURS_MATCH_I1);
		if(minDiff>diff){
			id=it->id;
			minDiff=diff;
			pf=*it;
		}
	}
	if(minDiff>10000)
		return -1;
	//SetShapeMatchText(m_areaDiff,m_rtDiff,m_eolDiff,m_cmptDiff);
	return id;
}

int  CPartsDatabase::ShapeRetrivalByFeature(IplImage* pMask,PartsInfo& pf,BOOL bDebug)
{
	if(!pMask)
		return -1;	
	Iterator it=m_pfList.begin();
	double minDiff=100000000;
	int id=-1;
	for(;it!=m_pfList.end();++it){
		double diff=MatchTemplate(pMask,it->pTemplate,bDebug);
		if(minDiff>diff){
			id=it->id;
			minDiff=diff;
			pf=*it;
		}
	}
	if(minDiff>10000)
		return -1;
	SetShapeMatchText(minDiff,m_rtDiff,m_eolDiff,m_cmptDiff);
	return id;
}
void CPartsDatabase::GetSymmImage(IplImage* pQTmpt)
{
	int wHalf=pQTmpt->width/2;
	RGBQUAD rgb;
	for(int y=0;y<pQTmpt->height;++y){
		BYTE* pQuery=(BYTE*)(pQTmpt->imageData+y*pQTmpt->widthStep);
		for(int x=0;x<wHalf;++x){
			FCColor::CopyPixel(&rgb,pQuery,pQTmpt->nChannels);
			FCColor::CopyPixel(pQuery,pQuery+pQTmpt->width-1-x-x,pQTmpt->nChannels);
			FCColor::CopyPixel(pQuery+pQTmpt->width-1-x-x,&rgb,pQTmpt->nChannels);
			pQuery+=pQTmpt->nChannels;
		}
	}
}

//利用基础特征进行匹配检索
double CPartsDatabase::MatchShapesByShapeFeature(PartsInfo& pf1,PartsInfo& pf2)
{
	/*if((pf1.holeNum==0&&pf2.holeNum>0)||(pf1.holeNum>0&&pf2.holeNum==0)){
		return 1000000;
	}*/
	m_areaDiff=(pf1.area-pf2.area)*(pf1.area-pf2.area)*m_bArea/m_s[0];
	m_rtDiff=(pf1.rectangularity-pf2.rectangularity)*(pf1.rectangularity-pf2.rectangularity)/m_s[1];
	m_eolDiff=(pf1.eolngatedness-pf2.eolngatedness)*(pf1.eolngatedness-pf2.eolngatedness)/m_s[2];
	m_cmptDiff=(pf1.compactness-pf2.compactness)*(pf1.compactness-pf2.compactness)/m_s[3];

	/*m_areaDiff=abs(pf1.area-pf2.area)/(m_max[0]-m_min[0]+MY_ZERO)*m_bArea;
	m_rtDiff=abs(pf1.rectangularity-pf2.rectangularity)/(m_max[1]-m_min[1]+MY_ZERO);
	m_eolDiff=abs(pf1.eolngatedness-pf2.eolngatedness)/(m_max[2]-m_min[2]+MY_ZERO);
	m_cmptDiff=abs(pf1.compactness-pf2.compactness)/(m_max[3]-m_min[3]+MY_ZERO);*/
	
	double mmDiff=MatchShapesByMoments(pf1.huMmts,pf2.huMmts,CV_CONTOURS_MATCH_I1);
	//double mmDiff=1;
	double diff=0.0;
	if(m_bArea)
		diff=m_areaDiff*m_rtDiff*m_eolDiff*m_cmptDiff*mmDiff;
	else diff=(m_rtDiff+m_eolDiff+m_cmptDiff)*mmDiff;


	//diff=m_areaDiff*m_areaDiff+m_rtDiff*m_rtDiff+m_eolDiff*m_eolDiff+m_cmptDiff*m_cmptDiff;
	return diff;
}
IplImage* CPartsDatabase::CptTemplate(IplImage* pMask)
{
	IplImage* pRomate=m_spAnalysis.Rotate(pMask);
	CvPoint orign={pMask->width/2-imgTmpltW/2,pMask->height/2-imgTmpltH/2};
	CvRect rc=cvRect(orign.x,orign.y,imgTmpltW,imgTmpltH);
	cvSetImageROI(pRomate,rc);
	IplImage* pDst=AutoStorage->CreateIplImage(imgTmpltW,imgTmpltH,8,1);
	cvZero(pDst);
	cvCopy(pRomate,pDst);
	AutoStorage->ReleaseIplImage(pRomate);
	return pDst;
}
double CPartsDatabase::MatchTemplate(IplImage* pMask,IplImage* pMaskTrain,BOOL bDebug)
{
	IplImage* pQTmpt=CptTemplate(pMask);
	if(bDebug){
		cvShowImage("qery",pQTmpt);
		cvShowImage("train",pMaskTrain);
		cvWaitKey(0);
	}
	long len=pMaskTrain->width*pMaskTrain->height;
	BYTE* pTrain=(BYTE*)(pMaskTrain->imageData);
	BYTE* pQuery=(BYTE*)(pQTmpt->imageData);
	double diff=0;
	//cvThreshold(pMaskTrain,pMaskTrain,64,255,CV_THRESH_BINARY);
//	cvThreshold(pQTmpt,pQTmpt,64,255,CV_THRESH_BINARY);
	for(int i=0;i<len;++i){
		int a=*pQuery;
		int b=*pTrain;
		if(abs(a-b)>150){
			diff++;
		}
		pQuery++;
		pTrain++;
	}
	AutoStorage->ReleaseIplImage(pQTmpt);
	return diff;
}

void CPartsDatabase::SetShapeMatchText(double areaDiff,double rtDiff,double eolDiff,double cmptDiff)
{
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CString str;
	str.Format("面积：%0.4lf,矩形度：%0.4lf,细长度:%0.4lf，精致度：%0.4lf",areaDiff,rtDiff,eolDiff,cmptDiff);
	pMainFrame->SetStatusBarText(str);
}
void CPartsDatabase::Insert(PartsInfo& pf)
{		
	scoped_lock lk(m_mutex);
	if(pf.id==-1){
		pf.id=m_uuid*100+m_bfID*10+m_cxID;
		//m_cxID++;
	}
	m_pfList.push_back(pf);
}
void CPartsDatabase::Insert(PartsInfoList& out)
{
	IplImage* pImgRead=HVidicom->CloneImageRead();	
	
	if(!pImgRead)
		return;
	IplImage* pI=m_bgSub.PrepareImage(pImgRead);
//	IplImage* pMask =m_bgSub.BackgroundDiffOfMean(pI);
	IplImage* pMask =m_bgSub.SxfBGDiffModel(pI);
	//CptSimpleFeature()
	m_br.BwLabeling(pMask,NULL,0);
	m_br.Filter(m_br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	if(m_br.GetNumBlobs()==0)
		return;
	cvZero(pMask);
	CBlob* pBlob=m_br.GetBlob(0);
	//InsertKpDescriptor(pI,pBlob,out);
	InsertHogDescriptor(pI,pBlob,out);
	m_cxID=1;
	Save();

	AutoStorage->ReleaseIplImage(pI);
	AutoStorage->ReleaseIplImage(pMask);
	cvReleaseImage(&pImgRead);
}
void CPartsDatabase::InsertHogDescriptor(IplImage* pI,CBlob* pBlob,PartsInfoList& out)
{
	PartsInfo ptInfo1;
	IplImage* pImgDst=Normalize(pI,*pBlob);	
	
	//插入零件特征
	CptSimpleFeature(*pBlob,ptInfo1);
	CptHogDescriptors(pImgDst,ptInfo1);
	ptInfo1.pTemplate=cvCreateImage(cvSize(64,64),pImgDst->depth,pImgDst->nChannels);
	cvResize(pImgDst,ptInfo1.pTemplate);	
	Insert(ptInfo1);	

	//插入零件对称方向的特征
	PartsInfo ptInfo2;
	IplImage* pDst2=AutoStorage->CreateIplImage(pImgDst->width,pImgDst->height,pImgDst->depth,pImgDst->nChannels);
	Rotate(pImgDst,pDst2,180);

	CptSimpleFeature(*pBlob,ptInfo2);
	CptHogDescriptors(pDst2,ptInfo2);
	ptInfo2.pTemplate=cvCreateImage(cvSize(64,64),pDst2->depth,pDst2->nChannels);
	cvResize(pDst2,ptInfo2.pTemplate);	
	ptInfo2.angle=180;
	Insert(ptInfo2);	


	AutoStorage->ReleaseIplImage(pImgDst);
	AutoStorage->ReleaseIplImage(pDst2);
	out.push_back(ptInfo1);
	out.push_back(ptInfo2);
	m_bfID++;
}
void CPartsDatabase::InsertKpDescriptor(IplImage* pImg,CBlob* pBlob,PartsInfoList& out)
{
	PartsInfo ptInfo;
	CptSimpleFeature(*pBlob,ptInfo);
	CptKeyPointsDescriptors(pImg,pBlob,ptInfo);
	CvRect rc=pBlob->GetBoundingBox();
	cvSetImageROI(pImg,rc);
	//IplImage* imgTmp=
	ptInfo.pTemplate=cvCreateImage(cvSize(rc.width,rc.height),8,1);
	cvCvtColor(pImg,ptInfo.pTemplate,CV_RGB2GRAY);
	Insert(ptInfo);
	out.push_back(ptInfo);
	m_bfID++;
}
void CPartsDatabase::Delete(int id)
{
	scoped_lock lk(m_mutex);
	Iterator it=m_pfList.end();
	it=Find(id);
	while(it!=m_pfList.end()){
		m_pfList.erase(it);
		it=Find(id);
	}
	Save();
}
Iterator CPartsDatabase::Find(int id)
{
	Iterator it=m_pfList.begin();
	for(;it!=m_pfList.end();++it){
		if(it->id==id)
			return it;
	}
	return m_pfList.end();
}
int CPartsDatabase::NewParts()
{
	if(m_pfList.empty()){
		m_uuid=1;
	}else{
		PartsInfo& pf=m_pfList.at(m_pfList.size()-1);
		m_uuid=(pf.id/100)+1;
	}	
	m_bfID=1;
	m_cxID=1;
	return m_uuid;
}
double CPartsDatabase::MatchShapesByMoments(CvHuMoments& huMoments1,CvHuMoments& huMoments2,int method)
{
	
	double ma[7], mb[7];
	int i, sma, smb;
	double eps = 1.e-5;
	double mmm;
	double result = 0;

	ma[0] = huMoments1.hu1;
	ma[1] = huMoments1.hu2;
	ma[2] = huMoments1.hu3;
	ma[3] = huMoments1.hu4;
	ma[4] = huMoments1.hu5;
	ma[5] = huMoments1.hu6;
	ma[6] = huMoments1.hu7;

	mb[0] = huMoments2.hu1;
	mb[1] = huMoments2.hu2;
	mb[2] = huMoments2.hu3;
	mb[3] = huMoments2.hu4;
	mb[4] = huMoments2.hu5;
	mb[5] = huMoments2.hu6;
	mb[6] = huMoments2.hu7;

	switch (method)
	{
	case 1:
		{
			for( i = 0; i < 7; i++ )
			{
				double ama = fabs( ma[i] );
				double amb = fabs( mb[i] );

				if( ma[i] > 0 )
					sma = 1;
				else if( ma[i] < 0 )
					sma = -1;
				else
					sma = 0;
				if( mb[i] > 0 )
					smb = 1;
				else if( mb[i] < 0 )
					smb = -1;
				else
					smb = 0;

				if( ama > eps && amb > eps )
				{
					ama = 1. / (sma * log10( ama ));
					amb = 1. / (smb * log10( amb ));
					result += fabs( -ama + amb );
				}
			}
			break;
		}

	case 2:
		{
			for( i = 0; i < 7; i++ )
			{
				double ama = fabs( ma[i] );
				double amb = fabs( mb[i] );

				if( ma[i] > 0 )
					sma = 1;
				else if( ma[i] < 0 )
					sma = -1;
				else
					sma = 0;
				if( mb[i] > 0 )
					smb = 1;
				else if( mb[i] < 0 )
					smb = -1;
				else
					smb = 0;

				if( ama > eps && amb > eps )
				{
					ama = sma * log10( ama );
					amb = smb * log10( amb );
					result += fabs( -ama + amb );
				}
			}
			break;
		}

	case 3:
		{
			for( i = 0; i < 7; i++ )
			{
				double ama = fabs( ma[i] );
				double amb = fabs( mb[i] );

				if( ma[i] > 0 )
					sma = 1;
				else if( ma[i] < 0 )
					sma = -1;
				else
					sma = 0;
				if( mb[i] > 0 )
					smb = 1;
				else if( mb[i] < 0 )
					smb = -1;
				else
					smb = 0;

				if( ama > eps && amb > eps )
				{
					ama = sma * log10( ama );
					amb = smb * log10( amb );
					mmm = fabs( (ama - amb) / ama );
					if( result < mmm )
						result = mmm;
				}
			}
			break;
		}
	default:
		CV_Error( CV_StsBadArg, "Unknown comparison method" );
	}

	return result;
}
void CPartsDatabase::Save()
{	
	Backup();
	std::string strDB(m_curDir);
	strDB.append(m_dbName);
	FileStorage fs(strDB,CV_STORAGE_WRITE);
	if(!fs.isOpened())
		return;
	cvWriteInt(*fs,"parts_info_count",m_pfList.size());
	int i=0;
	Iterator it=m_pfList.begin();
	for(;it!=m_pfList.end();++it,++i){
		std::stringstream buff;
		buff<<"part-"<<i<<'\0';
		cvStartWriteStruct(*fs,buff.str().c_str(),CV_NODE_MAP);

		cvWriteInt(*fs,"id",it->id);		
		cvWriteString(*fs,"name",it->name.c_str());
		cvWriteReal(*fs,"angle",it->angle);
		cvWriteReal(*fs,"area",it->area);
		cvWriteReal(*fs,"rectangularity",it->rectangularity);
		cvWriteReal(*fs,"compactness",it->compactness);
		cvWriteReal(*fs,"eolngatedness",it->eolngatedness);

		cvStartWriteStruct(*fs,"huMoments",CV_NODE_SEQ);
		char dt[2]={'d','\0'};
		cvWriteRawData(*fs,&(it->huMmts),7,dt);
		cvEndWriteStruct(*fs);
		
		
		
		Write(fs,"HOGDescriptor",it->hogDescriptors);

		if(it->pTemplate)
			cvWrite(*fs,"template_image",it->pTemplate);
		write(fs,"keypoints",it->keyPoints);
		if(!((it->kpDescriptors).empty()))
			fs<<"keypoint_descriptors"<<it->kpDescriptors;
		cvStartWriteStruct(*fs,"min_box",CV_NODE_SEQ);
		char mdt[2]={'f','\0'};
		cvWriteRawData(*fs,&(it->minBBox),5,mdt);
		cvEndWriteStruct(*fs);

		CvMat* data=m_spAnalysis.ToMatrix(it->contourTriangleLst);
		if(data){
			cvWrite(*fs,"contourTriangleList",data);
			cvReleaseMat(&data);
		}

		cvEndWriteStruct(*fs);
	}
	
	CalcStdDeviation();
}
void CPartsDatabase::Backup()
{
	CString strDB;
	CString strBackup;
	string strTmp=m_curDir+m_dbName;
	strDB.Format("%s",strTmp.c_str());
	strBackup.Format("%s.bak",strTmp.c_str());
	TRY
	{
		CFile::Remove(strBackup);
		
	}
	CATCH(CFileException, pEX)
	{
		
	}
	END_CATCH

	TRY
	{
		CFile::Rename(strDB, strBackup);
	}
	CATCH(CFileException, pEX)
	{
		
	}
	END_CATCH
		
}
void CPartsDatabase::Load()
{
	std::string strDB(m_curDir);
	strDB.append(m_dbName);
	FileStorage fs(strDB,CV_STORAGE_READ);
	if(!fs.isOpened())
		return;
	int parts_count=0;
	fs["parts_info_count"]>>parts_count;
	
	for(int i=0;i<parts_count;++i){
		PartsInfo pf;
		std::stringstream buff;
		buff<<"part-"<<i<<'\0';
		FileNode fn=fs[buff.str().c_str()];
		fn["id"]>>pf.id;
		fn["name"]>>pf.name;
		fn["angle"]>>pf.angle;
		fn["area"]>>pf.area;
		fn["rectangularity"]>>pf.rectangularity;
		fn["compactness"]>>pf.compactness;
		fn["eolngatedness"]>>pf.eolngatedness;
		

		CvFileNode* huNode=cvGetFileNodeByName(*fs,*fn,"huMoments");
		char dt[2]={'d','\0'};
		cvReadRawData(*fs,huNode,&(pf.huMmts),dt);
		
		Read(fn["HOGDescriptor"],pf.hogDescriptors);
		pf.pTemplate=(IplImage*)cvReadByName(*fs,*fn,"template_image");
		readKeyPoints(fn["keypoints"],pf.keyPoints);
		fn["keypoint_descriptors"]>>pf.kpDescriptors;
		CvFileNode* mbNode=cvGetFileNodeByName(*fs,*fn,"min_box");
		char mdt[2]={'f','\0'};
		cvReadRawData(*fs,mbNode,&(pf.minBBox),mdt);
		CvMat* mat=(CvMat*)cvReadByName(*fs,*fn,"contourTriangleList");
		m_spAnalysis.FromMatrix(mat,pf.contourTriangleLst);
		cvReleaseMat(&mat);
		m_trainDescriptors.push_back(pf.kpDescriptors);
		m_pfList.push_back(pf);
	}
	m_kpTemplate.Add(m_trainDescriptors);
	CalcStdDeviation();
}


void CPartsDatabase::GetIplImage(PartsInfo& pf,IplImage* pMaskShow){
	
}
void CPartsDatabase::ClearDataBase()
{
	Iterator it=m_pfList.begin();
	for(;it!=m_pfList.end();++it){		
		cvReleaseImage(&(it->pTemplate));
	}
	m_pfList.clear();
	Save();
}
void CPartsDatabase::CalcStdDeviation()
{
	int num=m_pfList.size();
	if(num<=1)
		return;
	for(int i=0;i<4;++i){
		m_mean[i]=0;
		m_s[i]=0;
		m_max[i]=0;
		m_min[i]=100000000;
	}
	Iterator it=m_pfList.begin();
	for (;it!=m_pfList.end();++it){
		m_mean[0]=m_mean[0]+it->area;
		m_mean[1]=m_mean[1]+it->rectangularity;
		m_mean[2]=m_mean[2]+it->eolngatedness;
		m_mean[3]=m_mean[3]+it->compactness;
		if(m_min[0]>it->area)
			m_min[0]=it->area;
		if(m_max[0]<it->area)
			m_max[0]=it->area;

		if(m_min[1]>it->rectangularity)
			m_min[1]=it->rectangularity;
		if(m_max[1]<it->rectangularity)
			m_max[1]=it->rectangularity;
		
		if(m_min[2]>it->eolngatedness)
			m_min[2]=it->eolngatedness;
		if(m_max[2]<it->eolngatedness)
			m_max[2]=it->eolngatedness;

		if(m_min[3]>it->compactness)
			m_min[3]=it->compactness;
		if(m_max[3]<it->compactness)
			m_max[3]=it->compactness;

	}
	
	for(int i=0;i<4;++i){
		m_mean[i]=m_mean[i]/num;
	}
	
	for(it=m_pfList.begin();it!=m_pfList.end();++it){
		m_s[0]+=(it->area-m_mean[0])*(it->area-m_mean[0]);
		m_s[1]+=(it->rectangularity-m_mean[1])*(it->rectangularity-m_mean[1]);
		m_s[2]+=(it->eolngatedness-m_mean[2])*(it->eolngatedness-m_mean[2]);
		m_s[3]+=(it->compactness-m_mean[3])*(it->compactness-m_mean[3]);
	}
	for(int j=0;j<4;++j){
		m_s[j]=m_s[j]/(num-1);
	}	
}
void CPartsDatabase::readKeyPoints(const FileNode& node, std::vector<KeyPoint>& keypoints)
{
	keypoints.resize(0);
	FileNodeIterator it = node.begin(), it_end = node.end();
	for( ; it != it_end; )
	{
		KeyPoint kpt;
		it >> kpt.pt.x >> kpt.pt.y >> kpt.size >> kpt.angle >> kpt.response >> kpt.octave >> kpt.class_id;
		keypoints.push_back(kpt);
	}
}

void CPartsDatabase::CptHogDescriptors(IplImage* pImgDst,PartsInfo& pf)
{
	HOGDescriptor hog(Size(128,128),Size(32,32),Size(16,16),Size(16,16),12);       //得到图像的大小
	Mat img(pImgDst);
	hog.compute(img,pf.hogDescriptors);             //特征提取
	
}
double CPartsDatabase::MatchShapesByHOG(vector<float>& hogDescriptor1,vector<float>& hogDescriptor2,double minDiff)
{
	vector<float>::iterator it1=hogDescriptor1.begin();
	vector<float>::iterator it2=hogDescriptor2.begin();
	double diff=0.0;
	for(;it1!=hogDescriptor1.end()&&it2!=hogDescriptor2.end();++it1,++it2){
		diff+=((*it1)-(*it2))*((*it1)-(*it2));
		if(diff>=minDiff){
			return diff;
		}
	}
	return diff;
}
int  CPartsDatabase::ShapeRetrivalByHOGFeature(IplImage* pImg,IplImage* pMask,PartsInfo& pf,BOOL bDebug)
{
	if(!pImg||!pMask)
		return -1;
	if(m_spAnalysis.CalcArea(pMask)<10)
		return -1;
	PartsInfo spInfo;
	CptHogDescriptors(pImg,spInfo);

	Iterator it=m_pfList.begin();
	double minDiff=100000000;
	int id=-1;
	for(;it!=m_pfList.end();++it){
		double diff=MatchShapesByHOG(spInfo.hogDescriptors,it->hogDescriptors);
		if(minDiff>diff){
			id=it->id;
			minDiff=diff;
			pf=*it;
		}
	}
	if(minDiff>10000)
		return -1;
	return id;
}

BOOL CPartsDatabase::CptPartsInfo(IplImage* pImg,IplImage* pMask,PartsInfo& pf)
{
	IplImage* pImgDst=AutoStorage->CreateIplImage(pImg->width,pImg->height,pImg->depth,pImg->nChannels);
	
	m_spAnalysis.Rotate(pImg,pMask,pImgDst,NULL);
	CptHogDescriptors(pImgDst,pf);
	
	//GetSymmImage()
	AutoStorage->ReleaseIplImage(pImgDst);
	return TRUE;
}


void CPartsDatabase::Write(FileStorage& fs, const string& objname, vector<float>& hogDescriptor)
{
	WriteStructContext ws(fs, objname, CV_NODE_SEQ);
	vector<float>::iterator it=hogDescriptor.begin();
	for( ;it!=hogDescriptor.end();++it)
	{
		write(fs, *it);
	}
}
void CPartsDatabase::Read(const FileNode& node, vector<float>& hogDescriptor)
{
	hogDescriptor.resize(0);
	FileNodeIterator it = node.begin(), it_end = node.end();
	for( ; it != it_end; )
	{
		float val;
		it>>val;
		hogDescriptor.push_back(val);
	}
	
} 
IplImage* CPartsDatabase::Normalize(IplImage* pImg,CBlob& blob,int w,int h)
{
	IplImage* pImgTmp=AutoStorage->CreateIplImage(pImg->width,pImg->height,pImg->depth,pImg->nChannels);
	IplImage* pMask=AutoStorage->CreateIplImage(pImg->width,pImg->height,8,1);
	cvZero(pMask);
	blob.FillBlob( pMask, CV_CVX_WHITE);
	m_spAnalysis.Rotate(pImg,pMask,blob.GetMinBoundingRect(),pImgTmp,FMax<int>(w,h));
	
	
	CvPoint orign={pImgTmp->width/2-w/2,pImgTmp->height/2-h/2};
	CvRect rc=cvRect(orign.x,orign.y,w,h);
	cvSetImageROI(pImgTmp,rc);
	/*cvShowImage("sf",pImgTmp);
	cvWaitKey(0);*/
	IplImage* pImgDst=AutoStorage->CreateIplImage(w,h,8,1);
	cvCvtColor(pImgTmp,pImgDst,CV_RGB2GRAY);
	AutoStorage->ReleaseIplImage(pMask);
	AutoStorage->ReleaseIplImage(pImgTmp);

	/*cvShowImage("sf",pImgDst);
	cvWaitKey(0);*/
	return pImgDst;
}

//主要的和库比较的函数
void CPartsDatabase::PartsRetrieval(IplImage* pImg,CBlobResult& br,ResultSet& rs,FeatureSel fsel,IplImage* pMask)
//
{
	for(int i=0;i<br.GetNumBlobs();++i){	
		PartsInfo ptInfoIn;                   //特征结构体
		CBlob* pBlob=br.GetBlob(i);                              //
	
		CptSimpleFeature(*pBlob,ptInfoIn);                      //
		switch(fsel){
			case FS_HOG:
				{
					IplImage* pImgDst=Normalize(pImg,*pBlob);        //
					CptHogDescriptors(pImgDst,ptInfoIn);              //
					AutoStorage->ReleaseIplImage(pImgDst);
				}
				break;
			case FS_SURF:
				{
					CptKeyPointsDescriptors(pImg,pBlob,ptInfoIn);
				}
				break;
			case FS_PARTSSEGMNT:
				{
					
					PartsRecognition(pBlob,pMask);
					break;
				}
		}
		if(fsel==FS_PARTSSEGMNT)
			continue;
		Result out;
		if(SinglePartDetection(ptInfoIn,out,fsel)){                              //只包含零件特征 无位置
			rs.push_back(out);        //添加判断完了数据到rs；
		}		
	}
}
BOOL CPartsDatabase::SinglePartDetection(PartsInfo& ptInfoIn,Result& out,FeatureSel fsel)
{
	Iterator it=m_pfList.begin();
	double minDiff=DBL_MAX;
	for(;it!=m_pfList.end();++it){
		if(it->id==811)
			continue;
		double diff=DBL_MAX;
		switch (fsel){
			case FS_HOG:
				{
					
					diff=MatchShapesByHOG(ptInfoIn.hogDescriptors,it->hogDescriptors,minDiff);
				}
				break;
			case FS_SURF:
				{
					if(ptInfoIn.kpDescriptors.empty())
						break;
					diff=m_kpTemplate.Match(ptInfoIn.kpDescriptors,it->kpDescriptors);
					
				}
				break;

		}
		
		if(minDiff>diff){
			out.id=it->id;
			out.name=it->name;
			out.angle=it->angle;
			out.pTemplate=it->pTemplate;
			out.diff=diff;
			minDiff=diff;
		}
	}
	return TRUE;
}

BOOL CPartsDatabase::CptSimpleFeature(CBlob& blob,PartsInfo& ptInfo)
{
	ptInfo.area=CamCalib->ICSArea2WCSArea(blob.Area());
	CvBox2D box2d=blob.GetMinBoundingRect();
	ptInfo.minBBox=m_spAnalysis.GetRealMinBox(box2d);

	Pt32fVec dpList;
	blob.FindDominantPoints(HVidicom->m_p1,HVidicom->m_p2,HVidicom->m_p2);
	CamCalib->ICSpts2WCSpts(blob.GetDominantPoints(),dpList);
	m_spAnalysis.CptContourTriangle(dpList,ptInfo.contourTriangleLst);

	ptInfo.rectangularity=blob.Area()/(box2d.size.height*box2d.size.width);
	ptInfo.eolngatedness=FMin<float>(box2d.size.height,box2d.size.width)/FMax<float>(box2d.size.height,box2d.size.width);
	double arclen= blob.Perimeter();
	ptInfo.compactness=(arclen*arclen)/blob.Area();

	return TRUE;
}


void  CPartsDatabase::Rotate(IplImage* pSrc,IplImage* pDst,double angle)
{
	float m[6];
	m[0] = (float)(cos(angle*CV_PI/180.));
	m[1] = (float)(sin(angle*CV_PI/180.));
	m[2] = pSrc->width*0.5f;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = pSrc->height*0.5f;
	CvMat M = cvMat (2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(pSrc, pDst, &M);
}
void CPartsDatabase::InputConfig()
{
	CHAR buffer[100];	
	CString strConfig;
	strConfig.Format("%sconfig.ini",m_curDir.c_str());
	GetPrivateProfileString("Retrieval Parameter", "db_name","Database.xml",buffer,sizeof(buffer),strConfig);
	m_dbName.clear();
	m_dbName.append(buffer);

	m_bArea=GetPrivateProfileInt("Retrieval Parameter", "area_flag", 1, strConfig);
}

void CPartsDatabase::MatchTriangleformDB(int qryIdx,Triangle& tri,TMatchVec& out)
{
	Iterator it=m_pfList.begin();
	for(;it!=m_pfList.end();++it){
		TriVec& cntLst=it->contourTriangleLst;
		TriVec::iterator tit=cntLst.begin();
		for (;tit!=cntLst.end();++tit)
		{
			tit->diff=m_spAnalysis.MatchTriangle(tri,*tit);
		}
	}
	for(int i=0;i<4;++i){
		double minDiff=DBL_MAX;
		int    imgIdx=-1;
		int    triIdx=-1;
		it=m_pfList.begin();
		for(int r=0;it!=m_pfList.end();++it,++r){
			TriVec& cntLst=it->contourTriangleLst;
			TriVec::iterator tit=cntLst.begin();
			for (int c=0;tit!=cntLst.end();++tit,++c)
			{
				if(tit->diff<0)
					continue;
				if(minDiff>tit->diff){
					imgIdx=r;
					triIdx=c;
					minDiff=tit->diff;
					//triMatch=*tit;
				}
			}
		}

		TMatch tm;
		tm.imgIdx=imgIdx;
		tm.qryIdx=qryIdx;
		tm.triIdx=triIdx;
		TriVec& cntLst=m_pfList[imgIdx].contourTriangleLst;
		if(cntLst.empty())
			return;
		tm.tri=cntLst.at(triIdx);
		cntLst[triIdx].diff=-1;
		out.push_back(tm);
	}
}
void    CPartsDatabase::PartsRecognition(CBlob* pBlob,IplImage* pMask)
{
	if(!pBlob)
		return;
	std::vector<CvPoint>& dpList=pBlob->GetDominantPoints();
	std::vector<CvPoint2D32f> wcsDpLst;
	CamCalib->ICSpts2WCSpts(dpList,wcsDpLst);
	TriVec triVecOut;
	m_spAnalysis.CptContourTriangle(wcsDpLst,triVecOut);
	while(HaveNext(triVecOut)){
		TMatch tmBest;
		CvBox2D bbox=PartsSegmentation(triVecOut,wcsDpLst,tmBest,pMask);
		RemoveMatchPart(bbox,triVecOut);
	}
}
CvBox2D CPartsDatabase::PartsSegmentation(TriVec& triLst,Pt32fVec& dpLst,TMatch& tmBest,IplImage* pMask)
{
	double minDiff=DBL_MAX;
	Triangle tri;
	int best=0;
	TriVec::iterator it=triLst.begin();
	for(int i=0;it!=triLst.end();++it,++i){
		//m_spAnalysis.DrawTrianglePoints(*it,CV_RGB(255,255,255),pMask,0);
		if(it->diff==1.0)
			continue;
		TMatchVec matchVec;
		MatchTriangleformDB(i,*it,matchVec);

		int bestTmp;
		double diff=KeyPointsMatch(*it,dpLst,matchVec,bestTmp,pMask);
		if(minDiff>diff){
			best=i;
			tri=*it;
			minDiff=diff;
			tmBest=matchVec[bestTmp];
		}
		//增加后续处理程序，开始测试时可以只简单绘制出来
		/*Triangle triDst;
		CvMat* warp_mat=m_spAnalysis.GetAffineTransform(matchVec[best].tri,tri);	
		m_spAnalysis.Transform(matchVec[best].tri,triDst,warp_mat);
		m_spAnalysis.DrawTrianglePoints(triDst,CV_RGB(255,255,255),pMask,1);
		cvReleaseMat(&warp_mat);*/
		//m_spAnalysis.DrawMinBoundingRect(bbox,pMask);
		//TriVec& cntLst=m_pfList[matchVec[best].imgIdx].contourTriangleLst;
		//m_spAnalysis.DrawTrianglePoints(tri,matchVec[best],cntLst,CV_RGB(255,255,255),pMask);
		//	RemoveMatchPart(bbox,matchVec[best]);
	}
	triLst[best].diff=1.0;
	TriVec& cntLst=m_pfList[tmBest.imgIdx].contourTriangleLst;
	//CShapeAnalysis::DrawTrianglePoints(tri,CV_RGB(0,0,255),pMask);
	//m_spAnalysis.DrawTrianglePoints(tri,tmBest,cntLst,CV_RGB(0,255,0),pMask);
	CvBox2D box2d=m_pfList[tmBest.imgIdx].minBBox;
	//m_spAnalysis.DrawMinBoundingRect(box2d,pMask);
	return m_spAnalysis.CptMinBoundingRect(tmBest.tri,tri,box2d,pMask);
}
//对每个三角特征的前k个最佳匹配，通过其他角点的配对来选择最佳的配对
double CPartsDatabase::KeyPointsMatch(Triangle& tri,Pt32fVec& dpLst,TMatchVec& matchVec,int& best,IplImage* pMask)
{
	TMatchVec::iterator it=matchVec.begin();
	double minDiff=DBL_MAX;
	for(int i=0;it!=matchVec.end();++it,++i){
		CvMat* warp_mat=m_spAnalysis.GetAffineTransform(it->tri,tri);	
		TriVec& cntLstSrc=m_pfList[it->imgIdx].contourTriangleLst;
		double diff=m_spAnalysis.MatchDominantPoints(cntLstSrc,dpLst,warp_mat);
		
		if(minDiff>diff){
			minDiff=diff;
			best=i;
		}
		cvReleaseMat(&warp_mat);
	}
	/*CvBox2D box2d=m_pfList[matchVec[best].imgIdx].minBBox;
	return m_spAnalysis.CptMinBoundingRect(matchVec[best].tri,tri,box2d);*/
	return minDiff;
}
BOOL  CPartsDatabase::HaveNext(TriVec& triLst)
{	
	/*static int flag=1;
	if(flag==1){
		flag++;
		return TRUE;
	}
	return FALSE;*/


	TriVec::iterator it=triLst.begin();
	for(;it!=triLst.end();++it){
		if(it->diff!=1.0)
			return TRUE;
	}
	return FALSE;
}
void  CPartsDatabase::RemoveMatchPart(CvBox2D bbox,TriVec& triLst)
{
	CvPoint2D32f pt2d[4];
	bbox.size.height+=15;
	bbox.size.width+=15;
	cvBoxPoints(bbox,pt2d);
	CvMat* B=cvCreateMat(4,1,CV_32FC2);
	cvSetData(B,pt2d,B->step);
	TriVec::iterator it=triLst.begin();
	for(;it!=triLst.end();++it){
		CvPoint2D32f pt;
		CamCalib->CptICSPoint(it->pos,pt);
		if(cvPointPolygonTest(B,pt,0)>=0){
			it->diff=1.0;
		}
	}
	cvReleaseMat(&B);
}

void CPartsDatabase::Insert(IplImage* pImg,IplImage* pMask,PartsInfo& ptInfo)
{
	NewParts();

	m_br.BwLabeling(pMask,NULL,0);
	m_br.Filter(m_br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	if(m_br.GetNumBlobs()==0)
		return;
	cvZero(pMask);
	CBlob* pBlob=m_br.GetBlob(0);
	
	CptSimpleFeature(*pBlob,ptInfo);
	
	CvRect rc=pBlob->GetBoundingBox();
	cvSetImageROI(pImg,rc);
	IplImage* imgTmp=cvCreateImage(cvSize(rc.width,rc.height),8,1);
	ptInfo.pTemplate=cvCreateImage(cvSize(64,64),8,1);
	cvCvtColor(pImg,imgTmp,CV_RGB2GRAY);
	cvResize(imgTmp,ptInfo.pTemplate);
	cvReleaseImage(&imgTmp);
	Insert(ptInfo);	
	Save();
}