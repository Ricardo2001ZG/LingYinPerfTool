// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "pch.h"

#define LYCOMMONAPI extern "C" __declspec(dllexport)

LYCOMMONAPI HRESULT GetPidFromAppName(LPWSTR& pszAppProcessName,DWORD& pnProcessID) 
{
    return LingYinPerfTool::GetPidFromAppName(pszAppProcessName, pnProcessID);
}

LYCOMMONAPI HRESULT GetFunctionAddressFromSymbol(DWORD& pnProcessID, LPWSTR& pszFunctionDllName, PCSTR pszFunctionName, ULONG64& pnFunctionAddress)
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

BOOL APIENTRY DllMain( HMODULE hModule,
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