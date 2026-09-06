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

#ifndef WKFDEBUGPREFOBJECT_HPP
#define WKFDEBUGPREFOBJECT_HPP

#include "WkfPrefObject.hpp"

namespace wkf
{
struct DebugPrefData
{
   bool mDeveloperMenu{false};
};

class WKF_EXPORT DebugPrefObject : public PrefObjectT<DebugPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "DeveloperOptions";

   DebugPrefObject(QObject* parent = nullptr);
   ~DebugPrefObject() override = default;

   bool IsSharedPreference() const final { return true; }

   void Apply() override;

   DebugPrefData ReadSettings(QSettings& aSettings) const override;
   void          SaveSettingsP(QSettings& aSettings) const override;
   bool          DeveloperMenu() const { return mCurrentPrefs.mDeveloperMenu; }

signals:
   void DeveloperMenuVisibilityChanged(bool aState);
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::DebugPrefData)
#endif
