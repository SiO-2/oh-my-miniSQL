#include "BufferManager.h"

//设置dirty与busy
void BufferManager::SetDirty(const BID &bid)
{
    blocks[bid].dirty = true;
}
void BufferManager::SetUndirty(const BID &bid)
{
    blocks[bid].dirty = false;
}
void BufferManager::SetValid(const BID &bid)
{
    blocks[bid].valid = true;
}
void BufferManager::SetUnValid(const BID &bid)
{
    blocks[bid].valid = false;
}

/*
    函数功能：获取文件的大小
    传入参数：带路径的文件名
    返回值：文件大小，单位为Byte
*/
size_t BufferManager::GetFileSize(const std::string &filename) const
{
    FILE *fp = fopen(filename.c_str(), "r");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fclose(fp);
    return size; //单位是：byte
}

/*
    函数功能：进行BID的分配
    传入参数：带路径的文件名，以及地址偏移
    返回值：分配好的block的bid
*/
BID BufferManager::GetBlock(const string &filename, const unsigned int &offset) const
{
    BID empty = 0;
    for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
    {
        if (blocks[bid].filename == filename && blocks[bid].offset == offset)
            return bid;
        else if (blocks[bid].valid == false)
            empty = bid;
    }
    return empty;
}

/*
    函数功能：将block初始化
    传入参数：block的bid，带路径的文件名，block在文件中的偏移
    返回值：没有返回值
*/
void BufferManager::SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset)
{
    blocks[bid].filename = filename;
    blocks[bid].offset = offset;
    SetValid(bid);
    SetUndirty(bid);
}

/*
    函数功能：将文件读入到blocks中
    传入参数：文件名以及block在文件中的偏移量（偏移量可缺省）
    返回值：vector<BID> bids，即文件对应的所有的block的bid
*/
vector<BID> BufferManager::ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>())
{
    vector<BID> bids;
    FILE *fp;
    vector<unsigned int> offsets(offset);
    if ((fp = fopen(filename.c_str(), "rb")) == NULL)
    {
        if ((fp = fopen(filename.c_str(), "wb+")) == NULL)
        {
            printf("Can't open %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }
    if (offsets.empty())
    {
        unsigned int max_offset = GetFileSize(filename) / BLOCKSIZE;
        for (int i = 0; i <= max_offset; i++)
        {
            offsets.push_back(i);
        }
    }

    vector<unsigned int>::iterator it;
    for (it = offsets.begin(); it != offsets.end(); it++)
    {
        fseek(fp, *it * BLOCKSIZE, SEEK_SET);
        bids.push_back(GetBlock(filename, *it));
        if (blocks[*bids.end()].valid == false)
        {
            fread(blocks[*bids.end()].data, BLOCKSIZE, 1, fp);
            SetBlockInfo(*bids.end(), filename, *it);
        }
    }
    fclose(fp);
    return bids;
}

/*
    函数功能：将块写回文件
    传入参数：传入参数为需要写回的块的bid
    返回值：没有返回值
*/
void BufferManager::WriteBlock2File(const BID &bid)
{
    FILE *fp;
    if (blocks[bid].dirty) //没有修改就不用重新写回
    {
        if ((fp = fopen(blocks[bid].filename.c_str(), "wb")) == NULL)
        {
            printf("Can't open %s\n", blocks[bid].filename);
            exit(EXIT_FAILURE);
        }
        fseek(fp, blocks[bid].offset * BLOCKSIZE, SEEK_SET);
        fwrite(blocks[bid].data, BLOCKSIZE, 1, fp);
        fclose(fp);
    }
    SetUnValid(bid);
}

/*
    函数功能：当表被删除时，将对应的block清空
    传入参数：被删除文件的带路径文件名
    返回值：没有返回值
*/
void BufferManager::FlushBlock(const string &filename)
{
    for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
    {
        if (blocks[bid].valid && blocks[bid].filename == filename)
            SetUnValid(bid);
    }
}
