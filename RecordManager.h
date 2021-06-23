#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iterator>
#include "MiniSQL.h"
#include "BufferManager.h"
// #include "IndexManager.h"
using namespace std;

class RecordManager
{
private:
	BufferManager *bmanager;
	// IndexManager *imanager;
	Tuple ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const;

public:
	RecordManager()
	{
		bmanager = new BufferManager();
	};
	~RecordManager()
	{
		delete bmanager;
	};

	/*
		函数功能：根据表名得到存储表数据的文件名
		传入参数：表名
		返回值：存储表数据文件的文件名
	*/
	string GetDataFileName(const string tablename) const
	{
		// cout<<"[DataFileName Debug]:"<<TABLE_PATH<<" "<<tablename<<" "<<TABLE_SUFFIX<<endl;
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
		函数功能：创建一个表数据文件
		传入参数：Table类变量
		返回值：没有返回值
	*/
	void CreateTableFile(const Table &table);

	/*
		函数功能：清空被删除文件的block
		传入参数：Table类变量
		返回值：没有返回值
	*/
	void DropTableFile(const Table &table);

	/*
		函数功能：向表文件中插入元组，支持每次一条元组的插入操作
		传入参数：表名称，元组变量
		返回值：没有返回值
	*/
	// By wyc: 由于tuple len是新加的, catalog没适配，我这里获取不到，里面的那部分先注释掉了

	void InsertTuple(const Table &table, const Tuple &tuple);
	// void InsertTuple(const Table &table, const Tuple &tuple, Index & index);

	bool ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;

	//数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
	// wyc：似乎所有写Select的人都忽略了还有Attr list这个东西，罢了，我在Interpreter里加吧
	vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
	// vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>(), Index& index) const;

	//删除元组，支持每次一条或多条记录的删除操作
	void DeleteTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>());

	unsigned int GetTuplelen(const Table &table) const;
};

#endif //MINISQL_RECORDMANAGER_H