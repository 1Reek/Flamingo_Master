// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A53A96F2_F005_4DE1_964A_55DEFA2168BF__INCLUDED_)
#define AFX_STDAFX_H__A53A96F2_F005_4DE1_964A_55DEFA2168BF__INCLUDED_

#ifndef WINVER				// ָ��Ҫ������ƽ̨
#define WINVER 0x0501		// �޸�Ϊ : Windows 2000
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501     // �޸�Ϊ : Windows 2000
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410	// ������ : Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600		// IE����Ͱ汾�� : 6.0
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A53A96F2_F005_4DE1_964A_55DEFA2168BF__INCLUDED_)
