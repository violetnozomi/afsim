// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVAIRCOMBATVISUALIZATION_HPP
#define RVAIRCOMBATVISUALIZATION_HPP

#include "RvPlugin.hpp"
#include "air_combat/WkfAirCombatDataContainer.hpp"
#include "air_combat/WkfAirCombatDisplayInterface.hpp"
#include "air_combat/WkfAirCombatVisualizationPrefObject.hpp"
#include "air_combat/WkfAirCombatVisualizationPrefWidget.hpp"

namespace RvAirCombatVisualization
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aName, const size_t aUniqueId);
   ~Plugin() override = default;

   void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;

protected:
   void                    GuiUpdate() override;
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

private:
   void AdvanceTimeRead(const rv::ResultData& aData) override;

   void PopulateData(const rv::ResultData&                aData,
                     wkf::AirCombatDataEngagementSummary& aTempData,
                     rv::ResultPlatform*                  aPlatform);

   const bool HasSA_Data(rv::ResultPlatform* aPlatform, const double aSimTime);

   std::unique_ptr<wkf::AirCombatDisplayInterface>        mDisplayInterfacePtr;
   PluginUiPointer<wkf::AirCombatVisualizationPrefWidget> mPrefWidgetPtr;
   wkf::Platform*                                         mSourcePlatformPtr;
   std::vector<wkf::Platform*>                            mTargetPlatforms;
   wkf::DataContainer                                     mDataContainer;
   bool                                                   mEnabled;
};
} // namespace RvAirCombatVisualization

#endif
