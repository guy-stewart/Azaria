//-----------------------------------------------------------------
// Wave Object
// C++ Header - Wave.h
//-----------------------------------------------------------------

#ifndef __WAVEMAN_H__
#define __WAVEMAN_H__

//-----------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------
#include <MMSystem.h>

typedef enum
{
    SOUND_CHOOSE = 0,
	NUMSOUNDS,
} WAVE_RAMID;


// I want to leave these here so they will be easier to edit:
#ifdef __WAVEMAN_CPP__
static char *waveRamFileNames[] = {
	"data\\sounds\\wave\\choose.wav",
};
#endif

typedef enum
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
} WAVE_FILEID;

// I want to leave these here so they will be easier to edit:
#ifdef __WAVEMAN_CPP__
// data\\sounds\\wave
static char *waveFileNames[] = {
	"startup.wav",
	"popup.wav",
	"clunk.wav",
	"fire.wav",
	"choose.wav",
	"Hurt.wav",
	"ShipSink.wav",
	"Gopissd2.wav",
	"Ompissd3.wav",
	"Ogpissd5.wav",
	"Ogpissd4.wav",
	"Shpisd1.wav",
	"Thunder.wav",
	"Hshppis3.wav",
	"Wzpissd3.wav",
	"Hpissed3.wav",

};
#endif

class WaveMan // the Wave Manager
{
private:
  char m_path[_MAX_FNAME]; // gets set to 'WavemanPath'
  DWORD m_Volume;
  LPDIRECTSOUND       m_pDirectSound;
  LPDIRECTSOUNDBUFFER m_pDSBuffer[NUMSOUNDS];
  LPDIRECTSOUNDBUFFER m_pDSeffect;

public:
	WaveMan();
	~WaveMan();

public: // interface
	BOOL WaveMan::Init(HWND hWnd, char *WavePath);
	void Play(WAVE_RAMID iRamId);
	void Play(WAVE_FILEID iFileNameindex);

	BOOL SetVolume(DWORD volume);
	DWORD GetVolume();
/*
lVolume

Values range from 0 (0 db, no volume adjustment) to -10,000 (-100 db, essentially silent). 
Amplification is not currently supported.

Volume units of are hundredths of a decibel, where zero is the original 
volume of the stream.  Positive decibels correspond to amplification and
negative decibels correspond to attenuation. The decibel (db) scale 
corresponds to the logarithmic hearing characteristics of the ear. 
An attenuation of 20 db makes a buffer sound half as loud; an attenuation 
of 40 db makes a buffer sound one quarter as loud. Amplification is not 
currently supported.  The pan control is cumulative with volume control.
*/

private: // internal
	BOOL WaveMan::InitDSBuffers();
	BOOL WaveMan::RestoreDSBuffers();
	void WaveMan::CloseDirectSound();
	void WaveMan::Play(LPDIRECTSOUNDBUFFER pDSBuffer);
	LPDIRECTSOUNDBUFFER InitDSBuffer(const char *filename);

};

extern WaveMan waveman;
#endif
