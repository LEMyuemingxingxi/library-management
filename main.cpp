// 控制台版主程序（符合作业菜单 1-8 要求，供对照/答辩使用）
// 当前 MFC 工程默认编译图形界面版；如需控制台版，可将本文件加入项目并改为 Console 子系统。

#include <iostream>
#include <limits>
#include <string>
#include "BookManager.h"
#include "ConsoleUtil.h"
using namespace std;

BookManager bookManager;

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool readInt(int& value, const string& prompt) {
    cout << prompt;
    if (!(cin >> value)) {
        clearInputBuffer();
        cout << "错误：请输入有效的整数！" << endl;
        return false;
    }
    return true;
}

bool readDouble(double& value, const string& prompt) {
    cout << prompt;
    if (!(cin >> value)) {
        clearInputBuffer();
        cout << "错误：请输入有效的数字！" << endl;
        return false;
    }
    return true;
}

bool readString(string& value, const string& prompt) {
    cout << prompt;
    cin >> value;
    if (value.empty()) {
        cout << "错误：输入不能为空！" << endl;
        return false;
    }
    return true;
}

void showMenu() {
    cout << "\n==================== 图书管理系统 ====================" << endl;
    cout << "当前图书数量：" << bookManager.getBookCount() << " 本" << endl;
    cout << "请选择操作：" << endl;
    cout << "  1. 添加图书（纸质图书 / 电子图书）" << endl;
    cout << "  2. 显示所有图书信息" << endl;
    cout << "  3. 根据图书编号查询图书" << endl;
    cout << "  4. 根据图书编号删除图书" << endl;
    cout << "  5. 对图书按价格升序排序" << endl;
    cout << "  6. 保存所有图书到文件（books.txt）" << endl;
    cout << "  7. 从文件加载所有图书" << endl;
    cout << "  8. 退出系统" << endl;
    cout << "======================================================" << endl;
    cout << "请输入操作序号：";
}

int main() {
    setupConsoleUTF8();
    cout << "欢迎使用图书管理系统！" << endl;
    bookManager.loadFile();

    int choice = 0;
    while (true) {
        showMenu();

        if (!readInt(choice, "")) {
            continue;
        }

        switch (choice) {
        case 1: {
            int type;
            string bookID, bookName;
            double price;

            if (!readInt(type, "请选择图书类型（1-纸质图书，2-电子图书）：")) break;
            if (!readString(bookID, "请输入图书编号：")) break;
            if (!readString(bookName, "请输入图书名称：")) break;
            if (!readDouble(price, "请输入价格：")) break;

            if (type == 1) {
                int pageCount;
                if (!readInt(pageCount, "请输入页数：")) break;
                bookManager.addBook(bookID, bookName, price, pageCount);
            }
            else if (type == 2) {
                string format;
                double fileSize;
                if (!readString(format, "请输入格式（pdf/txt/epub）：")) break;
                if (!readDouble(fileSize, "请输入文件大小（MB）：")) break;
                bookManager.addBook(bookID, bookName, price, format, fileSize);
            }
            else {
                cout << "错误：无效的图书类型，请输入 1 或 2！" << endl;
            }
            break;
        }
        case 2:
            bookManager.showAllBooks();
            break;
        case 3: {
            string id;
            if (!readString(id, "请输入图书编号：")) break;
            bookManager.searchBook(id);
            break;
        }
        case 4: {
            string id;
            if (!readString(id, "请输入图书编号：")) break;
            bookManager.deleteBook(id);
            break;
        }
        case 5:
            bookManager.sortBooks();
            break;
        case 6:
            bookManager.saveFile();
            break;
        case 7:
            bookManager.loadFile();
            break;
        case 8:
            bookManager.saveFile();
            cout << "感谢使用图书管理系统，再见！" << endl;
            return 0;
        default:
            cout << "错误：无效的操作序号，请输入 1-8！" << endl;
            break;
        }
    }

    return 0;
}
