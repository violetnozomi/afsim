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

#include "WkfHDD_PageSMS.hpp"

#include <iomanip>
#include <sstream>

#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"

HDD::PageSMS::PageSMS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_sms" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageSMS::Draw, this));
   mPageName = "SMS";
}

void HDD::PageSMS::SetWeaponData(const HDD::SimData::WeaponDataSummary& aData)
{
   mData.weaponList         = aData.weaponList;
   mData.selectedWeapon     = aData.selectedWeapon;
   mData.numSelectedWeapon  = aData.numSelectedWeapon;
   mData.engagedTargetList  = aData.engagedTargetList;
   mData.numChaff           = aData.numChaff;
   mData.numFlares          = aData.numFlares;
   mData.numDecoys          = aData.numDecoys;
   mData.weaponBayDoorsOpen = aData.weaponBayDoorsOpen;
   mData.supportingWeapon   = aData.supportingWeapon;
   mData.masterWarning      = aData.masterWarning;
   mData.masterCaution      = aData.masterCaution;
   mData.shootCueActive     = aData.shootCueActive;
   mData.shootCueBlink      = aData.shootCueBlink;
   mData.masterArmActive    = aData.masterArmActive;
   mData.jammingDetected    = aData.jammingDetected;

   // The first time we get weapon data, configure our page.
   if (!mConfigured && !aData.weaponList.empty())
   {
      size_t weaponTotal      = 0;
      size_t currentSlotIndex = 1;
      // For each weapon, loop through checking for bombs, srms, and mrms.
      // Load bombs into the aircraft first, then srms, then mrms, starting
      // from the center of the aircraft outwards.  Unknown weapon types will
      // be counted as mrms.  The slot index given maps to the correct position
      // using the GetWeaponLocation function.
      std::vector<HDD::SimData::WeaponNameQty> bombs;
      std::vector<HDD::SimData::WeaponNameQty> srms;
      std::vector<HDD::SimData::WeaponNameQty> mrms;

      // Populate our lists.
      for (const auto& w : mData.weaponList)
      {
         QString weaponName = QString::fromStdString(w.weaponName);
         // Skip gun
         if (weaponName.contains("gun", Qt::CaseInsensitive))
         {
            continue;
         }
         if (weaponName.contains("bomb", Qt::CaseInsensitive) || weaponName.contains("agm", Qt::CaseInsensitive))
         {
            bombs.push_back(w);
         }
         else if (weaponName.contains("srm", Qt::CaseInsensitive) || weaponName.contains("fox2", Qt::CaseInsensitive) ||
                  weaponName.contains("srv", Qt::CaseInsensitive))
         {
            srms.push_back(w);
         }
         else
         {
            mrms.push_back(w);
         }
      }

      // Then prioritize loading bombs, srms, and mrms.
      for (const auto& w : bombs)
      {
         weaponTotal += w.quantity;

         SMS_Weapon tmpWeapon;
         for (size_t i = 0; i < w.quantity; i++)
         {
            tmpWeapon.weaponSlots.emplace(currentSlotIndex++, true);
         }
         mWeapons.emplace(w.weaponName, tmpWeapon);
      }

      for (const auto& w : srms)
      {
         weaponTotal += w.quantity;

         SMS_Weapon tmpWeapon;
         for (size_t i = 0; i < w.quantity; i++)
         {
            tmpWeapon.weaponSlots.emplace(currentSlotIndex++, true);
         }
         mWeapons.emplace(w.weaponName, tmpWeapon);
      }

      for (const auto& w : mrms)
      {
         weaponTotal += w.quantity;

         SMS_Weapon tmpWeapon;
         for (size_t i = 0; i < w.quantity; i++)
         {
            tmpWeapon.weaponSlots.emplace(currentSlotIndex++, true);
         }
         mWeapons.emplace(w.weaponName, tmpWeapon);
      }

      mUseTwoRows = (weaponTotal > 8);
      mConfigured = true;
   }
   // If our weapons are configured, update and handle if our quantities have changed.
   else
   {
      // Set our selected weapon index for drawing (defaults to 0 if no weapon selected)
      SetSelectedWeaponIndex(mData.selectedWeapon);

      // Update our weapons that are drawing here.  Match our slot visibility to the current quantity.
      // Make sure that we handle time reversing in RV here.
      for (const auto& w : mData.weaponList)
      {
         if (mWeapons.find(w.weaponName) != mWeapons.end())
         {
            SMS_Weapon& smsWeapon = mWeapons.at(w.weaponName);

            smsWeapon.quantity = w.quantity;
            // Set all weapons visibility to false.
            for (auto& vis : smsWeapon.weaponSlots)
            {
               vis.second = false;
            }
            // From the end of the list, set visiblity for w.quantity towards the front of the list
            // This will ensure we are drawing the correct configuration.
            int wLeft = w.quantity;
            for (auto vis = smsWeapon.weaponSlots.rbegin(); vis != smsWeapon.weaponSlots.rend(); ++vis)
            {
               vis->second = true;
               if (--wLeft < 1)
               {
                  break;
               }
            }
         }
      }
   }
}

void HDD::PageSMS::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   double scale = 1.0;
   if (mSize == Page::Small)
   {
      DrawSelected(vectorGroup, 0.0, 0.45, 0.9, 0.15, scale);
      DrawArmed(vectorGroup, 0.0, 0.25, 0.9, 0.15, scale);
      DrawCountermeasures(vectorGroup, 0.0, 0.0, 0.9, 0.3, scale);
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
      // If we have <= 8 weapons.
      if (!mUseTwoRows)
      {
         DrawArmed(vectorGroup, 0.0, 0.575, 0.3, 0.1, scale);
         DrawSelected(vectorGroup, 0.0, 0.45, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, -0.2, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.325, 0.4, 0.1, scale);
      }
      // If we have > 8 weapons.
      else
      {
         DrawSelected(vectorGroup, 0.0, 0.6, 0.3, 0.1, scale);
         DrawArmed(vectorGroup, 0.325, 0.6, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, 0.0, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.425, 0.4, 0.1, scale);
      }
      DrawCountermeasuresAndDoorsOpen(vectorGroup, 0.0, -0.575, 0.9, 0.1, scale);
   }
   else if (mSize == Page::Large)
   {
      // If we have <= 8 weapons.
      if (!mUseTwoRows)
      {
         DrawArmed(vectorGroup, 0.0, 0.70, 0.3, 0.1, scale);
         DrawSelected(vectorGroup, 0.0, 0.575, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, -0.2, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.325, 0.4, 0.1, scale);
      }
      // If we have > 8 weapons.
      else
      {
         DrawSelected(vectorGroup, 0.0, 0.88125, 0.3, 0.1, scale);
         DrawArmed(vectorGroup, 0.325, 0.88125, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, 0.1, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.425, 0.4, 0.1, scale);
      }
      DrawCountermeasuresAndDoorsOpen(vectorGroup, 0.0, -0.65, 0.9, 0.1, scale);
   }
   else if (mSize == Page::Full)
   {
      scale = 0.5;
      // If we have <= 8 weapons.
      if (!mUseTwoRows)
      {
         DrawArmed(vectorGroup, 0.0, 0.70, 0.3, 0.1, scale);
         DrawSelected(vectorGroup, 0.0, 0.575, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, -0.2, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.325, 0.4, 0.1, scale);
      }
      // If we have > 8 weapons.
      else
      {
         DrawSelected(vectorGroup, 0.0, 0.75, 0.3, 0.1, scale);
         DrawArmed(vectorGroup, 0.75, 0.225, 0.3, 0.1, scale);
         DrawAllWeapons(vectorGroup, 0.0, 0.1, scale);
         DrawFuelTanks(vectorGroup, 0.0, -0.425, 0.4, 0.1, scale);
      }
      DrawCountermeasuresAndDoorsOpen(vectorGroup, 0.0, -0.65, 1.9, 0.1, scale);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageSMS::DrawWingLines(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Left wing array
   osg::ref_ptr<osg::Vec2Array> leftWingArray(new osg::Vec2Array);
   leftWingArray->push_back(osg::Vec2d(-(aW * 0.075) * w, (aH * 0.5) * h));
   leftWingArray->push_back(osg::Vec2d(-(aW * 0.075) * w, (aH * 0.1) * h));
   leftWingArray->push_back(osg::Vec2d(-(aW * 0.5) * w, -(aH * 0.5) * h));

   // Right wing array
   osg::ref_ptr<osg::Vec2Array> rightWingArray(new osg::Vec2Array);
   rightWingArray->push_back(osg::Vec2d((aW * 0.075) * w, (aH * 0.5) * h));
   rightWingArray->push_back(osg::Vec2d((aW * 0.075) * w, (aH * 0.1) * h));
   rightWingArray->push_back(osg::Vec2d((aW * 0.5) * w, -(aH * 0.5) * h));

   vectorXform->addChild(mOverlayPtr->CreateGeometry(leftWingArray, GL_LINE_STRIP, Color::Gray).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(rightWingArray, GL_LINE_STRIP, Color::Gray).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawFuelTanks(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const double             aScale)
{
   // If we have fuel tanks, draw up to a max of 3 of them.
   if (mData.extTankCount > 0)
   {
      double w      = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
      double h      = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;
      double radius = aH * 0.4;

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

      // Calculate the top bar width
      double                       lineWidth = aW - (radius * 2);
      osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
      lineArray->push_back(osg::Vec2d(-(lineWidth * 0.5) * w, (aH * 0.5) * h));
      lineArray->push_back(osg::Vec2d((lineWidth * 0.5) * w, (aH * 0.5) * h));

      // Draw the lines.  Always draw 3 potential slots.
      double lineSpacing = lineWidth / 2;
      for (size_t i = 0; i < 3; i++)
      {
         double xOffset = (-(lineWidth * 0.5) + (lineSpacing * i));
         // Line extending down to the circle
         lineArray->push_back(osg::Vec2d(xOffset * w, (aH * 0.5) * h));
         lineArray->push_back(osg::Vec2d(xOffset * w, ((aH * 0.5) - (aH * 0.2)) * h));
      }
      vectorXform->addChild(mOverlayPtr->CreateGeometry(lineArray, GL_LINES, Color::Gray).get());

      // For each fuel tank
      for (size_t i = 0; i < mData.extTankCount; i++)
      {
         // If we only have one tank put it in the middle, and
         // if we have two tanks, put one on the left and one on the right.
         if (mData.extTankCount == 1 || (mData.extTankCount == 2 && i == 1))
         {
            i++;
         }

         // If we've drawn 3 fuel tanks, we are done drawing.
         if (i >= 3)
         {
            break;
         }
         double xOffset = (-(lineWidth * 0.5) + (lineSpacing * i));
         // Create the fuel circle
         double                       yOffset = -(aH * 0.5) + radius;
         osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 20)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = radius * cos(ang_rad);
            double y       = radius * sin(ang_rad);
            circleArray->push_back(osg::Vec2d((x + xOffset) * w, (y + yOffset) * h));
         }
         auto circle = mOverlayPtr->CreateGeometry(circleArray, GL_POLYGON, Color::Green);
         vectorXform->addChild(circle.get());
         float fontSize = 0.045f * w;
         circle->addChild(
            mOverlayPtr->AddTextItem("F", xOffset * w, yOffset * h, fontSize, Color::Black, osgText::Text::CENTER_CENTER));
      }

      aVectorGroup->addChild(vectorXform.get());
   }
}

void HDD::PageSMS::DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                                       const double             aX,
                                       const double             aY,
                                       const double             aW,
                                       const double             aH,
                                       const double             aScale)
{
   // This is for the small display only.

   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   float fontSize = 0.09f * w;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Countermeasures
   // Chaff
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("CHAFF", -(aW * 0.5) * w, 0.1 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream chaff;
   chaff << std::setw(3) << std::setfill('0') << mData.numChaff;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(chaff.str(), (aW * 0.5) * w, 0.1 * h, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   // Decoys
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("DECOYS", -(aW * 0.5) * w, 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream decoys;
   decoys << std::setw(3) << std::setfill('0') << mData.numDecoys;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(decoys.str(), (aW * 0.5) * w, 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());

   // Flares
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("FLARES", -(aW * 0.5) * w, -0.1 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream flares;
   flares << std::setw(3) << std::setfill('0') << mData.numFlares;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(flares.str(), (aW * 0.5) * w, -0.1 * h, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawCountermeasuresAndDoorsOpen(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   const double             aX,
                                                   const double             aY,
                                                   const double             aW,
                                                   const double             aH,
                                                   const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   float fontSize = 0.045f * w;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Weapon Bay Doors.  First check if we have doors, otherwise don't draw this.
   // If we do have doors, draw if they are open or closed.
   if (mData.hasDoors)
   {
      // Doors label
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("DOORS", -(aW * 0.5) * w, 0.05 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());

      // Open or closed label
      if (mData.weaponBayDoorsOpen)
      {
         vectorXform->addChild(
            mOverlayPtr->AddTextItem("  OPEN", -(aW * 0.5) * w, 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
      }
      else
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("  CLOSED", -(aW * 0.5) * w, 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER)
               .get());
      }
   }

   // Countermeasures
   // Chaff
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("CHAFF", ((aW * 0.5) - 0.275) * w, 0.05 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream chaff;
   chaff << std::setw(3) << std::setfill('0') << mData.numChaff;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(chaff.str(), (aW * 0.5) * w, 0.05 * h, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   // Decoys
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("DECOYS", ((aW * 0.5) - 0.275) * w, 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream decoys;
   decoys << std::setw(3) << std::setfill('0') << mData.numDecoys;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(decoys.str(), (aW * 0.5) * w, 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());

   // Flares
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("FLARES", ((aW * 0.5) - 0.275) * w, -0.05 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream flares;
   flares << std::setw(3) << std::setfill('0') << mData.numFlares;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(flares.str(), (aW * 0.5) * w, -0.05 * h, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawWeaponBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const double             aScale)
{
   // This is a debugging function for seeing the weapon box (The space the weapon is drawn in).
   // This can be used to see how much space there is to work with when adding a new weapon type.
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, (aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * w, (aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * w, -(aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, -(aH * 0.5) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_POLYGON, osg::Vec4(0.2, 0.2, 0.2, 1.0)).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawWeaponBomb(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH,
                                  const size_t             aWeaponIndex,
                                  const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // If this weapon is selected, draw it in white rather than green.
   osg::Vec4 selectedColor = (mSelectedWeaponIndex == aWeaponIndex) ? Color::White : Color::Green;

   osg::ref_ptr<osg::Vec2Array> tubeArray(new osg::Vec2Array);
   tubeArray->push_back(osg::Vec2d((0.3 * aW) * w, (0.158 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.3 * aW) * w, -(0.106 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.0625 * aW) * w, -(0.41 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.0625 * aW) * w, -(0.41 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.3 * aW) * w, -(0.106 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.3 * aW) * w, (0.158 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.1125 * aW) * w, (0.354 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.05 * aW) * w, (0.368 * aH) * h));
   tubeArray->push_back(osg::Vec2d(0.0 * w, (0.382 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.05 * aW) * w, (0.368 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.1125 * aW) * w, (0.354 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> midBoxArray(new osg::Vec2Array);
   midBoxArray->push_back(osg::Vec2d((0.3375 * aW) * w, (0.158 * aH) * h));
   midBoxArray->push_back(osg::Vec2d((0.3375 * aW) * w, -(0.106 * aH) * h));
   midBoxArray->push_back(osg::Vec2d(-(0.3375 * aW) * w, -(0.106 * aH) * h));
   midBoxArray->push_back(osg::Vec2d(-(0.3375 * aW) * w, (0.158 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> bottomFinArray(new osg::Vec2Array);
   bottomFinArray->push_back(osg::Vec2d((0.1375 * aW) * w, -(0.316 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.3125 * aW) * w, -(0.344 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.3125 * aW) * w, -(0.394 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.3125 * aW) * w, -(0.394 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.3125 * aW) * w, -(0.344 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.1375 * aW) * w, -(0.316 * aH) * h));

   vectorXform->addChild(mOverlayPtr->CreateGeometry(tubeArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(midBoxArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(bottomFinArray, GL_POLYGON, selectedColor).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawWeaponMRM(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const size_t             aWeaponIndex,
                                 const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // If this weapon is selected, draw it in white rather than green.
   osg::Vec4 selectedColor = (mSelectedWeaponIndex == aWeaponIndex) ? Color::White : Color::Green;

   osg::ref_ptr<osg::Vec2Array> tubeArray(new osg::Vec2Array);
   tubeArray->push_back(osg::Vec2d((0.06875 * aW) * w, (0.422 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.06875 * aW) * w, -(0.422 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.06875 * aW) * w, -(0.422 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.06875 * aW) * w, (0.422 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.05625 * aW) * w, (0.460 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.025 * aW) * w, (0.474 * aH) * h));
   tubeArray->push_back(osg::Vec2d(0.0 * w, (0.486 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.025 * aW) * w, (0.474 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.05625 * aW) * w, (0.460 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> midFinArray(new osg::Vec2Array);
   midFinArray->push_back(osg::Vec2d((0.1 * aW) * w, (0.04 * aH) * h));
   midFinArray->push_back(osg::Vec2d((0.26 * aW) * w, -(0.0064 * aH) * h));
   midFinArray->push_back(osg::Vec2d((0.26 * aW) * w, -(0.0492 * aH) * h));
   midFinArray->push_back(osg::Vec2d(-(0.26 * aW) * w, -(0.0492 * aH) * h));
   midFinArray->push_back(osg::Vec2d(-(0.26 * aW) * w, -(0.0064 * aH) * h));
   midFinArray->push_back(osg::Vec2d(-(0.1 * aW) * w, (0.04 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> bottomFinArray(new osg::Vec2Array);
   bottomFinArray->push_back(osg::Vec2d((0.1 * aW) * w, -(0.392 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.24 * aW) * w, -(0.448 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.24 * aW) * w, -(0.47 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.24 * aW) * w, -(0.47 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.24 * aW) * w, -(0.448 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.1 * aW) * w, -(0.392 * aH) * h));

   vectorXform->addChild(mOverlayPtr->CreateGeometry(tubeArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(midFinArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(bottomFinArray, GL_POLYGON, selectedColor).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawWeaponSRM(osg::ref_ptr<osg::Group> aVectorGroup,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const size_t             aWeaponIndex,
                                 const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // If this weapon is selected, draw it in white rather than green.
   osg::Vec4 selectedColor = (mSelectedWeaponIndex == aWeaponIndex) ? Color::White : Color::Green;

   osg::ref_ptr<osg::Vec2Array> tubeArray(new osg::Vec2Array);
   tubeArray->push_back(osg::Vec2d((0.035 * aW) * w, (0.31 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.035 * aW) * w, -(0.31 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.015 * aW) * w, -(0.32 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.0 * aW) * w, -(0.34 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.015 * aW) * w, -(0.32 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.035 * aW) * w, -(0.31 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.035 * aW) * w, (0.31 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.015 * aW) * w, (0.32 * aH) * h));
   tubeArray->push_back(osg::Vec2d((0.0 * aW) * w, (0.34 * aH) * h));
   tubeArray->push_back(osg::Vec2d(-(0.015 * aW) * w, (0.32 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> frontFinArray(new osg::Vec2Array);
   frontFinArray->push_back(osg::Vec2d((0.0 * aW) * w, (0.31 * aH) * h));
   frontFinArray->push_back(osg::Vec2d((0.04 * aW) * w, (0.29 * aH) * h));
   frontFinArray->push_back(osg::Vec2d((0.12 * aW) * w, (0.25 * aH) * h));
   frontFinArray->push_back(osg::Vec2d(-(0.12 * aW) * w, (0.25 * aH) * h));
   frontFinArray->push_back(osg::Vec2d(-(0.04 * aW) * w, (0.29 * aH) * h));
   frontFinArray->push_back(osg::Vec2d((0.0 * aW) * w, (0.31 * aH) * h));

   osg::ref_ptr<osg::Vec2Array> bottomFinArray(new osg::Vec2Array);
   bottomFinArray->push_back(osg::Vec2d((0.0 * aW) * w, -(0.26 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.12 * aW) * w, -(0.29 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.12 * aW) * w, -(0.30 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.12 * aW) * w, -(0.30 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d(-(0.12 * aW) * w, -(0.29 * aH) * h));
   bottomFinArray->push_back(osg::Vec2d((0.0 * aW) * w, -(0.26 * aH) * h));

   vectorXform->addChild(mOverlayPtr->CreateGeometry(tubeArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(frontFinArray, GL_POLYGON, selectedColor).get());
   vectorXform->addChild(mOverlayPtr->CreateGeometry(bottomFinArray, GL_POLYGON, selectedColor).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawAllWeapons(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale)
{
   // Draw our weapon configuration in full.
   // Draw the wing lines.
   DrawWingLines(aVectorGroup, aX, aY + 0.4, 0.7, 0.25, aScale);


   for (auto& weapon : mWeapons)
   {
      for (auto& w : weapon.second.weaponSlots)
      {
         if (w.second)
         {
            const QPointF pos        = GetWeaponLocation(w.first);
            QString       weaponName = QString::fromStdString(weapon.first);
            if (weaponName.contains("bomb", Qt::CaseInsensitive) || weaponName.contains("agm", Qt::CaseInsensitive))
            {
               DrawWeaponBomb(aVectorGroup, aX + pos.x(), aY + pos.y(), 0.08, 0.38, w.first, aScale);
            }
            else if (weaponName.contains("srm", Qt::CaseInsensitive) ||
                     weaponName.contains("fox2", Qt::CaseInsensitive) || weaponName.contains("srv", Qt::CaseInsensitive))
            {
               DrawWeaponSRM(aVectorGroup, aX + pos.x(), aY + pos.y(), 0.08, 0.38, w.first, aScale);
            }
            else
            {
               DrawWeaponMRM(aVectorGroup, aX + pos.x(), aY + pos.y(), 0.08, 0.38, w.first, aScale);
            }
         }
      }
   }
}

void HDD::PageSMS::DrawArmed(osg::ref_ptr<osg::Group> aVectorGroup,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH,
                             const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   float fontSize = 0.07f * w * ((mSize == Page::Small) ? 2.0 : 1.0); // If we're on the small page, double the font size.

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, (aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * w, (aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * w, -(aH * 0.5) * h));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, -(aH * 0.5) * h));

   // Set the border color for the master arm box.
   // While the weapon is in safe mode, set to cyan.
   // While armed, set to green.
   osg::Vec4 borderColor;
   if (mData.masterArmActive)
   {
      borderColor = Color::Green;
   }
   else
   {
      borderColor = Color::Cyan;
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, borderColor).get());

   // If our master arm switch is active, draw it.
   if (mData.masterArmActive)
   {
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("ARMED", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("SAFE", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageSMS::DrawSelected(osg::ref_ptr<osg::Group> aVectorGroup,
                                const double             aX,
                                const double             aY,
                                const double             aW,
                                const double             aH,
                                const double             aScale)
{
   // If we have a selected weapon, draw the selected box showing this.
   if (!mData.selectedWeapon.empty())
   {
      double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
      double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

      float fontSize =
         0.04f * w * ((mSize == Page::Small) ? 2.0 : 1.0); // If we're on the small page, double the font size.

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

      // Box rect
      osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
      boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, (aH * 0.5) * h));
      boxArray->push_back(osg::Vec2d((aW * 0.5) * w, (aH * 0.5) * h));
      boxArray->push_back(osg::Vec2d((aW * 0.5) * w, -(aH * 0.5) * h));
      boxArray->push_back(osg::Vec2d(-(aW * 0.5) * w, -(aH * 0.5) * h));

      // Set the border color for the selected weapon box.
      // While the weapon is in safe mode, set to cyan.
      // While armed, set to green.
      // If the selected weapon is out of ammo, set to red.
      osg::Vec4 borderColor;
      if (mData.masterArmActive)
      {
         borderColor = Color::Green;
      }
      else
      {
         borderColor = Color::Cyan;
      }
      if (mData.numSelectedWeapon <= 0)
      {
         borderColor = Color::Red;
      }
      vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, borderColor).get());

      std::string weaponName = mData.selectedWeapon;
      for (auto& c : weaponName)
         c = toupper(c);
      if (mSize == Page::Small)
      {
         vectorXform->addChild(
            mOverlayPtr->AddTextItem("SEL:", -(aW * 0.45) * w, 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(weaponName, (aW * 0.45) * w, 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }
      else
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("SELECTED:", 0.0, (aH * 0.35) * h, fontSize, Color::White, osgText::Text::CENTER_TOP)
               .get());
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(weaponName, 0.0, -(aH * 0.35) * h, fontSize, Color::White, osgText::Text::CENTER_BOTTOM)
               .get());
      }

      aVectorGroup->addChild(vectorXform.get());
   }
}

const QPointF HDD::PageSMS::GetWeaponLocation(const size_t aIndex) const
{
   // 8 slots
   // Prioritize loading from the middle outwards (x value increases as index gets higher)
   if (!mUseTwoRows)
   {
      switch (aIndex)
      {
      case 1: // Slot 4
         return QPointF(-0.05, 0.335);
      case 2: // Slot 5
         return QPointF(0.05, 0.335);
      case 3: // Slot 3
         return QPointF(-0.15, 0.275);
      case 4: // Slot 6
         return QPointF(0.15, 0.275);
      case 5: // Slot 2
         return QPointF(-0.25, 0.215);
      case 6: // Slot 7
         return QPointF(0.25, 0.215);
      case 7: // Slot 1
         return QPointF(-0.35, 0.155);
      case 8: // Slot 8
         return QPointF(0.35, 0.155);
      default:
         return QPointF(-1.0, -1.0);
      }
   }
   // 16 slots
   // We'll load in the middle of the top row and bottom row before expanding
   // e.g we'd fill slots 4,5,12,13 before going back to filling slots 3,6, then 11,14
   else
   {
      switch (aIndex)
      {
      case 1: // Slot 4
         return QPointF(-0.05, 0.335);
      case 2: // Slot 5
         return QPointF(0.05, 0.335);
      case 3: // Slot 12
         return QPointF(-0.05, -0.065);
      case 4: // Slot 13
         return QPointF(0.05, -0.065);
      case 5: // Slot 3
         return QPointF(-0.15, 0.275);
      case 6: // Slot 6
         return QPointF(-0.15, 0.275);
      case 7: // Slot 11
         return QPointF(-0.15, -0.125);
      case 8: // Slot 14
         return QPointF(0.15, -0.125);
      case 9: // Slot 2
         return QPointF(-0.25, 0.215);
      case 10: // Slot 7
         return QPointF(0.25, 0.215);
      case 11: // Slot 10
         return QPointF(-0.25, -0.185);
      case 12: // Slot 15
         return QPointF(0.25, -0.185);
      case 13: // Slot 1
         return QPointF(-0.35, 0.155);
      case 14: // Slot 8
         return QPointF(0.35, 0.155);
      case 15: // Slot 9
         return QPointF(-0.35, -0.245);
      case 16: // Slot 16
         return QPointF(0.35, -0.245);
      default:
         return QPointF(-1.0, -1.0);
      }
   }
}

void HDD::PageSMS::SetSelectedWeaponIndex(const std::string& aSelectedWeapon)
{
   // If we have a selected weapon, check the bomb, srm, and mrm list and grab
   // the first visible index.  That will be our selected weapon index.
   if (!aSelectedWeapon.empty())
   {
      for (auto& weapon : mWeapons)
      {
         // If we found the weapon in our weapon list
         if (mWeapons.find(aSelectedWeapon) != mWeapons.end())
         {
            // Get the first visible weapon.
            for (auto& w : weapon.second.weaponSlots)
            {
               // If the weapon is visible.
               if (w.second)
               {
                  mSelectedWeaponIndex = w.first;
                  break;
               }
            }
         }
      }
      return;
   }
   // If we made it here there isn't a weapon left to select.
   mSelectedWeaponIndex = 0;
}
