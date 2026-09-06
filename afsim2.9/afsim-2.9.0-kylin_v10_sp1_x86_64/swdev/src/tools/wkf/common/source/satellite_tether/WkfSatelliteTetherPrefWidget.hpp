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

#ifndef WKFSATELLITETETHERPREFWIDGET_HPP
#define WKFSATELLITETETHERPREFWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfSatelliteTetherPrefObject.hpp"

class QCheckBox;

namespace wkf
{
class WKF_COMMON_EXPORT SatelliteTetherPrefWidget : public PrefWidgetT<SatelliteTetherPrefObject>
{
public:
   SatelliteTetherPrefWidget();
   ~SatelliteTetherPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Space"; }

private:
   void ReadPreferenceData(const SatelliteTetherPrefData& aPrefData) override;
   void WritePreferenceData(SatelliteTetherPrefData& aPrefData) override;

   QCheckBox* mShowNames;
   QCheckBox* mShowFutureTracks;
   QCheckBox* mEnableLighting;
};
} // namespace wkf

#endif
