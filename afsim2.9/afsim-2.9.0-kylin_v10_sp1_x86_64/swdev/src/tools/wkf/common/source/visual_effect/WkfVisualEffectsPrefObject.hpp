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

#ifndef WKFVISUALEFFECTSPREFERENCESOBJECT_HPP
#define WKFVISUALEFFECTSPREFERENCESOBJECT_HPP

#include "WkfPlugin.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
struct VisualEffectsPrefData
{
   bool   mShowTrailingEffects{true};
   bool   mShowExplosions{true};
   double mMapExplosionScalar{1.0};
   double mMapExplosionLifetime{2.0};
};

class VisualEffectsPrefObject : public wkf::PrefObjectT<VisualEffectsPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "VisualEffects";

   explicit VisualEffectsPrefObject(QObject* parent = nullptr);

signals:
   void ShowTrailingEffectsChanged(bool aShow);
   void ShowExplosionsChanged(bool aShow);
   void MapExplosionScaleChanged(double aScalar);
   void MapExplosionLifetimeChanged(double aLifetime);

private:
   void                  Apply() override;
   VisualEffectsPrefData ReadSettings(QSettings& aSettings) const override;
   void                  SaveSettingsP(QSettings& aSettings) const override;

   bool   mShowTrailingEffects;
   bool   mShowExplosions;
   double mMapExplosionScalar;
   double mMapExplosionLifetime;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::VisualEffectsPrefData)

#endif // !WKFVISUALEFFECTSPREFERENCESOBJECT_HPP
