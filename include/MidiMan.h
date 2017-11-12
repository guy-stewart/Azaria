/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       MidiMan.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 04/12/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __MIDIMAN_INCLUDED__
#define __MIDIMAN_INCLUDED__

typedef int MIDIBUFF;

//----------------------------------------------------------------------------
// Simple little MIDI interface ...
class MIDIMan
{
public:
	enum {
		DONTLOOP = 0,
		LOOP = 1
	};

  // Public Constructor(s)/Destructor
public:
	MIDIMan();
	~MIDIMan();

public: // Interface:
	// returns handle to MIDI stream if it could find && play the file ... (-1 == failed)
  MIDIBUFF Play(const char *sFileName, BOOL loop = FALSE);

  BOOL Stop(MIDIBUFF hMidi);
  BOOL Pause(MIDIBUFF hMidi);
  BOOL Restart(MIDIBUFF hMidi);

  // Get/Set MIDI volume:
	// Volume
	// Address of the location to contain the current volume setting. 
	// The low-order word of this location contains the left-channel 
	// volume setting, and the high-order word contains the right-channel 
	// setting. A value of 0xFFFF represents full volume, and a value of 
	// 0x0000 is silence.
	// If a device does not support both left and right volume control, 
	// the low-order word of the specified location contains the mono volume level.
	// Any value set by using the midiOutSetVolume function is returned, 
	// regardless of whether the device supports that value.
  
  BOOL SetVolume(DWORD volume);
  DWORD GetVolume();

  // Test to see if it is a valid midi handle:
  BOOL valid(MIDIBUFF hMidi);

private:
	DWORD m_Volume;
	int m_max;		// how many files can be loaded
};

#endif