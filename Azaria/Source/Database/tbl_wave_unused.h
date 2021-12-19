
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
    SOUND_POPUP,
	SOUND_CLUNK,
	SOUND_FIRE,
	SOUND_CLICK,
	SOUND_HURT,
	SOUND_SHIPSINK,
	SOUND_GOPISD2,
	SOUND_OMPISD3,
	SOUND_OGPISD5,
	SOUND_OGPISD4,
	SOUND_SHPISD1,
	SOUND_THUNDER,
	SOUND_HSPISD3,
	SOUND_WZPISD3,
	SOUND_HHPISD3,
	TBL_WAVE_COUNT//last in enum
};

// I want to leave these here so they will be easier to edit:
#ifdef __WAVETABLE_OWNER__
// data\\sounds\\wave
static char *waveRamFileNames[] = {
	"startup",
	"popup",
	"clunk",
	"fire",
	"choose",
	"Hurt",
	"ShipSink",
	"Gopissd2",
	"Ompissd3",
	"Ogpissd5",
	"Ogpissd4",
	"Shpisd1",
	"Thunder",
	"Hshppis3",
	"Wzpissd3",
	"Hpissed3",
};
#endif
