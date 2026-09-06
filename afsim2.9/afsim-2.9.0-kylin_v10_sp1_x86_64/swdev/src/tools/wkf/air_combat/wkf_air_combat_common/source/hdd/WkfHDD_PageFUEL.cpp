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

#include "WkfHDD_PageFUEL.hpp"

#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"

HDD::PageFUEL::PageFUEL(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_fuel" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageFUEL::Draw, this));
   mPageName = "FUEL";
}

void HDD::PageFUEL::SetFuelData(const HDD::SimData::FuelData& aData)
{
   mData.fuelValid = aData.fuelValid;
   // If fuel isn't valid, don't bother setting other vars.
   if (mData.fuelValid)
   {
      mData.fuelFlow_pph     = aData.fuelFlow_pph;
      mData.fuelCapInt_lbs   = aData.fuelCapInt_lbs;
      mData.fuelCapExt_lbs   = aData.fuelCapExt_lbs;
      mData.fuelInternal_lbs = aData.fuelInternal_lbs;
      mData.fuelExternal_lbs = aData.fuelExternal_lbs;
      mData.joker_lbs        = aData.joker_lbs;
      mData.bingo_lbs        = aData.bingo_lbs;
   }
}

void HDD::PageFUEL::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   double scale = 1.0;
   // Percentage of internal and external fuel left from 0-100%
   double internalPercent =
      (mData.fuelValid && mData.fuelCapInt_lbs > 0) ? round((mData.fuelInternal_lbs / mData.fuelCapInt_lbs) * 100) : 0;
   double externalPercent =
      (mData.fuelValid && mData.fuelCapExt_lbs > 0) ? round((mData.fuelExternal_lbs / mData.fuelCapExt_lbs) * 100) : 0;

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      DrawSmallFuelText(vectorGroup, 0.0, 0.1, 0.75, 0.75, scale);

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
      // Draw the fuel bars
      DrawFuelBar(vectorGroup, -0.2, 0.15, 0.275, 0.475, scale, "INTERNAL", internalPercent);
      DrawFuelBar(vectorGroup, -0.2, -0.35, 0.275, 0.175, scale, "EXTERNAL", externalPercent);

      // Draw the fuel remaining box
      DrawFuelTankBox(vectorGroup, 0.2, 0.2775, 0.325, 0.22, scale);

      // Draw the fuel flow box
      DrawFuelFlowBox(vectorGroup, 0.2, 0.02625, 0.325, 0.22, scale);

      // Draw joker and bingo
      DrawJokerBingo(vectorGroup, 0.2, -0.3125, 0.325, 0.25, scale);
   }
   else if (mSize == Page::Large)
   {
      // Draw the fuel bars
      DrawFuelBar(vectorGroup, -0.2, 0.3, 0.275, 0.625, scale, "INTERNAL", internalPercent);
      DrawFuelBar(vectorGroup, -0.2, -0.45, 0.275, 0.275, scale, "EXTERNAL", externalPercent);

      // Draw the fuel remaining box
      DrawFuelTankBox(vectorGroup, 0.2, 0.5025, 0.325, 0.22, scale);

      // Draw the fuel flow box
      DrawFuelFlowBox(vectorGroup, 0.2, 0.0975, 0.325, 0.22, scale);

      // Draw joker and bingo
      DrawJokerBingo(vectorGroup, 0.2, -0.4625, 0.325, 0.25, scale);
   }
   else if (mSize == Page::Full)
   {
      scale = 0.5;
      // Draw the fuel bars
      DrawFuelBar(vectorGroup, -0.325, 0.3, 0.275, 0.625, scale, "INTERNAL", internalPercent);
      DrawFuelBar(vectorGroup, -0.325, -0.45, 0.275, 0.275, scale, "EXTERNAL", externalPercent);

      // Draw the fuel remaining box
      DrawFuelTankBox(vectorGroup, 0.325, 0.5025, 0.325, 0.22, scale);

      // Draw the fuel flow box
      DrawFuelFlowBox(vectorGroup, 0.325, 0.0975, 0.325, 0.22, scale);

      // Draw joker and bingo
      DrawJokerBingo(vectorGroup, 0.325, -0.4625, 0.325, 0.25, scale);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageFUEL::DrawFuelBar(osg::ref_ptr<osg::Group> aVectorGroup,
                                const double             aX,
                                const double             aY,
                                const double             aW,
                                const double             aH,
                                const double             aScale,
                                const std::string&       aText,
                                const double             aYFill)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the gauge in it's position
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Bar fill rect
   double                       fillHeight = aH * (aYFill / 100);
   osg::ref_ptr<osg::Vec2Array> fillArray(new osg::Vec2Array);
   fillArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5) + fillHeight) * h));
   fillArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5) + fillHeight) * h));
   fillArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   fillArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(fillArray, GL_POLYGON, Color::Magenta).get());

   // Bar rect
   osg::ref_ptr<osg::Vec2Array> barArray(new osg::Vec2Array);
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(barArray, GL_LINE_LOOP, Color::Cyan).get());

   float fontSize = 0.055f * w;
   // Text label above the bar
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(aText, aX * w, (aY + (aH * 0.5) + 0.075) * h, fontSize, Color::Cyan, osgText::Text::CENTER_TOP)
         .get());

   if (mData.fuelValid)
   {
      // Fuel percentage text label
      vectorXform->addChild(mOverlayPtr->AddTextItem(std::to_string(static_cast<int>(aYFill)) + "%",
                                                     aX * w,
                                                     aY * h,
                                                     fontSize,
                                                     Color::White,
                                                     osgText::Text::CENTER_CENTER));
   }
   else
   {
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("N/A", aX * w, aY * h, fontSize, Color::White, osgText::Text::CENTER_CENTER));
   }

   // Make sure the text draws on top of the polygons
   osg::StateSet* ss = vectorXform->getOrCreateStateSet();
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageFUEL::DrawFuelTankBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                    const double             aX,
                                    const double             aY,
                                    const double             aW,
                                    const double             aH,
                                    const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the gauge in it's position
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Bar rect
   osg::ref_ptr<osg::Vec2Array> barArray(new osg::Vec2Array);
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(barArray, GL_LINE_LOOP, Color::Green).get());

   float fontSize = 0.045f * w;
   // Draw the labels
   vectorXform->addChild(mOverlayPtr->AddTextItem("TOT:",
                                                  (aX - (aW * 0.40)) * w,
                                                  (aY + (aH * 0.325)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_TOP));
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("INT:", (aX - (aW * 0.40)) * w, aY * h, fontSize, Color::Green, osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("EXT:",
                                                  (aX - (aW * 0.40)) * w,
                                                  (aY - (aH * 0.325)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_BOTTOM));

   // If fuel is valid fill in the values, otherwise draw N/A
   if (mData.fuelValid)
   {
      std::stringstream totalFuel;
      double            totFuelRemaining = mData.fuelInternal_lbs + mData.fuelExternal_lbs;
      totalFuel << std::fixed << std::setprecision(0) << std::setw(5) << totFuelRemaining;
      vectorXform->addChild(mOverlayPtr->AddTextItem(totalFuel.str(),
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY + (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_TOP));

      std::stringstream intFuel;
      intFuel << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelInternal_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(intFuel.str(),
                                                     (aX + (aW * 0.40)) * w,
                                                     aY * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream extFuel;
      extFuel << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelExternal_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(extFuel.str(),
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY - (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_BOTTOM));
   }
   else
   {
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY + (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_TOP));
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("N/A", (aX + (aW * 0.40)) * w, aY * h, fontSize, Color::Green, osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY - (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_BOTTOM));
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageFUEL::DrawFuelFlowBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                    const double             aX,
                                    const double             aY,
                                    const double             aW,
                                    const double             aH,
                                    const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the gauge in it's position
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   boxArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   boxArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   boxArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, Color::Cyan).get());

   float fontSize = 0.045f * w;
   // Draw the labels
   vectorXform->addChild(mOverlayPtr->AddTextItem("FF:",
                                                  (aX - (aW * 0.40)) * w,
                                                  (aY + (aH * 0.325)) * h,
                                                  fontSize,
                                                  Color::Cyan,
                                                  osgText::Text::LEFT_TOP));
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("TIME:", (aX - (aW * 0.40)) * w, aY * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("DIST:",
                                                  (aX - (aW * 0.40)) * w,
                                                  (aY - (aH * 0.325)) * h,
                                                  fontSize,
                                                  Color::Cyan,
                                                  osgText::Text::LEFT_BOTTOM));

   // If fuel is valid fill in the values, otherwise draw N/A
   if (mData.fuelValid)
   {
      std::stringstream fuelFlow;
      fuelFlow << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelFlow_pph;
      vectorXform->addChild(mOverlayPtr->AddTextItem(fuelFlow.str(),
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY + (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_TOP));

      double totFuelRemaining   = mData.fuelInternal_lbs + mData.fuelExternal_lbs;
      double timeToEmptyMinutes = totFuelRemaining / (mData.fuelFlow_pph / 60.0);
      vectorXform->addChild(mOverlayPtr->AddTextItem(GetTimeToEmptyString(timeToEmptyMinutes),
                                                     (aX + (aW * 0.40)) * w,
                                                     aY * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream dist;
      double            nm = (timeToEmptyMinutes / 60) * mData.ktas;
      dist << std::fixed << std::setprecision(0) << std::setw(5) << nm;
      vectorXform->addChild(mOverlayPtr->AddTextItem(dist.str(),
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY - (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_BOTTOM));
   }
   else
   {
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY + (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_TOP));
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("N/A", (aX + (aW * 0.40)) * w, aY * h, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.40)) * w,
                                                     (aY - (aH * 0.325)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_BOTTOM));
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageFUEL::DrawJokerBingo(osg::ref_ptr<osg::Group> aVectorGroup,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH,
                                   const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the objects
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   float labelFontSize  = 0.055f * w;
   float numberFontSize = 0.045f * w;
   // Draw the labels
   vectorXform->addChild(mOverlayPtr->AddTextItem("JOKER",
                                                  (aX + (aW * 0.5)) * w,
                                                  (aY + (aH * 0.5)) * h,
                                                  labelFontSize,
                                                  Color::Cyan,
                                                  osgText::Text::RIGHT_TOP));
   vectorXform->addChild(mOverlayPtr->AddTextItem("BINGO",
                                                  (aX + (aW * 0.5)) * w,
                                                  (aY - (aH * 0.3)) * h,
                                                  labelFontSize,
                                                  Color::Cyan,
                                                  osgText::Text::RIGHT_BOTTOM));

   // If fuel is valid fill in the values, otherwise draw N/A
   if (mData.fuelValid)
   {
      std::stringstream joker;
      joker << std::fixed << std::setprecision(0) << std::setw(5) << mData.joker_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(joker.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.275)) * h,
                                                     numberFontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_TOP));

      std::stringstream bingo;
      bingo << std::fixed << std::setprecision(0) << std::setw(5) << mData.bingo_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(bingo.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY - (aH * 0.5)) * h,
                                                     numberFontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_BOTTOM));
   }
   else
   {
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.275)) * h,
                                                     numberFontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_TOP));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY - (aH * 0.5)) * h,
                                                     numberFontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_BOTTOM));
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageFUEL::DrawSmallFuelText(osg::ref_ptr<osg::Group> aVectorGroup,
                                      const double             aX,
                                      const double             aY,
                                      const double             aW,
                                      const double             aH,
                                      const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the objects
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   float fontSize = 0.1f * w;
   // Draw the labels
   vectorXform->addChild(mOverlayPtr->AddTextItem("TOT",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.375)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("INT",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.225)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("EXT",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.075)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("JOKER",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * -0.075)) * h,
                                                  fontSize,
                                                  Color::Cyan,
                                                  osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("BINGO",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * -0.225)) * h,
                                                  fontSize,
                                                  Color::Cyan,
                                                  osgText::Text::LEFT_CENTER));
   vectorXform->addChild(mOverlayPtr->AddTextItem("FF",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * -0.375)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));

   // If fuel is valid fill in the values, otherwise draw N/A
   if (mData.fuelValid)
   {
      std::stringstream totFuel;
      double            totFuelRemaining = mData.fuelInternal_lbs + mData.fuelExternal_lbs;
      totFuel << std::fixed << std::setprecision(0) << std::setw(5) << totFuelRemaining;
      vectorXform->addChild(mOverlayPtr->AddTextItem(totFuel.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.375)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream intFuel;
      intFuel << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelInternal_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(intFuel.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.225)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream extFuel;
      extFuel << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelExternal_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(extFuel.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.075)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream joker;
      joker << std::fixed << std::setprecision(0) << std::setw(5) << mData.joker_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(joker.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.075)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream bingo;
      bingo << std::fixed << std::setprecision(0) << std::setw(5) << mData.bingo_lbs;
      vectorXform->addChild(mOverlayPtr->AddTextItem(bingo.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.225)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_CENTER));

      std::stringstream ff;
      ff << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelFlow_pph;
      vectorXform->addChild(mOverlayPtr->AddTextItem(ff.str(),
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.375)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));
   }
   else
   {
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.375)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.225)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * 0.075)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.075)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.225)) * h,
                                                     fontSize,
                                                     Color::Cyan,
                                                     osgText::Text::RIGHT_CENTER));
      vectorXform->addChild(mOverlayPtr->AddTextItem("N/A",
                                                     (aX + (aW * 0.5)) * w,
                                                     (aY + (aH * -0.375)) * h,
                                                     fontSize,
                                                     Color::Green,
                                                     osgText::Text::RIGHT_CENTER));
   }

   aVectorGroup->addChild(vectorXform.get());
}

std::string HDD::PageFUEL::GetTimeToEmptyString(double aMinutes)
{
   // This gets a string representing how long the aircraft can fly
   // in h:m
   // Check if the minutes are valid
   if (aMinutes <= 0.0)
   {
      return "0:00";
   }

   double hours = std::floor(std::floor(std::round(aMinutes) / 60.0));
   double mins  = static_cast<int>(std::floor(std::round(aMinutes))) % 60;

   std::stringstream s;
   s << hours << ":" << std::fixed << std::setprecision(0) << mins;
   return s.str();
}
