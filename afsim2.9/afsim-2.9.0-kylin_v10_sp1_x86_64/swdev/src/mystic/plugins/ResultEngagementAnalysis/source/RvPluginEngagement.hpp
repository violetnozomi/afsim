// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLUGINENGAGEMENT_HPP
#define RVPLUGINENGAGEMENT_HPP

#include <mutex>
#include <set>

#include "RvPlugin.hpp"
#include "statistics/WkfEngagementStatistics.hpp"

namespace rv
{
class MsgBase;
class MsgWeaponFired;
class MsgWeaponTerminated;
}; // namespace rv

namespace RvEngagement
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

private slots:
   void ShowEngagements();
   void EngagementFilterChanged();

private:
   void EngagementFilterChangedPrivate();
   void ClearScenario(bool aFullReset) override;

   void WeaponFiredEventRead(const rv::MsgWeaponFired& aEvent) override;
   void WeaponTerminatedEventRead(const rv::MsgWeaponTerminated& aEvent) override;


   void ProcessWeaponFired(const rv::MsgWeaponFired* aEvent);
   void ProcessWeaponTerminated(const rv::MsgWeaponTerminated* aEvent);

   void        BuildRowContextMenu(QMenu* aMenuPtr, const QStringList& aStrList);
   void        TraceEvent(const QStringList& aStrList);
   static bool EngageSort(const std::pair<rv::MsgBase*, unsigned int>& i, const std::pair<rv::MsgBase*, unsigned int>& j);

   wkf::EngagementStatistics*      mEngagementStatistics;
   std::vector<const rv::MsgBase*> mEngagementEvents;
   std::vector<const rv::MsgBase*> mUnprocessedEngagementEvents;
   std::mutex                      mMutex;
};
} // namespace RvEngagement
#endif
