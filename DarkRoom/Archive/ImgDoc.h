// imgdoc.h : interface of the CImgDoc class
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
#ifndef __IMGDOC_INCLUDED__
#define __IMGDOC_INCLUDED__

#include "imgapi.h"

class CImgDoc : public CDocument
{
protected: // create from serialization only
	CImgDoc();
	DECLARE_DYNCREATE(CImgDoc)

// Attributes
public:
	HDIB GetHDIB() const
		{ return m_hDIB; }
	CPalette* GetDocPalette() const
		{ return m_palDIB; }
	CSize GetDocSize() const
		{ return m_sizeDoc; }
	CSize GetPageSize() const;

	DWORD GetFrames() const
		{ return m_frames; }
	void SetFrames(DWORD frames)
		{ m_frames = frames;}

	BYTE GetFP10S() const
		{ return m_fp10s; }
	void SetFP10S(BYTE fp10s)
		{ m_fp10s = fp10s;}

// Operations
public:
	void ReplaceHDIB(HDIB hDIB);
	void InitDIBData();
	void BuildSequence(BOOL bFlipDiag, BOOL bAscending);
	BOOL FlipDiag(BOOL bLeft);
	void FlipVert();

// Implementation
protected:
	virtual ~CImgDoc();

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

protected:
	HDIB m_hDIB;
	DWORD m_frames;	// number of frames in animation sequence.
	BYTE m_fp10s;
	CPalette* m_palDIB;
	CSize m_sizeDoc;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual BOOL    OnNewDocument();

// Generated message map functions
protected:
	//{{AFX_MSG(CImgDoc)
	afx_msg void OnGeneratePortvector();
	afx_msg void OnGenBts();
	afx_msg void OnCustomOne();
	afx_msg void OnCompressAnimation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////


