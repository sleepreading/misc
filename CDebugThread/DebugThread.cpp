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
// �������ӽ���
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
        // ������˳���Ϣ�����Լ��ӽ���
        if (dbe. dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
            break;
        // ������Լ��Ӵ���
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

// ������Ϣ�������
BOOL CDebugThread::OnDebugEvent(DEBUG_EVENT *pEvent)
{
    BOOL rc = TRUE;

    switch (pEvent->dwDebugEventCode) {
    case CREATE_PROCESS_DEBUG_EVENT:
        m_hDebug = pEvent->u.CreateProcessInfo.hProcess;
        // ��¼�߳�ID���߳̾���Ĺ�ϵ
        m_gthreads[pEvent->dwThreadId] = pEvent->u.CreateProcessInfo.hThread;
        break;

    case CREATE_THREAD_DEBUG_EVENT:
        // ��¼�߳�ID���߳̾���Ĺ�ϵ
        m_gthreads [pEvent->dwThreadId] = pEvent->u.CreateThread.hThread;
        break;

    case EXIT_THREAD_DEBUG_EVENT:
        // �߳��˳�ʱ����߳�ID
        m_gthreads.erase (pEvent->dwThreadId);
        break;

    case EXCEPTION_DEBUG_EVENT:
        // �жϴ������
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

// �жϴ������
BOOL CDebugThread::OnDebugException(DEBUG_EVENT *pEvent)
{
    void* pBreakAdd = pEvent->u.Exception.ExceptionRecord.ExceptionAddress;
    CONTEXT ct;
    HANDLE hThread;
    DWORD code = pEvent->u.Exception.ExceptionRecord.ExceptionCode;

    switch (code) {
    // �ϵ��ж�
    case EXCEPTION_BREAKPOINT: {
        // �õ��߳̾��
        hThread = m_gthreads[pEvent->dwThreadId];
        // �õ��߳�������
        ct.ContextFlags = CONTEXT_FULL;
        GetThreadContext(hThread, &ct);
        // �������Ը��ݶϵ��ַ����ĺ����������֪������Ӧ�ĺ����������жϡ�
        if (m_gFunBreaks.find((DWORD)pBreakAdd) != m_gFunBreaks.end()) {
            // ������ڵĶϵ�
            LPBYTE callMem = NULL;
            DWORD dwRead;
            // ȡ����ڲ���
            int nMemLen = 4 * 4; // ȱʡȡ��4�������� ������޸����ֵ
            callMem = (LPBYTE)malloc(nMemLen);
            // ȡ�ò���ֵ
            ReadProcessMemory(m_hDebug, (void*)(ct.Esp + 4), callMem, nMemLen, &dwRead);

            // ����Ϳ��Լ�¼���������������

            // DisplayInputParams(callMem, nMemLen);

            CString s;
            DWORD* pMem = (DWORD*)callMem;

            s.Format("BreakPoint at 0x%08X : params = 0x%08X 0x%08X 0x%08X 0x%08X", pBreakAdd, *pMem,
                     *(pMem + 1), *(pMem + 2), *(pMem + 3));
            m_pOwner->m_result.AddString(s);

            // ȡ�ú�����ڵ�ԭ����

            // ��ͣ���������̣߳���֤����ִ�в��������߳��ж�

            SuspendAllThread(pEvent->dwThreadId);

            // Eipָ���1��ָ��ԭ����������д�

            ct.Eip--;

            // ���õ�������

            ct.EFlags |= 0x00100;
            m_gSingleThreads[pEvent->dwThreadId] = pBreakAdd;

            // ��ԭ���Ĵ���д��

            SetBreakPoint((DWORD)pBreakAdd, m_gFunBreaks[(DWORD)pBreakAdd]);

            // �����̵߳���������Ϣ

            SetThreadContext(hThread, &ct);


            // ���÷��ص�ַ�Ķϵ㣬��֤����ʱ���Եõ���������ͷ���ֵ

            void* pCallBackAdd;
            DWORD buf;
            // �õ����ص�ַ
            ReadProcessMemory(m_hDebug, (void*)ct.Esp, &buf, sizeof(buf), &dwRead);
            pCallBackAdd = (void*)buf;
            if ((DWORD)pCallBackAdd < 0x80000000) {
                RETURN_FUN rtFun;

                // �������������ַ

                rtFun.pFunPoint = pBreakAdd;
                rtFun. nMemLen = min(nMemLen, sizeof(rtFun.params));
                memcpy(rtFun. params, callMem, nMemLen);
                if (m_gReturnMap.find((DWORD)pCallBackAdd) == m_gReturnMap.end()) {

                    // ��һ�����÷��ضϵ�

                    RETURN_CODE rtCode;

                    rtCode.code = SetBreakPoint((DWORD)pCallBackAdd, 0xCC);
                    m_gReturnMap[(DWORD)pCallBackAdd] = rtCode;
                }
                // �õ��������صĵ�ַ�Ĺ����
                RETURN_CODE* pRtCode = &m_gReturnMap[(DWORD)pCallBackAdd];
                RETURN_FUN_MAP*  pMap = &pRtCode->pMap;

                // �õ��̹߳����ĺ������ض�ջ�����

                if (pMap->find(pEvent->dwThreadId) == pMap->end()) {
                    RETURN_FUN_STACK rs;
                    (*pMap)[pEvent->dwThreadId] = rs;
                }
                // ���溯������������������ع����
                RETURN_FUN_STACK * pRs = &(*pMap)[pEvent->dwThreadId];
                pRs->push(rtFun);
            }
            // �ͷ��ڴ�
            if (callMem)
                free(callMem);
            return true;
        }

        // ��������ʱ�����Ķϵ��ж���Ϣ

        // ������Եõ��������н�����������

        if (m_gReturnMap.find((DWORD)pBreakAdd) != m_gReturnMap.end()) {
            // �ָ��������ش��Ķϵ�
            RETURN_CODE* pRtCode = &m_gReturnMap[(DWORD)pBreakAdd];
            SetBreakPoint((DWORD)pBreakAdd, pRtCode->code);
            if (pRtCode->pMap.find(pEvent->dwThreadId) != pRtCode->pMap.end()) {
                // �õ���������ֵ
                DWORD dwRc = ct.Eax;

                // ȡ�����������������ַ

                // �����㲻��ͨ��esp+4��ȡ�ú��������������������esp+4��ֵ

                //  ��һ���Ǻ��������������ֻ�к������ʱ�̵�esp��4�Ŵ���

                //  ��������������ں�����ڴ����뽫���������ָ�뱣������

                //   Ŀ�ľ���Ϊ��ȡ���������ʱ���á�

                // �����ͨ��ReadProcessMemory����ȡ���������ֵ

                RETURN_FUN_STACK * pRs = &pRtCode->pMap[pEvent->dwThreadId];
                RETURN_FUN p = pRs->top();

                // ��¼��������ͺ������н��

                // DisplayOutputParam(dwRc, &p);

                CString s;

                s.Format("*Return Fun rc = 0x%08X  Fun = 0x%08X", dwRc, p.pFunPoint);
                m_pOwner->m_result.AddString(s);

                // ɾ���������˵Ķ�ջ
                pRs->pop();

                // ���߳����Ƿ��к������صĶϵ㣬���û�У���������鱨

                if (pRs->empty())
                    pRtCode->pMap.erase(pEvent->dwThreadId);
                // ��������ָ��
                ct.Eip--;
                // ���÷��ص�ַ���Ƿ����������������˶ϵ�
                if (pRtCode->pMap.empty() == false) {
                    // ����У��������õ�������
                    ct.EFlags |= 0x00100;
                    m_gSingleThreads[pEvent->dwThreadId] = pBreakAdd;
                    SuspendAllThread(pEvent->dwThreadId);
                } else
                    // �ͷŹ��������
                    m_gReturnMap.erase((DWORD)pBreakAdd);
                SetThreadContext(hThread, &ct);
                return true;
            }
        }
    }
    break;

    // �����ж�ʱ�Ĵ����������öϵ�
    case EXCEPTION_SINGLE_STEP:
        // �õ������ϵ��ָ��
        if (m_gSingleThreads.find((DWORD)pEvent->dwThreadId) != m_gSingleThreads.end()) {
            // �������öϵ�
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
    // 0x80000000���ϵĵ�ַΪϵͳ�������򣬲������޸�
    if ( pAdd >= 0x80000000 || pAdd == 0)
        return code;
    // ȡ��ԭ���Ĵ���
    rc = ReadProcessMemory(m_hDebug, (void*)pAdd, &b, sizeof(BYTE), &dwRead);
    // ԭ���Ĵ����׼���޸ĵĴ�����ͬ��û�б�Ҫ���޸�
    if (rc == 0 || b == code)
        return code;
    // �޸�ҳ�뱣������
    VirtualProtectEx(m_hDebug, (void*)pAdd, sizeof(unsigned char), PAGE_READWRITE, &dwOldFlg);
    // �޸�Ŀ�����
    WriteProcessMemory(m_hDebug, (void*)pAdd, &code, sizeof(unsigned char), &dwRead);
    // �ָ�ҳ�뱣������
    VirtualProtectEx(m_hDebug, (void*)pAdd, sizeof(unsigned char), dwOldFlg, &dwOldFlg);
    return b;
}

// ����������õ������������ͺ�����ڵ�ַ
void CDebugThread::AnalysisExportFunction(void *pAddBase, DWORD dwAddress, char* szDllName)
{
    PIMAGE_DOS_HEADER pdosHdr;
    PIMAGE_NT_HEADERS pntHdr;
    pdosHdr = (PIMAGE_DOS_HEADER)pAddBase;
    // ����ǲ���PE�ļ�
    if (pdosHdr->e_magic != IMAGE_DOS_SIGNATURE)
        return;
    // ����ƫ����������DOS������
    pntHdr = (PIMAGE_NT_HEADERS)(pdosHdr->e_lfanew + (DWORD)pAddBase);
    if (pntHdr->Signature != IMAGE_NT_SIGNATURE)
        return;
    PIMAGE_EXPORT_DIRECTORY pExport;
    DWORD dwSize;
    // �õ�������ĵ�ַ
    pExport = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(pAddBase, false,
              IMAGE_DIRECTORY_ENTRY_EXPORT, &dwSize);
    // ��û�е�����
    if (pExport == NULL)
        return ;
    DWORD pExportBegin, pExportEnd;
    // �õ�����������ڴ��еĵ�ַ�ʹ�С
    pExportBegin = (DWORD)pntHdr->OptionalHeader.DataDirectory[
                       IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    pExportEnd = pExportBegin + dwSize;
    PWORD pOrdFun;
    // �õ�������ָ��
    char**  spName = (char**)ImageRvaToVa(pntHdr, pAddBase, pExport->AddressOfNames, 0);
    // �õ���������ڵ�ַָ��
    PDWORD pFun = (PDWORD)ImageRvaToVa(pntHdr, pAddBase,
                                       pExport->AddressOfFunctions, 0);
    // �õ�����˳���ָ��
    pOrdFun  = (PWORD)ImageRvaToVa(pntHdr, pAddBase, pExport->AddressOfNameOrdinals, 0);
    if (pFun == 0)
        return;

    HTREEITEM hDllItem = m_pOwner->m_funList.InsertItem(szDllName);
    // �õ���������������ڵ�ַ
    for (int i = 0; i < pExport->NumberOfFunctions; i++) {
        char* pName = 0;
        // �õ��������ָ��
        DWORD pFunPoint = *(pFun + i);
        if (pFunPoint == 0)
            continue;
        // DLL��Forword����������
        if (pFunPoint > pExportBegin && pFunPoint < pExportEnd)
            continue;
        // ����˳��ŵõ�������
        for (int j = 0; j < pExport->NumberOfNames; j++) {
            if (*(pOrdFun + j) == i) {
                pName = (char*)ImageRvaToVa(pntHdr, pAddBase, (ULONG) * (spName + j), 0);
                break;
            }
        }

        // ����͵õ��˺��������ƺͺ�����ڵ�ַ

        // ����pName���Ǻ������ƣ����pName��Ϊ0��ʾ���������ƣ�

        // ���pName==0��������ֻ��˳��ţ�û������

        // pFunPoint���Ǻ����������ڵ�ַ

        // dwAddress + pFunPoint���Ǻ������ڴ����ڵ�ַ

        // �����ͬ��Ӧ���ý��������ͺ�����ڵ�ַ����������

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
    if (hMap == 0) { // ʧ��
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

// ��ͣ�����߳�
void CDebugThread::SuspendAllThread(DWORD dwThreadId)
{
    THREAD_MAP::iterator it;
    for (it = m_gthreads.begin(); it != m_gthreads.end(); it++) {
        // �����������е��̶߳���ͣ
        if ((*it).first != dwThreadId)
            ::SuspendThread((*it).second);
    }

}

//  �ָ������߳�
void CDebugThread::ResumeAllThread(DWORD dwThreadId)
{
    THREAD_MAP::iterator it;
    for (it = m_gthreads.begin(); it != m_gthreads.end(); it++) {

        // �����������е��̶߳��ָ�

        if ((*it).first != dwThreadId)
            ::ResumeThread ((*it).second);
    }
}

BOOL CDebugThread::GetDLLName(DEBUG_EVENT *pEvent, char *szDllName)
{
    DWORD dwRead = 0;
    DWORD pDll = 0;
    // û���ļ�����Ϣ
    if (pEvent->u.LoadDll.lpImageName == NULL)
        return FALSE;
    // ��ȡĿ����̵�����
    ReadProcessMemory(m_hDebug, pEvent->u.LoadDll.lpImageName, &pDll,
                      sizeof(DWORD), &dwRead);
    // û���ļ�����Ϣ
    if (pDll == NULL)
        return FALSE;

    dwRead = 0;
    int n = MAX_PATH;

    // ѭ����ȡ�ļ�����ֱ���ɹ���n=0

    while (n) {
        if (ReadProcessMemory(m_hDebug, (LPVOID)pDll, szDllName, n, &dwRead)) {
            // �ɹ�
            break;
        }
        n--;
    }
    if (n == 0)
        return FALSE;
    if (pEvent->u.LoadDll.fUnicode) {
        // �ļ���ΪUNICODE��
        char temp[MAX_PATH] = {0};
        // ת��ΪANSI��
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
