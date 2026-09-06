// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKF_BM_PLUGIN_BASE_HPP
#define WKF_BM_PLUGIN_BASE_HPP

#include "wkf_common_export.h"

#include <QPointer>

#include "VaCallbackHolder.hpp"
#include "WkfBM_PlatformDatabase.hpp"
#include "WkfBM_PlatformSidePlot.hpp"
#include "WkfBM_PlatformStatus.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"
#include "WkfBM_PlatformTypePlot.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
class Scenario;

namespace bm
{
class WKF_COMMON_EXPORT PluginBase : public wkf::Plugin
{
   Q_OBJECT

public:
   PluginBase(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles = eNO_ROLE);

   ~PluginBase() noexcept override;

   // Calls Update on both plots if they exist.
   void UpdatePlots(double aMaxTime);

   // Calls SetTimeIndicator on both plots if they exist.
   void SetTimeIndicator(double aTime);

   // Calls Redraw on both plots if they exist.
   void RedrawPlots();

   //! Calls RescanAll(aSimTime) for each platform status if visible.
   void RescanPlatformStatus(double aSimTime);

protected:
   wkf::bm::PlatformPlotDatabase mPlatformData;

   virtual void InitializePlatformStatus(PlatformStatus& aPlatformStatus) = 0;

   //! Adds an aux data rule set to the platform status.
   virtual void AddAuxDataRuleSet(PlatformStatus& aPlatformStatus, QString aKey, RuleSet::Mode aMode) = 0;

   virtual void AddResourceAuxDataRuleSet(PlatformStatus& aPlatformStatus, QString aNumerator, QString aDenominator) = 0;

private:
   void         MakeSidePlot();
   void         MakeTypePlot();
   void         MakePlatformStatus();
   virtual void NewPlotCreated() {}

   void StdScenarioRemoved(wkf::Scenario*);
   void PlatformsAdded(const wkf::PlatformList& aPlatforms);
   void Close();

   std::vector<QPointer<wkf::bm::PlotWidget>>     mPlots;
   std::vector<QPointer<wkf::bm::PlatformStatus>> mPlatformStatus;

   SidePlotPrefObject       mPlatformSidePreferences;
   TypePlotPrefObject       mPlatformTypePreferences;
   PlatformStatusPrefObject mPlatformStatusPreferences;

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace bm
} // namespace wkf

#endif
