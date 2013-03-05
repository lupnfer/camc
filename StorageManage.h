#pragma once
#include <cxcore.h>
#include <vector>
//����ʵ����������������ͼ����ڴ�����

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
	IMGLIST m_storageCreate;//����������
	IMGLIST m_storageRelease;//���ͷŵ���
	boost::mutex  m_imgMutex;
	boost::mutex  m_matMutex;
	//int     m_count;//���ڼ������������Ķ������ĳ����ʱ�����Զ�ȫ���ͷ�ͼ���ڴ�

	MATLIST m_matListCreate;
	MATLIST m_matListRelease;
public:
	IplImage* CreateIplImage(int w,int h,int depth,int nChannels);
	CvMat*    CreateMat(int r,int c,int type);
	void ReleaseMat(CvMat* mat);
	void ReleaseIplImage(IplImage* pImg);
	void ClearAll();
};

