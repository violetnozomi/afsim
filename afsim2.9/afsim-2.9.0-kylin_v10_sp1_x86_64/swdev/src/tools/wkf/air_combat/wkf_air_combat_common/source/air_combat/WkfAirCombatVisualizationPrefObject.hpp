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

#ifndef WKFAIRCOMBATVISUALIZATIONPREFOBJECT_HPP
#define WKFAIRCOMBATVISUALIZATIONPREFOBJECT_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfPlugin.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
struct AirCombatVisualizationPrefData
{
   int  mFirstMeter             = 1;
   int  mSecondMeter            = 2;
   int  mThirdMeter             = 3;
   bool mShowStateData          = true;
   bool mShowWeaponsAccent      = true;
   bool mShowFuelAccent         = true;
   bool mShowTransmissionAccent = true;
   bool mShowSignatureAccent    = true;
   bool mShowVisualization      = false;
};

class WKF_AIR_COMBAT_COMMON_EXPORT AirCombatVisualizationPrefObject :
   public wkf::PrefObjectT<AirCombatVisualizationPrefData>
{
   Q_OBJECT

public:
   AirCombatVisualizationPrefObject(QObject* aParent = nullptr);

   const bool GetShowVisualization() { return mCurrentPrefs.mShowVisualization; }

signals:
   void FirstMeterChanged(int aMeasurement);
   void SecondMeterChanged(int aMeasurement);
   void ThirdMeterChanged(int aMeasurement);
   void ShowStateDataChanged(bool aShow);
   void ShowWeaponsAccentChanged(bool aShow);
   void ShowFuelAccentChanged(bool aShow);
   void ShowTransmissionAccentChanged(bool aShow);
   void ShowSignatureAccentChanged(bool aShow);
   void ShowVisualizationChanged(bool aShow);

private:
   void Apply() override;

   AirCombatVisualizationPrefData ReadSettings(QSettings& aSettings) const override;
   void                           SaveSettingsP(QSettings& aSettings) const override;

   AirCombatVisualizationPrefData mUnsavedPrefData;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::AirCombatVisualizationPrefData)

#endif // !WKFAirCombatVISUALIZATIONPREFOBJECT_HPP
