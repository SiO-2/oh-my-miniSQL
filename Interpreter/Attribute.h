#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include <string>
using namespace std;

// 三种数据类型
typedef enum{
    INT_UNIT,
    CHAR_UNIT,
    FLOAT_UNIT
} TYPE_UNIT;

// 记录属性的类
class Attribute{
public: //方便调试先都Public
    string name;//属性名
    int type;//属性的类型，实际上是TYPE_UNIT, 记成int保留可扩展性
    int charlen;//如果是char类型，保存其最大长度 char(n)
    bool notnull;//not null标记
    bool unique;//unique 标记
    bool primary_key;// primary key 标记
public:
    // name: Attribute constructor
    // Function: init value in class
    Attribute(string name, string typestr, bool notnull = false, bool unique = false, bool primary_key = false);
    // name:set_pk
    // Function: set primary key of attribute, becase "primary key(pk)" usually occurs in the end
    void set_pk(bool pk);
    // name: print
    // Function: Print Attribute info
    void print();
};

#endif