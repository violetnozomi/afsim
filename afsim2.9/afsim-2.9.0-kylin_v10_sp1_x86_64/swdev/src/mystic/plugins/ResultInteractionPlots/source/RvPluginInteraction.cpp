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
#include "RvPluginInteraction.hpp"

#include <QAction>
#include <QMenu>

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "WaterfallPlotDialog.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvInteractionPlots::Plugin, "Interaction Plots", "Displays waterfall plots", "mystic")

RvInteractionPlots::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
}

void RvInteractionPlots::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      rv::ResultData* results = rvEnv.GetData();
      if (nullptr != results)
      {
         rv::ResultDb* db = results->GetDb();
         if (nullptr != db)
         {
            rv::ResultPlatform* ep_platform = db->FindPlatformByName(aEntityPtr->GetName());
            if (nullptr != ep_platform)
            {
               QAction* waterfallAction = new QAction(QIcon::fromTheme("plot"), QString("Waterfall Plot"), aMenu);
               aMenu->addAction(waterfallAction);
               waterfallAction->setData(QString(aEntityPtr->GetName().c_str()));
               connect(waterfallAction,
                       &QAction::triggered,
                       this,
                       static_cast<void (Plugin::*)(void)>(&Plugin::DrawWaterfallPlot));
            }
         }
      }
   }
}

void RvInteractionPlots::Plugin::ClearScenario(bool aFullReset)
{
   emit ScenarioCleared();
   // figure out some way to hide reports here!
}

void RvInteractionPlots::Plugin::DrawWaterfallPlot()
{
   QAction* sender       = dynamic_cast<QAction*>(QObject::sender());
   QString  platformName = sender->data().toString();

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* ep_platform = db->FindPlatformByName(platformName);
         if (ep_platform)
         {
            WaterfallPlotDialog* plot = new WaterfallPlotDialog(wkfEnv.GetMainWindow()->centralWidget(), db, ep_platform);
            QString              title = QString("Waterfall Plot: ") + platformName;
            plot->setWindowTitle(title);
            plot->setAttribute(Qt::WA_DeleteOnClose, true);
            plot->show();
            connect(this, &Plugin::ScenarioCleared, plot, &QWidget::deleteLater);
            connect(this, &Plugin::TimeSet, plot, &WaterfallPlotDialog::SetTime);
            //           connect(this, &Plugin::SetCacheRange, model, &WaterfallModel::SetCacheRange);
            //            report->EnableCustomPlotContextMenu();
            //            connect(report, &wkf::SensorDetectionReport::CustomPlotContextMenu, this,
            //            &RvDetectionReport::Plugin::CustomPlotContextMenu);
            TimeSet(results->GetSimTime());
         }
      }
   }
}

void RvInteractionPlots::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   float time = aData.GetSimTime();

   // lets update our reports
   emit TimeSet(time);
}

void RvInteractionPlots::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}
