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

#include "WkfAttachmentZoneCircle.hpp"

#include "UtMath.hpp"
#include "UtUnitTypes.hpp"
#include "UtoCircleShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaScenario.hpp"

namespace wkf
{
AttachmentZoneCircle::AttachmentZoneCircle(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : AttachmentZone(aParent, aViewerPtr, "circle_zone")
   , mOuterCirclePtrL(nullptr)
   , mOuterCircleShapeNameL()
   , mOuterCirclePtrH(nullptr)
   , mOuterCircleShapeNameH()
   , mInnerCirclePtrL(nullptr)
   , mInnerCircleShapeNameL()
   , mInnerCirclePtrH(nullptr)
   , mInnerCircleShapeNameH()
   , mCircleFillPtr(nullptr)
   , mCircleShapeNameF()
   , mRadius(100000.0)
   , mMinRadius(0.0)
   , mBegAngle(0.0)
   , mEndAngle(360.0)
   , mX(0.0)
   , mY(0.0)
   , mUseShader(false)
   , mShowTopLines(true)
{
   SetName(GetName() + "_" + GetUniqueIdString());
   SetType<AttachmentZoneCircle>();
}

AttachmentZoneCircle::~AttachmentZoneCircle() {}

void AttachmentZoneCircle::PrivateLoad()
{
   mUseShader = false;
   if (mOuterCircleShapeNameL.empty())
   {
      mOuterCircleShapeNameL = "AttachmentZoneCircleEdgeLo_" + GetUniqueIdString();
   }
   if (mOuterCircleShapeNameH.empty())
   {
      mOuterCircleShapeNameH = "AttachmentZoneCircleEdgeHi_" + GetUniqueIdString();
   }
   if (mInnerCircleShapeNameL.empty())
   {
      mInnerCircleShapeNameL = "AttachmentZoneCircleInnerEdgeLo_" + GetUniqueIdString();
   }
   if (mInnerCircleShapeNameH.empty())
   {
      mInnerCircleShapeNameH = "AttachmentZoneCircleInnerEdgeHi_" + GetUniqueIdString();
   }

   if (mCircleShapeNameF.empty())
   {
      mCircleShapeNameF = "AttachmentZoneCircleFace_" + GetUniqueIdString();
   }

   // Remove the previous models.
   RemoveShapeFromParent(mOuterCircleShapeNameL);
   RemoveShapeFromParent(mOuterCircleShapeNameH);
   RemoveShapeFromParent(mInnerCircleShapeNameL);
   RemoveShapeFromParent(mInnerCircleShapeNameH);
   RemoveShapeFromParent(mCircleShapeNameF);

   // Load the circles.
   UtoCircleShape circleOuterl;
   UtoCircleShape circleOuterh;
   UtoCircleShape circleInnerl;
   UtoCircleShape circleInnerh;
   UtoCircleShape circlef;

   mOuterCirclePtrL = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mOuterCircleShapeNameL, circleOuterl));
   mOuterCirclePtrH = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mOuterCircleShapeNameH, circleOuterh));
   mInnerCirclePtrL = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mInnerCircleShapeNameL, circleInnerl));
   mInnerCirclePtrH = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mInnerCircleShapeNameH, circleInnerh));
   mCircleFillPtr   = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mCircleShapeNameF, circlef));

   // Make sure the shape was added.
   if (mOuterCirclePtrL && mOuterCirclePtrH && mCircleFillPtr)
   {
      mOuterCirclePtrL->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mOuterCirclePtrH->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mCircleFillPtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);

      mOuterCirclePtrL->EnableLineMode(true);
      mOuterCirclePtrH->EnableLineMode(true);
      mOuterCirclePtrH->SetPattern(mShowTopLines ? 1 : 0);
      mOuterCirclePtrH->EnableSmoothSides(true);
      LoadState();
      SetUseShader(true);
   }
   if (mInnerCirclePtrL && mInnerCirclePtrH)
   {
      mInnerCirclePtrL->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mInnerCirclePtrH->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);

      mInnerCirclePtrL->EnableLineMode(true);
      mInnerCirclePtrH->EnableLineMode(true);
      mInnerCirclePtrH->EnableSmoothSides(true);
   }
}

void AttachmentZoneCircle::LoadState()
{
   SetRadius(mRadius);
   SetMinRadius(mMinRadius);
   SetOrientation(mHeading, mPitch, mRoll);
   SetBegAngle(mBegAngle);
   SetEndAngle(mEndAngle);
   SetX(mX);
   SetY(mY);
   SetBaseHeight(mBaseHeight);
   SetHeight(mHeight);
   SetColor(mColor);
   SetLineWidth(mLineWidth);
   SetLineStyle(mLineStyle);
   SetEnableFill(mEnableFill);
   mColorChanged     = false;
   mLineStyleChanged = false;
   mLineWidthChanged = false;
   mFillChanged      = false;
}

bool AttachmentZoneCircle::Pick(vespa::VaViewer*                aViewerPtr,
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

void AttachmentZoneCircle::SetRadius(const UtLength& aRadius)
{
   mRadius = aRadius;

   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetRadius(aRadius);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetRadius(aRadius);
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetRadius(aRadius);
      if (mUseShader)
      {
         mCircleFillPtr->SetUniformf("maxRange", mRadius);
      }
   }
}

void AttachmentZoneCircle::SetMinRadius(const UtLength& aMinRadius)
{
   if (aMinRadius >= mRadius)
   {
      mMinRadius = mRadius - 0.1;
   }
   else
   {
      mMinRadius = aMinRadius;
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetRadius(aMinRadius);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetRadius(aMinRadius);
   }
   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetMinRadius(aMinRadius);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetMinRadius(aMinRadius);
   }
   if (mCircleFillPtr)
   {
      if (mUseShader)
      {
         mCircleFillPtr->SetUniformf("minRange", mMinRadius);
         mCircleFillPtr->SetUsesShader(mMinRadius != 0);
      }
   }
}

void AttachmentZoneCircle::SetOrientation(double aHeading, double aPitch, double aRoll)
{
   mHeading = aHeading;
   mPitch   = aPitch;
   mRoll    = aRoll;

   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
}

void AttachmentZoneCircle::SetBegAngle(const UtAngleR& aBegAngle)
{
   mBegAngle = aBegAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneCircle::SetEndAngle(const UtAngleR& aEndAngle)
{
   mEndAngle = aEndAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneCircle::SetAngleBounds()
{
   // uto is 0 'south' and CCW
   // SEDIT was 0 'north' and CW
   // this corrects for the difference
   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetAngleBounds(mBegAngle, mEndAngle);
      // Disable inner circle visibility if not drawing full circle.
      if (!UtMath::NearlyEqual(mEndAngle - mBegAngle, 360.0))
      {
         mInnerCirclePtrL->SetPattern(0);
      }
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetAngleBounds(mBegAngle, mEndAngle);
      if (!UtMath::NearlyEqual(mEndAngle - mBegAngle, 360.0))
      {
         mInnerCirclePtrH->SetPattern(0);
      }
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetAngleBounds(mBegAngle, mEndAngle);
   }
}

void AttachmentZoneCircle::NormalizeAngles()
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

void AttachmentZoneCircle::SetX(const UtLength& aX)
{
   mX = aX;
   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetPosition(UtoPosition(mX, mY, 0.0));
   }
}

void AttachmentZoneCircle::SetY(const UtLength& aY)
{
   mY = aY;
   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetPosition(UtoPosition(mX, mY, 0.0));
   }
}

void AttachmentZoneCircle::SetBaseHeight(const UtLength& aBaseHeight)
{
   mBaseHeight = aBaseHeight;

   if (mOuterCirclePtrL && mOuterCirclePtrH)
   {
      UtoPosition p = mOuterCirclePtrL->Position();
      p.z()         = -aBaseHeight;
      mOuterCirclePtrL->SetPosition(p);
      p.z() -= mHeight;
      mOuterCirclePtrH->SetPosition(p);
   }
   if (mInnerCirclePtrL && mInnerCirclePtrH)
   {
      UtoPosition p = mInnerCirclePtrL->Position();
      p.z()         = -aBaseHeight;
      mInnerCirclePtrL->SetPosition(p);
      p.z() -= mHeight;
      mInnerCirclePtrH->SetPosition(p);
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetSides(UtoPosition(0, 0, -mHeight));
      UtoPosition p = mCircleFillPtr->Position();
      p.z()         = -aBaseHeight;
      mCircleFillPtr->SetPosition(p);
      SetFillColor(mFillColor);
   }
}

void AttachmentZoneCircle::SetHeight(const UtLength& aHeight)
{
   mHeight = aHeight;

   if (mOuterCirclePtrL && mOuterCirclePtrH)
   {
      UtoPosition p = mOuterCirclePtrL->Position();
      p.z() += -aHeight;
      mOuterCirclePtrH->SetPosition(p);
   }
   if (mInnerCirclePtrL && mInnerCirclePtrH)
   {
      UtoPosition p = mInnerCirclePtrL->Position();
      p.z() += -aHeight;
      mInnerCirclePtrH->SetPosition(p);
   }
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetSides(UtoPosition(0, 0, -mHeight));
      mCircleFillPtr->SetUniformf("zoneHeight", mHeight);
      SetFillColor(mFillColor);
   }
}

void AttachmentZoneCircle::SetColor(const UtColor& aColor)
{
   mColor        = aColor;
   mColorChanged = true;
   UtoColor color{static_cast<unsigned char>(mColor[0] * 255),
                  static_cast<unsigned char>(mColor[1] * 255),
                  static_cast<unsigned char>(mColor[2] * 255),
                  static_cast<unsigned char>(mColor[3] * 255)};

   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->DisableLighting();
      mOuterCirclePtrL->SetColor(color);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->DisableLighting();
      mOuterCirclePtrH->SetColor(color);
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->DisableLighting();
      mInnerCirclePtrL->SetColor(color);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->DisableLighting();
      mInnerCirclePtrH->SetColor(color);
   }
}

void AttachmentZoneCircle::SetFillColor(const UtColor& aFillColor)
{
   mFillColor        = aFillColor;
   mFillColorChanged = true;
   if (mCircleFillPtr)
   {
      float    alpha = ((mEnableFill || (mHeight != 0)) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));
      mCircleFillPtr->DisableLighting();
      mCircleFillPtr->SetColor(color);
   }
}

void AttachmentZoneCircle::SetBlendFactor(float aFactor)
{
   mBlendFactor = aFactor;
   if (mCircleFillPtr)
   {
      float    alpha = ((mEnableFill || (mHeight != 0)) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));
      mCircleFillPtr->DisableLighting();
      mCircleFillPtr->SetColor(color);
   }
}

void AttachmentZoneCircle::SetLineWidth(double aLineWidth)
{
   mLineWidth        = aLineWidth;
   mLineWidthChanged = true;
   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetLineWidth(aLineWidth);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetLineWidth(aLineWidth);
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetLineWidth(aLineWidth);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetLineWidth(aLineWidth);
   }
}

void AttachmentZoneCircle::SetLineStyle(int aLineStyle)
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

   if (mOuterCirclePtrL)
   {
      mOuterCirclePtrL->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
   if (mOuterCirclePtrH)
   {
      mOuterCirclePtrH->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPattern(UtMath::NearlyEqual(mEndAngle - mBegAngle, 360.0) ? mLineStipplePattern : 0,
                                   mLineStippleFactor);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPattern(UtMath::NearlyEqual(mEndAngle - mBegAngle, 360.0) ? mLineStipplePattern : 0,
                                   mLineStippleFactor);
   }
}

void AttachmentZoneCircle::SetEnableFill(bool aEnableFill)
{
   mEnableFill  = aEnableFill;
   mFillChanged = true;

   if (mCircleFillPtr)
   {
      if (mEnableFill)
      {
         mCircleFillPtr->EnableLineMode(false);
      }
      else
      {
         mCircleFillPtr->EnableLineMode(true);
      }
      mCircleFillPtr->DisableLighting();
   }
   SetFillColor(mFillColor);
}

double AttachmentZoneCircle::GetArea() const
{
   return UtMath::cPI * mRadius * mRadius; // * (mEndAngle - mBegAngle / UtMath::cTWO_PI); // holding off on doing this
                                           // to be consistent with the
                                           // ellipse
}

double AttachmentZoneCircle::GetCircumference() const
{
   return UtMath::cTWO_PI * mRadius; // * (mEndAngle - mBegAngle / UtMath::cTWO_PI);
}

void AttachmentZoneCircle::SetUseShader(bool aUseShader)
{
   if (mUseShader != aUseShader)
   {
      mUseShader = aUseShader;
      ShadersToggled();
   }
}

void AttachmentZoneCircle::ShadersToggled()
{
   if (mCircleFillPtr)
   {
      mCircleFillPtr->SetProgram(UtoShaders::UseProgram("zoneLimits.vert", "zoneLimits.frag"), mUseShader);
      mCircleFillPtr->SetUniformf("maxRange", mRadius);
      mCircleFillPtr->SetUniformf("minRange", mMinRadius);
      mCircleFillPtr->SetUsesShader(mMinRadius != 0);
   }
}

void AttachmentZoneCircle::SetShowTopLines(bool aShowTopLines)
{
   if (mShowTopLines != aShowTopLines)
   {
      if (mOuterCirclePtrH)
      {
         mOuterCirclePtrH->SetPattern(mShowTopLines ? 1 : 0);
      }
      if (mInnerCirclePtrH)
      {
         mInnerCirclePtrH->SetPattern(mShowTopLines ? 1 : 0);
      }
   }
}
} // namespace wkf
