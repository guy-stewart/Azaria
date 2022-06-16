/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include "one.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

void ReadPalFile( const char *filename, PALETTE *pal);

void ReadPalFromFile(const char *path, const char *name, PALETTE *pal) {
        ONE one;
        if (one.Read(path, name, pal->ape)) {
                memcpy(pal->ape, one.GetPalette(), sizeof(PALETTEENTRY)*256);
                printf("\n\n");
        } else {
                printf("failed to read %s/%s.one\n", path, name);
        }
}

const char *SetExtension(const char *fname, const char *ext) {
	static char path[_MAX_FNAME];
//	char drive[_MAX_DRIVE];
//	char dir[_MAX_DIR];
//	splitpath(path, drive, dir, fname, NULL);
	makepath(path, NULL, NULL, fname, ext);
        return path;
}

void convert_one_file(const char *path, const char *name, PALETTE *pal) {
        ONE one;
        if (one.Read(path, name, pal->ape)) {
                one.WritePng(SetExtension(name, "png"));
        } else {
                printf("failed to read %s/%s.one\n", path, name);
        }
}

#define AzariaCD961112_obj "/6tb/Home/Singularity/AzariaCD961112/data/ones/objects"
#define AzariaCD961112_views "/6tb/Home/Singularity/AzariaCD961112/data/ones/views"
#define AzariaCD2020_obj "/6tb/Home/Singularity/AzariaCD2020/data/ones/objects"
#define AzariaCD2020_views "/6tb/Home/Singularity/AzariaCD2020/data/ones/views"

int main(int argc, char **argv)
{
        PALETTE pal;

        printf("%s\n", argv[0]);
        printf("%s\n", argv[1]);
        ReadPalFile(     "/6tb/Home/Singularity/AzariaCD2020/data/palettes/gui.pal", &pal);
        if (argc > 1) {
                convert_one_file(NULL,argv[1], &pal);
        } else {
                convert_one_file("/6tb/Home/Singularity/AzariaCD2020/data/ones/views","vila", &pal);
                convert_one_file("/6tb/Home/Singularity/AzariaCD2020/data/ones/objects","burnwood", &pal);
        }

        return 0;
}

