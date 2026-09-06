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

#include "WkfHDD_PageFCS.hpp"

HDD::PageFCS::PageFCS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_fcs" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageFCS::Draw, this));
   mPageName = "FCS";
}

void HDD::PageFCS::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   double scale = 1.0;
   if (mSize == Page::Small)
   {
      scale = 1.0;
      DrawControlBox(vectorGroup, 0.0, 0.1, 0.225, 0.3, scale);

      // Draw a text box
      float labelFontSize = 0.125f * static_cast<float>(mOverlayPtr->GetWidth());
      mOverlayPtr->DrawLabel(0.0,
                             -0.45 * static_cast<float>(mOverlayPtr->GetHeight()),
                             mPageName.toStdString().c_str(),
                             labelFontSize,
                             osg::Vec4(0.0, 0.8, 0.8, 1.0),
                             osgText::Text::CENTER_BOTTOM);
   }
   else if (mSize == Page::Medium)
   {
      scale = 1.0;
      DrawControlBox(vectorGroup, 0.8, 0.0, 0.225, 0.3, scale * 0.45);
      DrawAircraftTopView(vectorGroup, 0.0, -0.45, scale * 1.25);
      DrawAircraftSideView(vectorGroup, 0.025, 0.45, 0.375, 0.125, scale);
   }
   else if (mSize == Page::Large)
   {
      scale = 1.0;
      DrawControlBox(vectorGroup, 0.8, 0.0, 0.225, 0.3, scale * 0.45);
      DrawAircraftTopView(vectorGroup, 0.0, -0.6, scale * 1.25);
      DrawAircraftSideView(vectorGroup, 0.02, 0.6, 0.375, 0.125, scale);
   }
   else if (mSize == Page::Full)
   {
      scale = 0.75;
      DrawControlBox(vectorGroup, 1.0, -0.05, 0.225, 0.3, scale * 0.45);
      DrawAircraftTopView(vectorGroup, -0.05, -0.40, scale * 1.25);
      DrawAircraftSideView(vectorGroup, -0.05, 0.45, 0.375, 0.125, scale);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageFCS::DrawControlBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aBoxW,
                                  const double             aBoxH,
                                  const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Outer Box
   osg::ref_ptr<osg::Vec2Array> boxOuterArray(new osg::Vec2Array);
   boxOuterArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY + aBoxH) * h));
   boxOuterArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY - aBoxH) * h));
   boxOuterArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY - aBoxH) * h));
   boxOuterArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY + aBoxH) * h));

   // Inner Box /w gaps for the horizontal/vertical lines
   osg::ref_ptr<osg::Vec2Array> boxTopRight(new osg::Vec2Array);
   boxTopRight->push_back(osg::Vec2d((aX + aBoxW * 0.075) * w, (aY + aBoxH * 0.5) * h));
   boxTopRight->push_back(osg::Vec2d((aX + aBoxW * 0.5) * w, (aY + aBoxH * 0.5) * h));
   boxTopRight->push_back(osg::Vec2d((aX + aBoxW * 0.5) * w, (aY + aBoxW * 0.075) * h));
   osg::ref_ptr<osg::Vec2Array> boxBottomRight(new osg::Vec2Array);
   boxBottomRight->push_back(osg::Vec2d((aX + aBoxW * 0.5) * w, (aY - aBoxW * 0.075) * h));
   boxBottomRight->push_back(osg::Vec2d((aX + aBoxW * 0.5) * w, (aY - aBoxH * 0.5) * h));
   boxBottomRight->push_back(osg::Vec2d((aX + aBoxW * 0.075) * w, (aY - aBoxH * 0.5) * h));
   osg::ref_ptr<osg::Vec2Array> boxBottomLeft(new osg::Vec2Array);
   boxBottomLeft->push_back(osg::Vec2d((aX - aBoxW * 0.075) * w, (aY - aBoxH * 0.5) * h));
   boxBottomLeft->push_back(osg::Vec2d((aX - aBoxW * 0.5) * w, (aY - aBoxH * 0.5) * h));
   boxBottomLeft->push_back(osg::Vec2d((aX - aBoxW * 0.5) * w, (aY - aBoxW * 0.075) * h));
   osg::ref_ptr<osg::Vec2Array> boxTopLeft(new osg::Vec2Array);
   boxTopLeft->push_back(osg::Vec2d((aX - aBoxW * 0.5) * w, (aY + aBoxW * 0.075) * h));
   boxTopLeft->push_back(osg::Vec2d((aX - aBoxW * 0.5) * w, (aY + aBoxH * 0.5) * h));
   boxTopLeft->push_back(osg::Vec2d((aX - aBoxW * 0.075) * w, (aY + aBoxH * 0.5) * h));

   // Scale the control position by the w/h of the box
   double cX = mData.centerControlPos.x() * aBoxW;
   double cY = mData.centerControlPos.y() * aBoxH;

   // Center diamond
   osg::ref_ptr<osg::Vec2Array> diamondArray(new osg::Vec2Array);
   diamondArray->push_back(osg::Vec2d((aX + cX) * w, (aY + cY + aBoxH * 0.125) * h));
   diamondArray->push_back(osg::Vec2d((aX + cX + aBoxW * 0.2) * w, (aY + cY) * h));
   diamondArray->push_back(osg::Vec2d((aX + cX) * w, (aY + cY - aBoxH * 0.125) * h));
   diamondArray->push_back(osg::Vec2d((aX + cX - aBoxW * 0.2) * w, (aY + cY) * h));

   // Diamond horizontal and vertical lines (lines from diamond to edge of box)
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aX + cX) * w, (aY + cY + aBoxH * 0.125) * h));
   linesArray->push_back(osg::Vec2d((aX + cX) * w, (aY + aBoxH) * h));
   linesArray->push_back(osg::Vec2d((aX + cX + aBoxW * 0.2) * w, (aY + cY) * h));
   linesArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY + cY) * h));
   linesArray->push_back(osg::Vec2d((aX + cX) * w, (aY + cY - aBoxH * 0.125) * h));
   linesArray->push_back(osg::Vec2d((aX + cX) * w, (aY - aBoxH) * h));
   linesArray->push_back(osg::Vec2d((aX + cX - aBoxW * 0.2) * w, (aY + cY) * h));
   linesArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY + cY) * h));

   // Scale the bottom control position to the box width
   double bX = mData.bottomControlPos * aBoxW;
   double bY = -0.4;

   // Bottom diamond
   osg::ref_ptr<osg::Vec2Array> diamond2Array(new osg::Vec2Array);
   diamond2Array->push_back(osg::Vec2d((aX + bX) * w, (aY + bY + aBoxH * 0.125) * h));
   diamond2Array->push_back(osg::Vec2d((aX + bX + aBoxW * 0.2) * w, (aY + bY) * h));
   diamond2Array->push_back(osg::Vec2d((aX + bX) * w, (aY + bY - aBoxH * 0.125) * h));
   diamond2Array->push_back(osg::Vec2d((aX + bX - aBoxW * 0.2) * w, (aY + bY) * h));

   // Bottom horizontal lines
   osg::ref_ptr<osg::Vec2Array> lines2Array(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aX + bX + aBoxW * 0.2) * w, (aY + bY) * h));
   linesArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY + bY) * h));
   linesArray->push_back(osg::Vec2d((aX + bX - aBoxW * 0.2) * w, (aY + bY) * h));
   linesArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY + bY) * h));
   linesArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY + bY - aBoxH * 0.05) * h));
   linesArray->push_back(osg::Vec2d((aX - aBoxW) * w, (aY + bY + aBoxH * 0.05) * h));
   linesArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY + bY - aBoxH * 0.05) * h));
   linesArray->push_back(osg::Vec2d((aX + aBoxW) * w, (aY + bY + aBoxH * 0.05) * h));

   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxOuterArray, GL_LINE_LOOP, Color::Cyan).get());
   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxTopRight, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxBottomRight, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxBottomLeft, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(boxTopLeft, GL_LINE_STRIP, Color::Cyan).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(linesArray, GL_LINES, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(diamondArray, GL_POLYGON, Color::Green).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(lines2Array, GL_LINES, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(diamond2Array, GL_POLYGON, Color::Green).get());
}

void HDD::PageFCS::DrawAircraftTopView(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   osg::ref_ptr<osg::Geode> vectorGeode(new osg::Geode);

   // All of the acArrays piece together into the full aircraft.
   osg::ref_ptr<osg::Vec2Array> acArray(new osg::Vec2Array);
   acArray->push_back(osg::Vec2d((aX + 0.06250) * w, (aY + 0.525) * h));
   acArray->push_back(osg::Vec2d((aX + 0.09125) * w, (aY + 0.35) * h));
   acArray->push_back(osg::Vec2d((aX + 0.23) * w, (aY + 0.24) * h));
   acArray->push_back(osg::Vec2d((aX + 0.23) * w, (aY + 0.14) * h));
   acArray->push_back(osg::Vec2d((aX + 0.08) * w, (aY + 0.0925) * h));
   osg::ref_ptr<osg::Vec2Array> ac2Array(new osg::Vec2Array);
   ac2Array->push_back(osg::Vec2d((aX - 0.06250) * w, (aY + 0.525) * h));
   ac2Array->push_back(osg::Vec2d((aX - 0.09125) * w, (aY + 0.35) * h));
   ac2Array->push_back(osg::Vec2d((aX - 0.23) * w, (aY + 0.24) * h));
   ac2Array->push_back(osg::Vec2d((aX - 0.23) * w, (aY + 0.14) * h));
   ac2Array->push_back(osg::Vec2d((aX - 0.08) * w, (aY + 0.0925) * h));

   osg::ref_ptr<osg::Vec2Array> ac3Array(new osg::Vec2Array);
   ac3Array->push_back(osg::Vec2d((aX + 0.1) * w, (aY + 0.3425) * h));
   ac3Array->push_back(osg::Vec2d((aX + 0.1) * w, (aY + 0.3175) * h));
   ac3Array->push_back(osg::Vec2d((aX + 0.23) * w, (aY + 0.215) * h));
   osg::ref_ptr<osg::Vec2Array> ac4Array(new osg::Vec2Array);
   ac4Array->push_back(osg::Vec2d((aX - 0.1) * w, (aY + 0.3425) * h));
   ac4Array->push_back(osg::Vec2d((aX - 0.1) * w, (aY + 0.3175) * h));
   ac4Array->push_back(osg::Vec2d((aX - 0.23) * w, (aY + 0.215) * h));

   osg::ref_ptr<osg::Vec2Array> ac5Array(new osg::Vec2Array);
   ac5Array->push_back(osg::Vec2d((aX + 0.19) * w, (aY + 0.1275) * h));
   ac5Array->push_back(osg::Vec2d((aX + 0.19) * w, (aY + 0.1675) * h));
   ac5Array->push_back(osg::Vec2d((aX + 0.075) * w, (aY + 0.13) * h));
   ac5Array->push_back(osg::Vec2d((aX + 0.0725) * w, (aY + 0.11) * h));
   osg::ref_ptr<osg::Vec2Array> ac6Array(new osg::Vec2Array);
   ac6Array->push_back(osg::Vec2d((aX - 0.19) * w, (aY + 0.1275) * h));
   ac6Array->push_back(osg::Vec2d((aX - 0.19) * w, (aY + 0.1675) * h));
   ac6Array->push_back(osg::Vec2d((aX - 0.075) * w, (aY + 0.13) * h));
   ac6Array->push_back(osg::Vec2d((aX - 0.0725) * w, (aY + 0.11) * h));

   osg::ref_ptr<osg::Vec2Array> ac7Array(new osg::Vec2Array);
   ac7Array->push_back(osg::Vec2d((aX + 0.06) * w, (aY + 0.14) * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.10) * w, (aY + 0.05) * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.10) * w, aY * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.055) * w, (aY + 0.03) * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.0475) * w, (aY + 0.06) * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.05) * w, (aY + 0.095) * h));
   ac7Array->push_back(osg::Vec2d((aX + 0.05) * w, (aY + 0.10) * h));
   osg::ref_ptr<osg::Vec2Array> ac8Array(new osg::Vec2Array);
   ac8Array->push_back(osg::Vec2d((aX + 0.10) * w, (aY + 0.02) * h));
   ac8Array->push_back(osg::Vec2d((aX + 0.055) * w, (aY + 0.05) * h));
   ac8Array->push_back(osg::Vec2d((aX + 0.0575) * w, (aY + 0.0275) * h));
   osg::ref_ptr<osg::Vec2Array> ac9Array(new osg::Vec2Array);
   ac9Array->push_back(osg::Vec2d((aX - 0.06) * w, (aY + 0.14) * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.10) * w, (aY + 0.05) * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.10) * w, aY * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.055) * w, (aY + 0.03) * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.0475) * w, (aY + 0.06) * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.05) * w, (aY + 0.095) * h));
   ac9Array->push_back(osg::Vec2d((aX - 0.05) * w, (aY + 0.10) * h));
   osg::ref_ptr<osg::Vec2Array> ac10Array(new osg::Vec2Array);
   ac10Array->push_back(osg::Vec2d((aX - 0.10) * w, (aY + 0.02) * h));
   ac10Array->push_back(osg::Vec2d((aX - 0.055) * w, (aY + 0.05) * h));
   ac10Array->push_back(osg::Vec2d((aX - 0.0575) * w, (aY + 0.0275) * h));

   osg::ref_ptr<osg::Vec2Array> ac11Array(new osg::Vec2Array);
   ac11Array->push_back(osg::Vec2d((aX + 0.10) * w, (aY + 0.04) * h));
   ac11Array->push_back(osg::Vec2d((aX + 0.14) * w, (aY + 0.00) * h));
   ac11Array->push_back(osg::Vec2d((aX + 0.14) * w, (aY - 0.025) * h));
   ac11Array->push_back(osg::Vec2d((aX + 0.045) * w, (aY - 0.045) * h));
   ac11Array->push_back(osg::Vec2d((aX + 0.03) * w, (aY + 0.04) * h));
   osg::ref_ptr<osg::Vec2Array> ac12Array(new osg::Vec2Array);
   ac12Array->push_back(osg::Vec2d((aX - 0.10) * w, (aY + 0.04) * h));
   ac12Array->push_back(osg::Vec2d((aX - 0.14) * w, (aY + 0.00) * h));
   ac12Array->push_back(osg::Vec2d((aX - 0.14) * w, (aY - 0.025) * h));
   ac12Array->push_back(osg::Vec2d((aX - 0.045) * w, (aY - 0.045) * h));
   ac12Array->push_back(osg::Vec2d((aX - 0.03) * w, (aY + 0.04) * h));

   osg::ref_ptr<osg::Vec2Array> ac13Array(new osg::Vec2Array);
   ac13Array->push_back(osg::Vec2d((aX + 0.03) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.015) * w, (aY + 0.025) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.015) * w, (aY + 0.025) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.03) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.025) * w, (aY + 0.0375) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.02) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.015) * w, (aY + 0.0375) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.01) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX - 0.005) * w, (aY + 0.0375) * h));
   ac13Array->push_back(osg::Vec2d(aX * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.005) * w, (aY + 0.0375) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.01) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.015) * w, (aY + 0.0375) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.02) * w, (aY + 0.04) * h));
   ac13Array->push_back(osg::Vec2d((aX + 0.025) * w, (aY + 0.0375) * h));

   osg::ref_ptr<osg::Vec2Array> ac14Array(new osg::Vec2Array);
   ac14Array->push_back(osg::Vec2d((aX + 0.0325) * w, (aY + 0.025) * h));
   ac14Array->push_back(osg::Vec2d((aX + 0.07) * w, (aY + 0.02) * h));
   ac14Array->push_back(osg::Vec2d((aX - 0.0325) * w, (aY + 0.025) * h));
   ac14Array->push_back(osg::Vec2d((aX - 0.07) * w, (aY + 0.02) * h));

   osg::ref_ptr<osg::Vec2Array> ac15Array(new osg::Vec2Array);
   ac15Array->push_back(osg::Vec2d((aX + 0.06250) * w, (aY + 0.525) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.0475) * w, (aY + 0.56) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.0425) * w, (aY + 0.53) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.041) * w, (aY + 0.58) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.030) * w, (aY + 0.60) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.020) * w, (aY + 0.585) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.010) * w, (aY + 0.60) * h));
   ac15Array->push_back(osg::Vec2d((aX + 0.0) * w, (aY + 0.59) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.0) * w, (aY + 0.59) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.010) * w, (aY + 0.60) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.020) * w, (aY + 0.585) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.030) * w, (aY + 0.60) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.041) * w, (aY + 0.58) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.0425) * w, (aY + 0.53) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.0475) * w, (aY + 0.56) * h));
   ac15Array->push_back(osg::Vec2d((aX - 0.06250) * w, (aY + 0.525) * h));

   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(acArray, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac2Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac3Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac4Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac5Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac6Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac7Array, GL_LINE_LOOP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac8Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac9Array, GL_LINE_LOOP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac10Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac11Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac12Array, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac13Array, GL_LINE_LOOP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac14Array, GL_LINES, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(ac15Array, GL_LINE_STRIP, Color::Cyan).get());
}

void HDD::PageFCS::DrawAircraftSideView(osg::ref_ptr<osg::Group> aVectorGroup,
                                        const double             aX,
                                        const double             aY,
                                        const double             aW,
                                        const double             aH,
                                        const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Main body
   osg::ref_ptr<osg::Vec2Array> acArray(new osg::Vec2Array);
   acArray->push_back(osg::Vec2d((aX + aW * 0.48) * w, (aY + aH * 0.08) * h)); // Rear top
   acArray->push_back(osg::Vec2d((aX + aW * 0.24) * w, (aY + aH * 0.10) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.24) * w, (aY + aH * 0.12) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.44) * w, (aY + aH * 0.20) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.52) * w, (aY + aH * 0.25) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.60) * w, (aY + aH * 0.25) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.74) * w, (aY + aH * 0.0) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.84) * w, (aY - aH * 0.05) * h));
   acArray->push_back(osg::Vec2d((aX - aW) * w, (aY - aH * 0.3) * h)); // Nose
   acArray->push_back(osg::Vec2d((aX - aW * 0.84) * w, (aY - aH * 0.55) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.68) * w, (aY - aH * 0.7) * h));
   acArray->push_back(osg::Vec2d((aX - aW * 0.36) * w, (aY - aH * 0.8) * h));
   acArray->push_back(osg::Vec2d((aX + aW * 0.52) * w, (aY - aH * 0.825) * h));
   acArray->push_back(osg::Vec2d((aX + aW * 0.68) * w, (aY - aH * 0.7) * h));
   acArray->push_back(osg::Vec2d((aX + aW * 0.74) * w, (aY - aH * 0.55) * h));
   acArray->push_back(osg::Vec2d((aX + aW * 0.75) * w, (aY - aH * 0.35) * h));
   acArray->push_back(osg::Vec2d((aX + aW * 0.75) * w, (aY - aH * 0.25) * h)); // Rear bottom

   // Rear wing
   osg::ref_ptr<osg::Vec2Array> rearWingArray(new osg::Vec2Array);
   rearWingArray->push_back(osg::Vec2d((aX + aW * 0.76) * w, (aY + aH * 0.76) * h));
   rearWingArray->push_back(osg::Vec2d((aX + aW * 0.70) * w, (aY - aH * 0.10) * h));
   rearWingArray->push_back(osg::Vec2d((aX + aW * 0.44) * w, (aY - aH * 0.10) * h));
   rearWingArray->push_back(osg::Vec2d((aX + aW * 0.62) * w, (aY + aH * 0.72) * h));

   // Rear wing 2
   osg::ref_ptr<osg::Vec2Array> rearWing2Array(new osg::Vec2Array);
   rearWing2Array->push_back(osg::Vec2d((aX + aW * 0.46) * w, (aY - aH * 0.14) * h));
   rearWing2Array->push_back(osg::Vec2d((aX + aW * 0.70) * w, (aY - aH * 0.12) * h));
   rearWing2Array->push_back(osg::Vec2d((aX + aW * 0.90) * w, (aY - aH * 0.26) * h));
   rearWing2Array->push_back(osg::Vec2d((aX + aW * 0.70) * w, (aY - aH * 0.22) * h));

   // Extra line geometry (window, exhaust)
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aX + aW * 0.68) * w, (aY - aH * 0.7) * h));
   linesArray->push_back(osg::Vec2d((aX + aW * 0.68) * w, (aY - aH * 0.22) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.74) * w, (aY + aH * 0.0) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.60) * w, (aY + aH * 0.0) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.60) * w, (aY + aH * 0.0) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.44) * w, (aY + aH * 0.20) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.60) * w, (aY + aH * 0.0) * h));
   linesArray->push_back(osg::Vec2d((aX - aW * 0.64) * w, (aY + aH * 0.18) * h));

   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(acArray, GL_LINE_STRIP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(rearWingArray, GL_LINE_LOOP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(rearWing2Array, GL_LINE_LOOP, Color::Cyan).get());
   aVectorGroup->addChild(mOverlayPtr->CreateGeometry(linesArray, GL_LINES, Color::Cyan).get());
}
