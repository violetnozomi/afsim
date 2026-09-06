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
#ifndef RVPLATFORMBROWSERPREFOBJECT_HPP
#define RVPLATFORMBROWSERPREFOBJECT_HPP

#include "WkfPrefObject.hpp"

namespace RvPlatformBrowser
{
struct PrefData
{
   bool mShowInactive{false};
};

class PrefObject : public wkf::PrefObjectT<PrefData>
{
   Q_OBJECT

   friend class PrefWidget;

public:
   PrefObject(QObject* parent = nullptr);

signals:
   void ShowInactiveChanged(const bool aShowInactive);

private:
   void     Apply() override;
   PrefData ReadSettings(QSettings& aSettings) const override;
   void     SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace RvPlatformBrowser

Q_DECLARE_METATYPE(RvPlatformBrowser::PrefData)
#endif
