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

#ifndef COMMENTPREFOBJECT_HPP
#define COMMENTPREFOBJECT_HPP

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct CommentPrefData
{
   int  mTimeoutSec{5};
   bool mEnabled{true};
   bool mShowTimestamp{false};
};

class WKF_COMMON_EXPORT CommentPrefObject : public wkf::PrefObjectT<CommentPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "CommentOptions";

   CommentPrefObject(QObject* aParent = nullptr);

   int  GetTimeoutSec() { return mCurrentPrefs.mTimeoutSec; }
   bool GetEnabled() { return mCurrentPrefs.mEnabled; }
   bool GetShowTimestamp() { return mCurrentPrefs.mShowTimestamp; }


signals:
   void EnabledChanged(bool aEnabled);
   void ShowTimestampChanged(bool aEnabled);
   void TimeoutChanged(bool aEnabled);

private:
   void Apply() override;
   void SetPreferenceDataP(const CommentPrefData& aPrefData) override;

   CommentPrefData ReadSettings(QSettings& aSettings) const override;
   void            SaveSettingsP(QSettings& aSettings) const override;

   bool mEnabledChanged;
   bool mShowTimestampChanged;
   bool mTimeoutChanged;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::CommentPrefData);
#endif
