// ****************************************************************************
// CUI
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

#include "dis/WsfDisFileDevice.hpp"

#include <ctime>
#if !defined(_WIN32)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "DisPdu.hpp"
#include "DisTime.hpp"
#include "GenFileO.hpp"
#include "UtMemory.hpp"
#include "WsfScenario.hpp"
#include "WsfSystemLog.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisFileDevice::WsfDisFileDevice(const std::string& aRecordFile, WsfDisInterface* aDisInterfacePtr)
   : WsfDisDevice(aDisInterfacePtr)
   , mGenIOPtr{ut::make_unique<GenFileO>(aRecordFile.c_str(), GenBuf::BigEndian)}
{
   if (mGenIOPtr->IsValid())
   {
      // Write a header to the file

      std::string hdrStr("CME Replay");
      hdrStr += " Date: ";
      time_t curTime = time(nullptr); // Current date/time
      hdrStr += std::string(ctime(&curTime));

      hdrStr += " User: ";
#if defined(_WIN32)
      hdrStr += "<unknown>";
#else
      struct passwd* pPwdEnt;

      pPwdEnt = getpwuid(getuid()); // get user name
      if (pPwdEnt != nullptr)
      {
         hdrStr += std::string(pPwdEnt->pw_name);
      }
      else
      {
         hdrStr += "<unknown>";
      }
#endif
      mGenIOPtr->Put(hdrStr.c_str());

      WsfDisUtil::GetScenario(aDisInterfacePtr).GetSystemLog().WriteOutputLogEntry("DIS", aRecordFile);
   }
   else
   {
      throw OpenError();
   }
   mDeviceType = cDEVICE_RECORD;
}

// virtual
void WsfDisFileDevice::PutPduP(double aSimTime, DisPdu& aPdu)
{
   aPdu.SetTime(aSimTime);
   mGenIOPtr->Put(aSimTime);
   aPdu.Put(*mGenIOPtr);
   mGenIOPtr->Send();
}
