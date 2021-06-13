#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <regex>
// #include <cstring>
#include "Interpreter.h"
#include "Basicop.h"
#include "SqlError.h"
#include "Attribute.h"

// DEBUG INFO开关
// #define DEBUG 0
using namespace std;

Interpreter::Interpreter(string sql){
    this->sql = strip(sql);
}

void Interpreter::Parse(){
    string t = this->sql;
    // int pos = t.find_first_of(' ');
    // string token = t.substr(0, pos);
    // t.erase(0, pos);
    // t = strip(t);
    // transform(token.begin(), token.end(), token.begin(), ::toupper);
    string token = get_token(t);
    // cout<<"token = "<<token<<endl;
    try{
        if(token == "CREATE"){
            // pos = t.find_first_of(' ');
            // token = t.substr(0, pos);
            // t.erase(0, pos);
            // t = strip(t);
            token = get_token(t);
            if(token == "TABLE"){
                this->CreateTable(t); 
            }else if(token == "INDEX"){
                this->CreateIndex(t); 
            }

        }else if(token == "INSERT"){
            token = get_token(t);
            if(token == "INTO"){
                this->Insert(t);
            }
        }else if(token == "SELECT"){
            this->Select(t);
        }
    }catch(SyntaxError e){
        cout<<"[Error]: "<<e.msg<<endl;
    }
    
}

void Interpreter::Select(string str){
    /*
        Select a.attr1 b.attr2 from tablea as a, tableb as b where a.xx = b.yy;
    */
    string ostr = str;
    int from_pos = str.find("from");
    if( from_pos == string::npos){    from_pos = str.find("FROM");    }
    int where_pos = str.find("where");
    if(where_pos == string::npos){    where_pos = str.find("WHERE");    }


    if( from_pos == string::npos){
        SyntaxError e("No from in select query\n");
        throw e;
    }

    string attr_str = str.substr(0, from_pos), from_str, where_str;
    if( where_pos != string::npos){
        from_str = str.substr(from_pos + 4, where_pos - from_pos - 5); 
        where_str = str.substr(where_pos+5, str.length() - where_pos - 5);
    }else{ 
        from_str = str.substr(from_pos + 4, str.length() - from_pos - 4 );
        where_str = "";
    }
    cout<<"[debug]: \nattr string="<<attr_str<<"\nfrom string="<<from_str<<"\nwhere string="<<where_str<<endl;
    vector<string> attr_vec;
    vector<string> table_vec;
    vector<string> table_temp;
    map<string, string> table_name_map;
    strip(attr_str);
    strip(from_str);
    strip(where_str);

    split(attr_str, attr_vec, ',');
    split(from_str, table_temp, ',');

    for(vector<string>::iterator iter= table_temp.begin(); iter != table_temp.end(); iter++){
        string table_str = *iter;
        strip(table_str);
        vector<string> infield_vec;
        split(table_str, infield_vec, ' ');
        if(infield_vec.size() == 1){
            table_name_map[infield_vec[0]] = infield_vec[0];            
            table_vec.push_back(infield_vec[0]);
        }else if(infield_vec.size() == 3 && icasecompare(infield_vec[1], "as") ){
            table_name_map[infield_vec[2]] = infield_vec[0];
            table_vec.push_back(infield_vec[0]);
        }else{
            SyntaxError e("Invalid table name in from\n");
            throw e;
        }
    }
       
    if(where_pos == string::npos){
        string from_str = str.substr(from_pos + 5, str.length() - from_pos - 5 );
        cout<<"[debug]: from substr=\""<<from_str<<"\""<<endl;
    }
}

void Interpreter::Insert(string str){
    string ostr = str;
    string targ_table_name = get_token(str);

    int pos = str.find_first_of('(');
    string s1 = str.substr(0, pos);
    strip(s1);
    if( ! icasecompare(s1, "VALUES") || str[ str.length() - 1] != ')'){
        cout<<"[debug]: insert query="<<s1<<endl;
        SyntaxError e("Invalid Syntax please insert value by: insert into tablename values(values...)\n");
        throw e;
    }
    str = str.substr(pos+1, str.length() - 2 - pos);
    cout<<"[debug]: insert in () = \""<<str<<"\""<<endl;

}

void Interpreter::CreateIndex(string str){
    string ostr = str;
    // // cout<<"create index function now"<<endl;
    vector<string> sv;
    int pos = str.find_first_of('(');
    if( pos == string::npos){
        SyntaxError e("No ( after indexname");
        throw e;
    }
    string s1 = str.substr(0, pos);
    strip(s1);
    split(s1, sv, ' ');
    if(sv.size() != 3  || (!icasecompare(sv[1], "on")) ){
        cout<<"[debug]: parse string="<<s1<<sv.size()<<endl;
        SyntaxError e("Invalid Syntax please create index by: create index index_name on table_name(attributes)\n");
        throw e;
    }
    string index_name = sv[0], targ_table_name = sv[2];
    if( str[ str.length() - 1] != ')' ){
        SyntaxError e("when create index char after ) is not allowed");
        throw e;
    }
    str = str.substr(pos+1, str.length() - 1 - pos - 1);
    cout<<"[debug]: create index in () attrs = "<<str<<endl;
}

void Interpreter::CreateTable(string str){
    string ostr = str;
    // cout<<"create table function now"<<endl;
    int pos = str.find_first_of('(');
    if( pos == string::npos){
        SyntaxError e("No ( after tablename");
        throw e;
    }
    string tablename = str.substr(0, pos);
    if( tablename.find_first_of(" ()[]") != string::npos ){
        string emsg = "Wrong Tablename = " + tablename;
        SyntaxError e(emsg);
        throw e;
    }
    str = str.erase(0, pos+1);
    // pos = str.find_first_of(')');
    // if(pos != str.length()-1){
    //     SyntaxError e("Char after ) is not allowed");
    //     throw e;
    // }else if(pos == string::npos){
    //     SyntaxError e("No ) found");
    //     throw e;
    // }
    if( str[ str.length() - 1] != ')' ){
        SyntaxError e("Char after ) is not allowed");
        throw e;
    }
    str = str.substr(0, str.length() - 1);

    // 分析括号内的
    vector<string> sv;
    vector<Attribute> Attributes;
    #ifdef DEBUG
        cout<<"[debug]: in () string = \""<<str<<"\""<<endl;
        split(str, sv, ',');
        for(auto iter: sv){
            cout<<"[debug]: each attr = \""<<(iter)<<"\""<<endl;
        }
    #endif

    for (vector<string>::const_iterator iter = sv.cbegin(); iter != sv.cend(); iter++) {
        vector<string> attrvec;
        string line = *iter;
        strip(line);
        std::regex re_pk("primary key\\(.+\\)", regex_constants::icase);
        if( std::regex_match(line, re_pk) ){
            int p = line.find_first_of(')');
            string pk_name = line.substr(12, p-12);
            // cout<<"[debug]: pk line = "<<line<<", pkname = \""<<pk_name<<"\""<<endl;
            int flag = 0;
            for(vector<Attribute>::iterator Attr = Attributes.begin(); Attr != Attributes.end(); Attr++ ){
                // cout<<"[debug]: each attr name when find pk = "<<((*Attr).name)<<endl;
                if( (*Attr).name == pk_name ){
                    (*Attr).set_pk(true);
                    cout<<"[debug]: set pk of "<< pk_name<<endl;
                    flag = 1;
                    break;
                }
            }
            if(flag == 0){
                SyntaxError e("No primary key attr name");
                throw e;
            }
            continue;
        }
        split(line, attrvec, ' ');

        #ifdef DEBUG
            for(auto iterunit: attrvec){
                cout<<"[debug]: each unit in attr = \""<<(iterunit)<<"\""<<endl;
            }
        #endif

        if( attrvec.size() < 2 ){
            SyntaxError e("create table loss information\n");
            throw e; 
        }
        string attrname = attrvec[0];
        string typestr = attrvec[1];
        bool notnull = false, unique = false;
        for(vector<string>::const_iterator attr = attrvec.cbegin() + 2; attr != attrvec.cend(); attr++ ){
            if( icasecompare( (*attr), "unique" ) ) {
                unique = true;
            }else if( icasecompare( (*attr), "not" ) && icasecompare( (*(attr+1) ), "null" )  ){
                notnull = true;
                attr++;
            }else{
                SyntaxError e("Invalid attributes\n");
                throw e; 
            }
        }
        // cout<<"[debug]: attrname = \""<<attrname<<"\""<<endl;
        Attribute A(attrname, typestr, notnull=notnull, unique=unique);
        // A.print();
        Attributes.push_back(A);    
    }

    // 输出环节
    // #ifdef DEBUG
        cout<<"[info]: Create Table Back Info:"<<endl;
        cout<<"[info]: Table Name = "<<tablename<<endl;
        for(vector<Attribute>::iterator iter = Attributes.begin(); iter != Attributes.end(); iter++ ){
            (*iter).print();
        }
    // #endif
    // std::cout<<"create table name = "<<tablename<<endl;

    // std::cout<<str<<endl;
}