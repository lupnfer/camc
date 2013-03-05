#pragma once
#include "cmdimgeffect.h"
#include "ObjImage.h"
class CCmdImgRemoveNoise :
	public CCmdImgEffect
{
public:
	CCmdImgRemoveNoise(int r1,int r2):radio1(r1),radio2(r2){}
	~CCmdImgRemoveNoise(void);
	virtual void Implement(FCObjImage & img, FCObjProgress * Percent /* = NULL */);
	BOOL   Is_Noise(FCObjImage & img,int x,int y,int radio1,int radio2);
	void   Remove_Noise(FCObjImage & img,int x,int y,int radio);
private:
	int radio1;//圆环的内径
	int radio2;//圆环的外径

};
