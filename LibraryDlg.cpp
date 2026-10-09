#include "pch.h"
#include "library.h"
#include "LibraryDlg.h"
#include "afxdialogex.h"
#include "StringUtil.h"
#include "Book.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLibraryDlg::CLibraryDlg(CWnd* pParent)
	: CDialogEx(IDD_LIBRARY_DIALOG, pParent)
	, m_id(_T(""))
	, m_name(_T(""))
	, m_price(_T(""))
	, m_extra1(_T(""))
	, m_extra2(_T(""))
	, m_selectedIndex(-1)
{
}

void CLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_id);
	DDX_Text(pDX, IDC_EDIT2, m_name);
	DDX_Text(pDX, IDC_EDIT3, m_price);
	DDX_Text(pDX, IDC_EDIT4, m_extra1);
	DDX_Text(pDX, IDC_EDIT5, m_extra2);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_typeCombo);
	DDX_Control(pDX, IDC_COMBO2, m_formatCombo);
}

BEGIN_MESSAGE_MAP(CLibraryDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CLibraryDlg::OnLvnItemchangedList1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CLibraryDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CLibraryDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLibraryDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLibraryDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CLibraryDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CLibraryDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CLibraryDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CLibraryDlg::OnBnClickedButton7)
END_MESSAGE_MAP()

BOOL CLibraryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(_T("图书管理系统"));

	m_listFont.CreatePointFont(110, _T("Microsoft YaHei UI"));
	m_list.SetFont(&m_listFont);

	m_rowHeightImageList.Create(1, 22, ILC_COLOR, 1, 1);
	m_list.SetImageList(&m_rowHeightImageList, LVSIL_SMALL);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);
	m_list.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 90);
	m_list.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(2, _T("价格"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(3, _T("类型"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(4, _T("页数"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(5, _T("格式"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(6, _T("大小(MB)"), LVCFMT_LEFT, 80);

	m_typeCombo.AddString(_T("纸质图书"));
	m_typeCombo.AddString(_T("电子图书"));
	m_typeCombo.SetCurSel(0);

	m_formatCombo.AddString(_T("pdf"));
	m_formatCombo.AddString(_T("txt"));
	m_formatCombo.AddString(_T("epub"));
	m_formatCombo.SetCurSel(0);

	m_manager.loadFile();
	RefreshList();
	UpdateTypeFields();
	SetEditMode(false);

	return TRUE;
}

void CLibraryDlg::OnCancel()
{
	m_manager.saveFile();
	CDialogEx::OnCancel();
}

bool CLibraryDlg::IsPaperType() const
{
	return m_typeCombo.GetCurSel() == 0;
}

CString CLibraryDlg::GetSelectedFormat() const
{
	CString format;
	const int sel = m_formatCombo.GetCurSel();
	if (sel >= 0) {
		m_formatCombo.GetLBText(sel, format);
	}
	return format;
}

void CLibraryDlg::SetFormatSelection(const CString& format)
{
	if (format == _T("txt")) {
		m_formatCombo.SetCurSel(1);
	}
	else if (format == _T("epub")) {
		m_formatCombo.SetCurSel(2);
	}
	else {
		m_formatCombo.SetCurSel(0);
	}
}

void CLibraryDlg::UpdateTypeFields()
{
	const bool paper = IsPaperType();
	SetDlgItemText(IDC_STATIC_EXTRA1, paper ? _T("页数：") : _T("格式："));
	GetDlgItem(IDC_EDIT4)->ShowWindow(paper ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_COMBO2)->ShowWindow(paper ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_STATIC_EXTRA2)->ShowWindow(paper ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_EDIT5)->ShowWindow(paper ? SW_HIDE : SW_SHOW);
	if (paper) {
		m_extra2.Empty();
	}
	else {
		m_extra1.Empty();
		if (m_formatCombo.GetCurSel() < 0) {
			m_formatCombo.SetCurSel(0);
		}
	}
	UpdateData(FALSE);
}

void CLibraryDlg::RefreshList()
{
	m_list.DeleteAllItems();
	for (int i = 0; i < m_manager.getBookCount(); ++i) {
		const Book* book = m_manager.getBookAt(i);
		if (!book) {
			continue;
		}

		CString typeText;
		CString pageText = _T("-");
		CString formatText = _T("-");
		CString sizeText = _T("-");

		if (book->getBookType() == "PaperBook") {
			typeText = _T("纸质");
			const auto* paper = dynamic_cast<const PaperBook*>(book);
			if (paper) {
				pageText.Format(_T("%d"), paper->getPageCount());
			}
		}
		else {
			typeText = _T("电子");
			const auto* ebook = dynamic_cast<const EBook*>(book);
			if (ebook) {
				formatText = StdToCString(ebook->getFormat());
				sizeText.Format(_T("%.1f"), ebook->getFileSize());
			}
		}

		CString priceText;
		priceText.Format(_T("%.2f"), book->getPrice());

		m_list.InsertItem(i, StdToCString(book->getBookID()));
		m_list.SetItemText(i, 1, StdToCString(book->getBookName()));
		m_list.SetItemText(i, 2, priceText);
		m_list.SetItemText(i, 3, typeText);
		m_list.SetItemText(i, 4, pageText);
		m_list.SetItemText(i, 5, formatText);
		m_list.SetItemText(i, 6, sizeText);
	}
	AutoSizeListColumns();
}

void CLibraryDlg::AutoSizeListColumns()
{
	static const int minWidths[] = { 90, 120, 70, 70, 60, 60, 80 };
	for (int i = 0; i < 7; ++i) {
		m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		if (m_list.GetColumnWidth(i) < minWidths[i]) {
			m_list.SetColumnWidth(i, minWidths[i]);
		}
	}
}

CString CLibraryDlg::FormatBookInfo(const Book* book) const
{
	if (!book) {
		return _T("");
	}

	CString info;
	info.Format(_T("编号：%s\r\n名称：%s\r\n价格：%.2f\r\n类型：%s"),
		StdToCString(book->getBookID()).GetString(),
		StdToCString(book->getBookName()).GetString(),
		book->getPrice(),
		book->getBookType() == "PaperBook" ? _T("纸质图书") : _T("电子图书"));

	if (book->getBookType() == "PaperBook") {
		const auto* paper = dynamic_cast<const PaperBook*>(book);
		if (paper) {
			CString extra;
			extra.Format(_T("\r\n页数：%d"), paper->getPageCount());
			info += extra;
		}
	}
	else {
		const auto* ebook = dynamic_cast<const EBook*>(book);
		if (ebook) {
			CString extra;
			extra.Format(_T("\r\n格式：%s\r\n大小：%.1f MB"),
				StdToCString(ebook->getFormat()).GetString(),
				ebook->getFileSize());
			info += extra;
		}
	}
	return info;
}

void CLibraryDlg::ClearInputs()
{
	m_id.Empty();
	m_name.Empty();
	m_price.Empty();
	m_extra1.Empty();
	m_extra2.Empty();
	m_typeCombo.SetCurSel(0);
	m_formatCombo.SetCurSel(0);
	m_selectedIndex = -1;
	UpdateTypeFields();
	UpdateData(FALSE);
}

void CLibraryDlg::SetEditMode(bool editing)
{
	GetDlgItem(IDC_EDIT1)->EnableWindow(!editing);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(!editing);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(editing);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(editing);
}

void CLibraryDlg::LoadBookToInputs(int index)
{
	const Book* book = m_manager.getBookAt(index);
	if (!book) {
		return;
	}

	m_id = StdToCString(book->getBookID());
	m_name = StdToCString(book->getBookName());
	m_price.Format(_T("%.2f"), book->getPrice());

	if (book->getBookType() == "PaperBook") {
		m_typeCombo.SetCurSel(0);
		const auto* paper = dynamic_cast<const PaperBook*>(book);
		if (paper) {
			m_extra1.Format(_T("%d"), paper->getPageCount());
		}
		m_extra2.Empty();
	}
	else {
		m_typeCombo.SetCurSel(1);
		const auto* ebook = dynamic_cast<const EBook*>(book);
		if (ebook) {
			SetFormatSelection(StdToCString(ebook->getFormat()));
			m_extra2.Format(_T("%.1f"), ebook->getFileSize());
		}
	}

	m_selectedIndex = index;
	UpdateTypeFields();
	UpdateData(FALSE);
	SetEditMode(true);
}

void CLibraryDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if ((pNMLV->uChanged & LVIF_STATE) == 0) {
		*pResult = 0;
		return;
	}

	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_list.ScreenToClient(&point);

	LVHITTESTINFO lvinfo{};
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;
	const int hitItem = m_list.SubItemHitTest(&lvinfo);

	if (hitItem != LB_ERR) {
		for (int i = 0; i < m_list.GetItemCount(); ++i) {
			if (m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
				LoadBookToInputs(i);
				break;
			}
		}
	}
	else {
		ClearInputs();
		SetEditMode(false);
	}

	*pResult = 0;
}

void CLibraryDlg::OnCbnSelchangeCombo1()
{
	UpdateTypeFields();
}

void CLibraryDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	if (m_id.IsEmpty()) {
		MessageBox(_T("编号不能为空！"), _T("添加失败"), MB_ICONWARNING);
		return;
	}
	if (m_name.IsEmpty()) {
		MessageBox(_T("名称不能为空！"), _T("添加失败"), MB_ICONWARNING);
		return;
	}
	if (m_price.IsEmpty()) {
		MessageBox(_T("价格不能为空！"), _T("添加失败"), MB_ICONWARNING);
		return;
	}

	const double price = _ttof(m_price);
	bool ok = false;

	if (IsPaperType()) {
		if (m_extra1.IsEmpty()) {
			MessageBox(_T("页数不能为空！"), _T("添加失败"), MB_ICONWARNING);
			return;
		}
		const int pageCount = _ttoi(m_extra1);
		ok = m_manager.addBook(CStringToStd(m_id), CStringToStd(m_name), price, pageCount);
	}
	else {
		if (m_formatCombo.GetCurSel() < 0 || m_extra2.IsEmpty()) {
			MessageBox(_T("请选择格式并填写文件大小！"), _T("添加失败"), MB_ICONWARNING);
			return;
		}
		const double fileSize = _ttof(m_extra2);
		ok = m_manager.addBook(CStringToStd(m_id), CStringToStd(m_name), price,
			CStringToStd(GetSelectedFormat()), fileSize);
	}

	if (!ok) {
		MessageBox(_T("添加失败，请检查输入是否合法或编号是否重复。"), _T("添加失败"), MB_ICONWARNING);
		return;
	}

	RefreshList();
	ClearInputs();
	SetEditMode(false);
	MessageBox(_T("添加成功！"), _T("提示"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton2()
{
	if (m_selectedIndex < 0) {
		MessageBox(_T("请先选择要修改的图书。"), _T("修改失败"), MB_ICONWARNING);
		return;
	}

	UpdateData(TRUE);

	if (m_name.IsEmpty() || m_price.IsEmpty()) {
		MessageBox(_T("名称和价格不能为空！"), _T("修改失败"), MB_ICONWARNING);
		return;
	}

	const double price = _ttof(m_price);
	bool ok = false;

	if (IsPaperType()) {
		if (m_extra1.IsEmpty()) {
			MessageBox(_T("页数不能为空！"), _T("修改失败"), MB_ICONWARNING);
			return;
		}
		const int pageCount = _ttoi(m_extra1);
		ok = m_manager.updatePaperBookAt(m_selectedIndex, CStringToStd(m_id),
			CStringToStd(m_name), price, pageCount);
	}
	else {
		if (m_formatCombo.GetCurSel() < 0 || m_extra2.IsEmpty()) {
			MessageBox(_T("请选择格式并填写文件大小！"), _T("修改失败"), MB_ICONWARNING);
			return;
		}
		const double fileSize = _ttof(m_extra2);
		ok = m_manager.updateEBookAt(m_selectedIndex, CStringToStd(m_id),
			CStringToStd(m_name), price, CStringToStd(GetSelectedFormat()), fileSize);
	}

	if (!ok) {
		MessageBox(_T("修改失败，请检查输入是否合法或编号是否重复。"), _T("修改失败"), MB_ICONWARNING);
		return;
	}

	RefreshList();
	ClearInputs();
	SetEditMode(false);
	MessageBox(_T("修改成功！"), _T("提示"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton3()
{
	if (m_selectedIndex < 0) {
		MessageBox(_T("请先选择要删除的图书。"), _T("删除失败"), MB_ICONWARNING);
		return;
	}

	UpdateData(TRUE);
	if (!m_manager.deleteBook(CStringToStd(m_id))) {
		MessageBox(_T("删除失败！"), _T("删除失败"), MB_ICONWARNING);
		return;
	}

	RefreshList();
	ClearInputs();
	SetEditMode(false);
	MessageBox(_T("删除成功！"), _T("提示"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton4()
{
	CString filter = _T("文本文档(*.txt)|*.txt|所有文件(*.*)|*.*||");
	CFileDialog saveDlg(FALSE, _T("txt"), _T("books"), OFN_OVERWRITEPROMPT, filter, this);
	if (saveDlg.DoModal() != IDOK) {
		return;
	}

	if (!m_manager.saveFile(CStringToStd(saveDlg.GetPathName()))) {
		MessageBox(_T("文件保存失败！"), _T("保存失败"), MB_ICONWARNING);
		return;
	}

	MessageBox(_T("文件保存成功！"), _T("保存成功"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton5()
{
	if (!m_manager.sortBooks()) {
		MessageBox(_T("当前没有图书可排序。"), _T("排序"), MB_ICONINFORMATION);
		return;
	}

	RefreshList();
	MessageBox(_T("已按价格升序排序。"), _T("排序成功"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton6()
{
	UpdateData(TRUE);
	if (m_id.IsEmpty()) {
		MessageBox(_T("请输入要查询的图书编号！"), _T("查询"), MB_ICONWARNING);
		return;
	}

	const Book* book = m_manager.findBookById(CStringToStd(m_id));
	if (!book) {
		MessageBox(_T("未找到该编号的图书！"), _T("查询结果"), MB_ICONINFORMATION);
		return;
	}

	MessageBox(FormatBookInfo(book), _T("查询结果"), MB_ICONINFORMATION);
}

void CLibraryDlg::OnBnClickedButton7()
{
	CString filter = _T("文本文档(*.txt)|*.txt|所有文件(*.*)|*.*||");
	CFileDialog openDlg(TRUE, _T("txt"), _T("books"), OFN_FILEMUSTEXIST, filter, this);
	if (openDlg.DoModal() != IDOK) {
		return;
	}

	if (!m_manager.loadFile(CStringToStd(openDlg.GetPathName()))) {
		MessageBox(_T("文件加载失败或文件不存在！"), _T("加载失败"), MB_ICONWARNING);
		return;
	}

	RefreshList();
	ClearInputs();
	SetEditMode(false);
	MessageBox(_T("文件加载成功！"), _T("加载成功"), MB_ICONINFORMATION);
}
