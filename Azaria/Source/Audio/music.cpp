

#include "../include/stdapp.h"
#include "../include/dsWave.h"
#include "../include/xfile.h"
#include "paths.h"
#include "music.h"

Wave * Music::waveBackground[BACKGROUND_MUSIC_COUNT];

#define FADE_TIME 4000

WORD c_current = 0;

//----------------------------------------------------------------------------
void Music::Init(const char *path) {
  int i;
  char filepath[_MAX_PATH];
  for (i=0;i<BACKGROUND_MUSIC_COUNT;i++) {
    makepath(filepath, NULL, path, backgroundMusic[i][0], "wav");
    waveBackground[i] = new Wave(filepath);
  }
  waveBackground[c_current]->Play(0);
}

//----------------------------------------------------------------------------
void Music::Cleanup() {
  int i;
  for (i=0;i<BACKGROUND_MUSIC_COUNT;i++) {
    delete waveBackground[i];
    waveBackground[i] = NULL;
  }
}

//----------------------------------------------------------------------------
void Music::SetTrack(WORD track_id)
{
	if (track_id != c_current) {
    if (waveBackground[c_current]) {
  		waveBackground[c_current]->Pause(FADE_TIME);
	  	c_current = track_id;
		  waveBackground[c_current]->Play(FADE_TIME);
    }
	}
}

//----------------------------------------------------------------------------
void Music::SetVolume(int volume) {
	waveBackground[c_current]->SetVolume(volume);
}

//----------------------------------------------------------------------------
void Music::Pause() {
  Cleanup();
}

//----------------------------------------------------------------------------
void Music::Resume() {
}

