//////////////////////////////////////////////////////////////////
//																//
//		��; : ͼ������ࣨ���ڸ��ӵ�ͼ������ࣩ							//
//		���� : [������] / 2004-12-2							//
//		���� : 2004-12-2											//						//							//
//////////////////////////////////////////////////////////////////
#pragma once
#include "objbase.h"
#include "objImage.h"
class FCObjImage;
class FCObjProgress;
class FCImageProc :public FCObject
{
public:
	FCImageProc(void);
	~FCImageProc(void);
public:
	//ͼ�����㷨ִ��ǰ��Ҫ���õĲ�������
	class Paramer:public FCObject
	{
		public:
			Paramer(){}
	};
	virtual void SetParamer(Paramer& paramer){};
	virtual void Apply(FCObjImage& imgSrc,FCObjImage& imgDst,FCObjProgress * Percent = NULL)=0;
	void Apply(FCObjImage& imgSrcDst,FCObjProgress * Percent = NULL)
	{
		FCObjImage img(imgSrcDst.Width(),imgSrcDst.Height(),8);
		Apply(imgSrcDst,img,Percent);
		imgSrcDst=img;
	}
};
