#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <cstdio>
#include <utility>
#include "MiniSQL.h"

using namespace std;

struct Block
{
    string filename;
    unsigned int bid;
    bool dirty;
    bool busy;
    char data[BlockSize];
    int count;
};

class BufferManager
{
public:
    BufferManager();

    ~BufferManager() = default;

    //将所有的块写回文件中
    void WriteAllBlock2File();

    //将块写回文件
    void WriteBlock2File();

    //获取表的可插入位置，找到文件中最后一个block，如果有空间则返回块，如果没有则返回NULL
    int GetTailBlock(string filename);

    //根据文件和bid设置dirty
    void SetDirty(const string &filename, unsigned int bid);

    //根据filenam与bid从文件中获取返回指向block中数据的指针，如果是新的block则返回空指针
    char *GetBlockData(string filename, unsigned int bid, bool new = false);

    //删除给定表的给定块的给定位移处的一条记录
    void DeleteValues(int blockoffset, int offset, Table tableinfo);

    //新建文件，包括表文件、元文件等
    void CreateFile(string filename);

    //删除文件
    void RemoveFile(string filename);

    //释放block的占用
    void SetFree(string filename, unsigned int bid);
};

#endif //MINISQL_BUFFERMANAGER_H
