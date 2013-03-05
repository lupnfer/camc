#include "stdafx.h"
#include "ObjImage.h"
#include "FColor.h"
#include "oxo_helper.h"
#include "win32_Func.h"
#include "Quantize.h"
#include "FCSinglePixelProcessBase.h"
#include "ObjProgress.h"
#include "stetch.h"

#include "highgui.h"


extern void  aggRotateImage_AA_32Bit (FCObjImage & img, int nAngle) ;
extern void  aggStretchImage_AA_32Bit (FCObjImage & img, int nNewWidth, int nNewHeight) ;
extern void  aggStretchImage_AA_24Bit (FCObjImage & img, int nNewWidth, int nNewHeight) ;

//==============================================================================
static void  __fooFillImageRect (const FCObjImage & img, const RECT & rc, DWORD dwPixel)
{
	if (IsRectEmpty(&rc))
		return  ;

	int		nSpan = img.ColorBits() / 8 ; // 1, 2, 3, 4
	for (int y=rc.top ; y < rc.bottom ; y++)
	{
		BYTE	* pPixel = img.GetBits (rc.left, y) ;
		for (int x=rc.left ; x < rc.right ; x++, pPixel += nSpan)
			FCColor::CopyPixel (pPixel, &dwPixel, nSpan) ;
	}
}
//===================================================================
void  FCObjImage::__InitClassMember ()
{
    ::ZeroMemory (&m_DibInfo, sizeof(m_DibInfo)) ;
    ::ZeroMemory (m_dwBitFields, sizeof(DWORD)*3) ;
    m_pByte = NULL ; m_ppLine = NULL ; m_pPalette = NULL ;m_pDibInfo=NULL;
	//m_paper.SetImage(this);
}
//===================================================================
BOOL  FCObjImage::BoundRect (RECT & rect) const
{
	RECT	rcImg = {0, 0, Width(), Height()} ;
	return ::IntersectRect (&rect, &rcImg, &rect) ;
}
void  FCObjImage::BoundRect (RECT_F & rect) const
{
	rect.left = FMax ((double)0.0, rect.left) ;
	rect.top = FMax ((double)0.0, rect.top) ;
	rect.right = FMin ((double)Width(), rect.right) ;
	rect.bottom = FMin ((double)Height(), rect.bottom) ;
	NormalizeRect_F (rect) ;
}
//===================================================================
void  FCObjImage::Destroy ()
{
	if (m_ppLine != NULL)
		delete[] m_ppLine ;

    FLib_FreePixel (m_pByte) ;

    if (m_pPalette != NULL)
        delete[] m_pPalette ;

    __InitClassMember () ;
}
//===================================================================

BOOL  FCObjImage::__Create (int iWidth, int iHeight, WORD wColorBit, DWORD dwType, const DWORD pField[3])
{
	if (iHeight < 0)
		return FALSE ; // 不支持自上而下存储的DIB
	if (this->GetHandle() != NULL)
		this->Unload () ;

	// retrieve screen's setting
	if ((iWidth == 0) || (iHeight == 0) || (wColorBit == 0))
	{
		HDC		hSrcDC = ::CreateIC (TEXT("display"), NULL, NULL, NULL) ;
		if (iWidth == 0)	iWidth = (DWORD) GetDeviceCaps (hSrcDC, HORZRES) ;
		if (iHeight == 0)	iHeight = (DWORD) GetDeviceCaps (hSrcDC, VERTRES) ;
		if (wColorBit == 0)	wColorBit = (WORD) GetDeviceCaps (hSrcDC, BITSPIXEL) ;	
		::DeleteDC(hSrcDC) ;
	}

	// create DIB info header
	BITMAPV4HEADER	* bmhf = (BITMAPV4HEADER *) new BYTE[sizeof(BITMAPV4HEADER) + ((wColorBit <= 8) ? (4 * (1 << wColorBit)) : 0)] ;
	bmhf->bV4Size =sizeof (*bmhf) ;
	//bmhf->bV4Size          = sizeof (*bmhf) ;
	bmhf->bV4Width         = iWidth ;
	bmhf->bV4Height        = iHeight ;
	bmhf->bV4Planes        = 1 ;
	bmhf->bV4BitCount      = wColorBit ;
	bmhf->bV4V4Compression = dwType ;
	if (dwType == BI_BITFIELDS)
		::CopyMemory (&bmhf->bV4RedMask, pField, 12) ;

	// create DIBSection
	m_hBitmap = CreateDIBSection (NULL, (BITMAPINFO *)bmhf, DIB_RGB_COLORS, (VOID **)&m_pByte, NULL, 0) ;
	delete[] bmhf ;
	if (m_hBitmap == NULL)
		return FALSE ;

	// get created DIB's information
	DIBSECTION		DibInfo ;
	::GetObject (m_hBitmap, sizeof (DIBSECTION), &DibInfo) ;
	CopyMemory (&m_DibInfo, &DibInfo.dsBmih, sizeof (BITMAPINFOHEADER) + 12) ;
	m_DibInfo.biSize = sizeof (m_DibInfo) ;
	
	m_pByte = FLib_MallocPixel (GetPitch()*Height()) ;
	FAssert (((int)m_pByte % 4) == 0) ; // 四字节边界对齐

	// create a line pointer, to accelerate pixel access
	m_ppLine = (BYTE **) new BYTE [sizeof(BYTE *) * iHeight] ;
	DWORD	dwPitch = this->GetPitch () ;
	m_ppLine[0] = m_pByte + (iHeight - 1) * dwPitch ;
	for (int Y = 1 ; Y < iHeight ; Y++)
		m_ppLine[Y] = m_ppLine[Y - 1] - dwPitch ;

	// if the DIB's color bit <= 8, set a gray palette to it
	if ((this->ColorBits() <= 8) )
		this->SetGrayPalette () ;
	return TRUE ;
}
//===================================================================
BOOL  FCObjImage::Create (BITMAPINFO * bmif, BYTE * pBits)
{
	if (bmif != NULL)
	{
		BITMAPV4HEADER	* pBmf = (BITMAPV4HEADER *) &bmif->bmiHeader ;
		RGBQUAD			* pPalette = (RGBQUAD *)((BYTE *)pBmf + pBmf->bV4Size) ;

		// not support RLE-format DIB
		if ((pBmf->bV4V4Compression == BI_RLE8) || (pBmf->bV4V4Compression == BI_RLE4))
			return FALSE ;

		// create DIB
		if (!this->__Create (pBmf->bV4Width, abs(pBmf->bV4Height), pBmf->bV4BitCount, pBmf->bV4V4Compression, &pBmf->bV4RedMask))
			return FALSE ;

		// set palette
		if (this->ColorBits() <= 8)
			this->SetColorTable (0, 1 << this->ColorBits(), pPalette) ;

		// set pixel data
		if (pBits != NULL)
			::CopyMemory (this->GetMemStart(), pBits, this->GetPitch() * this->Height()) ;
	}
	return TRUE ;
}
//===================================================================

BOOL  FCObjImage::Create (const BITMAPINFOHEADER * pBmif)
{
	if (pBmif == NULL)
    {
		FAssert(FALSE) ; return FALSE ;
    }
	if ((pBmif->biHeight <= 0) || (pBmif->biWidth <= 0) || (pBmif->biBitCount <= 0))
    {
        FAssert(FALSE) ;
		return FALSE ; // 不支持自上而下存储的DIB
    }
	if (IsValidImage())
		this->Destroy() ;

    // init struct
    ZeroMemory (&m_DibInfo, sizeof(m_DibInfo)) ;
    m_DibInfo.biSize          = sizeof(BITMAPINFOHEADER) ;
    m_DibInfo.biWidth         = pBmif->biWidth ;
    m_DibInfo.biHeight        = pBmif->biHeight ;
    m_DibInfo.biPlanes        = 1 ;
    m_DibInfo.biBitCount      = pBmif->biBitCount ;
    m_DibInfo.biCompression   = pBmif->biCompression ;
    m_DibInfo.biXPelsPerMeter = pBmif->biXPelsPerMeter ;
    m_DibInfo.biYPelsPerMeter = pBmif->biYPelsPerMeter ;

    // 这里假定BI_BITFIELDS只对16 bit有效
    if (pBmif->biBitCount == 16)
    {
        m_dwBitFields[0] = MASK16_RED_555 ; // 16 bit默认格式5-5-5
        m_dwBitFields[1] = MASK16_GREEN_555 ;
        m_dwBitFields[2] = MASK16_BLUE_555 ;
        if (pBmif->biCompression == BI_BITFIELDS) // 用户提供
            ::CopyMemory (m_dwBitFields, pBmif + 1, 12) ;
    }
    else
        m_DibInfo.biCompression = BI_RGB ; // 32位色就别搞这么复杂了吧

	// create DIB's bytes , 像素区域必须初始化为0，因为很多地方用到了这个概念
    m_pByte = FLib_MallocPixel (GetPitch()*Height()) ;
    FAssert (((int)m_pByte % 4) == 0) ; // 四字节边界对齐

	// create a line pointer, to accelerate pixel access
	m_ppLine = (BYTE **) new BYTE [sizeof(BYTE *) * Height()] ;
	const DWORD     dwPitch = GetPitch() ;
	m_ppLine[0] = m_pByte + (Height() - 1) * dwPitch ;
	for (int Y = 1 ; Y < Height() ; Y++)
		m_ppLine[Y] = m_ppLine[Y - 1] - dwPitch ;

    // 8bit 创建灰度调色板
	if (ColorBits() <= 8)
    {
        m_pPalette = new RGBQUAD[1 << ColorBits()] ;
		SetGrayPalette() ;
    }
	//对应纸张的创建
	//m_paper.Create(pBmif->biWidth,pBmif->biHeight);
    return TRUE ;
}
//===================================================================
BOOL  FCObjImage::Create (int iWidth, int iHeight, WORD wColorBit)
{
    BITMAPINFOHEADER    bmih = {sizeof(BITMAPINFOHEADER)} ;
    bmih.biWidth = iWidth ; bmih.biHeight = iHeight ; bmih.biBitCount = wColorBit ;
    bmih.biCompression = BI_RGB ;
	bmih.biXPelsPerMeter=7874;
	bmih.biYPelsPerMeter=7874;
    return this->Create (&bmih) ;
}
//===================================================================
FCObjImage & FCObjImage::operator= (const FCObjImage & image)
{
	if (image.IsValidImage() && (&image != this))
	{
        BITMAPINFOHEADER   * pBmhf = FLib_NewImgInfoNoPalette (image) ;
        if (Create (pBmhf))
		{
			// copy the pixels
			::CopyMemory (GetMemStart(), image.GetMemStart(), image.GetPitch()*image.Height()) ;
			// copy the palette
			if (image.ColorBits() <= 8)
				FCObjImage::fooCopyPalette (*this, image) ;
            // copy position
			FCObjGraph::operator=(image) ;
			////m_paper=(image.GetPaper());
		}
        delete[] pBmhf ;
	}
    
	return *this ;

}
double	FCObjImage::GetPixelData1 (int x, int y) 
{
	if(x>=FCObjImage::Width()||y>=FCObjImage::Height()||x<0||y<0)
		return -1;
	double ret=GetPixelData(x, y);
	return ret;
}
//===================================================================
DWORD  FCObjImage::GetPixelData (int x, int y) const
{
	
	const BYTE    * pPixel = GetBits (x, y) ;
	switch (ColorBits ())
	{
		case  1 :  return 0x01 & (* pPixel >> (7 - (x & 7))) ;
		case  4 :  return 0x0F & (* pPixel >> (x & 1 ? 0 : 4)) ;
		case  8 :  return *(BYTE*)pPixel ;
		case 16 :  return *(WORD*)pPixel ;
		case 24 :
			{
				DWORD		dwrgb = 0 ;
				* (RGBTRIPLE *) &dwrgb = * (RGBTRIPLE *) pPixel ;
				return dwrgb ;
			}
		case 32 :  return *(DWORD*)pPixel ;
	}
	return 0 ;
}
//===================================================================
void  FCObjImage::SetPixelData (int x, int y, DWORD dwPixel)
{
    BYTE    * pPixel = GetBits (x, y) ;
	switch (ColorBits())
	{
		case  1 :  * pPixel &= ~(1     << (7 - (x & 7))) ;
				   * pPixel |= dwPixel << (7 - (x & 7)) ;
				   break ;
		case  4 :  * pPixel &= 0x0F    << (x & 1 ? 4 : 0) ;
				   * pPixel |= dwPixel << (x & 1 ? 0 : 4) ;
                   break ;
        case  8 :
        case 16 :
        case 24 :
        case 32 :  FCColor::CopyPixel (pPixel, &dwPixel, ColorBits() / 8) ;
                   break ;
    }
}
//===================================================================
void  FCObjImage::ConvertToGray ()
{
	if (!IsValidImage() || IsGrayPalette())
		return ;

	if (ColorBits() <= 8)
	{
		int         nNum = 1 << ColorBits() ;
		RGBQUAD     * palette = new RGBQUAD[nNum] ;
		this->GetColorTable (0, nNum, palette) ;
		if (ColorBits() <= 4) // <= 4, 只修改调色板
		{
			for (int i=0 ; i < nNum ; i++)
				::FillMemory (&palette[i], 3, FCColor::GetGrayscale (&palette[i])) ;
			this->SetColorTable (0, nNum, palette) ;
		}
		else // == 8 bit, 修改调色板和索引
		{
			for (int i=0 ; i < nNum ; i++)
				palette[i].rgbBlue = FCColor::GetGrayscale (&palette[i]) ;
			// 修改索引
			for (int y=0 ; y < Height() ; y++)
			{
				BYTE    * pLine = GetBits (y) ;
				for (int x=0 ; x < Width() ; x++, pLine++)
					*pLine = palette[*pLine].rgbBlue ;
			}
			this->SetGrayPalette () ;
		}
		delete[] palette ;
		return ;
	}

    // 16bit夹在里面真是麻烦，So...
    if (ColorBits() == 16)
        ConvertTo24Bit() ;

	// >= 24 bit color
	FCObjImage      OldDib (*this) ; // 保存原始数据
	int             nSpan = OldDib.ColorBits() / 8 ; FAssert(nSpan >= 3);
	if (this->Create (OldDib.Width(), OldDib.Height(), 8))
	{
		for (int y=0 ; y < Height() ; y++)
		{
			BYTE    * pDest = this->GetBits (y),
                    * pSrc = OldDib.GetBits (y) ;
			for (int x=0 ; x < Width() ; x++, pSrc += nSpan)
				*pDest++ = FCColor::GetGrayscale ((RGBQUAD*)pSrc) ;
		}
        this->SetGrayPalette () ; // set gray palette
		this->SetGraphObjPos (OldDib.GetGraphObjPos()) ;
	}
}
//===================================================================
void  FCObjImage::ConvertTo16Bit ()
{
	if (!IsValidImage() || (ColorBits () == 16))
		return ;

	// 保存原始数据
	FCObjImage		OldPic (* (FCObjImage *) this) ;
	if (!Create (OldPic.Width(), OldPic.Height(), 16))
        return ;

	// 获取调色板
	RGBQUAD     * pPal = NULL ;
	if (OldPic.ColorBits() <= 8)
	{
        int     nNum = 1 << OldPic.ColorBits() ;
        pPal = new RGBQUAD [nNum] ;
        OldPic.GetColorTable (0, nNum, pPal) ;
        for (int i=0 ; i < nNum ; i++) // palette to 16 bit
            *(DWORD*) &pPal[i] = FCColor::Combine16Bit_555 (pPal[i]) ;
	}

	// 颜色转换
	int       nOldSpan = OldPic.ColorBits() / 8 ; // 3 or 4
	for (int y=0 ; y < Height() ; y++)
	{
        BYTE    * pNew = this->GetBits (y),
                * pOld = OldPic.GetBits (y) ;
        for (int x=0 ; x < Width() ; x++, pNew += 2, pOld += nOldSpan)
            switch (OldPic.ColorBits())
	        {
		        case 1 :
		        case 4 :
                case 8 : // 1,4,8 ==> 16
                    FCColor::CopyPixel (pNew, &pPal[(BYTE)OldPic.GetPixelData(x,y)], 2) ;
                    break ;
                case 24 :
                case 32 : // 24,32 ==> 16
                    * (WORD *) pNew = FCColor::Combine16Bit_555 (*(RGBQUAD*)pOld) ;
                    break ;
            }
	}
	if (pPal != NULL)
		delete[] pPal ;
	this->SetGraphObjPos (OldPic.GetGraphObjPos()) ;
}
//===================================================================

//===================================================================
//===================================================================
void  FCObjImage::ConvertTo24Bit()
{
	this->__ConvertToTrueColor (24) ;
}
void  FCObjImage::ConvertTo32Bit()
{
	this->__ConvertToTrueColor (32) ;
}
//===================================================================
void  FCObjImage::__ConvertToTrueColor (int iColor)
{
	if (!IsValidImage() || (ColorBits() == iColor))
		return ;

	// 保存原始数据
	FCObjImage      OldPic (*this) ;
	if (!Create (OldPic.Width(), OldPic.Height(), iColor))
        return ;

	// 获取调色板
	RGBQUAD     * prgb = NULL ;
	if (OldPic.ColorBits() <= 8)
	{
		prgb = new RGBQUAD [1 << OldPic.ColorBits()] ;
		OldPic.GetColorTable (0, 1 << OldPic.ColorBits(), prgb) ;
	}

	// 颜色转换
	int			nNewSpan = this->ColorBits() / 8, // 3 or 4
				nOldSpan = OldPic.ColorBits() / 8 ;
	for (int y = 0 ; y < Height() ; y++)
	{
		BYTE	* pNew = this->GetBits (y),
				* pOld = OldPic.GetBits (y) ;
        for (int x=0 ; x < Width() ; x++, pNew += nNewSpan, pOld += nOldSpan)
			switch (OldPic.ColorBits())
			{
				case 1 :
				case 4 :
                case 8 : // 1,4,8 ==> 24,32
                    FCColor::CopyPixel (pNew, &prgb[(BYTE)OldPic.GetPixelData(x,y)], 3) ;
                    break ;
				case 16 : // 16 ==> 24,32
                    {
                        RGBQUAD     crTrans ;
                        if (OldPic.m_dwBitFields[1] == MASK16_GREEN_555)
                            crTrans = FCColor::Split16Bit_555 (*(WORD*)pOld) ;
                        else
                            if (OldPic.m_dwBitFields[1] == MASK16_GREEN_565)
                                crTrans = FCColor::Split16Bit_565 (*(WORD*)pOld) ;
                        FCColor::CopyPixel (pNew, &crTrans, 3) ;
                    }
                    break ;
				case 24 :
				case 32 : // 24,32 ==> 32,24
                    FCColor::CopyPixel (pNew, pOld, 3) ;
                    break ;
			}
	}
	if (prgb != NULL)
		delete[] prgb ;
	if (iColor == 32)
		SetAlphaChannelValue (0xFF) ; // 设置32位色alpha通道值
	this->SetGraphObjPos (OldPic.GetGraphObjPos()) ;
}
//===================================================================
// copy from CXImage 5.99
void  FCObjImage::ConvertQuantize (int nColorBits)
{
	if (!IsValidImage() || (ColorBits() == nColorBits))
		return ;
	this->ConvertTo24Bit() ; // 24bit 方便处理

	FCObjImage		imgOld(*this) ; // 保存原图
	if (!Create (imgOld.Width(), imgOld.Height(), nColorBits))
		return ;

	// 得到量化调色板
	BYTE		* pOld = new BYTE [sizeof(BITMAPINFOHEADER)+imgOld.GetPitch()*imgOld.Height()] ;
	const int	nNum = 1 << nColorBits ; // 2, 16, 256
	RGBQUAD		* pPalette = new RGBQUAD[nNum] ;
	::CopyMemory (pOld, &imgOld.m_DibInfo, sizeof(BITMAPINFOHEADER)) ;
	CopyMemory (pOld+sizeof(BITMAPINFOHEADER), imgOld.GetMemStart(), imgOld.GetPitch()*imgOld.Height()) ;
	switch (nColorBits)
	{
		case 1 : // 24 ==> 1
			{
				SetColorTable (0, 1, &FCColor::crBlack) ;
				SetColorTable (1, 1, &FCColor::crWhite) ;
				pPalette[0] = FCColor::crBlack ; pPalette[1] = FCColor::crWhite ;
			}
			break ;
		case 4 : // 24 ==> 4
		case 8 : // 24 ==> 8
			{
				CQuantizer		quan (nNum, 7) ;
				quan.ProcessImage (pOld) ;
				quan.SetColorTable (pPalette) ;
				this->SetColorTable (0, nNum, pPalette) ;
			}
			break ;
	}
	delete[] pOld ;

    // 计算像素
	for (int y=0 ; y < Height() ; y++)
		for (int x=0 ; x < Width() ; x++)
		{
            RGBQUAD     c ;
            FCColor::CopyPixel (&c, imgOld.GetBits(x,y), 3) ;

			int     nindex = FCColor::GetNearestPaletteIndex (pPalette, nNum, c) ;
			SetPixelData (x, y, nindex) ;

			RGBQUAD ce = pPalette[nindex] ;
			long    er = (long)c.rgbRed - (long)ce.rgbRed;
			long    eg = (long)c.rgbGreen - (long)ce.rgbGreen;
			long    eb = (long)c.rgbBlue - (long)ce.rgbBlue;

            FCColor::CopyPixel (&c, imgOld.GetBits(FClamp(x+1,0,Width()-1),y), 3) ;
			c.rgbRed = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbRed + ((er*7)/16))));
			c.rgbGreen = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbGreen + ((eg*7)/16))));
			c.rgbBlue = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbBlue + ((eb*7)/16))));
            FCColor::CopyPixel (imgOld.GetBits(FClamp(x+1,0,Width()-1),y), &c,3) ;
			int     coeff ;
			for(int i=-1 ; i < 2 ; i++)
			{
				switch(i)
				{
					case -1 : coeff=2; break;
					case 0 : coeff=4; break;
					case 1 : coeff=1; break;
				}

                FCColor::CopyPixel (&c, imgOld.GetBits(FClamp(x+1,0,Width()-1),FClamp(y+1,0,Height()-1)), 3) ;
				c.rgbRed = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbRed + ((er * coeff)/16))));
				c.rgbGreen = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbGreen + ((eg * coeff)/16))));
				c.rgbBlue = (BYTE)FMin(255L,FMax(0L,(long)(c.rgbBlue + ((eb * coeff)/16))));
                FCColor::CopyPixel (imgOld.GetBits(FClamp(x+1,0,Width()-1),FClamp(y+1,0,Height()-1)), &c,3) ;
			}
		}
	delete[] pPalette ;
}
//===================================================================
void  FCObjImage::GetAlphaChannel (FCObjImage * imgAlpha) const
{
	if ((imgAlpha == NULL) || !IsValidImage() || (ColorBits() != 32))
    {
		FAssert(FALSE) ; return ;
    }

	// 创建返回位图
	imgAlpha->Create (Width(), Height(), 8) ;

	for (int y=0 ; y < Height() ; y++)
	{
        BYTE    * pImg = GetBits(y),
                * pAlpha = imgAlpha->GetBits(y) ;
		for (int x=0 ; x < Width() ; x++, pImg+=4, pAlpha++)
			*pAlpha = pImg[3] ;
	}
}
//===================================================================
void  FCObjImage::AppendAlphaChannel (const FCObjImage & alpha)
{
	if (IsValidImage() && alpha.IsValidImage() &&
        (ColorBits() == 32) && (alpha.ColorBits() == 8) &&
        (Width() == alpha.Width()) && (Height() == alpha.Height()))
    {
		for (int y=0 ; y < Height() ; y++)
		{
			const BYTE  * pAlpha = alpha.GetBits (y) ;
			RGBQUAD     * pDest = (RGBQUAD *)this->GetBits (y) ;
			for (int x=0 ; x < Width () ; x++){
				pDest[x].rgbReserved = pAlpha[x];
				if(pDest[x].rgbReserved!=0){
					continue;
				}
			}
		}
    }
}
//===================================================================
void  FCObjImage::CopyAlphaChannel (const FCObjImage & SrcImg32)
{
    if (IsValidImage() && SrcImg32.IsValidImage() &&
        (ColorBits() == 32) && (SrcImg32.ColorBits() == 32) &&
        (Width() == SrcImg32.Width()) && (Height() == SrcImg32.Height()))
    {
        for (int y=0 ; y < Height() ; y++)
		{
            RGBQUAD     * pAlpha = (RGBQUAD *)SrcImg32.GetBits (y),
                        * pDest = (RGBQUAD *)this->GetBits (y) ;
			for (int x=0 ; x < Width() ; x++)
				pDest[x].rgbReserved = pAlpha[x].rgbReserved ;
		}
    }
}
//===================================================================
void  FCObjImage::SetAlphaChannelValue (int nValue)
{
    FCPixelSetAlphaValue    CmdSetAlpha(nValue) ;
    SinglePixelProcessProc (CmdSetAlpha) ;
}
//===================================================================
bool  FCObjImage::IsGrayPalette () const
{
	bool	bRet = false ;
	if (IsValidImage() && (ColorBits() <= 8))
	{
		const int   nNum = 1 << ColorBits() ;
		RGBQUAD     * pal = new RGBQUAD[nNum] ;
		GetColorTable (0, nNum, pal) ;
		bRet = true ;
		for (int i=0 ; (i < nNum) && bRet ; i++)
			if ((pal[i].rgbBlue == pal[i].rgbGreen) && (pal[i].rgbBlue == pal[i].rgbRed))
				continue ;
			else
				bRet = false ;
		delete[] pal ;
	}
	return bRet ;
}
//===================================================================
void  FCObjImage::fooCopyPalette (FCObjImage & DibDest, const FCObjImage & DibSrc)
{
	if (DibDest.IsValidImage() && (DibDest.ColorBits() <= 8) && (DibDest.ColorBits() == DibSrc.ColorBits()))
	{
		const int   nNum = 1 << DibSrc.ColorBits() ;
		RGBQUAD     * pPalette = new RGBQUAD[nNum] ;
		DibSrc.GetColorTable (0, nNum, pPalette) ;
		DibDest.SetColorTable (0, nNum, pPalette) ;
		delete[] pPalette ;
	}
}

//===================================================================
BOOL  FCObjImage::GetColorTable (int iFirstIndex, int iNumber, RGBQUAD * pColors) const
{
    if (!IsValidImage() || (ColorBits() > 8) || (pColors == NULL) || (m_pPalette == NULL))
    {
        FAssert(FALSE) ; return FALSE ;
    }
    int     nColorNum = 1 << ColorBits() ;
    for (int i=0 ; i < iNumber ; i++)
    {
        int    nIndex = iFirstIndex + i ;
        if (nIndex < nColorNum)
            pColors[i] = m_pPalette[nIndex] ;
    }
    return TRUE ;
}
//===================================================================
BOOL  FCObjImage::SetColorTable (int iFirstIndex, int iNumber, RGBQUAD * pColors)
{
    if (!IsValidImage() || (ColorBits() > 8) || (pColors == NULL) || (m_pPalette == NULL))
    {
        FAssert(FALSE) ; return FALSE ;
    }
    int     nColorNum = 1 << ColorBits() ;
    for (int i=0 ; i < iNumber ; i++)
    {
        int    nIndex = iFirstIndex + i ;
        if (nIndex < nColorNum)
            m_pPalette[nIndex] = pColors[i] ;
    }
    return TRUE ;
}
//===================================================================
void  FCObjImage::SetGrayPalette ()
{
    if (IsValidImage() && (ColorBits() <= 8))
    {
        // 设置调色板
        int			nNum = 1 << ColorBits() ;
        int			nSpan = 255 / (nNum - 1) ;
        RGBQUAD		* palette = new RGBQUAD[nNum] ;
        for (int i=0 ; i < nNum ; i++)
        {
            palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = i * nSpan ;
        }
        SetColorTable (0, nNum, palette) ;
        delete[] palette ;
    }
}
//===================================================================
BOOL  FCObjImage::GetSubBlock (FCObjImage * SubImg, const RECT & rcBlock) const
{
	if (!IsValidImage() || (SubImg == NULL) || (SubImg == (FCObjImage*)this) || (ColorBits() < 8))
		return FALSE ;

	RECT    rcImg = {0, 0, Width(), Height()}, rcDest ;
	if (::IntersectRect (&rcDest, &rcImg, &rcBlock) == 0)
		return FALSE ; // 没有相交区域
	
	if (!SubImg->Create (RECTWIDTH(rcDest), RECTHEIGHT(rcDest), ColorBits()))
		return FALSE ;

	// copy pixel
	DWORD    dwCopyLength = SubImg->Width() * ColorBits() / 8 ;
	for (int i=0 ; i < SubImg->Height() ; i++)
		CopyMemory (SubImg->GetBits(i), GetBits(rcDest.left, rcDest.top + i), dwCopyLength) ;

	// copy palette
	if (ColorBits() <= 8)
		FCObjImage::fooCopyPalette (*SubImg, *this) ;

	// set position
	SubImg->SetGraphObjPos (rcDest.left, rcDest.top) ;	
	//m_paper.GetSubBlock((SubImg->GetPaper()),rcDest);
	return TRUE ;
}
//===================================================================
BOOL  FCObjImage::CoverBlock (const FCObjImage & Img, int x, int y)
{
	
	if (!IsValidImage() || !Img.IsValidImage() || (ColorBits() != Img.ColorBits()) || (ColorBits() < 8))
    {
        FAssert (FALSE) ;
		return FALSE ;
    }

	RECT    rcNow = {0, 0, Width(), Height()}, rcDest,
            rcSub = {x, y, x+Img.Width(), y+Img.Height()} ;
	if (::IntersectRect (&rcDest, &rcNow, &rcSub) == 0)
		return FALSE ; // 没有相交区域

	DWORD    dwSubPitch = RECTWIDTH(rcDest) * Img.ColorBits() / 8 ;
	for (int cy=rcDest.top ; cy < rcDest.bottom ; cy++) // copy
	{
		BYTE	* pDest = this->GetBits (rcDest.left, cy),
				* pSrc = Img.GetBits (rcDest.left-x, cy-y) ; // 边界情况
		CopyMemory (pDest, pSrc, dwSubPitch) ;
	}
	//RECT rcBlock={x,y,x+Img.Width(),y+Img.Height()};
	////m_paper.CoverBlock(Img.GetPaper(),rcSub);
	return TRUE ;
}
//===================================================================
BOOL  FCObjImage::TileBlock (const FCObjImage & Img, int x, int y)
{
    int    nYStart = y ;
    while (nYStart < Height())
    {
        int    nXStart = x ;
        while (nXStart < Width())
        {
            CoverBlock (Img, nXStart, nYStart) ; // 安全工作全部交给此函数
            nXStart += Img.Width() ;
        }
        nYStart += Img.Height() ;
    }
    return TRUE ;
}
//===================================================================
void  FCObjImage::CombineAlphaImg (const FCObjImage & Img32, int x, int y, int nAlphaPercent)
{
	if (!IsValidImage() || (Img32.ColorBits() != 32) || (ColorBits() != 32))
		return ;

	RECT	dib1 = {0, 0, Width(), Height()}, rcDest,
			dib2 = {x, y, x+Img32.Width(), y+Img32.Height()} ;
	if (::IntersectRect (&rcDest, &dib1, &dib2) == 0)
		return ;

	nAlphaPercent = FClamp (nAlphaPercent, 0, 100) ;
	for (int cy=rcDest.top ; cy < rcDest.bottom ; cy++)
	{
		RGBQUAD		* pDest = (RGBQUAD*)this->GetBits (rcDest.left, cy),
					* pSrc = (RGBQUAD*)Img32.GetBits (rcDest.left-x, cy-y) ; // 边界情况
		for (int cx=rcDest.left ; cx < rcDest.right ; cx++, pDest++, pSrc++)
			if (nAlphaPercent == 100)
				FCColor::CombineAlphaPixel(pDest, *pDest, pDest->rgbReserved, *pSrc, pSrc->rgbReserved) ;
			else
				FCColor::CombineAlphaPixel(pDest, *pDest, pDest->rgbReserved, *pSrc, pSrc->rgbReserved*nAlphaPercent/100) ;
	}
}
//===================================================================
// alpha混和是图像处理的心脏，它的效率和安全性关系着整个软件
// 因此我才这么麻烦的处理之
// rcSrc一定要在MaskImg32内部，rcDest可以相交
void  FCObjImage::AlphaBlend (const FCObjImage & MaskImg32, const RECT &rcDest, const RECT &rcSrc, int nAlphaPercent)
{
	// 安全检查
	if (!IsValidImage() || (ColorBits() < 24) || !MaskImg32.IsValidImage() || (MaskImg32.ColorBits() != 32))
    {
		FAssert(FALSE) ; return ;
    }
	const RECT     rcMaskImg = {0, 0, MaskImg32.Width(), MaskImg32.Height()} ;
	if (!IsRectInRect (rcMaskImg, rcSrc)) // rcSrc一定要在MaskImg32内部
    {
        FAssert(FALSE) ; return ;
    }

	nAlphaPercent = FClamp (nAlphaPercent, 0, 100) ;
	if ((RECTWIDTH(rcDest) == RECTWIDTH(rcSrc)) && (RECTHEIGHT(rcDest) == RECTHEIGHT(rcSrc)))
	{
		RECT		rcCover, rcImg={0, 0, Width(), Height()} ;
		if (::IntersectRect(&rcCover, &rcDest, &rcImg) == 0)
			return ;

		const int   nStartX = rcSrc.left + rcCover.left - rcDest.left,
                    nSpan = ColorBits() / 8 ; // 3 or 4
		if (nAlphaPercent == 100)
		{
			for (int y=rcCover.top ; y < rcCover.bottom ; y++)
			{
				BYTE		* pDest = this->GetBits (rcCover.left, y) ;
				RGBQUAD		* pSrc = (RGBQUAD*)MaskImg32.GetBits (nStartX, rcSrc.top + y - rcDest.top) ; // 边界情况
				for (int x=rcCover.left ; x < rcCover.right ; x++, pDest += nSpan, pSrc++)
				{
					FCColor::AlphaBlendPixel (pDest, (BYTE*)pSrc, pSrc->rgbReserved) ;
				}
			}
		}
		else
		{
			for (int y=rcCover.top ; y < rcCover.bottom ; y++)
			{
				BYTE		* pDest = this->GetBits (rcCover.left, y) ;
				RGBQUAD		* pSrc = (RGBQUAD*)MaskImg32.GetBits (nStartX, rcSrc.top + y - rcDest.top) ; // 边界情况
				for (int x=rcCover.left ; x < rcCover.right ; x++, pDest += nSpan, pSrc++)
				{
					int		nNewAlpha = pSrc->rgbReserved * nAlphaPercent / 100 ;
					FCColor::AlphaBlendPixel (pDest, (BYTE*)pSrc, nNewAlpha) ;
				}
			}
		}
	}
	else
	{
		FCObjImage      imgBlock ;
        MaskImg32.GetSubBlock (&imgBlock, rcSrc) ;
        imgBlock.Stretch (RECTWIDTH(rcDest), RECTHEIGHT(rcDest)) ;
		const RECT      rcBlock = {0, 0, imgBlock.Width(), imgBlock.Height()} ;
		this->AlphaBlend (imgBlock, rcDest, rcBlock, nAlphaPercent) ;
	}
}
//===================================================================
// pixel logical operation (>=8 bit)
void  FCObjImage::LogicalBlend (const FCObjImage & MaskImg, LOGICAL_OP LogOP, int x, int y)
{
	if (!IsValidImage() || !MaskImg.IsValidImage() || (MaskImg.ColorBits() < 8) || (ColorBits() != MaskImg.ColorBits()))
    {
        FAssert(FALSE) ;
		return ;
    }

	RECT	rcSrc1 = {0, 0, Width(), Height()}, rcDest,
			rcSrc2 = {x, y, x+MaskImg.Width(), y+MaskImg.Height()} ;
	if (::IntersectRect (&rcDest, &rcSrc1, &rcSrc2) == 0)
		return ; // 没有相交区域

	int		nSpan = ColorBits() / 8, // 1,2,3,4
			nLoop = FMin (3, nSpan) ; // 不破坏alpha通道
	for (int cy=rcDest.top ; cy < rcDest.bottom ; cy++)
	{
		BYTE	* pDest = this->GetBits (rcDest.left, cy),
				* pSrc = MaskImg.GetBits (rcDest.left-x, cy-y) ; // 要考虑边界情况
		for (int cx=rcDest.left ; cx < rcDest.right ; cx++, pDest+=nSpan, pSrc+=nSpan)
			for (int j=0 ; j < nLoop ; j++)
			{
				switch (LogOP)
				{
					case LOGI_OR :
						pDest[j] |= pSrc[j] ;
						break ;
					case LOGI_AND :
						pDest[j] &= pSrc[j] ;
						break ;
					case LOGI_XOR :
						pDest[j] ^= pSrc[j] ;
						break ;
					case LOGI_SEL_ADD :
                        if (pSrc[j] == 0xFF)
                            pDest[j] = 0xFF ;
                        break ;
					case LOGI_SEL_SUB :
                        if (pSrc[j] == 0xFF)
                            pDest[j] = 0 ;
                        break ;
				}
			}
	}
}
//==============================================================================
int  FCObjImage::Serialize (BOOL bSave, BYTE * pSave)
{
	// 不支持调色板保存
	BYTE	* pBak = pSave ;
	if (bSave) // 存
	{
        CopyMemory (pSave, &m_DibInfo, sizeof(m_DibInfo)) ;
        CopyMemory (pSave + sizeof(m_DibInfo), m_dwBitFields, 12) ;
        pSave += sizeof(m_DibInfo) + 12 ;

		DWORD    dwWrite = GetPitch() * Height() ;
		CopyMemory (pSave, GetMemStart(), dwWrite) ;
		pSave += dwWrite ;
	}
	else // 取
	{
        this->Create ((BITMAPINFOHEADER*)pSave) ;
		pSave += sizeof(m_DibInfo) + 12 ;

		DWORD    dwWrite = GetPitch() * Height() ;
		CopyMemory (GetMemStart(), pSave, dwWrite) ;
		pSave += dwWrite ;
	}
	pSave += FCObjGraph::Serialize (bSave, pSave) ;
	return pSave - pBak ;
}
//==============================================================================
// 单象素处理过程
void  FCObjImage::SinglePixelProcessProc (FCSinglePixelProcessBase & PixelProcessor, FCObjProgress * progress)
{
    // some object need to create image.
    PixelProcessor.OnCreateImage (this) ;

	if (!PixelProcessor.ValidateColorBits (this))
		return ;

	// 计算处理区域
	RECT		rcImg = {0,0,Width(),Height()}, rcBlock, rcDest ;
	if (PixelProcessor.GetProcessRect() == NULL)
		::CopyRect (&rcBlock, &rcImg) ;
	else
		::CopyRect (&rcBlock, PixelProcessor.GetProcessRect()) ;
	if (::IntersectRect (&rcDest, &rcImg, &rcBlock) == 0)
		return ; // 处理区域为空

	// 处理前准备工作
	int     nSpan = ColorBits() / 8 ; // 每象素字节数3, 4
	PixelProcessor.OnEnterProcess (this, rcDest) ;

	// 遍历处理区域像素
    if (progress != NULL)
        progress->ResetProgress() ; // reset to 0
	for (int y=rcDest.top ; y < rcDest.bottom ; y++)
	{
		BYTE	* pPixel = GetBits (rcDest.left, y) ;
		for (int x=rcDest.left ; x < rcDest.right ; x++, pPixel += nSpan)
		{
			PixelProcessor.ProcessPixel (this, x, y, pPixel) ;
		}
		if (progress != NULL)
			progress->SetProgress (y * 100 / Height()) ;
	}

	// 收尾工作
	PixelProcessor.OnLeaveProcess (this) ;
}
//==============================================================================
// 缩放/stretch (>=8 bit)
void  FCObjImage::Stretch (int nNewWidth, int nNewHeight)
{
	if (!IsValidImage() || (nNewWidth <= 0) || (nNewHeight <= 0) || (ColorBits() < 8))
    {
		FAssert(FALSE) ; return ;
    }
	if ((nNewWidth == Width()) && (nNewHeight == Height()))
		return ;

/*	if (m_bRatio) // 成比例缩放
	{
		double		duScale = min (m_nNewWidth/(double)img.Width(), m_nNewHeight/(double)img.Height()) ;
		nWidth = duScale * img.Width() ;
		nHeight = duScale * img.Height() ;
	}*/

	FCObjImage		imgOld(*this) ; // 保存原图
	if (!Create (nNewWidth, nNewHeight, imgOld.ColorBits()))
    {
        FAssert(FALSE) ; return ;
    }

	if (ColorBits() <= 8)
		FCObjImage::fooCopyPalette (*this, imgOld) ; // 复制调色板

    double    fStepY = imgOld.Height() / (double)Height(),
              fStepX = imgOld.Width() / (double)Width() ;
    int       * pTabX = new int[Width()],
              nPixSpan = ColorBits() / 8 ;

    // 初始化索引表
    for (int xx=0 ; xx < Width() ; xx++)
    {
        pTabX[xx] = (int)(xx*fStepX) ;
        FAssert (pTabX[xx] < imgOld.Width()) ;
    }
    for (int mm=0 ; mm < (Width() - 1) ; mm++)
        pTabX[mm] = (pTabX[mm+1] - pTabX[mm]) * nPixSpan ;
    // pTabX里现在存放X轴像素字节差

    for (int y=0 ; y < Height() ; y++)
    {
        BYTE     * pPixel = GetBits (y),
                 * pOld = imgOld.GetBits ((int)(y*fStepY)) ;
        for (int x=0 ; x < Width() ; x++)
        {
            FCColor::CopyPixel (pPixel, pOld, nPixSpan) ;
            pOld += pTabX[x] ;
            pPixel += nPixSpan ;
        }
    }
    delete[] pTabX ;
}
//==============================================================================
FCObjImage& FCObjImage::GetBGImage()
{
	if(!BACKGROUND.IsValidImage()){
		BACKGROUND.Create(16,16,32);
		BYTE white=255;
		memset(BACKGROUND.GetMemStart(),white,
			     BACKGROUND.GetPitch()*BACKGROUND.Height());
	}
	return BACKGROUND;
}

//===================================================================

BOOL  FCObjImage::Load(PCTSTR szFileName)
{	
	IplImage* pImgSrc=cvLoadImage(szFileName);
	this->Create(pImgSrc->width,pImgSrc->height,pImgSrc->nChannels*8);
	BYTE* pBuffer=((uchar*)(pImgSrc->imageData));
	for(int i=0;i<pImgSrc->height;++i){		
		memcpy(m_ppLine[i],pBuffer,GetPitch());
		pBuffer+=pImgSrc->widthStep;
	}
	cvReleaseImage(&pImgSrc);
	return TRUE;
}

BOOL FCObjImage::Save (PCSTR szFileName, int nFlag)
{
	if(!IsValidImage()) return FALSE;
	IplImage* pIplImg=cvCreateImage(cvSize(Width(),Height()),IPL_DEPTH_8U,ColorBits()/8);
	BYTE* pBuffer=((uchar*)(pIplImg->imageData));
	for(int i=0;i<Height();++i){	
		memcpy(pBuffer,m_ppLine[i],pIplImg->widthStep);
		pBuffer+=pIplImg->widthStep;
	}
	cvSaveImage(szFileName,pIplImg);
	cvReleaseImage(&pIplImg);
}

//==============================================================================
// 旋转/rotate (32 bit)
void  FCObjImage::Rotate_Smooth (int nAngle)
{
	if (!IsValidImage() || (ColorBits() != 32))
	{
		FAssert(FALSE) ; return ;
	}

	aggRotateImage_AA_32Bit (*this, nAngle) ;
}
// 加边框
// bCopyEdge：增加边框后是否复制边缘象素填充扩充部分
void  FCObjImage::ExpandFrame (BOOL bCopyEdge, int iLeft, int iTop, int iRight, int iBottom)
{
	if ((ColorBits() < 8) || (iLeft < 0) || (iTop < 0) || (iRight < 0) || (iBottom < 0))
		return ;

	FCObjImage		imgOld(*this) ; // 保存原图
	if (!Create (imgOld.Width()+iLeft+iRight, imgOld.Height()+iTop+iBottom, imgOld.ColorBits()))
		return ;

	// 拷贝源图区域
	const DWORD     dwLineBytes = imgOld.Width() * imgOld.ColorBits() / 8 ;
	for (int i=0 ; i < imgOld.Height() ; i++)
		CopyMemory (GetBits (iLeft, i + iTop), imgOld.GetBits(i), dwLineBytes) ;

	// 复制调色板
	if (ColorBits() <= 8)
		FCObjImage::fooCopyPalette (*this, imgOld) ;

	// 是否复制边缘
	if (!bCopyEdge)
		return ;

	//	复制四角
	RECT	rcUpL = {0, 0, iTop, iLeft},
		rcUpR = {Width()-iRight, 0, Width(), iTop},
		rcDownL = {0, Height()-iBottom, iLeft, Height()},
		rcDownR = {Width()-iRight, Height()-iBottom, Width(), Height()} ;
	__fooFillImageRect (*this, rcUpL, imgOld.GetPixelData (0, 0)) ;
	__fooFillImageRect (*this, rcUpR, imgOld.GetPixelData (imgOld.Width()-1, 0)) ;
	__fooFillImageRect (*this, rcDownL, imgOld.GetPixelData (0, imgOld.Height()-1)) ;
	__fooFillImageRect (*this, rcDownR, imgOld.GetPixelData (imgOld.Width()-1, imgOld.Height()-1)) ;

	//	复制四边
	DWORD		dwPitch = GetPitch() ;
	int			ct, nSpan = ColorBits() / 8 ;
	BYTE		* pSrc, * pDest ;
	//	上
	if (IsInside (iLeft, iTop))
		pSrc = GetBits (iLeft, iTop) ;
	pDest = pSrc + dwPitch ;
	for (ct = 0 ; ct < iTop ; ct++, pDest += dwPitch)
		CopyMemory (pDest, pSrc, dwLineBytes) ;
	//	下
	if (IsInside (iLeft, imgOld.Height() + iTop))
		pDest = GetBits (iLeft, imgOld.Height() + iTop) ;
	pSrc = pDest + dwPitch ;
	for (ct = 0 ; ct < iBottom ; ct++, pDest -= dwPitch)
		CopyMemory (pDest, pSrc, dwLineBytes) ;
	//	左
	if (IsInside (iLeft, iTop))
		pSrc = GetBits (iLeft, iTop) ;
	pDest = GetBits (0, iTop) ;
	for (ct = 0 ; ct < imgOld.Height() ; ct++, pDest -= dwPitch, pSrc -= dwPitch)
	{
		BYTE	* pTemp = pDest ;
		for (int i=0 ; i < iLeft ; i++, pTemp += nSpan)
			FCColor::CopyPixel (pTemp, pSrc, nSpan) ;
	}
	//	右
	if (IsInside (iLeft + imgOld.Width(), iTop))
		pDest = GetBits (iLeft + imgOld.Width(), iTop) ;
	pSrc = pDest - nSpan ;
	for (ct = 0 ; ct < imgOld.Height() ; ct++, pDest -= dwPitch, pSrc -= dwPitch)
	{
		BYTE	* pTemp = pDest ;
		for (int i=0 ; i < iRight ; i++, pTemp += nSpan)
			FCColor::CopyPixel (pTemp, pSrc, nSpan) ;
	}
}
//==============================================================================
// 擦除边框/erase frame (>=8 bit)
void  FCObjImage::EraseFrame (int iLeft, int iTop, int iRight, int iBottom)
{
	if ((ColorBits() < 8) || (iLeft < 0) || (iTop < 0) || (iRight < 0) || (iBottom < 0) ||
		(iLeft + iRight >= Width()) || (iTop + iBottom >= Height()))
		return ;

	FCObjImage		Old(*this) ; // 保存原图
	if (Create (Old.Width()-iLeft-iRight, Old.Height()-iTop-iBottom, Old.ColorBits()))
	{
		// 拷贝源图区域
		DWORD		dwPitch = Width() * ColorBits() / 8 ;
		for (int y=0 ; y < Height() ; y++)
			CopyMemory (GetBits (y), Old.GetBits (iLeft, iTop + y), dwPitch) ;

		if (ColorBits() <= 8)
			FCObjImage::fooCopyPalette (*this, Old) ;
	}
}
//==============================================================================
// 缩放/stretch (>=24 bit)
void  FCObjImage::Stretch_Smooth (int nNewWidth, int nNewHeight)
{
	if (!IsValidImage() || (nNewWidth <= 1) || (nNewHeight <= 1) || (ColorBits() < 24))
	{
		FAssert(FALSE) ; return ;
	}
	if ((nNewWidth == Width()) && (nNewHeight == Height()))
		return ;

	switch (ColorBits())
	{
	case 24 : aggStretchImage_AA_24Bit (*this, nNewWidth, nNewHeight) ; break ;
	case 32 : aggStretchImage_AA_32Bit (*this, nNewWidth, nNewHeight) ; break ;
	default : FAssert(FALSE) ; return ;
	}
}

void   FCObjImage::GetPixel(int x,int y, BYTE &r,BYTE &g,BYTE &b)
{
	DWORD dwpixel =0;
	dwpixel = GetPixelData(x,y);
	b= (BYTE )dwpixel;
	g = (BYTE)(dwpixel>>8);
	r = (BYTE)(dwpixel>>16);

}
//////////////////////////////////////////////////////////////////////////
//
void   FCObjImage::SetPixel(int x, int y ,BYTE r,BYTE g,BYTE b)
{
	DWORD	dwrgb = 0;
	if(!IsInside(x,y))
		return;
	if(ColorBits()==32){//保持透明度分量不变。
			dwrgb = GetPixelData(x,y) ;
			dwrgb &= 0xFF000000;
	}

	dwrgb |= b;
	dwrgb |= (g<<8);
	dwrgb |= (r<<16);
	SetPixelData(x,y,dwrgb);
}

void   FCObjImage::SetGrayPixel(int x, int y ,BYTE gray)
{
	DWORD	dwrgb = 0;
	if(!IsInside(x,y))
		return;
	if(ColorBits()==32){//保持透明度分量不变。
		dwrgb = GetPixelData(x,y) ;
		dwrgb &= 0xFF000000;
	}

	dwrgb |= gray;
	SetPixelData(x,y,dwrgb);
}
void FCObjImage::AntiAliased_Zoom(int w)
{
	if (!IsValidImage() || (w<=1) || (ColorBits()!=8))
	{
		return ;
	}
	FCObjImage		imgOld(*this) ; // 保存原图
	if (!Create (Width()/w, Height()/w, imgOld.ColorBits()))
	{
		FAssert(FALSE) ; return ;
	}

	if (ColorBits() <= 8)
		FCObjImage::fooCopyPalette (*this, imgOld) ; // 复制调色板

	double    fStepY = w,
		fStepX = w ;
	int       * pTabX = new int[Width()];

	// 初始化索引表
	for (int xx=0 ; xx < Width() ; xx++)
	{
		pTabX[xx] = (int)(xx*fStepX) ;
		FAssert (pTabX[xx] < imgOld.Width()) ;
	}
	for (int mm=0 ; mm < (Width() - 1) ; mm++)
		pTabX[mm] = (pTabX[mm+1] - pTabX[mm]);
	// pTabX里现在存放X轴像素字节差

	int dwPitch=imgOld.GetPitch();
	unsigned int aver=w*w;
	for (int y=0 ; y < Height() ; y++)
	{
		BYTE     * pPixel = GetBits (y),
			* pOld = imgOld.GetBits ((int)(y*fStepY)) ;
		for (int x=0 ; x < Width() ; x++)
		{
			unsigned int dwSum=0;
			BYTE* pTmp=pOld;
			for (int sY=0 ; sY < w ; sY++, pTmp -= dwPitch)
				for (int sX=0 ; sX < w ; sX++)
				{
					if (pTmp[sX] != 0)  dwSum += pTmp[sX];
				}
			*pPixel=dwSum/aver;
			pOld += pTabX[x] ;
			pPixel += 1;
		}
	}
	delete[] pTabX ;
}
BOOL FCObjImage::Load(IplImage* pImgSrc){
	if (pImgSrc==NULL)
		return FALSE;
	this->Create(pImgSrc->width,pImgSrc->height,pImgSrc->nChannels*8);
	BYTE* pBuffer=((uchar*)(pImgSrc->imageData));
	for(int i=pImgSrc->height-1;i>=0;--i){
		if(pImgSrc->origin==1)
			memcpy(m_ppLine[i],pBuffer,GetPitch());
		else{
			memcpy(m_ppLine[pImgSrc->height-1-i],pBuffer,GetPitch());
		}
		pBuffer+=pImgSrc->widthStep;
	}
	return TRUE;
}
IplImage* FCObjImage::ToIplImage()
{
	if(IsValidImage()){
		IplImage* pIplImg=cvCreateImage(cvSize(Width(),Height()),IPL_DEPTH_8U,ColorBits()/8);
		pIplImg->origin=1;
		BYTE* pBuffer=((uchar*)(pIplImg->imageData));
		for(int i=Height()-1;i>=0;--i){	
			memcpy(pBuffer,m_ppLine[i],pIplImg->widthStep);
			pBuffer+=pIplImg->widthStep;
		}
		return pIplImg;
	}
	return NULL;
}