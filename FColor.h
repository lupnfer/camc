//////////////////////////////////////////////////////////////////
//																//
//		��; : Color��ط�װ									//
//		���� : [Foolish] / 2003-4-17							//
//		���� : 2004-2-16										//
//		��ҳ : http://www.crazy-bit.com/						//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= ����	//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_COLOR_H__
#define	 __FOO_COLOR_H__
#include "StdDefine.h"

inline BOOL operator==(const RGBTRIPLE & cr1, const RGBTRIPLE & cr2) {
	return (cr1.rgbtBlue == cr2.rgbtBlue) && (cr1.rgbtGreen == cr2.rgbtGreen) && (cr1.rgbtRed == cr2.rgbtRed) ;
}
//=============================================================================
//	Color class
//=============================================================================
class FCColor
{
public :
	// ��������32bit���صĵ�Ч���أ���������ǳ���Ҫ(speed)����ȫ���Ͳ�����
	// cr1������    cr2��ǰ��
    static void     CombineAlphaPixel (RGBQUAD * pDest,
                                       const RGBQUAD & cr1, int nAlpha1,
                                       const RGBQUAD & cr2, int nAlpha2) ;
	// ��������͸���Ҫ�ˣ���ȫ��鵱Ȼ������:-)������pSrc�е�alpha���㣬nAlphaSrcһ��Ҫ��֤��0��0xFF֮��
	static void     AlphaBlendPixel (RGBQUAD * pDest, const RGBQUAD * pSrc) ;
	static void     AlphaBlendPixel (BYTE * pDest, const BYTE * pSrc, int nAlphaSrc) ;

	// �õ���ɫ������cr��ӽ�����ɫ����
	static int		GetNearestPaletteIndex (const RGBQUAD * pPalette, int nNum, const RGBQUAD & cr) ;

	// �õ�����
	static double	GetLight (BYTE *prgb) ;

	// �������أ�Ӧ�ñ�memcpy��
	static void     CopyPixel (void *pDest, const void *pSrc, int nBytes) ;

	// RGB <==> HLS
	static void		 RGBtoHLS (const RGBTRIPLE &rgb, double *H, double *L, double *S) ;
	static RGBTRIPLE HLStoRGB (const double &H, const double &L, const double &S) ;

	// RGB <==> HSV
	static void		 RGBtoHSV (const RGBTRIPLE &rgb, double *H, double *S, double *V) ;
	static RGBTRIPLE HSVtoRGB (const double &H, const double &S, const double &V) ;

	// RGB <==> CMYK
	static void		 RGBtoCMYK (const RGBTRIPLE &rgb, int *cyan, int *magenta, int *yellow, int *black) ;
	static RGBTRIPLE CMYKtoRGB (int cyan, int magenta, int yellow, int black) ;

    // Computes bilinear interpolation of four pixels.
    // The pixels in 'crPixel' in the following order: [0,0], [1,0], [0,1], [1,1].
    static RGBQUAD   Get_Bilinear_Pixel (double & x, double & y, BOOL bHasAlpha,
                                         const RGBQUAD crPixel[4]) ;

    static int		SumRGB (BYTE * prgb) ;
	static void		SwapRGB (BYTE * prgb) ;
	static RGBQUAD	RGBtoRGBQUAD (COLORREF color) ;
	static COLORREF RGBQUADtoRGB (RGBQUAD rgb) ;
	static BYTE		GetGrayscale (const RGBQUAD * prgb) ;
	static RGBQUAD	Split16Bit_565 (WORD wPixel) ;
	static RGBQUAD	Split16Bit_555 (WORD wPixel) ;
	static WORD		Combine16Bit_565 (const RGBQUAD & rgb) ;
	static WORD		Combine16Bit_555 (const RGBQUAD & rgb) ;
public :
	static RGBQUAD    crBlack ;
	static RGBQUAD    crWhite ;
protected :
	FCColor() {} // don't create object
} ;

//=============================================================================
//	inline implement
//=============================================================================
inline RGBQUAD  FCColor::RGBtoRGBQUAD (COLORREF color) {
	RGBQUAD		rgb = {GetBValue(color), GetGValue(color), GetRValue(color), (BYTE)(color>>24)} ;
	return rgb ;
}
inline COLORREF  FCColor::RGBQUADtoRGB (RGBQUAD rgb) {
	return RGB (rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue) ;
}
inline BYTE  FCColor::GetGrayscale (const RGBQUAD * prgb) {
	register UINT32		tol = 30*prgb->rgbRed + 59*prgb->rgbGreen + 11*prgb->rgbBlue ;
	return (tol / 100) ;
}
inline int  FCColor::SumRGB (BYTE * prgb) {
	return (prgb[0] + prgb[1] + prgb[2]) ;
}
inline void  FCColor::SwapRGB (BYTE * prgb) {
	BYTE	temp = prgb[0] ; prgb[0] = prgb[2] ; prgb[2] = temp ;
}
//===================================================================
inline RGBQUAD  FCColor::Split16Bit_565 (WORD wPixel) {
	RGBQUAD		rgb ;
	rgb.rgbRed   = (BYTE) ((MASK16_RED_565 & wPixel) >> 8) ;
	rgb.rgbGreen = (BYTE) ((MASK16_GREEN_565 & wPixel) >> 3) ;
	rgb.rgbBlue  = (BYTE) ((MASK16_BLUE_565 & wPixel) << 3) ;
	return rgb ;
}
inline RGBQUAD  FCColor::Split16Bit_555 (WORD wPixel) {
	RGBQUAD		rgb ;
	rgb.rgbRed   = (BYTE) ((MASK16_RED_555 & wPixel) >> 7) ;
	rgb.rgbGreen = (BYTE) ((MASK16_GREEN_555 & wPixel) >> 2) ;
	rgb.rgbBlue  = (BYTE) ((MASK16_BLUE_555 & wPixel) << 3) ;
	return rgb ;
}
inline WORD  FCColor::Combine16Bit_565 (const RGBQUAD & rgb) {
	WORD		wPixel ;
	wPixel  = ((rgb.rgbRed >> 3) << 11) ;
	wPixel |= ((rgb.rgbGreen >> 2) << 5) ;
	wPixel |= (rgb.rgbBlue >> 3) ;
	return wPixel ;
}
inline WORD  FCColor::Combine16Bit_555 (const RGBQUAD & rgb) {
	WORD		wPixel ;
	wPixel  = ((rgb.rgbRed >> 3) << 10) ;
	wPixel |= ((rgb.rgbGreen >> 3) << 5) ;
	wPixel |= (rgb.rgbBlue >> 3) ;
	return wPixel ;
}
//===================================================================
inline double  FCColor::GetLight (BYTE *prgb) {
    double		cmax = FMax (prgb[0], FMax (prgb[1], prgb[2])) ;
	double		cmin = FMin (prgb[0], FMin (prgb[1], prgb[2])) ;
	return (cmax+cmin) / 2.0 / 255.0 ;
}
//===================================================================
FLib_forceinline void  FCColor::CopyPixel (void *pDest, const void *pSrc, int nBytes) {
	if (nBytes == 4)
		*(DWORD*)pDest = *(DWORD*)pSrc ;
	else
        if (nBytes == 3)
            *(RGBTRIPLE*)pDest = *(RGBTRIPLE*)pSrc ;
        else
            if (nBytes == 1)
                *(BYTE*)pDest = *(BYTE*)pSrc ;
            else
                if (nBytes == 2)
                    *(WORD*)pDest = *(WORD*)pSrc ;
}
//===================================================================
inline int  FCColor::GetNearestPaletteIndex (const RGBQUAD * pPalette, int nNum, const RGBQUAD & cr)
{
	if (pPalette == NULL)
		return -1 ;

	int		nDistance = 200000, // > 255*255*3
			nIndex = 0 ;
	for (int i=0 ; i < nNum ; i++)
	{
		int  k = FSquare (pPalette[i].rgbBlue-cr.rgbBlue) +
				 FSquare (pPalette[i].rgbGreen-cr.rgbGreen) +
				 FSquare (pPalette[i].rgbRed-cr.rgbRed) ;
		if (k == 0)
		{
			nIndex = i ;
			break ;
		}
		if (k < nDistance)
		{
			nDistance = k ;
			nIndex = i ;
		}
	}
	return nIndex ;
}
//===================================================================
// ��������32bit���صĵ�Ч���أ���������ǳ���Ҫ(speed)����ȫ���Ͳ�����
// cr1������    cr2��ǰ��
FLib_forceinline void  FCColor::CombineAlphaPixel (RGBQUAD * pDest, const RGBQUAD & cr1, int nAlpha1,
                                                   const RGBQUAD & cr2, int nAlpha2) {
	if ((nAlpha1 != 0) || (nAlpha2 != 0))
	{
		if (nAlpha2 == 0)
		{
			pDest->rgbBlue  = cr1.rgbBlue ;
			pDest->rgbGreen = cr1.rgbGreen ;
			pDest->rgbRed   = cr1.rgbRed ;
			pDest->rgbReserved = nAlpha1 ;
			return ;
		}
		if ((nAlpha1 == 0) || (nAlpha2 == 0xFF))
		{
			pDest->rgbBlue  = cr2.rgbBlue ;
			pDest->rgbGreen = cr2.rgbGreen ;
			pDest->rgbRed   = cr2.rgbRed ;
			pDest->rgbReserved = nAlpha2 ;
			return ;
		}
		// �����ò����ж�[0,0xFF]�����������
		int    nTmp1 = 0xFF * nAlpha1, nTmp2 = 0xFF * nAlpha2 ;
		int    nTmp12 = nAlpha1 * nAlpha2 ;
		int    nTemp = nTmp1 + nTmp2 - nTmp12 ;
		pDest->rgbBlue  = (nTmp2*cr2.rgbBlue + (nTmp1 - nTmp12)*cr1.rgbBlue)/nTemp ;
		pDest->rgbGreen = (nTmp2*cr2.rgbGreen + (nTmp1 - nTmp12)*cr1.rgbGreen)/nTemp ;
		pDest->rgbRed   = (nTmp2*cr2.rgbRed + (nTmp1 - nTmp12)*cr1.rgbRed)/nTemp ;
		pDest->rgbReserved = nTemp / 0xFF ;
/*		����Ĵ�����δ�Ż����ģ��ɶ��Ը���Щ
		int    nTemp = 0xFF*(nAlpha1 + nAlpha2) - nAlpha1*nAlpha2 ;
		pDest->rgbBlue  = min(0xFF, (0xFF*cr2.rgbBlue*nAlpha2 + (0xFF - nAlpha2)*cr1.rgbBlue*nAlpha1)/nTemp) ;
		pDest->rgbGreen = min(0xFF, (0xFF*cr2.rgbGreen*nAlpha2 + (0xFF - nAlpha2)*cr1.rgbGreen*nAlpha1)/nTemp) ;
		pDest->rgbRed   = min(0xFF, (0xFF*cr2.rgbRed*nAlpha2 + (0xFF - nAlpha2)*cr1.rgbRed*nAlpha1)/nTemp) ;
		pDest->rgbReserved = nTemp / 0xFF ;*/
	}
	else
	{
		pDest->rgbBlue = pDest->rgbGreen = pDest->rgbRed = 0xFF ;
		pDest->rgbReserved = 0 ;
	}
}
//===================================================================
FLib_forceinline void  FCColor::AlphaBlendPixel (BYTE * pDest, const BYTE * pSrc, int nAlphaSrc)
{
    if (nAlphaSrc == 0xFF)
    {
        *(RGBTRIPLE*)pDest = *(RGBTRIPLE*)pSrc ;
        return ;
    }
    if (nAlphaSrc == 0)
        return ;
    // �����ò����ж�[0,0xFF]�����������
    if (*pSrc != *pDest)
        *pDest = (*pSrc - *pDest) * nAlphaSrc / 0xFF + *pDest ;
    pSrc++ ; pDest++ ;
    if (*pSrc != *pDest)
        *pDest = (*pSrc - *pDest) * nAlphaSrc / 0xFF + *pDest ;
    pSrc++ ; pDest++ ;
    if (*pSrc != *pDest)
        *pDest = (*pSrc - *pDest) * nAlphaSrc / 0xFF + *pDest ;
}
FLib_forceinline void  FCColor::AlphaBlendPixel (RGBQUAD * pDest, const RGBQUAD * pSrc)
{
	FCColor::AlphaBlendPixel ((BYTE*)pDest, (BYTE*)pSrc, pSrc->rgbReserved) ;
}
//===================================================================

#endif
