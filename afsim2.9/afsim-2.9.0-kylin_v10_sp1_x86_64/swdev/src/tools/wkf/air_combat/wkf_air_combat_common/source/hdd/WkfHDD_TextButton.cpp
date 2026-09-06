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

#include "WkfHDD_TextButton.hpp"

#include "hdd/WkfHDD_Overlay.hpp"

HDD::TextButton::TextButton(const std::string& aOverlayName,
                            const std::string& aText,
                            vespa::VaViewer*   aViewerPtr,
                            const QRectF&      aRect)
   : Button(aOverlayName + aText, aViewerPtr, aRect)
   , mText(aText)
{
}

void HDD::TextButton::Draw()
{
   double h = static_cast<double>(mOverlayPtr->GetHeight());

   // Size the font from the window size.
   float fontSize = 0.4f * static_cast<float>(h);

   // Draw the current page name
   mOverlayPtr->DrawLabel(0.0, 0.0, mText.c_str(), fontSize, osg::Vec4(0.0, 0.8, 0.8, 1.0), osgText::Text::CENTER_CENTER);
}
