#pragma once

#include "resource.h"
#include "SkinLib/SkinLib.h"

class CFlamingoClient;

class CSystemSettingDlg : public CDialogImpl<CSystemSettingDlg>
{
public:
	CSystemSettingDlg(void);
	virtual ~CSystemSettingDlg(void);

	enum { IDD = IDD_SYSTEMSETTINGS };

	BEGIN_MSG_MAP_EX(CSystemSettingDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		
		COMMAND_ID_HANDLER_EX(IDC_AUTOREPLY, OnCheckAutoReply)
		COMMAND_ID_HANDLER_EX(IDC_DESTROYAFTERREAD, OnCheckDestroyAfterRead)
		COMMAND_ID_HANDLER_EX(IDOK, OnBtnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnBtnCancel)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	
	void OnCheckAutoReply(UINT uNotifyCode, int nID, CWindow wndCtl);			
	void OnCheckDestroyAfterRead(UINT uNotifyCode, int nID, CWindow wndCtl);
	
	void OnBtnOK(UINT uNotifyCode, int nID, CWindow wndCtl);					
	void OnBtnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);						
	
	BOOL InitUI();
	void UninitUI();
public:
	CFlamingoClient*		m_pFMGClient;

private:
	CSkinDialog			m_SkinDlg;
		
	CSkinButton			m_btnMute;					//�ر���������
	CSkinButton			m_btnAutoReply;				//�Զ��ظ�
	CSkinEdit			m_edtAutoReply;				//�Զ��ظ�����
	CSkinButton			m_btnDestroyAfterRead;		//�ĺ󼴷�
	CSkinComboBox		m_cboDurationRead;			//�ĺ󼴷ٳ���ʱ��
	CSkinButton			m_btnRevokeChatMsg;			//��Ϣ���ع���
	CSkinButton			m_btnExitPrompt;			//�ر����Ի���ʱ������ʾ�Ի���
	CSkinButton			m_btnExitWhenClose;			//�ر����Ի���ʱֱ���˳�
	CSkinButton			m_btnShowLastMsg;			//���촰����ʾ��������¼

	CSkinButton			m_btnOK;					//ȷ��
	CSkinButton			m_btnCancel;				//ȡ��
	
	HFONT				m_hFont;

	HICON m_hDlgIcon,	m_hDlgSmallIcon;
};
