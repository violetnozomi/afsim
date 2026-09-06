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

#include "WkfHDD_ArrowButton.hpp"

#include "hdd/WkfHDD_Overlay.hpp"

HDD::ArrowButton::ArrowButton(const std::string& aOverlayName,
                              const ArrowDir     aDirection,
                              vespa::VaViewer*   aViewerPtr,
                              const QRectF&      aRect)
   : Button(aOverlayName, aViewerPtr, aRect)
   , mDirection(aDirection)
{
}

void HDD::ArrowButton::Draw()
{
   double w = static_cast<double>(mOverlayPtr->GetWidth());
   double h = static_cast<double>(mOverlayPtr->GetHeight());

   // Create the triangle geometry
   osg::ref_ptr<osg::Vec2Array> triArray(new osg::Vec2Array);

   // Based on a direction given, draw the arrow (0=up, 1=right, 2=down, 3=left)
   switch (mDirection)
   {
      // Draw an up arrow
   case ArrowDir::Up:
      triArray->push_back(osg::Vec2d(-0.40 * w, -0.30 * h));
      triArray->push_back(osg::Vec2d(0.40 * w, -0.30 * h));
      triArray->push_back(osg::Vec2d(0.0 * w, 0.30 * h));
      break;
      // Draw a right arrow
   case ArrowDir::Right:
      triArray->push_back(osg::Vec2d(-0.30 * w, 0.40 * h));
      triArray->push_back(osg::Vec2d(-0.30 * w, -0.40 * h));
      triArray->push_back(osg::Vec2d(0.30 * w, 0.0 * h));
      break;
      // Draw a down arrow
   case ArrowDir::Down:
      triArray->push_back(osg::Vec2d(-0.40 * w, 0.30 * h));
      triArray->push_back(osg::Vec2d(0.40 * w, 0.30 * h));
      triArray->push_back(osg::Vec2d(0.0 * w, -0.30 * h));
      break;
      // Draw a left arrow
   case ArrowDir::Left:
      triArray->push_back(osg::Vec2d(0.30 * w, 0.40 * h));
      triArray->push_back(osg::Vec2d(0.30 * w, -0.40 * h));
      triArray->push_back(osg::Vec2d(-0.30 * w, 0.0 * h));
      break;
   default:
      break;
   }

   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   mOverlayPtr->GetGeode()->addChild(
      mOverlayPtr->CreateGeometry(triArray, GL_LINE_LOOP, osg::Vec4(0.0, 0.6, 0.6, 1.0)).get());
}
