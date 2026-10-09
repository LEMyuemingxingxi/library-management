#pragma once
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// 抽象基类：图书
class Book {
private:
    string bookID;   // 图书编号（唯一）
    string bookName; // 图书名称
    double price;    // 价格

public:
    virtual ~Book();

    string getBookID() const;
    string getBookName() const;
    double getPrice() const;

    // Setter 带合法性检查，返回 true 表示设置成功
    bool setBookID(const string& id);
    bool setBookName(const string& name);
    bool setPrice(double p);

    virtual void showInfo() const = 0;       // 纯虚函数，派生类必须实现
    virtual string getBookType() const = 0;    // 纯虚函数，返回图书类型
    virtual void saveToFile(ofstream& ofs) const;
    bool operator<(const Book& other) const;
    friend ostream& operator<<(ostream& os, const Book& book);

protected:
    Book(const string& bookID = "", const string& bookName = "", double price = 0.0); // protected 防止直接创建抽象类对象
};

// 纸质图书
class PaperBook : public Book {
private:
    int pageCount; // 页数

public:
    PaperBook(const string& bookID = "", const string& bookName = "", double price = 0.0, int pageCount = 0);
    ~PaperBook() override;

    int getPageCount() const;
    bool setPageCount(int count);

    void showInfo() const override;
    string getBookType() const override;
    void saveToFile(ofstream& ofs) const override;
};

// 电子图书
class EBook : public Book {
private:
    string format;   // 格式：pdf / txt / epub
    double fileSize; // 文件大小（MB）

public:
    EBook(const string& bookID = "", const string& bookName = "", double price = 0.0,
        const string& format = "", double fileSize = 0.0);
    ~EBook() override;

    string getFormat() const;
    double getFileSize() const;
    bool setFormat(const string& fmt);
    bool setFileSize(double size);

    void showInfo() const override;
    string getBookType() const override;
    void saveToFile(ofstream& ofs) const override;
};
