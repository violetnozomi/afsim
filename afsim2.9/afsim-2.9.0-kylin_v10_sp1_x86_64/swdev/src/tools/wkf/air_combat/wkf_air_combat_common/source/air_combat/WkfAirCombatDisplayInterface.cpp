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

#include "WkfAirCombatDisplayInterface.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QGLWidget>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>
#include <osg/LineWidth>
#include <osg/Texture2D>

#include "VaUtils.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
UtoColor ConvertUtColor(const UtColor& aColor, double aConversionFactor = 255)
{
   float r, g, b, a;
   aColor.Get(r, g, b, a);
   r *= aConversionFactor;
   g *= aConversionFactor;
   b *= aConversionFactor;
   a *= 255;

   return UtoColor(r, g, b, a);
}
} // namespace

wkf::AirCombatDisplayInterface::AirCombatDisplayInterface(wkf::AirCombatVisualizationPrefObject& aPrefObject)
   : QObject()
   , mPrefObjectPtr(&aPrefObject)
   , mBlinkTimer(new QTimer(this))
{
   mCallbacks.Add(wkf::Observer::ViewerDestroyed.Connect([&](vespa::VaViewer* aViewer) { ViewerDestroyedCB(aViewer); }));

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::FirstMeterChanged,
           this,
           [&](int aMeasurement) {
              ChangeMeterMeasurementAtLevel(1, static_cast<wkf::AirCombatDisplayInterface::MeterMeasurement>(aMeasurement));
           });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::SecondMeterChanged,
           this,
           [&](int aMeasurement) {
              ChangeMeterMeasurementAtLevel(2, static_cast<wkf::AirCombatDisplayInterface::MeterMeasurement>(aMeasurement));
           });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ThirdMeterChanged,
           this,
           [&](int aMeasurement) {
              ChangeMeterMeasurementAtLevel(3, static_cast<wkf::AirCombatDisplayInterface::MeterMeasurement>(aMeasurement));
           });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowStateDataChanged,
           this,
           [&](bool aShow) { ShowStateData(aShow); });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowWeaponsAccentChanged,
           this,
           [&](bool aShow) { ShowAccents(aShow, wkf::AirCombatDisplayInterface::AccentType::cWEAPONS); });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowFuelAccentChanged,
           this,
           [&](bool aShow) { ShowAccents(aShow, wkf::AirCombatDisplayInterface::AccentType::cFUELSTATE); });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowTransmissionAccentChanged,
           this,
           [&](bool aShow) { ShowAccents(aShow, wkf::AirCombatDisplayInterface::AccentType::cTRANSMISSION); });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowSignatureAccentChanged,
           this,
           [&](bool aShow) { ShowAccents(aShow, wkf::AirCombatDisplayInterface::AccentType::cSIGNATURE); });

   connect(mPrefObjectPtr,
           &wkf::AirCombatVisualizationPrefObject::ShowVisualizationChanged,
           this,
           [&](bool aShow) { SetVisibility(aShow); });

   // Change interaction/overlay colors when team color changes
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           [&]()
           {
              auto* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

              // Update interaction colors.
              for (auto& interactionPair : mInteractionMap)
              {
                 for (auto& interaction : interactionPair.second)
                 {
                    switch (interaction->GetType())
                    {
                    case InteractionType::cDETECTION:
                    {
                       UtoColor color =
                          ConvertUtColor(teamVisPrefs->GetTeamUtColor(interactionPair.first->GetSide()), 225);
                       interaction->SetColor(color);
                       break;
                    }
                    case InteractionType::cWEZ:
                    {
                       UtoColor color =
                          ConvertUtColor(teamVisPrefs->GetTeamUtColor(interactionPair.first->GetSide()), 155);
                       interaction->SetColor(color);
                       break;
                    }
                    case InteractionType::cBASE:
                    default:
                    {
                       break;
                    }
                    }
                 }
              }
           });

   // Initialize the timer
   connect(mBlinkTimer,
           &QTimer::timeout,
           [&]()
           {
              for (const auto& interactionPair : mInteractionMap)
              {
                 for (const auto& interaction : interactionPair.second)
                 {
                    if (mShowVisualization && interaction->IsBlinking())
                    {
                       interaction->ToggleVisibility();
                    }
                 }
              }
           });

   mBlinkTimer->start(500);
}

// Updates each attachment that corresponds to a platform in the given data container
void wkf::AirCombatDisplayInterface::Update(wkf::DataContainer& aDataContainer)
{
   if (mShowVisualization)
   {
      for (auto& dataObject : aDataContainer.GetEngagementDataList())
      {
         wkf::Platform* platform = vaEnv.GetStandardScenario()->FindPlatformByName(dataObject.platformData.mPlatformName);

         if (platform != nullptr)
         {
            // Update the decorator that corresponds with this data
            if (mDecoratorMap.find(platform) != mDecoratorMap.end())
            {
               mDecoratorMap.at(platform)->Update(dataObject);
            }

            // Update all interactions that correspond with this data
            if (mInteractionMap.find(platform) != mInteractionMap.end())
            {
               for (auto& interaction : mInteractionMap.at(platform))
               {
                  interaction->Update(dataObject);
               }
            }
         }
      }
   }

   for (auto& overlay : mOverlayMap)
   {
      overlay.second->UpdateData(aDataContainer.GetEngagementDataList());
   }
}

// Creates a new decorator for the given platform
void wkf::AirCombatDisplayInterface::CreateDecoratorAttachment(wkf::Platform& aPlatform)
{
   if (mDecoratorMap.find(&aPlatform) == mDecoratorMap.end())
   {
      auto newAttachment = ut::make_unique<AttachmentDecoratorObject>(aPlatform, "", *mPrefObjectPtr);
      newAttachment->SetVisibility(mShowVisualization);
      mDecoratorMap.emplace(&aPlatform, std::move(newAttachment));
   }
   else
   {
      auto newAttachment = new AttachmentDecoratorObject(aPlatform, "", *mPrefObjectPtr);
      newAttachment->SetVisibility(mShowVisualization);
      mDecoratorMap.at(&aPlatform).reset(newAttachment);
   }
}

// Creates a new interaction line given a source, a target, and a type
void wkf::AirCombatDisplayInterface::CreateLineAttachment(wkf::Platform&                                  aPlatform,
                                                          wkf::Platform&                                  aTarget,
                                                          wkf::AirCombatDisplayInterface::InteractionType aType)
{
   auto newAttachment = new AttachmentInteractionObject(aPlatform, aTarget, aType);
   newAttachment->SetVisibility(mShowVisualization);

   if (mInteractionMap.find(&aPlatform) != mInteractionMap.end())
   {
      mInteractionMap.at(&aPlatform).emplace_back(newAttachment);
   }
   else
   {
      std::vector<std::unique_ptr<AttachmentInteractionObject>> objectList;
      objectList.emplace_back(newAttachment);
      mInteractionMap.emplace(&aPlatform, std::move(objectList));
   }
}

void wkf::AirCombatDisplayInterface::CreateOverlay(wkf::Platform& aPlatform, vespa::VaViewer& aViewer)
{
   auto overlay = new AirCombatOverlay(aPlatform);
   aViewer.AddOverlay(*overlay);
   mOverlayMap.emplace(&aPlatform, aViewer.FindFirstOverlayOfType<wkf::AirCombatOverlay>());
}

void wkf::AirCombatDisplayInterface::RemoveOverlay(wkf::Platform& aSourcePlatform, vespa::VaViewer& aViewer)
{
   auto overlay = mOverlayMap.find(&aSourcePlatform);
   if (overlay != mOverlayMap.end())
   {
      aViewer.RemoveOverlay("AirCombatOverlay");
      mOverlayMap.erase(overlay);
   }
}

void wkf::AirCombatDisplayInterface::AddManualTargetToOverlay(wkf::Platform& aSourcePlatform, wkf::Platform& aTargetPlatform)
{
   auto overlay = mOverlayMap.find(&aSourcePlatform);
   if (overlay != mOverlayMap.end())
   {
      overlay->second->AddTarget(aTargetPlatform);
   }
}

void wkf::AirCombatDisplayInterface::RemoveManualTargetFromAllOverlays(wkf::Platform& aTargetPlatform)
{
   for (auto& overlay : mOverlayMap)
   {
      overlay.second->RemoveTarget(aTargetPlatform);
   }
}

void wkf::AirCombatDisplayInterface::SetVisibility(const bool aVisible)
{
   mShowVisualization = aVisible;
   for (auto& decorator : mDecoratorMap)
   {
      decorator.second->SetVisibility(aVisible);
   }

   for (auto& interactions : mInteractionMap)
   {
      for (auto& interaction : interactions.second)
      {
         interaction->SetVisibility(aVisible);
      }
   }
}

void wkf::AirCombatDisplayInterface::ViewerDestroyedCB(vespa::VaViewer* aViewerPtr)
{
   auto correspondsToViewer = [&](const std::pair<wkf::Platform*, wkf::AirCombatOverlay*>& aOverlayPair)
   { return aOverlayPair.second->GetViewer() == aViewerPtr; };

   auto overlay = std::find_if(mOverlayMap.begin(), mOverlayMap.end(), correspondsToViewer);
   if (overlay != mOverlayMap.end())
   {
      mOverlayMap.erase(overlay);
   }
}

// For each decorator, change the data measured by the meter at a given level (1, 2, or 3)
void wkf::AirCombatDisplayInterface::ChangeMeterMeasurementAtLevel(int aMeterLevel,
                                                                   wkf::AirCombatDisplayInterface::MeterMeasurement aMeasurement)
{
   for (auto& decorator : mDecoratorMap)
   {
      decorator.second->ChangeMeterMeasurementAtLevel(aMeterLevel, aMeasurement);
   }
}

// For each decorator, enable/disable the drawing of platform state data
void wkf::AirCombatDisplayInterface::ShowStateData(bool aShow)
{
   for (auto& decorator : mDecoratorMap)
   {
      decorator.second->ShowStateData(aShow);
   }
}

// For each decorator, show/hide all accents of a given type
void wkf::AirCombatDisplayInterface::ShowAccents(bool aShow, wkf::AirCombatDisplayInterface::AccentType aType)
{
   for (auto& decorator : mDecoratorMap)
   {
      decorator.second->ShowAccents(aShow, aType);
   }
}

// Returns true if a given platform has a decoration associated with it
// or if it is the source of an interaction
bool wkf::AirCombatDisplayInterface::IsPlatformDecorated(wkf::Platform& aPlatform) const
{
   return (mDecoratorMap.find(&aPlatform) != mDecoratorMap.end() ||
           (mInteractionMap.find(&aPlatform) != mInteractionMap.end()));
}

// Removes the decorations and interactions associated with a given platform
void wkf::AirCombatDisplayInterface::RemoveAttachments(wkf::Platform& aPlatform)
{
   auto decorator = mDecoratorMap.find(&aPlatform);
   if (decorator != mDecoratorMap.end())
   {
      decorator->second->RemoveAttachment();
      mDecoratorMap.erase(decorator);
   }

   auto interactionData = mInteractionMap.find(&aPlatform);
   if (interactionData != mInteractionMap.end())
   {
      // Remove interactions from this platform
      for (auto& interaction : mInteractionMap.at(&aPlatform))
      {
         interaction->RemoveAttachment();
      }

      // Remove interactions to this platform
      for (auto& interactionPair : mInteractionMap)
      {
         for (auto& interaction : interactionPair.second)
         {
            if (interaction->GetTargetPlatform() == &aPlatform)
            {
               interaction->RemoveAttachment();
            }
         }
      }

      mInteractionMap.erase(interactionData);
   }
}

// The base class for decorator objects and interaction objects
wkf::AirCombatDisplayInterface::AttachmentDataObject::AttachmentDataObject(wkf::Platform&     aPlatform,
                                                                           const std::string& aAttachmentName)
   : mPlatformPtr(&aPlatform)
   , mAttachmentId(0)
{
}

// Removes the associated attachment from the platform given at construction
void wkf::AirCombatDisplayInterface::AttachmentDataObject::RemoveAttachment()
{
   mPlatformPtr->RemoveAttachment(mAttachmentId);
}

// The data object that's represented by a WkfAttachmentDecorator
wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::AttachmentDecoratorObject(
   wkf::Platform&                         aPlatform,
   const std::string&                     aAttachmentName,
   wkf::AirCombatVisualizationPrefObject& aPrefObject)
   : AttachmentDataObject(aPlatform, aAttachmentName)
   , mOutermostMeterLevel(0)
   , mMeterDiameter(25)
   , mLevelSpacing(15)
   , mAccentOffset(5)
   , mAccentDiameter(20)
{
   wkf::AttachmentDecorator* attachmentPtr =
      vespa::make_attachment<wkf::AttachmentDecorator>(aPlatform,
                                                       vaEnv.GetStandardViewer(),
                                                       QString::fromStdString(aAttachmentName),
                                                       false,
                                                       false,
                                                       0,
                                                       0);

   vespa::VaAttachment::LoadAttachment(*attachmentPtr);
   mAttachmentId = attachmentPtr->GetUniqueId();

   mShowStateData = aPrefObject.GetPreferences().mShowStateData;

   // Initialize the circular meters
   CircularMeter firstMeter;
   firstMeter.mType         = static_cast<MeterMeasurement>(aPrefObject.GetPreferences().mFirstMeter);
   firstMeter.mCurrentValue = 0;
   firstMeter.mLevel        = 1;
   if (firstMeter.mType != MeterMeasurement::cNONE)
   {
      mOutermostMeterLevel = 1;
   }
   mMeterMap.emplace(1, firstMeter);

   CircularMeter secondMeter;
   secondMeter.mType         = static_cast<MeterMeasurement>(aPrefObject.GetPreferences().mSecondMeter);
   secondMeter.mCurrentValue = 0;
   secondMeter.mLevel        = 2;
   if (secondMeter.mType != MeterMeasurement::cNONE)
   {
      mOutermostMeterLevel = 2;
   }
   mMeterMap.emplace(2, secondMeter);

   CircularMeter thirdMeter;
   thirdMeter.mType         = static_cast<MeterMeasurement>(aPrefObject.GetPreferences().mThirdMeter);
   thirdMeter.mCurrentValue = 0;
   thirdMeter.mLevel        = 3;
   if (thirdMeter.mType != MeterMeasurement::cNONE)
   {
      mOutermostMeterLevel = 3;
   }
   mMeterMap.emplace(3, thirdMeter);

   // Initialize the accents
   DataAccent weaponsAccent;
   weaponsAccent.mType     = AccentType::cWEAPONS;
   weaponsAccent.mPosition = AccentPosition::cTOP_LEFT;
   weaponsAccent.mEnabled  = aPrefObject.GetPreferences().mShowWeaponsAccent;
   mAccents.emplace_back(weaponsAccent);

   DataAccent transmissionAccent;
   transmissionAccent.mType     = AccentType::cTRANSMISSION;
   transmissionAccent.mPosition = AccentPosition::cTOP_RIGHT;
   transmissionAccent.mEnabled  = aPrefObject.GetPreferences().mShowTransmissionAccent;
   transmissionAccent.mColor    = QColor(75, 0, 130);
   mAccents.emplace_back(transmissionAccent);

   DataAccent fuelAccent;
   fuelAccent.mType     = AccentType::cFUELSTATE;
   fuelAccent.mPosition = AccentPosition::cBOTTOM_LEFT;
   fuelAccent.mEnabled  = aPrefObject.GetPreferences().mShowFuelAccent;
   mAccents.emplace_back(fuelAccent);

   DataAccent engagementAccent;
   engagementAccent.mType     = AccentType::cSIGNATURE;
   engagementAccent.mPosition = AccentPosition::cBOTTOM_RIGHT;
   engagementAccent.mEnabled  = aPrefObject.GetPreferences().mShowSignatureAccent;
   engagementAccent.mColor    = Qt::darkRed;
   mAccents.emplace_back(engagementAccent);
}

// Updates the data and the attachment according to the given platform data
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::Update(const wkf::AirCombatDataEngagementSummary& aData)
{
   mCurrentData = aData;

   wkf::AttachmentDecorator* decorator =
      dynamic_cast<wkf::AttachmentDecorator*>(mPlatformPtr->FindAttachment(mAttachmentId));

   if (decorator != nullptr)
   {
      // Each of the circular meters and the decorators are drawn on a single QImage.
      // The image is then given to the decorator attachment to render.
      double iconScale = vaEnv.GetStandardViewer()->GetIconScale();
      QImage image(256 * iconScale, 128 * iconScale, QImage::Format::Format_ARGB32_Premultiplied);
      image.fill(Qt::transparent);

      // Set the offset of the decorator such that the platform's icon is in the center of the data rings
      vespa::VaAttachment*      attachment = mPlatformPtr->FindAttachment(mAttachmentId);
      wkf::AttachmentDecorator* decorator  = dynamic_cast<wkf::AttachmentDecorator*>(attachment);
      if (decorator != nullptr)
      {
         decorator->SetOffset(-((image.width() / 4) / 0.75),
                              -((image.height() / 2) /
                                0.75)); // VaDecoratorNode multiplies the given offset by 0.75 for some reason
      }

      // Draw the circular meters
      for (auto& meterData : mMeterMap)
      {
         UpdateMeter(meterData.second);

         if (meterData.second.mType != wkf::AirCombatDisplayInterface::MeterMeasurement::cNONE)
         {
            DrawMeter(image, meterData.second, meterData.first);
         }
      }

      // Draw the accents
      for (auto& accent : mAccents)
      {
         if (accent.mEnabled)
         {
            UpdateAccent(accent);
            DrawAccent(image, accent);
         }
      }

      // Draw the state data (if enabled)
      if (mShowStateData)
      {
         DrawStateData(image);
      }

      // Update the decorator attachment
      decorator->SetImage(image, "");
   }
}

void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::SetVisibility(const bool aVisible)
{
   wkf::AttachmentDecorator* decorator =
      dynamic_cast<wkf::AttachmentDecorator*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (decorator != nullptr)
   {
      decorator->SetStateVisibility(aVisible);
   }
}

bool wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::IsVisible() const
{
   wkf::AttachmentDecorator* decorator =
      dynamic_cast<wkf::AttachmentDecorator*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (decorator != nullptr)
   {
      return decorator->GetStateVisibility();
   }

   return false;
}

// Changes the measurement of the meter at the given level (1, 2, or 3)
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::ChangeMeterMeasurementAtLevel(
   int                                              aLevel,
   wkf::AirCombatDisplayInterface::MeterMeasurement aMeasurement)
{
   auto measurement = mMeterMap.find(aLevel);
   if (measurement != mMeterMap.end())
   {
      measurement->second.mType = aMeasurement;

      if (aMeasurement != wkf::AirCombatDisplayInterface::MeterMeasurement::cNONE && aLevel > mOutermostMeterLevel)
      {
         mOutermostMeterLevel = aLevel;
      }
      else if (aMeasurement == wkf::AirCombatDisplayInterface::MeterMeasurement::cNONE)
      {
         // Recalculate the outermost meter level
         mOutermostMeterLevel = 0;
         for (const auto& meterData : mMeterMap)
         {
            if (meterData.first > mOutermostMeterLevel && meterData.second.mType != MeterMeasurement::cNONE)
            {
               mOutermostMeterLevel = meterData.first;
            }
         }
      }
   }
}

// Set accents visible or invisible
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::ShowAccents(bool aShow,
                                                                            wkf::AirCombatDisplayInterface::AccentType aType)
{
   for (auto& accent : mAccents)
   {
      if (accent.mType == aType)
      {
         accent.mEnabled = aShow;
      }
   }
}

// Updates the target value of the meter based on what it's measuring
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::UpdateMeter(
   wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::CircularMeter& aMeter)
{
   switch (aMeter.mType)
   {
   case wkf::AirCombatDisplayInterface::MeterMeasurement::cDEFENSIVENESS:
   {
      aMeter.mTargetValue = mCurrentData.saData.mDefensiveness;
      break;
   }
   case wkf::AirCombatDisplayInterface::MeterMeasurement::cFUEL:
   {
      aMeter.mTargetValue = mCurrentData.fuelData.mNormalizedFuelLevel;
      break;
   }
   case wkf::AirCombatDisplayInterface::MeterMeasurement::cGFORCE:
   {
      if (mCurrentData.kinematicData.mGLoad >= 1.0)
      {
         aMeter.mTargetValue = std::min(((mCurrentData.kinematicData.mGLoad - 1.0) / 8.0), 1.0);
      }
      else
      {
         double ratio = std::min(((mCurrentData.kinematicData.mGLoad + 3.0) / 4.0), 3.0);
         ratio        = 1.0 - ratio;
         ratio *= -1;
         aMeter.mTargetValue = ratio;
      }
      break;
   }
   case wkf::AirCombatDisplayInterface::MeterMeasurement::cNONE:
   {
      break;
   }
   }

   // Interpolate the current value towards the target value
   if (aMeter.mCurrentValue != aMeter.mTargetValue)
   {
      aMeter.mCurrentValue = aMeter.mCurrentValue + (aMeter.mTargetValue - aMeter.mCurrentValue) * 0.3;

      switch (aMeter.mType)
      {
      case wkf::AirCombatDisplayInterface::MeterMeasurement::cDEFENSIVENESS:
      {
         aMeter.mCurrentColor = Qt::green;
      }
      case wkf::AirCombatDisplayInterface::MeterMeasurement::cFUEL:
      {
         aMeter.mCurrentColor = aMeter.mCurrentValue > 0 ? Qt::green : Qt::red;
         break;
      }
      case wkf::AirCombatDisplayInterface::MeterMeasurement::cGFORCE:
      {
         if (aMeter.mCurrentValue >= 0)
         {
            aMeter.mCurrentColor = Qt::green;
         }
         else
         {
            if (aMeter.mCurrentValue >= -0.25)
            {
               aMeter.mCurrentColor = Qt::yellow;
            }
            else
            {
               aMeter.mCurrentColor = Qt::red;
            }
         }
         break;
      }
      case wkf::AirCombatDisplayInterface::MeterMeasurement::cNONE:
         break;
      }
   }
}

// Draws the given meter on the given image
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::DrawMeter(
   QImage&                                                                   aImage,
   wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::CircularMeter& aMeter,
   int                                                                       aLevel)
{
   QPen     pen;
   QPainter painter(&aImage);
   painter.setRenderHints(QPainter::RenderHint::Antialiasing);

   double iconScale = vaEnv.GetStandardViewer()->GetIconScale();
   double diameter  = mMeterDiameter + (mLevelSpacing * aLevel);
   diameter *= iconScale;
   QRect circleRect(0, 0, diameter, diameter);
   circleRect.moveCenter(QPoint(aImage.width() / 4, aImage.height() / 2));

   // Draw the empty portion of the meter.
   pen.setColor(Qt::darkGray);
   pen.setWidth(2.0);
   painter.setPen(pen);
   painter.drawEllipse(circleRect);

   // Draw the filled portion of the meter based on the current value,
   // after interpolation towards the target value
   pen.setColor(aMeter.mCurrentColor);
   pen.setWidth(3.5);
   painter.setPen(pen);

   double startAngle = 180 * 16; // Qt expects angle values in 1/16th of a degree
   double spanAngle  = -(2 * osg::PI * aMeter.mCurrentValue) * (180 / osg::PI) * 16;

   painter.drawArc(circleRect, startAngle, spanAngle);

   // Draw the baseline of the meter
   pen.setColor(Qt::black);
   pen.setWidth(4.0);
   painter.setPen(pen);
   int x1 = circleRect.center().x() - (diameter / 3);
   int x2 = x1 - (diameter / 4);
   int y  = circleRect.center().y();
   painter.drawLine(x1, y, x2, y);
}

// Updates the accent based on the current data
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::UpdateAccent(
   wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::DataAccent& aAccent)
{
   switch (aAccent.mType)
   {
   case wkf::AirCombatDisplayInterface::AccentType::cSIGNATURE:
   {
      if (mCurrentData.signatureData.mWeaponBayDoorsOpen)
      {
         aAccent.mText = "W";
      }
      else if (mCurrentData.signatureData.mAfterburnerOn)
      {
         aAccent.mText = "A";
      }
      else if (mCurrentData.signatureData.mContrailing)
      {
         aAccent.mText = "C";
      }
      else
      {
         aAccent.mText = "--";
      }
      break;
   }
   case wkf::AirCombatDisplayInterface::AccentType::cFUELSTATE:
   {
      if (mCurrentData.fuelData.mJokerReached && !mCurrentData.fuelData.mBingoReached)
      {
         aAccent.mText  = "J";
         aAccent.mColor = Qt::darkBlue;
      }
      else if (mCurrentData.fuelData.mBingoReached)
      {
         aAccent.mText  = "B";
         aAccent.mColor = Qt::darkRed;
      }
      else
      {
         if (mCurrentData.fuelData.mNormalizedFuelLevel < 0.1)
         {
            aAccent.mText  = "F";
            aAccent.mColor = Qt::darkRed;
         }
         else if (mCurrentData.fuelData.mNormalizedFuelLevel < 0.5)
         {
            aAccent.mText  = "F";
            aAccent.mColor = Qt::darkYellow;
         }
         else
         {
            aAccent.mText  = "F";
            aAccent.mColor = Qt::darkGreen;
         }
      }
      break;
   }
   case wkf::AirCombatDisplayInterface::AccentType::cTRANSMISSION:
   {
      if (mCurrentData.emissionsData.mRadarEmitting && mCurrentData.emissionsData.mJammerEmitting)
      {
         aAccent.mText = "X";
      }
      else if (mCurrentData.emissionsData.mRadarEmitting)
      {
         aAccent.mText = "R";
      }
      else if (mCurrentData.emissionsData.mJammerEmitting)
      {
         aAccent.mText = "J";
      }
      else
      {
         aAccent.mText = "--";
      }
      break;
   }
   case wkf::AirCombatDisplayInterface::AccentType::cWEAPONS:
   {
      int weaponsRemaining = 0;
      for (const auto& weaponQuantity : mCurrentData.weaponsData.mWeaponsQtyMap)
      {
         weaponsRemaining += weaponQuantity.second;
      }

      double percentageRemaining = 0.0;
      if (mCurrentData.weaponsData.mInitialTotalWeaponCount > 0)
      {
         percentageRemaining = weaponsRemaining / mCurrentData.weaponsData.mInitialTotalWeaponCount;
      }

      if (percentageRemaining > 0.5)
      {
         aAccent.mColor = Qt::darkGreen;
      }
      else if (percentageRemaining > 0.0)
      {
         aAccent.mColor = Qt::darkBlue;
      }
      else
      {
         aAccent.mColor = Qt::darkRed;
      }

      aAccent.mText = "W";
      break;
   }
   }
}

// Draws the given accent on the given image
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::DrawAccent(
   QImage&                                                                aImage,
   wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::DataAccent& aAccent)
{
   QPainter painter(&aImage);

   painter.setRenderHints(QPainter::RenderHint::Antialiasing);
   painter.setBrush(aAccent.mColor);
   painter.setPen(aAccent.mColor);

   int    xPos = 0, yPos = 0;
   int    outermostDiameter = mMeterDiameter + (mLevelSpacing * mOutermostMeterLevel);
   double iconScale         = vaEnv.GetStandardViewer()->GetIconScale();
   outermostDiameter *= iconScale;

   switch (aAccent.mPosition)
   {
   case AccentPosition::cTOP_LEFT:
   {
      xPos = (aImage.width() / 4) - (outermostDiameter / 2) - (mAccentOffset * iconScale);
      yPos = (aImage.height() / 2) - (outermostDiameter / 2) - (mAccentOffset * iconScale);
      break;
   }
   case AccentPosition::cTOP_RIGHT:
   {
      xPos = (aImage.width() / 4) + (outermostDiameter / 2) + (mAccentOffset * iconScale);
      yPos = (aImage.height() / 2) - (outermostDiameter / 2) - (mAccentOffset * iconScale);
      break;
   }
   case AccentPosition::cBOTTOM_LEFT:
   {
      xPos = (aImage.width() / 4) - (outermostDiameter / 2) - (mAccentOffset * iconScale);
      yPos = (aImage.height() / 2) + (outermostDiameter / 2) + (mAccentOffset * iconScale);
      break;
   }
   case AccentPosition::cBOTTOM_RIGHT:
   {
      xPos = (aImage.width() / 4) + (outermostDiameter / 2) + (mAccentOffset * iconScale);
      yPos = (aImage.height() / 2) + (outermostDiameter / 2) + (mAccentOffset * iconScale);
      break;
   }
   }

   QRect accentRect = QRect(0, 0, mAccentDiameter, mAccentDiameter);
   accentRect.moveCenter(QPoint(xPos, yPos));
   painter.drawEllipse(accentRect);

   painter.setRenderHints(QPainter::RenderHint::Antialiasing, false);
   painter.setBrush(Qt::BrushStyle::NoBrush);
   painter.setPen(Qt::white);
   QFont font = painter.font();
   font.setPointSize(font.pointSize() * QApplication::desktop()->devicePixelRatio());
   painter.setFont(font);
   painter.drawText(accentRect, Qt::AlignCenter, QString::fromStdString(aAccent.mText));
}

// Draws some of the current data as text on the decorator
void wkf::AirCombatDisplayInterface::AttachmentDecoratorObject::DrawStateData(QImage& aImage)
{
   QPainter painter(&aImage);

   QFont font = painter.font();
   font.setPointSize(font.pointSize() * QApplication::desktop()->devicePixelRatio());
   font.setBold(true);
   painter.setFont(font);

   // Calculate the width of the platform name
   int maxWidth =
      painter.boundingRect(0, 0, 128, 128, 0, QString::fromStdString(mCurrentData.platformData.mPlatformName)).width();

   // Calculate the maximum width of the data regions
   std::stringstream stream;
   stream << "########"; // Use this 8 hash string to serve as a max width test string
   std::string stringLengthTest = stream.str();
   int         temp = painter.boundingRect(0, 0, 128, 128, 0, QString::fromStdString(stringLengthTest)).width();
   maxWidth         = std::max(temp, maxWidth);

   stream.str("");
   stream << std::fixed << std::setprecision(1) << "ALT ";
   std::string altitude1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(1);
   stream << static_cast<int>(mCurrentData.kinematicData.mAltitude_ft);
   std::string altitude2 = stream.str();

   stream.str("");
   stream << std::fixed << std::setprecision(1) << "VS ";
   std::string roc1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(1);
   if (mCurrentData.kinematicData.mVerticalSpeed_fpm >= 0)
   {
      stream << "+";
   }
   stream << static_cast<int>(mCurrentData.kinematicData.mVerticalSpeed_fpm);
   std::string roc2 = stream.str();

   stream.str("");
   stream << std::fixed << std::setprecision(1) << "KCAS ";
   std::string kcas1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(1);
   stream << static_cast<int>(mCurrentData.kinematicData.mKCAS);
   std::string kcas2 = stream.str();

   stream.str("");
   stream << std::fixed << std::setprecision(2) << "MACH ";
   std::string mach1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(2);
   stream << mCurrentData.kinematicData.mMach;
   std::string mach2 = stream.str();

   stream.str("");
   stream << std::fixed << std::setprecision(1) << "G   ";
   std::string gLoad1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(1);
   if (mCurrentData.kinematicData.mGLoad >= 0)
   {
      stream << "+";
   }
   stream << mCurrentData.kinematicData.mGLoad;
   std::string gLoad2 = stream.str();

   stream.str("");
   stream << std::fixed << std::setprecision(1) << "AOA ";
   std::string alpha1 = stream.str();
   stream.str("");
   stream << std::fixed << std::setprecision(1);
   if (mCurrentData.kinematicData.mAlpha_deg >= 0)
   {
      stream << "+";
   }
   stream << mCurrentData.kinematicData.mAlpha_deg;
   std::string alpha2 = stream.str();

   // Draw the background box using the maximum width of the text
   painter.setPen(Qt::transparent);
   painter.setBrush(QColor(50, 50, 50, 150));

   int    outermostDiameter = mMeterDiameter + (mLevelSpacing * mOutermostMeterLevel);
   double iconScale         = vaEnv.GetStandardViewer()->GetIconScale();
   outermostDiameter *= iconScale;

   int xPos = (aImage.width() / 4) + (outermostDiameter / 2) + (mAccentOffset * iconScale) + (mAccentDiameter / 2) + 5;
   int yPos = (aImage.height() / 2) - (outermostDiameter / 2) - (mAccentOffset * iconScale) - (mAccentDiameter / 2);
   painter.drawRect(xPos, yPos, maxWidth + 10, ((font.pointSize() + 5) * 7));

   // Draw the text
   painter.setPen(Qt::white);
   xPos += 5;

   // Set a lighter weight for the info
   font.setWeight(QFont::Weight::Normal);
   painter.setFont(font);

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(mCurrentData.platformData.mPlatformName));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(altitude1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(altitude2));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(roc1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(roc2));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(kcas1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(kcas2));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(mach1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(mach2));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(gLoad1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(gLoad2));
   yPos += font.pointSize() + 5;

   painter.drawText(xPos, yPos, 128, font.pointSize() + 5, 0, QString::fromStdString(alpha1));
   painter.drawText(xPos - 60, yPos, 128, font.pointSize() + 5, Qt::AlignRight, QString::fromStdString(alpha2));
}

// The data object that's represented by a WkfAttachmentVariableInteraction
wkf::AirCombatDisplayInterface::AttachmentInteractionObject::AttachmentInteractionObject(wkf::Platform& aPlatform,
                                                                                         wkf::Platform& aTargetPlatform,
                                                                                         InteractionType aType,
                                                                                         const std::string& aAttachmentName)
   : AttachmentDataObject(aPlatform, aAttachmentName)
   , mBlinking(false)
   , mTargetPlatformPtr(&aTargetPlatform)
   , mInteractionType(aType)
{
   wkf::AttachmentVariableInteraction* attachmentPtr =
      vespa::make_attachment<wkf::AttachmentVariableInteraction>(aPlatform,
                                                                 aTargetPlatform,
                                                                 vaEnv.GetStandardViewer(),
                                                                 aAttachmentName);
   vespa::VaAttachment::LoadAttachment(*attachmentPtr);
   mAttachmentId = attachmentPtr->GetUniqueId();

   // Initialize the interaction line based on the type
   switch (aType)
   {
   case wkf::AirCombatDisplayInterface::InteractionType::cBASE:
   {
      attachmentPtr->SetColor(UtoColor(115, 115, 115));
      attachmentPtr->SetFillFactor(1);
      attachmentPtr->SetWidth(1);
      break;
   }
   case wkf::AirCombatDisplayInterface::InteractionType::cDETECTION:
   {
      auto* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (teamVisPrefs != nullptr)
      {
         UtoColor teamColor = ConvertUtColor(teamVisPrefs->GetTeamUtColor(aPlatform.GetSide()), 155);
         attachmentPtr->SetColor(teamColor);
      }
      else
      {
         attachmentPtr->SetColor(UtoColor(220, 220, 220, 155));
      }
      attachmentPtr->SetOffset(1);
      attachmentPtr->SetWidth(3);
      break;
   }
   case wkf::AirCombatDisplayInterface::InteractionType::cWEZ:
   {
      auto* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (teamVisPrefs != nullptr)
      {
         UtoColor teamColor = ConvertUtColor(teamVisPrefs->GetTeamUtColor(aPlatform.GetSide()), 225);
         attachmentPtr->SetColor(teamColor);
      }
      else
      {
         attachmentPtr->SetColor(UtoColor(220, 220, 220, 155));
      }
      attachmentPtr->SetOffset(3);
      attachmentPtr->SetWidth(3);
      break;
   }
   }
}

void wkf::AirCombatDisplayInterface::AttachmentInteractionObject::Update(const wkf::AirCombatDataEngagementSummary& aData)
{
   mCurrentData = aData;

   wkf::AttachmentVariableInteraction* interaction =
      dynamic_cast<wkf::AttachmentVariableInteraction*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (interaction != nullptr)
   {
      switch (mInteractionType)
      {
      case InteractionType::cBASE:
      {
         break;
      }
      case InteractionType::cDETECTION:
      {
         auto currentRange_nm = mCurrentData.engagementData.mCurrentRangeMap_nm.find(mTargetPlatformPtr->GetName());
         if (currentRange_nm != mCurrentData.engagementData.mCurrentRangeMap_nm.end())
         {
            auto targetDetection_nm =
               mCurrentData.engagementData.mRawDetectionAgainstTargetMap_nm.find(mTargetPlatformPtr->GetName());
            if (targetDetection_nm != mCurrentData.engagementData.mRawDetectionAgainstTargetMap_nm.end())
            {
               double range_nm = currentRange_nm->second;
               if (range_nm > std::numeric_limits<double>::epsilon())
               {
                  double normalizedRange = targetDetection_nm->second / range_nm;
                  if (normalizedRange > 1.0)
                  {
                     normalizedRange = 1.0;
                  }
                  interaction->SetFillFactor(normalizedRange);

                  auto tracked = std::find(mCurrentData.engagementData.mTrackedPlatformNames.begin(),
                                           mCurrentData.engagementData.mTrackedPlatformNames.end(),
                                           mTargetPlatformPtr->GetName());
                  if (tracked != mCurrentData.engagementData.mTrackedPlatformNames.end())
                  {
                     mBlinking = true;
                  }
                  else
                  {
                     mBlinking = false;
                     interaction->SetStateVisibility(true);
                  }
               }
            }
         }

         break;
      }
      case InteractionType::cWEZ:
      {
         auto currentRange_nm = mCurrentData.engagementData.mCurrentRangeMap_nm.find(mTargetPlatformPtr->GetName());
         if (currentRange_nm != mCurrentData.engagementData.mCurrentRangeMap_nm.end())
         {
            auto targetWEZ_nm = mCurrentData.engagementData.mRawWEZAgainstTargetMap_nm.find(mTargetPlatformPtr->GetName());
            if (targetWEZ_nm != mCurrentData.engagementData.mRawWEZAgainstTargetMap_nm.end())
            {
               double range_nm = currentRange_nm->second;
               if (range_nm > std::numeric_limits<double>::epsilon())
               {
                  double normalizedWezRange = targetWEZ_nm->second / range_nm;
                  if (normalizedWezRange > 1.0)
                  {
                     normalizedWezRange = 1.0;
                  }
                  interaction->SetFillFactor(normalizedWezRange);
                  normalizedWezRange >= 1.0 ? mBlinking = true : mBlinking = false;
               }
            }
         }
      }
      }
   }
}

void wkf::AirCombatDisplayInterface::AttachmentInteractionObject::SetVisibility(const bool aVisible)
{
   wkf::AttachmentVariableInteraction* interaction =
      dynamic_cast<wkf::AttachmentVariableInteraction*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (interaction != nullptr)
   {
      interaction->SetStateVisibility(aVisible);
   }
}

void wkf::AirCombatDisplayInterface::AttachmentInteractionObject::ToggleVisibility()
{
   wkf::AttachmentVariableInteraction* interaction =
      dynamic_cast<wkf::AttachmentVariableInteraction*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (interaction != nullptr)
   {
      interaction->SetStateVisibility(!interaction->GetStateVisibility());
   }
}

bool wkf::AirCombatDisplayInterface::AttachmentInteractionObject::IsVisible() const
{
   wkf::AttachmentVariableInteraction* interaction =
      dynamic_cast<wkf::AttachmentVariableInteraction*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (interaction != nullptr)
   {
      return interaction->GetStateVisibility();
   }

   return false;
}

void wkf::AirCombatDisplayInterface::AttachmentInteractionObject::SetColor(const UtoColor& aColor)
{
   wkf::AttachmentVariableInteraction* interaction =
      dynamic_cast<wkf::AttachmentVariableInteraction*>(mPlatformPtr->FindAttachment(mAttachmentId));
   if (interaction != nullptr)
   {
      interaction->SetColor(aColor);
   }
}
