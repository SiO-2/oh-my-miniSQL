#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include "MiniSQL.h"
#include "BufferManager.h"
using namespace std;

//数据文件的存放目录
const string TABLE_PATH = ".database/data/";
const string INDEX_PATH = ".database/index/";
const string META_PATH = ".database/meta/";

//文件后缀名
const string TABLE_SUFFIX = ".table";
const string INDEX_SUFFIX = ".index";
const string META_SUFFIX = ".meta";

class RecordManager
{
private:
	BufferManager *bmanager;
	// IndexManager *imanager;

public:
	RecordManager(){};
	~RecordManager(){};

	/*
		函数功能：根据表名得到存储表数据的文件名
		传入参数：表名
		返回值：存储表数据文件的文件名
	*/
	string GetDataFileName(const string tablename) const
	{
		return TABLE_PATH + tablename + TABLE_SUFFIX;
	}

	/*
		函数功能：根据表名得到存储表index的文件名
		传入参数：表名
		返回值：存储表index文件的文件名
	*/
	string GetIndexFileName(const string tablename) const
	{
		return INDEX_PATH + tablename + INDEX_SUFFIX;
	}

	/*
		函数功能：根据表名得到存储表元数据的文件名
		传入参数：表名
		返回值：存储表元数据文件的文件名
	*/
	string GetMetaFileName(const string tablename) const
	{
		return META_PATH + tablename + META_SUFFIX;
	}

	/*
		函数功能：创建一个新的表文件
		传入参数：表名称
		返回值：没有返回值
	*/
	void CreateTableFile(const Table &table);

	/*
		函数功能：删除表文件
		传入参数：表名称
		返回值：没有返回值
	*/
	void DropTableFile(const Table &table);

	/*
		函数功能：向表文件中插入元组，支持每次一条元组的插入操作
		传入参数：表名称，元组变量
		返回值：没有返回值
	*/
	void InsertTuple(const string &tablename, const Tuple &tuple);

	//数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
	bool SelectTuple(const string &tablename, const vector<string> &attribute = vector<string>(), const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;

	//删除元组，支持每次一条或多条记录的删除操作
	bool DelectTuple(const string &tablename, const vector<ConditionUnit> &condition = vector<ConditionUnit>());
};

#endif //MINISQL_RECORDMANAGER_H