#pragma once

#include "UserSessionData.h"
#include "MessageLogger.h"
#include "UserConfig.h"
#include <map>

#define GROUP_ID_BASE 0x0FFFFFFF

#define IsGroupTarget(u) ((u >= GROUP_ID_BASE) ? TRUE : FALSE)

//��ǰ��¼���û���Ϣ
class CUserMgr
{
public:
    static CUserMgr& GetInstance();

public:
	CUserMgr(void);
	~CUserMgr(void);

public:
	tstring GetUserFolder();							// ��ȡ�û��ļ��д��·��
	
	tstring GetPersonalFolder();						// ��ȡ�����ļ��д��·��
	tstring GetChatPicFolder();							// ��ȡ����ͼƬ���·��
	tstring GetCustomUserThumbFolder();					// ��ȡ�Զ����û�ͷ��Ŀ¼

	tstring GetUserHeadPicFullName(UINT uFace);			// ��ȡ�û�ͷ��ͼƬȫ·���ļ���
	tstring GetBuddyHeadPicFullName(UINT uFace);		// ��ȡ����ͷ��ͼƬȫ·���ļ���
	tstring GetGroupHeadPicFullName(UINT nGroupNum);	// ��ȡȺͷ��ͼƬȫ·���ļ���
	tstring GetSessHeadPicFullName(UINT nUTalkNum);		// ��ȡȺ��Աͷ��ͼƬȫ·���ļ���
	tstring GetChatPicFullName(LPCTSTR lpszFileName);	// ��ȡ����ͼƬȫ·���ļ���
	tstring GetMsgLogFullName();						// ��ȡ��Ϣ��¼ȫ·���ļ���
	tstring GetDefaultRecvFilePath();					// ��ȡĬ�Ͻ����ļ�·����

	BOOL IsNeedUpdateBuddyHeadPic(UINT nUTalkNum);		// �ж��Ƿ���Ҫ���º���ͷ��
	BOOL IsNeedUpdateGroupHeadPic(UINT nGroupNum);		// �ж��Ƿ���Ҫ����Ⱥͷ��
	BOOL IsNeedUpdateSessHeadPic(UINT nUTalkNum);		// �ж��Ƿ���Ҫ����Ⱥ��Աͷ��

	BOOL IsSelf(PCTSTR pszAccountName);
	
	
	BOOL IsFriend(UINT uAccountID);						//ָ���˺�ID�Ƿ��Ǻ���
	BOOL IsFriend(PCTSTR pszAccountName);				//ָ���˺����Ƿ��Ǻ���

	BOOL AddFriend(UINT uAccountID, PCTSTR pszAccountName, PCTSTR pszNickName);
	BOOL DeleteFriend(UINT uAccountID);

	
	tstring GetAccountName(UINT uAccountID) const;			//ͨ���ڲ�ID��ȡ�˻���
	void	SetAccountName(UINT uAccountID, PCTSTR pszAccountName);

	tstring GetNickName(UINT uAccountID) const;				//ͨ���ڲ�ID��ȡ�ǳ���
	void	SetNickName(UINT uAccountID, PCTSTR pszNickName);

	tstring GetGroupName(UINT uGroupCode);
	tstring	GetGroupAccount(UINT uGroupCode);

	UINT	GetFaceID(UINT uAccountID) const;
	void	SetFaceID(UINT uAccountID, UINT uFaceID);

	PCTSTR  GetCustomFace(UINT uAccountID) const;

	void	SetStatus(UINT uAccountID, long nStatus);	//�����Լ�����ĳ�����ѵ�״̬
	long	GetStatus(UINT uAccountID);					//��ȡĳ���Լ�����ĳ�����ѵ�״̬

    void	SetClientType(UINT uAccountID, long nNewClientType);
    long	GetClientType(UINT uAccountID);

	UINT	GetMsgID(UINT uAccountID) const;
	BOOL	SetMsgID(UINT uAccountID, UINT uMsgID);

	tstring GetNickName(PCTSTR pszAccountName) const;	//ͨ���˻���ȡ�ǳ���

	UINT	GetFaceID(PCTSTR pszAccountName) const;		//ͨ���˻���ȡ�ǳ�

	void	ResetToOfflineStatus();						//���ó�����״̬

	BOOL ExitGroup(UINT uGroupID);						//�˳�Ⱥ��
	BOOL IsGroupMember(UINT uAccountID, UINT uGroupID);	//�˻�uAccount�Ƿ���ָ��Ⱥ�ĳ�Ա

	BOOL LoadRecentList();								//�Ӵ����м��������ϵ��
	BOOL StoreRecentList();								//�������ϵ�˴洢������

	long GetRecentListCount();							//��ȡ�����ϵ������
	void ClearRecentList();								//��������ϵ��
	BOOL DeleteRecentItem(UINT uAccountID);				//ɾ��ָ�����ѻỰ

	BOOL LoadTeamInfo();								//�Ӵ����м��ط�����Ϣ
	BOOL StoreTeamInfo();								//��������Ϣ�洢������

	BOOL LoadBuddyInfo();
	BOOL SaveBuddyInfo();

	PCTSTR GetMarkNameByAccountID(UINT uAccountID);
	long GetTeamIndexByAccountID(UINT uAccountID);
	

	void ClearUserInfo();

	long GetTotalBuddyCount();	

public:		
	HWND			m_hProxyWnd;
	HWND			m_hCallBackWnd;		//����Ϊ�����ڶԻ���
	
	CBuddyInfo		m_UserInfo;
	CBuddyList		m_BuddyList;
	
	CMessageList	m_MsgList;
	CMessageLogger	m_MsgLogger;

	CGroupList		m_GroupList;
	CRecentList		m_RecentList;

private:
	std::map<UINT, BuddyInfo*>		m_mapLocalBuddyInfo;
};
