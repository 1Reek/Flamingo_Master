#pragma once

#include "resource.h"
#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "RichEditUtil.h"
#include "CascadeWinManager.h"
#include "FontSelDlg.h"
#include "FaceSelDlg.h"
#include "PicBarDlg.h"
#include "SplitterCtrl.h"


class CFlamingoClient;

//Ⱥ���Ի���
class CMultiChatDlg : public CDialogImpl<CMultiChatDlg>, public CMessageFilter
{
public:
	CMultiChatDlg(void);
	~CMultiChatDlg(void);

	enum { IDD = IDD_MULTICHATDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CMultiChatDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COPYDATA(OnCopyData)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_MOVE(OnMove)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_DROPFILES(OnDropFiles)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_ID_HANDLER_EX(201, OnBtn_Font)
		COMMAND_ID_HANDLER_EX(202, OnBtn_Face)
		COMMAND_ID_HANDLER_EX(203, OnBtn_ShakeWindow)
		COMMAND_ID_HANDLER_EX(204, OnBtn_Image)
		COMMAND_ID_HANDLER_EX(205, OnBtn_ScreenShot)
		COMMAND_ID_HANDLER_EX(302, OnBtn_SaveAs)

		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
		COMMAND_ID_HANDLER_EX(ID_BTN_SEND, OnBtn_Send)
		COMMAND_HANDLER_EX(ID_BTN_ARROW, BN_CLICKED, OnBtn_Arrow)

		COMMAND_ID_HANDLER_EX(ID_32901, OnPressEnterMenuItem)					//�س���������Ϣ
		COMMAND_ID_HANDLER_EX(ID_32902, OnPressCtrlEnterMenuItem)				//Ctrl+�س�������Ϣ


		NOTIFY_HANDLER_EX(ID_TOOLBAR_TOP, TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER_EX(ID_TOOLBAR_MID, TBN_DROPDOWN, OnToolbarDropDown)

		MESSAGE_HANDLER_EX(WM_UPDATE_FONTINFO, OnUpdateFontInfo)
		MESSAGE_HANDLER_EX(FACE_CTRL_SEL, OnFaceCtrlSel)
		MESSAGE_HANDLER_EX(WM_SETDLGINITFOCUS, OnSetDlgInitFocus)

		NOTIFY_HANDLER_EX(ID_RICHEDIT_RECV, EN_LINK, OnRichEdit_Recv_Link)
		NOTIFY_HANDLER_EX(ID_RICHEDIT_SEND, EN_PASTE, OnRichEdit_Send_Paste)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_DBLCLK, OnGMemberList_DblClick)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_RCLICK, OnGMemberList_RClick)
		COMMAND_ID_HANDLER_EX(ID_MENU_CUT, OnMenu_Cut)
		COMMAND_ID_HANDLER_EX(ID_MENU_COPY, OnMenu_Copy)
		COMMAND_ID_HANDLER_EX(ID_MENU_PASTE, OnMenu_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_SELALL, OnMenu_SelAll)
		COMMAND_ID_HANDLER_EX(ID_MENU_CLEAR, OnMenu_Clear)
		COMMAND_RANGE_HANDLER_EX(ID_MENU_ZOOMRATIO_400, ID_MENU_ZOOMRATIO_50, OnMenu_ZoomRatio)
		COMMAND_ID_HANDLER_EX(ID_MENU_SAVEAS, OnMenu_SaveAs)
		COMMAND_ID_HANDLER_EX(ID_MENU_VIEW_INFO, OnMenu_ViewInfo)
		COMMAND_ID_HANDLER_EX(ID_MENU_SEND_MSG, OnMenu_SendMsg)

		MESSAGE_HANDLER_EX(FMG_MSG_SENDCHATMSG_RESULT, OnSendChatMsgResult)	

		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_PROGRESS, OnSendFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_RESULT, OnSendFileResult)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_PROGRESS, OnRecvFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_RESULT, OnRecvFileResult)
	END_MSG_MAP()

public:
	void OnUpdateGroupInfo();									// ����Ⱥ��Ϣ
	void OnUpdateGroupNumber();									// ����Ⱥ����
	void OnUpdateGroupHeadPic();								// ����Ⱥͷ��
	void OnUpdateGMemberNumber(WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա����
	void OnUpdateGMemberHeadPic(WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Աͷ��

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnMove(CPoint ptPos);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	void OnDropFiles(HDROP hDropInfo);
	void OnClose();
	void OnDestroy();

	void OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl);		// ������ѡ�񹤾�������ť
	void OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����顱��ť
	void OnBtn_ShakeWindow(UINT uNotifyCode, int nID, CWindow wndCtl);//�����ڶ�������ť
	void OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl);	// ������ͼƬ����ť
	void OnBtn_MsgNotPrompt(UINT uNotifyCode, int nID, CWindow wndCtl);	// ������Ϣ����ʾ����ť
	void OnBtn_ScreenShot(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ͼ����
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
	LRESULT OnGMemberList_DblClick(LPNMHDR pnmh);					// ��Ⱥ��Ա���б�˫����Ϣ
	LRESULT OnGMemberList_RClick(LPNMHDR pnmh);						// ��Ⱥ��Ա���б��Ҽ�������Ϣ
	void OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����С��˵�
	void OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����ơ��˵�
	void OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ճ�����˵�
	void OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ȫ��ѡ�񡱲˵�
	void OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���������˵�
	void OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl);// ����ʾ�������˵�
	void OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����Ϊ���˵�
	void OnMenu_ViewInfo(UINT uNotifyCode, int nID, CWindow wndCtl);// ���鿴���ϡ��˵�
	void OnMenu_SendMsg(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��������Ϣ���˵�
	void OnMenu_DeleteSelectMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);//��ɾ��ѡ����Ϣ��¼���˵�

	void OnPressEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);		//�س���������Ϣ
	void OnPressCtrlEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);	//Ctrl+�س�������Ϣ

	LRESULT OnSendChatMsgResult(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRecvFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRecvFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	BOOL OnRichEdit_MouseMove(MSG* pMsg);		// ����/�����ı��������ƶ���Ϣ
	BOOL OnRichEdit_LBtnDblClk(MSG* pMsg);		// ����/�����ı�������˫����Ϣ
	BOOL OnRichEdit_RBtnDown(MSG* pMsg);		// ����/�����ı��������Ҽ�������Ϣ

	void UpdateDlgTitle();				// ���¶Ի��������
	BOOL UpdateGroupNameCtrl();			// ����Ⱥ���ƿؼ�
	BOOL UpdateGroupMemberList();		// ����Ⱥ��Ա�б�

	BOOL InitTopToolBar();				// ��ʼ��Top������
	BOOL InitMidToolBar();				// ��ʼ��Middle������
	BOOL InitRichEditOleCallback();		// ��ʼ��IRichEditOleCallback�ӿ�

	BOOL Init();						// ��ʼ��
	BOOL UnInit();						// ����ʼ��

	void CalculateMsgLogCountAndOffset();

	void SetHotRgn();

	BOOL HandleFileDragResult(PCTSTR lpszFileName);

	int FindMemberListByUin(UINT nUTalkUin);
	void GetSenderInfo(UINT nUTalkUin, CString& strName, tstring& strAccount);
	void _RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText);
	BOOL _RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex);
	BOOL HandleFontInfo(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	void AddMsg(HWND hRichEditWnd, LPCTSTR lpText);
	void AddMsgToSendEdit(LPCTSTR lpText);
	void AddMsgToRecvEdit(time_t nTime, LPCTSTR lpText);
	void AddMsgToRecvEdit(CBuddyMessage* lpGroupMsg);
	void AddMsgToRecvEdit(std::vector<CContent*>& arrContent);
	void AddMsgToRecvEdit(std::vector<GROUP_MSG_LOG*>& arrMsgLog);

	void AnalyseContent(tstring& strContent, HWND hRichWnd=NULL);
	BOOL HandleSysFaceId(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleCustomPic(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);


	void ResizeImageInRecvRichEdit();
	BOOL GetImageDisplaySizeInRichEdit(PCTSTR pszFileName, HWND hWnd, long& nWidth, long& nHeight);	//�����ڳߴ�������ͼƬ��С

    void SendConfirmMessage(const CUploadFileResult* pUploadFileResult);

	void RecordWindowSize();
	void ReCaculateCtrlPostion(long nMouseY);

public:
	CFlamingoClient*		m_lpFMGClient;
	CFaceList*				m_lpFaceList;
	HWND					m_hMainDlg;
	std::set<UINT>			m_setTargetIDs;

private:
	CSkinDialog				m_SkinDlg;
	CSkinPictureBox			m_picHead;
	CSkinHyperLink			m_lnkGroupName;
	CSkinStatic				m_staGroupCategory;
	CSkinStatic				m_staMemberTitle;
	CSkinButton				m_btnClose, m_btnSend, m_btnArrow;
	CSkinMenu				m_SkinMenu;
	CSkinToolBar			m_tbTop, m_tbMid;
	CFontSelDlg				m_FontSelDlg;
	CFaceSelDlg				m_FaceSelDlg;
	CPicBarDlg				m_PicBarDlg;
	CSkinRichEdit			m_richRecv, m_richSend;
	CSkinListCtrl			m_ListCtrl;

	CSplitterCtrl			m_SplitterCtrl;
	
	CAccelerator			m_Accelerator;
	HICON					m_hDlgIcon, m_hDlgSmallIcon;
	HWND					m_hRBtnDownWnd;
	POINT					m_ptRBtnDown;
	IImageOle*				m_pLastImageOle;
	int						m_cxPicBarDlg, m_cyPicBarDlg;

	CString					m_strCurLink;

	UINT					m_nGroupId, m_nGroupNumber;
	CString					m_strAccount;
	CString					m_strGroupName;
	CString					m_strUserName;
	int						m_nMemberCnt, m_nOnlineMemberCnt;

	HRGN					m_HotRgn;

	std::map<CString, long>	m_mapSendFileInfo;		//�����ļ����У������ļ����������ֵΪ-1�����ļ��������ֵ����0����ͼƬ�ڴ���λ�õ�����
	std::map<CString, long>	m_mapRecvFileInfo;		//�����ļ����У������ļ���ʾ������ֵΪ-1����ȡ�������ļ�����ֵ����1����ȡ������ͼƬӦ�����λ������

	BOOL					m_bPressEnterToSendMessage;	//TRUE���س���������Ϣ, FALSE��Ctrl+Enter������Ϣ

	BOOL					m_bDraged;					//�û��Ƿ���ק�������ı���ĳߴ�
	RECT					m_rtRichRecv;
	RECT					m_rtMidToolBar;
	RECT					m_rtSplitter;
	RECT					m_rtRichSend;
};
