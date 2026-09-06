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

#include "WsfPathConstraints.hpp"

#include <limits>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"

namespace
{
const double cPATH_CONSTRAINT_ACCEL_LIMIT = 6.0 * UtEarth::cACCEL_OF_GRAVITY;
}

const double WsfPathConstraints::cDEFAULT_MAX_FLIGHT_ANGLE = UtMath::cPI_OVER_2 * 0.95;
const double WsfPathConstraints::cDEFAULT_MAX_LINEAR_ACCEL = cPATH_CONSTRAINT_ACCEL_LIMIT;
const double WsfPathConstraints::cDEFAULT_MAX_RADIAL_ACCEL = cPATH_CONSTRAINT_ACCEL_LIMIT;
// Used as a substitute for std::numeric_limits<double>::max(), since it has less tendency to overflow.
const double WsfPathConstraints::cLARGE_DOUBLE = 1.0E50;

WsfPathConstraints::WsfPathConstraints()
   : mMaxSpeed(cLARGE_DOUBLE)
   , mMinSpeed(0.0)
   , mMaxLinearAccel(cDEFAULT_MAX_LINEAR_ACCEL)
   , mMaxClimbRate(cLARGE_DOUBLE)
   , mMaxFlightPathAngle(cDEFAULT_MAX_FLIGHT_ANGLE)
   , mMaxAltitude(cLARGE_DOUBLE)
   , mMinAltitude(-cLARGE_DOUBLE)
   , mTurnRateLimit(0.0)
   , mRollAngleLimit(0.0)
   , mBodyVertLimit(0.0)
   , mMaxRadialAccel(cDEFAULT_MAX_RADIAL_ACCEL)
   , mRollRateLimit(0.0)
   , mHeadingPursuitGain(5.0)
   , mMaxTurnRate(0.0)
   , mIsOnGround(false)
{
}

bool WsfPathConstraints::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if ((command == "maximum_linear_acceleration") ||
       (command == "default_linear_acceleration")) // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxLinearAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaxLinearAccel, 0.0);
   }
   else if ((command == "maximum_radial_acceleration") ||
            (command == "default_radial_acceleration")) // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxRadialAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaxRadialAccel, 0.0);
   }
   else if (command == "turn_g_limit")
   {
      double gLimit;
      aInput.ReadValueOfType(gLimit, UtInput::cACCELERATION);
      aInput.ValueGreater(gLimit, UtEarth::cACCEL_OF_GRAVITY);
      mMaxRadialAccel = sqrt(gLimit * gLimit - UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY);
   }
   else if ((command == "maximum_climb_rate") || (command == "default_climb_rate") || // for backward compatibility
            (command == "default_dive_rate"))                                         // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxClimbRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxClimbRate, 0.0);
   }
   else if (command == "turn_rate_limit")
   {
      aInput.ReadValueOfType(mTurnRateLimit, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mTurnRateLimit, 0.0);
   }
   else if (command == "bank_angle_limit")
   {
      aInput.ReadValueOfType(mRollAngleLimit, UtInput::cANGLE);
      if (mRollAngleLimit != 0)
      {
         aInput.ValueInClosedRange(mRollAngleLimit, 0.0, 85.0 * UtMath::cRAD_PER_DEG);
         mMaxRadialAccel = tan(mRollAngleLimit) * UtEarth::cACCEL_OF_GRAVITY;
      }
   }
   else if (command == "heading_pursuit_gain")
   {
      aInput.ReadValue(mHeadingPursuitGain);
      aInput.ValueGreater(mHeadingPursuitGain, 0.0);
   }
   else if (command == "body_g_limit")
   {
      aInput.ReadValueOfType(mBodyVertLimit, UtInput::cACCELERATION);
      aInput.ValueGreater(mBodyVertLimit, UtEarth::cACCEL_OF_GRAVITY);
   }
   else if (command == "roll_rate_limit")
   {
      aInput.ReadValueOfType(mRollRateLimit, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mRollRateLimit, 0.0);
   }
   else if (command == "maximum_flight_path_angle")
   {
      aInput.ReadValueOfType(mMaxFlightPathAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMaxFlightPathAngle, 0.0, UtMath::cPI_OVER_2);
   }
   else if (command == "maximum_altitude")
   {
      aInput.ReadValueOfType(mMaxAltitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaxAltitude, mMinAltitude);
   }
   else if (command == "minimum_altitude")
   {
      aInput.ReadValueOfType(mMinAltitude, UtInput::cLENGTH);
      aInput.ValueLessOrEqual(mMinAltitude, mMaxAltitude);
   }
   else if (command == "maximum_speed")
   {
      aInput.ReadValueOfType(mMaxSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMaxSpeed, 0.0);
   }
   else if (command == "minimum_speed")
   {
      aInput.ReadValueOfType(mMinSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinSpeed, 0.0);
   }
   else if (command == "maximum_turn_rate")
   {
      aInput.ReadValueOfType(mMaxTurnRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mMaxTurnRate, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
