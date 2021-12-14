// OneView.cpp : implementation of the COneView class
//

#include "stdafx.h"
#include "darkroom.h"

#include "Onedoc.h"
#include "OneView.h"
#include "imgapi.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COneView

IMPLEMENT_DYNCREATE(COneView, CImgView)

BEGIN_MESSAGE_MAP(COneView, CImgView)
	//{{AFX_MSG_MAP(COneView)
	//}}AFX_MSG_MAP

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COneView construction/destruction

COneView::COneView()
{
}

/////////////////////////////////////////////////////////////////////////////
// COneView commands

