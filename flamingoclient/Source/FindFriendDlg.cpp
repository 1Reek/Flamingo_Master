#include "stdafx.h"
#include "FindFriendDlg.h"
#include "FlamingoClient.h"
#include "UserSnapInfoDlg.h"
#include "Utils.h"
#include "EncodeUtil.h"

// CFindFriendDlgʵ�ִ���
CFindFriendDlg::CFindFriendDlg()
{
	m_pFMGClient = NULL;
}

CFindFriendDlg::~CFindFriendDlg()
{
}

BOOL CFindFriendDlg::PreTranslateMessage(MSG* pMsg)
{
	//֧�ֻس�������
	if (pMsg->hwnd==m_hWnd && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
 	{
		PostMessage(WM_COMMAND, (WPARAM)IDC_BTN_ADD, 0);
		return TRUE;
 	}

	if (pMsg->hwnd==m_hWnd && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
 	{
		ShowWindow(SW_HIDE);
		return TRUE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CFindFriendDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	AddMessageFilter();

	InitUI();
	CenterWindow(m_pFMGClient->m_UserMgr.m_hCallBackWnd);
	
	return TRUE;
}

BOOL CFindFriendDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\FindFriendDlgBg.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("������ϵ�˻�Ⱥ��"));	
	m_SkinDlg.MoveWindow(0, 0, 350, 200, TRUE);


	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_edtAddId.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtAddId.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtAddId.SetTransparent(TRUE, hDlgBgDC);
	m_edtAddId.SubclassWindow(GetDlgItem(IDC_EDIT_ADD));

	m_btnAdd.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnAdd.SetTransparent(TRUE, hDlgBgDC);
	m_btnAdd.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnAdd.SetRound(4, 4);
	m_btnAdd.SubclassWindow(GetDlgItem(IDC_BTN_ADD));
	m_btnAdd.SetWindowText(_T("����"));
	m_btnAdd.SetToolTipText(_T("�����ť���в���"));

	m_btnFindTypeSingle.Attach(GetDlgItem(IDC_RADIO_FINDTYPE1));
	m_btnFindTypeSingle.SetCheck(TRUE);
	//m_btnFindTypeSingle.SetTransparent(TRUE, hDlgBgDC);

	m_btnFindTypeGroup.Attach(GetDlgItem(IDC_RADIO_FINDTYPE2));
	//m_btnFindTypeGroup.SetTransparent(TRUE, hDlgBgDC);

	m_staticAddInfo.SetTransparent(TRUE, hDlgBgDC);
	m_staticAddInfo.SetLinkColor(RGB(225, 0, 0));
	m_staticAddInfo.SetLinkType(SKIN_LINK_ADDNEW);
	m_staticAddInfo.SubclassWindow(GetDlgItem(IDC_STATIC_ADDINFO));

	return TRUE;
}

void CFindFriendDlg::OnAddFriend(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strAccountToAdd;
	m_edtAddId.GetWindowText(strAccountToAdd);
	strAccountToAdd.Trim();
	if(strAccountToAdd.GetLength()>32 || strAccountToAdd.GetLength()<=0)
	{
		m_staticAddInfo.SetWindowText(_T("���ҵ��˺ų��ȱ�����1��30���ַ�֮�䣡"));
		m_edtAddId.SetFocus();

		return;
	}

	long nRelationType = 0;
	if(m_pFMGClient->m_UserMgr.IsSelf(strAccountToAdd))
		nRelationType = 1;
    //ע����԰����˺ź��������userid�����Ե��¿��Լ��Ѿ��Ǻ��ѵ��û�Ϊ���ѣ��ⲻ��bug��Release�治�����������
	else if(m_pFMGClient->m_UserMgr.IsFriend(strAccountToAdd))
		nRelationType = 2;
	
	if(nRelationType != 0)
	{
		TCHAR szData[64];
		CUserSnapInfoDlg userSnapInfoDlg;
		userSnapInfoDlg.SetUserFaceID(m_pFMGClient->m_UserMgr.GetFaceID(strAccountToAdd));
		memset(szData, 0, sizeof(szData));
		_stprintf_s(szData, ARRAYSIZE(szData), _T("�˻���%s"), strAccountToAdd.GetString());
		userSnapInfoDlg.SetAccountName(szData);
		memset(szData, 0, sizeof(szData));
		_stprintf_s(szData, ARRAYSIZE(szData), _T("�ǳƣ�%s"), m_pFMGClient->m_UserMgr.GetNickName(strAccountToAdd).c_str());
		userSnapInfoDlg.SetNickName(szData);
		if(nRelationType == 1)
			userSnapInfoDlg.SetOperationDescription(_T("����������Լ�Ϊ���ѡ�"));
		else if(nRelationType == 2)
			userSnapInfoDlg.SetOperationDescription(_T("���˻��Ѿ������ĺ��ѣ�������ӡ�"));

		userSnapInfoDlg.SetOKButtonText(_T("��Ϊ����"));
		userSnapInfoDlg.EnableOKButton(FALSE);

		userSnapInfoDlg.DoModal(m_hWnd, NULL);
	
		return;
	}
	
	long nType = m_btnFindTypeSingle.GetCheck() ? 1 : 2;
	//�����˻�����Ӻ���	
	m_pFMGClient->FindFriend(strAccountToAdd, nType, m_hWnd);
	
	m_staticAddInfo.SetWindowText(_T("���ڲ����û������Ե�..."));	
}

LRESULT CFindFriendDlg::OnFindFriendResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFindFriendResult* pResult = (CFindFriendResult*)lParam;
	if(pResult == NULL)
		return 0;
	
	long nResultCode = pResult->m_nResultCode;
	
	if(nResultCode == FIND_FRIEND_NOT_FOUND)
		m_staticAddInfo.SetWindowText(_T("�����ҵ��˻������ڣ�"));
	else if(nResultCode == FIND_FRIEND_FAILED)
		m_staticAddInfo.SetWindowText(_T("������ϣ������˻�ʧ�ܣ�"));
	else if(nResultCode == FIND_FRIEND_FOUND)
	{
		m_staticAddInfo.SetWindowText(_T(""));
		
		CUserSnapInfoDlg userSnapInfoDlg;

        BOOL bGroup = IsGroupTarget(pResult->m_uAccountID);
		
		userSnapInfoDlg.SetAccountID(pResult->m_uAccountID);
		TCHAR szInfo[32] = {0};
		userSnapInfoDlg.SetUserFaceID(0);
		TCHAR szAccount[32] = {0};
        EncodeUtil::Utf8ToUnicode(pResult->m_szAccountName, szAccount, ARRAYSIZE(szAccount));
		if(bGroup)
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("Ⱥ�˻���%s"), szAccount);
		else
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("�˻���%s"), szAccount);
		userSnapInfoDlg.SetAccountName(szInfo);

		TCHAR szNickName[32] = {0};
		memset(szInfo, 0, sizeof(szInfo));
        EncodeUtil::Utf8ToUnicode(pResult->m_szNickName, szNickName, ARRAYSIZE(szNickName));
		if(bGroup)
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("Ⱥ���ƣ�%s"), szNickName);
		else
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("�ǳƣ�%s"), szNickName);
		userSnapInfoDlg.SetNickName(szInfo);

		if(bGroup)
			userSnapInfoDlg.SetOKButtonText(_T("�����Ⱥ"));
		else
			userSnapInfoDlg.SetOKButtonText(_T("��Ϊ����"));
		
		if(bGroup && m_pFMGClient->m_UserMgr.IsGroupMember(m_pFMGClient->m_UserMgr.m_UserInfo.m_uUserID, pResult->m_uAccountID))
		{
			userSnapInfoDlg.SetOperationDescription(_T("���Ѿ��Ǹ�Ⱥ��Ա�������ظ���ӡ�"));
			userSnapInfoDlg.EnableOKButton(FALSE);
		}

		if(userSnapInfoDlg.DoModal(m_hWnd, NULL) == IDOK)
		{
			m_staticAddInfo.SetWindowText(_T("���������Ѿ����ͣ���ȴ��Է���Ӧ��"));
			m_pFMGClient->AddFriend(pResult->m_uAccountID);
		}
			
	}
	
	delete pResult;

	m_btnAdd.EnableWindow(TRUE);
	m_edtAddId.EnableWindow(TRUE);
		
	return (LRESULT)1;

}

HBRUSH CFindFriendDlg::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
{
	if (wndStatic == m_staticAddInfo) 
	{
		::SetTextColor(dc, RGB(255, 0, 0));
		::SetBkMode(dc, TRANSPARENT); 
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	SetMsgHandled(FALSE);

	return 0;
}

void CFindFriendDlg::OnClose()
{
	m_staticAddInfo.SetWindowText(_T(""));
	ShowWindow(SW_HIDE);
}

void CFindFriendDlg::OnDestroy()
{
	UninitUI();
}

void CFindFriendDlg::UninitUI()
{
	if(m_edtAddId.IsWindow())
		m_edtAddId.DestroyWindow();

	if(m_btnAdd.IsWindow())
		m_btnAdd.DestroyWindow();

	if(m_btnFindTypeSingle.IsWindow())
		m_btnFindTypeSingle.DestroyWindow();

	if(m_btnFindTypeGroup.IsWindow())
		m_btnFindTypeGroup.DestroyWindow();

	if(m_staticAddInfo.IsWindow())
		m_staticAddInfo.DestroyWindow();	
}

void CFindFriendDlg::AddMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
}

void CFindFriendDlg::RemoveMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}

void CFindFriendDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(bShow)
		AddMessageFilter();
	else
		RemoveMessageFilter();
}