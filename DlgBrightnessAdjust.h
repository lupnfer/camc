#pragma once
#include "ObjImage.h"

// CDlgBrightnessAdjust dialog

class CDlgBrightnessAdjust : public CDialog
{
	DECLARE_DYNAMIC(CDlgBrightnessAdjust)

public:
	CDlgBrightnessAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBrightnessAdjust();

// Dialog Data
	enum { IDD = IDD_BRIGHTNESSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint(); 
private:
	FCObjImage* m_pImgThumb;
public:
	// …Ë÷√Àı¬‘Õº
	void SetThumbImage();
	// ªÒµ√Àı¬‘Õº
	FCObjImage* GetThumbImage(void);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnCancel();
	virtual void OnOK();
};
