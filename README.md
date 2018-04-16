Welcome to the log4z wiki!  
## Introduction:  
[![Build Status](https://travis-ci.org/zsummer/log4z.svg?branch=master)](https://travis-ci.org/zsummer/log4z)
```
Log4z is an open source C++ lightweight & cross platform log library.   
It provides in a C++ application log and trace debug function for 7*24h service program.  
Support 64/32 of windows/linux/mac/android/iOS.   
Log4z是一款开源的轻量级高性能的跨平台日志库 主要针对7*24小时服务器程序的日志输出与跟踪调试,   
支持64/32位的windows/linux/mac/android/iOS等操作系统.  
```
## Feature:  
```
1. MIT open source license,very liberal.  
   MIT开源授权 授权的限制非常小.  
2. Cross Platform Linux & Windows & mac, Lightweight  only one cpp sourse file one header file.  
   跨平台支持linux & windows & mac, 轻量级 只有一个头文件和一个cpp文件.  
3. 不需要关心创建和销毁等问题 全部自动化.  
4. Multi-Logger Output, Rolling File, Priority Filter, Thread Safe.  
   支持多日志分流,滚动日志记录, 优先级过滤, 多线程安全.
5. Screen Display Log with Different Color.  
   屏显日志彩色输出.  
6. Support format-style and stream-style write log.  
   支持格式化形式和流形式输入日志.    
7. Support configure from file and hot update, or can direct fast work without any config.  
   可通过配置文件配置并进行配置热更新,也可以不做任何配置直接快速使用.   
8. High-performance, 800k lines/sec complex log input/output, 2M lines/sec single string input/output.  
   非常高的吞吐和序列化性能, 针对单线程和多线程下的指令级调优. 80万/sec的复杂类型的日志写入速度,和200万/sec的单一字符串写入速度.    

```
### File Name Format:   
**LogName_YearMonthDayHourMinute_ProcessID_BlockNumber.log**  
**日志名_年月日时分_进程ID_文件滚动分块编号.log**  
  
```   
E:\GITHUB\LOG4Z\PROJECT\BIN  
│──advanced_test.exe  
│──config.cfg  
│──fflua_test.exe  
│──fast_test.exe  
│──stress_test.exe  
│  
├─log  
│──│──advanced_test_2013100921_003852_000.log  
│──│──advanced_test_2013101313_005920_000.log    
│──│──fast_test_2013100921_003855_000.log  
│──│  
│──└─2013_10  
│─────│────Dynamic_2013100921_003852_000.log  
│  
└─Stress  
───│────NetWork_2013101313_007196_000.log  
───│────NetWork_2013101313_007196_001.log  
───│────NetWork_2013101313_007196_002.log  
```  
### 日志内容[Log Content]  
![advanced_test](https://github.com/YaoXuanZhi/log4z/advanced_test.png)
![fflua_test](https://github.com/YaoXuanZhi/log4z/fflua_test.gif)

```
[2018-01-01 22:56:19][ALARM]:-----------------  log4z thread started!   ----------------------------
[2018-01-01 22:56:21]<13032>[ERROR]:main LOGE advanced_test.cpp:150
[2018-01-01 22:56:21]<13032>[ALARM]:main LOGA advanced_test.cpp:151
[2018-01-01 22:56:21]<13032>[FATAL]:main LOGF advanced_test.cpp:152
[2018-01-01 22:56:21]<13032>[FATAL]: = = = = = = = = = = = = = = =  = = = = =  advanced_test.cpp:153
[2018-01-01 22:56:21]<13032>[DEBUG]:main LOGD advanced_test.cpp:147
[2018-01-01 22:56:21]<13032>[INFO]:main LOGI advanced_test.cpp:148
[2018-01-01 22:56:21]<13032>[WARN]:main LOGW advanced_test.cpp:149
[2018-01-01 22:56:21]<13032>[ERROR]:main LOGE advanced_test.cpp:150
[2018-01-01 22:56:21]<13032>[ALARM]:main LOGA advanced_test.cpp:151
[2018-01-01 22:56:21]<13032>[FATAL]:main LOGF advanced_test.cpp:152
[2018-01-01 22:56:21]<13032>[FATAL]: = = = = = = = = = = = = = = =  = = = = =  advanced_test.cpp:153
[2018-01-01 22:56:29]<13032>[ALARM]:main quit .. hit 'enter' to exit. advanced_test.cpp:162
```
## Example  
```  
  #include "log4z.h"  
  using namespace zsummer::log4z;  
  int main(int argc, char *argv[])  
  {  
      //start log4z  
      ILog4zManager::getRef().start();  
      
      //begin test stream log input....  
      LOGT("stream input *** " << "LOGT LOGT LOGT LOGT" << " *** ");  
      LOGD("stream input *** " << "LOGD LOGD LOGD LOGD" << " *** ");  
      LOGI("stream input *** " << "LOGI LOGI LOGI LOGI" << " *** ");  
      LOGW("stream input *** " << "LOGW LOGW LOGW LOGW" << " *** ");  
      LOGE("stream input *** " << "LOGE LOGE LOGE LOGE" << " *** ");  
      LOGA("stream input *** " << "LOGA LOGA LOGA LOGA" << " *** ");  
      LOGF("stream input *** " << "LOGF LOGF LOGF LOGF" << " *** ");  
  
      // cannot support VC6 or VS2003  
      //begin test format log input....  
      LOGFMTT("format input *** %s *** %d ***", "LOGFMTT", 123456);  
      LOGFMTD("format input *** %s *** %d ***", "LOGFMTD", 123456);  
      LOGFMTI("format input *** %s *** %d ***", "LOGFMTI", 123456);  
      LOGFMTW("format input *** %s *** %d ***", "LOGFMTW", 123456);  
      LOGFMTE("format input *** %s *** %d ***", "LOGFMTE", 123456);  
      LOGFMTA("format input *** %s *** %d ***", "LOGFMTA", 123456);  
      LOGFMTF("format input *** %s *** %d ***", "LOGFMTF", 123456);  
  
      LOGA("main quit ...");  
      return 0;  
  }  
```  

# How to compile  
### Directly Using The Way 直接使用:    
Add files log4z.cpp log4z.h and compile together in the existing projects  
把文件 log4z.cpp log4z.h添加到已有项目中并一起编译.  
### Make Library To Use   
**In Windows:**  
```
Open and compile log4z/vc2005/log4z.vcproj
```  
**In Linux :**   
```
cd log4z/g++   
cmake .   
make   
```  
#About The Author  
Author: YaweiZhang  
Mail: yawei.zhang@foxmail.com  
GitHub: https://github.com/zsummer/log4z  

Author: Yaoxuanzhi 
Mail: yaoxuanzhi@outlook.com  
GitHub: https://github.com/YaoXuanZhi/log4z
