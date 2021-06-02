#pragma once

#include <vector>

struct LOGIN_ACCOUNT_INFO	// ��¼�˺���Ϣ
{
	TCHAR szUser[32];		// �û���
	TCHAR szPwd[32];		// ����
	int nStatus;			// ��¼״̬
	BOOL bRememberPwd;		// ��ס����
	BOOL bAutoLogin;		// �Զ���¼
	//BOOL bAlreadyLogin;		// �Ƿ��Ѿ���¼
};

class CLoginAccountList		// ��¼�˺��б�
{
public:
	CLoginAccountList(void);
	~CLoginAccountList(void);

	BOOL LoadFile(LPCTSTR lpszFileName);					// ���ص�¼�ʺ��б��ļ�
	BOOL SaveFile(LPCTSTR lpszFileName);					// �����¼�ʺ��б��ļ�

	BOOL Add(LPCTSTR lpszUser, LPCTSTR lpszPwd,				// ����ʺ�
		int nStatus, BOOL bRememberPwd, BOOL bAutoLogin);
	BOOL Del(int nIndex);									// ɾ���ʺ�
	BOOL Modify(int nIndex, LPCTSTR lpszUser, LPCTSTR lpszPwd, // �޸��ʺ�
		int nStatus, BOOL bRememberPwd, BOOL bAutoLogin);
	void Clear();											// ��������ʺ�

	int GetCount();											// ��ȡ�ʺ�����
	LOGIN_ACCOUNT_INFO* GetAccountInfo(int nIndex);		// ��ȡ�ʺ���Ϣ

	LOGIN_ACCOUNT_INFO* Find(LPCTSTR lpszUser);			// �����ʺ�

	int GetLastLoginUser();
	void SetLastLoginUser(LPCTSTR lpszUser);

	LOGIN_ACCOUNT_INFO* GetLastLoginAccountInfo();
	BOOL GetLastLoginAccountInfo(LOGIN_ACCOUNT_INFO* lpAccount);

	BOOL IsAutoLogin();

private:
	void Encrypt(char* lpBuf, int nLen);					// ����
	void Decrypt(char* lpBuf, int nLen);					// ����
	int _fread(void* lpBuf, int nSize, int nCount, FILE* fp);
	int _fwrite(const void* lpBuf, int nSize, int nCount, FILE* fp);

private:
	int m_nLastLoginUser;									// ����¼�û�����
	std::vector<LOGIN_ACCOUNT_INFO*> m_arrLoginAccount;	// ��¼�˺��б�
};
