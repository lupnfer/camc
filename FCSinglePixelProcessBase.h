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
	RECT        * m_prcImg ; // 处理的区域
	FCObjImage  * m_pImgOld ; // 有些时候要保存原图后处理，在构造时new出
};

//=============================================================================
// 填充颜色/fill color (>=24 bit)
class FCPixelFillColor : public FCSinglePixelProcessBase
{
public :
	// nAlpha == -1, 不填充alpha, 否则填充
	FCPixelFillColor (RGBQUAD crFill, int nAlpha = -1, RECT * prcFill = NULL) ;
	virtual BOOL  ValidateColorBits (const FCObjImage * pImg) ;
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	RGBQUAD		m_crFill ;
	int			m_nAlpha ;
	BOOL		m_bIsFillAlpha ;
} ;
//=============================================================================
// 设置alpha通道值/set alpha channel value (32 bit)
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
// 组合颜色/combine color (32 bit)
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
// 反片/negate (>=24 bit)
class FCPixelInvert : public FCSinglePixelProcessBase
{
public :
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
} ;
//=============================================================================
// 灰化/gray scale (>=24 bit)
class FCPixelGrayscale : public FCSinglePixelProcessBase
{
public :
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
} ;
//=============================================================================
// 调节RGB值/adjust RGB (>=24 bit)
class FCPixelAdjustRGB : public FCSinglePixelProcessBase
{
public :
	FCPixelAdjustRGB (int DeltaR, int DeltaG, int DeltaB) : m_iDeltaR(DeltaR), m_iDeltaG(DeltaG), m_iDeltaB(DeltaB) {}
	virtual void  ProcessPixel (FCObjImage * pImg, int x, int y, BYTE * pPixel) ;
protected :
	int		m_iDeltaR, m_iDeltaG, m_iDeltaB ;
} ;
