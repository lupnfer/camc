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
	CvMat*        m_homography;//��Ӧ����ͼ������ϵ����������ϵ��ת������Z=0)
	CvMat*        m_invhomography;//����Ӧ������������ϵ��ͼ������ϵ��ת������Z=0)


	CvPoint2D32f* m_corners;

	CvMat* m_cornersICS;//ͼ������ϵ�µĽǵ�����
	CvMat* m_cornersWCS;//��������ϵ�µĽǵ�����
	int           m_cornerCount;
	CvSize        m_boardSize;

	int           m_originIdx;//ָ������������ϵԭ�㣬���û�ָ��ĳ���ǵ�
	CvPoint2D32f  m_origin;

	float         m_unitW;
	float         m_unitH;
	float         m_width;//���ڶ����ͼ��Ŀ�ȣ����أ�
	float         m_height;//���ڶ����ͼ��ĸ߶ȣ����أ�
	boost::mutex  m_mutex;
private:
	double        m_areaRate;//���̸�ʵ���������ͼ������ϵ�����֮�ȣ����������ص�ʵ�����
	void    CptAreaRate();
};

#define  CamCalib (CCameraCalibration::Instance())