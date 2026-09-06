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
#ifndef WKFTypeVisibilityPREFOBJECT_HPP
#define WKFTypeVisibilityPREFOBJECT_HPP

#include "wkf_common_export.h"

#include <QSet>

#include "WkfPrefObject.hpp"

namespace wkf
{
using TypeSet = QSet<QString>;

struct TypeVisibilityPrefData
{
   TypeSet mHiddenTypes{}; // Default is not to hide any types.
};

class WKF_COMMON_EXPORT TypeVisibilityPrefObject : public wkf::PrefObjectT<TypeVisibilityPrefData>
{
   Q_OBJECT

public:
   TypeVisibilityPrefObject(QObject* parent = nullptr);
   ~TypeVisibilityPrefObject() override;

   bool IsVisible(const std::string& aTypeName) const;
   void ExternalAddPreference(const QString& aTypeName, bool aVisible);

   static constexpr const char* cNAME = "TypeVisibilityPrefObject";

signals:
   void TypeVisibilityChanged();

private:
   void                   Apply() override;
   TypeVisibilityPrefData ReadSettings(QSettings& aSettings) const override;
   void                   SaveSettingsP(QSettings& aSettings) const override;

   void SetPreferenceDataP(const TypeVisibilityPrefData& aTypeVisibilityPrefData) override;

   bool   mPrefsChanged{false};
   size_t mVisibilityFilterId{0};
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::TypeVisibilityPrefData)
#endif
