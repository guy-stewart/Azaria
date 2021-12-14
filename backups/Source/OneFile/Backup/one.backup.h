/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in V  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  V document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       one.h
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/23/95  gas  Created.
; 
----------------------------------------------------------------------------*/

// majic numbers are used to test for out-of-bounds writes to images.
#define ONE_MAGIC	0xA55A
#define ONE_EVIL	0xFACE

// The one destructor will only attempt to free the bits or pal
// if they were allocated by a ONE constructor.
#define CUSTOM_PALETTE	0x01
#define ONE_OWNS_BITS	0x02
#define ONE_OWNS_PAL	0x04

//----------------------------------------------------------------------------
// DO NOT CHANGE HEADER DATA ELEMENTS. FIXED FILE I/O USED.
typedef struct {
public:
	long pitch;
	long width;
	long height;
public:
	unsigned char m_version;		// Read/Written to/from file
	unsigned char m_flags;			// Private flags for use by ONE methods
	unsigned char m_frames;			// number of frames for animation
	unsigned char m_fpti;			// frames per time interval(1..32); time interval(seconds, minutes, hours, days)

private:
	long reserved_for_future_use_1;
public:
	unsigned int m_size;
	WORD reserved_for_future_use_2;
	WORD user_flags;
} ONE_HEADER;

//----------------------------------------------------------------------------
// all images used in the game are referenced through class ONE:
class ONE {
	// data elements.
public:
	ONE_HEADER m_hdr;
	PALETTEENTRY *m_pal;
	unsigned char * m_bits;

public: // constructors, destructors.
	void INIT() {
		m_hdr.pitch=0;
		m_hdr.width=0;
		m_hdr.height=0;
		m_bits=0;
		m_hdr.m_size=0;
		m_hdr.m_flags=0;
		m_pal=NULL;
		m_hdr.m_version = 0;
		m_hdr.user_flags = 0;
	};

	ONE(int width, int height) {Init(width, height);}
	ONE(void *bits, int _width, int _height, int _pitch);
	ONE() {INIT();}
	~ONE();

private:
	BOOL allocBits(int size);
public:
	BOOL freeBits();

public:
	inline unsigned char * getBits() { return m_bits;}
	inline unsigned int getSize() { return m_hdr.m_size;}
	inline long GetWidth() {return m_hdr.width;}
	inline long GetPitch() {return m_hdr.pitch;}
	inline long GetHeight() {return m_hdr.height;}
	void SetCustomPalette(PALETTEENTRY *pal) {m_hdr.m_flags|=CUSTOM_PALETTE; m_pal = pal;}
	inline PALETTEENTRY *GetPalette() {return (m_hdr.m_flags&CUSTOM_PALETTE)?m_pal:NULL;}
	// Handy for testing images for corruption after blitting onto.
	BOOL Validate();
	int Init(int width, int height);

	int Read(char * filename);
	int Read(char *path, char *filename);
	int ReadSprite(char *path, char *filename);

	int Write(char * lpszPathName);
};

//----------------------------------------------------------------------------
int oneWriteBMP(char *filename, unsigned char *bits, int width, int height, int pitch);
int oneWriteCustomBMP(char *filename, unsigned char *bits, int width, int height, int pitch, PALETTEENTRY *pal);

//----------------------------------------------------------------------------
typedef struct {
public:
	int x;
	int y;
	int length;
} MASK;

MASK * maskCreate(unsigned char *bits, int h, int w, int pitch, unsigned char ck);

int maskWrite(const char *lpszPathName, MASK *vector);
extern maskRead(const char *path, const char * lpszPathName, MASK **vector);
int maskFree(MASK **vector);
void maskGetBoundingRect(MASK *pvect, RECT *rect);
