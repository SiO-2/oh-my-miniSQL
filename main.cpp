// #include <Python.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include "Interpreter.h"
// #include "Basicop.h"
// #include "Attribute.h"

using namespace std;

// string query = "create table muring(charattr char(10), intattr int not null unique, floatattr float not null unique, primary key(intattr))";
string query = "Drop Table muring";
// string query = "insert into testtable values(\"chartest\", 10, 10.5)";
// 上面三个都可以通过和Catalog的测试了
// string query = "create index idname on tbname(asdkhfjabldkfjykugl)";
// string query = "select a.attr b.attr from table1 as a, table2 as b where a.v1 = b.v2";
// string query = "select intattr, floatattr from table where intattr = 12";

int main(){
    // string text="abc";
    // cout<<text[ text.length() - 1]<<endl;
    Interpreter I(query);
    I.Parse();
    // cout<<stof("3.3")<<endl;
    system("pause");
    return 0;
}
