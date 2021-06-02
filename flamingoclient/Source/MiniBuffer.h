#pragma once
#include <stdint.h>

//�����ڳ����������Զ��ͷŵ������ڴ���
class CMiniBuffer
{
public:
    //FIXME: ��int64_tǿ��ת����int32���ܻ�������(����ڴ���ʵ���䲻������!!)
	CMiniBuffer(int64_t nSize, BOOL bAutoRelease = TRUE);
	~CMiniBuffer();

	void Release();

	int64_t GetSize();
	char* GetBuffer();
	
    //������Ҫʹ��char* �ĵط�������ֱ��ʹ��CMiniBuffer����
	operator PSTR(); 
	
	void EnableAutoRelease(BOOL bAutoRelease);
	BOOL IsAutoRelease();

private:
	BOOL	m_bAutoRelease;
    int64_t	m_nSize;
	char*	m_pData;
};