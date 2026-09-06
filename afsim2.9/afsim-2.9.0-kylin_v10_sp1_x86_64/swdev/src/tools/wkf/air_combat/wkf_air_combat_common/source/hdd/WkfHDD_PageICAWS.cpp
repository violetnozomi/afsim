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

#include "WkfHDD_PageICAWS.hpp"

HDD::PageICAWS::PageICAWS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_icaws" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageICAWS::Draw, this));
   mPageName = "ICAWS";
}

void HDD::PageICAWS::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   double scale = 1.0;
   if (mSize == Page::Small)
   {
      DrawBoxWithText(vectorGroup, "ICAWS FAILURE", 0.0, 0.4, 0.45, 0.075, scale, mData.icawsFailure);
      DrawBoxWithText(vectorGroup, "ENGINE FIRE", 0.0, 0.2, 0.45, 0.075, scale, mData.engineFire);
      DrawBoxWithText(vectorGroup, "FUEL LOW", 0.0, 0.0, 0.45, 0.075, scale, mData.fuelLow);
      DrawBoxWithText(vectorGroup, "STALL", 0.0, -0.2, 0.45, 0.075, scale, mData.stalling);

      // Draw a text box
      float labelFontSize = 0.125f * static_cast<float>(mOverlayPtr->GetWidth());
      mOverlayPtr->DrawLabel(0.0,
                             -0.45 * static_cast<float>(mOverlayPtr->GetHeight()),
                             mPageName.toStdString().c_str(),
                             labelFontSize,
                             osg::Vec4(0.0, 0.8, 0.8, 1.0),
                             osgText::Text::CENTER_BOTTOM);
   }
   else
   {
      DrawBoxWithText(vectorGroup, "ICAWS FAILURE", 0.0, 0.4, 0.45, 0.075, scale, mData.icawsFailure);
      DrawBoxWithText(vectorGroup, "ENGINE FIRE", 0.0, 0.2, 0.45, 0.075, scale, mData.engineFire);
      DrawBoxWithText(vectorGroup, "FUEL LOW", 0.0, 0.0, 0.45, 0.075, scale, mData.fuelLow);
      DrawBoxWithText(vectorGroup, "STALL", 0.0, -0.2, 0.45, 0.075, scale, mData.stalling);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageICAWS::DrawBoxWithText(osg::ref_ptr<osg::Group> aVectorGroup,
                                     const std::string&       aText,
                                     const double             aX,
                                     const double             aY,
                                     const double             aW,
                                     const double             aH,
                                     const double             aScale,
                                     const bool               aState)
{
   double w        = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h        = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;
   float  fontSize = 0.2f * aW * w;

   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d((aX + aW) * w, (aY + aH) * h));
   boxArray->push_back(osg::Vec2d((aX + aW) * w, (aY - aH) * h));
   boxArray->push_back(osg::Vec2d((aX - aW) * w, (aY - aH) * h));
   boxArray->push_back(osg::Vec2d((aX - aW) * w, (aY + aH) * h));

   // If the warning is active for this box, draw it red with black text, otherwise draw green.
   if (aState)
   {
      osg::ref_ptr<osg::Geode> boxGeode = mOverlayPtr->CreateGeometry(boxArray, GL_POLYGON, Color::Red).get();
      boxGeode->addChild(
         mOverlayPtr->AddTextItem(aText.c_str(), aX * w, aY * h, fontSize, Color::Black, osgText::Text::CENTER_CENTER).get());
      aVectorGroup->addChild(boxGeode);
   }
   else
   {
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, Color::Green).get());
      aVectorGroup->addChild(
         mOverlayPtr->AddTextItem(aText.c_str(), aX * w, aY * h, fontSize, Color::Green, osgText::Text::CENTER_CENTER).get());
   }
}
