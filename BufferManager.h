#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <string>
#include "MiniSQL.h"

using namespace std;

typedef unsigned int BID;
const BID MAX_BLOCK_NUMBER = 0x4000;
const unsigned int BLOCKSIZE = 4096;

struct Block
{
    string filename;
    unsigned int offset; //对于文件而言的偏移地址
    bool valid;          //由于不需要考虑多线程，直接通过valid表示
    bool dirty;
    char data[BLOCKSIZE];
};

class BufferManager
{
private:
    //record不能调用分配和管理block的函数

    Block blocks[MAX_BLOCK_NUMBER];

    //返回对应的bid
    BID GetBlock(const string &filename, const unsigned int &offset) const;     
    
    //写block的相关信息
    void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset);

    //设置dirty,busy与valid
    void SetDirty(const BID &bid);
    void SetUndirty(const BID &bid);
    void SetValid(const BID &bid);   //表示占用该block
    void SetUnValid(const BID &bid); //相当于释放这个block

public:
    //record可以调用的只有读文件和写文件

    BufferManager();

    ~BufferManager() = default;


    //将文件读入block
    BID ReadFile2Block(const string &filename, const unsigned int &offset);

    //将块写回文件
    void WriteBlock2File(const BID &bid);


};

#endif //MINISQL_BUFFERMANAGER_H
