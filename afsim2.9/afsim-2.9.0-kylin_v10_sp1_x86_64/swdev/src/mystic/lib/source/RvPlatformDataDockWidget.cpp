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
#include "RvPlatformDataDockWidget.hpp"

#include <QIcon>
#include <QMenu>

#include "RvEventPipeUtil.hpp"
#include "RvPlottingWidget.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "VaUtils.hpp"
#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfFactory.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitTypes.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "data_ring/WkfAttachmentDataRing.hpp"
#include "data_ring/WkfRingDataDialog.hpp"

rv::PlatformDataDockWidget::PlatformDataDockWidget(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
   : wkf::PlatformDataDockWidget(parent, f)
   , mContextMenuItemPlugin(nullptr)
   , mContextMenuItemId("")
{
}

void rv::PlatformDataDockWidget::PlotActionHandler()
{
   int         catType     = sender()->property("catType").toInt();
   std::string catName     = sender()->property("catName").toString().toStdString();
   QString     xAxis       = "Time";
   QString     yAxis       = sender()->property("dataType").toString();
   QString     subCategory = sender()->property("subcat").toString();

   PlottingWidget* plotWidget =
      vaEnv.GetFactory()->CreateDockWidget<PlottingWidget>("PlottingWidget", wkfEnv.GetMainWindow()->centralWidget());

   // Find the updater
   if (mContextMenuItemPlugin)
   {
      std::unique_ptr<PlotUpdater> updaterPtr =
         mContextMenuItemPlugin->GetPlotUpdater(xAxis, yAxis, wkf::PlatformGrouping(catType, catName), subCategory);
      if (updaterPtr != nullptr)
      {
         plotWidget->SetUpdater(std::move(updaterPtr));
      }
   }
   plotWidget->show();
}

void rv::PlatformDataDockWidget::AddRingActionHandler()
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      wkf::Platform* platform = scenario->FindPlatformByName(mPlatformName);
      if (platform != nullptr)
      {
         vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
         if (viewer != nullptr)
         {
            // The updater names for latitude, longitude, and altitude are different than
            // the values that they use in the dock widget.
            QString dataName = sender()->property("dataType").toString();
            if (dataName == "Latitude" || dataName == "Longitude")
            {
               dataName = "Position";
            }
            else if (dataName == "Altitude MSL")
            {
               dataName = "Altitude";
            }

            wkf::RingDataDialog* dialog = new wkf::RingDataDialog(wkfEnv.GetMainWindow());

            // Initialize the maximum or minimum value to 200% of the current value.
            double value = sender()->property("initialValue").toDouble();
            value *= 2;
            value > 0 ? dialog->SetMaxValue(value) : dialog->SetMinValue(value);

            // Determine the unit to display in the dialog.
            QString unit;
            if (dataName == "Position")
            {
               unit = " degrees";
            }
            else
            {
               auto updater = mContextMenuItemPlugin->GetUpdater(QString::fromStdString(mPlatformName), dataName);
               if (updater != nullptr)
               {
                  unit = updater->GetValueString();
                  unit = unit.remove(QRegExp("\\W^\\/"));
                  unit = unit.remove(QRegExp("\\."));
                  unit = unit.remove(QRegExp("[0]"));
                  unit = unit.prepend(" ");
               }
            }
            dialog->SetUnit(unit);

            // Create a ring attachment when the dialog is accepted.
            connect(dialog,
                    &QDialog::accepted,
                    [&]()
                    {
                       wkf::AttachmentDataRing* attachment =
                          vespa::make_attachment<wkf::AttachmentDataRing>(*platform, viewer);

                       auto updater = mContextMenuItemPlugin->GetUpdater(QString::fromStdString(mPlatformName), dataName);
                       if (updater != nullptr)
                       {
                          attachment->SetUpdater(std::move(updater));
                          attachment->SetLevel(platform->FindAttachmentsOfType<wkf::AttachmentDataRing>().size());
                          attachment->SetMinValue(dialog->GetMinValue());
                          attachment->SetMaxValue(dialog->GetMaxValue());
                          attachment->SetMinColor(dialog->GetMinColor());
                          attachment->SetMaxColor(dialog->GetMaxColor());
                          attachment->SetClampedToBase(dialog->GetClamped());
                       }
                    });

            dialog->exec();
         }
      }
   }
}

void rv::PlatformDataDockWidget::RemoveRingActionHandler()
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      wkf::Platform* platform = scenario->FindPlatformByName(mPlatformName);
      if (platform != nullptr)
      {
         int     removedRingLevel = 0;
         QString senderData       = sender()->property("dataType").toString();
         for (auto& ring : platform->FindAttachmentsOfType<wkf::AttachmentDataRing>())
         {
            if (ring->GetUpdater()->GetUpdaterName() == senderData ||
                (ring->GetUpdater()->GetUpdaterName() == "Altitude" && senderData == "Altitude MSL") ||
                (ring->GetUpdater()->GetUpdaterName() == "Position" &&
                 (senderData == "Latitude" || senderData == "Longitude")))
            {
               removedRingLevel = ring->GetLevel();
               platform->RemoveAttachment(ring->GetUniqueId());
            }
         }

         // Lower each remaining ring.
         if (removedRingLevel != 0)
         {
            for (auto& ring : platform->FindAttachmentsOfType<wkf::AttachmentDataRing>())
            {
               if (ring->GetLevel() > removedRingLevel)
               {
                  ring->SetLevel(ring->GetLevel() - 1);
               }
            }
         }
      }
   }
}

void rv::PlatformDataDockWidget::BuildMenuForItem(QMenu& aMenu, QTreeWidgetItem& aItem)
{
   mContextMenuItemPlugin = dynamic_cast<Plugin*>(GetPluginFromItem(&aItem));
   mContextMenuItemPlugin->BuildItemContextMenu(&aMenu, &aItem);
   auto pdtItem = dynamic_cast<wkf::PlatformDataTreeItem&>(aItem);

   if (aMenu.actions().empty() && pdtItem.Plottable())
   {
      QString  text   = QString("Plot ") + mPlatformName.c_str();
      QAction* action = new QAction(QIcon::fromTheme("plot"), text, &aMenu);
      action->setProperty("catType", wkf::PlatformGrouping::eIndividual);
      action->setProperty("catName", mPlatformName.c_str());
      action->setProperty("dataType", aItem.text(0));
      connect(action, &QAction::triggered, this, &PlatformDataDockWidget::PlotActionHandler);
      aMenu.addAction(action);

      rv::ResultData* results = rvEnv.GetData();
      if (nullptr != results)
      {
         rv::ResultDb* db = results->GetDb();
         if (nullptr != db)
         {
            rv::ResultPlatform* ep_platform = db->FindPlatformByName(mPlatformName);
            if (nullptr != ep_platform)
            {
               int                             platIdx = ep_platform->GetPlatformIndex();
               std::set<wkf::PlatformGrouping> cats    = wkfEnv.GetGroupings(platIdx);
               for (auto& it : cats)
               {
                  QString  text   = QString("Plot ") + it.TypeString().c_str() + " = " + it.Name().c_str();
                  QAction* action = new QAction(QIcon::fromTheme("plot"), text, &aMenu);
                  action->setProperty("catType", it.Type());
                  action->setProperty("catName", it.Name().c_str());
                  action->setProperty("dataType", aItem.text(0));
                  connect(action, &QAction::triggered, this, &PlatformDataDockWidget::PlotActionHandler);
                  aMenu.addAction(action);
               }

               AddRingItem(aMenu, aItem);
            }
         }
      }
   }
}

void rv::PlatformDataDockWidget::AddRingItem(QMenu& aMenu, QTreeWidgetItem& aItem)
{
   bool ringExists = false;

   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      wkf::Platform* platform = scenario->FindPlatformByName(mPlatformName);
      if (platform != nullptr)
      {
         for (const auto& ring : platform->FindAttachmentsOfType<wkf::AttachmentDataRing>())
         {
            wkf::Updater* updater = ring->GetUpdater();
            if (updater != nullptr)
            {
               if (updater->GetUpdaterName() == aItem.text(0) ||
                   (updater->GetUpdaterName() == "Altitude" && aItem.text(0) == "Altitude MSL") ||
                   (updater->GetUpdaterName() == "Position" &&
                    (aItem.text(0) == "Latitude" || aItem.text(0) == "Longitude")))
               {
                  ringExists = true;
                  break;
               }
            }
         }
      }
   }

   if (ringExists)
   {
      QAction* ringAction = new QAction(QIcon::fromTheme("data_ring"), "Remove " + aItem.text(0) + " Ring", &aMenu);
      ringAction->setProperty("dataType", aItem.text(0));
      connect(ringAction, &QAction::triggered, this, &PlatformDataDockWidget::RemoveRingActionHandler);
      aMenu.addAction(ringAction);
   }
   else
   {
      QAction* ringAction = new QAction(QIcon::fromTheme("data_ring"), "Add " + aItem.text(0) + " Ring", &aMenu);
      ringAction->setProperty("dataType", aItem.text(0));

      double initialValue = 0;
      if (aItem.text(0) == "Latitude")
      {
         rv::ResultData* data = rvEnv.GetData();
         if (data != nullptr)
         {
            int                 index = data->FindPlatformIndex(mPlatformName);
            rv::ResultPlatform* plat  = data->FindPlatform(index);
            if (plat != nullptr)
            {
               const auto state = plat->FindFirstBefore<rv::MsgEntityState>(rvEnv.GetData()->GetSimTime());
               if (state != nullptr)
               {
                  UtEntity tmpEntity;
                  tmpEntity.SetLocationWCS(toVec3(state->state().locationWCS()).GetData());

                  double lat, lon, alt;
                  tmpEntity.GetLocationLLA(lat, lon, alt);
                  initialValue = lat;
               }
            }
         }
      }
      else if (aItem.text(0) == "Longitude")
      {
         rv::ResultData* data = rvEnv.GetData();
         if (data != nullptr)
         {
            int                 index = data->FindPlatformIndex(mPlatformName);
            rv::ResultPlatform* plat  = data->FindPlatform(index);
            if (plat != nullptr)
            {
               const auto state = plat->FindFirstBefore<rv::MsgEntityState>(rvEnv.GetData()->GetSimTime());
               if (state != nullptr)
               {
                  UtEntity tmpEntity;
                  tmpEntity.SetLocationWCS(toVec3(state->state().locationWCS()).GetData());

                  double lat, lon, alt;
                  tmpEntity.GetLocationLLA(lat, lon, alt);
                  initialValue = lon;
               }
            }
         }
      }
      else
      {
         QString valueStr = aItem.text(1).split(" ").first();
         initialValue     = valueStr.toDouble();
      }
      ringAction->setProperty("initialValue", initialValue);

      connect(ringAction, &QAction::triggered, this, &PlatformDataDockWidget::AddRingActionHandler);
      aMenu.addAction(ringAction);
   }
}
