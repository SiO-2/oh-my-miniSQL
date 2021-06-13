#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include "Interpreter.h"
#include "Basicop.h"
#include "Attribute.h"

using namespace std;

// string query = "create table testtable(attr char(10), test int not null unique, here float not null unique, primary key(attr))";
// string query = "create index idname on tbname(asdkhfjabldkfjykugl)";
// string query = "insert into tbname values(ksuydfglj)";
string query = "select a.attr b.attr from table1 as a, table2 as b where a.v1 = b.v2";

int main(){
    // string text="abc";
    // cout<<text[ text.length() - 1]<<endl;
    Interpreter I(query);
    I.Parse();
    // cout<<stoi("19")<<endl;
    system("pause");
    return 0;
}
