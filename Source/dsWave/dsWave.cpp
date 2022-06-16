

// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_toc.html

// https://soundprogramming.net/programming/tutorial-using-sdl2-and-sdl_mixer-to-play-samples/
// https://github.com/aminosbh/sdl2-mixer-sample/blob/master/src/main.c

/*==========================================================================
 ***************************************************************************/
#include "dsWave.h"
#include "dsConst.h"
#include <SDL2/SDL_mixer.h>

SDL_AudioSpec Wave::wav_spec;
Wave** Wave::ramWave;

//----------------------------------------------------------------------------
bool Wave::Init()
{
	// Set up the audio stream
    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    if( result < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }
	return true;
}

//----------------------------------------------------------------------------
Wave::Wave()
{
	chunk = NULL;
}

//----------------------------------------------------------------------------
Wave::Wave(const char *filename)
{
	chunk = Mix_LoadWAV(filename);
	if (!chunk) {
		printf("%s:%d failed to load wave file %s\n", __FILE__, __LINE__, filename);
	}
}

//----------------------------------------------------------------------------
Wave::~Wave()
{
}

//----------------------------------------------------------------------------
int atten[] = {-10000, -1600, -800, -600, -300, -200, -100, -75, -50, -25, 0};
void Wave::SetFxVolume(int volume)
{
  volume /=10;
}

//----------------------------------------------------------------------------
void Wave::SetMusicVolume(int volume)
{
  volume /=10;
}

//----------------------------------------------------------------------------
#define FADE_TIME_DELTA 100
BOOL Wave::Play(WORD fadetime) // fade time in milliseconds
{
	printf("%s:%d Wave::Play(fadetime:%d)\n",__FILE__, __LINE__,  fadetime);
	if (chunk) {
		Mix_PlayChannel(-1, chunk, 0);
	}
	return true;
}

//----------------------------------------------------------------------------
BOOL Wave::Play(char *filename, BOOL bPaused)
{
	// TODO: free chunk;
	chunk = Mix_LoadWAV(filename);
	if (chunk && !bPaused) {
		Mix_PlayChannel(-1, chunk, 0);
	}
	return TRUE;
}

//----------------------------------------------------------------------------
void Wave::PlayThread()
{
}

//----------------------------------------------------------------------------
void Wave::StartThread(Wave *thing)
{
  thing->PlayThread();
}

//----------------------------------------------------------------------------
void Wave::Stop()
{
}

//----------------------------------------------------------------------------
void Wave::Pause(WORD fadetime)
{
}

//----------------------------------------------------------------------------
HRESULT Wave::SetVolume(LONG value)
{
	return 0;
}

//----------------------------------------------------------------------------
HRESULT Wave::SetPan(LONG value)
{
	return 0;
}

//----------------------------------------------------------------------------
HRESULT Wave::SetFrequency(DWORD value)
{
	return 0;
}

//----------------------------------------------------------------------------
HRESULT Wave::Release()
{
	return 0;
}
