// ImgView.h : interface of the CImgView class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
#ifndef __IMGVIEW_INCLUDED__
#define __IMGVIEW_INCLUDED__

class CImgView : public CScrollView
{
protected: // create from serialization only
	CImgView();
	DECLARE_DYNCREATE(CImgView)

// Attributes
public:
	virtual CImgDoc* GetDocument()
		{
			ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgDoc)));
			return (CImgDoc*) m_pDocument;
		}

// Operations
public:

// Implementation
public:
	virtual ~CImgView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);

	// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// Generated message map functions
protected:
	//{{AFX_MSG(CImgView)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
	afx_msg void OnSetsize();
	afx_msg void OnAnimation();
	afx_msg void OnSetFrames();
	afx_msg void OnSuround();
	afx_msg void OnFlipdiag();
	afx_msg void OnFlipvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// frames dialog

class frames : public CDialog
{
// Construction
public:
	frames(CWnd* pParent = NULL);   // standard constructor

	void SetFP10S(BYTE fp10s)
		{ m_FramesPer10Secs = fp10s; }
	BYTE GetFP10S()
		{ return m_FramesPer10Secs; }

	void SetCount(DWORD frames)
		{ m_framecount = frames; }
	DWORD GetCount()
		{ return m_framecount; }

// Dialog Data
	//{{AFX_DATA(frames)
	enum { IDD = IDD_FRAMES };
	DWORD	m_framecount;
	BYTE	m_FramesPer10Secs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(frames)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(frames)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

