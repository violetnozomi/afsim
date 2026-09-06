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

#ifndef WKFAIRCOMBATVISUALIZATIONPREFWIDGET_HPP
#define WKFAIRCOMBATVISUALIZATIONPREFWIDGET_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfAirCombatVisualizationPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfAirCombatPrefWidget.h"

namespace wkf
{
class WKF_AIR_COMBAT_COMMON_EXPORT AirCombatVisualizationPrefWidget :
   public wkf::PrefWidgetT<AirCombatVisualizationPrefObject>
{
public:
   AirCombatVisualizationPrefWidget();

   QString GetCategoryHint() const override { return "Air Combat"; }

private:
   void ReadPreferenceData(const AirCombatVisualizationPrefData& aPrefData) override;
   void WritePreferenceData(AirCombatVisualizationPrefData& aPrefData) override;

   const std::string mNoRingOption        = "None";
   const std::string mDefensivenessOption = "Defensiveness";
   const std::string mFuelLevelOption     = "Fuel Level";
   const std::string mGLoadOption         = "G Load";

   Ui_WkfAirCombatPrefWidget mUi;
};
} // namespace wkf

#endif // !WKFAIRCOMBATVISUALIZATIONPREFWIDGET_HPP
