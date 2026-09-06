// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFSATELLITETETHERPREFOBJECT_HPP
#define WKFSATELLITETETHERPREFOBJECT_HPP

class QSettings;
#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct SatelliteTetherPrefData
{
   bool mShowFutureTrack{true};
   bool mShowNames{true};
   bool mEnableLighting{false};
};

class WKF_COMMON_EXPORT SatelliteTetherPrefObject : public PrefObjectT<SatelliteTetherPrefData>
{
   Q_OBJECT
public:
   static constexpr const char* cNAME = "SatelliteTetherPreferences";
   explicit SatelliteTetherPrefObject(QObject* parent = nullptr);

   bool GetShowFutureTrack() const { return mCurrentPrefs.mShowFutureTrack; }
   void SetShowFutureTrack(bool aShow);

   bool GetShowNames() const { return mCurrentPrefs.mShowNames; }
   void SetShowNames(bool aShow);

   bool GetLightingEnabled() const { return mCurrentPrefs.mEnableLighting; }
   void SetLightingEnabled(bool aEnabled);
signals:
   void ShowFutureTrackChanged(bool aShow);
   void ShowNamesChanged(bool aShow);
   void LightingChanged(bool aState);

private:
   void EmitAllSignals();

   void                    Apply() override;
   SatelliteTetherPrefData ReadSettings(QSettings& aSettings) const override;
   void                    SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::SatelliteTetherPrefData)
#endif
