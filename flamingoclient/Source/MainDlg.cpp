// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "IniFile.h"
#include "net/IUProtocolData.h"
#include "SystemSettingDlg.h"
#include "ClosePromptDlg.h"
#include "IULog.h"
#include "MobileStatusDlg.h"
#include "UserSessionData.h"
#include "ModifyMarkNameDlg.h"
#include "TeamDlg.h"
#include "BuddyChatDlg.h"
#include "GroupChatDlg.h"
#include "SessChatDlg.h"
#include "BuddyInfoDlg.h"
#include "GroupInfoDlg.h"
#include "FindFriendDlg.h"
#include "RemoteDesktopDlg.h"
#include "AddFriendConfirmDlg.h"
#include "Updater.h"
#include "UpdateDlg.h"
#include "CreateNewGroupDlg.h"
#include "GDIFactory.h"
#include "IULog.h"
#include "Startup.h"
#include "MultiChatMemberSelectionDlg.h"
#include "ChatDlgCommon.h"
#include "EncodeUtil.h"
#include "UIText.h"
#include "PerformanceCounter.h"
#include "net/Msg.h"

#pragma comment(lib, "winmm.lib")

extern HWND g_hwndOwner;

#define LOGIN_TIMER_ID			  1
#define RECVCHATMSG_TIMER_ID	  2
#define ADDFRIENDREQUEST_TIMER_ID 3
#define EXITAPP_TIMER_ID		  4	
#define RECONNECT_TIMER_ID        5

//��������Ϣ�������ڿ�͸�
#define BUDDYINFOFLOATWND_WIDTH   300
#define BUDDYINFOFLOATWND_HEIGHT  150

//���˵������Ӳ˵�������
enum MAIN_PANEL_MEMU
{
	MAIN_PANEL_STATUS_SUBMENU_INDEX,
	MAIN_PANEL_TRAYICON_SUBMENU_INDEX,
	MAIN_PANEL_TRAYICON_SUBMENU2_INDEX,
	MAIN_PANEL_RECENT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_SUBMENU_INDEX,
	MAIN_PANEL_GROUP_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_MAIN_SUBMENU_INDEX,
	MAIN_PANEL_LOCK_SUBMENU_INDEX,
	MAIN_PANEL_RECENTLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_GROUPLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_TEAM_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_BLANK_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_GROUPLIST_BLANK_CONTEXT_SUBMENU_INDEX
};


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtSign.m_hWnd)   
	{
		if (pMsg->message == WM_CHAR && pMsg->wParam == VK_RETURN) 
		{
			SetFocus();
		} 
	}

 	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
 	{
 		return FALSE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

CMainDlg::CMainDlg(void) : m_LoginDlg(&m_FMGClient)
{
	//m_BuddyChatDlg.m_lpFMGClient = &m_FMGClient;
	m_nLastMsgType = FMG_MSG_TYPE_BUDDY;
	m_nLastSenderId = 0;
	m_bPicHeadPress = FALSE;
	m_hAppIcon = NULL;
	memset(m_hLoginIcon, 0, sizeof(m_hLoginIcon));
	m_nCurLoginIcon = 0;
	m_hMsgIcon = NULL;
	m_dwLoginTimerId = 0;
	m_dwMsgTimerId = 0;
	m_dwAddFriendTimerId = 0;
    m_dwReconnectTimerId = 0;

    m_bAlreadySendReloginRequest = false;


    m_bEnableReconnect = true;
    m_uReconnectInterval = 3000;

	memset(m_hAddFriendIcon, 0, sizeof(m_hAddFriendIcon));

	m_hDlgIcon = m_hDlgSmallIcon = NULL;
	memset(&m_stAccountInfo, 0, sizeof(m_stAccountInfo));

	m_pFindFriendDlg = new CFindFriendDlg();
    m_pRemoteDesktopDlg = new CRemoteDesktopDlg();

	m_nYOffset = 0;
	m_bFold = TRUE;
	m_bSideState = TRUE;

	m_hHotRgn = NULL;
	m_nBuddyListHeadPicStyle = BLC_BIG_ICON_STYLE;
	m_bShowBigHeadPicInSel = TRUE;
	m_bPanelLocked = FALSE;
	m_bAlreadyLogin = FALSE;

	m_bShowOnlineBuddyOnly = FALSE;

	m_rcTrayIconRect.SetRectEmpty();

	m_nCurSelIndexInMainTab = -1;

    m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
}

CMainDlg::~CMainDlg(void)
{
	delete m_pFindFriendDlg;
    delete m_pRemoteDesktopDlg;
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);
	
	//ȥ����󻯰�ť
	ModifyStyle(WS_MAXIMIZEBOX, 0);
	InitUI();

	if(!m_FMGClient.InitProxyWnd())
	{
		::MessageBox(NULL, _T("��ʼ��������ʧ�ܣ�Flamingo�޷�������"), _T(""), MB_OK|MB_TOPMOST);
		::ExitProcess(0);
		return FALSE;
	}

	LoadAppIcon(m_FMGClient.GetStatus());
	m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("Flamingoδ��¼"));

	//����UsersĿ¼
	CString strAppPath(g_szHomePath);
	CString strUsersDirectory(strAppPath);
	strUsersDirectory += _T("Users");
	if(!Hootina::CPath::IsDirectoryExist(strUsersDirectory))
		Hootina::CPath::CreateDirectory(strUsersDirectory);
	
	// ����ϵͳ�����б�
	tstring strFileName = Hootina::CPath::GetAppPath() + _T("Face\\FaceConfig.xml");
	m_FaceList.LoadConfigFile(strFileName.c_str());

	// ���ص�¼�ʺ��б�
	strFileName = Hootina::CPath::GetAppPath()+_T("Users\\LoginAccountList.dat");
	m_LoginAccountList.LoadFile(strFileName.c_str());

	//���������������ʾ��¼�Ի���
    //�����¼�Ի�������������¼,�����Ի���Ҳ��Ӧ�ô�������
    if (!StartLogin(m_LoginAccountList.IsAutoLogin()))
        return TRUE;

	//TODO: �����˻��������û�ͷ��
	UINT nUTalkUin = _tcstoul(m_stAccountInfo.szUser, NULL, 10);

	//����Ĭ��λ��
	long cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	cxScreen -= 110;
	::SetWindowPos(m_hWnd, NULL, cxScreen-280, 78, 280, 675, SWP_SHOWWINDOW);

	ModifyStyle(0, WS_CLIPCHILDREN);
	ModifyStyleEx(WS_EX_APPWINDOW, 0);


	m_MultiChatDlg.m_lpFMGClient = &m_FMGClient;
	m_MultiChatDlg.m_lpFaceList = &m_FaceList;
	
	return TRUE;
}

void CMainDlg::OnSysCommand(UINT nID, CPoint pt)
{
	if (nID == SC_MINIMIZE)
	{
        m_BuddyInfoFloatWnd.ShowWindow(SW_HIDE);
        ShowWindow(SW_HIDE);
		return;
	}

	SetMsgHandled(FALSE);
}

void CMainDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CMainDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMainDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 280;
	lpMMI->ptMinTrackSize.y = 520;

	lpMMI->ptMaxTrackSize.x = 608;
	lpMMI->ptMaxTrackSize.y = ::GetSystemMetrics(SM_CYSCREEN);
}

void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetFocus() == m_edtSign.m_hWnd)
		SetFocus();
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_TrayIcon.OnTimer(nIDEvent);

	if (nIDEvent == m_dwMsgTimerId)		// ����Ϣ��˸����
	{
		static BOOL bSwitch = FALSE;

		if (m_dwLoginTimerId != NULL)	// δ��¼�ɹ�
			return;

		bSwitch = !bSwitch;
		if (bSwitch)
			m_TrayIcon.ModifyIcon(NULL, _T(""));
		else
			m_TrayIcon.ModifyIcon(m_hMsgIcon, _T(""));
	}
	else if (nIDEvent == m_dwLoginTimerId)	// ��¼����
	{
		m_TrayIcon.ModifyIcon(m_hLoginIcon[m_nCurLoginIcon], _T(""));
		m_nCurLoginIcon++;
		if (m_nCurLoginIcon >= 6)
			m_nCurLoginIcon = 0;
	}
	else if(nIDEvent == m_dwAddFriendTimerId)	//������Ӻ�����ʾ����
	{
		static BOOL bSwitch = FALSE;

		bSwitch = !bSwitch;
		if (bSwitch)
			m_TrayIcon.ModifyIcon(m_hAddFriendIcon[0], _T("�Ӻ�������"));
		else
			m_TrayIcon.ModifyIcon(m_hAddFriendIcon[1], _T("�Ӻ�������"));
	}
    else if (nIDEvent == m_dwReconnectTimerId)
    {
        if (!m_bAlreadySendReloginRequest)
        {
            long nStatus = m_FMGClient.GetStatus();
            LoadAppIcon(nStatus);
            CString strIconInfo;
            if (nStatus == STATUS_OFFLINE)
                strIconInfo.Format(_T("%s\r\n%s\r\n"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(), m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());

            m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo, 1, TRUE, _T("��ܰ��ʾ"), _T("���Ѿ����ߣ����ڰ�������......"), 3000);

            m_FMGClient.Login();
        }
    }
}

//void CMainDlg::OnSize(UINT nType, CSize size)
//{
//	SetMsgHandled(FALSE);
//
//	CRect rcClient;
//	GetClientRect(&rcClient);
//	//HDWP hdwp = ::BeginDeferWindowPos(18);
//	
//	if(m_btnMainMenu.IsWindow())
//		m_btnMainMenu.MoveWindow(6, rcClient.Height() - 28, 22, 20);
//	
//	if(m_btnMultiChat.IsWindow())
//		m_btnMultiChat.MoveWindow(30, rcClient.Height() - 28, 22, 20, TRUE);
//
//	//if (m_btnSystemSet.IsWindow())
//		//m_btnSystemSet.MoveWindow(28 + 6, rcClient.Height() - 28, 22, 20);
//
//	//if (m_btnMsgMgr.IsWindow())
//	//	m_btnMsgMgr.MoveWindow(72, rcClient.Height()-30, 22, 22);
//
//	//if (m_btnSafe.IsWindow())
//	//	m_btnSafe.MoveWindow(94, rcClient.Height()-30, 62, 22);
//
//	if(m_picHead.IsWindow())
//		m_picHead.MoveWindow(10, 30, 64, 64, TRUE);
//	
//	if(m_staNickName.IsWindow())
//		m_staNickName.MoveWindow(80, 34, 100, 20, FALSE);
//
//	if (m_btnFind.IsWindow())
//		m_btnFind.MoveWindow(rcClient.right - 60, rcClient.bottom-29, 50, 20);
//
//	if (m_btnSign.IsWindow())
//		m_btnSign.MoveWindow(80, 60, rcClient.Width()-102, 22);
//
//	if (m_edtSign.IsWindow())
//		m_edtSign.MoveWindow(80, 60, rcClient.Width()-102, 22);
//
//	if (m_edtSearch.IsWindow())
//		m_edtSearch.MoveWindow(0, 126, rcClient.Width(), 27);
//
//	//if (m_btnMsgBox.IsWindow())
//	//	m_btnMsgBox.MoveWindow(rcClient.Width()-48, 80, 22, 20);
//
//	//if (m_btnChangeExterior.IsWindow())
//	//	m_btnChangeExterior.MoveWindow(rcClient.Width()-26, 80, 20, 20);
//
//	//if (m_btnAppMgr.IsWindow())
//	//	m_btnAppMgr.MoveWindow(rcClient.Width()-38, rcClient.bottom-60, 22, 22);
//
//	if (m_tbBottom.IsWindow())
//		m_tbBottom.MoveWindow(46, rcClient.bottom-60, 212, 22);
//
//	if (m_TabCtrl.IsWindow())
//	{
//		int nCount = m_TabCtrl.GetItemCount();
//		if (nCount > 0)
//		{
//			int nWidth = (rcClient.Width()-2) / nCount;
//			int nRemainder = (rcClient.Width()-2) % nCount;
//
//			for (int i = 0; i < nCount; i++)
//			{
//				m_TabCtrl.SetItemSize(i, nWidth, 48, nWidth-19, 19);
//			}
//
//			m_TabCtrl.SetItemSize(nCount-1, nWidth+nRemainder, 48, nWidth+nRemainder-19, 19);
//		}
//
//		m_TabCtrl.MoveWindow(0, 153, rcClient.right, 49);
//	}
//
//	if (m_BuddyListCtrl.IsWindow())
//		m_BuddyListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);
//
//	if (m_GroupListCtrl.IsWindow())
//		m_GroupListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);
//
//	if (m_RecentListCtrl.IsWindow())
//		m_RecentListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);
//
//	if (m_picLogining.IsWindow())
//		m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150);
//	
//	if (m_staUTalkNum.IsWindow())
//		m_staUTalkNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14);
//
//	if (m_staLogining.IsWindow())
//		m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16);
//
//	if (m_btnCancel.IsWindow())
//		m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);
//
//	if (m_btnUnlock.IsWindow())
//		m_btnUnlock.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);
//}

//HBRUSH CSkinStatic::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
//{
//	if(wndStatic == m_staNickName) 
//		return (HBRUSH)::GetStockObject(NULL_BRUSH);
//	return NULL;
//}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	//m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 280, 2, 127));	
	//m_SkinDlg.InvalidateRect(NULL, TRUE);

	CRect rcClient;
	GetClientRect(&rcClient);
	HDWP hdwp = ::BeginDeferWindowPos(18);
	
	if(m_btnMainMenu.IsWindow())
		::DeferWindowPos(hdwp, m_btnMainMenu.m_hWnd, NULL, 6, rcClient.Height()-28, 22, 20, SWP_NOZORDER);
		//m_btnMainMenu.MoveWindow(6, rcClient.Height() - 28, 22, 20);
	
	if(m_btnMultiChat.IsWindow())
		::DeferWindowPos(hdwp, m_btnMultiChat.m_hWnd, NULL, 30, rcClient.Height()-28, 22, 20, SWP_NOZORDER);
		//m_btnMultiChat.MoveWindow(30, rcClient.Height() - 28, 22, 20, TRUE);

	if(m_picHead.IsWindow())
		::DeferWindowPos(hdwp, m_picHead.m_hWnd, NULL, 10, 30, 64, 64, SWP_NOZORDER);
		//m_picHead.MoveWindow(10, 30, 64, 64, TRUE);
	
	if(m_staNickName.IsWindow())
		::DeferWindowPos(hdwp, m_staNickName.m_hWnd, NULL, 80, 34, rcClient.Width()-102, 20, SWP_NOZORDER);
		//m_staNickName.MoveWindow(80, 34, 100, 20, FALSE);

	if (m_btnFind.IsWindow())
		::DeferWindowPos(hdwp, m_btnFind.m_hWnd, NULL, rcClient.right-60, rcClient.bottom-29, 50, 20, SWP_NOZORDER);
		//m_btnFind.MoveWindow(rcClient.right - 60, rcClient.bottom-29, 50, 20);

	if (m_btnSign.IsWindow())
		::DeferWindowPos(hdwp, m_btnSign.m_hWnd, NULL, 80, 60, rcClient.Width()-102, 22, SWP_NOZORDER);
		//m_btnSign.MoveWindow(80, 60, rcClient.Width()-102, 22);

	if (m_edtSign.IsWindow())
		::DeferWindowPos(hdwp, m_edtSign.m_hWnd, NULL, 80, 60, rcClient.Width()-102, 22, SWP_NOZORDER);
		//m_edtSign.MoveWindow(80, 60, rcClient.Width()-102, 22);

	if (m_edtSearch.IsWindow())
		::DeferWindowPos(hdwp, m_edtSearch.m_hWnd, NULL, 0, 126, rcClient.Width(), 27, SWP_NOZORDER);
		//m_edtSearch.MoveWindow(0, 126, rcClient.Width(), 27);

	if (m_tbBottom.IsWindow())
		::DeferWindowPos(hdwp, m_tbBottom.m_hWnd, NULL, 46, rcClient.bottom-60, 212, 22, SWP_NOZORDER);
		//m_tbBottom.MoveWindow(46, rcClient.bottom-60, 212, 22);

	if (m_TabCtrl.IsWindow())
	{
		int nCount = m_TabCtrl.GetItemCount();
		if (nCount > 0)
		{
			int nWidth = (rcClient.Width()-2) / nCount;
			int nRemainder = (rcClient.Width()-2) % nCount;

			for (int i = 0; i < nCount; i++)
			{
				m_TabCtrl.SetItemSize(i, nWidth, 48, nWidth-19, 19);
			}

			m_TabCtrl.SetItemSize(nCount-1, nWidth+nRemainder, 48, nWidth+nRemainder-19, 19);
		}
		
		::DeferWindowPos(hdwp, m_TabCtrl.m_hWnd, NULL, 0, 153, rcClient.right-2, 49, SWP_NOZORDER);
		//m_TabCtrl.MoveWindow(0, 153, rcClient.right, 49);
	}

	if (m_BuddyListCtrl.IsWindow())
		::DeferWindowPos(hdwp, m_BuddyListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height()-236, SWP_NOZORDER);
		//m_BuddyListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);

	if (m_GroupListCtrl.IsWindow())
		::DeferWindowPos(hdwp, m_GroupListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height()-236, SWP_NOZORDER);
		//m_GroupListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);

	if (m_RecentListCtrl.IsWindow())
		::DeferWindowPos(hdwp, m_RecentListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height()-236, SWP_NOZORDER);
		//m_RecentListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);

	if (m_picLogining.IsWindow())
		::DeferWindowPos(hdwp, m_picLogining.m_hWnd, NULL, (rcClient.Width() - 220) / 2, 76, 220, 150, SWP_NOZORDER);
		//m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150);
	
	if (m_staUTalkNum.IsWindow())
		::DeferWindowPos(hdwp, m_staUTalkNum.m_hWnd, NULL, rcClient.left, 226, rcClient.Width(), 14, SWP_NOZORDER);
		//m_staUTalkNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14);

	if (m_staLogining.IsWindow())
		::DeferWindowPos(hdwp, m_staLogining.m_hWnd, NULL, rcClient.left, 240, rcClient.Width(), 16, SWP_NOZORDER);
		//m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16);

	if (m_btnCancel.IsWindow())
		::DeferWindowPos(hdwp, m_btnCancel.m_hWnd, NULL, (rcClient.Width() - 86) / 2, 304, 86, 30, SWP_NOZORDER);
		//m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);

	if (m_btnUnlock.IsWindow())
		::DeferWindowPos(hdwp, m_btnUnlock.m_hWnd, NULL, (rcClient.Width() - 86) / 2, 304, 86, 30, SWP_NOZORDER);
		//m_btnUnlock.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);

	::EndDeferWindowPos(hdwp);

	//������������WS_CLIPCHILDREN��������ڲ����˽���ɫ���������ǩ����̬�ı�����Ҫ���»���һ�£��Է�ֹ��������
	if(m_staNickName.IsWindow())
		m_staNickName.Invalidate(FALSE);
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
	switch (nHotKeyID)
	{
	case 1001:
		{
			if(::IsWindowVisible(m_hWnd))
				ShowWindow(SW_HIDE);
			else
				OnTrayIconNotify(WM_TRAYICON_NOTIFY, NULL, WM_LBUTTONUP);
		}
		break;
	}
}

void CMainDlg::OnClose()
{
	if(m_FMGClient.m_UserConfig.IsEnableExitPrompt())
	{
		CClosePromptDlg closePromptDlg;
		closePromptDlg.m_pFMGClient = &m_FMGClient;
		int nRet = closePromptDlg.DoModal(m_hWnd, NULL);

		if(nRet == IDC_EXIT)
		{
			CloseDialog(IDOK);
		}
		else if(nRet == IDC_MINIMIZE)
		{
			ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if(m_FMGClient.m_UserConfig.IsEnableExitWhenCloseMainDlg())
			CloseDialog(IDOK);
		else
			ShowWindow(SW_HIDE);
	}
}

void CMainDlg::OnDestroy()
{	
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	m_FMGClient.m_UserConfig.SetMainDlgX(rcWindow.left);
	m_FMGClient.m_UserConfig.SetMainDlgY(rcWindow.top);
	m_FMGClient.m_UserConfig.SetMainDlgWidth(rcWindow.Width());
	m_FMGClient.m_UserConfig.SetMainDlgHeight(rcWindow.Height());
	
	m_FMGClient.m_UserConfig.SetBuddyListHeadPicStyle(m_nBuddyListHeadPicStyle);
	m_FMGClient.m_UserConfig.EnableBuddyListShowBigHeadPicInSel(m_bShowBigHeadPicInSel);
	m_FMGClient.m_UserConfig.SetFaceID(m_FMGClient.m_UserMgr.m_UserInfo.m_nFace);
	m_FMGClient.m_UserConfig.SetCustomFace(m_FMGClient.m_UserMgr.m_UserInfo.m_strCustomFace.c_str());
	m_FMGClient.SaveUserConfig();
	
	//�洢�û���Ϣ
	m_FMGClient.m_UserMgr.SaveBuddyInfo();
	//�洢���ѷ�����Ϣ
	m_FMGClient.m_UserMgr.StoreTeamInfo();
	//�洢�����ϵ���б�
	m_FMGClient.m_UserMgr.StoreRecentList();


	m_FMGClient.Uninit();

	UninitUI();

	//m_btnSystemSet.DestroyWindow();
	//m_TrayIcon.RemoveIcon();
	m_SkinMenu.DestroyMenu();
	//if (m_BuddyChatDlg.IsWindow())
	//	m_BuddyChatDlg.DestroyWindow();
	//if (m_GroupChatDlg.IsWindow())
	//	m_GroupChatDlg.DestroyWindow();

	DestroyAppIcon();
	DestroyLoginIcon();
	DestroyAddFriendIcon();

	if (m_hMsgIcon != NULL)
	{
		::DestroyIcon(m_hMsgIcon);
		m_hMsgIcon = NULL;
	}

	if (m_hDlgIcon != NULL)
	{
		::DestroyIcon(m_hDlgIcon);
		m_hDlgIcon = NULL;
	}

	if (m_hDlgSmallIcon != NULL)
	{
		::DestroyIcon(m_hDlgSmallIcon);
		m_hDlgSmallIcon = NULL;
	}

	::UnregisterHotKey(m_hWnd, 1001);	// ��ע����ȡ��Ϣ�ȼ�

	// ������������
	//tstring strFileName = m_FMGClient.GetPersonalFolder()+_T("MiscConfig.xml");
	//SaveMiscConfig(strFileName.c_str());
	
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	CGDIFactory::Uninit();
}

void CMainDlg::OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CAboutDlg dlg;
	dlg.DoModal(m_hWnd, NULL);
}

void CMainDlg::CloseDialog(int nVal)
{
	if(IsFilesTransferring())
	{
		if(IDNO == ::MessageBox(m_hWnd, _T("�������ļ����ڴ��䣬ȷ��Ҫ�˳�Flamingo��"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION))
			return;
	}
	
	if (m_dwLoginTimerId != NULL)
	{
		KillTimer(m_dwLoginTimerId);
		m_dwLoginTimerId = NULL;
	}
	if (m_dwMsgTimerId != NULL)
	{
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
	}

	if(m_LogonUserInfoDlg.IsWindow())
		m_LogonUserInfoDlg.DestroyWindow();

	if(m_ModifyPasswordDlg.IsWindow())
		m_ModifyPasswordDlg.DestroyWindow();


	m_TrayIcon.RemoveIcon();
	CloseAllDlg();
	m_CascadeWinManager.Clear();
	//ShowWindow(SW_HIDE);

	//if (m_FMGClient.GetStatus() != STATUS_OFFLINE)
	//{
	//	m_FMGClient.Logout();
	//}
	//else
	//{
		DestroyWindow();
		::PostQuitMessage(nVal);
	//}
}

// ��ʼ��Top������
BOOL CMainDlg::InitTopToolBar()
{
	int nIndex = m_tbTop.AddItem(101, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 35, 20);
	//m_tbTop.SetItemText(nIndex, _T("10"));
	//m_tbTop.SetItemToolTipText(nIndex, _T("UTalk�ռ���Ϣ����\r\n  ���Ѷ�̬(10+)"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\qzone.png"));

	//nIndex = m_tbTop.AddItem(102, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ΢��"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\wblog.png"));

	//nIndex = m_tbTop.AddItem(103, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 35, 20);
	//m_tbTop.SetItemText(nIndex, _T("99"));
	//m_tbTop.SetItemToolTipText(nIndex, _T("UTalk����\r\n  �ռ���(84)\r\n  Ⱥ�ʼ�(20)\r\n  Ư��ƿ(1)"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\mail.png"));

	//nIndex = m_tbTop.AddItem(104, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("������"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\friend.png"));

	//nIndex = m_tbTop.AddItem(105, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ����"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\paipai.png"));

	//nIndex = m_tbTop.AddItem(106, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("UTalkǮ��"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\purse.png"));

	//nIndex = m_tbTop.AddItem(107, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("���Ѹ�������\r\n  ��������������μ���������"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\soso.png"));

	//nIndex = m_tbTop.AddItem(108, STBI_STYLE_BUTTON);
	//m_tbTop.SetItemSize(nIndex, 22, 20);
	//m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ���ҵ���Ѷ"));
	//m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\Today.png"));

	m_tbTop.SetLeftTop(2, 0);
	m_tbTop.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTopToolBar(4, 80, 4+210, 80+20);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_TOP);

	return TRUE;
}

// ��ʼ��Bottom������
BOOL CMainDlg::InitBottomToolBar()
{
	int nIndex = m_tbBottom.AddItem(201, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("�ֻ�����"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\WirelessLive.png"));

	//nIndex = m_tbBottom.AddItem(202, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("UTalk��Ϸ"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\UTalkGame.png"));

	//nIndex = m_tbBottom.AddItem(203, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("UTalk����"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\UTalkPet.png"));

	//nIndex = m_tbBottom.AddItem(204, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("UTalk����"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\UTalkMusic.png"));

	//nIndex = m_tbBottom.AddItem(205, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("��Ѷ��Ƶ"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\UTalkLive.png"));

	//nIndex = m_tbBottom.AddItem(206, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("UTalk����"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\Tourism.png"));

	//nIndex = m_tbBottom.AddItem(207, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("UTalk���Թܼ�"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\UTalkPCMgr.png"));

	//nIndex = m_tbBottom.AddItem(208, STBI_STYLE_BUTTON);
	//m_tbBottom.SetItemSize(nIndex, 22, 22);
	//m_tbBottom.SetItemPadding(nIndex, 5);
	//m_tbBottom.SetItemToolTipText(nIndex, _T("���Ĺ���"));
	//m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
	//	_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\PaiPai.png"));

	m_tbBottom.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBottomToolBar(46, rcClient.bottom-60, 46+212, (rcClient.bottom-60)+22);
	m_tbBottom.Create(m_hWnd, rcBottomToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_BOTTOM);

	return TRUE;
}

// ��ʼ��Tab��
BOOL CMainDlg::InitTabCtrl()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nWidth = (rcClient.Width()-2) / 3;
	int nRemainder = (rcClient.Width()-2) % 3;

	int nIndex = m_TabCtrl.AddItem(301, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth+nRemainder, 48, nWidth+nRemainder-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("�����ϵ��"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_last_normal.png"), _T("MainTabCtrl\\icon_last_selected.png"));

	nIndex = m_TabCtrl.AddItem(302, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 48, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("��ϵ��"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_contacts_normal.png"), _T("MainTabCtrl\\icon_contacts_selected.png"));

	nIndex = m_TabCtrl.AddItem(303, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 48, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("Ⱥ/������"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_group_normal.png"), _T("MainTabCtrl\\icon_group_selected.png"));

	

	//nIndex = m_TabCtrl.AddItem(302, STCI_STYLE_DROPDOWN);
	//m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	//m_TabCtrl.SetItemToolTipText(nIndex, _T("����"));
	//m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\FriendSocial_TabBtn_Normal.png"),
	//	_T("MainTabCtrl\\FriendSocial_TabBtn_Focus.png"));

	

	//nIndex = m_TabCtrl.AddItem(304, STCI_STYLE_BUTTON);
	//m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	//m_TabCtrl.SetItemToolTipText(nIndex, _T("΢��"));
	//m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\WBlog_TabBtn_Normal_Msg.png"),
	//	_T("MainTabCtrl\\WBlog_TabBtn_Focus.png"));

	
	m_TabCtrl.SetBgPic(_T("MainTabCtrl\\main_tab_bkg.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsBgPic(NULL, _T("MainTabCtrl\\main_tab_highlight.png"), _T("MainTabCtrl\\main_tab_check.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsArrowPic(_T("MainTabCtrl\\main_tabbtn_highlight.png"), _T("MainTabCtrl\\main_tabbtn_down.png"));

	CRect rcTabCtrl(0, 154, rcClient.right, 154+48);
	m_TabCtrl.Create(m_hWnd, rcTabCtrl, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_TABCTRL_MAIN, NULL);

	m_TabCtrl.SetCurSel(1);

	return TRUE;
}

// ��ʼ�������б�ؼ�
BOOL CMainDlg::InitBuddyListCtrl()
{
	m_BuddyListCtrl.SetMargin(CRect(2,0,2,0));
	m_BuddyListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_BuddyListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_BuddyListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_BuddyListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_BuddyListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_BuddyListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_BuddyListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_BuddyListCtrl.SetStyle(BLC_STANDARD_ICON_STYLE);
	m_BuddyListCtrl.SetShowBigIconInSel(TRUE);
	m_BuddyListCtrl.SetBuddyTeamHeight(24);
	m_BuddyListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_BuddyListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_BuddyListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_BuddyListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height()-236);
	m_BuddyListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_LISTCTRL_BUDDY);

	return TRUE;
}

// ��ʼ��Ⱥ�б�ؼ�
BOOL CMainDlg::InitGroupListCtrl()
{
	m_GroupListCtrl.SetMargin(CRect(2,0,2,0));
	m_GroupListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_GroupListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_GroupListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_GroupListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_GroupListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_GroupListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_GroupListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_GroupListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_GroupListCtrl.SetShowBigIconInSel(TRUE);
	m_GroupListCtrl.SetBuddyTeamHeight(24);
	m_GroupListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_GroupListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_GroupListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_GroupListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height());
	m_GroupListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LISTCTRL_GROUP);

	return TRUE;
}

// ��ʼ�������ϵ���б�ؼ�
BOOL CMainDlg::InitRecentListCtrl()
{
	m_RecentListCtrl.SetMargin(CRect(2,0,2,0));
	m_RecentListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	//m_RecentListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_RecentListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	//m_RecentListCtrl.AddBuddyItemSmallIcon(int nID, LPCTSTR lpszToolTipText, LPCTSTR lpszFileName);
	//m_RecentListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	//m_RecentListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	//m_RecentListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	//m_RecentListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	//m_RecentListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	//m_RecentListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_RecentListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_RecentListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	//m_RecentListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_RecentListCtrl.SetShowBigIconInSel(TRUE);
	//m_RecentListCtrl.SetBuddyTeamHeight(24);
	m_RecentListCtrl.SetBuddyTeamHeight(0);
	m_RecentListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_RecentListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_RecentListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_RecentListCtrl.SetBuddyItemPadding(1);
	m_RecentListCtrl.SetBuddyTeamExpand(0, TRUE);
	//m_RecentListCtrl.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height()-236);
	m_RecentListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LISTCTRL_RECENT);

	return TRUE;
}

BOOL CMainDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 280, 2, 127));	
	m_SkinDlg.SetBgColor(RGB(255, 255, 255));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), _T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T(""));


	HDC hDlgBgDC = m_SkinDlg.GetBgDC();
	
	CRect rcClient;
	GetClientRect(&rcClient);

	m_btnMainMenu.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMainMenu.SetTransparent(TRUE, hDlgBgDC);
	m_btnMainMenu.SetBgPic(_T("menu_btn_normal.png"), 
		_T("menu_btn_highlight.png"), _T("menu_btn_highlight.png"), _T("menu_btn_normal.png"));
	m_btnMainMenu.SubclassWindow(GetDlgItem(ID_BTN_MAIN_MENU));
	//long nMainDlgLeft = m_FMGClient.m_UserMgr.m_MainDlgConfig.GetLeft();
	m_btnMainMenu.MoveWindow(6, rcClient.Height() - 28, 22, 20, TRUE);
	m_btnMainMenu.SetToolTipText(_T("���˵�"));

	m_btnMultiChat.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMultiChat.SetTransparent(TRUE, hDlgBgDC);
	m_btnMultiChat.SetBgPic(_T("multichat_btn_normal.png"), 
		_T("multichat_btn_highlight.png"), _T("multichat_btn_highlight.png"), _T("multichat_btn_normal.png"));
	m_btnMultiChat.SubclassWindow(GetDlgItem(ID_BTN_MULTI_CHAT));
	//long nMainDlgLeft = m_FMGClient.m_UserMgr.m_MainDlgConfig.GetLeft();

	m_btnMultiChat.MoveWindow(30, rcClient.Height() - 28, 22, 20, TRUE);
	m_btnMultiChat.SetToolTipText(_T("��ϢȺ��"));

	//m_btnSystemSet.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnSystemSet.SetButtonType(SKIN_PUSH_BUTTON);
	//m_btnSystemSet.SetTransparent(TRUE, hDlgBgDC);
	//m_btnSystemSet.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnSystemSet.SetBgPic(_T("SystemSet_normal.png"), _T("SystemSet_highlight.png"), _T("SystemSet_highlight.png"), _T("SystemSet_normal.png"));
	//m_btnSystemSet.SetIconPic(_T("SystemSet.png"));
	//m_btnSystemSet.SubclassWindow(GetDlgItem(ID_BTN_SYSTEM_SET));
	//m_btnSystemSet.MoveWindow(28 + 6, rcClient.Height() - 28, 22, 20, TRUE);
	//m_btnSystemSet.SetToolTipText(_T("��ϵͳ����"));

	//m_btnMsgMgr.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnMsgMgr.SetTransparent(TRUE, hDlgBgDC);
	//m_btnMsgMgr.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnMsgMgr.SetIconPic(_T("message.png"));
	//m_btnMsgMgr.SubclassWindow(GetDlgItem(ID_BTN_MSG_MGR));
	//m_btnMsgMgr.MoveWindow(72, rcClient.Height()-30, 22, 22, TRUE);
	//m_btnMsgMgr.SetToolTipText(_T("����Ϣ������"));

	//m_btnSafe.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnSafe.SetTransparent(TRUE, hDlgBgDC);
	//m_btnSafe.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL);
	//m_btnSafe.SetIconPic(_T("UTalkSafe.png"));
	//m_btnSafe.SubclassWindow(GetDlgItem(ID_BTN_SAFE));
	//m_btnSafe.MoveWindow(94, rcClient.Height()-30, 62, 22, TRUE);
	//m_btnSafe.SetToolTipText(_T("�򿪰�ȫ�ſ�"));

	//m_btnFind.SetButtonType(SKIN_FIND_BUTTON);
	m_btnFind.SetTransparent(TRUE, hDlgBgDC);
	m_btnFind.SetBgPic(_T("��Ӻ���_Ĭ��״̬.png"), _T("��Ӻ���_���״̬.png"), _T("��Ӻ���_���״̬.png"), _T("��Ӻ���_���״̬.png"));
	//m_btnFind.SetIconPic(_T("find.png"));
	m_btnFind.SubclassWindow(GetDlgItem(ID_BTN_FIND));
	m_btnFind.MoveWindow(rcClient.right - 60, rcClient.bottom-29, 50, 20, FALSE);
	m_btnFind.SetWindowText(_T("     ����"));
	m_btnFind.SetToolTipText(_T("������ϵ�˺�Ⱥ��"));

	m_picHead.SetTransparent(TRUE, hDlgBgDC);
	m_picHead.SetShowCursor(TRUE);
	m_picHead.SetBgPic(_T("HeadCtrl\\Padding4Normal.png"), _T("HeadCtrl\\Padding4Hot.png"), _T("HeadCtrl\\Padding4Hot.png"));
	m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
	m_picHead.MoveWindow(10, 30, 64, 64, FALSE);
	m_picHead.SetToolTipText(_T("����޸ĸ�������"));


	m_staNickName.SetTransparent(TRUE, hDlgBgDC);
	m_staNickName.SetTextColor(RGB(255, 255, 255));
	m_staNickName.SubclassWindow(GetDlgItem(ID_STATIC_NICKNAME));
	m_staNickName.SetWindowText(_T("�û��ǳ�"));
	HFONT hFontNickName = CGDIFactory::GetBoldFont(20);
	m_staNickName.SetFont(hFontNickName);
	m_staNickName.MoveWindow(80, 60, rcClient.Width()-102, 20, TRUE);

	m_btnSign.SetButtonType(SKIN_ICON_BUTTON);
	m_btnSign.SetTransparent(TRUE, hDlgBgDC);
	m_btnSign.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL, CRect(3,0,3,0));
	m_btnSign.SetTextAlign(DT_LEFT);
	m_btnSign.SubclassWindow(GetDlgItem(ID_BTN_SIGN));
	m_btnSign.MoveWindow(80, 60, rcClient.Width()-102, 22, TRUE);
	m_btnSign.SetWindowText(_T("��һ����,ʲô��û�����¡�"));
	m_btnSign.SetToolTipText(_T("��һ����,ʲô��û�����¡�"));

	HFONT hFontSign = CGDIFactory::GetFont(19); 
	m_btnSign.SetFont(hFontSign);
	m_edtSign.SetBgNormalPic(_T("SignEditBg.png"), CRect(1,1,1,1));
	m_edtSign.SubclassWindow(GetDlgItem(ID_EDIT_SIGN));
	m_edtSign.MoveWindow(80, 60, rcClient.Width()-102, 22, TRUE);
	m_edtSign.ShowWindow(SW_HIDE);
	m_edtSign.SetFont(hFontSign);

	m_SkinMenu.SetBgPic(_T("Menu\\menu_left_bg.png"), _T("Menu\\menu_right_bg.png"));
	m_SkinMenu.SetSelectedPic(_T("Menu\\menu_selected.png"));
	m_SkinMenu.SetSepartorPic(_T("Menu\\menu_separtor.png"));
	m_SkinMenu.SetArrowPic(_T("Menu\\menu_arrow.png"));
	m_SkinMenu.SetCheckPic(_T("Menu\\menu_check.png"));
	m_SkinMenu.SetTextColor(RGB(0, 20, 35));
	m_SkinMenu.SetSelTextColor(RGB(254, 254, 254));
	m_SkinMenu.LoadMenu(ID_MENU_MAIN_PANEL);

	DWORD dwMenuIDs[] = {ID_MENU_IMONLINE, ID_MENU_QME, ID_MENU_AWAY, 
		ID_MENU_BUSY, ID_MENU_MUTE, ID_MENU_INVISIBLE, ID_MENU_IMOFFLINE, 
		ID_MENU_LOCK, ID_MENU_GROUP_HOMEPAGE};
	CString strFileNames[] = {_T("Status\\imonline.png"), _T("Status\\Qme.png"), 
		_T("Status\\away.png"), _T("Status\\busy.png"), _T("Status\\mute.png"), 
		_T("Status\\invisible.png"), _T("Status\\imoffline.png"), _T("lock20.png"),
		_T("groupmainpage.png")};

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_STATUS_SUBMENU_INDEX);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_TRAYICON_SUBMENU2_INDEX);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(3);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(4);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(5);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(6);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));

	//PopupMenu = m_SkinMenu.GetSubMenu(0);

	//m_btnStatus.SetButtonType(SKIN_MENU_BUTTON);
	//m_btnStatus.SetTransparent(TRUE, hDlgBgDC);
	//m_btnStatus.SetBgPic(NULL, _T("menubtn_highlight.png"), _T("menubtn_highlight.png"), NULL);
	//m_btnStatus.SetArrowPic(_T("MainPanel_FolderNode_expandTexture.png"));
	//m_btnStatus.SetIconPic(_T("Status\\imonline.png"));
	//m_btnStatus.SubclassWindow(GetDlgItem(ID_BTN_STATUS));
	//m_btnStatus.MoveWindow(54, 30, 35, 22, TRUE);
	//m_btnStatus.SetMenu(PopupMenu.m_hMenu);

	m_edtSearch.SetBgNormalPic(_T("SearchBar\\bg.png"), CRect(0,0,0,0));
	m_edtSearch.SetIconPic(_T("SearchBar\\main_search_normal.png"));
	m_edtSearch.SetDefaultText(_T("��������ϵ�ˡ������顢Ⱥ"));
	m_edtSearch.SubclassWindow(GetDlgItem(ID_EDIT_SEARCH));
	m_edtSearch.MoveWindow(0, 126, rcClient.Width(), 28, FALSE);

	m_btnMail.SetButtonType(SKIN_ICON_BUTTON);
	m_btnMail.SetTransparent(TRUE, hDlgBgDC);
	m_btnMail.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnMail.SetIconPic(_T("MidToolBar\\aio_quickbar_msglog.png"));
	m_btnMail.SubclassWindow(GetDlgItem(IDC_BTN_MAIL));
	m_btnMail.MoveWindow(10, 100, 22, 20, TRUE);

    CIniFile iniFile;
    CString strIniFile;
    strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
    TCHAR szMyWebsiteLinkTooltip[32];
    memset(szMyWebsiteLinkTooltip, 0, sizeof(szMyWebsiteLinkTooltip));
    iniFile.ReadString(_T("ui"), _T("mywebsitetooltiptext"), UI_DEFAULT_MYWEBSITE_TOOLTIP, szMyWebsiteLinkTooltip, ARRAYSIZE(szMyWebsiteLinkTooltip), strIniFile);
    m_btnMail.SetToolTipText(szMyWebsiteLinkTooltip);
	
	//m_btnMsgBox.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnMsgBox.SetTransparent(TRUE, hDlgBgDC);
	//m_btnMsgBox.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnMsgBox.SetIconPic(_T("MsgBox.png"));
	//m_btnMsgBox.SubclassWindow(GetDlgItem(ID_BTN_MSG_BOX));
	//m_btnMsgBox.MoveWindow(rcClient.Width()-48, 80, 22, 20, TRUE);
	//m_btnMsgBox.SetToolTipText(_T("����Ϣ����"));

	//m_btnChangeExterior.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnChangeExterior.SetTransparent(TRUE, hDlgBgDC);
	//m_btnChangeExterior.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnChangeExterior.SetIconPic(_T("colour.png"));
	//m_btnChangeExterior.SubclassWindow(GetDlgItem(ID_BTN_CHANGE_EXTERIOR));
	//m_btnChangeExterior.MoveWindow(rcClient.Width()-26, 80, 20, 20, TRUE);
	//m_btnChangeExterior.SetToolTipText(_T("�������"));

	//m_btnAppMgr.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnAppMgr.SetTransparent(TRUE, hDlgBgDC);
	//m_btnAppMgr.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnAppMgr.SetIconPic(_T("appbox_mgr_btn.png"));
	//m_btnAppMgr.SubclassWindow(GetDlgItem(ID_BTN_APP_MGR));
	//m_btnAppMgr.MoveWindow(rcClient.Width()-38, rcClient.bottom-60, 22, 22, TRUE);
	//m_btnAppMgr.SetToolTipText(_T("��Ӧ�ù�����"));

	m_picLogining.SetTransparent(TRUE, hDlgBgDC);
	m_picLogining.SubclassWindow(GetDlgItem(ID_PIC_LOGINING));
	m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150, TRUE);

	m_staUTalkNum.SetTransparent(TRUE, hDlgBgDC);
	m_staUTalkNum.SubclassWindow(GetDlgItem(ID_STATIC_UTalkNUM));
	m_staUTalkNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14, FALSE);

	m_staLogining.SetTransparent(TRUE, hDlgBgDC);
	m_staLogining.SubclassWindow(GetDlgItem(ID_STATIC_LOGINING));
	m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16, FALSE);

	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"),
		_T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	//m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(ID_BTN_CANCEL));
	m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30, TRUE);
	
	m_btnUnlock.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnUnlock.SetTransparent(TRUE, hDlgBgDC);
	m_btnUnlock.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"),_T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	m_btnUnlock.SubclassWindow(GetDlgItem(IDC_UNLOCK));
	m_btnUnlock.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30, TRUE);

	//InitTopToolBar();		// ��ʼ��Top������
	//InitBottomToolBar();	// ��ʼ��Bottom������
	InitTabCtrl();			// ��ʼ��Tab��
	InitBuddyListCtrl();	// ��ʼ�������б�ؼ�
	InitGroupListCtrl();	// ��ʼ��Ⱥ�б�ؼ�
	InitRecentListCtrl();	// ��ʼ�������ϵ���б�ؼ�

    CRect rcBuddyInfoFloatWnd(0, 0, BUDDYINFOFLOATWND_WIDTH, BUDDYINFOFLOATWND_HEIGHT);
    m_BuddyInfoFloatWnd.Create(m_hWnd, rcBuddyInfoFloatWnd, NULL, WS_POPUP);

	return TRUE;
}

void CMainDlg::UninitUI()
{
	if(m_picHead.IsWindow())
		m_picHead.DestroyWindow();

	if (m_staNickName.IsWindow())
		m_staNickName.DestroyWindow();

    if (m_MultiChatDlg.IsWindow())
        m_MultiChatDlg.DestroyWindow();
}

// ��ʾָ����庯��(bShow��TRUE��ʾ��ʾ����壬FALSE��ʾ��ʾ��¼���)
void CMainDlg::ShowPanel(BOOL bShow)
{
	int nShow = bShow ? SW_HIDE : SW_SHOW;
	m_picLogining.ShowWindow(nShow);
	m_staUTalkNum.ShowWindow(nShow);
	m_staLogining.ShowWindow(nShow);
	m_btnCancel.ShowWindow(nShow);


    nShow = bShow ? SW_SHOW : SW_HIDE;
	m_btnMail.ShowWindow(nShow);

	m_btnMainMenu.ShowWindow(nShow);

	m_btnMultiChat.ShowWindow(nShow);

	//m_btnSystemSet.ShowWindow(nShow);
	//m_btnMsgMgr.ShowWindow(nShow);
	//m_btnSafe.ShowWindow(nShow);
	m_btnFind.ShowWindow(nShow);
	//m_btnStatus.ShowWindow(nShow);
	m_edtSearch.ShowWindow(nShow);
	m_picHead.ShowWindow(nShow);
	m_staNickName.ShowWindow(nShow);
	m_btnSign.ShowWindow(nShow);
	//m_tbTop.ShowWindow(nShow);
	//m_tbBottom.ShowWindow(nShow);
	//m_btnMsgBox.ShowWindow(nShow);
	//m_btnChangeExterior.ShowWindow(nShow);
	//m_btnAppMgr.ShowWindow(nShow);;
	m_TabCtrl.ShowWindow(nShow);
	m_BuddyListCtrl.ShowWindow(nShow);


	m_btnUnlock.ShowWindow(SW_HIDE);
	
	RECT rtWindow;
	HRGN hTemp = NULL;
	if (bShow)	// ��ʾ�����
	{
		m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 135, 2, 67));
		m_picLogining.SetBitmap(NULL, TRUE);
		//StatusMenuBtn_SetIconPic(m_btnStatus, m_FMGClient.GetStatus());

		//::SetTimer(m_hWnd, ID_TIMER_DETECT_CLOSE_TO_SIDE, 500, NULL);
		//�����ȵ�����
		m_picHead.GetClientRect(&rtWindow);
		m_hHotRgn = ::CreateRectRgnIndirect(&rtWindow);

		m_edtSearch.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);
		
		m_staNickName.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_btnSign.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_TabCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_BuddyListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_GroupListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_RecentListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_btnFind.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);
		
	}
	else		// ��ʾ��¼���
	{
		m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 65, 4, 4));

		tstring strFileName = Hootina::CPath::GetAppPath() + _T("Image\\Logining.png");
		m_picLogining.SetBitmap(strFileName.c_str(), TRUE);

		m_staUTalkNum.SetWindowText(m_stAccountInfo.szUser);
		m_staLogining.SetWindowText(_T("���ڵ�¼"));
		m_staLogining.SetFocus();

		m_btnCancel.GetClientRect(&rtWindow);
		m_hHotRgn = ::CreateRectRgnIndirect(&rtWindow);
	}

	InvalidateRect(NULL, TRUE);
	
	m_SkinDlg.SetHotRegion(m_hHotRgn);

	ModifyStyleEx(WS_EX_APPWINDOW, 0);

	m_bPanelLocked = FALSE;
}

void CMainDlg::ShowLockPanel()
{
	//��ʾ���д���
	m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 65, 4, 4));
	tstring strFileName = Hootina::CPath::GetAppPath() + _T("Image\\Logining.png");
	m_picLogining.SetBitmap(strFileName.c_str(), TRUE);
	m_picLogining.ShowWindow(SW_SHOW);
	m_btnUnlock.ShowWindow(SW_SHOW);
	m_staUTalkNum.ShowWindow(SW_SHOW);
	m_staLogining.ShowWindow(SW_SHOW);

	m_staUTalkNum.SetWindowText(m_stAccountInfo.szUser);
	m_staLogining.SetWindowText(_T("������"));
	
	
	//�������д���
	m_btnCancel.ShowWindow(SW_HIDE);
	m_btnMainMenu.ShowWindow(SW_HIDE);
	//m_btnSystemSet.ShowWindow(SW_HIDE);
	m_btnFind.ShowWindow(SW_HIDE);
	m_edtSearch.ShowWindow(SW_HIDE);
	m_picHead.ShowWindow(SW_HIDE);
	m_staNickName.ShowWindow(SW_HIDE);
	m_btnSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_HIDE);
    m_btnMultiChat.ShowWindow(SW_HIDE);

    m_btnMail.ShowWindow(SW_HIDE);

	m_TabCtrl.ShowWindow(SW_HIDE);
	m_RecentListCtrl.ShowWindow(SW_HIDE);
	m_BuddyListCtrl.ShowWindow(SW_HIDE);
	m_GroupListCtrl.ShowWindow(SW_HIDE);

	if(m_pFindFriendDlg!=NULL && m_pFindFriendDlg->IsWindow())
		m_pFindFriendDlg->ShowWindow(SW_HIDE);

	if(m_MsgTipDlg.IsWindow())
	{
		m_MsgTipDlg.ShowWindow(SW_HIDE);
	}
	std::map<UINT, CBuddyChatDlg*>::iterator iter = m_mapBuddyChatDlg.begin();
	for(;iter!=m_mapBuddyChatDlg.end(); ++iter)
	{
		if(iter->second!=NULL && iter->second->IsWindow())
			iter->second->ShowWindow(SW_HIDE);
	}

	std::map<UINT, CGroupChatDlg*>::iterator iter2 = m_mapGroupChatDlg.begin();
	for(;iter2!=m_mapGroupChatDlg.end(); ++iter2)
	{
		if(iter2->second!=NULL && iter2->second->IsWindow())
			iter2->second->ShowWindow(SW_HIDE);
	}

	std::map<UINT, CSessChatDlg*>::iterator iter3 = m_mapSessChatDlg.begin();
	for(;iter3!=m_mapSessChatDlg.end(); ++iter3)
	{
		if(iter3->second!=NULL && iter3->second->IsWindow())
			iter3->second->ShowWindow(SW_HIDE);
	}


	InvalidateRect(NULL, TRUE);

	m_bPanelLocked = TRUE;
}


bool CMainDlg::StartLogin(BOOL bAutoLogin/* = FALSE*/)
{
	m_FMGClient.SetCallBackWnd(m_hWnd);
	if (bAutoLogin)
	{
//���԰�Ͳ�Ҫ������
#ifndef _DEBUG
        Updater updater(&m_FMGClient.m_FileTask);
		//updater.m_lpProcotol = &m_FMGClient.m_IUProtocol;

		//��Ҫ����
		if(CheckOnlyOneInstance() && updater.IsNeedUpdate())
		{
			if(IDYES == ::MessageBox(m_hWnd, _T("��⵽�°汾���Ƿ����ھ�������"), _T("�°汾��ʾ"), MB_YESNO|MB_ICONQUESTION|MB_TOPMOST))
			{
				CUpdateDlg updateDlg;
				updateDlg.m_pFMGClient =  &m_FMGClient;
				updateDlg.m_aryFileInfo = updater.m_aryUpdateFileList;
				//updateDlg.m_lpProtocol = &m_FMGClient.m_IUProtocol;
				updateDlg.DoModal(m_hWnd, NULL);
			}
		}
#endif
		
		BOOL bRet = m_LoginAccountList.GetLastLoginAccountInfo(&m_stAccountInfo);
		if (!bRet)
			return true;

        m_LoginDlg.SetLoginAccountInfo(&m_stAccountInfo);
        m_LoginDlg.DoLogin();
	}
	else
	{
		ShowWindow(SW_HIDE);
		LoadAppIcon(m_FMGClient.GetStatus());
		m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("Flamingoδ��¼"));

		//ShowWindow(SW_HIDE);
		//ShowWindow(SW_SHOW);

		m_LoginDlg.m_lpFMGClient = &m_FMGClient;
		m_LoginDlg.m_pLoginAccountList = &m_LoginAccountList;
		m_LoginDlg.SetDefaultAccount(m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		m_LoginDlg.SetDefaultPassword(m_FMGClient.m_UserMgr.m_UserInfo.m_strPassword.c_str());
		if (m_LoginDlg.DoModal(g_hwndOwner) != IDOK)	// ��ʾ��¼�Ի���
		{
			CloseDialog(IDOK);
			return false;
		}
		m_LoginDlg.GetLoginAccountInfo(&m_stAccountInfo);
	}

    m_nMainPanelStatus = MAINPANEL_STATUS_LOGINING;
    
    ShowPanel(FALSE);		// ��ʾ��¼���
	ShowWindow(SW_SHOW);

	LoadLoginIcon();
	m_dwLoginTimerId = SetTimer(LOGIN_TIMER_ID, 200, NULL);

	m_FMGClient.SetUser(m_stAccountInfo.szUser, m_stAccountInfo.szPwd);
	
    //�Ѿ��ĳ�ͬ����¼��
	//m_FMGClient.Login();

    return true;
}

LRESULT CMainDlg::OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// wParamΪuID, lParamΪ�����Ϣ
	m_TrayIcon.OnTrayIconNotify(wParam, lParam);


	UINT uID = (UINT)wParam;
	UINT uIconMsg = (UINT)lParam;

	switch (uIconMsg)
	{
	case WM_LBUTTONUP:
		OnTrayIcon_LButtunUp();
		break;
	case WM_RBUTTONUP:
		OnTrayIcon_RButtunUp();
		break;
	case WM_MOUSEHOVER:
		OnTrayIcon_MouseHover();
		break;
	case WM_MOUSELEAVE:
		OnTrayIcon_MouseLeave();
		break;
	}
	return 0;
}

void CMainDlg::OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
	{
		m_LoginDlg.ShowWindow(SW_SHOW);
		m_LoginDlg.SetFocus();
	}
	else if (IsWindow())
	{
		ShowWindow(SW_SHOW);
		SetFocus();
	}
}

void CMainDlg::OnMenu_LockPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ShowLockPanel();
}

void CMainDlg::OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
		m_LoginDlg.PostMessage(WM_CLOSE);
	else if (IsWindow())
	{
		//PostMessage(WM_CLOSE);
		CloseDialog(IDOK);
	}
}

void CMainDlg::OnMenu_Mute(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    m_FMGClient.m_UserConfig.EnableMute(!m_FMGClient.m_UserConfig.IsEnableMute());
}

//������ĸ���ǩ��
void CMainDlg::OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_btnSign.GetWindowText(strText);
	m_edtSign.SetWindowText(strText);

	m_btnSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_SHOW);

	m_edtSign.SetSel(0, -1);
	m_edtSign.SetFocus();
}

void CMainDlg::OnBtn_UnlockPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ShowPanel(TRUE);
	//�ָ�ԭ���Ĳ˵�
}

// ���˵���ťѹ��
void CMainDlg::OnBtn_MainMenu(UINT uNotifyCode, int nId, CWindow wndCtl)
{
	CSkinMenu PopupMenu;
	CRect rcItem;
	GetClientRect(&rcItem);

	PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_MAIN_SUBMENU_INDEX);
	if (PopupMenu.IsMenu())
	{
		m_SkinDlg.ClientToScreen(&rcItem);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rcItem.right-rcItem.Width(), rcItem.bottom - 100, m_hWnd, &rcItem);
	}
}

void CMainDlg::OnBtn_MultiChat(UINT uNotifyCode, int nId, CWindow wndCtl)
{
	CMultiChatMemberSelectionDlg multiChatMemberSelectionDlg;
	multiChatMemberSelectionDlg.m_pFMGClient = &m_FMGClient;
	if(multiChatMemberSelectionDlg.DoModal(NULL, NULL) != IDOK)
		return;

	//����Ⱥ�����촰��
	if(multiChatMemberSelectionDlg.m_setSelectedIDs.empty())
		return;

	if(m_MultiChatDlg.IsWindow())
	{
		m_MultiChatDlg.DestroyWindow();	
	}
	m_MultiChatDlg.m_setTargetIDs = multiChatMemberSelectionDlg.m_setSelectedIDs;
	m_MultiChatDlg.Create(NULL);
	m_MultiChatDlg.ShowWindow(SW_NORMAL);
	::SetForegroundWindow(m_MultiChatDlg.m_hWnd);
	m_MultiChatDlg.SetFocus();
}

//������Ӻ��ѶԻ���
void CMainDlg::OnBtn_ShowAddFriendDlg(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// ��ʾ���Һ��ѶԻ���
	if(m_pFindFriendDlg == NULL)
		return;
	if(m_pFindFriendDlg->IsWindow())
	{
		m_pFindFriendDlg->ShowWindow(SW_SHOWNORMAL);	
		m_pFindFriendDlg->SetFocus();
	}
	else
	{
		m_pFindFriendDlg->m_pFMGClient = &m_FMGClient;
		if (m_pFindFriendDlg->Create(m_hWnd, NULL) == NULL)
			return;
		m_pFindFriendDlg->ShowWindow(SW_SHOWNORMAL);	
	}

	//if(m_pAddFriendConfirmDlg == NULL)
	//	return;
	//if(m_pAddFriendConfirmDlg->IsWindow())
	//{
	//	m_pAddFriendConfirmDlg->ShowWindow(SW_SHOWNORMAL);	
	//	m_pAddFriendConfirmDlg->SetFocus();
	//}
	//else
	//{
	//	m_pAddFriendConfirmDlg->m_pFMGClient = &m_FMGClient;
	//	if (m_pAddFriendConfirmDlg->Create(m_hWnd, NULL) == NULL)
	//		return;
	//	m_pAddFriendConfirmDlg->ShowWindow(SW_SHOWNORMAL);	
	//}
}

void CMainDlg::OnBtn_ShowSystemSettingDlg(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CSystemSettingDlg systemSettingDlg;
	systemSettingDlg.m_pFMGClient = &m_FMGClient;

	systemSettingDlg.DoModal(m_hWnd, NULL);
}

void CMainDlg::OnBtn_OpenMail(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CIniFile iniFile;
    CString strIniFile;
    strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
    TCHAR szMyWebsiteLink[512];
    memset(szMyWebsiteLink, 0, sizeof(szMyWebsiteLink));
    iniFile.ReadString(_T("ui"), _T("mywebsitelink"), UI_DEFAULT_MYWEBSITE, szMyWebsiteLink, ARRAYSIZE(szMyWebsiteLink), strIniFile);

    ::ShellExecute(NULL, _T("open"), szMyWebsiteLink, NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlg::OnBtn_ModifyPassword(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_ModifyPasswordDlg.m_pFMGClient = &m_FMGClient;
	if(!m_ModifyPasswordDlg.IsWindow())
		m_ModifyPasswordDlg.Create(NULL);
	
	m_ModifyPasswordDlg.ShowWindow(SW_SHOW);
	::SetForegroundWindow(m_ModifyPasswordDlg.m_hWnd);
	m_ModifyPasswordDlg.SetFocus();
}


LRESULT CMainDlg::OnTabCtrlDropDown(LPNMHDR pnmh)
{
	CSkinMenu PopupMenu;
	CRect rcItem;
	int nCurSel, nIndex;

	nCurSel = m_TabCtrl.GetCurSel();

	switch (nCurSel)
	{
	case 0:
		nIndex = MAIN_PANEL_RECENT_SUBMENU_INDEX;
		break;

	case 1:
		nIndex = MAIN_PANEL_BUDDYLIST_SUBMENU_INDEX;
		break;

	case 2:
		nIndex = MAIN_PANEL_GROUP_SUBMENU_INDEX;
		break;

	case 4:
		nIndex = 6;
		break;

	default:
		return 0;
	}

	PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	if (!PopupMenu.IsMenu())
		return 0;

	m_TabCtrl.GetItemRectByIndex(nCurSel, rcItem);
	m_TabCtrl.ClientToScreen(&rcItem);
	
	if(m_nBuddyListHeadPicStyle == BLC_BIG_ICON_STYLE)
	{
		PopupMenu.CheckMenuItem(ID_MENU_BIGHEADPIC, MF_CHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_SMALLHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_STDHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
	}
	else if(m_nBuddyListHeadPicStyle == BLC_SMALL_ICON_STYLE)
	{
		PopupMenu.CheckMenuItem(ID_MENU_BIGHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_SMALLHEADPIC, MF_CHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_STDHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
	}
	else if(m_nBuddyListHeadPicStyle == BLC_STANDARD_ICON_STYLE)
	{
		PopupMenu.CheckMenuItem(ID_MENU_BIGHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_SMALLHEADPIC, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_MENU_STDHEADPIC, MF_CHECKED|MF_BYCOMMAND);
	}
	
	if(m_bShowBigHeadPicInSel)
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_CHECKED|MF_BYCOMMAND);
	else
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);

	//��ʾ�ǳƺ��˻�����ʾ�ǳơ���ʾ�˻������Ӳ˵���
	long nNameStyle = m_FMGClient.m_UserConfig.GetNameStyle();
	if(nNameStyle == NAME_STYLE_SHOW_NICKNAME_AND_ACCOUNT)
	{
		PopupMenu.CheckMenuItem(ID_32911, MF_CHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32912, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32913, MF_UNCHECKED|MF_BYCOMMAND);
	}
	else if(nNameStyle == NAME_STYLE_SHOW_NICKNAME)
	{
		PopupMenu.CheckMenuItem(ID_32911, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32912, MF_CHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32913, MF_UNCHECKED|MF_BYCOMMAND);
	}
	else
	{	
		PopupMenu.CheckMenuItem(ID_32911, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32912, MF_UNCHECKED|MF_BYCOMMAND);
		PopupMenu.CheckMenuItem(ID_32913, MF_CHECKED|MF_BYCOMMAND);
	}
	
	//��ʾ��ˬ���ϲ˵���
	PopupMenu.CheckMenuItem(ID_32914, (m_FMGClient.m_UserConfig.IsEnableSimpleProfile()?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
	
	//��ʾ������ϵ��
	PopupMenu.CheckMenuItem(IDM_SHOW_ONLINEBUDDY_ONLY, (m_bShowOnlineBuddyOnly?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
	
	
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
		rcItem.right-19, rcItem.bottom + 4, m_hWnd, &rcItem);
	

	return 0;
}

LRESULT CMainDlg::OnBuddyListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	
	//::MessageBox(m_hWnd, _T("������������Ի���"), _T(""), MB_OK);
	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nUTalkUin = m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		SendMessage(WM_SHOW_BUDDYCHATDLG, 0, nUTalkUin);
	}	
	return 0;
}

LRESULT CMainDlg::OnBuddyListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	//::SetForegroundWindow(m_hWnd);
    BLNMHDREx* hdr = (BLNMHDREx*)pnmh;
	CSkinMenu PopupMenu;
	if(hdr->nPostionFlag == POSITION_ON_ITEM)
	{
		PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_BUDDYLIST_CONTEXT_SUBMENU_INDEX);
		if(m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTeamCount() > 1)
		{
			PopupMenu.EnableMenuItem(IDM_MOVEITEM, MF_ENABLED|MF_BYCOMMAND);
			InsertTeamMenuItem(PopupMenu);
		}
		else
		{
			//TODO: �������治֧��MF_BYCOMMAND���ò˵�
			PopupMenu.EnableMenuItem(8, MF_GRAYED|MF_BYPOSITION);
		}
	}
	else if(hdr->nPostionFlag == POSITION_ON_TEAM)
	{
		PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_BUDDYLIST_TEAM_CONTEXT_SUBMENU_INDEX);
	}
	else if(hdr->nPostionFlag == POSITION_ON_BLANK)
	{
		PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_BUDDYLIST_BLANK_CONTEXT_SUBMENU_INDEX);
	}

	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return 1;
}

LRESULT CMainDlg::OnBuddyListHover(LPNMHDR pnmh)
{
    RECT rtBuddyInfoFloatWnd;
    ::GetWindowRect(m_BuddyInfoFloatWnd.m_hWnd, &rtBuddyInfoFloatWnd);
    
    BLNMHDREx* hdr = (BLNMHDREx*)pnmh;
    if (hdr == NULL || hdr->nTeamIndex < 0 || hdr->nItemIndex < 0)
    {
        //��겻�ں����б��ϣ��Ҳ��ڸ�����Ϣ�����ϲ����ظ�����Ϣ����
        POINT ptCursor;
        ::GetCursorPos(&ptCursor);
        //�������ұ߾��rect
        RECT rtBuddyInfoFloatWndPlusGap;
        rtBuddyInfoFloatWndPlusGap.top = rtBuddyInfoFloatWnd.top;
        rtBuddyInfoFloatWndPlusGap.bottom = rtBuddyInfoFloatWnd.bottom;
        rtBuddyInfoFloatWndPlusGap.left = rtBuddyInfoFloatWnd.left;
        rtBuddyInfoFloatWndPlusGap.right = rtBuddyInfoFloatWnd.right + 15;
        if (!::PtInRect(&rtBuddyInfoFloatWndPlusGap, ptCursor))
        {
            m_BuddyInfoFloatWnd.ShowWindow(SW_HIDE);
            return 0;
        }
      
        return 0;
    }

    RECT rtMainDlg;
    ::GetWindowRect(m_hWnd, &rtMainDlg);
    if (!m_BuddyInfoFloatWnd.IsWindowVisible())
        m_BuddyInfoFloatWnd.ShowWindow(SW_SHOW);   

    int xLeftBuddyInfoFloatWnd = rtMainDlg.left - rtBuddyInfoFloatWnd.right + rtBuddyInfoFloatWnd.left;
    //���������̫������Ļ��ߣ���Ϣ���ڽ���ʾ�������ڵ��ұ�
    if (rtMainDlg.left < BUDDYINFOFLOATWND_WIDTH)
        xLeftBuddyInfoFloatWnd = rtMainDlg.right;

    CString strHeadImg = m_BuddyListCtrl.GetBuddyItemHeadPic(hdr->nTeamIndex, hdr->nItemIndex);
    UINT nBuddyID = m_BuddyListCtrl.GetBuddyItemID(hdr->nTeamIndex, hdr->nItemIndex);
    m_BuddyInfoFloatWnd.SetHeadImg(strHeadImg);
    if (nBuddyID >= 0)
    {
        CBuddyInfo* pBuddyInfo = m_FMGClient.m_UserMgr.m_BuddyList.GetBuddy(nBuddyID);
        if (pBuddyInfo != NULL)
        {
            m_BuddyInfoFloatWnd.SetDataText(pBuddyInfo->m_strNickName.c_str(),
                pBuddyInfo->m_strSign.c_str(),
                pBuddyInfo->m_strEmail.c_str(),
                pBuddyInfo->m_strAddress.c_str());
        }      
    }

    POINT itemStart;
    itemStart.x = hdr->rtItem.left;
    itemStart.y = hdr->rtItem.top;
    ::ClientToScreen(m_BuddyListCtrl.m_hWnd, &itemStart);

    //TODO: ����߶�25���б���߶�55����д��,����ĳɸ����б���ʾģʽ�¼������
    //::SetWindowPos(m_BuddyInfoFloatWnd.m_hWnd,
    //               m_hWnd,
    //               xLeftBuddyInfoFloatWnd,
    //               rtMainDlg.top + 200 + (hdr->nTeamIndex + 1) * 25 + hdr->nItemIndex * 55,
    //               0,
    //               0,
    //               SWP_NOSIZE | SWP_SHOWWINDOW);

    ::SetWindowPos(m_BuddyInfoFloatWnd.m_hWnd,
        m_hWnd,
        xLeftBuddyInfoFloatWnd,
        itemStart.y,
        0,
        0,
        SWP_NOSIZE | SWP_SHOWWINDOW);




    return 1;
}

LRESULT CMainDlg::OnGroupListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nGroupCode = m_GroupListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		SendMessage(WM_SHOW_GROUPCHATDLG, nGroupCode, 0);
	}	
	return 0;
}

LRESULT CMainDlg::OnGroupListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	//::SetForegroundWindow(m_hWnd);
    BLNMHDREx* hdr = (BLNMHDREx*)pnmh;
	if(hdr->nPostionFlag == POSITION_ON_ITEM)
	{
		CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_GROUPLIST_CONTEXT_SUBMENU_INDEX);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	}
	else if(hdr->nPostionFlag==POSITION_ON_TEAM || hdr->nPostionFlag==POSITION_ON_BLANK)
	{
		CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_GROUPLIST_BLANK_CONTEXT_SUBMENU_INDEX);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	}


	return (LRESULT)1;
}

LRESULT CMainDlg::OnRecentListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	//::MessageBox(m_hWnd, _T("������������Ի���"), _T(""), MB_OK);
	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nUTalkUin = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		if(IsGroupTarget(nUTalkUin))
		{
			SendMessage(WM_SHOW_GROUPCHATDLG, (WPARAM)nUTalkUin, 0);
		}
		else
		{
			if(!m_FMGClient.m_UserMgr.IsFriend(nUTalkUin))
			{
				tstring strNickName = m_RecentListCtrl.GetBuddyItemNickName(nTeamIndex, nIndex);
				CString strInfo;
				strInfo.Format(_T("%s�Ѿ��������ĺ��ѣ����ȼӶԷ�Ϊ���Ѻ������졣"), strNickName.c_str());
				::MessageBox(m_hWnd, strInfo, g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
				return 1;
			}
			
			SendMessage(WM_SHOW_BUDDYCHATDLG, 0, nUTalkUin);
		}
	}	
	return 0;

	//int nTeamIndex, nIndex;
	//m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	//if (nTeamIndex != -1 && nIndex != -1)
	//{
	//	int nIndex2 = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);

	//	CRecentList* lpRecentList = m_FMGClient.GetRecentList();
	//	if (lpRecentList != NULL)
	//	{
	//		CRecentInfo* lpRecentInfo = lpRecentList->GetRecent(nIndex2);
	//		if (lpRecentInfo != NULL)
	//		{
	//			if (0 == lpRecentInfo->m_nType)			// ����
	//			{
	//				SendMessage(WM_SHOW_BUDDYCHATDLG, 0, lpRecentInfo->m_nUTalkUin);
	//			}
	//			else if (1 == lpRecentInfo->m_nType)	// Ⱥ
	//			{
	//				CGroupList* lpGroupList = m_FMGClient.GetGroupList();
	//				if (lpGroupList != NULL)
	//				{
	//					CGroupInfo* lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nUTalkUin);
	//					if (lpGroupInfo != NULL)
	//					{
	//						SendMessage(WM_SHOW_GROUPCHATDLG, lpGroupInfo->m_nGroupCode, 0);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//return 0;
}

LRESULT CMainDlg::OnRecentListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	//::SetForegroundWindow(m_hWnd);

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_RECENTLIST_CONTEXT_SUBMENU_INDEX);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return (LRESULT)1;
}

LRESULT CMainDlg::OnTabCtrlSelChange(LPNMHDR pnmh)
{
	int nCurSel = m_TabCtrl.GetCurSel();
	switch (nCurSel)
	{
	case 0:
		m_RecentListCtrl.ShowWindow(SW_SHOW);
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_RecentListCtrl.SetBuddyTeamExpand(0, TRUE);
		//m_RecentListCtrl.SetBuddyListCtrlType(BUDDY_LIST_TYPE_RECENT);
		m_RecentListCtrl.SetFocus();
		break;

	case 1:
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.ShowWindow(SW_SHOW);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.SetFocus();
		break;

	case 2:
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.ShowWindow(SW_HIDE);	
		m_GroupListCtrl.ShowWindow(SW_SHOW);
		m_GroupListCtrl.SetFocus();
		break;
	}

	m_nCurSelIndexInMainTab = nCurSel;

	return 0;
}

void CMainDlg::OnRefreshBuddyList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//m_FMGClient.StartGetUserInfoTask(USER_INFO_TYPE_SELF);
	m_FMGClient.GetFriendList();
}

void CMainDlg::OnShowOnlineBuddyOnly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bShowOnlineBuddyOnly = !m_bShowOnlineBuddyOnly;
	::SendMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

void CMainDlg::OnMainMenu_About(UINT uNotifyCode, int nID, CWindow wndCtl)
{

}

// ��ȡ������ť(ȡ����¼)
void CMainDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_FMGClient.CancelLogin();
	CloseDialog(IDCANCEL);
	CString strMainExe = g_szHomePath;
	strMainExe.Format(_T("%sFlamingo.exe"), g_szHomePath);
	if((int)::ShellExecute(NULL, NULL, strMainExe.GetString(), NULL, NULL, SW_SHOW) <= 32)
	{
		::MessageBox(NULL, _T("��ʾ��¼�Ի���ʧ�ܣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
	}
	//CLoginDlg LoginDlg;
	//if (LoginDlg.DoModal() != IDOK)	// ��ʾ��¼�Ի���
	//{
	//	EndDialog(LoginDlg.m_hWnd, IDOK);
	//	return;
	//}
}

// ���û�ͷ�񡱿ؼ�
void CMainDlg::OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_LogonUserInfoDlg.m_pFMGClient = &m_FMGClient;
	if(!m_LogonUserInfoDlg.IsWindow())
		m_LogonUserInfoDlg.Create(NULL);
	
	m_LogonUserInfoDlg.ShowWindow(SW_SHOW);
	::SetForegroundWindow(m_LogonUserInfoDlg.m_hWnd);
}

// ����ͷ�񡱲˵�
void CMainDlg::OnMenu_BigHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl* lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (1 == nCurSel)
	{
		nIndex = 4;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		//lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_BIG_ICON_STYLE)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, ID_MENU_STDHEADPIC, ID_MENU_BIGHEADPIC, MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_GRAYED|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_BIG_ICON_STYLE);
	m_nBuddyListHeadPicStyle = BLC_BIG_ICON_STYLE;
	lpBuddyListCtrl->Invalidate();
}

// ��Сͷ�񡱲˵�
void CMainDlg::OnMenu_SmallHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	UINT nCheck;
	CBuddyListCtrl* lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (1 == nCurSel)
	{
		nIndex = 4;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (4 == nCurSel)
	{
		nIndex = 5;
		//lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_SMALL_ICON_STYLE)
		return;

	nCheck = lpBuddyListCtrl->IsShowBigIconInSel() ? MF_CHECKED : MF_UNCHECKED;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, 
		ID_MENU_STDHEADPIC, ID_MENU_SMALLHEADPIC, MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_ENABLED|MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, nCheck|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_SMALL_ICON_STYLE);
	m_nBuddyListHeadPicStyle = BLC_SMALL_ICON_STYLE;
	lpBuddyListCtrl->Invalidate();
}

// ����׼ͷ�񡱲˵�
void CMainDlg::OnMenu_StdHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl* lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (1 == nCurSel)
	{
		nIndex = 4;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		//lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_STANDARD_ICON_STYLE)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, ID_MENU_STDHEADPIC, ID_MENU_STDHEADPIC, MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_GRAYED|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_STANDARD_ICON_STYLE);
	m_nBuddyListHeadPicStyle = BLC_STANDARD_ICON_STYLE;
	lpBuddyListCtrl->Invalidate();
}

// ��ѡ��ʱ��ʾ��ͷ�񡱲˵�
void CMainDlg::OnMenu_ShowBigHeadPicInSel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl* lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (1 == nCurSel)
	{
		nIndex = 4;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		//lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	UINT nState = PopupMenu.GetMenuState(ID_MENU_SHOWBIGHEADPICINSEL, MF_BYCOMMAND);
	if (nState & MF_CHECKED)
	{
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
		lpBuddyListCtrl->SetShowBigIconInSel(FALSE);
		m_bShowBigHeadPicInSel = FALSE;
	}
	else
	{
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_CHECKED|MF_BYCOMMAND);
		lpBuddyListCtrl->SetShowBigIconInSel(TRUE);
		m_bShowBigHeadPicInSel = TRUE;
	}
	lpBuddyListCtrl->Invalidate();
}

// ������ǩ�����༭�ı���
void CMainDlg::OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strOldText, strNewText;
	m_btnSign.GetWindowText(strOldText);
	m_edtSign.GetWindowText(strNewText);

	strNewText.Trim();
	if(strNewText.GetLength() > 127)
	{
		::MessageBox(m_hWnd, _T("����ǩ�����ܳ���127���ַ���"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	BOOL bModify = TRUE;
	if(m_FMGClient.IsOffline())
	{
		::MessageBox(m_hWnd, _T("����ǰ��������״̬���޷��޸ĸ���ǩ���������ߺ����ԡ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if(!strOldText.IsEmpty() && strNewText.IsEmpty())
		{
			if(::MessageBox(m_hWnd, _T("ȷ��Ҫ���ĸ���ǩ����"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION) != IDYES)
			{
				bModify = FALSE;
			}
		}

		if(bModify && strOldText!=strNewText)
		{
			m_FMGClient.ModifyUTalkSign(strNewText);
			m_btnSign.SetWindowText(strNewText);
		}
	}

	if(!bModify)
		m_btnSign.SetWindowText(strOldText);
	
	m_edtSign.ShowWindow(SW_HIDE);
	m_btnSign.ShowWindow(SW_SHOW);
}

// ���û�����״̬���˵�
void CMainDlg::OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	long nNewStatus = GetStatusFromMenuID(nID);
	//�����Ҫ���ĵ�Ŀ��״̬�뵱ǰ״̬һ����������
	if (nNewStatus == m_FMGClient.GetStatus())	
		return;


	//m_FMGClient.ChangeStatus(nNewStatus);

    if (nNewStatus == STATUS_OFFLINE)
    {
        m_FMGClient.GoOffline();
        //������־����
        m_nMainPanelStatus = MAINPANEL_STATUS_USERGOOFFLINE;
    }
	else if(nNewStatus == STATUS_ONLINE)
		m_FMGClient.GoOnline();

	//StatusMenuBtn_SetIconPic(m_btnStatus, nNewStatus);	
}

void CMainDlg::OnBuddyListAddTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if(m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTeamCount() > 20)
	{
		::MessageBox(m_hWnd, _T("�����������ܳ���20����"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	CTeamDlg newTeamDlg;
	newTeamDlg.SetType(TEAM_OPERATION_ADDTEAM);
	newTeamDlg.m_pFMGClient = &m_FMGClient;
	if(newTeamDlg.DoModal(m_hWnd, NULL) != IDOK)
		return;

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

void CMainDlg::OnBuddyListDeleteTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nBuddyIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if(nTeamIndex<0 || nBuddyIndex>0)
		return;

	if(m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTeamCount() <= 1)
	{
		::MessageBox(m_hWnd, _T("���ٵ���һ�����顣"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

    CString strTeamName = m_BuddyListCtrl.GetBuddyTeamName(nTeamIndex);
    if (strTeamName == DEFAULT_TEAMNAME)
    {
        ::MessageBox(m_hWnd, _T("Ĭ�Ϸ��鲻��ɾ����"), g_strAppTitle.c_str(), MB_ICONINFORMATION | MB_OK);
        return;
    }

	if(IDYES != ::MessageBox(m_hWnd, _T("ɾ���÷����Ժ󣬸����µĺ��ѽ�����Ĭ�Ϸ����С�\nȷ��Ҫɾ���÷�����"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION))
		return;

    m_FMGClient.DeleteTeam(strTeamName);   
}

void CMainDlg::OnBuddyListModifyTeamName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nBuddyIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if(nTeamIndex<0 || nBuddyIndex>0)
		return;

	CTeamDlg newTeamDlg;
	newTeamDlg.SetType(TEAM_OPERATION_MODIFYTEAMNAME);
	newTeamDlg.m_pFMGClient = &m_FMGClient;
	newTeamDlg.m_nTeamIndex = nTeamIndex;
	if(newTeamDlg.DoModal(m_hWnd, NULL) != IDOK)
		return;

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

void CMainDlg::OnMoveBuddyToTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex;
	int nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if(nTeamIndex<0 || nIndex<0)
		return;

	UINT uUserID = (UINT)m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	if(uUserID == 0)
		return;

	CBuddyInfo* pBuddyInfo = NULL;
	CBuddyTeamInfo* pTeamInfo = m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTeamByIndex(nTeamIndex);
	if(pTeamInfo == NULL)
		return;

    CString strOldTeamName;
	//��ȡԭ������
	for(std::vector<CBuddyInfo*>::iterator iter=pTeamInfo->m_arrBuddyInfo.begin(); iter!=pTeamInfo->m_arrBuddyInfo.end(); ++iter)
	{
		pBuddyInfo =*iter;
		if(pBuddyInfo!=NULL && pBuddyInfo->m_uUserID == uUserID)
		{
            strOldTeamName = pTeamInfo->m_strName.c_str();
			break;
		}
	}
	
	//��ȡĿ�������
	long nTargetIndex = nID-TEAM_MENU_ITEM_BASE;
	CBuddyTeamInfo* pTargetTeamInfo = m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTeamByIndex(nTargetIndex);
	if(pTargetTeamInfo == NULL)
		return;
    CString strNewTeamName = pTargetTeamInfo->m_strName.c_str();
	
    m_FMGClient.MoveFriendToOtherTeam(uUserID, strOldTeamName, strNewTeamName);
}

//�Ҽ��˵�ɾ������
void CMainDlg::OnMenu_DeleteFriend(UINT uNotifyCode, int nID, CWindow wndCtl)
{

	int nTeamIndex, nBuddyIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if(nTeamIndex==-1 || nBuddyIndex==-1)
	{
		::MessageBox(m_hWnd, _T("������Ҫɾ���ĺ����ϰ����Ҽ��˵���"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString strNickName(m_BuddyListCtrl.GetBuddyItemNickName(nTeamIndex, nBuddyIndex));
	CString strMarkName(m_BuddyListCtrl.GetBuddyItemMarkName(nTeamIndex, nBuddyIndex));

	CString strInfo;
	strInfo.Format(_T("ȷ��Ҫɾ��%s(%s)��"), strMarkName, strNickName);
	int nRet = ::MessageBox(m_hWnd, strInfo, g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION);
	if (IDNO == nRet)
		return;

	UINT uUserID = (UINT)m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nBuddyIndex);	
	m_FMGClient.DeleteFriend(uUserID);
}

void CMainDlg::OnClearAllRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nRet = ::MessageBox(m_hWnd, _T("��ջỰ�б���޷��ָ���ȷ��Ҫ��ջỰ�б���"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONWARNING);
	if(nRet != IDYES)
		return;

	m_FMGClient.m_UserMgr.ClearRecentList();
	::PostMessage(m_hWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
}

void CMainDlg::OnDeleteRecentItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nRecentIndex;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nRecentIndex);
	if(nTeamIndex==-1 || nRecentIndex==-1)
	{
		::MessageBox(m_hWnd, _T("������Ҫɾ���ĻỰ�ϰ����Ҽ��˵���"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	int nRet = ::MessageBox(m_hWnd, _T("ȷ��Ҫɾ���ûỰ��"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION);
	if (IDNO == nRet)
		return;

	UINT uUserID = (UINT)m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nRecentIndex);	
	m_FMGClient.m_UserMgr.DeleteRecentItem(uUserID);
	::PostMessage(m_hWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
}

void CMainDlg::OnMenu_SendBuddyMessage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnBuddyListDblClk(NULL);
}

void CMainDlg::OnMenu_SendBuddyMessageFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnRecentListDblClk(NULL);
}

void CMainDlg::OnMenu_ViewBuddyInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex<0 || nIndex<0)
		return;

	UINT nUTalkUin = m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	::PostMessage(m_hWnd, WM_SHOW_BUDDYINFODLG, NULL, nUTalkUin);
}

void CMainDlg::OnMenu_ViewBuddyInfoFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex<0 || nIndex<0)
		return;

	UINT nUTalkUin = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	::PostMessage(m_hWnd, WM_SHOW_BUDDYINFODLG, NULL, nUTalkUin);
}

void CMainDlg::OnMenu_ModifyBuddyMarkName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex<0 || nIndex<0)
		return;

	UINT uUserID = m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	if(uUserID == 0)
		return;

	CModifyMarkNameDlg modifyMarkNameDlg;
	modifyMarkNameDlg.m_uUserID = uUserID;
	modifyMarkNameDlg.m_pFMGClient = &m_FMGClient;
	if(modifyMarkNameDlg.DoModal(m_hWnd, NULL) != IDOK)
		return;

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

void CMainDlg::OnMenu_ShowNameChoice(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch(nID)
	{
	//��ʾ�ǳƺ��˺�
	case ID_32911:
		m_FMGClient.m_UserConfig.SetNameStyle(NAME_STYLE_SHOW_NICKNAME_AND_ACCOUNT);
		break;
	//��ʾ�ǳ�
	case ID_32912:
		m_FMGClient.m_UserConfig.SetNameStyle(NAME_STYLE_SHOW_NICKNAME);
		break;
	//��ʾ�˺�
	case ID_32913:
		m_FMGClient.m_UserConfig.SetNameStyle(NAME_STYLE_SHOW_ACCOUNT);
		break;
	
	//��ʾ��ˬ����
	case ID_32914:
		m_FMGClient.m_UserConfig.EnableSimpleProfile(!m_FMGClient.m_UserConfig.IsEnableSimpleProfile());
		break;

	default:
		break;
	}

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

void CMainDlg::OnMenu_SendGroupMessage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnGroupListDblClk(NULL);
}

void CMainDlg::OnMenu_ViewGroupInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nGroupCode = m_GroupListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		::PostMessage(m_hWnd, WM_SHOW_GROUPINFODLG, nGroupCode, NULL);
	}		
}

void CMainDlg::OnMenu_ExitGroup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nGroupCode = m_GroupListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		CString strPromptInfo;
		strPromptInfo.Format(_T("ȷʵҪ�˳�Ⱥ[%s(%s)]��"), m_GroupListCtrl.GetBuddyItemMarkName(nTeamIndex, nIndex), m_GroupListCtrl.GetBuddyItemNickName(nTeamIndex, nIndex));
		if(IDNO == ::MessageBox(m_hWnd, strPromptInfo, g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION))
			return;

		m_FMGClient.DeleteFriend(nGroupCode);
	}		
}

void CMainDlg::OnMenu_CreateNewGroup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CCreateNewGroupDlg dlg;
	dlg.m_pFMGClient = &m_FMGClient;
	if(IDOK != dlg.DoModal(NULL, NULL))
		return;
}

LRESULT CMainDlg::OnNetError(UINT uMsg, WPARAM wParam, LPARAM lParam)
{        
    //������ʱ���Ѿ�������ֱ���˳��������ظ�����
    if (m_dwReconnectTimerId != 0)
        return 0;
    
    m_FMGClient.m_UserMgr.ResetToOfflineStatus();

    ::SendMessage(m_FMGClient.m_UserMgr.m_hCallBackWnd, FMG_MSG_UPDATE_BUDDY_INFO, 0, (LPARAM)(m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID));
    ::SendMessage(m_FMGClient.m_UserMgr.m_hCallBackWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
    ::SendMessage(m_FMGClient.m_UserMgr.m_hCallBackWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
    ::SendMessage(m_FMGClient.m_UserMgr.m_hCallBackWnd, FMG_MSG_UPDATE_GROUP_LIST, 0, 0);

    //û�п������������û���������Ҳ��Ҫ����
    if (m_bEnableReconnect && m_nMainPanelStatus != MAINPANEL_STATUS_USERGOOFFLINE)
    {
        m_nMainPanelStatus = MAINPANEL_STATUS_RECONNECTING;
        m_dwReconnectTimerId = SetTimer(RECONNECT_TIMER_ID, m_uReconnectInterval, NULL);   
    }
    
    return 1;
}

// ��¼������Ϣ
LRESULT CMainDlg::OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    long nCode = (long)lParam;
    
    //�������ɹ�������Է��͵�¼�������ʶ���ٴη���������¼���ݰ�
    if (m_nMainPanelStatus == MAINPANEL_STATUS_RECONNECTING && nCode != LOGIN_SUCCESS)
    {
        m_bAlreadySendReloginRequest = false;
        return 0;
    }
    
 	KillTimer(m_dwLoginTimerId);
 	m_dwLoginTimerId = NULL;

	long nStatus = m_FMGClient.GetStatus();

 	LoadAppIcon(nStatus);
	
	switch (nCode)
	{
	case LOGIN_SUCCESS:				// ��¼�ɹ�
		{
            if (m_dwReconnectTimerId != 0)
            {
                KillTimer(m_dwReconnectTimerId);
                m_dwReconnectTimerId = 0;
            }
        
            ShowPanel(TRUE);
			ShowWindow(SW_SHOW);
			Invalidate();

            m_FMGClient.InitNetThreads();

            //��ȡ����ʱ����
            CIniFile iniFile;
            CString strIniFile;
            strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);

            if (iniFile.ReadInt(_T("server"), _T("enablereconnect"), 1, strIniFile) != 0)
                m_bEnableReconnect = true;
            else
                m_bEnableReconnect = false;

            //���������������û��Ҫ��ȡ����ʱ������
            if (m_bEnableReconnect)
            {
                m_uReconnectInterval = iniFile.ReadInt(_T("server"), _T("reconnectinterval"), 5000, strIniFile);
                if (m_uReconnectInterval <= 0)
                    m_uReconnectInterval = 5000;
            }

			CreateEssentialDirectories();
			
			//��¼�ɹ��Ժ󣬻�ȡ������ʱ��
			//m_FMGClient.RequestServerTime();

            
            //TODO: �������ʱ�������߳�һ������������
            m_FMGClient.GetFriendList();
			m_FMGClient.StartCheckNetworkStatusTask();
			//m_FMGClient.StartHeartbeatTask();

			// �����¼�ʺ��б�
			LOGIN_ACCOUNT_INFO* lpAccount = m_LoginAccountList.Find(m_stAccountInfo.szUser);
			if (lpAccount != NULL)
				memcpy(lpAccount, &m_stAccountInfo, sizeof(LOGIN_ACCOUNT_INFO));
			else
				m_LoginAccountList.Add(m_stAccountInfo.szUser, m_stAccountInfo.szPwd,
					m_stAccountInfo.nStatus, m_stAccountInfo.bRememberPwd, m_stAccountInfo.bAutoLogin);
			m_LoginAccountList.SetLastLoginUser(m_stAccountInfo.szUser);
			tstring strFileName = Hootina::CPath::GetAppPath()+_T("Users\\LoginAccountList.dat");
			m_LoginAccountList.SaveFile(strFileName.c_str());

			// ������������
			//strFileName = m_FMGClient.GetPersonalFolder()+_T("MiscConfig.xml");
			//LoadMiscConfig(strFileName.c_str());

			// ע����ȡ��Ϣ�ȼ�(Ctrl+Alt+D)
			::RegisterHotKey(m_hWnd, 1001, MOD_CONTROL|MOD_ALT, g_cHotKey);

			//Ϊ�˵��Է��㣬���԰�Ͳ��������������- -��
//#ifndef _DEBUG
			//�洢��ǰ��¼�˺��������ļ�
			//SaveCurrentLogonUserToFile();
//#endif
			
			//���ؼӺ���֪ͨIcon
			LoadAddFriendIcon();

			m_FMGClient.LoadUserConfig();
			//����ͷ������Ϣ
			m_nBuddyListHeadPicStyle = m_FMGClient.m_UserConfig.GetBuddyListHeadPicStyle();
			m_BuddyListCtrl.SetStyle((BLCTRL_STYLE)m_nBuddyListHeadPicStyle);
			m_bShowBigHeadPicInSel = m_FMGClient.m_UserConfig.IsEnableBuddyListShowBigHeadPicInSel(); 
			m_BuddyListCtrl.SetShowBigIconInSel(m_bShowBigHeadPicInSel);

            //�����ɹ��󣬱���������ԭ����λ��
            if (m_nMainPanelStatus != MAINPANEL_STATUS_RECONNECTING)
            {
                SetWindowPos(NULL,
                    m_FMGClient.m_UserConfig.GetMainDlgX(),
                    m_FMGClient.m_UserConfig.GetMainDlgY(),
                    m_FMGClient.m_UserConfig.GetMainDlgWidth(),
                    m_FMGClient.m_UserConfig.GetMainDlgHeight(),
                    SWP_SHOWWINDOW);
            }
	

			m_FMGClient.m_UserMgr.LoadTeamInfo();
			m_FMGClient.m_UserMgr.LoadBuddyInfo();
			//���������ϵ���б�
			m_FMGClient.m_UserMgr.LoadRecentList();
			if(m_FMGClient.m_UserMgr.GetRecentListCount()>0 && m_nCurSelIndexInMainTab<0)
			{
				
				m_TabCtrl.SetCurSel(0);
				OnTabCtrlSelChange(NULL);
				//TODO: ����û�к���ʱ�������ϵ�˲���ʾ������bug��
				//::PostMessage(m_hWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
			}
			else
			{
				if(m_nCurSelIndexInMainTab < 0)
					m_TabCtrl.SetCurSel(1);
				else
					m_TabCtrl.SetCurSel(m_nCurSelIndexInMainTab);
				OnTabCtrlSelChange(NULL);
			}

			::SetForegroundWindow(m_hWnd);
			::SetFocus(m_hWnd);
			m_bAlreadyLogin = TRUE;

            m_nMainPanelStatus = MAINPANEL_STATUS_LOGIN;

            //��¼�ɹ�Ҳ����ñ�־λ���Ա��´�����
            m_bAlreadySendReloginRequest = false;
		}
		break;

	case LOGIN_FAILED:				// ��¼ʧ��
		{
			MessageBox(_T("������ϣ���¼ʧ�ܣ�"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();

            m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
		}
		break;

	case LOGIN_UNREGISTERED:
		{						
			MessageBox(_T("�û���δע�ᣡ"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
		}
		break;

	case LOGIN_PASSWORD_ERROR:		// �������
		{
			MessageBox(_T("�������"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
		}
		break;
	
	case LOGIN_SERVER_REFUSED:
		{
			ShowPanel(FALSE);		// ��ʾ��¼���
			MessageBox(_T("�������ܾ����ĵ�¼����"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
		}

	case LOGIN_USER_CANCEL_LOGIN:	// �û�ȡ����¼
		{
			StartLogin();
            m_nMainPanelStatus = MAINPANEL_STATUS_NOTLOGIN;
		}
		break;
	}

	return 0;
}

// ע��������Ϣ
LRESULT CMainDlg::OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(IDOK);
	return 0;
}

// �����û���Ϣ
LRESULT CMainDlg::OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//CBuddyInfoResult* lpBuddyInfoResult = (CBuddyInfoResult*)lParam;
	//if (NULL == lpBuddyInfoResult)
	//	return 0;
	////m_FMGClient.OnUpdateBuddyInfo(lpBuddyInfoResult);
	//delete lpBuddyInfoResult;

	return 0;
}

// ���º����б���Ϣ
LRESULT CMainDlg::OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;

	BuddyListSortOnStaus();
	
	UpdateBuddyTreeCtrl(uAccountID);		// ���º����б�ؼ�

    //TODO: ���ڷŵ����������ϵ�˺�Ⱥ���б�֮��
    m_FMGClient.m_RecvMsgThread.EnableUI(true);

	return 1;
}

// ����Ⱥ�б���Ϣ
LRESULT CMainDlg::OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//BOOL bSuccess = (BOOL)lParam;
	//if (!bSuccess)
	//	return 0;

	UpdateGroupTreeCtrl();		// ����Ⱥ�б�ؼ�


	return 0;
}

// ���������ϵ���б���Ϣ
LRESULT CMainDlg::OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	UpdateRecentTreeCtrl();		// ���������ϵ���б�ؼ�

	return 0;
}

// ������Ϣ
LRESULT CMainDlg::OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyMessage* pBuddyMessage = (CBuddyMessage*)lParam;
	if(pBuddyMessage == NULL)
		return 0;
	BOOL bShakeWindowMsg = pBuddyMessage->IsShakeWindowMsg();
	BOOL bSynchronizeMsg = FALSE;
	
	UINT nUTalkUin = 0;
	UINT nSenderID = pBuddyMessage->m_nFromUin;
	//���ƽ̨��ͬ����������Ϣ
	if(nSenderID == m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
		bSynchronizeMsg = TRUE;
	
	if(bSynchronizeMsg)
		nUTalkUin = pBuddyMessage->m_nToUin;
	//�������ѷ�������Ϣ
	else
		nUTalkUin = (UINT)wParam;

	UINT nMsgID = pBuddyMessage->m_nMsgId;;
	if (nUTalkUin<=0/* || nMsgID<=0*/)
		return 0;

	//����Ǵ��ڶ�����Ϣ��ֱ�ӵ������촰�ڲ����Ŷ���������������Ƕ�����Ϣ����״̬����������
	if(bShakeWindowMsg)
	{
		ShowBuddyChatDlg(nUTalkUin, TRUE, TRUE, nMsgID);
		return 0;
	}
	else
	{
        //�Ƿ����þ���
        if (!m_FMGClient.m_UserConfig.IsEnableMute())
        {
            tstring strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
            ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
        }
	}

	std::map<UINT, CBuddyChatDlg*>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nUTalkUin);

	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* lpBuddyDlg = iter->second;
		if (lpBuddyDlg != NULL && lpBuddyDlg->IsWindow())
		{
			if(bSynchronizeMsg)
				lpBuddyDlg->OnRecvMsg(nSenderID, nMsgID);
			else
				lpBuddyDlg->OnRecvMsg(nUTalkUin, nMsgID);
			return 0;
		}
	}

	//�����豸ͬ����������Ϣ
	if(bSynchronizeMsg)
	{
		CMessageList* lpMsgList = m_FMGClient.GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender* lpMsgSender = lpMsgList->GetMsgSender(FMG_MSG_TYPE_BUDDY, nSenderID);
			if (lpMsgSender != NULL)
			{
				lpMsgSender->DelMsgById(nMsgID);
				
				if (lpMsgSender->GetMsgCount() <= 0)
					lpMsgList->DelMsgSender(FMG_MSG_TYPE_BUDDY, nSenderID);
			}
		}

		return 1;
	}

	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
	m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(FMG_MSG_TYPE_BUDDY, nUTalkUin);

	if (0 == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(RECVCHATMSG_TIMER_ID, ::GetCaretBlinkTime(), NULL);
		
	return 0;
}

// Ⱥ��Ϣ
LRESULT CMainDlg::OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nMsgId = (UINT)lParam;
	if (0 == nGroupCode/* || 0 == nMsgId*/)
		return 0;

	std::map<UINT, CGroupChatDlg*>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg* lpGroupDlg = iter->second;
		if (lpGroupDlg != NULL && lpGroupDlg->IsWindow())
		{
			lpGroupDlg->OnRecvMsg(nGroupCode, nMsgId);
			return 0;
		}
	}

	CMessageList* lpMsgList = m_FMGClient.GetMessageList();
	if (lpMsgList != NULL)
	{
		CMessageSender* lpMsgSender = lpMsgList->GetMsgSender(FMG_MSG_TYPE_GROUP, nGroupCode);
		if (lpMsgSender != NULL)
		{
			if (lpMsgSender->GetMsgCount() == 1)
			{
                //�Ƿ����þ���
                if (!m_FMGClient.m_UserConfig.IsEnableMute())
                {
                    tstring strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
                    ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
                }               
			}
		}
	}

	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetItemIndexByID(nGroupCode, nTeamIndex, nIndex);
	m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(FMG_MSG_TYPE_GROUP, nGroupCode);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ��ʱ�Ự��Ϣ
LRESULT CMainDlg::OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = (UINT)wParam;
	UINT nMsgId = (UINT)lParam;
	if (0 == nUTalkUin || 0 == nMsgId)
		return 0;

    // �Ƿ����þ���
    if (!m_FMGClient.m_UserConfig.IsEnableMute())
    {
        tstring strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
        ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
    }

	std::map<UINT, CSessChatDlg*>::iterator iter;
	iter = m_mapSessChatDlg.find(nUTalkUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg* lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
			lpSessChatDlg->OnRecvMsg(nUTalkUin, nMsgId);
			return 0;
		}
	}

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(FMG_MSG_TYPE_SESS, nUTalkUin);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ����״̬�ı���Ϣ
LRESULT CMainDlg::OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nUTalkUin)
		return 0;

	CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();
	if (NULL == lpBuddyList)
		return 0;

	CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
	if (NULL == lpBuddyInfo)
		return 0;

	CBuddyTeamInfo* lpBuddyTeam = lpBuddyList->GetBuddyTeam(lpBuddyInfo->m_nTeamIndex);
	if (NULL == lpBuddyTeam)
		return 0;

	int nOnlineCnt = lpBuddyTeam->GetOnlineBuddyCount();
	BOOL bOnline = TRUE;
	if(lpBuddyInfo->m_nStatus==STATUS_OFFLINE || lpBuddyInfo->m_nStatus==STATUS_INVISIBLE)
		bOnline = FALSE;

	if (bOnline)	// ���ź�����������
	{
        // �Ƿ����þ���
        if (!m_FMGClient.m_UserConfig.IsEnableMute())
        {
            tstring strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\Global.wav");
            ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
        }
	}

	int nTeamIndex, nIndex;	// ��ȡ����������
	m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);

	// ���º��ѷ�����������
	m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineCnt);

	// ���º���ͷ��
	CString strThumbPath;
	if(lpBuddyInfo->m_bUseCustomFace && lpBuddyInfo->m_bCustomFaceAvailable)
	{
		strThumbPath.Format(_T("%s%d.png"), m_FMGClient.m_UserMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
		if(!Hootina::CPath::IsFileExist(strThumbPath))
			strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	}
	else
		strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, !bOnline);

	// ���ú�������״̬���������߶���
	m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, bOnline, TRUE);

    if (lpBuddyInfo->m_nStatus == online_type_android_cellular || lpBuddyInfo->m_nStatus == online_type_android_wifi)
	{
		strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
		m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, TRUE);
	}
    else if (lpBuddyInfo->m_nStatus == online_type_offline || lpBuddyInfo->m_nStatus == online_type_pc_invisible)
	{
		strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
		m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, FALSE);
	}


	m_BuddyListCtrl.Invalidate();	// ˢ�º����б�ؼ�

	return 0;
}

// ����������Ϣ
LRESULT CMainDlg::OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszReason = _T("�������ߣ�");
	MessageBox(lpszReason, _T("��ʾ"), MB_OK);
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainDlg::OnScreenshotMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //CScreenshotInfo* pScreenshotInfo = (CScreenshotInfo*)lParam;
    //if (pScreenshotInfo == NULL)
    //    return 0;
    
    if (!m_pRemoteDesktopDlg->IsWindow())
    {
        m_pRemoteDesktopDlg->m_pFMGClient = &m_FMGClient;
        m_pRemoteDesktopDlg->Create(m_hWnd, 0);       
    }

    

    //BITMAPINFOHEADER*	bmpHeader = (BITMAPINFOHEADER*)pScreenshotInfo->m_strBmpHeader.c_str();

    //const char* bmpbuf = pScreenshotInfo->m_strBmpData.c_str();

    m_pRemoteDesktopDlg->ShowWindow(SW_SHOW);
    return 0;

    //HDC hdc = ::GetDC(m_pRemoteDesktopDlg->m_hWnd);
    //BITMAPINFO* bmpinfo = (BITMAPINFO*)bmpHeader;//λͼ��Ϣ 
    //HBITMAP hbitmap = CreateDIBitmap(hdc, bmpHeader, CBM_INIT, bmpbuf, bmpinfo, DIB_RGB_COLORS);

    //PBITMAPINFO bmpInf = (PBITMAPINFO)pScreenshotInfo->m_strBmpHeader.c_str();
    //m_pRemoteDesktopDlg->MoveWindow(0, 0, bmpInf->bmiHeader.biWidth - 200, bmpInf->bmiHeader.biHeight - 200, FALSE);

    //CClientDC dc(m_pRemoteDesktopDlg->m_hWnd);

    //int nPaletteSize = 0;
    //BYTE* buf = ((BYTE*)bmpInf) +
    //    sizeof(BITMAPINFOHEADER) +
    //    sizeof(RGBQUAD)*nPaletteSize;

    //int nOffset;
    //BYTE r, g, b;
    //int nWidth = bmpInf->bmiHeader.biWidth*bmpInf->bmiHeader.biBitCount / 8;
    //nWidth = ((nWidth + 3) / 4) * 4; //4�ֽڶ���

    ////if (bmpInf->bmiHeader.biBitCount == 8)
    ////{
    ////    for (int i = 0; i<bmpInf->bmiHeader.biHeight; i++)
    ////    {
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            RGBQUAD rgbQ;
    ////            rgbQ = bmpInf->bmiColors[buf[i*nWidth + j]];
    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(rgbQ.rgbRed, rgbQ.rgbGreen, rgbQ.rgbBlue)); //������ʾ
    ////        }
    ////    }
    ////}
    ////else if (bmpInf->bmiHeader.biBitCount == 16)
    ////{
    ////    for (int i = 0; i<bm.bmHeight; i++)
    ////    {
    ////        nOffset = i*nWidth;
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            b = buf[nOffset + j * 2] & 0x1F;
    ////            g = buf[nOffset + j * 2] >> 5;
    ////            g |= (buf[nOffset + j * 2 + 1] & 0x03) << 3;
    ////            r = (buf[nOffset + j * 2 + 1] >> 2) & 0x1F;

    ////            r *= 8;
    ////            b *= 8;
    ////            g *= 8;

    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(r, g, b)); //������ʾ
    ////        }
    ////    }
    ////}
    ////else if (bmpInf->bmiHeader.biBitCount == 24)
    ////{
    ////    for (int i = 0; i<bm.bmHeight; i++)
    ////    {
    ////        nOffset = i*nWidth;
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            b = buf[nOffset + j * 3];
    ////            g = buf[nOffset + j * 3 + 1];
    ////            r = buf[nOffset + j * 3 + 2];

    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(r, g, b)); //������ʾ
    ////        }
    ////    }
    ////}
    ///*else*/ if (bmpInf->bmiHeader.biBitCount == 32)
    //{
    //    for (int i = 0; i<bmpInf->bmiHeader.biHeight; i++)
    //    {
    //        nOffset = i*nWidth;
    //        for (int j = 0; j<bmpInf->bmiHeader.biWidth; j++)
    //        {
    //            b = buf[nOffset + j * 4];
    //            g = buf[nOffset + j * 4 + 1];
    //            r = buf[nOffset + j * 4 + 2];

    //            dc.SetPixel(j, bmpInf->bmiHeader.biHeight - i, RGB(r, g, b)); //������ʾ
    //        }
    //    }
    //}
    //
    //return 0;
}

// Ⱥϵͳ��Ϣ
LRESULT CMainDlg::OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//UINT nGroupCode = (UINT)lParam;
	//if (0 == nGroupCode)
	//	return 0;

	//tstring strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\system.wav");	// ������ϵͳ��Ϣ��ʾ��
	//::sndPlaySound(strFileName.c_str(), SND_ASYNC);

	//UpdateMsgIcon();

	//if (m_MsgTipDlg.IsWindow())
	//	m_MsgTipDlg.AddMsgSender(long_SYSGROUP, nGroupCode);

	//if (NULL == m_dwMsgTimerId)
	//	m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ���º��Ѻ���
LRESULT CMainDlg::OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nUTalkUin)
		return 0;

 	NotifyBuddyChatDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_NUMBER);		// ֪ͨ�������촰�ڸ���
 	NotifyBuddyInfoDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_NUMBER);		// ֪ͨ������Ϣ���ڸ���

	return 0;
}

// ����Ⱥ��Ա����
LRESULT CMainDlg::OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GMEMBER_NUMBER, wParam, lParam);// ֪ͨȺ���촰�ڸ���
	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_NUMBER);	// ֪ͨȺ��Ա��Ϣ���ڸ���
	NotifySessChatDlg(nUTalkUin, FMG_MSG_UPDATE_GMEMBER_NUMBER);				// ֪ͨȺ��Ա���촰�ڸ���

	return 0;
}

// ����Ⱥ����
LRESULT CMainDlg::OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)lParam;
	if (0 == nGroupCode)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_NUMBER, 0, 0);// ֪ͨȺ���촰�ڸ���
 	NotifyGroupInfoDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_NUMBER);		// ֪ͨȺ��Ϣ���ڸ���

	return 0;
}

// ���º��Ѹ���ǩ��
LRESULT CMainDlg::OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = lParam;
	if (0 == nUTalkUin)
		return 0;

	CBuddyInfo* lpBuddyInfo = m_FMGClient.GetUserInfo(nUTalkUin);
	if (lpBuddyInfo == NULL)
		return 0;

	if (m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID == nUTalkUin)	// �����û�����ǩ��
	{
		if (!lpBuddyInfo->m_strSign.empty())
		{
			m_btnSign.SetWindowText(lpBuddyInfo->m_strSign.c_str());
			m_btnSign.SetToolTipText(lpBuddyInfo->m_strSign.c_str());
		}
		else
		{
			m_btnSign.SetWindowText(_T(""));
			m_btnSign.SetToolTipText(_T("����޸ĸ���ǩ��"));
		}
	}
	else	// ���º��Ѹ���ǩ��
	{
		CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();		// ���º����б�ؼ��ĸ���ǩ��
		if (lpBuddyList != NULL)
		{
			CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
			if (lpBuddyInfo != NULL)
			{
				int nTeamIndex, nIndex;
				m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
				m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str());
			}
		}

		NotifyBuddyChatDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_SIGN);		// ֪ͨ�������촰�ڸ���
		NotifyBuddyInfoDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_SIGN);		// ֪ͨ������Ϣ���ڸ���
	}

	return 0;
}

// ����Ⱥ��Ա����ǩ��
LRESULT CMainDlg::OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
		return 0;

	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_SIGN);	// ֪ͨȺ��Ա��Ϣ���ڸ���

	return 0;
}

// �����û���Ϣ
LRESULT CMainDlg::OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = lParam;
	if (0 == nUTalkUin)
		return 0;
	
 	CBuddyInfo* lpBuddyInfo = m_FMGClient.GetUserInfo(nUTalkUin);
 	if (lpBuddyInfo == NULL)	// �����û��ǳ�
		return 0;
	if(lpBuddyInfo->m_uUserID == m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
	{
		//����ǳ�Ϊ�գ�����ʾ�˻���
		if(lpBuddyInfo->m_strNickName.empty())
			m_staNickName.SetWindowText(lpBuddyInfo->m_strAccount.c_str());
		else
			m_staNickName.SetWindowText(lpBuddyInfo->m_strNickName.c_str());	

		//m_staNickName.Invalidate(FALSE);

		long nStatus = m_FMGClient.GetStatus();
		LoadAppIcon(nStatus);
		CString strIconInfo;
		if(nStatus == STATUS_OFFLINE)
			strIconInfo.Format(_T("%s\r\n%s\r\n����"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(), m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		else
			strIconInfo.Format(_T("%s\r\n%s\r\n����"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(), m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		
		m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
	}
	else
	{
		int nTeamIndex, nIndex;	// ��ȡ����������
		m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
		m_BuddyListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());
		m_BuddyListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
		m_BuddyListCtrl.Invalidate(FALSE);
	}


	//ͬʱ����ͷ����Ϣ 
	PostMessage(FMG_MSG_UPDATE_BUDDY_NUMBER, 0, (LPARAM)nUTalkUin);
	PostMessage(FMG_MSG_UPDATE_BUDDY_HEADPIC, 0, (LPARAM)nUTalkUin);
	PostMessage(FMG_MSG_UPDATE_BUDDY_SIGN, 0, (LPARAM)nUTalkUin);
	
	//֪ͨȺ�Ĵ��ڸ���
	for(const auto& iter : m_FMGClient.m_UserMgr.m_GroupList.m_arrGroupInfo)
	{
		if(iter->IsMember(nUTalkUin))
			PostMessage(FMG_MSG_UPDATE_GROUP_INFO, nUTalkUin, 0);
	}
	// ֪ͨ������Ϣ���ڸ���
	NotifyBuddyInfoDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_INFO);		
	
#ifdef _DEBUG
	static long i = 0;
	AtlTrace(_T("Update Buddy Info: %d.\n"), i);
	i++;
#endif
	return 0;
}

LRESULT CMainDlg::OnUpdateChatDlgOnlineStatus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;
	//long nStatus = (long)lParam;
	std::map<UINT, CBuddyChatDlg*>::iterator iter;
	iter = m_mapBuddyChatDlg.find(uAccountID);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* pBuddyChatDlg = iter->second;
#ifndef _DEBUG
		if(pBuddyChatDlg == NULL)
		{
			LOG_ERROR("Why 0x%08x is null pointer of a chat dlg��", pBuddyChatDlg);
			return 0;
		}
#endif
															
		pBuddyChatDlg->OnUpdateBuddyNumber();
		pBuddyChatDlg->OnUpdateBuddySign();		// ���º���ǩ��֪ͨ
		pBuddyChatDlg->OnUpdateBuddyHeadPic();
		//pBuddyChatDlg->OnUpdateBuddyHeadPic();
#ifndef _DEBUG
		if(!pBuddyChatDlg->IsWindow())
		{
			LOG_ERROR("Why 0x%08x is a invalid window of a chat dlg��", pBuddyChatDlg);
			return 0;
		}
#endif

		pBuddyChatDlg->Invalidate(FALSE);
	}

	return 1;
}

// ����Ⱥ��Ա��Ϣ
LRESULT CMainDlg::OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
		return 0;

	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_INFO);	// ֪ͨȺ��Ա��Ϣ���ڸ���

	return 0;
}

// ����Ⱥ��Ϣ
LRESULT CMainDlg::OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;

 	NotifyGroupChatDlg(uAccountID, FMG_MSG_UPDATE_GROUP_INFO, 0, 0);	// ֪ͨȺ���촰�ڸ���
 	NotifyGroupInfoDlg(uAccountID, FMG_MSG_UPDATE_GROUP_INFO);		// ֪ͨȺ��Ϣ���ڸ���
//	CGroupInfo* pGroupInfo = NULL;
//	CBuddyInfo* pBuddyInfo = NULL;
//	std::map<UINT, CGroupChatDlg*>::iterator iterGroupChatDlg;
//	UINT nGroupCode = 0;
//	for(const auto& iter : m_FMGClient.m_UserMgr.m_GroupList.m_arrGroupInfo)
//	{
//		pGroupInfo = iter;
//		if(pGroupInfo == NULL)
//			continue;
//
//		pBuddyInfo = pGroupInfo->GetMemberByUin(uAccountID);
//		if(pBuddyInfo == NULL)
//			continue;
//		
//		nGroupCode = pGroupInfo->m_nGroupCode;
//		iterGroupChatDlg = m_mapGroupChatDlg.find(nGroupCode);
//		if(iterGroupChatDlg != m_mapGroupChatDlg.end())
//		{
//#ifndef _DEBUG
//			if(iterGroupChatDlg->second->IsWindow())
//			{
//#endif
//				iterGroupChatDlg->second->OnUpdateGroupInfo();
//#ifndef _DEBUG
//			}
//#endif
//		}
//		
//	}

	return 0;
}

// HTREEITEM CMainDlg::FindTreeItemByUTalkUin(CTreeViewCtrl& treeCtrl, UINT nUTalkUin)
// {
// 	HTREEITEM hItem = treeCtrl.GetRootItem();
// 	while (hItem != NULL)
// 	{
// 		UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 		if (nUTalkUin2 == nUTalkUin)
// 			return hItem;
// 
// 		HTREEITEM hFindItem = FindTreeItemByUTalkUin2(treeCtrl, hItem, nUTalkUin);
// 		if (hFindItem != NULL)
// 			return hFindItem;
// 
// 		hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXT);
// 	}
// 	return NULL;
// }
// 
// HTREEITEM CMainDlg::FindTreeItemByUTalkUin2(CTreeViewCtrl& treeCtrl, HTREEITEM hItem, UINT nUTalkUin)
// {
// 	if (treeCtrl.ItemHasChildren(hItem))
// 	{
// 		HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
// 
// 		while (hChildItem != NULL)
// 		{
// 			UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 			if (nUTalkUin2 == nUTalkUin)
// 				return hItem;
// 
// 			HTREEITEM hFindItem = FindTreeItemByUTalkUin2(treeCtrl, hChildItem, nUTalkUin);
// 			if (hFindItem != NULL)
// 				return hFindItem;
// 
// 			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
// 		}
// 	}
// 
// 	UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 	return  (nUTalkUin2 == nUTalkUin) ? hItem : NULL;
// }

// ���º���ͷ��ͼƬ
LRESULT CMainDlg::OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = (UINT)lParam;
	if(nUTalkUin == 0)
		return 0;

	CBuddyInfo* lpBuddyInfo = m_FMGClient.GetUserInfo(nUTalkUin);
	if(lpBuddyInfo == NULL)
		return 0;

	CString strThumbPath;
	if(lpBuddyInfo->m_bUseCustomFace  && lpBuddyInfo->m_bCustomFaceAvailable)
	{
		strThumbPath.Format(_T("%s%d.png"), m_FMGClient.m_UserMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
		if(!Hootina::CPath::IsFileExist(strThumbPath))
			strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	}
	else
		strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	
	//�����Լ���ͷ��
	if (m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID == nUTalkUin)	// �����û�ͷ��
	{
		BOOL bGray = m_FMGClient.GetStatus()!=STATUS_OFFLINE ? FALSE : TRUE;
		m_picHead.SetBitmapWithoutCache(strThumbPath, FALSE, bGray);
		m_picHead.Invalidate();
		return 0;
	}

	// ���º����б�ͷ��
	UINT nUTalkNum = 0;
	BOOL bGray = FALSE;
	CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();
	if (lpBuddyList != NULL)
	{
		CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
		if (lpBuddyInfo != NULL)
		{
			nUTalkNum = lpBuddyInfo->m_uUserID;
			bGray = lpBuddyInfo->m_nStatus != STATUS_OFFLINE ? FALSE : TRUE;
		}
	}

	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
	m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);

	// ���������ϵ���б�ͷ��
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		CRecentList* lpRecentList = m_FMGClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo* lpRecentInfo = lpRecentList->GetRecent(nIndex);
			if (lpRecentInfo != NULL)
			{
				if (0 == lpRecentInfo->m_nType)			// ����
				{
					if (nUTalkUin == lpRecentInfo->m_uUserID)
					{
						m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
						m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
						break;
					}
				}
			}
		}
	}

	NotifyBuddyChatDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_HEADPIC);		// ֪ͨ�������촰�ڸ���

	return 0;
}

// ����Ⱥ��Աͷ��ͼƬ
LRESULT CMainDlg::OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;

	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GMEMBER_HEADPIC, wParam, lParam);	// ֪ͨȺ���촰�ڸ���
	NotifySessChatDlg(nUTalkUin, FMG_MSG_UPDATE_GMEMBER_HEADPIC);						// ֪ͨȺ��Ա���촰�ڸ���

	return 0;
}

// ����Ⱥͷ��ͼƬ
LRESULT CMainDlg::OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;

	// ����Ⱥ�б�ͷ��
	UINT nGroupNum = 0;
	CGroupList* lpGroupList = m_FMGClient.GetGroupList();
	if (lpGroupList != NULL)
	{
		CGroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
		if (lpGroupInfo != NULL)
			nGroupNum = lpGroupInfo->m_nGroupNumber;
	}

	tstring strFileName = m_FMGClient.GetGroupHeadPicFullName(nGroupNum);
	if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
		strFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetItemIndexByID(nGroupCode, nTeamIndex, nIndex);
	m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);

	// ���������ϵ���б�ͷ��
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		CRecentList* lpRecentList = m_FMGClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo* lpRecentInfo = lpRecentList->GetRecent(nIndex);
			if (lpRecentInfo != NULL)
			{
				if (1 == lpRecentInfo->m_nType)	// Ⱥ
				{
					CGroupList* lpGroupList = m_FMGClient.GetGroupList();
					if (lpGroupList != NULL)
					{
						CGroupInfo* lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_uUserID);
						if (lpGroupInfo != NULL && nGroupCode == lpGroupInfo->m_nGroupCode)
						{
							m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
							m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);
							break;
						}
					}
				}
			}
		}
	}

	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_HEADPIC, wParam, lParam);// ֪ͨȺ���촰�ڸ���

	return 0;
}

// �ı�����״̬������Ϣ
LRESULT CMainDlg::OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)wParam;
	long nNewStatus = lParam;
	if (!bSuccess)
		MessageBox(_T("�ı�����״̬ʧ�ܣ�"));
	return 0;
}

LRESULT CMainDlg::OnSendAddFriendRequestResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == ADD_FRIEND_FAILED)
	{
		::MessageBox(m_hWnd, _T("������ϣ����ͼӺ�������ʧ�ܣ����Ժ����ԣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
	}

	return 1;
}

LRESULT CMainDlg::OnRecvAddFriendRequest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COperateFriendResult* pResult = (COperateFriendResult*)lParam;
	if(pResult == NULL)
		return (LRESULT)0;

	//���˺����Լ���ID��cmdΪApply���˻����ǳ�Ϊ��ʱ�����Լ�������������ĳɹ�Ӧ�𣬺��Ե�
	if(pResult->m_uCmd==Apply && pResult->m_uAccountID==m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID &&*(pResult->m_szAccountName)==0 &&*(pResult->m_szNickName)==0)
	{
		delete pResult;
		return 0;
	}
	
	AddFriendInfo* pAddFriendInfo = new AddFriendInfo();
	pAddFriendInfo->nCmd = pResult->m_uCmd;
	pAddFriendInfo->uAccountID = pResult->m_uAccountID;

	TCHAR szData[64] = {0};
    EncodeUtil::Utf8ToUnicode(pResult->m_szAccountName, szData, ARRAYSIZE(szData));
	_tcscpy_s(pAddFriendInfo->szAccountName, ARRAYSIZE(pAddFriendInfo->szAccountName), szData);

	memset(szData, 0, sizeof(szData));
    EncodeUtil::Utf8ToUnicode(pResult->m_szNickName, szData, ARRAYSIZE(szData));
	_tcscpy_s(pAddFriendInfo->szNickName, ARRAYSIZE(pAddFriendInfo->szNickName), szData);
	
	if(pAddFriendInfo->nCmd == Agree)
	{
		m_FMGClient.m_UserMgr.AddFriend(pAddFriendInfo->uAccountID, pAddFriendInfo->szAccountName, pAddFriendInfo->szNickName);
		PostMessage(FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
		//���»�ȡ�º����б�����
        m_FMGClient.GetFriendList();
	}
	
	delete pResult;

	m_FMGClient.m_aryAddFriendInfo.push_back(pAddFriendInfo);

    // �Ƿ����þ���
    if (!m_FMGClient.m_UserConfig.IsEnableMute())
    {
        tstring strFileName(Hootina::CPath::GetAppPath() + _T("Sound\\system.wav"));	// ���żӺ�����ʾ��
        ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
    }
	
	m_dwAddFriendTimerId = SetTimer(ADDFRIENDREQUEST_TIMER_ID, ::GetCaretBlinkTime(), NULL);
	
	return 1;
}

LRESULT CMainDlg::OnDeleteFriendResult(UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)lParam;
	if(!IsGroupTarget(uAccountID))
	{
		if(wParam == DELETE_FRIEND_FAILED)
			::MessageBox(m_hWnd, _T("���������⣬ɾ������ʧ�ܣ����Ժ����ԣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONWARNING);
		else if(wParam == DELETE_FRIEND_SUCCESS)
		{
			//ɾ��֮���Ѿ�û�к����ˣ�����������������б�
            if (m_FMGClient.m_UserMgr.m_BuddyList.GetBuddyTotalCount() == 0)
                ::SendMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
            else
                //ɾ�����ѳɹ��Ժ���º����б�
                m_FMGClient.GetFriendList();

			//ɾ���ɹ�֮�󣬹ر�֮ǰ��ú��ѵ����촰��
			ShowBuddyChatDlg(lParam, FALSE);

            //::MessageBox(m_hWnd, _T("ɾ�����ѳɹ���"), g_strAppTitle.c_str(), MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		::MessageBox(m_hWnd, _T("��Ⱥ�ɹ���"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		ShowGroupChatDlg(uAccountID, FALSE);
        m_FMGClient.GetFriendList();
		::PostMessage(m_hWnd, FMG_MSG_UPDATE_GROUP_LIST, 0, 0);
	}
	
	//���������е���Ϣ��Ȼ������������
	//TODO: ��δ��������⣺
	//��ɾ��ĳ��������Ϣ�ĺ���ʱ����Ⱥ��ʱ������һ������Ϣ��Ⱥ����Ȼ��˸��������һ������Ϣ�ĺ��ѾͲ���˸�ˣ���
	int nTeamIndex, nIndex;
	int nCount = m_FMGClient.m_UserMgr.m_MsgList.GetMsgSenderCount();
	UINT nUTalkUin = 0;
	CMessageSender* pSender = NULL;
	for(int i=0; i<nCount; ++i)
	{
		pSender = m_FMGClient.m_UserMgr.m_MsgList.GetMsgSender(i);
		if(pSender != NULL)
		{
			nUTalkUin = pSender->GetSenderId();
			if(nUTalkUin > 0)
			{
				if(!IsGroupTarget(nUTalkUin))
				{
					m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
					m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);
					//m_BuddyListCtrl.Invalidate(FALSE);
				}
				else
				{
					m_GroupListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
					m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);
				}
			}
		}
	}
	
	
	return (LRESULT)1;
}

LRESULT CMainDlg::OnSelfStatusChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	//UINT uAccountID = (UINT)wParam;
	//if(uAccountID != m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
	//	return (LRESULT)0;

	long nStatus = (long)lParam;
	
	//�Լ��������PC������
    //��������״̬�����Է��������͵�����֪ͨ
    if (m_nMainPanelStatus != MAINPANEL_STATUS_RECONNECTING && nStatus != STATUS_MOBILE_ONLINE && nStatus != STATUS_MOBILE_OFFLINE)
	{
		m_FMGClient.GoOffline();
        m_FMGClient.Uninit();
        //ShowPanel(FALSE);
		CloseAllDlg();
		
		ShowWindow(SW_SHOW);
		CString strInfo;
		strInfo.Format(_T("�𾴵�%s(%s)\r\n    �����˺�����һ̨�����ϵ�¼���㱻�����ߡ�����ⲻ���㱾�����Բ������������޸����룡"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(),  m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		
		CMobileStatusDlg dlg;
		dlg.SetWindowTitle(_T("����֪ͨ"));
		dlg.SetInfoText(strInfo);
		dlg.EnableAutoDisappear(FALSE);
		dlg.DoModal(m_hWnd, NULL);

		m_FMGClient.m_UserMgr.ClearUserInfo();
		m_FMGClient.m_UserMgr.m_UserInfo.Reset();

		m_TrayIcon.RemoveIcon();
		memset(&m_stAccountInfo, 0, sizeof(LOGIN_ACCOUNT_INFO));
		StartLogin(FALSE);

		return (LRESULT)1;
	}
	else if(nStatus==STATUS_MOBILE_ONLINE)
	{
		CString strInfo;
		strInfo.Format(_T("�𾴵�%s(%s)\r\n    �����˺����ֻ������ߡ�\r\n    ����ⲻ���㱾�����Բ������������޸����롣"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(),  m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		CMobileStatusDlg dlg;
		
		dlg.SetWindowTitle(_T("�ֻ�������ʾ"));
		dlg.SetInfoText(strInfo);
		dlg.EnableAutoDisappear(TRUE);
		dlg.DoModal(NULL, NULL);
		return (LRESULT)1;
	}
	//else if(nStatus==STATUS_MOBILE_OFFLINE)
	//{
	//	::MessageBox(m_hWnd, _T("�����˺����ƶ���Flamingo���ߡ�\r\n����ⲻ���㱾�����Բ������������޸����룡"), _T("��ܰ��ʾ"), MB_OK|MB_ICONINFORMATION);
	//	return (LRESULT)1;
	//}

	return (LRESULT)1;
}

LRESULT CMainDlg::OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;

	if(nUTalkUin == m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
	{
		PostMessage(WM_COMMAND, (WPARAM)ID_PIC_HEAD, 0);
		return 1;
	}

	switch (uMsg)
	{
	case WM_SHOW_BUDDYCHATDLG:
		ShowBuddyChatDlg(nUTalkUin, TRUE);
		break;
	case WM_SHOW_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nUTalkUin, TRUE);
		break;
	case WM_SHOW_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_BUDDYINFODLG:
		ShowBuddyInfoDlg(nUTalkUin, TRUE);
		break;
	case WM_SHOW_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nUTalkUin, TRUE);
		break;
	case WM_SHOW_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, TRUE);
		break;
	case WM_CLOSE_BUDDYCHATDLG:
		ShowBuddyChatDlg(nUTalkUin, FALSE);
		break;
	case WM_CLOSE_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nUTalkUin, FALSE);
		break;
	case WM_CLOSE_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_BUDDYINFODLG:
		ShowBuddyInfoDlg(nUTalkUin, FALSE);
		break;
	case WM_CLOSE_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nUTalkUin, FALSE);
		break;
	case WM_CLOSE_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, FALSE);
		break;
	case WM_SHOW_USERINFODLG:
		ShowUserInfoDlg(nUTalkUin, TRUE);
	case WM_CLOSE_USERINFODLG:
		ShowUserInfoDlg(nUTalkUin, FALSE);
	}
	return 0;
}

LRESULT CMainDlg::OnCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(IDOK);
	return (LRESULT)1;
}

LRESULT CMainDlg::OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long nType = (long)wParam;
	UINT nSenderId = (UINT)lParam;

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DelMsgSender(nType, nSenderId);

	if (nType == FMG_MSG_TYPE_BUDDY)
	{
		int nTeamIndex, nIndex;
		m_BuddyListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
		//���������ϵ��������Ϣ����Ŀ
		SendMessage(FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
	}
	else if (nType == FMG_MSG_TYPE_GROUP)
	{
		int nTeamIndex, nIndex;
		m_GroupListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
		//���������ϵ��������Ϣ����Ŀ
		SendMessage(FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
	}

	UpdateMsgIcon();
	
	//TODO: ���´���ŵ�UpdateMsgIcon��ȥ
	CMessageList* lpMsgList = m_FMGClient.GetMessageList();
	if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() <= 0)
	{
		if (m_MsgTipDlg.IsWindow())
			m_MsgTipDlg.DestroyWindow();
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
		m_nLastMsgType = FMG_MSG_TYPE_BUDDY;
		m_nLastSenderId = 0;

		CString strIconInfo;
		strIconInfo.Format(_T("%s\r\n%s\r\n����"), m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.c_str(), m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
		m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
	}

	return 0;
}

LRESULT CMainDlg::OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_MsgTipDlg.IsWindow())
		::PostMessage(m_MsgTipDlg.m_hWnd, WM_CLOSE, 0, 0);
	
	KillTimer(m_dwMsgTimerId);
	m_dwMsgTimerId = NULL;
	//�Ƚ�������ͼ��ָ�������״̬
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n����"), 
						m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.data(),				
						m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.data());
						
	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
	return 0;
}

void CMainDlg::ShowBuddyChatDlg(UINT nUTalkUin, BOOL bShow, BOOL bShakeWindowMsg, UINT nMsgID)
{
	if (nUTalkUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyChatDlg*>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nUTalkUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (!lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->Create(NULL);
				//lpBuddyChatDlg->ShowWindow(SW_SHOW);
				lpBuddyChatDlg->ShowWindow(SW_RESTORE);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				if(bShakeWindowMsg)
				{
					::Sleep(3000);
					ShakeWindow(lpBuddyChatDlg->m_hWnd, 1);
					lpBuddyChatDlg->OnRecvMsg(nUTalkUin, nMsgID);
				}
			}
		}
		else
		{
			//�״�������߼�
			CBuddyChatDlg* lpBuddyChatDlg = new CBuddyChatDlg;
			if (lpBuddyChatDlg != NULL)
			{
				long nLastWidth = m_FMGClient.m_UserConfig.GetChatDlgWidth();
				long nLastHeight = m_FMGClient.m_UserConfig.GetChatDlgHeight();
				lpBuddyChatDlg->m_lpFMGClient = &m_FMGClient;
				lpBuddyChatDlg->m_lpFaceList = &m_FaceList;
				lpBuddyChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpBuddyChatDlg->m_hMainDlg = m_hWnd;
				lpBuddyChatDlg->m_nUTalkUin = nUTalkUin;
                lpBuddyChatDlg->m_UserId = nUTalkUin;
                lpBuddyChatDlg->m_LoginUserId = m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID;
				m_mapBuddyChatDlg[nUTalkUin] = lpBuddyChatDlg;
				lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				lpBuddyChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
				SendMessage(lpBuddyChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				if(bShakeWindowMsg)
				{
					ShakeWindow(lpBuddyChatDlg->m_hWnd, 1);
					lpBuddyChatDlg->OnRecvMsg(nUTalkUin, nMsgID);
				}
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyChatDlg*>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nUTalkUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
				{
					lpBuddyChatDlg->DestroyWindow();
					delete lpBuddyChatDlg;
				}
				
			}
			m_mapBuddyChatDlg.erase(iter);
		}
		
	}
}

void CMainDlg::ShowGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	if (nGroupCode == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupChatDlg*>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (!lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_RESTORE);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}
		}
		else
		{
			CGroupChatDlg* lpGroupChatDlg = new CGroupChatDlg;
			if (lpGroupChatDlg != NULL)
			{
				long nLastWidth = m_FMGClient.m_UserConfig.GetGroupDlgWidth();
				long nLastHeight = m_FMGClient.m_UserConfig.GetGroupDlgHeight();
				lpGroupChatDlg->m_lpFMGClient = &m_FMGClient;
				lpGroupChatDlg->m_lpFaceList = &m_FaceList;
				lpGroupChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpGroupChatDlg->m_hMainDlg = m_hWnd;
				lpGroupChatDlg->m_nGroupCode = nGroupCode;
				m_mapGroupChatDlg[nGroupCode] = lpGroupChatDlg;
				lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				lpGroupChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
				SendMessage(lpGroupChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupChatDlg*>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
			m_mapGroupChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSessChatDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow)
{
	if (nUTalkUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CSessChatDlg*>::iterator iter;
		iter = m_mapSessChatDlg.find(nUTalkUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg* lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (!lpSessChatDlg->IsWindow())
					lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
			}
		}
		else
		{
			CSessChatDlg* lpSessChatDlg = new CSessChatDlg;
			if (lpSessChatDlg != NULL)
			{
				lpSessChatDlg->m_lpFMGClient = &m_FMGClient;
				lpSessChatDlg->m_lpFaceList = &m_FaceList;
				lpSessChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpSessChatDlg->m_hMainDlg = m_hWnd;
				lpSessChatDlg->m_nGroupCode = nGroupCode;
				lpSessChatDlg->m_nUTalkUin = nUTalkUin;
				m_mapSessChatDlg[nUTalkUin] = lpSessChatDlg;
				lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CSessChatDlg*>::iterator iter;
		iter = m_mapSessChatDlg.find(nUTalkUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg* lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
			m_mapSessChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	/*CString strText, strGroupName, strAdminName, strAdminUin, strMsg;
	UINT nGroupNumber;

	CGroupList* lpGroupList = m_FMGClient.GetGroupList();
	CMessageList* lpMsgList = m_FMGClient.GetMessageList();
	if (NULL == lpGroupList || NULL == lpMsgList)
		return;

	CGroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
	CMessageSender* lpMsgSender = lpMsgList->GetMsgSender(long_SYSGROUP, nGroupCode);
	if (NULL == lpMsgSender)
		return;

	if (lpGroupInfo != NULL)
		strGroupName = lpGroupInfo->m_strName.c_str();

	int nMsgCnt = lpMsgSender->GetMsgCount();
	for (int i = 0; i < nMsgCnt; i++)
	{
		CSysGroupMessage* lpSysGroupMsg = lpMsgSender->GetSysGroupMsg(i);
		if (lpSysGroupMsg != NULL)
		{
			strAdminName = lpSysGroupMsg->m_strAdminNickName.c_str();
			strAdminUin = lpSysGroupMsg->m_strAdminUin.c_str();
			strMsg = lpSysGroupMsg->m_strMsg.c_str();
			nGroupNumber = lpSysGroupMsg->m_nGroupNumber;

			if (lpSysGroupMsg->m_strSubType == _T("group_request_join_agree"))
			{
				LPCTSTR lpFmt = _T("����Ա��ͬ��������Ⱥ%s(%u)");
				strText.Format(lpFmt, strGroupName, nGroupNumber);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_request_join_deny"))
			{
				LPCTSTR lpFmt = _T("����Ա�Ѿܾ�������Ⱥ%s(%u)���ܾ����ɣ�%s");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strMsg);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_leave"))
			{
				LPCTSTR lpFmt = _T("%s(%u)����Ա%s(%s)�ѽ����Ƴ�����Ⱥ��");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strAdminName, strAdminUin);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
		}
	}*/
	//lpMsgList->DelMsgSender(long_SYSGROUP, nGroupCode);
	//OnDelMsgSender(NULL, long_SYSGROUP, nGroupCode);
}

void CMainDlg::ShowUserInfoDlg(UINT nUTalkUin, BOOL bShow)
{
	if (nUTalkUin == 0)
	{
		return;
	}

	if (!bShow)
	{
		DestroyWindow();
	}
}

void CMainDlg::ShowBuddyInfoDlg(UINT nUTalkUin, BOOL bShow)
{
	if (nUTalkUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nUTalkUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = new CBuddyInfoDlg;
			// �������϶Ի���
			if (nUTalkUin != m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
			{
				tstring strFriendNickName(m_FMGClient.m_UserMgr.GetNickName(nUTalkUin));
				strFriendNickName += _T("������");
				lpBuddyInfoDlg->SetWindowTitle(strFriendNickName.c_str());
			}

			if (lpBuddyInfoDlg != NULL)
			{
				m_mapBuddyInfoDlg[nUTalkUin] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpFMGClient = &m_FMGClient;
				lpBuddyInfoDlg->m_nUTalkUin = nUTalkUin;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nUTalkUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapBuddyInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow)
{
	if (0 == nGroupCode || 0 == nUTalkUin)
		return;

	if (bShow)
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nUTalkUin = nUTalkUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapGMemberInfoDlg[key] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpFMGClient = &m_FMGClient;
				//lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nUTalkUin = nUTalkUin;
				//lpBuddyInfoDlg->m_bIsGMember = TRUE;
				//lpBuddyInfoDlg->m_nGroupCode = nGroupCode;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nUTalkUin = nUTalkUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapGMemberInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow)
{
	if (0 == nGroupCode)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupInfoDlg*>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (!lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
		else
		{
			CGroupInfoDlg* lpGroupInfoDlg = new CGroupInfoDlg;
			if (lpGroupInfoDlg != NULL)
			{
				m_mapGroupInfoDlg[nGroupCode] = lpGroupInfoDlg;
				lpGroupInfoDlg->m_lpFMGClient = &m_FMGClient;
				lpGroupInfoDlg->m_hMainDlg = m_hWnd;
				lpGroupInfoDlg->m_nGroupCode = nGroupCode;
				lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupInfoDlg*>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
			m_mapGroupInfoDlg.erase(iter);
		}
	}
}

// ֪ͨ�������촰�ڸ���
void CMainDlg::NotifyBuddyChatDlg(UINT nUTalkUin, UINT uMsg)
{
	std::map<UINT, CBuddyChatDlg*>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nUTalkUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg != NULL && lpBuddyChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyChatDlg->OnUpdateBuddyNumber();
				break;

			case FMG_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyChatDlg->OnUpdateBuddySign();
				break;

			case FMG_MSG_UPDATE_BUDDY_HEADPIC:
				lpBuddyChatDlg->OnUpdateBuddyHeadPic();
				break;
			}
		}
	}
}

// ֪ͨȺ���촰�ڸ���
void CMainDlg::NotifyGroupChatDlg(UINT nGroupCode, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::map<UINT, CGroupChatDlg*>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg* lpGroupChatDlg = iter->second;
		if (lpGroupChatDlg != NULL && lpGroupChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GROUP_INFO:
				lpGroupChatDlg->OnUpdateGroupInfo();
				break;

			case FMG_MSG_UPDATE_GROUP_NUMBER:
				lpGroupChatDlg->OnUpdateGroupNumber();
				break;

			case FMG_MSG_UPDATE_GROUP_HEADPIC:
				lpGroupChatDlg->OnUpdateGroupHeadPic();
				break;

			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
				lpGroupChatDlg->OnUpdateGMemberNumber(wParam, lParam);
				break;

			case FMG_MSG_UPDATE_GMEMBER_HEADPIC:
				lpGroupChatDlg->OnUpdateGMemberHeadPic(wParam, lParam);
				break;
			}
		}
	}
}

// ֪ͨ��ʱ�Ự���촰�ڸ���
void CMainDlg::NotifySessChatDlg(UINT nUTalkUin, UINT uMsg)
{
	std::map<UINT, CSessChatDlg*>::iterator iter;
	iter = m_mapSessChatDlg.find(nUTalkUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg* lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
 			switch (uMsg)
 			{
			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
				lpSessChatDlg->OnUpdateGMemberNumber();
				break;
 			case FMG_MSG_UPDATE_GMEMBER_HEADPIC:
 				lpSessChatDlg->OnUpdateGMemberHeadPic();
 				break;
 			}
		}
	}
}

// ֪ͨ������Ϣ���ڸ���
void CMainDlg::NotifyBuddyInfoDlg(UINT nUTalkUin, UINT uMsg)
{
	if(nUTalkUin == m_FMGClient.m_UserMgr.m_UserInfo.m_uUserID)
	{
		if(m_LogonUserInfoDlg.IsWindow())
			m_LogonUserInfoDlg.UpdateCtrlData();

		return;
	}
	
	std::map<UINT, CBuddyInfoDlg*>::iterator iter;
	iter = m_mapBuddyInfoDlg.find(nUTalkUin);
	if (iter != m_mapBuddyInfoDlg.end())
	{
		CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_BUDDY_INFO:
				lpBuddyInfoDlg->UpdateCtrls();
				break;

			case FMG_MSG_UPDATE_BUDDY_NUMBER:
				//lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case FMG_MSG_UPDATE_BUDDY_SIGN:
				//lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// ֪ͨȺ��Ա��Ϣ���ڸ���
void CMainDlg::NotifyGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, UINT uMsg)
{
	CGMemberInfoMapKey key;
	key.m_nGroupCode = nGroupCode;
	key.m_nUTalkUin = nUTalkUin;
	std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
	iter = m_mapGMemberInfoDlg.find(key);
	if (iter != m_mapGMemberInfoDlg.end())
	{
		CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GMEMBER_INFO:
				//lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
				//lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case FMG_MSG_UPDATE_GMEMBER_SIGN:
				//lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// ֪ͨȺ��Ϣ���ڸ���
void CMainDlg::NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg)
{
	std::map<UINT, CGroupInfoDlg*>::iterator iter;
	iter = m_mapGroupInfoDlg.find(nGroupCode);
	if (iter != m_mapGroupInfoDlg.end())
	{
		CGroupInfoDlg* lpGroupInfoDlg = iter->second;
		if (lpGroupInfoDlg != NULL && lpGroupInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GROUP_INFO:
				lpGroupInfoDlg->OnUpdateGroupInfo();
				break;

			case FMG_MSG_UPDATE_GROUP_NUMBER:
				lpGroupInfoDlg->OnUpdateGroupNumber();
				break;
			}
		}
	}
}

void CMainDlg::UpdateBuddyTreeCtrl(UINT uAccountID/*=0*/)
{
	CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();
	if (NULL == lpBuddyList)
		return;

	int nBuddyTeamCount = lpBuddyList->GetBuddyTeamCount();
	//��¼��һ�κ��ѷ���ؼ��Ǵ���������չ��״̬
	std::vector<BOOL> aryTeamExpandStatus;
	aryTeamExpandStatus.resize(nBuddyTeamCount);
	for(long i=0; i<nBuddyTeamCount; ++i)
		aryTeamExpandStatus[i] = m_BuddyListCtrl.IsBuddyTeamExpand(i);

	m_BuddyListCtrl.DelAllItems();

	BOOL bShowSimpleProfile = m_FMGClient.m_UserConfig.IsEnableSimpleProfile();
	long nNameStyle = m_FMGClient.m_UserConfig.GetNameStyle();
					

	//�û�ͷ��
	CString strThumbPath;
	BOOL bMale = TRUE;
	int nBuddyCount = 0;
	int nValidBuddyCount = 0;
	int nOnlineBuddyCount = 0;
	int nTeamIndex = 0;

	for (int i = 0; i < nBuddyTeamCount; i++)
	{
        nValidBuddyCount = 0;
        nBuddyCount = lpBuddyList->GetBuddyCount(i);
		nOnlineBuddyCount = lpBuddyList->GetOnlineBuddyCount(i);
		nTeamIndex = 0;

		CBuddyTeamInfo* lpBuddyTeamInfo = lpBuddyList->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			nTeamIndex = m_BuddyListCtrl.AddBuddyTeam(i);
			m_BuddyListCtrl.SetBuddyTeamName(nTeamIndex, lpBuddyTeamInfo->m_strName.c_str());
			//ATLTRACE(_T("nBuddyCount: %d.\n"), nBuddyCount);
			//m_BuddyListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nBuddyCount);
			m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineBuddyCount);
			m_BuddyListCtrl.SetBuddyTeamExpand(nTeamIndex, aryTeamExpandStatus[i]);
		}

		for (int j = 0; j < nBuddyCount; j++)
		{
			CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(i, j);
			if (lpBuddyInfo != NULL)
			{
				if(lpBuddyInfo->m_uUserID==0)
					continue;

				if(lpBuddyInfo->m_strAccount.empty())
					continue;

				if(m_bShowOnlineBuddyOnly)
				{
					if(lpBuddyInfo->m_nStatus==STATUS_OFFLINE || lpBuddyInfo->m_nStatus==STATUS_INVISIBLE /*|| lpBuddyInfo->m_nStatus==STATUS_MOBILE_OFFLINE*/)
						continue;
				}
				
				CString strUTalkNum;
				//strUTalkNum.Format(_T("%u"),lpBuddyInfo->m_nUTalkNum);
				strUTalkNum.Format(_T("%s"), lpBuddyInfo->m_strAccount.c_str());

				bMale = lpBuddyInfo->m_nGender!=0 ? FALSE : TRUE;
				BOOL bGray = FALSE;
                if (lpBuddyInfo->m_nStatus == online_type_offline || lpBuddyInfo->m_nStatus == online_type_pc_invisible)
					bGray = TRUE;
				BOOL bOnlineFlash = FALSE;
				if(uAccountID == lpBuddyInfo->m_uUserID)
					bOnlineFlash = TRUE;

				/* = m_FMGClient.GetBuddyHeadPicFullName(lpBuddyInfo->m_nUTalkNum)*/;
				//if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
				if(lpBuddyInfo->m_nFace >= (UINT)USER_THUMB_COUNT)
					lpBuddyInfo->m_nFace = 0;
				if(lpBuddyInfo->m_bUseCustomFace && lpBuddyInfo->m_bCustomFaceAvailable)
				{
					strThumbPath.Format(_T("%s%d.png"), m_FMGClient.m_UserMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
					if(!Hootina::CPath::IsFileExist(strThumbPath))
						strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
				}
				else
					strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);

				//int nIndex = m_BuddyListCtrl.AddBuddyItem(nTeamIndex, lpBuddyInfo->m_nUTalkUin);
				int nIndex = m_BuddyListCtrl.AddBuddyItem(nTeamIndex, lpBuddyInfo->m_uUserID);
				m_BuddyListCtrl.SetBuddyItemUTalkNum(nTeamIndex, nIndex, strUTalkNum);
				m_BuddyListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
				m_BuddyListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());

    //            BEGIN_PERFORMANCECOUNTER
				//LOG_INFO(_T("AccountID=%u, AccountName=%s, NickName=%s, MarkName=%s."), 
				//			lpBuddyInfo->m_uUserID,
				//			lpBuddyInfo->m_strAccount.c_str(),
				//			lpBuddyInfo->m_strNickName.c_str(),
				//			lpBuddyInfo->m_strMarkName.c_str());
    //            END_PERFORMANCECOUNTER
				
				if(nNameStyle == NAME_STYLE_SHOW_NICKNAME)
				{
					m_BuddyListCtrl.SetBuddyItemMode(nTeamIndex, nIndex, BLCTRL_DISPLAY_SHOW_NICKNAME);
				}
				else if(nNameStyle == NAME_STYLE_SHOW_ACCOUNT)
				{
					m_BuddyListCtrl.SetBuddyItemMode(nTeamIndex, nIndex, BLCTRL_DISPLAY_SHOW_ACCOUNT);
				}
				else
				{
					m_BuddyListCtrl.SetBuddyItemMode(nTeamIndex, nIndex, BLCTRL_DISPLAY_SHOW_NICKNAME_ACCOUNT);
				}
				
				//��ˬ���ϲ���ʾ����ǩ��
				if(bShowSimpleProfile)
					m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str(), FALSE);
				else
					m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str(), TRUE);
				
				m_BuddyListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bMale);
				m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
				
				//SetBuddyItemOnline���һ�����������߶���
				m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, !bGray, bOnlineFlash);
                if (lpBuddyInfo->m_nStatus == online_type_android_cellular || lpBuddyInfo->m_nStatus == online_type_android_wifi)
				{
					strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
					m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, TRUE);
				}
                else if (lpBuddyInfo->m_nStatus == online_type_offline || lpBuddyInfo->m_nStatus == online_type_pc_invisible)
                {
                    strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
                    m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, FALSE);
                }
                    
				++nValidBuddyCount;
			}
		}
		m_BuddyListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nValidBuddyCount);
	}

	if (m_BuddyListCtrl.IsWindowVisible())
		m_BuddyListCtrl.Invalidate();
}

void CMainDlg::UpdateGroupTreeCtrl()
{
	CGroupList* lpGroupList = m_FMGClient.GetGroupList();
	if (NULL == lpGroupList)
		return;

	m_GroupListCtrl.DelAllItems();

	int nTeamIndex = m_GroupListCtrl.AddBuddyTeam(0);
	m_GroupListCtrl.SetBuddyTeamName(nTeamIndex, _T("�ҵ�Ⱥ"));
	m_GroupListCtrl.SetBuddyTeamExpand(nTeamIndex, TRUE);

	int nActualGroupCount = 0;
	int nGroupCount = lpGroupList->GetGroupCount();
	for (int i = 0; i < nGroupCount; i++)
	{
		CGroupInfo* lpGroupInfo = lpGroupList->GetGroup(i);
		if(lpGroupInfo==NULL || lpGroupInfo->m_strName.empty())
			continue;

		tstring strFileName = m_FMGClient.GetGroupHeadPicFullName(lpGroupInfo->m_nGroupNumber);
		if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
			strFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");

		int nIndex = m_GroupListCtrl.AddBuddyItem(nTeamIndex, lpGroupInfo->m_nGroupCode);
		m_GroupListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strAccount.c_str());
		m_GroupListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
		m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);

		//LOG_INFO(_T("GroupID=%u, GroupName=%s, GroupName=%s."), 
		//			lpGroupInfo->m_nGroupCode,
		//			lpGroupInfo->m_strAccount.c_str(),
		//			lpGroupInfo->m_strName.c_str());

		++nActualGroupCount;
		
	}

	m_GroupListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nActualGroupCount);
	m_GroupListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nActualGroupCount);

	if (m_GroupListCtrl.IsWindowVisible())
		m_GroupListCtrl.Invalidate();
}

void CMainDlg::UpdateRecentTreeCtrl()
{
	CRecentList* lpRecentList = m_FMGClient.GetRecentList();
	if (NULL == lpRecentList)
		return;

	m_RecentListCtrl.DelAllItems();

	//TODO: Ŀǰֻ��һ�������ϵ��
	int nTeamIndex = m_RecentListCtrl.AddBuddyTeam(-1);
	//m_RecentListCtrl.SetBuddyTeamName(nTeamIndex, _T("�����ϵ��"));
	m_RecentListCtrl.SetBuddyTeamName(nTeamIndex, _T(""));
	m_RecentListCtrl.SetBuddyTeamExpand(nTeamIndex, TRUE);

	int nCount = 0;
	CString strUserID;
	UINT uFaceID = 0;
	CString strThumbPath;
	TCHAR szTime[8];
	long nNewMsgCount;
	CMessageSender* pMessageSender = NULL;

	int nRecentCount = lpRecentList->GetRecentCount();
	for (int i=nRecentCount-1; i >=0 ; --i)
	{
		CRecentInfo* lpRecentInfo = lpRecentList->GetRecent(i);
		if (lpRecentInfo == NULL)
			continue;
		
		if (FMG_MSG_TYPE_BUDDY == lpRecentInfo->m_nType)			// ����
		{
			CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();
			//�������������ѣ�����ݺ�����Ϣ��ʾ
			if (lpBuddyList != NULL)
			{
				CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(lpRecentInfo->m_uUserID);
				if (lpBuddyInfo != NULL)
				{
					BOOL bGray = TRUE;
					if(lpBuddyInfo->m_nStatus!=STATUS_OFFLINE && lpBuddyInfo->m_nStatus!=STATUS_INVISIBLE)
						bGray = FALSE;

                    if (lpBuddyInfo->m_bUseCustomFace&& lpBuddyInfo->m_bCustomFaceAvailable)
					{
						strThumbPath.Format(_T("%s%d.png"), m_FMGClient.m_UserMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
						if(!Hootina::CPath::IsFileExist(strThumbPath))
							strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
					}
					else
						strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);

					int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
					strUserID.Format(_T("%u"), lpBuddyInfo->m_uUserID);
					m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpBuddyInfo->m_uUserID);
					m_RecentListCtrl.SetBuddyItemAccountID(nTeamIndex, nIndex, lpBuddyInfo->m_uUserID);

					m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
					//m_RecentListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
					m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
					m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);
					if(lpBuddyInfo->m_nStatus == STATUS_MOBILE_ONLINE)
					{
						strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
						m_RecentListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath);
					}
					
					memset(szTime, 0, sizeof(szTime));
					if(IsToday(lpRecentInfo->m_MsgTime))	
						FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
					else
						FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
					m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

					pMessageSender = m_FMGClient.m_UserMgr.m_MsgList.GetMsgSender(FMG_MSG_TYPE_BUDDY,  lpBuddyInfo->m_uUserID);
					if(pMessageSender != NULL)
						nNewMsgCount = pMessageSender->GetDisplayMsgCount();
					else
						nNewMsgCount = 0;
					m_RecentListCtrl.SetBuddyItemNewMsgCount(nTeamIndex, nIndex, nNewMsgCount);

					nCount++;
				}
				//����Ѿ����Ǻ����ˣ�����������ϵ���е���Ϣ��ʾ
				//else
				//{
				//	uFaceID = lpRecentInfo->m_uFaceID;
				//	if(uFaceID > USER_THUMB_COUNT)
				//		uFaceID = 0;

				//	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, uFaceID);

				//	int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
				//	strUserID.Format(_T("%u"), lpRecentInfo->m_uUserID);
				//	m_RecentListCtrl.SetBuddyItemAccountID(nTeamIndex, nIndex, lpRecentInfo->m_uUserID);
				//	m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpRecentInfo->m_uUserID);

				//	m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpRecentInfo->m_szName);
				//	//m_RecentListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
				//	m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, TRUE);
				//	m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);
				//	
				//	memset(szTime, 0, sizeof(szTime));
				//	if(IsToday(lpRecentInfo->m_MsgTime))	
				//		FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
				//	else
				//		FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
				//	m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

				//	nCount++;
				//}
			}	
		}
		else if (FMG_MSG_TYPE_GROUP == lpRecentInfo->m_nType)	// Ⱥ
		{
			CGroupList* lpGroupList = m_FMGClient.GetGroupList();
			if (lpGroupList != NULL)
			{
				CGroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(lpRecentInfo->m_uUserID);
				if (lpGroupInfo != NULL)
				{
					
					strThumbPath.Format(_T("%sImage\\DefGroupHeadPic.png"), g_szHomePath);

					int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
					m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpGroupInfo->m_nGroupCode);
					m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
					m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, FALSE);
					m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);

					memset(szTime, 0, sizeof(szTime));
					if(IsToday(lpRecentInfo->m_MsgTime))	
						FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
					else
						FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
					m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

					pMessageSender = m_FMGClient.m_UserMgr.m_MsgList.GetMsgSender(FMG_MSG_TYPE_GROUP,  lpRecentInfo->m_uUserID);
					if(pMessageSender != NULL)
						nNewMsgCount = pMessageSender->GetDisplayMsgCount();	
					else
						nNewMsgCount = 0;	
					m_RecentListCtrl.SetBuddyItemNewMsgCount(nTeamIndex, nIndex, nNewMsgCount);

					nCount++;
				}
			}
		}
		else if (FMG_MSG_TYPE_SESS == lpRecentInfo->m_nType)	// ������������
		{
		}
		
	}// end for-loop

	m_RecentListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nCount);
	m_RecentListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nCount);

	if (m_RecentListCtrl.IsWindowVisible())
		m_RecentListCtrl.Invalidate();
}

void CMainDlg::OnTrayIcon_LButtunUp()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList* lpMsgList = m_FMGClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CMessageSender* lpMsgSender = lpMsgList->GetLastMsgSender();
			if (lpMsgSender != NULL)
			{			
				long nType = lpMsgSender->GetMsgType();
				UINT nSenderId = lpMsgSender->GetSenderId();
				UINT nGroupCode = lpMsgSender->GetGroupCode();


				switch (nType)
				{
				case FMG_MSG_TYPE_BUDDY:
					ShowBuddyChatDlg(nSenderId, TRUE);
					break;
				case FMG_MSG_TYPE_GROUP:
					ShowGroupChatDlg(nSenderId, TRUE);
					break;
				case FMG_MSG_TYPE_SESS:
					ShowSessChatDlg(nGroupCode, nSenderId, TRUE);
					break;
				}
			}
		}
	}
	else if(m_dwAddFriendTimerId != 0)
	{
		KillTimer(m_dwAddFriendTimerId);
		m_dwAddFriendTimerId = 0;

		ShowAddFriendConfirmDlg();
	}
	else
	{
		if (m_LoginDlg.IsWindow())
		{
			m_LoginDlg.ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_LoginDlg.m_hWnd);
		}
		else if (IsWindow())
		{
			ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_hWnd);
		}
	}

	//�Ƚ�������ͼ��ָ�������״̬
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n����"), 
						m_FMGClient.m_UserMgr.m_UserInfo.m_strNickName.data(),				
						m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.data());
						
	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
}

void CMainDlg::OnTrayIcon_RButtunUp()
{
	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DestroyWindow();

	int nPos = MAIN_PANEL_TRAYICON_SUBMENU_INDEX;		//��¼ǰ�����̲˵�����
	
	if(!m_bAlreadyLogin)
		nPos = MAIN_PANEL_TRAYICON_SUBMENU_INDEX;
	else
	{
		if(m_bPanelLocked)
			nPos = MAIN_PANEL_LOCK_SUBMENU_INDEX;		//�������ʱ�����̲˵�����
		else
			nPos = MAIN_PANEL_TRAYICON_SUBMENU2_INDEX;		//�������������̲˵�����
	}


	CPoint pt;
	GetCursorPos(&pt);

	::SetForegroundWindow(m_hWnd);

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nPos);
    //�����Ƿ��Ѿ��������޸ġ��ر�����������ѡ��
    if (m_FMGClient.m_UserConfig.IsEnableMute())
        PopupMenu.ModifyMenu(ID_MENU_MUTE, MF_BYCOMMAND | MF_STRING /*| MF_POPUP*/, ID_MENU_MUTE, _T("�ر���������(�ѹر�)"));
    else
        PopupMenu.ModifyMenu(ID_MENU_MUTE, MF_BYCOMMAND | MF_STRING /*| MF_POPUP*/, ID_MENU_MUTE, _T("�ر���������"));
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
	::PostMessage(m_hWnd, WM_NULL, 0, 0);
}

void CMainDlg::OnTrayIcon_MouseHover()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList* lpMsgList = m_FMGClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CRect rcTrayIcon(0, 0, 0, 0);
			
			//Win7ϵͳ���ܲ�ȡ���ַ�ʽ��ȡ����������ͼ���λ��
			if(!RunTimeHelper::IsVista())
				m_TrayIcon.GetTrayIconRect(&rcTrayIcon);
			
			//TODO: Ȩ��֮�ƣ�����Ҫ�Ӹ����Ͻ��λ�ò��Ե�����
			if((rcTrayIcon.left==0 && rcTrayIcon.right==0) || (rcTrayIcon.top==0 && rcTrayIcon.bottom==0))
			{	
				POINT pt;
				::GetCursorPos(&pt);
				rcTrayIcon.left   = pt.x-80;
				rcTrayIcon.top    = pt.y-90;
				rcTrayIcon.right  = pt.x+20;
				rcTrayIcon.bottom = pt.y+10;
			}
			m_rcTrayIconRect = rcTrayIcon; 
			m_MsgTipDlg.m_rcTrayIcon2 = rcTrayIcon;
			m_MsgTipDlg.m_lpFMGClient = &m_FMGClient;
			m_MsgTipDlg.m_hMainDlg = m_hWnd;
			m_MsgTipDlg.m_rcTrayIcon = rcTrayIcon;
			if (!m_MsgTipDlg.IsWindow())
			{
				m_MsgTipDlg.Create(m_hWnd);
			}
			
			m_MsgTipDlg.ShowWindow(SW_SHOWNOACTIVATE);	
		}
	}
	//::OutputDebugString(_T("WM_MOUSEHOVER\n"));
}

void CMainDlg::OnTrayIcon_MouseLeave()
{
	if (m_MsgTipDlg.IsWindow())
	{
		CRect rcWindow;
		m_MsgTipDlg.GetWindowRect(&rcWindow);

		POINT pt = {0};
		::GetCursorPos(&pt);

		if (!::PtInRect(&rcWindow, pt) && !::PtInRect(&m_rcTrayIconRect, pt))
		{
			m_MsgTipDlg.DestroyWindow();
			//m_MsgTipDlg.ShowWindow(SW_HIDE);
		}
		else
			m_MsgTipDlg.StartTrackMouseLeave();
	}
	//::OutputDebugString(_T("WM_MOUSELEAVE\n"));
}

BOOL CMainDlg::LoadAppIcon(long nStatus)
{
	DestroyAppIcon();

	tstring strFileName;
	switch (nStatus)
	{
	case STATUS_ONLINE:
		strFileName = Hootina::CPath::GetAppPath() + _T("Image\\imonline.ico");
		break;
	case STATUS_OFFLINE:
		strFileName = Hootina::CPath::GetAppPath() + _T("Image\\offline.ico");
		break;
	}
	m_hAppIcon = AtlLoadIconImage(strFileName.c_str(), LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	return m_hAppIcon != NULL ? TRUE : FALSE;
}

void CMainDlg::DestroyAppIcon()
{
	if (m_hAppIcon != NULL)
	{
		::DestroyIcon(m_hAppIcon);
		m_hAppIcon = NULL;
	}
}

BOOL CMainDlg::LoadLoginIcon()
{
	DestroyLoginIcon();

	CString strFileName;
	for (int i = 0; i < 6; i++)
	{
		strFileName.Format(_T("%sImage\\Loading_%d.ico"), Hootina::CPath::GetAppPath().c_str(), i+1);
		m_hLoginIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}

	return TRUE;
}

void CMainDlg::DestroyLoginIcon()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_hLoginIcon[i] != NULL)
		{
			::DestroyIcon(m_hLoginIcon[i]);
			m_hLoginIcon[i] = NULL;
		}
	}
}

BOOL CMainDlg::LoadAddFriendIcon()
{
	DestroyAddFriendIcon();

	CString strFileName;
	for (int i = 0; i < ARRAYSIZE(m_hAddFriendIcon); i++)
	{
		strFileName.Format(_T("%sImage\\speaker_%d.ico"), Hootina::CPath::GetAppPath().c_str(), i+1);
		m_hAddFriendIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}

	return TRUE;
}

void CMainDlg::DestroyAddFriendIcon()
{
	for (int i = 0; i < ARRAYSIZE(m_hAddFriendIcon); i++)
	{
		if (m_hAddFriendIcon[i] != NULL)
		{
			::DestroyIcon(m_hAddFriendIcon[i]);
			m_hAddFriendIcon[i] = NULL;
		}
	}
}

void CMainDlg::UpdateMsgIcon()
{
	CMessageList* lpMsgList = m_FMGClient.GetMessageList();
	if (NULL == lpMsgList)
		return;

	CMessageSender* lpMsgSender = lpMsgList->GetLastMsgSender();
	if (NULL == lpMsgSender)
		return;
	
	long nMsgType = lpMsgSender->GetMsgType();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	if (m_nLastMsgType != nMsgType || m_nLastSenderId != nSenderId)
	{
		m_nLastMsgType = nMsgType;
		m_nLastSenderId = nSenderId;

		if (m_hMsgIcon != NULL)
		{
			::DestroyIcon(m_hMsgIcon);
			m_hMsgIcon = NULL;
		}

		CString strHeadPicFileName;

		if (FMG_MSG_TYPE_BUDDY == nMsgType)
		{
			//strHeadPicFileName = GetHeadPicFullName(0, nSenderId);
			strHeadPicFileName.Format(_T("%sUsers\\%s\\UserThumb\\%d.png"), g_szHomePath, m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str(), nSenderId);
			if(!Hootina::CPath::IsFileExist(strHeadPicFileName))
				strHeadPicFileName.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, m_FMGClient.m_UserMgr.GetFaceID(nSenderId));
		}
		else if (FMG_MSG_TYPE_GROUP == nMsgType)
			strHeadPicFileName = GetHeadPicFullName(nSenderId, 0);
		else if (FMG_MSG_TYPE_SESS == nMsgType)
			strHeadPicFileName = GetHeadPicFullName(nGroupCode, nSenderId);
// 		else if (long_SYSGROUP == nMsgType)
// 			strHeadPicFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefSysHeadPic.ico");

		m_hMsgIcon = ExtractIcon(strHeadPicFileName);
		if (NULL == m_hMsgIcon)
		{
			if (FMG_MSG_TYPE_BUDDY == nMsgType || FMG_MSG_TYPE_SESS == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_BUDDY, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			else if (FMG_MSG_TYPE_GROUP == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_GROUPCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			//else if (long_SYSGROUP == nMsgType)
			//	m_hMsgIcon = AtlLoadIconImage(IDI_SYS, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		}
	}
}

CString CMainDlg::GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin)
{
	
	
	//UINT nGroupNum, nUTalkNum;
	//GetNumber(nGroupCode, nUTalkUin, nGroupNum, nUTalkNum);

	//if (nGroupCode != 0 && nUTalkUin != 0)	// Ⱥ��Ա
	//	return m_FMGClient.GetSessHeadPicFullName(nUTalkNum).c_str();
	//else if (nGroupCode != 0)			// Ⱥ
	//	return m_FMGClient.GetGroupHeadPicFullName(nGroupNum).c_str();
	//else								// ����
	//	return m_FMGClient.GetBuddyHeadPicFullName(nUTalkUin).c_str();

	return _T("");
}

void CMainDlg::GetNumber(UINT nGroupCode, UINT nUTalkUin, UINT& nGroupNum, UINT& nUTalkNum)
{
	/*nGroupNum = nUTalkNum = 0;

	if (nGroupCode != 0 && nUTalkUin != 0)
	{
		CGroupList* lpGroupList = m_FMGClient.GetGroupList();
		if (lpGroupList != NULL)
		{
			CBuddyInfo* lpBuddyInfo = lpGroupList->GetGroupMemberByCode(nGroupCode, nUTalkUin);
			if (lpBuddyInfo != NULL)
				nUTalkNum = lpBuddyInfo->m_nUTalkNum;
		}
	}
	else if (nGroupCode != 0)
	{
		CGroupList* lpGroupList = m_FMGClient.GetGroupList();
		if (lpGroupList != NULL)
		{
			CGroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
			if (lpGroupInfo != NULL)
				nGroupNum = lpGroupInfo->m_nGroupNumber;
		}
	}
	else if (nUTalkUin != 0)
	{
		CBuddyList* lpBuddyList = m_FMGClient.GetBuddyList();
		if (lpBuddyList != NULL)
		{
			CBuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
			if (lpBuddyInfo != NULL)
				nUTalkNum = lpBuddyInfo->m_nUTalkNum;
		}
	}*/
}

HICON CMainDlg::ExtractIcon(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL ==*lpszFileName)
		return NULL;

	int cx = 16, cy = 16;
	HBITMAP hBmp = NULL;

	Gdiplus::Bitmap imgHead(lpszFileName);
	if (imgHead.GetLastStatus() != Gdiplus::Ok)
		return NULL;

	if (imgHead.GetWidth() != cx || imgHead.GetHeight() != cy)
	{
		Gdiplus::Bitmap* pThumbnail = (Gdiplus::Bitmap*)imgHead.GetThumbnailImage(cx, cy, NULL, NULL);
		if (pThumbnail != NULL)
		{
			pThumbnail->GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
			delete pThumbnail;
		}
	}
	else
	{
		imgHead.GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
	}

	if (NULL == hBmp)
		return NULL;

	HICON hIcon = NULL;

	CImageList imgList;
	BOOL bRet = imgList.Create(cx, cy, TRUE | ILC_COLOR32, 1, 1);
	if (bRet)
	{
		imgList.SetBkColor(RGB(255,255,255));
		imgList.Add(hBmp);
		hIcon = imgList.ExtractIcon(0);
	}
	::DeleteObject(hBmp);
	imgList.Destroy();

	return hIcon;
}

void CMainDlg::CloseAllDlg()
{
	{
		std::map<UINT, CBuddyChatDlg*>::iterator iter;
		for (iter = m_mapBuddyChatDlg.begin(); iter != m_mapBuddyChatDlg.end(); iter++)
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->DestroyWindow();

				delete lpBuddyChatDlg;
			}
		}
		m_mapBuddyChatDlg.clear();
	}

	{
		std::map<UINT, CGroupChatDlg*>::iterator iter;
		for (iter = m_mapGroupChatDlg.begin(); iter != m_mapGroupChatDlg.end(); iter++)
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
		}
		m_mapGroupChatDlg.clear();
	}

	{
		std::map<UINT, CSessChatDlg*>::iterator iter;
		for (iter = m_mapSessChatDlg.begin(); iter != m_mapSessChatDlg.end(); iter++)
		{
			CSessChatDlg* lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
		}
		m_mapSessChatDlg.clear();
	}

	{
		std::map<UINT, CBuddyInfoDlg*>::iterator iter;
		for (iter = m_mapBuddyInfoDlg.begin(); iter != m_mapBuddyInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapBuddyInfoDlg.clear();
	}

	{
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		for (iter = m_mapGMemberInfoDlg.begin(); iter != m_mapGMemberInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapGMemberInfoDlg.clear();
	}

	{
		std::map<UINT, CGroupInfoDlg*>::iterator iter;
		for (iter = m_mapGroupInfoDlg.begin(); iter != m_mapGroupInfoDlg.end(); iter++)
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
		}
		m_mapGroupInfoDlg.clear();
	}

	//���ٲ��Һ��ѶԻ���
	if(m_pFindFriendDlg!=NULL && m_pFindFriendDlg->IsWindow())
		m_pFindFriendDlg->DestroyWindow();

	//�����ҵ����϶Ի���
	if(m_LogonUserInfoDlg.IsWindow())
		m_LogonUserInfoDlg.DestroyWindow();

    //������Ϣ����
    if (m_BuddyInfoFloatWnd.IsWindow())
        m_BuddyInfoFloatWnd.DestroyWindow();
}

// �Ӳ˵�ID��ȡ��Ӧ��UTalk_STATUS
long CMainDlg::GetStatusFromMenuID(int nMenuID)
{
	switch (nMenuID)
	{
	case ID_MENU_IMONLINE:
		return STATUS_ONLINE;
	case ID_MENU_IMOFFLINE:
		return STATUS_OFFLINE;
	default:
		return STATUS_OFFLINE;
	}
}

// ����ָ��״̬����״̬�˵���ť��ͼ��
void CMainDlg::StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, long nStatus)
{
	LPCTSTR lpszFileName;

	switch (nStatus)
	{
	case STATUS_ONLINE:
		lpszFileName = _T("Status\\imonline.png");
		break;
	case STATUS_OFFLINE:
		lpszFileName = _T("Status\\imoffline.png");
		break;
	default:
		return;
	}

	btnStatus.SetIconPic(lpszFileName);
	btnStatus.Invalidate();
}

//��������״̬�Ժ����б����������������ǰ�棬�������ں���
void CMainDlg::BuddyListSortOnStaus()
{
	long nTeamCount = m_FMGClient.m_UserMgr.m_BuddyList.m_arrBuddyTeamInfo.size();
	for(long i=0; i<nTeamCount; ++i)
	{
		m_FMGClient.m_UserMgr.m_BuddyList.m_arrBuddyTeamInfo[i]->Sort();
	}
}

void CMainDlg::ShowAddFriendConfirmDlg()
{	
	size_t nCount = m_FMGClient.m_aryAddFriendInfo.size();
	if(nCount == 0)
		return;

	AddFriendInfo* pAddFriendInfo = m_FMGClient.m_aryAddFriendInfo[nCount-1];
	if(pAddFriendInfo == NULL)
		return;

	CString strWindowTitle;
	CString strInfo;
	CAddFriendConfirmDlg AddFriendConfirmDlg;
	AddFriendConfirmDlg.m_pFMGClient = &m_FMGClient;

	//���˼��Լ�
	if(pAddFriendInfo->nCmd==Apply)
	{
		strWindowTitle = _T("�Ӻ�������");
		strInfo.Format(_T("%s(%s)���������Ϊ���ѣ��Ƿ�ͬ�⣿"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
		AddFriendConfirmDlg.ShowAgreeButton(TRUE);
		AddFriendConfirmDlg.ShowRefuseButton(TRUE);
		AddFriendConfirmDlg.ShowOKButton(FALSE);
	}
	//�Լ������ܾ����˻��߱����˾ܾ�
	else if(pAddFriendInfo->nCmd==Refuse)
	{
		strWindowTitle = g_strAppTitle.c_str();
		strInfo.Format(_T("�����߶Է�(%s(%s))�ܾ��˼Ӻ�������"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
		AddFriendConfirmDlg.ShowAgreeButton(FALSE);
		AddFriendConfirmDlg.ShowRefuseButton(FALSE);
		AddFriendConfirmDlg.ShowOKButton(TRUE);
	}
	//�Լ����߱���ͬ���˼Ӻ�������
	else if(pAddFriendInfo->nCmd==Agree)
	{		
		strWindowTitle = g_strAppTitle.c_str();
        //Ⱥ�Ŵ���0xFFFFFFF
        if (pAddFriendInfo->uAccountID < 0xFFFFFFF)
			strInfo.Format(_T("����%s(%s)�Ѿ��Ǻ���������ʼ����ɡ�"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
		else
			strInfo.Format(_T("���Ѿ��ɹ�����Ⱥ��%s(%s)��������ʼ����ɡ�"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
		AddFriendConfirmDlg.ShowAgreeButton(FALSE);
		AddFriendConfirmDlg.ShowRefuseButton(FALSE);
		AddFriendConfirmDlg.ShowOKButton(TRUE);
		
		//m_FMGClient.m_UserMgr.AddFriend(pAddFriendInfo->uAccountID, pAddFriendInfo->szAccountName, pAddFriendInfo->szNickName);
		//PostMessage(FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
		//���»�ȡ�º����б�����
        m_FMGClient.GetFriendList();
	}
	
	AddFriendConfirmDlg.SetWindowInfo(strInfo);
	AddFriendConfirmDlg.SetWindowTitle(strWindowTitle);

	int nRet = AddFriendConfirmDlg.DoModal(m_hWnd, NULL);
	
	if(nRet==ID_ADDCONFIRM_REFUSE || nRet==ID_ADDCONFIRM_AGREE)
	{		
		UINT uCmd = (nRet==ID_ADDCONFIRM_AGREE ? Agree : Refuse);
		m_FMGClient.ResponseAddFriendApply(pAddFriendInfo->uAccountID, uCmd);
	}
	
	DELETE_PTR(pAddFriendInfo);
	m_FMGClient.m_aryAddFriendInfo.erase(m_FMGClient.m_aryAddFriendInfo.end()-1);
}

BOOL CMainDlg::InsertTeamMenuItem(CSkinMenu& popMenu)
{
	//if(popMenu.GetMenuItemCount() >= 5)
	//	return TRUE;
	
	int nTeamIndex = -1;
	int nItemIndex = -1;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nItemIndex);
	if(nTeamIndex<0 || nItemIndex<0)
		return FALSE;

	CSkinMenu* subMenu = new CSkinMenu();
	subMenu->CreatePopupMenu();


	size_t nCount = m_FMGClient.m_UserMgr.m_BuddyList.m_arrBuddyTeamInfo.size();
	for(size_t i=0; i<nCount; ++i)
	{
		if(i == nTeamIndex)
			continue;
		
		subMenu->AppendMenu(MF_STRING|MF_BYPOSITION, TEAM_MENU_ITEM_BASE+i, m_FMGClient.m_UserMgr.m_BuddyList.m_arrBuddyTeamInfo[i]->m_strName.c_str());
	}

	popMenu.ModifyMenu(8, MF_POPUP|MF_BYPOSITION, (UINT_PTR)subMenu->m_hMenu, NULL);

	return TRUE;
}

void CMainDlg::SaveCurrentLogonUserToFile()
{	
	CIniFile iniFile;
	CString strIniFile;
	strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
	CString strAccountList;
	iniFile.ReadString(_T("LogonUserList"), _T("AccountName"), _T(""), strAccountList.GetBuffer(256), 256, strIniFile);
	strAccountList.ReleaseBuffer();
	strAccountList += m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str();
	strAccountList +=_T(";");
	iniFile.WriteString(_T("LogonUserList"), _T("AccountName"), strAccountList, strIniFile);
}

void CMainDlg::DeleteCurrentUserFromFile()
{	
	CIniFile iniFile;
	CString strIniFile;
	strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
	CString strAccountList;
	iniFile.ReadString(_T("LogonUserList"), _T("AccountName"), _T(""), strAccountList.GetBuffer(256), 256, strIniFile);
	strAccountList.ReleaseBuffer();
	
	CString strCurrentAccount;
	strCurrentAccount.Format(_T("%s;"), m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());
	strAccountList.Replace(strCurrentAccount, _T(""));
	iniFile.WriteString(_T("LogonUserList"), _T("AccountName"), strAccountList, strIniFile);
}

BOOL CMainDlg::IsFilesTransferring()
{
	std::map<UINT, CBuddyChatDlg*>::iterator iter;
	for (iter = m_mapBuddyChatDlg.begin(); iter != m_mapBuddyChatDlg.end(); iter++)
	{
		CBuddyChatDlg* lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg == NULL)
			continue;
		if(lpBuddyChatDlg->IsFilesTransferring())
			return TRUE;
	}
	
	return FALSE;
}

void CMainDlg::CreateEssentialDirectories()
{
	CString strAppPath(g_szHomePath);
	
	CString strUsersDirectory(strAppPath);
	strUsersDirectory += _T("Users");
	if(!Hootina::CPath::IsDirectoryExist(strUsersDirectory))
		Hootina::CPath::CreateDirectory(strUsersDirectory);

	CString strCurrentUserDirectory;
	strCurrentUserDirectory.Format(_T("%s\\%s"), strUsersDirectory, m_FMGClient.m_UserMgr.m_UserInfo.m_strAccount.c_str());

	if(!Hootina::CPath::IsDirectoryExist(strCurrentUserDirectory))
		Hootina::CPath::CreateDirectory(strCurrentUserDirectory);

	CString strChatImagesDirectory;
	strChatImagesDirectory.Format(_T("%s\\ChatImage\\"), strCurrentUserDirectory);
	if(!Hootina::CPath::IsDirectoryExist(strChatImagesDirectory))
		Hootina::CPath::CreateDirectory(strChatImagesDirectory);

	CString strUserThumbDirectory;
	strUserThumbDirectory.Format(_T("%s\\UserThumb\\"), strCurrentUserDirectory);
	if(!Hootina::CPath::IsDirectoryExist(strUserThumbDirectory))
		Hootina::CPath::CreateDirectory(strUserThumbDirectory);

}