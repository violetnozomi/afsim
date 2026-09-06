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
#ifndef RVPLUGINVASCENARIOMANAGER_HPP
#define RVPLUGINVASCENARIOMANAGER_HPP

#include "RvPlugin.hpp"

namespace wkf
{
class Environment;
}

namespace RvVaScenarioManager
{
class Interface;

class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;
   void PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform) override;

   void GuiUpdate() override;
   void ClearScenario(bool aFullReset) override;

private:
   // managed by Qt
   Interface* mInterfacePtr;
};
} // namespace RvVaScenarioManager
#endif
