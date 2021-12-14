/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       XFile.h
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 05/10/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifndef __XFILE_INCLUDED__
#define __XFILE_INCLUDED__

#include "../opensource/fcaseopen.h"
#include "ptypes.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#include <io.h>
#include <stdio.h>
//#include <wtypes.h>
#include "stdapp.h"
#include <strings.h>
#include <libgen.h>
#include <cstring>

void splitpath(const char *path, char * drive, char * dir, char * fname, char * ext);
void makepath(char *path, const char * drive, const char * dir, const char * fname, const char * ext);

// Kind of like CFile
class XFile {
public:
	enum SeekFlags {
		Begin = SEEK_SET,
		Current = SEEK_CUR,
		End = SEEK_END
	};

	enum OpenFlags {
		modeRead =          O_RDONLY | O_BINARY,
		modeWrite =         O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
		modeReadWrite =     O_RDWR | O_BINARY | O_CREAT | O_TRUNC,
		modeWriteText =     O_WRONLY | O_TEXT | O_CREAT | O_TRUNC,

	/*
		shareCompat =       0x0000,
		shareExclusive =    0x0010,
		shareDenyWrite =    0x0020,
		shareDenyRead =     0x0030,
		shareDenyNone =     0x0040,
		modeNoInherit =     0x0080,
		modeCreate =        0x1000,
		modeNoTruncate =    0x2000,
	
		typeText =          0x4000, // typeText and typeBinary are used in
		typeBinary =   (int)0x8000 // derived classes only
	*/
		};

private:
	int m_fh;
	long m_length;
	char m_path[_MAX_PATH];
public:
	XFile(const char *path = NULL)
	{
		m_fh=-1; 
		m_length=0;
		if (path && *path)
			strcpy(m_path, path);
		else
			m_path[0] = 0;
	}

	~XFile() {if (m_fh >=0) {close(m_fh);}}
	BOOL Open(const char *path, UINT nOpenFlags, int pmode = S_IREAD|S_IWRITE )
	{
		m_fh = caseopen(path, nOpenFlags, pmode);

		if (m_fh != -1)
		{
		   return TRUE;
		}
		return FALSE;
	}

	BOOL Open(UINT nOpenFlags, int pmode = S_IREAD|S_IWRITE )
	{
		m_fh = caseopen(m_path, nOpenFlags, pmode);

		if (m_fh != -1)
		{
		   return TRUE;
		}
		return FALSE;
	}

	inline ssize_t Read( void* lpBuf, UINT nCount )
	{ return read(m_fh, lpBuf, nCount); }

	inline ssize_t Write( void* lpBuf, UINT nCount )
	{ return write(m_fh, lpBuf, nCount); }

	void flush()
	{fsync(m_fh);}

	inline off_t GetLength()
	{
		m_length = SeekToEnd();
		SeekToBegin();
		return m_length;
	}

	inline int GetHandle() 
	{return m_fh;}

	inline off_t GetPosition()
	{return lseek(m_fh, 0L, SEEK_CUR);}

	inline long Seek(long pos, int origin )
	{return lseek(m_fh, pos, origin);}

	inline void SeekToBegin()
    {lseek(m_fh, 0L, SEEK_SET);}

	inline off_t SeekToEnd()
	{return lseek(m_fh, 0L, SEEK_END);}

	void SetPath(const char *path)
	{strcpy(m_path, path);}
    
    const char * GetPath()
    {return m_path;}

	void SetExtension(const char *ext)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		splitpath(m_path, drive, dir, fname, NULL);
		makepath(m_path, drive, dir, fname, ext);
	}
};

#endif // __XFILE_INCLUDED__
