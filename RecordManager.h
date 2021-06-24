#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include "MiniSQL.h"
#include "BufferManager.h"
#include "IndexManager.h"
using namespace std;

class RecordManager
{
private:
	Tuple ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const;

public:
	IndexManager *imanager;
	BufferManager *bmanager;

public:
	RecordManager()
	{
		bmanager = new BufferManager();
		imanager = new IndexManager();
	};
	~RecordManager()
	{
		delete bmanager;
		delete imanager;
	};

	string GetDataFileName(const string tablename) const
	{
		// cout<<"[DataFileName Debug]:"<<TABLE_PATH<<" "<<tablename<<" "<<TABLE_SUFFIX<<endl;
		return TABLE_PATH + tablename + TABLE_SUFFIX;
	}

	string GetIndexFileName(const string tablename) const
	{
		return INDEX_PATH + tablename + INDEX_SUFFIX;
	}

	string GetMetaFileName(const string tablename) const
	{
		return META_PATH + tablename + META_SUFFIX;
	}

	// void CreateTableFile(const Table &table);

	// void DropTableFile(const Table &table);

	void InsertTuple(const Table &table, const Tuple &tuple);
	// void InsertTuple(const Table &table, const Tuple &tuple, Index & index);

	bool ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;

	vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
	// vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>(), Index& index) const;

	void DeleteTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>());

	unsigned int GetTuplelen(const Table &table) const;

	void CreateIndex(const Index &index);
};

#endif //MINISQL_RECORDMANAGER_H