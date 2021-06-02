#pragma once

#include <vector>

struct CASCADE_WIN_INFO			// ���������Ϣ
{
	HWND hWnd;					// ���ھ��
	POINT pt;					// �������Ͻ�����
};

class CCascadeWinManager		// ������ڹ�����
{
public:
	CCascadeWinManager(void);
	~CCascadeWinManager(void);

public:
	BOOL Add(HWND hWnd, int cx, int cy);				// ��Ӵ��ڣ��������ʾ
	void Del(HWND hWnd);								// ɾ������
	BOOL SetPos(HWND hWnd, int x, int y);				// ���ô���λ��
	void Clear();										// ������д���

private:
	std::vector<CASCADE_WIN_INFO*> m_arrWinInfo;		// ������Ϣ����
};
