// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfTrackDisplayInterface.hpp"

#include <array>

#include "UtoShape.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "WkfAttachmentLabel.hpp"
#include "WkfAttachmentModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMilStdIconPrefObject.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
const std::string cWEDGE_STRING     = "wedge";
const std::string cCUSTOM_STRING    = "customTrackModel";
const std::string cMIL_STD_PREFIX   = "milStdModel_";
const std::string cATTACHMENT_NAME  = "mapDisplayModel";
const QString     cCUSTOM_ICON_PATH = "/customIcons/";
} // namespace

wkf::TrackDisplayInterface::TrackDisplayInterface(QObject* aParent)
   : QObject(aParent)
{
   // Forces reload of saved images.
   TrackImageChanged();
   // Forces reload of colors.
   TrackColorChanged();

   mCallbacks.Add(wkf::Observer::TrackAdded.Connect(&TrackDisplayInterface::TrackAddedHandler, this));
   mCallbacks.Add(wkf::Observer::ModelsReloaded.Connect(&TrackDisplayInterface::TrackImageChanged, this));

   auto trackPrefObj = wkfEnv.GetPreferenceObject<TrackVisibilityPrefObject>();
   connect(trackPrefObj, &TrackVisibilityPrefObject::TrackImageChanged, this, &TrackDisplayInterface::TrackImageChanged);
   connect(trackPrefObj, &TrackVisibilityPrefObject::TrackIconChanged, this, &TrackDisplayInterface::TrackIconChanged);
   connect(trackPrefObj, &TrackVisibilityPrefObject::TrackColorChanged, this, &TrackDisplayInterface::TrackColorChanged);

   auto teamPrefObj = wkfEnv.GetPreferenceObject<TeamVisibilityPrefObject>();
   connect(teamPrefObj, &TeamVisibilityPrefObject::TeamColorChanged, this, &TrackDisplayInterface::TrackColorChanged);

   auto milStdPrefObj = wkfEnv.GetPreferenceObject<MilStdIconPrefObject>();
   connect(milStdPrefObj, &MilStdIconPrefObject::UseSymbologyChanged, this, &TrackDisplayInterface::TrackIconChanged);
}

vespa::VaAttachmentModel* wkf::TrackDisplayInterface::GetModel(vespa::VaEntity* aEntityPtr) const
{
   auto attachmentVector = aEntityPtr->FindAttachmentsOfType<vespa::VaAttachmentModel>();

   // Using iteration by value.
   for (auto modelPtr : attachmentVector)
   {
      if ((modelPtr->GetName() == cATTACHMENT_NAME) && (modelPtr->IsMyViewer(vaEnv.GetStandardViewer())))
      {
         return modelPtr;
      }
   }
   return nullptr;
}

void wkf::TrackDisplayInterface::TrackAddedHandler(wkf::Track* aTrackPtr)
{
   wkf::Scenario* currentScenario = vaEnv.GetStandardScenario();
   if (currentScenario && aTrackPtr->GetScenario() == currentScenario)
   {
      auto modelPtr = vespa::make_attachment<wkf::AttachmentModel>(*aTrackPtr, vaEnv.GetStandardViewer());
      if (modelPtr != nullptr)
      {
         modelPtr->Initialize();
         modelPtr->SetName(cATTACHMENT_NAME);

         UpdateTrackIcon(*aTrackPtr);
         modelPtr->SetProxy(true);

         UpdateTrackLabelColor(aTrackPtr);
      }
   }
}

void wkf::TrackDisplayInterface::UpdateTrackLabelColor(wkf::Track* aTrack) const
{
   bool showColor = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>()->GetShowColor();

   wkf::AttachmentLabel* labelPtr    = dynamic_cast<wkf::AttachmentLabel*>(aTrack->FindAttachment("label"));
   auto*                 teamPrefObj = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   const auto& side = aTrack->GetSide();
   if (showColor && !side.empty())
   {
      labelPtr->SetColor(
         teamPrefObj->GetTeamColor(QString::fromStdString(side))); // set the label color as the track team color
   }
   else
   {
      labelPtr->SetColor(teamPrefObj->GetTeamColor(
         wkf::TeamVisibilityPrefData::cDEFAULT_TEAM)); // set the label color as the default team color
   }
}

// TODO Notes: When a custom image is changed via prefs and on startup
void wkf::TrackDisplayInterface::TrackImageChanged()
{
   // create a directory for custom icons in settings directory if one does not already exist
   const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
   if (!QDir(appDataPath + cCUSTOM_ICON_PATH).exists())
   {
      QDir().mkdir(appDataPath + cCUSTOM_ICON_PATH);
   }

   auto prefObj = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>();
   // Custom
   UpdateTrackImage(QString(), prefObj->GetCustomIconPath());

   for (int i = 0; i < SPATIAL_DOMAIN_COUNT; i++)
   {
      const SpatialDomain sd = static_cast<SpatialDomain>(i);
      UpdateTrackImage(QString::fromStdString(SpatialDomainToString(sd)), prefObj->GetDomainIconPath(sd));
   }

   TrackIconChanged();

   vaEnv.GetModelDatabase()->ReloadModels();
}

void wkf::TrackDisplayInterface::UpdateMilStdModels() {}

// TODO Notes: Only called in TrackImageChanged
// When SetIcon is called, it is necessary to re-apply other settings (billboard, light, etc.) because they are reset to
// their default state
void wkf::TrackDisplayInterface::UpdateTrackImage(const QString& aDomain, const QString& aImagePath)
{
   if (!aImagePath.isEmpty())
   {
      // Update the image with a new custom icon
      const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      const QString iconPath = appDataPath + cCUSTOM_ICON_PATH + QString::fromStdString(cCUSTOM_STRING) + aDomain + ".png";

      const QImage image(aImagePath);
      if (!image.isNull())
      {
         image.scaled(64, 64).save(iconPath);
      }
      else
      {
         QFile(iconPath).remove();
      }

      // Update the model definition to reflect the change in image
      vespa::VaModelDefinition newModel(iconPath.toStdString(), 1.0f, osg::Vec3(-4, 3, 0));
      vaEnv.GetModelDatabase()->AddCustomModelDefinition(cCUSTOM_STRING + aDomain.toStdString(), newModel);
   }
}

void wkf::TrackDisplayInterface::TrackColorChanged() const
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntityList list;
      scenario->GetEntityList(list);
      for (auto* i : list)
      {
         wkf::Track* track = dynamic_cast<wkf::Track*>(i);
         if (track != nullptr)
         {
            UpdateTrackColor(*track);
            UpdateTrackLabelColor(track);
         }
      }
   }
}

void wkf::TrackDisplayInterface::UpdateTrackColor(Track& aTrack) const
{
   vespa::VaAttachmentModel* model = GetModel(&aTrack);

   if (model != nullptr)
   {
      auto prefObj     = wkfEnv.GetPreferenceObject<TrackVisibilityPrefObject>();
      auto teamPrefObj = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

      model->UseTeamColorUniform(true);
      // UseTeamColorUniform() currently sets the setting for all instances of that model. This is an issue when the
      // platform and track use the same model.
      if (prefObj->GetShowColor())
      {
         model->SetColor(teamPrefObj->GetTeamUtColor(aTrack.GetSide()));
      }
      else
      {
         model->SetColor(teamPrefObj->GetTeamUtColor(wkf::TeamVisibilityPrefData::cDEFAULT_TEAM));
      }
   }
}


// TODO Notes: Called when the type of icon (custom, wedge, etc.) is changed and when a custom image is changed
void wkf::TrackDisplayInterface::TrackIconChanged()
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntityList list;
      scenario->GetEntityList(list);
      for (auto& i : list)
      {
         wkf::Track* track = dynamic_cast<wkf::Track*>(i);
         if (track != nullptr)
         {
            UpdateTrackIcon(*track);
         }
      }
   }
   vaEnv.GetModelDatabase()->ReloadModels();
}

void wkf::TrackDisplayInterface::UpdateTrackIcon(Track& aTrack)
{
   vespa::VaAttachmentModel* model = GetModel(&aTrack);
   if (model != nullptr)
   {
      auto milStdPrefs = wkfEnv.GetPreferenceObject<MilStdIconPrefObject>();
      auto prefObj     = wkfEnv.GetPreferenceObject<TrackVisibilityPrefObject>();
      if (milStdPrefs->GetUseSymbology())
      {
         SetMilStdIcon(*model, aTrack);
      }
      else
      {
         switch (prefObj->GetTrackIconType())
         {
         case tracks::IconType::cWEDGE_ICON:
            SetWedgeIcon(*model);
            break;
         case tracks::IconType::cCUSTOM_ICON:
            SetCustomIcon(*model);
            break;
         case tracks::IconType::cRULES_ICON:
            if (prefObj->GetPlatformIcons())
            {
               SetPlatformIcon(*model, aTrack);
            }
            else
            {
               SetDomainIcon(*model, aTrack);
            }
            break;
         }
         model->UseTeamColorUniform(true);
         // UseTeamColorUniform() currently sets the setting for all instances of that model. This is an issue when the
         // platform and track use the same model.
         if (prefObj->GetShowColor())
         {
            model->SetColor(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(aTrack.GetSide()));
         }
         else
         {
            model->SetColor(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(
               wkf::TeamVisibilityPrefData::cDEFAULT_TEAM));
         }
      }

      model->SetScale(prefObj->GetTrackScale() / 100.0f);
      // Applying GetShowDirection takes place within each application's Track plugin.

      auto*                 trackPref = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>();
      wkf::AttachmentLabel* labelPtr  = dynamic_cast<wkf::AttachmentLabel*>(aTrack.FindAttachment("label"));
      labelPtr->SetFont(trackPref->GetTrackFont());
   }
}

void wkf::TrackDisplayInterface::SetWedgeIcon(vespa::VaAttachmentModel& aModel)
{
   aModel.SetIcon(cWEDGE_STRING);
}

void wkf::TrackDisplayInterface::SetCustomIcon(vespa::VaAttachmentModel& aModel)
{
   aModel.SetIcon(cCUSTOM_STRING);
}

void wkf::TrackDisplayInterface::SetDomainIcon(vespa::VaAttachmentModel& aModel, Track& aTrack)
{
   auto prefObj = wkfEnv.GetPreferenceObject<TrackVisibilityPrefObject>();

   if (prefObj->GetDomainIconPath(aTrack.GetSpatialDomain()).isEmpty())
   {
      aModel.SetIcon(cWEDGE_STRING);
   }
   else
   {
      aModel.SetIcon(cCUSTOM_STRING + SpatialDomainToString(aTrack.GetSpatialDomain()));
   }
}

void wkf::TrackDisplayInterface::SetPlatformIcon(vespa::VaAttachmentModel& aModel, Track& aTrack)
{
   if (!(aTrack.GetTrackType().empty() || aTrack.GetTargetPlatform().empty()))
   {
      auto* scenario = vaEnv.GetStandardScenario();
      if (scenario != nullptr)
      {
         auto* platform = dynamic_cast<wkf::Platform*>(scenario->FindEntity(aTrack.GetTargetPlatform()));
         if (platform != nullptr)
         {
            aModel.SetIcon(platform->GetIcon());
            return;
         }
      }
   }

   SetDomainIcon(aModel, aTrack);
}

void wkf::TrackDisplayInterface::SetMilStdIcon(vespa::VaAttachmentModel& aModel, Track& aTrack)
{
   WkfMilStd2525d::Side         side       = WkfMilStd2525d::GetSideFromTeam(aTrack.GetSide());
   std::string                  milStdIcon = (!aTrack.GetTrackType().empty()) ? aTrack.GetIcon() : "";
   WkfMilStd2525d::PartialInput input(side, aTrack.GetSpatialDomain(), milStdIcon);
   aModel.SetIcon(wkfEnv.GetMilStdIconLoader().GetModelName(input));
   aModel.UseTeamColorUniform(false);
}
