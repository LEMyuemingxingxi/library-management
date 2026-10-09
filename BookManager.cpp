#include "BookManager.h"
#include "Book.h"
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

BookManager::BookManager() {}

BookManager::~BookManager() {}

void BookManager::rebuildIndex() {
    bookIndex.clear();
    for (int i = 0; i < static_cast<int>(bookList.size()); ++i) {
        bookIndex[bookList[i]->getBookID()] = i;
    }
}

bool BookManager::addBook(const string& bookID, const string& bookName, double price, int pageCount) {
    if (bookID.empty() || bookName.empty()) {
        cout << "错误：图书编号和名称不能为空！" << endl;
        return false;
    }
    if (price < 0) {
        cout << "错误：价格不能为负数！" << endl;
        return false;
    }
    if (pageCount <= 0) {
        cout << "错误：页数必须大于 0！" << endl;
        return false;
    }
    if (isBookIDExist(bookID)) {
        cout << "错误：图书编号已存在！" << endl;
        return false;
    }

    unique_ptr<Book> book = make_unique<PaperBook>(bookID, bookName, price, pageCount);
    bookList.push_back(move(book));
    bookIndex[bookID] = static_cast<int>(bookList.size()) - 1;
    cout << "纸质图书添加成功！" << endl;
    return true;
}

bool BookManager::addBook(const string& bookID, const string& bookName, double price,
    const string& format, double fileSize) {
    if (bookID.empty() || bookName.empty()) {
        cout << "错误：图书编号和名称不能为空！" << endl;
        return false;
    }
    if (price < 0) {
        cout << "错误：价格不能为负数！" << endl;
        return false;
    }
    if (format != "pdf" && format != "txt" && format != "epub") {
        cout << "错误：格式只能是 pdf、txt 或 epub！" << endl;
        return false;
    }
    if (fileSize <= 0) {
        cout << "错误：文件大小必须大于 0！" << endl;
        return false;
    }
    if (isBookIDExist(bookID)) {
        cout << "错误：图书编号已存在！" << endl;
        return false;
    }

    unique_ptr<Book> book = make_unique<EBook>(bookID, bookName, price, format, fileSize);
    bookList.push_back(move(book));
    bookIndex[bookID] = static_cast<int>(bookList.size()) - 1;
    cout << "电子图书添加成功！" << endl;
    return true;
}

bool BookManager::deleteBook(const string& id) {
    if (id.empty()) {
        cout << "错误：图书编号不能为空！" << endl;
        return false;
    }
    if (!isBookIDExist(id)) {
        cout << "错误：图书编号不存在！" << endl;
        return false;
    }

    bookList.erase(bookList.begin() + bookIndex[id]);
    rebuildIndex();
    cout << "图书删除成功！" << endl;
    return true;
}

void BookManager::searchBook(const string& id) {
    if (id.empty()) {
        cout << "错误：图书编号不能为空！" << endl;
        return;
    }
    if (!isBookIDExist(id)) {
        cout << "错误：图书编号不存在！" << endl;
        return;
    }

    // 多态调用：基类指针 -> 派生类 showInfo()
    Book* book = bookList[bookIndex[id]].get();
    book->showInfo();
}

void BookManager::showAllBooks() {
    if (bookList.empty()) {
        cout << "当前没有图书记录。" << endl;
        return;
    }
    cout << "========== 全部图书（共 " << bookList.size() << " 本）==========" << endl;
    for (const auto& book : bookList) {
        book->showInfo(); // 多态调用
    }
    cout << "============================================" << endl;
}

bool BookManager::sortBooks() {
    if (bookList.empty()) {
        cout << "当前没有图书可排序。" << endl;
        return false;
    }
    sort(bookList.begin(), bookList.end(), // STL sort 算法
        [](const unique_ptr<Book>& a, const unique_ptr<Book>& b) {
            return *a < *b;
        });
    rebuildIndex(); // 排序后下标变化，需重建 map 索引
    cout << "已按价格升序排序完成！" << endl;
    return true;
}

bool BookManager::saveFile(const string& filepath) {
    ofstream ofs(filepath);
    if (!ofs.is_open()) {
        cout << "错误：无法打开 books.txt 进行写入！" << endl;
        return false;
    }

    for (const auto& book : bookList) {
        book->saveToFile(ofs);
    }
    ofs.close();

    if (ofs.fail()) {
        cout << "错误：写入文件时发生异常！" << endl;
        return false;
    }

    cout << "已成功保存 " << bookList.size() << " 本图书到 books.txt。" << endl;
    return true;
}

bool BookManager::loadFile(const string& filepath) {
    ifstream ifs(filepath);
    if (!ifs.is_open()) {
        cout << "提示：未找到 books.txt，将以空列表启动。" << endl;
        return false;
    }

    bookList.clear();
    bookIndex.clear();

    string type, bookID, bookName;
    double price;
    int loaded = 0;

    while (ifs >> type >> bookID >> bookName >> price) { // 文件格式：类型 ID 名称 价格 ...
        if (type == "PaperBook") {
            int pageCount;
            if (!(ifs >> pageCount)) {
                cout << "警告：纸质图书数据格式错误，跳过该行。" << endl;
                continue;
            }
            if (!isBookIDExist(bookID)) {
                unique_ptr<Book> book = make_unique<PaperBook>(bookID, bookName, price, pageCount);
                bookList.push_back(move(book));
                bookIndex[bookID] = static_cast<int>(bookList.size()) - 1;
                ++loaded;
            }
        }
        else if (type == "EBook") {
            string format;
            double fileSize;
            if (!(ifs >> format >> fileSize)) {
                cout << "警告：电子图书数据格式错误，跳过该行。" << endl;
                continue;
            }
            if (!isBookIDExist(bookID)) {
                unique_ptr<Book> book = make_unique<EBook>(bookID, bookName, price, format, fileSize);
                bookList.push_back(move(book));
                bookIndex[bookID] = static_cast<int>(bookList.size()) - 1;
                ++loaded;
            }
        }
        else {
            cout << "警告：未知图书类型 \"" << type << "\"，跳过该行。" << endl;
        }
    }

    if (ifs.bad()) {
        cout << "错误：读取文件时发生 IO 异常！" << endl;
        return false;
    }

    cout << "已从 books.txt 加载 " << loaded << " 本图书。" << endl;
    return true;
}

bool BookManager::isBookIDExist(const string& id) const {
    return bookIndex.find(id) != bookIndex.end();
}

int BookManager::getBookCount() const {
    return static_cast<int>(bookList.size());
}

const Book* BookManager::getBookAt(int index) const {
    if (index < 0 || index >= static_cast<int>(bookList.size())) {
        return nullptr;
    }
    return bookList[index].get();
}

const Book* BookManager::findBookById(const string& id) const {
    const auto it = bookIndex.find(id);
    if (it == bookIndex.end()) {
        return nullptr;
    }
    return bookList[it->second].get();
}

bool BookManager::updatePaperBookAt(int index, const string& bookID, const string& bookName,
    double price, int pageCount) {
    if (index < 0 || index >= static_cast<int>(bookList.size())) {
        return false;
    }
    if (bookID.empty() || bookName.empty() || price < 0 || pageCount <= 0) {
        return false;
    }
    const string oldId = bookList[index]->getBookID();
    if (bookID != oldId && isBookIDExist(bookID)) {
        return false;
    }

    bookList[index] = make_unique<PaperBook>(bookID, bookName, price, pageCount);
    rebuildIndex();
    return true;
}

bool BookManager::updateEBookAt(int index, const string& bookID, const string& bookName,
    double price, const string& format, double fileSize) {
    if (index < 0 || index >= static_cast<int>(bookList.size())) {
        return false;
    }
    if (bookID.empty() || bookName.empty() || price < 0) {
        return false;
    }
    if (format != "pdf" && format != "txt" && format != "epub") {
        return false;
    }
    if (fileSize <= 0) {
        return false;
    }
    const string oldId = bookList[index]->getBookID();
    if (bookID != oldId && isBookIDExist(bookID)) {
        return false;
    }

    bookList[index] = make_unique<EBook>(bookID, bookName, price, format, fileSize);
    rebuildIndex();
    return true;
}
