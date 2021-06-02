#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "SQLite 3.7.17/sqlite3.h"	// ʹ��sqlite3��Ҫ������ͷ�ļ�

#pragma comment(lib, "sqlite3.lib")

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

struct BUDDY_MSG_LOG		// ������Ϣ��¼
{
	UINT		nID;		//TODO: ���ID��ʲô�ã�
	UINT		nUTalkNum;
	tstring		strNickName;
	UINT64		nTime;
	BOOL		bSendFlag;
	tstring		strContent;
};

struct GROUP_MSG_LOG		// Ⱥ��Ϣ��¼
{
	UINT nID;
	UINT nGroupNum;
	UINT nUTalkNum;
	tstring strNickName;
	UINT64 nTime;
	tstring strContent;
};

struct SESS_MSG_LOG			// ��ʱ�Ự(Ⱥ��Ա)��Ϣ��¼
{
	UINT nID;
	UINT nUTalkNum;
	tstring strNickName;
	UINT64 nTime;
	BOOL bSendFlag;
	tstring strContent;
};

void CloneStr(const wchar_t* src, wchar_t** dest, UINT* destlen);

class CMessageLogger
{
public:
	CMessageLogger(void);
	~CMessageLogger(void);

public:
	tstring GetMsgLogFileName();								// ��ȡ��Ϣ��¼�ļ�����
	void SetMsgLogFileName(LPCTSTR lpszFileName);				// ������Ϣ��¼�ļ�����

	BOOL IsMsgLogFileValid();									// �ж���Ϣ��¼�ļ��Ƿ���Ч
	BOOL CreateMsgLogFile();									// ������Ϣ��¼�ļ�

	BOOL WriteBuddyMsgLog(UINT nUTalkNum, LPCTSTR lpNickName,	// д��һ��������Ϣ��¼
		UINT64 nTime, BOOL bSendFlag, LPCTSTR lpContent);
	BOOL WriteGroupMsgLog(UINT nGroupNum, UINT nUTalkNum,		// д��һ��Ⱥ��Ϣ��¼
		LPCTSTR lpNickName, UINT64 nTime, LPCTSTR lpContent);
	BOOL WriteSessMsgLog(UINT nUTalkNum, LPCTSTR lpNickName,	// д��һ����ʱ�Ự(Ⱥ��Ա)��Ϣ��¼
		UINT64 nTime, BOOL bSendFlag, LPCTSTR lpContent);

	BOOL WriteBuddyMsgLog(BUDDY_MSG_LOG* lpMsgLog);			// д��һ��������Ϣ��¼
	BOOL WriteGroupMsgLog(GROUP_MSG_LOG* lpMsgLog);			// д��һ��Ⱥ��Ϣ��¼
	BOOL WriteSessMsgLog(SESS_MSG_LOG* lpMsgLog);				// д��һ����ʱ�Ự(Ⱥ��Ա)��Ϣ��¼

	UINT ReadBuddyMsgLog(UINT nUTalkNum, UINT nOffset,			// ����һ�������������Ϣ��¼
		UINT nRows, std::vector<BUDDY_MSG_LOG*>& arrMsgLog);
	UINT ReadGroupMsgLog(UINT nGroupNum, UINT nOffset,			// ����һ�������Ⱥ��Ϣ��¼
		UINT nRows, std::vector<GROUP_MSG_LOG*>& arrMsgLog);
	UINT ReadSessMsgLog(UINT nUTalkNum, UINT nOffset,			// ����һ���������ʱ�Ự(Ⱥ��Ա)��Ϣ��¼
		UINT nRows, std::vector<SESS_MSG_LOG*>& arrMsgLog);

	UINT GetBuddyMsgLogCount(UINT nUTalkNum);					// ��ȡ������Ϣ��¼��
	UINT GetGroupMsgLogCount(UINT nGroupNum);					// ��ȡȺ��Ϣ��¼��
	UINT GetSessMsgLogCount(UINT nUTalkNum);					// ��ȡ��ʱ�Ự(Ⱥ��Ա)��Ϣ��¼��

	BOOL DelAllBuddyMsgLog();									// ɾ�����к�����Ϣ��¼
	BOOL DelAllGroupMsgLog();									// ɾ������Ⱥ��Ϣ��¼
	BOOL DelAllSessMsgLog();									// ɾ��������ʱ�Ự(Ⱥ��Ա)��Ϣ��¼

	BOOL DelBuddyMsgLog(UINT nUTalkNum);						// ɾ��ָ�����ѵ�������Ϣ��¼
	BOOL DelGroupMsgLog(UINT nGroupNum);						// ɾ��ָ��Ⱥ��������Ϣ��¼
	BOOL DelSessMsgLog(UINT nUTalkNum);							// ɾ��ָ����ʱ�Ự(Ⱥ��Ա)��������Ϣ��¼

	UINT DelBuddyMsgLogByText(LPCTSTR lpStr);					// ��ȡ��ϢIDͨ����Ϣ�ı�

	BOOL DelBuddyMsgLogByID(UINT nID);							// ɾ��ָ��ID�ĺ�����Ϣ��¼
	BOOL DelGroupMsgLogByID(UINT nID);							// ɾ��ָ��ID��Ⱥ��Ϣ��¼
	BOOL DelSessMsgLogByID(UINT nID);							// ɾ��ָ��ID����ʱ�Ự(Ⱥ��Ա)��Ϣ��¼

	void Lock();												// ��������
	void UnLock();												// ��������

	// ����ӽӿڣ�
	// 1������ĳ��ʱ�䡢ָ�����ݶ�����Ϣ��¼
	// 2������ĳ��ʱ��ɾ����Ϣ��¼

private:
	UINT ReadBuddyMsgLogCount(UINT nUTalkNum, UINT nOffset, UINT nRows);
	UINT ReadGroupMsgLogCount(UINT nGroupNum, UINT nOffset, UINT nRows);
	UINT ReadSessMsgLogCount(UINT nUTalkNum, UINT nOffset, UINT nRows);

	void Replace(tstring& strText, const TCHAR* lpOldStr, const TCHAR* lpNewStr);

private:
	tstring m_strFileName;
	HANDLE m_hMutex;
};
