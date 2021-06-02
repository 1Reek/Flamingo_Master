#pragma once

#include "resource.h"
#include "CustomMsgDef.h"
#include "Utils.h"
#include "SkinLib/SkinLib.h"
#include "FlamingoClient.h"

class CGroupInfoDlg : public CDialogImpl<CGroupInfoDlg>, public CMessageFilter
{
public:
	CGroupInfoDlg(void);
	~CGroupInfoDlg(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_GROUPINFODLG };

	BEGIN_MSG_MAP_EX(CGroupInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_BTN_OK, BN_CLICKED, OnBtn_Ok)
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)
	END_MSG_MAP()

public:
	void OnUpdateGroupInfo();	// ����Ⱥ��Ϣ
	void OnUpdateGroupNumber();	// ����Ⱥ����

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_Ok(UINT uNotifyCode, int nID, CWindow wndCtl);		// ��ȷ������ť
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ȡ������ť

	CGroupInfo* GetGroupInfoPtr();
	void UpdateCtrls();

	BOOL Init();		// ��ʼ��
	void UnInit();		// ����ʼ��

public:
	CFlamingoClient* m_lpFMGClient;
	HWND m_hMainDlg;
	UINT m_nGroupCode;

private:
	CSkinDialog m_SkinDlg;

	CSkinStatic m_staName, m_staCreater, m_staClass;
	CSkinStatic m_staNumber, m_staCreateTime, m_staRemark;
	CSkinStatic m_staMemo, m_staFingerMemo;
	CSkinStatic m_staCardName, m_staPhone, m_staGender, m_staEmail;
	CSkinStatic m_staRemark2;

	CSkinEdit m_edtName, m_edtCreater, m_edtClass;
	CSkinEdit m_edtNumber, m_edtCreateTime, m_edtRemark;
	CSkinEdit m_edtMemo, m_edtFingerMemo;
	CSkinEdit m_edtCardName, m_edtPhone, m_edtGender, m_edtEmail;
	CSkinEdit m_edtRemark2;

	CSkinButton m_btnOk, m_btnCancel;

	HICON m_hDlgIcon, m_hDlgSmallIcon;
};
