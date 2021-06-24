// Basicop.h
// Function: Basic Operations used in Interpreter
// Author: wang yichen
// Date: 2021-6-12
#ifndef _BASICOP_H_
#define _BASICOP_H_
#include "MiniSQL.h"
#include <string>
#include <vector>
#include <sstream>
using namespace std;

// Name: split
// Function: like split in Python, split string into vector with seperator = flag
// Example: split "abc def hij" by ' ' into ["abc", "def", "hij"]
// Input:
//      string& str: string to be split
//      vector<string>& sv: vector to put string after split
//      char flag: character to split
// output: void
// Decription: 
//      Implement by istringstream
void split(string& str, vector<string>& sv, char flag);

// Name: split
// Function: like split in Python, split string into vector with seperator = flag
// Example: split "abc and def and hij" by "and" into ["abc", "def", "hij"]
// Input:
//      string& str: string to be split
//      vector<string>& sv: vector to put string after split
//      string flag: string to split
// output: void
// Decription: 
//      Implement by string.find
void split_string(string& str, vector<string>& sv, string& flag);


// Name: get_token
// Function: get one token from string and erase token from string
// Example: token got from "aaa bbb ccc" is "aaa", and string will be erase to "bbb ccc"
// Input:
//      string &s: string input
// output: 
//      token string
// Decription: 
//      Implement by string.find
string get_token(string &s); 

// Name: strip
// Function: remove space from [both sides] of string, like strip in Python
// Example: "   aaabbb   " -> strip -> "aaabbb"
// Input:
//      string &str: string input
// output: 
//      string after strip
// Decription: 
//      input string is changed as well
string strip(string& s);


// Name: lstrip
// Function: remove space from [left side] of string, like strip in Python
// Example: "   aaabbb   " -> lstrip -> "aaabbb   "
// Input:
//      string &str: string input
// output: 
//      string after strip
// Decription: 
//      input string is changed as well
string lstrip(string& s);


// Name: rstrip
// Function: remove space from [left side] of string, like strip in Python
// Example: "   aaabbb   " -> rstrip -> "   aaabbb"
// Input:
//      string &str: string input
// output: 
//      string after strip
// Decription: 
//      input string is changed as well
string rstrip(string& s);


// Name: icasecompare
// Function: compare two string equal or not ignoring case difference
// Example: icasecompare("abc", "ABC") returns true
// Input:
//      string &a: string 1 to be compared
//      string &b: string 2 to be compared
// output: 
//      bool: equal or not
bool icasecompare(const string& a, const string& b);

/*
@brief 根据字符串str解析数据类型，非法则标记为int，（后续我们会对Int做额外的容错处理）
@param str 待解析的字符串
@return 解析出的DataType类型变量
*/
DataType ParseDataType(string& str);


/*
@brief 根据字符串str，和对应的type 解析出union value数据, 这里对int做容错处理，无法解析时throw SyntaxError
@param str 待解析的字符串
@param type str经由 ParseDataType(str)得到的类型
@return 解析出的union Value类型
*/
Value ParseStringType(DataType type, string& str);

/*
@brief 根据where字符串str（如a=1 and b=2)，解析出condition
@param where_str 待解析的字符串(eg. a < 1 and b <> 3)
@return 解析出的ConditionUnit vector
*/
vector<ConditionUnit> ParseCondition(string where_str);

#endif