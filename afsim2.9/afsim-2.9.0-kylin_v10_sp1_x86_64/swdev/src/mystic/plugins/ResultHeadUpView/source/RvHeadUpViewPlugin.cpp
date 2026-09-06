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

#include "RvHeadUpViewPlugin.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvInterpolationPrefObject.hpp"
#include "RvP6dofEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvWkEventPipeClasses.hpp"
#include "UtAirspeed.hpp"
#include "UtQtMemory.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "hud/WkfHUD_Overlay.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   RvHeadsUpDisplay::Plugin,
   "Head Up View",
   "The Head Up View plugin provides an out-the-window (OTW) display with a generic Head Up Display (HUD).",
   "mystic",
   false) // Plugin not loaded by default

namespace
{
constexpr const char* cHEADS_UP_VIEW = "HeadUpView";
}

RvHeadsUpDisplay::Plugin::Plugin(const QString& aPluginName, const size_t& aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new wkf::HUD_PrefWidget)
{
   mPrefObjectPtr = mPrefWidgetPtr.data()->GetPreferenceObject();
}

void RvHeadsUpDisplay::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   UpdateDataContainer(aData);
}

void RvHeadsUpDisplay::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   // Check if our entity is a platform
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      wkf::Platform* platPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
      if (HasSituationAwarenessProcessor(platPtr) && mHuds.find(QString::fromStdString(platPtr->GetName())) == mHuds.end())
      {
         auto airCombatMenu = aMenu->findChild<QMenu*>("Air Combat");
         if (!airCombatMenu)
         {
            airCombatMenu = ut::qt::make_qt_ptr<QMenu>(QString("Air Combat"), aMenu);
            aMenu->addMenu(airCombatMenu);
            airCombatMenu->setObjectName("Air Combat");
         }

         QString      name          = aEntityPtr->GetName().c_str();
         wkf::Action* connectAction = new wkf::Action(QIcon::fromTheme("headsup"), QString("Head Up View"), airCombatMenu);
         connectAction->setData(name);
         connect(connectAction, &QAction::triggered, this, &Plugin::ConnectToPlatform);
      }
   }
}

void RvHeadsUpDisplay::Plugin::ClearScenario(bool aFullReset)
{
   Reset();
}

void RvHeadsUpDisplay::Plugin::ConnectToPlatform()
{
   QAction* sender       = dynamic_cast<QAction*>(QObject::sender());
   QString  platformName = sender->data().toString();

   /** Check if our sender is valid. */
   if (sender)
   {
      /** If the platform has a hud, release the old one. */
      if (mHuds.find(platformName) != mHuds.end())
      {
         if (mHuds.at(platformName))
         {
            ReleasePlatform(mHuds.at(platformName));
         }
      }

      // Create the HUD widgets
      QPointer<wkf::HUD_DockWidget> aDockWidget =
         new wkf::HUD_DockWidget(platformName, wkfEnv.GetMainWindow()->centralWidget());

      // Docking and immediately floating the window will guarantee the dock widget has a title bar.
      aDockWidget->setFloating(false);
      aDockWidget->setFloating(true);

      // Set the dock widget's geometry to spawn at the same location as the main window.
      aDockWidget->setGeometry(QRect{wkfEnv.GetMainWindow()->geometry().x(), wkfEnv.GetMainWindow()->geometry().y(), 0, 0});

      // Set the width and height of the dock widget to that which is specified in the preferences object.
      unsigned int width  = mPrefObjectPtr->GetResolutionX() / QApplication::desktop()->devicePixelRatio();
      unsigned int height = mPrefObjectPtr->GetResolutionY() / QApplication::desktop()->devicePixelRatio();
      aDockWidget->SetDefaultWidth(width);
      aDockWidget->SetDefaultHeight(height);

      connect(aDockWidget.data(),
              &wkf::ImmersiveViewDockWidget::ImmersiveViewClosed,
              [this, aDockWidget]() { ReleasePlatform(aDockWidget); });
      aDockWidget->setWindowTitle(platformName);

      // If this check is not here, HUD will stop loading after being opened ~4 times.
      if (aDockWidget->IsInitialized())
      {
         ViewerInitializedHandler(aDockWidget);
      }
      else
      {
         connect(aDockWidget.data(),
                 &wkf::HUD_DockWidget::ImmersiveViewInitialized,
                 [this, aDockWidget]() { ViewerInitializedHandler(aDockWidget); });
      }

      // Add our dock widget(HUD) to the list of HUDs
      mHuds.emplace(platformName, aDockWidget);
   }
}

void RvHeadsUpDisplay::Plugin::FirstPerson(bool aState)
{
   QAction* sender = dynamic_cast<QAction*>(QObject::sender());

   // If the sender is valid, update the checkbox and set first person for the sender.
   if (sender)
   {
      sender->blockSignals(true);
      sender->setChecked(aState);
      sender->blockSignals(false);
      mHuds[sender->objectName()].data()->SetFirstPerson(aState);
   }
}

QList<wkf::PrefWidget*> RvHeadsUpDisplay::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvHeadsUpDisplay::Plugin::GuiUpdate()
{
   for (auto hud : mHuds)
   {
      UpdateDataForHud(hud.second.data());
   }
}

void RvHeadsUpDisplay::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvHeadsUpDisplay::Plugin::ReleasePlatform(QPointer<wkf::HUD_DockWidget> aDockWidget)
{
   // Find the HUD in the HUD list, and remove it.
   mHuds.at(aDockWidget.data()->GetConnectedPlatformName())->hide();
   mHuds.erase(aDockWidget.data()->GetConnectedPlatformName());
}

void RvHeadsUpDisplay::Plugin::Reset()
{
   for (auto hud : mHuds)
   {
      delete hud.second;
   }
   mHuds.clear();
}

void RvHeadsUpDisplay::Plugin::ShowHUD(bool aState)
{
   QAction* sender = dynamic_cast<QAction*>(QObject::sender());

   // If the sender is valid, update the checkbox and show/hide the HUD for the sender.
   if (sender)
   {
      sender->blockSignals(true);
      sender->setChecked(aState);
      sender->blockSignals(false);
      mHuds[sender->objectName()].data()->ShowHUD(aState);
   }
}

void RvHeadsUpDisplay::Plugin::UpdateDataContainer(const rv::ResultData& aData)
{
   float simTime = aData.GetSimTime();

   // Use a temporary data container for the sim event below
   std::map<std::string, wkf::HUD_DataContainer::PlatformData> tempPlatDatas;
   // Setup a temporary platform data structure to load with data
   wkf::HUD_DataContainer::PlatformData tempPlatData;

   for (std::map<QString, QPointer<wkf::HUD_DockWidget>>::iterator it = mHuds.begin(); it != mHuds.end(); it++)
   {
      const QString& aConnectedPlatform = it->first;

      // If our connected platform is a non-empty name
      if (!aConnectedPlatform.isEmpty())
      {
         // Get the platform
         rv::ResultPlatform* platform = aData.FindPlatform(aData.FindPlatformIndex(aConnectedPlatform.toStdString()));

         // If we have a resulting platform that we can get data from
         if (platform != nullptr)
         {
            // Platform Status Message
            const rv::MsgPlatformStatus* platformStatusData = platform->FindFirstBefore<rv::MsgPlatformStatus>(simTime);

            // Check for a broken or removed platform here, and activate the kill frame if so.
            if (platformStatusData != nullptr)
            {
               if (platformStatusData->broken() || platformStatusData->removed())
               {
                  mHuds[aConnectedPlatform].data()->GetHUD()->SetConnectionDestroyed(true);
               }
            }
            else
            {
               mHuds[aConnectedPlatform].data()->GetHUD()->SetConnectionDestroyed(false);
            }

            // HUD Message
            const rv::MsgHUD_Data* hudData = platform->FindFirstBefore<rv::MsgHUD_Data>(simTime);

            // If we have HUD Data, use it.
            if (hudData != nullptr)
            {
               mHuds[aConnectedPlatform]->GetHUD()->SetHudMode(
                  static_cast<wkf::HUD_DataContainer::eHudMode>(hudData->hudMode()));
            }

            // Use EntityState data first.  If there is other data, it will override with data below
            auto state = platform->FindFirstBefore<rv::MsgEntityState>(aData.GetSimTime());
            if (state != nullptr)
            {
               // Get the lat and lon from stateData (using code from RvPlatformDataInterface)
               UtEntity tmpEntity;
               tmpEntity.SetLocationWCS(rv::toVec3(state->state().locationWCS()).GetData());
               tmpEntity.SetOrientationWCS(state->state().orientationWCS().x(),
                                           state->state().orientationWCS().y(),
                                           state->state().orientationWCS().z());
               tmpEntity.SetVelocityWCS(rv::toVec3(state->state().velocityWCS()).GetData());
               tmpEntity.SetAccelerationWCS(rv::toVec3(state->state().accelerationWCS()).GetData());

               const rv::EntityState&    eState = state->state();
               const rv::MsgEntityState* nState = nullptr;
               nState                           = platform->FindFirstBefore<rv::MsgEntityState>(aData.GetSimTime());

               float  elapsed = aData.GetSimTime() - state->simTime();
               double orientationWCS[3];
               tmpEntity.GetOrientationWCS(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
               if (nState && wkfEnv.GetPreferenceObject<rv::InterpolationPrefObject>()->GetAngularInterpolation() &&
                   (nState->simTime() != state->simTime()))
               {
                  float        factor = elapsed / (nState->simTime() - state->simTime());
                  UtQuaternion startQ(eState.orientationWCS().x(), eState.orientationWCS().y(), eState.orientationWCS().z());
                  UtQuaternion nextQ(nState->state().orientationWCS().x(),
                                     nState->state().orientationWCS().y(),
                                     nState->state().orientationWCS().z());
                  UtQuaternion interpQ = UtQuaternion::Slerp(startQ, nextQ, factor);
                  interpQ.Get(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
                  tmpEntity.SetOrientationWCS(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
                  // build a quaternion and apply it to the orientation
               }

               double yaw, pitch, roll;
               tmpEntity.GetOrientationNED(yaw, pitch, roll);
               tempPlatData.heading_deg = yaw * UtMath::cDEG_PER_RAD;
               tempPlatData.pitch_deg   = pitch * UtMath::cDEG_PER_RAD;
               tempPlatData.roll_deg    = roll * UtMath::cDEG_PER_RAD;
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

            // Set the data (will override any data from above)

            if (flightData != nullptr)
            {
               tempPlatData.simTime = simTime;
               tempPlatData.lat_deg = flightData->lat_deg();
               tempPlatData.lon_deg = flightData->lon_deg();

               if (flightData->kcasValid())
               {
                  tempPlatData.kcas = flightData->kcas();
               }
               else
               {
                  tempPlatData.kcas = 0.0;
               }

               tempPlatData.ktas              = flightData->ktas();
               tempPlatData.mach              = flightData->mach();
               tempPlatData.altitudeMSL_ft    = flightData->altBaro_ft();
               tempPlatData.radarAltitude_ft  = flightData->altRadar_ft();
               tempPlatData.verticalSpeed_fpm = flightData->vertSpd_fpm();

               if (flightData->alphaValid())
               {
                  tempPlatData.alpha_deg = flightData->alpha_deg();
               }
               else
               {
                  tempPlatData.alpha_deg = 0.0;
               }

               if (flightData->betaValid())
               {
                  tempPlatData.beta_deg = flightData->beta_deg();
               }
               else
               {
                  tempPlatData.beta_deg = 0.0;
               }

               if (flightData->nzValid())
               {
                  tempPlatData.gLoad = flightData->gLoad();
               }
               else
               {
                  tempPlatData.gLoad = 0.0;
               }

               if (flightData->gAvailValid())
               {
                  tempPlatData.gAvail = flightData->gAvail();
               }
               else
               {
                  tempPlatData.gAvail = 0.0;
               }

               tempPlatData.heading_deg = flightData->heading_deg();
               tempPlatData.pitch_deg   = flightData->pitch_deg();
               tempPlatData.roll_deg    = flightData->roll_deg();

               if (flightData->nxValid())
               {
                  tempPlatData.gX = flightData->gX();
               }
               else
               {
                  tempPlatData.gX = 0.0;
               }

               if (flightData->nyValid())
               {
                  tempPlatData.gY = flightData->gY();
               }
               else
               {
                  tempPlatData.gY = 0.0;
               }

               tempPlatData.pilotType     = flightData->pilotType();
               tempPlatData.afterburnerOn = flightData->afterburnerOn();
               tempPlatData.contrailing   = flightData->contrailing();
               tempPlatData.stallWarning  = flightData->stallWarning();
               tempPlatData.alphaValid    = flightData->alphaValid();
               tempPlatData.betaValid     = flightData->betaValid();
               tempPlatData.kcasValid     = flightData->kcasValid();
               tempPlatData.nxValid       = flightData->nxValid();
               tempPlatData.nyValid       = flightData->nyValid();
               tempPlatData.nzValid       = flightData->nzValid();
               tempPlatData.gAvailValid   = flightData->gAvailValid();
            }

            if (controlsData != nullptr)
            {
               tempPlatData.speedBrakeInput      = controlsData->speedBrakeInput();
               tempPlatData.speedBrakeNormalized = controlsData->speedBrakeNormalized();
               tempPlatData.throttleInput        = controlsData->throttleInput();
               tempPlatData.thrustNormalized     = controlsData->thrustNormalized();
               tempPlatData.throttleValid        = controlsData->throttleValid();

               if (controlsData->throttleValid())
               {
                  if (tempPlatData.thrustNormalized > 1.0)
                  {
                     tempPlatData.engineThrottleNormalizedMilitary    = 1.0;
                     tempPlatData.engineThrottleNormalizedAfterburner = tempPlatData.thrustNormalized - 1.0;
                  }
                  else
                  {
                     tempPlatData.engineThrottleNormalizedMilitary    = tempPlatData.thrustNormalized;
                     tempPlatData.engineThrottleNormalizedAfterburner = 0.0;
                  }

                  if (tempPlatData.throttleInput > 1.0)
                  {
                     tempPlatData.throttleMilitaryControllerPosition    = 1.0;
                     tempPlatData.throttleAfterburnerControllerPosition = tempPlatData.throttleInput - 1.0;
                  }
                  else
                  {
                     tempPlatData.throttleMilitaryControllerPosition    = tempPlatData.throttleInput;
                     tempPlatData.throttleAfterburnerControllerPosition = 0.0;
                  }
               }
               else
               {
                  tempPlatData.engineThrottleNormalizedMilitary      = 0.0;
                  tempPlatData.engineThrottleNormalizedAfterburner   = 0.0;
                  tempPlatData.throttleMilitaryControllerPosition    = 0.0;
                  tempPlatData.throttleAfterburnerControllerPosition = 0.0;
               }
            }
            else
            {
               tempPlatData.speedBrakeInput      = 0.0;
               tempPlatData.speedBrakeNormalized = 0.0;
               tempPlatData.throttleInput        = 0.0;
               tempPlatData.thrustNormalized     = 0.0;
               tempPlatData.throttleValid        = false;

               tempPlatData.engineThrottleNormalizedMilitary      = 0.0;
               tempPlatData.engineThrottleNormalizedAfterburner   = 0.0;
               tempPlatData.throttleMilitaryControllerPosition    = 0.0;
               tempPlatData.throttleAfterburnerControllerPosition = 0.0;
            }

            if (navData != nullptr)
            {
               tempPlatData.waypoints.clear();

               tempPlatData.numWaypoints          = static_cast<size_t>(navData->waypointData().numWaypoints());
               tempPlatData.currentWaypoint       = static_cast<size_t>(navData->waypointData().currentWaypoint());
               tempPlatData.headingBug_deg        = navData->waypointData().headingBug_deg();
               tempPlatData.distanceToWaypoint_nm = navData->waypointData().distanceToWaypoint_nm();
               tempPlatData.timeToWaypoint_sec    = navData->waypointData().timeToWaypoint_sec();
               tempPlatData.wayptDataValid        = navData->wayptDataValid();

               for (auto& wp : navData->waypointData().waypoints())
               {
                  wkf::HUD_DataContainer::HudWaypointData hudWp;
                  hudWp.lat_deg = wp.lat_deg();
                  hudWp.lon_deg = wp.lon_deg();
                  tempPlatData.waypoints.push_back(hudWp);
               }
            }
            else
            {
               tempPlatData.waypoints.clear();
               tempPlatData.numWaypoints          = 0;
               tempPlatData.currentWaypoint       = 0;
               tempPlatData.headingBug_deg        = 0.0;
               tempPlatData.distanceToWaypoint_nm = 0.0;
               tempPlatData.timeToWaypoint_sec    = 0.0;
               tempPlatData.wayptDataValid        = false;
            }

            if (fuelData != nullptr)
            {
               tempPlatData.currentInternalFuelQuantity_lbs = fuelData->fuelInternal_lbs();
               tempPlatData.currentExternalFuelQuantity_lbs = fuelData->fuelExternal_lbs();
               tempPlatData.currentTotalFuelQuantity_lbs = fuelData->fuelInternal_lbs() + fuelData->fuelExternal_lbs();
               tempPlatData.internalFuelCapacity_lbs     = fuelData->fuelCapInt_lbs();
               tempPlatData.externalFuelCapacity_lbs     = fuelData->fuelCapExt_lbs();
               tempPlatData.totalFuelCapacity_lbs        = fuelData->fuelCapInt_lbs() + fuelData->fuelCapExt_lbs();
               tempPlatData.grossWeight_lbs              = fuelData->grossWgt_lbs();
               tempPlatData.joker_lbs                    = fuelData->joker_lbs();
               tempPlatData.bingo_lbs                    = fuelData->bingo_lbs();
               tempPlatData.fuelFlow_pph                 = fuelData->fuelFlow_pph();
               tempPlatData.fuelValid                    = fuelData->fuelValid();

               if (tempPlatData.currentTotalFuelQuantity_lbs < tempPlatData.bingo_lbs)
               {
                  tempPlatData.bingoFlagActive = true;
               }
               else
               {
                  tempPlatData.bingoFlagActive = false;
               }
            }
            else
            {
               tempPlatData.currentInternalFuelQuantity_lbs = 0.0;
               tempPlatData.currentExternalFuelQuantity_lbs = 0.0;
               tempPlatData.currentTotalFuelQuantity_lbs    = 0.0;
               tempPlatData.internalFuelCapacity_lbs        = 0.0;
               tempPlatData.externalFuelCapacity_lbs        = 0.0;
               tempPlatData.totalFuelCapacity_lbs           = 0.0;
               tempPlatData.grossWeight_lbs                 = 0.0;
               tempPlatData.joker_lbs                       = 0.0;
               tempPlatData.bingo_lbs                       = 0.0;
               tempPlatData.fuelFlow_pph                    = 0.0;
               tempPlatData.fuelValid                       = false;
               tempPlatData.bingoFlagActive                 = false;
            }

            if (weaponsData != nullptr)
            {
               tempPlatData.selectedWeapon    = weaponsData->selectedWeapon();
               tempPlatData.numSelectedWeapon = static_cast<size_t>(weaponsData->numSelectedWeapon());

               if (!weaponsData->selectedWeapon().empty())
               {
                  tempPlatData.selectedWeaponValid = true;
               }
               else
               {
                  tempPlatData.selectedWeaponValid = false;
               }

               if (!weaponsData->engagedTargetList().empty())
               {
                  if (weaponsData->engagedTargetList()[0].targetIndex() > 0)
                  {
                     tempPlatData.engagedTargetValid = true;
                     tempPlatData.engagedTargetData.targetIndex =
                        static_cast<size_t>(weaponsData->engagedTargetList()[0].targetIndex());
                     tempPlatData.engagedTargetData.rangeMax_nm = weaponsData->engagedTargetList()[0].rangeMax_nm();
                     tempPlatData.engagedTargetData.rangeNe_nm  = weaponsData->engagedTargetList()[0].rangeNe_nm();
                     tempPlatData.engagedTargetData.rangeMin_nm = weaponsData->engagedTargetList()[0].rangeMin_nm();
                     tempPlatData.engagedTargetData.rangeCurrent_nm =
                        weaponsData->engagedTargetList()[0].rangeCurrent_nm();
                     tempPlatData.engagedTargetData.rangeClosureRate_kts =
                        weaponsData->engagedTargetList()[0].rangeClosureRate_kts();
                     tempPlatData.engagedTargetData.timeToIntercept_sec =
                        weaponsData->engagedTargetList()[0].timeToIntercept_sec();
                     tempPlatData.engagedTargetData.timeToActive_sec =
                        weaponsData->engagedTargetList()[0].timeToActive_sec();
                     tempPlatData.engagedTargetData.targetKTAS = weaponsData->engagedTargetList()[0].targetKTAS();
                     tempPlatData.engagedTargetData.targetAspect_deg =
                        weaponsData->engagedTargetList()[0].targetAspect_deg();
                     tempPlatData.engagedTargetData.targetAz_deg = weaponsData->engagedTargetList()[0].targetAz_deg();
                     tempPlatData.engagedTargetData.targetEl_deg = weaponsData->engagedTargetList()[0].targetEl_deg();
                     ;
                  }
                  else
                  {
                     tempPlatData.engagedTargetValid                     = false;
                     tempPlatData.engagedTargetData.targetIndex          = 0;
                     tempPlatData.engagedTargetData.rangeMax_nm          = 0.0;
                     tempPlatData.engagedTargetData.rangeNe_nm           = 0.0;
                     tempPlatData.engagedTargetData.rangeMin_nm          = 0.0;
                     tempPlatData.engagedTargetData.rangeCurrent_nm      = 0.0;
                     tempPlatData.engagedTargetData.rangeClosureRate_kts = 0.0;
                     tempPlatData.engagedTargetData.timeToIntercept_sec  = 0.0;
                     tempPlatData.engagedTargetData.timeToActive_sec     = 0.0;
                     tempPlatData.engagedTargetData.targetKTAS           = 0.0;
                     tempPlatData.engagedTargetData.targetAspect_deg     = 0.0;
                     tempPlatData.engagedTargetData.targetAz_deg         = 0.0;
                     tempPlatData.engagedTargetData.targetEl_deg         = 0.0;
                  }
               }
               else
               {
                  tempPlatData.engagedTargetValid                     = false;
                  tempPlatData.engagedTargetData.targetIndex          = 0;
                  tempPlatData.engagedTargetData.rangeMax_nm          = 0.0;
                  tempPlatData.engagedTargetData.rangeNe_nm           = 0.0;
                  tempPlatData.engagedTargetData.rangeMin_nm          = 0.0;
                  tempPlatData.engagedTargetData.rangeCurrent_nm      = 0.0;
                  tempPlatData.engagedTargetData.rangeClosureRate_kts = 0.0;
                  tempPlatData.engagedTargetData.timeToIntercept_sec  = 0.0;
                  tempPlatData.engagedTargetData.timeToActive_sec     = 0.0;
                  tempPlatData.engagedTargetData.targetKTAS           = 0.0;
                  tempPlatData.engagedTargetData.targetAspect_deg     = 0.0;
                  tempPlatData.engagedTargetData.targetAz_deg         = 0.0;
                  tempPlatData.engagedTargetData.targetEl_deg         = 0.0;
               }

               tempPlatData.weaponBayDoorsOpen = weaponsData->weaponBayDoorsOpen();
               tempPlatData.supportingWeapon   = weaponsData->supportingWeapon();
               tempPlatData.masterWarning      = weaponsData->masterWarning();
               tempPlatData.masterCaution      = weaponsData->masterCaution();
               tempPlatData.shootCueActive     = weaponsData->shootCueActive();
               tempPlatData.shootCueBlink      = weaponsData->shootCueBlink();
               tempPlatData.masterArmActive    = weaponsData->masterArmActive();
               tempPlatData.jammingDetected    = weaponsData->jammingDetected();
               tempPlatData.numChaff           = static_cast<size_t>(weaponsData->numChaff());
               tempPlatData.numFlares          = static_cast<size_t>(weaponsData->numFlares());
               tempPlatData.numDecoys          = static_cast<size_t>(weaponsData->numDecoys());
            }
            else
            {
               tempPlatData.selectedWeapon                         = "";
               tempPlatData.numSelectedWeapon                      = 0;
               tempPlatData.selectedWeaponValid                    = false;
               tempPlatData.engagedTargetData.targetIndex          = 0;
               tempPlatData.engagedTargetData.rangeMax_nm          = 0.0;
               tempPlatData.engagedTargetData.rangeNe_nm           = 0.0;
               tempPlatData.engagedTargetData.rangeMin_nm          = 0.0;
               tempPlatData.engagedTargetData.rangeCurrent_nm      = 0.0;
               tempPlatData.engagedTargetData.rangeClosureRate_kts = 0.0;
               tempPlatData.engagedTargetData.timeToIntercept_sec  = 0.0;
               tempPlatData.engagedTargetData.timeToActive_sec     = 0.0;
               tempPlatData.engagedTargetData.targetKTAS           = 0.0;
               tempPlatData.engagedTargetData.targetAspect_deg     = 0.0;
               tempPlatData.engagedTargetData.targetAz_deg         = 0.0;
               tempPlatData.engagedTargetData.targetEl_deg         = 0.0;
               tempPlatData.engagedTargetValid                     = false;
               tempPlatData.weaponBayDoorsOpen                     = false;
               tempPlatData.supportingWeapon                       = false;
               tempPlatData.masterWarning                          = false;
               tempPlatData.masterCaution                          = false;
               tempPlatData.shootCueActive                         = false;
               tempPlatData.shootCueBlink                          = false;
               tempPlatData.masterArmActive                        = false;
               tempPlatData.jammingDetected                        = false;
               tempPlatData.numChaff                               = 0;
               tempPlatData.numFlares                              = 0;
               tempPlatData.numDecoys                              = 0;
            }

            // Once we are done populating the data, add it to the temporary container
            const wkf::HUD_DataContainer::PlatformData& platData = tempPlatData;
            tempPlatDatas.emplace(aConnectedPlatform.toStdString(), platData);
         }
      }
   }
   // Lastly, update the data container
   mDataContainer.SetPlatformData(tempPlatDatas);
}

void RvHeadsUpDisplay::Plugin::UpdateDataForHud(QPointer<wkf::HUD_DockWidget> aDockWidget)
{
   // If our HUD window is valid
   if (aDockWidget)
   {
      // If our HUD is visible, update the data. Otherwise, return.
      if (!aDockWidget.data()->HUD_Visible())
      {
         return;
      }
      else
      {
         // Get the HUD
         wkf::HUD* hud = aDockWidget.data()->GetHUD();

         // Get our platform data map
         std::map<std::string, wkf::HUD_DataContainer::PlatformData> aDC = mDataContainer.GetPlatformData();

         // If our HUD isn't attached to any platform, we can't get data.  Return.
         if (aDC.find(aDockWidget->GetConnectedPlatformName().toStdString()) == aDC.end())
         {
            return;
         }

         // Get a reference to the platform data for this HUD.
         const wkf::HUD_DataContainer::PlatformData& platformData =
            aDC.at(aDockWidget->GetConnectedPlatformName().toStdString());

         // Set the HUD data
         hud->mPlatData = platformData;
      }
   }
}

void RvHeadsUpDisplay::Plugin::ViewerInitializedHandler(QPointer<wkf::HUD_DockWidget> aDockWidget)
{
   // Set the appropriate first person and hud values to match the preferences,
   // then raise the window to the front.
   aDockWidget.data()->SetFirstPerson(true);
   aDockWidget.data()->ShowHUD(true);
   aDockWidget->raise();
}

bool RvHeadsUpDisplay::Plugin::HasSituationAwarenessProcessor(wkf::Platform* aPlatform)
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

const bool RvHeadsUpDisplay::Plugin::HasSA_Data(rv::ResultPlatform* aPlatformPtr, const double aSimTime)
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
