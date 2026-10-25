//*****************************************************************************
// @(#) $Id: textures.cpp,v 1.18 2003/10/21 08:41:57 virzak Exp $ 
// 
//  Description: 
//      Provides functions to read bmp and targa files.
//
//  =
//
//*****************************************************************************





#ifdef _WIN32
#include "stdafx.h"
#endif

#include "textures.h"
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "engine.h"


/* simple loader for 24bit bitmaps (data is in rgb-format) */
int loadBMP(const char *filename, textureImage *texture)
{
  FILE *file;
  unsigned short int bfType;
  long int bfOffBits;
  short int biPlanes;
  short int biBitCount;
  long int biSizeImage;
  int i;
  unsigned char temp;
  /* make sure the file is there and open it read-only (binary) */
  if ((file = fopen(filename, "rb")) == NULL)
  {
    output->printf("File not found : %s\n", filename);
    return 0;
  }
  if(!fread(&bfType, sizeof(short int), 1, file))
  {
    output->printf("Error reading file!\n");
    return 0;
  }
  /* check if file is a bitmap */
  if (bfType != 19778)
  {
    output->printf("Not a Bitmap-File!\n");
    return 0;
  }    
  /* get the file size */
  /* skip file size and reserved fields of bitmap file header */
  fseek(file, 8, SEEK_CUR);
  /* get the position of the actual bitmap data */
  if (!fread(&bfOffBits, sizeof(long int), 1, file))
  {
    output->printf("Error reading file!\n");
    return 0;
  }
  output->printf("Data at Offset: %ld\n", bfOffBits);
  /* skip size of bitmap info header */
  fseek(file, 4, SEEK_CUR);
  /* get the width of the bitmap */
  fread(&texture->sizeX, sizeof(int), 1, file);
  output->printf("Width of Bitmap: %d\n", texture->sizeX);
  /* get the height of the bitmap */
  fread(&texture->sizeY, sizeof(int), 1, file);
  output->printf("Height of Bitmap: %d\n", texture->sizeY);
  /* get the number of planes (must be set to 1) */
  fread(&biPlanes, sizeof(short int), 1, file);
  if (biPlanes != 1)
  {
    output->printf("Error: number of Planes not 1!\n");
    return 0;
  }
  /* get the number of bits per pixel */
  if (!fread(&biBitCount, sizeof(short int), 1, file))
  {
    output->printf("Error reading file!\n");
    return 0;
  }
  output->printf("Bits per Pixel: %d\n", biBitCount);
  if (biBitCount != 24)
  {
    output->printf("Bits per Pixel not 24\n");
    return 0;
  }
  /* calculate the size of the image in bytes */
  biSizeImage = texture->sizeX * texture->sizeY * 3;
  output->printf("Size of the image data: %ld\n", biSizeImage);
  texture->data = new unsigned char [biSizeImage];
  /* seek to the actual data */
  fseek(file, bfOffBits, SEEK_SET);
  if (!fread(texture->data, biSizeImage, 1, file))
  {
    output->printf("Error loading file!\n");
    return 0;
  }
  /* swap red and blue (bgr -> rgb) */
  for (i = 0; i < biSizeImage; i += 3)
  {
    temp = texture->data[i];
    texture->data[i] = texture->data[i + 2];
    texture->data[i + 2] = temp;
  } 
  return 1;
}

bool loadGLTextures(char * filename, GLuint *texture)  /* Load Bitmaps And Convert To Textures */
{
  bool status;
  textureImage *texti;
  
  status = false;
  texti = new textureImage;
  if (loadBMP(filename, texti))
  {
    status = true;
    glGenTextures(3, &texture[0]);   /* create three textures */
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    /* actually generate the texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texti->sizeX, texti->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, texti->data);
    /* use no filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    /* the second texture */
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texti->sizeX, texti->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, texti->data);
    /* use linear filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* the third texture */
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    rnd::texImageMipmapped(texti->sizeX, texti->sizeY, 3, texti->data);
    /* use mipmapping */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  /* free the ram we used in our texture generation process */
  if (texti)
  {
    if (texti->data) delete [] texti->data;
    delete texti;
  }  
  return status;
}






// These defines are used to tell us about the type of TARGA file it is
#define TGA_RGB     2    // This tells us it's a normal RGB (really BGR) file
#define TGA_A     3    // This tells us it's a ALPHA file
#define TGA_RLE    10    // This tells us that the targa is Run-Length Encoded (RLE)

// This is our image structure for our targa data
struct tImageTGA : public tImage
{
  int channels;      // The channels in the image (3 = RGB : 4 = RGBA)
};








// JPEG!!!
//
//

// This stores the important jpeg data
struct tImageJPG : public tImage
{
  int rowSpan;
};






/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////  This decodes the jpeg and fills in the tImageJPG structure
/////
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

static void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
  // Read in the header of the jpeg file
  jpeg_read_header(cinfo, TRUE);
  
  // Start to decompress the jpeg file with our compression info
  jpeg_start_decompress(cinfo);

  // Get the image dimensions and row span to read in the pixel data
  pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
  pImageData->sizeX   = cinfo->image_width;
  pImageData->sizeY   = cinfo->image_height;
  
  // Allocate memory for the pixel buffer
  pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
    
  // Here we use the library's state variable cinfo.output_scanline as the
  // loop counter, so that we don't have to keep track ourselves.
  
  // Create an array of row pointers
  unsigned char** rowPtr = new unsigned char * [pImageData->sizeY];
  for (int i = 0; i < pImageData->sizeY; ++i)
    rowPtr[pImageData->sizeY-i-1] = &(pImageData->data[i*pImageData->rowSpan]);

  // Now comes the juice of our work, here we extract all the pixel data
  int rowsRead = 0;
  while (cinfo->output_scanline < cinfo->output_height) 
  {
    // Read in the current row of pixels and increase the rowsRead count
    rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], 
        cinfo->output_height - rowsRead);
  }
  
  // Delete the temporary row pointers
  delete [] rowPtr;

  // Finish decompressing the data
  jpeg_finish_decompress(cinfo);
}


///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////  This loads the JPG file and returns it's data in a tImageJPG struct
/////
///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

static tImageJPG *LoadJPG(const char *filename)
{
  struct jpeg_decompress_struct cinfo;
  tImageJPG *pImageData = NULL;
  FILE *pFile;
  
  // This is the only function you should care about.  You don't need to
  // really know what all of this does (since you can't cause it's a library!) :)
  // Just know that you need to pass in the jpeg file name, and get a pointer
  // to a tImageJPG structure which contains the width, height and pixel data.
  // Be sure to free the data after you are done with it, just like a bitmap.
  
  // Open a file pointer to the jpeg file and check if it was found and opened 
  if((pFile = fopen(filename, "rb")) == NULL) 
  {
    // Display an error message saying the file was not found, then return NULL
    //MessageBox(g_hWnd, "Unable to load JPG File!", "Error", MB_OK);
    output->printf("Unable to load JPG File!\n");
    return NULL;
  }
  
  // Create an error handler
  jpeg_error_mgr jerr;

  // Have our compression info object point to the error handler address
  cinfo.err = jpeg_std_error(&jerr);
  
  // Initialize the decompression object
  jpeg_create_decompress(&cinfo);
  
  // Specify the data source (Our file pointer)  
  jpeg_stdio_src(&cinfo, pFile);
  
  // Allocate the structure that will hold our eventual jpeg data (must free it!)
  pImageData = new tImageJPG;

  // Decode the jpeg file and fill in the image data structure to pass back
  DecodeJPG(&cinfo, pImageData);
  
  // This releases all the stored memory for reading and decoding the jpeg
  jpeg_destroy_decompress(&cinfo);
  
  // Close the file pointer that opened the file
  fclose(pFile);

  // Return the jpeg data (remember, you must free this data after you are done)
  return pImageData;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *














// TARGA!!!
//
//



static tImageTGA *LoadTGA(const char *filename)
{
  tImageTGA *pImageData = NULL;    // This stores our important image data
  unsigned short int width = 0, height = 0;      // The dimensions of the image
  unsigned char length = 0;          // The length in bytes to the pixels
  unsigned char imageType = 0;          // The image type (RLE, RGB, Alpha...)
  unsigned char bits = 0;            // The bits per pixel for the image (16, 24, 32)
  FILE *pFile = NULL;          // The file pointer
  int channels = 0;          // The channels of the image (3 = RGA : 4 = RGBA)
  int stride = 0;            // The stride (channels * width)
  int i = 0;              // A counter
    
  // This function loads in a TARGA (.TGA) file and returns its data to be
  // used as a texture or what have you.  This currently loads in a 16, 24
  // and 32-bit targa file, along with RLE compressed files.  Eventually you
  // will want to do more error checking to make it more robust.  This is
  // also a perfect start to go into a modular class for an engine.
  // Basically, how it works is, you read in the header information, then
  // move your file pointer to the pixel data.  Before reading in the pixel
  // data, we check to see the if it's an RLE compressed image.  This is because
  // we will handle it different.  If it isn't compressed, then we need another
  // check to see if we need to convert it from 16-bit to 24 bit.  24-bit and
  // 32-bit textures are very similar, so there's no need to do anything special.
  // We do, however, read in an extra bit for each color.

  // Open a file pointer to the targa file and check if it was found and opened 
  if((pFile = fopen(filename, "rb")) == NULL) 
  {
    // Display an error message saying the file was not found, then return NULL
    *output << "Error : Unable to load TGA File!\n";
    return NULL;
  }
    
  // Allocate the structure that will hold our eventual image data (must free it!)
  pImageData = new tImageTGA;

  // Read in the length in bytes from the header to the pixel data
  fread(&length, sizeof(unsigned char), 1, pFile);
  
  // Jump over one byte
  fseek(pFile,1,SEEK_CUR); 

  // Read in the imageType (RLE, RGB, etc...)
  fread(&imageType, sizeof(unsigned char), 1, pFile);
  
  // Skip past general information we don't care about
  fseek(pFile, 9, SEEK_CUR); 

  // Read the width, height and bits per pixel (16, 24 or 32)
  fread(&width,  sizeof(unsigned short int), 1, pFile);
  fread(&height, sizeof(unsigned short int), 1, pFile);
  fread(&bits,   sizeof(unsigned char), 1, pFile);
  
  // Now we move the file pointer to the pixel data
  fseek(pFile, length + 1, SEEK_CUR); 

  // Check if the image is RLE compressed or not
  if(imageType != TGA_RLE)
  {
    // Check if the image is a 24 or 32-bit image
    if(bits == 24 || bits == 32)
    {
      // Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
      // Next, we calculate the stride and allocate enough memory for the pixels.
      channels = bits / 8;
      stride = channels * width;
      pImageData->data = new unsigned char[stride * height];

      // Load in all the pixel data line by line
      for(int y = 0; y < height; ++y)
      {
        // Store a pointer to the current line of pixels
        unsigned char *pLine = &(pImageData->data[stride * y]);

        // Read in the current line of pixels
        fread(pLine, stride, 1, pFile);
      
        // Go through all of the pixels and swap the B and R values since TGA
        // files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
        for(i = 0; i < stride; i += channels)
        {
          unsigned char temp     = pLine[i];
          pLine[i]     = pLine[i + 2];
          pLine[i + 2] = temp;
        }
      }
    }
    // Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
    else if(bits == 16)
    {
      unsigned short pixels = 0;
      /*int*/ unsigned char r=0, g=0, b=0;

      // Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
      // We then calculate the stride and allocate memory for the pixels.
      channels = 3;
      stride = channels * width;
      pImageData->data = new unsigned char[stride * height];

      // Load in all the pixel data pixel by pixel
      for(int i = 0; i < width*height; ++i)
      {
        // Read in the current pixel
        fread(&pixels, sizeof(unsigned short), 1, pFile);
        
        // To convert a 16-bit pixel into an R, G, B, we need to
        // do some masking and such to isolate each color value.
        // 0x1f = 11111 in binary, so since 5 bits are reserved in
        // each unsigned short for the R, G and B, we bit shift and mask
        // to find each value.  We then bit shift up by 3 to get the full color.
#ifdef __ICC
#pragma warning (disable:810)
#endif

        b = (unsigned char) ((pixels & 0x1f) << 3);
        g = (unsigned char) (((pixels >> 5) & 0x1f) << 3);
        r = (unsigned char) (((pixels >> 10) & 0x1f) << 3);
#ifdef __ICC
#pragma warning (default:810)
#endif
        
        // This essentially assigns the color to our array and swaps the
        // B and R values at the same time.
        pImageData->data[i * 3 + 0] = r;
        pImageData->data[i * 3 + 1] = g;
        pImageData->data[i * 3 + 2] = b;
      }
    }  
    // Else return a NULL for a bad or unsupported pixel format
    else
      return NULL;
  }
  // Else, it must be Run-Length Encoded (RLE)
  else
  {
    // First, let me explain real quickly what RLE is.  
    // For further information, check out Paul Bourke's intro article at: 
    // http://astronomy.swin.edu.au/~pbourke/dataformats/rle/
    // 
    // Anyway, we know that RLE is a basic type compression.  It takes
    // colors that are next to each other and then shrinks that info down
    // into the color and a integer that tells how much of that color is used.
    // For instance:
    // aaaaabbcccccccc would turn into a5b2c8
    // Well, that's fine and dandy and all, but how is it down with RGB colors?
    // Simple, you read in an color count (rleID), and if that number is less than 128,
    // it does NOT have any optimization for those colors, so we just read the next
    // pixels normally.  Say, the color count was 28, we read in 28 colors like normal.
    // If the color count is over 128, that means that the next color is optimized and
    // we want to read in the same pixel color for a count of (colorCount - 127).
    // It's 127 because we add 1 to the color count, as you'll notice in the code.

    // Create some variables to hold the rleID, current colors read, channels, & stride.
    unsigned char rleID = 0;
    int colorsRead = 0;
    channels = bits / 8;
    stride = channels * width;

    // Next we want to allocate the memory for the pixels and create an array,
    // depending on the channel count, to read in for each pixel.
    pImageData->data = new unsigned char[stride * height];
    unsigned char *pColors = new unsigned char [channels];

    // Load in all the pixel data
    while(i < width*height)
    {
      // Read in the current color count + 1
      fread(&rleID, sizeof(unsigned char), 1, pFile);
      
      // Check if we don't have an encoded string of colors
      if(rleID < 128)
      {
        // Increase the count by 1
        rleID++;

        // Go through and read all the unique colors found
        while(rleID)
        {
          // Read in the current color
          fread(pColors, sizeof(unsigned char) * channels, 1, pFile);

          // Store the current pixel in our image array
          pImageData->data[colorsRead + 0] = pColors[2];
          pImageData->data[colorsRead + 1] = pColors[1];
          pImageData->data[colorsRead + 2] = pColors[0];

          // If we have a 4 channel 32-bit image, assign one more for the alpha
          if(bits == 32)
            pImageData->data[colorsRead + 3] = pColors[3];

          // Increase the current pixels read, decrease the amount
          // of pixels left, and increase the starting index for the next pixel.
          ++i;
          rleID--;
          colorsRead += channels;
        }
      }
      // Else, let's read in a string of the same character
      else
      {
        // Minus the 128 ID + 1 (127) to get the color count that needs to be read
        rleID -= 127;

        // Read in the current color, which is the same for a while
        fread(pColors, sizeof(unsigned char) * channels, 1, pFile);

        // Go and read as many pixels as are the same
        while(rleID)
        {
          // Assign the current pixel to the current index in our pixel array
          pImageData->data[colorsRead + 0] = pColors[2];
          pImageData->data[colorsRead + 1] = pColors[1];
          pImageData->data[colorsRead + 2] = pColors[0];

          // If we have a 4 channel 32-bit image, assign one more for the alpha
          if(bits == 32)
            pImageData->data[colorsRead + 3] = pColors[3];

          // Increase the current pixels read, decrease the amount
          // of pixels left, and increase the starting index for the next pixel.
          ++i;
          rleID--;
          colorsRead += channels;
        }
        
      }
        
    }

    // Free up pColors
    delete[] pColors;
  }

  // Close the file pointer that opened the file
  fclose(pFile);

  // Fill in our tImageTGA structure to pass back
  pImageData->channels = channels;
  pImageData->sizeX    = width;
  pImageData->sizeY    = height;

  // Return the TGA data (remember, you must free this data after you are done)
  return pImageData;
}





void CreateTexture(unsigned int textureArray[], char * strFileName, int textureID)
{
  if(!strFileName)                  // Return from the function if no file name was passed in
    return;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *  
  
  // Just like the other image loaders, we return a pointer to some image data,
  // which needs to be freed once we are finished with it, along with it's data.

  tImageTGA *pImage = LoadTGA(strFileName);      // Load the image and store the data

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
  

  if(pImage == NULL) {                  // If we can't load the file, quit!
    output->printf("can't load the file : %s\n", strFileName);
    return;
        }

  // Generate a texture with the associative texture ID stored in the array
  glGenTextures(1, &textureArray[textureID]);

  // Bind the texture to the texture arrays index and init the texture
  glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

  // Assume that the texture is a 24 bit RGB texture (We convert 16-bit ones to 24-bit)
  int textureType = GL_RGB;

  // If the image is 32-bit (4 channels), then we need to specify GL_RGBA for an alpha
  if(pImage->channels == 4)
    textureType = GL_RGBA;
    
  // Build Mipmaps (builds different versions of the picture for distances - looks better)
  rnd::texImageMipmapped(pImage->sizeX, pImage->sizeY, pImage->channels,
            pImage->data);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


  // Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR_MIPMAP_LINEAR
  // is the smoothest.  GL_LINEAR_MIPMAP_NEAREST is faster than GL_LINEAR_MIPMAP_LINEAR, 
  // but looks blochy and pixilated.  Good for slower computers though. 
    
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  

  // Now we need to free the image data that we loaded since OpenGL stored it as a texture

  if (pImage)                    // If we loaded the image
  {
    if (pImage->data)              // If there is texture data
    {
      delete[] pImage->data;          // Free the texture data, we don't need it anymore
    }

    delete pImage;                // Free the image structure
  }
}



enum e_FILE_TYPE {
  F_BMP=0,
  F_TGA,
  F_JPG
};


void CTexture::onCreateWindow() {
  e_FILE_TYPE type;
  m_texture = new GLuint [1];
  
  int last_four = (int) (m_fname.length()-4);
  
  if (last_four<0) return;
  
  tImage * pImage;
  
  if (m_fname.substr(last_four) == ".bmp") {
    type = F_BMP;
  } else if (m_fname.substr(last_four) == ".tga") {
    type = F_TGA;
  } else if (m_fname.substr(last_four) == ".jpg") {
    type = F_JPG;
  } else {
    *output << "Unknown extension : " << m_fname.substr(last_four) << endl;
    return;
  }
  
  if (type == F_BMP) {
    pImage = new textureImage;
    if (loadBMP(m_fname.c_str(), (textureImage *) pImage))
    {
      m_success = true;
    }
  } else if (type == F_TGA) {
      pImage = LoadTGA(m_fname.c_str());      // Load the image and store the data

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
  

  if(pImage == NULL) {                  // If we can't load the file, quit!
    *output << "Can't load the file : " << m_fname <<"\n";
    return;
        }
        m_success = true;
  } else if (type == F_JPG) {
    pImage = LoadJPG(m_fname.c_str());  // Load the image and store the data
    if(pImage == NULL) {  // If we can't load the file, quit!
      *output << "Can't load the file : " << m_fname <<"\n";
      return;
    }
    m_success = true;
  }
  
  if (m_success) {
    glGenTextures(1, &m_texture[0]);   /* create textures */
    glBindTexture(GL_TEXTURE_2D, m_texture[0]);
    /* actually generate the texture */
    if (type == F_BMP) {
      rnd::texImageMipmapped(pImage->sizeX, pImage->sizeY, 3, pImage->data);
    } else if (type == F_TGA) {
  int textureType = GL_RGB;

  // If the image is 32-bit (4 channels), then we need to specify GL_RGBA for an alpha
  if (((tImageTGA *)pImage)->channels == 4)
    textureType = GL_RGBA;
    
  // Build Mipmaps (builds different versions of the picture for distances - looks better)
  rnd::texImageMipmapped(pImage->sizeX, pImage->sizeY,
            ((tImageTGA *)pImage)->channels, pImage->data);

    } else if (type == F_JPG) {
  rnd::texImageMipmapped(pImage->sizeX, pImage->sizeY, 3, pImage->data);

    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
      
    /* free the ram we used in our texture generation process */
  if (type == F_BMP) {
    if (pImage)
    {
      if (pImage->data)
        delete [] pImage->data;
      delete pImage;
    }  
  } else if (type == F_TGA) {
    if (m_success)    // If we loaded the image
    {
      if (pImage->data)  // If there is texture data
      {
        delete[] pImage->data;  // Free the texture data, we don't need it anymore
      }

      delete pImage;  // Free the image structure
    }

  } else if (type == F_JPG) {
    if (m_success)  // If we loaded the image
    {
      if (pImage->data)  // If there is texture data
      {
      delete pImage->data;  // Free the texture data, we don't need it anymore
      }

      delete pImage;  // Free the image structure
    }

  }

}



void CTexture::onDestroyWindow() {
  if (m_success) {
    glDeleteTextures(1, m_texture);
  }
  delete [] m_texture;
}





//
// @(#) $Log: textures.cpp,v $
// @(#) Revision 1.18  2003/10/21 08:41:57  virzak
// @(#) Completely rewrote the texture classes.
// @(#) CTexture now can exist without OpenGL context, since it is a module now.
// @(#) It also contains the member filename of the file.
// @(#) CTextureContainer is a class the creates and destroys textures, but not
// @(#) on OpenGL context creations and destructions but on user requests and
// @(#) in its destructor.
// @(#) On OpenGL context creation/destruction every CTexture object performs
// @(#) it's stuff, but called from CTextureContainer callbacks.
// @(#)
// @(#) Revision 1.17  2003/09/11 21:08:52  virzak
// @(#) Changed jpeg handling to assume origin in lower left corner
// @(#)
// @(#) Revision 1.16  2003/09/11 19:34:26  virzak
// @(#) Replaced \t by "  "
// @(#)
// @(#) Revision 1.15  2003/09/11 19:33:17  virzak
// @(#) Came up with a base class for all the textures
// @(#)
// @(#) Revision 1.14  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.13  2003/08/29 08:19:35  virzak
// @(#) Removed unnecessary functions
// @(#)
// @(#) Revision 1.12  2003/08/28 09:09:53  virzak
// @(#) Added functions to initialize common textures
// @(#)
// @(#) Revision 1.11  2003/08/05 19:35:46  virzak
// @(#) Got rid of Windows warning
// @(#)
// @(#) Revision 1.10  2003/08/04 07:44:00  virzak
// @(#) Created the class CTexture, which contains the texture info for
// @(#) texture mapping.
// @(#) Textures supported so far are BMP, JPEG and TARGA.
// @(#) A lot of work is still required
// @(#)
// @(#) Revision 1.9  2003/07/23 03:13:16  virzak
// @(#) fixed bug
// @(#)
// @(#) Revision 1.8  2003/07/20 01:52:45  virzak
// @(#) BUG
// @(#)
// @(#) Revision 1.7  2003/07/20 01:33:02  virzak
// @(#) *** empty log message ***
// @(#)
// @(#) Revision 1.6  2003/07/20 00:51:35  virzak
// @(#) Fixed two windows bugs
// @(#)
// @(#) Revision 1.5  2003/07/20 00:43:13  virzak
// @(#) Added tagra textures
// @(#)
//


