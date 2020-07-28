// TestDebug.cpp : implementation of the CTestDebugApp class
//

#include "stdafx.h"
#include "TestDebug.h"
#include "TestDebugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestDebugApp

BEGIN_MESSAGE_MAP(CTestDebugApp, CWinApp)
    //{{AFX_MSG_MAP(CTestDebugApp)
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDebugApp Consruct

CTestDebugApp::CTestDebugApp()
{
}

/////////////////////////////////////////////////////////////////////////////

CTestDebugApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTestDebugApp::InitInstance

BOOL CTestDebugApp::InitInstance()
{
    AfxEnableControlContainer();
    AfxInitRichEdit();

#ifdef _AFXDLL
    Enable3dControls();
#else
    Enable3dControlsStatic();
#endif

    CTestDebugDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
