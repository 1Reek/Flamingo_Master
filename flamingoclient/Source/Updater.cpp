#include "stdafx.h"
#include "Updater.h"
#include "UserSessionData.h"
#include "File2.h"
#include "EncodeUtil.h"
#include "Utils.h"
#include "FileTaskThread.h"

Updater::Updater(CFileTaskThread* pFileTask)
{
    m_pFileTask = pFileTask;
}

Updater::~Updater()
{
	m_aryUpdateFileList.clear();
}

BOOL Updater::IsNeedUpdate()
{
	//assert(m_lpProcotol != NULL);

	TCHAR szLocalVersionFile[MAX_PATH] = {0};
	_tcscpy_s(szLocalVersionFile, MAX_PATH, g_szHomePath);
	_tcscat_s(szLocalVersionFile, MAX_PATH, _T("update"));
	//�������Ŀ¼��Update�ļ��в��������ȴ���֮
	//���߲����ڣ�Update�ļ��д������ɹ�������İ汾�ļ�Ҳ�����ز��ɹ�
	if(!::PathFileExists(szLocalVersionFile) && !::CreateDirectory(szLocalVersionFile, NULL))
		return FALSE;
	
	_tcscat_s(szLocalVersionFile, MAX_PATH, _T("\\update2.version"));
	//��Ȿ�ذ汾�ļ��Ƿ���ڣ������ڣ���ӷ���������
	if(!::PathFileExists(szLocalVersionFile))
	{	
		//����ʧ��Ҳ������
        if (!m_pFileTask->DownloadFileSynchronously("update2.version", szLocalVersionFile, FALSE))
			return FALSE;
	}
	
	//������ذ汾�ļ�����
	CFile file;
	if(!file.Open(szLocalVersionFile, FALSE))
		return FALSE;
	
	const char* pBuffer = file.Read();
	CString strLocal;
	if(pBuffer != NULL)
	{
        EncodeUtil::Utf8ToUnicode(pBuffer, strLocal.GetBuffer(file.GetSize() * 2), file.GetSize() * 2);
		strLocal.ReleaseBuffer();
	}
	if(strLocal.IsEmpty())
		return FALSE;

	file.Close();

	//�ӷ���������У׼�汾�ļ�
	TCHAR szCorrectionVersionFile[MAX_PATH] = {0};
	_tcscpy_s(szCorrectionVersionFile, MAX_PATH, g_szHomePath);
	_tcscat_s(szCorrectionVersionFile, MAX_PATH, _T("update\\update.version"));
	if(!m_pFileTask->DownloadFileSynchronously("update.version", szCorrectionVersionFile, TRUE))
		return FALSE;

	//��ȡ�������汾�ļ���Ϣ
	if(!file.Open(szCorrectionVersionFile, FALSE))
		return FALSE;

	CString strCorrection;
	pBuffer = file.Read();
	if(pBuffer != NULL)
	{
        EncodeUtil::Utf8ToUnicode(pBuffer, strCorrection.GetBuffer(file.GetSize() * 2), file.GetSize() * 2);
		strCorrection.ReleaseBuffer();
	}
	if(strCorrection.IsEmpty())
		return FALSE;

	file.Close();

	
	//�������ϰ汾���ڱ��ذ汾��Ҫ����
	if(strCorrection <= strLocal)
		return FALSE;

	SplitString(strCorrection, _T("\r\n"), m_aryUpdateFileList);	
	
	return TRUE;
}
