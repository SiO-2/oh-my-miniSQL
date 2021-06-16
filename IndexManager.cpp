#include "IndexManager.h"

const int FLOAT = -1;
const int INT = 0;

IndexManager::IndexManager()
{
	//vector<Index> indexList;
}

IndexManager::IndexManager(vector<Index>& indexList)
{
	for (int i = 0; i < indexList.size(); i++)
	{
		int keySize = this->getKeySize(indexList[i].type);
		switch (indexList[i].type)
		{
		case INT: this->intIndices.push_back(new BPTree<int>(indexList[i].indexName, keySize, 19)); break;
		case FLOAT: this->floatIndices.push_back(new BPTree<float>(indexList[i].indexName, keySize, 19)); break;
		default: this->stringIndices.push_back(new BPTree<string>(indexList[i].indexName, keySize, 19)); break;
		}
	}
}


IndexManager::~IndexManager()
{
	//
}

//Create an empty index with new fileName
void IndexManager::createIndex(string fileName, int type)
{
	int keySize = getKeySize(type);
	int degree = 19;
	FILE* fp = fopen(fileName.c_str(), "wb+");
	if (!fp)
		return;
	fclose(fp);
	if (type == INT)
		intIndices.push_back(new BPTree<int>(fileName, keySize, degree));
	else if (type == FLOAT)
		floatIndices.push_back(new BPTree<float>(fileName, keySize, degree));
	else
		stringIndices.push_back(new BPTree<string>(fileName, keySize, degree));
}

void IndexManager::dropIndex(string fileName, int type)
{
	if (type == INT)
	{
		for (int i = 0; i < intIndices.size(); i++)
		{
			if (intIndices[i]->fileName == fileName)
			{
				delete intIndices[i];
				std::vector<BPTree<int>*>::iterator it = intIndices.begin() + i;
				intIndices.erase(it);
				return;
			}
		}
	}
	else if (type == FLOAT)
	{
		for (int i = 0; i < floatIndices.size(); i++)
		{
			if (floatIndices[i]->fileName == fileName)
			{
				delete floatIndices[i];
				std::vector<BPTree<float>*>::iterator it = floatIndices.begin() + i;
				floatIndices.erase(it);
				return;
			}
		}
	}
	else
	{
		for (int i = 0; i < stringIndices.size(); i++)
		{
			if (stringIndices[i]->fileName == fileName)
			{
				delete stringIndices[i];
				std::vector<BPTree<string>*>::iterator it = stringIndices.begin() + i;
				stringIndices.erase(it);
				return;
			}
		}
	}
	//remove(fileName.c_str());//Remove file from disk
}

int IndexManager::searchIndex(string fileName, string key, int type)
{
	setKey(type, key);
	if (type == INT)
	{
		for (int i = 0; i < intIndices.size(); i++)
		{
			if (intIndices[i]->fileName == fileName)
				return intIndices[i]->searchKey(intTemp);
		}
		return -1;
	}
	else if (type == FLOAT)
	{
		for (int i = 0; i < floatIndices.size(); i++)
		{
			if (floatIndices[i]->fileName == fileName)
				return floatIndices[i]->searchKey(floatTemp);
		}
		return -1;
	}
	else
	{
		for (int i = 0; i < stringIndices.size(); i++)
		{
			if (stringIndices[i]->fileName == fileName)
				return stringIndices[i]->searchKey(stringTemp);
		}
		return -1;
	}
}

bool IndexManager::insertKeyIntoIndex(string fileName, string key, int type, int offset)
{
	setKey(type, key);
	if (type == INT)
	{
		for (int i = 0; i < intIndices.size(); i++)
		{
			if (intIndices[i]->fileName == fileName)
			{
				return intIndices[i]->insertKey(intTemp, offset);
				//return;
			}
		}
		cout << "Index Not Found!" << endl;
	}
	else if (type == FLOAT)
	{
		for (int i = 0; i < floatIndices.size(); i++)
		{
			if (floatIndices[i]->fileName == fileName)
			{
				return floatIndices[i]->insertKey(floatTemp, offset);
				
			}
		}
		cout << "Index Not Found!" << endl;
	}
	else
	{
		for (int i = 0; i < stringIndices.size(); i++)
		{
			if (stringIndices[i]->fileName == fileName)
			{
				return stringIndices[i]->insertKey(stringTemp, offset);
				//return;
			}
		}
		cout << "Index Not Found!" << endl;
	}
	return false;
}

bool IndexManager::deleteKeyFromIndex(string fileName, string key, int type)
{
	setKey(type, key);
	if (type == INT)
	{
		for (int i = 0; i < intIndices.size(); i++)
		{
			if (intIndices[i]->fileName == fileName)
			{
				return intIndices[i]->deleteKey(intTemp);
				//return;
			}
			
		}
		cout << "Index Not Found!" << endl;
	}
	else if (type == FLOAT)
	{
		for (int i = 0; i < floatIndices.size(); i++)
		{
			if (floatIndices[i]->fileName == fileName)
			{
				return floatIndices[i]->deleteKey(floatTemp);
				//return;
			}
			
		}
		cout << "Index Not Found!" << endl;
	}
	else
	{
		for (int i = 0; i < stringIndices.size(); i++)
		{
			if (stringIndices[i]->fileName == fileName)
			{
				return stringIndices[i]->deleteKey(stringTemp);
				//return;
			}
			
		}
		cout << "Index Not Found!" << endl;
	}
	return false;
}
