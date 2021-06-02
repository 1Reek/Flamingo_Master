// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "FlamingoClient.h"
#include "TrayIcon.h"
#include "FaceList.h"
#include "LoginAccountList.h"
#include "CascadeWinManager.h"
#include "RecentListCtrl.h"
#include "BuddyListCtrl.h"
#include "LoginDlg.h"
//#include "VerifyCodeDlg.h"
#include "MsgTipDlg.h"
#include "LogonUserInfoDlg.h"
#include "ModifyPasswordDlg.h"
#include "MultiChatDlg.h"
#include "BuddyInfoFloatWnd.h"

//����˵�ID����
#define TEAM_MENU_ITEM_BASE       0xF001

//�����״̬
enum MAINPANEL_STATUS
{
    MAINPANEL_STATUS_NOTLOGIN,     //�����δ��¼
    MAINPANEL_STATUS_LOGINING,     //��������ڵ�¼
    MAINPANEL_STATUS_LOGIN,        //������Ѿ���¼
    MAINPANEL_STATUS_RECONNECTING, //�������������
    MAINPANEL_STATUS_USERGOOFFLINE //������û���������
};

class CGMemberInfoMapKey
{
public:
	CGMemberInfoMapKey(void):m_nGroupCode(0), m_nUTalkUin(0){}
	~CGMemberInfoMapKey(void){}

public:
	bool operator < (const CGMemberInfoMapKey& rhs) const
	{
		if(m_nGroupCode < rhs.m_nGroupCode) return true;
		if(m_nGroupCode > rhs.m_nGroupCode) return false;
		return  m_nUTalkUin < rhs.m_nUTalkUin;
	}

public:
	UINT m_nGroupCode;
	UINT m_nUTalkUin;
};

class CBuddyChatDlg;
class CBuddyInfoDlg;
class CGroupChatDlg;
class CGroupInfoDlg;
class CSessChatDlg;

class CFindFriendDlg;
class CRemoteDesktopDlg;

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	CMainDlg(void);
	~CMainDlg(void);

	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_HOTKEY(OnHotKey)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDM_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)
		COMMAND_ID_HANDLER_EX(IDC_UNLOCK, OnBtn_UnlockPanel)				    //����������
		COMMAND_ID_HANDLER_EX(ID_MENU_SHOW_MAIN_PANEL, OnMenu_ShowMainPanel)
		COMMAND_ID_HANDLER_EX(ID_MENU_LOCK, OnMenu_LockPanel)					//����������
		COMMAND_ID_HANDLER_EX(ID_MENU_EXIT, OnMenu_Exit)
		COMMAND_ID_HANDLER_EX(ID_MENU_MUTE, OnMenu_Mute)						//�ر���������
		COMMAND_ID_HANDLER_EX(ID_BTN_FIND, OnBtn_ShowAddFriendDlg)
		COMMAND_ID_HANDLER_EX(IDM_SYSTEMSETTINGS, OnBtn_ShowSystemSettingDlg)	//��ϵͳ���öԻ���
		COMMAND_ID_HANDLER_EX(IDM_MODIFYPASSWORD, OnBtn_ModifyPassword)			//�޸�����
        COMMAND_ID_HANDLER_EX(IDC_BTN_MAIL, OnBtn_OpenMail)      
		COMMAND_ID_HANDLER_EX(ID_BTN_MAIN_MENU, OnBtn_MainMenu)					//���˵���ť
		COMMAND_ID_HANDLER_EX(ID_BTN_MULTI_CHAT, OnBtn_MultiChat)				//Ⱥ����ť
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)				//ȡ����¼
		NOTIFY_HANDLER_EX(ID_TABCTRL_MAIN, TCN_DROPDOWN, OnTabCtrlDropDown)
		NOTIFY_HANDLER_EX(ID_TABCTRL_MAIN, TCN_SELCHANGE, OnTabCtrlSelChange)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_BUDDY, NM_DBLCLK, OnBuddyListDblClk)		 //˫�������б��еĺ���
		NOTIFY_HANDLER_EX(ID_LISTCTRL_BUDDY, NM_RCLICK, OnBuddyListRButtonUp)	 //�Ҽ������б��еĺ���
        NOTIFY_HANDLER_EX(ID_LISTCTRL_BUDDY, NM_HOVER, OnBuddyListHover)	     //����ں����б�������ͣ
		NOTIFY_HANDLER_EX(ID_LISTCTRL_GROUP, NM_DBLCLK, OnGroupListDblClk)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_GROUP, NM_RCLICK, OnGroupListRButtonUp)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_RECENT, NM_DBLCLK, OnRecentListDblClk)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_RECENT, NM_RCLICK, OnRecentListRButtonUp)	 //�Ҽ������б��еĺ���
		COMMAND_HANDLER_EX(ID_PIC_HEAD, BN_CLICKED, OnPic_Clicked_Head)			 //����ҵ�ͷ��
		
		COMMAND_ID_HANDLER_EX(IDM_ADDTEAM, OnBuddyListAddTeam);					//�����б���ӷ���
		COMMAND_ID_HANDLER_EX(IDM_DELETETEAM, OnBuddyListDeleteTeam)			//�����б�ɾ������
		COMMAND_ID_HANDLER_EX(IDM_MODIFYTEAMNAME, OnBuddyListModifyTeamName)	//���ѷ����޸�����
		COMMAND_RANGE_HANDLER_EX(TEAM_MENU_ITEM_BASE, 0xF015, OnMoveBuddyToTeam)//�ƶ���ϵ����ָ������

		
		COMMAND_ID_HANDLER_EX(ID_MENU_BIGHEADPIC, OnMenu_BigHeadPic)			//��ͷ��˵�
		COMMAND_ID_HANDLER_EX(ID_MENU_SMALLHEADPIC, OnMenu_SmallHeadPic)		//Сͷ��˵�
		COMMAND_ID_HANDLER_EX(ID_MENU_STDHEADPIC, OnMenu_StdHeadPic)			//��׼ͷ��˵�
		COMMAND_ID_HANDLER_EX(ID_MENU_SHOWBIGHEADPICINSEL, OnMenu_ShowBigHeadPicInSel)
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)								//����ǩ����ť
		COMMAND_HANDLER_EX(ID_EDIT_SIGN, EN_KILLFOCUS, OnEdit_Sign_KillFocus)		//����ǩ���༭��
		COMMAND_RANGE_HANDLER_EX(ID_MENU_IMONLINE, ID_MENU_IMOFFLINE, OnMenu_Status)
		COMMAND_ID_HANDLER_EX(ID_ABOUT, OnMainMenu_About)

		COMMAND_ID_HANDLER_EX(IDM_REFRESH_BUDDYLIST, OnRefreshBuddyList)		//ˢ����ϵ���б�
		COMMAND_ID_HANDLER_EX(IDM_SHOW_ONLINEBUDDY_ONLY, OnShowOnlineBuddyOnly) //����ʾ���ߺ���

		COMMAND_ID_HANDLER_EX(IDM_SENDMSG, OnMenu_SendBuddyMessage)				//���ͼ�ʱ��Ϣ
		COMMAND_ID_HANDLER_EX(IDM_SENDMSGFROMRECENTLIST, OnMenu_SendBuddyMessageFromRecentList)
		COMMAND_ID_HANDLER_EX(IDM_VIEWBUDDYINFO, OnMenu_ViewBuddyInfo)			//�鿴��������
		COMMAND_ID_HANDLER_EX(IDM_VIEWBUDDYINFO_FROMRECENTLIST, OnMenu_ViewBuddyInfoFromRecentList)		
        COMMAND_ID_HANDLER_EX(IDM_MODIFY_BUDDY_MARKNAME, OnMenu_ModifyBuddyMarkName)//�޸ĺ��ѱ�ע
        COMMAND_ID_HANDLER_EX(IDM_DELETEFRIEND, OnMenu_DeleteFriend)            //ɾ������
		COMMAND_ID_HANDLER_EX(IDM_CLEARRECENTLIST, OnClearAllRecentList)
		COMMAND_ID_HANDLER_EX(IDM_DELETERECENTITEM, OnDeleteRecentItem);

		COMMAND_ID_HANDLER_EX(IDM_SENDGROUP_MSG, OnMenu_SendGroupMessage)		//�Ҽ��˵�����Ⱥ��Ϣ
		COMMAND_ID_HANDLER_EX(IDM_VIEW_GROUPINFO, OnMenu_ViewGroupInfo)			//�Ҽ��˵��鿴Ⱥ����
		COMMAND_ID_HANDLER_EX(IDM_CREATENEWGROUP, OnMenu_CreateNewGroup)		//�Ҽ��˵�����Ⱥ
        COMMAND_ID_HANDLER_EX(IDM_EXIT_GROUP, OnMenu_ExitGroup)					//�Ҽ��˵��˳���Ⱥ

		//��ʾ�ǳƺ��˻�����ʾ�ǳơ���ʾ�˺š���ʾ��ˬ����
		COMMAND_RANGE_HANDLER_EX(ID_32911, ID_32914, OnMenu_ShowNameChoice)
        
        //�������
        MESSAGE_HANDLER_EX(FMG_MSG_NET_ERROR, OnNetError)

		MESSAGE_HANDLER_EX(FMG_MSG_LOGIN_RESULT, OnLoginResult)
		MESSAGE_HANDLER_EX(FMG_MSG_LOGOUT_RESULT, OnLogoutResult)

		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_INFO, OnUpdateBuddyInfo)		 //�����û���Ϣ
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_NUMBER, OnUpdateBuddyNumber)	 //�����û��ǳ�(TODO: ����Ҫ��)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_HEADPIC, OnUpdateBuddyHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_SIGN, OnUpdateBuddySign)
		MESSAGE_HANDLER_EX(FMG_MSG_SELF_STATUS_CHANGE, OnSelfStatusChange)

		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_CHATDLG_USERINFO, OnUpdateChatDlgOnlineStatus)			//�������촰�����û�״̬
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_LIST, OnUpdateBuddyList)		 //���º����б���Ϣ
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_LIST, OnUpdateGroupList)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_RECENT_LIST, OnUpdateRecentList)	 //���������ϵ���б�
		MESSAGE_HANDLER_EX(FMG_MSG_BUDDY_MSG, OnBuddyMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_GROUP_MSG, OnGroupMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_SESS_MSG, OnSessMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_STATUS_CHANGE_MSG, OnStatusChangeMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_KICK_MSG, OnKickMsg)
        MESSAGE_HANDLER_EX(FMG_MSG_SCREENSHOT, OnScreenshotMsg)
        
		MESSAGE_HANDLER_EX(FMG_MSG_SYS_GROUP_MSG, OnSysGroupMsg)
		
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_NUMBER, OnUpdateGMemberNumber)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_NUMBER, OnUpdateGroupNumber)
		
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_SIGN, OnUpdateGMemberSign)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_INFO, OnUpdateGMemberInfo)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_INFO, OnUpdateGroupInfo)				//����Ⱥ��Ϣ
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_HEADPIC, OnUpdateGMemberHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_HEADPIC, OnUpdateGroupHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_CHANGE_STATUS_RESULT, OnChangeStatusResult)
		MESSAGE_HANDLER_EX(FMG_MSG_ADDFREIND, OnSendAddFriendRequestResult)			//�Լ��Ӻ����Ƿ��ͳɹ�
		MESSAGE_HANDLER_EX(FMG_MSG_RECVADDFRIENDREQUSET, OnRecvAddFriendRequest)		//�յ��Ӻ������֪ͨ
		MESSAGE_HANDLER_EX(FMG_MSG_DELETEFRIEND, OnDeleteFriendResult)				//ɾ�����ѽ��

		MESSAGE_HANDLER_EX(WM_TRAYICON_NOTIFY, OnTrayIconNotify)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYCHATDLG, OnShowOrCloseDlg)				//��ʾ��ر����������Ի���
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_MAINDLG, OnCloseDlg)						//�ر����Ի���
		MESSAGE_HANDLER_EX(WM_DEL_MSG_SENDER, OnDelMsgSender)
		MESSAGE_HANDLER_EX(WM_CANCEL_FLASH, OnCancelFlash)

		REFLECT_NOTIFICATIONS()													//��Ϣ����֪ͨ��
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);
	void OnClose();
	void OnDestroy();
	void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_LockPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_Mute(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_UnlockPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_ShowAddFriendDlg(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_ShowSystemSettingDlg(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnBtn_OpenMail(UINT uNotifyCode, int nID, CWindow wndCtl);  
	void OnBtn_ModifyPassword(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_MainMenu(UINT uNotifyCode, int nId, CWindow wndCtl);
	void OnBtn_MultiChat(UINT uNotifyCode, int nId, CWindow wndCtl);
	LRESULT OnTabCtrlDropDown(LPNMHDR pnmh);
	LRESULT OnTabCtrlSelChange(LPNMHDR pnmh);
	LRESULT OnBuddyListDblClk(LPNMHDR pnmh);
	LRESULT OnBuddyListRButtonUp(LPNMHDR pnmh);
    LRESULT OnBuddyListHover(LPNMHDR pnmh);   
	LRESULT OnGroupListDblClk(LPNMHDR pnmh);
	LRESULT OnGroupListRButtonUp(LPNMHDR pnmh);
	LRESULT OnRecentListDblClk(LPNMHDR pnmh);
	LRESULT OnRecentListRButtonUp(LPNMHDR pnmh);
	
	void OnRefreshBuddyList(UINT uNotifyCode, int nID, CWindow wndCtl);			// ˢ����ϵ���б�
	void OnShowOnlineBuddyOnly(UINT uNotifyCode, int nID, CWindow wndCtl);		// ����ʾ���ߺ���
	void OnMainMenu_About(UINT uNotifyCode, int nID, CWindow wndCtl);			// ���˵����ڲ˵����
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);				// ��ȡ������ť(ȡ����¼)
	void OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl);			// ���û�ͷ�񡱿ؼ�
	void OnMenu_BigHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);			// ����ͷ�񡱲˵�
	void OnMenu_SmallHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);		// ��Сͷ�񡱲˵�
	void OnMenu_StdHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);			// ����׼ͷ�񡱲˵�
	void OnMenu_ShowBigHeadPicInSel(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ѡ��ʱ��ʾ��ͷ�񡱲˵�
	void OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl);	// ������ǩ�����༭�ı���
	void OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl);				// ���û�����״̬���˵�

	void OnBuddyListAddTeam(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBuddyListDeleteTeam(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBuddyListModifyTeamName(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMoveBuddyToTeam(UINT uNotifyCode, int nID, CWindow wndCtl);


	void OnMenu_SendBuddyMessage(UINT uNotifyCode, int nID, CWindow wndCtl);	//�Ҽ��˵����ͼ�ʱ��Ϣ
	void OnMenu_SendBuddyMessageFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);	//�����ϵ���Ҽ��˵����ͼ�ʱ��Ϣ
	void OnMenu_DeleteFriend(UINT uNotifyCode, int nID, CWindow wndCtl);		//�Ҽ��˵�ɾ������
	void OnClearAllRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDeleteRecentItem(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_ViewBuddyInfo(UINT uNotifyCode, int nID, CWindow wndCtl);		//�Ҽ��˵��鿴��������
	void OnMenu_ViewBuddyInfoFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnMenu_ModifyBuddyMarkName(UINT uNotifyCode, int nID, CWindow wndCtl);		//�Ҽ��˵��޸ĺ��ѱ�ע����

	void OnMenu_SendGroupMessage(UINT uNotifyCode, int nID, CWindow wndCtl);	
	void OnMenu_ViewGroupInfo(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_ExitGroup(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_CreateNewGroup(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnMenu_ShowNameChoice(UINT uNotifyCode, int nID, CWindow wndCtl);		//������ʾ�����Ӳ˵�

	void CloseDialog(int nVal);
	void OnFindBtn(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitTopToolBar();		// ��ʼ��Top������
	BOOL InitBottomToolBar();	// ��ʼ��Bottom������
	BOOL InitTabCtrl();			// ��ʼ��Tab��
	BOOL InitBuddyListCtrl();	// ��ʼ�������б�ؼ�
	BOOL InitGroupListCtrl();	// ��ʼ��Ⱥ�б�ؼ�
	BOOL InitRecentListCtrl();	// ��ʼ�������ϵ���б�ؼ�

	BOOL InitUI();
	void UninitUI();

    //����true,��ʾ��¼�Ի����ߵ���������¼����;����false,��ʾֱ�ӹر��˵�¼�Ի���
	bool StartLogin(BOOL bAutoLogin = FALSE);
	void CloseAllDlg();

    LRESULT OnNetError(UINT uMsg, WPARAM wParam, LPARAM lParam);    
	LRESULT OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ��¼������Ϣ
	LRESULT OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ע��������Ϣ
	LRESULT OnGoOffline(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ����
	LRESULT OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// �����û���Ϣ
	LRESULT OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º����б���Ϣ
	LRESULT OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����Ⱥ�б���Ϣ
	LRESULT OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���������ϵ���б���Ϣ
	LRESULT OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ������Ϣ
	LRESULT OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// Ⱥ��Ϣ
	LRESULT OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// ��ʱ�Ự��Ϣ
	LRESULT OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����״̬�ı���Ϣ
	LRESULT OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// ����������Ϣ
    LRESULT OnScreenshotMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ��Ļ��ͼ��Ϣ   
	LRESULT OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// Ⱥϵͳ��Ϣ
	LRESULT OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���º��Ѻ���
	LRESULT OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա����_
	LRESULT OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ����
	LRESULT OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º��Ѹ���ǩ��
	LRESULT OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա����ǩ��
	LRESULT OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º�����Ϣ
	LRESULT OnUpdateChatDlgOnlineStatus(UINT uMsg, WPARAM wParam, LPARAM lParam);	////�������촰���к��ѵ�����������״̬	
	LRESULT OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա��Ϣ
	LRESULT OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����Ⱥ��Ϣ
	LRESULT OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���º���ͷ��ͼƬ
	LRESULT OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);// ����Ⱥ��Աͷ��ͼƬ
	LRESULT OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥͷ��ͼƬ
	LRESULT OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	// �ı�����״̬������Ϣ
	LRESULT OnSendAddFriendRequestResult(UINT uMsg, WPARAM wParam, LPARAM lParam);//�Ӻ����������������ĳɰܽ��
	LRESULT OnRecvAddFriendRequest(UINT uMsg, WPARAM wParam, LPARAM lParam);// �յ��Ӻ��ѵ�����
	LRESULT OnDeleteFriendResult(UINT message, WPARAM wParam, LPARAM lParam);//ɾ�����ѽ��
	LRESULT OnSelfStatusChange(UINT message, WPARAM wParam, LPARAM lParam); //�Լ��������ն�������֪ͨ
	
	LRESULT OnCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam);


	void ShowBuddyChatDlg(UINT nUTalkUin, BOOL bShow, BOOL bShakeWindowMsg=FALSE, UINT nMsgID=1000);
	void ShowGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowSessChatDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow);
	void ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowBuddyInfoDlg(UINT nUTalkUin, BOOL bShow);
																	 
	void ShowUserInfoDlg(UINT nUTalkUin, BOOL bShow);						// ��ʾ���߹رո�����Ϣ�Ի���
	void ShowGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow);
	void ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow);

	void NotifyBuddyChatDlg(UINT nUTalkUin, UINT uMsg);						// ֪ͨ�������촰�ڸ���
	void NotifyGroupChatDlg(UINT nGroupCode,								// ֪ͨȺ���촰�ڸ���
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	void NotifySessChatDlg(UINT nUTalkUin, UINT uMsg);						// ֪ͨ��ʱ�Ự���촰�ڸ���
	void NotifyBuddyInfoDlg(UINT nUTalkUin, UINT uMsg);						// ֪ͨ������Ϣ���ڸ���
	void NotifyGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, UINT uMsg);	// ֪ͨȺ��Ա��Ϣ���ڸ���
	void NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg);					// ֪ͨȺ��Ϣ���ڸ���

	void UpdateBuddyTreeCtrl(UINT uAccountID=0);
	void BuddyListSortOnStaus();											//�������ߺ�����״̬���к����б�
	void UpdateGroupTreeCtrl();
	void UpdateRecentTreeCtrl();

	void OnTrayIcon_LButtunUp();
	void OnTrayIcon_RButtunUp();
	void OnTrayIcon_MouseHover();
	void OnTrayIcon_MouseLeave();

	BOOL LoadAppIcon(long nStatus);
	void DestroyAppIcon();
	BOOL LoadLoginIcon();
	void DestroyLoginIcon();
	BOOL LoadAddFriendIcon();
	void DestroyAddFriendIcon();
	void UpdateMsgIcon();

	CString GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin);
	void GetNumber(UINT nGroupCode, UINT nUTalkUin, UINT& nGroupNum, UINT& nUTalkNum);
	HICON ExtractIcon(LPCTSTR lpszFileName);

	void ShowPanel(BOOL bShow);												// ��ʾָ����庯��(bShow��TRUE��ʾ��ʾ����壬FALSE��ʾ��ʾ��¼���)
	void ShowLockPanel();

	long GetStatusFromMenuID(int nMenuID);									// �Ӳ˵�ID��ȡ��Ӧ��UTalk_STATUS
	void StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, long nStatus);	// ����ָ��״̬����״̬�˵���ť��ͼ��

	void CreateEssentialDirectories();										//��¼�ɹ��Ժ󴴽������ļ���

	void ShowAddFriendConfirmDlg();											//��ʾ�Ӻ�������Ի��� 

	BOOL InsertTeamMenuItem(CSkinMenu& popMenu);

	void SaveCurrentLogonUserToFile();										//����ǰ��¼���˻����浽�ļ���
	void DeleteCurrentUserFromFile();										//���ļ���ɾ����ǰ��¼���˻�

	BOOL IsFilesTransferring();												//����Ի����Ƿ����ļ����ڽ��д���
public:
	BOOL m_bSideState;

private:
	CFlamingoClient			m_FMGClient;
			
	CSkinDialog				m_SkinDlg;										//����ܶԻ���
	CLoginDlg				m_LoginDlg;
	CFindFriendDlg*			m_pFindFriendDlg;								//���Һ��ѶԻ���
	CMsgTipDlg				m_MsgTipDlg;
	CLogonUserInfoDlg       m_LogonUserInfoDlg;
	CModifyPasswordDlg		m_ModifyPasswordDlg;
	CMultiChatDlg			m_MultiChatDlg;									//Ⱥ����Ϣ����
    CRemoteDesktopDlg*      m_pRemoteDesktopDlg;                            //Զ�����洰��

	CSkinButton				m_btnMainMenu;
	CSkinMenu				m_SkinMenu;										//���½Ǵ�ͷ��Ĳ˵�
	CSkinButton				m_btnMultiChat;									//Ⱥ����ť
	CSkinButton				m_btnFind;										//���Һ��Ѱ�ť
	CSkinEdit				m_edtSign, m_edtSearch;
	CSkinPictureBox			m_picHead;
	
	BOOL					m_bPicHeadPress;
	CSkinStatic				m_staNickName;
	CSkinButton				m_btnSign;

	CSkinButton				m_btnMail;
	
	CSkinToolBar			m_tbTop,	m_tbBottom;
	CSkinTabCtrl			m_TabCtrl;		
	//CBuddyListCtrl			m_RecentListCtrl;
	CRecentListCtrl			m_RecentListCtrl;
	CBuddyListCtrl			m_BuddyListCtrl;
	CBuddyListCtrl			m_GroupListCtrl;

    CBuddyInfoFloatWnd      m_BuddyInfoFloatWnd;                //�����ں����б�������ʾ����ʾ����
	

	CSkinPictureBox			m_picLogining;
	CSkinStatic				m_staUTalkNum;
	CSkinStatic				m_staLogining;
	CSkinButton				m_btnCancel;
	CSkinButton				m_btnUnlock;									//������ť


	std::map<UINT, CBuddyChatDlg*> m_mapBuddyChatDlg;
	std::map<UINT, CGroupChatDlg*> m_mapGroupChatDlg;
 	std::map<UINT, CSessChatDlg*>	m_mapSessChatDlg;
 	std::map<UINT, CBuddyInfoDlg*> m_mapBuddyInfoDlg;
 	std::map<CGMemberInfoMapKey, CBuddyInfoDlg*> m_mapGMemberInfoDlg;
 	std::map<UINT, CGroupInfoDlg*> m_mapGroupInfoDlg;

	Gdiplus::GdiplusStartupInput	m_gdiplusStartupInput;
	ULONG_PTR						m_gdiplusToken;
	CFaceList						m_FaceList;
	CTrayIcon						m_TrayIcon;

	long							m_nLastMsgType;
	UINT							m_nLastSenderId;

	HICON							m_hAppIcon;
	HICON							m_hLoginIcon[6];
	int								m_nCurLoginIcon;
	HICON							m_hMsgIcon;
	HICON							m_hAddFriendIcon[2];
	DWORD							m_dwLoginTimerId;
	DWORD							m_dwMsgTimerId;
	DWORD							m_dwAddFriendTimerId;					//�Ӻ���֪ͨ
    DWORD                           m_dwReconnectTimerId;                   //����������ʱ��ID
	//DWORD							m_dwExitAppTimerId;						//�������˳���ʱ��

    bool                            m_bEnableReconnect;                     //�Ƿ��������
    UINT                            m_uReconnectInterval;                   //����ʱ�������������0����λ����

    bool                            m_bAlreadySendReloginRequest;           //�����������Ƿ��Ѿ����͹���¼�����

	HICON							m_hDlgIcon, m_hDlgSmallIcon;
	CLoginAccountList				m_LoginAccountList;
	LOGIN_ACCOUNT_INFO				m_stAccountInfo;						//��ǰ����Ҫ��¼���˻���Ϣ
	CCascadeWinManager				m_CascadeWinManager;
		
	long							m_nYOffset;
	long							m_bFold;
	long							m_nBuddyListHeadPicStyle;				//�����б���ͷ����
	BOOL							m_bShowBigHeadPicInSel;					//����Сͷ��ģʽʱ��ѡ����ʾ��ͷ��
	BOOL							m_bPanelLocked;							//����崦������״̬

	BOOL							m_bAlreadyLogin;				        //�Ƿ��Ѿ���¼��

	HRGN							m_hHotRgn;

	BOOL							m_bShowOnlineBuddyOnly;

	CRect							m_rcTrayIconRect;

	long							m_nCurSelIndexInMainTab;		//�����Tabѡ��������

    MAINPANEL_STATUS                m_nMainPanelStatus;             //�����״̬
};
