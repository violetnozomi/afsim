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

#include "VaDecoratorNode.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QGLWidget>
#include <QImage>
#include <QPainter>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/PolygonStipple>

#include "UtoShaders.hpp"
#include "VaEnvironment.hpp"

vespa::VaDecoratorNode::VaDecoratorNode(int aMode /*= cDefault*/, float aOffsetX /*= 0.0f*/, float aOffsetY /*= 0.0f*/)
   : osg::Geode()
   , mWidth(1)
   , mHeight(1)
   , mText()
   , mOffset(aOffsetX, aOffsetY)
{
   static GLubyte stipple[] = {0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55,
                               0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55,
                               0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55,
                               0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA,
                               0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA,
                               0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA,
                               0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA,
                               0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
                               0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};

   mCentered = aMode & cCentered;

   osg::ref_ptr<osg::Geometry> contentGeometry = new osg::Geometry;
   if (aMode & cSuppressSmallFeatureCulling)
   {
      contentGeometry->setComputeBoundingBoxCallback(new ComputeInstancedBoundingBoxCallback());
   }

   osg::StateSet*  ss = getOrCreateStateSet();
   osg::Vec3Array* v  = new osg::Vec3Array;
   v->push_back(osg::Vec3(0, 1, 0));
   v->push_back(osg::Vec3(1, 1, 0));
   v->push_back(osg::Vec3(1, 0, 0));
   v->push_back(osg::Vec3(0, 0, 0));
   contentGeometry->setVertexArray(v);
   contentGeometry->setSupportsDisplayList(false);
   contentGeometry->setUseDisplayList(false);
   contentGeometry->setUseVertexBufferObjects(true);
   addDrawable(contentGeometry);

   if (aMode & cStemmed)
   {
      mStemGeometry = new osg::Geometry;
      v->push_back(osg::Vec3(aOffsetX, aOffsetY, 0));
      mStemGeometry->setVertexArray(v);
      mStemGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 3, 2));
      addDrawable(mStemGeometry);
   }

   if (aMode & cFramed)
   {
      mBorderGeometry = new osg::Geometry;
      mBorderGeometry->setVertexArray(v);
      mBorderGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
      addDrawable(mBorderGeometry);
   }

   contentGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

   if ((aMode & cDrawWhenOccluded) || (aMode & cHideBehindEarth))
   {
      if (aMode & cStippleWhenOccluded)
      {
         osg::Geometry* stippleGeometry = static_cast<osg::Geometry*>(contentGeometry->clone(osg::CopyOp::DEEP_COPY_ALL));
         addDrawable(stippleGeometry);

         stippleGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonStipple(stipple));
         stippleGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::GREATER));
         stippleGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());

         ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL));
         ss->setAttributeAndModes(new osg::BlendFunc());
      }
      else
      {
         ss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
         ss->setAttributeAndModes(new osg::BlendFunc());
      }
   }
   else
   {
      ss->setAttributeAndModes(new osg::BlendFunc());
   }

   if (aMode & (cStemmed | cFramed))
   {
      osg::LineWidth* linewidth = new osg::LineWidth;
      linewidth->setWidth(2.0f);

      osg::Image*    whiteImg = new osg::Image;
      unsigned char* white    = new unsigned char[4];
      memset(white, 255, 4);
      whiteImg->setImage(1, 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, white, osg::Image::USE_NEW_DELETE);
      osg::Texture2D* tex = new osg::Texture2D(whiteImg);
      tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

      if (aMode & cStemmed)
      {
         mStemGeometry->getOrCreateStateSet()->addUniform(new osg::Uniform("offsetAndScale", osg::Vec4(0, 0, 1, 1)));
         mStemGeometry->getOrCreateStateSet()->addUniform(new osg::Uniform("color", osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)));
         mStemGeometry->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
         mStemGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
      }
      if (aMode & cFramed)
      {
         mBorderGeometry->getOrCreateStateSet()->addUniform(new osg::Uniform("color", osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f)));
         mBorderGeometry->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
         mBorderGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
      }
   }

   std::string type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
   if (aMode & cHideBehindEarth)
   {
      ss->setDefine("HIDE_BEHIND_EARTH");
   }
   osg::ref_ptr<osg::Program> prog = UtoShaders::UseProgram("decorator" + type + ".vert", "decorator" + type + ".frag");
   ss->addUniform(new osg::Uniform("color", osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)));
   ss->setAttribute(prog, osg::StateAttribute::ON);
   ss->setRenderBinDetails(1000, "RenderBin");
}

void vespa::VaDecoratorNode::SetImage(const QImage& aImage, const QString& aImageDescription)
{
   mText.clear();
   mImageDescription = aImageDescription;
   SetTextureFromImage(aImage, osg::Texture2D::LINEAR);

   mWidth  = aImage.width();
   mHeight = aImage.height();
   AdjustGeometry(mWidth, mHeight);
}

void vespa::VaDecoratorNode::SetText(const QString& aText,
                                     const QColor&  aBackgroundColor,
                                     const QColor&  aTextColor,
                                     TextMode       aMode)
{
   mText = aText;
   mImageDescription.clear();

   int   hdpif = QApplication::desktop()->devicePixelRatio();
   QRect initR(0, 0, 128, 128);
   QRect r = QApplication::fontMetrics().boundingRect(initR, Qt::TextWordWrap, aText);
   r.setWidth(r.width() * hdpif);
   r.setHeight(r.height() * hdpif);
   int     w = r.width() + 6;
   int     h = r.height() + 6;
   QPixmap pixmap(w, h);
   pixmap.fill(
      QColor(aBackgroundColor.red(), aBackgroundColor.green(), aBackgroundColor.blue(), aBackgroundColor.alpha() * 0.5));
   QPainter painter(&pixmap);
   QFont    f = painter.font();
   f.setPointSize(f.pointSize() * hdpif);
   painter.setFont(f);
   if (aMode == cBackdrop)
   {
      painter.setPen(Qt::black);
      painter.drawText(QRect(2, h - r.height() - 2, r.width(), r.height()), Qt::TextWordWrap, aText);
   }
   if (aMode == cOutline)
   {
      painter.setPen(Qt::black);
      painter.drawText(QRect(2, h - r.height() - 2, r.width(), r.height()), Qt::TextWordWrap, aText);
      painter.drawText(QRect(2, h - r.height() - 4, r.width(), r.height()), Qt::TextWordWrap, aText);
      painter.drawText(QRect(4, h - r.height() - 2, r.width(), r.height()), Qt::TextWordWrap, aText);
      painter.drawText(QRect(4, h - r.height() - 4, r.width(), r.height()), Qt::TextWordWrap, aText);
   }
   painter.setPen(aTextColor);
   painter.drawText(QRect(3, h - r.height() - 3, r.width(), r.height()), Qt::TextWordWrap, aText);

   QImage image = pixmap.toImage();
   SetTextureFromImage(image, osg::Texture2D::NEAREST);

   mWidth  = r.width() + 6;
   mHeight = r.height() + 6;
   AdjustGeometry(mWidth, mHeight);
}

void vespa::VaDecoratorNode::SetText(const QString& aText,
                                     const QColor&  aTextColor,
                                     const QFont&   aFont,
                                     TextMode       aMode /*= cStandard*/)
{
   mText = aText;
   mImageDescription.clear();

   int   hdpif = QApplication::desktop()->devicePixelRatio();
   QRect initR(0, 0, 128, 128);
   QRect r = QFontMetrics(aFont).boundingRect(aText);
   r.setWidth(r.width() * hdpif);
   r.setHeight(r.height() * hdpif);
   int     w = r.width() + 6;
   int     h = r.height() + 6;
   QPixmap pixmap(w, h);
   if (aMode == cRectangle)
   {
      pixmap.fill(QColor(80, 80, 80, 220));
   }
   else
   {
      pixmap.fill(QColor(aTextColor.red(), aTextColor.green(), aTextColor.blue(), 0.0));
   }
   QPainter painter(&pixmap);
   QFont    f = aFont;
   f.setPointSize(f.pointSize() * hdpif);
   painter.setFont(f);
   if (aMode == cBackdrop)
   {
      painter.setPen(Qt::black);
      painter.drawText(QRect(2, h - r.height() - 2, r.width(), r.height()), 0, aText);
   }
   if (aMode == cOutline)
   {
      painter.setPen(Qt::black);
      painter.drawText(QRect(2, h - r.height() - 2, r.width(), r.height()), 0, aText);
      painter.drawText(QRect(2, h - r.height() - 4, r.width(), r.height()), 0, aText);
      painter.drawText(QRect(4, h - r.height() - 2, r.width(), r.height()), 0, aText);
      painter.drawText(QRect(4, h - r.height() - 4, r.width(), r.height()), 0, aText);
   }
   painter.setPen(aTextColor);
   painter.drawText(QRect(3, h - r.height() - 3, r.width(), r.height()), 0, aText);

   QImage image = pixmap.toImage();
   SetTextureFromImage(image, osg::Texture2D::NEAREST);

   mWidth  = r.width() + 6;
   mHeight = r.height() + 6;
   AdjustGeometry(mWidth, mHeight);
}

void vespa::VaDecoratorNode::SetColor(float aRed, float aGreen, float aBlue, float aAlpha)
{
   if (mBorderGeometry)
   {
      mBorderGeometry->getOrCreateStateSet()->getUniform("color")->set(osg::Vec4(aRed, aGreen, aBlue, aAlpha));
   }
   if (mStemGeometry)
   {
      mStemGeometry->getOrCreateStateSet()->getUniform("color")->set(osg::Vec4(aRed, aGreen, aBlue, aAlpha));
   }
}

void vespa::VaDecoratorNode::SetOffset(float aOffsetX, float aOffsetY)
{
   mOffset[0] = aOffsetX;
   mOffset[1] = aOffsetY;
   AdjustGeometry(mWidth, mHeight);
}

void vespa::VaDecoratorNode::SetMultColor(float aRed, float aGreen, float aBlue, float aAlpha)
{
   getOrCreateStateSet()->getUniform("color")->set(osg::Vec4(aRed, aGreen, aBlue, aAlpha));
}

void vespa::VaDecoratorNode::SetScale(float aValue)
{
   mScale = aValue;
   AdjustGeometry(mW, mH);
}

void vespa::VaDecoratorNode::SetTextureFromImage(const QImage& aImage, const osg::Texture2D::FilterMode& aFilter)
{
   QImage GL_formatted = QGLWidget::convertToGLFormat(aImage);

   osg::Image*    textureImagePtr = new osg::Image;
   unsigned char* newData         = new unsigned char[aImage.width() * aImage.height() * 4];
   memcpy(newData, GL_formatted.bits(), aImage.width() * aImage.height() * 4);
   textureImagePtr
      ->setImage(aImage.width(), aImage.height(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, newData, osg::Image::USE_NEW_DELETE);
   osg::Texture2D* tex = new osg::Texture2D(textureImagePtr);
   tex->setBorderColor(osg::Vec4d(1.0, 1.0, 1.0, 0));
   tex->setFilter(osg::Texture2D::MIN_FILTER, aFilter);
   tex->setFilter(osg::Texture2D::MAG_FILTER, aFilter);
   tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
   tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
   tex->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
   tex->setResizeNonPowerOfTwoHint(false);
   getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
}

void vespa::VaDecoratorNode::AdjustGeometry(int aWidth, int aHeight)
{
   mW = aWidth;
   mH = aHeight;
   if (mCentered)
   {
      getOrCreateStateSet()->addUniform(
         new osg::Uniform("offsetAndScale", osg::Vec4(-0.5 * aWidth, -0.5 * aHeight, aWidth, aHeight) * mScale));
   }
   else
   {
      getOrCreateStateSet()->addUniform(
         new osg::Uniform("offsetAndScale", osg::Vec4(0.75 * mOffset.x(), 0.75 * mOffset.y(), aWidth, aHeight) * mScale));
   }
}
