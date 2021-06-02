#pragma once

#include <vector>

class CThreadPoolTask
{
public:
	CThreadPoolTask();
	virtual ~CThreadPoolTask();

public:
	virtual BOOL IsRunning();
	virtual void SetRunning(BOOL bRunning);
	virtual int Run();
	virtual int Stop();						//���������е���������Ľӿڣ�����ʵ�֣�
	virtual void TaskFinish();				//��������������Ѿ������������ṩһ��������Դ�Ļ���

private:
	BOOL m_bRunning;
};

struct TP_THREAD_INFO		// �߳���Ϣ�ṹ
{
	HANDLE hThread;
	LPARAM lParam;
};

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();

public:
	BOOL Init(int nThreadNums);
	void UnInit();
	BOOL AddTask(CThreadPoolTask* lpTask);
	BOOL InsertTask(int nIndex, CThreadPoolTask* lpTask);
	void RemoveTask(CThreadPoolTask* lpTask);
	void RemoveAllTask();

private:
	static UINT APIENTRY ThreadProc(LPVOID lpParam);	// �̺߳���

private:
	std::vector<TP_THREAD_INFO*> m_arrThreadInfo;		// �߳���Ϣ����
	std::vector<CThreadPoolTask*> m_arrTask;			// ������Ϣ����
	
	CRITICAL_SECTION m_csThreadInfo;					// �߳���Ϣ�ٽ���
	CRITICAL_SECTION m_csTask;							// ������Ϣ�ٽ���

	HANDLE m_hSemaphore_Task;							// ����֪ͨ�ź���
	HANDLE m_hEvent_Exit;								// �˳��߳�֪ͨ�¼����
	HANDLE m_hEvent_ExitAll;							// �����߳̾��˳����֪ͨ�¼����

	long m_lThreadNums;									// �߳�����
	long m_lRunningThreadNums;							// ���ڹ����е��̼߳���
};