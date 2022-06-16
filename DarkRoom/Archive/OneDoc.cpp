// OneDoc.cpp : implementation of the COneDoc class
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

#include "stdafx.h"
#include "darkroom.h"
#include <limits.h>
#include <stdio.h>

#include "OneDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COneDoc

IMPLEMENT_DYNCREATE(COneDoc, CImgDoc)

BEGIN_MESSAGE_MAP(COneDoc, CImgDoc)
	//{{AFX_MSG_MAP(COneDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COneDoc construction/destruction
COneDoc::COneDoc()
{
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1,1);     // dummy value to make CScrollView happy
}

/////////////////////////////////////////////////////////////////////////////
// COneDoc diagnostics

#ifdef _DEBUG
void COneDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COneDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COneDoc commands
