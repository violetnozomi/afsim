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

#include "WkfHDD_PageHeader.hpp"

#include <QTime>
#include <osg/PositionAttitudeTransform>

#include "UtCalendar.hpp"
#include "UtMath.hpp"

void HDD::PageHeader::SetLandingGear(const double aInput)
{
   mData.landingGearNose  = aInput;
   mData.landingGearLeft  = aInput;
   mData.landingGearRight = aInput;
}

void HDD::PageHeader::SetThrustThrottle(const double aThrust, const bool aThrottleValid, const double aThrottle)
{
   mData.thrustNormalized = aThrust;
   mData.throttleValid    = aThrottleValid;
   if (mData.throttleValid)
   {
      mData.throttleInput = aThrottle;
   }
}

HDD::PageHeader::PageHeader(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr =
      new Overlay(("hdd_header" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
   aViewerPtr->AddOverlay(mOverlayPtr);
   SetPosition(aPosition);
   mOverlayPtr->SetVisible(true);
   mOverlayPtr->SetDrawFunction(std::bind(&PageHeader::Draw, this));
   mPageName = "HEADER";
}

void HDD::PageHeader::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // Draw the 12 box regions of the header from left to right
   DrawEngBox(vectorGroup, -0.457, 0.0, 0.08, 0.08);
   DrawFuelBox(vectorGroup, -0.374, 0.0, 0.08, 0.08);
   DrawGunBox(vectorGroup, -0.291, 0.0, 0.08, 0.08);
   DrawLandingGearBox(vectorGroup, -0.208, 0.0, 0.08, 0.08);
   DrawIcawsBox(vectorGroup, -0.125, 0.0, 0.08, 0.08);
   // DrawBox(vectorGroup, -0.042, 0.0, 0.08, 0.08); // This is an unused region for now.
   DrawCommsBox(vectorGroup, 0.042, 0.0, 0.08, 0.08);
   DrawNavBox(vectorGroup, 0.125, 0.0, 0.08, 0.08);
   DrawAdfBox(vectorGroup, 0.208, 0.0, 0.08, 0.08);
   DrawMenuBox(vectorGroup, 0.291, 0.0, 0.08, 0.08);
   DrawFlightDataBox(vectorGroup, 0.374, 0.0, 0.08, 0.08);
   DrawTimeBox(vectorGroup, 0.457, 0.0, 0.08, 0.08);

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageHeader::DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                              const double             aX,
                              const double             aY,
                              const double             aW,
                              const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Bar rect
   osg::ref_ptr<osg::Vec2Array> barArray(new osg::Vec2Array);
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY + (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX + (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   barArray->push_back(osg::Vec2d((aX - (aW * 0.5)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(barArray, GL_LINE_LOOP, Color::Green).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawEngBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH)
{
   double w      = static_cast<float>(mOverlayPtr->GetWidth());
   double h      = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;
   double radius = 0.03;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   mOverlayPtr->SetLineWidthMultiplier(6.0f);
   // Green ring for 270 degrees
   osg::ref_ptr<osg::Vec2Array> greenArray(new osg::Vec2Array);
   for (int ang_deg = 90; ang_deg >= -180; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = radius * cos(ang_rad);
      double y       = radius * sin(ang_rad);
      greenArray->push_back(osg::Vec2d(x * w, y * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(greenArray, GL_LINE_STRIP, Color::Green).get());

   // Red ring for 60 degrees
   osg::ref_ptr<osg::Vec2Array> redArray(new osg::Vec2Array);
   for (int ang_deg = -180; ang_deg >= -240; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = radius * cos(ang_rad);
      double y       = radius * sin(ang_rad);
      redArray->push_back(osg::Vec2d(x * w, y * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(redArray, GL_LINE_STRIP, Color::Red).get());

   // Thrust needle, slightly smaller radius than the ring size
   double needleAng_deg = 0.0;
   if (mData.thrustNormalized >= 1.0)
   {
      double abThrustPos = mData.thrustNormalized - 1.0;
      if (abThrustPos > 0.0)
      {
         // Using afterburner
         needleAng_deg = 270.0 + 60.0 * abThrustPos;
      }
      else
      {
         // No afterburner but full MIL
         needleAng_deg = 270.0;
      }
   }
   else
   {
      needleAng_deg = 270.0 * mData.thrustNormalized;
   }

   osg::ref_ptr<osg::MatrixTransform> needleXform(new osg::MatrixTransform());
   needleXform->postMult(osg::Matrix::rotate(osg::Quat(needleAng_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, -1))));
   osg::ref_ptr<osg::Vec2Array> needleArray(new osg::Vec2Array);
   needleArray->push_back(osg::Vec2d(0.0, 0.0));
   needleArray->push_back(osg::Vec2d(0.0, radius * 0.8 * h));

   // Set the line width for the needle
   mOverlayPtr->SetLineWidthMultiplier(3.0f);

   // If the needle is in the red region, use red, otherwise use green.
   osg::Vec4 needleColor = (needleAng_deg > 270.0) ? Color::Red : Color::Green;
   needleXform->addChild(mOverlayPtr->CreateGeometry(needleArray, GL_LINES, needleColor).get());

   if (mData.throttleValid)
   {
      double throttleAng_deg = 0.0;
      if (mData.throttleInput >= 1.0)
      {
         double abThrottlePos = mData.throttleInput - 1.0;
         if (abThrottlePos > 0.0)
         {
            // Using afterburner
            throttleAng_deg = 270.0 + 60.0 * abThrottlePos;
         }
         else
         {
            // No afterburner but full MIL
            throttleAng_deg = 270.0;
         }
      }
      else
      {
         throttleAng_deg = 270.0 * mData.throttleInput;
      }

      // Throttle input needle, goes on the outside of the ring
      osg::ref_ptr<osg::MatrixTransform> throttleXform(new osg::MatrixTransform());
      throttleXform->postMult(osg::Matrix::rotate(osg::Quat(throttleAng_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, -1))));
      osg::ref_ptr<osg::Vec2Array> throttleArray(new osg::Vec2Array);
      throttleArray->push_back(osg::Vec2d(0.0, radius * 0.925 * h));
      throttleArray->push_back(osg::Vec2d(0.0, radius * 1.2 * h));

      osg::Vec4 throttleColor = (throttleAng_deg > 270.0) ? Color::Red : Color::Green;
      throttleXform->addChild(mOverlayPtr->CreateGeometry(throttleArray, GL_LINES, throttleColor).get());

      vectorXform->addChild(throttleXform.get());
   }

   // Reset the line width
   mOverlayPtr->SetLineWidthMultiplier(1.0f);
   vectorXform->addChild(needleXform.get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawFuelBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   double fontSize = 0.175f * mOverlayPtr->GetHeight();

   // Draw our labels.
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("GW", -0.035 * w, 0.30 * mOverlayPtr->GetHeight(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("TOT", -0.035 * w, 0.10 * mOverlayPtr->GetHeight(), fontSize, Color::Green, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("INT", -0.035 * w, -0.10 * mOverlayPtr->GetHeight(), fontSize, Color::Green, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("EXT", -0.035 * w, -0.30 * mOverlayPtr->GetHeight(), fontSize, Color::Green, osgText::Text::LEFT_CENTER)
         .get());

   // If our fuel data is valid, fill it in, otherwise use N/A for values.
   if (mData.fuelValid)
   {
      std::stringstream weight;
      weight << std::fixed << std::setw(3) << std::setprecision(1) << (mData.weight / 1000.0);
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(weight.str(), 0.035 * w, 0.30 * mOverlayPtr->GetHeight(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      double            totFuelRemaining = (mData.intFuelRemaining + mData.extFuelRemaining) / 1000.0;
      std::stringstream totFuel;
      totFuel << std::fixed << std::setw(3) << std::setprecision(1) << totFuelRemaining;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(totFuel.str(), 0.035 * w, 0.10 * mOverlayPtr->GetHeight(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());

      std::stringstream intFuel;
      intFuel << std::fixed << std::setw(3) << std::setprecision(1) << (mData.intFuelRemaining / 1000.0);
      vectorXform->addChild(mOverlayPtr
                               ->AddTextItem(intFuel.str(),
                                             0.035 * w,
                                             -0.10 * mOverlayPtr->GetHeight(),
                                             fontSize,
                                             Color::Magenta,
                                             osgText::Text::RIGHT_CENTER)
                               .get());

      std::stringstream extFuel;
      extFuel << std::fixed << std::setw(3) << std::setprecision(1) << (mData.extFuelRemaining / 1000.0);
      vectorXform->addChild(mOverlayPtr
                               ->AddTextItem(extFuel.str(),
                                             0.035 * w,
                                             -0.30 * mOverlayPtr->GetHeight(),
                                             fontSize,
                                             Color::Magenta,
                                             osgText::Text::RIGHT_CENTER)
                               .get());
   }
   else
   {
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("N/A", 0.035 * w, 0.30 * mOverlayPtr->GetHeight(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("N/A", 0.035 * w, 0.10 * mOverlayPtr->GetHeight(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("N/A", 0.035 * w, -0.10 * mOverlayPtr->GetHeight(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("N/A", 0.035 * w, -0.30 * mOverlayPtr->GetHeight(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawGunBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   double fontSize = 0.175f * mOverlayPtr->GetHeight();

   // One of 4 possible slots in the box
   const double pos[] = {0.3, 0.1, -0.1, -0.3};
   size_t       index = 0;
   for (auto&& weapon : mData.weapons)
   {
      std::string weaponName = weapon.first;
      for (auto& c : weaponName)
         c = toupper(c);
      osg::Vec4 tempColor = (weapon.first.compare(mData.selectedWeapon) == 0) ? Color::White : Color::Green;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(weaponName, -0.035 * w, pos[index] * mOverlayPtr->GetHeight(), fontSize, tempColor, osgText::Text::LEFT_CENTER)
            .get());
      std::stringstream str;
      str << std::setw(3) << weapon.second;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(str.str(), 0.035 * w, pos[index] * mOverlayPtr->GetHeight(), fontSize, tempColor, osgText::Text::RIGHT_CENTER)
            .get());

      // Break if we've hit 4 weapons.
      if (index > 3)
      {
         break;
      }
      index++;
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawLandingGearBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                         const double             aX,
                                         const double             aY,
                                         const double             aW,
                                         const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Draw the landing gear boxes.
   mOverlayPtr->SetLineWidthMultiplier(2.0f);

   // Nose gear
   osg::Vec4 tempColor = (mData.landingGearNose >= 0.99 || mData.landingGearNose <= 0.01) ? Color::Green : Color::Red;
   osg::ref_ptr<osg::Vec2Array> noseArray(new osg::Vec2Array);
   noseArray->push_back(osg::Vec2d((aX - (aW * 0.08)) * w, (aY + (aH * 0.08) - 0.005) * h));
   noseArray->push_back(osg::Vec2d((aX + (aW * 0.08)) * w, (aY + (aH * 0.08) - 0.005) * h));
   noseArray->push_back(osg::Vec2d((aX + (aW * 0.08)) * w, (aY - (aH * 0.08) - 0.005) * h));
   noseArray->push_back(osg::Vec2d((aX - (aW * 0.08)) * w, (aY - (aH * 0.08) - 0.005) * h));
   vectorXform->addChild(
      mOverlayPtr->CreateGeometry(noseArray, (mData.landingGearNose <= 0.01) ? GL_LINE_LOOP : GL_POLYGON, tempColor).get());

   // Left gear
   tempColor = (mData.landingGearLeft >= 0.99 || mData.landingGearLeft <= 0.01) ? Color::Green : Color::Red;
   osg::ref_ptr<osg::Vec2Array> leftArray(new osg::Vec2Array);
   leftArray->push_back(osg::Vec2d((aX - (aW * 0.08) - 0.016) * w, (aY + (aH * 0.08) - 0.024) * h));
   leftArray->push_back(osg::Vec2d((aX + (aW * 0.08) - 0.016) * w, (aY + (aH * 0.08) - 0.024) * h));
   leftArray->push_back(osg::Vec2d((aX + (aW * 0.08) - 0.016) * w, (aY - (aH * 0.08) - 0.024) * h));
   leftArray->push_back(osg::Vec2d((aX - (aW * 0.08) - 0.016) * w, (aY - (aH * 0.08) - 0.024) * h));
   vectorXform->addChild(
      mOverlayPtr->CreateGeometry(leftArray, (mData.landingGearLeft <= 0.01) ? GL_LINE_LOOP : GL_POLYGON, tempColor).get());

   // Right gear
   tempColor = (mData.landingGearRight >= 0.99 || mData.landingGearRight <= 0.01) ? Color::Green : Color::Red;
   osg::ref_ptr<osg::Vec2Array> rightArray(new osg::Vec2Array);
   rightArray->push_back(osg::Vec2d((aX - (aW * 0.08) + 0.016) * w, (aY + (aH * 0.08) - 0.024) * h));
   rightArray->push_back(osg::Vec2d((aX + (aW * 0.08) + 0.016) * w, (aY + (aH * 0.08) - 0.024) * h));
   rightArray->push_back(osg::Vec2d((aX + (aW * 0.08) + 0.016) * w, (aY - (aH * 0.08) - 0.024) * h));
   rightArray->push_back(osg::Vec2d((aX - (aW * 0.08) + 0.016) * w, (aY - (aH * 0.08) - 0.024) * h));
   vectorXform->addChild(
      mOverlayPtr->CreateGeometry(rightArray, (mData.landingGearRight <= 0.01) ? GL_LINE_LOOP : GL_POLYGON, tempColor).get());

   // Landing Gear Label
   mOverlayPtr->SetLineWidthMultiplier(1.0f);
   double fontSize = 0.15f * mOverlayPtr->GetHeight();
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("LANDING", aX * w, 0.35 * mOverlayPtr->GetHeight(), fontSize, Color::Green, osgText::Text::CENTER_CENTER)
         .get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("GEAR", aX * w, 0.20 * mOverlayPtr->GetHeight(), fontSize, Color::Green, osgText::Text::CENTER_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawIcawsBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // If any of the appropriate warnings are true, draw them.
   // Otherwise, just draw an ICAWS label.
   if (mData.masterCaution || mData.masterWarning || mData.stallWarning)
   {
      double fontSize  = 0.2f * mOverlayPtr->GetHeight();
      double blinkTime = round(mData.simTime * 2) / 2.0; // Get time an increment of 0.5 for fast blinking.
      if (mData.masterCaution && mData.simTime > blinkTime)
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("CAUTION", aX * w, (aY - 0.025) * h, fontSize, Color::Yellow, osgText::Text::CENTER_CENTER)
               .get());
      }
      if (mData.stallWarning && mData.simTime > blinkTime)
      {
         vectorXform->addChild(
            mOverlayPtr->AddTextItem("STALL", aX * w, aY * h, fontSize, Color::Green, osgText::Text::CENTER_CENTER).get());
      }

      if (mData.masterWarning && mData.simTime > blinkTime)
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("WARNING", aX * w, (aY + 0.025) * h, fontSize, Color::Red, osgText::Text::CENTER_CENTER)
               .get());
      }
   }
   else
   {
      double fontSize = 0.3f * mOverlayPtr->GetHeight();

      vectorXform->addChild(
         mOverlayPtr->AddTextItem("ICAWS", aX * w, aY * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawCommsBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.2f * mOverlayPtr->GetHeight();
   // Top frequency label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("128.250", aX * w, (aY - 0.03) * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Bottom frequency label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("128.300", aX * w, (aY + 0.03) * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Com label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("COM", aX * w, aY * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Draw the circle around the Com label
   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg >= -360; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = 0.02 * cos(ang_rad);
      double y       = 0.02 * sin(ang_rad);
      circleArray->push_back(osg::Vec2d((aX + x) * w, (aY + y) * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_STRIP, Color::Cyan).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawNavBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.2f * mOverlayPtr->GetHeight();
   // Top frequency label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("113.700", aX * w, (aY - 0.03) * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Bottom frequency label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("117.200", aX * w, (aY + 0.03) * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Nav label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("NAV", aX * w, aY * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Draw the circle around the Nav label
   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg >= -360; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = 0.02 * cos(ang_rad);
      double y       = 0.02 * sin(ang_rad);
      circleArray->push_back(osg::Vec2d((aX + x) * w, (aY + y) * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_STRIP, Color::Cyan).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawAdfBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double labelFontSize = 0.225f * mOverlayPtr->GetHeight();
   double numFontSize   = 0.175f * mOverlayPtr->GetHeight();

   // ADF label
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("ADF", aX * w, (aY + 0.03) * h, labelFontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("284.0", aX * w, (aY + 0.0125) * h, numFontSize, Color::Cyan, osgText::Text::CENTER_CENTER)
         .get());

   // XPDR label
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("XPDR", aX * w, (aY - 0.0125) * h, labelFontSize, Color::Cyan, osgText::Text::CENTER_CENTER)
         .get());
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("1200", aX * w, (aY - 0.03) * h, numFontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawMenuBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY + (aH * 0.4)) * h));
   boxArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY + (aH * 0.4)) * h));
   boxArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY - (aH * 0.4)) * h));
   boxArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY - (aH * 0.4)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, Color::Gray).get());

   // Horizontal lines
   osg::ref_ptr<osg::Vec2Array> hLinesArray(new osg::Vec2Array);
   hLinesArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY + (aH * 0.24)) * h));
   hLinesArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY + (aH * 0.24)) * h));
   hLinesArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY + (aH * 0.08)) * h));
   hLinesArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY + (aH * 0.08)) * h));
   hLinesArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY - (aH * 0.08)) * h));
   hLinesArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY - (aH * 0.08)) * h));
   hLinesArray->push_back(osg::Vec2d((aX - (aW * 0.4)) * w, (aY - (aH * 0.24)) * h));
   hLinesArray->push_back(osg::Vec2d((aX + (aW * 0.4)) * w, (aY - (aH * 0.24)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(hLinesArray, GL_LINES, Color::Gray).get());

   // Vertical lines
   osg::ref_ptr<osg::Vec2Array> vLinesArray(new osg::Vec2Array);
   vLinesArray->push_back(osg::Vec2d((aX + (aW * 0.24)) * w, (aY - (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX + (aW * 0.24)) * w, (aY + (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX + (aW * 0.08)) * w, (aY - (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX + (aW * 0.08)) * w, (aY + (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX - (aW * 0.08)) * w, (aY - (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX - (aW * 0.08)) * w, (aY + (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX - (aW * 0.24)) * w, (aY - (aH * 0.4)) * h));
   vLinesArray->push_back(osg::Vec2d((aX - (aW * 0.24)) * w, (aY + (aH * 0.4)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(vLinesArray, GL_LINES, Color::Gray).get());

   // Menu label
   double fontSize = 0.3f * mOverlayPtr->GetHeight();
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("MENU", aX * w, aY * h, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   // Make sure the text draws on top of the polygons
   osg::StateSet* ss = vectorXform->getOrCreateStateSet();
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawFlightDataBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                        const double             aX,
                                        const double             aY,
                                        const double             aW,
                                        const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   double fontSize = 0.175f * mOverlayPtr->GetHeight();

   // Altitude
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("ALT", -0.04 * w, 0.3 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream alt;
   alt << std::setw(6) << std::setprecision(0) << static_cast<size_t>(mData.altBaro_ft);
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(alt.str(), 0.04 * w, 0.3 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
         .get());

   // Heading text
   double heading_deg = mData.heading_deg;

   if (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   if (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   int hdg_deg = (int)(heading_deg + 0.5);
   if (hdg_deg == 0)
   {
      hdg_deg = 360.0;
   }
   if (hdg_deg > 360)
   {
      hdg_deg -= 360.0;
   }

   // Heading
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("HDG", -0.04 * w, 0.0 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream hdg;
   hdg << std::setfill('0') << std::setw(3) << hdg_deg;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(hdg.str(), 0.04 * w, 0.0 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
         .get());

   // Speed
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("SPD", -0.04 * w, -0.3 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream spd;
   spd << std::setw(4) << std::setprecision(0) << static_cast<size_t>(mData.ktas);
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(spd.str(), 0.04 * w, -0.3 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::DrawTimeBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth());

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.175f * mOverlayPtr->GetHeight();

   // Time label
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("TIME", aX * w, 0.125 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::CENTER_CENTER)
         .get());
   // Time value label
   UtCalendar c = UtCalendar();
   c.AdvanceTimeBy(12 * 3600);
   c.AdvanceTimeBy(mData.simTime);
   QTime       time(c.GetHour(), c.GetMinute(), c.GetSecond());
   std::string timestr = time.toString(Qt::ISODate).toStdString();
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(timestr, aX * w, -0.125 * mOverlayPtr->GetHeight(), fontSize, Color::Cyan, osgText::Text::CENTER_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageHeader::SetFuelData(const HDD::SimData::FuelData& aData)
{
   mData.fuelValid = aData.fuelValid;
   // Don't bother setting data if fuel isn't valid
   if (mData.fuelValid)
   {
      mData.intFuelRemaining = aData.fuelInternal_lbs;
      mData.extFuelRemaining = aData.fuelExternal_lbs;
      mData.weight           = aData.grossWgt_lbs;
   }
}

void HDD::PageHeader::SetWeaponData(const HDD::SimData::WeaponDataSummary& aData)
{
   // Weapon Box
   for (auto w : aData.weaponList)
   {
      mData.weapons[w.weaponName] = w.quantity;
   }
   mData.selectedWeapon = aData.selectedWeapon;

   // ICAWS Box
   mData.masterWarning = aData.masterWarning;
   mData.masterCaution = aData.masterCaution;
}
