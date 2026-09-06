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

#include "WkfAirCombatVisualizationPrefObject.hpp"

wkf::AirCombatVisualizationPrefObject::AirCombatVisualizationPrefObject(QObject* aParent)
   : PrefObjectT<AirCombatVisualizationPrefData>(aParent, "AirCombatVisualizationOptions")
{
}

void wkf::AirCombatVisualizationPrefObject::Apply()
{
   if (mUnsavedPrefData.mFirstMeter != mCurrentPrefs.mFirstMeter)
   {
      mUnsavedPrefData.mFirstMeter = mCurrentPrefs.mFirstMeter;
      emit FirstMeterChanged(mUnsavedPrefData.mFirstMeter);
   }

   if (mUnsavedPrefData.mSecondMeter != mCurrentPrefs.mSecondMeter)
   {
      mUnsavedPrefData.mSecondMeter = mCurrentPrefs.mSecondMeter;
      emit SecondMeterChanged(mUnsavedPrefData.mSecondMeter);
   }

   if (mUnsavedPrefData.mThirdMeter != mCurrentPrefs.mThirdMeter)
   {
      mUnsavedPrefData.mThirdMeter = mCurrentPrefs.mThirdMeter;
      emit ThirdMeterChanged(mUnsavedPrefData.mThirdMeter);
   }

   if (mUnsavedPrefData.mShowStateData != mCurrentPrefs.mShowStateData)
   {
      mUnsavedPrefData.mShowStateData = mCurrentPrefs.mShowStateData;
      emit ShowStateDataChanged(mUnsavedPrefData.mShowStateData);
   }

   if (mUnsavedPrefData.mShowWeaponsAccent != mCurrentPrefs.mShowWeaponsAccent)
   {
      mUnsavedPrefData.mShowWeaponsAccent = mCurrentPrefs.mShowWeaponsAccent;
      emit ShowWeaponsAccentChanged(mUnsavedPrefData.mShowWeaponsAccent);
   }

   if (mUnsavedPrefData.mShowFuelAccent != mCurrentPrefs.mShowFuelAccent)
   {
      mUnsavedPrefData.mShowFuelAccent = mCurrentPrefs.mShowFuelAccent;
      emit ShowFuelAccentChanged(mUnsavedPrefData.mShowFuelAccent);
   }

   if (mUnsavedPrefData.mShowTransmissionAccent != mCurrentPrefs.mShowTransmissionAccent)
   {
      mUnsavedPrefData.mShowTransmissionAccent = mCurrentPrefs.mShowTransmissionAccent;
      emit ShowTransmissionAccentChanged(mUnsavedPrefData.mShowTransmissionAccent);
   }

   if (mUnsavedPrefData.mShowSignatureAccent != mCurrentPrefs.mShowSignatureAccent)
   {
      mUnsavedPrefData.mShowSignatureAccent = mCurrentPrefs.mShowSignatureAccent;
      emit ShowSignatureAccentChanged(mUnsavedPrefData.mShowSignatureAccent);
   }

   if (mUnsavedPrefData.mShowVisualization != mCurrentPrefs.mShowVisualization)
   {
      mUnsavedPrefData.mShowVisualization = mCurrentPrefs.mShowVisualization;
      emit ShowVisualizationChanged(mUnsavedPrefData.mShowVisualization);
   }
}

wkf::AirCombatVisualizationPrefData wkf::AirCombatVisualizationPrefObject::ReadSettings(QSettings& aSettings) const
{
   AirCombatVisualizationPrefData pData;
   if (aSettings.contains("firstMeter"))
   {
      pData.mFirstMeter = (aSettings.value("firstMeter").toInt());
   }

   if (aSettings.contains("secondMeter"))
   {
      pData.mSecondMeter = (aSettings.value("secondMeter").toInt());
   }

   if (aSettings.contains("thirdMeter"))
   {
      pData.mThirdMeter = (aSettings.value("thirdMeter").toInt());
   }

   if (aSettings.contains("showStateData"))
   {
      pData.mShowStateData = (aSettings.value("showStateData").toBool());
   }

   if (aSettings.contains("showWeaponsAccent"))
   {
      pData.mShowWeaponsAccent = aSettings.value("showWeaponsAccent").toBool();
   }

   if (aSettings.contains("showFuelAccent"))
   {
      pData.mShowFuelAccent = aSettings.value("showFuelAccent").toBool();
   }

   if (aSettings.contains("showTransmissionAccent"))
   {
      pData.mShowTransmissionAccent = aSettings.value("showTransmissionAccent").toBool();
   }

   if (aSettings.contains("showSignatureAccent"))
   {
      pData.mShowSignatureAccent = aSettings.value("showSignatureAccent").toBool();
   }

   if (aSettings.contains("showVisualization"))
   {
      pData.mShowVisualization = aSettings.value("showVisualization").toBool();
   }

   return pData;
}

void wkf::AirCombatVisualizationPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("firstMeter", mCurrentPrefs.mFirstMeter);
   aSettings.setValue("secondMeter", mCurrentPrefs.mSecondMeter);
   aSettings.setValue("thirdMeter", mCurrentPrefs.mThirdMeter);
   aSettings.setValue("showStateData", mCurrentPrefs.mShowStateData);
   aSettings.setValue("showWeaponsAccent", mCurrentPrefs.mShowWeaponsAccent);
   aSettings.setValue("showFuelAccent", mCurrentPrefs.mShowFuelAccent);
   aSettings.setValue("showTransmissionAccent", mCurrentPrefs.mShowTransmissionAccent);
   aSettings.setValue("showSignatureAccent", mCurrentPrefs.mShowSignatureAccent);
   aSettings.setValue("showVisualization", mCurrentPrefs.mShowVisualization);
}
