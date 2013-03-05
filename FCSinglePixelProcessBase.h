#pragma once

class FCObjImage;
class FColor;
class FCSinglePixelProcessBase
{
public :
	FCSinglePixelProcessBase () ;
	virtual ~FCSinglePixelProcessBase () ;
	virtual BOOL  ValidateColorBits (const FCObjImage * pImg) ;

    virtual void  OnCreateImage (FCObjImage * pImg) {}
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) =0 ;
	virtual void  OnEnterProcess (FCObjImage * pImg, RECT & rcImg) {}
	virtual void  OnLeaveProcess (FCObjImage * pImg) {}
public :
	RECT    * GetProcessRect() const {return m_prcImg ;}
	void    SetProcessRect (const RECT * prcImg) ;
	void    SetBackupImage (const FCObjImage * pImg) ;
	FCObjImage   * GetBackupImage() const {return m_pImgOld ;}
protected :
	RECT        * m_prcImg ; // ���������
	FCObjImage  * m_pImgOld ; // ��Щʱ��Ҫ����ԭͼ�����ڹ���ʱnew��
};

//=============================================================================
// �����ɫ/fill color (>=24 bit)
class FCPixelFillColor : public FCSinglePixelProcessBase
{
public :
	// nAlpha == -1, �����alpha, �������
	FCPixelFillColor (RGBQUAD crFill, int nAlpha = -1, RECT * prcFill = NULL) ;
	virtual BOOL  ValidateColorBits (const FCObjImage * pImg) ;
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	RGBQUAD		m_crFill ;
	int			m_nAlpha ;
	BOOL		m_bIsFillAlpha ;
} ;
//=============================================================================
// ����alphaͨ��ֵ/set alpha channel value (32 bit)
class FCPixelSetAlphaValue : public FCSinglePixelProcessBase
{
public :
	FCPixelSetAlphaValue (int nAlpha) : m_nAlpha(nAlpha) {}
	virtual BOOL  ValidateColorBits (const FCObjImage * pImg) ;
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	int		m_nAlpha ;
} ;
//=============================================================================
// �����ɫ/combine color (32 bit)
class FCPixelCombineColor : public FCSinglePixelProcessBase
{
public :
	FCPixelCombineColor (RGBQUAD crFill) : m_crFill(crFill) {}
	virtual BOOL  ValidateColorBits (const FCObjImage * pImg) ;
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	RGBQUAD		m_crFill ;
} ;
//=============================================================================
// ��Ƭ/negate (>=24 bit)
class FCPixelInvert : public FCSinglePixelProcessBase
{
public :
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
} ;
//=============================================================================
// �һ�/gray scale (>=24 bit)
class FCPixelGrayscale : public FCSinglePixelProcessBase
{
public :
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
} ;
//=============================================================================
// ����RGBֵ/adjust RGB (>=24 bit)
class FCPixelAdjustRGB : public FCSinglePixelProcessBase
{
public :
	FCPixelAdjustRGB (int DeltaR, int DeltaG, int DeltaB) : m_iDeltaR(DeltaR), m_iDeltaG(DeltaG), m_iDeltaB(DeltaB) {}
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	int		m_iDeltaR, m_iDeltaG, m_iDeltaB ;
} ;
