// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfPlatformHistoryStateColorWidget.hpp"

#include "WkfPlatformHistoryPrefObject.hpp"

wkf::PlatformHistoryStateColorWidget::PlatformHistoryStateColorWidget()
   : PlatformHistoryColorWidget()
{
   mUi.setupUi(this);
}

void wkf::PlatformHistoryStateColorWidget::WritePreferenceData(TraceLinesPrefData& aPrefData)
{
   aPrefData.mTraceLineColorationList[0] = mUi.mDefaultColor->GetColor();
   aPrefData.mTraceLineColorationList[1] = mUi.mDetectedColor->GetColor();
   aPrefData.mTraceLineColorationList[2] = mUi.mTrackedColor->GetColor();
   aPrefData.mTraceLineColorationList[3] = mUi.mAttackedColor->GetColor();
   aPrefData.mTraceLineColorationList[4] = mUi.mKilledColor->GetColor();
}

void wkf::PlatformHistoryStateColorWidget::ReadPreferenceData(const TraceLinesPrefData& aPrefData)
{
   size_t s = aPrefData.mTraceLineColorationList.size();

   mUi.mDefaultColor->SetColor(Qt::magenta);
   mUi.mDetectedColor->SetColor(Qt::white);
   mUi.mTrackedColor->SetColor(Qt::yellow);
   mUi.mAttackedColor->SetColor(Qt::red);
   mUi.mKilledColor->SetColor(Qt::black);
   if (s > 0)
   {
      mUi.mDefaultColor->SetColor(aPrefData.mTraceLineColorationList[0]);
      if (s > 1)
      {
         mUi.mDetectedColor->SetColor(aPrefData.mTraceLineColorationList[1]);
         if (s > 2)
         {
            mUi.mTrackedColor->SetColor(aPrefData.mTraceLineColorationList[2]);
            if (s > 3)
            {
               mUi.mAttackedColor->SetColor(aPrefData.mTraceLineColorationList[3]);
               if (s > 4)
               {
                  mUi.mKilledColor->SetColor(aPrefData.mTraceLineColorationList[4]);
               }
            }
         }
      }
   }
}
