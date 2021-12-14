/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       error.h
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
#pragma once

extern char missing_file_path1[_MAX_PATH];
extern char missing_file_path2[_MAX_PATH];
extern char missing_file_name[_MAX_FNAME];

#define __cdecl

void errorCriticalStop(const char * msg);
void WarnUser( const LPSTR fmt, ... );

extern void __cdecl Msg( const LPSTR fmt, ... );
