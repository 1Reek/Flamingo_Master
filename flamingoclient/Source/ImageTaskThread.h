#pragma once
#include "Thread.h"
#include <list>
#include <mutex>
#include <condition_variable>

class CIUProtocol;
class CFileItemRequest;
class CFlamingoClient;
class CUploadFileResult;

/**
 *  �����շ�����ͼƬ��ͷ����࣬����imgServer
 */
class CImageTaskThread : public CThread
{
public:
	CImageTaskThread();
	virtual ~CImageTaskThread(void);

public:
	virtual void Stop() override;
	
	BOOL AddItem(CFileItemRequest* pItem);
	void RemoveItem(CFileItemRequest* pItem);									//�Ƴ����������ػ����ϴ�����
	void ClearAllItems();

	void HandleItem(CFileItemRequest* pFileItem);
protected:
	virtual void Run() override;

private:
    long UploadImage(PCTSTR pszFileName, HWND hwndReflection, HANDLE hCancelEvent, CUploadFileResult& uploadFileResult);
    long DownloadImage(LPCSTR lpszFileName, LPCTSTR lpszDestPath, BOOL bOverwriteIfExist, HWND hwndReflection, HANDLE hCancelEvent);
    BOOL UploadUserThumb(PCTSTR pszFileName, HWND hwndReflection, CUploadFileResult& uploadFileResult);
	void FillUploadFileResult(CUploadFileResult& uploadFileResult, PCTSTR pszLocalName, PCSTR pszRemoteName, int64_t nFileSize, char* pszMd5);
    
public:
	CIUProtocol*					m_pProtocol;
	CFlamingoClient*				m_lpFMGClient;

private:
	CFileItemRequest*				m_pCurrentTransferringItem;					//��ǰ���ڴ������
	std::list<CFileItemRequest*>	m_Filelist;	
	std::mutex						m_mtItems;
	std::condition_variable			m_cvItems;
    int32_t                         m_seq;
};