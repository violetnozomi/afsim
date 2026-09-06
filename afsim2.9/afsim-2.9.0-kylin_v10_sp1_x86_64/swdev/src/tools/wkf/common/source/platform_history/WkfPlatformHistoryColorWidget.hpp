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
#ifndef WkfPlatformHistoryTraceLinePrefWidget_HPP
#define WkfPlatformHistoryTraceLinePrefWidget_HPP

#include "wkf_common_export.h"

#include <QWidget>

namespace wkf
{
struct TraceLinesPrefData;

class WKF_COMMON_EXPORT PlatformHistoryColorWidget : public QWidget
{
public:
   PlatformHistoryColorWidget();
   ~PlatformHistoryColorWidget() = default;
   virtual void ReadPreferenceData(const TraceLinesPrefData& aPrefData) {}
   virtual void WritePreferenceData(TraceLinesPrefData& aPrefData) {}
};
} // namespace wkf
#endif // WkfPlatformHistoryTraceLinePrefWidget_HPP
