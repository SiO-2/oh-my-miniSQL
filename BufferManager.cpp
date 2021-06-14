#include "BufferManager.h"

//将文件读入block
void BufferManager::ReadFile2Block(const string &filename, const signed int &offset){

}

//将块写回文件
void BufferManager::WriteBlock2File(const string &filename, const signed int &offset){

}

//设置dirty与busy
void BufferManager::SetDirty(const unsigned int &bid){

}
void BufferManager::SetUndirty(const unsigned int &bid){

}
void BufferManager::SetBusy(const unsigned int &bid){

}
void BufferManager::SetUnbusy(const unsigned int &bid){

}

//释放buffer manager中指定的block
void BufferManager::ReleaseBlock(){

}

//删除文件在Block中的数据
void BufferManager::BlockFlush(const string &filename){

}