//*****************************************************************************
// @(#) $Id: textures.h,v 1.14 2003/12/10 09:32:14 virzak Exp $ 
// 
//  Description: 
//      Provides functions to read bmp and targa files.
//
//  =
//
//*****************************************************************************


#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "render.h"
#include <map>
#include "app.h"
#include "output.h"

using namespace::std;

struct tImage {
  int sizeX;        // The width of the image in pixels
  int sizeY;        // The height of the image in pixels
  unsigned char *data;  // The image pixel data
  virtual ~tImage() {}
};


/* a texture struct */
struct textureImage : public tImage {
};


int loadBMP(const char *filename, textureImage *texture);
bool loadGLTextures(char * filename, GLuint *texture);   /* Load Bitmaps And Convert To Textures */
void CreateTexture(unsigned int textureArray[], char * strFileName, int textureID);



class CTexture : public CWindowDependent {
    GLuint * m_texture;
    bool m_success;
    const string m_fname;
  public:

    CTexture(const string & filename, bool callback=true) : 
        CWindowDependent(callback), m_success(false), m_fname(filename) {}
    ~CTexture() {}

    void glBind() const {
      if (m_success) rnd::bindTexture(m_texture[0]);
    }
    
    virtual void onCreateWindow();
    virtual void onDestroyWindow();
};




// This class contains a map that associates strings with objects

template<class T>
class CNamedContainer {
  
  //public:
  protected:
    // Members
    map<string, T *, less<string> > m_elems;

  public:
    // Destructor
    virtual ~CNamedContainer() {
      while (!m_elems.empty()) {
        delete m_elems.begin()->second;
        m_elems.erase(m_elems.begin());
      }
    }

    // Methods
    virtual void add(const string & name, T * obj) {
      pair<string, T *> entry;
      entry.first = name;
      entry.second = obj;
      m_elems.insert(entry);
    }

    T * find(const string & s) {
      typename map<string, T *, less<string> >::iterator it = m_elems.find(s);
      if (it != m_elems.end()) {
        return it->second;
      } else {
        return NULL;
      }
    }
    
};


class CTextureContainer : 
    public CNamedContainer<CTexture>, 
    public CWindowDependent {

  public:
    // Methods
    void addTexture(const string & tex_name, const string & file_name) {
      add(tex_name, new CTexture(file_name, false));
    }
      
    virtual void onCreateWindow() {
      for (map<string, CTexture *, less<string> >::iterator 
          i = m_elems.begin();
          i != m_elems.end(); 
          ++i) 
      {
        i->second->onCreateWindow();
      }
    }
    
    virtual void onDestroyWindow() {
      for (map<string, CTexture *, less<string> >::iterator 
          i = m_elems.begin();
          i != m_elems.end(); 
          ++i) 
      {
        i->second->onDestroyWindow();
      }
    }

    void glBind(const string & t_name) {
      map<string, CTexture *, less<string> >::const_iterator tex_entry = 
        m_elems.find(t_name);
      
      if ( tex_entry != m_elems.end() ) {
        CTexture * tex = tex_entry->second;
        if (tex) tex->glBind();
      }
      
    }
    
};





#endif


//
// @(#) $Log: textures.h,v $
// @(#) Revision 1.14  2003/12/10 09:32:14  virzak
// @(#) Got rid of g++ warning
// @(#)
// @(#) Revision 1.13  2003/12/08 11:14:15  virzak
// @(#) Removed callbacks
// @(#) Added an argument to CTexture constructor whether to register callbacks
// @(#)
// @(#) Revision 1.12  2003/12/05 17:09:17  virzak
// @(#) Started a template CNamedContainer that contains a map string-T*
// @(#)
// @(#) Revision 1.11  2003/10/21 08:41:57  virzak
// @(#) Completely rewrote the texture classes.
// @(#) CTexture now can exist without OpenGL context, since it is a module now.
// @(#) It also contains the member filename of the file.
// @(#) CTextureContainer is a class the creates and destroys textures, but not
// @(#) on OpenGL context creations and destructions but on user requests and
// @(#) in its destructor.
// @(#) On OpenGL context creation/destruction every CTexture object performs
// @(#) it's stuff, but called from CTextureContainer callbacks.
// @(#)
// @(#) Revision 1.10  2003/09/12 03:30:45  virzak
// @(#) removed unnecessary output
// @(#)
// @(#) Revision 1.9  2003/09/11 19:33:17  virzak
// @(#) Came up with a base class for all the textures
// @(#)
// @(#) Revision 1.8  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.7  2003/09/02 21:47:17  virzak
// @(#) Fixed g++ bugs
// @(#)
// @(#) Revision 1.6  2003/08/29 08:22:00  virzak
// @(#) Added CTextureContainer class that keeps track of textures. It extends
// @(#) CModule class and textures are alocated and deallocated at window
// @(#) creation and destruction
// @(#)
// @(#) Revision 1.5  2003/08/28 09:09:53  virzak
// @(#) Added functions to initialize common textures
// @(#)
// @(#) Revision 1.4  2003/08/04 07:44:00  virzak
// @(#) Created the class CTexture, which contains the texture info for
// @(#) texture mapping.
// @(#) Textures supported so far are BMP, JPEG and TARGA.
// @(#) A lot of work is still required
// @(#)
// @(#) Revision 1.3  2003/07/20 00:43:13  virzak
// @(#) Added tagra textures
// @(#)
//


