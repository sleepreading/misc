// TestDebugDlg.cpp :
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

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    CRichEditCtrl	m_test;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Control(pDX, IDC_RICHEDIT_TEST, m_test);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDebugDlg ダイアログ

CTestDebugDlg::CTestDebugDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CTestDebugDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CTestDebugDlg)
    m_file = _T("notepad.exe");
    //}}AFX_DATA_INIT
    m_pThread = 0;
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BOOL CTestDebugDlg::IsFrameWnd() const
{
    if (m_hWnd == 0)
        return FALSE;
    return TRUE;
}

void CTestDebugDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestDebugDlg)
    DDX_Control(pDX, IDC_TREE_FUN, m_funList);
    DDX_Control(pDX, IDC_LIST_RESULT, m_result);
    DDX_Text(pDX, IDC_EDIT_FILE, m_file);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDebugDlg, CDialog)
    //{{AFX_MSG_MAP(CTestDebugDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(NM_CLICK, IDC_TREE_FUN, OnClickTreeFun)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CTestDebugDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_result.SetHorizontalExtent(600);

    return TRUE;
}

void CTestDebugDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialog::OnSysCommand(nID, lParam);
    }
}


void CTestDebugDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

HCURSOR CTestDebugDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CTestDebugDlg::OnOK()
{
    UpdateData();

    if (m_file.IsEmpty())
        return;

    if (m_pThread)
        return;

    m_funList.DeleteAllItems();
    m_result.ResetContent();

    m_pThread = (CDebugThread*)AfxBeginThread(RUNTIME_CLASS(CDebugThread), 0, 0, CREATE_SUSPENDED, 0);

    m_pThread->m_pszCmd = m_file;
    m_pThread->m_pOwner = this;
    m_pThread->ResumeThread();
    GetDlgItem(IDOK)->EnableWindow(FALSE);
}

void CTestDebugDlg::DebugEnd()
{
    m_pThread = 0;
    GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CTestDebugDlg::OnClickTreeFun(NMHDR* pNMHDR, LRESULT* pResult)
{
    POINT pt;
    HTREEITEM pNode;

    if (m_pThread == 0)
        return;
    GetCursorPos(&pt);
    m_funList.ScreenToClient(&pt);
    UINT nFlag;

    pNode = m_funList.HitTest(pt, &nFlag);
    if (pNode == NULL)
        return;
    if (nFlag & TVHT_ONITEMSTATEICON ) {
        DWORD pAdd = m_funList.GetItemData(pNode);
        if (pAdd) {
            if (m_funList.GetCheck(pNode))
                m_pThread->SetBreak(pAdd, FALSE);
            else
                m_pThread->SetBreak(pAdd, TRUE);
        }
    }
    *pResult = 0;
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();



    return TRUE;
}
