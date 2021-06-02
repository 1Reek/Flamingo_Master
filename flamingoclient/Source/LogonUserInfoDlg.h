#pragma once

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UserSessionData.h"

class CFlamingoClient;

class CLogonUserInfoDlg : public CDialogImpl<CLogonUserInfoDlg>, public CMessageFilter
{
public:
	CLogonUserInfoDlg(void);
	~CLogonUserInfoDlg(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_LOGONUSERINFO };

	BEGIN_MSG_MAP_EX(CLogonUserInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(IDC_SYSHEAD, BN_CLICKED, OnSysHead)		//�����û�ͷ��
		COMMAND_HANDLER_EX(IDC_CUSTOMHEAD, BN_CLICKED, OnCustomHead)	//�����û�ͷ��
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnBtn_OK)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnBtn_Cancel)
		MESSAGE_HANDLER_EX(FMG_MSG_UPLOAD_USER_THUMB, OnUploadUserThumbResult)
	END_MSG_MAP()

public:
	void UpdateCtrlData();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnClose();
	void OnDestroy();
	void OnSysHead(UINT uNotifyCode, int nID, CWindow wndCtl);//�����û�ͷ��
	void OnCustomHead(UINT uNotifyCode, int nID, CWindow wndCtl);//�����û�ͷ��
	void OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl);	
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnUploadUserThumbResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	

	BOOL ValidatePhone(const CString& strPhone);


	BOOL Init();		// ��ʼ��
	void UnInit();		// ����ʼ��

public:
	CFlamingoClient*		m_pFMGClient;
	UINT				m_uUserID;
	UINT				m_uSysFaceID;	//ϵͳͷ��ID
	TCHAR				m_szCustomFacePath[MAX_PATH];

private:
	CSkinDialog			m_SkinDlg;

	CSkinEdit			m_edtNickName;
	CSkinButton			m_btnSysHead;
	CSkinButton			m_btnCustomHead;
	CSkinEdit			m_edtSign;
	CSkinPictureBox		m_picHead;
	CButton				m_btnGenderMale;
	CButton				m_btnGenderFemale;
	CDateTimePickerCtrl	m_dtpBirthday;
	CSkinEdit			m_edtAddress;
	CSkinEdit			m_edtPhone;
	CSkinEdit			m_edtMail;

	CSkinButton			m_btnOK;
	CSkinButton			m_btnCancel;

	HICON				m_hDlgIcon;
	HICON				m_hDlgSmallIcon;

	BOOL				m_bUseCustomThumb;	//�Ƿ�ʹ���Զ���ͷ��
	TCHAR				m_szCustomFaceRemotePath[MAX_PATH];

};
