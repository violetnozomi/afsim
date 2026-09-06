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

#include "VaOverlayZoomBox.hpp"

#include "UtLog.hpp"
#include "Uto.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{
VaOverlayZoomBox::VaOverlayZoomBox()
   : VaOverlay("zoom_box")
   , mZoomBoxPtr(nullptr)
   , mMouseX(0)
   , mMouseY(0)
   , mSemiWidth(30)
   , mSemiHeight(20)
   , mZoomDirection(1)
   , mZoomAnimationState(0)
{
   SetType<VaOverlayZoomBox>();
}

VaOverlayZoomBox::VaOverlayZoomBox(const std::string& aName)
   : VaOverlay(aName)
   , mZoomBoxPtr(nullptr)
   , mMouseX(0)
   , mMouseY(0)
   , mSemiWidth(30)
   , mSemiHeight(20)
   , mZoomDirection(1)
   , mZoomAnimationState(0)
{
   SetType<VaOverlayZoomBox>();
}

VaOverlayZoomBox::VaOverlayZoomBox(const VaOverlayZoomBox& aSrc)
   : VaOverlay(aSrc)
   , mZoomBoxPtr(nullptr)
   , mMouseX(aSrc.mMouseX)
   , mMouseY(aSrc.mMouseY)
   , mSemiWidth(aSrc.mSemiWidth)
   , mSemiHeight(aSrc.mSemiHeight)
   , mZoomDirection(aSrc.mZoomDirection)
   , mZoomAnimationState(0)
{
   SetType<VaOverlayZoomBox>();
}

VaOverlayZoomBox::~VaOverlayZoomBox()
{
   mCallbacks.Clear();
   UnbindShape("zoom_box_shape");
}

VaOverlayZoomBox* VaOverlayZoomBox::Clone() const
{
   return new VaOverlayZoomBox(*this);
}

bool VaOverlayZoomBox::Initialize()
{
   bool initialized = false;

   UtoPolyLineShape zoomBox;
   zoomBox.SetColor(UtoColor(240, 50, 0));
   zoomBox.SetAggregation(UtoPolyLineShape::LINES);
   zoomBox.SetWidth(2.0);
   // zoomBox.EnableAntiAliasLines(true);
   mZoomBoxPtr = dynamic_cast<UtoPolyLineShape*>(BindShape("zoom_box_shape", zoomBox));
   if (mZoomBoxPtr != nullptr)
   {
      initialized = true;
      SetVisible(false);
      mCallbacks.Add(VaObserver::UpdateCursorPosition.Connect(&VaOverlayZoomBox::UpdateCursorPositionCB,
                                                              this,
                                                              GetViewer()->GetUniqueId()));

      unsigned int dummy;
      GetViewer()->WorldToScreen(VaEnvironment::Instance().GetCurrentCursorPosition(),
                                 (unsigned&)mMouseX,
                                 (unsigned&)mMouseY,
                                 dummy);
   }
   else
   {
      ut::log::warning() << "Could not bind shape: zoom_box";
   }

   return initialized;
}

void VaOverlayZoomBox::Update()
{
   if (IsVisible())
   {
      UpdateAnimation();

      mZoomBoxPtr->Clear();
      int zoomf = 5 * (1 - mZoomDirection); // IN = 0, OUT = 10
      // Draw upper right corner of box
      mZoomBoxPtr->Insert(0, UtoPosition(mMouseX + mSemiWidth, mMouseY + mSemiHeight - 10, 0));
      mZoomBoxPtr->Insert(1, UtoPosition(mMouseX + mSemiWidth - zoomf, mMouseY + mSemiHeight - zoomf, 0));
      mZoomBoxPtr->Insert(2, mZoomBoxPtr->At(1));
      mZoomBoxPtr->Insert(3, UtoPosition(mMouseX + mSemiWidth - 10, mMouseY + mSemiHeight, 0));

      // Draw upper left corner of box
      mZoomBoxPtr->Insert(4, UtoPosition(mMouseX - mSemiWidth, mMouseY + mSemiHeight - 10, 0));
      mZoomBoxPtr->Insert(5, UtoPosition(mMouseX - mSemiWidth + zoomf, mMouseY + mSemiHeight - zoomf, 0));
      mZoomBoxPtr->Insert(6, mZoomBoxPtr->At(5));
      mZoomBoxPtr->Insert(7, UtoPosition(mMouseX - mSemiWidth + 10, mMouseY + mSemiHeight, 0));

      // Draw lower left corner of box
      mZoomBoxPtr->Insert(8, UtoPosition(mMouseX - mSemiWidth, mMouseY - mSemiHeight + 10, 0));
      mZoomBoxPtr->Insert(9, UtoPosition(mMouseX - mSemiWidth + zoomf, mMouseY - mSemiHeight + zoomf, 0));
      mZoomBoxPtr->Insert(10, mZoomBoxPtr->At(9));
      mZoomBoxPtr->Insert(11, UtoPosition(mMouseX - mSemiWidth + 10, mMouseY - mSemiHeight, 0));

      // Draw lower right corner of box
      mZoomBoxPtr->Insert(12, UtoPosition(mMouseX + mSemiWidth, mMouseY - mSemiHeight + 10, 0));
      mZoomBoxPtr->Insert(13, UtoPosition(mMouseX + mSemiWidth - zoomf, mMouseY - mSemiHeight + zoomf, 0));
      mZoomBoxPtr->Insert(14, mZoomBoxPtr->At(13));
      mZoomBoxPtr->Insert(15, UtoPosition(mMouseX + mSemiWidth - 10, mMouseY - mSemiHeight, 0));
   }
}

void VaOverlayZoomBox::SetZoomDirection(ZoomDirection aZoomDir)
{
   mZoomDirection = aZoomDir;
   // Reset the animation state
   mZoomAnimationState = 0;
}

void VaOverlayZoomBox::UpdateAnimation()
{
   SetBoxSize(90 + 30 * mZoomDirection * (mZoomAnimationState / 4 - 1),
              60 + 20 * mZoomDirection * (mZoomAnimationState / 4 - 1));

   if (mZoomAnimationState++ >= 12)
   {
      mZoomAnimationState = 0; // reset the animation state
      SetVisible(false);
   }
}

void VaOverlayZoomBox::SetBoxSize(int aWidth, int aHeight)
{
   mSemiWidth  = aWidth / 2;
   mSemiHeight = aHeight / 2;
}

void VaOverlayZoomBox::UpdateCursorPositionCB(VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   mMouseX = aMouseX;
   mMouseY = aMouseY;
}
} // end namespace vespa
