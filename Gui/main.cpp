#include "MarketWin.h"
#include "MarketKey.h"
#include "MarketUpdate.h"

#include <QtWidgets/QApplication>
#include <qmessagebox.h>
#include <qstylefactory.h>
#include <qfile.h>
#include <QtConcurrentRun>

#include "json.hpp"
#include "Pipe.hpp"

#include "resource.h"

#include <sstream>
#include <string>
#include <map>
#include <thread>

using namespace std;

bool get_mainData(QApplication& a);
bool get_keyData(QApplication& a);
bool get_Update(QApplication& a);


string appFile = "D:\\用户文档\\桌面\\桌面整理\\酷Q\\data\\app\\cn.xiaoxiaoge.Market\\";
map<long long, wstring> WinGroupList;

string local_computer;
string version;
string key;
string exceedDateTime;

Pipe WinPipe;

std::function<void(const char*)> sqlExec = NULL;

//#define _DEBUG

void LogSql(string sql)
{
	if (sqlExec != NULL)
	{
		sqlExec(sql.c_str());
	}
	//wMain.sqlExec(sql.c_str());
}

//void test()
//{
//
//	QFuture <void> future = QtConcurrent::run([&]()
//	{
//
//		char sql[] = "INSERT INTO log (Time, QQId, GroupId, Type, Word) VALUES (datetime(CURRENT_TIMESTAMP,'localtime'),'0','0','test','word');";
//
//
//		while (true)
//		{
//			if (sqlExec != NULL)
//				sqlExec(sql);
//
//			Sleep(80);
//		}
//	}
//	);
//}


void PipeCommand(string sql, QApplication& a)
{
	if (sql == "/*close*/")
	{
		a.quit();
	}




}


void ReadPipe(QApplication& a)
{
	WinPipe.initClient();

	QFuture <void> future = QtConcurrent::run([&]()
	//auto ph = new std::thread([&]()
	{
		while (true)
		{
			//static int countEmpty = 0;
			//int countEmpty = 0;

			string buf;
			WinPipe.ReadData(buf);

			//MessageBoxA(NULL, buf.c_str(), "test", NULL);
			if (!buf.empty())
			{
				//countEmpty = 0;

				//if (buf[0] - '0' == PIPEMSGTYPE::SQL)
				//{
				LogSql(buf.c_str());
				//}
				//else if (buf[0] - '0' == PIPEMSGTYPE::COMMAND)

				if (buf.find("/*") != string::npos && buf.find("*/") != string::npos)
				{
					PipeCommand(buf.c_str(), a);
				}
			}
			/*else
			{
				countEmpty++;
			}


			if (countEmpty > 20)
			{
				MessageBoxA(NULL, "由于连续多次接收到空消息,界面将自动关闭", "错误", MB_OK);
				a.quit();
				break;
			}*/


		}
	});
}



int main(int argc, char* argv[])
{
	//QApplication::setStyle(QStyleFactory::create("Fusion"));

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/logo.png"));

	ReadPipe(a);

	QFile qss(":/moren.qss");

	qss.open(QFile::ReadOnly);
	a.setStyleSheet(qss.readAll());
	qss.close();

#ifndef _DEBUG


	if (argc < 2)
	{
		return -1;
	}

	if (!strcmp(argv[1], "MarketWin"))
	{

		if (get_mainData(a) == false)
			return 0;
#endif // 0

		MarketWin wMain;

		sqlExec = [&](const char* sql) {
			wMain.sqlExec(sql);
		};

		//test();
		wMain.show();

		return a.exec();
#ifndef _DEBUG

	}
	else if (!strcmp(argv[1], "MarketKey"))
	{
		if (get_keyData(a) == false)
			return 0;

		MarketKey wKey;
		wKey.show();

		return a.exec();

	}
	else if (!strcmp(argv[1], "MarketUpdate"))
	{

		if (get_Update(a) == false)
			return 0;

		MarketUpdate wUpdata;
		wUpdata.runApp();

		return a.exec();

	}
	return -2;
#endif // 0
}

//读取文件全部内容
string readFileAll(const char* file)
{
	std::ifstream in(file);
	std::ostringstream tmp;
	tmp << in.rdbuf();
	return tmp.str();
}


bool get_mainData(QApplication& a)
{
	try
	{
		string temp_buf = readFileAll("win.tmp");

		boost::property_tree::wptree temp_json;

		//解析json字符串
		wstringstream temp_ss(OperateStr::string2wstring(temp_buf));
		boost::property_tree::read_json(temp_ss, temp_json);

		//读取文件路径
		wstring wAppFile = temp_json.get<wstring>(L"AppFile");
		appFile = OperateStr::wstring2string(wAppFile);

		//读取机器码
		wstring wComputerId = temp_json.get<wstring>(L"ComputerId", L"");
		local_computer = OperateStr::wstring2string(wComputerId);

		//读取key
		wstring wKey = temp_json.get<wstring>(L"Key", L"");
		key = OperateStr::wstring2string(wKey);

		//读取到期时间
		wstring wExceedDateTime = temp_json.get<wstring>(L"ExceedDateTime", L"");
		exceedDateTime = OperateStr::wstring2string(wExceedDateTime);

		//读取版本号
		wstring wVersion = temp_json.get<wstring>(L"Version", L"");
		version = OperateStr::wstring2string(wVersion);


		//读取群列表
		auto groupList_json = temp_json.get_child_optional(L"GroupList");
		if (groupList_json)
		{
			for (auto temp : groupList_json.value())
			{
				wstring id = temp.first.c_str();
				wstring name = temp.second.get<wstring>(L"Name");
				WinGroupList[_wtoll(id.c_str())] = name;
			}

		}

		//监测是否为空
		if (appFile.empty())
		{
			throw exception("数据路径为空");
		}

		remove("./win.tmp");

	}
	catch (exception & e)
	{
		MessageBoxA(NULL, e.what(), "主界面 互通错误", MB_OK);

		return false;
	}
	catch (...)
	{
		MessageBoxA(NULL, "读取数据异常", "主界面 互通错误", MB_OK);

		return false;
	}

	return true;
}

bool get_keyData(QApplication& a)
{
	try
	{
		string temp_buf = readFileAll("win.tmp");
		remove("./win.tmp");

		boost::property_tree::wptree temp_json;

		//解析json字符串
		wstringstream temp_ss(OperateStr::string2wstring(temp_buf));
		boost::property_tree::read_json(temp_ss, temp_json);

		//读取文件路径
		wstring wAppFile = temp_json.get<wstring>(L"AppFile", L"");
		appFile = OperateStr::wstring2string(wAppFile);

		//读取机器码
		wstring wComputerId = temp_json.get<wstring>(L"ComputerId", L"");
		local_computer = OperateStr::wstring2string(wComputerId);

		//读取key
		wstring wKey = temp_json.get<wstring>(L"Key", L"");
		key = OperateStr::wstring2string(wKey);


		//监测是否为空
		if (appFile.empty())
		{
			throw exception("数据路径为空");
		}

		//监测是否为空
		if (wComputerId.empty())
		{
			throw exception("数据路径为空");
		}
	}
	catch (exception & e)
	{
		MessageBoxA(NULL, e.what(), "秘钥 互通错误，请尝试重启酷Q", MB_OK);

		//a.closeAllWindows();
		return false;
	}
	catch (...)
	{
		MessageBoxA(NULL, "读取数据异常，请尝试重启酷", "秘钥 互通错误", MB_OK);

		//a.closeAllWindows();
		return false;
	}

	return true;
}

bool get_Update(QApplication& a)
{
	try
	{
		string temp_buf = readFileAll("win.tmp");
		remove("./win.tmp");


		boost::property_tree::wptree temp_json;

		//解析json字符串
		wstringstream temp_ss(OperateStr::string2wstring(temp_buf));
		boost::property_tree::read_json(temp_ss, temp_json);

		//读取文件路径
		wstring wAppFile = temp_json.get<wstring>(L"AppFile");
		appFile = OperateStr::wstring2string(wAppFile);

		//读取版本号
		wstring wVersion = temp_json.get<wstring>(L"Version", L"");
		version = OperateStr::wstring2string(wVersion);

		//监测是否为空
		if (appFile.empty())
		{
			throw exception("数据路径为空");
		}

	}
	catch (exception & e)
	{
		MessageBoxA(NULL, e.what(), "检查更新 互通错误", MB_OK);

		//a.closeAllWindows();
		return false;
	}
	catch (...)
	{
		MessageBoxA(NULL, "读取数据异常", "检查更新 互通错误", MB_OK);

		//	a.closeAllWindows();
		return false;
	}

	return true;
}