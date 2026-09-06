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

#include "GeneralTime.hpp"

#include "ProxyWatcher.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPM_Root.hpp"

PostProcessor::GeneralTime::GeneralTime()
   : ReportWidget()
   , mEndTime(0)
{
   mUI.setupUi(this);
   mUI.mBeginTime->SetValue(0);
   mUI.mEndTime->SetValue(0);
   mUI.mTimeInterval->SetValue(1); // TODO: make the default time interval more adaptive
}

void PostProcessor::GeneralTime::WriteData(std::ostream& aStream)
{
   // if line edit is invalid, it will be read as "0.000"
   UtTimeValue begin = mUI.mBeginTime->GetValue().GetAsUnit("seconds");
   UtTimeValue end = mUI.mEndTime->GetValue().GetAsUnit("seconds");
   UtTimeValue interval = mUI.mTimeInterval->GetValue().GetAsUnit("seconds");

   aStream << "start_time " << begin
           << "\nend_time " << end
           << "\ninterval " << interval
           << "\ntime_format "
           << (mUI.mCB_TimeH->isChecked() ? "h:" : "")
           << (mUI.mCB_TimeH->isChecked() ? "m:" : "")
           << 's'
           << ((mUI.mSB_TimeDecimal->value() > 0) ? ('.' + QString::number(mUI.mSB_TimeDecimal->value()).toStdString()) : "")
           << "\n";
}

void PostProcessor::GeneralTime::UpdateEndTime(const WsfProxy::Time& aEndTime)
{
   if (mEndTime != aEndTime)
   {
      mUI.mEndTime->SetValue(aEndTime);
      mEndTime = aEndTime;
   }
}

void PostProcessor::GeneralTime::RestoreDefaults()
{
   mUI.mEndTime->SetValue(mEndTime);
   mUI.mCB_TimeH->setChecked(false);
   mUI.mCB_TimeM->setChecked(false);
   mUI.mSB_TimeDecimal->setValue(5);
}
