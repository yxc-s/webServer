WebServer

简介:
该项目于2024.02开发，旨在学习网络通信的相关知识，并使用C++编写网络框架，来巩固应用场景中对于所学知识的运用。
该框架的设计思想基于socket网络编程的C++高性能server框架 + bridge（用于服务器内部处理http与socket框架的通信) + http基础框架的结合。
其中server接受并管理网络连接，bridge将有请求的连接交给http模块进行解析并响应。


技术栈:
C++17
线程池
小顶堆计时器
http数据解析与响应
RAII


使用方式：
1、linux环境下，进入build文件夹，终端输入：cmake..
2、进入bin文件夹，终端输入cmake hpts
3、运行服务器: ./hpts
4、浏览器访问ip+端口
5、ip地址和端口在./config/socketbaseconfig.ini中配置
6、静态资源放到./satic/pages中

TODO：
日志系统。
数据库。


