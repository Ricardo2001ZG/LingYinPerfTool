#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

// 标准 c 库
#include <stdlib.h>

// Windows 头文件
#include <windows.h>
#include "TlHelp32.h"
#include "DbgHelp.h"

// Dbghelp
#pragma comment(lib, "dbghelp.lib")