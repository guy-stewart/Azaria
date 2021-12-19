

#include "../include/stdapp.h"
#include "../include/dsWave.h"
#include "../include/xfile.h"
#include "paths.h"
#include "music.h"

static Wave waveBackground[BACKGROUND_MUSIC_COUNT];

WORD c_current = 0;

//----------------------------------------------------------------------------
void Music::Init()
{
  int i;
  char path[_MAX_PATH];
  for (i=0;i<BACKGROUND_MUSIC_COUNT;i++)
  {
	makepath(path, NULL, AltWavePath, backgroundMusic[i][0], ".wav");
	waveBackground[i].Play(path, TRUE);
  }

  waveBackground[c_current].Play(0);
}

//----------------------------------------------------------------------------
void Music::Cleanup()
{
  int i;
  for (i=0;i<BACKGROUND_MUSIC_COUNT;i++)
  {
	waveBackground[i].Stop();
  }
}

//----------------------------------------------------------------------------
void Music::SetTrack(WORD track_id)
{
	if (track_id != c_current)
	{
		waveBackground[c_current].Pause(FADE_TIME);
		c_current = track_id;
		waveBackground[c_current].Play(FADE_TIME);
	}
}

//----------------------------------------------------------------------------
void Music::SetVolume(int volume)
{
	waveBackground[c_current].SetVolume(volume);
}

//----------------------------------------------------------------------------
void Music::Pause()
{
  Cleanup();
  //waveBackground[c_current].Pause(0);
}

//----------------------------------------------------------------------------
void Music::Resume()
{
  Init();
  //waveBackground[c_current].Play(FADE_TIME);
}

