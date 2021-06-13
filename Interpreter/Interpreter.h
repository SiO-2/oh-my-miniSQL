#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <string>
using namespace std;
// sql解释器
class Interpreter{
public:
    string sql; // sql语句字符串
    Interpreter(std::string sql); //构造函数
    void Parse(); // 解析sql的函数
    void CreateTable(std::string str); // 针对create table 场景的函数
    void CreateIndex(string str); // 针对create index 场景的函数
    void Insert(string str); // 针对insert 场景的函数
    void Select(string str); // 针对select 场景的函数
};

#endif