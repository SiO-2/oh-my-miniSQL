#include "Basicop.h"
#include "MiniSQL.h"
#include <algorithm>

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