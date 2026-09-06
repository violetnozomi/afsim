// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVCOMMVISPLUGIN_HPP
#define RVCOMMVISPLUGIN_HPP

#include "RvCommVisDialog.hpp"
#include "RvPlugin.hpp"
#include "comm_vis/WkfCommVisDataContainer.hpp"
#include "comm_vis/WkfCommVisDialog.hpp"

namespace RvCommVis
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void CommInfoRead(rv::ResultDb& aData, const rv::MsgCommInfo& aCommInfo) override;
   void NetworkInfoRead(rv::ResultDb& aData, const rv::MsgNetworkInfo& aNetworkInfo) override;
   void RouterInfoRead(rv::ResultDb& aData, const rv::MsgRouterInfo& aRouterInfo) override;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void Start();

private:
   ut::qt::UiPointer<wkf::CommVisDialog> mDialog;
   wkf::DataContainer                    mCommData;
};
} // namespace RvCommVis
#endif
