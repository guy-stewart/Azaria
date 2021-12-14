/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       error.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 02/03/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#include "includes.h"
#include "display.h"
#include <stdio.h>
#include <stdarg.h>

char missing_file_path1[_MAX_PATH];
char missing_file_path2[_MAX_PATH];
char missing_file_name[_MAX_FNAME];

class LOGGER : XFile {
  int flushcount;
public:
    LOGGER()
	{
		Open("mayalog.txt",  modeWriteText);
	}
	void Writeln(char *msg) 
	{
		Write(msg, strlen(msg));
		if (flushcount++ > 25)
		{
		  flush();
		  flushcount=0;
		}
	}
};

LOGGER logger;

void WarnUser( const LPSTR fmt, ... )
{
    char buff[256];

    va_list args;
    va_start (args, fmt);
    
    vsprintf( buff, fmt, args );
    strcat( buff, "\r\n" );

	display.Disable(TRUE);
    assert(false);
#ifdef _WIN32
    int rc;
	rc = MessageBox(NULL, buff, buff, MB_ABORTRETRYIGNORE);
	switch (rc)
	{
	case IDABORT:
		errorCriticalStop(buff);
		break;
	case IDRETRY:
		break;
	case IDIGNORE:
		break;
	}
#endif
}

void __cdecl Msg( const LPSTR fmt, ... );

void errorCriticalStop(const char * msg)
{
	Msg(msg);
	display.Disable(TRUE);
    assert(false);
#ifdef _WIN32
	MessageBox(NULL, msg, msg, MB_OK);
#endif
	exit(2);
}

#ifdef DEBUG
/*
 * Msg
 */
void OutputDebugString(const char * buff)
{
    printf("%s", buff);
}

void __cdecl Msg( const LPSTR fmt, ... )
{
    char        buff[256];
    va_list args;
    va_start (args, fmt);
    
    vsprintf( buff, fmt, args);
    strcat( buff, "\n" );
    OutputDebugString( buff );

	logger.Writeln(buff);
} /* Msg */

void __cdecl _Msg( const LPSTR fmt, ... )
{
    char        buff[256];
    va_list args;
    va_start (args, fmt);
    vsprintf( buff, fmt, args);
    OutputDebugString( buff );
	logger.Writeln(buff);
}
#endif


