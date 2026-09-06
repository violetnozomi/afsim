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
#include "RvPluginDetectionReport.hpp"

#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QVBoxLayout>

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvSensorDetectionModel.hpp"
#include "UtQtGL2DPlotWidget.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "statistics/WkfSensorDetectionReport.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvDetectionReport::Plugin,
                          "Detection Report",
                          "Displays a report of sensor detection attempts.",
                          "mystic")

RvDetectionReport::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
}

void RvDetectionReport::Plugin::ClearScenario(bool aFullReset)
{
   emit ScenarioCleared();
   // figure out some way to hide reports here!
}

void RvDetectionReport::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
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
               if (!ep_platform->GetDetectionAttemptMap().empty())
               {
                  wkf::Action* connectAction =
                     new wkf::Action(QIcon::fromTheme("plot"), QString("Sensor Detection Report"), aMenu);
                  connectAction->setData(QString(aEntityPtr->GetName().c_str()));
                  connect(connectAction,
                          &QAction::triggered,
                          this,
                          static_cast<void (RvDetectionReport::Plugin::*)(void)>(&RvDetectionReport::Plugin::CreateReport));
               }
            }
         }
      }
   }
}

void RvDetectionReport::Plugin::CreateReport()
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
         if (nullptr != ep_platform)
         {
            wkf::SensorDetectionReport* report = new wkf::SensorDetectionReport(wkfEnv.GetMainWindow());
            QString                     title  = QString("Sensor Detection Attempts: ") + platformName;
            report->setWindowTitle(title);
            report->setAttribute(Qt::WA_DeleteOnClose, true);
            RvDetectionReport::SensorDetectionModel* model =
               new RvDetectionReport::SensorDetectionModel(ep_platform, report);
            report->SetModel(model);
            report->show();
            connect(this, &RvDetectionReport::Plugin::ScenarioCleared, report, &QWidget::deleteLater);
            connect(this, &RvDetectionReport::Plugin::TimeSet, report, &wkf::SensorDetectionReport::SetTime);
            connect(this,
                    &RvDetectionReport::Plugin::SetCacheRange,
                    model,
                    &RvDetectionReport::SensorDetectionModel::SetCacheRange);
            report->EnableCustomPlotContextMenu();
            connect(report,
                    &wkf::SensorDetectionReport::CustomPlotContextMenu,
                    this,
                    &RvDetectionReport::Plugin::CustomPlotContextMenu);
            TimeSet(results->GetSimTime());
         }
      }
   }
   // build the report
   // give the model to the report
}

void RvDetectionReport::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   float time = aData.GetSimTime();

   // lets update our reports
   std::pair<float, float> rangeAvailable = aData.GetCacheRange(); // this is the data that is available
   emit                    SetCacheRange(rangeAvailable.first, rangeAvailable.second);
   emit                    TimeSet(time);
}

void RvDetectionReport::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvDetectionReport::Plugin::CustomPlotContextMenu(const QPoint& aPoint, const QPointF& aDataPoint, bool aPlot2D)
{
   if (aPlot2D)
   {
      QMenu menu;
      menu.addAction("Set current time: " + QString::number(aDataPoint.x()));
      if (menu.exec(aPoint))
      {
         rvEnv.SetTime(aDataPoint.x());
      }
   }
}
