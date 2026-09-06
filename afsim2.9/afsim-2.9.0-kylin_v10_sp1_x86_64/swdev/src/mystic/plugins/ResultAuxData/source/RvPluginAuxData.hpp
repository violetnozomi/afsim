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
#ifndef RV_PLUGIN_AUX_DATA_HPP
#define RV_PLUGIN_AUX_DATA_HPP

#include "RvPlugin.hpp"
#include "WkfPlatformDataTreeItem.hpp"

namespace wkf
{
class PlatformGrouping;
class UnitTreeWidgetItem;
} // namespace wkf

namespace rv
{
class MsgAuxData;
}

namespace RvAuxData
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   //! Returns a single-element list of newly allocated platform data items.
   //! Qt is expected to take ownership of the returned item.
   QList<QTreeWidgetItem*> GetPlatformData(const std::string& aPlatformName) override;

   //! Returns a newly allocated RvAuxData::PlotUpdater.
   std::unique_ptr<rv::PlotUpdater> GetPlotUpdater(const QString&               aX,
                                                   const QString&               aY,
                                                   const wkf::PlatformGrouping& aGrouping,
                                                   const QString&               aSubcategory) const override;

   //! Returns a newly allocated RvAuxData::Updater.
   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

private slots:
   void AdvanceTimeRead(const rv::ResultData& aData) override;

private:
   void UpdateAuxData(wkf::Platform* aPlatform);
   void UpdateAuxDataImpl(const rv::MsgAuxData* aAuxData);

   wkf::PlatformDataTreeItem*                        mAuxDataItemPtr;
   std::map<std::string, wkf::PlatformDataTreeItem*> mAuxDataWidgets;
};
} // namespace RvAuxData
#endif
