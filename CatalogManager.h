#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"MiniSQL.h"
using namespace std;

class CatalogManager
{
public:
	CatalogManager();
	~CatalogManager();

	//创建包含Catalog的表，需要检查重复性，返回值true表示成功，false表示失败
	bool CreateTable(Table& table);	
	//创建索引，需要检查重复性，返回值1表示成功，0表示名称重复， -1表示对应的表不存在
	int CreateIndex(Index& index);	

	//删除表，通过name判断删除哪张表，需要检查存在性，返回值true表示成功，false表示失败
	bool DropTable(string& name);	
	//删除索引，通过name判断删除哪个索引，需要检查存在性，返回值true表示成功，false表示失败
	bool DropIndex(string& name);	

	//这里实际上只能检测type,类型正确返回true
	bool CheckAttr(Attribute& attr, struct Unit& data); 
	//这个我没用到，不知道要不要调用
	bool CheckCond(ConditionUnit& cond); 

	//这里有table不需要传入attribute to：wyc, (wyc:get)
	//判断是否可以插入：检查每个DataUnit中的数据类型是否与表定义的数据类型匹配
	bool InsertTest(string& table_name, Tuple& data);	
	
	//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
	//返回值：-3（表格不存在）-2（attr出错） -1（选择条件出错）；0（只能通过遍历Record查询）；1（可以利用索引优化查询）
	pair<int, string> SelectTest(string& table_name, vector<string>& Attr, vector<ConditionUnit>& condition);	

	//获取表名为table_name的Catalog信息，如果不存在则返回空的Table
	Table* GetTableCatalog(string& table_name);	
	Index* GetIndexCatalog(string& index_name);
		
	//返回table_name表中的Index指针集合，如果不存在则返回空的Index
	vector<Index*> TableToIndex(string& table_name);
	//获取所有的Table指针
	vector<Table*> GetAllTable();
	//获取所有的Index指针
	vector<Index*> GetAllIndex();


	//判断表格是否存在，选择条件是否有误，将attr_name转化成attr_num
	//返回值：-2（表格不存在） -1（删除条件出错）；0（只能通过遍历Record删除）；1（可以利用索引优化删除）
	pair<int, string> DeleteTest(string& table_name, vector<ConditionUnit>& condition);	


private:
	vector<Table*> m_table;
	vector<Index*> m_index;
	
    string table_name = "./database/meta/table.meta";
    string index_name = "./database/meta/index.meta";

	string NameToTF(string& name);
	string NameToIF(string& name);

	void replace();

	int FindTable(string& table_name);
	int FindIndex(string& index_name);

	void readint(int& t, fstream& f);
	void readstring(string&s, fstream& f);
	void readbool(bool&b, fstream& f);
	void readAttr(Attribute& a, fstream& f);
	void readIndex(Index& i, fstream& f);
	void writestring(string&s, fstream& f);
	void writeint(int& i, fstream& f);
	void writebool(bool&b, fstream& f);
	void writeAttr(Attribute& a, fstream& f);
	void writeTable(Table* t, fstream& f);
	void writeIndex(Index* i, fstream& f);
	void writeallTable(fstream& f);
	void writeallIndex(fstream& f);
	void readallTable(fstream& f);
	void readallIndex(fstream& f);
	Table* readTable(fstream& f);
};


