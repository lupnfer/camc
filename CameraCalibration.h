#pragma once

#include <vector>
typedef struct tagWCSPoint{
	CvPoint2D32f point;
	float        diff;
	bool operator >(const tagWCSPoint &that) const { return this->diff > that.diff; }
}WCSPoint;
class CCameraCalibration
{
private:
	CCameraCalibration(void);
	~CCameraCalibration(void);
public:
	static CCameraCalibration* _instance;
	static CCameraCalibration* Instance();
public:
	BOOL FindChessBoardCorners(IplImage* image,CvSize pattern_size,int flag,int nboard);
	void Findhomography();
	void SetOriginPoint(POINT p);
	void CptWCSCorners();
	void SetUnitWH(float w,float h){m_unitH=h;m_unitW=w;}
	void Save();
	void Load();

	void  CptWCSPoint(CvPoint2D32f ptICS,CvPoint2D32f& ptWCS);
	void  CptICSPoint(CvPoint2D32f ptWCS,CvPoint2D32f& ptICS);

	double  UnitTest(std::vector<WCSPoint>& errorArray);
	BOOL  IsValid();
	CvPoint2D32f GetOrigin(){return m_origin;}
	void DrawWordCoordiateSystem(IplImage* img);

	void ICSpts2WCSpts(std::vector<CvPoint>& icsPts,std::vector<CvPoint2D32f>& wcsPts);
	double ICSArea2WCSArea(double icsArea);
	double GetAreaRate(){return m_areaRate;}
private:
	CvMat*        m_homography;//单应矩阵，图像坐标系向世界坐标系的转换矩阵（Z=0)
	CvMat*        m_invhomography;//反向单应矩阵，世界坐标系向图像坐标系的转换矩阵（Z=0)


	CvPoint2D32f* m_corners;

	CvMat* m_cornersICS;//图像坐标系下的角点坐标
	CvMat* m_cornersWCS;//世界坐标系下的角点坐标
	int           m_cornerCount;
	CvSize        m_boardSize;

	int           m_originIdx;//指定的世界坐标系原点，由用户指定某个角点
	CvPoint2D32f  m_origin;

	float         m_unitW;
	float         m_unitH;
	float         m_width;//用于定标的图像的宽度（像素）
	float         m_height;//用于定标的图像的高度（像素）
	boost::mutex  m_mutex;
private:
	double        m_areaRate;//棋盘格实际面积与其图像坐标系面积与之比，即单个像素的实际面积
	void    CptAreaRate();
};

#define  CamCalib (CCameraCalibration::Instance())