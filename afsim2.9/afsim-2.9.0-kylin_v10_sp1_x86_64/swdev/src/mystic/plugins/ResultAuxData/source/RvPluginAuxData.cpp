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
#include "RvPluginAuxData.hpp"

#include "RvAuxDataPlotUpdater.hpp"
#include "RvAuxDataUpdater.hpp"
#include "RvEventPipeMessages.hpp"
#include "RvPlottingWidget.hpp"
#include "RvResultPlatform.hpp"
#include "UtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitTypes.hpp"
#include "WkfVtkEnvironment.hpp"
#include "data_ring/WkfAttachmentDataRing.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   RvAuxData::Plugin,
   "Aux Data",
   "Gets platform data from the simulation and prepares it for display within the platform data display",
   "mystic")

static float GetRvSimTime() noexcept
{
   auto* results = rvEnv.GetData();
   if (results)
   {
      return results->GetSimTime();
   }
   return 0;
}

static rv::ResultPlatform* GetRvPlatform(const wkf::Platform* aPlatform) noexcept
{
   auto* results = rvEnv.GetData();
   if (results)
   {
      auto* db = results->GetDb();
      if (db)
      {
         return db->FindPlatform(aPlatform->GetIndex());
      }
   }
   return nullptr;
}

RvAuxData::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
   mAuxDataItemPtr = new wkf::PlatformDataTreeItem();
   mAuxDataItemPtr->setText(0, "AuxData");
   mAuxDataItemPtr->SetUnhideable(true);
   mAuxDataItemPtr->setData(0, wkf::DISPLAY_PRECEDENCE, 2);

   connect(&wkfEnv, &wkf::Environment::PlatformOfInterestChanged, this, &RvAuxData::Plugin::UpdateAuxData);
}

std::unique_ptr<wkf::Updater> RvAuxData::Plugin::GetUpdater(const QString&     aPlatformName,
                                                            const QString&     aDatum,
                                                            const unsigned int aContext) const
{
   return wkf::make_updater<RvAuxData::Updater>(aPlatformName.toStdString(), aDatum.toStdString());
}

QList<QTreeWidgetItem*> RvAuxData::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   auto children = mAuxDataItemPtr->takeChildren();
   for (auto* child : children)
   {
      delete child;
   }
   mAuxDataWidgets.clear();

   return {mAuxDataItemPtr};
}

void RvAuxData::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   UpdateAuxData(wkfEnv.GetPlatformOfInterest());
}

std::unique_ptr<rv::PlotUpdater> RvAuxData::Plugin::GetPlotUpdater(const QString&               aX,
                                                                   const QString&               aY,
                                                                   const wkf::PlatformGrouping& aGrouping,
                                                                   const QString&               aSubcategory) const
{
   return ut::make_unique<RvAuxData::PlotUpdater>(aX, aY, aGrouping);
}

static QString AuxDataValueToString(const rv::AuxDataValue& aValue)
{
   switch (aValue.type())
   {
   case rv::AuxDataType::Boolean:
      return aValue.boolean() ? "true" : "false";
      break;
   case rv::AuxDataType::Integer:
      return QString::number(aValue.integer());
      break;
   case rv::AuxDataType::Real:
      return QString::number(aValue.real());
      break;
   case rv::AuxDataType::Text:
      return QString::fromStdString(aValue.text());
      break;
   default:
      return "(unknown type)";
   }
}

void RvAuxData::Plugin::UpdateAuxData(wkf::Platform* aPlatform)
{
   if (aPlatform)
   {
      auto* rvPlatform = GetRvPlatform(aPlatform);
      if (rvPlatform)
      {
         const auto* msg = rvPlatform->FindFirstBefore<rv::MsgAuxData>(GetRvSimTime());
         if (msg)
         {
            UpdateAuxDataImpl(msg);
         }
      }
   }
}

void RvAuxData::Plugin::UpdateAuxDataImpl(const rv::MsgAuxData* aAuxData)
{
   // Update existing items and add new ones.
   for (auto& iter : aAuxData->auxData())
   {
      auto& item = mAuxDataWidgets[iter.name()];
      if (item == nullptr)
      {
         switch (iter.type())
         {
         case rv::AuxDataType::Integer:
         case rv::AuxDataType::Real:
            item = new wkf::UnitlessTreeWidgetItem(mAuxDataItemPtr, QString::fromStdString(iter.name()));
            item->SetPlottable();
            break;
         case rv::AuxDataType::Boolean:
         case rv::AuxDataType::Text:
         case rv::AuxDataType::Unknown:
         default:
            item = new wkf::PlatformDataTreeItem(mAuxDataItemPtr);
            break;
         }
         item->SetUnhideable();
         item->setText(0, QString::fromStdString(iter.name()));
         item->setData(0, wkf::UPDATER_ROLE, true);
      }

      item->setText(1, AuxDataValueToString(iter));
   }
   /*
   // Remove old unneeded nodes.
   for (auto iter = mAuxDataWidgets.begin(); iter != mAuxDataWidgets.end();)
   {
      const auto beginIt = aAuxData->auxData().begin();
      const auto endIt   = aAuxData->auxData().end();

      auto found = std::find_if(beginIt, endIt,
                                [iter](const rv::AuxDataValue& aValue)
      {
         return iter->first == aValue.name();
      });
      if (found != endIt)
      {
         // if found, do not remove
         iter++;
      }
      else
      {
         // if not found, remove
         delete mAuxDataItemPtr->takeChild(found - beginIt);
         iter = mAuxDataWidgets.erase(iter);
      }
   }*/
}
