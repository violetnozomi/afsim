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

#ifndef DISTIME_HPP
#define DISTIME_HPP

#include "dis_export.h"

#include <map>

#include "DisEntityId.hpp"
#include "DisTypes.hpp"

typedef void (*pt2_IRIG_TimeFn)(long*, double*);

class DIS_EXPORT DisTime
{
public:
   ~DisTime();


   // **************
   // Static Time functions
   // **************

   // Get the seconds after the hour from a raw time stamp and
   // set "aAbsoluteFlag".
   static double GetSecsAfterHour(DisUint32 aTimeStamp, unsigned int& aAbsoluteFlag);

   // Get the time for a timestamp accounting for wrapping around the hour
   static double GetAbsoluteTime(DisUint32 aTimeStamp);

   // Returns a timestamp based on the number of seconds past the hour.
   static DisUint32 GetAbsoluteTimestamp();

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
   static double GetBestAvailableTime(DisUint32          aTimeStamp,
                                      const DisEntityId& aDisEntityId,
                                      double             aSimTime,
                                      unsigned int       aAdjustDelta = 1);

   // Set the time in a timestamp
   static void SetTimeStamp(DisUint32& aTimestamp, double aTime);

   // Returns a timestamp for a give time
   static DisUint32 GetTimeStamp(double aTime);

   // Adjust a double time so that it has the same precision as a DIS
   // timestamp.  This basically is a double->timestamp->double converter.
   static double AdjustTime(double aTime);


   // Transfer ownership of an entity to an alternate entity.
   // Call this method when a Transfer Ownership PDU is received
   // so the "GetTime" methods return a correct time.
   static void AddToAltOwnerList(const DisEntityId& aId, const DisEntityId& aNewOwner);

   // Remove EntityIds from the alternate owner list which
   // is used by the "GetTime" methods.  It is good to call
   // this method when a entity times out or goes inactive.
   static void RemoveFromAltOwnerList(const DisEntityId& aId);

   // Reset the internal variables.  This should be called whenever
   // the simulation is 'reset' (i.e.: clock set to 0.0)
   static void Reset();

   static bool UseAbsoluteTimestamp() { return sUseAbsoluteTimestamp; }
   static void SetUseAbsoluteTimestamp(bool aFlag) { sUseAbsoluteTimestamp = aFlag; }

   // Function and member for integration of SIL precision time card.
   static pt2_IRIG_TimeFn mIRIG_TimeFnPtr;

   bool IsValid() const;

protected:
   // Hide the constructors
   DisTime();
   DisTime(const DisTime&);

private:
   class Site
   {
   public:
      Site();
      ~Site();

      double GetTime(DisUint32 aTimestamp, double aSimTime, unsigned int aAdjustDelta);

      static const double sSecPerTick;

      bool IsValid() const;

   private:
      static const float sRamp[5];
      int                mPassNum;
      double             mDeltaHold[20];
      double             mHour;
      double             mTimeAfterHourPast;
      double             mDeltaBase;
      double             mDeltaBasePast;
   };

   typedef std::map<DisEntityId, unsigned int> OwnerList;
   typedef std::map<unsigned int, Site>        SiteList;

   static OwnerList sOwnerList;
   static SiteList  sSiteList;

   // 1 / 3600
   static const double sOneOver3600;

   // Current hour in seconds
   static double sHour;

   // Past value of time past hour
   static double sTimeAfterHourPast;

   // Delta between absolute time a local computer time
   static double sDelta;

   // Initial Flag
   static unsigned int sInitial;

   // Absolute timestamp chosen
   static bool sUseAbsoluteTimestamp;
};


// Static
inline double DisTime::GetSecsAfterHour(DisUint32 aTimestamp, unsigned int& aAbsoluteFlag)
{
   aAbsoluteFlag = static_cast<unsigned int>(aTimestamp & 1);
   return (aTimestamp >> 1 & 0x7FFFFFFF) * Site::sSecPerTick;
}

// Static
inline DisUint32 DisTime::GetTimeStamp(double aTime)
{
   DisUint32 timeStamp;
   SetTimeStamp(timeStamp, aTime);
   return timeStamp;
}


#endif
