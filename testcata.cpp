#include <iostream>
#include "CatalogManager.h"
#include "MiniSQL.h"

int main()
{
    vector<Table*> vt;
    CatalogManager catalog;
    Table t;
Tuple data;
data.valid = 1;
Unit s;
Value m;
string v("123");
m.char_n_value = v.c_str();
s.datatype = DataType::INT_UNIT;
s.value = m;
data.tuple_value.push_back(s);
string name = "a";
    catalog.InsertTest(name, data);
    // t.m_metadata.attr_num=1;
    // t.m_metadata.name="fusing";
    // Attribute s;
    // s.name = "ring";
    // s.charlen = 0;
    // s.notnull = 1;
    // s.type = DataType::INT_UNIT;
    // vt.push_back(&t);
    // t.m_attribute.push_back(s);
    // catalog.CreateTable(t);
    // Index ind;
    // ind.attr_num = 1;
    // ind.index_name = "pfu";
    // ind.table_name = "fusing";
    // catalog.CreateIndex(ind);
    
}