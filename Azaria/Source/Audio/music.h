
#pragma once
#include "dsWave.h"

enum BACKGROUND_MUSIC {
	THEME = 0,
	NATURE,
	VILLAGE,
	CITY,
	BACKGROUND_MUSIC_COUNT
};

static const char *backgroundMusic[BACKGROUND_MUSIC_COUNT][10] = {
{"kam002aa","kam003aa","kam004aa","kam005aa","kam006aa","kam007aa","kam008aa","kam009aa","kam010aa","kam011aa"},
{"kam102ca","kam103aa","kam104aa","kam105aa","kam107aa","kam112aa"},
{"kam204aa","kam206aa","kam207ba","kam208ba","kam209ba","kam210ba"},
{"kam306ba","kam310ba","kam311ba","kam312aa","kam313aa","kam314aa","kam316aa","kam317aa"}
};

class Music {
  static Wave *waveBackground[BACKGROUND_MUSIC_COUNT];
public:
    static void Init(const char *path);
    static void SetTrack(WORD track_id);
    static void Pause();
    static void Resume();
    static void Cleanup();
    static void SetVolume(int volume);
};





