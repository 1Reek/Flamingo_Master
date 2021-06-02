#include "stdafx.h"
#include "ImageTaskThread.h"
#include "UserSessionData.h"
#include "IULog.h"
#include "MD5Sum.h"
#include "Utils.h"
#include "EncodeUtil.h"
#include "Path.h"
#include "FlamingoClient.h"
#include "File2.h"
#include "MiniBuffer.h"
#include "net/IUProtocolData.h"
#include "net/IUSocket.h"
#include "net/FileMsg.h"
#include "net/protocolstream.h"
#include "net/IUProtocolData.h"

CImageTaskThread::CImageTaskThread() :  m_seq(0)
{
	m_lpFMGClient = NULL;
	m_pProtocol = NULL;
	
	m_pCurrentTransferringItem = NULL;
}

CImageTaskThread::~CImageTaskThread()
{
	ClearAllItems();
}


void CImageTaskThread::Stop()
{
	m_bStop = true;
	m_cvItems.notify_one();
}

BOOL CImageTaskThread::AddItem(CFileItemRequest* pItem)
{
	if (NULL == pItem)
		return FALSE;

	std::lock_guard<std::mutex> guard(m_mtItems);
	m_Filelist.push_back(pItem);
	m_cvItems.notify_one();

	return TRUE;
}

void CImageTaskThread::RemoveItem(CFileItemRequest* pItem)
{
	if(pItem == NULL)
		return;

	std::lock_guard<std::mutex> guard(m_mtItems);
	//δ��ʼ���ػ��ϴ�����ֱ���Ƴ�
	if(pItem->m_bPending)
	{
		std::list<CFileItemRequest*>::iterator iter = m_Filelist.begin();
		for(; iter!=m_Filelist.end(); ++iter)
		{
			if(pItem ==*iter)
			{
				m_Filelist.erase(iter);
				delete pItem;
				break;
			}
		}
	}
	//�������ػ��ϴ�����ʹ���ź�����ֹͣ
	else
	{
		if(pItem->m_hCancelEvent != NULL)
			::SetEvent(pItem->m_hCancelEvent);
	}

	//AtlTrace(_T("File item count: %d.\n"), m_Filelist.size());
}

void CImageTaskThread::ClearAllItems()
{
	std::lock_guard<std::mutex> guard(m_mtItems);
	for (auto& iter : m_Filelist)
	{
		delete iter;
	}

	m_Filelist.clear();
}

void CImageTaskThread::Run()
{
	while (!m_bStop)
	{
		CFileItemRequest* pFileItem;
		{
			std::unique_lock<std::mutex> guard(m_mtItems);
			while (m_Filelist.empty())
			{
				if (m_bStop)
					return;

				m_cvItems.wait(guard);
			}

			pFileItem = m_Filelist.front();
			m_Filelist.pop_front();
		}

		HandleItem(pFileItem);
	}
}

void CImageTaskThread::HandleItem(CFileItemRequest* pFileItem)
{
	if(pFileItem==NULL || pFileItem->m_uType!=NET_DATA_FILE)
		return;

	//��ʼ���д���
	pFileItem->m_bPending = FALSE;
	m_pCurrentTransferringItem = pFileItem;
	
	BOOL bRet = FALSE;
	CUploadFileResult* pUploadFileResult = new CUploadFileResult();
	if(pFileItem->m_nFileType == FILE_ITEM_UPLOAD_USER_THUMB)
	{
		bRet = UploadUserThumb(pFileItem->m_szFilePath, pFileItem->m_hwndReflection,*pUploadFileResult);
		long nUploadUserThumbResult = (bRet!=FALSE ? UPLOAD_USER_THUMB_RESULT_SUCCESS : UPLOAD_USER_THUMB_RESULT_FAILED);
        ::PostMessage(pFileItem->m_hwndReflection, FMG_MSG_UPLOAD_USER_THUMB, (WPARAM)nUploadUserThumbResult, (LPARAM)pUploadFileResult);
	}
    else if (pFileItem->m_nFileType == FILE_ITEM_UPLOAD_CHAT_IMAGE)
    {
        //�ϴ��ļ����ʧ�ܣ�����������
        pUploadFileResult->m_uSenderID = pFileItem->m_uSenderID;
        pUploadFileResult->m_setTargetIDs = pFileItem->m_setTargetIDs;
        pUploadFileResult->m_nFileType = pFileItem->m_nFileType;
        pUploadFileResult->m_hwndReflection = pFileItem->m_hwndReflection;
        long nRetCode;
        while (pFileItem->m_nRetryTimes < 3)
        {
            //nRetCode = m_pProtocol->UploadFile3(pFileItem->m_szFilePath, pFileItem->m_hwndReflection, pFileItem->m_hCancelEvent,*pUploadFileResult);
            nRetCode = UploadImage(pFileItem->m_szFilePath, pFileItem->m_hwndReflection, pFileItem->m_hCancelEvent, *pUploadFileResult);
            if (nRetCode == FILE_UPLOAD_SUCCESS || nRetCode == FILE_UPLOAD_USERCANCEL)
                break;

            ::Sleep(3000);

            ++pFileItem->m_nRetryTimes;
        }

        //�����û�ȡ���������ϴ��ɹ���ʧ�ܶ�Ҫ���߶Է�
        if (nRetCode != FILE_UPLOAD_USERCANCEL)
        {
            //SendConfirmMessage(pUploadFileResult);

            //��¡һ���ϴ��������������PostMessage
            CUploadFileResult* pResult = new CUploadFileResult();
            pResult->Clone(pUploadFileResult);
            long nSendFileResultCode = (nRetCode == FILE_UPLOAD_SUCCESS ? SEND_FILE_SUCCESS : SEND_FILE_FAILED);
            //����Ի����Ѿ��رգ���ֱ�ӷ���������
            if (::IsWindow(pFileItem->m_hwndReflection))
                ::PostMessage(pFileItem->m_hwndReflection, FMG_MSG_SEND_FILE_RESULT, (WPARAM)nSendFileResultCode, (LPARAM)pResult);
            else
                ::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_SEND_FILE_RESULT, (WPARAM)nSendFileResultCode, (LPARAM)pResult);
        }
    }
	else if(pFileItem->m_nFileType==FILE_ITEM_DOWNLOAD_USER_THUMB)
	{
		CString strDumyPath;
		CString strThumb;
		//std::set<UINT>::const_iterator iter = pFileItem->m_setTargetIDs.begin();
		UINT uTargetID = pFileItem->m_uAccountID;
		strThumb.Format(_T("%s%u.png"), m_lpFMGClient->m_UserMgr.GetCustomUserThumbFolder().c_str(), uTargetID);
		long nRetCode = DownloadImage(pFileItem->m_szUtfFilePath, strThumb, TRUE, pFileItem->m_hwndReflection, pFileItem->m_hCancelEvent);

		if(nRetCode == FILE_DOWNLOAD_SUCCESS)
		{		
			TransformImage(strThumb, strThumb, 64, 64, strDumyPath);
			::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_CUSTOMFACE_AVAILABLE, (WPARAM)(uTargetID), 0);
		}
        else
            LOG_ERROR("download user thumb [%s] failed, userid = %d.", pFileItem->m_szUtfFilePath, pFileItem->m_uAccountID);
	}
    //��������ͼƬ
	else if(pFileItem->m_nFileType == FILE_ITEM_DOWNLOAD_CHAT_IMAGE)
	{
		long nRetCode;
		while(pFileItem->m_nRetryTimes < 3)
		{
			if(pFileItem->m_szUtfFilePath[0] == NULL)
				break;
			//nRetCode = m_pProtocol->DownloadFile3(pFileItem->m_szUtfFilePath, pFileItem->m_szFilePath, TRUE, pFileItem->m_hwndReflection, pFileItem->m_hCancelEvent);
            nRetCode = DownloadImage(pFileItem->m_szUtfFilePath, pFileItem->m_szFilePath, TRUE, pFileItem->m_hwndReflection, pFileItem->m_hCancelEvent);
            if(nRetCode==FILE_DOWNLOAD_SUCCESS || nRetCode==FILE_DOWNLOAD_USERCANCEL)
				break;

			++pFileItem->m_nRetryTimes;
			::Sleep(3000);
		}

        //TODO������ͼƬ�������ʧ���ˣ���֪ͨ������
		
		CBuddyMessage* lpMsg = pFileItem->m_pBuddyMsg;

		UINT nSenderID = lpMsg->m_nFromUin;
		UINT nTargetID = lpMsg->m_nToUin;
		
		if(IsGroupTarget(nTargetID))
		{
			//������ƽ̨ͬ������Ϣ��������ʾ�û�
			if(m_lpFMGClient->m_UserMgr.m_UserInfo.m_uUserID != nSenderID)
				::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_GROUP_MSG, 0, (LPARAM)lpMsg);
		}
		else
		{
			//����ƽ̨ͬ������Ϣ
			if(m_lpFMGClient->m_UserMgr.m_UserInfo.m_uUserID == nSenderID)
			{
				::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_BUDDY_MSG, 0, (LPARAM)lpMsg);
			}	
			else
			{
				//�������ѷ�������Ϣ
				if(nTargetID == m_lpFMGClient->m_UserMgr.m_UserInfo.m_uUserID)
					::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_BUDDY_MSG, 0, (LPARAM)lpMsg);
				else
				{
					lpMsg->m_nFromUin = nTargetID;
					::PostMessage(m_lpFMGClient->m_UserMgr.m_hProxyWnd, FMG_MSG_BUDDY_MSG, 0, (LPARAM)lpMsg);
				}
			}
		}
	}
	
    //TODO: �ǵ��ڶ�Ӧ��Ŀ�괰�ڳ�ɾ������������ڴ�й¶
	//delete pUploadFileResult;
	delete pFileItem;

	m_pCurrentTransferringItem = NULL;
}

//void CImageTaskThread::SendConfirmMessage(const CUploadFileResult* pUploadFileResult)
//{
//	if(pUploadFileResult == NULL)
//		return;
//	
//	//�ϴ�ͼƬ���
//	if(pUploadFileResult->m_nFileType == FILE_ITEM_UPLOAD_CHAT_IMAGE)
//	{
//		time_t nTime = time(NULL);
//		TCHAR szMd5[64] = {0};
//		EncodeUtil::AnsiToUnicode(pUploadFileResult->m_szMd5, szMd5, ARRAYSIZE(szMd5));
//		CString strImageName;
//		strImageName.Format(_T("%s.%s"), szMd5, Hootina::CPath::GetExtension(pUploadFileResult->m_szLocalName).c_str());
//		long nWidth = 0;
//		long nHeight = 0;
//		GetImageWidthAndHeight(pUploadFileResult->m_szLocalName, nWidth, nHeight);
//		char szUtf8FileName[MAX_PATH] = {0};
//		EncodeUtil::UnicodeToUtf8(strImageName, szUtf8FileName, ARRAYSIZE(szUtf8FileName));
//		CStringA strImageAcquireMsg;
//        //if (pUploadFileResult->m_bSuccessful)
//        //    strImageAcquireMsg.Format("{\"msgType\":2,\"time\":%llu,\"clientType\":1,\"content\":[{\"pic\":[\"%s\",\"%s\",%u,%d,%d]}]}", nTime, szUtf8FileName, pUploadFileResult->m_szRemoteName, pUploadFileResult->m_dwFileSize, nWidth, nHeight);
//        //else
//        //    strImageAcquireMsg.Format("{\"msgType\":2,\"time\":%llu,\"clientType\":1,\"content\":[{\"pic\":[\"%s\",\"\",%u,%d,%d]}]}", nTime, szUtf8FileName, pUploadFileResult->m_dwFileSize, nWidth, nHeight);
//
//		if(pUploadFileResult->m_bSuccessful)
//            strImageAcquireMsg.Format("{\"msgType\":2,\"time\":%llu,\"clientType\":1,\"content\":[{\"pic\":[\"%s\",\"%s\",%u,%d,%d]}]}", nTime, szUtf8FileName, pUploadFileResult->m_szRemoteName, pUploadFileResult->m_dwFileSize, nWidth, nHeight);
//		else
//            strImageAcquireMsg.Format("{\"msgType\":2,\"time\":%llu,\"clientType\":1,\"content\":[{\"pic\":[\"%s\",\"\",%u,%d,%d]}]}", nTime, szUtf8FileName, pUploadFileResult->m_dwFileSize, nWidth, nHeight);
//
//		long nBodyLength = strImageAcquireMsg.GetLength()+1;
//		char* pszMsgBody = new char[nBodyLength];
//		memset(pszMsgBody, 0, nBodyLength);
//		strcpy_s(pszMsgBody, nBodyLength, strImageAcquireMsg);
//		CSentChatConfirmImageMessage* pConfirm = new CSentChatConfirmImageMessage();
//		pConfirm->m_hwndChat = pUploadFileResult->m_hwndReflection;
//		pConfirm->m_pszConfirmBody = pszMsgBody;
//		pConfirm->m_uConfirmBodySize = nBodyLength-1;
//		pConfirm->m_uSenderID = pUploadFileResult->m_uSenderID;
//		pConfirm->m_setTargetIDs = pUploadFileResult->m_setTargetIDs;
//		if(pConfirm->m_setTargetIDs.size() > 1)
//			pConfirm->m_nType = CHAT_CONFIRM_TYPE_MULTI;
//		else
//			pConfirm->m_nType = CHAT_CONFIRM_TYPE_SINGLE;
//
//		m_lpFMGClient->m_SendMsgThread.AddItem(pConfirm);
//	}
//}

//TODO: �������������ļ����ⲿ������ԣ������ļ�md5Ҳ������һ�Σ����Ե�ֻ������ͨ�Ų���
//�޸ĵ�,�ĳɴ�һ���ļ�������ֻ��������ͨ�Ų��֡�
long CImageTaskThread::UploadImage(PCTSTR pszFileName, HWND hwndReflection, HANDLE hCancelEvent, CUploadFileResult& uploadFileResult)
{
    _tcscpy_s(uploadFileResult.m_szLocalName, ARRAYSIZE(uploadFileResult.m_szLocalName), pszFileName);

    //�ļ�md5ֵ
    char szMd5[64] = { 0 };
    //TODO: ��ʾ�û�����У���ļ�
    FileProgress* pFileProgress = NULL;
    pFileProgress = new FileProgress();
    memset(pFileProgress, 0, sizeof(FileProgress));
    pFileProgress->nPercent = -1;
    pFileProgress->nVerificationPercent = 0;
    _tcscpy_s(pFileProgress->szDestPath, ARRAYSIZE(pFileProgress->szDestPath), pszFileName);
    ::PostMessage(hwndReflection, FMG_MSG_SEND_FILE_PROGRESS, 0, (LPARAM)pFileProgress);
    int64_t nFileSize = 0;
    long nRetCode = GetFileMd5ValueA(pszFileName, szMd5, ARRAYSIZE(szMd5), nFileSize, hwndReflection, hCancelEvent);
    if (nRetCode == GET_FILE_MD5_FAILED)
    {
        LOG_ERROR(_T("Failed to upload image:%s as unable to get file md5."), pszFileName);
        return FILE_UPLOAD_FAILED;
    }
    else if (nRetCode == GET_FILE_MD5_USERCANCEL)
    {
        LOG_INFO(_T("User cancel to upload image:%s."), pszFileName);
        return FILE_UPLOAD_USERCANCEL;
    }

    if (nFileSize == 0)
    {
        LOG_ERROR(_T("Failed to upload image:%s as filesize is 0."), pszFileName);
        return FILE_UPLOAD_FAILED;
    }

    strcpy_s(uploadFileResult.m_szMd5, ARRAYSIZE(uploadFileResult.m_szMd5), szMd5);
    uploadFileResult.m_nFileSize = nFileSize;

    HANDLE	hFile = ::CreateFile(pszFileName,
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR(_T("Failed to upload image:%s as unable to open the file."), pszFileName);
        return FILE_UPLOAD_FAILED;
    }

    //����hFile�ں������ý���ʱ�Զ��ر�
    CAutoFileHandle autoFile(hFile);

    //�ļ�utf8��ʽ����
    char szUtf8Name[MAX_PATH] = { 0 };
    EncodeUtil::UnicodeToUtf8(::PathFindFileName(pszFileName), szUtf8Name, ARRAYSIZE(szUtf8Name));

    CIUSocket& iusocket = CIUSocket::GetInstance();
    if (!iusocket.ConnectToImgServer())
    {
        LOG_ERROR(_T("Failed to connect to ImgServer when upload image:%s as unable to open the file."), pszFileName);
        return false;
    }

    int64_t offsetX = 0;

    while (true)
    {
        std::string outbuf;
        net::BinaryStreamWriter writeStream(&outbuf);
        writeStream.WriteInt32(msg_type_upload_req);
        writeStream.WriteInt32(m_seq);
        writeStream.WriteCString(szMd5, 32);
        writeStream.WriteInt64(offsetX);
        writeStream.WriteInt64(nFileSize);
        int64_t eachfilesize = 512 * 1024;
        if (nFileSize - offsetX < eachfilesize)
            eachfilesize = nFileSize - offsetX;
        CMiniBuffer buffer(eachfilesize);
        DWORD dwFileRead;
        if (!::ReadFile(hFile, buffer.GetBuffer(), (DWORD)eachfilesize, &dwFileRead, NULL) || eachfilesize != (int64_t)dwFileRead)
            break;
        std::string filedata;
        filedata.append(buffer.GetBuffer(), (size_t)buffer.GetSize());
        writeStream.WriteString(filedata);
        writeStream.Flush();
        file_msg headerx = { outbuf.length() };
        outbuf.insert(0, (const char*)&headerx, sizeof(headerx));
        if (!iusocket.SendOnImgPort(outbuf.c_str(), (int64_t)outbuf.length()))
            break;

        offsetX += eachfilesize;
        pFileProgress = new FileProgress();
        memset(pFileProgress, 0, sizeof(FileProgress));
        //AtlTrace(_T("nTotalSent:%d\n"), nTotalSent);
        //AtlTrace(_T("nFileSize:%d\n"), nFileSize);
        //nTotalSent*100���ܻᳬ��long�ķ�Χ��������ʱת����__int64
        pFileProgress->nPercent = (long)((__int64)offsetX * 100 / nFileSize);
        //AtlTrace(_T("pFileProgress->nPercent:%d\n"), pFileProgress->nPercent);
        _tcscpy_s(pFileProgress->szDestPath, ARRAYSIZE(pFileProgress->szDestPath), pszFileName);
        ::PostMessage(hwndReflection, FMG_MSG_SEND_FILE_PROGRESS, 0, (LPARAM)pFileProgress);

        file_msg header;
        if (!iusocket.RecvOnImgPort((char*)&header, (int64_t)sizeof(header)))
            break;

        CMiniBuffer recvBuf(header.packagesize);
        if (!iusocket.RecvOnImgPort(recvBuf.GetBuffer(), recvBuf.GetSize()))
            break;

        net::BinaryStreamReader readStream(recvBuf.GetBuffer(), (size_t)recvBuf.GetSize());
        int32_t cmd;
        if (!readStream.ReadInt32(cmd) || cmd != msg_type_upload_resp)
            break;

        //int seq;
        if (!readStream.ReadInt32(m_seq))
            break;

        int32_t nErrorCode = 0;
        if (!readStream.ReadInt32(nErrorCode))
            break;

        std::string filemd5;
        size_t md5length;
        if (!readStream.ReadString(&filemd5, 0, md5length) || md5length != 32)
            break;

        int64_t offset;
        if (!readStream.ReadInt64(offset))
            break;

        int64_t filesize;
        if (!readStream.ReadInt64(filesize))
            break;

        std::string dummyfiledata;
        size_t filedatalength;
        if (!readStream.ReadString(&dummyfiledata, 0, filedatalength) || filedatalength != 0)
            break;

        if (nErrorCode == file_msg_error_complete)
        {
            FillUploadFileResult(uploadFileResult, pszFileName, filemd5.c_str(), nFileSize, szMd5);
            LOG_INFO(_T("Succeed to upload file:%s as there already exist file on server."), pszFileName);

            //TODO: ����ⲿ���ͷ�������ڴ�й¶
            pFileProgress = new FileProgress();
            memset(pFileProgress, 0, sizeof(FileProgress));
            pFileProgress->nPercent = 100;
            _tcscpy_s(pFileProgress->szDestPath, ARRAYSIZE(pFileProgress->szDestPath), pszFileName);
            ::PostMessage(hwndReflection, FMG_MSG_SEND_FILE_PROGRESS, 0, (LPARAM)pFileProgress);
            iusocket.CloseImgServerConnection();
            return FILE_UPLOAD_SUCCESS;
        }
    }

    iusocket.CloseImgServerConnection();
    return FILE_UPLOAD_FAILED;
}

void CImageTaskThread::FillUploadFileResult(CUploadFileResult& uploadFileResult, PCTSTR pszLocalName, PCSTR pszRemoteName, int64_t nFileSize, char* pszMd5)
{
	uploadFileResult.m_bSuccessful = TRUE;
	uploadFileResult.m_nFileSize = nFileSize;
	_tcscpy_s(uploadFileResult.m_szLocalName, ARRAYSIZE(uploadFileResult.m_szLocalName), pszLocalName);
	strcpy_s(uploadFileResult.m_szRemoteName, ARRAYSIZE(uploadFileResult.m_szRemoteName), pszRemoteName);
	strcpy_s(uploadFileResult.m_szMd5, ARRAYSIZE(uploadFileResult.m_szMd5), pszMd5);
}

long CImageTaskThread::DownloadImage(LPCSTR lpszFileName, LPCTSTR lpszDestPath, BOOL bOverwriteIfExist, HWND hwndReflection, HANDLE hCancelEvent)
{
    //TODO: ȷ���Ƿ񸲸ǵķ���Ӧ���Ǹ���md5ֵ���жϱ��ص��ļ������ص��ļ��Ƿ���ȫ��ͬ
    if (Hootina::CPath::IsFileExist(lpszDestPath) && IUGetFileSize2(lpszDestPath)>0 && !bOverwriteIfExist)
    {
        LOG_INFO(_T("File %s already exsited, there is no need to download."), lpszDestPath);
        return FILE_DOWNLOAD_SUCCESS;
    }

    //ƫ����
    int64_t nOffset = 0;
    long nBreakType = FILE_DOWNLOAD_SUCCESS;

    char* pBuffer = NULL;
    //nCurrentContentSizeΪ��ǰ�յ��İ����ļ������ֽ�����С
    long nCurrentContentSize = 0;
    DWORD dwSizeToWrite = 0;
    DWORD dwSizeWritten = 0;
    BOOL bRet = TRUE;
    long nFileSize = 0;

    HANDLE hFile = ::CreateFile(lpszDestPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //AtlTrace(_T("lpszDestPath:%s.\n"), lpszDestPath);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR(_T("Failed to download file %s as unable to create the file."), lpszDestPath);
        return FILE_DOWNLOAD_FAILED;
    }

    CIUSocket& iusocket = CIUSocket::GetInstance();
    if (!iusocket.ConnectToImgServer())
    {
        LOG_ERROR(_T("Failed to connect to ImgServer when download file %s as unable to create the file."), lpszDestPath);
        return FILE_DOWNLOAD_FAILED;
    }

    CAutoFileHandle autoFileHandle(hFile);
    FileProgress* pFileProgress = NULL;

    int64_t offset = 0;
    while (true)
    {
        std::string outbuf;
        net::BinaryStreamWriter writeStream(&outbuf);
        writeStream.WriteInt32(msg_type_download_req);
        writeStream.WriteInt32(m_seq);
        writeStream.WriteCString(lpszFileName, strlen(lpszFileName));
        int64_t dummyoffset = 0;
        writeStream.WriteInt64(dummyoffset);
        int64_t dummyfilesize = 0;
        writeStream.WriteInt64(dummyfilesize);
        std::string dummyfiledata;
        writeStream.WriteString(dummyfiledata);
        int32_t clientNetType = client_net_type_broadband;
        writeStream.WriteInt32(clientNetType);
        writeStream.Flush();

        file_msg header = { outbuf.length() };
        outbuf.insert(0, (const char*)&header, sizeof(header));

        if (!iusocket.SendOnImgPort(outbuf.c_str(), (int64_t)outbuf.length()))
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        file_msg recvheader;
        if (!iusocket.RecvOnImgPort((char*)&recvheader, (int64_t)sizeof(recvheader)))
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        CMiniBuffer buffer(recvheader.packagesize);
        if (!iusocket.RecvOnImgPort(buffer.GetBuffer(), recvheader.packagesize))
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        net::BinaryStreamReader readStream(buffer.GetBuffer(), (size_t)recvheader.packagesize);
        int32_t cmd;
        if (!readStream.ReadInt32(cmd) || cmd != msg_type_download_resp)
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        //int seq;
        if (!readStream.ReadInt32(m_seq))
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        int32_t nErrorCode;
        if (!readStream.ReadInt32(nErrorCode) && nErrorCode != file_msg_error_not_exist)
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            if (nErrorCode == file_msg_error_not_exist)
                LOG_ERROR("File %s does not exsit on server.", lpszFileName);
        }

        std::string filemd5;
        size_t md5length;
        if (!readStream.ReadString(&filemd5, 0, md5length) || md5length == 0)
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        int64_t offset;
        if (!readStream.ReadInt64(offset))
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        int64_t filesize = 0;
        if (!readStream.ReadInt64(filesize) || filesize <= 0)
        {
            LOG_ERROR("Download img failed: read filesize error, filesize=%lld, img name=%s", filesize, filemd5.c_str());
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        std::string filedata;
        size_t filedatalength;
        if (!readStream.ReadString(&filedata, 0, filedatalength) || filedatalength == 0)
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        DWORD dwBytesWritten;
        if (!::WriteFile(hFile, filedata.c_str(), filedata.length(), &dwBytesWritten, NULL) || dwBytesWritten != filedata.length())
        {
            nBreakType = FILE_DOWNLOAD_FAILED;
            break;
        }

        offset += (int64_t)filedata.length();

        //��������TODO: ���ڷ���������ͼƬ������ڴ���Ϊδ�ͷŶ������ڴ�й¶��������
        pFileProgress = new FileProgress();
        memset(pFileProgress, 0, sizeof(FileProgress));
        _tcscpy_s(pFileProgress->szDestPath, ARRAYSIZE(pFileProgress->szDestPath), lpszDestPath);
        pFileProgress->nPercent = long(((__int64)offset * 100) / filesize);
        ::PostMessage(hwndReflection, FMG_MSG_RECV_FILE_PROGRESS, 0, (LPARAM)(pFileProgress));

        if (nErrorCode == file_msg_error_complete)
        {
            nBreakType = FILE_DOWNLOAD_SUCCESS;
            break;
        }
    }// end while-loop

    //�ر���ͼƬ������������
    iusocket.CloseImgServerConnection();

    //���سɹ�
    if (nBreakType == FILE_DOWNLOAD_SUCCESS)
    {
        //::CloseHandle(hFile);
        LOG_INFO(_T("Succeed to download file: %s."), lpszDestPath);
        pFileProgress = new FileProgress();
        memset(pFileProgress, 0, sizeof(FileProgress));
        _tcscpy_s(pFileProgress->szDestPath, ARRAYSIZE(pFileProgress->szDestPath), lpszDestPath);
        pFileProgress->nPercent = /*nOffset* 100 / nFileSize*/100;
        ::PostMessage(hwndReflection, FMG_MSG_RECV_FILE_PROGRESS, 0, (LPARAM)(pFileProgress));
    }
    //����ʧ�ܻ����û�ȡ������
    else
    {
        if (nBreakType == FILE_DOWNLOAD_FAILED)
            LOG_ERROR(_T("Failed to download image: %s."), lpszDestPath);
        else
            LOG_INFO(_T("User canceled to download file: %s."), lpszDestPath);
        //Ϊ����ɾ�����صİ��Ʒ����ʽ�ر��ļ����
        autoFileHandle.Release();
        ::DeleteFile(lpszDestPath);
    }

    return nBreakType;
}

BOOL CImageTaskThread::UploadUserThumb(PCTSTR pszFileName, HWND hwndReflection, CUploadFileResult& uploadFileResult)
{
    if (UploadImage(pszFileName, hwndReflection, NULL, uploadFileResult) != FILE_UPLOAD_SUCCESS)
        return FALSE;

    return TRUE;
}