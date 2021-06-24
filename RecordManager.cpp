#include "RecordManager.h"
/**
*@brief 向表文件中插入元组，支持每次一条元组的插入操作，并调用index的函数更新index
*@param table 待插入的表
*@param tuple 待插入的元组
*@return 没有返回值
*/
void RecordManager::InsertTuple(const Table &table, const Tuple &tuple, const vector<Index> &indexs)
{
    string tablename = table.m_metadata.name;
    // wyc: 写错了吧，我帮你改了
    // string tablename = GetDataFileName(table.m_metadata.name);
    string filename_data = GetDataFileName(tablename);
    vector<unsigned int> block_offset; //其实只需要一个unsigned int类型的变量，但函数调用需要传入vector
    vector<unsigned int> tuple_offset;

    //计算block_offset和tuple_offset来获取bid
    unsigned int file_size = bmanager->GetFileSize(filename_data); //每次写入时都写到文件的最后一个block中
    unsigned int boffset = file_size / BLOCKSIZE;                  //根据文件大小计算block
    if (boffset != 0)
        boffset--;
    unsigned int toffset = 0;
    unsigned int tuple_len = GetTuplelen(table);
    block_offset.push_back(boffset);
    vector<BID> bids(bmanager->ReadFile2Block(filename_data, block_offset)); //得到被写入的bid
    for (toffset = 0; toffset < BLOCKSIZE; toffset += tuple_len)             //通过遍历找到valid == 0,即可以被写入的tuple地址
    {
        if (bmanager->blocks[*bids.begin()].data[toffset] == 0)
            break;
    }
    if (toffset >= BLOCKSIZE) //当前block内写不下时，新建一个block
    {
        boffset++;
        toffset = 0;
        bids = bmanager->ReadFile2Block(filename_data, block_offset);
    }
    tuple_offset.push_back(toffset);

    for (int i = 0; i < indexs.size(); i++) //更新index
    {
        imanager->insertIndex(indexs[i], tuple.tuple_value[indexs[i].attr_num], boffset * BLOCKSIZE + toffset);
    }

    bmanager->blocks[*bids.begin()].SetDirty(); //设置为dirty
    //计算获得写入的地址，由于一次只能插入一条元组，所以不需要遍历
    BID bid = *bids.begin();
    char *data_addr = bmanager->blocks[*bids.begin()].data + *tuple_offset.begin();
    //先设置valid
    memcpy(data_addr++, &tuple.valid, 1 * sizeof(char));
    //通过tuple获取data
    for (int i = 0; i < table.m_metadata.attr_num; i++)
    {
        switch (table.m_attribute[i].type)
        {
        case INT_UNIT:
            memcpy(data_addr, &tuple.tuple_value[i].value.int_value, sizeof(int));
            data_addr += sizeof(int);
            break;
        case CHAR_UNIT:
            memcpy(data_addr, tuple.tuple_value[i].value.char_n_value, table.m_attribute[i].charlen * sizeof(char));
            data_addr += table.m_attribute[i].charlen;
            break;
        case FLOAT_UNIT:
            memcpy(data_addr, &tuple.tuple_value[i].value.float_value, sizeof(float));
            data_addr += sizeof(float);
            break;
        default:
            break;
        }
    }
    bmanager->WriteBlock2File(bid);
}

/**
*@brief 从一个block中抽取单条Tuple
*@param table Table类变量
*@param bid block对应的bid
*@param tuple_offset 代取元组在block->data中的偏移量
*@return 一个Tuple类变量
*/
Tuple RecordManager::ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const
{
    Tuple tuple;
    char *data_addr = (bmanager->blocks[bid]).data + tuple_offset; //获取数据的地址

    //抽取数据类型
    memcpy(&tuple.valid, data_addr++, 1 * sizeof(char)); //先设置valid
    for (int i = 0; i < table.m_metadata.attr_num; i++)
    {
        Value temp_value; //新建一个tuple
        switch (table.m_attribute[i].type)
        {
        case INT_UNIT:
            memcpy(&(temp_value.int_value), data_addr, sizeof(int));
            data_addr += sizeof(int);
            break;
        case CHAR_UNIT:
            temp_value.char_n_value = (char *)malloc(table.m_attribute[i].charlen * sizeof(char));
            memcpy(temp_value.char_n_value, data_addr, table.m_attribute[i].charlen * sizeof(char));
            data_addr += table.m_attribute[i].charlen;
            break;
        case FLOAT_UNIT:
            memcpy(&(temp_value.float_value), data_addr, sizeof(float));
            data_addr += sizeof(float);
            break;
        default:
            break;
        }
        // wyc: 这里是不是应该是对temp_value做点什么，不然temp_value不是空的吗，另，tuple里是unit,不是直接的Value, 为了让他不要报错，我加了下面这一行
        Unit unit(temp_value, (DataType)table.m_attribute[i].type);
        tuple.tuple_value.push_back(unit); //将tuple push到结果中
    }
    return tuple;
}

/**
*@brief 判断tuple是否满足条件
*@param tuple Tuple类变量
*@param condition 判断的条件【缺省时为无条件，返回true】
*@return bool型变量
*/
bool RecordManager::ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition) const
{
    if (condition.size() == 0)
        return true;
    for (int i = 0; i < condition.size(); i++)
    {
        Value data_value, condition_value;
        switch (condition[i].op_code)
        {
        case EQ_: //=
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) != 0)
                return false;
            break;
        case NE_: //!=
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) == 0)
                return false;
            break;
        case L_: //<
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) >= 0)
                return false;
            break;
        case G_: //>
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) <= 0)
                return false;
            break;
        case LE_: //<=
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) > 0)
                return false;
            break;
        case GE_: //>=
            if (memcmp(&tuple.tuple_value[condition[i].attr_num], &condition[i].value, sizeof(condition[i].value)) < 0)
                return false;
            break;
        }
    }
    return true;
}

/**
*@brief 数据查询，可以通过指定用 and 连接的多个条件进行查询，支持等值查询和区间查询
*@param table Table类变量
*@param condition 查找条件
*@return vector<Tuple>，即符合条件的元组
*/
vector<Tuple> RecordManager::SelectTuple(const Table &table, const vector<ConditionUnit> &condition) const
{
    unsigned int offset = 0;
    unsigned int tuple_len = GetTuplelen(table);
    vector<Tuple> result;
    string filename_data = GetDataFileName(table.m_metadata.name);
    vector<BID> bids;
    bids = bmanager->ReadFile2Block(filename_data);
    vector<BID>::iterator it;
    for (it = bids.begin(); it != bids.end(); it++)
    {
        for (unsigned int tuple_offset = 0; tuple_offset < BLOCKSIZE; tuple_offset += tuple_len)
        {
            if (bmanager->blocks[*it].data[tuple_offset] == 1) //先判断该处的tuple数据是否有效
            {
                Tuple tuple = ExtractTuple(table, *it, tuple_offset);
                if (ConditionTest(tuple, condition) && tuple.valid == true)
                    result.push_back(tuple);
            }
        }
    }
    return result;
}

/**
*@brief 删除元组，支持每次一条或多条记录的删除操作
*@param table Table类变量
*@param condition 查找条件
*@return 没有返回值
*/
void RecordManager::DeleteTuple(const Table &table, const vector<ConditionUnit> &condition)
{
    const bool unvalid = false;
    unsigned int offset = 0;
    unsigned int tuple_len = GetTuplelen(table);
    vector<Tuple> result;
    string filename_data = GetDataFileName(table.m_metadata.name);
    vector<BID> bids;
    bids = bmanager->ReadFile2Block(filename_data);
    vector<BID>::iterator it;
    for (it = bids.begin(); it != bids.end(); it++)
    {
        for (unsigned int tuple_offset = 0; tuple_offset < BLOCKSIZE; tuple_offset += tuple_len)
        {
            Tuple tuple = ExtractTuple(table, *it, tuple_offset);
            if (ConditionTest(tuple, condition) && tuple.valid == true)
            {
                memcpy((bmanager->blocks[*it]).data + tuple_offset, &unvalid, sizeof(unvalid) * sizeof(char)); //设置valid为0
                bmanager->blocks[*bids.begin()].SetDirty();                                                    //将该tuple所在的block设置为dirty
            }
        }
    }
}

/**
*@brief 得到Table类中一条Tuple的存储字节数
*@param table Table类变量
*@return 返回tuple的长度
*/
unsigned int RecordManager::GetTuplelen(const Table &table) const
{
    unsigned int tuple_len;
    tuple_len = 1; //valid位的存储需要一个字节
    for (int i = 0; i < table.m_metadata.attr_num; i++)
    {
        switch (table.m_attribute[i].type)
        {
        case INT_UNIT:
            tuple_len += sizeof(int);
            break;
        case CHAR_UNIT:
            tuple_len += table.m_attribute[i].charlen;
            break;
        case FLOAT_UNIT:
            tuple_len += sizeof(float);
            break;
        }
    }
    return tuple_len;
}

/**
*@brief 从数据文件中读取tuple并插入index文件建立索引
*@param index 待插入的index
*@return 没有返回值
*/
void RecordManager::CreateIndex(const Index &index)
{
    unsigned int offset = 0;
    Table table(*(index.table));
    Unit unit;
    unsigned int tuple_len = GetTuplelen(table);
    string filename_data = GetDataFileName(table.m_metadata.name);
    vector<BID> bids(bmanager->ReadFile2Block(filename_data));
    vector<BID>::iterator it;
    for (it = bids.begin(); it != bids.end(); it++)
    {
        BID bid = *it;
        for (unsigned int tuple_offset = 0; tuple_offset < BLOCKSIZE; tuple_offset += tuple_len)
        {
            if (bmanager->blocks[bid].data[tuple_offset] == 1) //先判断该处的tuple数据是否有效
            {
                Tuple tuple = ExtractTuple(table, bid, tuple_offset);
                unit = tuple.tuple_value[index.attr_num];
                if (ConditionTest(tuple) && tuple.valid == true)
                    imanager->insertIndex(index, unit, offset);
            }
        }
    }
}
