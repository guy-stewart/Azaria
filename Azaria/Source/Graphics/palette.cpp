
#include "palette.h"


//----------------------------------------------------------------------------
void dumpPalette(PALETTE *a) {
	int i;
	Msg("  Red Green  Blue");
	for (i=0;i<256;i++)	{
		Msg("%3d: %5d %5d %5d", i, a->ape[i].peRed, a->ape[i].peGreen, a->ape[i].peBlue);
	}		
}

//----------------------------------------------------------------------------
void PrintPaletteDuplicates(PALETTE *m_palette)  {
	int i;
	Msg("  Red Green  Blue");
    for( i=0; i<256; i++ )  {
			int j;
			for (j=i+1;j<256;j++) {
				if (m_palette[i].peRed == m_palette[j].peRed
				 && m_palette[i].peGreen == m_palette[j].peGreen
				 && m_palette[i].peBlue == m_palette[j].peBlue)	{
					Msg("%3d: %5d %5d %5d", i, m_palette[i].peRed, m_palette[i].peGreen, m_palette[i].peBlue);
					Msg("  -------> Duplicate color at %d.", j);
					break;
				}
			}		
        }
}

void PrintPalettes(PALETTE *a, PALETTE *b) {
	int i;
	Msg("  Red Green  Blue");
    for( i=0; i<256; i++ ) {
			int j;
			for (j=i+1;j<256;j++) {
				if (a->ape[i].peRed == b->ape[j].peRed
				 && a->ape[i].peGreen == b->ape[j].peGreen
				 && a->ape[i].peBlue == b->ape[j].peBlue) {
					Msg("%3d: %5d %5d %5d", i, a->ape[i].peRed, a->ape[i].peGreen, a->ape[i].peBlue);
					Msg("  -------> Duplicate color at %d.", j);
					break;
				}
			}		
        }
}
