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

#include "WkfBM_PluginBase.hpp"

#include <QAction>
#include <QMenu>
#include <QString>
#include <QStringList>

#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"

wkf::bm::PluginBase::PluginBase(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueId, aRoles)
{
   auto* mainWindow = wkfEnv.GetMainWindow();
   if (mainWindow)
   {
      QMenu* toolsMenu = mainWindow->GetMenuByName("Tools");
      if (toolsMenu)
      {
         QMenu* bm = toolsMenu->addMenu("Battle Management");
         bm->setIcon(QIcon::fromTheme("track")); // placeholder icon

         QAction* act = bm->addAction("Platform Counts...");
         connect(act, &QAction::triggered, this, &PluginBase::MakeSidePlot);

         act = bm->addAction("Platform Type Counts...");
         connect(act, &QAction::triggered, this, &PluginBase::MakeTypePlot);

         act = bm->addAction("Platform Status...");
         connect(act, &QAction::triggered, this, &PluginBase::MakePlatformStatus);
      }

      connect(mainWindow, &wkf::MainWindow::MainWindowClose, this, &PluginBase::Close);
   }

   mCallbacks += wkf::Observer::StandardScenarioRemoved.Connect(&PluginBase::StdScenarioRemoved, this);
   mCallbacks += wkf::Observer::PlatformsAdded.Connect(&PluginBase::PlatformsAdded, this);
}

wkf::bm::PluginBase::~PluginBase()
{
   Close();
}

void wkf::bm::PluginBase::UpdatePlots(double aMaxTime)
{
   for (auto& plot : mPlots)
   {
      if (plot && plot->isVisible())
      {
         plot->Update(aMaxTime);
      }
   }
}

void wkf::bm::PluginBase::SetTimeIndicator(double aTime)
{
   for (auto& plot : mPlots)
   {
      if (plot && plot->isVisible())
      {
         plot->SetTimeIndicator(aTime);
      }
   }
}

void wkf::bm::PluginBase::RedrawPlots()
{
   for (auto& plot : mPlots)
   {
      if (plot && plot->isVisible())
      {
         plot->Redraw();
      }
   }
}

void wkf::bm::PluginBase::RescanPlatformStatus(double aSimTime)
{
   for (auto& ps : mPlatformStatus)
   {
      ps->RescanAll(aSimTime);
   }
}

void wkf::bm::PluginBase::MakeSidePlot()
{
   auto* plot = ut::qt::make_qt_ptr<wkf::bm::PlatformSidePlot>(mPlatformData,
                                                               mPlatformSidePreferences,
                                                               "Platform Count",
                                                               wkfEnv.GetMainWindow()->centralWidget());

   plot->resize(800, 600);
   plot->show();

   mPlots.emplace_back(plot);

   NewPlotCreated();
}

void wkf::bm::PluginBase::MakeTypePlot()
{
   auto* plot = ut::qt::make_qt_ptr<wkf::bm::PlatformTypePlot>(mPlatformData,
                                                               mPlatformTypePreferences,
                                                               "Platform Type Count",
                                                               wkfEnv.GetMainWindow()->centralWidget());

   plot->resize(800, 600);
   plot->show();

   mPlots.emplace_back(plot);

   NewPlotCreated();
}

void wkf::bm::PluginBase::MakePlatformStatus()
{
   auto* ps =
      ut::qt::make_qt_ptr<wkf::bm::PlatformStatus>(wkfEnv.GetMainWindow(), mPlatformStatusPreferences, &mPlatformData);
   ps->setAttribute(Qt::WA_DeleteOnClose);

   connect(ps,
           &PlatformStatus::AddAuxDataColumn,
           [this, ps](QString aKey, wkf::bm::RuleSet::Mode aMode) { AddAuxDataRuleSet(*ps, aKey, aMode); });

   connect(ps,
           &PlatformStatus::AddResourceAuxDataColumn,
           [this, ps](QString aNumerator, QString aDenominator)
           { AddResourceAuxDataRuleSet(*ps, aNumerator, aDenominator); });

   connect(ps,
           &PlatformStatus::destroyed,
           [this, ps]()
           {
              auto it = std::remove(mPlatformStatus.begin(), mPlatformStatus.end(), ps);
              mPlatformStatus.erase(it, mPlatformStatus.end());
           });

   InitializePlatformStatus(*ps);

   ps->show();
   mPlatformStatus.emplace_back(ps);

   NewPlotCreated();
}

void wkf::bm::PluginBase::StdScenarioRemoved(wkf::Scenario*)
{
   Close();
}

void wkf::bm::PluginBase::PlatformsAdded(const wkf::PlatformList& aPlatforms)
{
   for (auto& ps : mPlatformStatus)
   {
      for (const auto& platform : aPlatforms)
      {
         ps->AddPlatform(platform);
      }
   }
}

void wkf::bm::PluginBase::Close()
{
   for (auto& plot : mPlots)
   {
      if (plot)
      {
         delete plot;
      }
   }
   mPlots.clear();
   for (auto& ps : mPlatformStatus)
   {
      ps->close();
      ps->deleteLater();
   }
   mPlatformStatus.clear();
}
