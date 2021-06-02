#pragma once

#include "resource.h"
#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "FlamingoClient.h"
#include "RichEditUtil.h"
#include "CascadeWinManager.h"
#include "FontSelDlg.h"
#include "FaceSelDlg.h"
#include "PicBarDlg.h"

class CSessChatDlg : public CDialogImpl<CSessChatDlg>, public CMessageFilter
{
public:
	CSessChatDlg(void);
	~CSessChatDlg(void);

	enum { IDD = IDD_BUDDYCHATDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CSessChatDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COPYDATA(OnCopyData)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_MOVE(OnMove)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER_EX(ID_LINK_BUDDYNAME, BN_CLICKED, OnLnk_BuddyName)
		COMMAND_ID_HANDLER_EX(201, OnBtn_Font)
		COMMAND_ID_HANDLER_EX(202, OnBtn_Face)
		COMMAND_ID_HANDLER_EX(208, OnBtn_Image)
		COMMAND_ID_HANDLER_EX(214, OnBtn_MsgLog)
		COMMAND_ID_HANDLER_EX(302, OnBtn_SaveAs)

		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
		COMMAND_ID_HANDLER_EX(ID_BTN_SEND, OnBtn_Send)
		COMMAND_HANDLER_EX(ID_BTN_ARROW, BN_CLICKED, OnBtn_Arrow)

		NOTIFY_HANDLER_EX(ID_TOOLBAR_TOP, TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER_EX(ID_TOOLBAR_MID, TBN_DROPDOWN, OnToolbarDropDown)

		MESSAGE_HANDLER_EX(WM_UPDATE_FONTINFO, OnUpdateFontInfo)
		MESSAGE_HANDLER_EX(FACE_CTRL_SEL, OnFaceCtrlSel)
		MESSAGE_HANDLER_EX(WM_SETDLGINITFOCUS, OnSetDlgInitFocus)

		NOTIFY_HANDLER_EX(ID_RICHEDIT_RECV, EN_LINK, OnRichEdit_Recv_Link)
		NOTIFY_HANDLER_EX(ID_RICHEDIT_SEND, EN_PASTE, OnRichEdit_Send_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_CUT, OnMenu_Cut)
		COMMAND_ID_HANDLER_EX(ID_MENU_COPY, OnMenu_Copy)
		COMMAND_ID_HANDLER_EX(ID_MENU_PASTE, OnMenu_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_SELALL, OnMenu_SelAll)
		COMMAND_ID_HANDLER_EX(ID_MENU_CLEAR, OnMenu_Clear)
		COMMAND_RANGE_HANDLER_EX(ID_MENU_ZOOMRATIO_400, ID_MENU_ZOOMRATIO_50, OnMenu_ZoomRatio)
		COMMAND_ID_HANDLER_EX(ID_MENU_SAVEAS, OnMenu_SaveAs)
	END_MSG_MAP()

public:
	void OnRecvMsg(UINT nUTalkUin, UINT nMsgId);
	void OnUpdateGMemberNumber();				// ����Ⱥ��Ա����
	void OnUpdateGMemberHeadPic();				// ����Ⱥ��Աͷ��

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnMove(CPoint ptPos);
	void OnSize(UINT nType, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	void OnClose();
	void OnDestroy();

	void OnLnk_BuddyName(UINT uNotifyCode, int nID, CWindow wndCtl);// ���������ơ������ӿؼ�
	void OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl);		// ������ѡ�񹤾�������ť
	void OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����顱��ť
	void OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl);	// ������ͼƬ����ť
	void OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);	// ����Ϣ��¼����ť
	void OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��������Ϊ����ť

	void OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���رա���ť
	void OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����͡���ť
	void OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl);	// ����ͷ����ť

	LRESULT OnToolbarDropDown(LPNMHDR pnmh);

	LRESULT OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����������Ϣ
	LRESULT OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam);		// �����顱�ؼ�ѡȡ��Ϣ
	LRESULT OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���öԻ����ʼ����

	LRESULT OnRichEdit_Recv_Link(LPNMHDR pnmh);						//	��������Ϣ�����ı������ӵ����Ϣ
	LRESULT OnRichEdit_Send_Paste(LPNMHDR pnmh);
	void OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����С��˵�
	void OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����ơ��˵�
	void OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ճ�����˵�
	void OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ȫ��ѡ�񡱲˵�
	void OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���������˵�
	void OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl);// ����ʾ�������˵�
	void OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����Ϊ���˵�

	BOOL OnRichEdit_MouseMove(MSG* pMsg);		// ����/�����ı��������ƶ���Ϣ
	BOOL OnRichEdit_LBtnDblClk(MSG* pMsg);		// ����/�����ı�������˫����Ϣ
	BOOL OnRichEdit_RBtnDown(MSG* pMsg);		// ����/�����ı��������Ҽ�������Ϣ

	CGroupInfo* GetGroupInfoPtr();		// ��ȡȺ��Ϣָ��
	CBuddyInfo* GetBuddyInfoPtr();		// ��ȡ������Ϣָ��
	CBuddyInfo* GetUserInfoPtr();		// ��ȡ�û���Ϣָ��

	void UpdateData();					// ��������
	void UpdateDlgTitle();				// ���¶Ի��������
	void UpdateBuddyNameCtrl();			// ���º������ƿؼ�
	void UpdateGroupNameCtrl();			// ����Ⱥ���ƿؼ�
	void CalcTitleBarRect();
	BOOL InitTopToolBar();				// ��ʼ��Top������
	BOOL InitMidToolBar();				// ��ʼ��Middle������
	BOOL InitRichEditOleCallback();		// ��ʼ��IRichEditOleCallback�ӿ�


	BOOL Init();						// ��ʼ��
	BOOL UnInit();						// ����ʼ��

	void AddMsg(CSessMessage* lpSessMsg);
	void _RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText);
	BOOL _RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex);
	BOOL HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	void AddMsg(HWND hRichEditWnd, LPCTSTR lpText);
	void AddMsgToSendEdit(LPCTSTR lpText);
	void AddMsgToRecvEdit(time_t nTime, LPCTSTR lpText);
	void AddMsgToRecvEdit(CSessMessage* lpSessMsg);

	void OpenMsgLogBrowser();			// ����Ϣ��¼�������
	void CloseMsgLogBrowser();			// �ر���Ϣ��¼�������

public:
	CFlamingoClient* m_lpFMGClient;
	CFaceList* m_lpFaceList;
	CCascadeWinManager* m_lpCascadeWinManager;
	HWND m_hMainDlg;
	UINT m_nGroupCode;
	UINT m_nUTalkUin;

private:
	CSkinDialog m_SkinDlg;
	CSkinPictureBox m_picHead;
	CSkinHyperLink m_lnkBuddyName;
	CSkinStatic m_staGroupName;
	//CSkinPictureBox m_picAD_1, m_picAD_2, m_picAD_3;
	CSkinButton m_btnClose, m_btnSend, m_btnArrow;
	CSkinMenu m_SkinMenu;
	CSkinToolBar m_tbTop, m_tbMid;
	CFaceSelDlg m_FaceSelDlg;
	CFontSelDlg m_FontSelDlg;
	CPicBarDlg m_PicBarDlg;
	CSkinRichEdit m_richRecv, m_richSend;

	CAccelerator m_Accelerator;
	HICON m_hDlgIcon, m_hDlgSmallIcon;
	HFONT m_fontBuddyNameLink;
	HWND m_hRBtnDownWnd;
	POINT m_ptRBtnDown;
	IImageOle* m_pLastImageOle;
	int m_cxPicBarDlg, m_cyPicBarDlg;
	CRect m_rcTitleBar;
	BOOL m_bHasMinBtn, m_bHasMaxBtn, m_bHasCloseBtn;
	CRect m_rcMinBtn, m_rcMaxBtn, m_rcCloseBtn;
	UINT m_nGroupId;
	UINT m_nUTalkNumber;
	UINT m_nUserNumber;
	CString m_strBuddyName;
	CString m_strGroupName;
	CString m_strUserName;

	DWORD m_dwThreadId;			// MsgLogBrowser.exe�����߳�ID
};
