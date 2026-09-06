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

#include "WkfHDD_MenuButton.hpp"

#include <QRect>

#include "VaViewer.hpp"
#include "hdd/WkfHDD_Overlay.hpp"
#include "hdd/WkfHDD_Page.hpp"

HDD::MenuButton::MenuButton(const std::string& aOverlayName,
                            Page*              aPagePtr,
                            vespa::VaViewer*   aViewerPtr,
                            PageMenu*          aPageMenuPtr,
                            const QRectF&      aRect)
   : Button(aOverlayName, aViewerPtr, aRect)
   , mPagePtr(aPagePtr)
   , mPageMenuPtr(aPageMenuPtr)
   , mMenuOpen(false)
{
}

void HDD::MenuButton::Draw()
{
   // Height of the overlay, used for positioning.
   double h        = static_cast<double>(mOverlayPtr->GetHeight());
   float  fontSize = 0.225f * static_cast<float>(h); // size the font from the window size.

   // Draw the current page name.
   mOverlayPtr->DrawLabel(0.0, 0.2 * h, mPagePtr->GetPageName().toStdString(), fontSize, Color::Green, osgText::Text::CENTER_CENTER);

   // If the menu is open, draw "menu popup" in magenta.
   if (mMenuOpen)
   {
      mOverlayPtr->DrawLabel(0.0, 0.0 * h, "MENU", fontSize, Color::Magenta, osgText::Text::CENTER_CENTER);
      mOverlayPtr->DrawLabel(0.0, -0.2 * h, "POPUP", fontSize, Color::Magenta, osgText::Text::CENTER_CENTER);
   }
}

void HDD::MenuButton::SetPageVisible(const bool aState)
{
   mPagePtr->SetVisible(aState);
}

void HDD::MenuButton::SetMenuVisible(const bool aState)
{
   mPageMenuPtr->SetVisible(aState);
}

void HDD::MenuButton::SetMenuPosition(const size_t aPosition)
{
   mPageMenuPtr->SetPosition(aPosition);
}
