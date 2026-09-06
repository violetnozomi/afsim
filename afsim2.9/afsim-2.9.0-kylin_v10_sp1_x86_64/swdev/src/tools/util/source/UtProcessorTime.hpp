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

#ifndef UTPROCESSORTIME_HPP
#define UTPROCESSORTIME_HPP

#include "ut_export.h"

/** Processor time functions.

   This class provides methods to determine the amount of processor time that has elapsed.
*/

class UT_EXPORT UtProcessorTime
{
public:
   UtProcessorTime();
   virtual ~UtProcessorTime() = default;

   /** Return the elapsed processor time (user + system) since object creation or the last call to ResetTime().

       The return value is in seconds.
   */

   double GetTime();

   /** Same as GetTime() except user and system time are returned as separate components

       The return values are in seconds.
   */

   void GetTime(double& aUser, double& aSystem);

   /** Same as GetTime() except user, system, child user and child system times are returned as separate values.

       The return values are in seconds.
   */

   void GetTime(double& aUser, double& aSystem, double& aChildUser, double& aChildSystem);

   /** Return the raw processor time (in seconds) relative to some system-dependent reference.

       The return values is in seconds.
   */

   void GetRawTime(double& aUser, double& aSystem, double& aChildUser, double& aChildSystem) const;

   /** Reset the base time of the object.

       Subsequent calls to GetTime() will return the number of seconds
       that have elapsed since this was called.
   */

   void ResetTime();

private:
   /** The raw values at object creation or the last call to ResetTime(). */

   double mUserBase;
   double mSystemBase;
   double mChildUserBase;
   double mChildSystemBase;
};

#endif
