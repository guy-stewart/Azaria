/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; 
; File:       gameman.cpp
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "maya.h"

#ifndef __DDX_INCLUDED__
#define __DDX_INCLUDED__

#include "palette.h"

// Game Manager.
class GameManager {
public: // interface
	GameManager(){};
	~GameManager(){};
};

extern GameManager game;	// The one and only game manager.
