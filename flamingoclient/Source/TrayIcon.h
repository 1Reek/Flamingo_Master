#pragma once

class CTrayIcon		// ����ͼ���װ��
{
public:
	CTrayIcon(void);
	~CTrayIcon(void);

public:
	BOOL AddIcon(HWND hCallBackWnd, UINT uCallBackMsg, 
		        UINT uID, HICON hIcon, LPCTSTR lpszTip = NULL, 
                BOOL bPopupBalloon = FALSE, LPCTSTR lpszBalloonInfoTitle = NULL, LPCTSTR lpszBalloonInfo = NULL, UINT uTimeout = 3000);
	BOOL ModifyIcon(HICON hIcon, LPCTSTR lpszTip = NULL, UINT uID = 1,
                    BOOL bPopupBalloon = FALSE, LPCTSTR lpszBalloonInfoTitle = NULL, LPCTSTR lpszBalloonInfo = NULL, UINT uTimeout = 3000);
	BOOL RemoveIcon();
	LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent);
	BOOL GetTrayIconRect(RECT* lpRect);			// ��ȡ����ͼ������λ��

private:
	BOOL EnumNotifyWindow(HWND hWnd, RECT &rect);	// ö�ٻ�ȡ����ͼ������λ��
	HWND FindTrayNotifyWnd();						// ��ȡ��ͨ���������ھ��
	HWND FindNotifyIconOverflowWindow();			// ��ȡ������������ھ��

private:
	NOTIFYICONDATA m_stNotifyIconData;
	BOOL m_bHover;
	DWORD m_dwTimerId;
};
