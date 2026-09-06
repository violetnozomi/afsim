// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvSatelliteTetherPlugin.hpp"

#include <QMenu>

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvSatelliteTetherDockWidget.hpp"
#include "UtQtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfMainWindow.hpp"
#include "satellite_tether/WkfSatelliteTetherPrefObject.hpp"
#include "satellite_tether/WkfSatelliteTetherPrefWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvSatelliteTether::Plugin, "Satellite Tether", "Provides a tether-view catered for satellites", "mystic")

namespace
{
constexpr const char* cSATELLITE_TETHER_VIEW = "Satellite-TetherView";
}

RvSatelliteTether::Plugin::Plugin(const QString& aPluginName, const size_t& aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidget(new wkf::SatelliteTetherPrefWidget)
{
   auto tetherAction = ut::qt::make_qt_ptr<wkf::Action>(QIcon::fromTheme("satellite"),
                                                        QString("Satellite-Tether To Selection"),
                                                        wkfEnv.GetMainWindow());
   connect(tetherAction, &QAction::triggered, this, &Plugin::Tether);
   mActions.push_back(tetherAction);
}

void RvSatelliteTether::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   auto platform = dynamic_cast<wkf::Platform*>(aEntityPtr);
   if (platform)
   {
      if (platform->GetSpatialDomain() == wkf::SPATIAL_DOMAIN_SPACE)
      {
         QString  name = aEntityPtr->GetName().c_str();
         QAction* tetherAction =
            new QAction(QIcon::fromTheme("satellite"), QString("Satellite-Tether to %1").arg(name), aMenu);
         aMenu->addAction(tetherAction);
         connect(tetherAction, &QAction::triggered, this, [=]() { ConnectToPlatform(name); });
      }
   }
}

void RvSatelliteTether::Plugin::ClearScenario(bool aFullReset)
{
   for (auto&& w : mWidgetList)
   {
      w.second->blockSignals(true);
      delete w.second;
   }
   mWidgetList.clear();
}

void RvSatelliteTether::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   for (auto&& w : mWidgetList)
   {
      for (auto& platform : w.second->GetTrackedPlatforms())
      {
         w.second->Update(aData, platform);
      }
   }
}

void RvSatelliteTether::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

QList<wkf::PrefWidget*> RvSatelliteTether::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidget;
   return prefs;
}

void RvSatelliteTether::Plugin::Tether()
{
   wkf::Platform* platPtr = wkfEnv.GetPlatformOfInterest();
   if (platPtr)
   {
      if (platPtr->GetSpatialDomain() == wkf::SPATIAL_DOMAIN_SPACE)
      {
         ConnectToPlatform(platPtr->GetName().c_str());
      }
   }
}

void RvSatelliteTether::Plugin::ConnectToPlatform(const QString& aPlatformName)
{
   // Store pointer to this dockWidget,
   // since name is not unique due to multiple views can point to same entity, use a key value
   auto dw = new DockWidget(cSATELLITE_TETHER_VIEW + aPlatformName,
                            aPlatformName,
                            mWidgetNamer,
                            wkfEnv.GetMainWindow()->centralWidget());
   dw->setWindowTitle(cSATELLITE_TETHER_VIEW + QString(" ") + aPlatformName);
   mWidgetList[mWidgetNamer] = dw;
   connect(dw, &DockWidget::CleanUp, [this](int aId) { mWidgetList.erase(aId); });
   connect(mPrefWidget->GetPreferenceObject(), &wkf::SatelliteTetherPrefObject::ShowNamesChanged, dw, &DockWidget::ShowNames);
   connect(mPrefWidget->GetPreferenceObject(),
           &wkf::SatelliteTetherPrefObject::ShowFutureTrackChanged,
           dw,
           &DockWidget::ShowFutureTracks);
   connect(mPrefWidget->GetPreferenceObject(), &wkf::SatelliteTetherPrefObject::LightingChanged, dw, &DockWidget::EnableLighting);
   connect(dw,
           &DockWidget::ImmersiveViewInitialized,
           [this, dw]() { dw->SetLightingEnabled(mPrefWidget->GetPreferenceObject()->GetLightingEnabled()); });
   dw->ShowNames(mPrefWidget->GetPreferenceObject()->GetShowNames());
   dw->ShowFutureTracks(mPrefWidget->GetPreferenceObject()->GetShowFutureTrack());
   dw->SetLightingEnabled(mPrefWidget->GetPreferenceObject()->GetLightingEnabled());
   ++mWidgetNamer;
}
