// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfInteractionPrefObject.hpp"

#include <QSettings>

#include "WkfUtils.hpp"


constexpr const char* wkf::InteractionPrefData::cDETECT;
constexpr const char* wkf::InteractionPrefData::cTRACK;
constexpr const char* wkf::InteractionPrefData::cLOCALTRACK;
constexpr const char* wkf::InteractionPrefData::cFIRE;
constexpr const char* wkf::InteractionPrefData::cKILL;
constexpr const char* wkf::InteractionPrefData::cJAM;
constexpr const char* wkf::InteractionPrefData::cMESSAGE;
constexpr const char* wkf::InteractionPrefData::cTASK;

wkf::InteractionPrefObject::InteractionPrefObject(QObject* parent)
   : PrefObjectT<InteractionPrefData>(parent, "Interactions")
{
}

void wkf::InteractionPrefObject::SetColor(const QString& aName, const QColor& aColor)
{
   mCurrentPrefs.mColors[aName] = aColor;
   emit ColorChanged(aName, aColor);
}

QColor wkf::InteractionPrefObject::GetColor(const QString& aType) const
{
   auto c = mCurrentPrefs.mColors.find(aType);

   if (c != mCurrentPrefs.mColors.end())
   {
      return c.value();
   }
   else
   {
      return wkf::util::GetHashColor(aType);
   }
}

void wkf::InteractionPrefObject::SetTimeout(const double& aTimeout)
{
   mCurrentPrefs.mTimeout = aTimeout;
   emit TimeoutChanged(aTimeout);
}

void wkf::InteractionPrefObject::SetStackingAllowed(const bool aAllow)
{
   mCurrentPrefs.mStackingAllowed = aAllow;
   emit StackingAllowedChanged(aAllow);
}

void wkf::InteractionPrefObject::SetTooltipMode(PrefDataType::TooltipMode aMode)
{
   mCurrentPrefs.mTooltipMode = aMode;
   emit TooltipModeChanged(aMode);
}

void wkf::InteractionPrefObject::SetLineWidth(unsigned int aWidth)
{
   mCurrentPrefs.mLineWidth = aWidth;
   emit LineWidthChanged(aWidth);
}

void wkf::InteractionPrefObject::SetArchSegments(bool aState)
{
   mCurrentPrefs.mArchSegments = aState;
   emit ArchSegmentsChanged(aState);
}


void wkf::InteractionPrefObject::EmitAllSignals()
{
   for (const auto& k : mCurrentPrefs.mColors.keys())
   {
      emit ColorChanged(k, mCurrentPrefs.mColors[k]);
   }
   emit TimeoutChanged(mCurrentPrefs.mTimeout);
   emit StackingAllowedChanged(mCurrentPrefs.mStackingAllowed);
   emit TooltipModeChanged(mCurrentPrefs.mTooltipMode);
   emit LineWidthChanged(mCurrentPrefs.mLineWidth);
   emit ArchSegmentsChanged(mCurrentPrefs.mArchSegments);
}

void wkf::InteractionPrefObject::Apply()
{
   EmitAllSignals();
}

wkf::InteractionPrefData wkf::InteractionPrefObject::ReadSettings(QSettings& aSettings) const
{
   InteractionPrefData pData;
   // look for default colors
   for (const auto& k : mDefaultPrefs.mColors.keys())
   {
      pData.mColors[k] = aSettings.value(k + "_color", mDefaultPrefs.mColors[k]).value<QColor>();
   }
   for (auto& pref : mRegisteredPrefs)
   {
      QString tag        = pref.first;
      pData.mColors[tag] = aSettings.value(tag + "_color", pref.second).value<QColor>();
   }

   pData.mTimeout         = aSettings.value("timeout", mDefaultPrefs.mTimeout).toDouble();
   pData.mStackingAllowed = aSettings.value("stackingAllowed", mDefaultPrefs.mStackingAllowed).toBool();
   pData.mTooltipMode  = (PrefDataType::TooltipMode)aSettings.value("tooltip_mode", mDefaultPrefs.mTooltipMode).toInt();
   pData.mLineWidth    = aSettings.value("linewidth", mDefaultPrefs.mLineWidth).toUInt();
   pData.mArchSegments = aSettings.value("arch", mDefaultPrefs.mArchSegments).toBool();
   return pData;
}

void wkf::InteractionPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   for (const auto& k : mCurrentPrefs.mColors.keys())
   {
      aSettings.setValue(k + "_color", mCurrentPrefs.mColors[k]);
   }
   aSettings.setValue("timeout", mCurrentPrefs.mTimeout);
   aSettings.setValue("stackingAllowed", mCurrentPrefs.mStackingAllowed);
   aSettings.setValue("tooltip_mode", mCurrentPrefs.mTooltipMode);
   aSettings.setValue("linewidth", mCurrentPrefs.mLineWidth);
   aSettings.setValue("arch", mCurrentPrefs.mArchSegments);
}

void wkf::InteractionPrefObject::RegisterInteraction(const QString& aInteraction, const QString& aGroup, const QColor& aColor)
{
   if (mRegisteredPrefs.find(aInteraction) == mRegisteredPrefs.end())
   {
      mRegisteredPrefs[aInteraction]      = aColor;
      mCurrentPrefs.mColors[aInteraction] = aColor;
      NewInteractionTypeRegistered(aInteraction, aGroup, aColor);
   }
}
