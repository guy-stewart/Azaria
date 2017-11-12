

#define FADE_TIME 4000

typedef enum BACKGROUND_MUSIC;
enum BACKGROUND_MUSIC {
	THEME = 0,
	CITY,
	VILLAGE,
	NATURE,
	BACKGROUND_MUSIC_COUNT
};

static char *backgroundMusic[BACKGROUND_MUSIC_COUNT][10] = {
  {"Theme", "Theme2"},
  {"City"},
  {"Country"},
  {"Nature"},
};

class Music {
public:
  static void Music::Init();
  static void Music::SetTrack(WORD track_id);
  static void Music::Pause();
  static void Music::Resume();
  static void Music::Cleanup();
  static void Music::SetVolume(int volume);
};
