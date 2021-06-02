#pragma once
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include <vector>

//class CIUProtocol;
class CFlamingoClient;

//�Զ���ѹ�����������������ļ����ɹ�����true��ʧ�ܷ���false
bool Unzip();

class CUpdateDlg : public CDialogImpl<CUpdateDlg>
{
public:
	CUpdateDlg();
	virtual ~CUpdateDlg();

	enum {IDD = IDD_UPDATEDLG};

	BEGIN_MSG_MAP_EX(CUpdateDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	BOOL InitUI();
	void UninitUI();
	
	void ParseFileInfo();	//�����������ļ���Ϣ

public:
	CFlamingoClient*	     m_pFMGClient;
	std::vector<CString>	 m_aryFileInfo;

	std::vector<CString>	 m_aryFileDesc;		//��������������
	std::vector<CString>	 m_aryFileName;		//��������
	std::vector<CString>	 m_aryFileVersion;	//�������汾��
	

public:
	CSkinDialog			m_SkinDlg;
	CSkinStatic			m_UpdateFileName, m_UpdateRate;
	CProgressBarCtrl	m_UpdateProgressBar;
	CSkinListCtrl		m_UpdateListCtrl;

	HANDLE				m_hDownloadThread;
	HANDLE				m_hExitEvent;					//�Ի����˳��¼�

	static	DWORD WINAPI DownloadThread(LPVOID lpParameter);
};