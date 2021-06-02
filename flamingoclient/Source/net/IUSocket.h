#pragma once

#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <condition_variable>
#include <stdint.h>

class CRecvMsgThread;

//����ͨ�Ų�ֻ�������ݴ���ͽ���
class CIUSocket
{
private:
	CIUSocket();
    ~CIUSocket(void);

    CIUSocket(const CIUSocket& rhs) = delete;
    CIUSocket& operator = (const CIUSocket& rhs) = delete;

public:
    static CIUSocket&  GetInstance();

    void SetRecvMsgThread(CRecvMsgThread* pThread);

    bool Init();
    void Uninit();

    void Join();
	
	void	LoadConfig();
	void	SetServer(PCTSTR lpszServer);
	void	SetFileServer(PCTSTR lpszFileServer);
    void	SetImgServer(PCTSTR lpszImgServer);
	void	SetProxyServer(PCTSTR lpszProxyServer);
	void	SetPort(short nPort);
	void    SetFilePort(short nFilePort);
    void    SetImgPort(short nImgPort);
	void	SetProxyPort(short nProxyPort);
	void    SetProxyType(long nProxyType);

    //�����͹ر���������������Զ�����
    void    EnableReconnect(bool bEnable);

    /** 
    *@param timeout ��ʱʱ�䣬��λΪs
    **/
	bool	Connect(int timeout = 3);
    bool    Reconnect(int timeout = 3);
    bool	ConnectToFileServer(int timeout = 3);
    bool	ConnectToImgServer(int timeout = 3);
	
	bool	IsClosed();
	bool	IsFileServerClosed();
    bool	IsImgServerClosed();
	
	void	Close();
	void	CloseFileServerConnection();
    void	CloseImgServerConnection();

    /** �ж���ͨSocket���Ƿ��յ�����
     * @return -1���� 0������ 1������
     */
	int    CheckReceivedData();

	//�첽�ӿ�
    void    Send(const std::string& strBuffer);

   	//ͬ���ӿ�
    //nTimeout��λ����
    bool    Register(const char* pszUser, const char* pszNickname, const char* pszPassword, int nTimeout, std::string& strReturnData);
    bool    Login(const char* pszUser, const char* pszPassword, int nClientType, int nOnlineStatus, int nTimeout, std::string& strReturnData);

    //��ʱʱ��,��λΪ��
	bool    SendOnFilePort(const char* pBuffer, int64_t nSize, int nTimeout = 30);	
    bool	RecvOnFilePort(char* pBuffer, int64_t nSize, int nTimeout = 30);
    //bool    CheckRecvDataOnFilePort(int nTimeout = 3000);

    bool    SendOnImgPort(const char* pBuffer, int64_t nSize, int nTimeout = 30);
    bool	RecvOnImgPort(char* pBuffer, int64_t nSize, int nTimeout = 30);
    //bool    CheckRecvDataOnImgPort(int nTimeout = 3000);

private:   
    void    SendThreadProc();
    void    RecvThreadProc();

    void    SendHeartbeatPackage();

    bool	Send();
    bool	Recv();

    //����nBuffSize���ȵ��ֽڣ����������ȥ�������Ϊʧ��
    bool    SendData(const char* pBuffer, int nBuffSize, int nTimeout);
    //��ȡnBufferSize���ȵ��ֽڣ�����ղ���������Ϊʧ��
    bool    RecvData(char* pszBuff, int nBufferSize, int nTimeout);

    //���
    bool    DecodePackages();
    
private:	
	SOCKET							m_hSocket;				//һ����;Socket��������socket��
	SOCKET							m_hFileSocket;			//���ļ���Socket������socket��
    SOCKET							m_hImgSocket;			//�������Socket������socket��
	short							m_nPort;
	short							m_nFilePort;
    short							m_nImgPort;
	std::string						m_strServer;			//��������ַ
    std::string						m_strFileServer;		//�ļ���������ַ
    std::string						m_strImgServer;		    //ͼƬ��������ַ
	
	long							m_nProxyType;			//�������ͣ�0��ʹ�ô�����
    std::string						m_strProxyServer;		//�����������ַ
	short							m_nProxyPort;			//����������˿ں�

    long                            m_nLastDataTime;        //���һ���շ����ݵ�ʱ��
    std::mutex                      m_mutexLastDataTime;    //����m_nLastDataTime�Ļ�����
    long                            m_nHeartbeatInterval;   //������ʱ��������λ��,Ĭ��Ϊ10��
    int32_t                         m_nHeartbeatSeq;        //���������к�
	
	bool							m_bConnected;
	bool							m_bConnectedOnFileSocket;
    bool                            m_bConnectedOnImgSocket;

    std::unique_ptr<std::thread>    m_spSendThread;
    std::unique_ptr<std::thread>    m_spRecvThread;

    std::string                     m_strSendBuf;
    std::string                     m_strRecvBuf;

    std::mutex                      m_mtSendBuf;

    std::condition_variable         m_cvSendBuf;
    std::condition_variable         m_cvRecvBuf;

    bool                            m_bStop;

    bool                            m_bEnableReconnect;     //��������������Ƿ�����

    std::string                     m_strRecordUser;
    std::string                     m_strRecordPassword;
    int                             m_nRecordClientType;
    int                             m_nRecordOnlineStatus;
    int                             m_seq;


	CRecvMsgThread*					m_pRecvMsgThread;
};