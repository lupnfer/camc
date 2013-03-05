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
// WatchBar.cpp : implementation file
//

#include "stdafx.h"
#include "camc.h"
#include "WatchBar.h"
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_LIST_1	1

/////////////////////////////////////////////////////////////////////////////
// CWatchBar

CWatchBar::CWatchBar()
{
}

CWatchBar::~CWatchBar()
{
}


BEGIN_MESSAGE_MAP(CWatchBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWatchBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWatchBar message handlers

int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectGrid;
	this->GetClientRect(&rectGrid);
	m_wndWatch.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	m_wndWatch.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);
	m_wndWatch.EnableMarkSortedColumn (TRUE);
	m_wndWatch.SetReadOnly();
	m_wndWatch.InitGridColumn();
	m_wndWatch.LoadState (_T("BasicGrid"));
	m_wndWatch.AdjustLayout ();
	return 0;
}

void CWatchBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	if (CanAdjustLayout ())
	{
		CRect rc;
		GetClientRect(rc);

		m_wndWatch.SetWindowPos(NULL,
				rc.left + 1, rc.top + 1,
				rc.Width() - 2, rc.Height () - 2,
				SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CWatchBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndWatch.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void CWatchBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
	
	m_wndWatch.SetFocus ();
	
}
void CWatchBar::ShowResultSet(ResultSet& rs)
{
	m_wndWatch.RemoveAll();
	ResultSet::iterator it=rs.begin();
	for(;it!=rs.end();++it){
		CBCGPGridRow* pRow =m_wndWatch.CreateRow (m_wndWatch.GetColumnCount ());
		pRow->GetItem (0)->SetValue (it->id);
		_variant_t strName(it->name.c_str());
		CBCGPGridItem* pItem;
		pRow->GetItem (1)->SetValue (strName);
		pRow->GetItem (2)->SetValue (it->angle);
		CString strPos;
		strPos.Format("(%0.2f,%0.2f)",it->position.x,it->position.y);
		_variant_t val=(_variant_t)(LPCTSTR)strPos;
		pRow->GetItem(3)->SetValue(val);

		pRow->GetItem (4)->SetValue (it->diff);
		m_wndWatch.AddRow (pRow);
	}

}