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

#include "../pch.h"

HRESULT LingYinPerfTool::GetPidFromAppName(
	LPWSTR& pszAppProcessName,
	DWORD& nProcessId
)
{
	nProcessId = 0;
	BOOL bCloseHandleState = FALSE;
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
			nProcessId = pe.th32ProcessID;
			bCloseHandleState = CloseHandle(hSnapshot);
			break;
		}
	}
	if (nProcessId != 0) 
	{
		return S_OK;
	}
	return E_FAIL;
}