

#pragma once

#include "Data/Database/tbl_wave.h"
#include "dsWave.h"

class Effects {
    static Wave *effects[];
public:
    static void Init(const char *path);
    static void Play(WAVE_RAMID);
};
