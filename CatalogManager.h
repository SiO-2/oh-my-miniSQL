#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"MiniSQL.h"
using namespace std;


class CatalogManager
{
public:
	CatalogManager() {};
	~CatalogManager() {};

	bool CreateDatabase(string& db_name); //不知道需要不需要？
	bool CreateTable(struct Table& table);	//创建包含Catalog的表，需要检查重复性，返回值true表示成功，false表示失败
	bool CreateIndex(struct Index& index);	//创建索引，需要检查重复性，返回值true表示成功，false表示失败

	bool DropTable(string& table_name);	//删除表，通过table_name判断删除哪张表，需要检查存在性，返回值true表示成功，false表示失败
	bool DropIndex(string& index_name);	//删除索引，通过index_name判断删除哪个索引，需要检查存在性，返回值true表示成功，false表示失败
	bool DropDatabase(string& db_name); //需要吗？

	bool CheckAttr(Attribute& attr, DataUnit& data);
	bool CheckCond(ConditionUnit& cond);

	bool InsertTest(string& table_name, vector<struct DataUnit>& data);	//判断是否可以插入：检查每个DataUnit中的数据类型是否与表定义的数据类型匹配
	
	int SelectTest(string& table_name, vector<struct ConditionUnit>& condition);	//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
	//返回值：-1（选择条件出错）；0（只能通过遍历Record查询）；1（可以利用索引优化查询）
	// !!

	Table* GetTableCatalog(string& table_name);	//获取表名为table_name的Catalog信息，如果不存在则返回空的Table
	
	Index* TableToIndex(string& table_name);	//返回table_name表中的Index，如果不存在则返回空的Index

	int DeleteTest(string& table_name, vector<struct ConditionUnit>& condition);	//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
	//返回值：-1（删除条件出错）；0（只能通过遍历Record删除）；1（可以利用索引优化删除）

private:
	vector<struct Table> m_table;
	vector<struct Index> m_index;

	int FindTable(string& table_name);
	int FindIndex(string& index_name);
};


