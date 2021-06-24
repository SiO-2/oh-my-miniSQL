#pragma once
#include <iostream>
#include <vector>
// #include"CatalogManager.h"
// #include"RecordManager.h"
// #include"IndexManager.h"
using namespace std;

//数据文件的存放目录
const string TABLE_PATH = "./database/data/";
const string INDEX_PATH = "./database/index/";
const string META_PATH = "./database/meta/";

//文件后缀名
const string TABLE_SUFFIX = ".table";
const string INDEX_SUFFIX = ".index";
const string META_SUFFIX = ".meta";

typedef enum
{
	INT_UNIT,
	CHAR_UNIT,
	FLOAT_UNIT
} DataType;

typedef enum
{
	NO_OP_, // No Opeartion
	EQ_,	// =
	NE_,	// <>
	L_,		// <
	G_,		// >
	LE_,	// <=
	GE_		// >=
} OpCode;

class TableMetadata
{
public:
	string name;	   //表名
	int attr_num;	   //表中字段（列）数，属性attr的编号从1开始
	int primary_key;   //主键，用attr的编号表示，若没有则为-1
	int primary_index; //主索引，用attr的编号表示，若没有则为-1
	TableMetadata();
	TableMetadata(string name, int attr_num, int primary_key = -1, int primary_index = -1);
	TableMetadata(TableMetadata &t);
	void Print();
};

// 记录属性的类
class Attribute
{
public:				  //直接都Public算了吧
	string name;	  //属性名
	int type;		  //属性的类型，实际上是DataType, 记成int保留可扩展性
	int charlen;	  //如果是char类型，保存其最大长度 char(n)
	bool notnull;	  //not null标记
	bool unique;	  //unique 标记
	bool primary_key; // primary key 标记
public:
	// name: Attribute constructor
	// Function: init value in class
	Attribute(string name, string typestr, bool notnull = false, bool unique = false, bool primary_key = false);
	Attribute(){};
	// name:set_pk
	// Function: set primary key of attribute, becase "primary key(pk)" usually occurs in the end
	void set_pk(bool pk);
	// name: print
	// Function: Print Attribute info
	void Print();
};

class Table //数据库中的一张表
{
public:
	TableMetadata m_metadata;	   //表的定义信息
	vector<Attribute> m_attribute; //表中字段的信息
	vector<string> Index_name;
	Table(TableMetadata m_metadata, vector<Attribute> m_attribute);
	Table(Table &table);
	Table();
	void Print();
};

class Index //建立在表m_table中attr_num上的索引，名为m_index_name
{
public:
	string index_name;
	Table* table;	//确实需要指向table，但是这里不能放指针，读出来每次都不一样，等index那边搞定再改
	string table_name; //（zxy：这里应该存的是表名，然后从内存读出来的时候可以加载出表的指针（个人理解
	int attr_num;	//索引建立在该属性上
	Index(Index& index);
	Index();
	void Print();
};

/*
class DataUnit		//数据单元，里面包含了数据类型，值以及对应的属性编号，用于插入语句的输入以及选择语句的返回值
{
public:
	union Value
	{
		int int_value;
		char* char_n_value;
		float float_value;
	} value;
	int attr_num; //这个属性Interpreter调用的时候没法赋值
	// bool isnull;
	string attr_name;
	DataType data_type;
	void Print();
};
*/

//DataUnit
union Value
{
	int int_value;
	char *char_n_value;
	// string char_n_value;
	float float_value;
};

//不这样写根本没法做检测啊kora！！
class Unit
{
public:
	Value value;
	DataType datatype;
	Unit();
	Unit(Value value, DataType datatype);
	void Print();
};

class Tuple
{
public:
	vector<struct Unit> tuple_value;
	bool valid;
	Tuple();
	void Print();
	void Print(vector<int>& int_vec);
};

struct BPlusNode
{
	unsigned int block_offset;
	unsigned int tuple_offset;
};

class ConditionUnit //条件单元，用于select中的where，格式：attr_name attr_num  op_code  value（列 op 值）
{
public:
	string attr_name;
	int attr_num; //attr_num Interpreter调用的时候可能没法传值
	OpCode op_code;
	DataType data_type;
	union Value
	{
		int int_value;
		char *char_n_value; // 不能用string, union中的类型不能自带构造函数（by wyc: 不太确定 但确实改了就好了）
		float float_value;
		// Value(){};
	} value;
	ConditionUnit(){};
	ConditionUnit(string attr_name, int attr_num, OpCode op_code, DataType data_type);
	void Print();
};

// /*
// * 创建表，表的信息由table提供
// * 返回值：如果创建成功返回true，创建失败返回false
// */
// bool CreateTable(Table& table);

// /*
// * 删除表，表的信息由table_name提供
// * 返回值：如果删除成功返回true，创建失败返回false
// */
// bool DropTable(string& table_name);

// /*
// * 创建索引，索引的信息由index提供
// * 返回值：如果创建成功返回true，创建失败返回false
// */
// bool CreateIndex(Index& index);

// /*
// * 删除索引，索引的信息由table_name提供
// * 返回值：如果删除成功返回true，创建失败返回false
// */
// bool DropIndex(string& index_name);

// /*
// * 查找名为table_name的表的信息
// * 返回值：table_name的表的信息
// */
// Table GetTableInfo(string& table_name);

// /*
// * 将数据data插入到表名为table_name的表中
// * 返回值：如果插入成功返回true，插入失败返回false
// */
// bool Insert(string& table_name, vector<struct DataUnit>& data);

// /*
// * 将数据data插入到表名为table_name的表中
// * 返回值：选择的结果
// */
// vector<vector<struct DataUnit>> Select(string& table_name, vector<struct ConditionUnit>& condition);

// /*
// * 将数据data从表名为table_name删除
// * 返回值：如果删除成功返回true，删除失败返回false
// */
// bool Delete(string& table_name, vector<struct ConditionUnit>& condition);
