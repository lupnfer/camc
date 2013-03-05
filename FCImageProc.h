//////////////////////////////////////////////////////////////////
//																//
//		用途 : 图像处理基类（用于复杂的图像处理基类）							//
//		创建 : [刘建明] / 2004-12-2							//
//		更新 : 2004-12-2											//						//							//
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
	//图像处理算法执行前需要设置的参数对象。
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
