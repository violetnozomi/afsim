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

#include "WkfHDD_PageTWD.hpp"

#include <iomanip>
#include <sstream>

#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"

HDD::PageTWD::PageTWD(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_twd" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageTWD::Draw, this));
   mPageName = "TWD";
}

void HDD::PageTWD::SetCountermeasures(const int aNumChaff, const int aNumFlares, const int aNumDecoys)
{
   mData.numChaff  = aNumChaff;
   mData.numFlares = aNumFlares;
   mData.numDecoys = aNumDecoys;
}

void HDD::PageTWD::Set_ESM_RWR_TrackData(const std::vector<SimData::TrackProcessor>& aData,
                                         const std::string                           aESM_TrackProcessor)
{
   mData.tracks.clear();
   for (auto trackProcessor : aData)
   {
      // Handle ESM/RWR tracks if we have an ESM track processor
      if (trackProcessor.trackProcessorName == aESM_TrackProcessor)
      {
         // Add any tracks from an ESM/RWR processor to our track list if it has a valid bearing
         for (auto track : trackProcessor.trackList.mTracks)
         {
            if (track.second.bearingValid)
            {
               mData.tracks.emplace(track.first, track.second);
            }
         }
      }
   }
}

void HDD::PageTWD::Draw()
{
   double scale = 1.0;

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      DrawCompass(vectorGroup, 0.0, 0.0, 0.4);
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
      DrawCompass(vectorGroup, 0.0, 0.0, 0.4);
      DrawHeadingBox(vectorGroup, 0.0, 0.49, 0.1, 0.07, scale);
      DrawCountermeasures(vectorGroup, 0.0, -0.6, 0.9, 0.1, scale);
   }
   else if (mSize == Page::Large)
   {
      mCompassY = 0.14;

      DrawCompass(vectorGroup, mCompassX, mCompassY, mCompassRadius);
      DrawHeadingBox(vectorGroup, 0.0, 0.64, 0.1, 0.07, scale);
      DrawCountermeasures(vectorGroup, 0.0, -0.65, 0.9, 0.1, scale);
   }
   else if (mSize == Page::Full)
   {
      mCompassY      = 0.05;
      mCompassRadius = 0.3;

      scale = 0.5;
      DrawCompass(vectorGroup, mCompassX, mCompassY, mCompassRadius);
      DrawHeadingBox(vectorGroup, 0.0, 0.42, 0.05, 0.035, scale);
      DrawCountermeasures(vectorGroup, 0.0, -0.65, 1.9, 0.1, scale);
   }

   for (auto track : mData.tracks)
   {
      DrawTrack(vectorGroup, track.second, mCompassRadius, mCompassX, mCompassY, (mCompassRadius * 0.2), (mCompassRadius * 0.2));
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageTWD::DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Draw the three larger rings
   for (int r = 0; r < 3; r++)
   {
      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = ((aR * 0.25) + ((aR * 0.375) * r)) * cos(ang_rad);
         double y       = ((aR * 0.25) + ((aR * 0.375) * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      vectorXform->addChild(mOverlayPtr->CreateGeometry(ringArray, GL_LINE_LOOP, Color::White).get());
   }

   // Orient based on the heading
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorHeadingXForm(new osg::PositionAttitudeTransform());
   vectorHeadingXForm->setAttitude(osg::Quat((mData.heading_deg * UtMath::cRAD_PER_DEG), osg::Vec3(0.0f, 0.0f, 1.0f)));

   // Draw the 4 lines (two horizontal, two vertical) going
   // from the first large ring to the third ring.
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aR * 0.25) * pW(), 0.0));
   linesArray->push_back(osg::Vec2d((aR * 1.1) * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(-(aR * 0.25) * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(-(aR * 1.1) * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(0.0, (aR * 0.25) * pH()));
   linesArray->push_back(osg::Vec2d(0.0, (aR * 1.1) * pH()));
   linesArray->push_back(osg::Vec2d(0.0, -(aR * 0.25) * pH()));
   linesArray->push_back(osg::Vec2d(0.0, -(aR * 1.1) * pH()));
   vectorHeadingXForm->addChild(mOverlayPtr->CreateGeometry(linesArray, GL_LINES, Color::White).get());

   // Parts that should rotate with the heading add to vectorXForm.
   // Ticks every 10 degrees around the large ring.
   osg::ref_ptr<osg::Vec2Array> angleTicksArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 15)
   {
      double tickLength = (aR * 0.125);
      if (ang_deg == 45 || ang_deg == 135 || ang_deg == 225 || ang_deg == 315)
      {
         tickLength = (aR * 0.25);
      }
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad);
      double x2      = (aR - tickLength) * cos(ang_rad);
      double y       = aR * sin(ang_rad);
      double y2      = (aR - tickLength) * sin(ang_rad);
      angleTicksArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      angleTicksArray->push_back(osg::Vec2d(x2 * pW(), y2 * pH()));
   }
   vectorHeadingXForm->addChild(mOverlayPtr->CreateGeometry(angleTicksArray, GL_LINES, Color::Gray).get());
   vectorXform->addChild(vectorHeadingXForm);
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTWD::DrawHeadingBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH,
                                  const double             aScale)
{
   float fontSize = 0.045f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Heading box rect
   osg::ref_ptr<osg::Vec2Array> headingBoxArray(new osg::Vec2Array);
   headingBoxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   headingBoxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   headingBoxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   headingBoxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   // Save the pointer to the box polygon here so we can assure the heading text gets drawn over it.
   osg::ref_ptr<osg::Geode> box = mOverlayPtr->CreateGeometry(headingBoxArray, GL_POLYGON, Color::Black).get();
   vectorXform->addChild(box);
   vectorXform->addChild(mOverlayPtr->CreateGeometry(headingBoxArray, GL_LINE_LOOP, Color::White).get());

   // Heading
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

   // Draw the heading
   std::stringstream heading;
   heading << std::setw(3) << std::setfill('0') << hdg_deg;
   box->addChild(
      mOverlayPtr->AddTextItem(heading.str(), 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTWD::DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
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

   // Chaff
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("CHAFF", -(aW * 0.5) * w, 0.05 * h, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream chaff;
   chaff << std::setw(3) << std::setfill('0') << mData.numChaff;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(chaff.str(), -((aW * 0.5) - 0.275) * w, 0.05 * h, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
         .get());

   // Decoys
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("DECOYS", -(aW * 0.5) * w, 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream decoys;
   decoys << std::setw(3) << std::setfill('0') << mData.numDecoys;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(decoys.str(), -((aW * 0.5) - 0.275) * w, 0.0, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
         .get());

   // Flares
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("FLARES", ((aW * 0.5) - 0.275) * w, 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream flares;
   flares << std::setw(3) << std::setfill('0') << mData.numFlares;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(flares.str(), (aW * 0.5) * w, 0.0, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTWD::DrawTrack(osg::ref_ptr<osg::Group>             aVectorGroup,
                             const HDD::SimData::TargetTrackItem& aTrack,
                             const double                         aR,
                             const double                         aX,
                             const double                         aY,
                             const double                         aW,
                             const double                         aH)
{
   // Position to match the compass
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Calculate the position and orientation of the entity
   double az_rad = aTrack.tgtBearing_rad - (mData.heading_deg * UtMath::cRAD_PER_DEG);
   if (az_rad < -UtMath::cPI)
   {
      az_rad += UtMath::cTWO_PI;
   }
   if (az_rad > UtMath::cPI)
   {
      az_rad -= UtMath::cTWO_PI;
   }

   // Position the drawing in the center of the last ring.
   double tgtX = (aR * 0.8125) * sin(az_rad);
   double tgtY = (aR * 0.8125) * cos(az_rad);

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> entityXform(new osg::PositionAttitudeTransform());
   entityXform->setPosition(osg::Vec3d(tgtX * pW(), tgtY * pH(), 0.0));

   osg::Vec4 iconColor = Color::Yellow; // Assume bogie (unknown)
   if (aTrack.identification == SimData::ID_BANDIT)
   {
      iconColor = Color::Red;
   }
   else if (aTrack.identification == SimData::ID_FRIENDLY)
   {
      iconColor = Color::Green;
   }
   else if (aTrack.identification == SimData::ID_NEUTRAL)
   {
      iconColor = Color::White;
   }

   // Draw a square symbol by default, but draw a diamond for a missile
   if (aTrack.isMissile)
   {
      // Entity diamond
      osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
      entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
      entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
      entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
      entityXform->addChild(mOverlayPtr->CreateGeometry(entityArray, GL_LINE_LOOP, iconColor).get());

      float       fontSize = 0.045f * pW();
      std::string id       = GetIDString(aTrack);
      entityXform->addChild(
         mOverlayPtr->AddTextItem(id, 0.0, 0.0, fontSize, iconColor, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      // Entity square
      osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
      entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
      entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
      entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      entityXform->addChild(mOverlayPtr->CreateGeometry(entityArray, GL_LINE_LOOP, iconColor).get());

      float       fontSize = 0.045f * pW();
      std::string id       = GetIDString(aTrack);
      entityXform->addChild(
         mOverlayPtr->AddTextItem(id, 0.0, 0.0, fontSize, iconColor, osgText::Text::CENTER_CENTER).get());
   }

   vectorXform->addChild(entityXform.get());
   aVectorGroup->addChild(vectorXform.get());
}

const std::string HDD::PageTWD::GetIDString(const HDD::SimData::TargetTrackItem& aTrack)
{
   // Prioritize: Missile -> Type -> Side -> IFF -> Aux
   if (aTrack.isMissile)
   {
      // aTrack.tgtIdString should have a missile value, use it to look up a mapping
      return "M";
   }
   else if (aTrack.idTypeValid)
   {
      // aTrack.tgtIdString should be the type of the platform, use it to look up a mapping
   }
   else if (aTrack.idSideValid)
   {
      // aTrack.tgtIdString should have a side value, use it to look up a mapping
   }
   else if (aTrack.idIffValid)
   {
      // aTrack.tgtIdString should have an IFF value (friend, foe, unknown)
   }
   else if (aTrack.idAuxValid)
   {
      // aTrack.tgtIdString should have an aux data value, use it to look up a mapping
   }
   return "U";
}
