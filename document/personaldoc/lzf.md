# 0 My Work

本人在 MiniSQL 项目设计中负责的工作：

1. `Buffer`和`Record`相关的数据结构的设计
2. `Buffer`与`Record`模块的编写
3. 与`Index`模块协调整合对上层提供接口
4. 项目整体的调试



# 1 `BufferManager` 的实现

## 1.1 功能描述

`BufferManager` 维护⼀个大小为 $16MB(4096*4096B)$ 的 `buffer `，缓存表⽂件中的相关记录，实现按块读取。`BufferManager` 位于整个⼯程的最底层，直接与⽂件系统进⾏交互。对 `IndexManager`, `RecordManager` 提供访问⽂件、访问相关记录、操作 `Block` 的相关接口。

`BufferManager` 通过 `blocks` 数组在内存中维护⼀个 `BLOCKSIZE * MAX_BLOCK_NUMBER` 的空间⽤于进⾏缓存。`BufferManager` 所处理的主要请求与相关思路包括：

1. 给定文件名和所需块在文件中的偏移量，将数据从文件读到 `buffer` 中。如果没有指定偏移量则将整个文件读入。并在读入后设置 `block` 的对应属性。
2. 给定 `bid` 将 `blocks[bid]` 中的内容写回文件中。这一功能在该 `block` 被置换出或者退出程序时被调用。
3. 给定文件名，将对应的 `block` 给清空，在表被删除时调用。注意必须将 `block` 中所有的数据清空，因为我们在写回文件时是以整个 `block` 为单位的，如果之前的数据没有清空，再使用了这块 `block` 则很容易出现将之前文件中的数据写回当前文件的情况。

## 1.2 主要数据结构与接口

### 1.2.1 头文件引入与全局变量定义

```c++
#include <iostream>
#include <string>
#include <string.h>
#include "MiniSQL.h"

using namespace std;

typedef unsigned int BID;
const BID MAX_BLOCK_NUMBER = 4096;
const unsigned int BLOCKSIZE = 4096;
```

1. 引入系统自带的头文件 `iostream`, `string`, `string.h` 用于处理输入输出，以及支持字符串类型
2. 引入 `MiniSQL.h`  添加自定义的数据结构，如 `Table`, `Tuple` 等
3. `MAX_BLOCK_NUMBER` 作为静态全局变量，表示每个 `BufferManager` 有 `4096` 个 `block`
4. `BLOCKSIZE` 作为静态全局变量，表示每个 `Block` 的大小为 `4096` 字节

### 1.2.2 `Block` 类

> `Block` 类为 `BufferManager` 类提供具体操作的 `block` 变量

#### 1.2.2.1 成员变量

```c++
private:
    string filename;
    int offset;
    bool valid;
    bool dirty;

public:
    char data[BLOCKSIZE];
```

1. 私有成员变量

   - `string filename` 用于记录 `block` 中存储的是哪个文件中的数据

   - `int offset` 用于记录该 `block` 存储的数据再文件中的偏移量，便于将 `block` 中的数据写回文件
   - `bool valid` 用于标识该 `block` 目前是否被占用，便于分配
   - `bool dirty` 用于标识该 `block` 当前是否有修改，便于关闭程序时判断是否需要将数据写回文件

2. 公共成员变量

   - `char data[BLOCKSIZE]` 用于存放 `block` 中的具体数据

#### 1.2.2.2 成员函数

主要是用于对私有变量的访问与修改

```c++
string Block::GetFilename() const
{
    return this->filename;
}
int Block::GetOffset() const
{
    return this->offset;
}
bool Block::IsValid() const
{
    return this->valid;
}
bool Block::IsDirty() const
{
    return this->dirty;
}
void Block::SetFilename(const string &filename)
{
    this->filename = filename;
}
void Block::SetOffset(const unsigned int &offset)
{
    this->offset = offset;
}

void Block::SetDirty()
{
    this->dirty = true;
}
void Block::SetUnDirty()
{
    this->dirty = false;
}
void Block::SetValid()
{
    this->valid = true;
}
void Block::SetUnValid()
{
    this->valid = false;
}
```



### 1.2.3 `BufferManager` 类

#### 1.2.3.1 成员变量

```c++
public:
    Block blocks[MAX_BLOCK_NUMBER];
```

`Block blocks[MAX_BLOCK_NUMBER]` 是 `BufferManager` 中的 `block` 数组，设为公共成员变量是为了方便获取其存储的具体数据 `block.data`

#### 1.2.3.2 成员函数

```c++
private:
    BID GetBlock(const string &filename, const unsigned int &offset) const;
    void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset);
public:
    size_t GetFileSize(const std::string &filename) const;
    vector<BID> ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());
    void WriteBlock2File(const BID &bid);
    void FlushBlock(const string &filename);
```

1. 私有成员函数

   - `BID GetBlock(const string &filename, const unsigned int &offset) const;` 
     - 函数功能：进行 `BID` 的分配，这一过程只能有 `BufferManager` 进行，因此设为私有   
     - 传入参数：带路径的文件名，以及地址偏移
     - 返回值：分配好的 `block` 的 `bid`
   - `void SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset);` 
     - 函数功能：将 `block` 初始化，这也属于 `Block` 分配的一部分，故设为私有
     - 传入参数：`block` 的 `bid`，带路径的文件名，`block` 在文件中的偏移
     - 返回值：没有返回值

2. 公共成员函数

   - `size_t GetFileSize(const std::string &filename) const;` 
     - 函数功能：获取文件的大小，用于将文件读入 `Block` 时计算所需要的 `Block` 数量
     - 传入参数：带路径的文件名
     - 返回值：文件大小，单位为 `Byte`
   - `vector<BID> ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());`
     - 函数功能：将文件读入到 `buffer` 中
     - 传入参数：文件名以及 `block` 在文件中的偏移量（偏移量可缺省，此时将整个文件读入 `buffer` 中）
     - 返回值：`vector<BID> bids`，即文件对应的所有的 `block` 的 `bid`
   - `void WriteBlock2File(const BID &bid);`
     - 函数功能：将块写回文件
     - 传入参数：传入参数为需要写回的块的 `bid`
     - 返回值：没有返回值
   - `void FlushBlock(const string &filename);`
     - 函数功能：当表被删除时，将对应的 `block` 清空
     - 传入参数：被删除文件的带路径文件名
     - 返回值：没有返回值

   > 具体的函数实现可以参考附录

   

# 2 `RecordManager` 的实现

## 2.1 功能描述

`RecordManager` 最核⼼的功能在于直接执⾏对 `reocrds/tuples` 的操作。记录层⾯的操作在这模块内被实现。具体功能包括：

1. `insert`：给定数据表与待插入的记录后，将记录插⼊⾄ `buffer` 中。
2. `select`：给定数据表与查询条件后，从 `buffer` 中获取符合条件的记录并返回给上层。其中可以通过 `index` 优化查询的过程。
3. `delete`：与 `select` 类似，对给定表名和条件进⾏删除操作。

## 2.2 文件存储说明

在 `RecordManager` 中，需要操作的文件是存储具体数据的文件。存储的最小单元为一个 `block`，按记录逐条存储。其中第一个 `byte` 存储的 `valid` 标志着该处的记录是否有效，而之后的记录按照 `table` 类中存储的 `tuple_len` 进行存储与分隔。

## 2.3 主要数据结构与接口

### 2.3.1头文件引入

```c++
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include "MiniSQL.h"
#include "BufferManager.h"
#include "IndexManager.h"
```

1. 引入系统自带的头文件 `<iostream>`, `<string>`, `<string.h>`, `<fstream>` 用于处理各项输入以及对 `string` 类型的支持
2. 引入`<vector>`, `<iterator>`, `math.h` 支持向量、迭代器与数学运算，简化编程
3. 引入自己定义的头文件 `MiniSQL.h`, `BufferManager.h`, `IndexManager.h`，添加自定义的数据结构与函数



### 2.3.2 成员变量

```C++
public:
	IndexManager *imanager;
	BufferManager *bmanager;
```

1. `IndexManager *imanager`：用于调用 `index` 相关的操作
2. `BufferManager *bmanager`：用于管理分配 `Buffer` 中的 `Block`

### 2.3.3 成员函数

```c++
private:
	Tuple ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const;
public:
	RecordManager()
    {
        bmanager = new BufferManager();
        imanager = new IndexManager();
    };
    ~RecordManager()
    {
        delete bmanager;
        delete imanager;
    };
    string GetDataFileName(const string tablename) const
    {
        return TABLE_PATH + tablename + TABLE_SUFFIX;
    }
    string GetIndexFileName(const string tablename) const
    {
        return INDEX_PATH + tablename + INDEX_SUFFIX;
    }
    string GetMetaFileName(const string tablename) const
    {
        return META_PATH + tablename + META_SUFFIX;
    }
    void InsertTuple(const Table &table, const Tuple &tuple);
    bool ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
    vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
    void DeleteTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>());
    unsigned int GetTuplelen(const Table &table) const;
    void CreateIndex(const Index &index);
```

1. 构造函数与析构函数

   - `RecordManager()` 构造函数
   - `~RecordManager()` 析构函数

2. 获取对应文件名的函数

   - `string GetDataFileName(const string tablename) const`

   - `string GetIndexFileName(const string tablename) const`

   - `string GetMetaFileName(const string tablename) const`

3. 具体逻辑实现的函数

   - `void InsertTuple(const Table &table, const Tuple &tuple)`
     - 函数功能：向表文件中插入元组，支持每次一条元组的插入操作，并调用 `index` 的函数更新 `index`
     - 传入参数：待插入的表 `table`, 待插入的元组 `tuple`
     - 返回值：没有返回值
   - `Tuple ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const`
     - 函数功能：从一个 `block` 中抽取单条 `Tuple`
     - 传入参数：`Table` 类变量，`block` 对应的  `bid`，待取元组在 `block->data` 中的偏移量
     - 返回值：一个 `Tuple` 类变量
   - `bool ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const`
     - 函数功能：判断 `tuple` 是否满足条件
     - 传入参数：待判断的元组 `tuple`，判断的条件（缺省时为无条件，返回 `true`）
     - 返回值：`bool` 型变量
   - `vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const`
     - 函数功能：数据查询，可以通过指定用 `and` 连接的多个条件进行查询，支持等值查询和区间查询
     - 传入参数：`Table` 类变量 `table`，查找条件 `condition`
     - 返回值：`vector<Tuple>`，即符合条件的元组
   - `void DeleteTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>())`
     - 函数功：能删除元组，支持每次一条或多条记录的删除操作
     - 传入参数：`Table` 类变量 `table`，查找条件 `condition`
     - 返回值：没有返回值
   - `unsigned int GetTuplelen(const Table &table) const`
     - 函数功：能得到 `Table` 类中一条 `Tuple` 的存储字节数
     - 传入参数：`Table` 类变量 `table`
     - 返回值：返回 `tuple` 的长度
   - `void CreateIndex(const Index &index)`
     - 函数功能：从数据文件中读取 `tuple` 并插入 `index` 文件建立索引
     - 传入参数：待插入的 `index`
     - 返回值：没有返回值

   > 具体的函数实现可以参考附录

   

# 3 附录：代码实现

## 3.1 `BufferManager`

1. `BufferManager.h`

   ```c++
   #ifndef MINISQL_BUFFERMANAGER_H
   #define MINISQL_BUFFERMANAGER_H
   #include <iostream>
   #include <string>
   #include <string.h>
   #include "MiniSQL.h"
   
   using namespace std;
   
   typedef unsigned int BID;
   const BID MAX_BLOCK_NUMBER = 4096;
   const unsigned int BLOCKSIZE = 4096;
   
   class Block
   {
   private:
       string filename;
       int offset; 
       bool valid; 
       bool dirty;
   
   public:
       char data[BLOCKSIZE];
   
       Block()
       {
           filename = "";
           offset = -1;
           valid = false;
           dirty = false;
       };
       ~Block() {}
   
       string GetFilename() const
       {
           return this->filename;
       }
       int GetOffset() const
       {
           return this->offset;
       }
       bool IsValid() const
       {
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
   
       void SetDirty()
       {
           this->dirty = true;
       }
       void SetUnDirty()
       {
           this->dirty = false;
       }
       void SetValid() 
       {
           this->valid = true;
       }
       void SetUnValid() 
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
       Block blocks[MAX_BLOCK_NUMBER];
       BufferManager(){};
       ~BufferManager()
       {
           for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
               if (blocks[bid].IsDirty() && blocks[bid].IsValid())
                   WriteBlock2File(bid);
       };
       size_t GetFileSize(const std::string &filename) const;
       vector<BID> ReadFile2Block(const string &filename, const vector<unsigned int> &offset = vector<unsigned int>());
       void WriteBlock2File(const BID &bid);
       void FlushBlock(const string &filename);
   };
   
   #endif //MINISQL_BUFFERMANAGER_H
   
   ```

   

2. `BufferManager.cpp`

   ```c++
   #include "BufferManager.h"
   
   size_t BufferManager::GetFileSize(const std::string &filename) const
   {
       FILE *fp = fopen(filename.c_str(), "r");
       fseek(fp, 0, SEEK_END);
       size_t size = ftell(fp);
       fclose(fp);
       return size;
   }
   
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
   
   void BufferManager::SetBlockInfo(const BID &bid, const string &filename, const unsigned int &offset)
   {
       blocks[bid].SetFilename(filename);
       blocks[bid].SetOffset(offset);
       blocks[bid].SetUnDirty();
       blocks[bid].SetValid();
   }
   
   vector<BID> BufferManager::ReadFile2Block(const string &filename, const vector<unsigned int> &boffset)
   {
       vector<BID> bids;
       FILE *fp;
       vector<unsigned int> boffsets(boffset);
       if ((fp = fopen(filename.c_str(), "rb+")) == NULL)
       {
           if ((fp = fopen(filename.c_str(), "wb+")) == NULL)
           {
               cout << "[ReadFile2Block Fail]" << "\n";
               printf("Can't open %s\n", filename.c_str());
               exit(EXIT_FAILURE);
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
           if (blocks[bid].IsValid() == false) 
           {
               fread(blocks[bid].data, BLOCKSIZE, 1, fp);
               SetBlockInfo(bid, filename, *it);
           }
       }
       fclose(fp);
       return bids;
   }
   void BufferManager::WriteBlock2File(const BID &bid)
   {
       if (blocks[bid].IsDirty())
       {
           FILE *fp;
           if ((fp = fopen(blocks[bid].GetFilename().c_str(), "wb")) == NULL)
           {
               cout << "[WriteBlock2File Fail]" << "\n";
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
   void BufferManager::FlushBlock(const string &filename)
   {
       for (BID bid = 0; bid < MAX_BLOCK_NUMBER; bid++)
       {
           if (blocks[bid].IsValid() && blocks[bid].GetFilename() == filename)
           {
               memset(blocks[bid].data, 0, BLOCKSIZE);
               blocks[bid].SetUnValid();
               cout << "[BufferManager] FlushBlock setUnValid success" << "\n";
           }
       }
   }
   
   ```

   

## 3.2 `RecordManager`

1. `RecordManager.h`

   ```c++
   #ifndef MINISQL_RECORDMANAGER_H
   #define MINISQL_RECORDMANAGER_H
   
   #include <iostream>
   #include <string>
   #include <string.h>
   #include <vector>
   #include <fstream>
   #include <iterator>
   #include <math.h>
   #include "MiniSQL.h"
   #include "BufferManager.h"
   #include "IndexManager.h"
   using namespace std;
   
   class RecordManager
   {
   private:
   	Tuple ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const;
   
   public:
   	IndexManager *imanager;
   	BufferManager *bmanager;
   
   public:
   	RecordManager()
   	{
   		bmanager = new BufferManager();
   		imanager = new IndexManager();
   	};
   	~RecordManager()
   	{
   		delete bmanager;
   		delete imanager;
   	};
   
   	string GetDataFileName(const string tablename) const
   	{
   		return TABLE_PATH + tablename + TABLE_SUFFIX;
   	}
   
   	string GetIndexFileName(const string tablename) const
   	{
   		return INDEX_PATH + tablename + INDEX_SUFFIX;
   	}
   
   	string GetMetaFileName(const string tablename) const
   	{
   		return META_PATH + tablename + META_SUFFIX;
   	}
       
   	void InsertTuple(const Table &table, const Tuple &tuple);
   
   	bool ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
   
   	vector<Tuple> SelectTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>()) const;
   
   	void DeleteTuple(const Table &table, const vector<ConditionUnit> &condition = vector<ConditionUnit>());
   
   	unsigned int GetTuplelen(const Table &table) const;
   
   	void CreateIndex(const Index &index);
   };
   
   #endif
   ```

   

2. `RecordManager.cpp`

   ```c++
   #include "RecordManager.h"
   
   void RecordManager::InsertTuple(const Table &table, const Tuple &tuple)
   {
       string tablename = table.m_metadata.name;
       string filename_data = GetDataFileName(tablename);
       vector<unsigned int> block_offset;
       vector<unsigned int> tuple_offset;
   
       unsigned int file_size = bmanager->GetFileSize(filename_data); 
       unsigned int boffset = file_size / BLOCKSIZE;                  
       if (boffset != 0)
           boffset--;
       unsigned int toffset = 0;
       unsigned int tuple_len = GetTuplelen(table);
       block_offset.push_back(boffset);
       vector<BID> bids(bmanager->ReadFile2Block(filename_data, block_offset));
       for (toffset = 0; toffset < BLOCKSIZE; toffset += tuple_len)
       {
           if (bmanager->blocks[*bids.begin()].data[toffset] == 0)
               break;
       }
       if (toffset >= BLOCKSIZE) 
       {
           
           block_offset.pop_back();
           boffset++;
           toffset = 0;
           block_offset.push_back(boffset);
           bids = bmanager->ReadFile2Block(filename_data, block_offset);
       }
       tuple_offset.push_back(toffset);
   
       for (int i = 0; i < table.Index_name.size(); i++)
       {
           imanager->insertIndex(*table.Index_name[i], tuple.tuple_value[table.Index_name[i]->attr_num], boffset * BLOCKSIZE + toffset);
       }
   
       bmanager->blocks[*bids.begin()].SetDirty(); 
       BID bid = *bids.begin();
       char *data_addr = bmanager->blocks[*bids.begin()].data + *tuple_offset.begin();
       memcpy(data_addr++, &tuple.valid, 1 * sizeof(char));
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
   
   Tuple RecordManager::ExtractTuple(const Table &table, const BID bid, const unsigned int tuple_offset) const
   {
       Tuple tuple;
       char *data_addr = (bmanager->blocks[bid]).data + tuple_offset; 
       memcpy(&tuple.valid, data_addr++, 1 * sizeof(char)); 
       for (int i = 0; i < table.m_metadata.attr_num; i++)
       {
           Value temp_value; 
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
           Unit unit(temp_value, (DataType)table.m_attribute[i].type);
           tuple.tuple_value.push_back(unit);
       }
       return tuple;
   }
   
   bool RecordManager::ConditionTest(const Tuple &tuple, const vector<ConditionUnit> &condition) const
   {
       if (condition.size() == 0)
           return true;
       for (int i = 0; i < condition.size(); i++)
       {
           Value data_value, condition_value;
           if (condition[i].data_type == INT_UNIT)
           {
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
           else if (condition[i].data_type == FLOAT_UNIT)
           {
               switch (condition[i].op_code)
               {
               case EQ_: //=
                   if (fabs(tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value) >= EPSILON)
                       return false;
                   break;
               case NE_: //!=
                   if (fabs(tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value) < EPSILON)
                       return false;
                   break;
               case L_: //<
                   if (tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value >= EPSILON)
                       return false;
                   break;
               case G_: //>
                   if (tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value <= EPSILON)
                       return false;
                   break;
               case LE_: //<=
                   if (tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value > EPSILON)
                       return false;
                   break;
               case GE_: //>=
                   if (tuple.tuple_value[condition[i].attr_num].value.float_value - condition[i].value.float_value < EPSILON)
                       return false;
                   break;
               }
           }
           else if (condition[i].data_type == CHAR_UNIT)
           {
               switch (condition[i].op_code)
               {
               case EQ_: //=
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) != 0)
                       return false;
                   break;
               case NE_: //!=
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) == 0)
                       return false;
                   break;
               case L_: //<
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) >= 0)
                       return false;
                   break;
               case G_: //>
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) <= 0)
                       return false;
                   break;
               case LE_: //<=
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) > 0)
                       return false;
                   break;
               case GE_: //>=
                   if (strcmp(tuple.tuple_value[condition[i].attr_num].value.char_n_value, condition[i].value.char_n_value) < 0)
                       return false;
                   break;
               }
           }
       }
       return true;
   }
   
   vector<Tuple> RecordManager::SelectTuple(const Table &table, const vector<ConditionUnit> &condition) const
   {
       unsigned int offset = 0;
       unsigned int tuple_len = GetTuplelen(table);
       vector<Tuple> result;
       string filename_data = GetDataFileName(table.m_metadata.name);
       vector<BID> bids;
       bool flag = false;
       unsigned int attr_num;
       unsigned int index_num;
       unsigned int condition_num;
       if (condition.size() == 1 && condition.begin()->op_code == EQ_) 
       {
           for (int i = 0; i < table.Index_name.size() && flag == false; i++)
           {
               if (condition.begin()->attr_num == table.Index_name[i]->attr_num)
               {
                   index_num = i;
                   flag = true;
               }
           }
       }
       if (flag)
       {
           cout << "[Select with index]"
                << "\n";
           try
           {
               unsigned int offset = imanager->searchIndex(*table.Index_name[index_num], *condition.begin());
               unsigned int block_offset = offset / BLOCKSIZE;
               unsigned int tuple_offset = offset % BLOCKSIZE;
               vector<unsigned int> block_offsets;
               block_offsets.push_back(block_offset);
               bids = bmanager->ReadFile2Block(filename_data, block_offsets);
               Tuple tuple = ExtractTuple(table, *bids.begin(), tuple_offset);
               result.push_back(tuple);
           }
           catch (DBError e)
           {
           }
       }
       else
       {
           cout<<"[Select without index]"<<"\n";
           bids = bmanager->ReadFile2Block(filename_data);
           vector<BID>::iterator it;
           for (it = bids.begin(); it != bids.end(); it++)
           {
               for (unsigned int tuple_offset = 0; tuple_offset < BLOCKSIZE; tuple_offset += tuple_len)
               {
                   if (bmanager->blocks[*it].data[tuple_offset] == 1) 
                   {
                       Tuple tuple = ExtractTuple(table, *it, tuple_offset);
                       if (ConditionTest(tuple, condition) && tuple.valid == true)
                           result.push_back(tuple);
                   }
               }
           }
       }
       return result;
   }
   
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
                   memcpy((bmanager->blocks[*it]).data + tuple_offset, &unvalid, sizeof(unvalid) * sizeof(char));
                   bmanager->blocks[*bids.begin()].SetDirty();       
                   for (int i = 0; i < table.Index_name.size(); i++)
                   {
                       imanager->deleteIndex(*table.Index_name[i], tuple.tuple_value[table.Index_name[i]->attr_num]);
                   }
               }
           }
       }
   }
   
   unsigned int RecordManager::GetTuplelen(const Table &table) const
   {
       unsigned int tuple_len;
       tuple_len = 1; 
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
   
   void RecordManager::CreateIndex(const Index &index)
   {
       this->imanager->readIndexfromfile(index);
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
               if (bmanager->blocks[bid].data[tuple_offset] == 1) 
               {
                   Tuple tuple = ExtractTuple(table, bid, tuple_offset);
                   unit = tuple.tuple_value[index.attr_num];
                   offset = bmanager->blocks[bid].GetOffset() * BLOCKSIZE + tuple_offset;
                   if (ConditionTest(tuple) && tuple.valid == true)
                       imanager->insertIndex(index, unit, offset);
               }
           }
       }
   }
   
   ```

   