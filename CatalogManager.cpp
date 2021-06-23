#include "CatalogManager.h"
#include <fstream>

//创建，此时将索引信息读入内存；
CatalogManager::CatalogManager()
{
    fstream table_file, index_file;
    table_file.open(table_name, ios::in|ios::binary);
    index_file.open(index_name, ios::in|ios::binary);
    int table_n, index_n;
    if (table_file)
    {
        readint(table_n, table_file);
        Table* t;
        for (int i=0; i<table_n; i++)
        {
            t = readTable(table_file);
            m_table.push_back(t);
        }
        table_file.close();
    }
    else
    {
        table_file.open(table_name, ios::out|ios::binary);
        int n = 0;
        writeint(n, table_file);
        table_file.close();
    }
    if (index_file)
    {
        readint(index_n, index_file);
        Index* I;
        for (int i= 0; i<index_n; i++)
        {
            I = new Index;
            readIndex(*I, index_file);
            m_index.push_back(I);
        }
        index_file.close();
    }
    else
    {
        index_file.open(index_name, ios::out|ios::binary);
        int n=0;
        writeint(n, index_file);
        index_file.close();
    }
    // by wyc print to debug:
    // cout<<"[Catalog]: table list"<<endl;
    // for(auto table:this->m_table){
    //     table->Print();
    // }
    // cout<<"[catalog]: end of table list"<<endl;

}

//创建包含Catalog的表，需要检查重复性，返回值true表示成功，false表示失败
bool CatalogManager::CreateTable(Table& table)
{
    fstream table_file;
    int n = m_table.size();
    for (int i=0; i<n; i++)
    {
        if (table.m_metadata.name == m_table[i]->m_metadata.name)
            return false;
    }
    table_file.open(table_name, ios::out|ios::binary);
    // m_table[n] = table;
    m_table.push_back(&table);
    // writeTable(&table, table_file);
    writeallTable(table_file);
    table_file.close();
    table_file.open(NameToTF(table.m_metadata.name), ios::out|ios::binary);
    table_file.close();
    return true;
}

//创建索引，需要检查重复性，返回值true表示成功，false表示失败
bool CatalogManager::CreateIndex(Index& index)
{
    fstream index_file;
    int n = m_index.size();
    for (int i=0; i<n; i++)
    {
        if (index.index_name == m_index[i]->index_name)
            return false;
    }
    m_index.push_back(&index);
    index_file.open(index_name, ios::out|ios::binary);
    writeallIndex(index_file);
    // m_index[n] = index;
    index_file.close();
    index_file.open(NameToIF(index.index_name), ios::out|ios::binary);
    index_file.close();
    return true;
}

//删除表，通过name判断删除哪张表，需要检查存在性，返回值true表示成功，false表示失败
bool CatalogManager::DropTable(string& name)
{
    int n = m_table.size();
    fstream table_file;
    for (int i=0; i<n; i++)
    {
        if (name == m_table[i]->m_metadata.name){
            table_file.open(NameToTF(table_name), ios::out|ios::binary);
            m_table.erase(m_table.begin()+i);
            writeallTable(table_file);
            table_file.close();
            remove(NameToTF(name).c_str());
            return true;
        }
    }
    return false;
}

//删除索引，通过name判断删除哪个索引，需要检查存在性，返回值true表示成功，false表示失败
bool CatalogManager::DropIndex(string& name)
{
    int n = m_index.size();
    fstream index_file;
    for (int i=0; i<n; i++)
    {
        if (name == m_index[i]->index_name){
            index_file.open(NameToIF(index_name), ios::out|ios::binary);
            m_index.erase(m_index.begin()+i);
            writeallIndex(index_file);
            index_file.close();
            remove(NameToIF(name).c_str());
            return true;
        }
    }
    return false;
}

//这里有table不需要传入attribute to：wyc
//判断是否可以插入：检查每个DataUnit中的数据类型是否与表定义的数据类型匹配
bool CatalogManager::InsertTest(string& table_name, Tuple& data)
{
    Table* t;
    int n = m_table.size(), i;
    for (i=0; i<n; i++)
    {
        if (table_name == m_table[i]->m_metadata.name)
            break;
    }
    if (i==n) // 表不存在
        return false;
    t = m_table[i];
    n = t->m_metadata.attr_num;
    // wyc test
    // cout<<"[Catalog debug]:"<<endl;
    // for(auto attr: t->m_attribute){
    //     attr.Print();
    // }
    // cout<<"[Catalog debug]: end"<<endl;
    // // end of wyc test
    for (i=0; i<n; i++)
    {
        if (!CheckAttr(t->m_attribute[i], data.tuple_value[i]))
            return false;
    }
    return true;
}

//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
//返回值：-2（表格不存在） -1（选择条件出错）；0（只能通过遍历Record查询）；1（可以利用索引优化查询）
pair<int, string> CatalogManager::SelectTest(string& table_name, vector<string>& Attr, vector<ConditionUnit>& condition)
{
    pair<int, string> ret;
    ret.first = 0;
    ret.second = "";
    int i = FindTable(table_name), j;
    if (i == -1) {
        ret.first = -2;
        return ret;
    }
    Table *t = m_table[i];
    int m, n, flag;
    n = t->m_attribute.size();
    m = Attr.size();
    for (i=0; i<m; i++)
    {
        flag = 0;
        for (int j=0; j<n; j++)
        {
            if (Attr[i] == t->m_attribute[j].name)
                flag = 1;
        }
        if (flag == 0)
        {
            ret.first = -1;
            return ret;
        }
    }

    n = condition.size();
    m = t->m_attribute.size();
    ret.second = t->m_metadata.name;
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
        if (condition[i].attr_num == m) {
            ret.first = -1;
            return ret;
        }
        for (int k=0; k<m_index.size(); k++)
        {
            if (condition[i].attr_num == m_index[k]->attr_num){
                ret.first = 1;
                ret.second = m_index[k]->index_name;
            }
        }
    }

    return ret;
}

//获取表名为table_name的Catalog信息，如果不存在则返回空的Table
Table* CatalogManager::GetTableCatalog(string& table_name)
{
    int i = FindTable(table_name);
    if (i == -1)
        return NULL;
    Table *t = m_table[i];
    return t;
}

//返回table_name表中的Index，如果不存在则返回空的Index
Index* CatalogManager::TableToIndex(string& table_name)
{
    int i = FindIndex(table_name);
    if (i == -1)
        return NULL;
    Index *I = m_index[i];
    return I;
}

//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
//返回值：-2（表格不存在） -1（删除条件出错）；0（只能通过遍历Record删除）；1（可以利用索引优化删除）
pair<int, string> CatalogManager::DeleteTest(string& table_name, vector<ConditionUnit>& condition)
{
    pair<int, string> ret;
    ret.first = 0;
    ret.second = "";
    int i = FindTable(table_name), j;
    if (i == -1) {
        ret.first = -2;
        return ret;
    }
    Table *t = m_table[i];
    int n = condition.size();
    int m = t->m_attribute.size();
    ret.second = t->m_metadata.name;
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
        {
            ret.first = -1;
            return ret;
        }
        for (int k=0; k<m_index.size(); k++)
        {
            if (condition[i].attr_num == m_index[k]->attr_num)
            {
                ret.first = 1;
                ret.second = m_index[k]->index_name;
            }
        }
    }
    return ret;
}

//检测attr， 未定，根据需求改
bool CatalogManager::CheckAttr(Attribute& attr, struct Unit& data)
{
    return attr.type == data.datatype;
}

//检测condition
bool CatalogManager::CheckCond(ConditionUnit& cond)
{
    return true;
}

int CatalogManager::FindIndex(string& index_name)
{
    int n = m_index.size(), i;
    for (i=0; i<n; i++)
    {
        if (index_name == m_table[i]->m_metadata.name)
            return i;
    }
    return -1;
}

int CatalogManager::FindTable(string& table_name)
{
    int n = m_table.size(), i;
    for (i=0; i<n; i++)
    {
        if (table_name == m_table[i]->m_metadata.name)
            return i;
    }
    return -1;
}

void CatalogManager::readint(int& t, fstream& f)
{
    f.read((char*)&t, sizeof(int));
}

void CatalogManager::readstring(string&s, fstream& f)
{
    int n;
    readint(n, f);
    char* a = new char[n];
    f.read(a, n);
    s = a;
    delete[] a;
}
	
void CatalogManager::readbool(bool&b, fstream& f)
{
    f.read((char*)&b, sizeof(bool));
}
	
void CatalogManager::readAttr(Attribute& a, fstream& f)
{
    readstring(a.name, f);
    readint(a.type, f);
    readint(a.charlen, f);
    readbool(a.notnull, f);
    readbool(a.unique, f);
    readbool(a.primary_key, f);
}

void CatalogManager::readIndex(Index& i, fstream& f)
{
    readstring(i.index_name, f);
    readint(i.attr_num, f);
}

void CatalogManager::writestring(string&s, fstream& f)
{
    int n = s.size() + 1;
    writeint(n, f);
    f.write(s.c_str(), n-1);
    char a = '\0';
    f.write(&a, 1);
}

void CatalogManager::writeint(int& i, fstream& f)
{
    f.write((char*)&i, sizeof(int));
}

void CatalogManager::writebool(bool&b, fstream& f)
{
    f.write((char*)&b, sizeof(bool));
}

void CatalogManager::writeAttr(Attribute& a, fstream& f)
{
    writestring(a.name, f);
    writeint(a.type, f);
    writeint(a.charlen, f);
    writebool(a.notnull, f);
    writebool(a.unique, f);
    writebool(a.primary_key, f);
}

Table* CatalogManager::readTable(fstream& f)
{
    TableMetadata m_metadata;	//表的定义信息
	vector<Attribute> m_attribute;	//表中字段的信息
    Attribute* attr;
    Table* t;
    string tname;
    int num;
    readstring(tname, f);
    m_metadata.name = tname;
    readint(num, f);
    m_metadata.attr_num = num;
    for (int j=0; j<num; j++)
    {
        attr = new Attribute;
        readAttr(*attr, f);
        m_attribute.push_back(*attr);
        // m_attribute[j] = *attr;
    }
    t = new Table(m_metadata, m_attribute);
    return t;
}

void CatalogManager::writeTable(Table* t, fstream& f)
{
    int num = t->m_metadata.attr_num;
    writestring(t->m_metadata.name, f);
    writeint(num, f);
    for (int i=0; i<num; i++)
        writeAttr(t->m_attribute[i], f);
}

void CatalogManager::writeIndex(Index* i, fstream& f)
{
    writestring(i->index_name, f);
    writeint(i->attr_num, f);
}

void CatalogManager::writeallTable(fstream& f)
{
    int n = m_table.size();
    writeint(n, f);
    for (int i=0; i<n; i++)
        writeTable(m_table[i], f);
}

void CatalogManager::writeallIndex(fstream& f)
{
    int n = m_index.size();
    writeint(n, f);
    for (int i=0; i<n; i++)
        writeIndex(m_index[i], f);
}

string CatalogManager::NameToTF(string& name)
{
    string tf = TABLE_PATH + name + TABLE_SUFFIX;
    return tf;
}

string CatalogManager::NameToIF(string& name)
{
    string inf = INDEX_PATH + name + INDEX_SUFFIX;
    return inf;
}