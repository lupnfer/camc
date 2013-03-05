// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once

#include"FCLayerBar.h"

#include "DlgPenBar.h"
#include "DlgLineRelatedBar.h"
#include "DlgMagicBar.h"

#include "OutputView.h"
#include "watchbar.h"
#include "PropertiesViewBar.h"

#define CMDIFrameWnd CBCGPMDIFrameWnd

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // �ؼ���Ƕ���Ա
	CBCGPToolBar      m_wndToolBar;

	COutputViewBar			m_wndOutputView;
	CPropertiesViewBar		m_wndPropertiesBar;
	CWatchBar				m_wndWatchBar;

public:
	CBCGPStatusBar  m_wndStatusBar;

	FCLayerBar      m_fcLayerBar;
	
	void CreateBar();
private:
	
public:
	CString m_path;
	CString m_dbPath;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
protected:
	void CreateLayerBar();
	BOOL CreateDockingBars();
public:
	afx_msg void OnClose();
	afx_msg void OnLayerbar();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnPreviewdlg();
	virtual CDocument* GetActiveDocument();
	afx_msg void OnColordbmanager();
	void    SetStatusBarText(CString str);

public:
	
	
public:
	
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLayerbar(CCmdUI *pCmdUI);
	afx_msg void OnImportParamer();
};


