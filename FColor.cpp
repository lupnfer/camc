#include "stdafx.h"
#include "FColor.h"

RGBQUAD FCColor::crBlack = {0, 0, 0, 0xFF} ;
RGBQUAD FCColor::crWhite = {0xFF, 0xFF, 0xFF, 0xFF} ;

//===============================================================================
void  FCColor::RGBtoHLS (const RGBTRIPLE &rgb, double *H, double *L, double *S)
{
	double		r = (double)rgb.rgbtRed / 255.0 ;
	double		g = (double)rgb.rgbtGreen / 255.0 ;
	double		b = (double)rgb.rgbtBlue / 255.0 ;
	double		cmax = FMax (r, FMax (g, b)) ;
	double		cmin = FMin (r, FMin (g, b)) ;
	*L = (cmax+cmin) / 2.0 ;
	if (cmax == cmin)
	{
		*S = 0 ;
		*H = 0 ;
	}
	else
	{
		if (*L < 0.5) 
			*S = (cmax-cmin) / (cmax+cmin) ;
		else
			*S = (cmax-cmin) / (2.0-cmax-cmin) ;

		double     delta = cmax - cmin ;
		if (delta == 0.0)
			delta = 1.0 ;

		if (r == cmax)
			*H = (g-b) / delta ;
		else
			if (g == cmax)
				*H = 2.0 + (b-r) / delta ;
			else
				*H = 4.0 + (r-g) / delta ;
		*H /= 6.0 ;
		if (*H < 0.0)
			*H += 1 ;
	}
}
//===============================================================================
static double  __F_HLS_Value (const double &m1, double const &m2, double h)
{
	if (h < 0)
		h += 1.0 ;
	if (h > 1)
		h -= 1.0 ;
	if (6.0*h < 1)
		return (m1+(m2-m1)*h*6.0) ;
	if (2.0*h < 1)
		return m2 ;
	if (3.0*h < 2.0)
		return (m1+(m2-m1)*((2.0/3.0)-h)*6.0) ;
	return m1 ;
}
RGBTRIPLE  FCColor::HLStoRGB (const double& H, const double& L, const double& S)
{
	double		r, g, b ;
	if (S == 0)
	{
		r = g = b = L ;
	}
	else 
	{
		double		m1, m2 ;
		if (L <= 0.5)
			m2 = L * (1.0+S) ;
		else
			m2 = L + S - L*S ;
		m1 = 2.0*L - m2 ;
		r = __F_HLS_Value (m1, m2, H+1.0/3.0) ;
		g = __F_HLS_Value (m1, m2, H) ;
		b = __F_HLS_Value (m1, m2, H-1.0/3.0) ;
	}
	RGBTRIPLE	rgb ;
	rgb.rgbtRed   = FMax (FMin ((int)(r*255), 0xFF), 0) ;
	rgb.rgbtGreen = FMax (FMin ((int)(g*255), 0xFF), 0) ;
	rgb.rgbtBlue  = FMax (FMin ((int)(b*255), 0xFF), 0) ;
	return rgb ;
}
//===============================================================================
void  FCColor::RGBtoCMYK (const RGBTRIPLE &rgb, int *c, int *m, int *y, int *k)
{
	*c = 0xFF - rgb.rgbtRed ;
	*m = 0xFF - rgb.rgbtGreen ;
	*y = 0xFF - rgb.rgbtBlue ;
	*k = 0xFF ;

	if (*c < *k)  *k = *c ;
	if (*m < *k)  *k = *m ;
	if (*y < *k)  *k = *y ;

	*c = ((*c - *k) << 8) / (0x100 - *k) ;
	*m = ((*m - *k) << 8) / (0x100 - *k) ;
	*y = ((*y - *k) << 8) / (0x100 - *k) ;
}
RGBTRIPLE  FCColor::CMYKtoRGB (int c, int m, int y, int k)
{
	if (k != 0)
    {
		c = ((c * (0x100 - k)) >> 8) + k ;
		m = ((m * (0x100 - k)) >> 8) + k ;
		y = ((y * (0x100 - k)) >> 8) + k ;
    }
	RGBTRIPLE	rgb ;
	rgb.rgbtRed   = 0xFF - c ;
	rgb.rgbtGreen = 0xFF - m ;
	rgb.rgbtBlue  = 0xFF - y ;
	return rgb ;
}
//===============================================================================
void  FCColor::RGBtoHSV (const RGBTRIPLE &rgb, double *H, double *S, double *V)
{
	double	red, green, blue ;
	double	fmin, fmax ;

	red   = rgb.rgbtRed / 255.0 ;
	green = rgb.rgbtGreen / 255.0 ;
	blue  = rgb.rgbtBlue / 255.0 ;

	fmax = (red > green) ? FMax(red, blue) : FMax(green, blue) ;
	fmin = (red > green) ? FMin(green, blue) : FMin(red, blue) ;

	*V = fmax ;
	*S = (fmax != 0.0) ? ((fmax - fmin) / fmax) : 0.0 ;

	if (*S == 0.0)
		*H = 0.0 ;
	else
    {
		double  delta = fmax - fmin ;

		if (red == fmax)
			*H = (green - blue) / delta ;
		else
			if (green == fmax)
				*H = 2 + (blue - red) / delta ;
			else
				if (blue == fmax)
					*H = 4 + (red - green) / delta ;

		*H /= 6.0 ;
		if (*H < 0.0)
			*H += 1.0 ;
		else
			if (*H > 1.0)
				*H -= 1.0 ;
    }
}
//===============================================================================
RGBTRIPLE  FCColor::HSVtoRGB (const double& H, const double& S, const double& V)
{
	RGBTRIPLE   rgb ;

	if (S == 0.0)
    {
		rgb.rgbtBlue = rgb.rgbtGreen = rgb.rgbtRed = (int)(V * 255.0) ;
    }
	else
    {
		double		h, s, v ;
		double		f, p, q, t ;
		h = H * 6.0 ;
		s = S ;
		v = V ;

		if (h == 6.0)
			h = 0.0 ;

		f = h - (int)h ;
		p = v * (1.0 - s) ;
		q = v * (1.0 - s * f) ;
		t = v * (1.0 - s * (1.0 - f)) ;

		switch ((int)h)
		{
			case 0 :
				rgb.rgbtRed = (int)(v * 255.0) ;
				rgb.rgbtGreen = (int)(t * 255.0) ;
				rgb.rgbtBlue = (int)(p * 255.0) ;
				break ;
			case 1 :
				rgb.rgbtRed = (int)(q * 255.0) ;
				rgb.rgbtGreen = (int)(v * 255.0) ;
				rgb.rgbtBlue = (int)(p * 255.0) ;
				break ;
			case 2 :
				rgb.rgbtRed = (int)(p * 255.0) ;
				rgb.rgbtGreen = (int)(v * 255.0) ;
				rgb.rgbtBlue = (int)(t * 255.0) ;
				break ;
			case 3 :
				rgb.rgbtRed = (int)(p * 255.0) ;
				rgb.rgbtGreen = (int)(q * 255.0) ;
				rgb.rgbtBlue = (int)(v * 255.0) ;
				break ;
			case 4 :
				rgb.rgbtRed = (int)(t * 255.0) ;
				rgb.rgbtGreen = (int)(p * 255.0) ;
				rgb.rgbtBlue = (int)(v * 255.0) ;
				break ;
			case 5 :
				rgb.rgbtRed = (int)(v * 255.0) ;
				rgb.rgbtGreen = (int)(p * 255.0) ;
				rgb.rgbtBlue = (int)(q * 255.0) ;
				break ;
		}
    }
	return rgb ;
}
//===============================================================================
// Computes bilinear interpolation of four pixels.
// The pixels in 'crPixel' in the following order: [0,0], [1,0], [0,1], [1,1].
RGBQUAD  FCColor::Get_Bilinear_Pixel (double & x, double & y, BOOL bHasAlpha,
                                      const RGBQUAD crPixel[4])
{
    RGBQUAD     crRet = {0xFF, 0xFF, 0xFF, 0xFF} ;

    x = fmod (x, 1.0) ;
    y = fmod (y, 1.0) ;
    if (x < 0.0)
        x += 1.0 ;
    if (y < 0.0)
        y += 1.0 ;

    int     nAlpha[4] = {0xFF, 0xFF, 0xFF, 0xFF} ;
    for (int j=0 ; j < 4 ; j++)
        if (bHasAlpha)
            nAlpha[j] = crPixel[j].rgbReserved ;

    const double  ix = 1.0 - x,
                  iy = 1.0 - y ;
    double        m0 = ix * nAlpha[0] + x * nAlpha[1],
                  m1 = ix * nAlpha[2] + x * nAlpha[3] ;

    // dest alpha value
    const double  fAlpha = iy * m0 + y * m1 ;
    crRet.rgbReserved = (int)fAlpha ;

    if (crRet.rgbReserved) // has alpha
    {
        const BYTE   * pPixel0 = (BYTE*)&crPixel[0], * pPixel1 = (BYTE*)&crPixel[1],
                     * pPixel2 = (BYTE*)&crPixel[2], * pPixel3 = (BYTE*)&crPixel[3] ;
        for (int i=0 ; i < 3 ; i++)
        {
            m0 = ix * nAlpha[0] * pPixel0[i] + x * pPixel1[i] * nAlpha[1] ;
            m1 = ix * nAlpha[2] * pPixel2[i] + x * pPixel3[i] * nAlpha[3] ;
            ((BYTE*)&crRet)[i] = int((iy * m0 + y * m1) / crRet.rgbReserved) ;
        }
    }
    return crRet ;
}
//===============================================================================
