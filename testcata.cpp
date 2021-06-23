#include <iostream>
#include "CatalogManager.h"
#include "MiniSQL.h"

int main()
{
    vector<Table*> vt;
    CatalogManager catalog;
    Table t;
    t.m_metadata.attr_num=1;
    t.m_metadata.name="fusing";
    Attribute s;
    s.name = "ring";
    s.charlen = 0;
    s.notnull = 1;
    s.type = DataType::INT_UNIT;
    vt.push_back(&t);
    t.m_attribute.push_back(s);
    catalog.CreateTable(t);
    
}