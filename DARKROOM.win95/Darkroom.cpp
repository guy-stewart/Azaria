// darkroom.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "darkroom.h"

#include "mainfrm.h"
#include "OneDoc.h"
#include "OneView.h"
#include "imgdoc.h"
#include "imgview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDarkroomApp

BEGIN_MESSAGE_MAP(CDarkroomApp, CWinApp)
	//{{AFX_MSG_MAP(CDarkroomApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDarkroomApp construction
// Place all significant initialization in InitInstance

CDarkroomApp::CDarkroomApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDarkroomApp object

CDarkroomApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CDarkroomApp initialization

BOOL CDarkroomApp::InitInstance()
{
	// Standard initialization
	//  (if you are not using these features and wish to reduce the size
	// of your final executable, you should remove the following initialization

	Enable3dControls();     // Use 3d controls in dialogs
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU=10)

	// Register document templates which serve as connection between
	//  documents and views.  Views are contained in the specified view

	AddDocTemplate(new CMultiDocTemplate(IDR_IMGTYPE,
			RUNTIME_CLASS(CImgDoc),
			RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
			RUNTIME_CLASS(CImgView)));

	AddDocTemplate(new CMultiDocTemplate(IDR_ONETYPE,
			RUNTIME_CLASS(COneDoc),
			RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
			RUNTIME_CLASS(COneView)));

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	m_pMainWnd = pMainFrame;

	// enable file manager drag/drop and DDE Execute open
	m_pMainWnd->DragAcceptFiles();

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

#ifdef _MAC

BOOL CDarkroomApp::CreateInitialDocument()
{
	// do *not* create a new (empty) document, but do return TRUE since no
	// failure occurred

	return TRUE;
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg() : CDialog(CAboutDlg::IDD)
		{
			//{{AFX_DATA_INIT(CAboutDlg)
			//}}AFX_DATA_INIT
		}

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDarkroomApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDarkroomApp commands
