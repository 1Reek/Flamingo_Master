/** 
 * �������촰��, BuddyChatDlg.h
 * zhangyl 2017.05.26
 */
#pragma once

#include "resource.h"
#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "RichEditUtil.h"
#include "CascadeWinManager.h"
#include "FontSelDlg.h"
#include "FaceSelDlg.h"
#include "PicBarDlg.h"
#include "FileTransferCtrl.h"
#include "SplitterCtrl.h"
#include "ChatTabMgr.h"


class CFlamingoClient;
class CBuddyChatDlg : public CDialogImpl<CBuddyChatDlg>, public CMessageFilter
{
public:
	CBuddyChatDlg(void);
	~CBuddyChatDlg(void);

	enum { IDD = IDD_BUDDYCHATDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CBuddyChatDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COPYDATA(OnCopyData)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOVE(OnMove)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_DROPFILES(OnDropFiles)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER_EX(ID_PIC_HEAD, BN_CLICKED, OnLnk_BuddyName)			//���ͷ��ť
		COMMAND_HANDLER_EX(ID_LINK_BUDDYNAME, BN_CLICKED, OnLnk_BuddyName)
        COMMAND_ID_HANDLER_EX(104, OnBtn_RemoteDesktop)
		COMMAND_ID_HANDLER_EX(201, OnBtn_Font)
		COMMAND_ID_HANDLER_EX(202, OnBtn_Face)
		COMMAND_ID_HANDLER_EX(204, OnShakeWindow)								//���ڶ���
		COMMAND_ID_HANDLER_EX(208, OnBtn_Image)
		COMMAND_ID_HANDLER_EX(211, OnBtn_ScreenShot)							//��ͼ����
		COMMAND_ID_HANDLER_EX(214, OnBtn_MsgLog)								//��Ϣ��¼
		COMMAND_ID_HANDLER_EX(302, OnBtn_SaveAs)

		COMMAND_RANGE_HANDLER_EX(IDC_FIRSTMSGLOG, IDC_LASTMSGLOG, OnMsgLogPage) //��Ϣ��¼��ǰһ������һ������һ�������һ���ĸ���ť
		
		COMMAND_ID_HANDLER_EX(ID_32852, OnPressEnterMenuItem)					//�س���������Ϣ
		COMMAND_ID_HANDLER_EX(ID_32853, OnPressCtrlEnterMenuItem)				//Ctrl+�س�������Ϣ
		COMMAND_ID_HANDLER_EX(IDM_AUTOREPLY, OnAutoReply);

		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
		COMMAND_ID_HANDLER_EX(ID_BTN_SEND, OnBtn_Send)
		COMMAND_HANDLER_EX(ID_BTN_ARROW, BN_CLICKED, OnBtn_Arrow)

		COMMAND_ID_HANDLER_EX(IDM_OPENFILE, OnOpenTransferFileItem)
		COMMAND_ID_HANDLER_EX(IDM_OPENDIRECTORY, OnOpenTransferFileItem)

		NOTIFY_HANDLER_EX(ID_TOOLBAR_TOP, TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER_EX(ID_TOOLBAR_MID, TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER_EX(ID_TABCTRL_CHAT, TCN_DROPDOWN, OnTabCtrlDropDown)
		NOTIFY_HANDLER_EX(ID_TABCTRL_CHAT, TCN_SELCHANGE, OnTabCtrlSelChange)
		NOTIFY_HANDLER_EX(ID_CHAT_TAB_MGR, NM_CLICK, OnClickTabMgr)
		NOTIFY_HANDLER_EX(ID_FILE_TRANSFER, NM_CLICK, OnBtn_FileTransfer)

		MESSAGE_HANDLER_EX(WM_UPDATE_FONTINFO, OnUpdateFontInfo)
		MESSAGE_HANDLER_EX(FACE_CTRL_SEL, OnFaceCtrlSel)
		MESSAGE_HANDLER_EX(WM_SETDLGINITFOCUS, OnSetDlgInitFocus)

		MESSAGE_HANDLER_EX(FMG_MSG_SENDCHATMSG_RESULT, OnSendChatMsgResult)	//�����ѷ���Ϣ�ķ������
		//MESSAGE_HANDLER_EX(FMG_MSG_RECVFILE_REQUEST, OnRequestRecvFile)		//��������ļ�
		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_PROGRESS, OnSendFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_RESULT, OnSendFileResult)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_PROGRESS, OnRecvFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_RESULT, OnRecvFileResult)
		

		NOTIFY_HANDLER_EX(ID_RICHEDIT_RECV, EN_LINK, OnRichEdit_Recv_Link)
		NOTIFY_HANDLER_EX(ID_RICHEDIT_SEND, EN_PASTE, OnRichEdit_Send_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_CUT, OnMenu_Cut)
		COMMAND_ID_HANDLER_EX(ID_MENU_COPY, OnMenu_Copy)
		COMMAND_ID_HANDLER_EX(ID_MENU_PASTE, OnMenu_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_SELALL, OnMenu_SelAll)
		COMMAND_ID_HANDLER_EX(ID_MENU_CLEAR, OnMenu_Clear)
		COMMAND_RANGE_HANDLER_EX(ID_MENU_ZOOMRATIO_400, ID_MENU_ZOOMRATIO_50, OnMenu_ZoomRatio)
		COMMAND_ID_HANDLER_EX(ID_MENU_SAVEAS, OnMenu_SaveAs)
		COMMAND_ID_HANDLER_EX(IDM_EXPORT_MSGLOG, OnMenu_ExportMsgLog)			//������Ϣ��¼
		COMMAND_ID_HANDLER_EX(IDM_FINDINMSGLOG, OnMenu_FindInMsgLog)			//������Ϣ��¼
		COMMAND_ID_HANDLER_EX(IDM_CLEARMSGLOG, OnMenu_ClearMsgLog)				//�����Ϣ��¼
		COMMAND_ID_HANDLER_EX(IDM_DELETESELECTMSGLOG, OnMenu_DeleteSelectMsgLog)//ɾ��ѡ����Ϣ��¼
		COMMAND_ID_HANDLER_EX(IDM_SENDFILE, OnMenu_SendFile)					//�����ļ�
		COMMAND_ID_HANDLER_EX(IDM_SENDOFFLINEFILE, OnMenu_SendOfflineFile)		//���������ļ�
		COMMAND_ID_HANDLER_EX(IDM_SENDDIRECTORY, OnMenu_SendDirectory)			//�����ļ���
		COMMAND_ID_HANDLER_EX(IDM_SENDFILESETTINGS, OnMenu_SendFileSettings)	//�����ļ�����


	END_MSG_MAP()

public:
	void OnRecvMsg(UINT nUTalkUin, UINT nMsgId);								// ���պ�����Ϣ
	void OnUpdateBuddyNumber();													// ���º��Ѻ���֪ͨ
	void OnUpdateBuddySign();													// ���º���ǩ��֪ͨ
	void OnUpdateBuddyHeadPic();												// ���º���ͷ��֪ͨ

	BOOL IsFilesTransferring();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMove(CPoint ptPos);
	void OnSize(UINT nType, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	void OnDropFiles(HDROP hDropInfo);
	void OnClose();
	void OnDestroy();

	void OnLnk_BuddyName(UINT uNotifyCode, int nID, CWindow wndCtl);			// ���������ơ������ӿؼ�
    void OnBtn_RemoteDesktop(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl);					// ������ѡ�񹤾�������ť
	void OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl);					// �����顱��ť
	void OnShakeWindow(UINT uNotifyCode, int nID, CWindow wndCtl);				// �����ڶ�������ť
	void OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl);				// ������ͼƬ����ť
	void OnBtn_ScreenShot(UINT uNotifyCode, int nID, CWindow wndCtl);			// ��ͼ����
	void OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);				// ����Ϣ��¼����ť
	void OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);				// ��������Ϊ����ť

	void OnMsgLogPage(UINT uNotifyCode, int nID, CWindow wndCtl);				// ��Ϣ��¼��ҳ��ť

	void OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl);				// ���رա���ť
	void OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl);					// �����͡���ť
	void OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl);				// ����ͷ����ť

	void OnOpenTransferFileItem(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����ļ��ɹ��Ժ�ġ��򿪡��롰���ļ��С��˵���
	
	void OnPressEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);		//�س���������Ϣ
	void OnPressCtrlEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);	//Ctrl+�س�������Ϣ
	void OnAutoReply(UINT uNotifyCode, int nID, CWindow wndCtl);				// �Զ��ظ��˵�
	
	LRESULT OnToolbarDropDown(LPNMHDR pnmh);

	LRESULT OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ����������Ϣ
	LRESULT OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam);				// �����顱�ؼ�ѡȡ��Ϣ
	LRESULT OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ���öԻ����ʼ����

	LRESULT OnSendChatMsgResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRecvFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRecvFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTabCtrlDropDown(LPNMHDR pnmh);
	LRESULT OnTabCtrlSelChange(LPNMHDR pnmh);
	LRESULT OnClickTabMgr(LPNMHDR pnmh);
	LRESULT OnBtn_FileTransfer(LPNMHDR pnmh);
 	LRESULT OnRichEdit_Recv_Link(LPNMHDR pnmh);						//	��������Ϣ�����ı������ӵ����Ϣ
	LRESULT OnRichEdit_Send_Paste(LPNMHDR pnmh);
 	void OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl);		// �����С��˵�
 	void OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����ơ��˵�
 	void OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ճ�����˵�
 	void OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ȫ��ѡ�񡱲˵�
 	void OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���������˵�
	void OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl);// ����ʾ�������˵�
	void OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// �����Ϊ���˵�
	void OnMenu_ExportMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);//��������Ϣ��¼���˵�
	void OnMenu_FindInMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl); //��������Ϣ��¼���˵�
	void OnMenu_DeleteSelectMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);//��ɾ��ѡ����Ϣ��¼���˵�
	void OnMenu_ClearMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl); //�������Ϣ��¼���˵�
	
	void OnMenu_SendFile(UINT uNotifyCode, int nID, CWindow wndCtl); //�������ļ����˵�
	void OnMenu_SendOfflineFile(UINT uNotifyCode, int nID, CWindow wndCtl); //�����������ļ����˵�
	void OnMenu_SendDirectory(UINT uNotifyCode, int nID, CWindow wndCtl); //�������ļ��С��˵�
	void OnMenu_SendFileSettings(UINT uNotifyCode, int nID, CWindow wndCtl); //�������ļ����á��˵�



	BOOL OnRichEdit_MouseMove(MSG* pMsg);		// ����/�����ı��������ƶ���Ϣ
	BOOL OnRichEdit_LBtnDblClk(MSG* pMsg);		// ����/�����ı�������˫����Ϣ
	BOOL OnRichEdit_RBtnDown(MSG* pMsg);		// ����/�����ı��������Ҽ�������Ϣ

	CBuddyInfo* GetBuddyInfoPtr();		// ��ȡ������Ϣָ��
	CBuddyInfo* GetUserInfoPtr();		// ��ȡ�û���Ϣָ��

	void UpdateData();					// ��������
	void UpdateDlgTitle();				// ���¶Ի��������
	void UpdateBuddyNameCtrl();			// ���º������ƿؼ�
	void UpdateBuddySignCtrl();			// ���º���ǩ���ؼ�

	BOOL InitTopToolBar();				// ��ʼ��Top������
	BOOL InitMidToolBar();				// ��ʼ��Middle������
	BOOL InitRightTabWindow();			// ��ʼ���ұ߰�����Ϣ��¼�ͷ����ļ����ܵĴ���
	BOOL InitRichEditOleCallback();		// ��ʼ��IRichEditOleCallback�ӿ�

	void ShowMsgLogButtons(BOOL bShow);

	void InitFileTransferCtrl();		// ��ʼ���ļ�����ϵ�пؼ�
	void ShowFileTransferCtrl(BOOL bShow);//��ʾ�������ļ�����ؼ�
	void DestroyFileTransferCtrl();		// �����ļ�����ϵ�пؼ�

	BOOL Init();						// ��ʼ��
	BOOL UnInit();						// ����ʼ��
	//void CalcTitleBarRect();
	void SetHotRgn();					//�����ȵ�����

	void _RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText);
	BOOL _RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex);
	BOOL HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	void AddMsg(HWND hRichEditWnd, LPCTSTR lpText);
	void AddMsgToSendEdit(LPCTSTR lpText);
	void AddMsgToRecvEdit(time_t nTime, LPCTSTR lpText);
	void AddMsgToRecvEdit(CBuddyMessage* lpBuddyMsg);
	void AddMsgToMsgLogEdit(std::vector<CContent*>& arrContent);
	void AddMsgToMsgLogEdit(std::vector<BUDDY_MSG_LOG*>& arrMsgLog);
	void AddMsgToRecvEdit(std::vector<BUDDY_MSG_LOG*>& arrMsgLog);
	void AddMsgToRecvEdit(std::vector<CContent*>& arrContent);

	void InsertAutoReplyContent();

	void OpenMsgLogBrowser();			// ����Ϣ��¼�������
	void CloseMsgLogBrowser();			// �ر���Ϣ��¼�������
	void CalculateMsgLogCountAndOffset();

	BOOL GetImageDisplaySizeInRichEdit(PCTSTR pszFileName, HWND hWnd, long& nWidth, long& nHeight);	//��ȡ���ı��ؼ���ͼƬ����ʾ�ĳߴ�
	void ResizeImageInRecvRichEdit();

	void DisplayFileTransfer(BOOL bShow);
	void AnalyseContent(tstring& strContent, HWND hRichWnd=NULL);
	BOOL HandleShake(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleText(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleFontInfo(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleSysFaceId(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleCustomPic(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	BOOL HandleFile(LPCTSTR& p, tstring& strText, std::vector<CContent*>& arrContent);
	
	BOOL HandleFileDragResult(PCTSTR lpszFileName);
	BOOL SendOfflineFile(PCTSTR pszFileName);
	BOOL RecvOfflineFile(PCTSTR lpszDownloadName, PCTSTR pszFileName, long nFileSize);
	BOOL ShowRequestRecvFile(BOOL bOnlineFile, PCTSTR lpszDownloadName, PCTSTR pszFileName, long nFileSize);	//�����ļ�����

    void SendConfirmMessage(const CUploadFileResult* pUploadFileResult);

	void ShowLastMsgInRecvRichEdit();

	void RecordWindowSize();
	void ReCaculateCtrlPostion(long nMouseY);

    static UINT __stdcall RemoteDesktopProc(void* p);

public:
	CFlamingoClient*	m_lpFMGClient;
	CFaceList*			m_lpFaceList;
	CCascadeWinManager* m_lpCascadeWinManager;
	HWND				m_hMainDlg;
	int				    m_nUTalkUin;                
    int                 m_UserId;                   //��ǰ�������id
    int                 m_LoginUserId;              //��¼�û�id

private:
	CSkinDialog			m_SkinDlg;							
	CSkinPictureBox		m_picHead;
	CSkinHyperLink		m_lnkBuddyName;
	CSkinStatic			m_staBuddySign;
	//CSkinPictureBox	m_picAD_1, m_picAD_2, m_picAD_3;
	CSkinButton			m_btnMsgLog, m_btnClose, m_btnSend, m_btnArrow;
	CSkinButton			m_btnFirstMsgLog, m_btnPrevMsgLog, m_btnNextMsgLog, m_btnLastMsgLog;
	CSkinStatic			m_staMsgLogPage;
	CSkinMenu			m_SkinMenu;
	CSkinToolBar		m_tbTop, m_tbMid;
	CFaceSelDlg			m_FaceSelDlg;
	CFontSelDlg			m_FontSelDlg;
	CPicBarDlg			m_PicBarDlg;
	CSkinRichEdit		m_richRecv, m_richSend;
	CChatTabMgr			m_TabMgr;
	CSkinTabCtrl		m_RightTabCtrl;				//���촰���ұߵ�Tab����
	CSkinRichEdit		m_richMsgLog;				//��Ϣ��¼���ı��ؼ�
	CSplitterCtrl		m_SplitterCtrl;

	CSkinStatic			m_staPicUploadProgress;		//ͼƬ�ϴ����ȿؼ�

	//�ļ����䡰���ڡ��ؼ�
	CSkinPictureBox		m_SendFileThumbPicture;		//�ļ�ͼ��
	CSkinStatic			m_staSendFileDesc;			//�ļ��������ļ���+��С��
	CProgressBarCtrl	m_ProgressSendFile;			//�ļ�������
	CSkinHyperLink		m_lnkSendOffline;			//ת���߷��Ͱ�ť
	CSkinHyperLink		m_lnkSendFileCancel;		//ȡ�����Ͱ�ť

	CFileTransferCtrl	m_FileTransferCtrl;

	CRect				m_rcTitleBar;
	BOOL				m_bHasMinBtn, m_bHasMaxBtn, m_bHasCloseBtn;
	CRect				m_rcMinBtn, m_rcMaxBtn, m_rcCloseBtn;
	
	CAccelerator		m_Accelerator;
	BOOL				m_bPressEnterToSendMessage;	//TRUE���س���������Ϣ, FALSE��Ctrl+Enter������Ϣ
	BOOL				m_bMsgLogWindowVisible;		//�����¼���ڵ�ǰ�Ƿ�ɼ�
	BOOL				m_bFileTransferVisible;

	HICON				m_hDlgIcon, m_hDlgSmallIcon;
	HWND				m_hRBtnDownWnd;
	POINT				m_ptRBtnDown;
	IImageOle*			m_pLastImageOle;
	int					m_cxPicBarDlg, m_cyPicBarDlg;
	int					m_nMsgLogIndexInToolbar;	//��Ϣ��¼��toolbar�ϵ�����

	CMessageLogger		m_MsgLogger;				// ��Ϣ��¼
	UINT				m_nUTalkNumber;
	UINT				m_nUserNumber;
	CString				m_strBuddyName;
	CString				m_strBuddySign;
	CString				m_strUserName;

	CString				m_strLinkUrl;
	
	HRGN				m_HotRgn;

	BOOL				m_bEnableAutoReply;

	long				m_nMsgLogRecordOffset;		//��ǰ��Ϣ��¼��ƫ����+1
	long				m_nMsgLogCurrentPageIndex;	//��Ϣ��¼��ǰҳ��

	std::map<CString, long>	m_mapSendFileInfo;		//�����ļ����У������ļ����������ֵΪ-1�����ļ��������ֵ����0����ͼƬ�ڴ���λ�õ�����
	std::map<CString, long>	m_mapRecvFileInfo;		//�����ļ����У������ļ���ʾ������ֵΪ-1����ȡ�������ļ�����ֵ����1����ȡ������ͼƬӦ�����λ������


	BOOL				m_bDraged;					//�û��Ƿ���ק�������ı���ĳߴ�
	RECT				m_rtRichRecv;
	RECT				m_rtMidToolBar;
	RECT				m_rtSplitter;
	RECT				m_rtRichSend;

    time_t              m_nLastSendShakeWindowTime; //���һ�η��ʹ��ڶ����ĵ�ʱ��
};
