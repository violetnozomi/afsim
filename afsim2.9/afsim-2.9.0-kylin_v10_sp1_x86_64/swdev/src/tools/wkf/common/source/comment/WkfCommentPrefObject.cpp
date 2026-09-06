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

#include "WkfCommentPrefObject.hpp"

wkf::CommentPrefObject::CommentPrefObject(QObject* aParent /* = nullptr */)
   : wkf::PrefObjectT<CommentPrefData>(aParent, "CommentOptions")
   , mEnabledChanged(false)
   , mShowTimestampChanged(false)
   , mTimeoutChanged(false)
{
}

void wkf::CommentPrefObject::Apply()
{
   if (mEnabledChanged)
   {
      emit EnabledChanged(mCurrentPrefs.mEnabled);
      mEnabledChanged = false;
   }
   if (mShowTimestampChanged)
   {
      emit ShowTimestampChanged(mCurrentPrefs.mShowTimestamp);
      mShowTimestampChanged = false;
   }
   if (mTimeoutChanged)
   {
      emit TimeoutChanged(mCurrentPrefs.mTimeoutSec);
      mTimeoutChanged = false;
   }
}

void wkf::CommentPrefObject::SetPreferenceDataP(const CommentPrefData& aPrefData)
{
   mEnabledChanged       = aPrefData.mEnabled != mCurrentPrefs.mEnabled;
   mShowTimestampChanged = aPrefData.mShowTimestamp != mCurrentPrefs.mShowTimestamp;
   mTimeoutChanged       = aPrefData.mTimeoutSec != mCurrentPrefs.mTimeoutSec;

   BasePrefObject::SetPreferenceDataP(aPrefData);
}

wkf::CommentPrefData wkf::CommentPrefObject::ReadSettings(QSettings& aSettings) const
{
   CommentPrefData pData;
   pData.mTimeoutSec    = aSettings.value("timeout", mDefaultPrefs.mTimeoutSec).toInt();
   pData.mEnabled       = aSettings.value("enabled", mDefaultPrefs.mEnabled).toBool();
   pData.mShowTimestamp = aSettings.value("show_timestamp", mDefaultPrefs.mShowTimestamp).toBool();
   return pData;
}

void wkf::CommentPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("timeout", mCurrentPrefs.mTimeoutSec);
   aSettings.setValue("enabled", mCurrentPrefs.mEnabled);
   aSettings.setValue("show_timestamp", mCurrentPrefs.mShowTimestamp);
}
