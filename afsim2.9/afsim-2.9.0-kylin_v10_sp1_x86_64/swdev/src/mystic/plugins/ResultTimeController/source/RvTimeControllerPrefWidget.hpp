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
#ifndef RVTIMECONTROLLERPREFWIDGET_HPP
#define RVTIMECONTROLLERPREFWIDGET_HPP

#include "RvTimeControllerPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_PrefWidget.h"

namespace RvTimeController
{
class PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
public:
   PrefWidget();
   ~PrefWidget() override = default;

private:
   void ReadPreferenceData(const PrefData& aPrefData) override;
   void WritePreferenceData(PrefData& aPrefData) override;

   Ui::PrefWidgetUi mUi;
};
} // namespace RvTimeController
#endif // RVTIMECONTROLLERPREFWIDGET_HPP
