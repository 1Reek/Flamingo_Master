#pragma once

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "FlamingoClient.h"
#include "Utils.h"
#include "FontSelDlg.h"

#define		WM_UPDATE_FONTINFO		WM_USER + 2

extern CFontInfo g_fontInfo;				// ������Ϣ
extern TCHAR g_cHotKey;						// ��ȡ��Ϣ�ȼ�

//BOOL LoadMiscConfig(LPCTSTR lpszFileName);	// ������������
//BOOL SaveMiscConfig(LPCTSTR lpszFileName);	// ������������


class CFontSelDlg : public CDialogImpl<CFontSelDlg>
{
public:
	CFontSelDlg(void);
	~CFontSelDlg(void);

	enum { IDD = IDD_FONTSELDLG };

	BEGIN_MSG_MAP_EX(CFontSelDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(101, OnBtn_SysFont)
		COMMAND_ID_HANDLER_EX(102, OnBtn_MemberFont)
		COMMAND_HANDLER_EX(ID_COMBO_FONTNAME, CBN_SELCHANGE, OnCbo_SelChange_FontName)
		COMMAND_HANDLER_EX(ID_COMBO_FONTSIZE, CBN_SELCHANGE, OnCbo_SelChange_FontSize)
		COMMAND_ID_HANDLER_EX(104, OnBtn_Bold)
		COMMAND_ID_HANDLER_EX(105, OnBtn_Italic)
		COMMAND_ID_HANDLER_EX(106, OnBtn_UnderLine)
		COMMAND_HANDLER_EX(107, BN_CLICKED, OnBtn_Color)
	END_MSG_MAP()

public:
	CFontInfo& GetPublicFontInfo();		// ��ȡȫ��������Ϣ
	CFontInfo& GetFontInfo();			// ��ȡ������Ϣ

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_SysFont(UINT uNotifyCode, int nID, CWindow wndCtl);		// ��ϵͳ���塱��ť
	void OnBtn_MemberFont(UINT uNotifyCode, int nID, CWindow wndCtl);	// ����Ա�Ų��֡���ť
	void OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���������ơ���Ͽ�
	void OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl);	// �������С����Ͽ�
	void OnBtn_Bold(UINT uNotifyCode, int nID, CWindow wndCtl);			// ���Ӵ֡���ť
	void OnBtn_Italic(UINT uNotifyCode, int nID, CWindow wndCtl);		// ����б����ť
	void OnBtn_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���»��ߡ���ť
	void OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl);		// ����ɫ����ť

	BOOL InitFontClassToolBar();	// ��ʼ��������๤����
	BOOL InitFontNameComboBox();	// ��ʼ������������Ͽ�
	BOOL InitFontSizeComboBox();	// ��ʼ�������С��Ͽ�
	BOOL InitFontAttrToolBar();		// ��ʼ���������Թ�����

	BOOL Init();					// ��ʼ��
	void UnInit();					// ����ʼ��

public:
	CFlamingoClient*		m_pFMGClient;

private:
	CSkinDialog			m_SkinDlg;
	CSkinToolBar		m_tbFontClass; 
	CSkinToolBar		m_tbFontAttr;
	CSkinComboBox		m_cboFontName;
	CSkinComboBox		m_cboFontSize;
	CFontInfo			m_fontInfo;
};
