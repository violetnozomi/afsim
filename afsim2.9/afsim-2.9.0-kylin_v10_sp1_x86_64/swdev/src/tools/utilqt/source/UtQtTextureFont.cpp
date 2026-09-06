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

#include "UtQtTextureFont.hpp"

#include <math.h>
#include <stdio.h>

#include <QFile>

#include "UtQtTextureTGA.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureFont::TextureFont(const char* filename)
   : mFontHeight(32.0)
   , mFontWidth(16.0)
   , mScaleX(1.0)
   , mScaleY(1.0)
   , mFontTextureGL(0)
   , mFontBase(0)
{
   // Image contains 16x8 = 128 character subimages
   TextureTGA* fontTextureTGA = new TextureTGA(filename);
   mFontTextureGL             = fontTextureTGA->GLTexture();
   delete fontTextureTGA;
   fontTextureTGA = nullptr;

   mFontBase = glGenLists(128);                          // Create 128 display lists for the chars
   for (int charImage = 0; charImage < 128; charImage++) // Loop through the 128 chars
   {
      double cx = float(charImage % 16) / 16.0f; // X Position Of Current Character
      double cy = float(charImage / 16) / 8.0f;  // Y Position Of Current Character

      glNewList(mFontBase + charImage, GL_COMPILE); // Start building a char display list
      glBindTexture(GL_TEXTURE_2D, mFontTextureGL); // Bind to the font texture
      glBegin(GL_QUADS);
      glTexCoord2d(cx, 1.0 - cy - 0.125);
      glVertex2i(0, 0);
      glTexCoord2d(cx + 0.0625f, 1.0 - cy - 0.125);
      glVertex2i(16, 0);
      glTexCoord2d(cx + 0.0625f, 1.0 - cy);
      glVertex2i(16, 32);
      glTexCoord2d(cx, 1.0 - cy);
      glVertex2i(0, 32);
      glEnd();
      glEndList();
   }
}

// if using a qfont, it is not recommended to apply any scaling
TextureFont::TextureFont(const QFont& aFont)
   : mFontWidth(0.0)
   , mScaleX(1.0)
   , mScaleY(1.0)
   , mFontTextureGL(0)
   , mFontBase(0)
{
   QString text =
      " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"; // 32-126
   QFontMetrics fm(aFont);
   int          w = fm.width(text);
   int          h = fm.height() + fm.descent();
   mFontHeight    = h;
   QPixmap fontImage(QSize(w, h));
   fontImage.fill(Qt::transparent);
   QPen     white(QColor(255, 255, 255), 1);
   QPainter p(&fontImage);
   p.setPen(white);
   p.setFont(aFont);
   p.drawText(0, fm.height(), text);
   // Generate the GL texture
   glGenTextures(1, &mFontTextureGL);
   glBindTexture(GL_TEXTURE_2D, mFontTextureGL);
   glTexImage2D(GL_TEXTURE_2D, 0, fontImage.depth() / 8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(fontImage.toImage().bits()));
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // TODO want to use GL_NEAREST for these


   mFontBase = glGenLists(128);                          // Create 128 display lists for the chars
   for (int charImage = 0; charImage < 128; charImage++) // Loop through the 128 chars
   {
      if (charImage <= text.length())
      {
         int cw                                     = fm.width(text[charImage].toLatin1());
         mVariableWidth[text[charImage].toLatin1()] = cw;
         int   iminx                                = fm.width(text.mid(0, charImage));
         int   imaxx                                = iminx + cw;
         float cminx                                = iminx / (float)w;
         float cminy                                = 0.0f;
         float cmaxx                                = imaxx / (float)w;
         float cmaxy                                = 1.0f;

         glNewList(mFontBase + charImage, GL_COMPILE); // Start building a char display list
         glBindTexture(GL_TEXTURE_2D, mFontTextureGL); // Bind to the font texture
         glBegin(GL_QUADS);
         glTexCoord2d(cminx, cmaxy);
         glVertex2i(0, 0);
         glTexCoord2d(cmaxx, cmaxy);
         glVertex2i(cw, 0);
         glTexCoord2d(cmaxx, cminy);
         glVertex2i(cw, h);
         glTexCoord2d(cminx, cminy);
         glVertex2i(0, h);
         glEnd();
         glEndList();
      }
      else
      {
         glNewList(mFontBase + charImage, GL_COMPILE);
         glEndList();
      }
   }
}

TextureFont::~TextureFont()
{
   glDeleteLists(mFontBase, 128);        // Delete all of the font display lists (128 in total)
   glDeleteTextures(1, &mFontTextureGL); // Delete the font texture
}

void TextureFont::RenderText(double                        x,
                             double                        y,
                             const char*                   text,
                             eTextureFontTextJustification horizJustification,
                             eTextureFontTextJustification vertJustification,
                             int                           rotationAngle,
                             bool                          enableTexture) const
{
   RenderText(x, y, mScaleX, mScaleY, text, horizJustification, vertJustification, rotationAngle, enableTexture);
}

void TextureFont::RenderText(double                        x,
                             double                        y,
                             double                        scaleX,
                             double                        scaleY,
                             const char*                   text,
                             eTextureFontTextJustification horizJustification,
                             eTextureFontTextJustification vertJustification,
                             int                           rotationAngle,
                             bool                          enableTexture) const
{
   size_t num    = strlen(text);
   double height = mFontHeight;
   double width  = GetWidth(text, 1.0); // mFontWidth * num;
   double dx     = 0.0;
   double dy     = 0.0;

   if (horizJustification == eRightTextJustification)
   {
      dx = -width;
   }
   else if (horizJustification == eCenterHorizTextJustification)
   {
      dx = -width * 0.5;
   }
   else if (horizJustification == eLeftTextJustification)
   {
      dx = 0.0;
   }

   if (vertJustification == eBottomTextJustification)
   {
      dy = 0.0;
   }
   else if (vertJustification == eCenterVertTextJustification)
   {
      dy = -height * 0.5;
   }
   else if (vertJustification == eTopTextJustification)
   {
      dy = -height;
   }

   if (enableTexture)
      glEnable(GL_TEXTURE_2D);
   glPushMatrix();
   glTranslated(x, y, 0);
   glRotated(rotationAngle, 0, 0, 1);
   glScaled(scaleX, scaleY, 0);
   glTranslated(dx, dy, 0);
   for (size_t i = 0; i < num; ++i)
   {
      glCallList(mFontBase - 32 + text[i]);
      glTranslated(GetFontCharWidth(text[i]), 0.0, 0.0);
   }
   glPopMatrix();
   if (enableTexture)
      glDisable(GL_TEXTURE_2D);
}

double TextureFont::GetWidth(const char* text) const
{
   return GetWidth(text, mScaleX);
}

double TextureFont::GetHeight() const
{
   return GetHeight(mScaleY);
}

double TextureFont::GetHeight(double aScaleY) const
{
   return mFontHeight * aScaleY;
}

double TextureFont::GetWidth(const char* aText, double aScaleX) const
{
   size_t len = strlen(aText);
   if (mFontWidth > 0)
   {
      return len * mFontWidth * aScaleX;
   }
   else
   {
      double tw = 0.0;
      for (size_t i = 0; i < len; ++i)
      {
         tw += GetFontCharWidth(aText[i]);
      }
      return tw * aScaleX;
   }
}

double TextureFont::GetFontCharWidth(char aChar) const
{
   if (mFontWidth > 0)
   {
      return mFontWidth;
   }
   else
   {
      std::map<char, double>::const_iterator it = mVariableWidth.find(aChar);
      if (it != mVariableWidth.end())
      {
         return it->second;
      }
      else
      {
         return 0.0;
      }
   }
}
