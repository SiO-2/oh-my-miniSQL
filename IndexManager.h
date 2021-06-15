#pragma once
#include "BPTree.h" //�е�໹ûд��qaq,��װ���˾���
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
	//������������ֱ�ӱ�ʾstring�ĳ���
	
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
			return type;  //������������ֱ�ӱ�ʾstring�ĳ����Ա���ֱ�ӷ���keysize�ĳ���
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

