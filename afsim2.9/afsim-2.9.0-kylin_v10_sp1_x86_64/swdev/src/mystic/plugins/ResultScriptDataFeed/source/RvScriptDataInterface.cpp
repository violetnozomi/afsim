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
#include "RvScriptDataInterface.hpp"

#include <set>

#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"

RvScriptDataFeed::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mIndex(SCENARIO)
   , mReset(true)
{
}

void RvScriptDataFeed::Interface::SetPlatformOfInterest(const std::string& aPlatformName)
{
   QMutexLocker locker(&mMutex);
   mReset = true;
   mName  = aPlatformName;
   mIndex = LOOKUP_REQUIRED;
}

void RvScriptDataFeed::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   if (mIndex == LOOKUP_REQUIRED)
   {
      mIndex = aData.FindPlatformIndex(mName);
   }

   if (mIndex > 0)
   {
      rv::ResultPlatform* platform = aData.FindPlatform(mIndex);
      double              simTime  = aData.GetSimTime();

      if (platform != nullptr)
      {
         platform->BeginRead();
         auto dataMap = platform->GetStringKeyedMap<rv::MsgScriptData>();
         if (dataMap != nullptr)
         {
            for (auto& valueArray : *dataMap)
            {
               auto msg = dynamic_cast<rv::MsgScriptData*>(valueArray.second.FindFirstBefore(simTime));
               if (msg != nullptr)
               {
                  mPlatformData[valueArray.first] = msg->value();
               }
            }
         }
         platform->EndRead();
      }
   }
}

const RvScriptDataFeed::Interface::PlatformData& RvScriptDataFeed::Interface::GetPlatformData() const
{
   QMutexLocker locker(&mMutex);
   return mPlatformData;
}

bool RvScriptDataFeed::Interface::CheckReset()
{
   bool retval = mReset;
   mReset      = false;
   return retval;
}
