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

string query = "create table testtable(attr char(10), test int not null unique, here float not null unique, primary key(test))";
// string query = "create index idname on tbname(asdkhfjabldkfjykugl)";
// string query = "insert into tbname values(1, \"12\", 23, 3.3, \"NULLSTR\")";
// string query = "select a.attr b.attr from table1 as a, table2 as b where a.v1 = b.v2";
// string query = "select attr1, attr2 from table where v1 = 12";
// string query = "Drop Index Testname2";

int main(){
    // string text="abc";
    // cout<<text[ text.length() - 1]<<endl;
    Interpreter I(query);
    I.Parse();
    // cout<<stof("3.3")<<endl;
    system("pause");
    return 0;
}
