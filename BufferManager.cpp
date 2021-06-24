#include "BufferManager.h"
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
        if (blocks[bid].GetFilename() == filename && blocks[bid].GetOffset() == offset)
            return bid;
        else if (blocks[bid].IsValid() == false)
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
    blocks[bid].SetFilename(filename);
    blocks[bid].SetOffset(offset);
    blocks[bid].SetUnDirty();
    blocks[bid].SetValid();
}

/*
    函数功能：将文件读入到blocks中
    传入参数：文件名以及block在文件中的偏移量（偏移量可缺省）
    返回值：vector<BID> bids，即文件对应的所有的block的bid
*/
vector<BID> BufferManager::ReadFile2Block(const string &filename, const vector<unsigned int> &boffset)
{
    vector<BID> bids;
    FILE *fp;
    vector<unsigned int> boffsets(boffset);
    if ((fp = fopen(filename.c_str(), "rb+")) == NULL)
    {
        if ((fp = fopen(filename.c_str(), "wb+")) == NULL)
        {
            cout << "[ReadFile2Block Fail]" << endl;
            // cout<<"[ReadFile2Block]: Can't open "<<filename<<endl;
            printf("Can't open %s\n", filename.c_str());
            exit(EXIT_FAILURE); //be panic
        }
    }
    if (boffsets.empty())
    {
        unsigned int max_boffset = GetFileSize(filename) / BLOCKSIZE;
        for (int i = 0; i < max_boffset; i++)
        {
            boffsets.push_back(i);
        }
    }

    vector<unsigned int>::iterator it;
    for (it = boffsets.begin(); it != boffsets.end(); it++)
    {
        fseek(fp, *it * BLOCKSIZE, SEEK_SET);
        BID bid = GetBlock(filename, *it);
        bids.push_back(bid);
        if (blocks[bid].IsValid() == false) //如果该block是空的，需要从文件中读取，并设置其特征值
        {
            fread(blocks[bid].data, BLOCKSIZE, 1, fp);
            SetBlockInfo(bid, filename, *it);
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
    if (blocks[bid].IsDirty()) //没有修改就不用重新写回
    {
        FILE *fp;
        if ((fp = fopen(blocks[bid].GetFilename().c_str(), "wb")) == NULL)
        {
            cout << "[WriteBlock2File Fail]" << endl;
            // cout<<"[WriteBlock2File]: Can't open "<<blocks[bid].GetFilename()<<endl;
            printf("Can't open %s\n", blocks[bid].GetFilename().c_str());
            exit(EXIT_FAILURE);
        }
        fseek(fp, blocks[bid].GetOffset() * BLOCKSIZE, SEEK_SET);
        fwrite(blocks[bid].data, BLOCKSIZE, 1, fp);
        fclose(fp);
    }
    memset(blocks[bid].data, 0, BLOCKSIZE);
    blocks[bid].SetUnValid();
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
        if (blocks[bid].IsValid() && blocks[bid].GetFilename() == filename)
        {
            memset(blocks[bid].data, 0, BLOCKSIZE);
            blocks[bid].SetUnValid();
            cout << "[BufferManager] FlushBlock setUnValid success" << endl;
        }
    }
}
