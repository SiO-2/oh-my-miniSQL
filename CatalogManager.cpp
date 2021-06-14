#include "CatalogManager.h"

bool CatalogManager::CreateDatabase(string& db_name)
{

}

//创建包含Catalog的表，需要检查重复性，返回值true表示成功，false表示失败
bool CatalogManager::CreateTable(struct Table& table)
{
    int n = m_table.size();
    for (int i=0; i<n; i++)
    {
        if (table.m_metadata.name == m_table[i].m_metadata.name)
            return false;
    }
    return true;
}

//创建索引，需要检查重复性，返回值true表示成功，false表示失败
bool CatalogManager::CreateIndex(struct Index& index)
{
    int n = m_index.size();
    for (int i=0; i<n; i++)
    {
        if (index.index_name == m_index[i].index_name)
            return false;
    }
    return true;
}

//删除表，通过table_name判断删除哪张表，需要检查存在性，返回值true表示成功，false表示失败
bool CatalogManager::DropTable(string& table_name)
{
    int n = m_table.size();
    for (int i=0; i<n; i++)
    {
        if (table_name == m_table[i].m_metadata.name)
            return true;
    }
    return false;
}

//删除索引，通过index_name判断删除哪个索引，需要检查存在性，返回值true表示成功，false表示失败
bool CatalogManager::DropIndex(string& index_name)
{
    int n = m_index.size();
    for (int i=0; i<n; i++)
    {
        if (index_name == m_index[i].index_name)
            return true;
    }
    return false;
}

//判断是否可以插入：检查每个DataUnit中的数据类型是否与表定义的数据类型匹配
bool CatalogManager::InsertTest(string& table_name, vector<struct DataUnit>& data)
{
    Table* t;
    int n = m_table.size(), i;
    for (i=0; i<n; i++)
    {
        if (table_name == m_table[i].m_metadata.name)
            break;
    }
    if (i==n) // 表不存在
        return false;
    t = &m_table[i];
    n = t->m_metadata.attr_num;
    for (i=0; i<n; i++)
    {
        if (!CheckAttr(t->m_attribute[i], data[i]))
            return false;
    }
    return true;
}

//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
//返回值：-2（表格不存在） -1（选择条件出错）；0（只能通过遍历Record查询）；1（可以利用索引优化查询）
int CatalogManager::SelectTest(string& table_name, vector<struct ConditionUnit>& condition)
{
    int i = FindTable(table_name), j;
    int flag = -1;
    if (i == -1)
        return -2;
    Table *t = &m_table[i];
    int n = condition.size();
    int m = t->m_attribute.size();
    for (i=0; i<n; i++)
    {
        condition[i].attr_num = m;
        for (j=0; j<m; j++)
        {
            if (condition[i].attr_name == t->m_attribute[j].name)
            {
                condition[i].attr_num = j;
            }
        }
        if (condition[i].attr_num == m)
            return -1;
        for (int k=0; k<m_index.size(); k++)
        {
            if (condition[i].attr_num == m_index[k].attr_num)
                flag = 1;
        }
    }
    return flag;
}

//获取表名为table_name的Catalog信息，如果不存在则返回空的Table
Table* CatalogManager::GetTableCatalog(string& table_name)
{
    int i = FindTable(table_name);
    if (i == -1)
        return ;
    Table *t = &Table(m_table[i]);
    return t;
}

//返回table_name表中的Index，如果不存在则返回空的Index
Index* CatalogManager::TableToIndex(string& table_name)
{
    int i = FindIndex(table_name);
    if (i == -1)
        return NULL;
    Index *I = &Index(m_index[i]);
    return I;
}

//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
//返回值：-2（表格不存在） -1（删除条件出错）；0（只能通过遍历Record删除）；1（可以利用索引优化删除）
int CatalogManager::DeleteTest(string& table_name, vector<struct ConditionUnit>& condition)
{
    int i = FindTable(table_name), j;
    int flag = -1;
    if (i == -1)
        return -2;
    Table *t = &m_table[i];
    int n = condition.size();
    int m = t->m_attribute.size();
    for (i=0; i<n; i++)
    {
        condition[i].attr_num = m;
        for (j=0; j<m; j++)
        {
            if (condition[i].attr_name == t->m_attribute[j].name)
            {
                condition[i].attr_num = j;
            }
        }
        if (condition[i].attr_num == m)
            return -1;
        for (int k=0; k<m_index.size(); k++)
        {
            if (condition[i].attr_num == m_index[k].attr_num)
                flag = 1;
        }
    }
    return flag;
}

//检测attr， 未定，根据需求改
bool CatalogManager::CheckAttr(Attribute& attr, DataUnit& data)
{
    if (attr.type != data.data_type)
        return false;
    return true;
}

//检测condition, 同上, 等一个需求
bool CatalogManager::CheckCond(ConditionUnit& cond)
{

}

int CatalogManager::FindIndex(string& index_name)
{
    int n = m_index.size(), i;
    for (i=0; i<n; i++)
    {
        if (index_name == m_table[i].m_metadata.name)
            return i;
    }
    return -1;
}

int CatalogManager::FindTable(string& table_name)
{
    int n = m_table.size(), i;
    for (i=0; i<n; i++)
    {
        if (table_name == m_table[i].m_metadata.name)
            return i;
    }
    return -1;
}