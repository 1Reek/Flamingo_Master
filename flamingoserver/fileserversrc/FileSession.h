/** 
 * FileSession.h
 * zhangyl, 2017.03.17
 **/

#pragma once
#include "../net/Buffer.h"
#include "TcpSession.h"

class FileSession : public TcpSession
{
public:
    FileSession(const std::shared_ptr<TcpConnection>& conn, const char* filebasedir);
    virtual ~FileSession();

    FileSession(const FileSession& rhs) = delete;
    FileSession& operator =(const FileSession& rhs) = delete;

    //�����ݿɶ�, �ᱻ�������loop����
    void onRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime);   

private:
    //64λ�����ϣ�size_t��8���ֽ�
    bool process(const std::shared_ptr<TcpConnection>& conn, const char* inbuf, size_t length);
    
    bool onUploadFileResponse(const std::string& filemd5, int64_t offset, int64_t filesize, const std::string& filedata, const std::shared_ptr<TcpConnection>& conn);
    bool onDownloadFileResponse(const std::string& filemd5, int32_t clientNetType, const std::shared_ptr<TcpConnection>& conn);

    void resetFile();

private:
    int32_t           m_id;         //session id
    int32_t           m_seq;        //��ǰSession���ݰ����к�

    //��ǰ�ļ���Ϣ
    FILE*             m_fp{};
    int64_t           m_currentDownloadFileOffset{};    //��ǰ�������ص��ļ���ƫ����
    int64_t           m_currentDownloadFileSize{};      //��ǰ�������ص��ļ��Ĵ�С(��������Ժ������0)
    std::string       m_strFileBaseDir;                 //�ļ�Ŀ¼
    bool              m_bFileUploading;                 //�Ƿ��������ϴ��ļ��Ĺ�����
};