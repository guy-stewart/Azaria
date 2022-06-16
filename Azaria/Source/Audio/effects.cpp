


#include "effects.h"
#include "Data/Database/tbl_wave.h"
#include <stdio.h>
#include "dsWave.h"


Wave * Effects::effects[TBL_WAVE_COUNT];

void Effects::Init(const char *path) {
    char filepath[_MAX_PATH];

   	for (int i=0; i<TBL_WAVE_COUNT; i++) {
        makepath(filepath, NULL, path, waveRamFileNames[i], "wav");
		effects[i] = new Wave(filepath);
	}
}

//----------------------------------------------------------------------------
void Effects::Play(WAVE_RAMID ram_id)
{
	printf("%s:%d Wave::Play(RAM_ID:%d \"%s\")\n",__FILE__, __LINE__,  ram_id, waveRamFileNames[ram_id]);
    if (effects[ram_id]) {
        effects[ram_id]->Play(0);
    }
}

