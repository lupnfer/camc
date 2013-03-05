#pragma once
#include <cxcore.h>
#include <vector>
//用于实现连续创建和销毁图像的内存重用

typedef std::vector<IplImage*>  IMGLIST;
typedef std::vector<CvMat*>     MATLIST;
const int MAX_NUM=150;
class CStorageManage
{
private:
	CStorageManage(void);
	~CStorageManage(void);
private:
	static CStorageManage* _instance;
public:
	static CStorageManage* Instance();
private:
	IMGLIST m_storageCreate;//被创建的类
	IMGLIST m_storageRelease;//被释放的类
	boost::mutex  m_imgMutex;
	boost::mutex  m_matMutex;
	//int     m_count;//用于计数，当创建的对象大于某个数时，就自动全体释放图像内存

	MATLIST m_matListCreate;
	MATLIST m_matListRelease;
public:
	IplImage* CreateIplImage(int w,int h,int depth,int nChannels);
	CvMat*    CreateMat(int r,int c,int type);
	void ReleaseMat(CvMat* mat);
	void ReleaseIplImage(IplImage* pImg);
	void ClearAll();
};

