#include "BufferManager.h"

//假设实验展示的操作不会将buffer写满，所以先不考虑LRU（狗头
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

void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset)
{
    blocks[bid].filename = filename;
    blocks[bid].offset = offset;
    SetValid(bid);
    SetUndirty(bid);
}

//将文件读入block
void BufferManager::ReadFile2Block(const string &filename, const unsigned int &offset)
{
    FILE *fp;
    BID bid;
    if ((fp = fopen(filename.c_str(), "rb")) == NULL)
    {
        if ((fp = fopen(filename.c_str(), "wb+")) == NULL)
        {
            printf("Can't open %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }
    fseek(fp, offset * BLOCKSIZE, SEEK_SET);
    bid = GetBlock(filename, offset);
    if (blocks[bid].valid == false) //当valid为true时不需要重新写入
        fread(blocks[bid].data, BLOCKSIZE, 1, fp);
    fclose(fp);
    SetBlockInfo(bid, filename, offset);
}

//将块写回文件
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