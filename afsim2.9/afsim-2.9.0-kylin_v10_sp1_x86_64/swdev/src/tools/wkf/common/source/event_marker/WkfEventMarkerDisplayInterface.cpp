// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfEventMarkerDisplayInterface.hpp"

#include <QDir>
#include <QStandardPaths>

#include "VaEntity.hpp"
#include "VaModelDatabase.hpp"
#include "WkfAttachmentEventMarker.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::EventMarkerDisplayInterface::Marker::Marker(double aCreationTime, vespa::EntityPtr<BaseEventMarker> aEntity)
   : mCreationTime(aCreationTime)
   , mEntity(aEntity)
{
}

wkf::EventMarkerDisplayInterface::EventMarkerDisplayInterface(QObject* aParent, wkf::EventMarkerPrefWidget* aPrefWidgetPtr)
   : QObject(aParent)
   , mPrefWidgetPtr(aPrefWidgetPtr)
{
   ReloadCustomModels();

   auto* prefObjectPtr = mPrefWidgetPtr->GetPreferenceObject();
   connect(prefObjectPtr, &EventMarkerPrefObject::MarkerScaleChanged, this, &EventMarkerDisplayInterface::MarkerScaleChanged);
   connect(prefObjectPtr, &EventMarkerPrefObject::ShowEventChanged, this, &EventMarkerDisplayInterface::ShowEventChanged);
   connect(prefObjectPtr, &EventMarkerPrefObject::EventColorChanged, this, &EventMarkerDisplayInterface::EventColorChanged);
   connect(prefObjectPtr,
           &EventMarkerPrefObject::EventMarkerTypeChanged,
           this,
           &EventMarkerDisplayInterface::EventMarkerTypeChanged);
   connect(prefObjectPtr, &EventMarkerPrefObject::EventIconChanged, this, &EventMarkerDisplayInterface::EventImageChanged);

   mCallbacks.Add(
      wkf::Observer::StandardScenarioRemoved.Connect(&EventMarkerDisplayInterface::StandardScenarioRemovedCB, this));
   // handles user selecting the option to reload models
   mCallbacks.Add(wkf::Observer::ModelsReloaded.Connect(&EventMarkerDisplayInterface::ReloadCustomModels, this));
}

void wkf::EventMarkerDisplayInterface::AddMarker(vespa::EntityPtr<BaseEventMarker> aMarker, const vespa::VaPosition& aPos)
{
   if (aMarker != nullptr)
   {
      aMarker->SetPosition(aPos);
      // CppCheck complains that this can leak,
      // This is because of VaEntity::AddAttachment has a conditional to take ownership because it enforces a unique
      // name for the attachment. However this is a newly constructed attachment and therefore has a unique id that can
      // not conflict with any children of aMarker.
      auto        modelPtr = vespa::make_attachment<wkf::AttachmentEventMarker>(*aMarker, vaEnv.GetStandardViewer());
      std::string type     = aMarker->GetEventType();
      QColor      qcol     = GetTypeColor(type);
      UtColor     ucol(qcol.redF(), qcol.greenF(), qcol.blueF());
      QImage      iconImage = GetImage(type);

      wkf::MarkerType buttonSelection = GetMarkerType(type);
      if (buttonSelection == wkf::MarkerType::X)
      {
         modelPtr->SetIcon("x_marker");
         modelPtr->UseTeamColorUniform(true);
         modelPtr->SetColor(ucol);
      }
      else if (buttonSelection == wkf::MarkerType::CUSTOM && !iconImage.isNull())
      {
         std::string markerName = "custom_marker_" + type;
         modelPtr->SetIcon(markerName);
         modelPtr->UseTeamColorUniform(false);
      }
      // use DOT by default if not otherwise specified
      else
      {
         modelPtr->SetIcon("marker");
         modelPtr->UseTeamColorUniform(true);
         modelPtr->SetColor(ucol);
      }
      modelPtr->Initialize();
      // initialize assigns identity matrices, but we may want some other scaling matrix
      modelPtr->SetScale(mPrefWidgetPtr->GetPreferenceObject()->GetMarkerScale());
      mMarkers[type].emplace_back(aMarker->GetEventTime(), std::move(aMarker));
      // cppcheck-suppress memleak
   }
}

void wkf::EventMarkerDisplayInterface::CleanUp(double aSimTime)
{
   if (!mMarkers.empty())
   {
      // clear stale markers (time > marker.time + options.Timeout) || (time < marker.time)
      const double timeout = mPrefWidgetPtr->GetPreferenceObject()->GetTimeout();
      MarkerMap    newmap;
      for (auto&& it : mMarkers)
      {
         for (auto&& jt : it.second)
         {
            if ((aSimTime > jt.mCreationTime + timeout) || (aSimTime < jt.mCreationTime))
            {
               vaEnv.DeleteEntity(jt.mEntity);
            }
            else
            {
               newmap[it.first].emplace_back(jt);
            }
         }
         it.second.clear();
      }
      mMarkers = std::move(newmap);
   }
}

void wkf::EventMarkerDisplayInterface::DeleteEventMarkers()
{
   DeleteType("DAMAGED");
   DeleteType("REMOVED");
   DeleteType("WEAPON_HIT");
   DeleteType("WEAPON_MISSED");
}

QColor wkf::EventMarkerDisplayInterface::GetTypeColor(const std::string& aType) const
{
   return mPrefWidgetPtr->GetPreferenceObject()->GetEventColor(aType);
}

wkf::MarkerType wkf::EventMarkerDisplayInterface::GetMarkerType(const std::string& aType) const
{
   return mPrefWidgetPtr->GetPreferenceObject()->GetMarkerType(aType);
}

QImage wkf::EventMarkerDisplayInterface::GetImage(const std::string& aType) const
{
   return mPrefWidgetPtr->GetPreferenceObject()->GetEventImage(aType);
}

void wkf::EventMarkerDisplayInterface::ReloadCustomModels()
{
   // Manually add models to the model database (reload them when an image changes)
   QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
   auto    eventTypes  = mPrefWidgetPtr->GetPreferenceObject()->GetEventTypes();
   for (const auto& et : eventTypes)
   {
      std::string              modelName = "custom_marker" + et;
      std::string              iconPath  = appDataPath.toStdString() + "/custom_icons/custom_marker" + et + ".png";
      vespa::VaModelDefinition custom(iconPath, 0.3f, osg::Vec3(0, 0, 0), true, false);
      vaEnv.GetModelDatabase()->AddCustomModelDefinition(modelName, custom);
   }
   vaEnv.GetModelDatabase()->ReloadModels();
   // create a directory for custom icons in settings directory if one does not already exist
   if (!QDir(appDataPath + "/custom_icons").exists())
   {
      QDir().mkdir(appDataPath + "/custom_icons");
   }
}

void wkf::EventMarkerDisplayInterface::MarkerScaleChanged(float aState)
{
   for (auto& mapIt : mMarkers)
   {
      for (auto& it : mapIt.second)
      {
         auto* att = it.mEntity->FindFirstAttachmentOfType<wkf::AttachmentEventMarker>();
         att->SetScale(aState);
      }
   }
}

void wkf::EventMarkerDisplayInterface::ShowEventChanged(bool aState, const std::string& aEvent)
{
   if (!aState)
   {
      DeleteType(aEvent);
   }
}

void wkf::EventMarkerDisplayInterface::EventColorChanged(const QColor& aColor, const std::string& aEventType)
{
   for (auto& marker : mMarkers[aEventType])
   {
      auto*   att = marker.mEntity->FindFirstAttachmentOfType<wkf::AttachmentEventMarker>();
      UtColor ucol(aColor.redF(), aColor.greenF(), aColor.blueF());
      att->UseTeamColorUniform(true);
      att->SetColor(ucol);
   }
}

void wkf::EventMarkerDisplayInterface::EventMarkerTypeChanged(MarkerType aMarkerType, const std::string& aEventType)
{
   for (auto& marker : mMarkers[aEventType])
   {
      QColor color = GetTypeColor(aEventType);
      auto*  att   = marker.mEntity->FindFirstAttachmentOfType<wkf::AttachmentEventMarker>();
      if (aMarkerType == wkf::MarkerType::X)
      {
         att->SetIcon("x_marker");
         UtColor ucol(color.redF(), color.greenF(), color.blueF(), 1);
         att->UseTeamColorUniform(true);
         att->SetColor(ucol);
      }
      else if (aMarkerType == wkf::MarkerType::CUSTOM)
      {
         std::string markerName = "custom_marker_" + aEventType;
         att->SetIcon(markerName);
         att->UseTeamColorUniform(false);
      }
      else
      {
         att->SetIcon("marker");
         UtColor ucol(color.redF(), color.greenF(), color.blueF(), 1);
         att->UseTeamColorUniform(true);
         att->SetColor(ucol);
      }
   }
}

void wkf::EventMarkerDisplayInterface::EventImageChanged(const QImage& aImage, const std::string& aEventType)
{
   // Update the image with a new custom icon
   QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
   QImage  iconImage   = aImage.scaled(64, 64);

   UtPath  sourcePath = (vaEnv.GetModelDatabase()->GetModelDefinition("marker"))->GetDefinitionSourceFile("");
   QString iconPath   = appDataPath + "/custom_icons/custom_marker_" + QString::fromStdString(aEventType) + ".png";

   iconImage.save(iconPath);

   // Update the model definition to reflect the change in image
   vespa::VaModelDefinition newModel(iconPath.toStdString(), 0.3f, osg::Vec3(0, 0, 0), true);
   vaEnv.GetModelDatabase()->AddCustomModelDefinition("custom_marker_" + aEventType, newModel);
   vaEnv.GetModelDatabase()->ReloadModels();

   // Account for image changing without the marker type being modified
   if (GetMarkerType(aEventType) == wkf::MarkerType::CUSTOM)
   {
      for (auto& marker : mMarkers[aEventType])
      {
         auto*       att        = marker.mEntity->FindFirstAttachmentOfType<wkf::AttachmentEventMarker>();
         std::string markerName = "custom_marker_" + aEventType;
         att->SetIcon(markerName);
         att->UseTeamColorUniform(false);
      }
   }
}

void wkf::EventMarkerDisplayInterface::DeleteType(const std::string& aType)
{
   for (auto&& it : mMarkers[aType])
   {
      vaEnv.DeleteEntity(it.mEntity);
   }
   mMarkers[aType].clear();
}

void wkf::EventMarkerDisplayInterface::StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr)
{
   // If the standard scenario was removed, then the EventMarkers (which are owned by the Scenario) were deleted by the
   // Scenario. Therefore clear the internal list without deleting the Markers since they have already been deleted.
   mMarkers.clear();
}
