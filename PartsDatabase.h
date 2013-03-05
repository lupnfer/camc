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
*������ݿ⣬�������ͼ�������ı��棬��ȡ
*,��������PartsRetrival����ʵ�֣�
* ���ݿ�����ļ�ϵͳ��ʵ�֣�����������һ���ļ�����ͼƬ������ͼ�������ļ����У�
//һ��mxl�ļ��Ͱ��������еģ�
*********************/
using namespace cv;

typedef struct tagResult{
	int             id;
	string          name;
	double          angle;
	CvPoint2D32f    position;           //��������
	double          diff;
	IplImage        *pTemplate;//���ͼ��ģ��    
	tagResult():id(-1),angle(0),pTemplate(NULL),diff(0.0){}
}Result;
typedef vector<Result> ResultSet;                             //����ʶ��Ľ����ŵ�vector
//��������ʶ�������
typedef enum FeatureSelection{
	FS_SIMPLE		=0,
	FS_HUMEMENTS	=1,
	FS_HOG			=2,
	FS_SURF			=3,
	FS_PARTSSEGMNT	=4
}FeatureSel;

//�������ݿ�������Ļ�����Ϣ������          //д�����ݿ������
typedef struct tagPartsInformation{
//���������Ϣ
	int             id;
	string          name;

	double          angle;   //�������0���180�㣬��Ҫ¼�����ݿ�ʱָ�����ĸ�����������Ϊ0�㣬�෴����Ϊ180��

//�������
	double			area;//������
	double			rectangularity;//���ζȣ������������С��Ӿ��ε����֮��
	double			eolngatedness;//ϸ���ȣ���Ӿ��εĳ����
	double          compactness;
	CvHuMoments		huMmts;//������

	vector<float>     hogDescriptors;

	IplImage        *pTemplate;//��ת������ͼ��ģ�� 

	vector<KeyPoint> keyPoints;
	Mat              kpDescriptors;
	
	CvBox2D          minBBox;//��С��Ӿ��Σ�����ת��Ϊ��������ϵ��ԭ����������ģ�
	TriVec contourTriangleLst;//������������

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
	PartsInfoList	m_pfList;//�����������Ķ�̬����
	int				m_bfID;//ָ��ǰ�����ID
	int             m_cxID;
	

	boost::mutex	m_mutex;

	CShapeAnalysis		m_spAnalysis;
	CKeyPointTemplate	m_kpTemplate;


	double          m_s[4];//��Ÿ������������ı�׼��
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
//ͬʱ��ȡ������������
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
//���ڽ�����������һ������µķָ�
	void MatchTriangleformDB(int qryIdx,Triangle& tri,TMatchVec& out); 

	void    PartsRecognition(CBlob* pBlob,IplImage* pMask);
	BOOL    HaveNext(TriVec& tv);
	void    RemoveMatchPart(CvBox2D bbox,TriVec& triLst);
	double  KeyPointsMatch(Triangle& tri,Pt32fVec& dpLst,TMatchVec& matchVec,int& best,IplImage* pMask=NULL);	
	CvBox2D PartsSegmentation(TriVec& triLst,Pt32fVec& dpLst,TMatch& tmBest,IplImage* pMask=NULL);	
};

#define PartsDB     (CPartsDatabase::Instance())
