#include "stdafx.h"
#include "TestDebug.h"
#include "DebugThread.h"
#include <imagehlp.h>
#include "TestDebugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugThread

IMPLEMENT_DYNCREATE(CDebugThread, CWinThread)

CDebugThread::CDebugThread()
{
}

CDebugThread::~CDebugThread()
{
}

BOOL CDebugThread::InitInstance()
{
    return TRUE;
}

int CDebugThread::ExitInstance()
{
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDebugThread, CWinThread)
    //{{AFX_MSG_MAP(CDebugThread)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// 启动监视进程
int CDebugThread::Run()
{
    STARTUPINFO st = {0};
    PROCESS_INFORMATION pro = {0};
    st.cb = sizeof(st);
    DEBUG_EVENT dbe;
    BOOL rc;

    CreateProcess(NULL, (LPTSTR)(LPCTSTR)m_pszCmd, NULL, NULL, FALSE,
                  DEBUG_ONLY_THIS_PROCESS,
                  NULL, NULL, &st, &pro);

    CloseHandle(pro.hThread);
    CloseHandle(pro.hProcess);

    while (WaitForDebugEvent(&dbe, INFINITE)) {
        // 如果是退出消息，调试监视结束
        if (dbe. dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
            break;
        // 进入调试监视处理
        rc = OnDebugEvent(&dbe);
        if (rc)
            ContinueDebugEvent(dbe.dwProcessId , dbe.dwThreadId , DBG_CONTINUE );
        else
            ContinueDebugEvent(dbe.dwProcessId , dbe.dwThreadId ,
                               DBG_EXCEPTION_NOT_HANDLED);
    }

    m_pOwner->DebugEnd();
    return 0;
}

// 调试消息处理程序
BOOL CDebugThread::OnDebugEvent(DEBUG_EVENT *pEvent)
{
    BOOL rc = TRUE;

    switch (pEvent->dwDebugEventCode) {
    case CREATE_PROCESS_DEBUG_EVENT:
        m_hDebug = pEvent->u.CreateProcessInfo.hProcess;
        // 记录线程ID和线程句柄的关系
        m_gthreads[pEvent->dwThreadId] = pEvent->u.CreateProcessInfo.hThread;
        break;

    case CREATE_THREAD_DEBUG_EVENT:
        // 记录线程ID和线程句柄的关系
        m_gthreads [pEvent->dwThreadId] = pEvent->u.CreateThread.hThread;
        break;

    case EXIT_THREAD_DEBUG_EVENT:
        // 线程退出时清除线程ID
        m_gthreads.erase (pEvent->dwThreadId);
        break;

    case EXCEPTION_DEBUG_EVENT:
        // 中断处理程序
        rc = OnDebugException(pEvent);
        break;

    case LOAD_DLL_DEBUG_EVENT: {
        char szDllName[1024];

        if (GetDLLName(pEvent, szDllName))
            AnalysisFunction(pEvent->u.CreateProcessInfo.hFile, (DWORD)pEvent->u.LoadDll.lpBaseOfDll, szDllName);
    }
    break;
    }
    return rc;

}

// 中断处理程序
BOOL CDebugThread::OnDebugException(DEBUG_EVENT *pEvent)
{
    void* pBreakAdd = pEvent->u.Exception.ExceptionRecord.ExceptionAddress;
    CONTEXT ct;
    HANDLE hThread;
    DWORD code = pEvent->u.Exception.ExceptionRecord.ExceptionCode;

    switch (code) {
    // 断点中断
    case EXCEPTION_BREAKPOINT: {
        // 得到线程句柄
        hThread = m_gthreads[pEvent->dwThreadId];
        // 得到线程上下文
        ct.ContextFlags = CONTEXT_FULL;
        GetThreadContext(hThread, &ct);
        // 这里，你可以根据断点地址从你的函数管理表中知道你相应的函数发生了中断。
        if (m_gFunBreaks.find((DWORD)pBreakAdd) != m_gFunBreaks.end()) {
            // 函数入口的断点
            LPBYTE callMem = NULL;
            DWORD dwRead;
            // 取得入口参数
            int nMemLen = 4 * 4; // 缺省取得4个参数， 你可以修改这个值
            callMem = (LPBYTE)malloc(nMemLen);
            // 取得参数值
            ReadProcessMemory(m_hDebug, (void*)(ct.Esp + 4), callMem, nMemLen, &dwRead);

            // 这里就可以记录函数的输入参数。

            // DisplayInputParams(callMem, nMemLen);

            CString s;
            DWORD* pMem = (DWORD*)callMem;

            s.Format("BreakPoint at 0x%08X : params = 0x%08X 0x%08X 0x%08X 0x%08X", pBreakAdd, *pMem,
                     *(pMem + 1), *(pMem + 2), *(pMem + 3));
            m_pOwner->m_result.AddString(s);

            // 取得函数入口的原代码

            // 暂停所有其他线程，保证单步执行不被其他线程中断

            SuspendAllThread(pEvent->dwThreadId);

            // Eip指针减1，指向原来代码的运行处

            ct.Eip--;

            // 设置单步运行

            ct.EFlags |= 0x00100;
            m_gSingleThreads[pEvent->dwThreadId] = pBreakAdd;

            // 将原来的代码写回

            SetBreakPoint((DWORD)pBreakAdd, m_gFunBreaks[(DWORD)pBreakAdd]);

            // 设置线程的上下文信息

            SetThreadContext(hThread, &ct);


            // 设置返回地址的断点，保证返回时可以得到输出参数和返回值

            void* pCallBackAdd;
            DWORD buf;
            // 得到返回地址
            ReadProcessMemory(m_hDebug, (void*)ct.Esp, &buf, sizeof(buf), &dwRead);
            pCallBackAdd = (void*)buf;
            if ((DWORD)pCallBackAdd < 0x80000000) {
                RETURN_FUN rtFun;

                // 保存输入参数地址

                rtFun.pFunPoint = pBreakAdd;
                rtFun. nMemLen = min(nMemLen, sizeof(rtFun.params));
                memcpy(rtFun. params, callMem, nMemLen);
                if (m_gReturnMap.find((DWORD)pCallBackAdd) == m_gReturnMap.end()) {

                    // 第一次设置返回断点

                    RETURN_CODE rtCode;

                    rtCode.code = SetBreakPoint((DWORD)pCallBackAdd, 0xCC);
                    m_gReturnMap[(DWORD)pCallBackAdd] = rtCode;
                }
                // 得到函数返回的地址的管理表
                RETURN_CODE* pRtCode = &m_gReturnMap[(DWORD)pCallBackAdd];
                RETURN_FUN_MAP*  pMap = &pRtCode->pMap;

                // 得到线程关联的函数返回堆栈管理表

                if (pMap->find(pEvent->dwThreadId) == pMap->end()) {
                    RETURN_FUN_STACK rs;
                    (*pMap)[pEvent->dwThreadId] = rs;
                }
                // 保存函数输入参数至函数返回管理表
                RETURN_FUN_STACK * pRs = &(*pMap)[pEvent->dwThreadId];
                pRs->push(rtFun);
            }
            // 释放内存
            if (callMem)
                free(callMem);
            return true;
        }

        // 函数返回时产生的断点中断信息

        // 这里可以得到函数运行结果及输出参数

        if (m_gReturnMap.find((DWORD)pBreakAdd) != m_gReturnMap.end()) {
            // 恢复函数返回处的断点
            RETURN_CODE* pRtCode = &m_gReturnMap[(DWORD)pBreakAdd];
            SetBreakPoint((DWORD)pBreakAdd, pRtCode->code);
            if (pRtCode->pMap.find(pEvent->dwThreadId) != pRtCode->pMap.end()) {
                // 得到函数返回值
                DWORD dwRc = ct.Eax;

                // 取出函数的输入参数地址

                // 这里你不能通过esp+4来取得函数的输入参数。在这里esp+4的值

                //  不一定是函数的输入参数，只有函数入口时刻的esp＋4才代表

                //  输入参数，所以在函数入口处必须将输入参数的指针保存起来

                //   目的就是为了取得输出参数时利用。

                // 你可以通过ReadProcessMemory函数取得输出参数值

                RETURN_FUN_STACK * pRs = &pRtCode->pMap[pEvent->dwThreadId];
                RETURN_FUN p = pRs->top();

                // 记录输出参数和函数运行结果

                // DisplayOutputParam(dwRc, &p);

                CString s;

                s.Format("*Return Fun rc = 0x%08X  Fun = 0x%08X", dwRc, p.pFunPoint);
                m_pOwner->m_result.AddString(s);

                // 删除处理完了的堆栈
                pRs->pop();

                // 该线程上是否还有函数返回的断点，如果没有，清除关联情报

                if (pRs->empty())
                    pRtCode->pMap.erase(pEvent->dwThreadId);
                // 调整运行指针
                ct.Eip--;
                // 检查该返回地址上是否还有其他函数设置了断点
                if (pRtCode->pMap.empty() == false) {
                    // 如果有，必须设置单步运行
                    ct.EFlags |= 0x00100;
                    m_gSingleThreads[pEvent->dwThreadId] = pBreakAdd;
                    SuspendAllThread(pEvent->dwThreadId);
                } else
                    // 释放关联管理表
                    m_gReturnMap.erase((DWORD)pBreakAdd);
                SetThreadContext(hThread, &ct);
                return true;
            }
        }
    }
    break;

    // 单步中断时的处理，重新设置断点
    case EXCEPTION_SINGLE_STEP:
        // 得到单步断点的指针
        if (m_gSingleThreads.find((DWORD)pEvent->dwThreadId) != m_gSingleThreads.end()) {
            // 重新设置断点
            SetBreakPoint((DWORD)m_gSingleThreads[pEvent->dwThreadId], 0xCC);
            m_gSingleThreads.erase(pEvent->dwThreadId);
        }
        ResumeAllThread(pEvent->dwThreadId);
        break;
    default:
        return false;
    }
    return true;

}

BYTE CDebugThread::SetBreakPoint(DWORD pAdd, BYTE code)
{
    BYTE b;
    BOOL rc;
    DWORD dwRead, dwOldFlg;
    // 0x80000000以上的地址为系统共有区域，不可以修改
    if ( pAdd >= 0x80000000 || pAdd == 0)
        return code;
    // 取得原来的代码
    rc = ReadProcessMemory(m_hDebug, (void*)pAdd, &b, sizeof(BYTE), &dwRead);
    // 原来的代码和准备修改的代码相同，没有必要再修改
    if (rc == 0 || b == code)
        return code;
    // 修改页码保护属性
    VirtualProtectEx(m_hDebug, (void*)pAdd, sizeof(unsigned char), PAGE_READWRITE, &dwOldFlg);
    // 修改目标代码
    WriteProcessMemory(m_hDebug, (void*)pAdd, &code, sizeof(unsigned char), &dwRead);
    // 恢复页码保护属性
    VirtualProtectEx(m_hDebug, (void*)pAdd, sizeof(unsigned char), dwOldFlg, &dwOldFlg);
    return b;
}

// 分析导出表得到导出函数名和函数入口地址
void CDebugThread::AnalysisExportFunction(void *pAddBase, DWORD dwAddress, char* szDllName)
{
    PIMAGE_DOS_HEADER pdosHdr;
    PIMAGE_NT_HEADERS pntHdr;
    pdosHdr = (PIMAGE_DOS_HEADER)pAddBase;
    // 检查是不是PE文件
    if (pdosHdr->e_magic != IMAGE_DOS_SIGNATURE)
        return;
    // 计算偏移量，跳过DOS检查代码
    pntHdr = (PIMAGE_NT_HEADERS)(pdosHdr->e_lfanew + (DWORD)pAddBase);
    if (pntHdr->Signature != IMAGE_NT_SIGNATURE)
        return;
    PIMAGE_EXPORT_DIRECTORY pExport;
    DWORD dwSize;
    // 得到导出表的地址
    pExport = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(pAddBase, false,
              IMAGE_DIRECTORY_ENTRY_EXPORT, &dwSize);
    // 有没有导出表
    if (pExport == NULL)
        return ;
    DWORD pExportBegin, pExportEnd;
    // 得到导出表的在内存中的地址和大小
    pExportBegin = (DWORD)pntHdr->OptionalHeader.DataDirectory[
                       IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    pExportEnd = pExportBegin + dwSize;
    PWORD pOrdFun;
    // 得到函数名指针
    char**  spName = (char**)ImageRvaToVa(pntHdr, pAddBase, pExport->AddressOfNames, 0);
    // 得到函数的入口地址指针
    PDWORD pFun = (PDWORD)ImageRvaToVa(pntHdr, pAddBase,
                                       pExport->AddressOfFunctions, 0);
    // 得到函数顺序表指针
    pOrdFun  = (PWORD)ImageRvaToVa(pntHdr, pAddBase, pExport->AddressOfNameOrdinals, 0);
    if (pFun == 0)
        return;

    HTREEITEM hDllItem = m_pOwner->m_funList.InsertItem(szDllName);
    // 得到函数名及函数入口地址
    for (int i = 0; i < pExport->NumberOfFunctions; i++) {
        char* pName = 0;
        // 得到函数入口指针
        DWORD pFunPoint = *(pFun + i);
        if (pFunPoint == 0)
            continue;
        // DLL的Forword函数不处理
        if (pFunPoint > pExportBegin && pFunPoint < pExportEnd)
            continue;
        // 根据顺序号得到函数名
        for (int j = 0; j < pExport->NumberOfNames; j++) {
            if (*(pOrdFun + j) == i) {
                pName = (char*)ImageRvaToVa(pntHdr, pAddBase, (ULONG) * (spName + j), 0);
                break;
            }
        }

        // 这里就得到了函数的名称和函数入口地址

        // 其中pName就是函数名称（如果pName不为0表示函数有名称）

        // 如果pName==0表明函数只有顺序号，没有名称

        // pFunPoint就是函数的相对入口地址

        // dwAddress + pFunPoint就是函数在内存的入口地址

        // 这里，你同样应该用将函数名和函数入口地址管理起来。

        //  AddToMyDllList(pMyFunManager, pName, dwAddress + pFunPoint);

        if (pName) {
            CString s;
            s.Format("%s - 0x%08X", pName, dwAddress + pFunPoint);
            HTREEITEM hItem = m_pOwner->m_funList.InsertItem(s, 0, 0, hDllItem);
            m_pOwner->m_funList.SetItemData(hItem, dwAddress + pFunPoint);
        }

    }

}

void CDebugThread::AnalysisFunction(HANDLE hFile, DWORD dwAddress, char* szDllName)
{
    if (dwAddress >= 0x80000000)
        return;

    HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, 0);
    if (hMap == 0) { // 失败
        CloseHandle(hFile);
        return;
    }
    LPVOID pMap = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    if (pMap) {
        AnalysisExportFunction(pMap, dwAddress, szDllName);
    }
    UnmapViewOfFile(pMap);
    CloseHandle(hMap);
    CloseHandle(hFile);
}

// 暂停其他线程
void CDebugThread::SuspendAllThread(DWORD dwThreadId)
{
    THREAD_MAP::iterator it;
    for (it = m_gthreads.begin(); it != m_gthreads.end(); it++) {
        // 自身以外所有的线程都暂停
        if ((*it).first != dwThreadId)
            ::SuspendThread((*it).second);
    }

}

//  恢复其他线程
void CDebugThread::ResumeAllThread(DWORD dwThreadId)
{
    THREAD_MAP::iterator it;
    for (it = m_gthreads.begin(); it != m_gthreads.end(); it++) {

        // 自身以外所有的线程都恢复

        if ((*it).first != dwThreadId)
            ::ResumeThread ((*it).second);
    }
}

BOOL CDebugThread::GetDLLName(DEBUG_EVENT *pEvent, char *szDllName)
{
    DWORD dwRead = 0;
    DWORD pDll = 0;
    // 没有文件名信息
    if (pEvent->u.LoadDll.lpImageName == NULL)
        return FALSE;
    // 读取目标进程的内容
    ReadProcessMemory(m_hDebug, pEvent->u.LoadDll.lpImageName, &pDll,
                      sizeof(DWORD), &dwRead);
    // 没有文件名信息
    if (pDll == NULL)
        return FALSE;

    dwRead = 0;
    int n = MAX_PATH;

    // 循环读取文件名，直到成功或n=0

    while (n) {
        if (ReadProcessMemory(m_hDebug, (LPVOID)pDll, szDllName, n, &dwRead)) {
            // 成功
            break;
        }
        n--;
    }
    if (n == 0)
        return FALSE;
    if (pEvent->u.LoadDll.fUnicode) {
        // 文件名为UNICODE码
        char temp[MAX_PATH] = {0};
        // 转换为ANSI码
        WideCharToMultiByte(CP_ACP, 0, (wchar_t*)szDllName, 256, temp, sizeof(temp), "", FALSE);
        strcpy(szDllName, temp);
    }
    return TRUE;
}

void CDebugThread::SetBreak(DWORD pAdd, BOOL bDebug)
{
    if (bDebug) {
        if (m_gFunBreaks.find(pAdd) != m_gFunBreaks.end())
            return;
        m_gFunBreaks[pAdd] = SetBreakPoint(pAdd, 0xCC);
        return;
    } else {
        if (m_gFunBreaks.find(pAdd) == m_gFunBreaks.end())
            return;
        SetBreakPoint(pAdd, m_gFunBreaks[pAdd]);
        m_gFunBreaks.erase(pAdd);
    }
}
