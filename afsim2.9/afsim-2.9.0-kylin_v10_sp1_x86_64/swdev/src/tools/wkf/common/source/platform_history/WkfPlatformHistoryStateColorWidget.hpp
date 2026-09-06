// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WkfPlatformHistoryStatePrefWidget_HPP
#define WkfPlatformHistoryStatePrefWidget_HPP

#include "wkf_common_export.h"

#include "WkfPlatformHistoryColorWidget.hpp"
#include "ui_WkfPlatformHistoryStatePrefWidget.h"

namespace wkf
{
struct TraceLinesPrefData;

class WKF_COMMON_EXPORT PlatformHistoryStateColorWidget : public PlatformHistoryColorWidget
{
public:
   PlatformHistoryStateColorWidget();
   ~PlatformHistoryStateColorWidget() override = default;
   void ReadPreferenceData(const TraceLinesPrefData& aPrefData) override;
   void WritePreferenceData(TraceLinesPrefData& aPrefData) override;

private:
   Ui::WkfPlatformHistoryStatePrefWidget mUi;
};
} // namespace wkf
#endif // WkfPlatformHistoryStatePrefWidget_HPP
