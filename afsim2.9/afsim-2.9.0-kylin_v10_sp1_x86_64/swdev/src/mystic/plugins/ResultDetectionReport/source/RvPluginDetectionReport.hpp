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
#ifndef RVPLUGINDETECTIONREPORT_HPP
#define RVPLUGINDETECTIONREPORT_HPP

#include "RvPlugin.hpp"

namespace RvDetectionReport
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntity) override;
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   // this is a 2 Hz regular update when time isn't changing (for things that need updates outside of time changes)
   void RegularRead(const rv::ResultData& aData) override;
   void CustomPlotContextMenu(const QPoint& aPoint, const QPointF& aDataPoint, bool aPlot2D);

signals:
   void ScenarioCleared();
   void TimeSet(float aTime);
   void SetCacheRange(float aStartTime, float aEndTime);

private:
   void CreateReport();
   void ClearScenario(bool aFullReset) override;
};
} // namespace RvDetectionReport
#endif
