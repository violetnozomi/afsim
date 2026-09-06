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
#ifndef RVPLUGINSTATISTICS_HPP
#define RVPLUGINSTATISTICS_HPP

#include <mutex>
#include <set>

#include "RvMetaDataDialog.hpp"
#include "RvPlugin.hpp"
#include "filter_widget/WkfFilterWidget.hpp"

namespace RvStatistics
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

private slots:
   void ShowStats();
   void ShowEventList();
   void ShowMetaData();

   void ClearScenario(bool aFullReset) override;
   void MetaDataRead(const rv::MsgExecData& aDrawCommand) override;

signals:
   void ScenarioCleared();

private:
   PluginUiPointer<QDialog> mEventListDialog;
   WkfFilter::FilterWidget* mEventListWidget;
   MetaDataDialog*          mMetaDataDialog;
   std::vector<QStringList> mMetaDataStrings;
};
} // namespace RvStatistics
#endif
