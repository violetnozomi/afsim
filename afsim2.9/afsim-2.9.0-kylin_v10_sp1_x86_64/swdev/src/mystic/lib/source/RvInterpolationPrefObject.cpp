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
#include "RvInterpolationPrefObject.hpp"

rv::InterpolationPrefObject::InterpolationPrefObject(QObject* parent)
   : wkf::PrefObjectT<InterpolationPrefData>(parent, cNAME)
{
}

rv::InterpolationPrefData rv::InterpolationPrefObject::ReadSettings(QSettings& aSettings) const
{
   InterpolationPrefData pData;
   pData.mInterpolateAngles   = aSettings.value("interpolateAngle", mDefaultPrefs.mInterpolateAngles).toBool();
   pData.mExtrapolatePosition = aSettings.value("extrapolatePosition", mDefaultPrefs.mExtrapolatePosition).toBool();
   return pData;
}

void rv::InterpolationPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("interpolateAngle", mCurrentPrefs.mInterpolateAngles);
   aSettings.setValue("extrapolatePosition", mCurrentPrefs.mExtrapolatePosition);
}
