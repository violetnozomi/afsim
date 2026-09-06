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
#ifndef WKFMilStdPREFWIDGET_HPP
#define WKFMilStdPREFWIDGET_HPP

#include "wkf_export.h"

#include "WkfMilStdIconPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfMilStdIconPrefWidget.h"

namespace wkf
{
class WKF_EXPORT MilStdIconPrefWidget : public PrefWidgetT<MilStdIconPrefObject>
{
public:
   MilStdIconPrefWidget(QObject* parent = nullptr);

private:
   void ReadPreferenceData(const MilStdIconPrefData& aPrefData) override;
   void WritePreferenceData(MilStdIconPrefData& aPrefData) override;

   void UpdateTeamBox();
   void AddTeamToSection();
   void RemoveTeamFromSection();

   Ui::WkfMilStdIconPrefWidget mUi;

   std::map<QString, QTreeWidgetItem*> mTeamSections;
};
} // namespace wkf

#endif
