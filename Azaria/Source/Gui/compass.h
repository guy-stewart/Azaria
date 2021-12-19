/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       compass.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 05/31/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef  __COMPASS_INCLUDED__
#define  __COMPASS_INCLUDED__

#include "viewMast.h"

class COMPASS : public VIEW {
public:
    COMPASS(WORD view_id);
    ~COMPASS();

    virtual int Action(int etime, int flags);
    virtual WORD QueryCursor(SRECT p);
};


#endif
