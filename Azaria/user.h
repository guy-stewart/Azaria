/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
; File:       user.h
;
; Description:
;	Process all user actions.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "ptypes.h"

void User(); // process any user actions that affect the display.

void userMouseMove(POINT p);
void userMouseClick();
void userRButtonDown(POINT p);
void userRButtonUp(POINT p);
void userLButtonDown(POINT p);
void userLButtonUp(POINT p);

int userKeyPressed(int key, BOOL bVirtual);
int userKeyReleased(int key, BOOL bVirtual);

typedef struct {
	POINT mousePos;	  // always set to current mouse position
	BOOL keyControl;  // TRUE if last thing hit was key, FALSE if mouse moved.
	int current_view; // The current view the mouse pointer is on.
} USER;

extern USER user;

#define KEY_BACK	  0x08
#define KEY_RETURN	  0x0d

#define KEY_UP		  207
#define KEY_DOWN	  208
#define KEY_LEFT	  209
#define KEY_RIGHT	  210
#define KEY_CLEAR	  211
#define KEY_HOME	  212
