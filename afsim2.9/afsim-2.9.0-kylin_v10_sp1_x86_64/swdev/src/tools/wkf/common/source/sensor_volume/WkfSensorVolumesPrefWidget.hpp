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
#ifndef WkfSensorVolumesPrefWidget_HPP
#define WkfSensorVolumesPrefWidget_HPP

#include <set>

class QTreeWidgetItem;

#include "wkf_common_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfSensorVolumesPrefObject.hpp"
#include "ui_WkfSensorVolumesPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT SensorVolumesPrefWidget : public PrefWidgetT<SensorVolumesPrefObject>
{
public:
   enum EntryMethod
   {
      cCOMBOBOX = 0,
      cLINEEDIT = 1
   };
   SensorVolumesPrefWidget(EntryMethod aEntryMethod);
   ~SensorVolumesPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

   void AddSensorModeName(const std::string& aName);
   void AddWeaponModeName(const std::string& aName);

private:
   void AddSensorMode();
   void AddWeaponMode();
   void DeleteSensorMode();
   void DeleteWeaponMode();
   void SensorCellChanged(int aRow, int aColumn);
   void WeaponCellChanged(int aRow, int aColumn);

   enum ItemFunction
   {
      cCOLORCHANGE = 1,
      cMODEROOT    = 2
   };

   void ReadPreferenceData(const SensorVolumesPrefData& aPrefData) override;
   void WritePreferenceData(SensorVolumesPrefData& aPrefData) override;

   Ui::WkfSensorVolumesPrefWidget mUi;
   std::set<std::string>          mSensorModeNameSet;
   std::set<std::string>          mWeaponModeNameSet;
   EntryMethod                    mEntryMethod;
};
} // namespace wkf
#endif // WkfSensorVolumesPrefWidget_HPP
