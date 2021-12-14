// dibdoc.h : interface of the COneDoc class
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

#include "imgapi.h"
#include <one.h>
#include "ImgDoc.h"

class COneDoc : public CImgDoc
{
protected: // create from serialization only
	COneDoc();
	DECLARE_DYNCREATE(COneDoc)

// Attributes
public:
/*
	HDIB GetHDIB() const
		{ return m_hDIB; }
	CPalette* GetDocPalette() const
		{ return m_palDIB; }
	CSize GetDocSize() const
		{ return m_sizeDoc; }

// Operations
public:
	void ReplaceHDIB(HDIB hDIB);
	void InitDIBData();

// Implementation
protected:
	virtual ~COneDoc();

	//virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

protected:
	MASK *m_mask;
//	HDIB m_hDIB;
//	CPalette* m_palDIB;
//	CSize m_sizeDoc;
*/
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
//	virtual BOOL    OnNewDocument();

// Generated message map functions
protected:
	//{{AFX_MSG(COneDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
