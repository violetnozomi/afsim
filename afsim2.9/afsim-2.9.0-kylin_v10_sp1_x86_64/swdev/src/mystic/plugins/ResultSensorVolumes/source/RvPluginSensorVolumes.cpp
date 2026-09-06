// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPluginSensorVolumes.hpp"

#include "RvEventPipeUtil.hpp"
#include "RvPlatformSensorVolumes.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "VaScenario.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "sensor_volume/WkfSensorVolumesOptionHistoryManager.hpp"
#include "sensor_volume/WkfSensorVolumesPrefObject.hpp"
#include "sensor_volume/WkfSensorVolumesPrefWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvSensorVolumes::Plugin, "Sensor Volumes", "Plugin that supports drawing sensors' volumes", "mystic")

RvSensorVolumes::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new wkf::SensorVolumesPrefWidget(wkf::SensorVolumesPrefWidget::EntryMethod::cCOMBOBOX))
   , mIndividualIndex(0)
{
   SetOptionHistoryManager(new wkf::SensorVolumesOptionHistoryManager(aPluginName, this));
   mSensorTopItem = RegisterOption(nullptr, "Sensor Volumes");
   mWeaponTopItem = RegisterOption(nullptr, "Jammer Volumes");

   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::SensorVolumesPrefObject::ModesReset,
           this,
           &RvSensorVolumes::Plugin::ModesReset);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::SensorVolumesPrefObject::DrawModeChanged,
           this,
           &RvSensorVolumes::Plugin::DrawModeChanged);
}


void RvSensorVolumes::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   UtVariant v = aViewerPtr->GetData("type");
   // Add boresight vector support for any wkf::ImmersiveViewDockWidget
   if (!v.IsUnset() && v.GetString() == "immersive")
   {
      QMenu* addMenu = aMenu->findChild<QMenu*>("Vectors");
      if (!addMenu)
      {
         addMenu = aMenu->addMenu("Vectors");
         addMenu->setObjectName("Vectors");
      }

      // Find the current dockWidget that is immersive
      QMainWindow*                  centralWidget = wkfEnv.GetMainWindow()->centralWidget();
      wkf::ImmersiveViewDockWidget* currDockPtr   = nullptr;
      for (auto* i : centralWidget->children())
      {
         auto dockWidget = dynamic_cast<wkf::ImmersiveViewDockWidget*>(i);
         if (dockWidget != nullptr && dockWidget->IsMyViewer(aViewerPtr))
         {
            currDockPtr = dockWidget;
         }
      }
      if (currDockPtr == nullptr)
      {
         return;
      }

      // Find action to insert boresight actions after
      QAction* previousAction = nullptr;
      for (auto action : addMenu->actions())
      {
         if (action->text() == "New Vector...")
         {
            previousAction = action;
            break;
         }
      }
      if (previousAction == nullptr)
      {
         return;
      }

      // Add the new actions
      std::string         platName = currDockPtr->GetConnectedPlatformName().toStdString();
      rv::ResultPlatform* resPlat  = rvEnv.GetData()->FindPlatform(rvEnv.GetData()->FindPlatformIndex(platName));
      if (resPlat)
      {
         auto sparts = resPlat->GetParts(rv::PartType::sensor);
         if (sparts)
         {
            for (auto& sensor : *sparts)
            {
               auto artMsg = sensor.second.FindFirstBefore<rv::MsgPartArticulationStatus>(rvEnv.GetData()->GetSimTime());
               auto statusMsg = sensor.second.FindFirstBefore<rv::MsgPartStatus>(rvEnv.GetData()->GetSimTime());
               if (statusMsg == nullptr || !statusMsg->on() || artMsg == nullptr)
               {
                  continue;
               }
               std::string sensorName = sensor.first;
               QAction*    action     = new QAction(QString::fromStdString(sensorName), aMenu);
               action->setCheckable(true);
               action->setChecked(currDockPtr->GetBoresightShown(sensorName));
               addMenu->insertAction(previousAction, action);

               // On click of action
               connect(action,
                       &QAction::triggered,
                       [this, currDockPtr, sensorName]()
                       {
                          // If boresights are not shown yet, show them
                          if (!currDockPtr->GetBoresightShown(sensorName))
                          {
                             // Create and update the boresights for the first time
                             UpdateBoresight(currDockPtr, sensorName);

                             // Add the current dock to list of docks whose boresights are being updated.
                             mWidgetBoresights[currDockPtr].insert(sensorName);
                             connect(currDockPtr,
                                     &QObject::destroyed,
                                     [=](QObject* obj) { mWidgetBoresights.erase(currDockPtr); });
                          }
                          // If boresight is already shown, hide them
                          else
                          {
                             currDockPtr->HideBoresight(sensorName);
                             mWidgetBoresights[currDockPtr].erase(sensorName);
                          }
                       });
            }
         }
      }
   }
}

void RvSensorVolumes::Plugin::UpdateBoresight(wkf::ImmersiveViewDockWidget* aDockWidget, const std::string& aSpecificSensor)
{
   double simTime   = rvEnv.GetData()->GetSimTime();
   int    platIndex = rvEnv.GetData()->FindPlatformIndex(aDockWidget->GetConnectedPlatformName().toStdString());
   // Only update if the boresights are visible or this is the first call for the dock widget

   rv::ResultPlatform* resPlat = rvEnv.GetData()->FindPlatform(platIndex);
   if (resPlat)
   {
      resPlat->BeginRead();
      auto sparts = resPlat->GetParts(rv::PartType::sensor);
      for (auto& sensor : *sparts)
      {
         auto articulationMsg = sensor.second.FindFirstBefore<rv::MsgPartArticulationStatus>(simTime);
         auto statusMsg       = sensor.second.FindFirstBefore<rv::MsgPartStatus>(simTime);
         if (aSpecificSensor == sensor.first)
         {
            if (articulationMsg != nullptr && statusMsg->on())
            {
               double az;
               double el;
               double roll;
               rv::GetCombinedOrientation(*articulationMsg, az, el, roll);
               aDockWidget->ShowBoresight(articulationMsg->partName(), az, el);
            }
            else
            {
               aDockWidget->HideBoresight(sensor.first);
            }
         }
      }
      resPlat->EndRead();
   }
}


QList<wkf::PrefWidget*> RvSensorVolumes::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> list;
   list.push_back(mPrefWidgetPtr);
   return list;
}

void RvSensorVolumes::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvSensorVolumes::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   float simTime = aData.GetSimTime();

   if ((mIndividualIndex != 0) && (ShowIndividualOptions()))
   {
      rv::ResultPlatform* plat = aData.FindPlatform(mIndividualIndex);
      if (nullptr != plat)
      {
         plat->BeginRead();

         auto sparts = plat->GetParts(rv::PartType::sensor);
         if (sparts != nullptr)
         {
            for (auto& sensor : *sparts)
            {
               if (mIndividualsSensors.find(sensor.first) == mIndividualsSensors.end())
               {
                  mIndividualsSensors.insert(sensor.first);
                  bool check = (mSensorTopItem->checkState(0) != Qt::Unchecked);
                  if (mSensorVolumeMap[mSensorTopItem->type()].find(mIndividualIndex) !=
                      mSensorVolumeMap[mSensorTopItem->type()].end())
                  {
                     check = mSensorVolumeMap[mSensorTopItem->type()][mIndividualIndex]->IsDefaultVisible();
                     check =
                        mSensorVolumeMap[mSensorTopItem->type()][mIndividualIndex]->SensorVisible(sensor.first, check);
                  }
                  RegisterOption(mSensorTopItem, sensor.first.c_str(), "", true, check);
               }
            }
         }
         auto wparts = plat->GetParts(static_cast<rv::PartType::Value>(rv::MilPartType::weapon));
         if (wparts != nullptr)
         {
            for (auto& weapon : *wparts)
            {
               if (mIndividualsWeapons.find(weapon.first) == mIndividualsWeapons.end())
               {
                  mIndividualsWeapons.insert(weapon.first);
                  bool check = (mWeaponTopItem->checkState(0) != Qt::Unchecked);
                  if (mSensorVolumeMap[mWeaponTopItem->type()].find(mIndividualIndex) !=
                      mSensorVolumeMap[mWeaponTopItem->type()].end())
                  {
                     check = mSensorVolumeMap[mWeaponTopItem->type()][mIndividualIndex]->IsDefaultVisible();
                     check =
                        mSensorVolumeMap[mWeaponTopItem->type()][mIndividualIndex]->SensorVisible(weapon.first, check);
                  }


                  std::string                    modeName = "default";
                  const rv::MsgWeaponModeChange* modeChange =
                     weapon.second.FindFirstBefore<rv::MsgWeaponModeChange>(simTime);
                  if (modeChange != nullptr)
                  {
                     modeName = modeChange->modeName();
                  }
                  const rv::MsgEmitterModeDefinition* mode = plat->FindWeaponMode(modeName, weapon.first);
                  if (mode != nullptr)
                  {
                     RegisterOption(mWeaponTopItem, weapon.first.c_str(), "", true, check);
                  }
               }
            }
         }
         plat->EndRead();
      }
   }

   for (auto& jt : mSensorVolumeMap)
   {
      for (auto& it : jt.second)
      {
         if (jt.first == mSensorTopItem->type())
         {
            // handle sensors
            int                 index = it.first;
            rv::ResultPlatform* plat  = aData.FindPlatform(index);
            if (nullptr != plat)
            {
               std::map<std::string, rv::MsgEmitterModeDefinition*> sensorMap;
               plat->BeginRead();
               auto sparts = plat->GetParts(rv::PartType::sensor);
               if (sparts)
               {
                  for (auto& sensor : *sparts)
                  {
                     auto statusMsg = sensor.second.FindFirstBefore<rv::MsgPartStatus>(simTime);
                     if (statusMsg != nullptr)
                     {
                        if (statusMsg->on() && !statusMsg->disabled() && !statusMsg->broken())
                        {
                           std::string modeName = "default";
                           const auto* modeCh   = sensor.second.FindFirstBefore<rv::MsgSensorModeChange>(simTime);
                           if (modeCh != nullptr)
                           {
                              modeName = modeCh->modeName();
                           }

                           const rv::MsgEmitterModeDefinition* mode =
                              plat->FindSensorMode(modeName, statusMsg->partName());
                           if (mode != nullptr)
                           {
                              sensorMap[statusMsg->partName()] = new rv::MsgEmitterModeDefinition(*mode);
                              // mark to not hide and update mode information
                           }
                        }
                     }
                  }
               }
               it.second->MarkAll();
               if (sparts)
               {
                  for (auto& jt : sensorMap)
                  {
                     rv::MsgEmitterModeDefinition*        mode = jt.second;
                     const rv::MsgPartArticulationStatus* art  = nullptr;

                     auto sensor = sparts->find(jt.first);
                     if (sensor != sparts->end())
                     {
                        art = sensor->second.FindFirstBefore<rv::MsgPartArticulationStatus>(simTime);
                     }

                     auto* scenarioPtr = vaEnv.GetStandardScenario();
                     if (scenarioPtr)
                     {
                        it.second->UpdateAndUnmark(jt.first,
                                                   art,
                                                   mode,
                                                   scenarioPtr->FindPlatformByIndex(index),
                                                   vaEnv.GetStandardViewer()); // and unmark
                     }                                                         // look up articulations
                     delete mode;
                  }
               }
               it.second->HideMarked();
               sensorMap.clear();
               plat->EndRead();
            }
         }
         else if (jt.first == mWeaponTopItem->type())
         {
            // handle weapons
            /////////////////////////
            int                 index = it.first;
            rv::ResultPlatform* plat  = aData.FindPlatform(index);
            if (nullptr != plat)
            {
               std::map<std::string, rv::MsgEmitterModeDefinition*> weaponMap;
               plat->BeginRead();
               auto wparts = plat->GetParts(static_cast<rv::PartType::Value>(rv::MilPartType::weapon));
               if (wparts != nullptr)
               {
                  for (auto& weapon : *wparts)
                  {
                     auto statusMsg = weapon.second.FindFirstBefore<rv::MsgPartStatus>(simTime);
                     if (nullptr != statusMsg)
                     {
                        if (statusMsg->on() && !statusMsg->disabled() && !statusMsg->broken())
                        {
                           std::string                    modeName = "default";
                           const rv::MsgWeaponModeChange* modeChange =
                              weapon.second.FindFirstBefore<rv::MsgWeaponModeChange>(simTime);
                           if (modeChange != nullptr)
                           {
                              modeName = modeChange->modeName();
                           }
                           const rv::MsgEmitterModeDefinition* mode = plat->FindWeaponMode(modeName, weapon.first);
                           if (mode != nullptr)
                           {
                              weaponMap[statusMsg->partName()] = new rv::MsgEmitterModeDefinition(*mode);
                              // mark to not hide and update mode information
                           }
                        }
                     }
                  }
               }
               it.second->MarkAll();
               if (wparts != nullptr)
               {
                  for (auto& jt : weaponMap)
                  {
                     rv::MsgEmitterModeDefinition*        mode = jt.second;
                     const rv::MsgPartArticulationStatus* art  = nullptr;

                     auto weapon = wparts->find(jt.first);
                     if (weapon != wparts->end())
                     {
                        art = weapon->second.FindFirstBefore<rv::MsgPartArticulationStatus>(simTime);
                     }
                     auto* scenarioPtr = vaEnv.GetStandardScenario();
                     if (scenarioPtr)
                     {
                        it.second->UpdateAndUnmark(jt.first,
                                                   art,
                                                   mode,
                                                   scenarioPtr->FindPlatformByIndex(index),
                                                   vaEnv.GetStandardViewer()); // and unmark
                     }                                                         // look up articulations
                     delete mode;
                  }
               }
               it.second->HideMarked();
               weaponMap.clear();

               plat->EndRead();
            }
            /////////////////////////
         }
      }
   }

   for (const auto& pair : mWidgetBoresights)
   {
      for (const auto& sensorName : pair.second)
      {
         UpdateBoresight(pair.first, sensorName);
      }
   }
}

void RvSensorVolumes::Plugin::ResetOptionStates()
{
   for (auto& jt : mSensorVolumeMap)
   {
      for (auto& it : jt.second)
      {
         it.second->RemoveAttachments();
      }
   }
   mSensorVolumeMap.clear();
}

void RvSensorVolumes::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      int          p     = GetOptionHistoryManager()->GetParentOption(aOptionId);
      unsigned int index = aPlatformPtr->GetIndex();
      if (aState)
      {
         auto jt = mSensorVolumeMap[p].find(index);
         if (jt == mSensorVolumeMap[p].end())
         {
            // for every sensor
            mSensorVolumeMap[p][index] = ut::make_unique<PlatformVolumes>(*aPlatformPtr,
                                                                          mPrefWidgetPtr->GetPreferenceObject(),
                                                                          (p == mWeaponTopItem->type())); // create it
            jt                         = mSensorVolumeMap[p].find(index);
            jt->second->SetVisible(false);
         }
         if (jt != mSensorVolumeMap[p].end())
         {
            if (aOptionId == mSensorTopItem->type()) // all sensors
            {
               jt->second->SetVisible(true);
            }
            else if (aOptionId == mWeaponTopItem->type()) // all weapons
            {
               jt->second->SetVisible(true);
            }
            else // if it is an individual... how to identify weapons vs sensors
            {
               jt->second->SetSensorVisible(OptionName(aOptionId).toStdString(), true);
            }
            // show it
         }
      }
      else
      {
         auto jt = mSensorVolumeMap[p].find(index);
         if (jt == mSensorVolumeMap[p].end())
         {
            // for every sensor
            mSensorVolumeMap[p][index] = ut::make_unique<PlatformVolumes>(*aPlatformPtr,
                                                                          mPrefWidgetPtr->GetPreferenceObject(),
                                                                          (p == mWeaponTopItem->type())); // create it
            jt                         = mSensorVolumeMap[p].find(index);
            jt->second->SetVisible(false);
         }
         if (jt != mSensorVolumeMap[p].end())
         {
            if (aOptionId == p) // all sensors or all weapons
            {
               jt->second->SetVisible(false);
            }
            else // if it is an individual... how to identify weapons vs sensors
            {
               jt->second->SetSensorVisible(OptionName(aOptionId).toStdString(), false);
            }
         }
      }
      // turn on sensor volumes for the selected entity
   }
}

Qt::CheckState RvSensorVolumes::Plugin::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const
{
   if (aPlatformPtr)
   {
      int  p       = GetOptionHistoryManager()->GetParentOption(aOptionId);
      int  index   = aPlatformPtr->GetIndex();
      auto vtypeIt = mSensorVolumeMap.find(p);

      if (vtypeIt != mSensorVolumeMap.end())
      {
         std::map<unsigned int, std::unique_ptr<PlatformVolumes>>::const_iterator it = vtypeIt->second.find(index);
         if (it != vtypeIt->second.end())
         {
            if (aOptionId == p) // all sensors or all weapons
            {
               bool all;
               bool some;
               it->second->CheckSensorVisibility(all, some);
               return (all ? Qt::Checked : some ? Qt::PartiallyChecked : Qt::Unchecked);
            }
            else // if it is an individual... how to identify weapons vs sensors
            {
               if (it->second->SensorVisible(OptionName(aOptionId).toStdString(), false))
               {
                  return Qt::Checked;
               }
               else
               {
                  return Qt::Unchecked;
               }
            }
         }
      }
   }
   return Qt::Unchecked;
}

void RvSensorVolumes::Plugin::ModesReset()
{
   // notify the volumes
   for (auto& jt : mSensorVolumeMap)
   {
      for (auto& it : jt.second)
      {
         it.second->ModesReset();
      }
   }
}

void RvSensorVolumes::Plugin::DrawModeChanged(const wkf::SensorVolumesPrefData::DrawMode& aState)
{
   // notify the volumes
   for (auto& jt : mSensorVolumeMap)
   {
      for (auto& it : jt.second)
      {
         it.second->RemoveAttachments();
      }
   }
}

void RvSensorVolumes::Plugin::NewSensorModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode)
{
   mPrefWidgetPtr->AddSensorModeName(aNewMode.modeName());
}

void RvSensorVolumes::Plugin::NewWeaponModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode)
{
   mPrefWidgetPtr->AddWeaponModeName(aNewMode.modeName());
}

void RvSensorVolumes::Plugin::ClearScenario(bool aFullReset)
{
   for (auto& jt : mSensorVolumeMap)
   {
      for (auto& it : jt.second)
      {
         it.second->RemoveAttachments();
      }
      jt.second.clear();
   }
   mWidgetBoresights.clear();
}

void RvSensorVolumes::Plugin::ClearIndividualOptions()
{
   wkf::Plugin::ClearIndividualOptions();
   mIndividualsSensors.clear();
   mIndividualsWeapons.clear();
}

void RvSensorVolumes::Plugin::RegisterIndividualOptions(const std::string& aStringId)
{
   auto* plat = dynamic_cast<wkf::Platform*>(vaEnv.FindEntity(aStringId));
   if (plat)
   {
      mIndividualIndex = plat->GetIndex();
   }
   else
   {
      mIndividualIndex = 0;
   }
   // add individuals sensors
}
