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

#ifndef WKFPLATFORMDATAPREFOBJECT_HPP
#define WKFPLATFORMDATAPREFOBJECT_HPP

#include <set>

#include "WkfPrefObject.hpp"

namespace wkf
{
struct PlatformDataPrefData
{
   std::map<QString, bool> mStateList;
};

class PlatformDataPrefObject : public PrefObjectT<PlatformDataPrefData>
{
   Q_OBJECT
public:
   static constexpr const char* cNAME = "PlatformDataPreferences";

   PlatformDataPrefObject(QObject* parent /*= nullptr*/);

   bool HasPreferences(const QString& aItemName) const;
   bool IsVisible(const QString& aItemName) const;
   void SetHidden(const QString& aItemName);
   void ClearHidden();

private:
   void                 Apply() override;
   PlatformDataPrefData ReadSettings(QSettings& aSettings) const override;
   void                 SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::PlatformDataPrefData)
#endif
