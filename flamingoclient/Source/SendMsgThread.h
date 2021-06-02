#pragma once
#include <list>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "net/IUProtocolData.h"
#include "Utils.h"
#include "Thread.h"

class CUserMgr;
class CFlamingoClient;
class CBuddyMessage;
class CGroupMessage;
class CSessMessage;
class CContent;

// д��һ��������Ϣ��¼
void WriteBuddyMsgLog(CUserMgr* lpUserMgr, UINT nUTalkNum, LPCTSTR lpNickName, 
					  BOOL bSelf, CBuddyMessage* lpMsg);

// д��һ��Ⱥ��Ϣ��¼
void WriteGroupMsgLog(CUserMgr* lpUserMgr, UINT nGroupNum, UINT nUTalkNum, 
					  LPCTSTR lpNickName, CBuddyMessage* lpMsg);

// д��һ����ʱ�Ự(Ⱥ��Ա)��Ϣ��¼
void WriteSessMsgLog(CUserMgr* lpUserMgr, UINT nUTalkNum, LPCTSTR lpNickName, 
					 BOOL bSelf, CSessMessage* lpMsg);

class CMsgItem
{
public:
	CMsgItem(void);
	~CMsgItem(void);

public:
	long				m_nType;
	void*				m_lpMsg;
	UINT				m_nGroupNum;		
	UINT				m_nUTalkNum;		//TODO: ��Ҫɾ��
	std::vector<UINT>	m_arrTargetIDs;		//����Ϣ��Ŀ���û�ID
	tstring				m_strNickName;
	tstring				m_strGroupSig;
	HWND				m_hwndFrom;			//��Ϣ�����ĸ����촰��
};

class CNetData;
class CRegisterRequest;
class CLoginRequest;
class CUserBasicInfoRequest;
class CUserExtendInfoRequest;
class CLoginUserFriendsIDRequest;
class CSentChatMessage;
class CSentChatConfirmImageMessage;
class CFindFriendRequest;
class COperateFriendRequest;
class CHeartbeatMessageRequest;
class CUpdateLogonUserInfoRequest;
class CModifyPasswordRequest;
class CCreateNewGroupRequest;

class CSendMsgThread : public CThread
{
public:
    CSendMsgThread();
    virtual ~CSendMsgThread(void);

public:
	void AddItem(CNetData* pItem);
	void DeleteAllItems();
	
    BOOL AddBuddyMsg(UINT nFromUin, const tstring& strFromNickName, UINT nToUin, const tstring& strToNickName, time_t nTime, const tstring& strChatMsg, HWND hwndFrom = NULL);
	BOOL AddGroupMsg(UINT nGroupId, time_t nTime, LPCTSTR lpMsg, HWND hwndFrom=NULL);
	BOOL AddMultiMsg(const std::set<UINT> setAccountID, time_t nTime, LPCTSTR lpMsg, HWND hwndFrom=NULL);
	BOOL AddSessMsg(UINT nGroupId, UINT nToUin, time_t nTime, LPCTSTR lpMsg);

	virtual void Stop() override;

protected:
    virtual void Run() override;

private:
    void HandleItem(CNetData* pNetData);
	void HandleRegister(const CRegisterRequest* pRegisterRequest);
    void HandleLogon(const CLoginRequest* pLoginRequest);
	void HandleUserBasicInfo(const CUserBasicInfoRequest* pUserBasicInfo);
    void HandleChangeUserStatus(const CChangeUserStatusRequest* pChangeUserStatusRequest);
    void HandleGroupBasicInfo(const CGroupBasicInfoRequest* pGroupBasicInfo);
	BOOL HandleSentChatMessage(const CSentChatMessage* pSentChatMessage);				
	BOOL HandleSentConfirmImageMessage(const CSentChatConfirmImageMessage* pConfirmImageMessage);	//ͼƬ�ϴ��ɹ��Ժ��ȷ����Ϣ
	void HandleFindFriendMessage(const CFindFriendRequest* pFindFriendRequest);
	void HandleOperateFriendMessage(const COperateFriendRequest* pOperateFriendRequest);
	BOOL HandleHeartbeatMessage(const CHeartbeatMessageRequest* pHeartbeatRequest);
	void HandleUpdateLogonUserInfoMessage(const CUpdateLogonUserInfoRequest* pRequest);
	void HandleModifyPassword(const CModifyPasswordRequest* pModifyPassword);
	void HandleCreateNewGroup(const CCreateNewGroupRequest* pCreateNewGroup);
    void HandleOperateTeam(const CAddTeamInfoRequest* pAddNewTeam);
    void HandleModifyFriendMarkName(const CModifyFriendMakeNameRequest* pModifyFriendMakeNameRequest);
    void HandleMoveFriendMessage(const CMoveFriendRequest* pMoveFriendRequest);

	BOOL HandleFontInfo(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleSysFaceId(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleShakeWindowMsg(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleCustomPic(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleFile(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
    BOOL HandleRemoteDesktop(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL CreateMsgContent(const tstring& strChatMsg, std::vector<CContent*>& arrContent);
	
    //TODO: ���ĸ�������ʼ���Ժϲ���һ������
    BOOL SendBuddyMsg(CMsgItem* lpMsgItem);			// ���ͺ�����Ϣ
	BOOL SendGroupMsg(CMsgItem* lpMsgItem);			// ����Ⱥ��Ϣ
	BOOL SendMultiMsg(CMsgItem* lpMsgItem);			// Ⱥ����Ϣ
    BOOL SendMultiChatMessage(const char* pszChatMsg, int nChatMsgLength, UINT* pAccountList, int nAccountNum);
	BOOL SendSessMsg(CMsgItem* lpMsgItem);			// ����Ⱥ��Ա��Ϣ
    
	
	BOOL ProcessBuddyMsg(CBuddyMessage* pMsg);		//�Է��͵���Ϣ���мӹ�
	BOOL ProcessMultiMsg(CMsgItem* pMsgItem);		//��Ⱥ����Ϣ���мӹ�
	void AddItemToRecentSessionList(UINT uUserID, UINT uFaceID, PCTSTR pszNickName, PCTSTR pszText, time_t nMsgTime);		//�������ϵ���б������һ��
	std::wstring UnicodeToHexStr(const WCHAR* lpStr, BOOL bDblSlash);

public:
	CFlamingoClient*			m_lpFMGClient;
	CUserMgr*					m_lpUserMgr;

private:
	UINT						m_nMsgId;
	tstring						m_strGFaceKey;
	tstring						m_strGFaceSig;
	std::vector<CMsgItem*>		m_arrItem;

	std::list<CNetData*>		m_listItems;
    std::mutex                  m_mtItems;
    std::condition_variable     m_cvItems;

    int32_t                     m_seq{};            //�����к�
};
