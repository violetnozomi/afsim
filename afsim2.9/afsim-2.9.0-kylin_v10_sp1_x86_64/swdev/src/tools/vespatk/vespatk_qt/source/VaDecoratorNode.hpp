// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VADECORATORNODE_HPP
#define VADECORATORNODE_HPP

#include "vespatk_qt_export.h"

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>

class QColor;
class QFont;
class QImage;
#include <QString>

namespace vespa
{
// this class will draw Qt styled text in the scene with the use of SetText or
// a QImage using SetImage

class VESPATK_QT_EXPORT VaDecoratorNode : public osg::Geode
{
public:
   enum ModeBitMap
   {
      cDefault             = 0,
      cStemmed             = 1, // draw a stem from 0,0 to the offset
      cFramed              = 2, // draw a frame around the decorator
      cDrawWhenOccluded    = 4, // draw the decorator when occluded
      cStippleWhenOccluded = 8, // draw the decorator with a stipple effect when occluded
      cSuppressSmallFeatureCulling =
         16,                 // suppress OSG's desire to cull these out when they get relatively far from the camera
      cCentered        = 32, // center the image.  Note that this is incompatible with other offsets
      cHideBehindEarth = 64  // disable depth testing, but use shader to hide behind the earth
   };
   enum TextMode
   {
      cStandard  = 0,
      cBackdrop  = 1,
      cOutline   = 2,
      cRectangle = 3
   };

   VaDecoratorNode(int aMode = cDefault, float aOffsetX = 0.0f, float aOffsetY = 0.0f);
   ~VaDecoratorNode() override = default;

   void SetImage(const QImage& aImage, const QString& aImageDescription);
   void SetText(const QString& aText, const QColor& aBackground, const QColor& aTextColor, TextMode aMode = cStandard);
   void SetText(const QString& aText, const QColor& aTextColor, const QFont& aFont, TextMode aMode = cStandard);
   const QString& GetText() const { return mText; }
   const QString& GetImageDescription() const { return mImageDescription; }

   void SetColor(float aRed, float aGreen, float aBlue, float aAlpha);
   void SetOffset(float aOffsetX, float aOffsetY);
   void SetMultColor(float aRed, float aGreen, float aBlue, float aAlpha);

   void SetXOffset(float aOffset) { mOffset.x() = aOffset; }
   void SetYOffset(float aOffset) { mOffset.y() = aOffset; }

   // if you are using the decorator node for UI or text, do not use this.  It will make things blurry.
   // unscaled decorators should be pixel perfect.
   void SetScale(float aValue);

   int GetWidth() const { return mWidth; }
   int GetHeight() const { return mHeight; }

private:
   void SetTextureFromImage(const QImage& aImage, const osg::Texture2D::FilterMode& aFilter);
   void AdjustGeometry(int aWidth, int aHeight);

   class ComputeInstancedBoundingBoxCallback : public osg::Drawable::ComputeBoundingBoxCallback
   {
   public:
      osg::BoundingBox computeBound(const osg::Drawable& drawable) const override
      {
         osg::BoundingBox bounds;
         bounds.expandBy(120000000, 120000000, 120000000);
         bounds.expandBy(-120000000, -120000000, -120000000);
         return bounds;
      }
   };

   int                         mWidth;
   int                         mHeight;
   osg::ref_ptr<osg::Geometry> mBorderGeometry;
   osg::ref_ptr<osg::Geometry> mStemGeometry;
   QString                     mText;
   QString                     mImageDescription;
   osg::Vec2                   mOffset;
   bool                        mCentered{false};
   float                       mScale{1.0f};
   int                         mW{32};
   int                         mH{32};
};
} // namespace vespa

#endif
