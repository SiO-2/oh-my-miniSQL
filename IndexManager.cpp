#include "IndexManager.h"
#include "MiniSQL.h"

//函数功能：index manager的构造函数
//传入参数：已经存在的index的vector

// IndexManager::IndexManager(vector<Index> indexList) {//构造函数，需要读取index文件并建立对应B+树
// 	for(auto i = indexList.begin(); i != indexList.end(); i++) {
// 		readIndexfromfile(*i);
// 	}
// }

IndexManager::IndexManager(){

}

void:: IndexManager::setindexIntMap(vector<Index>& indexList){
	for(auto i = indexList.begin(); i != indexList.end(); i++) {
		readIndexfromfile(*i);
	}
}

//函数功能：在已经存在索引文件的情况下，读入索引文件，并创建B+树
void IndexManager::readIndexfromfile(const Index& index) {//
	
	int len;
	string filename_index = INDEX_PATH + index.index_name + ".index";

	int data_type = index.table->m_attribute[index.attr_num].type;


	
	ifstream newfile_in(filename_index.c_str());
	ofstream newfile_out;


	if (!newfile_in) {
		cout << "in Create index: file " << filename_index << "not exist" << endl;
		newfile_in.close();
	}
	else {
		newfile_out.open(filename_index.c_str());
		newfile_out.close();
	}


	int keysize;
	int degree; 
	//根据文件名创建不同类型的B+数，并在map容器中建立映射
	if (data_type == INT_UNIT) {
		keysize = sizeof(int);
		degree = getDegree(keysize);

		BPlusTree<int>* tree = new BPlusTree<int>(filename_index, keysize, degree);
		tree->ReadTree();
		indexIntMap.insert(intMap::value_type(filename_index, tree));
	}
	else if (data_type == FLOAT_UNIT) {
		keysize = sizeof(float);
		degree = getDegree(keysize);
		BPlusTree<float>* tree = new BPlusTree<float>(filename_index, keysize, degree);
		tree->ReadTree();
		indexFloatMap.insert(floatMap::value_type(filename_index, tree));
	}
	else if (data_type == CHAR_UNIT) {
		keysize = index.table->m_attribute[index.attr_num].charlen;
		degree = getDegree(keysize);
		BPlusTree<string>* tree = new BPlusTree<string>(filename_index, keysize, degree);
		tree->ReadTree();
		indexStringMap.insert(stringMap::value_type(filename_index, tree));
	}
	else {
		cout << "ERROR: in create index: Invalid type" << endl;
	}
}


void IndexManager::insertIndex(const Index& index, Unit unit_key, offsetNumber Offset)
{
	int len;
	string filename_index = INDEX_PATH + index.index_name + ".index";
	
	Value value = unit_key.value;
	DataType data_type = unit_key.datatype;

	// cout<<"[Index Debug]: intTmp"<<intTmp<<endl;
	// cout<<"[Index Debug]: datatype "<<data_type<<endl;

	if (data_type == INT_UNIT) //int 
	{
		// cout<<"[Index Debug]: value.int_value "<<value.int_value<<endl;
		len = sizeof(int);
		intTmp = value.int_value;

		intMap::iterator itInt = indexIntMap.find(filename_index);
		if (itInt == indexIntMap.end()) {
			cout << "Error:in insert index, index not exits" << endl;
			return;
		}
		else {
			itInt->second->Insert(intTmp, Offset);
			itInt->second->WriteBack();
			return;
		}
	}

	else if (data_type == FLOAT_UNIT) {

		len = sizeof(float);
		floatTmp = value.float_value;
		floatMap::iterator itFloat = indexFloatMap.find(filename_index);
		if (itFloat == indexFloatMap.end()) {
			cout << "Error:in insert index, index not exits" << endl;
			return;
		}
		else {
			itFloat->second->Insert(floatTmp, Offset);
			itFloat->second->WriteBack();
			return;
		}
	}

	

	else if (data_type == CHAR_UNIT) {
		len = index.table->m_attribute[index.attr_num].charlen;
		stringTmp = value.char_n_value;
		stringMap::iterator itString = indexStringMap.find(filename_index);
		if (itString == indexStringMap.end()) {
			cout << "Error:in insert index, index not exits" << endl;
			return;
		}
		else {
			itString->second->Insert(stringTmp, Offset);
			itString->second->WriteBack();
			return;
		}
	}
	else {
		cout << "ERROR: in insert index: Invalid type" << endl;
		return;
	}


}



void IndexManager::deleteIndex(const Index& index, Unit unit_key)
{
	int len;
	string filename_index = INDEX_PATH + index.index_name + ".index";

	Value value = unit_key.value;
	DataType data_type = unit_key.datatype;


	if (data_type == INT_UNIT) //int 
	{
		len = sizeof(int);
		intTmp = value.int_value;

		intMap::iterator itInt = indexIntMap.find(filename_index);
		if (itInt == indexIntMap.end()) {
			cout << "Error:in delete index, index not exits" << endl;
			return;
		}
		else {
			itInt->second->Delete(intTmp);
			itInt->second->WriteBack();
			return;
		}
	}

	else if (data_type == FLOAT_UNIT) {

		len = sizeof(float);
		floatTmp = value.float_value;
		floatMap::iterator itFloat = indexFloatMap.find(filename_index);
		if (itFloat == indexFloatMap.end()) {
			cout << "Error:in delete index, index not exits" << endl;
			return;
		}
		else {
			itFloat->second->Delete(floatTmp);
			itFloat->second->WriteBack();
			return;
		}
	}



	else if (data_type == CHAR_UNIT) {
		len = index.table->m_attribute[index.attr_num].charlen;
		stringTmp = value.char_n_value;
		stringMap::iterator itString = indexStringMap.find(filename_index);
		if (itString == indexStringMap.end()) {
			cout << "Error:in delete index, index not exits" << endl;
			return;
		}
		else {
			itString->second->Delete(stringTmp);
			itString->second->WriteBack();
			return;
		}
	}
	else {
		cout << "ERROR: in insert index: Invalid type" << endl;
		return;
	}




}





IndexManager::~IndexManager() {
	//销毁对象前将index的改变写回缓冲区中
	for(intMap::iterator itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++) {
		if(itInt->second) {
			itInt->second->WriteBack();
			delete itInt->second;
		}
	}
	for(stringMap::iterator itString = indexStringMap.begin(); itString != indexStringMap.end(); itString++) {
		if(itString->second) {
			itString->second->WriteBack();
			delete itString->second;
		}
	}
	for(floatMap::iterator itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++) {
		if(itFloat->second) {
			itFloat->second->WriteBack();
			delete itFloat->second;
		}
	}
}

int IndexManager::getDegree(int len) {//获取块能存储的key数量
	int degree =  BLOCKSIZE / (len + sizeof(offsetNumber) + 1);
	//那一个1是valid位
	return degree;
}


void IndexManager::createIndex(const Index& index) {//

	int len;
	string filename_index = INDEX_PATH + index.index_name + ".index";

    int data_type = index.table->m_attribute[index.attr_num].type;


	ifstream newfile_in(filename_index.c_str());
	ofstream newfile_out;


	if (newfile_in) {
		cout << "in Create index: file " << filename_index << "already exist" << endl;
		newfile_in.close();
	}
	else {
		newfile_out.open(filename_index.c_str());
		newfile_out.close();
	}




	int keysize;
	int degree;
	//根据文件名创建不同类型的B+数，并在map容器中建立映射
	if (data_type == INT_UNIT) {
		keysize = sizeof(int);
		degree = getDegree(keysize);

		BPlusTree<int>* tree = new BPlusTree<int>(filename_index, keysize, degree);
		indexIntMap.insert(intMap::value_type(filename_index, tree));
	}
	else if (data_type == FLOAT_UNIT) {
		keysize = sizeof(float);
		degree = getDegree(keysize);
		BPlusTree<float>* tree = new BPlusTree<float>(filename_index, keysize, degree);
		indexFloatMap.insert(floatMap::value_type(filename_index, tree));
	}
	else if (data_type == CHAR_UNIT) {
		keysize = index.table->m_attribute[index.attr_num].charlen;
		degree = getDegree(keysize);
		BPlusTree<string>* tree = new BPlusTree<string>(filename_index, keysize, degree);
		indexStringMap.insert(stringMap::value_type(filename_index, tree));
	}
	else {
		cout << "ERROR: in create index: Invalid type" << endl;
	}
}





void IndexManager::dropIndex(const Index& index) {//删除索引


	string filename_index = INDEX_PATH + index.index_name + ".index";

    int data_type = index.table->m_attribute[index.attr_num].type;


	ifstream oldfile(filename_index.c_str());
	if(oldfile.is_open()) {
		oldfile.close();
		remove(filename_index.c_str());
	}



	//在map容器中查找索引名，找到后删除B+树及该记录
	if(data_type == INT_UNIT) {
		intMap::iterator itInt = indexIntMap.find(filename_index);
		if(itInt == indexIntMap.end()) {
			cout << "Error:in drop index: index not exits" << endl;
			return;
		}
		else {
			delete itInt->second;
			indexIntMap.erase(itInt);
		}
	}
	else if(data_type == FLOAT_UNIT) {
		floatMap::iterator itFloat = indexFloatMap.find(filename_index);
		if(itFloat == indexFloatMap.end()) {
			cout << "Error:in drop index: index not exits" << endl;
			return;
		}
		else {
			delete itFloat->second;
			indexFloatMap.erase(itFloat);
		}
	}
	else if(data_type == CHAR_UNIT) {
		stringMap::iterator itString = indexStringMap.find(filename_index);
		if(itString == indexStringMap.end()) {
			cout << "Error:in drop index: index not exits" << endl;
			return;
		}
		else {
			delete itString->second;
			indexStringMap.erase(itString);
		}
	}
	else {
		cout << "ERROR: in drop index: Invalid type" << endl;
	}
}





offsetNumber IndexManager::searchIndex(const Index &index, ConditionUnit unit_key) {//查找索引，并返回偏移量

	int len;
	string filename_index = INDEX_PATH + index.index_name + ".index";

	ConditionUnit::Value value = unit_key.value;
	DataType data_type = unit_key.data_type;


	if(data_type == INT_UNIT) {

		intTmp = value.int_value;
		intMap::iterator itInt = indexIntMap.find(filename_index);
		if(itInt == indexIntMap.end()) {
			cout << "Error:in search index, index not exits" << endl;
			return -1;
		}
		else {
			return itInt->second->Search(intTmp);
		}
	}
	else if(data_type == FLOAT_UNIT) {
		floatTmp = value.float_value;
		floatMap::iterator itFloat = indexFloatMap.find(filename_index);
		if(itFloat == indexFloatMap.end()) {
			cout << "Error:in search index, index not exits" << endl;
			return -1;
		}
		else {
			return itFloat->second->Search(floatTmp);
		}
	}
	else if(data_type == CHAR_UNIT) {
		stringTmp = value.char_n_value;
		stringMap::iterator itString = indexStringMap.find(filename_index);
		if(itString == indexStringMap.end()) {
			cout << "Error:in search index, index not exits" << endl;
			return -1;
		}
		else {
			return itString->second->Search(stringTmp);
		}
	}
	else {
		cout << "ERROR: in search index: Invalid type" << endl;
		return -2;
	}
}

