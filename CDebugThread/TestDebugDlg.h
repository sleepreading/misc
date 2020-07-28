// TestDebugDlg.h :  interface of the CTestDebugDlg class
//

#if !defined(AFX_TESTDEBUGDLG_H__D4796EDD_1F54_4EAD_81D4_947249AE47C4__INCLUDED_)
#define AFX_TESTDEBUGDLG_H__D4796EDD_1F54_4EAD_81D4_947249AE47C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestDebugDlg Dialog

#include "DebugThread.h"

class CTestDebugDlg : public CDialog
{

public:
    void DebugEnd();
    CTestDebugDlg(CWnd* pParent = NULL);
    virtual BOOL IsFrameWnd() const;
    // Dialog Data
    //{{AFX_DATA(CTestDebugDlg)
    enum { IDD = IDD_TESTDEBUG_DIALOG };
    CTreeCtrl	m_funList;
    CListBox	m_result;
    CString	m_file;
    //}}AFX_DATA

    // ClassWizard
    //{{AFX_VIRTUAL(CTestDebugDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV
    //}}AFX_VIRTUAL

    // Implementation
protected:
    HICON m_hIcon;

    //{{AFX_MSG(CTestDebugDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnOK();
    afx_msg void OnClickTreeFun(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CDebugThread* m_pThread;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TESTDEBUGDLG_H__D4796EDD_1F54_4EAD_81D4_947249AE47C4__INCLUDED_)
