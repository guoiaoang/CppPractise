// ���̡߳��̳߳�
// �첽I/O��I/O���á�IOCP���������  // �ص�I/O��
// seletc��poll��epoll���¼�ģ��.��Ϣģ��.�Լ�IOCPģ��
// ͬ�������߳�������д���� // �ź�����...
// 
// 
// �麯�������麯��������������������������������̬����&��ֹĳЩ��Դû���ͷ�
// ����ָ�롢STD������
// select��epoll�ȶ�·I/O����
// �����ݡ��߲��������̡߳��ֲ�ʽ
// �̳߳� ����
// ���̼�ͨ�š��̼߳�ͨ�š���Դ����
// Redis�ֲ�ʽ�����������Boost��STL
// 
//
// �㷨�����ģʽ
// 

#include <iostream>    // std::cout\std::endl\...
// <WinSock2.h>����<Windows.h>  
// <Windows.h>����<WinSock.h>,�����<WinSock2.h>�ᵼ�¶���ض���
#include <WinSock2.h>  // WSAPoll 
//#include <Windows.h>   // GetStdHandle
namespace COLORCONSOLE {};  // ��ʵ�ֵ����ֿռ�����
using namespace COLORCONSOLE;
using namespace std;
#pragma warning(disable:4996)  // 'strcpy': This function or variable may be unsafe.

// ���Ŀ���̨��ɫ���������
namespace COLORCONSOLE {
	// print "\033[31;4mRed Underline Text\033[0m"
	// echo -e "\033[31mRed Text\033[0m"
#include <Windows.h>
// ��ɫ���䶨��
#define FOREGROUND_YELLOW (FOREGROUND_RED   | FOREGROUND_GREEN)  // ��ɫ
#define FOREGROUND_PURPLE (FOREGROUND_RED   | FOREGROUND_BLUE )  // ��ɫ
#define FOREGROUND_CYAN   (FOREGROUND_GREEN | FOREGROUND_BLUE )  // ��ɫ
#define FOREGROUND_WHITE  (FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_RED)  // ��ɫ
// text color �ı���ɫ
// [��ɫԼ��]
// namespace ���ֿռ� (��ɫ)
//     ���� main() function start ��������ʼ (��ɫ)
//         ���� paragraph ���� (��ɫ)
//             ���� name:value ��ֵ�� (��ɫ|��ɫ)
//                   ���� something һ������(��ɫ)  // note �ʼ�(��ɫ)
#define TCOLOR_NS		FOREGROUND_YELLOW	// namespace ���ֿռ�	����ɫ
#define TCOLOR_FUNC		FOREGROUND_RED		// main		 ��������ʼ ����ɫ
#define TCOLOR_PARA		FOREGROUND_GREEN	// paragraph ����		����ɫ
#define TCOLOR_KEY		FOREGROUND_CYAN		// key		(��ֵ��)ֵ	����ɫ
#define TCOLOR_VALUE	FOREGROUND_BLUE		// value	(��ֵ��)��	����ɫ
#define TCOLOR_NORMAL	FOREGROUND_WHITE	// normal	��������	����ɫ
#define TCOLOR_NOTE		FOREGROUND_PURPLE	// note		ע��		����ɫ
// ����ж���
#define COUTLINE_FUNC_MAIN	cout << endl << "Func " << ColorText(__FUNCSIG__, TCOLOR_FUNC) << " start" << endl;
#define COUTLINE_FUNC_PARA	ColorLine(__FUNCSIG__);			// �½ڿ�ʼʱ�����
#define COUTLINE_FUNC_INFO	cout << __FUNCSIG__ << endl;	// ִ�й����е����

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
		// ��ɫ�����ʾ��  // ������һ��ʹ��ԭʼ��ɫ���
		cout << ColorText("RED", FOREGROUND_RED) << ColorText("GREEN", FOREGROUND_GREEN) << ColorText("BLUE", FOREGROUND_BLUE)
			<< ColorText("YELLOW", FOREGROUND_YELLOW) << ColorText("PURPLE", FOREGROUND_PURPLE) << ColorText("CYAN", FOREGROUND_CYAN)
			<< ColorText("WHITE", FOREGROUND_WHITE) << endl;
		// �����ɫ��ʽ��Ϣ
		cout << ColorText("[", TCOLOR_NORMAL) << ColorText("��ɫԼ��", TCOLOR_KEY) << ColorText("]", TCOLOR_NORMAL) << endl;
		cout << ColorText("namespace ���ֿռ�", TCOLOR_NS) << endl;
		cout << "    ���� "
			<< ColorText("main() function", TCOLOR_FUNC) << ColorText(" start ", TCOLOR_NORMAL)
			<< ColorText("������", TCOLOR_FUNC) << ColorText("��ʼ", TCOLOR_NORMAL)
			<< endl;
		cout << "        ���� " << ColorText("paragraph ����", TCOLOR_PARA) << endl;
		cout << "            ���� "
			<< ColorText("key", TCOLOR_KEY) << ColorText(":", TCOLOR_NORMAL) << ColorText("value", TCOLOR_VALUE)
			<< ColorText(" ", TCOLOR_NORMAL)
			<< ColorText("��", TCOLOR_KEY) << ColorText("ֵ", TCOLOR_VALUE) << ColorText("��", TCOLOR_NORMAL)
			<< endl;
		cout << "                  ���� "
			<< ColorText("something һ������", TCOLOR_NORMAL) << ColorText("  // note �ʼ�", TCOLOR_NOTE)
			<< endl;
		// ������ɫ���ý�ɫ����
		cout << "example:" << endl;
		cout << "__FILE__   :[" << ColorText(__FILE__, TCOLOR_FUNC) << "]" << endl;
		cout << "__FUNCSIG__:[" << ColorText(__FUNCSIG__) << "]" << endl;
		return 0;
	}
}

// ����ļ���Ϣ��������������������
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
		// ������������������
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

// ����ѭ�����ж�
namespace ns_ifloop {
	void func_if() {
		COUTLINE_FUNC_PARA;
		// �Ƿ�ִ��
		if (true) {
			;
		}
		int a = 1;
		// ��������ִ��...;����������ִ��...
		if (a == 1) {
			;
		}
		else {
			;
		}
		// ����Ϊ...ʱִ��...;��������Ϊ...ʱִ��...
		if (a == 2) {
			;
		}
		else if (a == 1)
		{
		}
		// ����Ϊ...ʱִ��...;��������Ϊ...ʱִ��...;������������ִ��...
		if (a == 2) {
			;
		}
		else if (a == 1)
		{
		}
		else {
			;
		}
		// �������
		switch (a) {
		case 2:
		case 1:
			break;
		case 0:
			break;
		default:
			break;
		}
		// ��Ŀ���ʽ
		int b = a == 1 ? a + 2 : 3;
		// �ж�����
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
		// ��������
		for (int i = 0; i < 5; i++) {
			;
		}
		// �����Լ�
		for (int i = 5; i >= 0; --i) {
			;
		}
		// ���������ѭ��  // Ҳ�������for�������е�����λ��
		for (;;) {
			break;  // ����ѭ��
		}
		// Ƕ��ѭ�� + �����˳�
		for (int i = 0; i < 6; i++) {
			for (int j = 7; j >= 3; j--) {
				if (j <= i) {
					break;
				}
			}
		}
		// ֻ��"for (; 0;)"������ѭ����������Ϊ����ѭ��
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
		// while(...) do{...} ���
		while (true) {
			break;
		}
		// do{...} while(...)���
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

// ���졢�������麯��
namespace ns_constructFunction {
	// ���캯����
	// Ĭ�Ϲ���
	// ��ֵ����
	// ��������   // ���Ʋ�����
	// �ƶ�����   // ��ǰ�ำֵ�󽫲������ÿ�
	// ��������
	// �麯��     // 
	// ���麯��
	// ���������� // 
	// ������������
	// func() = delete;  //
	// func() = 0;

	class BaseA {
	public:
		BaseA() { COUTLINE_FUNC_INFO }        // Ĭ�Ϲ��캯��  // Ĭ�Ϲ��������̬�����������"virtual"
		BaseA(int a) { COUTLINE_FUNC_INFO }   // Ĭ�Ϲ��캯���Ķ�̬
		BaseA(char*a) { COUTLINE_FUNC_INFO }  // Ĭ�Ϲ��캯���Ķ�̬
		~BaseA() { COUTLINE_FUNC_INFO }       // ��������
		void func() = delete;  // "= delete;" ��ֹ�̳С��������� // C++11������
	};
	class BaseB {
	public:
		virtual ~BaseB() { COUTLINE_FUNC_INFO }           // "virtual ~ClassName(){...}" :��������������ֹ������Դδ�ͷ�
		virtual void func() = 0; // "virtual ret function() = 0":���麯��,����ֱ��ʹ�ø��࣬�����������д�ú���
	};
	class BaseC {
	public:
		// "virtual ~ClassName() = 0;"��������������
		// �����޷���д����Ҫ���������ִ���壬����ᱨ�Ҳ�������ִ����
		virtual ~BaseC() = 0;
		void func() { COUTLINE_FUNC_INFO }     // ��ͨ����
	};
	inline BaseC::~BaseC() { COUTLINE_FUNC_INFO }  // �����дʵ��

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
		//BaseB b;  // "virtual void B::func() = 0",����ʵ����������
		//BaseC c;  // "virtual C:~C() = 0",����ʵ����������
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

// ���� vector/list/map/deque  // ����������д
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
		vVec2.pop_back();  // ȥβ
		cout << "vVec1: ";
		for (std::vector<int>::iterator ite = vVec2.begin(); ite != vVec2.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
	}
	void func_list() {
		COUTLINE_FUNC_PARA;
		// ��ʼ��
		std::list<int> vList1 = { 4567,3456,1234 };
		vList1.push_front(6789);
		vList1.push_back(5678);
		cout << "vList1 init: ";
		for (auto ite = vList1.begin(); ite != vList1.end(); ++ite) {
			cout << *ite << " ";
		}
		cout << endl;
		// ����
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
		// std::map<>()û��pop_back()\push_back\...����

		// ��ʼ������������
		std::map<string, int> vMap = { make_pair("first",1234), make_pair("second",2345) };
		vMap.insert(make_pair("third", 3456));
		vMap["last"] = 4567;
		// �������
		cout << "vMap Size:" << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// ���Ҽ�ֵ��
		auto f = vMap.find("second");
		cout << "vMap find(\"second\") = " << f->second << endl;

		// ����ָ���ĵ�����
		vMap.erase(f);
		cout << "vMap Size: erase(iterator) " << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// ����ָ�����ļ�ֵ��
		vMap.erase("third");
		cout << "vMap Size: erase(\"third\")  " << vMap.size() << " ";
		for (std::map<string, int>::iterator ite = vMap.begin(); ite != vMap.end(); ++ite) {
			cout << "[" << ite->first.c_str() << ":" << ite->second << "] ";
		}
		cout << endl;

		// �ÿ�
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

// ����ת��  // dynamic_cast��static_cast��const_cast��reinterpret_cast 
#include <functional>  // std::function  // ���������ֿռ��а���
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
		// ��̬ת��:dynamic_cast<Type>(p),����������ж�  
		// ������ת��������
		Base *db = dynamic_cast<Base *>(pc);
		if (NULL != db) {
			db->func();
		}

		// ����ת���������
		Child *dc = dynamic_cast<Child *>(pb);
		if (!dc) {
			cout << "dynamic_cast<Child *>(pb) error!" << endl;
		}
	}
	void func_static_cast() {
		COUTLINE_FUNC_PARA;
		// ��̬����ת����static_cast<Type>(p)
		// �൱��C�����е�ǿ��ת����int a = (int)12.5f;
		double d = 12.5f;
		int i = static_cast<int>(d);
		cout << "d:" << d << " i:" << i << endl;
	}
	void func_const_cast() {
		COUTLINE_FUNC_PARA;
		// ֻ������ת����ȥ�����͵�const��volatile����
		const int a = 123;
		int &b = const_cast<int&>(a);
		b = 124;
		cout << "a:" << a << " b:" << b << endl;
	}
	void func_reinterpret_cast() {
		COUTLINE_FUNC_PARA;
		// ָ������ת����reinterpret_cast<Type>(p)
		auto lambda = [](int value) { cout << __FUNCSIG__ << " value:" << value << endl; };
		int *iFunc = reinterpret_cast<int*>(&lambda);
		cout << "lambda:" << &lambda << " iFunc:" << iFunc << endl;
		//std::function<void(int)> rFunc = reinterpret_cast<void(int)>(iFunc);
		//rFunc(123);

		auto fFunc = lambda; // �������ã������ַ��ͬ
		cout << "fFunc:" << &fFunc << " lambda:" << &lambda << endl;

		//
		void(*pFunc)(int);  // ��������ָ��
		pFunc = lambda;     // ����ָ�븳ֵ
		pFunc(123);         // ���ú���ָ��ָ��ĺ���  // void __cdecl PointerCast::funcName(int)
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

// ����ָ��
#include <memory>  // ptr
namespace ns_smartPointer {
	// ����ʵ��C++11����ָ��
	// https://blog.csdn.net/code_peak/article/details/119722167
	// 
	// auto_ptr   : �Զ���c++11����unique_prt���档(��������ڴ�й©��������ʹ��)
	// unique_ptr : Ψһ��c++11
	// shared_ptr : ����c++11
	// weak_ptr   : ��  ��c++11
	//
	// �ؼ�֪ʶ��
	// ��ʼ��		����ֵ���졢�������졢std::make_shared()��std::make_unique()
	// ��������		��std::move()��std::xx_ptr.reset()��unique_ptr<Class,deletor>ptr()�Զ������ٺ���
	// ֻ�ͷŲ�����	��std::auto_ptr.release()
	// ��ȡ����		��std::xx_ptr.get()
	// ����			��std::shared_ptr.use_count()
	class PointerBase {
	public:
		PointerBase(int val = 1122) :m_val(val) { cout << __FUNCSIG__ << " m_val:" << m_val << endl; }
		~PointerBase() { cout << __FUNCSIG__ << " m_val:" << m_val << endl; }
		int value() { return m_val; }
	private:
		int m_val;
	};

	// auto_ptr:ֻ��һ��ָ�룬��ֵʱ�ᱻ�ƶ�  // ���Ƽ�
	void func_autoptr() {
		COUTLINE_FUNC_PARA;
		// ��ʼ��
		std::auto_ptr<int> ap1(new int(8));
		std::auto_ptr<int> ap2;
		ap2.reset(new int(16));  // �����ڴ�
		cout << "ap1:" << *ap1.get() << endl;
		cout << "ap2:" << *ap2.get() << endl;
		cout << "release before : ap1 is " << (ap1.get() != NULL ? "not " : "") << "empty " << endl;
		ap1.release();  // �ͷ��ڴ�
		cout << "release after  : ap1 is " << (ap1.get() != NULL ? "not " : "") << "empty " << endl;

		// ������ֵ����ʱ�ᵼ��ָ��ת�ƣ��Ӷ������ڴ�й©
		//���Կ�������
		std::auto_ptr<int> ap3(new int(8));
		std::auto_ptr<int> ap4(ap3);
		cout << "ap3 is " << (ap3.get() != NULL ? "not " : "") << "empty " << endl;  // ap1 is empty
		cout << "ap4 is " << (ap4.get() != NULL ? "not " : "") << "empty " << endl;  // ap2 is not empty
		//���Ը�ֵ����
		std::auto_ptr<int> ap5(new int(8));
		std::auto_ptr<int> ap6 = ap5;
		cout << "ap5 is " << (ap5.get() != NULL ? "not " : "") << "empty " << endl;  // ap3 is empty
		cout << "ap6 is " << (ap6.get() != NULL ? "not " : "") << "empty " << endl;  // ap4 is not empty
	}

	// unique_ptr:ֻ��һ�������ȡ��ָ�� // ����
	void func_uniqueptr() {
#define COUTLINE_UNIQUEPTR_GET(VAL)		cout << ""#VAL"(" << VAL.get() << "): " << *VAL.get() << endl;
#define COUTLINE_UNIQUEPTR_ISEMPTY(VAL)		cout << ""#VAL"(" << VAL.get() << "): is " << (VAL.get() != NULL ? "not " : "") << "empty " << endl;
#define COUTLINE_UNIQUEPTR_PTRGET(STR,VAL)	cout << STR#VAL"(" << VAL.get() << "):" << VAL->value() << endl;
		// 
		COUTLINE_FUNC_PARA;
		// ��ʼ��
		unique_ptr<int> up1(new int(123));
		unique_ptr<int> up2;
		up2.reset(new int(234));
		unique_ptr<int> up3 = make_unique<int>(345);  // ����ĳ�ʼ������ // -std=c++14
		COUTLINE_UNIQUEPTR_GET(up1);
		COUTLINE_UNIQUEPTR_GET(up2);
		COUTLINE_UNIQUEPTR_GET(up3);

		// �ƶ�����  // ���ڴ�ת��  // ִ�к�ԭ�ڴ�ָ��ָ���
		unique_ptr<int> up4(make_unique<int>(456));
		COUTLINE_UNIQUEPTR_GET(up4);
		COUTLINE_UNIQUEPTR_ISEMPTY(up4);
		unique_ptr<int> up5(move(up4));  // �����ƶ�
		COUTLINE_UNIQUEPTR_GET(up5);
		COUTLINE_UNIQUEPTR_ISEMPTY(up4);
		COUTLINE_UNIQUEPTR_ISEMPTY(up5);
		unique_ptr<int> up6 = move(up5);  // ��ֵ�ƶ�
		COUTLINE_UNIQUEPTR_GET(up6);
		COUTLINE_UNIQUEPTR_ISEMPTY(up5);
		COUTLINE_UNIQUEPTR_ISEMPTY(up6);

		// ��ֵ��������
		unique_ptr<int> up7(make_unique<int>(567));;
		// std::unique_ptr<int> up8(up7);		// ��ֹ�������죬�޷�����ͨ�� // ����������ɾ���ĺ���
		// std::unique_ptr<int> up9; up9 = up7;	// ��ֹ��ֵ���죬�޷�����ͨ�� // ����������ɾ���ĺ���
		auto lambda = [](int val)->unique_ptr<int> { unique_ptr<int> up(new int(val)); return up; };
		unique_ptr<int> up10 = lambda(789);		// ͨ����������ֵ��ֵ  // ����ֵΪ��ֵ�����unique_ptr���䱣�棿����Ϊ��ʹmove()������Ч����
		COUTLINE_UNIQUEPTR_GET(up6);

		// һ��Ѷ���
		std::unique_ptr<int[]> upa1(new int[10]);
		std::unique_ptr<int[]> upa2;
		upa2.reset(new int[10]);
		std::unique_ptr<int[]> upa3(std::make_unique<int[]>(10));

		// ��Դ�ͷ�  // reset()�ͷ�ָ�����Դ��release()ֻ�ͷ�ָ�벻�ͷ���Դ
		ColorLine("// ��Դ�ͷţ�reset()�ͷ�ָ�����Դ��release()ֻ�ͷ�ָ�벻�ͷ���Դ", TCOLOR_NOTE);
		std::unique_ptr<PointerBase> upb1(new PointerBase(1234));
		std::unique_ptr<PointerBase> upb2(new PointerBase(2345));
		COUTLINE_UNIQUEPTR_PTRGET("before swap : ", upb1);
		COUTLINE_UNIQUEPTR_PTRGET("before swap : ", upb2);
		upb1.swap(upb2);  // ����ָ��
		COUTLINE_UNIQUEPTR_PTRGET("after  swap : ", upb1);
		COUTLINE_UNIQUEPTR_PTRGET("after  swap : ", upb2);
		// upb1.reset(upb2); // error
		upb1.reset(new PointerBase(3456)); // ���������浽��ǰָ����(Ĭ�ϵ�ǰָ���ÿ�)
		COUTLINE_UNIQUEPTR_PTRGET("after reset(3456) : ", upb1);
		upb1.reset();
		cout << "after reset(): upb1(" << upb1.get() << "): is " << (upb1.get() != NULL ? "not " : "") << "empty " << endl;
		upb2.release();  // ָ���ÿյ����ͷ���Դ(��Ϊû�е�����������)

		// ����Զ�����Դ�ͷź���  // ����->�Զ���lambda�ͷź���->lambda������ִ��delete��������
		ColorLine("// ����Զ�����Դ�ͷź���", TCOLOR_NOTE);
		auto deletor = [](PointerBase *pMySocket) { COUTLINE_FUNC_INFO; delete pMySocket; };
		//unique_ptr<mySocket, void(*)(mySocket * pMySocket)> ups1(new mySocket(4567), deletor);
		unique_ptr<PointerBase, decltype(deletor)> upd1(new PointerBase(4567), deletor);  // decltype(T):�Զ��Ƶ�����
		upd1.reset();  // ���������ͷ��Ա㼰ʱ�����Ϣ

		cout << __FUNCSIG__ << " : end" << endl;
#undef COUTLINE_UNIQUEPTR_GET
#undef COUTLINE_UNIQUEPTR_ISEMPTY
#undef COUTLINE_UNIQUEPTR_PTRGET
	}

	// shared_ptr������ͬһ��ָ�� // ����
	void func_shareptr() {
#define COUTLINE_SHAREPTR_INFO_END(XX) ""#XX" addr:" << XX << " use_count():" << XX.use_count() \
		<< " value:" << (XX.get() == NULL ? -1 : XX.get()->value()) << endl;
#define COUTLINE_SHAREPTR_INFO_1(XX) cout << "--" COUTLINE_SHAREPTR_INFO_END(XX)
#define COUTLINE_SHAREPTR_INFO_2(XX) cout << "  " COUTLINE_SHAREPTR_INFO_END(XX)
		//
		COUTLINE_FUNC_PARA;
		// ��ʼ��
		shared_ptr<int> sp1(new int(123));
		shared_ptr<int> sp2;
		sp2.reset(new int(234));
		shared_ptr<int> sp3 = make_shared<int>(345);
		// ���ü���
		shared_ptr<PointerBase> spb1(new PointerBase(1234));  // ��ʼ������Ϊ1
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():1 value:1234
		shared_ptr<PointerBase> spb2(spb1);			// ����+1=2
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():2 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb2);	//   spb2 addr:007E0D60 use_count():2 value:1234
		shared_ptr<PointerBase> spb3 = spb1;		// ����+1=3
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():3 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb3);	//   spb3 addr:007E0D60 use_count():3 value:1234
		shared_ptr<PointerBase> spb4 = move(spb3);	// �ƶ�spb3��ָ��ͼ�����spb4 // spb1=spb3=����Ϊ3
		COUTLINE_SHAREPTR_INFO_1(spb3);	// --spb3 addr:00000000 use_count():0 value:-1
		COUTLINE_SHAREPTR_INFO_2(spb4);	//   spb4 addr:007E0D60 use_count():3 value:1234
		spb4.reset();								// ����-1=2
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():2 value:1234
		COUTLINE_SHAREPTR_INFO_2(spb4);	//   spb4 addr:00000000 use_count():0 value:-1
		spb2.reset();								// ����-1=1
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:007E0D60 use_count():1 value:1234
		spb1.reset();	//����Ϊ0ʱִ������ // __thiscall SmartPointer::PointerBase::~PointerBase(void) m_val:1234
		COUTLINE_SHAREPTR_INFO_1(spb1);	// --spb1 addr:00000000 use_count():0 value:-1
#undef COUTLINE_SHAREPTR_INFO_END
#undef COUTLINE_SHAREPTR_INFO_1
#undef COUTLINE_SHAREPTR_INFO_2
	}

	// weak_ptr��// ����
	// 1.��������ָ���ѭ�����á�
	// 2.��shared_ptr��Դδ��ȫ�ͷ�ʱ�����»�ȡ��Դָ�룬Ȼ���ٽ��в��� 
	void func_weakptr() {
		COUTLINE_FUNC_PARA;
		// ��ʼ��
		std::shared_ptr<int> sp1(new int(123));	//	����һ��std::shared_ptr����
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		std::weak_ptr<int> wp2(sp1);			//ͨ�����캯���õ�һ��std::weak_ptr����
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		cout << "wp2 use_count(): " << wp2.use_count() << endl;
		std::weak_ptr<int> wp3 = sp1;			//ͨ����ֵ������õ�һ��std::weak_ptr����
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		std::weak_ptr<int> wp4 = wp2;			//ͨ��һ��std::weak_ptr����õ�����һ��std::weak_ptr����
		cout << "sp1 use_count(): " << sp1.use_count() << endl;
		// ���ü���
		//// tmpConn_ ��һ�� std::weak_ptr<TcpConnection> ����
		//// tmpConn_ ���õ�TcpConnection�Ѿ����٣�ֱ�ӷ���
		//if (tmpConn_.expired())
		//    return;
		//std::shared_ptr<TcpConnection> conn = tmpConn_.lock();
		//if (conn) {
		//    //��conn���в�����ʡ��...
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

// lambda ������� // std::bind()/std::palceholder::_1  // ����ָ��
namespace ns_functionBind {
	// C++ �е� Lambda ���ʽ
	// https://docs.microsoft.com/zh-cn/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160
	// C++11 ֮ std::function��std::bind��std::placeholders
	// https://blog.csdn.net/u014209688/article/details/90741985
	// Lambda ���ʽ��ʾ��
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
		// lambda ��������   // ����̫����ο������ĵ�
		auto lambda = [](int value)->char* {
			cout << __FUNCSIG__ << " value:" << value << endl; return __FUNCSIG__;
		};
		cout << lambda(456) << endl;
		int n = [](int x, int y) { return x + y; }(5, 4);
		cout << n << endl;
	}
	void func_stdFunction() {
		COUTLINE_FUNC_PARA;
		// ����ָ��
		// Ĭ�Ϻ���ֱ�ӵ���
		cout << ng_func() << endl;
		// ����ָ��
		char*(*pFunc)(int, char*, float);
		pFunc = ng_func;
		cout << pFunc(234, "pFunc", 2.34f) << endl;
		// std::function����ָ��
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
		// std::bind(������,�����,����)
		// std::placeholders::_1 // �ص�����
		// ��ͨ������������
		std::function<char*(int, char*, float)> fFunc2 =
			std::bind(&ng_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		cout << fFunc2(456, "fFunc2", 4.56f) << endl;
		// ������󡢵������ṩȱʡ����
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

// �쳣����  // ������
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
		cout << __FUNCSIG__ << " end" << endl;  //Ϊʲô catch ֮������ִ��??
		return __FUNCSIG__;
	}
	//
	void func_catch() {
		COUTLINE_FUNC_PARA;
		// ���
		try { cout << func_throw(1) << endl; }
		catch (...) { COUTLINE_CATCH_OTHER; }  // ��1���ᱻ����
		try { cout << endl << func_throw(0) << endl; }
		catch (char* &err) { COUTLINE_CATCH_TYPE(err); }
		try { cout << endl << func_throw(-1) << endl; }
		catch (int &err) { COUTLINE_CATCH_TYPE(err); }
		try { cout << endl << func_throw(-2) << endl; }
		catch (...) { COUTLINE_CATCH_OTHER; }

		// ����
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
			// warning C4290: ���� C++ �쳣�淶����ָʾ�������� __declspec(nothrow)
#pragma warning (disable:4290)
			// �Ʋ�ԭ�򣺵�ǰ��������throw(int)��֧�ֲ����ƣ��μ���
			// 1.���������棨�ȼ� 3��C4290
			// https://docs.microsoft.com/zh-cn/previous-versions/sa28fef8(v=vs.120)
			// 2.�쳣�淶 (������noexcept) (C++) --> �±��ܽ����쳣�淶�� Microsoft C++ ʵ��
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

// ���������߳�  // ������ʹ�� // ������
namespace ns_processThread {
	// std::this_thread::get_id()

	//
	struct ParamThread {
		// ע�⣺CreateThread����handle������������lpParamȡ��
		ParamThread* handle = NULL;  // ��ǰ�ṹ�����ĵ�ַ
		HANDLE   hMutex = NULL;
		HANDLE   hThread = NULL;
		// CreateThread�����������б�
		LPSECURITY_ATTRIBUTES lpSA = NULL;
		SIZE_T dwSize = 0;  // 0:�Զ�
		LPTHREAD_START_ROUTINE lpFunc = NULL;
		LPVOID lpParam = NULL;  // ע�⣺CreateThread����handle��������lpParam
		DWORD dwFlag = 0;  // 0:����
		DWORD dwTid = NULL;
	};
	DWORD WINAPI func_thread(LPVOID lpParam) {
		COUTLINE_FUNC_INFO;  // ����ʱ���������̣߳��ᵼ�������߳������׼���
		ParamThread *ppt = (ParamThread*)lpParam;
		WaitForSingleObject(ppt->hMutex, INFINITE);
		// ��Ҫ���̵߳ȴ����߳���ɺ����˳����Է�ֹ���߳��˳�ʱ�������̻߳��õ�����Դ
		cout << __FUNCSIG__ << " handle:" << ppt->hThread << " tid: " << ppt->dwTid
			<< " param: [" << (char*)ppt->lpParam << "]" << endl;
		ExitThread(1);  // �����˳��߳�
		cout << __FUNCSIG__ << " end" << endl;
		return 0;
	}
	void func_createThread1() {
		COUTLINE_FUNC_PARA;
		// �����߳���ز�����
		ParamThread pt;
		ParamThread *ppt = &pt;
		ppt->handle = ppt;
		ppt->hMutex = CreateMutex(NULL, TRUE, L"console output mutex");  // ���������� // TRUE:�������ȴ�
		ppt->lpFunc = func_thread;  // �̺߳���
		ppt->lpParam = "threadParam";// "--cmd=\"tree /F\" --dir=\"C:\\Program Files (x86)\"";
		ppt->dwFlag = CREATE_SUSPENDED; // 0:����������CREATE_SUSPENDED���ȴ�ResumeThread��
		//ppt->dwTid = 0;  // �߳�ID
		// �����߳�
		ppt->hThread = CreateThread(ppt->lpSA, ppt->dwSize,
			ppt->lpFunc, ppt->handle, ppt->dwFlag, &ppt->dwTid);  // �����������߳�
		cout << __FUNCSIG__ << " handle:" << ppt->hThread << " tid: " << ppt->dwTid \
			<< " param: [" << (char*)ppt->lpParam << "]" << endl;  // �߳���ֱ��ȡ������
		// �����߳�
		ResumeThread(ppt->hThread);
		// �߳�������Ҫʱ�䣬����ʱ�������������������������
		ReleaseMutex(ppt->hMutex);  // �ͷ�����Դ  // ���߳����źź�ż���ִ��
		// �˴���֪�����߳�ʲôʱ�������ϣ��������������
		WaitForSingleObject(pt.hThread, INFINITE); // �ȴ����߳��˳�
		cout << __FUNCSIG__ << " after WaitForSingleObject(thread)" << endl;
		CloseHandle(ppt->hMutex);  // �ͷ�����Դ
		cout << __FUNCSIG__ << " end" << endl;
	}
	// 
	struct ProcessCreate {
		BOOL bCreate = FALSE;
		//
		LPCWSTR lpApp = NULL;  // ����·����
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
		ppc->dwFlags = CREATE_NEW_CONSOLE;  // ���µĿ���̨���ڶ��Ǻϲ��������̴���
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
		// ��δ���ҵ�������´�����ִ��
		ColorLine("��δ���ҵ�������´�����ִ��", TCOLOR_NOTE);
		return;

		WCHAR szApp[MAX_PATH] = {};
		GetModuleFileName(NULL, szApp, MAX_PATH);
		// ShellExecute()
		// WinExec��LoadModule
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
		system("start cmd");  // ͨ������ϵͳ�Ŀ���ִ̨�����
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

// �����ź���  // Mutex/Semaphore
namespace ns_mutex {
	// std::lock_guard  // since C++17
	// std::lock_guard<std::mutex>
	// std::lock_guard<std::recursive_mutex>
	//
	// WakeAllConditionVariable  // ��������
	// InitializeCriticalSection //
	// InterlockedAdd(+) / InterlockedIncrement(-)  // ԭ����


	// ��
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
		// ����ʾ��
		for (; 0;) {
			HANDLE hMutex0 = CreateMutex(NULL, FALSE, __FILEW__);
			if (!hMutex0 || GetLastError()) {  // "hMutex0Ϊ��"��"hMutex0��Ϊ�յ�GetLastError��Ϊ0"
				cout << "CreateMutex() ERROR Handle:" << hMutex0 << " Code:" << GetLastError() << endl;
			} else {
				cout << "CreateMutex " << __FILEW__ << " OK" << endl;
				CloseHandle(hMutex0);
			}
			// ����У��  // ERROR_INVALID_HANDLE  // ERROR_ALREADY_EXISTS
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
			// Mutexs �������������ﲻ����� CloseHandle ����
		}
		// ����ִ��ʾ����
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
		};  // warning C4229: ʹ���˼�ʱ����: ���������ϵ����η�
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
	// �ź���
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
		BOOL bRelease = FALSE;  // �Ƿ�ɹ�
		LONG lRelease = 1;      // �ͷŸ���
		LONG lpPrev = 0;        // ����ǰ��������
		// WaitForSingleObject
		DWORD dwWait = 0;
		ParamSema(LPCWSTR lpName = NULL, LONG lMaximumCount = 1, LONG lInitialCount = 1)
			:lpName(lpName), lMax(lMaximumCount), lInit(lInitialCount){}
		~ParamSema() { if (hSema) CloseHandle(hSema); hSema = NULL; }
	} Sema;
	void func_semaphore() {
		// WINBASEAPI
		// BOOL  // �Ƿ�ִ�гɹ�
		// WINAPI
		// ReleaseSemaphore(
		// 	_In_ HANDLE hSemaphore,  // ���ľ��
		// 	_In_ LONG lReleaseCount,  // ��ǰ�ͷŸ���
		// 	_Out_opt_ LPLONG lpPreviousCount  // �Ѿ��ͷ��л�δʹ�õĸ���
		// );
		//
		// 1.CreateSemaphoreEx():
		//   dwDesiredAccess=SEMAPHORE_ALL_ACCESS,û��Ĭ��ֵ0
		// 2.ReleaseSemaphore():
		//   lpPreviousCount��ǰ����Ϊ֮ǰδʹ�õĸ���.
		//   lReleaseCount������������ܳ���CreateSemaphoreEx()�е�lMaximumCount
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
		Sema se(L"hSema1", 5, 1); // ����ʱ����һ�����õ��ź�
		Sema *pse = &se;
		pse->hSema = CreateSemaphore(pse->lpAttr, pse->lInit, pse->lMax, pse->lpName);
		if (!pse->hSema || GetLastError()) {
			COUTLINE_FUNC_SEMAPHORE_CREATE(" ERROR" << " Code:" << GetLastError());
			return;
		}
		COUTLINE_FUNC_SEMAPHORE_CREATE(" OK");
		// ReleaseSemaphore: ���ӿ�����

		COUTLINE_FUNC_SEMAPHORE_RELEASE(3);
		COUTLINE_FUNC_SEMAPHORE_RELEASE(1);
		// WaitForSingleObject: ���ٿ�����
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		// ��������maxʱʧ��(��Ч�ͷ�)
		COUTLINE_FUNC_SEMAPHORE_RELEASE(2);
		// ����ʹ��ʱ�ᵼ�¼���
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		COUTLINE_FUNC_SEMAPHORE_WAIT;
		//COUTLINE_FUNC_SEMAPHORE_WAIT; // ����ʹ�û���ּ���
		CloseHandle(pse->hSema);
		pse->hSema = NULL;
		//
		//
#undef COUTLINE_FUNC_SEMAPHORE_CREATE
#undef COUTLINE_FUNC_SEMAPHORE_RELEASE
#undef COUTLINE_FUNC_SEMAPHORE_WAIT
	}
	// �¼�
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
		BOOL bManual = FALSE;  // FALSE:��Ҫ�ֶ�����resetevent�ͷ��ź�
		BOOL bInit = FALSE;    // FALSE:��ʼ���ź�
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
		// SetEvent���һ���ź�
		Events ev(L"hEvent", TRUE, FALSE);
		Events*pev = &ev;
		pev->hEvent = CreateEvent(pev->lpAttr, pev->bManual, pev->bInit, pev->lpName);
		cout << "CreateEvent(" << pev->lpAttr << ", " << pev->bManual << ", " << pev->bInit << ", " 
			<< pev->lpName << ") --> " << pev->hEvent << " ErrorCode:" << GetLastError() << endl;
		pev->bSet = ResetEvent(pev->hEvent);  // �����ź�
		cout << "ResetEvent(" << pev->hEvent << ") --> " << pev->bSet << endl;
		pev->bSet = SetEvent(pev->hEvent);    // �ͷ��ź�
		cout << "SetEvent(" << pev->hEvent << ") --> " << pev->bSet << endl;
		pev->dwWait = WaitForSingleObject(pev->hEvent, INFINITE);
		cout << "WaitForSingleObject after" << endl;
	}
// 	// �ɵȴ����
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
		// ����������ʾ���μ�func_mutex();
		// һ��������Mutex\Semaphore
		// ����������Event
		ColorLine("// �����������ܻ᷵�ؾ���������ɹ���GetLastError����0�����������Ƿ���0��", TCOLOR_NOTE);
		ColorLine("// WaitForSingleObject() ����ʹ�ûᵼ������(INFINITEʱ���������������)", TCOLOR_NOTE);
		ColorLine("// WaitForMultipleObjects() ���ֻ���Ǵ����������صľ������ɵ�����", TCOLOR_NOTE);
		ColorLine("// ReleaseSemaphore() �ͷ���������Maxʱʧ��", TCOLOR_NOTE);
		ColorLine("// �����Ϊһ���ԣ������ظ�ʹ��", TCOLOR_NOTE);
		func_mutex();
		func_semaphore();
		func_event();
		return 0;
	}
}

// //////////////////////////////////////////////////////////////////////
// ����Ϊ�ѽ������֣�����Ϊδ��ɲ���
// //////////////////////////////////////////////////////////////////////

// socket
// <WinSock.h>�Ѿ����ذ�����<Windows.h>
//#include <WinSock.h>
//#include <WinSock2.h> // WSAPoll
#pragma comment(lib,"ws2_32.lib")
namespace ns_socket {
	// socket TCP/UDP �����/�ͻ���  
	// select\poll\epoll �����

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

	// ���߳�UDP��������������ģ��
	void func_simpleUDPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12344)) {
		COUTLINE_FUNC_PARA;
		SOCKET hSockSvr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (hSockSvr == INVALID_SOCKET) { return; }
		SOCKADDR_IN addrSvr = { };
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = sin_port;// htons(12344);
		addrSvr.sin_addr.S_un.S_addr = S_addr;// INADDR_ANY;
		::bind(hSockSvr, (SOCKADDR*)&addrSvr, sizeof(addrSvr));
		//ͨ��
		// 1 ���������Ƚ�������
		char szBuff[MAX_PATH] = { 0 };
		SOCKADDR_IN addrClient = { 0 };
		int nLen = sizeof(addrClient);
		int nRecv = recvfrom(hSockSvr, szBuff, MAX_PATH, 0, (SOCKADDR*)&addrClient, &nLen);
		printf("%s,%d\n", szBuff, nRecv);
		// 2 ��������
		sendto(hSockSvr, szBuff, strlen(szBuff), 0, (SOCKADDR*)&addrClient, sizeof(addrClient));

		closesocket(hSockSvr);
	}
	// ���߳�UDP�ͻ��˾�������ģ��
	void func_simpleUDPClient(ULONG S_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12344), char*szMsg = "Hello Server") {
		COUTLINE_FUNC_PARA;
		SOCKET hSockClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (hSockClient == INVALID_SOCKET) { return; }
		SOCKADDR_IN addrSvr = { };
		addrSvr.sin_family = AF_INET;
		addrSvr.sin_port = sin_port;// htons(12344);
		addrSvr.sin_addr.S_un.S_addr = S_addr;// inet_addr("127.0.0.1");
		// ͨ��
		// Լ���ͻ������ȷ�������
		char szSend[] = "Hello World";
		sendto(hSockClient, szSend, strlen(szSend), 0, (SOCKADDR*)&addrSvr, sizeof(addrSvr));
		//Ȼ���������
		char szBuff[MAX_PATH] = { 0 };
		int nLen = sizeof(addrSvr);
		int nRecv = recvfrom(hSockClient, szBuff, MAX_PATH, 0, (SOCKADDR*)&addrSvr, &nLen);
		printf("%s,%d\n", szBuff, nRecv);

		closesocket(hSockClient);
	}

	// ���߳�TCP�ͻ��˾�������ģ��
	void func_simpleTCPClient(ULONG S_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12345), char*szMsg = "Hello Server") {
		COUTLINE_FUNC_PARA;
		// ����
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addrServer.sin_port = htons(12345);
		addrServer.sin_family = AF_INET;
		char szRecv[MAX_PATH] = {};
		WSADATA wd = {};
		// ִ��
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
	// ���߳�TCP��������������ģ��
	void func_simpleTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
		// ����
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;// htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET; // IPv4
		addrServer.sin_port = sin_port;  // htons(12345);  // �ֽ���ת��
		addrServer.sin_zero;             // �ṹ�岹���ֽڣ�������
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szMsg[MAX_PATH] = {};
		// ִ��
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // ��ȡϵͳ�����SOCKET�ӿ�ID
		cout << "sockServer:" << sockServer << " ErrorCode:" << GetLastError() << endl;
		int iBindRtn = ::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // ��IP��˿ڰ󶨵�SOKCRT�ӿ�ID
		cout << "iBindRtn:" << iBindRtn << " ErrorCode:" << GetLastError() << endl;
		int iListenRtn = listen(sockServer, SOMAXCONN);  // ����IP:�˿�
		cout << "iListenRtn:" << iListenRtn << " ErrorCode:" << GetLastError() << endl;
		SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen); // �Ӷ�����ȡ��һ��������Ϣ
		cout << "sockClient:" << sockClient << " form " 
			<< inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port 
			<< " ErrorCode:" << GetLastError() << endl;
		int iRecvRtn = recv(sockClient, szMsg, MAX_PATH, 0);  // ��������Ϣ�ж�ȡ��Ϣ  // 0:ȱʡ
		cout << "[" << szMsg << "] is recv " << iRecvRtn << "/" << MAX_PATH << endl;
		int iSendRtn = send(sockClient, szMsg, strlen(szMsg) + 1, 0);  // ������Ϣ  // 0:ȱʡ
		cout << "[" << szMsg << "] is send " << iSendRtn << "/" << strlen(szMsg) + 1 << endl;
		closesocket(sockClient);
		closesocket(sockServer);
		WSACleanup();
	}
	// ���߳�TCP������select����ģ��
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

		// ����
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;  // htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET;			// IPv4
		addrServer.sin_port = sin_port;			// htons(12345);  // �ֽ���ת��
		addrServer.sin_zero;						// �ṹ�岹���ֽڣ�������
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szMsg[MAX_PATH] = {};
		// ִ��
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // ��ȡϵͳ�����SOCKET�ӿ�ID
		::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // ��IP��˿ڰ󶨵�SOKCRT�ӿ�ID
		listen(sockServer, SOMAXCONN);  // ����IP:�˿�

		// ��socket
		for (;0;) {
			SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
			cout << "sockClient:" << sockClient << " Error Code:" << GetLastError() << endl;
			recv(sockClient, szMsg, MAX_PATH, 0);
			sprintf(szMsg, "hello client");
			send(sockClient, szMsg, strlen(szMsg) + 1, 0);
			closesocket(sockClient);
			break;
		}

		// ���߳�select����ģ��
		for (;;) {
			// ���ڱ�������ID�͵�ַ��Ϣ�Ľṹ��
			struct SockInfo {
				SOCKET sock = 0;
				SOCKADDR_IN addr = {};
				SockInfo(SOCKET sockClient = 0, sockaddr_in addrClient = {}) 
					:sock(sockClient), addr(addrClient) { ; }
			};
			std::vector<SockInfo> vecClient;  // �ͻ�������
			fd_set readfds;  // ����˽���ע�ɶ��¼�
			while (true) {
				// ��ʼ������
				FD_ZERO(&readfds);
				// ��������socket���Ա��пͻ�������ʱ����accept
				FD_SET(sockServer, &readfds);  
				// ����ͻ���socket���Ա�ͻ��˷�����Ϣʱ����
				for (auto var : vecClient) { FD_SET(var.sock, &readfds); }  
				// �����socket��selectֻ���ע�Ƿ��пɶ��¼���������Ϊ����Ŀͻ������ǿ�д��
				// �ͻ��˵�connect��send�ᴥ������˵Ŀɶ��¼����Ա����˵���accept��recv
				// select��timeval����: null:����; 0:��������; >0:���ó�ʱʱ��  
				// select��timevalȡֵ������WSAPoll��ͬ
				int iSelectRtn = select(sockServer + 1, &readfds, NULL, NULL, NULL);// &tv);
				if (iSelectRtn == 0) { continue; }   // ��ʱ
				else if (iSelectRtn < 0) { break; }  // ����
				// �������socket�Ƿ�ɶ�  // �¿ͻ��˽���ʱ�ᴥ��
				if (FD_ISSET(sockServer, &readfds)) {
					FD_CLR(sockServer, &readfds);
					SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
					std::cout << "new client socket(" << sockClient << ") form "
						<< inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port
						<< " ErrorCode:" << GetLastError() << std::endl;
					if (sockClient == INVALID_SOCKET) { continue; }  // while(true){continue;}
					vecClient.push_back(SockInfo(sockClient,addrClient));
				}
				// ���ͻ��������Ƿ�ɶ�  // �ͻ��˷�����Ϣʱ�ᴥ���ɶ��¼�
				for (auto var : vecClient) {
					if (FD_ISSET(var.sock, &readfds)) {
						FD_CLR(var.sock, &readfds);
						memset(szMsg, 0, MAX_PATH);
						int iRecvRtn = recv(var.sock, szMsg, MAX_PATH, 0);
						std::cout << "socket(" << var.sock << ")"
							<< " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
							<< " recv msg(" << strlen(szMsg) << "/" << MAX_PATH << "):[" << szMsg << "]"
							<< " ErrorCode:" << GetLastError() << std::endl;
						if (iRecvRtn == 0) {  // �ͻ��˶Ͽ�����
							closesocket(var.sock);
							std::cout << "socket(" << var.sock << ")"
								<< " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
								<< " �ѶϿ�����" << " ErrorCode:" << GetLastError() << std::endl;
							// ɾ���ѶϿ��ͻ��˵�������Ϣ
							vector<SockInfo>::iterator ite = vecClient.begin();
							for (; ite != vecClient.end(); ++ite) { 
								if (ite->sock = var.sock) {
									vecClient.erase(ite);
									break;  // ��ֹiterator��forѭ��
								}
							}
							break;  // ��ֹFD_ISSET��forѭ��
						}
						// �ͻ������ǿ�д�ģ���˿����ڷ���˴����귢������Ϣ����������
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
	
	// ���߳�TCP������poll����ģ��
	void func_pollTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
#if(_WIN32_WINNT < 0x0600)
		ColorLine("ϵͳ�汾���ͣ���֧��poll����", TCOLOR_NOTE);
		return;
#endif // (_WIN32_WINNT < 0x0600)
		ColorLine("// Windows ���� poll ���Ƶĺ���Ϊ WSAPoll", TCOLOR_NOTE);

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

		// ����
		SOCKADDR_IN addrServer = {};
		addrServer.sin_addr.S_un.S_addr = S_addr;  // htonl(INADDR_ANY); // inet_addr("127.0.0.1");
		addrServer.sin_family = AF_INET;			// IPv4
		addrServer.sin_port = sin_port;			// htons(12345);  // �ֽ���ת��
		addrServer.sin_zero;						// �ṹ�岹���ֽڣ�������
		SOCKADDR_IN addrClient = {};
		int iAddrLen = sizeof(addrClient);
		char szRecv[MAX_PATH] = {};
		char szSend[MAX_PATH] = {};
		// ִ��
		WSADATA wd = { 0 };
		WSAStartup(MAKEWORD(2, 2), &wd);
		SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // ��ȡϵͳ�����SOCKET�ӿ�ID
		::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // ��IP��˿ڰ󶨵�SOKCRT�ӿ�ID
		listen(sockServer, SOMAXCONN);  // ����IP:�˿�

		// poll ģ��  // Windowsû��poll��ֻ��WSAPoll����Ҫ��ϵͳ�汾��XP����
		for (;;) {
			WSAPOLLFD fdArray[128] = {};
			ULONG fds = 0;
			INT timeout = -1;
			// ��ӷ�����socket
			fdArray[0].fd = sockServer;
			fdArray[0].events = POLLIN;
			fds += 1;

			while (true) {
				int iWSAPollRtn = WSAPoll(fdArray, fds, timeout);  // timeout<0:������0��������
				cout << "iWSAPollRtn:" << iWSAPollRtn << " count:" << fds << endl;
				if (iWSAPollRtn == 0) { continue; }
				else if (iWSAPollRtn < 0) { break; }
				// ��ѯ
				for (ULONG i = 0; i < fds; i++) {
					cout << "fdArray[" << i + 1 << "/" << fds << "]" << " fd:" << fdArray[i].fd
						<< " event:" << (fdArray[i].events == POLLIN ? "POLLIN" : "")
						<< (fdArray[i].events == POLLHUP ? "POLLHUP" : "")
						<< (fdArray[i].events == POLLERR ? "POLLERR" : "")
						<< " revents:" << fdArray[i].revents << endl;
					// �¼���ͻ��˺��¼���Ŀͻ��˲��ܲ��뱾��ѭ��
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
						// �Ͽ�����
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
					// ȥ��������
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

					// û�пͻ��ˣ�ֻ��һ��������ʱ�˳�
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

	// epoll ����ģ��
	void func_epollTCPServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
		COUTLINE_FUNC_PARA;
		ColorLine("// Windows û�������� epoll �ĺ���", TCOLOR_NOTE);
	}

	int main() {
		COUTLINE_FUNC_MAIN;
		COUTLINE_FUNC_INFO("::bind��std::bind���ܻ���������acceptʧ�ܣ�Ӧ��bindǰ���������޶���");
		// ���ٷ����ַ��������'\0'
		//func_simpleTCPServer();
		//func_simpleTCPClient();
		//func_selectTCPServer();
		return 0;
	}
}

// std::copy()/std::move()  // ��ֵ���á���ֵ����
namespace ns_copymove {
	// ��ֵ����ֵ:
	// ��ֵ��ָ���ʽ��������Ȼ���ڵĳ־û�������ֵ��ָ���ʽ����ʱ�Ͳ��ٴ��ڵ���ʱ����
	// ���еľ����������߶�������ֵ������ֵ��������
	// 
	// ��ֵ��������ֵ����(&&,int&& j = i++;)��
	// ��ֵ���ã���������һ����ֵ�������ӳ���ֵ�������ڡ�"abc",123�ȶ�����ֵ��
	// ����ͨ����ֵ���ã����ʹ����ʱ����������ֵ��Դ�����ٲ���Ҫ�Ŀ��������Ч��.
	int main() {
		COUTLINE_FUNC_MAIN;
		//std::copy();
		return 0;
	}
}

// ���ģʽ
namespace ns_designPattern {
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
};

//�߳�����ִ����  // std::map �޷����� LPVOID
namespace ns_threadTask {
	class ThreadTask {
	public:
		ThreadTask() {
			//std::map<std::function<LPVOID>, LPVOID>mapFunc;  // ����ָ�뼰��������ָ��
			//std::vector<std::map<std::function<LPVOID>, LPVOID>>taskList;  // �����б�
			//static unique_ptr<std::vector<std::map<std::function<LPVOID>, LPVOID>>>g_task; // ȫ��ֻ��һ�������б�
			////
			//auto lambda = [](LPVOID lpParam)->void {};
			//std::function<void(LPVOID)> f = lambda;
			////mapFunc.insert(f, NULL);
		}
		~ThreadTask() {}
		//auto getInstance() {
		//	// ȫ�־��û������б���󷵻أ����򷵻ر����ֵ
		//	// �����ʱӦ�ڶ�д�þ��ǰ��������
		//	if (!g_instance) { g_instance = make_shared<ThreadTask*>(this); }
		//	if (g_instance) { return g_instance; }
		//}
		//void start() {
		//	// ����ѭ�������˳�����
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
		//static std::shared_ptr<ThreadTask*> g_instance;  // ���ڱ���ȫ��Ψһ����ľ��
		//// �����Ϊȫ��ͳһ���ʶ���Ҫʹ���������������б�
		//// ����������ָ�롢������
		//// ����    ��std::vector<>          // vector����ȡ��
		//// ����ָ�룺std::unique_ptr<>      // һ������ֻ����һ���߳���ִ��һ��
		//// ��������std::function<LPVOID>  // LPVOID���ں�������
		//// ����ָ��������������������б������б�Ϊӳ�䣬ӳ�䱣��
		//std::unique_ptr<std::function<LPVOID>> m_task; // ָ��������������ָ��
		//std::unique_ptr<HANDLE>m_thread;  // �̺߳���
		//HANDLE m_mutex = NULL;
	};
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
}

// threadPool �̳߳�
namespace ns_threadPool {
	int main() {
		COUTLINE_FUNC_MAIN;
		return 0;
	}
}

//  �����ֿռ�
#include <istream>
int
main(
	_In_ int _Argc,
	_In_count_(_Argc) _Pre_z_ char ** _Argv,
	_In_z_ char ** _Env
) {
	// �ж�����ģʽ
	bool bSimple = false;
	for (int i = 0; i < _Argc; i++) {
		if (strcmp(_Argv[i], "--onlyStartInfo") == 0) {
			bSimple = true;
			break;
		}
	}
	// ִ�г���
	if (bSimple) {
		ColorLine("**** New Process Begin ****", TCOLOR_FUNC);
		ns_info::main(_Argc, _Argv, _Env);
		ColorLine("**** New Process Over ****", TCOLOR_FUNC);
	}
	else {
		COLORCONSOLE::main();
		ns_info::main(_Argc, _Argv, _Env);
		ns_constructFunction::main();	// ��������
		ns_pointerCast::main();			// ����ת��
		ns_smartPointer::main();		// ����ָ��
		ns_vector::main();				// STL����
		ns_functionBind::main();		// ������
		ns_exception::main();			// �쳣����
		ns_processThread::main();		// �������̲���δ��
		ns_mutex::main();				// ��

		ColorLine("// ���粿������������Ĭ�ϲ�ִ�У���Ҫ�ֶ���ע��", TCOLOR_NOTE);
		//ns_socket::main();
		//ns_socket::func_simpleTCPServer();
		//ns_socket::func_selectTCPServer();
		//ns_socket::func_pollTCPServer();
	}
	system("pause");
	return 0;
}