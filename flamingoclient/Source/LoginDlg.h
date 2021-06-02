#pragma once

#include "SkinLib/SkinLib.h"
#include "RegisterDialog.h"
#include "LoginSettingsDlg.h"
#include "LoginAccountList.h"

class CFlamingoClient;
struct LOGIN_ACCOUNT_INFO;

class CLoginDlg : public CDialogImpl<CLoginDlg>
{
public:
    CLoginDlg(CFlamingoClient*	pFMGClient);
	virtual ~CLoginDlg(void);

	enum { IDD = IDD_LOGINDLG };

    BEGIN_MSG_MAP_EX(CLoginDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
        MSG_WM_MEASUREITEM(OnMeasureItem)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_DESTROY(OnDestroy)
        COMMAND_HANDLER_EX(ID_COMBO_UID, CBN_EDITCHANGE, OnCbo_EditChange_UID)
        COMMAND_HANDLER_EX(ID_COMBO_UID, CBN_SELCHANGE, OnCbo_SelChange_UID)
        COMMAND_ID_HANDLER_EX(ID_CHECK_REMEMBER_PWD, OnBtn_RememberPwd)
        COMMAND_ID_HANDLER_EX(ID_CHECK_AUTO_LOGIN, OnBtn_AutoLogin)
        COMMAND_ID_HANDLER_EX(ID_BTN_LOGIN, OnBtn_Login)
        COMMAND_ID_HANDLER_EX(ID_BTN_SET, OnBtn_Set)
        COMMAND_ID_HANDLER_EX(ID_STATIC_REG_ACCOUNT, OnRegister)
        COMMAND_RANGE_HANDLER_EX(ID_MENU_IMONLINE, ID_MENU_INVISIBLE, OnMenu_LoginStatus)
		REFLECT_NOTIFICATIONS()														// ��Ϣ����֪ͨ��
	END_MSG_MAP()

public:
	BOOL GetLoginAccountInfo(LOGIN_ACCOUNT_INFO* lpAccount);
    BOOL SetLoginAccountInfo(const LOGIN_ACCOUNT_INFO* lpAccount);

	void SetDefaultAccount(PCTSTR pszDefaultAccount);
	void SetDefaultPassword(PCTSTR pszDefaultPassword);

    void DoLogin();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnClose();
	void OnDestroy();
	void OnCbo_EditChange_UID(UINT uNotifyCode, int nID, CWindow wndCtl);			// ��UTalk�ʺš���Ͽ�
	void OnCbo_SelChange_UID(UINT uNotifyCode, int nID, CWindow wndCtl);			// ��UTalk�ʺš���Ͽ�
	void OnBtn_RememberPwd(UINT uNotifyCode, int nID, CWindow wndCtl);				// ����ס���롱��ѡ��
	void OnBtn_AutoLogin(UINT uNotifyCode, int nID, CWindow wndCtl);				// ���Զ���¼����ѡ��
	void OnBtn_Login(UINT uNotifyCode, int nID, CWindow wndCtl);					// ����¼����ť
	void OnBtn_Set(UINT uNotifyCode, int nID, CWindow wndCtl);						// �����á���ť
	void OnMenu_LoginStatus(UINT uNotifyCode, int nID, CWindow wndCtl);				// ����¼״̬���˵���ť

	void DetectNewVersion();														//����°汾
	void OnRegister(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

	long GetStatusFromMenuID(int nMenuID);									// �Ӳ˵�ID��ȡ��Ӧ��UTalk_STATUS
	void StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, long nStatus);	// ����ָ��״̬����״̬�˵���ť��ͼ��
	void SetCurUser(LPCTSTR lpszUser, BOOL bPwdInvalid = FALSE);

    static UINT WINAPI LoginThreadProc(void* pParam);

public:
	CFlamingoClient*	m_lpFMGClient;
	CLoginAccountList*  m_pLoginAccountList;

private:
	CSkinDialog			m_SkinDlg;
	CSkinComboBox		m_cboUid;
	CSkinEdit			m_edtPwd;
	CSkinButton			m_btnRememberPwd;
	CSkinButton			m_btnAutoLogin;
	CSkinHyperLink		m_lnkRegAccount;											//ע�����Ӱ�ť
	CSkinHyperLink		m_lnkLostPwd;

	CSkinButton			m_btnSet;
	CSkinButton			m_btnLogin;
	CSkinButton			m_btnLoginStatus;
	CSkinPictureBox		m_picHead;
	CSkinMenu			m_SkinMenu;

	HICON m_hDlgIcon,	m_hDlgSmallIcon;
	LOGIN_ACCOUNT_INFO	m_stAccountInfo;
	BOOL				m_bFirstModify;

	CLoginSettingsDlg	m_LoginSettingDlg;											// ���öԻ���
	CRegisterDialog		m_RegDlg;													// ע��Ի���
	CString				m_strDefaultAccount;
	CString				m_strDefaultPassword;
};
