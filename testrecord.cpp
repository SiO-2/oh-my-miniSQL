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
    metadata.attr_num = 3;
    Attribute attr1("attr1", "int");
    Attribute attr2("attr2", "float");
    Attribute attr3("attr3", "char");
    attrs.push_back(attr1);
    attrs.push_back(attr2);
    attrs.push_back(attr3);
    Table table(metadata, attrs);
    table.Print();
    cout << endl;

    //创建Tuple
    cout << "待插入的元组信息" << endl;
    Tuple tuple;
    tuple.valid = 1;
    Value value1, value2, value3;
    value1.int_value = 10;
    value2.float_value = 5.0;
    value3.char_n_value = "testrecord";
    Unit unit1(value1, INT_UNIT);
    Unit unit2(value2, FLOAT_UNIT);
    Unit unit3(value3,CHAR_UNIT);
    tuple.tuple_value.push_back(unit1);
    tuple.tuple_value.push_back(unit2);
    tuple.tuple_value.push_back(unit3);
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

    cout << result.tuple_value.begin()->value.int_value << "  ";
    cout << result.tuple_value.end()->value.float_value << endl;
    cout << endl;
    return 0;
}