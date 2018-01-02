
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#if WIN32
#include <Windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#endif

#include "fflua/lua/fflua.h"
#include "Cliplus/Cliplus.h"
#include "../log4z.h"

using namespace zsummer::log4z;
using namespace ff;

class base_t
{
public:
	base_t():v(789){}
	void dump()
	{
		printf("in %s a:%d\n", __FUNCTION__, v);
	}
	int v;
};
class foo_t: public base_t
{
public:
	foo_t(int b):a(b)
	{
		printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
	}
	~foo_t()
	{
		printf("in %s\n", __FUNCTION__);
	}
	void print(int64_t a, base_t* p) const
	{
		printf("in foo_t::print a:%ld p:%p\n", (long)a, p);
	}

	static void dumy()
	{
		printf("in %s\n", __FUNCTION__);
	}
	int a;
};

//! lua talbe 可以自动转换为stl 对象
void dumy(map<string, string> ret, vector<int> a, list<string> b, set<int64_t> c)
{
	printf("in %s begin ------------\n", __FUNCTION__);
	for (map<string, string>::iterator it =  ret.begin(); it != ret.end(); ++it)
	{
		printf("map:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
	}
	printf("in %s end ------------\n", __FUNCTION__);
}

class clazz{
public:
	static void static_func(){
		printf("in clazz::%s end ------------\n", __FUNCTION__);
	}
};

static void lua_reg(lua_State* ls)
{
	//! 注册基类函数, ctor() 为构造函数的类型
	fflua_register_t<base_t, ctor()>(ls, "base_t")  //! 注册构造函数
		.def(&base_t::dump, "dump")     //! 注册基类的函数
		.def(&base_t::v, "v");          //! 注册基类的属性

	//! 注册子类，ctor(int) 为构造函数， foo_t为类型名称， base_t为继承的基类名称
	fflua_register_t<foo_t, ctor(int)>(ls, "foo_t", "base_t")
		.def(&foo_t::print, "print")        //! 子类的函数
		.def(&foo_t::a, "a");               //! 子类的字段

	fflua_register_t<>(ls)
		.def(&dumy, "dumy");                //! 注册静态函数


	fflua_register_t<clazz, ctor()>(ls, "clazz")
		.def(&clazz::static_func, "static_func"); 

}

#define MIN_ALPHA_VALUE 40
#define MIN_INACTIVE_ALPHA_VALUE 0
#define MAX_ALPHA_VALUE 255
#define LODWORD(ull) ((DWORD)((ULONGLONG)(ull) & 0x00000000ffffffff))

bool SetTransparent(HWND hAlphaWnd
    , UINT uAlpha/*0..255*/
    , bool bColorKey = false
    , COLORREF clrMaskKey = 0
    , bool bForceLayered = false)
{
    UINT nTransparent = max(MIN_INACTIVE_ALPHA_VALUE,min(uAlpha,255));
	DWORD dwExStyle = GetWindowLongPtr(hAlphaWnd, GWL_EXSTYLE);

    {
        if ((dwExStyle & WS_EX_LAYERED) == 0)
        {
            dwExStyle |= WS_EX_LAYERED;
	        SetWindowLong(hAlphaWnd, GWL_EXSTYLE, dwExStyle);
            LOGI("Transparency: Enabling WS_EX_LAYERED");
        }
        else
        {
            LOGA("Transparency: WS_EX_LAYERED was already enabled");
        }

        DWORD nNewFlags = (((nTransparent < 255) || bForceLayered) ? LWA_ALPHA : 0) | (bColorKey ? LWA_COLORKEY : 0);

        BYTE nCurAlpha = 0;
        DWORD nCurFlags = 0;
        COLORREF nCurColorKey = 0;
        BOOL bGet = FALSE;
            bGet = GetLayeredWindowAttributes(hAlphaWnd, &nCurColorKey, &nCurAlpha, &nCurFlags);

        if ((!bGet)
            || (nCurAlpha != nTransparent) || (nCurFlags != nNewFlags)
            || (bColorKey && (nCurColorKey != clrMaskKey)))
        {
            BOOL bSet = SetLayeredWindowAttributes(hAlphaWnd, clrMaskKey, nTransparent, nNewFlags);
            if (true)
            {
                DWORD nErr = bSet ? 0 : GetLastError();
                LOGFMTA("Transparency: Set(0x%08X, 0x%08X, %u, 0x%X) -> %u:%u",
                    LODWORD(hAlphaWnd), clrMaskKey, nTransparent, nNewFlags, bSet, nErr);
            }
        }
        else
            LOGA(L"Transparency: Attributes were not changed");
        // Ask the window and its children to repaint
        //RedrawWindow(hAlphaWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
    }
    return true;
}

HWND GetCliHelperWnd()
{
    char szWndTitle[256] = { 0 };
    GetConsoleTitleA(szWndTitle, 256);
    return FindWindowA("ConsoleWindowClass", szWndTitle);
}

class CCliApp:public CCliplus{
public:
	void LoadScript()
	{
		m_fflua.setModFuncFlag(true);
		//! 注册C++ 对象到lua中
		m_fflua.reg(lua_reg);
		//! 载入lua文件
		m_fflua.add_package_path("./");
		//m_fflua.load_file("test.lua");
		m_fflua.load_file("../test/test.lua");
	}

	void TestScript()
	{
		//! 获取全局变量
		int var = 0;
		assert(0 == m_fflua.get_global_variable("test_var", var));
		//! 设置全局变量
		assert(0 == m_fflua.set_global_variable("test_var", ++var));

		//! 执行lua 语句
		m_fflua.run_string("print(\"exe run_string!!\")");

		//! 调用lua函数, 基本类型作为参数
		int32_t arg1 = 1;
		float   arg2 = 2;
		double  arg3 = 3;
		string  arg4 = "4";
		m_fflua.call<bool>("test_func", arg1, arg2, arg3,  arg4);
		m_fflua.call<bool>("Mod:funcTest1", arg1, arg2);

		//! 调用lua函数，stl类型作为参数， 自动转换为lua talbe
		vector<int> vec;        vec.push_back(100);
		list<float> lt;         lt.push_back((float)99.99);
		set<string> st;         st.insert("OhNIce");
		map<string, int> mp;    mp["key"] = 200;
		m_fflua.call<string>("test_stl", vec, lt, st,  mp);

		//! 调用lua 函数返回 talbe，自动转换为stl结构
		vec = m_fflua.call<vector<int> >("test_return_stl_vector");
		lt  = m_fflua.call<list<float> >("test_return_stl_list");
		st  = m_fflua.call<set<string> >("test_return_stl_set");
		mp  = m_fflua.call<map<string, int> >("test_return_stl_map");

		//! 调用lua函数，c++ 对象作为参数, foo_t 必须被注册过
		foo_t* foo_ptr = new foo_t(456);
		m_fflua.call<void>("test_object", foo_ptr);

		//! 调用lua函数，c++ 对象作为返回值, foo_t 必须被注册过 
		assert(foo_ptr == m_fflua.call<foo_t*>("test_ret_object", foo_ptr));
		//! 调用lua函数，c++ 对象作为返回值, 自动转换为基类
		base_t* base_ptr = m_fflua.call<base_t*>("test_ret_base_object", foo_ptr);
		assert(base_ptr == foo_ptr);
	}
private:
	fflua_t m_fflua;

protected:
	void AcceptCommand(char *szCmd)
	{
		if (strncmp(szCmd, "run", 3) == 0) {
			LOGI("run lua code");
			m_fflua.run_string("print(\"Hello World!!!\")");
		}
		else if (strncmp(szCmd, "trans", 4) == 0) {
            int nTransparency = 1;
            sscanf(szCmd, "trans %d", &nTransparency);
            LOGFMTA("透明度为：%d", nTransparency);
            SetTransparent(GetCliHelperWnd(), nTransparency, true, RGB(100, 10, 50));
            //SetTransparent(GetCliHelperWnd(), nTransparency);
		}
		else if (strncmp(szCmd, "ldb", 3) == 0) {
			m_fflua.call<void>("pause","open ldb debugger", 1);
		}else
		{
			LOGW("Invaild Input << "<< szCmd);
		}
	}

	void ShowHelpTips()
	{
		LOGI("演示Cli窗口应用的功能");
	}

	void ShowQuitTip()
	{
		LOGI("按下任意键退出...");
	}
};

int main(int argc, char* argv[])
{
	ILog4zManager::getRef().setLoggerName(LOG4Z_MAIN_LOGGER_ID, "fflua");
	ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "./");
	//ILog4zManager::getRef().enableLogger(LOG4Z_MAIN_LOGGER_ID, false);
	ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, true);
	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_TRACE);
	ILog4zManager::getRef().setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID, true);
	ILog4zManager::getRef().setLoggerThreadId(LOG4Z_MAIN_LOGGER_ID, false);
	ILog4zManager::getRef().setLoggerOutFile(LOG4Z_MAIN_LOGGER_ID, true);
	ILog4zManager::getRef().start();

	CCliApp m_AppObj;
	try 
	{
		m_AppObj.LoadScript();
		m_AppObj.Open();
		m_AppObj.TestScript();
	}
	catch (exception& e)
	{
		LOGF("exception:"<<e.what());
	}
	return 0;
}
