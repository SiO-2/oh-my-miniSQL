#include "RecordManager.h"

/*
    函数功能：创建一个表元数据文件以及一个表数据文件
    传入参数：表名称
    返回值：没有返回值
*/
void RecordManager::CreateTableFile(const Table &table)
{
    string tablename = table.m_metadata.name;
    string filename_data = GetDataFileName(tablename);
    string filename_meta = GetMetaFileName(tablename);
    FILE *fp;
    if ((fp = fopen(filename_data.c_str(), "wb+")) == NULL)
    {
        printf("Can't create %s\n", tablename);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    if ((fp = fopen(filename_meta.c_str(), "wb+")) == NULL)
    {
        printf("Can't create %s\n", tablename);
        exit(EXIT_FAILURE);
    }
    
}

/*
    函数功能：删除表文件
    传入参数：表名称
    返回值：没有返回值
*/
void RecordManager::DropTableFile(const Table &table)
{
}

/*
    函数功能：向表文件中插入元组，支持每次一条元组的插入操作
    传入参数：表名称，元组变量
    返回值：没有返回值
*/
void RecordManager::InsertTuple(const string &tablename, const Tuple &tuple)
{
    string filename = DATA_FILE_PATH + tablename + TABLE_SUFFIX;
    unsigned int file_size = bmanager->GetFileSize(filename);
    vector<unsigned int> block_offset;
    vector<unsigned int> tuple_offset;
    block_offset.push_back(file_size / BLOCKSIZE);
    tuple_offset.push_back(file_size % BLOCKSIZE);
    vector<BID> bids(bmanager->ReadFile2Block(filename, block_offset));
    //由于插入时最多插入一条元组，所以不需要遍历
    char *tuple_add = bmanager->blocks[*bids.begin()].data + *tuple_offset.begin();
    // memcpy(tuple_add, );
}

/*
    函数功能：数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
*/
bool RecordManager::SelectTuple(const string &tablename, const vector<string> &attribute = vector<string>(), const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const
{
}

/*
    函数功能：删除元组，支持每次一条或多条记录的删除操作
*/
bool RecordManager::DelectTuple(const string &tablename, const vector<ConditionUnit> &condition = vector<ConditionUnit>())
{
}
