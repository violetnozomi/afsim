// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfHDD_PageHorizon.hpp"

HDD::PageHorizon::PageHorizon(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayHorizonPtr = new OverlayHorizon(("hdd_horizon" + std::to_string(mId)).c_str());
   aViewerPtr->AddOverlay(mOverlayHorizonPtr);
   SetPosition(aPosition);
   mOverlayHorizonPtr->SetVisible(true);
   mOverlayHorizonPtr->SetButton1ClickAction(
      [this](int x, int y, int s) -> bool
      {
         emit PageClicked(this);
         return false;
      });

   mPageName = "EFI";
}

const bool HDD::PageHorizon::IsVisible() const
{
   return mOverlayHorizonPtr->IsVisible();
}

const std::string& HDD::PageHorizon::OverlayName() const
{
   return mOverlayHorizonPtr->GetName();
}

void HDD::PageHorizon::BringOverlayToFront()
{
   mViewerPtr->BringOverlayToFront(mOverlayHorizonPtr);
}

void HDD::PageHorizon::RemoveOverlay()
{
   mViewerPtr->RemoveOverlay(mOverlayHorizonPtr->GetName());
}

void HDD::PageHorizon::SetOwnshipPitchAndRoll_deg(float aPitch_deg, float aRoll_deg)
{
   mOverlayHorizonPtr->SetOwnshipPitchAndRoll_deg(aPitch_deg, aRoll_deg);
}

void HDD::PageHorizon::SetPosition(const size_t aPosition)
{
   mPosition = aPosition;
   GetRect(mPosition, mRect);
   SetPageSize(mPosition);
   mOverlayHorizonPtr->SetPosition(mRect.x() * mViewerPtr->GetWidth(), mRect.y() * mViewerPtr->GetHeight());
   mOverlayHorizonPtr->SetSize(mRect.width() * mViewerPtr->GetWidth(), mRect.height() * mViewerPtr->GetHeight());
}

void HDD::PageHorizon::SetVisible(bool aState)
{
   mOverlayHorizonPtr->SetVisible(aState);
}
