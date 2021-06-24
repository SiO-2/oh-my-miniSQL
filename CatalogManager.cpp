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
        readallTable(table_file);
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
        readallIndex(index_file);
        index_file.close();
    }
    else
    {
        index_file.open(index_name, ios::out|ios::binary);
        int n=0;
        writeint(n, index_file);
        index_file.close();
    }
    replace();
    // by wyc print to debug:
    // cout<<"[Catalog]: table list"<<endl;
    // for(auto table:this->m_table){
    //     table->Print();
    // }
    // cout<<"[catalog]: end of table list"<<endl;
}

CatalogManager::~CatalogManager()
{
    int n = m_table.size();
    for (int i=0; i<n; i++)
        delete m_table[i];
    n = m_index.size();
    for (int i=0; i<n; i++)
        delete m_index[i];
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
    Table* t = new Table(table);
    m_table.push_back(t);
    // writeTable(&table, table_file);
    writeallTable(table_file);

    table_file.close();
    table_file.open(NameToTF(table.m_metadata.name), ios::out|ios::binary);
    table_file.close();
    return true;
}

//创建索引，需要检查重复性，返回值1表示成功，0表示名称重复， -1表示对应的表不存在
int CatalogManager::CreateIndex(Index& index)
{
    fstream index_file;
    int n = m_index.size();
    for (int i=0; i<n; i++)
    {
        if (index.index_name == m_index[i]->index_name)
            return 0;
    }
    int i = FindTable(index.table_name);
    if (i == -1)
        return -1;
    index.table = m_table[i];
    index.table_name = m_table[i]->m_metadata.name;
    Index* t = new Index(index);
    t->table_name = m_table[i]->m_metadata.name;
    m_index.push_back(t);
    index_file.open(index_name, ios::out|ios::binary);
    writeallIndex(index_file);
    // m_index[n] = index;
    index_file.close();
    index_file.open(NameToIF(index.index_name), ios::out|ios::binary);
    index_file.close();
    m_table[i]->Index_name.push_back(t);
    return 1;
}

//删除表，通过name判断删除哪张表，需要检查存在性，返回值true表示成功，false表示失败
bool CatalogManager::DropTable(string& name)
{
    int n = m_table.size();
    fstream table_file;
    for (int i=0; i<n; i++)
    {
        if (name == m_table[i]->m_metadata.name){
            table_file.open(table_name, ios::out|ios::binary);
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
    // cout << "[catalog Drop index]"<<endl;
    int n = m_index.size();
    fstream index_file;
    for (int i=0; i<n; i++)
    {
        if (name == m_index[i]->index_name){
            string tname = m_index[i]->table_name;
            // cout << tname;
            int k = FindTable(tname);
            if (k==-1)
                return false;
            index_file.open(index_name, ios::out|ios::binary);
            // m_index.erase(m_index.begin()+i);
            swap(*(std::begin(m_index)+i),*(std::end(m_index)-1));
            // delete m_index[m_index.size()-1];
            m_index.pop_back();
            writeallIndex(index_file);
            index_file.close();
            remove(NameToIF(name).c_str());
            index_file.open(table_name, ios::out|ios::binary);
            for (int j = 0; j<m_table[k]->Index_name.size(); j++)
                if (m_table[k]->Index_name[j]->index_name==name)
                {
                    swap(*(std::begin(m_table[k]->Index_name)+j),*(std::end( m_table[k]->Index_name)-1));
                    // delete m_table[k]->Index_name[m_table[k]->Index_name.size()-1];
                    m_table[k]->Index_name.pop_back();
                    break;
                }
            writeallTable(index_file);
            index_file.close();
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
    // cout << "[Catalog debug]:" <<endl;
    // cout << "table num:" << n << endl;
    for (i=0; i<n; i++)
    {
        // cout << m_table[i]->m_metadata.name << endl;
        if (table_name == m_table[i]->m_metadata.name)
            break;
    }
    if (i==n){
    // 表不存在
        cout<<"[Catalog Debug]: invalid table name \""<<table_name<<"\""<<"\n";
        return false;
    } 
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
        if (!CheckAttr(t->m_attribute[i], data.tuple_value[i])){
            cout<<"[Catalog Debug]: Check Attr Wrong for attribute \"" << t->m_attribute[i].name<<"\"\n";
            return false;
        }
    }
    return true;
}

//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
//返回值：-4（选择条件类型不匹配）-3（表不存在） -2（attr问题） -1（选择条件出错）；0（只能通过遍历Record查询）；1（可以利用索引优化查询）
pair<int, string> CatalogManager::SelectTest(string& table_name, vector<string>& Attr, vector<ConditionUnit>& condition)
{
    pair<int, string> ret;
    ret.first = 0;
    ret.second = "";
    int i = FindTable(table_name), j;
    if (i == -1) {
        ret.first = -3;
        ret.second = table_name;
        return ret;
    }
    Table *t = m_table[i];
    int m, n, flag;
    float va;
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
            ret.first = -2;
            ret.second = Attr[i];
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
                if (condition[i].data_type == t->m_attribute[j].type)
                    condition[i].attr_num = j;
                else if(condition[i].data_type == DataType::INT_UNIT && t->m_attribute[j].type == DataType::FLOAT_UNIT) //int到float隐式转换
                {
                    condition[i].data_type = DataType::FLOAT_UNIT;
                    va = (float)condition[i].value.int_value;
                    condition[i].value.float_value = va;
                    condition[i].attr_num = j;
                }
                else
                {
                    ret.first = -4;
                    ret.second = condition[i].attr_name + "," + TypeString[condition[i].data_type] + "," + TypeString[t->m_attribute[j].type];
                    return ret;
                }
            }
        }
        if (condition[i].attr_num == m) {
            ret.first = -1;
            ret.second = condition[i].attr_name;
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
    // cout<<"[Cata debug]: get table catalog for "<< table_name<<endl;
    int i = FindTable(table_name);
    if (i == -1)
        return NULL;
    Table *t = m_table[i];
    // cout<<"[Cata debug end]"<<endl;
    return t;
}

Index* CatalogManager::GetIndexCatalog(string& index_name)
{
    int i = FindIndex(index_name);
    if (i==-1)
        return NULL;
    Index *I = m_index[i];
    return I;
}

//返回table_name表中的Index，如果不存在则返回空的Index
vector<Index*> CatalogManager::TableToIndex(string& table_name)
{
    vector<Index*> ret;
    
    int i = FindTable(table_name);
    if (i == -1)
        return ret;
    int n = m_index.size();
    for (int i=0; i<n; i++)
    {
        if (m_index[i]->table_name == table_name)
            ret.push_back(m_index[i]);
    }
    return ret;
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
    if (data.datatype == DataType::INT_UNIT && attr.type == DataType::FLOAT_UNIT)
    {
        data.datatype = DataType::FLOAT_UNIT;
        float va = (float)data.value.int_value;
        data.value.float_value = va;
    }
    return attr.type == data.datatype;
}

//检测condition
bool CatalogManager::CheckCond(ConditionUnit& cond)
{
    return true;
}


//=============================================
//private， 不写注释了
vector<Table*> CatalogManager::GetAllTable()
{
    return m_table;
}

vector<Index*> CatalogManager::GetAllIndex()
{
    return m_index;
}

int CatalogManager::FindIndex(string& index_name)
{
    int n = m_index.size(), i;
    for (i=0; i<n; i++)
    {
        if (index_name == m_index[i]->index_name)
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
    readstring(i.table_name, f);
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
    Index* index = new Index;
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
    writestring(i->table_name, f);
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

void CatalogManager::readallTable(fstream& f)
{
    int table_n;
    m_table.clear();
    readint(table_n, f);
    Table* t;
    for (int i=0; i<table_n; i++)
    {
        t = readTable(f);
        m_table.push_back(t);
    }
}

void CatalogManager::readallIndex(fstream& f)
{
    int index_n;
    readint(index_n, f);
    m_index.clear();
    Index* I;
    for (int i= 0; i<index_n; i++)
    {
        I = new Index;
        readIndex(*I, f);
        m_index.push_back(I);
    }
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

void CatalogManager::replace()
{
    int it = m_index.size();
    for (int i=0; i<it; i++)
    {
        int j = FindTable(m_index[i]->table_name);
        m_table[j]->Index_name.push_back(m_index[i]);
        m_index[i]->table = m_table[j];
        // cout << m_index[i]->index_name << endl;
    }
}