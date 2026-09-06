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
#ifndef WKFHUDPREFWIDGETHPP
#define WKFHUDPREFWIDGETHPP

#include "wkf_air_combat_common_export.h"

#include "WkfHUD_PrefObject.hpp"
#include "WkfPrefWidget.hpp"

class QTreeWidgetItem;
class QComboBox;

namespace wkf
{
// The HeadsUpDisplay::PrefWidget provides the preferences widget with
// which the users can set their preferences for the HeadsUpDisplay plugin.
class WKF_AIR_COMBAT_COMMON_EXPORT HUD_PrefWidget : public wkf::PrefWidgetT<HUD_PrefObject>
{
public:
   HUD_PrefWidget();

   QString GetCategoryHint() const override { return "Air Combat"; }

private:
   void       ReadPreferenceData(const wkf::HUD_PrefData& aPrefData) override;
   void       WritePreferenceData(wkf::HUD_PrefData& aPrefData) override;
   QComboBox* mResolution;
};
} // namespace wkf

#endif
