#include "BufferManager.h"
// #define DEBUG
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

/**
*@brief 根据输入的filename与offset，返回与之匹配的block的bid;
*       如果没有匹配的block就返回一个可用的空block的bid        
*@param filename 带路径的文件名
*@param offset block在文件中的偏移量
*@return 分配好的block的bid
*/
BID BufferManager::GetBlock(const string &filename, const unsigned int &offset) const
{
    BID empty = 0;
    for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
    {
#ifdef DEBUG
        // cout << "bid = " << bid << endl;
        // cout << "blocks[bid].GetFilename() = " << blocks[bid].GetFilename() << endl;
        // cout << "filename = " << filename << endl;
        // cout << "blocks[bid].GetOffset() = " << blocks[bid].GetOffset() << endl;
        // cout << "offset = " << offset << endl;
#endif
        if (blocks[bid].GetFilename() == filename && blocks[bid].GetOffset() == offset)
            return bid;
        else if (blocks[bid].IsValid() == false)
            empty = bid;
    }
    return empty;
}

/**
*@brief 根据输入将block初始化
*@param bid block的bid
*@param filename 带路径的文件名
*@param offset block在文件中的偏移量
*@return 没有返回值
*/
void BufferManager::SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset)
{
    blocks[bid].SetFilename(filename);
    blocks[bid].SetOffset(offset);
    blocks[bid].SetUnDirty();
    blocks[bid].SetValid();
}

/**
*@brief 将文件读入到blocks中
*@param filename 带路径的文件名
*@param boffset 所有需要读取的block在文件中的偏移量【缺省则将整个文件读入】
*@return vector<BID>，即文件对应的所有的block的bid
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
            printf("Can't open %s\n", filename);
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
#ifdef DEBUG
        cout << "BufferManager::ReadFile2Block::91:: get_bid = " << bid << endl;
#endif
        if (blocks[bid].IsValid() == false) //如果该block是空的，需要从文件中读取，并设置其特征值
        {
            fread(blocks[bid].data, BLOCKSIZE, 1, fp);
            SetBlockInfo(bid, filename, *it);
        }
    }
#ifdef DEBUG
    cout << "BufferManager::ReadFile2Block::99" << endl;
#endif
    fclose(fp);
    return bids;
}


/**
*@brief 将指定的block写回file
*@param bid 需要写回文件的block的bid
*@return 没有返回值
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
            printf("Can't open %s\n", blocks[bid].GetFilename());
            exit(EXIT_FAILURE);
        }
        fseek(fp, blocks[bid].GetOffset() * BLOCKSIZE, SEEK_SET);
        fwrite(blocks[bid].data, BLOCKSIZE, 1, fp);
        fclose(fp);
    }
    blocks[bid].SetUnValid();
}

/**
*@brief 当表被删除时，将其表文件对应的block清空
*@param filename 带路径的文件名
*@return 没有返回值
*/
void BufferManager::FlushBlock(const string &filename)
{
    for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
    {
        if (blocks[bid].IsValid() && blocks[bid].GetFilename() == filename)
            blocks[bid].SetUnValid();
    }
}
