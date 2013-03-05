#pragma once


// CDlgRemoveNoise dialog

class CDlgRemoveNoise : public CDialog
{
	DECLARE_DYNAMIC(CDlgRemoveNoise)

public:
	CDlgRemoveNoise(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRemoveNoise();

// Dialog Data
	enum { IDD = IDD_DLGREMOVENOISE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	//ÄÚ¾¶
	CString m_radioone;
	//Íâ¾¶
	CString m_radiotwo;
public:
	int GetMaxRadio(void);
	int GetMinRadio(void);
protected:
	virtual void OnOK();
};
