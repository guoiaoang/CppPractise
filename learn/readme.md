# 项目简述：  
使用VS2015编译  
&nbsp;  

## stdFunction:
C++知识点练习，用于学习、复习与巩固(基本完成)。  
包括构造析构、类型转换、智能指针、STL容器、函数绑定、异常捕获、进程线程、同步锁、网络(select)等的示例。  
poll\epoll的示例参见"Linux\poll.cpp"。  
&nbsp;  

## calc:
大数乘法练习。  
```
muiti:10123456789 * 19876543210 = 201219326301126352690
muiti:99909101233423456789 * 99993198765433422106 = 9990230618109521189181023257816288377634
```
&nbsp;  

### example:
微软示例代码。  
&nbsp;  

## helloConsole:
简单显示程序运行环境(显示命令行参数与环境变量，用于其他项目中检验启动参数)。  
&nbsp;  

## splitPic:
BMP图片分割的简单实现(未完成)。  
&nbsp;  


## Win32AppConsolePrint:
Win32程序添加控制台输出示例。  
```c++
AllocConsole();
SetConsoleTitle("调试输出控制台");
freopen("CONOUT$","w+t",stdout);
freopen("CONIN$","r+t",stdin);
//FreeConsole();
```
&nbsp;  




