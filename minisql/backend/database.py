import pymysql
import pandas as pd

ip = 'cdb-gn0dzycj.gz.tencentcdb.com'
port = 10181
database = 'boardgames'
user = 'root'
password = '4881768549aA'

class clouddb():
    def __init__(self):
        self.__ip = 'cdb-gn0dzycj.gz.tencentcdb.com'
        self.__port = 10181
        self.__database = 'dbhw'
        self.__user = 'root'
        self.__password = '4881768549aA'
        self.connection = pymysql.connect(host=self.__ip, port=self.__port, database=self.__database, user=self.__user, password=self.__password)
        self.cursor = self.connection.cursor()  # 创建游标

    def get_table(self, sql):
        try:
            self.cursor.execute(sql)
            rows = self.cursor.fetchall()  # 获取所读取的内容，存储形式为元组
            df = pd.DataFrame(rows, index=range(1, len(rows)+1))  # 将元组形式的数据转化为DataFrame的形式，方便数据分析
            return df  # 函数的返回值为我们得到的DataFrame
        except Exception as e:
            print(repr(e))
            raise db_error(repr(e))

    def upd_table(self, sqls):
        try:
            if(type(sqls) == list):
                for sql in sqls:
                    self.cursor.execute(sql)  # 执行传入的sql语言，不同的sql语言执行结果不同，需要在调用函数时指定
            else:
                self.cursor.execute(sqls)  # 执行传入的sql语言，不同的sql语言执行结果不同，需要在调用函数时指定

            self.connection.commit()
            return True
        except Exception as e:
            self.connection.rollback()
            print(repr(e))
            raise db_error(repr(e))

    def __del__(self):
        self.cursor.close()  # 关闭游标
        self.connection.close()  # 关闭数据库连接


class db_error(Exception):
    pass