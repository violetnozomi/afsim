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

#include "hdd/WkfHDD_Button.hpp"

#include "VaViewer.hpp"
#include "hdd/WkfHDD_Overlay.hpp"

HDD::Button::Button(const std::string& aOverlayName, vespa::VaViewer* aViewerPtr, const QRectF& aRect)
   : mViewerPtr(aViewerPtr)
{
   mOverlayPtr = new Overlay(aOverlayName.c_str(), mViewerPtr->GetWidth(), mViewerPtr->GetHeight());
   mViewerPtr->AddOverlay(mOverlayPtr);
   mOverlayPtr->SetVisible(true);

   // Set the position and size of the overlay.
   mOverlayPtr->SetPosition(aRect.x(), aRect.y());
   mOverlayPtr->SetSize(aRect.width(), aRect.height());
}

void HDD::Button::BringOverlayToFront()
{
   if (mOverlayPtr && mOverlayPtr->IsVisible())
   {
      mViewerPtr->BringOverlayToFront(mOverlayPtr);
   }
}

void HDD::Button::RemoveOverlay()
{
   if (mViewerPtr && mOverlayPtr)
   {
      mViewerPtr->RemoveOverlay(mOverlayPtr->GetName());
   }
}

const bool HDD::Button::IsVisible() const
{
   return mOverlayPtr->IsVisible();
}

void HDD::Button::SetDrawFunction(const std::function<void()>& aDrawFunction)
{
   mOverlayPtr->SetDrawFunction(aDrawFunction);
}

void HDD::Button::SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction)
{
   mOverlayPtr->SetButton1ClickAction(aFunction);
}

void HDD::Button::SetPosition(const QRectF& aRect)
{
   mOverlayPtr->SetPosition(aRect.x(), aRect.y());
   mOverlayPtr->SetSize(aRect.width(), aRect.height());
}

void HDD::Button::SetVisible(bool aState)
{
   mOverlayPtr->SetVisible(aState);

   if (aState)
   {
      if (mViewerPtr && mOverlayPtr)
      {
         mViewerPtr->BringOverlayToFront(mOverlayPtr);
      }
   }
}
