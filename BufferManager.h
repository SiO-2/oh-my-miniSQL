#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <string>
#include "MiniSQL.h"

using namespace std;

typedef unsigned int BID;
const BID MAX_BLOCK_NUMBER = 0x7FFF;
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

    /*
        函数功能：进行BID的分配
        传入参数：带路径的文件名，以及地址偏移
        返回值：分配好的block的bid
    */
    BID GetBlock(const string &filename, const unsigned int &offset) const;

    /*
        函数功能：将block初始化
        传入参数：block的bid，带路径的文件名，block在文件中的偏移
        返回值：没有返回值
    */
    void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset);

    //设置dirty与valid
    void SetDirty(const BID &bid); 
    void SetUndirty(const BID &bid);
    void SetValid(const BID &bid);   //表示占用该block
    void SetUnValid(const BID &bid); //相当于释放这个block

public:
    //record不能调用分配和管理block的函数

    Block blocks[MAX_BLOCK_NUMBER];

    //record可以调用的只有读文件和写文件

    BufferManager();

    ~BufferManager() = default;

    /*
        函数功能：获取文件的大小
        传入参数：带路径的文件名
        返回值：文件大小，单位为Byte
    */
    size_t GetFileSize(const std::string &filename) const;

    /*
        函数功能：将文件读入到blocks中
        传入参数：文件名以及block在文件中的偏移量（偏移量可缺省）
        返回值：vector<BID> bids，即文件对应的所有的block的bid
    */
    vector<BID> BufferManager::ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());

    /*
        函数功能：将块写回文件
        传入参数：传入参数为需要写回的块的bid
        返回值：没有返回值
    */
    void WriteBlock2File(const BID &bid);

    /*
        函数功能：当表被删除时，将对应的block清空
        传入参数：被删除文件的带路径文件名
        返回值：没有返回值
    */
    void FlushBlock(const string &filename);
};

#endif //MINISQL_BUFFERMANAGER_H
