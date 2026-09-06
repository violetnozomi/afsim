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

#include "sa_display/WkfSA_DisplayPluginBase.hpp"

#include <cstdlib>

#include <QMenu>
#include <QString>

#include "UtQtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "sa_display/WkfSA_DisplayDockWidget.hpp"

namespace
{
constexpr const char* cSA_DISPLAY = "SA_Display";
}

wkf::SA_Display::PluginBase::PluginBase(const QString& aPluginName, const size_t aUniqueID, const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueID, aRoles)
{
   mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&PluginBase::PlatformsAddedCB, this));
   mCallbacks.Add(wkf::Observer::PlatformPendingDeletion.Connect(&PluginBase::PlatformToBeDeletedCB, this));
}

wkf::SA_Display::PluginBase::~PluginBase() {}

void wkf::SA_Display::PluginBase::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      wkf::Platform* platPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
      if ((HasSituationAwarenessProcessor(platPtr)) && mSA_Displays.find(platPtr->GetIndex()) == mSA_Displays.end())
      {
         // That big nested-if monstrosity means that this will only run if:
         //   - aEntityPtr is a pointer to a wkf::Platform
         //   - AND the platform has a PerceptionProcessor, OR the prefs say to always allow the option to open a PD to show up
         //   - AND there is no dock widget for this platform already
         QString name  = aEntityPtr->GetName().c_str();
         size_t  index = platPtr->GetIndex();

         auto airCombatMenu = aMenu->findChild<QMenu*>("Air Combat");
         if (!airCombatMenu)
         {
            airCombatMenu = ut::qt::make_qt_ptr<QMenu>(QString("Air Combat"), aMenu);
            aMenu->addMenu(airCombatMenu);
            airCombatMenu->setObjectName("Air Combat");
         }

         QAction* saViewAction = new QAction(QIcon::fromTheme("tether"), QString("SA View"), airCombatMenu);
         airCombatMenu->addAction(saViewAction);
         connect(saViewAction,
                 &QAction::triggered,
                 this,
                 [=]()
                 {
                    auto pd = ConnectToPlatform(index, name);
                    if (pd != nullptr)
                    {
                       pd->show();
                       NewDisplay(platPtr);
                    }
                 });
      }
   }
}

void wkf::SA_Display::PluginBase::PlatformsAddedCB(const wkf::PlatformList& aPlatforms)
{
   for (const auto& platform : aPlatforms)
   {
      QString platformName = QString::fromStdString(platform->GetName());

      // check to see if the entity is in any groups of interest?

      // If the platformName exist in this List of entity to show a tether view
      if (mPlatformTetheredOnStartList.count(platformName) > 0)
      {
         DockWidget* dw = wkfEnv.GetMainWindow()->centralWidget()->findChild<DockWidget*>(cSA_DISPLAY + platformName);
         if (!dw)
         {
            dw = ConnectToPlatform(platform->GetIndex(), platformName);
         }
         if (dw != nullptr)
         {
            NewDisplay(platform);
            dw->show();
         }
      }
   }
}

void wkf::SA_Display::PluginBase::PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr)
{
   size_t index = aPlatformPtr->GetIndex();
   // If the deleted platform was in the list of perception displays...
   if (mSA_Displays.find(index) != mSA_Displays.end())
   {
      //... then remove it from the interface's list of platforms of interest.
      RemoveDisplay(index);
   }
}

void wkf::SA_Display::PluginBase::GuiUpdate()
{
   // If the simulation was reset, reset our container and reset displays.
   if (mDataContainer.GetSimulationInitialized())
   {
      Reset();
      mDataContainer.SetSimulationInitialized(false);
   }

   // Check if we have displays to update first
   if (mSA_Displays.size() > 0)
   {
      // Loop through each SA Display finding and updating its data.
      for (auto& sad : mSA_Displays)
      {
         if (sad.second->ConnectionEstablished())
         {
            // If we have data in the data container for this SA display
            if (mDataContainer.GetSA_Data().find(sad.first) != mDataContainer.GetSA_Data().end())
            {
               const wkf::SA_Display::SA_Data&                  sadata = mDataContainer.GetSA_Data().at(sad.first);
               const std::vector<wkf::SA_Display::EntityTruth>& truth  = mDataContainer.GetSA_TruthPlatforms();
               mSA_Displays[sad.first]->SetSA_Data(sadata);
               mSA_Displays[sad.first]->SetSA_TruthPlatforms(truth);
            }
         }
      }
   }
}

void wkf::SA_Display::PluginBase::Reset()
{
   for (auto& sad : mSA_Displays)
   {
      delete sad.second.data();
   }
   mSA_Displays.clear();
}

wkf::SA_Display::DockWidget* wkf::SA_Display::PluginBase::ConnectToPlatform(size_t aIndex, const QString& aPlatformName)
{
   // Store pointer to this dockWidget,
   // since name is not unique due to multiple views can point to same entity, use a key value
   if (mSA_Displays.find(aIndex) == mSA_Displays.end())
   {
      auto it =
         mSA_Displays
            .emplace(aIndex,
                     new DockWidget(cSA_DISPLAY + aPlatformName, aPlatformName, wkfEnv.GetMainWindow()->centralWidget()))
            .first;
      int  key        = it->first;
      auto dockWidget = it->second;
      // Connect to destroyed signal so we can remove the dock widget from our list of widgets
      connect(dockWidget.data(),
              &QObject::destroyed,
              [=](QObject* obj)
              {
                 mPlatformTetheredOnStartList.erase(aPlatformName);
                 RemoveDisplay(key);
                 mSA_Displays.erase(key);
              });

      // Set Window title
      dockWidget->setWindowTitle(QString("Situation Awareness View: %1").arg(aPlatformName));

      // Set current prefs
      return dockWidget;
   }
   else
   {
      return nullptr;
   }
}

void wkf::SA_Display::PluginBase::Tether()
{
   wkf::Platform* platPtr = wkfEnv.GetPlatformOfInterest();
   if (platPtr)
   {
      auto pd = ConnectToPlatform(platPtr->GetIndex(), platPtr->GetName().c_str());
      if (pd != nullptr)
      {
         NewDisplay(platPtr);
         pd->show();
      }
   }
}
