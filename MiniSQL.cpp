#include <regex>
#include "MiniSQL.h"

TableMetadata::TableMetadata()
{
}

TableMetadata::TableMetadata(string name, int attr_num, int primary_key, int primary_index)
{
    this->name = name;
    this->attr_num = attr_num;
    this->primary_key = primary_key;
    this->primary_index = primary_index;
}

TableMetadata::TableMetadata(TableMetadata &t)
{
    this->attr_num = t.attr_num;
    this->name = t.name;
    this->primary_index = t.primary_index;
    this->primary_key = t.primary_key;
}

void TableMetadata::Print()
{
    cout << "[Table Meta]:\n";
    cout << "Name:" << this->name << endl;
    cout << "Attr num:" << this->attr_num << endl;
    // cout << "Primary Key:" << this->primary_key << endl;
    // cout << "Primary Index:" << this->primary_index << endl;
}

Attribute::Attribute(string name, string typestr, bool notnull, bool unique, bool primary_key)
{
    std::regex re_char("char\\(\\d+\\)", regex_constants::icase);
    std::regex re_int("int", regex_constants::icase);
    std::regex re_float("float", regex_constants::icase);
    std::regex re_num("\\d+", regex_constants::icase);
    this->charlen = -1;
    if (std::regex_match(typestr, re_int))
    {
        this->type = INT_UNIT;
    }
    else if (std::regex_match(typestr, re_float))
    {
        this->type = FLOAT_UNIT;
    }
    else if (std::regex_match(typestr, re_char))
    {
        this->type = CHAR_UNIT;
        sregex_iterator itr1(typestr.begin(), typestr.end(), re_num);
        sregex_iterator itr2;
        for (sregex_iterator itr = itr1; itr != itr2; ++itr)
        {
            // ！！！ 这里可能有点隐患，默认\d+是贪婪匹配了
            // string L = itr->str();
            // cout<<L<<endl;
            this->charlen = stoi(itr->str());
            break;
            // cout << itr->str() << endl;
        }
    }
    this->name = name;
    this->charlen = charlen;
    this->notnull = notnull;
    this->unique = unique;
    this->primary_key = primary_key;
    // cout<<"[info]: Create Attribute "<<(this->name)<<", type = "<<this->type<<", charlen = "<<this->charlen<<endl;
    // cout<<"[debug]: "<<this->name.length()<<endl;
}

void Attribute::set_pk(bool pk)
{
    // cout << "[debug]: call set pk for " << this->name << " to pk=" << pk << endl;
    this->primary_key = pk;
}

void Attribute::Print()
{
    // cout<<"[debug]: "<<this->name.length()<<endl;
    cout << "[Attribute info]: Attr:" << this->name << ", type:";
    if (this->type == INT_UNIT)
    {
        cout << "int";
    }
    else if (this->type == FLOAT_UNIT)
    {
        cout << "float";
    }
    else if (this->type == CHAR_UNIT)
    {
        cout << "char(" << this->charlen << ")";
    }
    else
    {
        cout << "Invalid";
    }
    if (this->notnull)
    {
        cout << ", not null";
    }
    if (this->unique)
    {
        cout << ", unique";
    }
    if (this->primary_key)
    {
        cout << ", primary key";
    }

    cout << endl;
}

Table::Table(TableMetadata m_metadata, vector<Attribute> m_attribute) : m_metadata(m_metadata), m_attribute(m_attribute){};

Table::Table(Table &table)
{
    this->m_metadata = table.m_metadata;
    this->m_attribute.assign(table.m_attribute.begin(), table.m_attribute.end());
}

void Table::Print()
{
    cout << "[Table Info]:\n";
    this->m_metadata.Print();
    for (auto Attr : this->m_attribute)
    {
        Attr.Print();
    }
}

// Index::Index(Index &index)
// {
//     this->attr_num = index.attr_num;
//     this->index_name = index.index_name;
//     this->table = index.table;
//     this->table_name = index.table_name;
// }

Index::Index()
{
}

Index::Index(string index_name, Table* table, string table_name, int attr_num):index_name(index_name), table(table), table_name(table_name), attr_num(attr_num){

}


void Index::Print() const
{
    cout << "[Index Info]: index name = " << this->index_name <<" on table \"" << this->table_name << "\" attribute \""<<this->table->m_attribute[this->attr_num].name<<"\"\n";
    // Table *table = this->table;
    // table->Print();
    // cout<<this->attr_num;
    // this->table->m_attribute[this->attr_num].name;
    // cout<<"\""<< endl;
}

Unit::Unit()
{
}

Unit::Unit(Value value, DataType datatype) : value(value)
{
    // this->value = value;
    this->datatype = datatype;
}

void Unit::Print()
{
    // cout << "[Unit info]: ";
    string str;
    switch (this->datatype)
    {
    case INT_UNIT:
        cout << "(int)" << this->value.int_value <<" ";
        break;
    case FLOAT_UNIT:
        cout << "(float)" << this->value.float_value<<" ";
        break;
    case CHAR_UNIT:
        str = this->value.char_n_value;
        cout << "(string)" << str <<" ";
        break;
    default:
        break;
    }
}
Tuple::Tuple() : tuple_value(), valid(true)
{
}

void Tuple::Print()
{
    cout << "[Tuple]:";
    for (auto unit : this->tuple_value)
    {
        unit.Print();
    }
    cout << "[End]" << endl;
}

void Tuple::Print(vector<int>& int_vec){
    cout << "[Tuple]:";
    for (int idx : int_vec)
    {
        this->tuple_value[idx].Print();
        // unit.Print();
    }
    cout << "[End]" << endl;
}
// void DataUnit::Print(){
//     cout<<"[DataUnit]: attrname = "<<this->attr_name<<", DataType = ";
//     switch (this->data_type)
//     {
//     case INT_UNIT:
//         cout<<"int: "<<this->value.int_value;
//         break;
//     case FLOAT_UNIT:
//         cout<<"float:"<<this->value.float_value;
//         break;
//     case CHAR_UNIT:
//         cout<<"char: "<<this->value.char_n_value;
//         break;

//     default:
//         break;
//     }
//     cout<<endl;
// }

ConditionUnit::ConditionUnit(string attr_name, int attr_num, OpCode op_code, DataType data_type) : value()
{
    this->attr_name = attr_name;
    this->attr_num = attr_num;
    this->op_code = op_code;
    this->data_type = data_type;
}

void ConditionUnit::Print()
{
    cout << "[Condition Unit]:\n"
         << "attr name = \"" << this->attr_name << "\", Op = \"";
    switch (this->op_code)
    {
    case EQ_:
        cout << "=\"";
        break;
    case NE_:
        cout << "!=\"";
        break;
    case LE_:
        cout << "<=\"";
        break;
    case GE_:
        cout << ">=\"";
        break;
    case L_:
        cout << "<\"";
        break;
    case G_:
        cout << ">\"";
        break;
    default:
        break;
    }
    cout << ", value = \"";
    switch (this->data_type)
    {
    case INT_UNIT:
        cout << "int:" << this->value.int_value;
        break;
    case FLOAT_UNIT:
        cout << "float:" << this->value.float_value;
        break;
    case CHAR_UNIT:
        cout << "char:" << this->value.char_n_value;
        break;

    default:
        break;
    }
    cout << "\"" << endl;
}