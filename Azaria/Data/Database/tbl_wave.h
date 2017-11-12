
#ifndef _tbl_wave_h
#define _tbl_wave_h

enum  WAVE_FILEID
{
    SOUND_CHOOSE = 0,
	NUMSOUNDS,
};

// I want to leave these here so they will be easier to edit:

#ifdef __WAVETABLE_OWNER__
static char *waveFileNames[] = {
	"choose.wav",
};
#endif

enum WAVE_RAMID
{
	SOUND_STARTUP = 0,
	SOUND_BTNDRAG,
	SOUND_BTNPRESS,
    SOUND_POPUP,
	SOUND_CLUNK,
	SOUND_FIRE,
	SOUND_CLICK,
	SOUND_HURT,
	SOUND_BURBLE,
	SOUND_THUNDER,
	SOUND_EXPLODE,
	SOUND_POP,
	SOUND_ARROW,
	SOUND_TON,
	SOUND_SLURP,
	SOUND_SPIT,
	SOUND_FUZZBUZZ,
	SOUND_BUZZFUZZ,
	TBL_WAVE_COUNT//last in enum
};

// I want to leave these here so they will be easier to edit:
#ifdef __WAVETABLE_OWNER__
// data\\sounds\\wave
static const char *waveRamFileNames[] = {
	"startup",
	"BtnDrag",
	"BtnPress",
	"Door",
	"clunk",
	"fire",
	"choose",
	"Hurt",
	"Burble",
	"Thunder",
	"Explode",
	"pop",
	"movingar",
	"ton",
	"Slurp",
	"Spit",
	"FuzzBuzz",
	"BuzzFuzz",
};
#endif

#endif

