#include "StdAfx.h"
#include ".\cmdimgblur_zoom.h"

//=================================================================================
// 径向缩放/blur zoom (>=24 bit)
// adapted from GIMP v1.3.21
void  CCmdImgBlur_Zoom::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() < 24))
		return ;

	FCObjImage	imgOld(img) ; // 保存原图
	int			nSpan = img.ColorBits() / 8 ; // 3, 4
	int			nCenX = img.Width()/2, nCenY = img.Height()/2 ;
	int			i ;
	for (int y=0 ; y < img.Height() ; y++)
	{
		BYTE	* pWrite = img.GetBits(y) ;
		for (int x=0 ; x < img.Width() ; x++, pWrite += nSpan)
		{
			int		nSumB=0, nSumG=0, nSumR=0, nSumA=0 ;

			// 统计
			for (i=0 ; i < m_nLength ; i++)
			{
			
				int		xx = (int)(nCenX + (x-nCenX) * (1.0 + 0.02 * i)) ;
				int		yy = (int)(nCenY + (y-nCenY) * (1.0 + 0.02 * i)) ;
				if ((yy < 0) || (yy >= img.Height()) || (xx < 0) || (xx >= img.Width()))
					break ;
			
				BYTE    * pPixel = imgOld.GetBits (xx, yy) ;
				if (nSpan == 4)
				{
					int		nAlpha = pPixel[3] ;
					if (nAlpha != 0)
					{
						nSumA += nAlpha ;
						nSumB += pPixel[0]*nAlpha ; nSumG += pPixel[1]*nAlpha ; nSumR += pPixel[2]*nAlpha ;
					}
				}
				else
				{
					nSumB += pPixel[0] ; nSumG += pPixel[1] ; nSumR += pPixel[2] ;
				}
			}

			// 设置象素
			if (i != 0)
				if (nSpan == 4)
				{
					if ((pWrite[3] = nSumA/i) != 0)
					{
						pWrite[0] = nSumB/nSumA ; pWrite[1] = nSumG/nSumA ; pWrite[2] = nSumR/nSumA ;
					}
				}
				else
				{
					pWrite[0] = nSumB/i ; pWrite[1] = nSumG/i ; pWrite[2] = nSumR/i ;
				}
		}
	//	if (Percent != NULL)
	//		Percent->SetProgress (100 * y / img.Height()) ;
	}
}