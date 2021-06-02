#pragma once

#include "SkinManager.h"

#define		SKIN_MENU_MAGIC		0x2010		// Ƥ���˵���Ŀ��Ϣ��ʶ

struct SKIN_MENU_ITEM_INFO					// Ƥ���˵���Ŀ��Ϣ�ṹ
{
	UINT nMagic;							// Ƥ���˵���Ŀ��Ϣ��ʶ
	UINT nID;								// �˵�ID
	UINT nState;							// �˵�״̬
	UINT nType;								// ����
	HMENU hSubMenu;							// �ӵ���(������ʽ�˵�)���
	TCHAR szText[256];						// �˵����ı�
	CImageEx* lpIconImgN;					// δѡ��ͼ��
	CImageEx* lpIconImgH;					// ѡ��ͼ��
};

class CSkinMenu								// Ƥ���˵���װ��
{
public:
	CSkinMenu(HMENU hMenu = NULL);
	~CSkinMenu(void);

public:
	BOOL SetBgPic(LPCTSTR lpszLeftBg, LPCTSTR lpszRightBg);	// ���ò˵����ͼƬ
	BOOL SetSelectedPic(LPCTSTR lpszFileName);		// ���ò˵���ѡ��ͼƬ
	BOOL SetSepartorPic(LPCTSTR lpszFileName);		// ���÷ָ���ͼƬ
	BOOL SetArrowPic(LPCTSTR lpszFileName);			// ���õ���ʽ�˵���ͷͼƬ
	BOOL SetCheckPic(LPCTSTR lpszFileName);			// ����Check״̬ͼƬ
	BOOL SetIcon(UINT nItem, BOOL bByPosition, LPCTSTR lpszIconN, LPCTSTR lpszIconH);	// ����ָ���˵�����ʹ�õ�ͼ��

	void SetTextColor(COLORREF clrText);			// ������ͨ�˵��ı���ɫ
	void SetSelTextColor(COLORREF clrSelText);		// ����ѡ�в˵��ı���ɫ
	void SetGrayedTextColor(COLORREF clrGrayedText);// ���ûһ��˵��ı���ɫ

public:
	BOOL CreateMenu();
	BOOL CreatePopupMenu();
	BOOL LoadMenu(LPCWSTR lpszResourceName);
	BOOL LoadMenu(UINT nIDResource);
	BOOL DestroyMenu();

	void OnInitMenuPopup(HMENU hMenuPopup, UINT nIndex, BOOL bSysMenu);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	CSkinMenu& operator =(HMENU hMenu);

	BOOL Attach(HMENU hMenu);
	HMENU Detach();

	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	BOOL TrackPopupMenu(UINT nFlags, int x, int y, HWND hWnd, LPCRECT lpRect = 0);
	BOOL TrackPopupMenuEx(UINT fuFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);

	BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem = 0,
		LPCTSTR lpszNewItem = NULL);
	UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
	UINT GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID, UINT nFlags);
	int GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount, UINT nFlags);
	CSkinMenu GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition, UINT nFlags, 
		UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, 
		UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
	BOOL RemoveMenu(UINT nPosition, UINT nFlags);
	BOOL CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);

	BOOL IsMenu();

private:
	SKIN_MENU_ITEM_INFO* GetMenuItemInfo(UINT nItem, BOOL bByPosition);	// ��ȡָ���˵�����Ϣ���ݺ���
	BOOL SetMenuItemInfo(UINT nItem, BOOL bByPosition, SKIN_MENU_ITEM_INFO* lpItemInfo);	// ����ָ���˵�����Ϣ���ݺ���

	BOOL EnableOwnerDraw(HMENU hMenu, BOOL bEnable);			// ʹ�ܲ˵��Ի���

	SIZE GetTextExtent(LPCTSTR lpszText);						// ��ȡָ�����ֵĿ�߶�

	void DrawBg(HDC hDC, CRect& rect);							// ���Ʋ˵����
	void DrawSepartor(HDC hDC, CRect& rect);					// ���Ʒָ���
	void DrawSelRect(HDC hDC, CRect& rect);						// ���ѡ������
	void DrawIcon(HDC hDC, CRect& rect, CImageEx* lpIconImg);	// ���Ʋ˵�ͼ��
	void DrawCheckState(HDC hDC, CRect& rect);					// ����Check״̬
	void DrawText(HDC hDC, CRect& rect, BOOL bGrayed,			// ���Ʋ˵��ı�
		BOOL bSelected, BOOL bIsSubMenu, LPCTSTR lpText);	
	void DrawArrow(HDC hDC, CRect& rect);						// ���Ƶ���ʽ�˵���ͷ

public:
	HMENU m_hMenu;							// �˵����

private:
	CImageEx* m_lpBgImgL,* m_lpBgImgR;	// �˵����ͼƬ
	CImageEx* m_lpSelectedImg;				// �˵���ѡ��ͼƬ
	CImageEx* m_lpSepartorImg;				// �ָ���ͼƬ
	CImageEx* m_lpArrowImg;				// ����ʽ�˵���ͷͼƬ
	CImageEx* m_lpCheckImg;				// Check״̬ͼƬ

	COLORREF m_clrText;						// ��ͨ�˵��ı���ɫ
	COLORREF m_clrSelText;					// ѡ�в˵��ı���ɫ
	COLORREF m_clrGrayedText;				// �һ��˵��ı���ɫ

	HFONT m_hFont;							// �˵�����
	int m_nLMargin;							// ��������
};