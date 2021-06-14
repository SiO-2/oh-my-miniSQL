#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <string>
#include "MiniSQL.h"

using namespace std;

typedef unsigned int BID;
const unsigned int MAX_BLOCK_NUMBER = 0x1000;
const unsigned int BLOCKSIZE = 0x4000;

struct Block
{
    string filename;
    BID bid;   //在buffer manager中的序号
    bool dirty;
    bool busy;
    bool valid;         //由于不需要考虑多线程，直接通过valid表示
    char data[BLOCKSIZE];
    unsigned int offset;    //对于文件而言的偏移地址
};

class BufferManager
{
private:
    Block blocks[MAX_BLOCK_NUMBER];
    BID GetBlock(const string &filename, int offset);

public:
    BufferManager();

    ~BufferManager() = default;

    //将文件读入block
    void ReadFile2Block(const string &filename, const unsigned int &offset);

    //将块写回文件
    void WriteBlock2File(const string &filename, const unsigned int &offset);

    //设置dirty与busy
    void SetDirty(const unsigned int &bid);
    void SetUndirty(const unsigned int &bid);
    void SetBusy(const unsigned int &bid);
    void SetUnbusy(const unsigned int &bid);

    //释放buffer manager中指定的block
    void ReleaseBlock();

    //删除文件在Block中的数据
    void BlockFlush(const string &filename);
};

#endif //MINISQL_BUFFERMANAGER_H
