#ifndef __Minisql__IndexManager__
#define __Minisql__IndexManager__

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "BPlusTree.h"
#include "BufferManager.h"
#include "MiniSQL.h"

// other positive integer represents char and the value is the number of char
using namespace std;

/*建好的索引是以.index文件的形式存在磁盘上的
数据库运行时，IndexManager类创建实例，将磁盘上的.index文件读入，并建立相应的B+树，再通过map容器映射起来
由于B+树和.index文件已建立映射，因此本接口的调用者不需要知道B+树的名称和实现，只需知道.index文件的名称和索引类型，即可对索引进行操作*/
class IndexManager {
private:
	BufferManager buffer;//index所使用的buffer

	//三种类型的map容器，将每个文件名（string）和一种B+树（BPlusTree）对应起来，用于保存和管理index
	typedef map<string, BPlusTree<int> *> intMap;
	typedef map<string, BPlusTree<string> *> stringMap;
	typedef map<string, BPlusTree<float> *> floatMap;

	//API * api;

	//容器的创建
	/*static */intMap indexIntMap;
	/*static */stringMap indexStringMap;
	/*static */floatMap indexFloatMap;

	//三种类型的临时变量，用来保存三种类型的搜索码
	int intTmp;
	float floatTmp;
	string stringTmp;


public:
	IndexManager(vector<Index> indexList);//构造函数，参数为API，会调用API来获取.index文件列表并导入index信息
	~IndexManager();//析构函数，销毁对象时会将所有index信息写回磁盘中
	void readIndexfromfile(const Index& index);

	void insertIndex(const Index& index, Unit unit_key, offsetNumber Offset);

	void deleteIndex(const Index& index, Unit unit_key);


	int getDegree(int len);

	void createIndex(const Index& index);

	void dropIndex(const Index& index);


	offsetNumber searchIndex(const Index& index, ConditionUnit unit_key);


};



#endif

