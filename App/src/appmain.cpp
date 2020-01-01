﻿#include <CQEVE.h>
#include <CQAPI.h>
#include <CQAPI_EX.h>

#include "APPINFO.h"

#include <Windows.h>
#include <thread>
#include <string>

#include "Fun.hpp"
#include "FreeResFile.hpp"

using namespace std;
using namespace CQ;


//请加上static,表示这个logger只有本cpp有效


//应用已被启用
EVE_Enable(Enable)
try
{
	OpenWin::close();
	appFile = CQ::getAppDirectory();

	//检查秘钥
	thread* ph = new thread([]()
		{

			while (true)
			{
				Sleep(1000 * 60 * 60 * 24);
				Conf::keyFun();
			}

		}
	);
	logger.testLog("应用已被启用");

	//检查是否为第一次启动
	if (FreeResFile::oneStart() == false)
		return 0;


	logger.testLog("End oneStart");

	ph->detach();
	Conf::keyFun();
	Conf::initConf();

	logger.testLog("End 应用被启用");
	logger.Info("应用被启用");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“应用已被启用” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“应用已被启用” 出现崩溃已被阻止 原因:未知");
}


//应用被停用
EVE_Disable(Disable)
try
{
	logger.Info("应用被停用");
	logger.testLog("应用被停用");

	OpenWin::close();

	return 0;
}
catch (exception & e)
{
	logger.Info(string("“应用即将被停用” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“应用即将被停用” 出现崩溃已被阻止 原因:未知");
}


EVE_Exit(Exit)
{
	logger.Info("酷Q关闭");
	logger.testLog("酷Q关闭");

	OpenWin::close();

	return 0;
}



MUST_AppInfo_RETURN(CQAPPID)


//操作


//群成员添加
EVE_System_GroupMemberIncrease(intoGroup)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");
	}

	logger.testLog("群成员添加");
	SendEmail a(fromGroup, beingOperateQQ, "成员进群");
	a.send();
	logger.testLog("End 群成员添加");

	return 0;
}
catch (exception & e)
{
	logger.Info(string("“群成员添加” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“群成员添加” 出现崩溃已被阻止 原因:未知");
}



//群成员退出
EVE_System_GroupMemberDecrease(exitGroup)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");

		return 0;
	}
	logger.testLog("群成员退出");

	if (g_otherSet.quitGroupSend)
	{
		SendEmail a(fromGroup, beingOperateQQ, "成员退群");
		a.send();
	}
	logger.testLog("End 群成员退出");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“群成员退出” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“群成员退出” 出现崩溃已被阻止 原因:未知");
}


//好友添加
EVE_Request_AddFriend(addFriend)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");
		return 0;
	}

	logger.testLog("好友添加");
	if (g_otherSet.addFriend)
	{
		CQ::setFriendAddRequest(responseFlag, 请求_通过, "");
	}

	logger.testLog("End 好友添加");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“好友添加” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“好友添加” 出现崩溃已被阻止 原因:未知");
}


//群添加
EVE_Request_AddGroup(addGroup)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");
		return 0;
	}

	logger.testLog("群添加");
	if (subType == 2)
	{
		if (g_otherSet.addGroupInto)
		{
			CQ::setGroupAddRequest(responseFlag, 请求_群邀请, 请求_通过, "");
		}

	}
	logger.testLog("End 群添加");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“群添加” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“群添加” 出现崩溃已被阻止 原因:未知");
}


//私聊消息
EVE_PrivateMsg(privateMsg)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");
		return 0;
	}

	return 0;
}
catch (exception & e)
{
	logger.Info(string("“私聊消息” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“私聊消息” 出现崩溃已被阻止 原因:未知");
}


//群消息
EVE_GroupMsg(groupMsg)
try
{
	if (g_otherSet.appRun == false)
	{
		logger.Info("请先激活软件");
		return 0;
	}

	//logger.testLog("群消息");
	KeyWordMsg a(fromGroup, fromQQ, msg);
	a.KeyWordFun();
	//logger.testLog("End 群消息");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“群消息” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“群消息” 出现崩溃已被阻止 原因:未知");
}


//菜单
EVE_Menu(__menuSet)
try
{
	if (g_otherSet.appRun == false)
	{
		MessageBox(NULL, L"请激活后使用，插件可能正在检测是否激活", L"未激活", MB_OK);
		return 0;
	}

	logger.testLog("菜单");
	if (OpenWin::openWin() == false)
	{
		MessageBox(NULL, L"界面打开失败,请查看日志中错误原因", L"失败", MB_OK);
	}

	logger.testLog("End 菜单");
	return 0;
}
catch (exception & e)
{
	logger.Info(string("“菜单” 出现崩溃已被阻止 原因:") + e.what());
}
catch (...)
{
	logger.Info("“菜单” 出现崩溃已被阻止 原因:未知");
}