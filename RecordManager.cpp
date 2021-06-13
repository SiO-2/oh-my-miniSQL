#include "RecordManager.h"

//新建表，表文件后缀名为".tb"
bool RecordManager::CreateTable(const string &table)
{
    bmanager->CreateFile(table + ".tb");
    return true;
}
//删除表
bool RecordManager::DropTable(const string &table)
{
    bmanager->RemoveFile(table + ".tb");
    return true;
}
//插入元组数据
bool RecordManager::InsertValue(const Table &table, const vector<struct DataUnit> &data)
{
    string table_name = table.m_metadata.name + ".tb";  //加上文件的后缀名
    int tail_bid = bmanager->GetTailBlock(table_name);  //找到存储该文件的最后一块block
    char *data; //存放block中存储的数据
    if (tail_bid != NULL)
    {
        data = bmanager->GetBlockData(table_name, tail_bid);
    }
    else
    {
        tail_bid = 0;
        data = bmanager->GetBlockData(table_name, tail_bid, true);
    }
    //将传入的数据写进block中
}
//无条件查找全部属性输出
bool RecordManager::Select(const struct Table &table)
{
}
//无条件查找部分属性输出
bool RecordManager::Select(const struct Table &table, const vector<string> &attribute)
{
}
//有条件查找全部属性输出
bool RecordManager::Select(const struct Table &table, vector<ConditionUnit> &condition)
{
}
//有条件查找部分属性输出
bool RecordManager::Select(const struct Table &table, const vector<string> &attribute, vector<ConditionUnit> &condition)
{
}
//无条件删除所有元组
bool RecordManager::Delete(const struct Table &Table)
{
}
//删除对应条件的元组
bool RecordManager::Delect(const struct Table &table, const vector<ConditionUnit> &condition)
{
}
//根据索引提供的位置信息进行查找全部属性输出
bool RecordManager::SelectFromIndex(const struct Table &table)
{
}
//根据索引提供的位置信息进行查找部分属性输出
bool RecordManager::SelectFromIndex(const struct Table &table, const vector<string> &attribute)
{
}
