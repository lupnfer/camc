#include "StdAfx.h"
#include ".\cmdimgbrightness.h"
#include "ObjProgress.h"
//=================================================================================
// 调节亮度/adjust brightness (>=24 bit)
void  CCmdImgBrightness::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	int nHeight = img.Height();
	int nWidth = img.Width();
	for(int y=0; y<nHeight;y++)
		{
			for(int x= 0;x<nWidth;x++)
				{
					//否则不发生变化。
					BYTE* bpPixel = img.GetBits(x,y);
					bpPixel[0] = FMin(bpPixel[0] + m_nPercent,255);
					bpPixel[1] = FMin(bpPixel[1] + m_nPercent,255);
					bpPixel[2] = FMin(bpPixel[2] + m_nPercent,255);
					
					

				}
			if (Percent != NULL)
			{
				Percent->SetProgress (y * 100 / img.Height()) ;
				//Percent->m_pbar.Clear();
				//Sleep(100);
			}
	}
	 
}
