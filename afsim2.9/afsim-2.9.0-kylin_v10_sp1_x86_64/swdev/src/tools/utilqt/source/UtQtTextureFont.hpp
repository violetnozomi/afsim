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

#ifndef TEXTUREFONT_H
#define TEXTUREFONT_H

#include "utilqt_export.h"

#include <qgl.h>

class UTILQT_EXPORT TextureFont
{
public:
   //--------------------------------------------------------------------
   //  Name: TextureFont
   //  Description: This create a TextureFont object which loads
   //               a font texture from the image file named filename.
   //               Note: this class deletes the GL texture for the
   //               font upon destruction.
   //               The font image should contain 16x8 (128) character
   //               subimages and typically is a TGA file where the
   //               font is in the alpha layer and the RGB layer
   //               is all white.
   //--------------------------------------------------------------------
   TextureFont(const char* filename);
   TextureFont(const QFont& aFont);

   virtual ~TextureFont();

   // Text justification enumerations
   typedef enum
   {
      eUndefinedTextJustification   = 0,
      eLeftTextJustification        = 1, // Text to left of ref point
      eRightTextJustification       = 2, // Text to right of ref point
      eCenterHorizTextJustification = 3, // Text centered on ref point
      eCenterVertTextJustification  = 4, // Text centered on ref point
      eTopTextJustification         = 5, // Text above ref point
      eBottomTextJustification      = 6  // Text below ref point
   } eTextureFontTextJustification;

   //--------------------------------------------------------------------
   //  Name: RenderText
   //  Description: This draws the texture font text with the specified
   //               horizontal and vertical justification based on
   //               the location x,y and using the scaling provided by
   //               scaleX and scaleY. The enableTexture flag is used
   //               to use a glEnable(GL_TEXTURE_2D) call and a
   //               glDisable(GL_TEXTURE_2D) call before and after
   //               rendering the text. If such a call is not performed
   //               either internally or externally the font may not be
   //               rendered since it requires the use of textures.
   //--------------------------------------------------------------------

   void RenderText(double                        x,
                   double                        y,
                   const char*                   text,
                   eTextureFontTextJustification horizJustification = eRightTextJustification,
                   eTextureFontTextJustification vertJustification  = eTopTextJustification,
                   int                           rotationAngle      = 0,
                   bool                          enableTexture      = false) const;

   void RenderText(double                        x,
                   double                        y,
                   double                        scaleX,
                   double                        scaleY,
                   const char*                   text,
                   eTextureFontTextJustification horizJustification = eRightTextJustification,
                   eTextureFontTextJustification vertJustification  = eTopTextJustification,
                   int                           rotationAngle      = 0,
                   bool                          enableTexture      = false) const;

   void SetScale(double aScale)
   {
      mScaleX = aScale;
      mScaleY = aScale;
   }
   void SetScaleX(double aScaleX) { mScaleX = aScaleX; }
   void SetScaleY(double aScaleY) { mScaleY = aScaleY; }

   double GetWidth(const char* text) const;
   double GetWidth(const char* text, double aScaleX) const;

   double GetHeight() const;
   double GetHeight(double aScaleY) const;

   double GetFontCharWidth(char aChar) const;
   void   BuildGlyph(GLuint idx, const QFont& aFont, char c);

protected:
   double                 mFontHeight;
   double                 mFontWidth;
   std::map<char, double> mVariableWidth;

   double mScaleX;
   double mScaleY;

   GLuint mFontTextureGL; // Texture used by the font
   GLuint mFontBase;      // Base display list for the font
};

#endif // TEXTUREFONT_H
