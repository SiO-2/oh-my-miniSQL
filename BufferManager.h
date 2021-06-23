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
    ~Block() {}

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
        // cout << "BufferManager.h::IsValid():49" << endl;
        // cout << "GetFilename return valid = " << this->valid << "   output finish" << endl;
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
    BID GetBlock(const string &filename, const unsigned int &offset) const;

    void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset);

public:
    //record不能调用分配和管理block的函数

    Block blocks[MAX_BLOCK_NUMBER];

    //record可以调用的只有读文件和写文件

    BufferManager(){};

    //析构时将所有有过修改的block写回文件
    ~BufferManager()
    {
        for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
            if (blocks[bid].IsDirty())
                WriteBlock2File(bid);

        // cout << "BufferManager.h>BufferManager>~BufferManager() finished" << endl;
    };

    size_t GetFileSize(const std::string &filename) const;

    vector<BID> ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());

    void WriteBlock2File(const BID &bid);

    void FlushBlock(const string &filename);
};

#endif //MINISQL_BUFFERMANAGER_H
