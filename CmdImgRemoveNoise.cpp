#include "StdAfx.h"
#include ".\cmdimgremovenoise.h"


CCmdImgRemoveNoise::~CCmdImgRemoveNoise(void)
{
}
void CCmdImgRemoveNoise::Implement(FCObjImage & img, FCObjProgress * Percent /* = NULL */)
{
	for(int y = 0; y<img.Height();y=y+1)
		for(int x= 0; x<img.Width();x=x+1)
		{
			BYTE* bpPixel = img.GetBits(x,y);

			if((bpPixel[0]!=255)||(bpPixel[1]!=255)||(bpPixel[2]!=255))//除去白色之外的点作为要处理的目标
			{
				if (Is_Noise(img,x,y,radio1,radio2)) {
					Remove_Noise(img,x,y,radio1);
				}
			}
		}
}
BOOL CCmdImgRemoveNoise::Is_Noise(FCObjImage & img,int x,int y,int radio1,int radio2)
{
	

	int NumWhite = 0;
	int NumScan =0;
	int xx,yy;
	xx=yy=0;
	for(yy = y - radio2 ;yy <= y + radio2 ; yy ++)
		for(xx = x - radio2 ; xx <= x + radio2 ; xx ++)
		{
			if ((xx<img.Width())&&(xx>=0)&&(yy<img.Height())&&(yy>=0)) {
				int nlen = (xx-x)*(xx-x)+(yy-y)*(yy-y);

				if (((int)sqrt((float)nlen)>radio1)&&((int)sqrt((float)nlen)<= radio2)) {
					NumScan ++;
					BYTE* bpPixel = img.GetBits(xx,yy);
					if ((bpPixel[0]==255)&& (bpPixel[1]==255)&& (bpPixel[2]==255)) {
						NumWhite ++;

					}
					else
					{
						continue;
					}
				}
				else{
					continue;
				}
			}
			else{
				continue;
			}

		}
		if (NumWhite == NumScan) {
			return TRUE;
		}
		else
			return FALSE;


}
void CCmdImgRemoveNoise::Remove_Noise(FCObjImage & img,int x,int y,int radio)
{
	int xx,yy;
	xx=yy=0;
	for(yy =y - radio ;yy<= y + radio ;yy++)
		for(xx = x - radio ; xx <= x + radio ;xx++)

		{
			if ((xx<img.Width())&&(xx>=0)&&(yy<img.Height())&&(yy>=0)) {
				int nlen = (xx-x)*(xx-x)+(yy-y)*(yy-y);

				if (((int)sqrt((float)nlen)<radio)) {
					BYTE* bpPixel = img.GetBits(xx,yy);
					bpPixel[0] = 255;
					bpPixel[1] = 255;
					bpPixel[2] = 255;
				}
			}
		}
}
