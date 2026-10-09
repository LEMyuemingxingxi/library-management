# 图书管理系统

浙江大学《面向对象程序设计》课程大程。用现代 C++ 统一管理纸质书和电子书。

## 功能

- 添加、显示、按编号查询、按编号删除、按价格排序
- 保存到 `books.txt`，下次启动可以继续用
- 控制台菜单，以及 MFC 图形界面

## 设计

- `Book`：抽象基类，编号、名称、价格和纯虚接口
- `PaperBook`：纸质书，增加页数
- `EBook`：电子书，增加格式和文件大小
- `BookManager`：用 `vector<unique_ptr<Book>>` 存放图书，用编号索引做增删查改

## 编译

用 Visual Studio 打开 `library.slnx` 后编译运行。仓库里没有可执行文件；如果本地有改过后缀的 `library.exe1`，改回 `library.exe` 也可以直接运行。
