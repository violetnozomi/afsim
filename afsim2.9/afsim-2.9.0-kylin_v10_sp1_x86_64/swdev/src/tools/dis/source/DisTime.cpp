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

#include "DisTime.hpp"

#include "DisValidationUtils.hpp"
#include "UtCast.hpp"

#if defined(_WIN32)
#include <ctime>

#include <windows.h>

#include <sys/timeb.h>
#include <winbase.h>
#else
#include <sys/time.h> // gettimeofday
#include <sys/times.h>
#if defined __linux__
#include <ctime>
#endif

#endif

//#include <iostream>   Only for debugging

const double DisTime::Site::sSecPerTick = 3600.0 / 2147483648.0;
const float  DisTime::Site::sRamp[5]    = {0.2F, 0.4F, 0.6F, 0.8F, 1.0F};

const double DisTime::sOneOver3600          = 1.0 / 3600.0;
double       DisTime::sHour                 = 0.0;
double       DisTime::sTimeAfterHourPast    = -1.0;
double       DisTime::sDelta                = 0.0;
unsigned int DisTime::sInitial              = 1;
bool         DisTime::sUseAbsoluteTimestamp = false;

pt2_IRIG_TimeFn DisTime::mIRIG_TimeFnPtr = nullptr;

DisTime::OwnerList DisTime::sOwnerList;
DisTime::SiteList  DisTime::sSiteList;


//  ****
//  **********  Public  ********
//  ****

DisTime::~DisTime() {}


// A DIS time stamp is only capable of representing the time within
// the 'current' hour.  In order to be able to represent times greater
// than one hour we have maintain some concept of 'current' time and
// also keep track of rollovers in the DIS time stamps.  For 'current'
// time we use the time last derived by this method.  We add the
// time stamp and possible rollover to the hour portion of the
// 'current' time to get a new 'current' time.
//
// We don't assume time stamps increase monotonically, but we do assume
// that adjacent messages are 'resonably' close to each other.
// Delta's outside of +/- 1800 seconds are assumed to trigger a
// rollover.
//


// Static
// Get the time for a time stamp accounting for wrapping around the hour
double DisTime::GetAbsoluteTime(DisUint32 aTimeStamp)
{
   double timeAfterHour = (aTimeStamp >> 1 & 0x7FFFFFFF) * Site::sSecPerTick;

   if (sTimeAfterHourPast >= 0.0)
   {
      double delta = timeAfterHour - sTimeAfterHourPast;

      if (delta < -1800.0)
      {
         sHour += 3600.0;
      }
      else if (delta > 1800.0)
      {
         sHour -= 3600.0;
      }
   }

   sTimeAfterHourPast = timeAfterHour;
   return timeAfterHour + sHour;
}

// Static
// Get the time for a time stamp accounting for wrapping around the hour
DisUint32 DisTime::GetAbsoluteTimestamp()
{
   long      seconds;
   double    microseconds;
   DisUint32 AbsoluteDisTimeStamp;
#if defined(_WIN32)
   if (mIRIG_TimeFnPtr) // Pointer to function for SIL precision time card
   {
      // Note this function is intended to return secondsAfterHour
      (*mIRIG_TimeFnPtr)(&seconds, &microseconds);
   }
   else
   {
      // Note: This is unchanged from way Windows has done this before SIL option
      time_t sec;
      time(&sec);
      seconds = static_cast<long>(sec);
      struct _timeb tstruct;
      _ftime(&tstruct);
      microseconds = tstruct.millitm * 1000.0;
   }
#else
   struct timeval  tv_timestamp;
   struct timezone tv_timezone;

   gettimeofday(&tv_timestamp, &tv_timezone);
   seconds      = tv_timestamp.tv_sec;
   microseconds = tv_timestamp.tv_usec;
#endif
   // calc # of seconds past the hour,
   // pack into upper 31 bits and set
   // lowest bit to 1
   double dblSecPastHour = seconds % 3600;
   dblSecPastHour += microseconds * 0.000001;
   AbsoluteDisTimeStamp = static_cast<DisUint32>(dblSecPastHour * 596523.2355555555555);
   AbsoluteDisTimeStamp = (AbsoluteDisTimeStamp << 1) | 1;

   return AbsoluteDisTimeStamp;
}

// Static
// Get a time that has been adjusted
// for latency in the network (if using relative time) and
// account for PDU timestamp wrapping through 3600 secs.
// "aDisEntityID" is the DisEntityId that best
// represents the owner of this PDU.  The Transfer Ownership
// PDU muddies the water for this function, but can be accounted
// for by calling "AddToAltOwnerList" and "RemoveFromAltOwnerList"
// when an entity is transfered to another owner and then removed from
// the active simulation.  The last argument controls whether the
// network latency deltas should be saved for future calls to
// this function.  Be sure to call "Reset()" when appropriate.
double DisTime::GetBestAvailableTime(DisUint32          aTimeStamp,
                                     const DisEntityId& aDisEntityId,
                                     double             aSimTime,
                                     unsigned int       aAdjustDelta)
{
   if (aTimeStamp & 1)
   {
      // Do absolute time stuff.  This may change some if we really
      // really get into absolute time or we assume the first packet
      // received may not be all that accurate.

      double time = GetAbsoluteTime(aTimeStamp);
      if (sInitial)
      {
         sInitial = 0;
         sDelta   = time - aSimTime;
      }

      return time - sDelta;
   }
   else
   {
      if (aDisEntityId.IsDefined())
      {
         // Do relative time stuff
         unsigned int siteKey;

         // First see if entity is really owned by some one else
         if (sOwnerList.empty())
         {
            // For a lot of sims this will be the most common path
            siteKey = static_cast<unsigned int>(aDisEntityId.GetSite()) << 16 |
                      static_cast<unsigned int>(aDisEntityId.GetApplication());
         }
         else
         {
            OwnerList::iterator iter(sOwnerList.find(aDisEntityId));
            if (iter == sOwnerList.end())
            {
               // Not on the list so use the orginal id
               siteKey = static_cast<unsigned int>(aDisEntityId.GetSite()) << 16 |
                         static_cast<unsigned int>(aDisEntityId.GetApplication());
            }
            else
            {
               // On list, so use the current new owner
               siteKey = (*iter).second;
            }
         }

         // Ok now we know what site to look for; go find it
         SiteList::iterator iter(sSiteList.find(siteKey));
         if (iter == sSiteList.end())
         {
            // Instantiate a new site and add to the table.
            // This should only happen a few times during a sim, so
            // we will pay the copy constructor penalty.
            std::pair<SiteList::iterator, bool> answer(
               sSiteList.insert(std::pair<const unsigned int, Site>(siteKey, Site())));
            iter = answer.first;
         }

         return (*iter).second.GetTime(aTimeStamp, aSimTime, aAdjustDelta);
      }
      else
      {
         // Don't know where this packet came from so just use
         // current time.
         return aSimTime;
      }
   }
}

// Static
// Set the time in a timestamp
void DisTime::SetTimeStamp(DisUint32& aTimeStamp, double aTime)
{
   // Time expressed as hours and fraction of hour
   double decHours = aTime * sOneOver3600;

   // Extract just the whole hours
   double intHours = static_cast<int>(decHours);

   // Extract the fraction of hour
   double fracHour = decHours - intHours;

   // Make a timestamp. DIS represents 1 hour as 2^31 - 1 (2147483647) 'ticks'
   // Use a 'relative' timestamp (LSB = 0) (absolute is LSB = 1).
   aTimeStamp = static_cast<DisUint32>(fracHour * 2147483647.0) << 1;
}

// Static
// Adjust a double time so that it has the same precision as a DIS
// timestamp.  This basically is a double->timestamp->double converter.
double DisTime::AdjustTime(double aTime)
{
   // Time expressed as hours and fraction of hour
   double decHours = aTime * sOneOver3600;

   // Extract just the whole hours
   double intHours = static_cast<int>(decHours);

   // Extract the fraction of hour
   double fracHour = decHours - intHours;

   // Make a timestamp. DIS represents 1 hour as 2^31 - 1 (2147483647) 'ticks'
   DisUint32 timestamp = static_cast<DisUint32>(fracHour * 2147483647.0);

   double timeAfterHour = timestamp * Site::sSecPerTick;

   // Convert the timestamp back to seconds and
   return (intHours * 3600.0) + timeAfterHour;
}

// Static
// Transfer ownership of an entity to an alternate entity.
// Call this method when a Transfer Ownership PDU is received
// so the "GetTime" methods return a correct time.
void DisTime::AddToAltOwnerList(const DisEntityId& aId, const DisEntityId& aNewOwner)
{
   // Remove from list just in case Entity got stuck on there earlier
   RemoveFromAltOwnerList(aId);

   unsigned int newOwner =
      static_cast<unsigned int>(aNewOwner.GetSite()) << 16 | static_cast<unsigned int>(aNewOwner.GetApplication());
   unsigned int origOwner =
      static_cast<unsigned int>(aId.GetSite()) << 16 | static_cast<unsigned int>(aId.GetApplication());

   if (newOwner != origOwner)
   {
      sOwnerList.insert(std::pair<const DisEntityId, unsigned int>(aId, newOwner));
   }
}

// Static
// Remove EntityIds from the alternate owner list which
// is used by the "GetTime" methods.  It is good to call
// this method when a entity times out or goes inactive.
void DisTime::RemoveFromAltOwnerList(const DisEntityId& aId)
{
   sOwnerList.erase(aId);
}

// Static
// Reset the internal variables.  This should be called whenever
// the simulation is 'reset' (i.e.: clock set to 0.0)
void DisTime::Reset()
{
   sHour              = 0.0;
   sTimeAfterHourPast = -1.0;
   sInitial           = 1;

   // Empty and destroy lists
   sOwnerList.clear();
   sSiteList.clear();
}


//  ****
//  **********  Private  ********
//  ****


DisTime::Site::Site()
   : mPassNum(-5)
   , mHour(0.0)
   , mTimeAfterHourPast(-1.0)
{
}


DisTime::Site::~Site() {}


double DisTime::Site::GetTime(DisUint32 aTimeStamp, double aSimTime, unsigned int aAdjustDelta)
{
   double timeAfterHour = (aTimeStamp >> 1 & 0x7FFFFFFF) * sSecPerTick;

   if (mTimeAfterHourPast >= 0.0)
   {
      double delta       = timeAfterHour - mTimeAfterHourPast;
      mTimeAfterHourPast = timeAfterHour;

      if (delta < -1800.0)
      {
         mHour += 3600.0;
      }
      else if (delta > 1800.0)
      {
         mHour -= 3600.0;
      }

      double disTime = timeAfterHour + mHour;

      if (aAdjustDelta)
      {
         if (++mPassNum >= 0)
         {
            if (mPassNum > 19)
            {
               mPassNum = -1;

               // Average delta over the last 20 updates for this site
               double tempy = 0.0;
               for (unsigned int j = 0; j < 20; ++j)
               {
                  tempy += mDeltaHold[j];
               }

               mDeltaBasePast = mDeltaBase;
               mDeltaBase     = tempy * 0.05;
            }
            else
            {
               mDeltaHold[mPassNum] = disTime - aSimTime;
            }
         }
      }

      // Compute index to ramp in drift
      unsigned int index;
      if (mPassNum > 4)
      {
         index = 4;
      }
      else if (mPassNum < 0)
      {
         index = 0;
      }
      else
      {
         index = ut::safe_cast<unsigned int, int>(mPassNum);
      }

      // Ramp in new delta.  The delta should stay fairly constant after sim has
      // been running for awhile
      delta = mDeltaBasePast + (mDeltaBase - mDeltaBasePast) * sRamp[index];

      return disTime - delta;
   }
   else
   {
      mTimeAfterHourPast = timeAfterHour;
      mDeltaBase         = timeAfterHour - aSimTime;
      mDeltaBasePast     = mDeltaBase;
      return timeAfterHour - mDeltaBase;
   }
}

bool DisTime::Site::IsValid() const
{
   bool isValid = true;

   if (!ValidateScalar(mHour))
      return false;
   if (!ValidateScalar(mTimeAfterHourPast))
      return false;
   if (!ValidateScalar(mDeltaBase))
      return false;
   if (!ValidateScalar(mDeltaBasePast))
      return false;

   return isValid;
}

bool DisTime::IsValid() const
{
   std::map<unsigned int, Site>::const_iterator siteIter;
   std::map<unsigned int, Site>::const_iterator endSiteIter = sSiteList.end();
   for (siteIter = sSiteList.begin(); siteIter != endSiteIter; ++siteIter)
   {
      if ((*siteIter).second.IsValid() == false)
         return false;
   }

   std::map<DisEntityId, unsigned int>::const_iterator idIter;
   std::map<DisEntityId, unsigned int>::const_iterator endIdIter = sOwnerList.end();
   for (idIter = sOwnerList.begin(); idIter != endIdIter; ++idIter)
   {
      if ((*idIter).first.IsValid() == false)
         return false;
   }

   return true;
}
