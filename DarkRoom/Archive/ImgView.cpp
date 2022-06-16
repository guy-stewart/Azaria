// ImgView.cpp : implementation of the CImgView class
//

#include "stdafx.h"
#include "darkroom.h"

#include "imgdoc.h"
#include "imgview.h"
#include "imgapi.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgView

IMPLEMENT_DYNCREATE(CImgView, CScrollView)

BEGIN_MESSAGE_MAP(CImgView, CScrollView)
	//{{AFX_MSG_MAP(CImgView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	ON_COMMAND(IDD_SETSIZE, OnSetsize)
	ON_COMMAND(IDD_ANIMATION, OnAnimation)
	ON_COMMAND(IDD_SET_FRAMES, OnSetFrames)
	ON_COMMAND(IDD_SUROUND, OnSuround)
	ON_COMMAND(IDD_FLIPDIAG, OnFlipdiag)
	ON_COMMAND(IDD_FLIPVERT, OnFlipvert)
	//}}AFX_MSG_MAP

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgView construction/destruction

CImgView::CImgView()
{
}

CImgView::~CImgView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CImgView drawing

void CImgView::OnDraw(CDC* pDC)
{
	CImgDoc* pDoc = GetDocument();

	HDIB hDIB = pDoc->GetHDIB();
	if (hDIB != NULL)
	{
		LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
		int cxDIB = (int) ::DIBWidth(lpDIB);         // Size of DIB - x
		int cyDIB = (int) ::DIBHeight(lpDIB);        // Size of DIB - y
		::GlobalUnlock((HGLOBAL) hDIB);
		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		CRect rcDest;
		if (pDC->IsPrinting())   // printer DC
		{
			// get size of printer page (in pixels)
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			int cyPage = pDC->GetDeviceCaps(VERTRES);
			// get printer pixels per inch
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

			//
			// Best Fit case -- create a rectangle which preserves
			// the DIB's aspect ratio, and fills the page horizontally.
			//
			// The formula in the "->bottom" field below calculates the Y
			// position of the printed bitmap, based on the size of the
			// bitmap, the width of the page, and the relative size of
			// a printed pixel (cyInch / cxInch).
			//
			rcDest.top = rcDest.left = 0;
			rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch)
					/ ((double)cxDIB * cxInch));
			rcDest.right = cxPage;
		}
		else   // not printer DC
		{
			rcDest = rcDIB;
		}
		::PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImgView printing

BOOL CImgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CImgView commands


LRESULT CImgView::OnDoRealize(WPARAM wParam, LPARAM)
{
	ASSERT(wParam != NULL);
	CImgDoc* pDoc = GetDocument();
	if (pDoc->GetHDIB() == NULL)
		return 0L;  // must be a new document

	CPalette* pPal = pDoc->GetDocPalette();
	if (pPal != NULL)
	{
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		ASSERT_KINDOF(CMainFrame, pAppFrame);

		CClientDC appDC(pAppFrame);
		// All views but one should be a background palette.
		// wParam contains a handle to the active view, so the SelectPalette
		// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
		CPalette* oldPalette = appDC.SelectPalette(pPal, ((HWND)wParam) != m_hWnd);

		if (oldPalette != NULL)
		{
			UINT nColorsChanged = appDC.RealizePalette();
			if (nColorsChanged > 0)
				pDoc->UpdateAllViews(NULL);
			appDC.SelectPalette(oldPalette, TRUE);
		}
		else
		{
			TRACE0("\tSelectPalette failed in CImgView::OnPaletteChanged\n");
		}
	}

	return 0L;
}

void CImgView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	ASSERT(GetDocument() != NULL);

	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());

	OnSetsize();
}

void CImgView::OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView)
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate)
	{
		ASSERT(pActivateView == this);
		OnDoRealize((WPARAM)m_hWnd, 0);   // same as SendMessage(WM_DOREALIZE);
	}
}

void CImgView::OnEditCopy()
{
	CImgDoc* pDoc = GetDocument();
	// Clean clipboard of contents, and copy the DIB.

	if (OpenClipboard())
	{
		BeginWaitCursor();
		EmptyClipboard();
		SetClipboardData (CF_DIB, CopyHandle((HANDLE) pDoc->GetHDIB()) );
		CloseClipboard();
		EndWaitCursor();
	}
}

void CImgView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetHDIB() != NULL);
}


void CImgView::OnEditPaste()
{
	HDIB hNewDIB = NULL;

	if (OpenClipboard())
	{
		BeginWaitCursor();

		hNewDIB = (HDIB) CopyHandle(::GetClipboardData(CF_DIB));

		CloseClipboard();

		if (hNewDIB != NULL)
		{
			CImgDoc* pDoc = GetDocument();
			pDoc->ReplaceHDIB(hNewDIB); // and free the old DIB
			pDoc->InitDIBData();    // set up new size & palette
			pDoc->SetModifiedFlag(TRUE);

			SetScrollSizes(MM_TEXT, pDoc->GetDocSize());
			OnDoRealize((WPARAM)m_hWnd,0);  // realize the new palette
			pDoc->UpdateAllViews(NULL);
		}
		EndWaitCursor();
	}
}

void CImgView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
}

void CImgView::OnSetsize() 
{
//	RECT rect;
	CImgDoc* pDoc = GetDocument();
	CSize size = pDoc->GetPageSize();

	pDoc->InitDIBData();    // set up new size & palette
	// pDoc->SetModifiedFlag(TRUE);
	SetScrollSizes(MM_TEXT, pDoc->GetDocSize(), size, size);
	// SetScrollSizes(MM_TEXT, pDoc->GetDocSize());
	OnDoRealize((WPARAM)m_hWnd,0);  // realize the new palette
	pDoc->UpdateAllViews(NULL);

/*  It would be nice to get this to work.  I was trying to get the window to
**	resize to the proper frame size so that exactly one frame was visible at
**	a time.

	SetRect(&rect, 0, 0, size.cx, size.cy);

	CalcWindowRect(&rect, CWnd::adjustOutside);

	SetWindowPos(0, 0, 0, rect.right, rect.bottom, SWP_NOZORDER|SWP_NOMOVE|SWP_SHOWWINDOW);
*/

}

void CImgView::OnAnimation() 
{
	CImgDoc* pDoc = GetDocument();

	pDoc->BuildSequence(FALSE, TRUE);

	OnSetsize();
}

void CImgView::OnSetFrames() 
{
	CImgDoc* pDoc = GetDocument();

	frames dlg;

	dlg.SetCount(pDoc->GetFrames());
	dlg.SetFP10S((BYTE)pDoc->GetFP10S());

	dlg.DoModal();

	pDoc->SetFrames(dlg.GetCount());
	pDoc->SetFP10S(dlg.GetFP10S());

	OnSetsize();
}

/////////////////////////////////////////////////////////////////////////////
// frames dialog
frames::frames(CWnd* pParent /*=NULL*/)
	: CDialog(frames::IDD, pParent)
{
	//{{AFX_DATA_INIT(frames)
	m_framecount = 0;
	m_FramesPer10Secs = 0;
	//}}AFX_DATA_INIT
}


void frames::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(frames)
	DDX_Text(pDX, IDC_FRAMECOUNT, m_framecount);
	DDV_MinMaxDWord(pDX, m_framecount, 1, 200);
	DDX_Text(pDX, IDC_FRAMEPER10SECS, m_FramesPer10Secs);
	DDV_MinMaxByte(pDX, m_FramesPer10Secs, 0, 250);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(frames, CDialog)
	//{{AFX_MSG_MAP(frames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// frames message handlers

void frames::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CImgView::OnSuround() 
{
	// TODO: Add your command handler code here
	CImgDoc* pDoc = GetDocument();

	pDoc->BuildSequence(TRUE, FALSE);

	OnSetsize();
}


void CImgView::OnFlipdiag() 
{
	// TODO: Add your command handler code here
	CImgDoc* pDoc = GetDocument();

	pDoc->FlipDiag(TRUE);

	OnSetsize();
}

void CImgView::OnFlipvert() 
{
	// TODO: Add your command handler code here
	CImgDoc* pDoc = GetDocument();

	pDoc->FlipVert();

	OnSetsize();
}
