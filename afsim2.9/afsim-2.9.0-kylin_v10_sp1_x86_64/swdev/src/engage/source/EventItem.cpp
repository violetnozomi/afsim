// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "EventItem.hpp"

//#include <cstdio>
//#include <iostream>

#include "UtInput.hpp"

//#include "Events.hpp"
//#include "Simulation.hpp"

// TODO - NEED to decide if we are going to use C++ or C style user formatting directives.

namespace engage
{

// =================================================================================================
EventItem::EventItem()
   : /*mEventType(TODO),*/
   mEnabled(false)
{
}

// =================================================================================================
bool EventItem::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "enable")
   {
      aInput.ReadValue(mEventType);
      mEnabled = true;
   }
   else if (command == "disable")
   {
      aInput.ReadValue(mEventType);
      mEnabled = false;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//// =================================================================================================
// void EventItem::Write(std::ostream& aStream,
//                       Simulation&   aSimulation) const
//{
//    //switch (mEventType)
//    //{
//    //   case TODO:
//    //      {
//    //         double r = Events::GetEvent(mEventIndex, aSimulation, 0) * mScaleFactor;
//    //         if (mFormat.empty())
//    //         {
//    //            aStream << ' ' << r;
//    //         }
//    //         else
//    //         {
//    //            char obuf[32];
//    //            sprintf(obuf, mFormat.c_str(), r);
//    //            aStream << obuf;
//    //         }
//    //      }
//    //      break;
//
//    //   default:
//    //      break;
//    //}
// }

} // namespace engage
