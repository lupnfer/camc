#include "StdAfx.h"
#include ".\cmdimgcontrast.h"

//=================================================================================
// 调节对比度/adjust Contrast (>=24 bit)
void  CCmdImgContrast::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	int nHeight = img.Height();
	int nWidth = img.Width();
	float fSumRed ,fSumGreen ,fSumBlue;
	fSumRed = fSumGreen = fSumBlue =0.0;
	BYTE bAverRed , bAverGreen , bAverBlue;
	bAverRed = bAverGreen = bAverBlue =0;
	for(int y=0; y<nHeight;y++)
		for(int x= 0;x<nWidth;x++)
		{
			BYTE* bpPixel = img.GetBits(x,y);
			fSumRed += bpPixel[0];
			fSumGreen += bpPixel[1];
			fSumBlue += bpPixel[2];
		}
	
	bAverRed = fSumRed/nHeight/nWidth;
	bAverGreen = fSumGreen/nHeight/nWidth;
	bAverBlue = fSumBlue/nHeight/nWidth;

	float contrast;
	contrast=(float)(m_nPercent + 100)/100.0;


	for(int y=0; y<nHeight;y++)
		for(int x= 0;x<nWidth;x++)
		{
			//否则不发生变化。
			BYTE* bpPixel = img.GetBits(x,y);
			bpPixel[0] = FMin((int) ((contrast*bpPixel[0])-(contrast-1)*bAverRed) , 255);
			bpPixel[1] = FMin((int)((contrast*bpPixel[1])-(contrast-1)*bAverGreen) , 255);
			bpPixel[2] = FMin((int)((contrast*bpPixel[2])-(contrast-1)*bAverBlue) , 255);


		}
}
