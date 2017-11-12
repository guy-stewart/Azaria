

#define FADE_TIME 4000

enum BACKGROUND_MUSIC {
	THEME = 0,
	CITY,
	VILLAGE,
	NATURE,
	BACKGROUND_MUSIC_COUNT
};

static const char *backgroundMusic[BACKGROUND_MUSIC_COUNT][10] = {
  {"Theme", "Theme2"},
  {"City"},
  {"Country"},
  {"Nature"},
};

class Music {
public:
    static void Init();
    static void SetTrack(WORD track_id);
    static void Pause();
    static void Resume();
    static void Cleanup();
    static void SetVolume(int volume);
};
