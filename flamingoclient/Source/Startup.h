#pragma once

HWND CreateOwnerWindow();

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT);

BOOL InitSocket();

void UnInitSocket();

/**
 *@param pszFileSuffixName Ҫ�������־�ļ�����չ������Ҫ��.��ͷ������log
 */
void ClearExpiredLog(PCTSTR pszFileSuffixName);