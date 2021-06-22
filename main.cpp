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

// string query = "create table testtable(charattr char(10), intattr int not null unique, floatattr float not null unique, primary key(intattr))";
// string query = "Drop Table testtable";
string query = "insert into testtable values(\"chartest\", 10, 10.5)";
// 上面三个都可以通过和Catalog的测试了
// string query = "select intattr, floatattr from testtable";
// string query = "create index idname on tbname(asdkhfjabldkfjykugl)";
// string query = "select a.attr b.attr from table1 as a, table2 as b where a.v1 = b.v2";

int main(int argc, char *argv[])
{
    // string text="abc";
    // cout<<text[ text.length() - 1]<<endl;
    if (argc == 2)
    {
        query = argv[1];
    }
    Interpreter I(query);
    I.Parse();
    // cout<<stof("3.3")<<endl;
    delete &I;
    system("pause");
    return 0;
}
