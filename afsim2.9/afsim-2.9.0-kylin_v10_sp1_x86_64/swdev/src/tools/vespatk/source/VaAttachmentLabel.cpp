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

#include "VaAttachmentLabel.hpp"

#include <assert.h>

#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaOverlayLabels.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
//! Method: VaAttachmentLabel
//! Description: VaAttachmentLabel constructor.  VaAttachmentLabel is an attachment that will display text when attached to a
//!              VaEntity.
//!
//! @param aParent - This parameter is the parent entity.  This should always be a valid entity.

// protected
VaAttachmentLabel::VaAttachmentLabel(VaEntity& aParent, VaViewer* aViewerPtr)
   : VaAttachment(aParent, aViewerPtr, "label")
   , mOverlayMap()
   , mLabelIndexMap()
   , mVisible(true)
   , mString()
   , mAlignment(0)
   , mSize(8)
{
   SetType<VaAttachmentLabel>();
   mColor[0] = 0.f;
   mColor[1] = 0.f;
   mColor[2] = 0.f;

   mLabelOffset[0] = 0;
   mLabelOffset[1] = 0;
}

//! Method: ~VaAttachmentLabel
//! Description: VaAttachmentLabel destructor.
//!

// public
VaAttachmentLabel::~VaAttachmentLabel()
{
   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         it->second->DeleteLabel(mLabelIndexMap[it->first]);
      }
   }
}

void VaAttachmentLabel::UpdateFrame(double aTime)
{
   int xy[2];

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (IsVisible())
      {
         it->second->ShowLabel(mLabelIndexMap[it->first], GetViewer()->CheckSphereOcclusion(GetParent().GetPosition()));
      }
      GetParent().GetScreenSpaceCoords(xy[0], xy[1], it->second->GetViewer());
      SetPosition(it->second->GetViewer(), xy);
   }
}

//! This method will load the visual label into the scene.
//!
//! @return void
// virtual private
void VaAttachmentLabel::PrivateLoad()
{
   if (mViewerPtr == nullptr)
   {
      VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
      {
         PrivateLoadForViewer(*it);
      }
   }
   else
   {
      PrivateLoadForViewer(mViewerPtr);
   }
}

void VaAttachmentLabel::PrivateLoadForViewer(VaViewer* aViewerPtr)
{
   int vid = aViewerPtr->GetUniqueId();
   // Find the labels overlay.
   std::map<int, VaOverlayLabels*>::iterator it = mOverlayMap.find(vid);

   if (it == mOverlayMap.end())
   {
      VaOverlayLabels overlay;
      mOverlayMap[vid] = dynamic_cast<VaOverlayLabels*>(aViewerPtr->AddOverlay(overlay));
   }

   if (nullptr == mOverlayMap[vid])
   {
      return; // this is a problem that seems to be arising from the use of multiple viewports.
              // we'll have to figure our how and why we get here when I have time to address
              // that feature.
   }

   if (mLabelIndexMap.find(vid) == mLabelIndexMap.end())
   {
      mLabelIndexMap[vid] = mOverlayMap[vid]->CreateLabel();
   }
   else
   {
      VaOverlayLabels::Label* labelPtr = mOverlayMap[vid]->FindLabel(mLabelIndexMap[vid]);
      if (labelPtr == nullptr)
      {
         mLabelIndexMap[vid] = mOverlayMap[vid]->CreateLabel();
      }
   }
   LoadState();
}

void VaAttachmentLabel::PrivateInitializeCallbacks()
{
   mCallbacks.Add(VaObserver::CameraUpdated.Connect(&VaAttachmentLabel::CameraUpdatedCB, this));
}

void VaAttachmentLabel::UpdateVisibility()
{
   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      it->second->ShowLabel(mLabelIndexMap[it->first], IsVisible());
   }
}

//! Method: SetColor
//! Description: Sets the color applied to the label text
//!
//! @param aColor - The color to apply to the label.

// public
void VaAttachmentLabel::SetColor(const UtColor& aColor)
{
   mColor = aColor;

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetColor(VaUtils::glToUtoColor(mColor));
      }
   }
}

//! Method: GetColor
//! Description: Gets the color applied to the label text
//!
//! @return The color that is currently applied to the label.

// public
const UtColor& VaAttachmentLabel::GetColor() const
{
   return mColor;
}

//! Method: SetString
//! Description: Sets the text string to show in the label.
//!
//! @param aInput The string.

// public
void VaAttachmentLabel::SetString(const std::string& aString)
{
   mString = aString;

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetString(mString);
      }
   }
}

//! Method: SetAlignment
//! Description: Sets the alignment of the label.
//!
//! @param aAlignment The alignment, see uto/UtoTextShape.hpp for the enumerations.

// public
void VaAttachmentLabel::SetAlignment(const int aAlignment)
{
   mAlignment = aAlignment;

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetAlignment(mAlignment);
      }
   }
}

//! Method: SetPosition
//! Description: Sets the position, in screen space (pixels) of the label
//!
//! @param aPosition[] The two component screen position (in pixels)

// public
void VaAttachmentLabel::SetPosition(VaViewer* aViewerPtr, const int aPosition[])
{
   int vid                   = aViewerPtr->GetUniqueId();
   mLabelPosition[vid].xy[0] = aPosition[0];
   mLabelPosition[vid].xy[1] = aPosition[1];

   std::map<int, VaOverlayLabels*>::iterator it = mOverlayMap.find(vid);
   if (it != mOverlayMap.end())
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetPosition(
            UtoPosition(mLabelPosition[vid].xy[0] + mLabelOffset[0], mLabelPosition[vid].xy[1] + mLabelOffset[1], 0));
      }
   }
}

const int* VaAttachmentLabel::GetPosition(VaViewer* aViewerPtr)
{
   return mLabelPosition[aViewerPtr->GetUniqueId()].xy;
}

//! Method: SetFont
//! Description: Sets the font of the label
//! @param aFace The new font style name.
//! @param aBold A flag to set the font style to bold.
//! @param aItalic A flag to set the font style to italic.

// public
void VaAttachmentLabel::SetFont(const std::string& aFace)
{
   mFace = aFace;
   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetFont(mFace);
      }
   }
}

std::string VaAttachmentLabel::Face() const
{
   return mFace;
}

//! Method: SetOffset
//! Description: This is called to change the screen space offset of the label.
//!
//! @param aX Offset on the horizontal axis measured in pixels.
//! @param aY Offset on the vertical axis measured in pixels.
// public
void VaAttachmentLabel::SetOffset(const int aX, const int aY)
{
   mLabelOffset[0] = aX;
   mLabelOffset[1] = aY;

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetPosition(UtoPosition(mLabelPosition[it->first].xy[0] + mLabelOffset[0],
                                                      mLabelPosition[it->first].xy[1] + mLabelOffset[1],
                                                      0));
      }
   }
}

//! SetSize (vespa::VaAttachmentLabel::SetSize)
//! @param int aSize Size of the text
//! @return void
// public
void VaAttachmentLabel::SetSize(int aSize)
{
   mSize = aSize;

   std::map<int, VaOverlayLabels*>::iterator it;
   for (it = mOverlayMap.begin(); it != mOverlayMap.end(); ++it)
   {
      if (it->second == nullptr)
      {
         return;
      }

      VaOverlayLabels::Label* labelPtr = it->second->FindLabel(mLabelIndexMap[it->first]);
      if (labelPtr != nullptr)
      {
         labelPtr->mShapePtr->SetSize(aSize);
      }
   }
}

//! Method: LoadState
//! Description: This method will set all state values and apply them to the label.
//!

// private
void VaAttachmentLabel::LoadState()
{
   SetColor(mColor);
   SetString(mString);
   SetAlignment(mAlignment);

   VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
   for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
   {
      if (mLabelPosition.find((*it)->GetUniqueId()) != mLabelPosition.end())
      {
         SetPosition(*it, mLabelPosition[(*it)->GetUniqueId()].xy);
      }
   }
   SetSize(mSize);
}

void VaAttachmentLabel::CameraUpdatedCB(VaViewer* aViewerPtr)
{
   UpdateFrame(0);
}

} // end namespace vespa.
