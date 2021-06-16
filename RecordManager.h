#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include "MiniSQL.h"
#include "BufferManager.h"
using namespace std;

//数据文件的存放目录
const string FILEPATH= ".database/data/";

//文件后缀名
const string TABLE_SUFFIX=".table";
const string INDEX_SUFFIX=".index";

class RecordManager
{
private:
	BufferManager *bmanager;
	IndexManager *imanager;

public:
	RecordManager(){};
	~RecordManager(){};

	//创建一个新的表
	bool CreateTableFile(const string &tablename);

	//删除表
	bool DropTableFile(const string &tablename);

	//插入记录，支持每次一条记录的插入操作
	bool InsertTuple(const string &tablename, const vector<struct DataUnit> &tuple);

	//数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
	bool SelectTuple(const Table &tablename, const vector<string> &attribute = NULL, const vector<ConditionUnit> &condition = NULL) const;

	//删除元组，支持每次一条或多条记录的删除操作
	bool DelectTuple(const Table &tablename, const vector<ConditionUnit> &condition = NULL);
};

#endif //MINISQL_RECORDMANAGER_H