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

#include "zone/WkfAttachmentZoneEllipse.hpp"

#include "UtMath.hpp"
#include "UtUnitTypes.hpp"
#include "UtoCircleShape.hpp"
#include "UtoEllipseShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaScenario.hpp"

namespace wkf
{
AttachmentZoneEllipse::AttachmentZoneEllipse(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : AttachmentZone(aParent, aViewerPtr, "ellipse_zone")
   , mOuterEllipsePtrL(nullptr)
   , mOuterEllipseShapeNameL()
   , mOuterEllipsePtrH(nullptr)
   , mOuterEllipseShapeNameH()
   , mInnerCirclePtrL(nullptr)
   , mInnerCircleShapeNameL()
   , mInnerCirclePtrH(nullptr)
   , mInnerCircleShapeNameH()
   , mEllipsePtrF(nullptr)
   , mEllipseShapeNameF()
   , mLonAxis(161803.4)
   , // the golden ratio, for aesthetics of course!
   mLatAxis(100000.0)
   , mBegAngle(0.0)
   , mEndAngle(360.0)
   , mRadius(0.0)
   , mMinRadius(0.0)
   , mUseShader(false)
   , mShowTopLines(true)
{
   SetName(GetName() + "_" + std::to_string(GetUniqueId()));
   SetType<AttachmentZoneEllipse>();
}

AttachmentZoneEllipse::~AttachmentZoneEllipse() {}

void AttachmentZoneEllipse::PrivateLoad()
{
   mUseShader = false;

   if (mOuterEllipseShapeNameL.empty())
   {
      mOuterEllipseShapeNameL = "AttachmentZoneEllipseEdgeLo_" + GetUniqueIdString();
   }
   if (mOuterEllipseShapeNameH.empty())
   {
      mOuterEllipseShapeNameH = "AttachmentZoneEllipseEdgeHi_" + GetUniqueIdString();
   }
   if (mInnerCircleShapeNameL.empty())
   {
      mInnerCircleShapeNameL = "AttachmentZoneEllipseInnerEdgeLo_" + GetUniqueIdString();
   }
   if (mInnerCircleShapeNameH.empty())
   {
      mInnerCircleShapeNameH = "AttachmentZoneEllipseInnerEdgeHi_" + GetUniqueIdString();
   }
   if (mEllipseShapeNameF.empty())
   {
      mEllipseShapeNameF = "AttachmentZoneEllipseFace_" + GetUniqueIdString();
   }

   // Remove the previous model.
   RemoveShapeFromParent(mOuterEllipseShapeNameL);
   RemoveShapeFromParent(mOuterEllipseShapeNameH);
   RemoveShapeFromParent(mInnerCircleShapeNameL);
   RemoveShapeFromParent(mInnerCircleShapeNameH);
   RemoveShapeFromParent(mEllipseShapeNameF);

   UtoEllipseShape ellipsel;
   UtoEllipseShape ellipseh;
   UtoCircleShape  circleInnerl;
   UtoCircleShape  circleInnerh;
   UtoEllipseShape ellipsef;

   // Load the ellipse.
   mOuterEllipsePtrL = dynamic_cast<UtoEllipseShape*>(AddShapeToParent(mOuterEllipseShapeNameL, ellipsel));
   mOuterEllipsePtrH = dynamic_cast<UtoEllipseShape*>(AddShapeToParent(mOuterEllipseShapeNameH, ellipseh));
   mInnerCirclePtrL  = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mInnerCircleShapeNameL, circleInnerl));
   mInnerCirclePtrH  = dynamic_cast<UtoCircleShape*>(AddShapeToParent(mInnerCircleShapeNameH, circleInnerh));
   mEllipsePtrF      = dynamic_cast<UtoEllipseShape*>(AddShapeToParent(mEllipseShapeNameF, ellipsef));

   // Make sure the ellipses was added.
   if (mOuterEllipsePtrL && mOuterEllipsePtrH && mEllipsePtrF)
   {
      mOuterEllipsePtrL->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mOuterEllipsePtrH->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mEllipsePtrF->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);

      mOuterEllipsePtrL->EnableLineMode(true);
      mOuterEllipsePtrH->EnableLineMode(true);
      mOuterEllipsePtrH->SetPattern(mShowTopLines ? 1 : 0);
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

void AttachmentZoneEllipse::LoadState()
{
   SetLonAxisRadius(mLonAxis);
   SetLatAxisRadius(mLatAxis);
   SetMinRadius(mMinRadius);
   SetOrientation(mHeading, mPitch, mRoll);
   SetBaseHeight(mBaseHeight);
   SetHeight(mHeight);
   SetBegAngle(mBegAngle);
   SetEndAngle(mEndAngle);
   SetX(mX);
   SetY(mY);
   SetColor(mColor);
   SetLineWidth(mLineWidth);
   SetLineStyle(mLineStyle);
   SetEnableFill(mEnableFill);
   mColorChanged     = false;
   mLineStyleChanged = false;
   mLineWidthChanged = false;
   mFillChanged      = false;
}

bool AttachmentZoneEllipse::Pick(vespa::VaViewer*                aViewerPtr,
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

void AttachmentZoneEllipse::SetLonAxisRadius(const UtLength& aLonAxis)
{
   mLonAxis = aLonAxis;
   SetRadius(std::max(mLonAxis, mLatAxis));
   UpdateInnerCircle();
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetRadii(mLonAxis, mLatAxis);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetRadii(mLonAxis, mLatAxis);
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetRadii(mLonAxis, mLatAxis);
   }
}

void AttachmentZoneEllipse::SetLatAxisRadius(const UtLength& aLatAxis)
{
   mLatAxis = aLatAxis;
   SetRadius(std::max(mLonAxis, mLatAxis));
   UpdateInnerCircle();
   if (mOuterEllipsePtrL != nullptr)
   {
      mOuterEllipsePtrL->SetRadii(mLonAxis, mLatAxis);
   }
   if (mOuterEllipsePtrH != nullptr)
   {
      mOuterEllipsePtrH->SetRadii(mLonAxis, mLatAxis);
   }
   if (mEllipsePtrF != nullptr)
   {
      mEllipsePtrF->SetRadii(mLonAxis, mLatAxis);
   }
}

void AttachmentZoneEllipse::SetOrientation(double aHeading, double aPitch, double aRoll)
{
   mHeading = aHeading;
   mPitch   = aPitch;
   mRoll    = aRoll;

   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetOrientation(UtoOrientation(aHeading, aPitch, aRoll));
   }
}

void AttachmentZoneEllipse::SetBegAngle(const UtAngleR& aBegAngle)
{
   mBegAngle = aBegAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneEllipse::SetEndAngle(const UtAngleR& aEndAngle)
{
   mEndAngle = aEndAngle;
   NormalizeAngles();
   SetAngleBounds();
}

void AttachmentZoneEllipse::SetAngleBounds()
{
   // uto is 0 'south' and CCW
   // SEDIT was 0 'north' and CW
   // this corrects for the difference
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetAngleBounds(mBegAngle, mEndAngle);
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetAngleBounds(mBegAngle, mEndAngle);
   }
   UpdateInnerCircle();
}

void AttachmentZoneEllipse::UpdateInnerCircle()
{
   // Draw inner circles if full ellipse is drawn and inner circle fits within ellipse
   bool drawInner = UtMath::NearlyEqual(mEndAngle - mBegAngle, 360.0) && mMinRadius < std::min(mLatAxis, mLonAxis);
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPattern(drawInner ? mLineStipplePattern : 0, mLineStippleFactor);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPattern(drawInner ? mLineStipplePattern : 0, mLineStippleFactor);
   }
}

void AttachmentZoneEllipse::NormalizeAngles()
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

void AttachmentZoneEllipse::SetX(const UtLength& aX)
{
   mX = aX;
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetPosition(UtoPosition(mX, mY, -mBaseHeight));
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetPosition(UtoPosition(mX, mY, -mHeight - mBaseHeight));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPosition(UtoPosition(mX, mY, 0.0));
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetPosition(UtoPosition(mX, mY, -mBaseHeight));
   }
}

void AttachmentZoneEllipse::SetY(const UtLength& aY)
{
   mY = aY;
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetPosition(UtoPosition(mX, mY, -mBaseHeight));
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetPosition(UtoPosition(mX, mY, -mHeight - mBaseHeight));
   }
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetPosition(UtoPosition(mX, mY, -mHeight - mBaseHeight));
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetPosition(UtoPosition(mX, mY, -mHeight - mBaseHeight));
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetPosition(UtoPosition(mX, mY, -mBaseHeight));
   }
}

void AttachmentZoneEllipse::SetBaseHeight(const UtLength& aBaseHeight)
{
   mBaseHeight = aBaseHeight;

   if (mOuterEllipsePtrL && mOuterEllipsePtrH)
   {
      UtoPosition p = mOuterEllipsePtrL->Position();
      p.z()         = -aBaseHeight;
      mOuterEllipsePtrL->SetPosition(p);
      p.z() -= mHeight;
      mOuterEllipsePtrH->SetPosition(p);
   }
   if (mInnerCirclePtrL && mInnerCirclePtrH)
   {
      UtoPosition p = mInnerCirclePtrL->Position();
      p.z()         = -aBaseHeight;
      mInnerCirclePtrL->SetPosition(p);
      p.z() -= mHeight;
      mInnerCirclePtrH->SetPosition(p);
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetSides(UtoPosition(0, 0, -mHeight));
      UtoPosition p = mEllipsePtrF->Position();
      p.z()         = -aBaseHeight;
      mEllipsePtrF->SetPosition(p);
      SetFillColor(mFillColor);
   }
}

void AttachmentZoneEllipse::SetHeight(const UtLength& aHeight)
{
   mHeight = aHeight;

   if (mOuterEllipsePtrL && mOuterEllipsePtrH)
   {
      UtoPosition p = mOuterEllipsePtrL->Position();
      p.z() -= aHeight;
      mOuterEllipsePtrH->SetPosition(p);
   }
   if (mInnerCirclePtrL && mInnerCirclePtrH)
   {
      UtoPosition p = mInnerCirclePtrL->Position();
      p.z() += -aHeight;
      mInnerCirclePtrH->SetPosition(p);
   }
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetSides(UtoPosition(0, 0, -mHeight));
      SetFillColor(mFillColor);
   }
}

void AttachmentZoneEllipse::SetColor(const UtColor& aColor)
{
   mColor        = aColor;
   mColorChanged = true;
   UtoColor color{static_cast<unsigned char>(mColor[0] * 255),
                  static_cast<unsigned char>(mColor[1] * 255),
                  static_cast<unsigned char>(mColor[2] * 255),
                  static_cast<unsigned char>(mColor[3] * 255)};

   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->EnableLighting(false);
      mOuterEllipsePtrL->SetColor(color);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->EnableLighting(false);
      mOuterEllipsePtrH->SetColor(color);
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

void AttachmentZoneEllipse::SetFillColor(const UtColor& aFillColor)
{
   mFillColor        = aFillColor;
   mFillColorChanged = true;
   if (mEllipsePtrF)
   {
      float    alpha = ((mEnableFill || (mHeight != 0)) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));

      mEllipsePtrF->EnableLighting(false);
      mEllipsePtrF->SetColor(color);
   }
}

void AttachmentZoneEllipse::SetBlendFactor(float aFactor)
{
   mBlendFactor = aFactor;
   if (mEllipsePtrF)
   {
      float    alpha = ((mEnableFill || (mHeight != 0)) ? 1.0 - mBlendFactor : 0.0);
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));

      mEllipsePtrF->EnableLighting(false);
      mEllipsePtrF->SetColor(color);
   }
}

void AttachmentZoneEllipse::SetLineWidth(double aLineWidth)
{
   mLineWidth        = aLineWidth;
   mLineWidthChanged = true;
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetLineWidth(aLineWidth);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetLineWidth(aLineWidth);
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

void AttachmentZoneEllipse::SetLineStyle(int aLineStyle)
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

   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
   UpdateInnerCircle();
}

void AttachmentZoneEllipse::SetEnableFill(bool aEnableFill)
{
   mEnableFill  = aEnableFill;
   mFillChanged = true;

   if (mEllipsePtrF != nullptr)
   {
      if (mEnableFill)
      {
         mEllipsePtrF->EnableLineMode(false);
      }
      else
      {
         mEllipsePtrF->EnableLineMode(true);
      }
   }
   SetFillColor(mFillColor);
}

double AttachmentZoneEllipse::GetArea() const
{
   return UtMath::cPI * mLatAxis * mLonAxis;
}

double AttachmentZoneEllipse::GetCircumference() const
{
   // This is an approximation
   return UtMath::cPI *
          sqrt(2.0 * (mLatAxis * mLatAxis + mLonAxis * mLonAxis) - (mLonAxis - mLatAxis) * (mLonAxis - mLatAxis) / 2.0);
}

void AttachmentZoneEllipse::SetUseShader(bool aUseShader)
{
   if (mUseShader != aUseShader)
   {
      mUseShader = aUseShader;
      ShadersToggled();
   }
}

void AttachmentZoneEllipse::ShadersToggled()
{
   if (mEllipsePtrF)
   {
      mEllipsePtrF->SetProgram(UtoShaders::UseProgram("zoneLimits.vert", "zoneLimits.frag"), mUseShader);
      mEllipsePtrF->SetUniformf("maxRange", mRadius);
      mEllipsePtrF->SetUniformf("minRange", mMinRadius);
      mEllipsePtrF->SetUsesShader(mMinRadius != 0);
   }
}

void AttachmentZoneEllipse::SetRadius(const UtLength& aRadius)
{
   mRadius = aRadius;
   if (mEllipsePtrF != nullptr)
   {
      if (mUseShader)
         mEllipsePtrF->SetUniformf("maxRange", (float)(double)mRadius);
   }
}

void AttachmentZoneEllipse::SetMinRadius(const UtLength& aMinRadius)
{
   mMinRadius = aMinRadius;
   if (mInnerCirclePtrL)
   {
      mInnerCirclePtrL->SetRadius(mMinRadius);
   }
   if (mInnerCirclePtrH)
   {
      mInnerCirclePtrH->SetRadius(mMinRadius);
   }
   if (mOuterEllipsePtrL)
   {
      mOuterEllipsePtrL->SetMinRadius(mMinRadius);
   }
   if (mOuterEllipsePtrH)
   {
      mOuterEllipsePtrH->SetMinRadius(mMinRadius);
   }
   if (mEllipsePtrF)
   {
      if (mUseShader)
      {
         mEllipsePtrF->SetUniformf("minRange", static_cast<float>(mMinRadius));
         mEllipsePtrF->SetUsesShader(mMinRadius != 0);
      }
   }
   UpdateInnerCircle();
}

void AttachmentZoneEllipse::SetShowTopLines(bool aShowTopLines)
{
   if (mShowTopLines != aShowTopLines)
   {
      if (mOuterEllipsePtrH)
      {
         mOuterEllipsePtrH->SetPattern(mShowTopLines ? 1 : 0);
      }
      if (mInnerCirclePtrH)
      {
         mInnerCirclePtrH->SetPattern(mShowTopLines ? 1 : 0);
      }
   }
}

} // namespace wkf
