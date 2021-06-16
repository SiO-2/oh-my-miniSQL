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
// Example: split "abc def hij" by " " into ["abc", "def", "hij"]
// Input:
//      string& str: string to be split
//      vector<string>& sv: vector to put string after split
//      char flag: character to split
// output: void
// Decription: 
//      Implement by istringstream
void split(string& str, vector<string>& sv, char flag);


// Name: get_token
// Function: get one token from string and erase token from string
// Example: token got from "aaa bbb ccc" is "aaa", and string will be erase to "bbb ccc"
// Input:
//      string &str: string input
// output: 
//      token string
// Decription: 
string get_token(string &str); 

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

DataType ParseDataType(string& str);

#endif