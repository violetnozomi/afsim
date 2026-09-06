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

#include "WkfHDD_PageTex.hpp"

#include "VaViewer.hpp"

HDD::PageTex::PageTex(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : PageTex(aViewerPtr, aPosition, aParentPtr, nullptr)
{
}

HDD::PageTex::PageTex(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, const char* aTexture, bool aSubTex)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayTexPtr = new vespa::VaOverlayTexture("hdd_background_" + QString::number(Id()).toStdString());
   aViewerPtr->AddOverlay(mOverlayTexPtr);

   GetRect(mPosition, mRect);
   mOverlayTexPtr->SetVisible(true);
   mOverlayTexPtr->SetPosition(mRect.x() * aViewerPtr->GetWidth(), mRect.y() * aViewerPtr->GetHeight());
   mOverlayTexPtr->SetSize(mRect.width() * aViewerPtr->GetWidth(), mRect.height() * aViewerPtr->GetHeight());
   mOverlayTexPtr->SetShader("VaOverlayTexture", "VaOverlayTextureColor");
   if (aTexture)
   {
      if (!aSubTex)
      {
         mOverlayTexPtr->LoadImageFile(aTexture);
      }
      else
      {
         mOverlayTexPtr->LoadSubTextureFromImageFile(aTexture, mRect.x(), mRect.y(), mRect.width(), mRect.height());
      }
   }

   mOverlayTexPtr->SetButton1ClickAction(
      [this](int x, int y, int s)
      {
         emit PageClicked(this);
         return false;
      });
}

const bool HDD::PageTex::IsVisible() const
{
   return mOverlayTexPtr->IsVisible();
}

const std::string& HDD::PageTex::OverlayName() const
{
   return mOverlayTexPtr->GetName();
}

void HDD::PageTex::RemoveOverlay()
{
   mViewerPtr->RemoveOverlay(mOverlayTexPtr->GetName());
}

void HDD::PageTex::SetPosition(const size_t aPosition)
{
   mPosition = aPosition;
   GetRect(mPosition, mRect);
   SetPageSize(mPosition);
   mOverlayTexPtr->SetPosition(mRect.x() * mViewerPtr->GetWidth(), mRect.y() * mViewerPtr->GetHeight());
   mOverlayTexPtr->SetSize(mRect.width() * mViewerPtr->GetWidth(), mRect.height() * mViewerPtr->GetHeight());
}

void HDD::PageTex::SetVisible(bool aState)
{
   mOverlayTexPtr->SetVisible(aState);
}
