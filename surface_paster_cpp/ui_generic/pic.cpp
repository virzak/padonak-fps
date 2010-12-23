/* ======================================================================== */
/* Include Files */
#include <stdlib.h>
#include <string.h>
#include "pic.h"

/* ======================================================================== */
/* Memory allocation and other macros */

#define PicAlloc()      ((Pic *) malloc((unsigned)(sizeof(Pic))))

#define NUMCHANNELS 3   /* 3 channels (RGB) per pixel, no Alpha */
#define CHANNELDEPTH 8  /* 8 bits per colour channel */

/* ===========================================================================
 
    Pic* PicOpen(filename, width, height)
 
    Purpose:
        This function opens an image file and writes the
 	appropriate header for the given resolution.

 	The image file is a compressed PNG file, with 8 bits
 	for each of red, green and blue.  Only the resolution
 	is variable.

    Parameters:
        char*  filename (in) : name of image file to open.
        short  width (in)    : number of pixels per scanline.
        short  height (in)   : number of scanlines for image.

    Returned:
        Pic*   pngFile       : file pointer for the image file,
 			       NULL if an error occured.

    Notes:
        Any error conditions are indicated by writing to
 	`stderr', in addition to returning the NULL file
 	pointer.

--------------------------------------------------------------------------- */

Pic*
PicOpen(const char *filename, short width, short height)
{
  Pic     *pngFile;	/* structure to hold image info */
  FILE    *fptr;	/* the output file */
  png_structp png_ptr;  /* structure needed by PNG library */
  png_infop info_ptr;   /* structure needed by PNG library */
  
  pngFile = (Pic *) NULL;
  
  if (width <= 0) 
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - `%d' is an invalid scanline width.\n",
	      width);
      
    } 
  else if (height <= 0) 
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - `%d' is an invalid number of scanlines.\n",
	      height);
      
    } 
  else if (! (fptr = fopen(filename, "w"))) 
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - couldn't open file `%s' for writing.\n",
	      filename);
      
    } 
  else if (! (pngFile = PicAlloc())) 
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `Pic' structure.\n");
      fclose(fptr);
    } 
  else if ( !(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
						NULL, NULL, NULL)) )
    {
      /* Couldn't allocate PNG data structure */
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `PNG' structure.\n");
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    } 
  else if ( !(info_ptr = png_create_info_struct(png_ptr)) )
    {
      /* Couldn't allocate PNG info structure */
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `PNG' info struct.\n");
      png_destroy_write_struct(&png_ptr,
			       (png_infopp)NULL);
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    }
  /* setup PNG error handling */
  else if ( setjmp(png_ptr->jmpbuf) )
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot setup PNG error handling\n");
      png_destroy_write_struct(&png_ptr, &info_ptr);
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    }
  /* allocate a temporary row */
  else if ( !(pngFile->tempLine = (Pic_byte*) 
	      malloc((unsigned)(width*height*NUMCHANNELS*sizeof(Pic_byte)))) )
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate temp buffer\n");
      png_destroy_write_struct(&png_ptr, &info_ptr);
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    }
  else
    {
      /* setup the pngFile structure */
      pngFile->width    = width;
      pngFile->height   = height;
      pngFile->scanline = 0;
      pngFile->fptr     = fptr;
      pngFile->filename = StrAlloc(strlen(filename) + 1);
      (void) strcpy(pngFile->filename, filename);
      pngFile->png_ptr  = png_ptr;
      pngFile->info_ptr = info_ptr;

      /* Setup PNG I/O */
      png_init_io(png_ptr, fptr);
	 
      /* Optionally setup a callback to indicate when a row has been
       * written. */  

      /* Setup filtering. Use Paeth filtering */
      png_set_filter(png_ptr, 0, PNG_FILTER_PAETH);

      /* Setup compression level. */
      png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);


      /* Setup PNG header information and write it to the file */
      png_set_IHDR(png_ptr, info_ptr,
		   width, height,
		   CHANNELDEPTH, 
		   PNG_COLOR_TYPE_RGB, 
		   PNG_INTERLACE_NONE, 
		   PNG_COMPRESSION_TYPE_DEFAULT, 
		   PNG_FILTER_TYPE_DEFAULT);
      png_write_info(png_ptr, info_ptr); 
    }
  
    return(pngFile);
}

Pic* PicROpen(const char *filename, int *w, int *h)
{
  Pic     *pngFile;	/* structure to hold image info */
  FILE    *fptr;	/* the input file */
  png_structp png_ptr;  /* structure needed by PNG library */
  png_infop info_ptr;   /* structure needed by PNG library */
  png_uint_32 width, height;
  int dummy1, dummy2, dummy3;
  
  pngFile = (Pic *) NULL;
  
  if (! (fptr = fopen(filename, "rb"))) 
    {
      fprintf(stderr,
	      "ERROR: PicROpen() - couldn't open file `%s' for reading.\n",
	      filename);
      
    } 
  else if (! (pngFile = PicAlloc())) 
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `Pic' structure.\n");
      fclose(fptr);
    } 
  else if ( !(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
						NULL, NULL, NULL)) )
    {
      /* Couldn't allocate PNG data structure */
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `PNG' structure.\n");
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    } 
  else if ( !(info_ptr = png_create_info_struct(png_ptr)) )
    {
      /* Couldn't allocate PNG info structure */
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot allocate `PNG' info struct.\n");
      png_destroy_read_struct(&png_ptr,
			       (png_infopp)NULL, (png_infopp)NULL);
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    }
  /* setup PNG error handling */
  else if ( setjmp(png_ptr->jmpbuf) )
    {
      fprintf(stderr,
	      "ERROR: PicOpen() - cannot setup PNG error handling\n");
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      free(pngFile);
      pngFile = NULL;
      fclose(fptr);
    }
  else
    {
      /* setup the pngFile structure */
      pngFile->scanline = 0;

      pngFile->fptr     = fptr;
      pngFile->filename = StrAlloc(strlen(filename) + 1);
      (void) strcpy(pngFile->filename, filename);
      pngFile->png_ptr  = png_ptr;
      pngFile->info_ptr = info_ptr;

      /* Setup PNG I/O */
      png_init_io(png_ptr, fptr);

		/* do a png_read_png(png_ptr, info_ptr, png_transforms, NULL) */
      png_read_info(png_ptr, info_ptr);

      /* Setup PNG header information and write it to the file */
      png_get_IHDR(png_ptr, info_ptr,
		   &width, &height,
		   &dummy1, &dummy2, &dummy3, NULL, NULL);

#ifdef __ICC
#pragma warning (disable:810)
#endif
      pngFile->width    = width;
      pngFile->height   = height;
#ifdef __ICC
#pragma warning (default:810)
#endif

      *w = (int)width;
      *h = (int)height;

		pngFile->tempLine = (Pic_byte*) 
	      malloc((unsigned)(width*height*NUMCHANNELS*sizeof(Pic_byte)));
    }
  
    return(pngFile);
}


/* ===========================================================================
                                                        	 
    boolean    PicWriteLine(pngFile, pixels)			 
                                                        	 
    Purpose:							 
        This function writes the given scanline to the given	 
 	image file.						 
                                                        	 
    Parameters:							 
        Pic*   pngFile (in) : name of image file to write.	 
        Pixel* pixels(in)   : scanline of rendered pixels.	 
                                                        	 
    Returned:							 
        boolean   status : TRUE - scanline written, else FALSE	 
                                                        	 
    Notes:							 
        The scanline will not be written if the given image	 
 	file has been completed.				 

--------------------------------------------------------------------------- */

boolean PicWriteLine(Pic* pngFile, Pic_Pixel* pixels)
{
  /* Don't do anything if the file is already complete: */
  if (pngFile->scanline == pngFile->height) {
    fprintf(stderr,
	    "WARNING: PicWriteLine() - `%s' is complete, scanline ignored.\n",
	    pngFile->filename);
    
    return(FALSE);
  }
  else
    { 
      /* The file isn't complete. Copy the array of "Pixel" structures
       * the temporary line. */
      int i;
      for (i = 0; i < pngFile->width; ++i) {
	pngFile->tempLine[NUMCHANNELS*i]   = pixels[i].r;
	pngFile->tempLine[NUMCHANNELS*i+1] = pixels[i].g;
	pngFile->tempLine[NUMCHANNELS*i+2] = pixels[i].b;
      }

      /* Write the current line to the file */
      png_write_row(pngFile->png_ptr, pngFile->tempLine);
      
      /* Increment the current scanline */
      ++pngFile->scanline;
      
      return(TRUE);
    }
}

boolean PicReadLine(Pic* pngFile, Pic_Pixel* pixels)
{
  /* Don't do anything if the file is already complete: */
  if (pngFile->scanline == pngFile->height) {
    fprintf(stderr,
	    "WARNING: PicWriteLine() - `%s' is complete, scanline ignored.\n",
	    pngFile->filename);
    
    return(FALSE);
  }
  else
    { 
      /* The file isn't complete. Copy the array of "Pixel" structures
       * the temporary line. */
      int i;

      /* Write the current line to the file */
      png_read_row(pngFile->png_ptr, pngFile->tempLine, NULL);

      /* Increment the current scanline */
      ++pngFile->scanline;

      for (i = 0; i < pngFile->width; ++i) {
		  pixels[i].r = pngFile->tempLine[NUMCHANNELS*i];
		  pixels[i].g = pngFile->tempLine[NUMCHANNELS*i+1];
		  pixels[i].b = pngFile->tempLine[NUMCHANNELS*i+2];
      }
      
      return(TRUE);
    }
}

/* ===========================================================================
                                                        	 
    void     PicClose(pngFile)				 
                                                        	 
    Purpose:							 
        This function closes an image file.			 
                                                        	 
    Parameters:							 
        Pic*   pngFile (in) : image file to be closed.	 
                                                        	 
    Notes:							 
        A warning will be issued if the image file is		 
 	incomplete, ie) all scanlines have not yet been		 
 	written. The file will still be closed, though.
                                                        	 
--------------------------------------------------------------------------- */
void 
PicClose(Pic* pngFile)
{

  /* Check to see if the file is incomplete. */
  if (pngFile->scanline < pngFile->height) {
    fprintf(stderr,
	    "WARNING: PicClose() - only %d of %d scanlines "
	    "written to `%s'.\n",
	    pngFile->scanline,
	    pngFile->height,
	    pngFile->filename);
    }
  else
    {
      /* Finish off the PNG file and release all allocated memory. */
      png_write_end(pngFile->png_ptr, pngFile->info_ptr);
      png_destroy_write_struct(&pngFile->png_ptr, &pngFile->info_ptr);
      free(pngFile->tempLine);
      free(pngFile->filename);
      
      fclose(pngFile->fptr);
    }
}

#ifdef __ICC
#pragma warning (disable:177)
#endif
static void 
PicRClose(Pic* pngFile)
{

  /* Finish off the PNG file and release all allocated memory. */
  png_read_end(pngFile->png_ptr, pngFile->info_ptr);
  png_destroy_read_struct(&pngFile->png_ptr, &pngFile->info_ptr, (png_infopp)NULL);
  free(pngFile->tempLine);
  free(pngFile->filename);
  
  fclose(pngFile->fptr);
}

/* ======================================================================== */


