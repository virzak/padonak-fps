/* ===========================================================================

  Name:	pngpic.h
 								 
  Purpose:							 
    Type declarations and associated macros for use
    with creatin PNG picture files (PNG files).

=========================================================================== */
/* Include Files */

#include <png.h>

/* ======================================================================== */
/* Data Structures */

typedef unsigned char   boolean;

#ifndef FALSE
#  define FALSE   ((boolean) 0)
#  define TRUE    ((boolean) 1)
#endif

typedef png_byte  Pic_byte;
typedef struct Pic_Pixel
{
  Pic_byte   r, g, b;
} Pic_Pixel;

typedef struct
{
  FILE*     fptr;
  char*     filename;
  
  short     width;
  short     height;
  short     scanline;
  
  png_structp png_ptr;
  png_infop info_ptr;
  
  Pic_byte* tempLine;
} Pic;

/* ======================================================================== */
/* Memory allocation and other macros */

#define StrAlloc(n)    ((char *)malloc((unsigned)(n)))
#define PixelAlloc(n)  ((Pic_Pixel *)malloc(((unsigned)(n))*sizeof(Pic_Pixel)))
#define PixelFree(p)   ((void) free((char *)(p)))

/* ======================================================================== */
/* General routines */

extern	Pic*    PicOpen(const char* filename, short width, short height );
extern	boolean PicWriteLine(Pic* pngFile, Pic_Pixel* pixels);
extern  void    PicClose(Pic* pngFile);
extern Pic* PicROpen(const char *filename, int *w, int *h);
extern boolean PicReadLine(Pic* pngFile, Pic_Pixel* pixels);
/* ======================================================================== */




