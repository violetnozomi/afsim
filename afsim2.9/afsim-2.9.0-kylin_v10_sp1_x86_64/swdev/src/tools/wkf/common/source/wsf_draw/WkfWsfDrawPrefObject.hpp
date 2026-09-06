// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFWSFDRAWPREFOBJECT_HPP
#define WKFWSFDRAWPREFOBJECT_HPP

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct WsfDrawPrefData
{
   bool mExtendedDraw{false};
};

class WKF_COMMON_EXPORT WsfDrawPrefObject : public PrefObjectT<WsfDrawPrefData>
{
   Q_OBJECT
public:
   static constexpr const char* cNAME = "WsfDrawPreferences";

   WsfDrawPrefObject(QObject* parent /*= nullptr*/)
      : wkf::PrefObjectT<WsfDrawPrefData>(parent, cNAME)
   {
   }
   void SetExtendedBrowserEnabled(bool aState) { mCurrentPrefs.mExtendedDraw = aState; }

signals:
   void ExtendedBrowserChanged(bool aState) const;

private:
   void            Apply() override {}
   WsfDrawPrefData ReadSettings(QSettings& aSettings) const override;
   void            SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::WsfDrawPrefData)

#endif
