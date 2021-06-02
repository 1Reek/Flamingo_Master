#pragma once

#include "SkinManager.h"

#define STBI_STYLE_BUTTON			0x0000
#define STBI_STYLE_SEPARTOR			0x0001
#define STBI_STYLE_CHECK			0x0002
#define STBI_STYLE_DROPDOWN			0x0008
#define STBI_STYLE_WHOLEDROPDOWN	0x0080

// STBI_STYLE_WHOLEDROPDOWN��Ҫ��STBI_STYLE_CHECKһ����
// STBI_STYLE_DROPDOWN��Ҫ��STBI_STYLE_WHOLEDROPDOWNһ����

class CSkinToolBarItem
{
public:
	CSkinToolBarItem(void);
	~CSkinToolBarItem(void);

public:
	int         m_nID;
	DWORD       m_dwStyle;
    int         m_nWidth;
    int         m_nHeight;
    int         m_nLeftWidth;
    int         m_nRightWidth;
	int         m_nPadding;
	int         m_nLeftMargin;
	int         m_nRightMargin;
	CString     m_strText;
	CString     m_strToolTipText;
    CImageEx*   m_lpBgImgN;
    CImageEx*   m_lpBgImgH;
    CImageEx*   m_lpBgImgD;
    CImageEx*   m_lpLeftH;
    CImageEx*   m_lpLeftD;
    CImageEx*   m_lpRightH;
    CImageEx*   m_lpRightD;
	CImageEx*   m_lpSepartorImg;
	CImageEx*   m_lpArrowImg;
	CImageEx*   m_lpIconImg;
	BOOL        m_bChecked;
};

class CSkinToolBar : public CWindowImpl<CSkinToolBar, CWindow>
{
public:
    CSkinToolBar(BOOL bVertical = FALSE);
	~CSkinToolBar(void);

	DECLARE_WND_CLASS(_T("__SkinToolBar__"))

	BEGIN_MSG_MAP_EX(CSkinToolBar)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		//MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

public:
	void SetLeftTop(int nLeft, int nTop);							//����Toolbar�а�ť��������߾���ϱ߾�
	void SetAutoSize(BOOL bAutoSize);								//ע�⣺����ӿڲ������ã���Ҫʹ�ã�
	void SetTransparent(BOOL bTransparent, HDC hBgDC);				//���������͸������toolbar����Ĵ������ݻ���ʾ����
	BOOL SetBgPic(LPCTSTR lpszFileName, const CRect& rcNinePart);	//�������屳��ͼƬ���ܱ�Բ�Ƿ�Χ,����ͼƬ�������˱���͸��ʱ����Ч
    void SetBgColor(COLORREF clrBgColor);                           //������ɫ�������˱���͸��ʱ����Ч

	int AddItem(int nID, DWORD dwStyle);
	void SetItemStyle(int nIndex, int dwStyle);
	void SetItemID(int nIndex, int nID);
	void SetItemSize(int nIndex, int nWidth, int nHeight, int nLeftWidth = 0, int nRightWidth = 0);
	void SetItemPadding(int nIndex, int nPadding);					//��ʵ������������϶
	void SetItemMargin(int nIndex, int nLeftMargin, int nRightMargin);
	void SetItemText(int nIndex, LPCTSTR lpszText);
	void SetItemToolTipText(int nIndex, LPCTSTR lpszText);
	BOOL SetItemBgPic(int nIndex, LPCTSTR lpNormal, LPCTSTR lpHighlight, LPCTSTR lpDown, const CRect& rcNinePart);
	BOOL SetItemLeftBgPic(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown, const CRect& rcNinePart);
	BOOL SetItemRightBgPic(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown, const CRect& rcNinePart);
	BOOL SetItemSepartorPic(int nIndex, LPCTSTR lpszFileName);
	BOOL SetItemArrowPic(int nIndex, LPCTSTR lpszFileName);
	BOOL SetItemIconPic(int nIndex, LPCTSTR lpszFileName);

	BOOL GetItemRectByIndex(int nIndex, CRect& rect);
	BOOL GetItemRectByID(int nID, CRect& rect);

	BOOL GetItemCheckState(int nIndex);
	void SetItemCheckState(int nIndex, BOOL bChecked);

    void SetUseGroup(bool bUseGroup);

private:
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();

	BOOL StartTrackMouseLeave();
	void CalcCenterRect(CRect& rcDest, int cx, int cy, CRect& rcCenter);
	CSkinToolBarItem* GetItemByIndex(int nIndex);
	CSkinToolBarItem* GetItemByID(int nID);
	int HitTest(POINT pt);

	void DrawParentWndBg(HDC hDC);
	void DrawItem(HDC hDC, int nIndex);

private:
    BOOL                            m_bVertical;        //�Ƿ��Ǵ�ֱtoolbar��Ĭ����ˮƽ��
    CToolTipCtrl					m_ToolTipCtrl;
	std::vector<CSkinToolBarItem*>	m_arrItems;
	CImageEx*						m_lpBgImg;
    COLORREF                        m_clrBgColor;       //Ĭ�ϱ�����ɫΪ��ɫ
	int								m_nPressIndex; 
	int								m_nHoverIndex;
	BOOL							m_bPressLorR;       //����߻����ұ�С��������
	BOOL							m_bHoverLorR;
	BOOL							m_bMouseTracking;
	int								m_nLeft;
	int								m_nTop;
	BOOL							m_bAutoSize;
	BOOL							m_bTransparent;
	HDC								m_hBgDC;
    BOOL                            m_bUseGroup;        //�Ƿ��toolbar�еİ�ť����һ�飬�������һ�飬ͬһʱ��ֻ��һ����ѡ�У�Ĭ�ϲ�����
};
