#pragma once

#include "ThreadPool.h"

class CIUProtocol;
class CFlamingoClient;

//����������
class CCheckNetworkStatusTask : public CThreadPoolTask
{
public:
	CCheckNetworkStatusTask(void);
	~CCheckNetworkStatusTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

public:
	CIUProtocol*	m_pProtocol;
	CFlamingoClient*	m_pTalkClient;
	BOOL			m_bStop;
};