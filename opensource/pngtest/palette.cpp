

#include "palette.h"
#include "xfile.h"

void ReadPalFile( const char *filename, PALETTE *pal)
{
    int                 fh;

    pal->dwRiff = 0;

    if (filename)
    {
        fh = open( filename, O_RDONLY);

        if (fh != -1)
        {
            size_t bytes_read = read(fh, pal, sizeof(PALETTE));
            if (bytes_read != sizeof(PALETTE)) {
                printf("%s:%d - palette is not the right size\n", __FILE__, __LINE__);
            }
            close(fh);
        }
		else
		{
			printf("Palette file (%s) is missing.", filename);
			throw(ERROR_FILE_NOT_FOUND);
		}
    }

    if (pal->dwRiff != 0x46464952 || // 'RIFF'
        pal->dwPal  != 0x204C4150 || // 'PAL '
        pal->dwData != 0x61746164 || // 'data'
        pal->palVersion != 0x0300 ||
        pal->palNumEntries > 256  ||
        pal->palNumEntries < 1)
    {
        printf("Palette file is corrupt.");
        printf("pal->palVersion = %x\n",pal->palVersion);

        throw(ERROR_FILE_NOT_FOUND);
    }
} 
