#pragma once
#include "cxcore.h"
#include "EdgeDetection.h"
#include <vector>
#include <string>
typedef struct tag_triangle{
	CvPoint2D32f left;
	CvPoint2D32f pos;
	CvPoint2D32f right;
	double a;
	double b;
	double c;
	
	double diff;//数据库中与待匹配三角特征的不相似度
	tag_triangle():a(0),b(0),c(0),diff(-1){}
}Triangle;

typedef std::vector<Triangle> TriVec;
typedef std::vector<CvPoint2D32f> Pt32fVec;
typedef struct tag_TMatch{
	Triangle tri;
	int      qryIdx;
	int      triIdx;
	int      imgIdx;
}TMatch;
typedef std::vector<TMatch> TMatchVec;
class CShapeAnalysis
{
public:
	CShapeAnalysis(void);
	~CShapeAnalysis(void);
private:
	CvSeq*  m_pContour;
	CvMemStorage* g_storage;
	double  m_contourLength;//记录轮廓的长度
public:
	int    m_ctLineWidth;
public:
	CvSeq*  FindContour(IplImage *mask,BOOL approxPoly=TRUE);
	int     CheckContourHoles();
	double  CalcArea(IplImage* pMask);

	CvBox2D PartsAngleDetection(CvSeq* pContour);
	CvBox2D PartsAngleDetection(IplImage* pMask);//输入8位的二值图像获取最小外接矩形
	CvRect  PartsBoundingRect(IplImage* pMask);
	

	void    DrawMinBoundingRect(CvBox2D box2d,IplImage* pImgShow,BOOL flag=TRUE);
	
	double  GetLength(){return m_contourLength;}

	void    CalcPGH(CvHistogram* hist);

	double 	CompareSURFDescriptors( const float* d1, const float* d2, double best, int length );
	int
		naiveNearestNeighbor( const float* vec, int laplacian,
		const CvSeq* model_keypoints,
		const CvSeq* model_descriptors );
	void
		flannFindPairs( const CvSeq*, const CvSeq* objectDescriptors,
		const CvSeq*, const CvSeq* imageDescriptors, std::vector<int>& ptpairs );

	void DrawContour(IplImage* pMask);

	IplImage* Rotate(IplImage* pSrcMask);

	IplImage* OffsetObject(IplImage* pMask,CvPoint2D32f objCenter);

	void Rotate(IplImage* pImg,IplImage* pSrcMask,IplImage* pImgDst,IplImage* pMskDst);

	void Rotate(IplImage* pImg,IplImage* pSrcMask,CvBox2D box2d,IplImage* pImgDst,double sz=120);
	

	void OffsetObject(IplImage* pImgSrc,IplImage* pMaskSrc,CvBox2D objBox,IplImage* pImgDst,IplImage* pMskDst);
	int bwlabel(IplImage* img, int n, int* labels);
	int find( int set[], int x );  

public:
	void CptInCicles(IplImage* pMask,IplImage* &out);
	int  CptInCicleRadius(IplImage* pMask,int x,int y,int max_radius,double eps=0.05);
	void GenLookupTable(int radius=200);
	void SaveLUT();
	void LoadLUT();

	void CptContourTriangle(std::vector<CvPoint2D32f>& dpList,std::vector<Triangle>& triLstOut);
	double  MatchTriangle(Triangle& src,Triangle& dst);
	void    MatchTriangle(Triangle& tri,std::vector<Triangle>& triLst,std::vector<double>& distList);
	double  MatchTriangle(std::vector<Triangle>& queryTriLst,std::vector<Triangle>& dbTriLst,Triangle& out);

	CvBox2D CptMinBoundingRect(Triangle& src,Triangle& dst,CvBox2D& box,IplImage* pMask=NULL);
	//void    CptMinBoundingRect(Triangle& src,Triangle& dst,CvBox2D& box,CvPoint* ptArray,IplImage* pMask=NULL)
	CvMat*  GetAffineTransform(Triangle& src,Triangle& dst);
	double  MatchDominantPoints(TriVec& src,Pt32fVec& dpLst,CvMat* warp_mat,double threshold=2.0);
	double  BestMatch(CvPoint2D32f pt,Pt32fVec& dpLst);
	Triangle GetMaxLengthTriangle(TriVec& triLst);
	void  DrawTrianglePoints(Triangle tri,CvScalar color,IplImage* pMask,int style=0);
	void  DrawTrianglePoints(Triangle triQry,TMatch& tm,TriVec& trainTriLst,CvScalar color,IplImage* pMask);
	void  Transform(CvPoint2D32f ptSrc,CvPoint2D32f& ptDst,CvMat* warp_mat);
	void  Transform(Triangle triSrc,Triangle& triDst,CvMat* warp_mat);
public:
	CvMat*  ToMatrix(TriVec& triLst);
	void    FromMatrix(CvMat* data,TriVec& triLst);

public:
	double GetRealAngle(CvBox2D box2d);
	CvBox2D GetRealMinBox(CvBox2D box2d);
	CvBox2D GetBox(CvPoint2D32f* pts);
private:
	std::vector<CvPoint> m_lookupTable;
	std::vector<int>     m_idxVec;
};
