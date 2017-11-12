/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       user.c
;
; Description:
;	Processes all user input.
;
;	1.  Buffer mouse events so that high resolution devices do not flood the net.
;	2.	Buffer user events so they can be processed 'between frames'.
;
;	### Things to do:
;	1.	Enable case-sensitivity in translation routine.
;
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/28/96  gas  Created.
----------------------------------------------------------------------------*/

#include "includes.h"
#include "user.h"
#include "gameman.h"

USER user;

typedef struct {
	int type;
	POINT pos;
	void(*func)(POINT p);
} USER_ACTION;

static USER_ACTION action;

#define USER_MOUSE 1

//----------------------------------------------------------------------------
void userMouseMove(POINT p)
{
	user.mousePos = p;
	user.keyControl = FALSE;
	gameman.MouseMove(p);
}

//----------------------------------------------------------------------------
void userRButtonDown(POINT p)
{
	gameman.RButtonDown(p);
}

//----------------------------------------------------------------------------
void userRButtonUp(POINT p)
{
	gameman.RButtonUp(p);
}

//----------------------------------------------------------------------------
void viewLButtonDown(POINT p)
{
	gameman.LButtonDown(p);
}

//----------------------------------------------------------------------------
void viewLButtonUp(POINT p)
{
	gameman.LButtonUp(p);
}

//----------------------------------------------------------------------------
void userLButtonDown(POINT p)
{
	action.func = viewLButtonDown;
	action.pos = p;
	action.type = USER_MOUSE;
}

//----------------------------------------------------------------------------
void userLButtonUp(POINT p)
{
	action.func = viewLButtonUp;
	action.pos = p;
	action.type = USER_MOUSE;
}

//----------------------------------------------------------------------------
void User()
{
	if (action.type == 0)
		return;

	switch (action.type)
	{
	case USER_MOUSE:
  		user.keyControl = FALSE;
		(*action.func)(action.pos);
		break;
	}
	action.type = 0;
}

//----------------------------------------------------------------------------
// translate virtual keys used in the game:
int userTranslateKey(int key)
{
	user.keyControl = FALSE;

#ifdef _MSC_VER // Windows
	// do some translation on the input:
	switch( key )
	{
	case VK_NUMPAD5:
	case VK_CLEAR:
	  key = KEY_CLEAR; // num pad 5 w/num-lock off.
	  break;
	case VK_NUMPAD2:
	case VK_DOWN:
	  key = KEY_DOWN;
	  break;
	case VK_NUMPAD4:
	case VK_LEFT:
	  key = KEY_LEFT;
	  break;
	case VK_NUMPAD6:
	case VK_RIGHT:
	  key=KEY_RIGHT;
	  break;
	case VK_NUMPAD8:
	case VK_UP:
	  key = KEY_UP;
	  break;
	case VK_NUMPAD7:
	case VK_HOME:
	  key = KEY_HOME;
	  break;
	// Don't handle the virtual message for these keys.
	  // rather, wait until the translated key message comes through.
	case VK_RETURN:
	case VK_BACK:
	default:
	  key = 0;
	}
#endif
    
	return key;
}

//----------------------------------------------------------------------------
int userKeyPressed(int key, BOOL bVirtual)
{
  if (bVirtual)
  {
    int t_key;

	t_key = userTranslateKey(key);
	if (!t_key)
	  return key;
	else
	  key = t_key;
  }

  return gameman.KeyPressed(key);
}

//----------------------------------------------------------------------------
int userKeyReleased(int key, BOOL bVirtual)
{
  if (bVirtual)
  {
    int t_key;

	t_key = userTranslateKey(key);
	if (!t_key)
	  return key;
	else
	  key = t_key;
  }

  return gameman.KeyReleased(key);
}
