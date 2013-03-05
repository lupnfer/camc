#pragma once
#include <vector>
#include <string>
#include<opencv2\imgproc\types_c.h>
#include "StdDefine.h"
#include "ShapeAnalysis.h"
#include "KeyPointTemplate.h"
#include "BgSub.h"
#include "Blob.h"
#include "BlobResult.h"
#include <limits> 
/********************
*零件数据库，负责零件图像，特征的保存，读取
*,检索（由PartsRetrival类来实现）
* 数据库采用文件系统来实现（特征保存在一个文件里、零件图片和轮廓图保存在文件夹中）
//一个mxl文件就包含了所有的？
*********************/
using namespace cv;

typedef struct tagResult{
	int             id;
	string          name;
	double          angle;
	CvPoint2D32f    position;           //世界坐标
	double          diff;
	IplImage        *pTemplate;//零件图像模板    
	tagResult():id(-1),angle(0),pTemplate(NULL),diff(0.0){}
}Result;
typedef vector<Result> ResultSet;                             //最终识别的结果存放的vector
//定义用于识别的特征
typedef enum FeatureSelection{
	FS_SIMPLE		=0,
	FS_HUMEMENTS	=1,
	FS_HOG			=2,
	FS_SURF			=3,
	FS_PARTSSEGMNT	=4
}FeatureSel;

//定义数据库内零件的基本信息和特征          //写到数据库里面的
typedef struct tagPartsInformation{
//零件基本信息
	int             id;
	string          name;

	double          angle;   //零件朝向0°或180°，需要录入数据库时指明是哪个朝向是正向，为0°，相反方向为180°

//零件特征
	double			area;//零件面积
	double			rectangularity;//矩形度：区域面积与最小外接矩形的面积之比
	double			eolngatedness;//细长度：外接矩形的长宽比
	double          compactness;
	CvHuMoments		huMmts;//矩特征

	vector<float>     hogDescriptors;

	IplImage        *pTemplate;//旋转后的零件图像模板 

	vector<KeyPoint> keyPoints;
	Mat              kpDescriptors;
	
	CvBox2D          minBBox;//最小外接矩形（坐标转化为世界坐标系，原点在零件中心）
	TriVec contourTriangleLst;//轮廓三角特征

	tagPartsInformation():id(-1),area(0),rectangularity(0),
		                       eolngatedness(0),compactness(0),pTemplate(NULL){}
}PartsInfo;
typedef std::vector<PartsInfo>		PartsInfoList;
typedef PartsInfoList::iterator		Iterator;

const int imgTmpltW=151;
const int imgTmpltH=151;

 
class CPartsDatabase
{
private:
	CPartsDatabase(void);
	~CPartsDatabase(void);
public:
	static CPartsDatabase* _instance;
	static CPartsDatabase* Instance();

	string   		m_curDir;
	string          m_dbName;
private:
	int m_uuid;
	PartsInfoList	m_pfList;//存放零件特征的动态数组
	int				m_bfID;//指向当前零件的ID
	int             m_cxID;
	

	boost::mutex	m_mutex;

	CShapeAnalysis		m_spAnalysis;
	CKeyPointTemplate	m_kpTemplate;


	double          m_s[4];//存放各个特征分量的标准差
	double          m_mean[4];

	double          m_min[4];
	double          m_max[4];

	double          m_areaDiff;
	double          m_rtDiff;
	double          m_eolDiff;
	double          m_cmptDiff;

	int             m_bArea;


	double          m_areaRate;

private:
	CEdgeDetection	m_ed;
	CBgSub			m_bgSub;
	CBlobResult     m_br;

	std::vector<cv::Mat> m_trainDescriptors;
public:
	void InitDatabase();



	void CptHuMoments(IplImage* pMask,CvHuMoments& huMoments);
	BOOL CptShapeInfo(IplImage* pMask,PartsInfo& pf);
	BOOL CptPartsInfo(IplImage* pImg,IplImage* pMask,PartsInfo& pf);

	void CptKeyPointsDescriptors(IplImage* pImg,CBlob* pBlob,PartsInfo& pf);
	
	double MatchShapesByHOG(vector<float>& hogDescriptor1,vector<float>& hogDescriptor2,double minDiff=DBL_MAX);
	double MatchTemplate(IplImage* pMaskQuery,IplImage* pMaskTrain,BOOL bDebug=FALSE);
	IplImage* CptTemplate(IplImage* pMask);

	int  ShapeRetrivalByMoments(IplImage* pMask,PartsInfo& pf);
	int  ShapeRetrivalByFeature(IplImage* pMask,PartsInfo& pf,BOOL bDebug=FALSE);
	int  ShapeRetrivalByHOGFeature(IplImage* pImg,IplImage* pMask,PartsInfo& pf,BOOL bDebug=FALSE);

public:
	void SetCurrentID(int id){}
	int  NewParts();
	void Insert(PartsInfo& spInfo);
	void Insert(IplImage* pImg,IplImage* pMask,PartsInfo& ptInfo);
	void Delete(int id);
	Iterator Find(int id);
	double MatchShapesByMoments(CvHuMoments& huMoments1,CvHuMoments& huMoments2,int method=1);
	double MatchShapesByShapeFeature(PartsInfo& pf1,PartsInfo& pf2);
	void Save();
	void Backup();
	void Load();
	
	void Rotate(IplImage* pSrc,IplImage* pDst,double angle);

	void GetIplImage(PartsInfo& pf,IplImage* pMaskShow);


	void ClearDataBase();
	void InputConfig();
	std::string GetCurrentPath(){return m_curDir;}
public:
//同时获取多个零件的特征
	void PartsRetrieval(IplImage* pImg,CBlobResult& br,ResultSet& rs,FeatureSel fsel=FS_SURF,IplImage* pMask=NULL);
	BOOL SinglePartDetection(PartsInfo& ptInfoIn,Result& Out,FeatureSel fsel=FS_SURF);


	IplImage* Normalize(IplImage* pImage,CBlob& blob,int w=128,int h=128);
	BOOL CptSimpleFeature(CBlob& blob,PartsInfo& pf);
	void CptHogDescriptors(IplImage* pImgDst,PartsInfo& pf);
public:
	void SetArea(int bArea){
		m_bArea=bArea;
	}
	void SetShapeMatchText(double areaDiff,double rtDiff,double eolDiff,double cmptDiff);
		
	void CalcStdDeviation();

	void readKeyPoints(const FileNode& node, std::vector<KeyPoint>& keypoints);

	void GetSymmImage(IplImage* pQTmpt);
	void Write(FileStorage& fs, const string& objname,vector<float>& hogDescriptor);
	void Read(const FileNode& node, vector<float>& hogDescriptor);

	void Insert(PartsInfoList& out);

	void InsertHogDescriptor(IplImage* pImg,CBlob* pblob,PartsInfoList& out);
	void InsertKpDescriptor(IplImage* pImg,CBlob* pblob,PartsInfoList& out);

	PartsInfoList& GetPartsInfoList(){return m_pfList;}
public:
//用于解决多零件挨在一起情况下的分割
	void MatchTriangleformDB(int qryIdx,Triangle& tri,TMatchVec& out); 

	void    PartsRecognition(CBlob* pBlob,IplImage* pMask);
	BOOL    HaveNext(TriVec& tv);
	void    RemoveMatchPart(CvBox2D bbox,TriVec& triLst);
	double  KeyPointsMatch(Triangle& tri,Pt32fVec& dpLst,TMatchVec& matchVec,int& best,IplImage* pMask=NULL);	
	CvBox2D PartsSegmentation(TriVec& triLst,Pt32fVec& dpLst,TMatch& tmBest,IplImage* pMask=NULL);	
};

#define PartsDB     (CPartsDatabase::Instance())
