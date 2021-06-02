#include "stdafx.h"
#include "UserSessionData.h"
#include <Dbghelp.h>

#pragma comment(lib, "DbgHelp")

// Dumps
/////////////////////////////////////////////////////////////////////////////////////////////////
class AutoUnhandleExceptionFilter
{
public:
	AutoUnhandleExceptionFilter::AutoUnhandleExceptionFilter(void)
	{
		::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	}
	static LONG WINAPI AutoUnhandleExceptionFilter::MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
	{
		if(IDYES == ::MessageBox(NULL, _T("��������һ�����������⣬��Ҫ�رգ����ǶԴ˱�ʾ�ǳ��ı�Ǹ�����ǽ��������������ھ�����������"), _T("�������"), MB_YESNO|MB_ICONERROR|MB_SYSTEMMODAL))
		{
			::ShellExecute(NULL,NULL, _T("Flamingo.exe"), NULL, g_szHomePath, SW_SHOW);
		}
		
		SYSTEMTIME st={0};
		::GetLocalTime(&st);

		TCHAR szDumpFilePath[MAX_PATH]={0};
		wsprintf(szDumpFilePath, 
				_T("%sUsers/%d%d%d%02d%02d%02d.dmp"), g_szHomePath,
				st.wYear, st.wMonth, st.wDay, 
				st.wHour,st.wMinute,st.wSecond);

		HANDLE lhDumpFile = ::CreateFile(szDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL ,NULL);
		if(INVALID_HANDLE_VALUE != lhDumpFile)
		{
			MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
			loExceptionInfo.ExceptionPointers = ExceptionInfo;
			loExceptionInfo.ThreadId = GetCurrentThreadId();
			loExceptionInfo.ClientPointers = TRUE;
			::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);
			::CloseHandle(lhDumpFile);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
};

//�����������ȫ�ֱ���
AutoUnhandleExceptionFilter autoUnhandleExceptionFilter;