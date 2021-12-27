
#pragma once

#include <stdio.h>
#include "jpeglib.h"

typedef int (*fn_read_JPEG_callback)(void *context,
                                     struct jpeg_decompress_struct *cinfo,
                                     JSAMPROW buffer,
                                     int rowstride,
                                     const char *filename);


int do_read_JPEG_file(struct jpeg_decompress_struct *cinfo,
                      const char *filename,
                      fn_read_JPEG_callback callback,
                      void *context);
