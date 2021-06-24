#include "Basicop.h"
#include "MiniSQL.h"
#include "SqlError.h"
#include <algorithm>
#include <cstring>

using namespace std;
void split(string& str, vector<string>& sv, char flag){
    sv.clear();
    istringstream iss(str);
    string temp;

    while (getline(iss, temp, flag)) {
        if(!temp.empty()){
            sv.push_back(temp);
        }
    }
    return;
}

void split_string(string& str, vector<string> &sv, string& flag){
    sv.clear();
    int pos;
    str += flag;//扩展字符串以方便操作
    int size = str.size();
    for (int i = 0; i < size; )
    {
        pos = str.find(flag, i);
        if (pos < size)
        {
            std::string s = str.substr(i, pos - i);
            sv.push_back(s);
            i = pos + flag.size() ;
        }
    }
    return;
}

string strip(string& s){
    if (s.empty()){
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

string lstrip(string& s){
    if (s.empty()){
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    // s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
string rstrip(string& s){
    if (s.empty()){
        return s;
    }
    // s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}



string get_token(string &s){
    s = lstrip(s);
    int pos = s.find_first_of(' ');
    string token = s.substr(0, pos);
    // transform(token.begin(), token.end(), token.begin(), ::toupper);
    s = s.erase(0, pos);
    return token;
}

bool icasecompare(const string& a, const string& b)
{
		if (a.length() == b.length()) {
		    return std::equal(a.begin(), a.end(), b.begin(),
		                      [](char a, char b) {
		                          return tolower(a) == tolower(b);
		                      });
        }
        return false;
}

DataType ParseDataType(string& str){
    DataType data_type;
    if( str[0] == '\"' && str[str.length() - 1] == '\"' ){
        str = str.substr(1, str.length() - 2);
        data_type = CHAR_UNIT;
    }else if( str.find(".") != string::npos ){
        data_type = FLOAT_UNIT;
        // float_value = stol(str);
    }else{
        data_type = INT_UNIT;
        // try{
            // int_value = stoi(str);
        // }catch(...){
            // SyntaxError e("Wrong condition value syntax");
            // throw e;
        // }
    }
    return data_type;
}

Value ParseStringType(DataType type, string& str){
    Value value;
    switch(type){
        case INT_UNIT:
            try{
                value.int_value = stoi(str);
            }catch(...){
                SyntaxError e("Wrong condition value syntax in " + str);
                throw e;
            }
            break;
        case FLOAT_UNIT:
            value.float_value = stof(str);
            break;
        case CHAR_UNIT:
            char* str_c = (char *)malloc(sizeof(char) * (str.length() + 1) );
            strcpy(str_c, str.c_str());
            value.char_n_value = str_c; 
            break;
    }
    return value;
}

vector<ConditionUnit> ParseCondition(string where_str){
    vector<ConditionUnit> cond_vec;
    vector<string> temp;
    strip(where_str);
    if(where_str.empty()){
        return cond_vec;
    }
    string flag = "and";
    split_string(where_str, temp, flag);
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
                SyntaxError e("Invalid Operation. must in (=, <, >, <=, >=, <>) but \"" + infield_vec[1] + "\"");
                throw e;
            }
            attr_name = infield_vec[0], value = infield_vec[2];
        }else{
            SyntaxError e("Invalid condition \"" + cond_str + "\"");
            throw e;
        }
        strip(attr_name);
        strip(value);
        
        data_type = ParseDataType(value);

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

    return cond_vec;
}