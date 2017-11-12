/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
; 
; File:       stdapp.h
; Description:    Standard (unchanging) include files for application.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __STDAPP_INCLUDED__
#define __STDAPP_INCLUDED__

#define DEBUG

#define WIN32_LEAN_AND_MEAN
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h> 
#include <mmsystem.h>

#include <ddraw.h>
#include <dsound.h>
#include <dplay.h>
#endif

#include "xfile.h"

#include "recover.h"
#include "rect.h"

#endif
