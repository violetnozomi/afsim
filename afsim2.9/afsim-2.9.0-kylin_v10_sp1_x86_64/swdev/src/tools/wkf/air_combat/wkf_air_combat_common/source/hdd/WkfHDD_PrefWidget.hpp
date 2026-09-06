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

#ifndef WKFHDD_PREFWIDGET_HPP
#define WKFHDD_PREFWIDGET_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_PrefObject.hpp"
#include "WkfPrefWidget.hpp"

class QComboBox;

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
public:
   PrefWidget();
   ~PrefWidget() override = default;

   QString GetCategoryHint() const override { return "Air Combat"; }

private:
   /** Read preference data.
    * @param aPrefData pref data to read from.
    */
   void ReadPreferenceData(const PrefData& aPrefData) override;

   /** Write preference data.
    * @param aPrefData pref data to write to.
    */
   void WritePreferenceData(PrefData& aPrefData) override;

   QComboBox*                   mResolution; ///< Resolution of HDD windows.
   std::map<size_t, QComboBox*> mPages;      ///< Map of page combo boxes.
};
} // namespace HDD
#endif // WKFHDD_PREFWIDGET_HPP
