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
#ifndef RVINTERPOLATIONPREFOBJECT_HPP
#define RVINTERPOLATIONPREFOBJECT_HPP

#include "WkfPrefObject.hpp"

namespace rv
{
struct InterpolationPrefData
{
   bool mInterpolateAngles{true};
   bool mExtrapolatePosition{true};
};

class InterpolationPrefObject : public wkf::PrefObjectT<InterpolationPrefData>
{
public:
   static constexpr const char* cNAME = "InterpolationOptions";

   InterpolationPrefObject(QObject* parent = nullptr);
   ~InterpolationPrefObject() override = default;

   bool GetAngularInterpolation() const { return mCurrentPrefs.mInterpolateAngles; }
   bool GetPositionalExtrapolation() const { return mCurrentPrefs.mExtrapolatePosition; }

private:
   void Apply() override {}

   InterpolationPrefData ReadSettings(QSettings& aSettings) const override;
   void                  SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace rv

Q_DECLARE_METATYPE(rv::InterpolationPrefData)
#endif
