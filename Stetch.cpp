#include "StdAfx.h"
#include ".\stetch.h"
#include "objImage.h"
#include "BCStroke.h"
#include "MainFrm.h"


CSketch::CSketch(FCObjImage& img)
{
	m_pStroke=NULL;
	m_idIndex=0;
	m_imgBack=img;
}

CSketch::~CSketch(void)
{
	SKETCH::iterator it;
	for(it=m_sketch.begin();it!=m_sketch.end();++it){
		delete *it;
	}
	for(it=m_removedStrokeList.begin();it!=m_removedStrokeList.end();++it)
		delete *it;
	m_sketch.clear();
	m_removedStrokeList.clear();
}
/*
 *zoom_img:图像的放大倍数
 *zoom_strokewidth：线宽的放大倍数
 *img:背景图像
 */
void CSketch::ToImage(FCObjImage& img,float zoom_img,float zoom_strokewidth,FCObjProgress * progress)
{	
	//img.Stretch(m_width*zoom_img,m_height*zoom_img);
	SKETCH::iterator it;
			// 遍历处理区域像素
    if (progress != NULL)
        progress->ResetProgress() ; // reset to 0
	int size=m_sketch.size();
	int i=0;
	m_width=img.Width();
	m_height=img.Height();	
	for(it=m_sketch.begin();it!=m_sketch.end();++it,++i){
		(*it)->Zoom(zoom_img,zoom_strokewidth);
		(*it)->Draw(img);		
		if (progress != NULL)
			progress->SetProgress ( i* 100 / size) ;
	}
}
void CSketch::Zoom(float paperRatio,float widthRatio)
{
	m_width*=paperRatio;
	m_height*=paperRatio;
	SKETCH::iterator it;
	for(it=m_sketch.begin();it!=m_sketch.end();++it){
		(*it)->Zoom(paperRatio,widthRatio);	
	}
}
BOOL CSketch::Load(CString fileName)
{
	
	
	return true;
}
void CSketch::Save(CString fileName)
{
	
}



void CSketch::Add(CBCStroke* lpStroke)
{
	if(lpStroke==NULL)
		return;
	m_idIndex++;
	lpStroke->SetId(m_idIndex);
	m_sketch.push_back(lpStroke);
	if(m_pStroke!=NULL){
		lpStroke->m_lpStrokePre=m_pStroke;
		m_pStroke->m_lpStrokeNext=lpStroke;
	}
	m_pStroke=lpStroke;
	m_removedStrokeList.remove(lpStroke);
}
void CSketch::Insert(CBCStroke* lpStroke)
{
	
}
void CSketch::Remove(CBCStroke* lpStroke)
{
	if(lpStroke==NULL)
		return;
	if(lpStroke->m_lpStrokePre!=NULL)
		lpStroke->m_lpStrokePre->m_lpStrokeNext=lpStroke->m_lpStrokeNext;
	if(lpStroke->m_lpStrokeNext!=NULL)
		lpStroke->m_lpStrokeNext->m_lpStrokePre=lpStroke->m_lpStrokePre;
	m_sketch.remove(lpStroke);
	if(m_pStroke==lpStroke)
		m_pStroke=lpStroke->m_lpStrokePre;
	m_removedStrokeList.push_back(lpStroke);

}
void CSketch::ReDraw(FCObjImage& img,RECT rc,FCObjProgress * Percent)
{
	RECT rcSave;
	FCObjImage imgBlock,imgCover=m_imgBack;
	SKETCH::iterator it;
	for(it=m_sketch.begin();it!=m_sketch.end();++it){
		RECT rcRet=(*it)->GetRect();
		if(::IntersectRect(&rcSave,&rc,&rcRet)){
			(*it)->Draw(imgCover);				
		}
	}
	imgCover.GetSubBlock(&imgBlock,rc);
	img.CoverBlock(imgBlock,rc.left,rc.top);
}