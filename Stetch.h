#pragma once
#include "objbase.h"
#include<list>
#include "ObjProgress.h"
#include "ObjImage.h"
/*
 * ���ܣ���ʾ�û����Ƶ����軭��ϵͳ��������Զ����һϵ����ϵͳ
 *       ���⻭ˢ���Ƶ�����ͼ��ʸ����ʽ���������޷Ŵ�Ҳ����ת��
 *		 Ϊ��դ��ʽͼ����BMP��
 * ����ʱ�䣺2005.4.15
 * ���ߣ�    ������
 */
class CBCStroke;
class FCObjImage;
class CParentPaper;
typedef std::list<CBCStroke*> SKETCH;

class CSketch :
	public FCObjGraph
{
public:
	CSketch(FCObjImage& img);
	~CSketch(void);
	void Init(CParentPaper* pPaper){
		m_pPaper=pPaper;
		m_pStroke=NULL;
	}
public:
	void ToImage(FCObjImage& img,float zoom_img=1,float zoom_strokewidth=1,FCObjProgress * Percent = NULL);
	BOOL Load(CString fileName);
	void Save(CString fileName);
	void Add(CBCStroke* lpStroke);
	void Insert(CBCStroke* lpStroke);
	void Remove(CBCStroke* lpStroke);
	int  Width(){return m_width;}
	int  Height(){return m_height;}
	void SetWidth(int w){m_width=w;}
	void SetHeight(int h){m_height=h;}
	CBCStroke* GetStrokeInRgn(POINT point){return NULL;}
	SKETCH& GetStrokeList(){
		return m_sketch;
	}
	CBCStroke* GetCurrentStroke(){return m_pStroke;}
	void ReDraw(FCObjImage& img,RECT rc,FCObjProgress * Percent = NULL);
	void Zoom(float paperRatio,float widthRatio);
public:
	int				m_width;
	int				m_height;
	SKETCH			m_sketch;
	CParentPaper*	m_pPaper;
	CBCStroke*		m_pStroke;
	long			m_idIndex;
	SKETCH          m_removedStrokeList;
protected:
	FCObjImage		m_imgBack;//����ͼ��
};
