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

#include "VaAttachmentEllipsoid.hpp"

#include <sstream>

#include "UtoSphereShape.hpp"
#include "VaUtils.hpp"

namespace vespa
{
VaAttachmentEllipsoid::VaAttachmentEllipsoid(VaEntity& aParent, VaViewer* aViewerPtr)
   : VaAttachment(aParent, aViewerPtr, "ellipsoid")
   , mEllipsoidPtr(nullptr)
   , mEllipsoidShapeName()
   , mAxisX(161803.4)
   , // the golden ratio, for aesthetics of course!
   mAxisY(100000.0)
   , mAxisZ(100000.0)
   , mYaw(0.0)
   , mPitch(0.0)
   , mRoll(0.0)
   , mColor()
   , mLineWidth(2.0)
   , mLineStyle(0)
   , mLineStippleFactor(1)
   , mLineStipplePattern(0xffff)
   , mEnableFill(false)
   , mShowSlices(true)
   , mShowStacks(true)
{
   std::ostringstream oss;
   oss << GetName() << '_' << GetUniqueId();
   SetName(oss.str().c_str());
   SetType<VaAttachmentEllipsoid>();
}

VaAttachmentEllipsoid::~VaAttachmentEllipsoid() {}

void VaAttachmentEllipsoid::PrivateLoad()
{
   static unsigned int uid = 0;

   if (mEllipsoidShapeName == "")
   {
      std::ostringstream oss;
      oss << "VaAttachmentEllipsoid_" << uid++;
      mEllipsoidShapeName = oss.str().c_str();
   }

   // Remove the previous model.
   RemoveShapeFromParent(mEllipsoidShapeName);
   mEllipsoidPtr = nullptr;

   // Load the ellipsoid.
   UtoSphereShape ellipsoid;
   mEllipsoidPtr = dynamic_cast<UtoSphereShape*>(AddShapeToParent(mEllipsoidShapeName, ellipsoid));

   // Make sure the ellipsoid was added.
   if (mEllipsoidPtr != nullptr)
   {
      LoadState();
   }
}

void VaAttachmentEllipsoid::LoadState()
{
   SetAxisX(mAxisX);
   SetAxisY(mAxisY);
   SetAxisZ(mAxisZ);
   SetOrientation(mYaw, mPitch, mRoll);
   SetColor(mColor);
   SetLineWidth(mLineWidth);
   SetLineStyle(mLineStyle);
   SetEnableFill(mEnableFill);
   ShowSliceLines(mShowSlices);
   ShowStackLines(mShowStacks);
}

void VaAttachmentEllipsoid::SetAxes(const UtLength& aAxisX, const UtLength& aAxisY, const UtLength& aAxisZ)
{
   SetAxisX(aAxisX);
   SetAxisY(aAxisY);
   SetAxisZ(aAxisZ);
}

void VaAttachmentEllipsoid::SetAxisX(const UtLength& aAxisX)
{
   mAxisX = aAxisX;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetCompressionFactorX(mAxisX);
   }
}

void VaAttachmentEllipsoid::SetAxisY(const UtLength& aAxisY)
{
   mAxisY = aAxisY;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetCompressionFactorY(mAxisY);
   }
}

void VaAttachmentEllipsoid::SetAxisZ(const UtLength& aAxisZ)
{
   mAxisZ = aAxisZ;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetCompressionFactorZ(mAxisZ);
   }
}

void VaAttachmentEllipsoid::SetOrientation(const UtAngleR& aYaw, const UtAngleR& aPitch, const UtAngleR& aRoll)
{
   mYaw   = aYaw;
   mPitch = aPitch;
   mRoll  = aRoll;

   if (mEllipsoidPtr != nullptr)
   {
      // Orientation in UTO is not based on NED coordinate frame
      mEllipsoidPtr->SetOrientation(UtoOrientation(mYaw, -mRoll, mPitch));
   }
}

void VaAttachmentEllipsoid::SetColor(const UtColor& aColor)
{
   mColor = aColor;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetColor(VaUtils::glToUtoColor(mColor));
   }
}

void VaAttachmentEllipsoid::SetLineWidth(double aLineWidth)
{
   mLineWidth = aLineWidth;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetLineWidth(aLineWidth);
   }
}

void VaAttachmentEllipsoid::SetLineStyle(int aLineStyle)
{
   mLineStyle = aLineStyle;

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

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
}

void VaAttachmentEllipsoid::SetEnableFill(bool aEnableFill)
{
   mEnableFill = aEnableFill;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->ShowSphereGeometry(aEnableFill);
   }
}

void VaAttachmentEllipsoid::ShowSliceLines(bool aShow)
{
   mShowSlices = aShow;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->ShowSliceLines(aShow);
   }
}

void VaAttachmentEllipsoid::ShowStackLines(bool aShow)
{
   mShowStacks = aShow;

   if (mEllipsoidPtr != nullptr)
   {
      mEllipsoidPtr->ShowStackLines(aShow);
   }
}

double VaAttachmentEllipsoid::GetVolume() const
{
   return UtMath::cFOUR_PI / 3.0 * mAxisX * mAxisY * mAxisZ;
}
} // namespace vespa
