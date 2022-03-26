/****************************************************************************************
*  LingYinPerfToolDll.h																	*
*																						*
*  Copyright (c) 2022 Ricardo2001zg All Rights Reserved.								*
*																						*
*  @file  KHagsRegeditDll.h																*
*  @brief readme:																		*
*  1.C++ 11																				*
*  	(1) nullptr																			*
*  	Use nullptr instead of NULL .														*
*  	(2) auto																			*
*  	(3) constexpr																		*
*  																						*
*  2.rules																				*
*  	(1) Semantic Versioning 2.0.0														*
*  	Use Semantic Versioning to write version number(x.y.z).								*
*  	Docs: https://semver.org/lang/zh-CN/												*
*  	(2) Doxygen																			*
*  	Use Doxygen to write annotation.													*
*  																						*
*  @author   Ricardo2001zg																*
*  @email    miao@ricardo2001zg.moe														*
*  @license  MIT																		*
*  																						*
*  @version  0.1.0																		*
*																						*
****************************************************************************************/

#include "pch.h"

/****************************************************************************************
* TODO list:
* √1、根据 PID 暂停线程
* √2、判断 EIP
* √3、根据 PID 恢复线程
* √4、符号表注入
* 5、KG_PrintLog Hook
* 6、AGS 替换 device
* 7、AGS 检测异步编译任务
* 
****************************************************************************************/

namespace LingYinPerfTool {
	/*! @fn HRESULT GetPidFromAppName(std::wstring& pszAppProcessName, DWORD& pnProcessID);
	*  @brief 传入程序进程名，返回查找到的第一个对应程序的 Pid（暂时仅实现单个同名进程获取）
	*  @param[in]  pszAppProcessName 输入需要查找 Pid 的程序进程名
	*  @param[out] pnProcessID		 返回打开程序的 Pid
	*  @return						 如果失败，返回 E_FAIL
	*/
	HRESULT GetPidFromAppName(std::wstring& pszAppProcessName, DWORD& pnProcessID);

	// TODO: std::wstring 兼容性改动

	/*! @fn HRESULT GetFunctionAddressFromSymbol(DWORD& pnProcessID, PCSTR pszFunctionName, ULONG64& pnFunctionAddress);
	*  @brief 传入进程 Pid 与需要获取地址的函数名，通过符号表查找函数地址
	*  @param[in]  nProcessID	      输入需要查找函数对应进程的 Pid
	*  @param[in]  pszFunctionDllName 输入需要查找函数所在的 DLL
	*  @param[in]  pszFunctionName    输入需要查找的函数名
	*  @param[out] nProcessID	      返回对应的函数地址
	*  @return					      如果失败，返回 E_FAIL
	*/
	HRESULT GetFunctionAddressFromSymbol(DWORD& pnProcessID, LPWSTR& pszFunctionDllName, PCSTR pszFunctionName, ULONG64& pnFunctionAddress);

	/*! @fn HRESULT CheckEIPAndSuspendByProcessID(DWORD& nProcessID, DWORD nAddress, size_t nSize);
	*  @brief 根据 Pid 暂停对应进程的所有线程
	*  @param[in] nProcessID 进程 Pid
	*  @param[in] nAddress	 注入目标函数地址
	*  @param[in] nSize		 目标函数大小
	*  @return				 如果失败，返回 E_FAIL
	*/
	HRESULT CheckEIPAndSuspendByProcessID(DWORD& pnProcessID, DWORD& pnAddress, size_t nSize);

	/*! @fn HRESULT RusumeByProcessID(DWORD& nProcessId);
	*  @brief 根据 Pid 恢复对应进程被暂停的所有线程
	*  @param[in] nProcessID 进程 Pid
	*  @return				 如果失败，返回 E_FAIL
	*/
	HRESULT RusumeByProcessID(DWORD& pnProcessId);

} 