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

#include "UtQtTextureTGA.hpp"

#include <cstdio>

#include <QFile>
#include <QMessageBox>

#include "UtCast.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureTGA::TextureTGA(const char* filename)
   : mTexture(0)
{
   if (LoadTGA(filename))
   {
      // Generate the GL texture
      glGenTextures(1, &mTexID);
      glBindTexture(GL_TEXTURE_2D, mTexID);
      glTexImage2D(GL_TEXTURE_2D, 0, mBpp / 8, mWidth, mHeight, 0, mType, GL_UNSIGNED_BYTE, mImageData);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (mImageData)
      {
         free(mImageData); // Free the texture image memory
      }

      if (mTexID > 0)
         mTexture = mTexID;
   }
}

TextureTGA::~TextureTGA() {}

GLuint TextureTGA::GLTexture()
{
   return mTexture;
}

bool TextureTGA::LoadTGA(const char* filename)
{
   QFile file(filename);
   if (!file.open(QIODevice::ReadOnly))
   {
      QString errorMsg = "Could not open texture file " + QString(filename) + " in TextureTGA::LoadTGA";
      QMessageBox::information(nullptr, "TextureTGA", errorMsg);
      return false;
   }
   QByteArray bytes = file.readAll();
   file.close();

   sTGAHeader tgaheader;
   if (bytes.size() < ut::cast_to_int(sizeof(sTGAHeader)))
   {
      QString errorMsg = "Texture File " + QString(filename) + " does not contain TGA header in TextureTGA::LoadTGA";
      QMessageBox::information(nullptr, "TextureTGA", errorMsg);
      return false;
   }
   memcpy(&tgaheader, bytes, sizeof(sTGAHeader));

   const GLubyte uncompressedTGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   const GLubyte compressedTGAheader[12]   = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   // See if header matches the predefined header of uncompressed or compressed TGA images
   if (memcmp(uncompressedTGAheader, &tgaheader, sizeof(tgaheader)) == 0)
   {
      // Load the uncompressed image
      LoadUncompressedTGA(bytes);
   }
   else if (memcmp(compressedTGAheader, &tgaheader, sizeof(tgaheader)) == 0)
   {
      // Load the compressed image
      LoadCompressedTGA(bytes);
   }
   else
   {
      QString errorMsg = "TGA file must be type 2 or type 10 in TextureTGA::LoadTGA";
      QMessageBox::information(nullptr, "Invalid Image", errorMsg);
      return false;
   }
   return true;
}

bool TextureTGA::LoadUncompressedTGA(QByteArray bytes)
{
   const char*  data   = bytes.constData();
   unsigned int offset = sizeof(sTGAHeader);

   // The following is unused and not needed
   //    sTGAHeader tgaheader;
   //    memcpy(tgaheader, data, sizeof(sTGAHeader));

   sTGAParams tgaParams;
   memcpy(tgaParams.header, data + offset, sizeof(tgaParams.header));
   offset += sizeof(tgaParams.header);

   mWidth          = tgaParams.header[1] * 256 + tgaParams.header[0]; // Determine The TGA Width  (highbyte*256+lowbyte)
   mHeight         = tgaParams.header[3] * 256 + tgaParams.header[2]; // Determine The TGA Height (highbyte*256+lowbyte)
   mBpp            = tgaParams.header[4];
   tgaParams.width = mWidth;
   tgaParams.height = mHeight;
   tgaParams.bpp    = mBpp;

   if ((mWidth <= 0) || (mHeight <= 0) || ((mBpp != 24) && (mBpp != 32))) // Confirm all information is valid
   {
      QString errorMsg = "Invalid texture information in TextureTGA::LoadUncompressedTGA";
      QMessageBox::information(nullptr, "ERROR", errorMsg);
      return false;
   }

   if (mBpp == 24)
      mType = GL_RGB;
   else
      mType = GL_RGBA;

   tgaParams.bytesPerPixel = (tgaParams.bpp / 8);
   tgaParams.imageSize = (tgaParams.bytesPerPixel * tgaParams.width * tgaParams.height); // Compute total memory needed
   mImageData          = (GLubyte*)malloc(tgaParams.imageSize);

   if (mImageData == nullptr)
   {
      QString errorMsg = "Could not allocate memory for image in TextureTGA::LoadUncompressedTGA";
      QMessageBox::information(nullptr, "ERROR", errorMsg);
      return false;
   }

   memcpy(mImageData, data + offset, tgaParams.imageSize);

   // Byte swapping optimized by Steve Thomas
   for (GLuint cswap = 0; cswap < tgaParams.imageSize - (GLuint)2; cswap += tgaParams.bytesPerPixel)
   {
      mImageData[cswap] ^= mImageData[cswap + 2] ^= mImageData[cswap] ^= mImageData[cswap + 2];
   }

   return true;
}

bool TextureTGA::LoadCompressedTGA(QByteArray bytes)
{
   const char* data = bytes.constData();
   /*
   FILE* fTGA;
   fTGA = fopen( filename, "rb" );

   if( fTGA == NULL )
   {
      QString errorMsg = "Could not open texture file " + QString(filename) + " in TextureTGA::LoadCompressedTGA";
      QMessageBox::information(0, "TextureTGA", errorMsg);
      return false;
   }*/

   sTGAHeader tgaheader;
   memcpy(&tgaheader, data, sizeof(sTGAHeader));
   unsigned int offset = sizeof(sTGAHeader);

   /*
   // Read the top header
   if( fread( &tgaheader, sizeof(sTGAHeader), 1, fTGA ) == 0 )
   {
      QString errorMsg = "Could not read file header for " + QString(filename) + " in TextureTGA::LoadCompressedTGA";
      QMessageBox::information(0, "TextureTGA", errorMsg);

      if( fTGA != NULL )
      {
         fclose(fTGA);
      }
      return false;
   }*/

   sTGAParams tgaParams;
   memcpy(tgaParams.header, data + offset, sizeof(tgaParams.header));
   offset += sizeof(tgaParams.header);

   /*
   if(fread(tgaParams.header, sizeof(tgaParams.header), 1, fTGA) == 0) // Attempt to read header
   {
      QString errorMsg = "Could not read info header in TextureTGA::LoadCompressedTGA";
      QMessageBox::information(0, "ERROR", errorMsg);
      if(fTGA != NULL)
      {
         fclose(fTGA);
      }
      return false;
   }*/

   mWidth           = tgaParams.header[1] * 256 + tgaParams.header[0]; // TGA Width  (highbyte*256+lowbyte)
   mHeight          = tgaParams.header[3] * 256 + tgaParams.header[2]; // TGA Height (highbyte*256+lowbyte)
   mBpp             = tgaParams.header[4];
   tgaParams.width  = mWidth;
   tgaParams.height = mHeight;
   tgaParams.bpp    = mBpp;

   if ((mWidth <= 0) || (mHeight <= 0) || ((mBpp != 24) && (mBpp != 32))) // Confirm all texture info is ok
   {
      QString errorMsg = "Invalid texture information in TextureTGA::LoadCompressedTGA";
      QMessageBox::information(nullptr, "ERROR", errorMsg);
      /*
      if(fTGA != NULL)
      {
         fclose(fTGA);
      }*/
      return false;
   }

   if (mBpp == 24)
      mType = GL_RGB;
   else
      mType = GL_RGBA;

   tgaParams.bytesPerPixel = (tgaParams.bpp / 8);
   tgaParams.imageSize =
      (tgaParams.bytesPerPixel * tgaParams.width * tgaParams.height); // Determine memory needed to store image
   mImageData = (GLubyte*)malloc(tgaParams.imageSize);

   if (mImageData == nullptr)
   {
      QString errorMsg = "Could not allocate memory for image in TextureTGA::LoadCompressedTGA";
      QMessageBox::information(nullptr, "ERROR", errorMsg);
      // fclose(fTGA);
      return false;
   }

   GLuint   pixelcount   = tgaParams.height * tgaParams.width;        // Number of pixels in the image
   GLuint   currentpixel = 0;                                         // Current pixel being read
   GLuint   currentbyte  = 0;                                         // Current byte
   GLubyte* colorbuffer  = (GLubyte*)malloc(tgaParams.bytesPerPixel); // Storage for 1 pixel

   do
   {
      GLubyte chunkheader = 0; // Storage for "chunk" header

      memcpy(&chunkheader, data + offset, sizeof(GLubyte));
      offset += sizeof(GLubyte);
      /*
      if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) // Read in the 1 byte header
      {
         QString errorMsg = "Could not read RLE header in TextureTGA::LoadCompressedTGA";
         QMessageBox::information(0, "ERROR", errorMsg);

         if(fTGA != NULL)
         {
            fclose(fTGA);
         }
         if(mImageData != NULL)
         {
            free(mImageData);
         }
         return false;
      }*/

      if (chunkheader < 128) // If the ehader is < 128, it means the that is the number of RAW color packets minus 1
      {                      // that follow the header

         chunkheader++; // add 1 to get number of following color values

         for (short counter = 0; counter < chunkheader; counter++) // Read RAW color values
         {
            memcpy(colorbuffer, data + offset, tgaParams.bytesPerPixel);
            offset += tgaParams.bytesPerPixel;
            /*
            if(fread(colorbuffer, 1, tgaParams.bytesPerPixel, fTGA) != tgaParams.bytesPerPixel) // Try to read 1 pixel
            {
               QString errorMsg = "Could not read image data in TextureTGA::LoadCompressedTGA";
               QMessageBox::information(0, "ERROR", errorMsg);

               if(fTGA != NULL)
               {
                  fclose(fTGA);
               }

               if(colorbuffer != NULL)
               {
                  free(colorbuffer);
               }

               if(mImageData != NULL)
               {
                  free(mImageData);
               }

               return false;
            }
            */
            // Write to memory
            mImageData[currentbyte]     = colorbuffer[2]; // Flip R and B color values around in the process
            mImageData[currentbyte + 1] = colorbuffer[1];
            mImageData[currentbyte + 2] = colorbuffer[0];

            if (tgaParams.bytesPerPixel == 4) // if its a 32 bpp image
            {
               mImageData[currentbyte + 3] = colorbuffer[3]; // copy the 4th byte
            }

            currentbyte += tgaParams.bytesPerPixel; // Increase thecurrent byte by the number of bytes per pixel
            currentpixel++;                         // Increase current pixel by 1

            if (currentpixel > pixelcount) // Ensure that we have not read too many pixels
            {
               QString errorMsg = "Too many pixels read in TextureTGA::LoadCompressedTGA";
               QMessageBox::information(nullptr, "ERROR", errorMsg);

               // if(fTGA != NULL)
               //{
               //    fclose(fTGA);
               // }

               if (colorbuffer != nullptr)
               {
                  free(colorbuffer);
               }

               if (mImageData != nullptr)
               {
                  free(mImageData);
               }

               return false;
            }
         }
      }
      else // chunkheader > 128 RLE data, next color repeated chunkheader - 127 times
      {
         chunkheader -= 127; // Subtract 127 to get rid of the ID bit

         memcpy(colorbuffer, data + offset, tgaParams.bytesPerPixel);
         offset += tgaParams.bytesPerPixel;
         /*
         if(fread(colorbuffer, 1, tgaParams.bytesPerPixel, fTGA) != tgaParams.bytesPerPixel) // Attempt to read
         following color values
         {
            QString errorMsg = "Could not read from file in TextureTGA::LoadCompressedTGA";
            QMessageBox::information(0, "ERROR", errorMsg);

            if(fTGA != NULL)
            {
               fclose(fTGA);
            }

            if(colorbuffer != NULL)
            {
               free(colorbuffer);
            }

            if(mImageData != NULL)
            {
               free(mImageData);
            }

            return false;
         }*/

         // Copy the color into the image data as many times as dictated by the header
         for (short counter = 0; counter < chunkheader; counter++)
         {
            mImageData[currentbyte]     = colorbuffer[2]; // Switch R and B bytes around while copying
            mImageData[currentbyte + 1] = colorbuffer[1];
            mImageData[currentbyte + 2] = colorbuffer[0];

            if (tgaParams.bytesPerPixel == 4) // If TGA images is 32 bpp
            {
               mImageData[currentbyte + 3] = colorbuffer[3]; // Copy 4th byte
            }

            currentbyte += tgaParams.bytesPerPixel; // Increase current byte by the number of bytes per pixel
            currentpixel++;                         // Increase pixel count by 1

            if (currentpixel > pixelcount) // Ensure that we have not written too many pixels
            {
               QString errorMsg = "Too many pixels read in TextureTGA::LoadCompressedTGA";
               QMessageBox::information(nullptr, "ERROR", errorMsg);

               return false;
            }
         }
      }
   } while (currentpixel < pixelcount); // Loop while there are still pixels left

   // fclose(fTGA);                       // Close the file*/
   return true; // Return success
}
