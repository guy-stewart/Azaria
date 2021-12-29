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

void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

class PNG {
        int width, height;
        png_byte color_type;
        png_byte bit_depth;
        png_structp png_ptr;
        png_infop info_ptr;
        int number_of_passes;
        png_bytep * row_pointers;
public:
        void read_png_file(const char *filename);
        void write_png_file(const char* file_name);
        void process_file(void);
};

// TODO: read a png RGBA file into a ONE class structure:
void ONE::ReadPng(const char* file_name)
{
        unsigned char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        m_hdr.width = png_get_image_width(png_ptr, info_ptr);
        m_hdr.height = png_get_image_height(png_ptr, info_ptr);
        m_hdr.pitch = m_hdr.width*4; // we will convert to RGBA8
        m_hdr.m_size = m_hdr.pitch * m_hdr.height;
        int color_type = png_get_color_type(png_ptr, info_ptr);
        int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        int number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during read_image");

        // -------------------------------------------------------------------
        // read the file image (might be other than RGB8)
        size_t row_bytes = png_get_rowbytes(png_ptr,info_ptr);
        png_bytep pbytes = (png_bytep) malloc(m_hdr.height * row_bytes);
        png_bytep * row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * m_hdr.height);
        for (int y=0; y<m_hdr.height; y++)
                row_pointers[y] = (png_byte*) pbytes + (y*row_bytes);
        png_read_image(png_ptr, row_pointers);
        // -------------------------------------------------------------------

        fclose(fp);

        // -------------------------------------------------------------------
        // Convert to RGBA8 if necessary ...
        if ((bit_depth != 8) || (row_bytes != (m_hdr.width*4))) {
                if (bit_depth == 16) {
                        m_bits = (PIXEL*)malloc(m_hdr.height * m_hdr.pitch);
                        RGBA8 *dst = (RGBA8*)m_bits;
                        RGBA16 *src = (RGBA16*)pbytes;
                        for (int y=0; y<m_hdr.height; y++) {
                                for (int x=0; x<m_hdr.width; x++) {
                                        RGBA8  *dpel = dst + y*m_hdr.width+x;
                                        RGBA16 *spel = src + y*m_hdr.width+x;
                                        dpel->peRed = spel->peRed;
                                        dpel->peGreen = spel->peGreen;
                                        dpel->peBlue = spel->peBlue;
                                        dpel->peAlpha = spel->peAlpha;
                                }
                        }
                }
        } else {
                m_bits = (PIXEL*)pbytes;
        }
        // -------------------------------------------------------------------

        free(row_pointers);
}

void ONE::WritePng(const char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, m_hdr.width, m_hdr.height,
                     8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_bytep * row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * m_hdr.height);
        for (int y=0; y<m_hdr.height; y++)
                row_pointers[y] = (png_byte*) (m_bits + (y*m_hdr.pitch));

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        free(row_pointers);

        fclose(fp);
}


void PNG::process_file(void)
{
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

        for (int y=0; y<height; y++) {
                png_byte* row = row_pointers[y];
                for (int x=0; x<width; x++) {
                        png_byte* ptr = &(row[x*4]);
                        printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                               x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

                        /* set red value to 0 and green value to the blue one */
                        ptr[0] = 0;
                        ptr[1] = ptr[2];
                }
        }
}

void ReadPalFile( const char *filename, PALETTE *pal);

void convert_one_file(const char *path, const char *name, PALETTE *pal) {
        ONE one;
        if (one.Read(path, name, pal->ape)) {
                one.WritePng(name);
        } else {
                printf("failed to read %s/%s.one\n", path, name);
        }
}

int main()
{
        PNG png;
        ONE one;

        PALETTE pal;
        ReadPalFile("azaria.pal", &pal);
        one.ReadPng("rgb-alpha-16-sRGB.png");
        // png.process_file();
        one.WritePng("processed.png");

        convert_one_file("/6tb/Home/Singularity/AzariaCD961112/DATA/ONES/OBJECTS","1", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD961112/DATA/ONES/OBJECTS","aa_01", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD/DATA/ONES/OBJECTS","AA_00", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD961112/DATA/ONES/OBJECTS","bomb1", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD/DATA/ONES/OBJECTS","bomb2", &pal);

        convert_one_file("/6tb/Home/Singularity/AzariaCD961112/DATA/ONES/OBJECTS","cEditBxM", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD/DATA/ONES/OBJECTS","cListBxS", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD/DATA/ONES/OBJECTS","cListBxM", &pal);
        convert_one_file("/6tb/Home/Singularity/AzariaCD/DATA/ONES/OBJECTS","cListBxL", &pal);

        return 0;
}

