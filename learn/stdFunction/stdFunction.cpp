// 多线程、线程池
// 异步I/O、I/O复用、IOCP、任务队列  // 重叠I/O？
// seletc、poll、epoll，事件模型.消息模型.以及IOCP模型
// 同步锁、线程锁、读写锁、 // 信号量、...
// 
// 
// 虚函数、纯虚函数、虚析构函数、纯虚析构函数，多态调用&防止某些资源没有释放
// 智能指针、STD函数、
// select和epoll等多路I/O复用
// 大数据、高并发、多线程、分布式
// 线程池 死锁
// 进程间通信、线程间通信、资源共享
// Redis分布式缓存服务器、Boost、STL
// 
//
// 算法、设计模式
// 

#include <iostream>    // std::cout\std::endl\...
// <WinSock2.h>隐含<Windows.h>  
// <Windows.h>隐含<WinSock.h>,后包含<WinSock2.h>会导致多次重定义
#include <WinSock2.h>  // WSAPoll 
//#include <Windows.h>   // GetStdHandle
namespace COLORCONSOLE {};  // 无实现的名字空间声明
using namespace COLORCONSOLE;
using namespace std;
#pragma warning(disable:4996)  // 'strcpy': This function or variable may be unsafe.

// 更改控制台彩色并输出文字
namespace COLORCONSOLE {
	// print "\033[31;4mRed Underline Text\033[0m"
	// echo -e "\033[31mRed Text\033[0m"
#include <Windows.h>
// 颜色补充定义
#define FOREGROUND_YELLOW (FOREGROUND_RED   | FOREGROUND_GREEN)  // 黄色
#define FOREGROUND_PURPLE (FOREGROUND_RED   | FOREGROUND_BLUE )  // 紫色
#define FOREGROUND_CYAN   (FOREGROUND_GREEN | FOREGROUND_BLUE )  // 青色
#define FOREGROUND_WHITE  (FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_RED)  // 白色
// text color 文本颜色
// [颜色约定]
// namespace 名字空间 (黄色)
//     └─ main() function start 主函数开始 (红色)
//         └─ paragraph 段落 (绿色)
//             └─ name:value 键值对 (青色|蓝色)
//                   └─ something 一般文字(白色)  // note 笔记(紫色)
#define TCOLOR_NS		FOREGROUND_YELLOW	// namespace 名字空间	：黄色
#define TCOLOR_FUNC		FOREGROUND_RED		// main		 主函数开始 ：红色
#define TCOLOR_PARA		FOREGROUND_GREEN	// paragraph 段落		：绿色
#define TCOLOR_KEY		FOREGROUND_CYAN		// key		(键值对)值	：青色
#define TCOLOR_VALUE	FOREGROUND_BLUE		// value	(键值对)键	：蓝色
#define TCOLOR_NORMAL	FOREGROUND_WHITE	// normal	常规文字	：白色
#define TCOLOR_NOTE		FOREGROUND_PURPLE	// note		注释		：紫色
// 输出行定义
#define COUTLINE_FUNC_MAIN	cout << endl << "Func " << ColorText(__FUNCSIG__, TCOLOR_FUNC) << " start" << endl;
#define COUTLINE_FUNC_PARA	ColorLine(__FUNCSIG__);			// 章节开始时的输出
#define COUTLINE_FUNC_INFO	cout << __FUNCSIG__ << endl;	// 执行过程中的输出

	class ColorText {
	public:
		ColorText(char*str = "", WORD cur = TCOLOR_VALUE, WORD def = TCOLOR_NORMAL)
			: m_str(str), m_cur(cur), m_def(def) {}
		friend ostream &operator << (ostream &output, const ColorText &colorPrint) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hOut, colorPrint.m_cur);
			output << colorPrint.m_str;
			SetConsoleTextAttribute(hOut, colorPrint.m_def);
			return output;
		}
	private:
		char *m_str;
		WORD m_cur;  // m_wAttributesThis
		WORD m_def;  // m_wAttributesDefault
	};
	void ColorLine(char*str, WORD cur = TCOLOR_PARA) { cout << ColorText(str, cur) << endl; }
	//
	int main() {
		COUTLINE_FUNC_MAIN;
		// 颜色输出及示例  // 仅下面一行使用原始颜色输出
		cout << ColorText("RED", FOREGROUND_RED) << ColorText("GREEN", FOREGROUND_GREEN) << ColorText("BLUE", FOREGROUND_BLUE)
			<< ColorText("YELLOW", FOREGROUND_YELLOW) << ColorText("PURPLE", FOREGROUND_PURPLE) << ColorText("CYAN", FOREGROUND_CYAN)
			<< ColorText("WHITE", FOREGROUND_WHITE) << endl;
		// 输出颜色格式信息
		cout << ColorText("[", TCOLOR_NORMAL) << ColorText("颜色约定", TCOLOR_KEY) << ColorText("]", TCOLOR_NORMAL) << endl;
		cout << ColorText("namespace 名字空间", TCOLOR_NS) << endl;
		cout << "    └─ "
			<< ColorText("main() function", TCOLOR_FUNC) << ColorText(" start ", TCOLOR_NORMAL)
			<< ColorText("主函数", TCOLOR_FUNC) << ColorText("开始", TCOLOR_NORMAL)
			<< endl;
		cout << "        └─ " << ColorText("paragraph 段落", TCOLOR_PARA) << endl;
		cout << "            └─ "
			<< ColorText("key", TCOLOR_KEY) << ColorText(":", TCOLOR_NORMAL) << ColorText("value", TCOLOR_VALUE)
			<< ColorText(" ", TCOLOR_NORMAL)
			<< ColorText("键", TCOLOR_KEY) << ColorText("值", TCOLOR_VALUE) << ColorText("对", TCOLOR_NORMAL)
			<< endl;
		cout << "                  └─ "
			<< ColorText("something 一般文字", TCOLOR_NORMAL) << ColorText("  // note 笔记", TCOLOR_NOTE)
			<< endl;
		// 下面颜色均用角色代替
		cout << "example:" << endl;
		cout << "__FILE__   :[" << ColorText(__FILE__, TCOLOR_FUNC) << "]" << endl;
		cout << "__FUNCSIG__:[" << ColorText(__FUNCSIG__) << "]" << endl;
		return 0;
	}
}

// 输出文件信息、启动参数及环境变量
namespace ns_info {
#define COUTLINE_FUNCINFO_DEFINE(ARG1)	cout << ""#ARG1":[" << ARG1 << "]" << endl;
#define COUTLINE_FUNCINFO_ONE(ARG1)		cout << ""#ARG1":[" << ARG1 << "]" << endl;
#define COUTLINE_FUNCINFO_VOID(FUNC)	cout << ""#FUNC":" << FUNC << endl; 
#define COUTLINE_FUNCINFO_TWO(FUNC,ARG1,ARG2)   FUNC(ARG1,ARG2); \
	cout << ""#FUNC"("#ARG1", "#ARG2") -> " << ""#FUNC"(" << ARG1 << ", " << ARG2 << ")" << endl;
#define COUTLINE_FUNCINFO_THREE(FUNC,ARG1,ARG2,ARG3)   FUNC(ARG1,ARG2,ARG3); \
	cout << ""#FUNC"("#ARG1", "#ARG2", "#ARG3") -> " \
	<< ""#FUNC"(" << ARG1 << ", " << ARG2 << ", " << ARG3 << ")" << endl;

	void func_define() {
		COUTLINE_FUNC_PARA;
		COUTLINE_FUNCINFO_DEFINE(__FUNCDNAME__);
		COUTLINE_FUNCINFO_DEFINE(__FUNCSIG__);
		COUTLINE_FUNCINFO_DEFINE(__FUNCTION__);
		COUTLINE_FUNCINFO_DEFINE(__FILE__);
		COUTLINE_FUNCINFO_DEFINE(__LINE__);
		COUTLINE_FUNCINFO_DEFINE(__DATE__);
		COUTLINE_FUNCINFO_DEFINE(__TIME__);
		COUTLINE_FUNCINFO_DEFINE(__TIMESTAMP__);
		COUTLINE_FUNCINFO_DEFINE(__COUNTER__);
	}
	void func_argEnv(int _Argc, char ** _Argv, char ** _Env) {
		COUTLINE_FUNC_PARA;
		// 启动参数及环境变量
		cout << ColorText("_Argc", TCOLOR_KEY) << ":[" << _Argc << "]" << endl;
		for (int i = 0; i < _Argc; ++i)
			cout << "_Argv[" << i << "]:[" << _Argv[i] << "]" << endl;
		cout << ColorText("Environment:", TCOLOR_KEY) << endl;
		for (int i = 0; _Env[i] != NULL; ++i)
			cout << "Environment[" << i << "]:[" << _Env[i] << "]" << endl;
	}
	void func_get() {
		COUTLINE_FUNC_PARA;
		// param is void
		COUTLINE_FUNCINFO_VOID(GetCurrentProcessId());
		COUTLINE_FUNCINFO_VOID(GetCurrentProcess());
		COUTLINE_FUNCINFO_VOID(GetCurrentThread());
		COUTLINE_FUNCINFO_VOID(GetCurrentThreadId());
		//COUTLINE_FUNCINFO_VOID(GetCurrentUmsThread());
		COUTLINE_FUNCINFO_VOID(GetSystemDefaultLangID());
		COUTLINE_FUNCINFO_VOID(GetSystemDefaultLCID());
		COUTLINE_FUNCINFO_VOID(GetSystemDefaultUILanguage());
		// param is more
		WCHAR szDir[MAX_PATH] = {};
		COUTLINE_FUNCINFO_TWO(GetSystemDirectory, szDir, MAX_PATH);
		COUTLINE_FUNCINFO_TWO(GetWindowsDirectory, szDir, MAX_PATH);
		COUTLINE_FUNCINFO_TWO(GetSystemWindowsDirectory, szDir, MAX_PATH);
		COUTLINE_FUNCINFO_TWO(GetCurrentDirectory, MAX_PATH, szDir);
		COUTLINE_FUNCINFO_THREE(GetModuleFileName, NULL, szDir, MAX_PATH);
	}
	//
	int main(int _Argc, char ** _Argv, char ** _Env) {
		COUTLINE_FUNC_MAIN;
		func_define();
		func_argEnv(_Argc, _Argv, _Env);
		func_get();
		return 0;
	}

#undef COUTLINE_FUNCINFO_DEFINE
#undef COUTLINE_FUNCINFO_ONE
#undef COUTLINE_FUNCINFO_VOID
#undef COUTLINE_FUNCINFO_TWO
#undef COUTLINE_FUNCINFO_THREE
}

// 条件循环及判断
namespace ns_ifloop {
	void func_if() {
		COUTLINE_FUNC_PARA;
		// 是否执行
		if (true) {
			;
		}
		int a = 1;
		// 条件成立执行...;条件不成立执行...
		if (a == 1) {
			;
		}
		else {
			;
		}
		// 条件为...时执行...;否则条件为...时执行...
		if (a == 2) {
			;
		}
		else if (a == 1)
		{
		}
		// 条件为...时执行...;否则条件为...时执行...;条件都不满足执行...
		if (a == 2) {
			;
		}
		else if (a == 1)
		{
		}
		else {
			;
		}
		// 多个条件
		switch (a) {
		case 2:
		case 1:
			break;
		case 0:
			break;
		default:
			break;
		}
		// 三目表达式
		int b = a == 1 ? a + 2 : 3;
		// 判断条件
		if (b == 1) { ; }
		if (b != 1) { ; }
		if (b > 1) { ; }
		if (b < 1) { ; }
		if (b >= 1) { ; }
		if (b <= 1) { ; }
		if (b) { ; }
		if (!b) { ; }
	}
	void func_for() {
		COUTLINE_FUNC_PARA;
		// 变量自增
		for (int i = 0; i < 5; i++) {
			;
		}
		// 变量自减
		for (int i = 5; i >= 0; --i) {
			;
		}
		// 空语句无限循环  // 也可以填充for后括号中的任意位置
		for (;;) {
			break;  // 跳出循环
		}
		// 嵌套循环 + 条件退出
		for (int i = 0; i < 6; i++) {
			for (int j = 7; j >= 3; j--) {
				if (j <= i) {
					break;
				}
			}
		}
		// 只有"for (; 0;)"不进入循环，其他均为无限循环
		unsigned int iCount = 0;
		for (;;) { cout << "break;" << endl; iCount++; if (iCount > 2) { break; } }
		iCount = 0;
		for (0;;) { cout << "for (0;;);" << endl; iCount++; if (iCount > 2) { break; } }
		iCount = 0;
		for (; 0;) { cout << "for (; 0;);" << endl; iCount++; if (iCount > 2) { break; } }
		iCount = 0;
		for (; ; 0) { cout << "for (; ; 0);" << endl; iCount++; if (iCount > 2) { break; } }
	}
	void func_while() {
		COUTLINE_FUNC_PARA;
		// while(...) do{...} 语句
		while (true) {
			break;
		}
		// do{...} while(...)语句
		do {
			break;
		} while (true);
	}
	void func_forwhile() {
		COUTLINE_FUNC_PARA;
		int collection_to_loop[] = { 123,234,345,456 };
		for each (auto var in collection_to_loop) {
			cout << var << endl;
		}
	}
	//
	void main() {
		COUTLINE_FUNC_MAIN;
		func_if();
		func_for();
		func_while();
		func_forwhile();
	}
}

// 构造、析构、虚函数
namespace ns_constructFunction {
	// 构造函数：
	// 默认构造
	// 赋值构造
	// 拷贝构造   // 复制参数类
	// 移动构造   // 当前类赋值后将参数类置空
	// 析构函数
	// 虚函数     // 
	// 纯虚函数
	// 虚析构函数 // 
	// 纯虚析构函数
	// func() = delete;  //
	// func() = 0;

	class BaseA {
	public:
		BaseA() { COUTLINE_FUNC_INFO }        // 默认构造函数  // 默认构造与其多态函数不允许加"virtual"
		BaseA(int a) { COUTLINE_FUNC_INFO }   // 默认构造函数的多态
		BaseA(char*a) { COUTLINE_FUNC_INFO }  // 默认构造函数的多态
		~BaseA() { COUTLINE_FUNC_INFO }       // 析构函数
		void func() = delete;  // "= delete;" 禁止继承、拷贝该类 // C++11的特性
	};
	class BaseB {
	public:
		virtual ~BaseB() { COUTLINE_FUNC_INFO }           // "virtual ~ClassName(){...}" :虚析构函数，防止子类资源未释放
		virtual void func() = 0; // "virtual ret function() = 0":纯虚函数,不能直接使用该类，且子类必须重写该函数
	};
	class BaseC {
	public:
		// "virtual ~ClassName() = 0;"：纯虚析构函数
		// 子类无法重写，需要在类外添加执行体，否则会报找不到函数执行体
		virtual ~BaseC() = 0;
		void func() { COUTLINE_FUNC_INFO }     // 普通函数
	};
	inline BaseC::~BaseC() { COUTLINE_FUNC_INFO }  // 类外编写实现

	class ChildA : public BaseA {
	public:
		ChildA() { COUTLINE_FUNC_INFO }
	};
	class ChildB : public BaseB {
	public:
		ChildB() { COUTLINE_FUNC_INFO }
		void func() { COUTLINE_FUNC_INFO }
	};
	class ChildC : public BaseC {
	public:
		ChildC() { COUTLINE_FUNC_INFO }
	};
	//
	void func_class_object() {
		COUTLINE_FUNC_PARA;
		BaseA a;  // ok
		//BaseB b;  // "virtual void B::func() = 0",不能实例化抽象类
		//BaseC c;  // "virtual C:~C() = 0",不能实例化抽象类
		ChildA ca;  // ok
		ChildB cb;  // ok
		ChildC cc;  // ok
	}
	int main() {
		COUTLINE_FUNC_MAIN;
		func_class_object();
		return 0;
	}
};

// 容器 vector/list/map/deque  // 简述不再详写
#include <vector>
#include <list>
#include <map>
namespace ns_vector {
	//std::copy();
	//std::move();
	//std::make_pair();
	//std::tuple_element();
	void func_vertor() {
		COUTLINE_FUNC_PARA;
		std::vector<int> vVec1 = { 12,23,34,45 };
		std::vector<int> vVec2(5);
		vVec2.resize(6);
		for (unsigned int i = 0; i < vVec2.size(); i++) {
			vVec2.at(i) = i*i;
		}
		cout << "vVec1: ";
		for (std::vector<int>::iterator ite = vVec2.begin(); ite != vVec2.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
		vVec2.pop_back();  // 去尾
		cout << "vVec1: ";
		for (std::vector<int>::iterator ite = vVec2.begin(); ite != vVec2.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
	}
	void func_list() {
		COUTLINE_FUNC_PARA;
		// 初始化
		std::list<int> vList1 = { 4567,3456,1234 };
		vList1.push_front(6789);
		vList1.push_back(5678);
		cout << "vList1 init: ";
		for (auto ite = vList1.begin(); ite != vList1.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
		// 排序
		vList1.sort();
		//vList1.sort<int>()
		cout << "vList1 sort(): ";
		for (auto ite = vList1.begin(); ite != vList1.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
	}
	void func_map() {
		COUTLINE_FUNC_PARA;
		// std::map<>()没有pop_back()\push_back\...函数

		// 初始化及插入数据
		std::map<string, int> vMap = { make_pair("first",1234), make_pair("second",2345) };
		vMap.insert(make_pair("third", 3456));
		vMap["last"] = 4567;
		// 输出数据
		cout << "vMap Size:" << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// 查找键值对
		auto f = vMap.find("second");
		cout << "vMap find(\"second\") = " << f->second << endl;

		// 擦除指定的迭代器
		vMap.erase(f);
		cout << "vMap Size: erase(iterator) " << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// 擦除指定键的键值对
		vMap.erase("third");
		cout << "vMap Size: erase(\"third\")  " << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// 置空
		vMap.clear();
		cout << "vMap Size: clear()         " << vMap.size() << endl;
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		func_vertor();
		func_list();
		func_map();
		return 0;
	}
}

// 类型转换  // dynamic_cast、static_cast、const_cast、reinterpret_cast 
#include <functional>  // std::function  // 不能在名字空间中包含
namespace ns_pointerCast {
	class Base {
	public:
		Base() { COUTLINE_FUNC_INFO; }
		virtual void func() { COUTLINE_FUNC_INFO; }
	};
	class Child : public Base {
	public:
		Child() { COUTLINE_FUNC_INFO; }
		void func() { COUTLINE_FUNC_INFO; }
	};
	//
	void func_dynamic_cast() {
		COUTLINE_FUNC_PARA;
		Base *pb = new Base;
		pb->func();
		Child *pc = new Child;
		pc->func();
		// 动态转换:dynamic_cast<Type>(p),会进行类型判断  
		// 派生类转基类正常
		Base *db = dynamic_cast<Base *>(pc);
		if (NULL != db) {
			db->func();
		}

		// 基类转派生类错误
		Child *dc = dynamic_cast<Child *>(pb);
		if (!dc) {
			cout << "dynamic_cast<Child *>(pb) error!" << endl;
		}
	}
	void func_static_cast() {
		COUTLINE_FUNC_PARA;
		// 静态类型转换：static_cast<Type>(p)
		// 相当于C语言中的强制转换：int a = (int)12.5f;
		double d = 12.5f;
		int i = static_cast<int>(d);
		cout << "d:" << d << " i:" << i << endl;
	}
	void func_const_cast() {
		COUTLINE_FUNC_PARA;
		// 只读属性转换：去掉类型的const或volatile属性
		const int a = 123;
		int &b = const_cast<int&>(a);
		b = 124;
		cout << "a:" << a << " b:" << b << endl;
	}
	void func_reinterpret_cast() {
		COUTLINE_FUNC_PARA;
		// 指针类型转换：reinterpret_cast<Type>(p)
		auto lambda = [](int value) { cout << __FUNCSIG__ << " value:" << value << endl; };
		int *iFunc = reinterpret_cast<int*>(&lambda);
		cout << "lambda:" << &lambda << " iFunc:" << iFunc << endl;
		//std::function<void(int)> rFunc = reinterpret_cast<void(int)>(iFunc);
		//rFunc(123);

		auto fFunc = lambda; // 并非引用，因而地址不同
		cout << "fFunc:" << &fFunc << " lambda:" << &lambda << endl;

		//
		void(*pFunc)(int);  // 声明函数指针
		pFunc = lambda;     // 函数指针赋值
		pFunc(123);         // 调用函数指针指向的函数  // void __cdecl PointerCast::funcName(int)
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		func_dynamic_cast();
		func_static_cast();
		func_const_cast();
		func_reinterpret_cast();
		return 0;
	}
};

// 智能指针
#include <memory>  // ptr
namespace ns_smartPointer {
	// 深入实践C++11智能指针
	// https://blog.csdn.net/code_peak/article/details/119722167
	// 
	// auto_ptr   : 自动，c++11后用unique_prt代替。(可能造成内存泄漏，不建议使用)
	// unique_ptr : 唯一，c++11
	// shared_ptr : 共享，c++11
	// weak_ptr   : 弱  ，c++11
	//
	// 关键知识：
	// 初始化		：赋值构造、拷贝构造、std::make_shared()、std::make_unique()
	// 析构销毁		：std::move()、std::xx_ptr.reset()、unique_ptr<Class,deletor>ptr()自定义销毁函数
	// 只释放不销毁	：std::auto_ptr.release()
	// 获取内容		：std::xx_ptr.get()
	// 计数			：std::shared_ptr.use_count()
	class PointerBase {
	public:
		PointerBase(int val = 1122) :m_val(val) { cout << __FUNCSIG__ << " m_val:" << m_val << endl; }
		~PointerBase() { cout << __FUNCSIG__ << " m_val:" << m_val << endl; }
		int value() { return m_val; }
	private:
		int m_val;
	};

	// auto_ptr:只有一个指针，赋值时会被移动  // 不推荐
	void func_autoptr() {
		COUTLINE_FUNC_PARA;
		// 初始化
		std::auto_ptr<int> ap1(new int(8));
		std::auto_ptr<int> ap2;
		ap2.reset(new int(16));  // 分配内存
		cout << "ap1:" << *ap1.get() << endl;
		cout << "ap2:" << *ap2.get() << endl;
		cout << "release before : ap1 is " << (ap1.get() != NULL ? "not " : "") << "empty " << endl;
		ap1.release();  // 释放内存
		cout << "release after  : ap1 is " << (ap1.get() != NULL ? "not " : "") << "empty " << endl;

		// 拷贝赋值构造时会导致指针转移，从而导致内存泄漏
		//测试拷贝构造
		std::auto_ptr<int> ap3(new int(8));
		std::auto_ptr<int> ap4(ap3);
		cout << "ap3 is " << (ap3.get() != NULL ? "not " : "") << "empty " << endl;  // ap1 is empty
		cout << "ap4 is " << (ap4.get() != NULL ? "not " : "") << "empty " << endl;  // ap2 is not empty
		//测试赋值构造
		std::auto_ptr<int> ap5(new int(8));
		std::auto_ptr<int> ap6 = ap5;
		cout << "ap5 is " << (ap5.get() != NULL ? "not " : "") << "empty " << endl;  // ap3 is empty
		cout << "ap6 is " << (ap6.get() != NULL ? "not " : "") << "empty " << endl;  // ap4 is not empty
	}

	// unique_ptr:只有一个对象获取到指针 // 常用
	void func_uniqueptr() {
#define COUTLINE_UNIQUEPTR_GET(VAL)		cout << ""#VAL"(" << VAL.get() << "): " << *VAL.get() << endl;
#define COUTLINE_UNIQUEPTR_ISEMPTY(VAL)		cout << ""#VAL"(" << VAL.get() << "): is " << (VAL.get() != NULL ? "not " : "") << "empty " << endl;
#define COUTLINE_UNIQUEPTR_PTRGET(STR,VAL)	cout << STR#VAL"(" << VAL.get() << "):" << VAL->value() << endl;
		// 
		COUTLINE_FUNC_PARA;
		// 初始化
		unique_ptr<int> up1(new int(123));
		unique_ptr<int> up2;
		up2.reset(new int(234));
		unique_ptr<int> up3 = make_unique<int>(345);  // 建议的初始化方法 // -std=c++14
		COUTLINE_UNIQUEPTR_GET(up1);
		COUTLINE_UNIQUEPTR_GET(up2);
		COUTLINE_UNIQUEPTR_GET(up3);

		// 移动拷贝  // 堆内存转移  // 执行后原内存指针指向空
		unique_ptr<int> up4(make_unique<int>(456));
		COUTLINE_UNIQUEPTR_GET(up4);
		COUTLINE_UNIQUEPTR_ISEMPTY(up4);
		unique_ptr<int> up5(move(up4));  // 拷贝移动
		COUTLINE_UNIQUEPTR_GET(up5);
		COUTLINE_UNIQUEPTR_ISEMPTY(up4);
		COUTLINE_UNIQUEPTR_ISEMPTY(up5);
		unique_ptr<int> up6 = move(up5);  // 赋值移动
		COUTLINE_UNIQUEPTR_GET(up6);
		COUTLINE_UNIQUEPTR_ISEMPTY(up5);
		COUTLINE_UNIQUEPTR_ISEMPTY(up6);

		// 赋值拷贝构造
		unique_ptr<int> up7(make_unique<int>(567));;
		// std::unique_ptr<int> up8(up7);		// 禁止拷贝构造，无法编译通过 // 尝试引用已删除的函数
		// std::unique_ptr<int> up9; up9 = up7;	// 禁止赋值构造，无法编译通过 // 尝试引用已删除的函数
		auto lambda = [](int val)->unique_ptr<int> { unique_ptr<int> up(new int(val)); return up; };
		unique_ptr<int> up10 = lambda(789);		// 通过函数返回值赋值  // 返回值为右值，因此unique_ptr将其保存？？或为了使move()函数有效？？
		COUTLINE_UNIQUEPTR_GET(up6);

		// 一组堆对象
		std::unique_ptr<int[]> upa1(new int[10]);
		std::unique_ptr<int[]> upa2;
		upa2.reset(new int[10]);
		std::unique_ptr<int[]> upa3(std::make_unique<int[]>(10));

		// 资源释放  // reset()释放指针和资源，release()只释放指针不释放资源
		ColorLine("// 资源释放：reset()释放指针和资源，release()只释放指针不释放资源", TCOLOR_NOTE);
		std::unique_ptr<PointerBase> upb1(new PointerBase(1234));
		std::unique_ptr<PointerBase> upb2(new PointerBase(2345));
		COUTLINE_UNIQUEPTR_PTRGET("before swap : ", upb1);
		COUTLINE_UNIQUEPTR_PTRGET("before swap : ", upb2);
		upb1.swap(upb2);  // 交换指针
		COUTLINE_UNIQUEPTR_PTRGET("after  swap : ", upb1);
		COUTLINE_UNIQUEPTR_PTRGET("after  swap : ", upb2);
		// upb1.reset(upb2); // error
		upb1.reset(new PointerBase(3456)); // 将参数保存到当前指针中(默认当前指针置空)
		COUTLINE_UNIQUEPTR_PTRGET("after reset(3456) : ", upb1);
		upb1.reset();
		cout << "after reset(): upb1(" << upb1.get() << "): is " << (upb1.get() != NULL ? "not " : "") << "empty " << endl;
		upb2.release();  // 指针置空但不释放资源(因为没有调用析构函数)

		// 添加自定义资源释放函数  // 构造->自定义lambda释放函数->lambda函数中执行delete触发析构
		ColorLine("// 添加自定义资源释放函数", TCOLOR_NOTE);
		auto deletor = [](PointerBase *pMySocket) { COUTLINE_FUNC_INFO; delete pMySocket; };
		//unique_ptr<mySocket, void(*)(mySocket * pMySocket)> ups1(new mySocket(4567), deletor);
		unique_ptr<PointerBase, decltype(deletor)> upd1(new PointerBase(4567), deletor);  // decltype(T):自动推导类型
		upd1.reset();  // 主动触发释放以便及时输出信息

		cout << __FUNCSIG__ << " : end" << endl;
#undef COUTLINE_UNIQUEPTR_GET
#undef COUTLINE_UNIQUEPTR_ISEMPTY
#undef COUTLINE_UNIQUEPTR_PTRGET
	}

	// shared_ptr：共享同一个指针 // 常用
	void func_shareptr() {
#define COUTLINE_SHAREPTR_INFO_END(XX) ""#XX" addr:" << XX << " use_count():" << XX.use_count() \
		<< " value:" << (XX.get() == NULL ? -1 : XX.get()->value()) << endl;
#define COUTLINE_SHAREPTR_INFO_1(XX) cout << "--" COUTLINE_SHAREPTR_INFO_END(XX)
#define COUTLINE_SHAREPTR_INFO_2(XX) cout << "  " COUTLINE_SHAREPTR_INFO_END(XX)
		//
		COUTLINE_FUNC_PARA;
		// 初始化
		shared_ptr<int> sp1(new int(123));
		shared_ptr<int> sp2;
		sp2.reset(new int(234));
		shared_ptr<int> sp3 = make_shared<int>(345);
		// 引用计数
		shared_ptr<PointerBase> spb1(new PointerBase(1234));  // 初始化计数为1
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():1 value:1234
		shared_ptr<PointerBase> spb2(spb1);			// 计数+1=2
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():2 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb2);	//   spb2 addr:007E0D60 use_count():2 value:1234
		shared_ptr<PointerBase> spb3 = spb1;		// 计数+1=3
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():3 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb3);	//   spb3 addr:007E0D60 use_count():3 value:1234
		shared_ptr<PointerBase> spb4 = move(spb3);	// 移动spb3的指针和计数给spb4 // spb1=spb3=计数为3
		COUTLINE_SHAREPTR_INFO_1(spb3);	// --spb3 addr:00000000 use_count():0 value:-1
		COUTLINE_SHAREPTR_INFO_2(spb4);	//   spb4 addr:007E0D60 use_count():3 value:1234
		spb4.reset();								// 计数-1=2
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():2 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb4);	//   spb4 addr:00000000 use_count():0 value:-1
		spb2.reset();								// 计数-1=1
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():1 value:1234
		spb1.reset();	//计数为0时执行析构 // __thiscall SmartPointer::PointerBase::~PointerBase(void) m_val:1234
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:00000000 use_count():0 value:-1
#undef COUTLINE_SHAREPTR_INFO_END
#undef COUTLINE_SHAREPTR_INFO_1
#undef COUTLINE_SHAREPTR_INFO_2
	}

	// weak_ptr：// 辅助
	// 1.打破智能指针的循环引用。
	// 2.当shared_ptr资源未完全释放时，重新获取资源指针，然后再进行操作 
	void func_weakptr() {
		COUTLINE_FUNC_PARA;
		// 初始化
		std::shared_ptr<int> sp1(new int(123));	//	创建一个std::shared_ptr对象
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		std::weak_ptr<int> wp2(sp1);			//通过构造函数得到一个std::weak_ptr对象
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		cout << "wp2 use_count(): " << wp2.use_count() << endl;
		std::weak_ptr<int> wp3 = sp1;			//通过赋值运算符得到一个std::weak_ptr对象
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		std::weak_ptr<int> wp4 = wp2;			//通过一个std::weak_ptr对象得到另外一个std::weak_ptr对象
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		// 引用计数
		//// tmpConn_ 是一个 std::weak_ptr<TcpConnection> 对象
		//// tmpConn_ 引用的TcpConnection已经销毁，直接返回
		//if (tmpConn_.expired())
		//    return;
		//std::shared_ptr<TcpConnection> conn = tmpConn_.lock();
		//if (conn) {
		//    //对conn进行操作，省略...
		//}
		//
		// std::vector<std::weak_ptr<Subscriber>> m_subscribers;
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		func_autoptr();
		func_uniqueptr();
		func_shareptr();
		func_weakptr();
		return 0;
	}
}

// lambda 函数及邦定 // std::bind()/std::palceholder::_1  // 函数指针
namespace ns_functionBind {
	// C++ 中的 Lambda 表达式
	// https://docs.microsoft.com/zh-cn/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160
	// C++11 之 std::function、std::bind、std::placeholders
	// https://blog.csdn.net/u014209688/article/details/90741985
	// Lambda 表达式的示例
	// https://docs.microsoft.com/zh-cn/cpp/cpp/examples-of-lambda-expressions?view=msvc-160
	char* ng_func(int value = 123456, char*str = "ng_func", float d = 1234.56f) {
		cout << __FUNCSIG__ << " int:" << value << " char:" << str << " float:" << d << endl;
		return __FUNCSIG__;
	}
	class FBind {
	public:
		char* ngc_func(int value = 234567, char*str = "ngc_func", float d = 2345.67f) {
			cout << __FUNCSIG__ << " int:" << value << " char:" << str << " float:" << d << endl;
			return __FUNCSIG__;
		}
	};
	//
	void func_lambda() {
		COUTLINE_FUNC_PARA;
		// lambda 函数调用   // 条件太多请参考在线文档
		auto lambda = [](int value)->char* {
			cout << __FUNCSIG__ << " value:" << value << endl; return __FUNCSIG__;
		};
		cout << lambda(456) << endl;
		int n = [](int x, int y) { return x + y; }(5, 4);
		cout << n << endl;
	}
	void func_stdFunction() {
		COUTLINE_FUNC_PARA;
		// 函数指针
		// 默认函数直接调用
		cout << ng_func() << endl;
		// 函数指针
		char*(*pFunc)(int, char*, float);
		pFunc = ng_func;
		cout << pFunc(234, "pFunc", 2.34f) << endl;
		// std::function函数指针
		std::function<char*(int, char*, float)> fFunc1 = ng_func;
		cout << fFunc1(345, "fFunc1", 3.45f) << endl;
		// function bind lambda
		auto lambda = [](int num, char*sz)->char* {
			cout << __FUNCSIG__ << " num:" << num << " char*:" << sz << endl; return __FUNCSIG__; };
		std::function<char*(int, char*)>fFunc2 = lambda;
		cout << lambda(456, "lambda") << endl;
		cout << fFunc2(567, "function") << endl;
	}
	void func_stdBind() {
		COUTLINE_FUNC_PARA;
		// std::bind(函数名,类对象,参数)
		// std::placeholders::_1 // 回调函数
		// 普通函数的正常绑定
		std::function<char*(int, char*, float)> fFunc2 =
			std::bind(&ng_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		cout << fFunc2(456, "fFunc2", 4.56f) << endl;
		// 绑定类对象、调换、提供缺省参数
		FBind fb;
		std::function<char*(char*, int)> fFunc3 =
			std::bind(&FBind::ngc_func, fb, std::placeholders::_2, std::placeholders::_1, 0.0f);
		cout << fFunc3("fFunc3", 567) << endl;
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		func_lambda();
		func_stdFunction();
		func_stdBind();
		return 0;
	}
}

// 异常捕获  // 待完善
namespace ns_exception {
#define COUTLINE_CATCH_TYPE(VAR) cout << __FUNCSIG__ << " catch " << typeid(VAR).name() << " = " << VAR << endl;
#define COUTLINE_CATCH_OTHER     cout << __FUNCSIG__ << " catch ..." << endl;
	// throw
	// try catch
	// exception...
	//
	// Exception specifications (throw, noexcept) (C++)
	// https://docs.microsoft.com/en-us/cpp/cpp/exception-specifications-throw-cpp?view=msvc-160
	char *func_throw(int num) throw(...) {
		cout << __FUNCSIG__ << " num = " << num << endl;
		switch (num) {
		case 0:  cout << __FUNCSIG__ << " throw [\"num = 0\"]" << endl; throw "\"num = 0\""; break;
		case -1: cout << __FUNCSIG__ << " throw [-1]" << endl; throw - 1;	         break;
		case -2: cout << __FUNCSIG__ << " throw [-2.345f]" << endl; throw - 2.345f;	     break;
		default: cout << __FUNCSIG__ << " switch default" << endl;                      break;
		}
		return __FUNCSIG__;
	}
	char* /*__declspec(nothrow)*/ func_catch(int num) {
		cout << __FUNCSIG__ << " num = " << num << endl;
		try { cout << func_throw(num) << endl; }
		catch (char* &err) { COUTLINE_CATCH_TYPE(err); }
		catch (int &err) { COUTLINE_CATCH_TYPE(err); }
		catch (...) { COUTLINE_CATCH_OTHER; }
		cout << __FUNCSIG__ << " end" << endl;  //为什么 catch 之后会继续执行??
		return __FUNCSIG__;
	}
	//
	void func_catch() {
		COUTLINE_FUNC_PARA;
		// 拆分
		try { cout << func_throw(1) << endl; }
		catch (...) { COUTLINE_CATCH_OTHER; }  // 传1不会被捕获到
		try { cout << endl << func_throw(0) << endl; }
		catch (char* &err) { COUTLINE_CATCH_TYPE(err); }
		try { cout << endl << func_throw(-1) << endl; }
		catch (int &err) { COUTLINE_CATCH_TYPE(err); }
		try { cout << endl << func_throw(-2) << endl; }
		catch (...) { COUTLINE_CATCH_OTHER; }

		// 完整
		//try { cout << func_throw(-1) << endl; }
		//catch (char* e) { cout << __FUNCSIG__ << " catch " << typeid(err).name() << " = " << err << endl; }
		//catch (int e)   { cout << __FUNCSIG__ << " catch " << typeid(err).name() << " = " << err << endl; }
		//catch (...)     { cout << __FUNCSIG__ << " catch ..." << endl; }
		cout << endl;
		cout << func_catch(1) << endl << endl;
		cout << func_catch(0) << endl << endl;
		cout << func_catch(-1) << endl << endl;
		cout << func_catch(-2) << endl;

		try {
			// warning C4290: 忽略 C++ 异常规范，但指示函数不是 __declspec(nothrow)
#pragma warning (disable:4290)
			// 推测原因：当前编译器对throw(int)的支持不完善，参见：
			// 1.编译器警告（等级 3）C4290
			// https://docs.microsoft.com/zh-cn/previous-versions/sa28fef8(v=vs.120)
			// 2.异常规范 (引发，noexcept) (C++) --> 下表总结了异常规范的 Microsoft C++ 实现
			// https://docs.microsoft.com/zh-cn/cpp/cpp/exception-specifications-throw-cpp?view=msvc-160
			auto lambda = [](int num)throw(int)->char* { throw num; };
			lambda(-3);
		}
		catch (int &err) { COUTLINE_CATCH_TYPE(err); }
		//assert(0);  // throw // noexcept 
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		func_catch();
		return 0;
	}
#undef COUTLINE_CATCH_TYPE
#undef COUTLINE_CATCH_OTHER
}

// 创建进程线程  // 非锁的使用 // 待完善
namespace ns_processThread {
	// std::this_thread::get_id()

	//
	struct ParamThread {
		// 注意：CreateThread传入handle，真正参数用lpParam取出
		ParamThread* handle = NULL;  // 当前结构体对象的地址
		HANDLE   hMutex = NULL;
		HANDLE   hThread = NULL;
		// CreateThread的真正参数列表
		LPSECURITY_ATTRIBUTES lpSA = NULL;
		SIZE_T dwSize = 0;  // 0:自动
		LPTHREAD_START_ROUTINE lpFunc = NULL;
		LPVOID lpParam = NULL;  // 注意：CreateThread传入handle，不传入lpParam
		DWORD dwFlag = 0;  // 0:立即
		DWORD dwTid = NULL;
	};
	DWORD WINAPI func_thread(LPVOID lpParam) {
		COUTLINE_FUNC_INFO;  // 创建时立即启动线程，会导致与主线程抢夺标准输出
		ParamThread *ppt = (ParamThread*)lpParam;
		WaitForSingleObject(ppt->hMutex, INFINITE);
		// 需要主线程等待子线程完成后再退出，以防止主线程退出时销毁子线程会用到的资源
		cout << __FUNCSIG__ << " handle:" << ppt->hThread << " tid: " << ppt->dwTid
			<< " param: [" << (char*)ppt->lpParam << "]" << endl;
		ExitThread(1);  // 主动退出线程
		cout << __FUNCSIG__ << " end" << endl;
		return 0;
	}
	void func_createThread1() {
		COUTLINE_FUNC_PARA;
		// 创建线程相关参数：
		ParamThread pt;
		ParamThread *ppt = &pt;
		ppt->handle = ppt;
		ppt->hMutex = CreateMutex(NULL, TRUE, L"console output mutex");  // 创建互斥锁 // TRUE:创建即等待
		ppt->lpFunc = func_thread;  // 线程函数
		ppt->lpParam = "threadParam";// "--cmd=\"tree /F\" --dir=\"C:\\Program Files (x86)\"";
		ppt->dwFlag = CREATE_SUSPENDED; // 0:立即启动；CREATE_SUSPENDED：等待ResumeThread；
		//ppt->dwTid = 0;  // 线程ID
		// 创建线程
		ppt->hThread = CreateThread(ppt->lpSA, ppt->dwSize,
			ppt->lpFunc, ppt->handle, ppt->dwFlag, &ppt->dwTid);  // 创建并启动线程
		cout << __FUNCSIG__ << " handle:" << ppt->hThread << " tid: " << ppt->dwTid \
			<< " param: [" << (char*)ppt->lpParam << "]" << endl;  // 线程外直接取出即可
		// 启动线程
		ResumeThread(ppt->hThread);
		// 线程启动需要时间，但耗时不定，不能在下面添加输出语句
		ReleaseMutex(ppt->hMutex);  // 释放锁资源  // 子线程有信号后才继续执行
		// 此处不知道子线程什么时候输出完毕，不能添加输出语句
		WaitForSingleObject(pt.hThread, INFINITE); // 等待子线程退出
		cout << __FUNCSIG__ << " after WaitForSingleObject(thread)" << endl;
		CloseHandle(ppt->hMutex);  // 释放锁资源
		cout << __FUNCSIG__ << " end" << endl;
	}
	// 
	struct ProcessCreate {
		BOOL bCreate = FALSE;
		//
		LPCWSTR lpApp = NULL;  // 绝对路径？
		LPWSTR lpCmd = NULL;
		LPSECURITY_ATTRIBUTES lpPA = NULL;
		LPSECURITY_ATTRIBUTES lpTA = NULL;
		BOOL bInher = FALSE;
		DWORD dwFlags = 0;  // CREATE_NEW_CONSOLE
		LPVOID lpEnv = NULL;
		LPCWSTR lpDir = NULL;
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi = {};
	};
	void func_createProcess() {
		COUTLINE_FUNC_PARA;
		//
		WCHAR szApp[MAX_PATH] = {};
		GetModuleFileName(NULL, szApp, MAX_PATH);
		//
		ProcessCreate pc;
		ProcessCreate *ppc = &pc;
		// ppc->lpApp = L"c://windows//notepad.exe";
		ppc->lpApp = szApp;
		ppc->lpCmd = L"--onlyStartInfo -desktop.ini -\"read write\" form CreateProcess()";
		ppc->dwFlags = CREATE_NEW_CONSOLE;  // 打开新的控制台窗口而非合并到父进程窗口
		ppc->bCreate = CreateProcess(ppc->lpApp, ppc->lpCmd, ppc->lpPA, ppc->lpTA,
			ppc->bInher, ppc->dwFlags, ppc->lpEnv, ppc->lpDir, &ppc->si, &ppc->pi);
		cout << "CreateProcess():" << (ppc->bCreate ? "Success" : "Error")
			<< " GetLastError(): " << GetLastError() << endl;
	}
	//
	struct ProcessShellEx {
		BOOL bSucc = FALSE;
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	};
	void func_shellExecute() {
		COUTLINE_FUNC_PARA;
		// 暂未查找到如何在新窗口中执行
		ColorLine("暂未查找到如何在新窗口中执行", TCOLOR_NOTE);
		return;

		WCHAR szApp[MAX_PATH] = {};
		GetModuleFileName(NULL, szApp, MAX_PATH);
		// ShellExecute()
		// WinExec和LoadModule
		ProcessShellEx pse;
		ProcessShellEx*ppse = &pse;
		ppse->sei.lpVerb = L"open";
		ppse->sei.lpFile = szApp;
		ppse->sei.fMask = SEE_MASK_NO_CONSOLE;
		ppse->sei.lpParameters = L"--onlyStartInfo form ShellExecuteEx()";
		ppse->bSucc = ShellExecuteEx(&ppse->sei);
		cout << "ShellExecute():" << ppse->bSucc << " GetLastError(): " << GetLastError() << endl;
	}
	//
	struct ProcessWinExec {
		UINT uRun = 0;
		LPCSTR lpCmd = NULL;
		UINT uShow = 0;
	};
	void fun_winexec() {
		COUTLINE_FUNC_PARA;
		//
		CHAR szApp[MAX_PATH] = {};
		GetModuleFileNameA(NULL, szApp, MAX_PATH);
		std::string strCmd(szApp);
		strCmd += "--onlyStartInfo -desktop.ini -\"read write\" form WinExec()";
		//
		ProcessWinExec pw;
		ProcessWinExec*ppw = &pw;
		ppw->lpCmd = strCmd.c_str();  // not WCHAR*
		ppw->uShow = SW_SHOW;
		ppw->uRun = WinExec(ppw->lpCmd, ppw->uShow);
		cout << "WinExec():" << (ppw->uRun ? "Success" : "Error")
			<< " WinExec(): " << GetLastError() << endl;
	}
	//
	void func_system() {
		COUTLINE_FUNC_PARA;
		system("start cmd");  // 通过调用系统的控制台执行语句
		cout << __FUNCSIG__ << "end" << endl;
	}
	//
	int main() {
		COUTLINE_FUNC_MAIN;
		func_createThread1();
		//func_createProcess();
		//func_shellExecute();
		//fun_winexec();
		//func_system();
		return 0;
	}
};

// 锁与信号量  // Mutex/Semaphore
namespace ns_mutex {
	// std::lock_guard  // since C++17
	// std::lock_guard<std::mutex>
	// std::lock_guard<std::recursive_mutex>
	//
	// WakeAllConditionVariable  // 条件变量
	// InitializeCriticalSection //
	// InterlockedAdd(+) / InterlockedIncrement(-)  // 原子锁


	// 锁
	typedef struct ParamMutex {
// WINBASEAPI
// _Ret_maybenull_
// HANDLE
// WINAPI
// CreateMutexW(
//     _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
//     _In_ BOOL bInitialOwner,
//     _In_opt_ LPCWSTR lpName
//     );
//
// WINBASEAPI
// BOOL
// WINAPI
// ReleaseMutex(
//     _In_ HANDLE hMutex
//     );
		HANDLE hMutex = NULL;
		// CreateMutex param:
		LPSECURITY_ATTRIBUTES lpAttr = NULL;
		BOOL bOwner = TRUE;
		LPCWSTR lpName = NULL;
		// ReleaseMutex
		BOOL bRelease = FALSE;
		// WaitForSingleObject
		DWORD dwWait = 0;
		ParamMutex(LPCWSTR lpName = NULL, BOOL bInitialOwner = TRUE)
			:lpName(lpName), bOwner(bInitialOwner) { }
		~ParamMutex() { if (hMutex) CloseHandle(hMutex); hMutex = NULL; }
	}Mutexs;
	void func_mutex() {
		COUTLINE_FUNC_PARA;
		// 创建示例
		for (; 0;) {
			HANDLE hMutex0 = CreateMutex(NULL, FALSE, __FILEW__);
			if (!hMutex0 || GetLastError()) {  // "hMutex0为空"或"hMutex0不为空但GetLastError不为0"
				cout << "CreateMutex() ERROR Handle:" << hMutex0 << " Code:" << GetLastError() << endl;
			} else {
				cout << "CreateMutex " << __FILEW__ << " OK" << endl;
				CloseHandle(hMutex0);
			}
			// 创建校验  // ERROR_INVALID_HANDLE  // ERROR_ALREADY_EXISTS
			HANDLE hMutex1 = CreateMutex(NULL, FALSE, NULL);
			cout << "hMutex1:" << hMutex1 << " Name:" << "(NULL)" << " ErrorCode:" << GetLastError() << endl;
			HANDLE hMutex2 = CreateMutex(NULL, FALSE, NULL);
			cout << "hMutex2:" << hMutex2 << " Name:" << "(NULL)" << " ErrorCode:" << GetLastError() << endl;
			HANDLE hMutex3 = CreateMutex(NULL, FALSE, L"hMutex");
			cout << "hMutex3:" << hMutex3 << " Name:" << "hMutex" << " ErrorCode:" << GetLastError() << endl;
			HANDLE hMutex4 = CreateMutex(NULL, FALSE, L"hMutex");
			cout << "hMutex4:" << hMutex4 << " Name:" << "hMutex" << " ErrorCode:" << GetLastError() << endl;
			CloseHandle(hMutex1);
			CloseHandle(hMutex2);
			CloseHandle(hMutex3);
			CloseHandle(hMutex4);
			break;
		}
		// wait
		for (; 0;) {
			ColorLine("WaitForSingleObject() and WaitForMultipleObjects():", TCOLOR_KEY);
			Mutexs mu[3] = { Mutexs(L"hMutex1"),Mutexs(L"hMutex2"),Mutexs(L"hMutex3") };
			for (int i = 0; i < 3; i++)
				mu[i].hMutex = CreateMutex(mu[i].lpAttr, mu[i].bOwner, mu[i].lpName);
			// WaitForSingleObject
			ReleaseMutex(mu[0].hMutex);
			mu[0].dwWait = WaitForSingleObject(mu[0].hMutex, INFINITE);
			// WaitForMultipleObjects
			HANDLE hMutexArr[2] = { mu[1].hMutex,mu[2].hMutex };
			ReleaseMutex(hMutexArr[0]);
			ReleaseMutex(hMutexArr[1]);
			DWORD dwWait = WaitForMultipleObjects(2, hMutexArr, TRUE, INFINITE);
			// Mutexs 内有析构，这里不再添加 CloseHandle 函数
		}
		// 互斥执行示例：
		Mutexs mtx(L"hMutex");
		Mutexs *pmtx = &mtx;
		pmtx->hMutex = CreateMutex(pmtx->lpAttr, pmtx->bOwner, pmtx->lpName);
		//
		cout << __FUNCSIG__ << " CreateThread() before" << endl;
		auto lambda = [](LPVOID lpParam)->DWORD WINAPI {
			cout << __FUNCSIG__ << " tid:" << GetCurrentThreadId() << endl;
			Mutexs*pme = (Mutexs*)lpParam;
			cout << "WaitForSingleObject(Mutex) before" << " tid:" << GetCurrentThreadId() << endl;
			WaitForSingleObject(pme->hMutex, INFINITE);
			cout << "WaitForSingleObject(Mutex) after" << " tid:" << GetCurrentThreadId() << endl;
			return 0;
		};  // warning C4229: 使用了记时错误: 忽略数据上的修饰符
		HANDLE hThread = CreateThread(NULL, 0, lambda, (LPVOID)pmtx, CREATE_SUSPENDED, NULL);
		cout << __FUNCSIG__ << " CreateThread() after" << endl;
		ResumeThread(hThread);
		//
		pmtx->bRelease = ReleaseMutex(pmtx->hMutex);
		//
		pmtx->dwWait = WaitForSingleObject(hThread, INFINITE);
		cout << __FUNCSIG__ << " WaitForSingleObject(hThread) after" << endl;
		// close handle
		CloseHandle(hThread); 
		hThread = NULL;
		CloseHandle(pmtx->hMutex); 
		pmtx->hMutex = NULL;
	}
	// 信号量
	typedef struct ParamSema {
// WINBASEAPI
// _Ret_maybenull_
// HANDLE
// WINAPI
// CreateSemaphoreW(
//     _In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
//     _In_     LONG lInitialCount,
//     _In_     LONG lMaximumCount,
//     _In_opt_ LPCWSTR lpName
//     );
// 
// WINBASEAPI
// BOOL
// WINAPI
// ReleaseSemaphore(
//     __in      HANDLE hSemaphore,
//     __in      LONG lReleaseCount,
//     __out_opt LPLONG lpPreviousCount
//     );
		HANDLE hSema = NULL;
		// CreateSemaphoreEx():
		LPSECURITY_ATTRIBUTES lpAttr = NULL;
		LONG lInit = 0;  // 0 < init << max
		LONG lMax = 0;  // 0 < max
		LPCWSTR lpName = NULL;
		// ReleaseSemaphore():
		BOOL bRelease = FALSE;  // 是否成功
		LONG lRelease = 1;      // 释放个数
		LONG lpPrev = 0;        // 调用前可用数量
		// WaitForSingleObject
		DWORD dwWait = 0;
		ParamSema(LPCWSTR lpName = NULL, LONG lMaximumCount = 1, LONG lInitialCount = 1)
			:lpName(lpName), lMax(lMaximumCount), lInit(lInitialCount){}
		~ParamSema() { if (hSema) CloseHandle(hSema); hSema = NULL; }
	} Sema;
	void func_semaphore() {
		// WINBASEAPI
		// BOOL  // 是否执行成功
		// WINAPI
		// ReleaseSemaphore(
		// 	_In_ HANDLE hSemaphore,  // 锁的句柄
		// 	_In_ LONG lReleaseCount,  // 当前释放个数
		// 	_Out_opt_ LPLONG lpPreviousCount  // 已经释放中还未使用的个数
		// );
		//
		// 1.CreateSemaphoreEx():
		//   dwDesiredAccess=SEMAPHORE_ALL_ACCESS,没有默认值0
		// 2.ReleaseSemaphore():
		//   lpPreviousCount当前调用为之前未使用的个数.
		//   lReleaseCount后可用总数不能超过CreateSemaphoreEx()中的lMaximumCount
		COUTLINE_FUNC_PARA;
		//
#define COUTLINE_FUNC_SEMAPHORE_CREATE(INFO)  \
			cout << "CreateSemaphore()" << INFO \
				<< " Handle:" << pse->hSema << " Name:" << pse->lpName \
				<< " Max:" << pse->lMax << " Init:" << pse->lInit << endl;

#define COUTLINE_FUNC_SEMAPHORE_RELEASE(NUM) \
		pse->lRelease = NUM; \
		pse->bRelease = ReleaseSemaphore(pse->hSema, pse->lRelease, &pse->lpPrev); \
		cout << "ReleaseSemaphore(" << pse->hSema << ", " << pse->lRelease << ", " \
			<< "&" << pse->lpPrev << ") : " << (pse->bRelease ? "TRUE" : "FALSE") << endl;
#define COUTLINE_FUNC_SEMAPHORE_WAIT \
		pse->dwWait = WaitForSingleObject(pse->hSema, INFINITE); \
		cout << "WaitForSingleObject(" << pse->hSema << ", INFINITE) is " << pse->dwWait << endl;
		//
		//
		Sema se(L"hSema1", 5, 1); // 创建时即有一个可用的信号
		Sema *pse = &se;
		pse->hSema = CreateSemaphore(pse->lpAttr, pse->lInit, pse->lMax, pse->lpName);
		if (!pse->hSema || GetLastError()) {
			COUTLINE_FUNC_SEMAPHORE_CREATE(" ERROR" << " Code:" << GetLastError());
			return;
		}
		COUTLINE_FUNC_SEMAPHORE_CREATE(" OK");
		// ReleaseSemaphore: 增加可用数

		COUTLINE_FUNC_SEMAPHORE_RELEASE(3);
		COUTLINE_FUNC_SEMAPHORE_RELEASE(1);
		// WaitForSingleObject: 减少可用数
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		// 总数超出max时失败(无效释放)
		COUTLINE_FUNC_SEMAPHORE_RELEASE(2);
		// 过渡使用时会导致假死
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		//COUTLINE_FUNC_SEMAPHORE_WAIT; // 过度使用会出现假死
		CloseHandle(pse->hSema);
		pse->hSema = NULL;
		//
		//
#undef COUTLINE_FUNC_SEMAPHORE_CREATE
#undef COUTLINE_FUNC_SEMAPHORE_RELEASE
#undef COUTLINE_FUNC_SEMAPHORE_WAIT
	}
	// 事件
	typedef struct ParamEvent {
// WINBASEAPI
// _Ret_maybenull_
// HANDLE
// WINAPI
// CreateEventW(
//     _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
//     _In_ BOOL bManualReset,
//     _In_ BOOL bInitialState,
//     _In_opt_ LPCWSTR lpName
//     );
// 
// WINBASEAPI
// BOOL
// WINAPI
// SetEvent(
//     _In_ HANDLE hEvent
//     );
// 
// WINBASEAPI
// BOOL
// WINAPI
// ResetEvent(
//     _In_ HANDLE hEvent
//     );
		HANDLE hEvent = NULL;
		// CreateEvent
		LPSECURITY_ATTRIBUTES lpAttr = NULL;
		BOOL bManual = FALSE;  // FALSE:需要手动调用resetevent释放信号
		BOOL bInit = FALSE;    // FALSE:初始无信号
		LPCWSTR lpName = NULL;
		//
		DWORD dwWait = 0;   // WaitForSingleObject...
		BOOL bSet = FALSE;  // SetEvent \ ResetEvent
		//
		ParamEvent(LPCWSTR lpName = NULL, BOOL bInitialState = FALSE, BOOL bManualReset = FALSE)
			:lpName(lpName), bInit(bInitialState), bManual(bManualReset) { }
		~ParamEvent() { if (hEvent) CloseHandle(hEvent); hEvent = NULL; }
	}Events;
	void func_event() {
		COUTLINE_FUNC_PARA;
		// SetEvent最多一个信号
		Events ev(L"hEvent", TRUE, FALSE);
		Events*pev = &ev;
		pev->hEvent = CreateEvent(pev->lpAttr, pev->bManual, pev->bInit, pev->lpName);
		cout << "CreateEvent(" << pev->lpAttr << ", " << pev->bManual << ", " << pev->bInit << ", " 
			<< pev->lpName << ") --> " << pev->hEvent << " ErrorCode:" << GetLastError() << endl;
		pev->bSet = ResetEvent(pev->hEvent);  // 消耗信号
		cout << "ResetEvent(" << pev->hEvent << ") --> " << pev->bSet << endl;
		pev->bSet = SetEvent(pev->hEvent);    // 释放信号
		cout << "SetEvent(" << pev->hEvent << ") --> " << pev->bSet << endl;
		pev->dwWait = WaitForSingleObject(pev->hEvent, INFINITE);
		cout << "WaitForSingleObject after" << endl;
	}
// 	// 可等待句柄
// 	typedef struct ParamWaitableTimerEx {
// 		HANDLE hTimer = NULL;
// 		LPSECURITY_ATTRIBUTES lpAttr = NULL;
// 		LPCWSTR lpName = NULL;
// 		DWORD dwFlags = 0;
// 		DWORD dwAccess = 0;
// 	}TimerEx;
// 	void func_timerex() {
// 		COUTLINE_FUNC_PARA;
// 		TimerEx te;
// 		TimerEx*pte = &te;
// 		pte->lpName = L"my wait able time";
// 		pte->hTimer = CreateWaitableTimerEx(pte->lpAttr, pte->lpName, pte->dwFlags, pte->dwAccess);
// 	}

	// 
	int main() {
		COUTLINE_FUNC_MAIN;
		// 互斥锁创建示例参见func_mutex();
		// 一次性锁：Mutex\Semaphore
		// 可重用锁：Event
		ColorLine("// 创建锁可能总会返回句柄。创建成功则GetLastError返回0。匿名锁总是返回0。", TCOLOR_NOTE);
		ColorLine("// WaitForSingleObject() 过度使用会导致阻塞(INFINITE时不会错误立即返回)", TCOLOR_NOTE);
		ColorLine("// WaitForMultipleObjects() 入参只能是创建函数返回的句柄所组成的数组", TCOLOR_NOTE);
		ColorLine("// ReleaseSemaphore() 释放总数超过Max时失败", TCOLOR_NOTE);
		ColorLine("// 多个锁为一次性，不能重复使用", TCOLOR_NOTE);
		func_mutex();
		func_semaphore();
		func_event();
		return 0;
	}
}

// //////////////////////////////////////////////////////////////////////
// 以上为已结束部分，以下为未完成部分
// //////////////////////////////////////////////////////////////////////

// socket
// <WinSock.h>已经隐藏包含在<Windows.h>
//#include <WinSock.h>
//#include <WinSock2.h> // WSAPoll
#pragma comment(lib,"ws2_32.lib")
namespace ns_socket {
	// socket TCP/UDP 服务端/客户端  
	// select\poll\epoll 服务端

// 	struct sockaddr {
// 		u_short sa_family;              /* address family */
// 		char    sa_data[14];            /* up to 14 bytes of direct address */
// 	};
//
// 	struct sockaddr_in {
// 		short   sin_family;
// 		u_short sin_port;
// 		struct  in_addr sin_addr;
// 		char    sin_zero[8];
// 	};

//#include <winsock.h>
//#pragma comment(lib,"ws2_32.lib")

	// 单线程UDP服务器经典网络模型
	void func_simpleUDPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12344)) {
		COUTLINE_FUNC_PARA;
		SOCKET hSockSvr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (hSockSvr == INVALID_SOCKET) { return; }
		SOCKADDR_IN addrSvr = { };
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = sin_port;// htons(12344);
		addrSvr.sin_addr.S_un.S_addr = S_addr;// INADDR_ANY;
		::bind(hSockSvr, (SOCKADDR*)&addrSvr, sizeof(addrSvr));
		//通信
		// 1 服务器首先接收数据
		char szBuff[MAX_PATH] = { 0 };
		SOCKADDR_IN addrClient = { 0 };
		int nLen = sizeof(addrClient);
		int nRecv = recvfrom(hSockSvr, szBuff, MAX_PATH, 0, (SOCKADDR*)&addrClient, &nLen);
		printf("%s,%d\n", szBuff, nRecv);
		// 2 发送数据
		sendto(hSockSvr, szBuff, strlen(szBuff), 0, (SOCKADDR*)&addrClient, sizeof(addrClient));

		closesocket(hSockSvr);
	}
	// 单线程UDP客户端经典网络模型
	void func_simpleUDPClient(ULONG S_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12344), char*szMsg = "Hello Server") {
		COUTLINE_FUNC_PARA;
		SOCKET hSockClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (hSockClient == INVALID_SOCKET) { return; }
		SOCKADDR_IN addrSvr = { };
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = sin_port;// htons(12344);
		addrSvr.sin_addr.S_un.S_addr = S_addr;// inet_addr("127.0.0.1");
		// 通信
		// 约定客户端首先发送数据
		char szSend[] = "Hello World";
		sendto(hSockClient, szSend, strlen(szSend), 0, (SOCKADDR*)&addrSvr, sizeof(addrSvr));
		//然后接收数据
		char szBuff[MAX_PATH] = { 0 };
		int nLen = sizeof(addrSvr);
		int nRecv = recvfrom(hSockClient, szBuff, MAX_PATH, 0, (SOCKADDR*)&addrSvr, &nLen);
		printf("%s,%d\n", szBuff, nRecv);

		closesocket(hSockClient);
	}

	// 单线程TCP客户端经典网络模型
	void func_simpleTCPClient(ULONG S_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12345), char*szMsg = "Hello Server") {
		COUTLINE_FUNC_PARA;
		// 设置
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addrServer.sin_port = htons(12345);
		addrServer.sin_family = AF_INET;
		char szRecv[MAX_PATH] = {};
		WSADATA wd = {};
		// 执行
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		int iConnectRtn = connect(sockClient, (sockaddr*)&addrServer, sizeof(addrServer));
		int iSendRtn = send(sockClient, szMsg, strlen(szMsg) + 1, 0);
		cout << "[" << szMsg << "] is send " << iSendRtn << "/" << strlen(szMsg) + 1 << endl;
		int iRecvRtn = recv(sockClient, szRecv, MAX_PATH, 0);
		cout << "[" << szRecv << "] is recv " << iRecvRtn << "/" << MAX_PATH << endl;
		closesocket(sockClient);
		WSACleanup();
	}
	// 单线程TCP服务器经典网络模型
	void func_simpleTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
		// 设置
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;// htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET; // IPv4
		addrServer.sin_port = sin_port;  // htons(12345);  // 字节序转换
		addrServer.sin_zero;             // 结构体补齐字节，不错处理
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szMsg[MAX_PATH] = {};
		// 执行
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 获取系统分配的SOCKET接口ID
		cout << "sockServer:" << sockServer << " ErrorCode:" << GetLastError() << endl;
		int iBindRtn = ::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // 将IP与端口绑定到SOKCRT接口ID
		cout << "iBindRtn:" << iBindRtn << " ErrorCode:" << GetLastError() << endl;
		int iListenRtn = listen(sockServer, SOMAXCONN);  // 监听IP:端口
		cout << "iListenRtn:" << iListenRtn << " ErrorCode:" << GetLastError() << endl;
		SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen); // 从队列中取出一条连接信息
		cout << "sockClient:" << sockClient << " form " 
			<< inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port 
			<< " ErrorCode:" << GetLastError() << endl;
		int iRecvRtn = recv(sockClient, szMsg, MAX_PATH, 0);  // 从连接信息中读取消息  // 0:缺省
		cout << "[" << szMsg << "] is recv " << iRecvRtn << "/" << MAX_PATH << endl;
		int iSendRtn = send(sockClient, szMsg, strlen(szMsg) + 1, 0);  // 返回信息  // 0:缺省
		cout << "[" << szMsg << "] is send " << iSendRtn << "/" << strlen(szMsg) + 1 << endl;
		closesocket(sockClient);
		closesocket(sockServer);
		WSACleanup();
	}
	// 单线程TCP服务器select网络模型
	void func_selectTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
// #ifndef FD_SETSIZE
// #define FD_SETSIZE      64
// #endif /* FD_SETSIZE */
// 
// typedef struct fd_set {
//         u_int   fd_count;               /* how many are SET? */
//         SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
// } fd_set;
// 
// 		int PASCAL FAR select(
// 			__in int nfds,
// 			__inout_opt fd_set FAR *readfds,
// 			__inout_opt fd_set FAR *writefds,
// 			__inout_opt fd_set FAR *exceptfds,
// 			__in_opt  const struct timeval FAR *timeout);

		// 设置
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;  // htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET;			// IPv4
		addrServer.sin_port = sin_port;			// htons(12345);  // 字节序转换
		addrServer.sin_zero;						// 结构体补齐字节，不错处理
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szMsg[MAX_PATH] = {};
		// 执行
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // 获取系统分配的SOCKET接口ID
		::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // 将IP与端口绑定到SOKCRT接口ID
		listen(sockServer, SOMAXCONN);  // 监听IP:端口

		// 简单socket
		for (;0;) {
			SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
			cout << "sockClient:" << sockClient << " Error Code:" << GetLastError() << endl;
			recv(sockClient, szMsg, MAX_PATH, 0);
			sprintf(szMsg, "hello client");
			send(sockClient, szMsg, strlen(szMsg) + 1, 0);
			closesocket(sockClient);
			break;
		}

		// 单线程select网络模型
		for (;;) {
			// 用于保存连接ID和地址信息的结构体
			struct SockInfo {
				SOCKET sock = 0;
				SOCKADDR_IN addr = {};
				SockInfo(SOCKET sockClient = 0, sockaddr_in addrClient = {}) 
					:sock(sockClient), addr(addrClient) { ; }
			};
			std::vector<SockInfo> vecClient;  // 客户端链接
			fd_set readfds;  // 服务端仅关注可读事件
			while (true) {
				// 初始化队列
				FD_ZERO(&readfds);
				// 加入服务端socket，以便有客户端连接时触发accept
				FD_SET(sockServer, &readfds);  
				// 加入客户端socket，以便客户端发来消息时触发
				for (auto var : vecClient) { FD_SET(var.sock, &readfds); }  
				// 服务端socket的select只需关注是否有可读事件到来，因为接入的客户端总是可写的
				// 客户端的connect和send会触发服务端的可读事件，以便服务端调用accept和recv
				// select的timeval参数: null:阻塞; 0:立即返回; >0:设置超时时间  
				// select的timeval取值含义与WSAPoll不同
				int iSelectRtn = select(sockServer + 1, &readfds, NULL, NULL, NULL);// &tv);
				if (iSelectRtn == 0) { continue; }   // 超时
				else if (iSelectRtn < 0) { break; }  // 错误
				// 检查服务端socket是否可读  // 新客户端接入时会触发
				if (FD_ISSET(sockServer, &readfds)) {
					FD_CLR(sockServer, &readfds);
					SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
					std::cout << "new client socket(" << sockClient << ") form "
						<< inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port
						<< " ErrorCode:" << GetLastError() << std::endl;
					if (sockClient == INVALID_SOCKET) { continue; }  // while(true){continue;}
					vecClient.push_back(SockInfo(sockClient,addrClient));
				}
				// 检查客户端连接是否可读  // 客户端发来消息时会触发可读事件
				for (auto var : vecClient) {
					if (FD_ISSET(var.sock, &readfds)) {
						FD_CLR(var.sock, &readfds);
						memset(szMsg, 0, MAX_PATH);
						int iRecvRtn = recv(var.sock, szMsg, MAX_PATH, 0);
						std::cout << "socket(" << var.sock << ")"
							<< " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
							<< " recv msg(" << strlen(szMsg) << "/" << MAX_PATH << "):[" << szMsg << "]"
							<< " ErrorCode:" << GetLastError() << std::endl;
						if (iRecvRtn == 0) {  // 客户端断开连接
							closesocket(var.sock);
							std::cout << "socket(" << var.sock << ")"
								<< " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
								<< " 已断开连接" << " ErrorCode:" << GetLastError() << std::endl;
							// 删除已断开客户端的连接信息
							vector<SockInfo>::iterator ite = vecClient.begin();
							for (; ite != vecClient.end(); ++ite) { 
								if (ite->sock = var.sock) {
									vecClient.erase(ite);
									break;  // 终止iterator的for循环
								}
							}
							break;  // 终止FD_ISSET的for循环
						}
						// 客户端总是可写的，因此可以在服务端处理完发来的消息后立即返回
						sprintf(szMsg, "hello! client socket %d form %s:%d",
							var.sock, inet_ntoa(var.addr.sin_addr), var.addr.sin_port);
						int iSendRtn = send(var.sock, szMsg, strlen(szMsg), 0);
						std::cout << "socket(" << var.sock << ")"
							<< " to   " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
							<< " send msg(" << strlen(szMsg) << "/" << iSendRtn << "):[" << szMsg << "]"
							<< " ErrorCode:" << GetLastError() << std::endl;
					}
				}
			}
		}
		
		WSACleanup();
	}
	
	// 单线程TCP服务器poll网络模型
	void func_pollTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
#if(_WIN32_WINNT < 0x0600)
		ColorLine("系统版本过低，不支持poll函数", TCOLOR_NOTE);
		return;
#endif // (_WIN32_WINNT < 0x0600)
		ColorLine("// Windows 上与 poll 类似的函数为 WSAPoll", TCOLOR_NOTE);

// #if(_WIN32_WINNT >= 0x0600)
// #if INCL_WINSOCK_API_PROTOTYPES
// WINSOCK_API_LINKAGE
// int
// WSAAPI
// WSAPoll(
//     _Inout_ LPWSAPOLLFD fdArray,
//     _In_ ULONG fds,
//     _In_ INT timeout
//     );
// #endif /* INCL_WINSOCK_API_PROTOTYPES */
// #endif // (_WIN32_WINNT >= 0x0600)

		// 设置
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;  // htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET;			// IPv4
		addrServer.sin_port = sin_port;			// htons(12345);  // 字节序转换
		addrServer.sin_zero;						// 结构体补齐字节，不错处理
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szRecv[MAX_PATH] = {};
		char szSend[MAX_PATH] = {};
		// 执行
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // 获取系统分配的SOCKET接口ID
		::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // 将IP与端口绑定到SOKCRT接口ID
		listen(sockServer, SOMAXCONN);  // 监听IP:端口

		// poll 模型  // Windows没有poll，只有WSAPoll，且要求系统版本在XP以上
		for (;;) {
			WSAPOLLFD fdArray[128] = {};
			ULONG fds = 0;
			INT timeout = -1;
			// 添加服务器socket
			fdArray[0].fd = sockServer;
			fdArray[0].events = POLLIN;
			fds += 1;

			while (true) {
				int iWSAPollRtn = WSAPoll(fdArray, fds, timeout);  // timeout<0:阻塞，0立即返回
				cout << "iWSAPollRtn:" << iWSAPollRtn << " count:" << fds << endl;
				if (iWSAPollRtn == 0) { continue; }
				else if (iWSAPollRtn < 0) { break; }
				// 查询
				for (ULONG i = 0; i < fds; i++) {
					cout << "fdArray[" << i + 1 << "/" << fds << "]" << " fd:" << fdArray[i].fd
						<< " event:" << (fdArray[i].events == POLLIN ? "POLLIN" : "")
						<< (fdArray[i].events == POLLHUP ? "POLLHUP" : "")
						<< (fdArray[i].events == POLLERR ? "POLLERR" : "")
						<< " revents:" << fdArray[i].revents << endl;
					// 新加入客户端后，新加入的客户端不能参与本轮循环
					if ((fdArray[i].fd == sockServer) && (fdArray[i].revents & POLLIN)) {
						SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
						cout << "accept sock " << sockClient << " form " << inet_ntoa(addrClient.sin_addr)
							<< ":" << addrClient.sin_port << endl;
						fdArray[fds].fd = sockClient;
						fdArray[fds].events = POLLIN;
						fds++;
						break;
					}
					else if ((fdArray[i].fd != sockServer) && (fdArray[i].revents & POLLIN)) {
						int iRectRtn = recv(fdArray[i].fd, szRecv, MAX_PATH, 0);
						cout << "recv form sock " << fdArray[i].fd
							<< " Msg(" << iRectRtn << "/" << MAX_PATH << "):"
							<< "[" << szRecv << "]" << endl;
						// 断开连接
						if (iRectRtn <= 0) {
							closesocket(fdArray[i].fd);
							memset(&fdArray[i], 0, sizeof(fdArray[i]));
							fds--;
							break;
						}
						sprintf(szSend, "server recv form %d msg:[%s]", fdArray[i].fd, szRecv);
						int iSendRtn = send(fdArray[i].fd, szSend, strlen(szSend), 0);
						cout << "send to   sock " << fdArray[i].fd << " Msg(" << iSendRtn << "/" << strlen(szSend) << "):"
							<< "[" << szSend << "]" << endl;
						break;
					}
					// 去除空连接
					for (ULONG i = 0; i < fds; i++) {
						if (fdArray[i].fd == 0 && fdArray[i].events == 0 && fdArray[i].revents == 0) {
							for (ULONG j = i + 1; j < fds; j++) {
								if (fdArray[j].fd && fdArray[j].events) {
									memcpy(&fdArray[i], &fdArray[j], sizeof(fdArray[i]));
									//memmove(&fdArray[i], &fdArray[j], sizeof(fdArray[i]));
									memset(&fdArray[j], 0, sizeof(fdArray[j]));
									break;
								}
							}
						}
					}

					// 没有客户端，只有一个服务器时退出
					if (fds == 1) {
						closesocket(sockServer);
						break;
					}
				}
			}
			break;
		}
		WSACleanup();
	}

	// epoll 网络模型
	void func_epollTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
		ColorLine("// Windows 没有类似于 epoll 的函数", TCOLOR_NOTE);
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		COUTLINE_FUNC_INFO("::bind与std::bind可能混淆，导致accept失败，应在bind前加作用域限定符");
		// 不再发送字符串后面的'\0'
		//func_simpleTCPServer();
		//func_simpleTCPClient();
		//func_selectTCPServer();
		return 0;
	}
}

// std::copy()/std::move()  // 左值引用、右值引用
namespace ns_copymove {
	// 左值与右值:
	// 左值是指表达式结束后依然存在的持久化对象，右值是指表达式结束时就不再存在的临时对象。
	// 所有的具名变量或者对象都是左值，而右值不具名。
	// 
	// 左值引用与右值引用(&&,int&& j = i++;)：
	// 右值引用，用以引用一个右值，可以延长右值的生命期。"abc",123等都是右值。
	// 可以通过右值引用，充分使用临时变量，或右值资源，减少不必要的拷贝，提高效率.
	int main() {
		COUTLINE_FUNC_MAIN;
		//std::copy();
		return 0;
	}
}

// 设计模式
namespace ns_designPattern {
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
};

//线程任务执行器  // std::map 无法存入 LPVOID
namespace ns_threadTask {
	class ThreadTask {
	public:
		ThreadTask() {
			//std::map<std::function<LPVOID>, LPVOID>mapFunc;  // 函数指针及函数参数指针
			//std::vector<std::map<std::function<LPVOID>, LPVOID>>taskList;  // 任务列表
			//static unique_ptr<std::vector<std::map<std::function<LPVOID>, LPVOID>>>g_task; // 全局只有一个任务列表
			////
			//auto lambda = [](LPVOID lpParam)->void {};
			//std::function<void(LPVOID)> f = lambda;
			////mapFunc.insert(f, NULL);
		}
		~ThreadTask() {}
		//auto getInstance() {
		//	// 全局句柄没有则进行保存后返回，有则返回保存的值
		//	// 检查句柄时应在读写该句柄前进行锁定
		//	if (!g_instance) { g_instance = make_shared<ThreadTask*>(this); }
		//	if (g_instance) { return g_instance; }
		//}
		//void start() {
		//	// 无限循环不能退出？？
		//	for (;;) {
		//		//m_thread = CreateThread()
		//		WaitForSingleObject(m_mutex, INFINITE);
		//		//auto task = m_task.get();
		//		//task(NULL);
		//	}
		//}
		//void pushTask(std::function<LPVOID> task) {
		//	m_task = make_unique<std::function<LPVOID>>(task);
		//	m_mutex = CreateMutexEx(NULL, L"myThread", 0, 0);
		//}

	private:
		//static std::shared_ptr<ThreadTask*> g_instance;  // 用于保存全局唯一对象的句柄
		//// 类对象为全局统一，故而需要使用容器保存任务列表
		//// 容器、智能指针、任务函数
		//// 容器    ：std::vector<>          // vector便于取用
		//// 智能指针：std::unique_ptr<>      // 一个任务只会在一个线程中执行一次
		//// 任务函数：std::function<LPVOID>  // LPVOID便于函数调用
		//// 联合指针存容器，容器存任务列表，任务列表为映射，映射保存
		//std::unique_ptr<std::function<LPVOID>> m_task; // 指向任务函数的联合指针
		//std::unique_ptr<HANDLE>m_thread;  // 线程函数
		//HANDLE m_mutex = NULL;
	};
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
}

// threadPool 线程池
namespace ns_threadPool {
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
}

//  主名字空间
#include <istream>
int
main(
	_In_ int _Argc,
	_In_count_(_Argc) _Pre_z_ char ** _Argv,
	_In_z_ char ** _Env
) {
	// 判断启动模式
	bool bSimple = false;
	for (int i = 0; i < _Argc; i++) {
		if (strcmp(_Argv[i], "--onlyStartInfo") == 0) {
			bSimple = true;
			break;
		}
	}
	// 执行程序
	if (bSimple) {
		ColorLine("**** New Process Begin ****", TCOLOR_FUNC);
		ns_info::main(_Argc, _Argv, _Env);
		ColorLine("**** New Process Over ****", TCOLOR_FUNC);
	}
	else {
		COLORCONSOLE::main();
		ns_info::main(_Argc, _Argv, _Env);
		ns_constructFunction::main();	// 构造析构
		ns_pointerCast::main();			// 类型转换
		ns_smartPointer::main();		// 智能指针
		ns_vector::main();				// STL容器
		ns_functionBind::main();		// 函数绑定
		ns_exception::main();			// 异常捕获
		ns_processThread::main();		// 创建进程部分未打开
		ns_mutex::main();				// 锁

		ColorLine("// 网络部分由于阻塞，默认不执行，需要手动打开注释", TCOLOR_NOTE);
		//ns_socket::main();
		//ns_socket::func_simpleTCPServer();
		//ns_socket::func_selectTCPServer();
		//ns_socket::func_pollTCPServer();
	}
	system("pause");
	return 0;
}