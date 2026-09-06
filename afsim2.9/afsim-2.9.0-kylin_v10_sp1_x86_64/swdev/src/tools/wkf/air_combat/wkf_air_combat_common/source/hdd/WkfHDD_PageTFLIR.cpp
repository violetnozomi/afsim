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

#include "WkfHDD_PageTFLIR.hpp"

HDD::PageTFLIR::PageTFLIR(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_tflir" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
   aViewerPtr->AddOverlay(mOverlayPtr);
   SetPosition(aPosition);
   mOverlayPtr->SetVisible(true);
   mOverlayPtr->SetButton1ClickAction(
      [this](int x, int y, int s) -> bool
      {
         // Only handle click events when visible
         if (IsVisible())
         {
            emit PageClicked(this);
         }
         return false;
      });
   mOverlayPtr->SetDrawFunction(std::bind(&PageTFLIR::Draw, this));
   mPageName = "TFLIR";
}

void HDD::PageTFLIR::Draw()
{
   double scale        = 1.0;
   double circleRadius = 0.4;

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   DrawCrosshair(vectorGroup, 0.0, 0.0, circleRadius, scale);
   DrawCircle(vectorGroup, 0.0, 0.0, circleRadius, scale);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      // Draw a text box
      float labelFontSize = 0.125f * static_cast<float>(mOverlayPtr->GetWidth());
      mOverlayPtr->DrawLabel(0.0,
                             -0.45 * static_cast<float>(mOverlayPtr->GetHeight()),
                             mPageName.toStdString().c_str(),
                             labelFontSize,
                             osg::Vec4(0.0, 0.8, 0.8, 1.0),
                             osgText::Text::CENTER_BOTTOM);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageTFLIR::DrawCircle(osg::ref_ptr<osg::Group> aVectorGroup,
                                const double             aX,
                                const double             aY,
                                const double             aRadius,
                                const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Circle
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aX + (aRadius * cos(ang_rad));
      double y       = aY + (aRadius * sin(ang_rad));
      circleArray->push_back(osg::Vec2d(x * w, y * h));
   }

   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_LOOP, Color::Magenta).get());
}

void HDD::PageTFLIR::DrawCrosshair(osg::ref_ptr<osg::Group> aVectorGroup,
                                   const double             aX,
                                   const double             aY,
                                   const double             aRadius,
                                   const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aX + (aRadius * 0.1)) * w, 0.0));
   linesArray->push_back(osg::Vec2d((aX + (aRadius * 0.7)) * w, 0.0)); // Right Line
   linesArray->push_back(osg::Vec2d((aX - (aRadius * 0.1)) * w, 0.0));
   linesArray->push_back(osg::Vec2d((aX - (aRadius * 0.7)) * w, 0.0)); // Left line
   linesArray->push_back(osg::Vec2d(aX, (aY + (aRadius * 0.1)) * h));
   linesArray->push_back(osg::Vec2d(aX, (aY + (aRadius * 0.7)) * h)); // Up line
   linesArray->push_back(osg::Vec2d(aX, (aY - (aRadius * 0.1)) * h));
   linesArray->push_back(osg::Vec2d(aX, (aY - (aRadius * 0.7)) * h)); // Down line

   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(linesArray, GL_LINES, Color::Cyan).get());
}
