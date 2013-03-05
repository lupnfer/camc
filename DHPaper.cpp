#include "StdAfx.h"
#include ".\dhpaper.h"
#include "FColor.h"
#include "GraphicsGems.h"
extern double V2Length(Vector2* v);



CDHPaper* CDHPaper::_instance=NULL;

CDHPaper* CDHPaper::Instance()
{
	if(_instance==NULL){
		_instance=new CDHPaper();
	}
	_instance->InitBrush(new CDHBrush());
	return _instance;
}
void CDHPaper::DrawSection(CDC *pDC,TPosition& p,COLORREF c)
{
	int r=(p.press+0.5)/2;
	
	CDHBrush* pBrush=static_cast<CDHBrush*>(m_pBrush);
	pBrush->GenSection(r);
	LPSECTION lpSection=pBrush->GetSection();
	GenTexture(lpSection,r,c);
	for(int y=0;y<=r;++y){
		for(int x=0;x<=r;++x){
			int index=y*r+y+x;
			if(lpSection[index].m_bUse){
				double diff;
					lpSection[index].m_c=c;
					pDC->SetPixel(p.m_x+x,p.m_y+y,lpSection[index].m_c);
					pDC->SetPixel(p.m_x-x,p.m_y+y,lpSection[index].m_c);
					pDC->SetPixel(p.m_x+x,p.m_y-y,lpSection[index].m_c);
					pDC->SetPixel(p.m_x-x,p.m_y-y,lpSection[index].m_c);
			}
		}
	}
}
void CDHPaper::DrawSection(FCObjImage& img,TPosition& p,BYTE alpha)
{
	int r=(p.press+0.5)/2;
	RECT rc,rcImg;
	rc.left=p.m_x-r;
	rc.top=p.m_y-r;
	rc.right=p.m_x+r;
	rc.bottom=p.m_y+r;
	rcImg.left=rcImg.top=0;
	rcImg.right=img.Width();
	rcImg.bottom=img.Height();

	if(::IntersectRect(&rc,&rc,&rcImg))
	{
		CDHBrush* pBrush=static_cast<CDHBrush*>(m_pBrush);
		pBrush->GenSection(r);
		LPSECTION lpSection=pBrush->GetSection();
		COLORREF c=0;
		GenTexture(lpSection,r,c);
		for(int y=rc.top;y<rc.bottom;++y){
			BYTE* pBytes=img.GetBits(rc.left,y);
			for(int x=rc.left;x<rc.right;++x,pBytes++)
			{
				int xTmp=abs(x-p.m_x);
				int yTmp=abs(y-p.m_y);
				int index=yTmp*r+yTmp+xTmp;
				if(lpSection[index].m_bUse){
					*pBytes=alpha;
				}
			}
		}
	}
	

/*
	for(int y=0;y<=r;++y){
		for(int x=0;x<=r;++x){
			int index=y*r+y+x;
			if(lpSection[index].m_bUse){
				img.SetGrayPixel(p.m_x+x,p.m_y+y,alpha);
				img.SetGrayPixel(p.m_x-x,p.m_y+y,alpha);
				img.SetGrayPixel(p.m_x+x,p.m_y-y,alpha);
				img.SetGrayPixel(p.m_x-x,p.m_y-y,alpha);
			}
		}
	}
	*/
}



void CDHPaper::FillPolygon(LPPOINT lpPoints,int nCount,FCObjImage& img, COLORREF nColor)
{
	// �������Ϸ���
	//ASSERT_VALID(pDC);
	ASSERT(lpPoints);
	ASSERT(nCount>2);

	int i=0,j=0,k=0;
	int y0=0,y1=0;  // ɨ���ߵ�������Сy����
	LPEdge pAET=NULL; // ��߱�ͷָ��
	LPEdge * pET=NULL;  // �߱�ͷָ��

	pAET=new Edge; // ��ʼ����ͷָ�룬��һ��Ԫ�ز���
	pAET->pNext=NULL;

	// ��ȡy����ɨ���߽߱�
	y0=y1=lpPoints[0].y;
	for(i=1;i<nCount;i++)
	{
		if(lpPoints[i].y<y0)
			y0=lpPoints[i].y;
		else if(lpPoints[i].y>y1)
			y1=lpPoints[i].y;
	}
	if(y0>=y1) return;

	// ��ʼ���߱���һ��Ԫ�ز���
	pET=new LPEdge[y1-y0+1];
	for(i=0;i<=y1-y0;i++)
	{
		pET[i]= new Edge;
		pET[i]->pNext=NULL;
	}

	for(i=0;i<nCount;i++)
	{
		j=(i+1)%nCount; // ��ɱߵ���һ��
		if(lpPoints[i].y != lpPoints[j].y)// ����ñ߲���ˮƽ�������߱�
		{
			LPEdge peg;   // ָ��ñߵ�ָ��
			LPEdge ppeg;  // ָ���ָ���ָ��

			// �����
			peg =new Edge;
			k=(lpPoints[i].y>lpPoints[j].y)?i:j;
			peg->ymax=lpPoints[k].y; // �ñ����y����
			k=(k==j)?i:j;  
			peg->x=(float)lpPoints[k].x; // �ñ���ɨ���߽���x����
			if(lpPoints[i].y != lpPoints[j].y) 
				peg->dx=(float)(lpPoints[i].x-lpPoints[j].x)/(lpPoints[i].y-lpPoints[j].y);// �ñ�б�ʵĵ���
			peg->pNext=NULL;

			// �����
			ppeg=pET[lpPoints[k].y-y0]; 
			while(ppeg->pNext)
				ppeg=ppeg->pNext;
			ppeg->pNext=peg;
		}// end if
	}// end for i

	// ɨ��
	for(i=y0;i<=y1;i++)
	{
		LPEdge peg0=pET[i-y0]->pNext;
		LPEdge peg1=pET[i-y0];
		if(peg0)// ���±߼���
		{
			while(peg1->pNext)
				peg1=peg1->pNext;
			peg1->pNext=pAET->pNext;
			pAET->pNext=peg0;
		}

		// ����x��������pAET
		peg0=pAET;
		while(peg0->pNext)
		{
			LPEdge pegmax=peg0;
			LPEdge peg1=peg0;
			LPEdge pegi=NULL;

			while(peg1->pNext)
			{
				if(peg1->pNext->x>pegmax->pNext->x)
					pegmax=peg1;
				peg1=peg1->pNext;
			}
			pegi=pegmax->pNext;
			pegmax->pNext=pegi->pNext;
			pegi->pNext=pAET->pNext;
			pAET->pNext=pegi;
			if(peg0 == pAET)
				peg0=pegi;
		}

		// ������߱�����
		peg0=pAET;
		while(peg0->pNext)
		{
			if(peg0->pNext->pNext)
			{
				//DrawLine((int)peg0->pNext->x,i,(int)peg0->pNext->pNext->x,i,pDC,nColor);

				peg0=peg0->pNext->pNext;
			}
			else
				break;
		}

		// ��ymax=i�Ľڵ�ӻ�߱�ɾ������ÿ���ڵ��xֵ����dx
		peg0=pAET;
		while(peg0->pNext)
		{
			if(peg0->pNext->ymax < i+2)
			{
				peg1=peg0->pNext;
				peg0->pNext=peg0->pNext->pNext; //ɾ��
				delete peg1;
				continue;
			}
			peg0->pNext->x+=peg0->pNext->dx; //��ÿ���ڵ��xֵ����dx
			peg0=peg0->pNext;
		}
	}

	// ɾ���߱�
	for(i=0;i<y1-y0;i++)
		if(pET[i])
			delete pET[i];
	if(pAET) 
		delete pAET;
	if(pET)
		delete[] pET; 
}