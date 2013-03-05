//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
#if !defined(AFX_BASICGRIDCTRL_H__56170B45_D9F3_4EA8_A6B5_C53C2308F22C__INCLUDED_)
#define AFX_BASICGRIDCTRL_H__56170B45_D9F3_4EA8_A6B5_C53C2308F22C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicGridCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl window
#include "PartsDatabase.h"

#define WM_SHOW_MESSAGE (WM_USER+200) 

class CBasicGridCtrl : public CBCGPGridCtrl
{
// Construction
public:
	CBasicGridCtrl();

// Attributes
protected:
private:
	
// Operations
public:
	virtual void InitGridColumn();
	void ShowRecordSet();
	void AddRowBack(PartsInfo& ptInfo);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBasicGridCtrl();
	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);
	virtual void OnSelChanged(const CBCGPGridRange&range,BOOL bSelect);


	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICGRIDCTRL_H__56170B45_D9F3_4EA8_A6B5_C53C2308F22C__INCLUDED_)
