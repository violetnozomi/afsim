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
#include "TetherViewPlugin.hpp"

#include <QMenu>
#include <QString>

#include "TetherViewDockWidget.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"

namespace
{
constexpr const char* cTETHER_VIEW = "TetherView";
}

WKF_PLUGIN_DEFINE_SYMBOLS(TetherView::Plugin,
                          "Tether View",
                          "The Tether View plugin adds an option to the platform-context menu to open a dockable "
                          "window with a camera that moves along with the \"tethered\" entity.",
                          "all")

TetherView::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId, eSECONDARY_VIEWER)
   , mKeyCount(1)
   , mPrefWidgetPtr(new PrefWidget)
{
   mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&Plugin::PlatformsAddedCB, this));

   wkf::Action* tetherAction = new wkf::Action(QIcon::fromTheme("tether"),
                                               QString("Tether To Selection"),
                                               wkfEnv.GetMainWindow(),
                                               QKeySequence("Ctrl+Shift+T"));
   connect(tetherAction, &QAction::triggered, this, &Plugin::Tether);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &PrefObject::HeadingLockEnabledChanged,
           this,
           &Plugin::HeadingLockEnabledChangedCB);
   connect(mPrefWidgetPtr->GetPreferenceObject(), &PrefObject::LightingEnabledChanged, this, &Plugin::LightingEnabledChangedCB);
   mActions.push_back(tetherAction);
}

QList<wkf::PrefWidget*> TetherView::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void TetherView::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::PointOfInterest>() || aEntityPtr->IsA_TypeOf<wkf::Track>() ||
       aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      QString name = aEntityPtr->GetName().c_str();
      if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
      {
         QAction* tetherAction = new QAction(QIcon::fromTheme("tether"), QString("Tether to %1").arg(name), aMenu);
         aMenu->addAction(tetherAction);
         connect(tetherAction, &QAction::triggered, this, [=]() { ConnectToPlatform(name)->show(); });
      }
      wkf::Platform* poi = wkfEnv.GetPlatformOfInterest();
      if (poi)
      {
         QString poiName = poi->GetName().c_str();
         if (poiName != name)
         {
            QAction* lookAtAction =
               new QAction(QIcon::fromTheme("tether"), QString("Look at %1 from %2").arg(name, poiName), aMenu);
            aMenu->addAction(lookAtAction);
            connect(lookAtAction, &QAction::triggered, this, [=]() { LookAt(poiName, name)->show(); });
         }
      }
   }
}

void TetherView::Plugin::PlatformsAddedCB(const wkf::PlatformList& aPlatforms)
{
   for (const auto& platform : aPlatforms)
   {
      QString platformName = QString::fromStdString(platform->GetName());

      // check to see if the entity is in any groups of interest?

      // If the platformName exist in this List of entity to show a tether view
      if (mPlatformTetheredOnStartList.count(platformName) > 0)
      {
         DockWidget* dw = wkfEnv.GetMainWindow()->centralWidget()->findChild<DockWidget*>(cTETHER_VIEW + platformName);
         if (!dw)
         {
            dw = ConnectToPlatform(platformName);
         }
         dw->show();
      }
   }
}

TetherView::DockWidget* TetherView::Plugin::ConnectToPlatform(const QString& aPlatformName)
{
   // Store pointer to this dockWidget,
   // since name is not unique due to multiple views can point to same entity, use a key value
   auto it =
      mDockWidgets
         .emplace(mKeyCount++,
                  new DockWidget(cTETHER_VIEW + aPlatformName, aPlatformName, wkfEnv.GetMainWindow()->centralWidget()))
         .first;
   auto dockWidget = it->second;
   // Connect to destroyed signal so we can removed the dock widget from out list of widgets
   connect(dockWidget.data(),
           &QObject::destroyed,
           [=](QObject* obj)
           {
              mPlatformTetheredOnStartList.erase(aPlatformName);
              mDockWidgets.erase(it->first);
           });

   // Set Window title
   dockWidget->setWindowTitle(QString("Tether: %1").arg(aPlatformName));

   connect(dockWidget.data(),
           &TetherView::DockWidget::ImmersiveViewInitialized,
           [this, dockWidget]()
           {
              dockWidget->SetLockToHeading(mPrefWidgetPtr->GetPreferenceObject()->GetHeadingLockEnabled());
              dockWidget->SetLightingEnabled(mPrefWidgetPtr->GetPreferenceObject()->GetLightingEnabled());
           });

   return dockWidget;
}

TetherView::DockWidget* TetherView::Plugin::LookAt(const QString& aFrom, const QString& aTo)
{
   // Store pointer to this dockWidget,
   // since name is not unique due to multiple views can point to same entity, use a key value
   auto it = mDockWidgets
                .emplace(mKeyCount++, new DockWidget(cTETHER_VIEW + aFrom, aFrom, wkfEnv.GetMainWindow()->centralWidget()))
                .first;
   auto dockWidget = it->second;
   // Connect to destroyed signal so we can removed the dock widget from out list of widgets
   connect(dockWidget.data(),
           &QObject::destroyed,
           [=](QObject* obj)
           {
              mPlatformTetheredOnStartList.erase(aFrom);
              mDockWidgets.erase(it->first);
           });

   dockWidget->LookAt(aTo);
   // Set Window title
   dockWidget->setWindowTitle(QString("Look At: %2, From: %1").arg(aFrom, aTo));
   return dockWidget;
}

void TetherView::Plugin::Tether()
{
   wkf::Platform* platPtr = wkfEnv.GetPlatformOfInterest();
   if (platPtr)
   {
      ConnectToPlatform(platPtr->GetName().c_str())->show();
   }
}

void TetherView::Plugin::LoadSettings(QSettings& aSettings)
{
   aSettings.beginGroup(cTETHER_VIEW);
   {
      int size = aSettings.beginReadArray("PlatformsVisible");
      for (int i = 0; i < size; ++i)
      {
         aSettings.setArrayIndex(i);
         mPlatformTetheredOnStartList.insert(aSettings.value("name").toString());
      }
      aSettings.endArray();
   }
   aSettings.endGroup();
}

void TetherView::Plugin::SaveSettings(QSettings& aSettings)
{
   aSettings.beginGroup(cTETHER_VIEW);
   {
      // remove the current array of tether windows, save only the current DockWidgets
      // This will remove DockWidget that have been closed or did not exist in the scenario
      aSettings.remove("PlatformsVisible");
      aSettings.beginWriteArray("PlatformsVisible");
      int count = 0;
      for (const auto& dock : mDockWidgets)
      {
         if (dock.second->SaveSettingsEnabled())
         {
            aSettings.setArrayIndex(count++);
            aSettings.setValue("name", dock.second->GetConnectedPlatformName());
         }
      }
      aSettings.endArray();
   }
   aSettings.endGroup();
}

void TetherView::Plugin::HeadingLockEnabledChangedCB(bool aEnabled)
{
   for (auto& dock : mDockWidgets)
   {
      dock.second->SetLockToHeading(aEnabled);
   }
}

void TetherView::Plugin::LightingEnabledChangedCB(bool aEnabled)
{
   for (auto& dock : mDockWidgets)
   {
      dock.second->SetLightingEnabled(aEnabled);
   }
}
