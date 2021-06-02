#pragma once

#include <vector>
#include "SkinManager.h"
#include "SkinScrollBar.h"

//������λ��
#define WM_SCROLLBAR_POS    WM_USER + 0x00FF

struct NMHDREx
{
    NMHDR   hdr;
    LPARAM  lParam;
};

class CLVColumn
{
public:
	CLVColumn(void);
	~CLVColumn(void);

public:
	CString     m_strText;  //��ͷ����
	CImageEx*   m_lpImage;
	DWORD       m_dwFmt;    //��ȡֵ��DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS
	int         m_nWidth;
};

class CLVItem
{
public:
	CLVItem(void);
	~CLVItem(void);

public:
	CString             m_strText;
    COLORREF            m_clrColor;
	Gdiplus::Bitmap*    m_lpImage;
	DWORD               m_dwFmt;
	LPARAM              m_lParam;
};

class CLVRow
{
public:
	CLVRow(int nCols);
	~CLVRow(void);

public:
	BOOL AddItem();
	BOOL InsertItem(int nCol);
	BOOL DeleteItem(int nCol);

	BOOL SetItem(int nCol, LPCTSTR lpszText, LPCTSTR lpszImage, 
		int cx, int cy, BOOL bGray, DWORD dwFmt, LPARAM lParam);

    //TODO: ��������Ҫ�ģ���
	BOOL SetItemText(int nCol,  LPCTSTR lpszText);
    BOOL SetItemColor(int nCol, COLORREF clrColor);
	BOOL SetItemImage(int nCol,  LPCTSTR lpszImage, int cx, int cy, BOOL bGray = FALSE);
	BOOL SetItemFormat(int nCol,  DWORD dwFmt);
	BOOL SetItemData(int nCol,  LPARAM lParam);

	CString GetItemText(int nCol);
	Gdiplus::Bitmap* GetItemImage(int nCol);
    COLORREF GetItemColor(int nCol);
	DWORD GetItemFormat(int nCol);
	LPARAM GetItemData(int nCol);

	int GetItemCount();
	BOOL DeleteAllItems();

private:
	CLVItem* GetItem(int nCol);

private:
	std::vector<CLVItem*> m_arrItems;
};

class CSkinListCtrl : public CWindowImpl<CSkinListCtrl, CWindow>
{
public:
	CSkinListCtrl(void);
	~CSkinListCtrl(void);

    DECLARE_WND_CLASS(_T("__SkinListCtrl__"))


	BEGIN_MSG_MAP_EX(CSkinListCtrl)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_SIZE(OnSize)
		MSG_WM_GETDLGCODE(OnGetDlgCode)
		//MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

public:
	BOOL SetBgPic(LPCTSTR lpszFileName);				// �����б���ͼƬ, ����ͼƬ�ڱ���͸��ʱ����Ч
    void SetHeaderBgColor(COLORREF clrBgColor);         // �����б��ͷ������ɫ��Ĭ����ɫΪ��ɫ�����ñ���͸��ʱ��������ɫ����Ч
    void SetBodyBgColor(COLORREF clrBgColor);           // �����б���屳����ɫ��Ĭ����ɫΪ��ɫ�����ñ���͸��ʱ��������ɫ����Ч
	BOOL SetHeadSeparatedPic(LPCTSTR lpszFileName);		// ���ñ�ͷ�ָ���ͼƬ

	BOOL SetHeadNormalPic(LPCTSTR lpszFileName);		// ���ñ�ͷ����ͼƬ(��ͨ״̬)
	BOOL SetHeadHotPic(LPCTSTR lpszFileName);			// ���ñ�ͷ����ͼƬ(����״̬)
	BOOL SetHeadPushedPic(LPCTSTR lpszFileName);		// ���ñ�ͷ����ͼƬ(����״̬)

    void SetGridLineVisible(BOOL bVisible);             // �������Ƿ�ɼ���Ĭ�ϲ��ɼ�
    void SetGridLineWidth(int nWidth);                  // ���������ߴ�ϸ��Ĭ��1px
    void SetGridLineColor(COLORREF clr);                // ������������ɫ��Ĭ��

	BOOL SetOddItemBgPic(LPCTSTR lpszFileName);			// �������б���ͼƬ(��ͨ״̬)
	BOOL SetEvenItemBgPic(LPCTSTR lpszFileName);		// ����˫�б���ͼƬ(��ͨ״̬)
	BOOL SetItemHoverBgPic(LPCTSTR lpszFileName);		// �����б���ͼƬ(����״̬)
	BOOL SetItemSelBgPic(LPCTSTR lpszFileName);			// �����б���ͼƬ(ѡ��״̬)

	void SetTransparent(BOOL bTransparent, HDC hBgDC);	// �����б����Ƿ�͸��
	void SetHeadVisible(BOOL bVisible);					// ���ñ�ͷ�Ƿ�ɼ�
	BOOL IsHeadVisible();								// �жϱ�ͷ�Ƿ�ɼ�
	void SetHeadHeight(int nHeight);					// ���ñ�ͷ�߶�
	void SetItemHeight(int nHeight);					// �����и߶�
	void SetItemImageSize(int cx, int cy);				// ������ͼ���С

	int AddColumn(LPCTSTR lpszText,						// ����У��൱�ڸ�����������һ����Ԫ��
		LPCTSTR lpszImage, DWORD dwFmt, int nWidth);
    //int AddColumns()
	int InsertColumn(int nCol, LPCTSTR lpszText,		// �����У�ԭ��ͬ��
		LPCTSTR lpszImage, DWORD dwFmt, int nWidth);
	BOOL DeleteColumn(int nCol);						// ɾ���У�ԭ��ͬ��

	int AddItem(LPCTSTR lpszText,						// ����У�ͬʱ���ø��е�һ����Ԫ�������
		LPCTSTR lpszImage, BOOL bGray, DWORD dwFmt, LPARAM lParam);
	int InsertItem(int nItem, LPCTSTR lpszText,			// �����У�ͬʱ���ø��е�һ����Ԫ������֣�nItem��ʶ��ǰһ�б����Ѿ����ڣ������������
		LPCTSTR lpszImage, BOOL bGray, DWORD dwFmt, LPARAM lParam);
	BOOL DeleteItem(int nItem);							// ɾ����

	int GetColumnCount();								// ��ȡ������
	BOOL DeleteAllColumns();							// ɾ��������
	int GetItemCount();									// ��ȡ������
	BOOL DeleteAllItems();								// ɾ��������

	BOOL SetColumnText(int nCol, LPCTSTR lpszText);		// ������ͷ�ı�
	BOOL SetColumnImage(int nCol, LPCTSTR lpszImage);	// ������ͷͼ��
	BOOL SetColumnFormat(int nCol, DWORD dwFmt);		// ���������ָ�ʽ
	BOOL SetColumnWidth(int nCol, int nWidth);			// �����п��

	CString GetColumnText(int nCol);					// ��ȡ���ı�
	DWORD GetColumnFormat(int nCol);					// ��ȡ�и�ʽ
	int GetColumnWidth(int nCol);						// ��ȡ�п��

	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);	// �������ı��������Ӧ�ĵ�Ԫ�񲻴��ڣ��򲻻�����κ�Ч��
    BOOL SetCellTextColor(int nItem, int nSubItem, COLORREF clrColor);	// ���õ�Ԫ���ı���ɫ�������Ӧ�ĵ�Ԫ�񲻴��ڣ��򲻻�����κ�Ч��
	BOOL SetItemImage(int nItem, int nSubItem,						// ������ͼ��
		LPCTSTR lpszImage, BOOL bGray = FALSE);
	BOOL SetItemFormat(int nItem, int nSubItem, DWORD dwFmt);		// �����и�ʽ
    //BOOL SetRowFormat(int nItem, DWORD dwFmt);		                // �����и�ʽ, TODO: �������Ҫ�Ż�(̫������)
	BOOL SetItemData(int nItem, int nSubItem, LPARAM lParam);		// ����������

	CString GetItemText(int nItem, int nSubItem);		// ��ȡ���ı�
	DWORD GetItemFormat(int nItem, int nSubItem);		// ��ȡ�и�ʽ
	LPARAM GetItemData(int nItem, int nSubItem);		// ��ȡ������


    //TODO: ��Щ��������Ҫ�ģ������������в������õ�Ԫ��
	void SetCurSelIndex(int nIndex);
	int GetCurSelItemIndex();							// ��ȡ��ǰѡ��������
	void SetItemTextColor(COLORREF clr);				// �������ı���ɫ(��ͨ״̬)
	void SetSelItemTextColor(COLORREF clr);				// ���ı���ɫ(ѡ��״̬)
    void SetSelItemBkColor(COLORREF clr);               // ��ѡ��״̬������ɫ

    //���ù�������λ��
    void SetVScrollBarPos(int nPos);
    void SetHScrollBarPos(int nPos);

    //�Ƿ����򸸴��ڻ㱨ʵʱ��ֱ��������λ��
    void EnableVScrollBarRealPostion(BOOL bEnable);
    //�Ƿ��������ͷ��ʾ����
    void EnableClickHeaderSort(BOOL bEnable);
    //�Ƿ�������п�ѡģʽ
    void EnableRowSelectable(BOOL bEnable);

private:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnTimer(UINT_PTR nIDEvent);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnSize(UINT nType, CSize size);
	UINT OnGetDlgCode(LPMSG lpMsg);
	//LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();

	CLVColumn* GetColumn(int nCol);
	CLVRow* GetItem(int nItem);

	void DrawParentWndBg(HDC hDC);
	void DrawHeader(HDC hDC);
    void DrawTopFixedRow(HDC hDC);
    void DrawBody(HDC hDC);

    //��ȡ�б�ͷ�������С
	BOOL GetHeaderRect(CRect& rect);
    //��ȡ�����̶��е�λ��
    BOOL GetTopFixedRowRect(CRect& rect);
    //��ȡ�б���岿����������й��������ȥ��������Ӧ�Ŀ���
	BOOL GetBodyRect(CRect& rect);
	BOOL GetItemRect(int nIndex, CRect& rect);
	int HeadHitTest(POINT pt);
	int ItemHitTest(POINT pt);
	BOOL StartTrackMouseLeave();
	void AdjustScrollBarSize();
	void CheckScrollBarStatus();
	void Scroll(int cx, int cy);
	void EnsureVisible(int nIndex);
	int GetColumnsWidth();

    //���ʹ�ֱ������ʵʱλ����������
    void PostVScrollBarRealPostion(int nVScrollBarPos);
    //����ͷ����������ʽ��ͷ
    void DrawHeaderItemSortStyle(int nSortStyleIndex);

	BOOL Init();
	void UnInit();

private:
	//CToolTipCtrl m_ToolTipCtrl;
	CSkinScrollBar          m_HScrollBar;
	CSkinScrollBar          m_VScrollBar;

	std::vector<CLVColumn*> m_arrCols;
	std::vector<CLVRow*>    m_arrRows;

	CImageEx*               m_lpBgImg;					// �б���ͼƬ
    COLORREF                m_clrHeaderBgColor;         // ��ͷ������ɫ
    COLORREF                m_clrBodyBgColor;           // ���屳����ɫ
	CImageEx*               m_lpHeadSeparatedImg;		// ��ͷ�ָ���ͼƬ
	CImageEx*               m_lpHeadImgN;				// ��ͷ����ͼƬ(��ͨ״̬)
	CImageEx*               m_lpHeadImgH;				// ��ͷ����ͼƬ(����״̬)
	CImageEx*               m_lpHeadImgP;				// ��ͷ����ͼƬ(����״̬)
	CImageEx*               m_lpOddItemBgImgN;			// ���б���ͼƬ(��ͨ״̬)
	CImageEx*               m_lpEvenItemBgImgN;			// ˫�б���ͼƬ(��ͨ״̬)
	CImageEx*               m_lpItemBgImgH;				// �б���ͼƬ(����״̬)
	CImageEx*               m_lpItemBgImgS;				// �б���ͼƬ(ѡ��״̬)

	BOOL                    m_bTransparent;			    // �б����Ƿ�͸����־
	HDC                     m_hBgDC;					// �����ڱ���DC
	BOOL                    m_bMouseTracking;			// �����ٱ�־
	BOOL                    m_bHeadVisible;				// ��ͷ�Ƿ�ɼ���־
	int                     m_nHeadHeight;				// ��ͷ�߶�
	int                     m_nRowHeight;				// �и߶�
	CSize                   m_szItemImage;				// ��ͼ���С
    BOOL                    m_bGridLineVisible;         // �Ƿ�������
    int                     m_nGridLineWidth;           // �����ߵĿ��

	int                     m_nPressHeadIndex;			// ��갴�±�ͷ������
	int                     m_nHoverHeadIndex;			// �����ͣ��ͷ������

	int                     m_nPressItemIndex;			// ��갴����������
	int                     m_nHoverItemIndex;			// �����ͣ��������
	int                     m_nSelItemIndex;			// ���ѡ����������

	int                     m_nLeft, m_nTop;			// ��ͼԭ������

	COLORREF                m_clrItemText;				// ���ı���ɫ(��ͨ״̬)
	COLORREF                m_clrSelItemText;           // ѡ�����ı���ɫ
    COLORREF                m_clrSelItemBkColor;        // ѡ���б�����ɫ
    COLORREF                m_clrGridLine;              // ��������ɫ

    int                     m_nTopFixedRow;             //�����̶���

    int                     m_nStartVisibleIndex;       //TODO: ��������Ҫ��
    int                     m_nEndVisibleIndex;         //TODO: ��������Ҫ��
    BOOL                    m_bEnablePostVScrollBarPos; //�Ƿ��򸸴��ڱ���ʵʱ������λ�ã�Ĭ��ΪFALSE
    BOOL                    m_bEnableClickHeaderSort;   //�����ͷ���Ƿ�֧������Ĭ�ϲ�������FALSE��
    BOOL                    m_bEnableRowSelectable;     //������Ƿ����ѡ�У�Ĭ�Ͽ��ԣ�TRUE��
};
