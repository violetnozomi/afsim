// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MapHoverInfoPREFWIDGET_HPP
#define MapHoverInfoPREFWIDGET_HPP

#include <QObject>
#include <QStringList>

#include "MapHoverInfoPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_MapHoverInfoWidget.h"

namespace MapHoverInfo
{
class PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
public:
   PrefWidget();

   QString GetCategoryHint() const override { return "Map"; }

private:
   void ReadPreferenceData(const PrefData& aPrefData) override;
   void WritePreferenceData(PrefData& aPrefData) override;

   Ui::MapHoverInfoWidget mUi;
};
} // namespace MapHoverInfo
#endif
