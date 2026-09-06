// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef TextureTGA_H
#define TextureTGA_H

#include "utilqt_export.h"

#include <qgl.h>

class UTILQT_EXPORT TextureTGA
{
public:
   //--------------------------------------------------------------------
   //  Name: TextureTGA
   //  Description: This create a TextureTGA object which loads
   //               a GL texture from the image file named filename.
   //               Note: the GL texture returned is not deleted by this
   //               class -- the caller is responsible for deleting
   //               the texture data.
   //--------------------------------------------------------------------
   TextureTGA(const char* filename);
   virtual ~TextureTGA();

   //--------------------------------------------------------------------
   //  Name: GLTexture
   //  Description: Returns the GL texture id.
   //--------------------------------------------------------------------
   GLuint GLTexture();

protected:
   bool LoadTGA(const char* filename);
   bool LoadUncompressedTGA(QByteArray bytes);
   bool LoadCompressedTGA(QByteArray bytes);

   GLuint   mTexture;   // GL texture
   GLubyte* mImageData; // Image Data (Up To 32 Bits)
   GLuint   mBpp;       // Image Color Depth In Bits Per Pixel
   GLuint   mWidth;     // Image Width
   GLuint   mHeight;    // Image Height
   GLuint   mTexID;     // Texture ID Used To Select A Texture
   GLuint   mType;      // Image Type (GL_RGB, GL_RGBA)

   // Structure for the TGA file header
   typedef struct
   {
      GLubyte Header[12]; // TGA file header data
   } sTGAHeader;

   // Structure for TGA image paramaters
   typedef struct
   {
      GLubyte header[6];     // First 6 useful bytes from header
      GLuint  bytesPerPixel; // Holds number of bytes per pixel
      GLuint  imageSize;     // Size of image in bytes
      GLuint  unused;        // Unused variable
      GLuint  type;          // Type of data (RGB, RGBA)
      GLuint  height;        // Image height
      GLuint  width;         // Image width
      GLuint  bpp;           // Bits per pixel
   } sTGAParams;
};

#endif // TextureTGA_H
