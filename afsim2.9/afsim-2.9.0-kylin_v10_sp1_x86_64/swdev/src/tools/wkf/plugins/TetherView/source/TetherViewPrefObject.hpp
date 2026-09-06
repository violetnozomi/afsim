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

#ifndef TetherViewPrefObject_hpp
#define TetherViewPrefObject_hpp

class QSettings;
#include "TetherViewDockWidget.hpp"
#include "WkfPrefObject.hpp"

namespace TetherView
{
struct PrefData
{
   bool mLockCameraToHeadingEnabled{true};
   bool mEnableLighting{false};
};

class PrefObject : public wkf::PrefObjectT<PrefData>
{
   Q_OBJECT

public:
   explicit PrefObject(QObject* parent = nullptr);

   bool GetHeadingLockEnabled() const { return mCurrentPrefs.mLockCameraToHeadingEnabled; }
   void SetHeadingLockEnabled(bool aEnabled) { mCurrentPrefs.mLockCameraToHeadingEnabled = aEnabled; }
   bool GetLightingEnabled() const { return mCurrentPrefs.mEnableLighting; }
   void SetLightingEnabled(bool aEnabled) { mCurrentPrefs.mEnableLighting = aEnabled; }

signals:
   void HeadingLockEnabledChanged(bool aEnabled);
   void LightingEnabledChanged(bool aEnabled);

private:
   void Apply() override;

   PrefData ReadSettings(QSettings& aSettings) const override;
   void     SaveSettingsP(QSettings& aSettings) const override;
   void     SetPreferenceDataP(const PrefData& aPrefData) override;
   bool     mLockCameraToHeadingChanged{false};
   bool     mLightingChanged{false};
};
} // namespace TetherView

Q_DECLARE_METATYPE(TetherView::PrefData)
#endif
