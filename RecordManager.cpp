#include "RecordManager.h"
// #define DEBUG
/*
    函数功能：创建一个表元数据文件以及一个表数据文件
    传入参数：Table类变量
    返回值：没有返回值
*/
void RecordManager::CreateTableFile(const Table &table)
{
    // cout<<"[Record Manager Debug]: tablename = "<<table.m_metadata.name<<endl;
    // string tablename = GetDataFileName(table.m_metadata.name);
    // wyc: 上面这个是不是写错了
    string tablename = table.m_metadata.name;
    string filename_data = GetDataFileName(tablename);
    FILE *fp;
    cout << "[Record Manager Debug]: tablename = " << tablename << endl;
    cout << "[Record Manager Debug]: filename = " << filename_data << endl;
    //新建数据文件，但无需写入
    if ((fp = fopen(filename_data.c_str(), "wb+")) == NULL)
    {
        cout << "Can't create " << tablename << " by cannot open " << filename_data << endl;
        // wyc: string 没法这么输出的
        // printf("Can't create %s by cannot open %s\n", tablename, filename_data);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

/*
    函数功能：清空被删除文件的block
    传入参数：Table类变量
    返回值：没有返回值
*/
void RecordManager::DropTableFile(const Table &table)
{
    string tablename = table.m_metadata.name;
    string filename_data = GetDataFileName(tablename);
    bmanager->FlushBlock(filename_data);
}

/*
    函数功能：向表文件中插入元组，支持每次一条元组的插入操作，并调用index的函数更新index
    传入参数：Table类变量，Tuple类变量
    返回值：没有返回值
*/
void RecordManager::InsertTuple(const Table &table, const Tuple &tuple)
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
#ifdef DEBUG
    printf("RecordManager.cpp>InsertTuple::59:: file_size = %d, BLOCKSIZE = %d, boffset = %d, toffset = %d\n", file_size, BLOCKSIZE, boffset, toffset);
    cout << "RecordManager::InsertTuple:58" << endl;
#endif
    // wyc: 由于只能传catalog给我的table, 而这个没法获取tuple len暂时，我只能先强行注释掉了
    // if (file_size != 0 && (toffset == 0 || (toffset + tuple_len > BLOCKSIZE))) //判断是否写得下
    // {
    //     boffset++;
    //     toffset = 0;
    // }
    vector<BID> bids(bmanager->ReadFile2Block(filename_data, block_offset)); //得到被写入的bid
    for (toffset = 0; toffset < BLOCKSIZE; toffset += tuple_len)             //通过遍历找到可以被写入的tuple地址
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

    bmanager->blocks[*bids.begin()].SetDirty(); //设置为dirty
    //计算获得写入的地址，由于一次只能插入一条元组，所以不需要遍历
    BID bid = *bids.begin();
    char *data_addr = bmanager->blocks[*bids.begin()].data + *tuple_offset.begin();
    //先设置valid
    memcpy(data_addr++, &tuple.valid, 1 * sizeof(char));
#ifdef DEBUG
    printf("RecordManager.cpp>InsertTuple::80:: boffset = %d, toffset = %d\n", boffset, toffset);
    printf("RecordManager::InsertTuple:blocks[%d].offset = %d\n", bid, bmanager->blocks[bid].GetOffset());
    cout << "RecordManager::InsertTuple:80" << endl;
    cout << "RecordManager::InsertTuple:table.m_metadata.attr_num = " << table.m_metadata.attr_num << endl;
#endif
    //通过tuple获取data
    for (int i = 0; i < table.m_metadata.attr_num; i++)
    {
        switch (table.m_attribute[i].type)
        {
        case INT_UNIT:
            memcpy(data_addr, &tuple.tuple_value[i].value.int_value, sizeof(int));
#ifdef DEBUG
            cout << "RecordManager::InsertTuple:i = " << i << endl;
            cout << "RecordManager::InsertTuple:tuple.tuple_value[i].value.int_value = " << tuple.tuple_value[i].value.int_value << endl;
            printf("RecordManager::InsertTuple:data_addr = 0x%x\n", data_addr);
            printf("RecordManager::InsertTuple:*data_addr = %d\n", *data_addr);
            cout << "RecordManager::InsertTuple:91" << endl;
#endif
            data_addr += sizeof(int);
            break;
        case CHAR_UNIT:
            memcpy(data_addr, tuple.tuple_value[i].value.char_n_value, table.m_attribute[i].charlen * sizeof(char));
#ifdef DEBUG
            cout << "RecordManager::InsertTuple:i = " << i << endl;
            printf("RecordManager::InsertTuple:tuple.tuple_value[i].value.char_n_value = %ns\n", tuple.tuple_value[i].value.char_n_value);
            printf("RecordManager::InsertTuple:data_addr = 0x%x\n", data_addr);
            char *char_n_value_test;
            memcpy(char_n_value_test, data_addr, table.m_attribute[i].charlen * sizeof(char));
            printf("RecordManager::InsertTuple:*data_addr = %10.10s\n", char_n_value_test);
            cout << "RecordManager::InsertTuple:98" << endl;
#endif
            data_addr += table.m_attribute[i].charlen;
            break;
        case FLOAT_UNIT:
            memcpy(data_addr, &tuple.tuple_value[i].value.float_value, sizeof(float));
#ifdef DEBUG
            cout << "RecordManager::InsertTuple:i = " << i << endl;
            cout << "RecordManager::InsertTuple:tuple.tuple_value[i].value.float_value = " << tuple.tuple_value[i].value.float_value << endl;
            printf("RecordManager::InsertTuple:data_addr = 0x%x\n", data_addr);
            printf("RecordManager::InsertTuple:*data_addr = %f\n", *(float *)data_addr);
            cout << "RecordManager::InsertTuple:105" << endl;
#endif
            data_addr += sizeof(float);
            break;
        default:
            break;
        }
    }
    bmanager->WriteBlock2File(bid);
#ifdef DEBUG
    printf("RecordManager::InsertTuple:130  block_data_addr = %d\n", bmanager->blocks[*bids.begin()].data);
#endif

    //注意，需要将offset传给index建立索引
    // vector<char> tuple_data;
}

/*
    函数功能：从一个block中抽取单条Tuple
    传入参数：Table类变量，bid，tuple_offset
    返回值：一个Tuple类变量
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
            // memcpy(&tuple.tuple_value[i].value.int_value, data_addr, sizeof(int));
            data_addr += sizeof(int);
            break;
        case CHAR_UNIT:
            // #ifdef DEBUG
            //             cout << "RecordManager::ExtractTuple:table.m_attribute[i].charlen = " << table.m_attribute[i].charlen << endl;
            // #endif
            temp_value.char_n_value = (char *)malloc(table.m_attribute[i].charlen * sizeof(char));
            memcpy(temp_value.char_n_value, data_addr, table.m_attribute[i].charlen * sizeof(char));
            data_addr += table.m_attribute[i].charlen;
            // tuple.tuple_value[i].value.char_n_value = (char *)malloc(table.m_attribute[i].charlen * sizeof(char));
            // memcpy(tuple.tuple_value[i].value.char_n_value, data_addr, table.m_attribute[i].charlen * sizeof(char));
            // data_addr += table.m_attribute[i].charlen;
            break;
        case FLOAT_UNIT:
            memcpy(&(temp_value.float_value), data_addr, sizeof(float));
            // memcpy(&tuple.tuple_value[i].value.float_value, data_addr, sizeof(float));
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

/*
    函数功能：判断tuple是否满足条件
    传入参数：Tuple类变量，条件
    返回值：bool型变量
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

/*
    函数功能：数据查询，可以通过指定用and 连接的多个条件进行查询，支持等值查询和区间查询
    传入参数：Table类变量，查找条件
    返回值：vector<Tuple>，即符合条件的元组
*/
vector<Tuple> RecordManager::SelectTuple(const Table &table, const vector<ConditionUnit> &condition) const
{
    unsigned int offset = 0;
    unsigned int tuple_len = GetTuplelen(table);
#ifdef DEBUG
    cout << "RecordManager::SelectTuple::230 tuple_len = " << tuple_len << endl;
#endif
    vector<Tuple> result;
    string filename_data = GetDataFileName(table.m_metadata.name);
#ifdef DEBUG
    // cout << "SelectTuple filename_data = " << filename_data << endl;
#endif
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
                if (ConditionTest(tuple) && tuple.valid == true)
                    result.push_back(tuple);
            }
        }
    }
    return result;
}

/*
    函数功能：删除元组，支持每次一条或多条记录的删除操作
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
            if (ConditionTest(tuple))
            {
                memcpy((bmanager->blocks[*it]).data + tuple_offset, &unvalid, sizeof(unvalid) * sizeof(char)); //设置valid为0
                bmanager->blocks[*bids.begin()].SetDirty();                                                    //将该tuple所在的block设置为dirty
            }
        }
    }
    //注意，需要将offset传给index建立索引
}

/*
    函数功能：得到Table类中一条Tuple的存储字节数
    传入参数：Table类变量
    返回值：返回tuple的长度
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