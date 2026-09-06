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
#ifndef RVORBITPLUGIN_HPP
#define RVORBITPLUGIN_HPP

#include "RvPlugin.hpp"
#include "orbit/WkfOrbitPluginBase.hpp"

namespace wkf
{
class AttachmentOrbit;
class Platform;
} // namespace wkf

namespace RvOrbit
{
class Plugin : public rv::PluginT<wkf::OrbitPluginBase>
{
   Q_OBJECT
public:
   explicit Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void ResetOptionStates() override;
   void SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;

   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   void ClearScenario(bool aFullReset) override;

private:
   void SetEpoch(const UtCalendar& aEpoch) override;

   std::map<unsigned int, unsigned int> mOrbitIndex; // keeps track of the active orbit message index
};
} // namespace RvOrbit
#endif
