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
// #define DEBUG
// DEBUG INFO开关
// #define DEBUG 0
using namespace std;


Interpreter::Interpreter():Cata(), Record(){
    vector<Index*> pindex_list = Cata.GetAllIndex();
    vector<Index> index_list;
    for(auto pindex: pindex_list){
        index_list.push_back(*pindex);
        // pindex->Print();
    }
    // cout<<"[Interpreter debug]: begin set index int map"<<endl;
    Record.imanager->setindexIntMap(index_list);
}

Interpreter::~Interpreter(){
    delete &Record;
    delete &Cata;
}

void Interpreter::Parse(string sql){
    string t = sql;
    strip(t);
    string token = get_token(t);
    // cout<<"token = "<<token<<"\n";
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
            }else{
                cout<<"[Syntax Error]: "<<"you can only create table or index"<<"\n";
            }

        }else if( icasecompare(token, "INSERT") ){
            token = get_token(t);
            if(  icasecompare(token, "INTO") ){
                this->Insert(t);
            }else{
                cout<<"[Syntax Error]: "<<"Insert must be followed by \"into\""<<"\n";
            }
        }else if( icasecompare(token, "SELECT") ){
            this->Select(t);
        }else if( icasecompare(token, "DROP") ){
            token = get_token(t);
            if( icasecompare(token, "TABLE") ){
                this->DropTable(t);
            }else if( icasecompare(token, "INDEX") ){
                this->DropIndex(t);
            }else{
                cout<<"[Syntax Error]: "<<"you can only drop table or index"<<"\n";
            }
        }else if( icasecompare(token, "DELETE") ){
            token = get_token(t);
            if( icasecompare(token, "FROM") ){
                this->Delete(t);
            }else{
                cout<<"[Syntax Error]: "<<"Delete must be followed by \"from\""<<"\n";
            }
        } else if( icasecompare(token, "SHOW") ){
            token = get_token(t);
            if( icasecompare(token, "table") ){
                this->ShowTable(t);
            }else if( icasecompare(token, "index") ){
                this->ShowIndex(t);
            }else if(icasecompare(token,"database")){
                this->ShowDatabase(t);
            }else{
                cout<<"[Syntax Error]: "<<"Show must be followed by \"table\" or \"index\" or \"database\""<<"\n";
            }
        }else{
            cout<<"[Error]: Wrong command can not interpret "<<token<<"\n";
        }
    }catch(SyntaxError e){
        cout<<"[Syntax Error]: "<<e.msg<<"\n";
        // throw e;
    }catch(DBError e){
        cout<<"[Runtime Error]: "<<e.msg<<"\n";
        // throw e;
    }catch(InternalError e){
        cout<<"[Internal Error]: "<<e.msg<<"\n";
    }
   
}

void Interpreter::ShowDatabase(std::string str){
    strip(str);
    if(!str.empty()){
        SyntaxError e("Show database can not be followed by other characters");
        throw e;
    }
    vector<Table *> table_pointer_vec = Cata.GetAllTable();
    vector<Index *> index_pointer_vec = Cata.GetAllIndex();
    cout<<"[All Table Info]:"<<"\n";
    for(auto table: table_pointer_vec){
        table->Print();
    }
    cout<<"[All Index Info]:"<<"\n";
    for(auto index: index_pointer_vec){
        index->Print();
    }
}

void Interpreter::ShowTable(string str){
    string &tablename = str;
    strip(tablename);
    Table *table = Cata.GetTableCatalog(tablename);
    if(table==NULL){
        DBError e("Invalid table name " + tablename);
        throw e;
    }
    table->Print();
    for(auto index: table->Index_name){
        index->Print();
    }
}

void Interpreter::ShowIndex(string str){
    string &indexname = str;
    strip(indexname);
    Index* index = Cata.GetIndexCatalog(indexname);
    if(index == NULL){
        DBError e("Invalid index name " + indexname);
        throw e;
    }
    index->Print();
}

void Interpreter::Delete(string str){
    string tablename = get_token(str);
    string token = get_token(str);
    if(! icasecompare(token, "WHERE") ){
        SyntaxError e("Delete table tablename from<<");
        throw e;
    }
    vector<ConditionUnit> cond_vec;
    Table * table = Cata.GetTableCatalog(tablename);
    if(table == NULL){
        DBError e("No such Table " + tablename);
        throw e;
    }
    try{
        cond_vec = ParseCondition(str);
    }catch(SyntaxError e){
        throw e;
    }

    // debug 信息
    // cout<<"[Interpreter Delete Debug]:"<<"\n";
    // for(auto cond:cond_vec){
    //     cond.Print();
    // }
    // cout<<"[Interpreter Delete Debug End]"<<"\n";

    // 数据存储
    // 表名 tablename
    // 条件 cond_vec
    pair<int, string> response;
    response = Cata.DeleteTest(tablename, cond_vec);
    if(response.first == -2){
        SyntaxError e("Invalid table name " + tablename);
        throw e;
    }else if(response.first == -1){
        SyntaxError e("Delete conditions error");
        throw e;
    }else if(response.first == 0){
        // 使用Record删除
        Table *table_pointer = Cata.GetTableCatalog(tablename);
        Record.DeleteTuple(*table_pointer, cond_vec);
        cout<<"Delete successfully"<<"\n";
    }else if(response.first == 1){
        // 索引删除
        string index_name = response.second; 
        cout<<"[Interpreter Delete]: by index "<<index_name<<"\n";
        cout<<"not supported yet"<<"\n";
    }else{
        SyntaxError e("Wrong catalog return value " + to_string(response.first));
        throw e;
    }

}

void Interpreter::DropTable(string str){
    strip(str);
    if( str.find(" ") != string::npos){
        SyntaxError e("Invalid Table Name in Drop Table");
        throw e;
    }
    // Here Table Name to Drop is 'str'
    // cout<<"[Debug info]: Drop Table Name=\""<<str<<"\""<<"\n";

    // 调用Catalog的部分
    string tablename = str;
    Table *table = Cata.GetTableCatalog(tablename);
    if(table == NULL){
        DBError e("Invalid table name \"" + tablename + "\"");
        throw e;
    }

    for(auto index: table->Index_name){
        this->DropIndex(index->index_name);
        // if( !(this->DropIndex(index->index_name)) ){
        //     cout<<"Drop index \"" + index->index_name + "\" of table \"" + tablename + "\" fail"<<"\n";
        // }
    }

    if( Cata.DropTable(str) ){
        cout<<"Drop Table \""<<str<<"\" succussfully"<<"\n";
    }else{
        cout<<"Drop Table \""<<str<<"\" failed"<<"\n";
    }
}

void Interpreter::DropIndex(string str){
    strip(str);
    if( str.find(" ") != string::npos){
        SyntaxError e("Invalid Table Name in Drop Index");
        throw e;
    }
    // Here Index Name to Drop is 'str'
    // cout<<"[info]: Drop Index Name=\""<<str<<"\""<<"\n";
    
    bool b = Cata.DropIndex(str);
    if(!b){
        DBError e("Drop index \"" + str + "\" failed");
        throw e;
    }
    cout<<"Drop index \""<<str<<"\" successfully"<<"\n";
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
    // cout<<"[debug]: \nattr string="<<attr_str<<"\nfrom string="<<from_str<<"\nwhere string="<<where_str<<"\n";
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
            cout<<"[Interpreter debug]: "<<infield_vec.size()<<endl;
            SyntaxError e("Invalid table name  \"" + table_str + "\"");
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
    Table* table = Cata.GetTableCatalog(table_vec[0]);
    if(table == NULL){
        DBError e("Invalid table name \"" + table_vec[0] + "\"");
        throw e;
    }
    cond_vec = ParseCondition(where_str);

    // debug 打印 condition 信息
    // for(auto cond:cond_vec){
    //     cond.Print();
    // }

    // cout<<"[debug]: select attr: "<<"\n";
    // for(auto iter:attr_vec){
    //     cout<<(iter)<<"\n";
    // }

    // 结果存储
    // where条件存储在 vector<ConditionUnit> cond_vec 里
    // from唯一的table名在 table_vec[0]
    // Select的属性名在 vector<string> attr_vec里
    
    // 对 select *的支持
    if( attr_vec.size() == 1 && attr_vec[0] == "*" ){
        attr_vec.clear();
    }


    // 调用Catalog
    pair<int, string> response;
    response = Cata.SelectTest(table_vec[0], attr_vec, cond_vec);
    if( response.first == -2 ){
        DBError e("select table does not exist");
        throw e;
    }else if(response.first == -1){
        DBError e("select conditions error");
        throw e;
    }else{
        // cout<<"[Catalog res]: select with or without index,"<<response.second<<"\n";
        // Call Record Manager
        Table* table = Cata.GetTableCatalog(table_vec[0]);
    #ifdef DEBUG
            printf("Interpreter::Select::348:: cond_vec[0].attr_num = %d\n", cond_vec[0].attr_num);
    #endif
        vector<Tuple> Select_Res = Record.SelectTuple(*table, cond_vec);
        // cout<<"[Interpreter Select Res without index]:"<<"\n";


        // 这里还需要做一下筛选属性
        if(attr_vec.empty()){
            for(auto tuple:Select_Res){
                tuple.Print();
            }
        }else{
            vector<int> int_vec;
            map<string, int> attr2idx;
            int idx = 0;
            for(auto attr: table->m_attribute){
                attr2idx[attr.name] = idx;
                idx ++;
            }

            for(auto attr_name: attr_vec){
                int_vec.push_back( attr2idx[attr_name] );
            }
            for(auto tuple:Select_Res){
                tuple.Print(int_vec);
            }
        }

        cout<<"[Interpreter Select Res End]:"<<"\n";
    }
    // else if(response.first == 1){
    //     cout<<"[Catalog res]: select with index"<<response.second<<"\n";
    // }

}

void Interpreter::Insert(string str){
    string ostr = str;
    string targ_table_name = get_token(str);

    int pos = str.find_first_of('(');
    string s1 = str.substr(0, pos);
    strip(s1);
    if( ! icasecompare(s1, "VALUES") || str[ str.length() - 1] != ')'){
        cout<<"[debug]: insert query="<<s1<<"\n";
        SyntaxError e("Invalid Syntax please insert value by: insert into tablename values(values...)\n");
        throw e;
    }
    str = str.substr(pos+1, str.length() - 2 - pos);
    // cout<<"[debug]: insert in () = \""<<str<<"\""<<"\n";

    vector<string> value_vec;
    split(str, value_vec, ',');

    int int_value; 
    float float_value;
    DataType data_type;
    Tuple tuple;
    for(vector<string>::iterator iter = value_vec.begin(); iter!=value_vec.end(); iter++){
        string value_str = *iter;
        strip(value_str);
        // if( value_str == "NULL" || value_str == "null"){
            // NULL 判断，暂不支持
        // }
        data_type = ParseDataType(value_str);
        Unit unit;
        Value value;
        try{
            value = ParseStringType(data_type, value_str);
        }catch( SyntaxError e){
            throw e;
        }
        unit.value = value;
        unit.datatype = data_type;
        tuple.tuple_value.push_back(unit);
    }

    // cout<<"[Insert Info]:"<<"\n";
    // for(auto tunit:tuple.tuple_value){

    //     tunit.Print();
    // }
    // 结果存储
    // string:targ_table_name
    // value: tuple

    // 调用catalog
    if( !Cata.InsertTest(targ_table_name, tuple) ){
        // cout<<"[Catalog res]: Insert invalid"<<"\n";
        DBError e("Insert invalid");
        throw e;
    }else {
        // cout<<"[Catalog res]: Insert validate"<<"\n";
    }

    // Call Record Manager
    // Befor that call Catalog to get whole table info
    Table * table = Cata.GetTableCatalog(targ_table_name);
    // table->Print();
    // tuple.Print();
    Record.InsertTuple(*table, tuple);
    cout<<"Insert successfully"<<endl;
}

void Interpreter::CreateIndex(string str){
    string ostr = str;
    // // cout<<"create index function now"<<"\n";
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
        cout<<"[debug]: parse string="<<s1<<sv.size()<<"\n";
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
    // cout<<"[debug]: create index in () attrs = "<<str<<"\n";

    // 结果存储
    // 索引属性的名字在 attr_name中
    // 索引名字在 index_name中
    // 对象表格在 targ_table_name中
    // cout<<"[debug create index]:"<<index_name<<" on "<<targ_table_name<<"("<<attr_name<<")"<<"\n";
    Table *table = Cata.GetTableCatalog(targ_table_name);

    // cout<<"[Interpreter Debug]: got table from cata"<<endl;

    if(table == NULL){
        DBError e("Invalid Table \"" + targ_table_name + "\"");
        throw e;
    }
    
    int count = 0, attr_num = -1;
    for(auto attr:table->m_attribute){
        if( attr_name == attr.name ){
            if(!attr.unique){
                DBError e("You can only build index on unique attribute");
                throw e;
            }
            attr_num = count;
            break;
        }else{
            count ++;
        }
    }
    if(attr_num == -1){
        DBError e("Invalid attribute name \"" + attr_name + "\"");
        throw e;
    }

    // cout<<"[Interpreter Debug]: begin create index"<<endl;
    Index index(index_name, table, targ_table_name, attr_num);

    // cout<<"[Interpreter Debug]: begin create index into cata"<<endl;
    if( !Cata.CreateIndex(index) ){
        InternalError e("Create index \"" + index_name + "\" failed");
        throw e;
    }
    // cout<<"[Interpreter Debug]: begin create index into record"<<endl;
    Record.CreateIndex(index);
    cout<<"Create index successfully"<<"\n";

}

void Interpreter::CreateTable(string str){
    string ostr = str;
    // cout<<"create table function now"<<"\n";
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

    // cout<<"[debug]: create string ="<<str<<"\n";
    // 分析括号内的
    vector<string> sv;
    vector<Attribute> Attributes;
    split(str, sv, ',');
    #ifdef DEBUG
        cout<<"[debug]: in () string = \""<<str<<"\""<<"\n";
        for(auto iter: sv){
            cout<<"[debug]: each attr = \""<<(iter)<<"\""<<"\n";
        }
    #endif

    map<string, string> attr2type;

    int pk_mark = -1, main_index = -1;
    for (vector<string>::const_iterator iter = sv.cbegin(); iter != sv.cend(); iter++) {
        vector<string> attrvec;
        string line = *iter;
        strip(line);
        std::regex re_pk("primary key\\(.+\\)", regex_constants::icase);
        if( std::regex_match(line, re_pk) ){
            int p = line.find_first_of(')');
            string pk_name = line.substr(12, p-12);
            // cout<<"[debug]: pk line = "<<line<<", pkname = \""<<pk_name<<"\""<<"\n";
            int flag = 0;
            int count = 0;
            if(pk_mark != -1){
                SyntaxError e("Duplicated Primary Key when Create Table");
                throw e;
            }
            for(vector<Attribute>::iterator Attr = Attributes.begin(); Attr != Attributes.end(); Attr++ ){
                // cout<<"[debug]: each attr name when find pk = "<<((*Attr).name)<<"\n";
                if( (*Attr).name == pk_name ){
                    (*Attr).set_pk(true);
                    Attr->unique = true;
                    // cout<<"[debug]: set pk of "<< pk_name<<"\n";
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
                cout<<"[debug]: each unit in attr = \""<<(iterunit)<<"\""<<"\n";
            }
        #endif

        if( attrvec.size() < 2 ){
            SyntaxError e("create table failed because of invalid attribute definition (loss parameters)");
            throw e; 
        }
        string attrname = attrvec[0];
        string typestr = attrvec[1];
        if(attr2type.count(attrname) == 1){
            SyntaxError e("Duplicated attribute name \"" + attrname + "\"");
            throw e;
        }
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
        // cout<<"[debug]: attrname = \""<<attrname<<"\""<<"\n";
        try{
            Attribute A(attrname, typestr, notnull=notnull, unique=unique);
            Attributes.push_back(A);    
            attr2type[attrname] = typestr;
        }catch(SyntaxError e){
            throw e;
        }
        // A.Print();
        
    }

    TableMetadata Meta(tablename, Attributes.size(), pk_mark, main_index);
    Table table(Meta, Attributes);

    // 输出环节
    // table.Print();

    // 调用Catalog
    if( Cata.CreateTable(table) ){
        cout<<"[Catalog info]: Create Table Successfully"<<"\n";
    }else{
        // cout<<"[Catalog info]: Create Table Failed"<<"\n";s
        DBError e("Create Table Failed because of duplicated table name" + tablename);
        throw e;
    }

    // 创建索引
    int attrcount = 0;
    for(auto attr:Attributes){
        if(attr.unique){
            // 为unique自动创建索引
            cout<<"Automatic build index for unique attribute \"" + attr.name + "\""<<"\n";
            string index_name = attr.name + "_autoindex_" + tablename; 
            Index index(index_name, &table, tablename, attrcount);

            if( !Cata.CreateIndex(index) ){
                InternalError e("Create index \"" + index_name + "\" failed");
                throw e;
            }
            // cout<<"[Interpreter Debug]: begin create index into record"<<endl;
            Record.CreateIndex(index);
            cout<<"Create index \"" + index_name + "\" successfully"<<"\n";

        }
        attrcount ++;
    }

    // Call Record Manager
    // Record.CreateTableFile(table);
}