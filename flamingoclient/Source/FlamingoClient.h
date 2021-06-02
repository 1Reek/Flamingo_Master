#pragma once

#include "UserMgr.h"
#include "UserConfig.h"
#include "CheckNetworkStatusTask.h"
#include "SendMsgThread.h"
#include "RecvMsgThread.h"
#include "FileTaskThread.h"
#include "ImageTaskThread.h"
#include "net/IUSocket.h"

class CFriendStatus;
class CUpdateDlg;

class CFlamingoClient
{
public:
    static CFlamingoClient& GetInstance();

public:
	CFlamingoClient(void);
	~CFlamingoClient(void);

public:
	bool InitProxyWnd();										// ��ʼ��������
    bool InitNetThreads();                                      // ��ʼ�������߳�
	void Uninit();												// ����ʼ���ͻ���

	void SetServer(PCTSTR pszServer);
    void SetFileServer(PCTSTR pszServer);
    void SetImgServer(PCTSTR pszServer);
    void SetPort(short port);
	void SetFilePort(short port);
    void SetImgPort(short port);

	void SetUser(LPCTSTR lpUserAccount, LPCTSTR lpUserPwd);		// ����UTalk���������
	void SetLoginStatus(long nStatus);							// ���õ�¼״̬
	void SetCallBackWnd(HWND hCallBackWnd);						// ���ûص����ھ��
	void SetRegisterWindow(HWND hwndRegister);					// ����ע�����ķ�������
	void SetModifyPasswordWindow(HWND hwndModifyPassword);		// �����޸���������������
	void SetCreateNewGroupWindow(HWND hwndCreateNewGroup);		// ���ô���Ⱥ������������
	void SetFindFriendWindow(HWND hwndFindFriend);				// ���ò����û������������

	void StartCheckNetworkStatusTask();							
	//void StartGetUserInfoTask(long nType);					//��ȡ����
	void StartHeartbeatTask();

	void Register(PCTSTR pszAccountName, PCTSTR pszNickName, PCTSTR pszPassword);
    void Login(int nStatus = STATUS_ONLINE);				    // ��¼
	BOOL Logout();												// ע��
	void CancelLogin();											// ȡ����¼
    void GetFriendList();                                       // ��ȡ�����б�
    void GetGroupMembers(int32_t groupid);                      // ��ȡȺ��Ա
    void ChangeStatus(int32_t nNewStatus);                      // �����Լ��ĵ�¼״̬        

	BOOL FindFriend(PCTSTR pszAccountName, long nType, HWND hReflectionWnd);                // ���Һ���
	BOOL AddFriend(UINT uAccountToAdd);
	void ResponseAddFriendApply(UINT uAccountID, UINT uCmd);	                            //��Ӧ�Ӻ�����������
	BOOL DeleteFriend(UINT uAccountID);							                            // ɾ������
        
    bool AddNewTeam(PCTSTR pszNewTeamName);                                                 //����·���
    bool DeleteTeam(PCTSTR pszOldTeamName);                                                 //ɾ������
    bool ModifyTeamName(PCTSTR pszNewTeamName, PCTSTR pszOldTeamName);                      //�޸ķ�������
    bool MoveFriendToOtherTeam(UINT uUserID, PCTSTR pszOldTeamName, PCTSTR pszNewTeamName); //�ƶ���������������

	BOOL UpdateLogonUserInfo(PCTSTR pszNickName, 
							 PCTSTR pszSignature,
							 UINT uGender,
							 long nBirthday,
							 PCTSTR pszAddress,
							 PCTSTR pszPhone,
							 PCTSTR pszMail,
							 UINT uSysFaceID,
							 PCTSTR pszCustomFacePath,
							 BOOL bUseCustomThumb);

	void SendHeartbeatMessage();
	void ModifyPassword(PCTSTR pszOldPassword, PCTSTR pszNewPassword);
	void CreateNewGroup(PCTSTR pszGroupName);
    void ModifyFriendMarkName(UINT friendID, PCTSTR pszNewMarkName);
	void ChangeStatus(long nStatus);							// �ı�����״̬
	void UpdateBuddyList();										// ���º����б�
	void UpdateGroupList();										// ����Ⱥ�б�
	void UpdateRecentList();									// ���������ϵ���б�
	void UpdateBuddyInfo(UINT nUTalkUin);						// ���º�����Ϣ
	void UpdateGroupMemberInfo(UINT nGroupCode, UINT nUTalkUin);// ����Ⱥ��Ա��Ϣ
	void UpdateGroupInfo(UINT nGroupCode);						// ����Ⱥ��Ϣ
	void UpdateBuddyNum(UINT nUTalkUin);						// ���º��Ѻ���
	void UpdateGroupMemberNum(UINT nGroupCode, UINT nUTalkUin);	// ����Ⱥ��Ա����
	void UpdateGroupMemberNum(UINT nGroupCode, std::vector<UINT>* arrUTalkUin);	// ����Ⱥ��Ա����
	void UpdateGroupNum(UINT nGroupCode);											// ����Ⱥ����
	void UpdateBuddySign(UINT nUTalkUin);											// ���º��Ѹ���ǩ��
	void UpdateGroupMemberSign(UINT nGroupCode, UINT nUTalkUin);					// ����Ⱥ��Ա����ǩ��
	void ModifyUTalkSign(LPCTSTR lpSign);											// �޸�UTalk����ǩ��
	void UpdateBuddyHeadPic(UINT nUTalkUin, UINT nUTalkNum);						// ���º���ͷ��
	void UpdateGroupMemberHeadPic(UINT nGroupCode, UINT nUTalkUin, UINT nUTalkNum);	// ����Ⱥ��Աͷ��
	void UpdateGroupHeadPic(UINT nGroupCode, UINT nGroupNum);						// ����Ⱥͷ��
	void UpdateGroupFaceSignal();													// ����Ⱥ��������


    BOOL SendBuddyMsg(UINT nFromUin, const tstring& strFromNickName, UINT nToUin, const tstring& strToNickName, time_t nTime, const tstring& strChatMsg, HWND hwndFrom = NULL);// ���ͺ�����Ϣ
	BOOL SendGroupMsg(UINT nGroupId, time_t nTime, LPCTSTR lpMsg, HWND hwndFrom);	// ����Ⱥ��Ϣ
	BOOL SendSessMsg(UINT nGroupId, UINT nToUin, time_t nTime, LPCTSTR lpMsg);		// ������ʱ�Ự��Ϣ
	BOOL SendMultiChatMsg(const std::set<UINT> setAccountID, time_t nTime, LPCTSTR lpMsg, HWND hwndFrom=NULL);//Ⱥ����Ϣ

	BOOL IsOffline();											// �Ƿ�����״̬


	long GetStatus();											// ��ȡ����״̬
	BOOL GetVerifyCodePic(const BYTE*& lpData, DWORD& dwSize);	// ��ȡ��֤��ͼƬ
	void SetBuddyListAvailable(BOOL bAvailable);
	BOOL IsBuddyListAvailable();

	CBuddyInfo* GetUserInfo(UINT uAccountID=0);			// ��ȡ�û���Ϣ
	CBuddyList* GetBuddyList();						// ��ȡ�����б�
	CGroupList* GetGroupList();						// ��ȡȺ�б�
	CRecentList* GetRecentList();						// ��ȡ�����ϵ���б�
	CMessageList* GetMessageList();					// ��ȡ��Ϣ�б�
	CMessageLogger* GetMsgLogger();					// ��ȡ��Ϣ��¼������

	tstring GetUserFolder();							// ��ȡ�û��ļ��д��·��

	tstring GetPersonalFolder(UINT nUserNum = 0);		// ��ȡ�����ļ��д��·��
	tstring GetChatPicFolder(UINT nUserNum = 0);		// ��ȡ����ͼƬ���·��

	tstring GetUserHeadPicFullName(UINT nUserNum = 0);	// ��ȡ�û�ͷ��ͼƬȫ·���ļ���
	tstring GetBuddyHeadPicFullName(UINT nUTalkNum);	// ��ȡ����ͷ��ͼƬȫ·���ļ���
	tstring GetGroupHeadPicFullName(UINT nGroupNum);	// ��ȡȺͷ��ͼƬȫ·���ļ���
	tstring GetSessHeadPicFullName(UINT nUTalkNum);		// ��ȡȺ��Աͷ��ͼƬȫ·���ļ���
	tstring GetChatPicFullName(LPCTSTR lpszFileName);	// ��ȡ����ͼƬȫ·���ļ���
	tstring GetMsgLogFullName(UINT nUserNum = 0);		// ��ȡ��Ϣ��¼ȫ·���ļ���

	BOOL IsNeedUpdateBuddyHeadPic(UINT nUTalkNum);		// �ж��Ƿ���Ҫ���º���ͷ��
	BOOL IsNeedUpdateGroupHeadPic(UINT nGroupNum);		// �ж��Ƿ���Ҫ����Ⱥͷ��
	BOOL IsNeedUpdateSessHeadPic(UINT nUTalkNum);		// �ж��Ƿ���Ҫ����Ⱥ��Աͷ��

	void RequestServerTime();							// ��ȡ������ʱ��
	time_t GetCurrentTime();							// ��ȡ��ǰʱ��(�Է�����ʱ��Ϊ��׼)
	void LoadUserConfig();								// �����û�������Ϣ
	void SaveUserConfig();								// �����û�������Ϣ

	void GoOnline();									
	void GoOffline();									// ���߻�������

	long ParseBuddyStatus(long nFlag);					// �����û�����״̬
	void CacheBuddyStatus();							// �����û�����״̬
	BOOL SetBuddyStatus(UINT uAccountID, long nStatus);
    BOOL SetBuddyClientType(UINT uAccountID, long nNewClientType);

private:
	void OnHeartbeatResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnNetworkStatusChange(UINT message, WPARAM wParam, LPARAM lParam);
	void OnRegisterResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnLoginResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateUserBasicInfo(UINT message, WPARAM wParam, LPARAM lParam);
    void OnUpdateGroupBasicInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnModifyInfoResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnRecvUserStatusChangeData(UINT message, WPARAM wParam, LPARAM lParam);
	void OnRecvAddFriendRequest(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUserStatusChange(UINT message, WPARAM wParam, LPARAM lParam);
    void OnSendConfirmMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateChatMsgID(UINT message, WPARAM wParam, LPARAM lParam);
	void OnFindFriend(UINT message, WPARAM wParam, LPARAM lParam);
	void OnBuddyCustomFaceAvailable(UINT message, WPARAM wParam, LPARAM lParam);
	void OnModifyPasswordResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnCreateNewGroupResult(UINT message, WPARAM wParam, LPARAM lParam);	
	void OnDeleteFriendResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddyList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateRecentList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnBuddyMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnGroupMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSessMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSysGroupMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnStatusChangeMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnKickMsg(UINT message, WPARAM wParam, LPARAM lParam);
    void OnScreenshotMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddyNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddySign(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberSign(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddyInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupInfo(UINT message, WPARAM wParam, LPARAM lParam);
	//void OnUpdateC2CMsgSig(UINT message, WPARAM wParam, LPARAM lParam);
	void OnChangeStatusResult(UINT message, WPARAM wParam, LPARAM lParam);
	void OnTargetInfoChange(UINT message, WPARAM wParam, LPARAM lParam);

	void OnInternal_GetBuddyData(UINT message, WPARAM wParam, LPARAM lParam);
	void OnInternal_GetGroupData(UINT message, WPARAM wParam, LPARAM lParam);
	void OnInternal_GetGMemberData(UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnInternal_GroupId2Code(UINT message, WPARAM wParam, LPARAM lParam);

	BOOL CreateProxyWnd();		// ����������
	BOOL DestroyProxyWnd();		// ���ٴ�����
	static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CUserMgr						m_UserMgr;
	CCheckNetworkStatusTask			m_CheckNetworkStatusTask;

    CSendMsgThread					m_SendMsgThread;
    CRecvMsgThread                  m_RecvMsgThread;
    CFileTaskThread					m_FileTask;
    CImageTaskThread                m_ImageTask;
    
	CUserConfig						m_UserConfig;

	std::vector<AddFriendInfo*>		m_aryAddFriendInfo;

private:
	time_t							m_ServerTime;			//������ʱ��
	DWORD							m_StartTime;			//��ʼ��ʱ��ʱ��

	BOOL							m_bNetworkAvailable;	//�����Ƿ����

	HWND							m_hwndRegister;			//ע�ᴰ��
	HWND							m_hwndFindFriend;		//���Һ��Ѵ���
	HWND							m_hwndModifyPassword;	//�޸����봰��
	HWND							m_hwndCreateNewGroup;	//����Ⱥ�鴰��

	BOOL							m_bBuddyIDsAvailable;	//�û�����ID�Ƿ����
	BOOL							m_bBuddyListAvailable;	//�û������б���Ϣ�Ƿ����

	std::map<UINT, long>			m_mapUserStatusCache;	//��������״̬���棺key���˻�ID��value��״̬��
	std::map<UINT, UINT>			m_mapAddFriendCache;	//�Ӻ��Ѳ�������: key���˻�ID��value�ǲ�����

	long							m_nGroupCount;
	BOOL							m_bGroupInfoAvailable;
	BOOL							m_bGroupMemberInfoAvailable;
};
