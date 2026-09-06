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
#include "WsfCyberEventResult.hpp"

#include "triggers/WsfCyberTrigger.hpp"

std::string wsf::event::CyberTriggerResult::FormatOutput(const std::string& aObserverType,
                                                         bool               aIsCSV) const // = false
{
   auto isPlatform        = (mTrigger.GetPlatform() != nullptr);
   auto triggerTypeString = (isPlatform) ? std::string("Platform_Trigger") : std::string("Global_Trigger");
   auto delim             = (aIsCSV) ? std::string(",") : std::string(" ");
   auto triggerName       = isPlatform ? mTrigger.GetName() : mTrigger.GetType();

   auto returnString = aObserverType + delim + triggerTypeString + delim + triggerName;
   if (isPlatform)
   {
      returnString += (delim + mTrigger.GetPlatform()->GetName());
   }
   else if (aIsCSV)
   {
      // Account for correct column placement when the trigger is global and does
      // not have a platform to output.
      returnString += delim;
   }

   return returnString;
}
