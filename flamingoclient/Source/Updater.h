#pragma once
#include <vector>
#include "Path.h"

class CFileTaskThread;
//�Զ�������
class Updater
{
public:
    Updater(CFileTaskThread* pFileTask);
	~Updater();

	BOOL IsNeedUpdate();

public:
    CFileTaskThread*	    m_pFileTask;
	std::vector<CString>	m_aryUpdateFileList;
};
