#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include "MiniSQL.h"
#include "BufferManager.h"
using namespace std;

class RecordManager
{
private:
	BufferManager *bmanager;
	IndexManager *imanager;

public:
	RecordManager(){};
	~RecordManager(){};

	bool CreateTable(const string &table);															   //创建表
	bool DropTable(const string &table);																   //删除表
	bool InsertValue(const string &table, const vector<struct DataUnit> &data);						   //插入值
	bool Select(const struct Table &table);																	   //无条件查找全部属性输出
	bool Select(const struct Table &table, const vector<string> &attribute);								   //无条件查找部分属性输出
	bool Select(const struct Table &table, vector<ConditionUnit> &condition);								   //有条件查找全部属性输出
	bool Select(const struct Table &table, const vector<string> &attribute, vector<ConditionUnit> &condition); //有条件查找部分属性输出
	bool Delete(const struct Table &Table);																	   //无条件删除所有元组
	bool Delect(const struct Table &table, const vector<ConditionUnit> &condition);							   //删除对应条件的元组
	bool SelectFromIndex(const struct Table &table);														   //根据索引提供的位置信息进行查找全部属性输出
	bool SelectFromIndex(const struct Table &table, const vector<string> &attribute);						   //根据索引提供的位置信息进行查找部分属性输出
};

#endif //MINISQL_RECORDMANAGER_H