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

#include "WsfAngleOfAttack.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"

WsfAngleOfAttack::WsfAngleOfAttack()
   : mLastAOA_Altitude(1.0E+10)
   , mLastAOA_Speed(1.0E+10)
   , mLastAOA(0.0)
{
}

//! Returns the pitch of the platform after the angle-of-attack is applied
double WsfAngleOfAttack::GetPitch(double aFlightPathAngle, double aAltitude, double aSpeed)
{
   double pitch = aFlightPathAngle + GetAngleOfAttack(aAltitude, aSpeed);
   if (pitch >= UtMath::cPI_OVER_2)
   {
      pitch = 0.9999 * UtMath::cPI_OVER_2;
   }
   else if (pitch <= -UtMath::cPI_OVER_2)
   {
      pitch = -0.9999 * UtMath::cPI_OVER_2;
   }
   return pitch;
}

bool WsfAngleOfAttack::ProcessInput(UtInput& aInput)
{
   bool    success              = true;
   auto    tablePtr             = std::make_shared<AOA_Table>();
   bool    explicitAltitudeUsed = false;
   bool    altitudeValid        = true;
   bool    speedValid           = true;
   bool    angleValid           = true;
   AOA_Row row;
   row.mAltitude = -1.0E+10;

   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "altitude")
      {
         if (!altitudeValid)
         {
            throw UtInput::OutOfContext(aInput);
         }
         if (!row.mSpeeds.empty())
         {
            tablePtr->mRows.push_back(row);
            row.mSpeeds.clear();
            row.mAngles.clear();
         }
         double altitude;
         aInput.ReadValueOfType(altitude, UtInput::cLENGTH);
         if ((!tablePtr->mRows.empty()) && (altitude <= tablePtr->mRows.back().mAltitude))
         {
            throw UtInput::BadValue(aInput, "altitudes must be monotonically increasing");
         }
         row.mAltitude        = static_cast<float>(altitude);
         explicitAltitudeUsed = true;
         altitudeValid        = false;
         speedValid           = true;
         angleValid           = true;
      }
      else if (command == "speed")
      {
         if (!speedValid)
         {
            throw UtInput::OutOfContext(aInput);
         }
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(speed, 0.0);
         if ((!row.mSpeeds.empty()) && (static_cast<float>(speed) <= row.mSpeeds.back()))
         {
            throw UtInput::BadValue(aInput, "speeds must be monotonically increasing");
         }
         std::string angleStr;
         aInput.ReadValue(angleStr);
         aInput.StringEqual(angleStr, "angle");
         double angle;
         aInput.ReadValueOfType(angle, UtInput::cANGLE);
         aInput.ValueInClosedRange(angle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         row.mSpeeds.push_back(static_cast<float>(speed));
         row.mAngles.push_back(static_cast<float>(angle));
         altitudeValid = explicitAltitudeUsed; // 'altitude' is valid only if seen before.
         angleValid    = false;                // Can't use 'angle' with 'speed-angle' form
      }
      else if (command == "angle")
      {
         // Single angle entry for this altitude (or for all altitudes).
         if (!angleValid)
         {
            throw UtInput::OutOfContext(aInput);
         }
         double angle;
         aInput.ReadValueOfType(angle, UtInput::cANGLE);
         aInput.ValueInClosedRange(angle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         row.mSpeeds.push_back(0.0F);
         row.mAngles.push_back(static_cast<float>(angle));
         tablePtr->mRows.push_back(row);
         row.mSpeeds.clear();
         row.mAngles.clear();
         altitudeValid = explicitAltitudeUsed; // 'altitude' is valid only if seen before.
         speedValid    = false;
         angleValid    = false;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   if (!row.mSpeeds.empty())
   {
      tablePtr->mRows.push_back(row);
   }
   if (!tablePtr->mRows.empty())
   {
      mAOA_TablePtr = tablePtr; // Non-null table
   }
   else
   {
      mAOA_TablePtr = std::make_shared<AOA_Table>(); // Null table
   }
   return success;
}

//! Given the altitude and speed, return the desired angle-of-attack to be used.
// private
double WsfAngleOfAttack::GetAngleOfAttack(double aAltitude, double aSpeed)
{
   if (mAOA_TablePtr == nullptr)
   {
      return 0.0; // No table... no AOA
   }
   else if ((fabs(aAltitude - mLastAOA_Altitude) < 10.0) && (fabs(aSpeed - mLastAOA_Speed) < 1.0))
   {
      return mLastAOA; // Not much change ... use last AOA
   }

   const AOA_Table* tablePtr = mAOA_TablePtr.get();
   float            altitude = static_cast<float>(aAltitude);

   double angle = 0.0;
   if (altitude <= tablePtr->mRows.front().mAltitude)
   {
      // Below the lowest altitude ... use the lowest altitude.
      angle = GetAngleOfAttack(aSpeed, tablePtr->mRows.front());
   }
   else if (altitude >= tablePtr->mRows.back().mAltitude)
   {
      // Above the highest altitude ... use the highest altitude.
      angle = GetAngleOfAttack(aSpeed, tablePtr->mRows.back());
   }
   else
   {
      // Interpolate along and between two rows.
      unsigned int upperIndex;
      for (upperIndex = 1; upperIndex < tablePtr->mRows.size(); ++upperIndex)
      {
         if (altitude < tablePtr->mRows[upperIndex].mAltitude)
         {
            break;
         }
      }
      assert(upperIndex < tablePtr->mRows.size());
      const AOA_Row& lowerRow   = tablePtr->mRows[upperIndex - 1];
      const AOA_Row& upperRow   = tablePtr->mRows[upperIndex];
      double         lowerAngle = GetAngleOfAttack(aSpeed, lowerRow);
      double         upperAngle = GetAngleOfAttack(aSpeed, upperRow);
      double         ratio      = (altitude - lowerRow.mAltitude) / (upperRow.mAltitude - lowerRow.mAltitude);
      angle                     = lowerAngle + ratio * (upperAngle - lowerAngle);
   }

   mLastAOA_Altitude = aAltitude;
   mLastAOA_Speed    = aSpeed;
   mLastAOA          = angle;
   return angle;
}

//! Given the speed and the angle-of-attack as a function of speed, return the angle-of-attack.
// private
double WsfAngleOfAttack::GetAngleOfAttack(double aSpeed, const AOA_Row& aRow) const
{
   float  speed = static_cast<float>(aSpeed);
   double angle = 0.0;
   if (speed <= aRow.mSpeeds.front())
   {
      angle = aRow.mAngles.front();
   }
   else if (speed >= aRow.mSpeeds.back())
   {
      angle = aRow.mAngles.back();
   }
   else
   {
      unsigned int upperIndex;
      for (upperIndex = 1; upperIndex < aRow.mSpeeds.size(); ++upperIndex)
      {
         if (speed < aRow.mSpeeds[upperIndex])
         {
            break;
         }
      }
      assert(upperIndex < aRow.mSpeeds.size());
      unsigned int lowerIndex = upperIndex - 1;
      double       lowerSpeed = aRow.mSpeeds[lowerIndex];
      double       upperSpeed = aRow.mSpeeds[upperIndex];
      double       lowerAngle = aRow.mAngles[lowerIndex];
      double       upperAngle = aRow.mAngles[upperIndex];
      double       ratio      = (speed - lowerSpeed) / (upperSpeed - lowerSpeed);
      angle                   = lowerAngle + ratio * (upperAngle - lowerAngle);
   }
   return angle;
}
