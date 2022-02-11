/****************************************************************************************
*  LingYinPerfToolDll.h																	*
*																						*
*  Copyright (c) 2022 Ricardo2001zg All Rights Reserved.								*
*																						*
*  @file  KHagsRegeditDll.h																*
*  @brief readme:																		*
*  1.Base on C++ 11																		*
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

#include "../pch.h"

HRESULT LingYinPerfTool::GetPidFromAppName(
	LPWSTR& pszAppProcessName,
	DWORD& pnProcessId
)
{
	pnProcessId = 0;
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	if (!Process32First(hSnapshot, &pe))
	{
		return E_FAIL;
	}
	while (Process32Next(hSnapshot, &pe))
	{
		if (lstrcmpiW(pszAppProcessName, pe.szExeFile) == 0)
		{
			pnProcessId = pe.th32ProcessID;
			CloseHandle(hSnapshot);
			break;
		}
	}
	if (pnProcessId != 0) 
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT LingYinPerfTool::GetFunctionAddressFromSymbol(
	DWORD& pnProcessID,
	LPWSTR& pszFunctionDllName,
	PCSTR pszFunctionName,
	ULONG64& pnFunctionAddress
)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pnProcessID);
	if (hProcess == nullptr)
	{
		return E_FAIL;
	}
	// 获取符号表中函数地址
	if (!SymInitialize(hProcess, NULL, FALSE))
	{
		CloseHandle(hProcess);
		return E_FAIL;
	}
	if (!SymRefreshModuleList(hProcess))
	{
		SymCleanup(hProcess);
		CloseHandle(hProcess);
		return E_FAIL;
	}
	char cMemory[1024];
	auto pSymbolInfo = reinterpret_cast<SYMBOL_INFO*>(cMemory);
	pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbolInfo->MaxNameLen = sizeof(cMemory) - sizeof(SYMBOL_INFO);
	if (SymFromName(hProcess, pszFunctionName, pSymbolInfo))
	{
		pnFunctionAddress = pSymbolInfo->Address;
	}
	SymCleanup(hProcess);
	// 获取函数所在 DLL 的基地址
	/*
	MODULEENTRY32 pe = { sizeof(MODULEENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pnProcessID);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	if (!Module32FirstW(hSnapshot, &pe))
	{
		return E_FAIL;
	}
	while (Module32NextW(hSnapshot, &pe))
	{
		if (lstrcmpiW(pszFunctionDllName, pe.szModule) == 0)
		{
			pe.hModule;
			CloseHandle(hSnapshot);
			break;
		}
	}
	pnFunctionAddress += reinterpret_cast<ULONG64>(pe.hModule);
	*/
	// 关闭句柄
	CloseHandle(hProcess);
	return S_OK;
}

HRESULT LingYinPerfTool::CheckEIPAndSuspendByProcessID(
	DWORD& pnProcessID, 
	DWORD& pnAddress, 
	size_t nSize
)
{
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) 
	{
		return E_FAIL;
	}
	THREADENTRY32 th32;
	th32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &th32))
	{
		return E_FAIL;
	}
	while (Thread32Next(hThreadSnap, &th32))
	{
		if (th32.th32OwnerProcessID == pnProcessID)
		{
			CONTEXT context = { 0 };
			HANDLE hThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (hThreadHandle != nullptr) 
			{
				SuspendThread(hThreadHandle);
			}
			else 
			{
				return E_FAIL;
			}
			context.ContextFlags = CONTEXT_CONTROL;
			if (!GetThreadContext(hThreadHandle, &context))
			{
				continue;
			}
			if (context.Rip >= pnAddress && context.Rip <= (pnAddress + nSize))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

HRESULT LingYinPerfTool::RusumeByProcessID(DWORD& pnProcessID)
{
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	THREADENTRY32 th32;
	th32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &th32))
	{
		return E_FAIL;
	}
	while (Thread32Next(hThreadSnap, &th32))
	{
		if (th32.th32OwnerProcessID == pnProcessID)
		{
			HANDLE hThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (hThreadHandle != nullptr)
			{
				ResumeThread(hThreadHandle);
			}
			else
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}