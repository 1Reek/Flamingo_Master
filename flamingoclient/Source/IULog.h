#ifndef __LOG_H__
#define __LOG_H__

//#ifdef LOG_EXPORTS
//#define LOG_API __declspec(dllexport)
//#else
//#define LOG_API __declspec(dllimport)
//#endif

#define LOG_API

enum LOG_LEVEL
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

//ע�⣺�����ӡ����־��Ϣ�������ģ����ʽ���ַ���Ҫ��_T()�����������
//e.g. LOG_INFO(_T("GroupID=%u, GroupName=%s, GroupName=%s."), lpGroupInfo->m_nGroupCode, lpGroupInfo->m_strAccount.c_str(), lpGroupInfo->m_strName.c_str());
#define LOG_INFO(...)     CIULog::Log(LOG_LEVEL_INFO, __FUNCSIG__,__LINE__, __VA_ARGS__)
#define LOG_WARNING(...)    CIULog::Log(LOG_LEVEL_WARNING, __FUNCSIG__, __LINE__,__VA_ARGS__)
#define LOG_ERROR(...)      CIULog::Log(LOG_LEVEL_ERROR, __FUNCSIG__,__LINE__, __VA_ARGS__)

class LOG_API CIULog
{
public:
    static bool Init(bool bToFile, bool bTruncateLongLog, PCTSTR pszLogFileName);
	static void Uninit();

    static void SetLevel(LOG_LEVEL nLevel);
	
	//������߳�ID�ź����ں���ǩ�����к�
	static bool Log(long nLevel, PCTSTR pszFmt, ...);
	//����߳�ID�ź����ں���ǩ�����к�
	static bool Log(long nLevel, PCSTR pszFunctionSig, int nLineNo, PCTSTR pszFmt, ...);		//ע��:pszFunctionSig����ΪAnsic�汾
    static bool Log(long nLevel, PCSTR pszFunctionSig, int nLineNo, PCSTR pszFmt, ...);
private:
    CIULog() = delete;
    ~CIULog() = delete;

    CIULog(const CIULog& rhs) = delete;
    CIULog& operator=(const CIULog& rhs) = delete;

    static void GetTime(char* pszTime, int nTimeStrLength);
	
private:
	static bool		        m_bToFile;			    //��־д���ļ�����д������̨  
	static HANDLE	        m_hLogFile;
    static bool             m_bTruncateLongLog;     //����־�Ƿ�ض�
    static LOG_LEVEL        m_nLogLevel;            //��־����
};

#endif // !__LOG_H__