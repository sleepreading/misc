#if !defined(AFX_DEBUGTHREAD_H__384097D9_0FC9_4E19_AB21_20A88841589C__INCLUDED_)
#define AFX_DEBUGTHREAD_H__384097D9_0FC9_4E19_AB21_20A88841589C__INCLUDED_

#pragma warning(disable:4786)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugThread.h : Head File
//
#include <map>
#include <stack>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDebugThread 线程


// 利用一个map进行管理线程ID和线程句柄之间的关系
// 同时也用一个map管理函数地址和断点的关系

typedef map<DWORD, HANDLE, less<DWORD> > THREAD_MAP;
typedef map<DWORD, BYTE, less<DWORD> > FUN_BREAK_MAP;
typedef struct RETURN_FUN {
    DWORD params[64];
    void* pFunPoint;
    int nMemLen;
} RETURN_FUN, *LPRETURN_FUN;

typedef stack< RETURN_FUN > RETURN_FUN_STACK;
typedef map<DWORD, RETURN_FUN_STACK, less<DWORD>,
        allocator< RETURN_FUN_STACK> > RETURN_FUN_MAP;
typedef struct RETURN_CODE {
    BYTE code;
    RETURN_FUN_MAP pMap;
} RETURN_CODE;
typedef map<DWORD, RETURN_CODE, less< DWORD >, allocator< RETURN_CODE > >FUN_RETURN_BREAK_MAP;

// 单步地址管理表
typedef map<DWORD, void*, less<DWORD> >SINGLE_THREAD;

class CTestDebugDlg;

class CDebugThread : public CWinThread
{
    DECLARE_DYNCREATE(CDebugThread)
protected:
    CDebugThread();

public:

public:
    void SetBreak(DWORD pAdd, BOOL bDebug);
    CString m_pszCmd;
    CTestDebugDlg* m_pOwner;

    //{{AFX_VIRTUAL(CDebugThread)
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual int Run();
    //}}AFX_VIRTUAL

protected:
    BOOL GetDLLName(DEBUG_EVENT* pEvent, char* szDllName);
    void ResumeAllThread(DWORD dwThreadId);
    void SuspendAllThread(DWORD dwThreadId);
    void AnalysisFunction(HANDLE hFile, DWORD dwAddress, char* szDllName);
    void AnalysisExportFunction(void* pAddBase, DWORD dwAddress, char* szDllName);
    BYTE SetBreakPoint(DWORD pAdd, BYTE code);
    BOOL OnDebugEvent(DEBUG_EVENT* pEvent);
    BOOL OnDebugException(DEBUG_EVENT* pEvent);
    virtual ~CDebugThread();

    //{{AFX_MSG(CDebugThread)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

private:
    HANDLE m_hDebug;
    THREAD_MAP m_gthreads;
    FUN_BREAK_MAP m_gFunBreaks;
    FUN_RETURN_BREAK_MAP m_gReturnMap;
    SINGLE_THREAD  m_gSingleThreads;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DEBUGTHREAD_H__384097D9_0FC9_4E19_AB21_20A88841589C__INCLUDED_)
 