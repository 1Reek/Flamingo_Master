#pragma once
#include "Buffer.h"
#include <stdint.h>

////////////////////////////////////
//�ļ�ϵ�еĺ���
////////////////////////////////////
//��һ���ļ�����ø��ļ��ľ��
HANDLE GetFileHandle(PCTSTR pszFileName);
//��ȡ�ļ���С
DWORD IUGetFileSize(PCTSTR pszFileName);		//��֧�ֳ���4G���ļ�
DWORD IUGetFileSize(HANDLE hFile);				//��֧�ֳ���4G���ļ�
UINT64 IUGetFileSize2(PCTSTR pszFileName);		//֧�ֳ���4G���ļ�

//��ȡ�ļ�md5ֵ
long GetFileMd5ValueA(PCTSTR pszFileName, char* pszMd5, long nMd5Length, int64_t& nFileSize, HWND hwndReflection=NULL, HANDLE hCancelEvent=NULL);
BOOL GetFileMd5ValueW(PCTSTR pszFileName, TCHAR* pszMd5, long nMd5Length);
//��ȡ�ļ����ϴ����ƣ���С|md5ֵ
BOOL GetFileUploadName(PCTSTR pszFileName, char* pszUploadName, long nUploadNameLength);
//��ȡ�ļ�ָ������
BOOL GetFileData(HANDLE hFile, DWORD dwOffset, DWORD dwLength, char* buffer);
BOOL GetFileData(PCTSTR pszFileName, DWORD dwOffset, DWORD dwLength, char* buffer);

class CAutoFileHandle
{
public:
	CAutoFileHandle(HANDLE hFile);
	~CAutoFileHandle();

public:
	operator HANDLE();
	void Release();

private:
	HANDLE m_hFile;
};

class CFile
{
public:
	CFile();
	~CFile();

	BOOL Open(LPCTSTR lpszFileName, BOOL bCreateAlways = TRUE);
	long GetSize();
	
	const char* Read();
	BOOL Write(const char* pBuffer, long nSize);
	HANDLE GetFileHandle(){return m_hFile;}

	void Close();

private:
	HANDLE	m_hFile;
	CBuffer	m_Buffer;
};


class CFile2
{
public:
// Flag values
	enum OpenFlags {
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
		osNoBuffer =       (int) 0x10000,
		osWriteThrough =   (int) 0x20000,
		osRandomAccess =   (int) 0x40000,
		osSequentialScan = (int) 0x80000,
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	static const HANDLE hFileNull;

// Constructors
	CFile2();
	CFile2(HANDLE hFile);
	CFile2(LPCTSTR lpszFileName, UINT nOpenFlags);

// Attributes
	HANDLE m_hFile;
	operator HANDLE() const;

	virtual ULONGLONG GetPosition() const;

// Operations
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags);

	static void PASCAL Rename(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
	static void PASCAL Remove(LPCTSTR lpszFileName);

	ULONGLONG SeekToEnd();
	void SeekToBegin();


	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual ULONGLONG GetLength() const;

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);

	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

// Implementation
public:
	virtual ~CFile2();

	enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
	enum BufferFlags 
	{ 
		bufferDirect = 0x01,
		bufferBlocking = 0x02
	};

protected:
	BOOL m_bCloseOnDelete;
	CString m_strFileName;
};