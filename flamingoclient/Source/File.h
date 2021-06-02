/**
 *  �ļ���д��, File.h
 *  zhangyl 2017.07.26 
 */
#ifndef __FILE_H__
#define __FILE_H__

//#ifdef FILE_EXPORTS
//#define FILE_API __declspec(dllexport)
//#else
//#define FILE_API __declspec(dllimport)
//#endif

#define FILE_API

//��֧�ִ��ļ�������2G��
//TODO: ��ʶλ��Ҫֻ�򿪣�ֻ���������ڼ���
class FILE_API CFileX
{
public:
    CFileX();
    CFileX(PCTSTR pszFileName);
    ~CFileX();

    CFileX(const CFileX& rhs) = delete;
    CFileX& operator=(const CFileX& rhs) = delete;
    
    bool Open(PCTSTR pszFileName, bool bCreateIfNotExsit = true);
    bool Read(char* pszBuffer, int nBufferLength, int* nReadLength = NULL);
    bool Write(const char* pszBuffer, int nBufferLength);

    bool IsValid();
    int GetFileSize();

    void Close();

    /** ͨ����չ��ɾ��һ���ļ�
     *@param pszDir�����ļ��� ʾ��E:\\doc\\
     *@param pszExtension ��չ�� ʾ��*.doc
     */
    static bool DeleteAllFileByExtension(PCTSTR pszDir, PCTSTR pszExtension);

    static bool IsDirectoryExist(LPCTSTR lpszPath);

private:
    HANDLE      m_hFile;
};

#endif //!__FILE_H__