// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvHeadDownViewPlugin.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QTime>

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "UtEntity.hpp"
#include "UtMemory.hpp"
#include "UtQtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPrefWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "hdd/WkfHDD_PrefWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvHeadsDownDisplay::Plugin,
                          "Head Down View",
                          "The Head Down View plugin provides a generic, interactive glass-cockpit head down view.",
                          "mystic",
                          false) // Plugin not loaded by default

namespace
{
constexpr const char* cHEADS_DOWN_DISPLAY = "HeadDownView";
}

RvHeadsDownDisplay::Plugin::Plugin(const QString& aPluginName, const size_t& aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new HDD::PrefWidget)
   , mInitialized(false)
{
   mPrefObjectPtr = mPrefWidgetPtr.data()->GetPreferenceObject();
}

void RvHeadsDownDisplay::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   // Check if the db is loaded and we haven't initialized.
   if (aData.GetDb()->GetPlatformCount() > 0 && !mInitialized)
   {
      mInitialized = true;
   }

   // Once we are initialized, update like normal.
   if (mInitialized)
   {
      UpdateDataContainer(aData);
   }
}

void RvHeadsDownDisplay::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   // Only build the HDD menu option for platforms.
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      wkf::Platform* platPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
      if (HasSituationAwarenessProcessor(platPtr) && mHdds.find(QString::fromStdString(platPtr->GetName())) == mHdds.end())
      {
         auto airCombatMenu = aMenu->findChild<QMenu*>("Air Combat");
         if (!airCombatMenu)
         {
            airCombatMenu = ut::qt::make_qt_ptr<QMenu>(QString("Air Combat"), aMenu);
            aMenu->addMenu(airCombatMenu);
            airCombatMenu->setObjectName("Air Combat");
         }

         wkf::Action* connectAction =
            new wkf::Action(QIcon::fromTheme("headsdown"), QString("Head Down View"), airCombatMenu);
         connectAction->setData(QString(aEntityPtr->GetName().c_str()));
         connect(connectAction, &QAction::triggered, this, &Plugin::ConnectToPlatformActionHandler);
      }
   }
}

void RvHeadsDownDisplay::Plugin::ClearScenario(bool aFullReset)
{
   // When the scenario gets reset?
   Reset();
}

void RvHeadsDownDisplay::Plugin::ConnectToPlatform(const QString& aPlatformName, QWidget* aParent)
{
   // If another HDD window has been opened for the platform with aPlatformName, delete the old one.
   if (mHdds.find(aPlatformName) != mHdds.end())
   {
      delete mHdds[aPlatformName];
      mHdds.erase(aPlatformName);
   }

   // If we don't have an HDD for this platform, connect and open one.
   if (mHdds.find(aPlatformName) == mHdds.end())
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      // Check if we have a valid scenario.
      if (scenarioPtr)
      {
         vespa::VaEntity*    attached = nullptr;
         vespa::VaEntityList entitylist;
         scenarioPtr->GetEntityList(entitylist);
         vespa::VaEntityList::iterator it;
         // Find the VaEntity
         for (it = entitylist.begin(); it != entitylist.end(); ++it)
         {
            if (QString::fromStdString((*it)->GetName()).compare(aPlatformName) == 0)
            {
               attached = (*it);
            }
         }
         // Initialize osg
         wkf::VtkEnvironment::InitializeOSG_Path();
         QPointer<HDD::Dockable> hdd = new HDD::Dockable(attached,
                                                         mPrefObjectPtr->GetResolutionX(),
                                                         mPrefObjectPtr->GetResolutionY(),
                                                         mPrefObjectPtr->GetPageList(),
                                                         wkfEnv.GetMainWindow()->centralWidget());
         connect(hdd.data(), &QWidget::destroyed, [=](QObject*) { ReleasePlatform(aPlatformName); });
         QString title = "HeadDownView: ";
         title += aPlatformName;
         hdd->setWindowTitle(title);

         // Set visibility for our window.
         if (hdd->isHidden())
         {
            hdd->setHidden(false);
         }
         hdd->show();
         hdd->raise();

         // Add it to the list of current HDDs, so it can be updated.
         mHdds.emplace(aPlatformName, hdd);
      }
   }
}

void RvHeadsDownDisplay::Plugin::ConnectToPlatformActionHandler()
{
   QAction* sender       = dynamic_cast<QAction*>(QObject::sender());
   QString  platformName = sender->data().toString();

   // Check if our sender is valid.
   if (sender)
   {
      // If the platform has a hdd, release the old one.
      if (mHdds.find(platformName) != mHdds.end())
      {
         if (mHdds.at(platformName))
         {
            ReleasePlatform(platformName);
         }
      }

      // Connect the platform to a new HDD window.
      ConnectToPlatform(platformName, wkfEnv.GetMainWindow());
   }
}

QList<wkf::PrefWidget*> RvHeadsDownDisplay::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvHeadsDownDisplay::Plugin::GuiUpdate()
{
   // If we have hdds, push current datacontainer data
   // into them so they can draw new data.
   if (mHdds.size() > 0)
   {
      for (auto hdd : mHdds)
      {
         // If we have data for this HDD.
         if (mDataContainer.GetHDDData().find(hdd.first) != mDataContainer.GetHDDData().end())
         {
            if (!mHdds.at(hdd.first).isNull())
            {
               const std::map<QString, HDD::HDD_Data>::mapped_type& headDownData =
                  mDataContainer.GetHDDData().at(hdd.first);

               const HDD::SimData::PlatformData&               platformData = headDownData.platformData;
               const HDD::SimData::TargetTrackItemList&        trackdata    = headDownData.masterTrackListData;
               const HDD::SimData::FuelData&                   fuelData     = headDownData.fuelData;
               const HDD::SimData::WeaponDataSummary&          weaponData   = headDownData.weaponData;
               const HDD::SimData::FlightControlsData&         controlsData = headDownData.flightControlsData;
               const HDD::SimData::NavData&                    navData      = headDownData.navData;
               const std::vector<HDD::SimData::TrackProcessor> tpData       = headDownData.trackProcessorList;
               const std::string                               esmTrackProcessorString = headDownData.esmTrackprocessor;
               // Note: These will be used when additional trackers are supported.
               // const std::string mwsTrackProcessorString   = headDownData.mwsTrackprocessor;
               // const std::string radarTrackProcessorString = headDownData.radarTrackprocessor;
               // const std::string irstTrackProcessorString  = headDownData.irstTrackprocessor;
               // const std::string dasTrackProcessorString   = headDownData.dasTrackprocessor;
               // const std::string flirTrackProcessorString  = headDownData.flirTrackprocessor;
               // const std::string eyeTrackProcessorString   = headDownData.eyeTrackprocessor;
               const HDD::SimData::AssetsList& assetData = headDownData.assetsList;

               mHdds[hdd.first]->SetPlatformData(platformData); // Pages needing platform data will use this for updating
               mHdds[hdd.first]->SetTrackData(
                  trackdata); // Pages needing track data (master track list) will use this for updating
               mHdds[hdd.first]->SetFuelData(fuelData);     // Pages needing fuel data will use this for updating
               mHdds[hdd.first]->SetWeaponData(weaponData); // Pages needing weapons data will use this for updating
               mHdds[hdd.first]->SetFlightControlsData(
                  controlsData); // Pages needing flight controls data will use this for updating

               // Pages needing secondary track processor data (not master track list) will use this for updating
               mHdds[hdd.first]->SetTrackProcessorData(tpData, esmTrackProcessorString);

               mHdds[hdd.first]->SetNavData(navData);          // Any pages needing nav data will use this for updating
               mHdds[hdd.first]->SetAssetsListData(assetData); // Any pages needing assets list data will use this for updating
            }
         }
      }
   }
}

void RvHeadsDownDisplay::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvHeadsDownDisplay::Plugin::ReleasePlatform(const QString& aPlatformName)
{
   if (mHdds.find(aPlatformName) != mHdds.end())
   {
      mHdds.at(aPlatformName)->hide();
      mHdds.erase(aPlatformName);
   }
}

void RvHeadsDownDisplay::Plugin::Reset()
{
   for (auto hdd : mHdds)
   {
      delete hdd.second;
   }
   mHdds.clear();

   mInitialized = false;
}

void RvHeadsDownDisplay::Plugin::UpdateDataContainer(const rv::ResultData& aData)
{
   // Get the current time.
   double simTime = aData.GetSimTime();

   // Only update the data container if we have a hdd to update.
   if (mHdds.size() > 0)
   {
      std::map<QString, HDD::HDD_Data> tempHddData;

      // For each hdd, update the data container
      for (auto hdd : mHdds)
      {
         // Get the platform from the result data.
         rv::ResultPlatform* platform = aData.FindPlatform(aData.FindPlatformIndex(hdd.first.toStdString()));
         if (platform != nullptr)
         {
            if (hdd.second != nullptr)
            {
               // Platform Status Message
               const rv::MsgPlatformStatus* platformStatusData = platform->FindFirstBefore<rv::MsgPlatformStatus>(simTime);

               // Check for a broken or removed platform here, and activate the kill frame if so.
               if (platformStatusData != nullptr)
               {
                  if (platformStatusData->broken() || platformStatusData->removed())
                  {
                     hdd.second.data()->SetConnectionDestroyed(true);
                  }
               }
               else
               {
                  hdd.second.data()->SetConnectionDestroyed(false);
               }
            }

            HDD::HDD_Data tempData;

            // Set the time
            tempData.platformData.simTime = simTime;

            // Use EntityState data first.  If there is other data, it will override with data below
            const auto entityState = platform->FindFirstBefore<rv::MsgEntityState>(rvEnv.GetData()->GetSimTime());
            if (entityState != nullptr)
            {
               rv::EntityState state = entityState->state();
               // Create a temporary entity to get the NED data (heading, pitch, roll)
               UtEntity tmpEntity;
               tmpEntity.SetLocationWCS(rv::toVec3(state.locationWCS()).GetData());
               tmpEntity.SetOrientationWCS(state.orientationWCS().x(),
                                           state.orientationWCS().y(),
                                           state.orientationWCS().z());
               tmpEntity.SetVelocityWCS(rv::toVec3(state.velocityWCS()).GetData());
               tmpEntity.SetAccelerationWCS(rv::toVec3(state.accelerationWCS()).GetData());

               double tmpHeading_rad, tmpPitch_rad, tmpRoll_rad;
               tmpEntity.GetOrientationNED(tmpHeading_rad, tmpPitch_rad, tmpRoll_rad);
               tempData.platformData.heading_deg = tmpHeading_rad * UtMath::cDEG_PER_RAD;
               tempData.platformData.pitch_deg   = tmpPitch_rad * UtMath::cRAD_PER_DEG;
               tempData.platformData.roll_deg    = tmpRoll_rad * UtMath::cRAD_PER_DEG;
            }

            // Most data will come from the SA messages
            const rv::MsgSA_FlightKinematicsData* flightData =
               platform->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aData.GetSimTime());
            const rv::MsgSA_FlightControlsData* controlsData =
               platform->FindFirstBefore<rv::MsgSA_FlightControlsData>(aData.GetSimTime());
            const rv::MsgSA_NavData*     navData  = platform->FindFirstBefore<rv::MsgSA_NavData>(aData.GetSimTime());
            const rv::MsgSA_FuelData*    fuelData = platform->FindFirstBefore<rv::MsgSA_FuelData>(aData.GetSimTime());
            const rv::MsgSA_WeaponsData* weaponsData =
               platform->FindFirstBefore<rv::MsgSA_WeaponsData>(aData.GetSimTime());
            const rv::MsgSA_TrackData* trackData = platform->FindFirstBefore<rv::MsgSA_TrackData>(aData.GetSimTime());
            const rv::MsgSA_PerceivedAssetsData* assetsData =
               platform->FindFirstBefore<rv::MsgSA_PerceivedAssetsData>(aData.GetSimTime());

            // Set the data (will override any data from above)

            if (flightData != nullptr)
            {
               tempData.platformData.platformIndex = flightData->platformIndex();
               tempData.platformData.simTime       = simTime;
               tempData.platformData.flightId      = flightData->flightId();
               tempData.platformData.idFlag        = flightData->idFlag();
               tempData.platformData.lat_deg       = flightData->lat_deg();
               tempData.platformData.lon_deg       = flightData->lon_deg();
               tempData.platformData.kcas          = flightData->kcas();
               tempData.platformData.ktas          = flightData->ktas();
               tempData.platformData.mach          = flightData->mach();
               tempData.platformData.altBaro_ft    = flightData->altBaro_ft();
               tempData.platformData.altRadar_ft   = flightData->altRadar_ft();
               tempData.platformData.vertSpd_fpm   = flightData->vertSpd_fpm();
               tempData.platformData.alpha_deg     = flightData->alpha_deg();
               tempData.platformData.beta_deg      = flightData->beta_deg();
               tempData.platformData.gLoad         = flightData->gLoad();
               tempData.platformData.gAvail        = flightData->gAvail();
               tempData.platformData.heading_deg   = flightData->heading_deg();
               tempData.platformData.pitch_deg     = flightData->pitch_deg();
               tempData.platformData.roll_deg      = flightData->roll_deg();
               tempData.platformData.gX            = flightData->gX();
               tempData.platformData.gY            = flightData->gY();
               tempData.platformData.pilotType     = flightData->pilotType();
               tempData.platformData.afterburnerOn = flightData->afterburnerOn();
               tempData.platformData.contrailing   = flightData->contrailing();
               tempData.platformData.stallWarning  = flightData->stallWarning();
               tempData.platformData.alphaValid    = flightData->alphaValid();
               tempData.platformData.betaValid     = flightData->betaValid();
               tempData.platformData.kcasValid     = flightData->kcasValid();
               tempData.platformData.nxValid       = flightData->nxValid();
               tempData.platformData.nyValid       = flightData->nyValid();
               tempData.platformData.nzValid       = flightData->nzValid();
               tempData.platformData.gAvailValid   = flightData->gAvailValid();
            }

            if (controlsData != nullptr)
            {
               tempData.flightControlsData.platformIndex           = controlsData->platformIndex();
               tempData.flightControlsData.stickRgtInput           = controlsData->stickRgtInput();
               tempData.flightControlsData.stickBackInput          = controlsData->stickBackInput();
               tempData.flightControlsData.rudderRgtInput          = controlsData->rudderRgtInput();
               tempData.flightControlsData.speedBrakeInput         = controlsData->speedBrakeInput();
               tempData.flightControlsData.throttleInput           = controlsData->throttleInput();
               tempData.flightControlsData.aileronRgtNormalized    = controlsData->aileronRgtNormalized();
               tempData.flightControlsData.aileronLftNormalized    = controlsData->aileronLftNormalized();
               tempData.flightControlsData.flapRgtNormalized       = controlsData->flapRgtNormalized();
               tempData.flightControlsData.flapLftNormalized       = controlsData->flapLftNormalized();
               tempData.flightControlsData.stabilizerRgtNormalized = controlsData->stabilizerRgtNormalized();
               tempData.flightControlsData.stabilizerLftNormalized = controlsData->stabilizerLftNormalized();
               tempData.flightControlsData.rudderRgtNormalized     = controlsData->rudderRgtNormalized();
               tempData.flightControlsData.rudderLftNormalized     = controlsData->rudderLftNormalized();
               tempData.flightControlsData.rudderNormalized        = controlsData->rudderNormalized();
               tempData.flightControlsData.speedBrakeNormalized    = controlsData->speedBrakeNormalized();
               tempData.flightControlsData.thrustNormalized        = controlsData->thrustNormalized();
               tempData.flightControlsData.landingGear             = controlsData->landingGear();
               tempData.flightControlsData.throttleValid           = controlsData->throttleValid();
               tempData.flightControlsData.aileronsValid           = controlsData->aileronsValid();
               tempData.flightControlsData.flapsValid              = controlsData->flapsValid();
               tempData.flightControlsData.twinRuddersValid        = controlsData->twinRuddersValid();
               tempData.flightControlsData.singleRudderValid       = controlsData->singleRudderValid();
            }
            else
            {
               tempData.flightControlsData.platformIndex           = 0;
               tempData.flightControlsData.stickRgtInput           = 0.0;
               tempData.flightControlsData.stickBackInput          = 0.0;
               tempData.flightControlsData.rudderRgtInput          = 0.0;
               tempData.flightControlsData.speedBrakeInput         = 0.0;
               tempData.flightControlsData.throttleInput           = 0.0;
               tempData.flightControlsData.aileronRgtNormalized    = 0.0;
               tempData.flightControlsData.aileronLftNormalized    = 0.0;
               tempData.flightControlsData.flapRgtNormalized       = 0.0;
               tempData.flightControlsData.flapLftNormalized       = 0.0;
               tempData.flightControlsData.stabilizerRgtNormalized = 0.0;
               tempData.flightControlsData.stabilizerLftNormalized = 0.0;
               tempData.flightControlsData.rudderRgtNormalized     = 0.0;
               tempData.flightControlsData.rudderLftNormalized     = 0.0;
               tempData.flightControlsData.rudderNormalized        = 0.0;
               tempData.flightControlsData.speedBrakeNormalized    = 0.0;
               tempData.flightControlsData.thrustNormalized        = 0.0;
               tempData.flightControlsData.landingGear             = 0.0;
               tempData.flightControlsData.throttleValid           = false;
               tempData.flightControlsData.aileronsValid           = false;
               tempData.flightControlsData.flapsValid              = false;
               tempData.flightControlsData.twinRuddersValid        = false;
               tempData.flightControlsData.singleRudderValid       = false;
            }

            if (navData != nullptr)
            {
               tempData.navData.platformIndex  = navData->platformIndex();
               tempData.navData.wayptDataValid = navData->wayptDataValid();

               tempData.navData.waypointData.numWaypoints          = navData->waypointData().numWaypoints();
               tempData.navData.waypointData.currentWaypoint       = navData->waypointData().currentWaypoint();
               tempData.navData.waypointData.headingBug_deg        = navData->waypointData().headingBug_deg();
               tempData.navData.waypointData.distanceToWaypoint_nm = navData->waypointData().distanceToWaypoint_nm();
               tempData.navData.waypointData.timeToWaypoint_sec    = navData->waypointData().timeToWaypoint_sec();

               for (auto& wp : navData->waypointData().waypoints())
               {
                  HDD::SimData::WaypointLatLon wayPt;
                  wayPt.lat_deg = wp.lat_deg();
                  wayPt.lon_deg = wp.lon_deg();

                  tempData.navData.waypointData.waypoints.push_back(wayPt);
               }
            }
            else
            {
               tempData.navData.platformIndex                      = 0;
               tempData.navData.wayptDataValid                     = false;
               tempData.navData.waypointData.numWaypoints          = 0;
               tempData.navData.waypointData.currentWaypoint       = 0;
               tempData.navData.waypointData.headingBug_deg        = 0.0;
               tempData.navData.waypointData.distanceToWaypoint_nm = 0.0;
               tempData.navData.waypointData.timeToWaypoint_sec    = 0.0;

               tempData.navData.waypointData.waypoints.clear();
            }

            if (fuelData != nullptr)
            {
               tempData.fuelData.platformIndex     = fuelData->platformIndex();
               tempData.fuelData.fuelInternal_lbs  = fuelData->fuelInternal_lbs();
               tempData.fuelData.fuelExternal_lbs  = fuelData->fuelExternal_lbs();
               tempData.fuelData.fuelCapInt_lbs    = fuelData->fuelCapInt_lbs();
               tempData.fuelData.fuelCapExt_lbs    = fuelData->fuelCapExt_lbs();
               tempData.fuelData.grossWgt_lbs      = fuelData->grossWgt_lbs();
               tempData.fuelData.joker_lbs         = fuelData->joker_lbs();
               tempData.fuelData.bingo_lbs         = fuelData->bingo_lbs();
               tempData.fuelData.fuelFlow_pph      = fuelData->fuelFlow_pph();
               tempData.fuelData.normalizedFuelQty = fuelData->normalizedFuelQty();
               tempData.fuelData.numExtTanks       = fuelData->numExtTanks();
               tempData.fuelData.dropTanksPresent  = fuelData->dropTanksPresent();
               tempData.fuelData.fuelValid         = fuelData->fuelValid();
            }
            else
            {
               tempData.fuelData.platformIndex     = 0;
               tempData.fuelData.fuelInternal_lbs  = 0.0;
               tempData.fuelData.fuelExternal_lbs  = 0.0;
               tempData.fuelData.fuelCapInt_lbs    = 0.0;
               tempData.fuelData.fuelCapExt_lbs    = 0.0;
               tempData.fuelData.grossWgt_lbs      = 0.0;
               tempData.fuelData.joker_lbs         = 0.0;
               tempData.fuelData.bingo_lbs         = 0.0;
               tempData.fuelData.fuelFlow_pph      = 0.0;
               tempData.fuelData.normalizedFuelQty = 0.0;
               tempData.fuelData.numExtTanks       = 0;
               tempData.fuelData.dropTanksPresent  = false;
               tempData.fuelData.fuelValid         = false;
            }

            if (weaponsData != nullptr)
            {
               tempData.weaponData.platformIndex      = weaponsData->platformIndex();
               tempData.weaponData.selectedWeapon     = weaponsData->selectedWeapon();
               tempData.weaponData.numSelectedWeapon  = weaponsData->numSelectedWeapon();
               tempData.weaponData.numChaff           = weaponsData->numChaff();
               tempData.weaponData.numFlares          = weaponsData->numFlares();
               tempData.weaponData.numDecoys          = weaponsData->numDecoys();
               tempData.weaponData.weaponBayDoorsOpen = weaponsData->weaponBayDoorsOpen();
               tempData.weaponData.supportingWeapon   = weaponsData->supportingWeapon();
               tempData.weaponData.masterWarning      = weaponsData->masterWarning();
               tempData.weaponData.masterCaution      = weaponsData->masterCaution();
               tempData.weaponData.shootCueActive     = weaponsData->shootCueActive();
               tempData.weaponData.shootCueBlink      = weaponsData->shootCueBlink();
               tempData.weaponData.masterArmActive    = weaponsData->masterArmActive();
               tempData.weaponData.jammingDetected    = weaponsData->jammingDetected();

               if (!weaponsData->weaponList().empty())
               {
                  for (auto& wpn : weaponsData->weaponList())
                  {
                     HDD::SimData::WeaponNameQty weapon;
                     weapon.quantity   = wpn.quantity();
                     weapon.weaponName = wpn.weaponName();

                     tempData.weaponData.weaponList.push_back(weapon);
                  }
               }
               else
               {
                  tempData.weaponData.weaponList.clear();
               }

               if (!weaponsData->engagedTargetList().empty())
               {
                  for (auto& tgt : weaponsData->engagedTargetList())
                  {
                     HDD::SimData::EngagedTargetData target;

                     target.targetIndex          = tgt.targetIndex();
                     target.rangeMax_nm          = tgt.rangeMax_nm();
                     target.rangeNe_nm           = tgt.rangeNe_nm();
                     target.rangeMin_nm          = tgt.rangeMin_nm();
                     target.rangeCurrent_nm      = tgt.rangeCurrent_nm();
                     target.rangeClosureRate_kts = tgt.rangeClosureRate_kts();
                     target.timeToIntercept_sec  = tgt.timeToIntercept_sec();
                     target.timeToActive_sec     = tgt.timeToActive_sec();
                     target.targetKTAS           = tgt.targetKTAS();
                     target.targetAspect_deg     = tgt.targetAspect_deg();
                     target.targetAz_deg         = tgt.targetAz_deg();
                     target.targetEl_deg         = tgt.targetEl_deg();

                     tempData.weaponData.engagedTargetList.push_back(target);
                  }
               }
               else
               {
                  tempData.weaponData.engagedTargetList.clear();
               }
            }
            else
            {
               tempData.weaponData.platformIndex      = 0;
               tempData.weaponData.selectedWeapon     = "";
               tempData.weaponData.numSelectedWeapon  = 0;
               tempData.weaponData.numChaff           = 0;
               tempData.weaponData.numFlares          = 0;
               tempData.weaponData.numDecoys          = 0;
               tempData.weaponData.weaponBayDoorsOpen = false;
               tempData.weaponData.supportingWeapon   = false;
               tempData.weaponData.masterWarning      = false;
               tempData.weaponData.masterCaution      = false;
               tempData.weaponData.shootCueActive     = false;
               tempData.weaponData.shootCueBlink      = false;
               tempData.weaponData.masterArmActive    = false;
               tempData.weaponData.jammingDetected    = false;

               tempData.weaponData.weaponList.clear();
               tempData.weaponData.engagedTargetList.clear();
            }

            if (trackData != nullptr)
            {
               // Loop through the track processors
               for (auto& trkProc : trackData->trackProcessors())
               {
                  // Temporary track processor (tp)
                  HDD::SimData::TrackProcessor tp;

                  // Set the name
                  tp.trackProcessorName = trkProc.trackProcessorName();

                  // Set the master flag
                  tp.isMasterProcessor = trkProc.isMasterProcessor();

                  for (auto& trk : trkProc.tracks())
                  {
                     HDD::SimData::TargetTrackItem track;

                     track.tgtPlatformIndex = trk.targetPlatformIndex();
                     track.tgtLat_deg       = trk.lat_deg();
                     track.tgtLon_deg       = trk.lon_deg();
                     track.tgtAlt_ft        = trk.altitude_ft();
                     track.tgtBearing_rad   = trk.bearing_deg() * UtMath::cRAD_PER_DEG;
                     track.tgtElevation_rad = trk.elevation_deg() * UtMath::cRAD_PER_DEG;
                     track.tgtRange_nm      = trk.range_nm();
                     track.tgtRangeRate_kts = trk.rangeRate_kts();
                     track.tgtSpeed_kts     = trk.targetSpeed_kts();
                     track.tgtHdg_deg       = trk.targetHdg_deg();
                     track.tgtAspect_deg    = trk.targetAspect_deg();
                     track.tgtIdString      = trk.idString();
                     track.tgtPlatformName  = trk.tgtPlatformName();
                     track.tgtTypeName      = trk.tgtTypeName();
                     track.identification =
                        static_cast<HDD::SimData::Identification>(static_cast<int>(trk.identification()));
                     track.latLonValid       = trk.latLonValid();
                     track.altitudeValid     = trk.altitudeValid();
                     track.bearingValid      = trk.bearingValid();
                     track.elevationValid    = trk.elevationValid();
                     track.rangeValid        = trk.rangeValid();
                     track.rangeRateValid    = trk.rangeRateValid();
                     track.targetSpeedValid  = trk.targetSpeedValid();
                     track.targetHdgValid    = trk.targetHdgValid();
                     track.targetAspectValid = trk.targetAspectValid();
                     track.idIffValid        = trk.idIffValid();
                     track.idAuxValid        = trk.idAuxValid();
                     track.idSideValid       = trk.idSideValid();
                     track.idTypeValid       = trk.idTypeValid();
                     track.isMissile         = trk.isMissile();

                     // Add to the list of tracks for this track processor
                     tp.trackList.mTracks.emplace(track.tgtPlatformIndex, track);
                  }

                  // If this is the MTP, save the data into the masterTrackListData
                  if (tp.isMasterProcessor)
                  {
                     tempData.masterTrackListData = tp.trackList;
                  }

                  // Add this track processor to the list
                  tempData.trackProcessorList.emplace_back(tp);
               }

               tempData.esmTrackprocessor   = trackData->esmTrackProcessor();
               tempData.mwsTrackprocessor   = trackData->mwsTrackProcessor();
               tempData.radarTrackprocessor = trackData->radarTrackProcessor();
               tempData.irstTrackprocessor  = trackData->irstTrackProcessor();
               tempData.dasTrackprocessor   = trackData->dasTrackProcessor();
               tempData.flirTrackprocessor  = trackData->flirTrackProcessor();
               tempData.eyesTrackprocessor  = trackData->eyesTrackProcessor();
            }

            if (assetsData != nullptr)
            {
               tempData.assetsList.platformIndex = assetsData->platformIndex();

               for (auto& asset : assetsData->assetsList())
               {
                  HDD::SimData::EntityPerceptionData entity;

                  entity.lat_deg        = asset.lat_deg();
                  entity.lon_deg        = asset.lon_deg();
                  entity.altitude_ft    = asset.altitude_ft();
                  entity.bearing_deg    = asset.bearing_deg();
                  entity.speed_kts      = asset.speed_kts();
                  entity.heading_deg    = asset.heading_deg();
                  entity.threatLevel    = asset.threatLevel();
                  entity.targetValue    = asset.targetValue();
                  entity.risk           = asset.risk();
                  entity.defensiveness  = asset.defensiveness();
                  entity.urgency        = asset.urgency();
                  entity.idString       = asset.idString();
                  entity.perceivedIndex = asset.perceivedIndex();
                  entity.perceivedName  = asset.perceivedName();
                  entity.perceivedType  = asset.perceivedType();
                  entity.flightId       = static_cast<unsigned short>(asset.flightId());
                  entity.idFlag         = asset.idFlag();
                  entity.identification = static_cast<HDD::SimData::Identification>(asset.identification());
                  entity.friendlyAsset  = asset.friendlyAsset();
                  entity.angleOnly      = asset.angleOnly();
                  entity.idIffFriend    = asset.idIffFriend();
                  entity.idIffFoe       = asset.idAuxFoe();
                  entity.idIffNeutral   = asset.idAuxNeutral();
                  entity.idIffUnknown   = asset.idAuxUnknown();
                  entity.idAuxFriend    = asset.idAuxFriend();
                  entity.idAuxFoe       = asset.idAuxFoe();
                  entity.idAuxNeutral   = asset.idAuxNeutral();
                  entity.idAuxUnknown   = asset.idAuxUnknown();
                  entity.idSideFriend   = asset.idSideFriend();
                  entity.idSideFoe      = asset.idSideFoe();
                  entity.idSideNeutral  = asset.idSideNeutral();
                  entity.idSideUnknown  = asset.idSideUnknown();
                  entity.idTypeFriend   = asset.idTypeFriend();
                  entity.idTypeFoe      = asset.idTypeFoe();
                  entity.idTypeNeutral  = asset.idTypeNeutral();
                  entity.idTypeUnknown  = asset.idTypeUnknown();
                  entity.isHostile      = asset.isHostile();
                  entity.altitudeValid  = asset.altitudeValid();
                  entity.speedValid     = asset.speedValid();
                  entity.headingValid   = asset.headingValid();
                  entity.isMissile      = asset.isMissile();

                  // Add to the list
                  tempData.assetsList.assetList.emplace_back(entity);
               }
            }

            // Add this hdd tempData to the list
            tempHddData.emplace(QString::fromStdString(platform->GetName()), tempData);

         } // if (platform != nullptr)
      }    // for (auto hdd : mHdds)

      // Update our HDD data
      mDataContainer.SetHDDData(tempHddData);
   } // if (mHdds.size() > 0)
}

bool RvHeadsDownDisplay::Plugin::HasSituationAwarenessProcessor(wkf::Platform* aPlatform)
{
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         auto platform = currentData->FindPlatform(aPlatform->GetIndex());
         if (platform != nullptr)
         {
            return HasSA_Data(platform, currentData->GetSimTime());
         }
      }
   }
   return false;
}

const bool RvHeadsDownDisplay::Plugin::HasSA_Data(rv::ResultPlatform* aPlatformPtr, const double aSimTime)
{
   // If the plaform has any SA messages, then it has SA data.  Check if the platform has SA messages.
   auto eData = aPlatformPtr->FindFirstBefore<rv::MsgSA_EngagementSummaryData>(aSimTime);
   if (eData == nullptr)
   {
      eData = aPlatformPtr->FindFirstAfter<rv::MsgSA_EngagementSummaryData>(aSimTime);
   }
   if (eData != nullptr)
   {
      return true;
   }

   auto fData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FlightControlsData>(aSimTime);
   if (fData == nullptr)
   {
      fData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FlightControlsData>(aSimTime);
   }
   if (fData != nullptr)
   {
      return true;
   }

   auto kData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aSimTime);
   if (kData == nullptr)
   {
      kData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FlightKinematicsData>(aSimTime);
   }
   if (kData != nullptr)
   {
      return true;
   }

   auto fuelData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FuelData>(aSimTime);
   if (fuelData == nullptr)
   {
      fuelData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FuelData>(aSimTime);
   }
   if (fuelData != nullptr)
   {
      return true;
   }

   auto gData = aPlatformPtr->FindFirstBefore<rv::MsgSA_GroupsData>(aSimTime);
   if (gData == nullptr)
   {
      gData = aPlatformPtr->FindFirstAfter<rv::MsgSA_GroupsData>(aSimTime);
   }
   if (gData != nullptr)
   {
      return true;
   }

   auto nData = aPlatformPtr->FindFirstBefore<rv::MsgSA_NavData>(aSimTime);
   if (nData == nullptr)
   {
      nData = aPlatformPtr->FindFirstAfter<rv::MsgSA_NavData>(aSimTime);
   }
   if (nData != nullptr)
   {
      return true;
   }

   auto paData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PerceivedAssetsData>(aSimTime);
   if (paData == nullptr)
   {
      paData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PerceivedAssetsData>(aSimTime);
   }
   if (paData != nullptr)
   {
      return true;
   }

   auto pbabData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PerceivedBogiesAndBanditsData>(aSimTime);
   if (pbabData == nullptr)
   {
      pbabData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PerceivedBogiesAndBanditsData>(aSimTime);
   }
   if (pbabData != nullptr)
   {
      return true;
   }

   auto ptatData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PrioritizedThreatsAndTargetsData>(aSimTime);
   if (ptatData == nullptr)
   {
      ptatData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PrioritizedThreatsAndTargetsData>(aSimTime);
   }
   if (ptatData != nullptr)
   {
      return true;
   }

   auto tData = aPlatformPtr->FindFirstBefore<rv::MsgSA_TrackData>(aSimTime);
   if (tData == nullptr)
   {
      tData = aPlatformPtr->FindFirstAfter<rv::MsgSA_TrackData>(aSimTime);
   }
   if (tData != nullptr)
   {
      return true;
   }

   auto wData = aPlatformPtr->FindFirstBefore<rv::MsgSA_WeaponsData>(aSimTime);
   if (wData == nullptr)
   {
      wData = aPlatformPtr->FindFirstAfter<rv::MsgSA_WeaponsData>(aSimTime);
   }
   if (wData != nullptr)
   {
      return true;
   }

   return false;
}
