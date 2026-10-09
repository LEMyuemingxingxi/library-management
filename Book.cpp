#include "Book.h"

Book::Book(const string& bookID, const string& bookName, double price)
    : bookID(bookID), bookName(bookName), price(price) {
}

Book::~Book() {}

string Book::getBookID() const { return bookID; }
string Book::getBookName() const { return bookName; }
double Book::getPrice() const { return price; }

bool Book::setBookID(const string& id) {
    if (id.empty()) {
        cout << "错误：图书编号不能为空！" << endl;
        return false;
    }
    bookID = id;
    return true;
}

bool Book::setBookName(const string& name) {
    if (name.empty()) {
        cout << "错误：图书名称不能为空！" << endl;
        return false;
    }
    bookName = name;
    return true;
}

bool Book::setPrice(double p) {
    if (p < 0) {
        cout << "错误：价格不能为负数！" << endl;
        return false;
    }
    price = p;
    return true;
}

void Book::saveToFile(ofstream& ofs) const {
    ofs << getBookType() << " " << getBookID() << " " << getBookName() << " " << getPrice();
}

bool Book::operator<(const Book& other) const {
    return getPrice() < other.getPrice(); // 供 sort 按价格排序
}

ostream& operator<<(ostream& os, const Book& book) {
    os << "[" << book.getBookType() << "] "
        << "编号:" << book.getBookID() << " "
        << "名称:" << book.getBookName() << " "
        << "价格:" << book.getPrice();
    return os;
}

PaperBook::PaperBook(const string& bookID, const string& bookName, double price, int pageCount)
    : Book(bookID, bookName, price), pageCount(pageCount) {
}

PaperBook::~PaperBook() {}

int PaperBook::getPageCount() const { return pageCount; }

bool PaperBook::setPageCount(int count) {
    if (count <= 0) {
        cout << "错误：页数必须大于 0！" << endl;
        return false;
    }
    pageCount = count;
    return true;
}

void PaperBook::showInfo() const {
    cout << *this << " 页数:" << getPageCount() << endl; // 重写基类纯虚函数
}

string PaperBook::getBookType() const { return "PaperBook"; }

void PaperBook::saveToFile(ofstream& ofs) const {
    Book::saveToFile(ofs);
    ofs << " " << getPageCount() << endl;
}

EBook::EBook(const string& bookID, const string& bookName, double price, const string& format, double fileSize)
    : Book(bookID, bookName, price), format(format), fileSize(fileSize) {
}

EBook::~EBook() {}

string EBook::getFormat() const { return format; }
double EBook::getFileSize() const { return fileSize; }

bool EBook::setFormat(const string& fmt) {
    if (fmt != "pdf" && fmt != "txt" && fmt != "epub") {
        cout << "错误：格式只能是 pdf、txt 或 epub！" << endl;
        return false;
    }
    format = fmt;
    return true;
}

bool EBook::setFileSize(double size) {
    if (size <= 0) {
        cout << "错误：文件大小必须大于 0！" << endl;
        return false;
    }
    fileSize = size;
    return true;
}

void EBook::showInfo() const {
    cout << *this << " 格式:" << getFormat() << " 大小:" << getFileSize() << "MB" << endl;
}

string EBook::getBookType() const { return "EBook"; }

void EBook::saveToFile(ofstream& ofs) const {
    Book::saveToFile(ofs);
    ofs << " " << getFormat() << " " << getFileSize() << endl;
}
