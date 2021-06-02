#include "stdafx.h"
#include "FontSelDlg.h"

CFontInfo g_fontInfo;		// ������Ϣ
TCHAR g_cHotKey = _T('D');	// ��ȡ��Ϣ�ȼ�

// ����Ϊͼ���㣬���Խ�����������������

//// ������������
//BOOL LoadMiscConfig(LPCTSTR lpszFileName)
//{
//	CXmlDocument xmlDoc;
//	CXmlNode xmlNode;
//
//	BOOL bRet = xmlDoc.Load(lpszFileName);
//	if (!bRet)
//		return FALSE;
//
//	bRet = xmlDoc.SelectSingleNode(_T("/Misc/FontInfo"), xmlNode);
//	if (bRet)
//	{
//		g_fontInfo.m_strName = xmlNode.GetAttribute(_T("Name"));
//		g_fontInfo.m_nSize = xmlNode.GetAttributeInt(_T("Size"));
//		tstring strColor = xmlNode.GetAttribute(_T("Color"));
//		g_fontInfo.m_clrText = HexStrToRGB(strColor.c_str());
//		g_fontInfo.m_bBold = xmlNode.GetAttributeInt(_T("Bold"));
//		g_fontInfo.m_bItalic = xmlNode.GetAttributeInt(_T("Italic"));
//		g_fontInfo.m_bUnderLine = xmlNode.GetAttributeInt(_T("UnderLine"));
//	}
//
//	bRet = xmlDoc.SelectSingleNode(_T("/Misc/HotKey"), xmlNode);
//	if (bRet)
//	{
//		tstring strHotKey = xmlNode.GetText();
//		g_cHotKey = toupper(strHotKey.at(0));
//	}
//
//	xmlNode.Release();
//	xmlDoc.Release();
//
//	return TRUE;
//}



//// ������������
//BOOL SaveMiscConfig(LPCTSTR lpszFileName)
//{
//	unsigned char cUtf8Header[3] = {0xEF,0xBB,0xBF};
//	const wchar_t* lpFontFmt = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<Misc>\r\n<!--������Ϣ-->\r\n<FontInfo Name=\"%s\" Size=\"%d\" Color=\"%s\" Bold=\"%d\" Italic=\"%d\" UnderLine=\"%d\"></FontInfo>\r\n<!--��ȡ��Ϣ�ȼ���Ŀǰֻ֧��Ctrl+Alt+ĳ��-->\r\n<HotKey>%c</HotKey>\r\n</Misc>\r\n");
//
//	if (NULL == lpszFileName)
//		return FALSE;
//
//	FILE* fp = _tfopen(lpszFileName, _T("wb"));
//	if (NULL == fp)
//		return FALSE;
//
//	wchar_t szColor[32] = {0};
//	RGBToHexStr(g_fontInfo.m_clrText, szColor, sizeof(szColor)/sizeof(WCHAR));
//
//	wchar_t szBuf[4096] = {0};
//	wsprintf(szBuf, lpFontFmt, g_fontInfo.m_strName.c_str(), g_fontInfo.m_nSize, 
//		szColor, g_fontInfo.m_bBold, g_fontInfo.m_bItalic, g_fontInfo.m_bUnderLine, g_cHotKey);
//
//	char* lpBuf = EncodeUtil::UnicodeToUtf8(szBuf);
//	if (NULL == lpBuf)
//	{
//		fclose(fp);
//		return FALSE;
//	}
//
//	fwrite(cUtf8Header, 3, 1, fp);
//	fwrite(lpBuf, strlen(lpBuf), 1, fp);
//	fclose(fp);
//
//	delete []lpBuf;
//
//	return TRUE;
//}

CFontSelDlg::CFontSelDlg(void)
{
}

CFontSelDlg::~CFontSelDlg(void)
{
}

// ��ȡȫ��������Ϣ
CFontInfo& CFontSelDlg::GetPublicFontInfo()
{
	return g_fontInfo;
}

// ��ȡ������Ϣ
CFontInfo& CFontSelDlg::GetFontInfo()
{
	return m_fontInfo;
}

BOOL CFontSelDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	Init();		// ��ʼ��

	ModifyStyle(WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0);
	return FALSE;
}

void CFontSelDlg::OnClose()
{
	DestroyWindow();
}

void CFontSelDlg::OnDestroy()
{
	UnInit();	// ����ʼ��
}

// ��ϵͳ���塱��ť
void CFontSelDlg::OnBtn_SysFont(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
		m_tbFontClass.SetItemCheckState(1, FALSE);
	else if (BN_UNPUSHED == uNotifyCode)
		m_tbFontClass.SetItemCheckState(0, TRUE);
}

// ����Ա�Ų��֡���ť
void CFontSelDlg::OnBtn_MemberFont(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
		m_tbFontClass.SetItemCheckState(0, FALSE);
	else if (BN_UNPUSHED == uNotifyCode)
		m_tbFontClass.SetItemCheckState(1, TRUE);
}

// ���������ơ���Ͽ�
void CFontSelDlg::OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel = m_cboFontName.GetCurSel();

	CString strText;
	m_cboFontName.GetLBText(nCurSel, strText);

	if (m_fontInfo.m_strName != strText.GetBuffer())
	{
		m_fontInfo.m_strName = strText.GetBuffer();
		g_fontInfo.m_strName = m_fontInfo.m_strName;
		long nIndex = m_cboFontName.FindString(0, m_fontInfo.m_strName.c_str());
		m_pFMGClient->m_UserConfig.SetFontName(m_fontInfo.m_strName.c_str());
		::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
	}
}

// �������С����Ͽ�
void CFontSelDlg::OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel = m_cboFontSize.GetCurSel();

	CString strText;
	m_cboFontSize.GetLBText(nCurSel, strText);

	int nSize = _tcstol(strText.GetBuffer(), NULL, 10);
	if (m_fontInfo.m_nSize != nSize)
	{
		m_fontInfo.m_nSize = nSize;
		g_fontInfo.m_nSize = m_fontInfo.m_nSize;
		m_pFMGClient->m_UserConfig.SetFontSize(m_fontInfo.m_nSize);
		::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
	}
}

// ���Ӵ֡���ť
void CFontSelDlg::OnBtn_Bold(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bBold = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bBold != bBold)
		{
			m_fontInfo.m_bBold = bBold;
			g_fontInfo.m_bBold = m_fontInfo.m_bBold;
			m_pFMGClient->m_UserConfig.EnableFontBold(m_fontInfo.m_bBold);
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}

// ����б����ť
void CFontSelDlg::OnBtn_Italic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bItalic = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bItalic != bItalic)
		{
			m_fontInfo.m_bItalic = bItalic;
			g_fontInfo.m_bItalic = m_fontInfo.m_bItalic;
			m_pFMGClient->m_UserConfig.EnableFontItalic(m_fontInfo.m_bItalic);
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}

// ���»��ߡ���ť
void CFontSelDlg::OnBtn_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bUnderLine = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bUnderLine != bUnderLine)
		{
			m_fontInfo.m_bUnderLine = bUnderLine;
			g_fontInfo.m_bUnderLine = m_fontInfo.m_bUnderLine;
			m_pFMGClient->m_UserConfig.EnableFontUnderline(m_fontInfo.m_bUnderLine);
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}

// ����ɫ����ť
void CFontSelDlg::OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CColorDialog colorDlg = m_pFMGClient->m_UserConfig.GetFontColor();
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF clrText = colorDlg.GetColor();
		if (m_fontInfo.m_clrText != clrText)
		{
			m_fontInfo.m_clrText = clrText;
			g_fontInfo.m_clrText = m_fontInfo.m_clrText;
			m_pFMGClient->m_UserConfig.SetFontColor(m_fontInfo.m_clrText);
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}

// ��ʼ��������๤����
BOOL CFontSelDlg::InitFontClassToolBar()
{
	int nIndex = m_tbFontClass.AddItem(101, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontClass.SetItemSize(nIndex, 20, 20);
	m_tbFontClass.SetItemPadding(nIndex, 0);
	m_tbFontClass.SetItemCheckState(nIndex, TRUE);
	m_tbFontClass.SetItemToolTipText(nIndex, _T("ϵͳ����"));
	m_tbFontClass.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontClass.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_sysfont_tab_button.png"));
	//m_tbFontClass.SetItemIconPic(nIndex, _T("MidToolBar\\SetFontTabButton.png"));

	//nIndex = m_tbFontClass.AddItem(102, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	//m_tbFontClass.SetItemSize(nIndex, 20, 20);
	m_tbFontClass.SetItemPadding(nIndex, 8);
	//m_tbFontClass.SetItemToolTipText(nIndex, _T("��Ա�Ų���"));
	//m_tbFontClass.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		//_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	//m_tbFontClass.SetItemIconPic(nIndex, _T("MidToolBar\\SetFontTabButton.png"));

	nIndex = m_tbFontClass.AddItem(103, STBI_STYLE_SEPARTOR);
	m_tbFontClass.SetItemSize(nIndex, 2, 20);
	m_tbFontClass.SetItemPadding(nIndex, 0);
	m_tbFontClass.SetItemSepartorPic(nIndex, _T("aio_qzonecutline_normal.png"));

	m_tbFontClass.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcFontClass(4, 7, 4+55, 7+32);
	m_tbFontClass.Create(m_hWnd, rcFontClass, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_FONTCLASS);

	return TRUE;
}

// ��ʼ������������Ͽ�
BOOL CFontSelDlg::InitFontNameComboBox()
{
	m_cboFontName.SubclassWindow(GetDlgItem(ID_COMBO_FONTNAME));

	m_cboFontName.SetRedraw(FALSE);
	m_cboFontName.m_Edit.EnableWindow(FALSE);

	m_cboFontName.SetArrowWidth(19);
	m_cboFontName.MoveWindow(10, 6, 100, 30, FALSE);
	m_cboFontName.SetItemHeight(-1, 18);

	m_cboFontName.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	m_cboFontName.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_cboFontName.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_cboFontName.SetArrowNormalPic(_T("ComboBox\\inputbtn_normal_bak.png"));
	m_cboFontName.SetArrowHotPic(_T("ComboBox\\inputbtn_highlight.png"));
	m_cboFontName.SetArrowPushedPic(_T("ComboBox\\inputbtn_down.png"));

	std::vector<tstring> arrSysFont;
	EnumSysFont(&arrSysFont);

	for (int i = 0; i < (int)arrSysFont.size(); i++)
	{
		m_cboFontName.AddString(arrSysFont[i].c_str());
	}
	arrSysFont.clear();
	
	CString strCustomFontName(m_pFMGClient->m_UserConfig.GetFontName());
	if(strCustomFontName.IsEmpty())
	{
		strCustomFontName = _T("΢���ź�");
		m_pFMGClient->m_UserConfig.SetFontName(strCustomFontName);
	}
	int nIndex = m_cboFontName.FindString(0, strCustomFontName);
	//int nIndex = m_pFMGClient->m_UserConfig.GetFontNameIndex();

	if(nIndex < 0)
		nIndex = 0;
	m_cboFontName.SetCurSel(nIndex);

	m_cboFontName.SetRedraw(TRUE);

	return TRUE;
}

// ��ʼ�������С��Ͽ�
BOOL CFontSelDlg::InitFontSizeComboBox()
{
	m_cboFontSize.SubclassWindow(GetDlgItem(ID_COMBO_FONTSIZE));

	m_cboFontSize.SetRedraw(FALSE);
	m_cboFontSize.m_Edit.EnableWindow(FALSE);
	m_cboFontSize.SetArrowWidth(19);
	m_cboFontSize.MoveWindow(166 - 48, 6, 50, 30, FALSE);
	m_cboFontSize.SetItemHeight(-1, 18);

	m_cboFontSize.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	m_cboFontSize.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_cboFontSize.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_cboFontSize.SetArrowNormalPic(_T("ComboBox\\inputbtn_normal_bak.png"));
	m_cboFontSize.SetArrowHotPic(_T("ComboBox\\inputbtn_highlight.png"));
	m_cboFontSize.SetArrowPushedPic(_T("ComboBox\\inputbtn_down.png"));

	CString strFontSize;
	for (int i = 9; i <= 72; i++)
	{
		strFontSize.Format(_T("%d"), i);
		m_cboFontSize.AddString(strFontSize);
	}
	
	m_fontInfo.m_nSize = m_pFMGClient->m_UserConfig.GetFontSize();
	strFontSize.Format(_T("%d"), m_fontInfo.m_nSize);
	int nIndex = m_cboFontSize.FindString(0, strFontSize);
	m_cboFontSize.SetCurSel(nIndex);

	m_cboFontSize.SetRedraw(TRUE);

	return TRUE;
}

// ��ʼ���������Թ�����
BOOL CFontSelDlg::InitFontAttrToolBar()
{
	int nIndex = m_tbFontAttr.AddItem(104, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("�Ӵ�"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\Bold.png"));

	nIndex = m_tbFontAttr.AddItem(105, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("��б"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\Italic.png"));

	nIndex = m_tbFontAttr.AddItem(106, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("�»���"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\underline.png"));

	nIndex = m_tbFontAttr.AddItem(107, STBI_STYLE_BUTTON);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("��ɫ"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\color.png"));

	m_tbFontAttr.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcFontAttr(217 - 48, 6, 217+104, 6+32);
	m_tbFontAttr.Create(m_hWnd, rcFontAttr, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_FONTATTR);

	m_tbFontAttr.SetItemCheckState(0, m_fontInfo.m_bBold);
	m_tbFontAttr.SetItemCheckState(1, m_fontInfo.m_bItalic);
	m_tbFontAttr.SetItemCheckState(2, m_fontInfo.m_bUnderLine);
	return TRUE;
}

// ��ʼ��
BOOL CFontSelDlg::Init()
{
	g_fontInfo.m_strName = m_pFMGClient->m_UserConfig.GetFontName();
	g_fontInfo.m_bBold = m_pFMGClient->m_UserConfig.IsEnableFontBold();
	g_fontInfo.m_bItalic = m_pFMGClient->m_UserConfig.IsEnableFontItalic();
	g_fontInfo.m_bUnderLine = m_pFMGClient->m_UserConfig.IsEnableFontUnderline();
	g_fontInfo.m_clrText = m_pFMGClient->m_UserConfig.GetFontColor();
	
	m_fontInfo = g_fontInfo;

	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetBgPic(_T("ChatFrame_FontSetup_background.bmp"), CRect(0, 1, 1, 0));
	
	//InitFontClassToolBar();		// ��ʼ��������๤����
	InitFontNameComboBox();		// ��ʼ������������Ͽ�
	InitFontSizeComboBox();		// ��ʼ�������С��Ͽ�
	InitFontAttrToolBar();		// ��ʼ���������Թ�����

	return TRUE;
}

// ����ʼ��
void CFontSelDlg::UnInit()
{
	if (m_tbFontClass.IsWindow())
		m_tbFontClass.DestroyWindow();

	if (m_tbFontAttr.IsWindow())
		m_tbFontAttr.DestroyWindow();

	if (m_cboFontName.IsWindow())
		m_cboFontName.DestroyWindow();

	if (m_cboFontSize.IsWindow())
		m_cboFontSize.DestroyWindow();
}