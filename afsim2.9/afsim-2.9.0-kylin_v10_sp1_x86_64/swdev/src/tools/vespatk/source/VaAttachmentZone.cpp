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

#include "VaAttachmentZone.hpp"

#include "VaEntity.hpp"
#include "VaSelectionBox.hpp"

namespace vespa
{
VaAttachmentZone::VaAttachmentZone(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachmentZoneBase(aParent, aViewerPtr, aName)
   , mColorChanged(false)
   , mFillColorChanged(false)
   , mLineWidthChanged(false)
   , mLineStyleChanged(false)
   , mFillChanged(false)
   , mClosedChanged(false)
   , mColor()
   , mFillColor()
   , mBlendFactor(1.0)
   , mLineWidth(2.0)
   , mLineStyle(0)
   , mLineStippleFactor(1)
   , mLineStipplePattern(0xffff)
   , mEnableFill(false)
   , mBaseHeight(0.0)
   , mHeight(0.0)
   , mMaxAlt(0.0)
   , mMinAlt(0.0)
   , mHeading(0.0)
   , mPitch(0.0)
   , mRoll(0.0)
   , mShapeName("")
   , mSlewAdjustment(ReverseAspect)
   , mSlewEntityPtr(nullptr)
   , mSlewEntityName("")
{
   SetType<VaAttachmentZone>();
}

VaAttachmentZone::~VaAttachmentZone() {}

bool VaAttachmentZone::BoxPick(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return false;
   }
   if (aBox.Contains(GetParent().GetPosition().GetLat(),
                     GetParent().GetPosition().GetLon(),
                     GetParent().GetPosition().GetAlt()))
   {
      if (aInside)
      {
         SetSelected(aViewerPtr, true, 0);
         return true;
      }
   }
   else
   {
      if (!aInside)
      {
         SetSelected(aViewerPtr, true, 0);
         return true;
      }
   }
   return false;
}

void VaAttachmentZone::PreApplyOptions(bool aForceUpdate)
{
   bool displayZones = true;
   bool displayIcon  = true;

   displayZones = displayZones && displayIcon;

   bool update = (displayZones != IsVisible());
   if ((update) || (aForceUpdate))
   {
      SetStateVisibility(displayZones);
   }
}

} // namespace vespa
