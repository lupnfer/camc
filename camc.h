// CAMC.h : CAMC Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// CCAMCApp:
// �йش����ʵ�֣������ CAMC.cpp
//

class CCAMCApp : public CWinApp,public CBCGPWorkspace 


{
public:
	CCAMCApp();

	BOOL	m_bHiColorIcons;
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCAMCApp theApp;
