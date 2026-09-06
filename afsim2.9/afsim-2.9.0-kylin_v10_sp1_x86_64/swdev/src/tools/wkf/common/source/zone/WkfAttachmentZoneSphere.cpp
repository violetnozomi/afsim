// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentZoneSphere.hpp"

#include "UtoShaders.hpp"
#include "UtoSphereShape.hpp"
#include "VaEntity.hpp"
#include "VaViewer.hpp"

namespace wkf
{

AttachmentZoneSphere::AttachmentZoneSphere(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : AttachmentZone(aParent, aViewerPtr, "ellipse_zone")
   , mSpherePtr(nullptr)
   , mSphereShapeName()
   , mBegAngle(0.0)
   , mEndAngle(360.0)
   , mRadius(0.0)
   , mUseShader(false)
   , mIsMaxAltSet(false)
   , mIsMinAltSet(false)
{
   SetName(GetName() + "_" + std::to_string(GetUniqueId()));
   SetType<AttachmentZoneSphere>();
}

void AttachmentZoneSphere::PrivateLoad()
{
   mUseShader = false;

   if (mSphereShapeName.empty())
   {
      mSphereShapeName = "AttachmentZoneSphere_" + GetUniqueIdString();
   }

   // Remove the previous model.
   RemoveShapeFromParent(mSphereShapeName);

   UtoSphereShape sphere;

   // Load the sphere.
   mSpherePtr = dynamic_cast<UtoSphereShape*>(AddShapeToParent(mSphereShapeName, sphere));

   // Make sure the ellipses was added.
   if (mSpherePtr)
   {
      mSpherePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      LoadState();
      SetUseShader(true);
   }
}

void AttachmentZoneSphere::LoadState()
{
   SetRadius(mRadius);
   SetMinRadius(mMinRadius);
   SetOrientation(mHeading, mPitch, mRoll);
   SetMaxAltitude(mMaxAlt);
   SetMinAltitude(mMinAlt);
   SetBegAngle(mBegAngle);
   SetEndAngle(mEndAngle);
   SetX(mX);
   SetY(mY);
   SetFillColor(mFillColor);
   SetColor(mColor);
   SetLineWidth(mLineWidth);
   SetLineStyle(mLineStyle);
   mColorChanged     = false;
   mLineStyleChanged = false;
   mLineWidthChanged = false;
   mFillChanged      = false;
}

bool AttachmentZoneSphere::Pick(vespa::VaViewer*                aViewerPtr,
                                int                             aMouseX,
                                int                             aMouseY,
                                bool                            aToggle,
                                bool                            aDoubleClick,
                                const vespa::VaAttachmentSubId& aAttachmentSubPartId)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return false;
   }
   bool selected = aToggle ? !GetSelected(aViewerPtr) : true;
   SetSelected(aViewerPtr, selected, aAttachmentSubPartId);
   return true;
}

void AttachmentZoneSphere::SetBegAngle(const UtAngleR& aBegAngle)
{
   mBegAngle = aBegAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneSphere::SetEndAngle(const UtAngleR& aEndAngle)
{
   mEndAngle = aEndAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneSphere::SetAngleBounds()
{
   if (mSpherePtr)
   {
      mSpherePtr->SetAngleBounds(mBegAngle, mEndAngle);
   }
}

void AttachmentZoneSphere::NormalizeAngles()
{
   mBegAngle = UtMath::NormalizeAngle0_360(mBegAngle);
   mEndAngle = UtMath::NormalizeAngle0_360(mEndAngle);
   if (mEndAngle == 0)
   {
      mEndAngle = 360.0;
   }
   if (mBegAngle >= mEndAngle)
   {
      mBegAngle = mBegAngle - 360.0;
   }
}

void AttachmentZoneSphere::SetX(const UtLength& aX)
{
   mX = aX;
   if (mSpherePtr)
   {
      mSpherePtr->SetPosition(UtoPosition(mX, mY, 0));
   }
}

void AttachmentZoneSphere::SetY(const UtLength& aY)
{
   mY = aY;
   if (mSpherePtr)
   {
      mSpherePtr->SetPosition(UtoPosition(mX, mY, 0));
   }
}

void AttachmentZoneSphere::SetMaxAltitude(const UtLength& aMaxAlt)
{
   if (mIsMaxAltSet && mSpherePtr)
   {
      mMaxAlt = aMaxAlt;
      mSpherePtr->SetMaxAltitude(-mMaxAlt + GetParent().GetPosition().GetAlt());
   }
}

void AttachmentZoneSphere::SetMinAltitude(const UtLength& aMinAlt)
{
   if (mIsMinAltSet && mSpherePtr)
   {
      mMinAlt = aMinAlt;
      mSpherePtr->SetMinAltitude(-mMinAlt + GetParent().GetPosition().GetAlt());
   }
}

void AttachmentZoneSphere::SetColor(const UtColor& aColor)
{
   mColor        = aColor;
   mColorChanged = true;
   if (mSpherePtr)
   {
      UtoColor color{static_cast<unsigned char>(mColor[0] * 255),
                     static_cast<unsigned char>(mColor[1] * 255),
                     static_cast<unsigned char>(mColor[2] * 255),
                     static_cast<unsigned char>(mColor[3] * 255)};

      mSpherePtr->EnableLighting(false);
      mSpherePtr->SetLineColor(color);
   }
}

void AttachmentZoneSphere::SetFillColor(const UtColor& aFillColor)
{
   mFillColor        = aFillColor;
   mFillColorChanged = true;
   if (mSpherePtr)
   {
      float    alpha = ((mEnableFill || !mIsMaxAltSet || mMaxAlt != 0) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));

      mSpherePtr->EnableLighting(false);
      mSpherePtr->SetColor(color);
   }
}

void AttachmentZoneSphere::SetBlendFactor(float aFactor)
{
   mBlendFactor = aFactor;
   if (mSpherePtr)
   {
      float    alpha = ((mEnableFill || (mMaxAlt != 0)) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));

      mSpherePtr->EnableLighting(false);
      mSpherePtr->SetColor(color);
   }
}

void AttachmentZoneSphere::SetLineWidth(double aLineWidth)
{
   mLineWidth        = aLineWidth;
   mLineWidthChanged = true;
   if (mSpherePtr)
   {
      mSpherePtr->SetLineWidth(aLineWidth);
   }
}

void AttachmentZoneSphere::SetLineStyle(int aLineStyle)
{
   mLineStyle        = aLineStyle;
   mLineStyleChanged = true;
   switch (mLineStyle)
   {
   case 0:
      mLineStipplePattern = 0xFFFF;
      break;
   case 1:
      mLineStipplePattern = 0xC0C0;
      break;
   case 2:
      mLineStipplePattern = 0xFF00;
      break;
   case 3:
      mLineStipplePattern = 0xF0F0;
      break;
   case 4:
      mLineStipplePattern = 0xFE38;
      break;
   default:
      mLineStipplePattern = 0xFFFF;
      break;
   }

   if (mSpherePtr)
   {
      mSpherePtr->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
}

void AttachmentZoneSphere::SetUseShader(bool aUseShader)
{
   if (mUseShader != aUseShader)
   {
      mUseShader = aUseShader;
      ShadersToggled();
   }
}

void AttachmentZoneSphere::ShadersToggled()
{
   if (mSpherePtr)
   {
      mSpherePtr->SetProgram(UtoShaders::UseProgram("zoneLimits.vert", "zoneLimits.frag"), mUseShader);
      mSpherePtr->SetUniformf("maxRange", mRadius);
      mSpherePtr->SetUniformf("minRange", 0);
   }
}

void AttachmentZoneSphere::SetRadius(const UtLength& aRadius)
{
   mRadius = aRadius;
   if (mSpherePtr != nullptr)
   {
      if (mUseShader)
         mSpherePtr->SetUniformf("maxRange", (float)(double)mRadius);
      mSpherePtr->SetRadius(aRadius);
   }
}

void AttachmentZoneSphere::SetMinRadius(const UtLength& aMinRadius)
{
   mMinRadius = aMinRadius;
   if (mSpherePtr != nullptr)
   {
      mSpherePtr->SetMinimumRadius(aMinRadius);
   }
}

void AttachmentZoneSphere::SetOrientation(double aHeading, double aPitch, double aRoll)
{
   mHeading = aHeading;
   mPitch   = aPitch;
   mRoll    = aRoll;

   if (mSpherePtr)
   {
      mSpherePtr->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
}

} // namespace wkf
