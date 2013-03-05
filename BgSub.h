#pragma once

#include <cxcore.h>
#include "cv_yuv_codebook.h"




class CBgSub
{
public:
	CBgSub(void);
	~CBgSub(void);
public:
	void CodebookBgSub(){}
private:
	//浮点性三通道图像
	IplImage *m_IavgF,*m_IdiffF, *m_IprevF, *m_IhiF, *m_IlowF;
	IplImage *m_Iscratch, *m_Isscratch2;
	//浮点性两通道图像
	IplImage *m_Igray1,*m_Igray2, *m_Igray3;
	IplImage *m_Ilow1, *m_Ilow2,  *m_Ilow3;
	IplImage *m_Ihi1,  *m_Ihi2,   *m_Ihi3;

	IplImage* m_Imaskt;

	float m_Icount;

	int   m_first;	
public:
	IplImage* PrepareImage(IplImage* pImgSrc);
//============平均背景法===================================
	void AllocateImages(CvSize sz);
	void ReleaseImages();
	void AccumulateBackground(IplImage *I);
	void CreateModelsfromStats();
	void LoadMeanModel();
	void SetHighThreshold(float scale);
	void SetLowThreshold(float scale);
	IplImage* BackgroundDiffOfMean(IplImage *I);
	BOOL IsMeanModelReady();
//==========================================================
private:
	codeBook *m_cB;  //This will be our linear model of the image, a vector of lengh = height*width
	int m_maxMod[CHANNELS]; //Add these (possibly negative) number onto max level when code_element determining if new pixel is foreground
	int m_minMod[CHANNELS]; //Subract these (possible negative) number from min level code_element when determining if pixel is foreground
	unsigned m_cbBounds[CHANNELS]; //Code Book bounds for learning
	long m_imageLen;
	bool m_ch[CHANNELS];
public:
	void InitCodebook();
	void LearningCodeBookModel(IplImage* pI);
	void EndCodeBookModelLearning();
	IplImage* BackgroundDiffOfCodebook(IplImage *I);

public:
	void InitBGStatModel(IplImage* pI,int type=CV_BG_MODEL_FGD);
	void UpdateBGStatModel(IplImage* pI);
	IplImage*  BackgroundDiffOfStatModel(IplImage* pI);
private:
	CvBGCodeBookModel*	m_model;
	CvBGStatModel*      m_bg_model;
public:

	IplImage* SxfBGDiffModel(IplImage* pI);
public:
	int  m_rLow;
	int  m_rHigh;
	int  m_bLow;
	int  m_bHigh;
	int  m_gLow;
	int  m_gHigh;
	int  m_bContrast;//采用哪种零度均衡化操作，0：不采用均衡，1：线性均衡，2：直方图均衡
public:
	double  m_vLow,m_vHi,m_sT,m_hT;	
	void	DoShadowRemoval(IplImage* pB,IplImage* pF,IplImage* pMask);

	void    EqualizeHist_1(const IplImage* pSrc,IplImage* pDst);
	void    EqualizeHist_3(const IplImage* pSrc,IplImage* pDst);

	void    EqualizeContrast_1(IplImage* pSrc,IplImage* pDst,int low,int high);
	void    EqualizeContrast_3(IplImage* pSrc);

	IplImage* MakeChessBoard(int imgW,int imgH,int cW,int cH);
};

