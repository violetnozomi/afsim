// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLUGININTERACTION_HPP
#define RVPLUGININTERACTION_HPP

#include "RvPlugin.hpp"

namespace RvInteractionPlots
{
class Interface;

class Plugin : public rv::Plugin
{
   Q_OBJECT
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);


protected:
private:
   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntity) override;

   void DrawWaterfallPlot();
   void ClearScenario(bool aFullReset) override;

   void AdvanceTimeRead(const rv::ResultData& aData) override;
   // this is a 2 Hz regular update when time isn't changing (for things that need updates outside of time changes)
   void RegularRead(const rv::ResultData& aData) override;

signals:
   void ScenarioCleared();
   void TimeSet(float aTime);
   void SetCacheRange(float aStartTime, float aEndTime);
};
} // namespace RvInteractionPlots
#endif
