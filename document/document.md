# MiniSQL设计总报告

组员：汪奕晨、罗志凡、章茹、张效源、陈信宇

课程：数据库系统原理



## 第一章 MiniSQL总体框架

### 1 MiniSQL功能分析

本项目主要设计一个MiniSQL数据库，允许用户通过命令行界面对数据库进行交互，支持基本的插入、查找、删除操作。

MiniSQL系统实现了以下具体的功能：

+ 支持三种基本的数据类型的存储：integer、char(n)、float
+ 支持表的定义，一个表可以定义32个属性，属性可以定义为unique，支持单主键的定义。
+ 支持B+树索引，对于表的主键，自动生成B+树索引，也可以通过create index生成自定义的索引。
+ 支持条件查询，各个条件之间用and连接，支持等值查询和区间查询。
+ 支持插入、条件删除操作。可以一次删除多条符合条件的元组。



### 2 MiniSQL功能结构

<img src=".\picture\structure.png" alt="structure" style="zoom: 33%;" />

Interpreter模块提供与用户交互的界面，该模块调用Catalog Manager的接口对数据进行预处理，调用API对数据库进行查找、插入和删除操作。Record Manager、Index Manager和Catalog Manager通过Buffer Manager对底层文件进行交互。



## 第二章 内部数据结构

### 1 B+树结构



### 2 元组数据结构

元组依次存放在内存的块中，文件中每个块的大小为4KB，文件和内存之间以块为单位进行传输。在一个块中，记录按照顺序存储，一条记录按照属性的顺序存储，在每一条记录的末尾有一个valid标记，记录该元组是否被删除。

每一条元组的具体的结构如下：

![tuple](.\picture\tuple.png)



## 第三章 模块功能与接口

### 1 Interpreter模块



### 2 API模块

#### 2.1 模块主要功能

API模块为Interpreter开放插入、删除、查找等操作的接口，简化Interpreter模块的设计。Interpreter模块调用Catalog Manager的接口对数据进行预处理；调用Index Manager的接口来建立、删除索引，或使用索引对对应属性上的值进行查找；调用Record Manager的接口对表进行建立、删除以及对对应元组进行插入、删除、查找等操作。



#### 2.2 模块接口定义



### 3 Catalog Manager模块



### 4 Record Manager模块

#### 4.1 模块主要功能

Record Manager管理数据库中实际的数据，主要功能为表的定义与删除、记录的插入、删除与查找操作。当表被建立的时候，本模块建立一个新的空间以存储数据，表被删除的时候，本模块删除存放在内存中的数据。在插入的过程中，本模块读入文件中最后一个块，将记录插入到该块的末尾处，并向上提供接口。在记录删除的查找过程中，本模块根据Index Manager返回的元组在文件中的位置信息找到满足条件的元组，将该元组的valid位设置为0，表示该元组被删除。在查找的过程中，本模块根据Index Manager返回的元组在文件中的位置查找满足条件的元组，将该元组信息解析出来封装成Tuple结构返回Interpreter用于输出。



#### 4.2 模块接口定义





### 5 Index Manager模块



### 6 Buffer Manager模块



### 7 DB Files模块





## 第四章 MiniSQL系统测试

