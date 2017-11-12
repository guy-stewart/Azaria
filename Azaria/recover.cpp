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
#include "stdapp.h"
#include <xfile.h>

#include <recover.h>
#include "display.h"

char missing_file_path1[_MAX_PATH];
char missing_file_path2[_MAX_PATH];
char missing_file_name[_MAX_FNAME];

class LOGGER : XFile {
  int flushcount;
public:
	LOGGER::LOGGER()	
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

void WarnUser( LPSTR fmt, ... )
{
    char buff[256];
	int rc;

    wvsprintf( buff, fmt, (char*)(&fmt+1) );
    lstrcat( buff, "\r\n" );

	display.Disable(TRUE);
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

}

void errorCriticalStop(char * msg)
{
	Msg(msg);
	display.Disable(TRUE);
	MessageBox(NULL, msg, msg, MB_OK);
	exit(2);
}

#ifdef DEBUG
/*
 * Msg
 */
void __cdecl Msg( LPSTR fmt, ... )
{
    char        buff[256];

    wvsprintf( buff, fmt, (char*)(&fmt+1));
    lstrcat( buff, "\n" );
    OutputDebugString( buff );

	logger.Writeln(buff);
} /* Msg */

void __cdecl _Msg( LPSTR fmt, ... )
{
    char        buff[256];
    wvsprintf( buff, fmt, (char*)(&fmt+1));
    OutputDebugString( buff );
	logger.Writeln(buff);
}
#endif


