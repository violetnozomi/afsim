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
#ifndef RVPLUGINSCRIPTDATAFEED_HPP
#define RVPLUGINSCRIPTDATAFEED_HPP

#include "RvEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "WkfPlatformDataTreeItem.hpp"

namespace wkf
{
class PlatformDataTreeItem;
class PlatformGrouping;
class UnitTreeWidgetItem;
} // namespace wkf

namespace RvScriptDataFeed
{
class Interface;

class UnionTreeItem : public wkf::PlatformDataTreeItem
{
public:
   UnionTreeItem(QTreeWidgetItem* aParent, const QString& aName, bool aPlottable)
      : PlatformDataTreeItem(aParent)
   {
      QTreeWidgetItem::setText(0, aName);
      if (aPlottable)
      {
         SetPlottable();
      }
   }

private:
};

class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;
   QList<QTreeWidgetItem*> GetPlatformData(const std::string& aPlatformName) override;


   std::unique_ptr<rv::PlotUpdater> GetPlotUpdater(const QString&               aX,
                                                   const QString&               aY,
                                                   const wkf::PlatformGrouping& aGrouping,
                                                   const QString&               aSubcategory) const override;
   void                             MenuPlot();

   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

private slots:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;
   void GuiUpdate() override;

private:
   // managed by Qt
   Interface* mInterfacePtr;

   using FeedId = QString;

   void CreateItem(QTreeWidgetItem* aParent, FeedId aType, rv::MsgScriptData_Value::FieldType aFieldType);

   std::map<FeedId, QString> mDataToStringMap;

   QList<QTreeWidgetItem*>          mTopLevelWidgets;
   std::map<FeedId, UnionTreeItem*> mDataWidgets;
   wkf::PlatformDataTreeItem*       mScriptParent;
};
} // namespace RvScriptDataFeed
#endif
