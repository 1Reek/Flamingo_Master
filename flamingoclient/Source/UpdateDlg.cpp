#include "stdafx.h"
#include "UpdateDlg.h"
#include "UserSessionData.h"
#include "FlamingoClient.h"
#include "UserMgr.h"
#include "CustomMsgDef.h"
#include "EncodeUtil.h"
#include "File2.h"
#include "UIText.h"
#include <TlHelp32.h>

CUpdateDlg::CUpdateDlg()
{
	m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDownloadThread = NULL;
	m_pFMGClient = NULL;
}

CUpdateDlg::~CUpdateDlg()
{
	::CloseHandle(m_hExitEvent);
}

BOOL CUpdateDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	ParseFileInfo();

	InitUI();

	CenterWindow();

	m_hDownloadThread = ::CreateThread(NULL, 0, DownloadThread, this, 0, NULL);

	return TRUE;
}

BOOL CUpdateDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\GeneralBg.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("Flamingo��������"));
	m_SkinDlg.MoveWindow(0, 0, 550, 350, TRUE);

	
	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_UpdateFileName.SubclassWindow(GetDlgItem(IDC_UPFILENAME));
	m_UpdateFileName.SetTransparent(TRUE, hDlgBgDC);

	m_UpdateRate.SubclassWindow(GetDlgItem(IDC_UPRATE));
	m_UpdateRate.SetWindowText(_T("00%"));
	m_UpdateRate.SetTransparent(TRUE, hDlgBgDC);

	m_UpdateProgressBar.m_hWnd = GetDlgItem(IDC_UPPROGRESS);
	m_UpdateProgressBar.SetPos(0);
	//m_UpdateProgressBar.SetStep(10);
	//m_UpdateProgressBar.SetRange(0, 100);
	m_UpdateProgressBar.SetBarColor(RGB(231, 179, 37));
	
	m_UpdateListCtrl.SubclassWindow(GetDlgItem(IDC_UPFILELIST));
	m_UpdateListCtrl.ModifyStyleEx(0, LVS_EX_GRIDLINES);
	m_UpdateListCtrl.SetTransparent(TRUE, hDlgBgDC);
	
	RECT rtListCtrl = {0};
	::GetClientRect(m_UpdateListCtrl.m_hWnd, &rtListCtrl);
	m_UpdateListCtrl.AddColumn(_T("�ļ�����"), NULL, DT_CENTER, 100);
	m_UpdateListCtrl.AddColumn(_T("��������"), NULL, DT_RIGHT, 350);
	//m_UpdateListCtrl.SetColumnFormat(0, LVS_EX_GRIDLINES);
	//m_UpdateListCtrl.SetColumnFormat(1, LVS_EX_GRIDLINES);

	size_t nSize = m_aryFileDesc.size();
	for(size_t i=0; i<nSize; ++i)
	{
		m_UpdateListCtrl.AddItem( _T(""), NULL, FALSE, DT_CENTER, 0);
		m_UpdateListCtrl.SetItemText(i, 0, m_aryFileDesc[i]);
		m_UpdateListCtrl.SetItemText(i, 1, _T("�ȴ�����"));
		m_UpdateListCtrl.SetItemFormat(i, 1, DT_RIGHT);
	}

	m_UpdateListCtrl.SetSelItemTextColor(RGB(255, 0, 0));
	m_UpdateListCtrl.SetCurSelIndex(0);
	

	return TRUE;
}

void CUpdateDlg::ParseFileInfo()
{
	size_t nSize = m_aryFileInfo.size();
	if(nSize < 2)
	{
		::MessageBox(m_hWnd, _T("���������ļ���������������ֹ��"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
		EndDialog(IDCANCEL);
		return;
	}

	CString strLine;
	CString strTrimLeft;
	CString strTrimRight;
	std::vector<CString> aryTemp;
	for(size_t i=1; i<m_aryFileInfo.size(); ++i)
	{
		strTrimLeft.Format(_T("<%d>"), i);
		strTrimRight.Format(_T("</%d>"), i);
		strLine = m_aryFileInfo[i];
		strLine.Trim(_T("\r\n"));
		strLine.Trim(_T(" "));
		strLine.TrimLeft(strTrimLeft);
		strLine.TrimRight(strTrimRight);
		SplitString(strLine, _T("|"), aryTemp);
		if(aryTemp.size() >= 3)
		{
			m_aryFileDesc.push_back(aryTemp[0]);
			m_aryFileName.push_back(aryTemp[1]);
			m_aryFileVersion.push_back(aryTemp[2]);
		}
	}
}

void CUpdateDlg::OnClose()
{
	EndDialog(IDCANCEL);

	::SetEvent(m_hExitEvent);
}

void CUpdateDlg::OnDestroy()
{
	UninitUI();
}

void CUpdateDlg::UninitUI()
{

}

DWORD WINAPI CUpdateDlg::DownloadThread(LPVOID lpParameter)
{
	CUpdateDlg* pDlg = (CUpdateDlg*)lpParameter;
	
	CString strFilePath;
	CString strFileName;
	tstring	strErrorInfo;

	long nSize = pDlg->m_aryFileName.size();
	if(nSize <= 0)
	{
		::MessageBox(pDlg->m_hWnd, _T("�����汾���ļ���������ʧ�ܣ����´����ԣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONEXCLAMATION);
		pDlg->OnClose();
		return 0;
	}

	pDlg->m_UpdateProgressBar.SetStep(nSize);
	pDlg->m_UpdateProgressBar.SetRange(0, nSize* 10);
	long nRate = 100 / nSize;
	pDlg->m_UpdateProgressBar.SetPos(100 % nSize);
	long nShowRate = 100 % nSize;
	TCHAR szPercent[8] = {0};
	_stprintf_s(szPercent, _T("%d%%"), nShowRate);
	pDlg->m_UpdateRate.SetWindowText(szPercent);

	//���е��ļ������سɹ�ΪTRUE,��֮ΪFALSE.
	BOOL bAllSuccess = TRUE;
	for(long i=0; i<nSize; ++i)
	{
		strFileName.Format(_T("%s.zip"), pDlg->m_aryFileName[i]);
	
		strFilePath.Format(_T("%sUpdate\\%s"), g_szHomePath, strFileName);
		
		pDlg->m_UpdateListCtrl.SetCurSelIndex(i);
		
		//�ȴ�1��
		if(WaitForSingleObject(pDlg->m_hExitEvent, 1000) == WAIT_OBJECT_0)
			return 1;
		
        char szUtf8FileName[MAX_PATH] = { 0 };
        EncodeUtil::UnicodeToUtf8(strFileName.GetString(), szUtf8FileName, ARRAYSIZE(szUtf8FileName));
        if (!pDlg->m_pFMGClient->m_FileTask.DownloadFileSynchronously(szUtf8FileName, strFilePath, TRUE))
		{
			strErrorInfo = strFileName;
			strErrorInfo += _T("����ʧ��");
			pDlg->m_UpdateFileName.SetWindowText(strErrorInfo.c_str());
			pDlg->m_UpdateListCtrl.SetItemText(i, 1, _T("����ʧ��"));

			bAllSuccess = FALSE;
			::MessageBox(pDlg->m_hWnd, _T("����ʧ�ܣ����´����ԣ�"), g_strAppTitle.c_str(), MB_OK | MB_ICONEXCLAMATION);
			pDlg->OnClose();
			return 0;		
		}
		else
		{
			pDlg->m_UpdateListCtrl.SetItemText(i, 1, _T("�������"));
			nShowRate += nRate;
			_stprintf_s(szPercent, _T("%d%%"), nShowRate);
			pDlg->m_UpdateRate.SetWindowText(szPercent);
			pDlg->m_UpdateProgressBar.SetPos(pDlg->m_UpdateProgressBar.GetPos() + 10);
		}
		if (bAllSuccess)
		{
			pDlg->Invalidate();
		}
	}
	CString strModulePath(g_szHomePath);
	strModulePath += _T("Flamingo.exe");
	
	if(bAllSuccess)
	{
		//::MessageBox(pDlg->m_hWnd, _T("������ɣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONEXCLAMATION);
        if (Unzip())
        {
            ::SendMessage(pDlg->m_pFMGClient->m_UserMgr.m_hCallBackWnd, WM_CLOSE_MAINDLG, 0, 0);
            ::ExitProcess(0);
        }
        else
            ::MessageBox(pDlg->m_hWnd, _T("����ʧ�ܣ����´����ԣ�"), g_strAppTitle.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}		
	else
	{
		tstring strBackupVersionFile = g_szHomePath;
		strBackupVersionFile += _T("config\\version.bak");

		tstring strVersionFile = g_szHomePath;
		strVersionFile += _T("config\\version.ver");

		CFile file;
		if(!file.Open(strBackupVersionFile.c_str(), FALSE))
			return 0;
		const char* pBuffer = file.Read();
		if(pBuffer == NULL)
			return 0;
		CFile file2;
		if(!file2.Open(strVersionFile.c_str(), TRUE) || !file2.Write(pBuffer, file.GetSize()))
			return 0;

		::MessageBox(pDlg->m_hWnd, _T("����ʧ�ܣ����´����ԣ�"), g_strAppTitle.c_str(), MB_OK|MB_ICONEXCLAMATION);

	}
	
	return 1;
}

bool Unzip()
{
	TCHAR szModulePath[MAX_PATH] = {0};
	TCHAR szUpdatePath[MAX_PATH] = {0};
	TCHAR szCmdLine[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szModulePath, MAX_PATH);
	int nLength = _tcslen(szModulePath);
	for(long i=nLength; i>=0; --i)
	{
		if(szModulePath[i] == _T('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}
	_tcscpy_s(szUpdatePath, MAX_PATH, szModulePath);
	_tcscat_s(szUpdatePath, MAX_PATH, _T("\\iUpdateAuto.exe"));

	if(!::PathFileExists(szUpdatePath))
	{
		::MessageBox(NULL, 
					 _T("�Ҳ�����ѹ����iUpdateAuto.exe�����ֶ���ѹUpdateĿ¼��ѹ�����������Ŀ¼�¡�"), 
					 g_strAppTitle.c_str(),
					 MB_OK|MB_TOPMOST|MB_ICONERROR);

		return false;
	}

	_tcscpy_s(szCmdLine, MAX_PATH, szModulePath);
	_tcscat_s(szCmdLine, MAX_PATH, _T("\\Flamingo.exe"));


    //bool bRunAutoUpdateSuccess = true;
	//OSVERSIONINFO osvi={0};
	//osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO); 
	//GetVersionEx(&osvi);
	//Vista����ϵͳ�����ڹ���ԱȨ�����г���ķ�ʽ
    if (!IsWindowsXPOrGreater())
	{
		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};
		si.cb = sizeof(si);
        if (!::CreateProcess(szUpdatePath, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            TCHAR szErrorInfo[64] = { 0 };
            _stprintf_s(szErrorInfo, ARRAYSIZE(szErrorInfo), _T("����ʧ�ܣ��޷�����iUpdateAuto.exe��\r\n�����룺 %u"), ::GetLastError());
            ::MessageBox(NULL, szErrorInfo, g_strAppTitle.c_str(), MB_OK | MB_TOPMOST | MB_ICONERROR);

            //bRunAutoUpdateSuccess = false;
        }
	}
	else
	{
        ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		SHELLEXECUTEINFOW sei={0};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.lpVerb = _T("runas");
		sei.lpFile = szUpdatePath;
		sei.lpParameters = szCmdLine;
		sei.lpDirectory = NULL;
		sei.nShow = SW_HIDE;
        if (!::ShellExecuteEx(&sei))
        {
            TCHAR szErrorInfo[64] = { 0 };
            _stprintf_s(szErrorInfo, ARRAYSIZE(szErrorInfo), _T("����ʧ�ܣ��޷�����iUpdateAuto.exe��\r\n�����룺 %u"), ::GetLastError());
            ::MessageBox(NULL, szErrorInfo, g_strAppTitle.c_str(), MB_OK | MB_TOPMOST | MB_ICONERROR);

            //bRunAutoUpdateSuccess = false;
        }

        ::CoUninitialize();
	}

    
    //if (!bRunAutoUpdateSuccess)
    //    return false;

    ////������ѹ����ɹ��Ժ�Ѱ�ҽ�ѹ������̣����ҵȴ��ý����˳����ȴ�����

    //PROCESSENTRY32 pe32 = {0};
    //pe32.dwSize = sizeof(PROCESSENTRY32);

    //HANDLE hProcessSnap;
    //hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    //if (hProcessSnap == INVALID_HANDLE_VALUE)
    //    return false;

    //if (!Process32First(hProcessSnap, &pe32))
    //{           
    //    CloseHandle(hProcessSnap);
    //    return false;
    //}

    //DWORD dwAutoUpdateProcessID = 0;
    //do
    //{
    //    if (_tcsicmp(pe32.szExeFile, _T("iUpdateAuto")) == 0)
    //    {
    //        dwAutoUpdateProcessID = pe32.th32ProcessID;
    //        break;
    //    }           

    //} while (Process32Next(hProcessSnap, &pe32));

    //CloseHandle(hProcessSnap);

    ////��ѹ�����Ѿ��˵�������Ϊ�����ɹ�
    //if (dwAutoUpdateProcessID == 0)
    //    return true;

    //if (dwAutoUpdateProcessID != 0)
    //{
    //    HANDLE hAutoUpdateProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwAutoUpdateProcessID);
    //    if (hAutoUpdateProcess != NULL)
    //        return false;
    //    DWORD dwRet = ::WaitForSingleObject(hAutoUpdateProcess, 5000);
    //    if (dwRet == WAIT_OBJECT_0)
    //        return true;
    //}

    //return false;
    return true;
}

