#pragma once
#include "Book.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
using namespace std;

// 图书管理类：负责增删查改、排序、文件读写
class BookManager {
private:
    vector<unique_ptr<Book>> bookList; // 多态容器：基类智能指针存放派生类对象
    map<string, int> bookIndex;        // 图书 ID -> 在 vector 中的下标

    void rebuildIndex(); // 重建 ID 索引

public:
    BookManager();
    ~BookManager();

    bool addBook(const string& bookID, const string& bookName, double price, int pageCount = 0);
    bool addBook(const string& bookID, const string& bookName, double price,
        const string& format, double fileSize);
    bool deleteBook(const string& id);
    void searchBook(const string& id);
    void showAllBooks();
    bool sortBooks();
    bool saveFile(const string& filepath = "books.txt");
    bool loadFile(const string& filepath = "books.txt");
    bool isBookIDExist(const string& id) const;
    int getBookCount() const;
    const Book* getBookAt(int index) const;
    const Book* findBookById(const string& id) const;
    bool updatePaperBookAt(int index, const string& bookID, const string& bookName, double price, int pageCount);
    bool updateEBookAt(int index, const string& bookID, const string& bookName, double price,
        const string& format, double fileSize);
};
