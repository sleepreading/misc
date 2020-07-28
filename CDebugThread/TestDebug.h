
#if !defined(AFX_TESTDEBUG_H__C561124D_DA3F_4CE8_87B3_A50A60D958DA__INCLUDED_)
#define AFX_TESTDEBUG_H__C561124D_DA3F_4CE8_87B3_A50A60D958DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTestDebugApp:
//

class CTestDebugApp : public CWinApp
{
public:
    CTestDebugApp();

    //{{AFX_VIRTUAL(CTestDebugApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL


    //{{AFX_MSG(CTestDebugApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TESTDEBUG_H__C561124D_DA3F_4CE8_87B3_A50A60D958DA__INCLUDED_)
