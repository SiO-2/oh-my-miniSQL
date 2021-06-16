#include "RecordManager.h"

//创建一个新的表
bool RecordManager::CreateTableFile(const string &tablename)
{
    string filename = FILEPATH + tablename + TABLE_SUFFIX;
    FILE *fp;
    if ((fp = fopen(filename.c_str(), "wb+")) == NULL)
    {
        printf("Can't create %s\n", table);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

//删除表
bool RecordManager::DropTableFile(const string &tablename){
    string filename=FILEPATH + tablename + TABLE_SUFFIX;
    FlushBlock(filename);
    remove(filename.c_str());
}

//插入记录，支持每次一条记录的插入操作
bool RecordManager::InsertTuple(const string &tablename, const Tuple &tuple){
    string filename=FILEPATH + tablename + TABLE_SUFFIX;
    for()
    BID bid=ReadFile2Block(filename)

}

//数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
bool RecordManager::SelectTuple(const string &tablename, const vector<string> &attribute = NULL, const vector<ConditionUnit> &condition = NULL) const;

//删除元组，支持每次一条或多条记录的删除操作
bool RecordManager::DelectTuple(const string &tablename, const vector<ConditionUnit> &condition = NULL);
