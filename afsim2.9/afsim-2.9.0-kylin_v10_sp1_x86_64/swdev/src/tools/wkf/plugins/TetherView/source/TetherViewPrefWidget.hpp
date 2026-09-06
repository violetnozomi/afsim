// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef TetherViewPrefWidget_hpp
#define TetherViewPrefWidget_hpp

#include "TetherViewPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_TetherViewPrefWidget.h"

namespace TetherView
{
class PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
public:
   PrefWidget();
   ~PrefWidget() override = default;

private:
   void ReadPreferenceData(const PrefData& aPrefData) override;
   void WritePreferenceData(PrefData& aPrefData) override;

   Ui::TetherViewPrefWidget mUi;
};
} // namespace TetherView

#endif
