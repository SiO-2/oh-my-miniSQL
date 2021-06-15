#pragma once
#include "BPTree.h" //有点多还没写完qaq,假装有了就行
#include "Index.h"
#include "Attribute.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class IndexManager
{
private:
	vector<BPTree<int>*> intIndices; // BTree
	vector<BPTree<string>*> stringIndices;
	vector<BPTree<float>*> floatIndices;

	
	static const int INT = 0;
	static const int FLOAT = -1;
	//这里想用整数直接表示string的长度
	
	int intTemp;
	float floatTemp;
	string stringTemp;
	

	void setKey(int type, string key) 
	{
		stringstream ss;
		ss << key;
		switch (type)
		{
		case INT: ss >> this->intTemp; break;
		case FLOAT: ss >> this->floatTemp; break;
		default:
			ss >> this->stringTemp; break;
		}
	}
	int getKeySize(int type)
	{
		switch (type)
		{
		case INT: return sizeof(int);
		case FLOAT: return sizeof(float);
		default:
			return type;  //这里想用整数直接表示string的长度以便于直接返回keysize的长度
		}
	}

public:
	IndexManager();
	explicit IndexManager(vector<Index>& indexList);
	~IndexManager();

	void createIndex(string fileName, int type);
	void dropIndex(string fileName, int type);
	int searchIndex(string fileName, string key, int type);
	bool insertKeyIntoIndex(string fileName, string key, int type, int offset);
	bool deleteKeyFromIndex(string fileName, string key, int type);
};

