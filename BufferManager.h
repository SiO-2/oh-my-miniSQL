#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H
// #define DEBUG
#include <iostream>
#include <string>
#include "MiniSQL.h"

using namespace std;

typedef unsigned int BID;
const BID MAX_BLOCK_NUMBER = 4096;
const unsigned int BLOCKSIZE = 4096;

class Block
{
private:
    string filename;
    int offset; //对于文件而言的偏移地址
    bool valid; //由于不需要考虑多线程，直接通过valid表示
    bool dirty;

public:
    //只有data可以给其他manager修改
    char data[BLOCKSIZE];

    Block()
    {
        filename = "";
        offset = -1;
        valid = false;
        dirty = false;
    };

    //大量的读取和设置函数主要是为了保护Block的相关信息不被修改，保证block的分配完全由buffermanager管理
    string GetFilename() const
    {
#ifdef DEBUG
        // cout << "GetFilename return filename = " << this->filename << "output finish" << endl;
#endif
        return this->filename;
    }
    int GetOffset() const
    {
        return this->offset;
    }
    bool IsValid() const
    {
#ifdef DEBUG
        cout << "GetFilename return valid = " << this->valid << "   output finish" << endl;
#endif
        return this->valid;
    }
    bool IsDirty() const
    {
        return this->dirty;
    }
    void SetFilename(const string &filename)
    {
        this->filename = filename;
    }
    void SetOffset(const unsigned int &offset)
    {
        this->offset = offset;
    }

    //设置dirty与valid
    void SetDirty()
    {
        this->dirty = true;
    }
    void SetUnDirty()
    {
        this->dirty = false;
    }
    void SetValid() //表示占用该block
    {
        this->valid = true;
    }
    void SetUnValid() //相当于释放这个block
    {
        this->valid = false;
    }
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

public:
    //record不能调用分配和管理block的函数

    Block blocks[MAX_BLOCK_NUMBER];

    //record可以调用的只有读文件和写文件

    BufferManager(){};

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
    vector<BID> ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());

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
