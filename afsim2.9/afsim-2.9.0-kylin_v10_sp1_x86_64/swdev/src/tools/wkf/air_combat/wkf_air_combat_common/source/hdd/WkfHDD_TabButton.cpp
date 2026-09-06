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

#include "WkfHDD_TabButton.hpp"

#include "VaViewer.hpp"
#include "hdd/WkfHDD_Overlay.hpp"
#include "hdd/WkfHDD_Page.hpp"

HDD::TabButton::TabButton(const std::string& aOverlayName, Page* aPagePtr, vespa::VaViewer* aViewerPtr, const QRectF& aRect)
   : Button(aOverlayName, aViewerPtr, aRect)
   , mPagePtr(std::move(aPagePtr))
{
   mOverlayPtr->SetDrawFunction(std::bind(&TabButton::Draw, this));
}

void HDD::TabButton::Draw()
{
   double w = static_cast<double>(mOverlayPtr->GetWidth());
   double h = static_cast<double>(mOverlayPtr->GetHeight());

   osg::ref_ptr<osg::Geode> vectorGeode(new osg::Geode);
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   // Create the TabButton geometry
   osg::ref_ptr<osg::Vec2Array> TabButtonArray(new osg::Vec2Array);
   TabButtonArray->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   TabButtonArray->push_back(osg::Vec2d(-0.4 * w, 0.48 * h));
   TabButtonArray->push_back(osg::Vec2d(0.4 * w, 0.48 * h));
   TabButtonArray->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   mOverlayPtr->GetGeode()->addChild(
      mOverlayPtr->CreateGeometry(TabButtonArray, GL_LINE_STRIP, osg::Vec4(0.0, 0.6, 0.6, 1.0)).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   // Size the font from the window size.
   float fontSize = 0.5f * static_cast<float>(h);

   // Draw the current page name
   mOverlayPtr->DrawLabel(0.0,
                          0.0,
                          mPagePtr->GetPageName().toStdString(),
                          fontSize,
                          osg::Vec4(0.0, 0.6, 0.6, 1.0),
                          osgText::Text::CENTER_CENTER);

   // Add the finished drawing
   vectorGroup->addChild(vectorGeode.get());
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::TabButton::SetPageVisible(const bool aState)
{
   mPagePtr->SetVisible(aState);
}

size_t const HDD::TabButton::GetPagePosition() const
{
   return mPagePtr->GetPosition();
}

void HDD::TabButton::SetPagePosition(const size_t aPosition)
{
   mPagePtr->SetPosition(aPosition);
}
