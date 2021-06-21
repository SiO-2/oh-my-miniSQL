#ifndef __Minisql__IndexManager__
#define __Minisql__IndexManager__

#include <map>
#include <string>
#include <sstream>
#include <fstream>
//#include "API.h"
#include "BPlusTree.h"
#include "Buffer_manager.h"
#define TYPE_FLOAT -1	//the type of the attribute,-1 represents float
#define TYPE_INT 0		//0 represents int
// other positive integer represents char and the value is the number of char


using namespace std;
class API;



class IndexInfo {
public:
	IndexInfo(){}
	IndexInfo(string name, string table, string attr, int type):indexName(name), tableName(table), Attribute(attr), type(type) {
	}
	string indexName;//index��
	string tableName;//����
	string Attribute;//������
	int type;//�������ͣ�����֮һ��
};

/*���õ���������.index�ļ�����ʽ���ڴ����ϵ�
���ݿ�����ʱ��IndexManager�ഴ��ʵ�����������ϵ�.index�ļ����룬��������Ӧ��B+������ͨ��map����ӳ������
����B+����.index�ļ��ѽ���ӳ�䣬��˱��ӿڵĵ����߲���Ҫ֪��B+�������ƺ�ʵ�֣�ֻ��֪��.index�ļ������ƺ��������ͣ����ɶ��������в���*/
class IndexManager {
private:
	BufferManager buffer;//index��ʹ�õ�buffer

	//�������͵�map��������ÿ���ļ�����string����һ��B+����BPlusTree����Ӧ���������ڱ���͹���index
	typedef map<string, BPlusTree<int> *> intMap;
	typedef map<string, BPlusTree<string> *> stringMap;
	typedef map<string, BPlusTree<float> *> floatMap;

	API * api;

	//�����Ĵ���
	/*static */intMap indexIntMap;
	/*static */stringMap indexStringMap;
	/*static */floatMap indexFloatMap;

	//�������͵���ʱ���������������������͵�������
	int intTmp;
	float floatTmp;
	string stringTmp;

	int getDegree(int type);//�õ�һ��block�����ܱ����index����
	int getKeySize(int type);//�õ���ǰ������Ĵ�С
	void setKey(int type, string key);//���õ�ǰ��������

public:
	IndexManager(API* ap);//���캯��������ΪAPI�������API����ȡ.index�ļ��б������index��Ϣ
	~IndexManager();//�������������ٶ���ʱ�Ὣ����index��Ϣд�ش�����

	void createIndex(string filePath, int type);//����һ��ָ�����ƣ�filePath�������ͣ�type����.index�ļ�
												//����createIndex("salary_index.index",TYPE_INT);
												//�ᴴ��һ����Ϊ��salary_index����.index�ļ�������ΪTYPE_INT

	void dropIndex(string filePath, int type);//ɾ��һ��ָ�����ƣ�filePath�������ͣ�type����.index�ļ����÷�ͬ��

	offsetNumber searchIndex(string filePath, string key, int type);//��B+����������Ϊkey��index������ƫ����
																	

	void insertIndex(string filePath, string key, offsetNumber blockOffset, int type);//��B+���в���һ��������Ϊkey��index
																					  

	void deleteIndex(string filePath, string key, int type);//��B+����ɾ��һ��������Ϊkey��index
															//����deleteIndex("salary_index.index","20000",TYPE_INT);


	void readIndex(string filePath, int type);//��һ��ָ�����ƣ�filePath�������ͣ�type����.index�ļ��ж�ȡindex��Ϣ��������ӦB+����������Ӧ��map�����н������ߵ�ӳ��
};



#endif

