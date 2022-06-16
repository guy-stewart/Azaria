// ImgDoc.cpp : implementation of the CImgDoc class
//

#include "stdafx.h"
#include "darkroom.h"
#include <limits.h>
#include <stdio.h>

#include "imgdoc.h"
#include <one.h>

HDIB FlipDibVert(HDIB a);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void WarnUser( LPSTR fmt, ... )
{
    char buff[256];
	int rc;

    wvsprintf( buff, fmt, (char*)(&fmt+1) );
    lstrcat( buff, "\r\n" );

	rc = MessageBox(NULL, buff, "Warning", MB_ABORTRETRYIGNORE);
	switch (rc)
	{
	case IDABORT:
		break;
	case IDRETRY:
		break;
	case IDIGNORE:
		break;
	}

}

/////////////////////////////////////////////////////////////////////////////
// CImgDoc

IMPLEMENT_DYNCREATE(CImgDoc, CDocument)

BEGIN_MESSAGE_MAP(CImgDoc, CDocument)
	//{{AFX_MSG_MAP(CImgDoc)
	ON_COMMAND(ID_GENERATE_PORTVECTOR, OnGeneratePortvector)
	ON_COMMAND(ID_GEN_BTS, OnGenBts)
	ON_COMMAND(IDD_CUSTOM_ONE, OnCustomOne)
	ON_COMMAND(IDC_COMPRESSED, OnCompressAnimation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgDoc construction/destruction

CImgDoc::CImgDoc()
{
	m_frames = 1;
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1,1);     // dummy value to make CScrollView happy
}

//----------------------------------------------------------------------------
CImgDoc::~CImgDoc()
{
	if (m_hDIB != NULL)
	{
		::GlobalFree((HGLOBAL) m_hDIB);
	}
	if (m_palDIB != NULL)
	{
		delete m_palDIB;
	}
}

//----------------------------------------------------------------------------
BOOL CImgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

//----------------------------------------------------------------------------
void CImgDoc::InitDIBData()
{
	if (m_palDIB != NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}
	if (m_hDIB == NULL)
	{
		return;
	}
	// Set up document size
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	if (::DIBWidth(lpDIB) > INT_MAX ||::DIBHeight(lpDIB) > INT_MAX)
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);
		m_hDIB = NULL;
		CString strMsg;
		strMsg.LoadString(IDS_DIB_TOO_BIG);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
		return;
	}
	m_sizeDoc = CSize((int) ::DIBWidth(lpDIB), (int) ::DIBHeight(lpDIB));
	::GlobalUnlock((HGLOBAL) m_hDIB);
	// Create copy of palette
	m_palDIB = new CPalette;
	if (m_palDIB == NULL)
	{
		// we must be really low on memory
		::GlobalFree((HGLOBAL) m_hDIB);
		m_hDIB = NULL;
		return;
	}
	if (::CreateDIBPalette(m_hDIB, m_palDIB) == NULL)
	{
		// DIB may not have a palette
		delete m_palDIB;
		m_palDIB = NULL;
		return;
	}
}


char * GetExtension(LPCTSTR lpszPathName);
#define SetExtension(PATH, EXT) makepathext((char*)PATH, PATH, EXT)
char * makepathext(char * lpszNewPath, LPCTSTR lpszPathName, LPCTSTR lpszNewExt);

//----------------------------------------------------------------------------
BOOL CImgDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!strcmp(GetExtension(lpszPathName), ".one"))
	{
		ONE one;
		DeleteContents();
		if (one.Read((char*)lpszPathName))
		{
			PALETTE *readpal = NULL;
			PALETTEENTRY *pal = one.GetPalette();

			if (!pal)
			{
				readpal = (PALETTE*)malloc(sizeof(PALETTE));
				ReadPalFile(NULL, "gui.pal", readpal);
				pal = readpal->ape;
			}

			if (one.IsCompressed())
			{
				one.UnCompress();
			}

			m_frames = one.GetFrames();
			m_fp10s = one.GetFP10S();

			m_hDIB = CreateDIB((LPSTR)one.getBits(), pal, one.GetHeight(), one.GetWidth(), one.GetPitch());

			if (readpal)
				free(readpal);

			if (!m_hDIB)
				return FALSE;
			else
				FlipDibVert(m_hDIB);
		}
		else
			return FALSE;
	}
	else
	{
		if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
		{
			ReportSaveLoadException(lpszPathName, &fe,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
			return FALSE;
		}

		DeleteContents();
		BeginWaitCursor();

		// replace calls to Serialize with ReadDIBFile function
		TRY
		{
			m_hDIB = ::ReadDIBFile(file);
		}
		CATCH (CFileException, eLoad)
		{
			file.Abort(); // will not throw an exception
			EndWaitCursor();
			ReportSaveLoadException(lpszPathName, eLoad,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
			m_hDIB = NULL;
			return FALSE;
		}
		END_CATCH
	}

	InitDIBData();
	EndWaitCursor();

	if (m_hDIB == NULL)
	{
		// may not be DIB format
		CString strMsg;
		strMsg.LoadString(IDS_CANNOT_LOAD_DIB);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);     // start off with unmodified
	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CImgDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!strcmp(GetExtension(lpszPathName), ".one"))
	{
		SetExtension(lpszPathName, ".bmp");
	}

	if (!file.Open(lpszPathName, CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		ReportSaveLoadException(lpszPathName, &fe,
			TRUE, AFX_IDP_INVALID_FILENAME);
		return FALSE;
	}

	// replace calls to Serialize with SaveDIB function
	BOOL bSuccess = FALSE;
	TRY
	{
		BeginWaitCursor();
		bSuccess = ::SaveDIB(m_hDIB, file);
		file.Close();
	}
	CATCH (CException, eSave)
	{
		file.Abort(); // will not throw an exception
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eSave,
			TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
		return FALSE;
	}
	END_CATCH

	EndWaitCursor();
	SetModifiedFlag(FALSE);     // back to unmodified

	if (!bSuccess)
	{
		// may be other-style DIB (load supported but not save)
		//  or other problem in SaveDIB
		CString strMsg;
		strMsg.LoadString(IDS_CANNOT_SAVE_DIB);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
	}

	return bSuccess;
}


//----------------------------------------------------------------------------
void CImgDoc::ReplaceHDIB(HDIB hDIB)
{
	if (m_hDIB != NULL)
	{
		::GlobalFree((HGLOBAL) m_hDIB);
	}
	m_hDIB = hDIB;
}

/////////////////////////////////////////////////////////////////////////////
// CImgDoc diagnostics

#ifdef _DEBUG
void CImgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

//----------------------------------------------------------------------------
char * GetExtension(LPCTSTR lpszPathName)
{
   static char ext[_MAX_FNAME];

   _splitpath( lpszPathName, NULL, NULL, NULL, ext );

   return ext;
}	

//----------------------------------------------------------------------------
char * makepathext(char * lpszNewPath, LPCTSTR lpszPathName, LPCTSTR lpszNewExt)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];

   _splitpath( lpszPathName, drive, dir, fname, NULL );
   _makepath( lpszNewPath, drive, dir, fname, lpszNewExt);

   return lpszNewPath;
}	

//----------------------------------------------------------------------------
// take a name like bob7339 and add 1 to it resulting in bob7340
void addone(char *name)
{
	int x;

	for (x = strlen(name) - 1;x>=0;x--)
	{
		if (name[x] >= '0' && name[x] <= '9')
		{
			name[x]++;
			if (name[x] > '9')
				name[x] = '0'; // need to carry,  keep going
			else
				break; // done,  no carry
		}
		else
		{
			name[x]++;
			if (name[x] > 'Z')
				name[x] = 'a'; // need to carry,  keep going
			else
				break; // done,  no carry
		}
	}
}

//----------------------------------------------------------------------------
char * nextfilename(char *lpszFileName)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath( lpszFileName, drive, dir, fname, ext );

   addone(fname);

   _makepath( lpszFileName, drive, dir, fname, ext );

   return lpszFileName;
}	

//----------------------------------------------------------------------------
int writeBmpToCustomOnefile(LPCTSTR path, char *ext, unsigned char *bits, int cx, int cy, int pitch,  PALETTEENTRY *palette)
{
	char szOneFile[_MAX_PATH];

	makepathext(szOneFile, path, ext);

	return oneWriteCustomBMP(szOneFile, bits, cx, cy, pitch, palette);
}

/////////////////////////////////////////////////////////////////////////////
// CImgDoc commands
int GenPortVect(
	LPCTSTR lpszPathName, 
	LPSTR lpDIB, // pointer to image bits
	int cx, // width of image
	int cy,
	int pitch) // height of image
{
	MASK *pvect = maskCreate((unsigned char *)lpDIB, cy, cx, pitch, lpDIB[0]);
	maskWrite(lpszPathName, pvect);
	maskFree(&pvect);
	return 1;
}

//----------------------------------------------------------------------------
void CImgDoc::OnGeneratePortvector() 
{
	// TODO: Add your command handler code here
	char szOneFile[_MAX_PATH];
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

	LPSTR lpDIBBits = FindDIBBits(lpDIB);

	makepathext(szOneFile, GetPathName(), "vct");

	int pitch;

	// do this to flip the image
	FlipDibVert(m_hDIB);

	pitch = SizeDIBBITS(m_hDIB)/m_sizeDoc.cy;

	GenPortVect(szOneFile, (char*)lpDIBBits, m_sizeDoc.cx, m_sizeDoc.cy, pitch);

	FlipDibVert(m_hDIB);

	::GlobalUnlock((HGLOBAL) m_hDIB);
}

//----------------------------------------------------------------------------
void CImgDoc::OnGenBts() 
{
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	LPSTR lpDIBBits = FindDIBBits(lpDIB);

	if (DIBBitCount(lpDIB) != 8)
	{
		  ::MessageBox(NULL, "I can only write 256 color one files", "Dear Idiot", 0);
		  return;
	}

	oneWriteBMP(
		GetPathName(), 
		(unsigned char *)lpDIBBits, 
		FALSE, 
		m_sizeDoc.cx, 
		m_sizeDoc.cy, 
		SizeDIBBITS(m_hDIB)/m_sizeDoc.cy,
		m_frames,
		m_fp10s);

	::GlobalUnlock((HGLOBAL) m_hDIB);
}

//----------------------------------------------------------------------------
void CImgDoc::OnCustomOne() 
{
	PALETTEENTRY pal[256];

	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	LPSTR lpDIBBits = FindDIBBits(lpDIB);

	if (DIBBitCount(lpDIB) != 8)
	{
		  ::MessageBox(NULL, "I can only write 256 color one files", "Dear Idiot", 0);
		  return;
	}

	CPalette* pPal = GetDocPalette();

	pPal->GetPaletteEntries(0, 256, pal);

	writeBmpToCustomOnefile(
		GetPathName(),
		"one", 
		(unsigned char *)lpDIBBits, 
		m_sizeDoc.cx, 
		m_sizeDoc.cy, 
		SizeDIBBITS(m_hDIB)/m_sizeDoc.cy,
		pal);

	::GlobalUnlock((HGLOBAL) m_hDIB);
}

//----------------------------------------------------------------------------
typedef struct {
	BYTE red;
	BYTE grn;
	BYTE blu;
} RGBSET;

void FlipDiagTrueBits(BYTE *_dst, BYTE *_src, DWORD height, DWORD width, DWORD new_pitch, DWORD old_pitch)
{
	DWORD w, h;
	RGBSET *src;
	RGBSET *dst;

	dst = (RGBSET*)(_dst);

	for (h=0;h<height; h++)
	{
		src = (RGBSET*)(_src+(height-(h+1))*old_pitch);

		for (w=0;w<width;w++)
		{
			dst[w*height+h] = src[width-(w+1)];
		}
	}
}

//----------------------------------------------------------------------------
void FlipDiagBits(BYTE *dst, BYTE *_src, DWORD height, DWORD width, DWORD new_pitch, DWORD old_pitch)
{
	DWORD w, h;
	BYTE *src;

	for (h=0;h<height; h++)
	{
		src = _src+(height-(h+1))*old_pitch;

		for (w=0;w<width;w++)
		{
			dst[w*height+h] = src[width-(w+1)];
		}	
	}
}

//----------------------------------------------------------------------------
BOOL CImgDoc::FlipDiag(BOOL bLeft)
{
	int x;
	HDIB hDIB = NULL; // handle to the new DIB
	LPSTR pDIB;
	LPSTR lpBits;

	LPSTR lpA = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	LPSTR lpABits = FindDIBBits(lpA);

	int old_pitch = DIBPitch(lpA);
	int new_pitch = DIBHeight(lpA);
	new_pitch += new_pitch%4;

	DWORD dwNewSize;

	x = SizeDIB(m_hDIB);
	x = SizeDIBBITS(m_hDIB);
	x = DIBHeight(lpA);
	x = DIBWidth(lpA);

	dwNewSize = SizeDIB(m_hDIB);

	/*
	 * Allocate memory for NEW DIB ...
	 */
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwNewSize);
	if (hDIB == 0)
		goto noram;

	// lock new dib memory:
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	// copy the header:
	memcpy(pDIB, lpA, SizeDIB(m_hDIB)-SizeDIBBITS(m_hDIB));

	// get a pointer to the bits to rotate the image onto:
	lpBits = FindDIBBits(pDIB);

	// Rotate the bits back into the original bit array:
	x = DIBBitCount(lpA);
	switch (x)
	{
	  case 0:
	  case 24:
		  FlipDiagTrueBits((BYTE*)lpBits, (BYTE*)lpABits, DIBHeight(lpA), DIBWidth(lpA), new_pitch, old_pitch);
		  break;
	  case 8:
		  FlipDiagBits((BYTE*)lpBits, (BYTE*)lpABits, DIBHeight(lpA), DIBWidth(lpA), new_pitch, old_pitch);
		  break;
	  default:
		{
		  char colors[32];
		  sprintf(colors, "image is %d bits", DIBBitCount(lpA));
		  ::MessageBox(NULL, "Not a true color or 256 color image", colors, 0);
		  return FALSE;
		}
	}

	// flip the height & width parms of the DIB:
	FlipDIB_H_W(pDIB);

	::GlobalUnlock((HGLOBAL) hDIB);
	::GlobalUnlock((HGLOBAL) m_hDIB);

	ReplaceHDIB(hDIB);

	return TRUE;
noram:
	::GlobalUnlock((HGLOBAL) m_hDIB);

	return FALSE;
}

//----------------------------------------------------------------------------
HDIB FlipDibVert(HDIB hDib)
{
	BYTE *temp;

	LPSTR lpA = (LPSTR) ::GlobalLock((HGLOBAL) hDib);
	BYTE *lpABits = (BYTE*)FindDIBBits(lpA);

	int i, a, b;
	int p = DIBPitch(lpA);
	int h = DIBHeight(lpA);
	int w = DIBWidth(lpA);
	int len;

	/*
	 * Allocate memory for NEW DIB ...
	 */
	temp = (BYTE*) malloc(w);
	if (temp == 0)
		goto noram;

	for (i=0;i<h/2;i++)
	{
		a = p*i;
		b = p*(h-(i+1));
		len = w;
		// Rotate the bits back into the original bit array:
		if (!DIBNumColors(lpA)) // true color image (no colors in palette.
		{
			a *= sizeof(RGBSET);
			b *= sizeof(RGBSET);
			len *= sizeof(RGBSET);
		}
		memcpy(temp, lpABits+a, len);
		memcpy(lpABits+a, lpABits+b, len);
		memcpy(lpABits+b, temp, len);
	}

	free(temp);

noram:
	::GlobalUnlock((HGLOBAL) hDib);

	return hDib;
}

//----------------------------------------------------------------------------
HDIB AppendDIB(HDIB a, HDIB b)
{
	int x;
	HDIB hDIB = NULL; // handle to the new DIB
	LPSTR pDIB;
	LPSTR lpBits;

	LPSTR lpA = (LPSTR) ::GlobalLock((HGLOBAL) a);
	LPSTR lpABits = FindDIBBits(lpA);

	LPSTR lpB = (LPSTR) ::GlobalLock((HGLOBAL) b);
	LPSTR lpBBits = FindDIBBits(lpB);

	DWORD dwDibSize;

	x = SizeDIB(a);
	x = SizeDIB(b);
	x = SizeDIBBITS(a);
	x = SizeDIBBITS(b);
	x = DIBHeight(lpA);
	x = DIBHeight(lpB);
	x = DIBWidth(lpA);
	x = DIBWidth(lpB);

	dwDibSize = SizeDIB(a) + SizeDIBBITS(b);

	if (DIBWidth(lpA) != DIBWidth(lpB))
	{ 
		::MessageBox(NULL, "bitmaps are of different widths", "", 0);
		return NULL;
	}

	/*
	 * Allocate memory for NEW DIB ...
	 */
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwDibSize);
	if (hDIB == 0)
		goto noram;

	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	memcpy(pDIB, lpA, SizeDIB(a)-SizeDIBBITS(a));

	lpBits = FindDIBBits(pDIB);
	memcpy(lpBits, lpABits, SizeDIBBITS(a));

	memcpy(lpBits+SizeDIBBITS(a), lpBBits, SizeDIBBITS(b));

	AddDIBHeight(pDIB, DIBHeight(lpB));

	::GlobalUnlock((HGLOBAL) hDIB);

noram:
	::GlobalUnlock((HGLOBAL) a);
	::GlobalUnlock((HGLOBAL) b);

	return hDIB;
}

//----------------------------------------------------------------------------
CSize CImgDoc::GetPageSize() const
{
	CSize size;
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

    if (m_hDIB == NULL) {
        size.cx = 1;
        size.cy = 1;
        return size;
    }

	size.cx = DIBWidth(lpDIB);

	if (m_frames)
	  size.cy = DIBHeight(lpDIB) / m_frames;
	else
	  size.cy = DIBHeight(lpDIB);

	::GlobalUnlock((HGLOBAL) m_hDIB);

	return size;
}

//----------------------------------------------------------------------------
// if bFlipDiag is true, then the images are flipped diagonally before attaching.
// if bAscending is true, then the images are attached to the bottom,
// otherwise they are attached to the top.
void CImgDoc::BuildSequence(BOOL bFlipDiag, BOOL bAscending) 
{
	int i = 0;
	HDIB hDIB;
	CImgDoc	nextframe;

	char nextfile[_MAX_PATH];

	if (CompressedDIB(m_hDIB))
	{
		MessageBox(NULL, "I Can't deal with compressed bitmaps!", GetPathName(), 0);
		return;
	}

	if (bFlipDiag)
	{
		if (!FlipDiag(TRUE))
		  return;
	}

	strcpy(nextfile, GetPathName());
	nextfilename(nextfile);

	while (nextframe.OnOpenDocument(nextfile))
	{
		if (CompressedDIB(nextframe.m_hDIB))
		{
			::MessageBox(NULL, "I Can't deal with compressed bitmaps!", nextframe.GetPathName(), 0);
			return;
		}

		if (bFlipDiag)
		{
			if (!nextframe.FlipDiag(TRUE))
			  return;
		}

		if (bAscending)
			hDIB = AppendDIB(nextframe.m_hDIB, m_hDIB);
		else
			hDIB = AppendDIB(m_hDIB, nextframe.m_hDIB);

		if (hDIB)
		{
			ReplaceHDIB(hDIB);
			InitDIBData();
			m_frames++;
		}
		else
		{
			::MessageBox(NULL, "Error appending file to Animation", nextfile, 0);
			return;
		}

		nextfilename(nextfile);
		if (i++>=800)
		{
			::MessageBox(NULL, "Can't do that many files,  stopping at %s", nextfile, 0);
			break;
		}
	}
	UpdateAllViews(NULL);
}

//----------------------------------------------------------------------------
void CImgDoc::FlipVert() 
{
	FlipDibVert(m_hDIB);
}


//----------------------------------------------------------------------------
void CImgDoc::OnCompressAnimation() 
{
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	LPSTR lpDIBBits = FindDIBBits(lpDIB);

	if (DIBBitCount(lpDIB) != 8)
	{
		  ::MessageBox(NULL, "I can only write 256 color one files", "Dear Idiot", 0);
		  return;
	}

	FlipDibVert(m_hDIB);

	oneWriteBMP(
		GetPathName(),
		(unsigned char *)lpDIBBits,
		TRUE, // Compress the images
		m_sizeDoc.cx,
		m_sizeDoc.cy,
		SizeDIBBITS(m_hDIB)/m_sizeDoc.cy,
		m_frames,
		m_fp10s
		);

	FlipDibVert(m_hDIB);

	::GlobalUnlock((HGLOBAL) m_hDIB);
}
