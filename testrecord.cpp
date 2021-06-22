#include <iostream>
#include "RecordManager.h"
#include "MiniSQL.h"

int main()
{
    RecordManager rmanager;

    //创建Table
    cout << "待插入的表信息" << endl;
    TableMetadata metadata;
    vector<Attribute> attrs;
    metadata.name = "recordtest";
    metadata.attr_num = 0;
    Attribute attr1("attr1", "int");
    Attribute attr2("attr2", "float");
    attrs.push_back(attr1);
    attrs.push_back(attr2);
    Table table(metadata, attrs);
    table.Print();
    cout << endl;

    //创建Tuple
    cout << "待插入的元组信息" << endl;
    Tuple tuple;
    tuple.valid = 1;
    Value value1, value2;
    value1.int_value = 10;
    value2.float_value = 5.0;
    Unit unit1(value1, INT_UNIT);
    Unit unit2(value2, FLOAT_UNIT);
    tuple.tuple_value.push_back(unit1);
    tuple.tuple_value.push_back(unit2);
    tuple.Print();
    cout << endl;

    // rmanager.CreateTableFile(table);
    // cin >> cont;
    cout << "------insert---------------" << endl;
    rmanager.InsertTuple(table, tuple);
    table.Print();
    cout << endl;
    
    cout << "------select---------------" << endl;
    vector<Tuple> results(rmanager.SelectTuple(table));
    Tuple result(*results.begin());
    result.Print();
    cout << endl;
    return 0;
}