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
#ifndef WkfPlatformHistoryBehaviorPrefWidget_HPP
#define WkfPlatformHistoryBehaviorPrefWidget_HPP

#include <map>
#include <string>

#include <QColor>

#include "platform_history/WkfPlatformHistoryColorWidget.hpp"
#include "ui_RvPlatformHistoryBehaviorPrefWidget.h"

namespace RvPlatformHistory
{
class PlatformHistoryBehaviorColorWidget : public wkf::PlatformHistoryColorWidget
{
public:
   PlatformHistoryBehaviorColorWidget();
   ~PlatformHistoryBehaviorColorWidget() override = default;

   void SetBehaviorColors(const std::map<std::string, QColor>& aBehaviorColors);

   void ReadPreferenceData(const wkf::TraceLinesPrefData& aPrefData) override;
   void WritePreferenceData(wkf::TraceLinesPrefData& aPrefData) override;

private slots:
   void BehaviorColorAdded();
   void BehaviorColorRemoved();

private:
   Ui::RvPlatformHistoryBehaviorPrefWidget mUi;
};
} // namespace RvPlatformHistory
#endif // WkfPlatformHistoryBehaviorPrefWidget_HPP
