安全传输平台( Security Transfer Platform), 简称(STP)

版本1：linux版本

版本2：支持跨平台

使用的开源库：
libevent-2.0.22-stable  
protobuf-2.5.0
glog-0.3.4
mysql-5.6.25
json
des

需要的模块
报文收发Server, Client, 包括readn、writen 等辅助函数

报文序列化和反序列化 Codec

数据库连接驱动MySQLConnector

配置文件Json

加解密组件DES



