#ifndef _SQLERROR_H_
#define _SQLERROR_H_
#include <string>
using namespace std;

// Sql运行过程中的报错
class SqlError{
public:
    string msg;
    SqlError(string msg);
};

// 语法错误子类
class SyntaxError: public SqlError{
public:
    SyntaxError(string msg);
};

// 数据库返回错误
class DBError: public SqlError{
public:
    DBError(string msg);
};

// 内部程序运行错误
class InternalError: public SqlError{
public:
    InternalError(string msg);
};
#endif