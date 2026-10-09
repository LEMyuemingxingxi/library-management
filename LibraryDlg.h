#pragma once

#include "resource.h"
#include "BookManager.h"

class CLibraryDlg : public CDialogEx
{
public:
	CLibraryDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIBRARY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	BookManager m_manager;
	CListCtrl m_list;
	CComboBox m_typeCombo;
	CComboBox m_formatCombo;
	CFont m_listFont;
	CImageList m_rowHeightImageList;

	CString m_id;
	CString m_name;
	CString m_price;
	CString m_extra1;
	CString m_extra2;

	int m_selectedIndex;

	void RefreshList();
	void AutoSizeListColumns();
	void ClearInputs();
	void UpdateTypeFields();
	bool IsPaperType() const;
	CString GetSelectedFormat() const;
	void SetFormatSelection(const CString& format);
	CString FormatBookInfo(const Book* book) const;
	void LoadBookToInputs(int index);
	void SetEditMode(bool editing);

public:
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
};
