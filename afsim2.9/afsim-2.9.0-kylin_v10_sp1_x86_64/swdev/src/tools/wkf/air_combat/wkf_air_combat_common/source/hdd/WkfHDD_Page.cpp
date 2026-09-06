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

#include "WkfHDD_Page.hpp"

size_t HDD::Page::sId = 1;

HDD::Page::Page(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : QObject(aParentPtr)
   , mId(GetUniqueId())
   , mPageName()
   , mRect()
   , mPosition(aPosition)
   , mViewerPtr(aViewerPtr)
{
}

void HDD::Page::SetPageSize(const size_t aPosition)
{
   switch (aPosition)
   {
   case 5:
   case 6:
   case 7:
   case 8:
   case 9:
   case 10:
   case 11:
   case 12:
      mSize = Page::Small;
      break;
   case 1:
   case 2:
   case 3:
   case 4:
      mSize = Page::Medium;
      break;
   case 13:
   case 14:
   case 15:
   case 16:
      mSize = Page::Large;
      break;
   case 17:
   case 18:
      mSize = Page::Full;
      break;
   default:
      mSize = Page::Small;
      break;
   }
}

void HDD::Page::GetRect(const size_t aPosition, QRectF& aRect)
{
   // Hardcoded rects for each defined position.
   switch (aPosition)
   {
   case 0:
      aRect.setRect(0.0f, 0.85f, 1.0f, 0.15f);
      break;
   case 1:
      aRect.setRect(0.0f, 0.25f, 0.25f, 0.6f);
      break;
   case 2:
      aRect.setRect(0.25f, 0.25f, 0.25f, 0.6f);
      break;
   case 3:
      aRect.setRect(0.5f, 0.25f, 0.25f, 0.6f);
      break;
   case 4:
      aRect.setRect(0.75f, 0.25f, 0.25f, 0.6f);
      break;
   case 5:
      aRect.setRect(0.0f, 0.0f, 0.125f, 0.25f);
      break;
   case 6:
      aRect.setRect(0.125f, 0.0f, 0.125f, 0.25f);
      break;
   case 7:
      aRect.setRect(0.25f, 0.0f, 0.125f, 0.25f);
      break;
   case 8:
      aRect.setRect(0.375f, 0.0f, 0.125f, 0.25f);
      break;
   case 9:
      aRect.setRect(0.5f, 0.0f, 0.125f, 0.25f);
      break;
   case 10:
      aRect.setRect(0.625f, 0.0f, 0.125f, 0.25f);
      break;
   case 11:
      aRect.setRect(0.75f, 0.0f, 0.125f, 0.25f);
      break;
   case 12:
      aRect.setRect(0.875f, 0.0f, 0.125f, 0.25f);
      break;
   case 13:
      aRect.setRect(0.0f, 0.0f, 0.25f, 0.85f);
      break;
   case 14:
      aRect.setRect(0.25f, 0.0f, 0.25f, 0.85f);
      break;
   case 15:
      aRect.setRect(0.5f, 0.0f, 0.25f, 0.85f);
      break;
   case 16:
      aRect.setRect(0.75f, 0.0f, 0.25f, 0.85f);
      break;
   case 17:
      aRect.setRect(0.0f, 0.0f, 0.5f, 0.85f);
      break;
   case 18:
      aRect.setRect(0.5f, 0.0f, 0.5f, 0.85f);
      break;
   default:
      aRect.setRect(0.0f, 0.0f, 1.0f, 1.0f);
      break;
   }
}

const bool HDD::Page::IsVisible() const
{
   return mOverlayPtr->IsVisible();
}

const std::string& HDD::Page::OverlayName() const
{
   return mOverlayPtr->GetName();
}

void HDD::Page::BringOverlayToFront()
{
   mViewerPtr->BringOverlayToFront(mOverlayPtr);
}

void HDD::Page::RemoveOverlay()
{
   mViewerPtr->RemoveOverlay(mOverlayPtr->GetName());
}

void HDD::Page::SetPageName(const QString& aName)
{
   mPageName = aName;
}

void HDD::Page::SetPosition(const size_t aPosition)
{
   // Set the position of this page and update the overlay to match.
   mPosition = aPosition;
   GetRect(mPosition, mRect);
   SetPageSize(mPosition);
   mOverlayPtr->SetPosition(mRect.x() * mViewerPtr->GetWidth(), mRect.y() * mViewerPtr->GetHeight());
   mOverlayPtr->SetSize(mRect.width() * mViewerPtr->GetWidth(), mRect.height() * mViewerPtr->GetHeight());
   mRatio = (mRect.width() * mViewerPtr->GetWidth()) / (mRect.height() * mViewerPtr->GetHeight());
   mOverlayPtr->Update(); // Redraw the page or else we'll get flickering when it swaps/resizes.
}

void HDD::Page::SetVisible(const bool aState)
{
   mOverlayPtr->SetVisible(aState);
}

void HDD::Page::AddClickRegion(ClickRegion aClickRegion)
{
   mClickRegions.push_back(aClickRegion);
}

void HDD::Page::HandleClickRegions(const int aMouseX, const int aMouseY)
{
   // Convert the click x/y to a point.
   QPointF pointClicked(aMouseX, aMouseY);

   // Loop through the click regions for this page.
   for (auto region : mClickRegions)
   {
      // If we have a region for our current page size.
      if (region.rect.find(mSize) != region.rect.end())
      {
         // Get the rect and use it to calculate the regionRect in pixels.
         QRectF r = region.rect.at(mSize);
         // Create a rect for this region in pixel coordinates rather than relative coords.
         double x = mOverlayPtr->GetRect().center().x() + (r.x() * pW()); // Center x of the click region
         double y = mOverlayPtr->GetRect().center().y() + (r.y() * pH()); // Center y of the click region
         double w = r.width() * pW();                                     // Width of the click region
         double h = r.height() * pH();                                    // Height of the click region
         // Subtract half width and height from the center position so our rect is centered.
         QRectF regionRect(x - (w * 0.5), y - (h * 0.5), w, h);

         // If we clicked inside this region, run it's function.
         if (regionRect.contains(pointClicked))
         {
            // If our function is valid.
            if (region.function)
            {
               region.function();
            }
         }
      }
   }
}
