#pragma once

enum NAME_STYLE
{
	NAME_STYLE_SHOW_NICKNAME_AND_ACCOUNT,		//��ʾ�ǳƺ��˺�
	NAME_STYLE_SHOW_NICKNAME,					//��ʾ�ǳ�
	NAME_STYLE_SHOW_ACCOUNT						//��ʾ�˺�
};

class CUserConfig
{
public:
	CUserConfig();
	~CUserConfig();

	BOOL LoadConfig(PCTSTR pszFilePath);
	BOOL SaveConfig(PCTSTR pszFilePath);

	void EnableMute(BOOL bMute);
	void EnableDestroyAfterRead(BOOL bEnable);
	void EnableAutoReply(BOOL bAutoReply);
	void SetAutoReplyContent(PCTSTR pszAutoReplyContent);
	void EnableRevokeChatMsg(BOOL bEnable);

	BOOL IsEnableMute() const;
	BOOL IsEnableDestroyAfterRead() const;
	BOOL IsEnableAutoReply() const;
	PCTSTR  GetAutoReplyContent() const;
	BOOL IsEnableRevokeChatMsg() const;

	void SetMainDlgX(long x);
	void SetMainDlgY(long y);
	void SetMainDlgWidth(long nWidth);
	void SetMainDlgHeight(long nHeight);

	void EnableExitPrompt(BOOL bEnable);				
	BOOL IsEnableExitPrompt();

	void EnableExitWhenCloseMainDlg(BOOL bEnable);
	BOOL IsEnableExitWhenCloseMainDlg();

	void SetBuddyListHeadPicStyle(long nStyle);
	void EnableBuddyListShowBigHeadPicInSel(BOOL bEnable);

	void SetNameStyle(long nStyle);
	long GetNameStyle();

	void EnableSimpleProfile(BOOL bEnable);
	BOOL IsEnableSimpleProfile();

	long GetMainDlgX();
	long GetMainDlgY();
	long GetMainDlgWidth() const;
	long GetMainDlgHeight() const;

	long GetBuddyListHeadPicStyle() const;
	BOOL IsEnableBuddyListShowBigHeadPicInSel() const;

	
	void SetChatDlgWidth(long nWidth);
	void SetChatDlgHeight(long nHeight);

	long GetChatDlgWidth() const;
	long GetChatDlgHeight() const;

	void SetGroupDlgWidth(long nWidth);
	void SetGroupDlgHeight(long nHeight);

	long GetGroupDlgWidth() const;
	long GetGroupDlgHeight() const;
	
	void EnablePressEnterToSend(BOOL bEnable);
	void SetFontName(PCTSTR pszFontName);
	void SetFontSize(long nSize);
	void SetFontColor(long lColor);
	void EnableFontBold(BOOL bBold);
	void EnableFontItalic(BOOL bItalic);
	void EnableFontUnderline(BOOL bUnderline);

	BOOL IsEnablePressEnterToSend() const;
	PCTSTR GetFontName() const;
	long GetFontSize() const;
	long GetFontColor() const;
	BOOL IsEnableFontBold() const;
	BOOL IsEnableFontItalic() const;
	BOOL IsEnableFontUnderline() const;

	void SetFaceID(UINT uFaceID);
	UINT GetFaceID() const;

	void SetCustomFace(PCTSTR pszCustomFace);
	PCTSTR GetCustomFace() const;

	void EnableShowLastMsgInChatDlg(BOOL bEnable);
	BOOL IsEnableShowLastMsgInChatDlg();


private:
	BOOL	m_bMute;				//�Ƿ�ر���������
	BOOL    m_bDestroyAfterRead;	//�Ƿ��ĺ󼴷�
	BOOL	m_bAutoReply;			//�Ƿ��Զ��ظ�
	TCHAR	m_szAutoReplyContent[256];	//�Զ��ظ�����
	BOOL	m_bEnableRevokeChatMsg; //�Ƿ�������Ϣ����

	//���Ի���
	long	m_xMainDlg;
	long	m_yMainDlg;
	long	m_cxMainDlg;
	long	m_cyMainDlg;

	BOOL    m_bEnableExitPrompt;					//�˳��Ƿ���ʾ
	BOOL	m_bExitWhenCloseMainDlg;				//�ر����Ի���ʱ�˳����򣨻�����С�������̣�

	long	m_nBuddyListHeadPicStyle;				//�����б���ͷ���񣨴�ͷ��Сͷ�񡢱�׼ͷ��
	BOOL	m_bBuddyListShowBigHeadPicInSel;		//����ʾģʽ��Сͷ��ʱ��ѡ��ĳ��ͷ���Ƿ���ʾ��ͷ��

	long	m_nNameStyleIndex;						//0��ʾ�ǳƺ��˺š�1��ʾ�ǳơ�2��ʾ�˺�
	BOOL	m_bShowSimpleProfile;					//�Ƿ���ʾ��ˬ����
	
	UINT	m_uFaceID;								//��ǰ�û�ͷ��ID
	TCHAR   m_szCustomFace[MAX_PATH];


	//����Ի���
	long	m_cxChatDlg;
	long	m_cyChatDlg;
	long	m_cxGroupDlg;
	long	m_cyGroupDlg;
	BOOL	m_bPressEnterToSend;
	CString	m_strFontName;
	long	m_nFontSize;
	long	m_lFontColor;
	BOOL	m_bFontBold;
	BOOL	m_bFontItalic;
	BOOL	m_bFontUnderline;

	BOOL	m_bShowLastMsg;

};
