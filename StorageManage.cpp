#include "StdAfx.h"
#include "StorageManage.h"

typedef boost::mutex::scoped_lock scoped_lock;

CStorageManage* CStorageManage::_instance=NULL;
CStorageManage::CStorageManage(void)
{

}

CStorageManage::~CStorageManage(void)
{
	ClearAll();
}
CStorageManage* CStorageManage::Instance()
{
	if(_instance==NULL)
		_instance=new CStorageManage();
	return _instance;
}
IplImage* CStorageManage::CreateIplImage(int w,int h,int depth,int nChannels)
{
	scoped_lock lk(m_imgMutex);
	int count=m_storageCreate.size();
	if(count>=MAX_NUM)
		ClearAll();
	IMGLIST::iterator it=m_storageRelease.begin();
	for(;it!=m_storageRelease.end();++it){
		if((*it)->width==w&&(*it)->height==h&&(*it)->depth==depth&&(*it)->nChannels==nChannels){
			IplImage* pImg=(*it);
			m_storageRelease.erase(it);
			//m_storageCreate.push_back(pImg);
			return pImg;
		}
	}
	CvSize sz={w,h};
	IplImage* pImg=cvCreateImage(sz,depth,nChannels);
	pImg->origin=1;
	m_storageCreate.push_back(pImg);
	return pImg;
}
void CStorageManage::ReleaseIplImage(IplImage* pImg)
{
	if(pImg==NULL)
		return;
	scoped_lock lk(m_imgMutex);
	IMGLIST::iterator it;
	for(it=m_storageRelease.begin();it!=m_storageRelease.end();++it){
		if((*it)==pImg){
			return;
		}
	}
	cvZero(pImg);
	pImg->roi=NULL;
	m_storageRelease.push_back(pImg);
}
void CStorageManage::ClearAll()
{

	AfxMessageBox("´æÔÚÄÚ´æÐ¹Â¶£¡");
	{
		scoped_lock lk(m_imgMutex);
		m_storageRelease.clear();
		IMGLIST::iterator it=m_storageCreate.begin();
		for(;it!=m_storageCreate.end();++it){
			cvReleaseImage(&(*it));
		}
		m_storageCreate.clear();
	}
	{
		scoped_lock lk(m_matMutex);
		m_matListRelease.clear();
		MATLIST::iterator it=m_matListCreate.begin();
		for(;it!=m_matListCreate.end();++it){
			cvReleaseMat(&(*it));
		}
		m_matListCreate.clear();
	}
	
}
CvMat* CStorageManage::CreateMat(int r,int c,int type)
{
	scoped_lock lk(m_matMutex);
	int count=m_matListCreate.size();
	if(count>=MAX_NUM)
		ClearAll();
	MATLIST::iterator it=m_matListRelease.begin();
	for(;it!=m_matListRelease.end();++it){
		int tpTmp=CV_MAT_TYPE((*it)->type);
		if((*it)->rows==r&&(*it)->cols==c&&tpTmp==type){
			CvMat* pMat=(*it);
			m_matListRelease.erase(it);
			return pMat;
		}
	}
	CvMat* pMat=cvCreateMat(r,c,type);
	m_matListCreate.push_back(pMat);
	return pMat;
}
void CStorageManage::ReleaseMat(CvMat* mat)
{
	if(mat==NULL)
		return;
	scoped_lock lk(m_matMutex);
	MATLIST::iterator it;
	for(it=m_matListRelease.begin();it!=m_matListRelease.end();++it){
		if((*it)==mat){
			return;
		}
	}
	cvZero(mat);
	m_matListRelease.push_back(mat);
}