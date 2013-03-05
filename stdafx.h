// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// ��Ŀ�ض��İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ��ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0400		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0400		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0410 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0400	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ��������������ȫ���Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS
#define _AFXDLL

#include <afxwin.h>         // MFC ���ĺͱ�׼���
#include <afxext.h>         // MFC ��չ
#include <afxdisp.h>        // MFC �Զ�����

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>



#include <afxdtctl.h>		// Internet Explorer 4 �����ؼ��� MFC ֧��
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows �����ؼ��� MFC ֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <opencv.hpp>
#include "BCGCBProInc.h"
#include <afxdisp.h>

//#include "minmax.h"
#pragma comment( lib, "kernel32.lib" )							
#pragma comment( lib, "user32.lib" )							
#pragma comment( lib, "gdi32.lib" )							
#pragma comment( lib, "Advapi32.lib" )	
//#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )								// Search For GLaux.lib While Linking  
//#pragma comment( lib, "cxcore.lib" )							// Search For cxcore.lib While Linking
#pragma comment( lib, "opencv_core231.lib" )							// Search For cxcore.lib While Linking
//#pragma comment( lib, "cv.lib" )								// Search For cv.lib While Linking
#pragma comment( lib, "opencv_calib3d231.lib" )	
//#pragma comment( lib, "highgui.lib" )							// Search For highgui.lib While Linking
#pragma comment( lib, "opencv_highgui231.lib" )

#pragma comment( lib, "opencv_imgproc231.lib" )

#pragma comment( lib, "opencv_legacy231.lib" )

#pragma comment( lib, "opencv_ts231.lib" )
#pragma comment( lib, "opencv_video231.lib" )


#pragma comment( lib, "opencv_ml231.lib" )
#pragma comment( lib, "opencv_objdetect231.lib" )

#pragma comment( lib, "opencv_gpu231.lib" )
#pragma comment( lib, "opencv_haartraining_engine.lib" )

#pragma comment( lib, "opencv_contrib231.lib" )
#pragma comment( lib, "opencv_flann231.lib" )
#pragma comment( lib, "opencv_features2d231.lib" )



// #pragma comment( lib, "BCGCBPRO1610Eval900.lib" )
#pragma comment( lib, "wintab32.lib" )
//#pragma comment( lib, "wintabx.lib" )
//#pragma comment( lib, ".\lib\wintab.lib" )
#pragma comment( lib, "r2vdll.lib" )

#pragma comment( lib, "HVDAILT.lib" )
#pragma comment( lib, "HVUtil.lib" )
#pragma comment( lib, "Raw2Rgb.lib" )

#pragma comment( lib, "FreeImage.lib") 

//#pragma comment( lib, "libcpmtd.lib" )
//#pragma comment( lib, "flib.lib" )