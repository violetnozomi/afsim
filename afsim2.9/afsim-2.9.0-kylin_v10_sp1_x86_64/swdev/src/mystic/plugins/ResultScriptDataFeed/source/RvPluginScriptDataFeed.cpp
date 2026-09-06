// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPluginScriptDataFeed.hpp"

#include <memory>

#include <QMenu>

#include "RvPlottingWidget.hpp"
#include "RvScriptDataInterface.hpp"
#include "RvScriptDataPlotUpdater.hpp"
#include "RvScriptDataUpdater.hpp"
#include "UtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfUnitTypes.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   RvScriptDataFeed::Plugin,
   "Script Data Feed",
   "Gets user published script data, and adds it to the platform data panel and supports plotting",
   "mystic")

RvScriptDataFeed::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mInterfacePtr(new Interface(this))
   , mScriptParent(nullptr)
{
   mScriptParent = new wkf::PlatformDataTreeItem();
   mScriptParent->setText(0, "Script Data");
   mTopLevelWidgets.push_back(mScriptParent);
}

QList<wkf::PrefWidget*> RvScriptDataFeed::Plugin::GetPreferencesWidgets() const
{
   return QList<wkf::PrefWidget*>();
}

QList<QTreeWidgetItem*> RvScriptDataFeed::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   mInterfacePtr->SetPlatformOfInterest(aPlatformName);

   return mTopLevelWidgets;
}

void RvScriptDataFeed::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
}

void RvScriptDataFeed::Plugin::RegularRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
}

void RvScriptDataFeed::Plugin::GuiUpdate()
{
   const RvScriptDataFeed::Interface::PlatformData& data = mInterfacePtr->GetPlatformData();

   if (mScriptParent)
   {
      mScriptParent->SetHidden(data.empty());
   }
   if (mInterfacePtr->CheckReset())
   {
      for (auto&& it : mDataWidgets)
      {
         delete it.second;
      }
      mDataWidgets.clear();
   }
   for (auto&& it : data)
   {
      std::map<FeedId, UnionTreeItem*>::iterator jt = mDataWidgets.find(it.first.c_str());
      if (jt == mDataWidgets.end())
      {
         CreateItem(mScriptParent, QString::fromStdString(it.first.c_str()), it.second.GetFieldType());
         jt = mDataWidgets.find(it.first.c_str());
         if (jt == mDataWidgets.end())
         {
            break;
         }
      }
      switch (it.second.GetFieldType())
      {
      case (rv::MsgScriptData_Value::cFT_floating):
         jt->second->setText(1, QString::number(it.second.floating()));
         break;
      case (rv::MsgScriptData_Value::cFT_integer):
         jt->second->setText(1, QString::number(it.second.integer()));
         break;
      case (rv::MsgScriptData_Value::cFT_boolean):
         jt->second->setText(1, it.second.boolean() ? "true" : "false");
         break;
      case (rv::MsgScriptData_Value::cFT_text):
         jt->second->setText(1, QString::fromStdString(it.second.text()));
         break;
      case (rv::MsgScriptData_Value::cFT_NONE):
      default:
         break;
      }
   }
}

void RvScriptDataFeed::Plugin::CreateItem(QTreeWidgetItem* aParent, FeedId aType, rv::MsgScriptData_Value::FieldType aFieldType)
{
   bool plot = false;
   if ((aFieldType == rv::MsgScriptData_Value::cFT_floating) || (aFieldType == rv::MsgScriptData_Value::cFT_integer))
   {
      plot = true;
   }
   wkf::PlatformDataTreeItem* item = nullptr;
   item = mDataWidgets[aType] = new UnionTreeItem(aParent, aType, plot);
   item->setData(0, wkf::UPDATER_ROLE, true);
   item->SetUnhideable();
   aParent->addChild(item);
}

std::unique_ptr<rv::PlotUpdater> RvScriptDataFeed::Plugin::GetPlotUpdater(const QString&               aX,
                                                                          const QString&               aY,
                                                                          const wkf::PlatformGrouping& aGrouping,
                                                                          const QString& aSubcategory) const
{
   return ut::make_unique<PlotUpdater>(aX, aY, aGrouping);
}

std::unique_ptr<wkf::Updater> RvScriptDataFeed::Plugin::GetUpdater(const QString&     aPlatformName,
                                                                   const QString&     aDatum,
                                                                   const unsigned int aContext) const
{
   return wkf::make_updater<Updater>(aPlatformName, aDatum);
}
