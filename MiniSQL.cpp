#include <regex>
#include "MiniSQL.h"

TableMetadata::TableMetadata(string name, int attr_num, int primary_key, int primary_index){
    this->name = name;
    this->attr_num = attr_num;
    this->primary_key = primary_key;
    this->primary_index = primary_index;
}


Attribute::Attribute(string name, string typestr, bool notnull, bool unique, bool primary_key){
    std::regex re_char("char\\(\\d+\\)", regex_constants::icase);
    std::regex re_int("int", regex_constants::icase);
    std::regex re_float("float", regex_constants::icase);
    std::regex re_num("\\d+", regex_constants::icase);
    this->charlen = -1;
    if( std::regex_match(typestr, re_int) ){
        this->type = INT_UNIT;
    }else if(std::regex_match(typestr, re_float) ){
        this->type = FLOAT_UNIT;
    }else if( std::regex_match(typestr, re_char) ){
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

void Attribute::set_pk(bool pk){
    cout<<"[debug]: call set pk for "<<this->name<<" to pk="<<pk<<endl;
    this->primary_key = pk;
}

void Attribute::print(){
    // cout<<"[debug]: "<<this->name.length()<<endl;
    cout<<"[info]: Attr:"<<this->name<<", type:";
    if(this->type == INT_UNIT){
        cout<<"int";
    }else if(this->type == FLOAT_UNIT){
        cout<<"float";
    }else if(this->type == CHAR_UNIT){
        cout<<"char("<<this->charlen<<")";
    }else{
        cout<<"Invalid";
    }
    if(this->notnull){
        cout<<", not null";
    }
    if(this->unique){
        cout<<", unique";
    }
    if(this->primary_key){
        cout<<", primary key";
    }

    cout<<endl;
}

Table::Table(TableMetadata m_metadata, vector<Attribute> m_attribute):m_metadata(m_metadata), m_attribute(m_attribute){
};