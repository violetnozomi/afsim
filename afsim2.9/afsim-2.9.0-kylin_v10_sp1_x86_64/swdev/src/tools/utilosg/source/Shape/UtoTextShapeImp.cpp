// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// TextShapeImp.cpp : implementation file

// project includes
#include "UtoTextShapeImp.hpp"

#include <string.h>

#include "UtCast.hpp"
#include "UtoEntity.hpp"
#include "UtoGeometryUtil.hpp"
#include "UtoTextShape.hpp"
#include "UtoViewer.hpp"

// osg includes
#include <osg/LineWidth>
#include <osg/PolygonOffset>
#include <osgText/Text>

// compares the content of an osg string with the contents of
// a std string.  if we decided to go outside the standard
// format, this function will need to be updated to match that.
bool CompareStrings(const osgText::String& rStringOSG, const UtoWString& rStringSTD)
{
   bool bSame = true;
   if (rStringOSG.size() == rStringSTD.size())
   {
      osgText::String::const_iterator lhsIter = rStringOSG.begin();
      UtoWString::const_iterator      rhsIter = rStringSTD.begin();
      for (; (lhsIter != rStringOSG.end()) && (rhsIter != rStringSTD.end()); ++lhsIter, ++rhsIter)
      {
         if ((UtoWString::value_type)*lhsIter != *rhsIter)
         {
            bSame = false;
            break;
         }
      }
   }
   return bSame;
}

UtoTextShapeImp::UtoTextShapeImp(const UtoWString& sString)
   : UtoGeodeShapeImp()
   , mTextNode(new osgText::Text())
   , m_Alignment(UtoTextShape::LEFT | UtoTextShape::BOTTOM)
   , m_nBackdropType(UtoTextShape::BACKDROP_NONE)
   , m_nBackdropImpType(UtoTextShape::BACKDROP_IMP_POLYGON_OFFSET)
   , m_oBackdropColor(0x00, 0x00, 0x00, 0x00)
{
   mTextNode->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
   mTextNode->setAxisAlignment(osgText::Text::SCREEN);

   // create local parameters
   std::string strFontFace = "arial.ttf";

   // set the shape to the default text
   SetFont(strFontFace);

   SetColor(UtoColor(255, 255, 255), UtoColor::RGBA);
   SetString(sString);
   SetSize(12);
   m_Geode->addDrawable(mTextNode.get());

   // automatically force the text to update
   mTextNode->update();
}

UtoTextShapeImp::UtoTextShapeImp(const UtoTextShapeImp& rhs)
   : UtoGeodeShapeImp(rhs)
   , mTextNode(new osgText::Text(*rhs.mTextNode.get()))
   , m_String(rhs.m_String)
   , m_nBackdropType(rhs.m_nBackdropType)
   , m_nBackdropImpType(rhs.m_nBackdropImpType)
   , m_oBackdropColor(rhs.m_oBackdropColor)
   , m_Size(rhs.m_Size)
{
   m_Geode->addDrawable(mTextNode.get());
   SetFont(rhs.m_FontFace);

   // Text is rendered normal to the screen and will pick up
   // different lighting since the text's normal is constantly
   // changing.  Disable lighting and hope no one will miss it.
   osg::StateSet* stateset = m_Geode->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

   SetupCallback();

   // automatically force the text to update
   mTextNode->update();
}

UtoTextShapeImp::~UtoTextShapeImp()
{
   RemoveCallback();
}

bool UtoTextShapeImp::SetString(const UtoWString& sString)
{
   // hang onto the original string.
   m_String = sString;

   mTextNode->setText(sString.c_str());

   // automatically force the text to update
   mTextNode->update();

   return true;
}

void UtoTextShapeImp::SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved)
{
   // call the base class
   UtoShapeImp::SetColor(Color, nClrMask, nReserved);

   // set the transparency
   SetTransparency();

   mTextNode->setColor(
      osg::Vec4(m_oBlendColor.RedF(), m_oBlendColor.GreenF(), m_oBlendColor.BlueF(), m_oBlendColor.AlphaF()));
}

void UtoTextShapeImp::GetBounds(float& aWidth, float& aHeight)
{
   osg::BoundingBox bb = mTextNode->computeBoundingBox();
   aWidth              = bb.xMax();
   aHeight             = bb.yMax();
   ;
}

bool UtoTextShapeImp::SetSize(int nSize)
{
   const float factor   = 113.0 / 40.0;
   float       nextSize = nSize * factor;
   // This increases the font resolution as the text gets bigger so they don't
   // look fuzzy and bad.
   float nextResolution = 32 + floor(nextSize / 100) * 16;
   // float nextResolution = 16+nextSize/100.0*32.0;
   m_Size = nSize;
   mTextNode->setCharacterSize(nextSize);
   mTextNode->setFontResolution((unsigned int)nextResolution, (unsigned int)nextResolution);

   // automatically force the text to update
   mTextNode->update();

   return true;
}

void UtoTextShapeImp::SetShader(osg::Program* aShader)
{
   osg::StateSet* stateset = mTextNode->getOrCreateStateSet();
   stateset->setAttribute(aShader, true);
}

bool UtoTextShapeImp::SetFont(const std::string& FontStyle)
{
   osgText::Font* font = osgText::readFontFile(FontStyle.c_str());
   if (font)
   {
      m_FontFace = FontStyle;
      mTextNode->setFont(font);
      return true;
   }

   // automatically force the text to update
   mTextNode->update();

   return false;
}

void UtoTextShapeImp::SetAlignment(unsigned long flags)
{
   if (!(flags & (UtoTextShape::LEFT | UtoTextShape::CENTER | UtoTextShape::RIGHT)))
   {
      // horizontal alignment has not been specified, default to LEFT
      flags |= UtoTextShape::LEFT;
   }

   if (!(flags & (UtoTextShape::TOP | UtoTextShape::VCENTER | UtoTextShape::BOTTOM)))
   {
      // vertical alignment has not been specified, default to BOTTOM
      flags |= UtoTextShape::BOTTOM;
   }

   m_Alignment = flags;

   osgText::Text::AlignmentType type = osgText::Text::LEFT_BOTTOM;

   if (flags & UtoTextShape::LEFT && flags & UtoTextShape::BOTTOM)
      type = osgText::Text::LEFT_BOTTOM;
   if (flags & UtoTextShape::LEFT && flags & UtoTextShape::VCENTER)
      type = osgText::Text::LEFT_CENTER;
   if (flags & UtoTextShape::LEFT && flags & UtoTextShape::TOP)
      type = osgText::Text::LEFT_TOP;

   if (flags & UtoTextShape::CENTER && flags & UtoTextShape::BOTTOM)
      type = osgText::Text::CENTER_BOTTOM;
   if (flags & UtoTextShape::CENTER && flags & UtoTextShape::VCENTER)
      type = osgText::Text::CENTER_CENTER;
   if (flags & UtoTextShape::CENTER && flags & UtoTextShape::TOP)
      type = osgText::Text::CENTER_TOP;

   if (flags & UtoTextShape::RIGHT && flags & UtoTextShape::BOTTOM)
      type = osgText::Text::RIGHT_BOTTOM;
   if (flags & UtoTextShape::RIGHT && flags & UtoTextShape::VCENTER)
      type = osgText::Text::RIGHT_CENTER;
   if (flags & UtoTextShape::RIGHT && flags & UtoTextShape::TOP)
      type = osgText::Text::RIGHT_TOP;

   mTextNode->setAlignment(type);

   // automatically force the text to update
   mTextNode->update();
}

void UtoTextShapeImp::Update(UtoRenderInfo* info, osg::NodeVisitor* nv)
{
   // The text is auto-scaled to always be bigger than the small feature
   // culling value, but the update doesn't happen until the cull traversal.
   // If it was last displayed small (close to camera), and now it is far
   // away it could be culled out and never be seen again.  Set the minimum
   // bounding sphere size to something that will be visible.

   // Reset the matrix just in case it was an orthographic camera
   // last frame.
   if (m_Owner)
   {
      UtoCameraPerspective* cam = info->m_Viewer->GetPerspective();
      UtoPosition           eye = cam->Position();
      double                epos[3];
      m_Owner->GetLocationWCS(epos);
      // vector shape position
      const osg::Vec3 vspos(m_Position);
      // vector eye (camera) position
      const osg::Vec3 veye(eye);
      // vector entity position
      const osg::Vec3 vepos(epos[0], epos[1], epos[2]);
      const double    height = info->m_Viewer->DisplaySize().cy;
      // vertical OpenGL Units to screen height scaling
      const double scale = 2 * tan(cam->FOV() * M_PI / 180 / 2) / height;

      // eye to shape distance, OpenGL units
      const double shape_depth = (veye - (vepos + vspos)).length();
      // depth * scaling to GL Units
      const double zoom = shape_depth * scale;

      // The plus character + comes out to be 10 pixels wide, use a base
      // size of 10 multiply by zoom and it will be 10 pixels on the screen.
      // It needs to be at least the small feature culling size (default 2
      // pixels).
      const double radius = zoom * 10;

      // last set initial bound and last frame's text bound
      osg::BoundingSphere gsphere = m_Geode->getInitialBound();
      osg::BoundingSphere tsphere = mTextNode->getBound();

      // Only update the bound if the radius is outside of 1/3 to 2 times
      // current radius or if the center has changed significantly.
      if (gsphere._radius < radius / 3 || radius * 2 < gsphere._radius ||
          (tsphere._center - gsphere._center).length2() > radius * radius)
      {
         tsphere._radius = radius;
         m_Geode->setInitialBound(tsphere);
         m_Geode->dirtyBound();
      }
   }
}

void UtoTextShapeImp::SetupCallback()
{
   if (!m_Callback.get())
   {
      m_Callback = new UtoUpdateCallback<UtoTextShapeImp>(this);
   }
   m_Geode->setUpdateCallback(m_Callback.get());
}

void UtoTextShapeImp::RemoveCallback()
{
   m_Geode->setUpdateCallback(nullptr);
}

void UtoTextShapeImp::SetTransparency()
{
   // determine how to set the transparency
   bool bApplyBlend = m_oBlendColor.HasTransparency();

   if (bApplyBlend)
      UtoGeometryUtil::EnableBlend(m_Geode.get());
   else
      UtoGeometryUtil::DisableBlend(m_Geode.get());
}

void UtoTextShapeImp::SetBackdropType(unsigned long nType)
{
   // set the text backdrop type
   mTextNode->setBackdropType((osgText::Text::BackdropType)nType);
   mTextNode->setBackdropImplementation((osgText::Text::BackdropImplementation)m_nBackdropImpType);
   // store the backdrop values
   m_nBackdropType = nType;
}

unsigned long UtoTextShapeImp::BackdropType() const
{
   return ut::safe_cast<unsigned long, int>(mTextNode->getBackdropType());
}

void UtoTextShapeImp::SetBackdropOffset(float fOffset)
{
   mTextNode->setBackdropOffset(fOffset);
}

void UtoTextShapeImp::SetBackdropOffset(float fHorizontal, float fVertical)
{
   mTextNode->setBackdropOffset(fHorizontal, fVertical);
}

float UtoTextShapeImp::BackdropHorizontalOffset() const
{
   return mTextNode->getBackdropHorizontalOffset();
}

float UtoTextShapeImp::BackdropVerticalOffset() const
{
   return mTextNode->getBackdropVerticalOffset();
}

void UtoTextShapeImp::SetBackdropColor(const UtoColor& rColor, unsigned int nClrMask)
{
   m_oBlendColor.Set(rColor, nClrMask);

   osg::Vec4 vBackdropClr(m_oBackdropColor.RedF(),
                          m_oBackdropColor.GreenF(),
                          m_oBackdropColor.BlueF(),
                          m_oBackdropColor.AlphaF());

   mTextNode->setBackdropColor(vBackdropClr);
}

const UtoColor& UtoTextShapeImp::BackdropColor() const
{
   return m_oBackdropColor;
}

void UtoTextShapeImp::SetBackdropRenderImp(unsigned long nImpType)
{
   // set the rendering implementation
   mTextNode->setBackdropImplementation((osgText::Text::BackdropImplementation)nImpType);
   // store the value
   m_nBackdropImpType = nImpType;
}

unsigned long UtoTextShapeImp::BackdropRenderImp() const
{
   return m_nBackdropImpType;
}
