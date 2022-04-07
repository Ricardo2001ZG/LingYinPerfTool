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

#include "../Publish/pch.h"
#include "../Publish/LingYinPerfToolDll.h"

HRESULT LingYinPerfTool::GetPidFromAppName(
	std::wstring& pszAppProcessName,
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
		if (lstrcmpi(pszAppProcessName.c_str(), pe.szExeFile) == 0)
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
	std::wstring& pszFunctionDllName,
	std::wstring& pszFunctionName,
	ULONG64& pnFunctionAddress
)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pnProcessID);
	if (hProcess == nullptr)
	{
		return E_FAIL;
	}
	// 获取符号表中函数地址
	if (!SymInitializeW(hProcess, NULL, FALSE))
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
	auto pSymbolInfo = reinterpret_cast<SYMBOL_INFOW*>(cMemory);
	pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFOW);
	pSymbolInfo->MaxNameLen = sizeof(cMemory) - sizeof(SYMBOL_INFOW);
	if (SymFromNameW(hProcess, pszFunctionName.c_str(), pSymbolInfo))
	{
		pnFunctionAddress = pSymbolInfo->Address;
	}
	SymCleanup(hProcess);
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

#define LYCOMMONAPI extern "C" __declspec(dllexport)

LYCOMMONAPI HRESULT GetPidFromAppName(std::wstring& pszAppProcessName, DWORD& pnProcessID)
{
	return LingYinPerfTool::GetPidFromAppName(pszAppProcessName, pnProcessID);
}

LYCOMMONAPI HRESULT GetFunctionAddressFromSymbol(DWORD& pnProcessID, std::wstring& pszFunctionDllName, std::wstring& pszFunctionName, ULONG64& pnFunctionAddress)
{
	return LingYinPerfTool::GetFunctionAddressFromSymbol(pnProcessID, pszFunctionDllName, pszFunctionName, pnFunctionAddress);
}

LYCOMMONAPI HRESULT CheckEIPAndSuspendByProcessID(DWORD& nProcessID, DWORD& pnAddress, size_t nSize)
{
	return LingYinPerfTool::CheckEIPAndSuspendByProcessID(nProcessID, pnAddress, nSize);
}

LYCOMMONAPI HRESULT RusumeByProcessID(DWORD& nProcessID)
{
	return LingYinPerfTool::RusumeByProcessID(nProcessID);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}