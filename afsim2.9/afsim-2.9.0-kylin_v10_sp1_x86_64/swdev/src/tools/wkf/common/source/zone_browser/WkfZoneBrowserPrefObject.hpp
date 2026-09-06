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

#ifndef WkPrefObjectZoneBrowser_HPP
#define WkPrefObjectZoneBrowser_HPP

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
// enum used because this option may be expanded in the future
enum ColorOption
{
   cSINGLESELECT,
   cRANDOM
};

struct ZoneBrowserPrefData
{
   ColorOption mColorChoice{cSINGLESELECT};
   QColor      mDefaultLineColor{0, 255, 0};
   QColor      mDefaultFillColor{255, 255, 0, 63};
};

class WKF_COMMON_EXPORT ZoneBrowserPrefObject : public wkf::PrefObjectT<ZoneBrowserPrefData>
{
   Q_OBJECT
public:
   static constexpr const char* cNAME = "ZoneBrowserOptions";

   ZoneBrowserPrefObject(QObject* aParentPtr = nullptr);

   ColorOption GetColorChoice() const { return mCurrentPrefs.mColorChoice; }
   QColor      GetDefaultLineColor() const { return mCurrentPrefs.mDefaultLineColor; }
   QColor      GetDefaultFillColor() const { return mCurrentPrefs.mDefaultFillColor; }

   bool IsSharedPreference() const override { return true; }

signals:
   void ColorChoiceChanged(ColorOption aColorChoice);
   void DefaultLineColorChanged(const QColor& aLineColor);
   void DefaultFillColorChanged(const QColor& aFillColor);

private:
   void                Apply() override;
   void                SetPreferenceDataP(const ZoneBrowserPrefData& aPrefData) override;
   ZoneBrowserPrefData ReadSettings(QSettings& aSettings) const override;
   void                SaveSettingsP(QSettings& aSettings) const override;

   bool mColorChoiceChanged;
   bool mLineColorChanged;
   bool mFillColorChanged;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::ColorOption);
Q_DECLARE_METATYPE(wkf::ZoneBrowserPrefData)

#endif
