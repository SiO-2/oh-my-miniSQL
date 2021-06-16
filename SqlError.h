#ifndef _SQLERROR_H_
#define _SQLERROR_H_
#include <string>
using namespace std;

// Sql运行过程中的报错
class SqlError{
public:
    // string origin_string;
    // int pos;
    string msg;
    SqlError(string msg);
};

// 语法错误子类
class SyntaxError: public SqlError{
public:
    SyntaxError(string msg);
};

class DBError: public SqlError{
public:
    DBError(string msg);
};

#endif