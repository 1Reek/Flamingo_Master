#include "stdafx.h"
#include "CascadeWinManager.h"

CCascadeWinManager::CCascadeWinManager(void)
{
}

CCascadeWinManager::~CCascadeWinManager(void)
{
	Clear();
}

// ��Ӵ��ڣ��������ʾ
BOOL CCascadeWinManager::Add(HWND hWnd, int cx, int cy)
{
	::SetWindowPos(hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE);

	int nCount = (int)m_arrWinInfo.size();
	if (nCount <= 0)
	{
		CWindow window;
		window.Attach(hWnd);
		window.CenterWindow(::GetDesktopWindow());
		window.Detach();
	}
	else
	{
		int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
		int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);

		CASCADE_WIN_INFO* lpWinInfo = m_arrWinInfo[nCount-1];
		if (NULL == lpWinInfo)
			return FALSE;

		int x = lpWinInfo->pt.x;
		int y = lpWinInfo->pt.y;

		if (x > cxScreen-cx || y > cyScreen-cy)
		{
			x = 20;
			y = 28;
		}
		else
		{
			x += 20;
			y += 28;
		}

		::MoveWindow(hWnd, x, y, cx, cy, FALSE);
	}

	CASCADE_WIN_INFO* lpWinInfo = new CASCADE_WIN_INFO;
	if (NULL == lpWinInfo)
		return FALSE;

	RECT rcWindow = {0};
	::GetWindowRect(hWnd, &rcWindow);

	lpWinInfo->hWnd = hWnd;
	lpWinInfo->pt.x = rcWindow.left;
	lpWinInfo->pt.y = rcWindow.top;
	m_arrWinInfo.push_back(lpWinInfo);
	return TRUE;
}

// ɾ������
void CCascadeWinManager::Del(HWND hWnd)
{
	for (int i = 0; i < (int)m_arrWinInfo.size(); i++)
	{
		CASCADE_WIN_INFO* lpWinInfo = m_arrWinInfo[i];
		if (lpWinInfo != NULL && hWnd == lpWinInfo->hWnd)
		{
			m_arrWinInfo.erase(m_arrWinInfo.begin()+i);
			break;
		}
	}
}

// ���ô���λ��
BOOL CCascadeWinManager::SetPos(HWND hWnd, int x, int y)
{
	if (::IsIconic(hWnd))	// �жϴ����Ƿ���С�������򲻸��´���λ��
		return FALSE;

	for (int i = 0; i < (int)m_arrWinInfo.size(); i++)
	{
		CASCADE_WIN_INFO* lpWinInfo = m_arrWinInfo[i];
		if (lpWinInfo != NULL && hWnd == lpWinInfo->hWnd)
		{
			lpWinInfo->pt.x = x;
			lpWinInfo->pt.y = y;
			return TRUE;
		}
	}
	return FALSE;
}

// ������д���
void CCascadeWinManager::Clear()
{
	for (int i = 0; i < (int)m_arrWinInfo.size(); i++)
	{
		CASCADE_WIN_INFO* lpWinInfo = m_arrWinInfo[i];
		if (lpWinInfo != NULL)
			delete lpWinInfo;
	}
	m_arrWinInfo.clear();
}
