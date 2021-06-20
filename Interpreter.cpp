#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstring>
#include "Interpreter.h"
#include "Basicop.h"
#include "SqlError.h"
// #include "Attribute.h"
#include "MiniSQL.h"

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
        if( icasecompare(token, "CREATE") ){
            // pos = t.find_first_of(' ');
            // token = t.substr(0, pos);
            // t.erase(0, pos);
            // t = strip(t);
            token = get_token(t);
            if( icasecompare(token, "TABLE") ){
                this->CreateTable(t); 
            }else if( icasecompare(token, "INDEX") ){
                this->CreateIndex(t); 
            }

        }else if( icasecompare(token, "INSERT") ){
            token = get_token(t);
            if(  icasecompare(token, "INTO") ){
                this->Insert(t);
            }
        }else if( icasecompare(token, "SELECT") ){
            this->Select(t);
        }else if( icasecompare(token, "DROP") ){
            token = get_token(t);
            if( icasecompare(token, "TABLE") ){
                this->DropTable(t);
            }else if( icasecompare(token, "INDEX") ){
                this->DropIndex(t);
            }
        }
    }catch(SyntaxError e){
        cout<<"[Error]: "<<e.msg<<endl;
    }
    
}

void Interpreter::DropTable(string str){
    strip(str);
    if( str.find(" ") != string::npos){
        SyntaxError e("Invalid Table Name in Drop Table");
        throw e;
    }
    // Here Table Name to Drop is 'str'
    cout<<"[info]: Drop Table Name=\""<<str<<"\""<<endl;
}

void Interpreter::DropIndex(string str){
    strip(str);
    if( str.find(" ") != string::npos){
        SyntaxError e("Invalid Table Name in Drop Index");
        throw e;
    }
    // Here Index Name to Drop is 'str'
    cout<<"[info]: Drop Index Name=\""<<str<<"\""<<endl;
}

void Interpreter::Select(string str){
    /*
        Select a.attr1 b.attr2 from tablea as a, tableb as b where a.xx = b.yy; // not support now;
        Select attrname1, attrname2 from table where cond1 = value1, cond2 = value2;
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
    vector<string> temp;
    vector<ConditionUnit> cond_vec;
    map<string, string> table_name_map;
    strip(attr_str);
    strip(from_str);
    strip(where_str);

    split(attr_str, attr_vec, ',');

    for(vector<string>::iterator iter= attr_vec.begin(); iter != attr_vec.end(); iter++){
        strip(*iter);
        if( (*iter).find_first_of(" ") != string::npos ){
            SyntaxError e("Invalid attribute name\n");
            throw e;
        }
        // int dotpos = (*iter).find_first_of(".");
        // if(dotpos != string::npos){
            // like a.attr
        // }
    }

    split(from_str, temp, ',');
    for(vector<string>::iterator iter= temp.begin(); iter != temp.end(); iter++){
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
       
    if( table_vec.size() < 1){
        SyntaxError e("No table is selected\n");
        throw e;
    }
    // 暂时不支持多表查询
    if( table_vec.size() > 1){
        SyntaxError e("Multiple Table Select is not supported yet\n");
        throw e;
    }

    temp.clear();
    split(where_str, temp, ',');
    for(vector<string>::iterator iter= temp.begin(); iter != temp.end(); iter++){
        string cond_str = *iter;
        string attr_name, value;
        DataType data_type; // where条件中的value类型
        float float_value;
        int int_value;
        OpCode Op;
        strip(cond_str);
        vector<string> infield_vec;
        split(cond_str, infield_vec, ' ');
        if(infield_vec.size() == 3){
            if(infield_vec[1] == "="){
                Op = EQ_;
            }else if(infield_vec[1] == "<"){
                Op = L_;
            }else if(infield_vec[1] == ">"){
                Op = G_;
            }else if(infield_vec[1] == "<="){
                Op = LE_;
            }else if(infield_vec[1] == ">="){
                Op = GE_;
            }else if(infield_vec[1] == "<>"){
                Op = NE_;
            }else{
                SyntaxError e("Invalid Operation. must in (=, <, >, <=, >=, <>).\n");
                throw e;
            }
            attr_name = infield_vec[0], value = infield_vec[2];
        }else{
            SyntaxError e("Invalid table name in from\n");
            throw e;
        }
        strip(attr_name);
        strip(value);
        
        data_type = ParseDataType(value);

        // if( value[0] == '\"' && value[value.length() - 1] == '\"' ){
        //     value = value.substr(1, value.length() - 2);
        //     data_type = CHAR_UNIT;
        // }else if( value.find(".") != string::npos ){
        //     data_type = CHAR_UNIT;
        // }else{
        //     data_type = INT_UNIT;
        // }
        ConditionUnit Cond(attr_name, -1, Op, data_type);
        switch(data_type){
            case INT_UNIT:
                try{
                    int_value = stoi(value);
                }catch(...){
                    SyntaxError e("Wrong condition value syntax");
                    throw e;
                }
                Cond.value.int_value = int_value;break;
            case FLOAT_UNIT:
                float_value = stof(value);
                Cond.value.float_value = float_value;break;
            case CHAR_UNIT:
                char* value_str_c = (char *)malloc(sizeof(char) * (value.length() + 1) );
                strcpy(value_str_c, value.c_str());
                // data_unit.value.char_n_value = value_str_c; break;
                Cond.value.char_n_value = value_str_c; break;
        }
        cond_vec.push_back(Cond);
    }

    // debug 打印 condition 信息
    for(auto cond:cond_vec){
        cond.Print();
    }

    // 结果存储
    // where条件存储在 vector<ConditionUnit> cond_vec 里
    // from唯一的table名在 table_vec[0]
    // Select的属性名在 vector<string> attr_vec里

    // cout<<"[debug]: select attr: "<<endl;
    // for(auto iter:attr_vec){
    //     cout<<(iter)<<endl;
    // }

}

void Interpreter::Insert(string str){
    // string ostr = str;
    // string targ_table_name = get_token(str);

    // int pos = str.find_first_of('(');
    // string s1 = str.substr(0, pos);
    // strip(s1);
    // if( ! icasecompare(s1, "VALUES") || str[ str.length() - 1] != ')'){
    //     cout<<"[debug]: insert query="<<s1<<endl;
    //     SyntaxError e("Invalid Syntax please insert value by: insert into tablename values(values...)\n");
    //     throw e;
    // }
    // str = str.substr(pos+1, str.length() - 2 - pos);
    // // cout<<"[debug]: insert in () = \""<<str<<"\""<<endl;

    // vector<string> value_vec;
    // vector<DataUnit> dataunit_vec;
    // split(str, value_vec, ',');

    // int int_value; 
    // float float_value;
    // DataType data_type;
    // for(vector<string>::iterator iter = value_vec.begin(); iter!=value_vec.end(); iter++){
    //     string value_str = *iter;
    //     strip(value_str);
    //     DataUnit data_unit;
    //     // if( value_str == "NULL" || value_str == "null"){
    //         // NULL 判断，暂不支持
    //     // }
    //     data_type = ParseDataType(value_str);
    //     data_unit.data_type = data_type;
    //     switch(data_type){
    //         case INT_UNIT:
    //             try{
    //                 int_value = stoi(value_str);
    //             }catch(...){
    //                 SyntaxError e("Wrong condition value syntax in " + value_str);
    //                 throw e;
    //             }
    //             data_unit.value.int_value = int_value;break;
    //         case FLOAT_UNIT:
    //             float_value = stof(value_str);
    //             data_unit.value.float_value = float_value;break;
    //         case CHAR_UNIT:
    //             char* value_str_c = (char *)malloc(sizeof(char) * (value_str.length() + 1) );
    //             strcpy(value_str_c, value_str.c_str());
    //             data_unit.value.char_n_value = value_str_c; break;
    //     }
    //     dataunit_vec.push_back(data_unit);
    // }

    // cout<<"[Insert Info]:"<<endl;
    // for(auto unit: dataunit_vec){
    //     unit.Print();
    // }
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
    
    string attr_name = str;
    strip(attr_name);
    if( attr_name.find(" ()[]") != string::npos){
        SyntaxError e("Invalide attribute name in create index");
        throw e;
    }
    // cout<<"[debug]: create index in () attrs = "<<str<<endl;

    // 结果存储
    // 索引属性的名字在 attr_name中
    // 索引名字在 index_name中
    // 对象表格在 targ_table_name中
    cout<<"[debug create index]:"<<index_name<<" on "<<targ_table_name<<"("<<attr_name<<")"<<endl;
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
    strip(tablename);
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

    // cout<<"[debug]: create string ="<<str<<endl;
    // 分析括号内的
    vector<string> sv;
    vector<Attribute> Attributes;
    split(str, sv, ',');
    #ifdef DEBUG
        cout<<"[debug]: in () string = \""<<str<<"\""<<endl;
        for(auto iter: sv){
            cout<<"[debug]: each attr = \""<<(iter)<<"\""<<endl;
        }
    #endif

    int pk_mark = -1, main_index = -1;
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
            int count = 0;
            for(vector<Attribute>::iterator Attr = Attributes.begin(); Attr != Attributes.end(); Attr++ ){
                // cout<<"[debug]: each attr name when find pk = "<<((*Attr).name)<<endl;
                if( (*Attr).name == pk_name ){
                    (*Attr).set_pk(true);
                    cout<<"[debug]: set pk of "<< pk_name<<endl;
                    flag = 1;
                    pk_mark = count;
                    main_index = count;
                    break;
                }
                count ++;
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
        // A.Print();
        Attributes.push_back(A);    
    }

    TableMetadata Meta(tablename, Attributes.size(), pk_mark, main_index);
    Table table(Meta, Attributes);

    table.Print();
    // 输出环节
    // #ifdef DEBUG
        // cout<<"[info]: Create Table Back Info:"<<endl;
        // cout<<"[info]: Table Name = "<<tablename<<endl;
        // for(vector<Attribute>::iterator iter = Attributes.begin(); iter != Attributes.end(); iter++ ){
        //     (*iter).Print();
        // }
    // #endif
    // std::cout<<"create table name = "<<tablename<<endl;

    // std::cout<<str<<endl;
}