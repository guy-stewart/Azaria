/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
; 
; File:       maya.h
; Description:    Common include files for application.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __MAYA_INCLUDED__
#define __MAYA_INCLUDED__

#include "includes.h"

#include "tables.h"
#include "spr_pool.h"
#include "spirit.h"

/*
 * fn prototypes
 */
DWORD VideoThread(LPVOID lParam);
int oneFixPal(ONE *one);
BOOL makeFontStuff( void );

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) "):" str

#endif
