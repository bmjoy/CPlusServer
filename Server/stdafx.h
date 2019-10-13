// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <queue>
#include <map>
#include <deque>
using namespace std;


// TODO:  在此处引用程序需要的其他头文件
#include "common/CommonDef.h"
#include "common/CommonReq.h"
#include "include/func.h"
#include "include/utils.h"
#include "include/json/json.h"
#include "include/LockFreeQueue.h"
#include "threadpool/ThreadPool.h"
#include "threadpool/MessageQueue.h"
#include "iocp/IocpModule.h"
#include "iocp/IocpSocket.h"
#include "iocp/IocpAccept.h"
#include "iocp.h"
#include "timer/timer.h"
#include "db/DBCommon.h"
#include "db/DBOperator.h"
#include "redis/RedisManager.h"
#include "redis/RedisOperator.h"
#include "CBaseServer.h"
#include "CPlusServer.h"
#include "CPlusClient.h"
#include "HttpManager.h"