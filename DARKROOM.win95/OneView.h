// OneView.h : interface of the COneView class
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

#include "ImgView.h"

class COneView : public CImgView
{
protected: // create from serialization only
	COneView();
	DECLARE_DYNCREATE(COneView)

// Attributes
public:
	virtual CImgDoc* GetDocument()
		{
			ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COneDoc)));
			return (CImgDoc*) m_pDocument;
		}

// Operations
public:
/*
// Implementation
public:
	virtual ~COneView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);

	// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
*/
// Generated message map functions
protected:
	//{{AFX_MSG(COneView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
