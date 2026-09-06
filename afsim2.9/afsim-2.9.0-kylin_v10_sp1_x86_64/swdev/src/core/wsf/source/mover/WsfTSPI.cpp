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

#include "WsfTSPI.hpp"

#include <iomanip>
#include <iostream>

#include "UtInput.hpp"

WsfTSPI::WsfTSPI()
   : mTime(0.0, UtInput::cTIME)
   , mLat(0.0, UtInput::cLENGTH)
   , mLon(0.0, UtInput::cLENGTH)
   , mAlt(0.0, UtInput::cLENGTH)
   , mSpeed(0.0, UtInput::cSPEED)
   , mHeading(0.0, UtInput::cANGLE)
   , mPitch(0.0, UtInput::cANGLE)
   , mRoll(0.0, UtInput::cANGLE)
{
}

// virtual
bool WsfTSPI::ProcessInput(UtInput& aInput)
{
   bool        ok         = true;
   std::string command    = aInput.GetCommand();
   Element*    elementPtr = nullptr;
   if (command == "time")
   {
      elementPtr = &mTime;
   }
   else if (command == "altitude")
   {
      elementPtr = &mAlt;
   }
   else if (command == "speed")
   {
      elementPtr = &mSpeed;
   }
   else if (command == "pitch")
   {
      elementPtr = &mPitch;
   }
   else if (command == "roll")
   {
      elementPtr = &mRoll;
   }
   else if (command == "heading")
   {
      elementPtr = &mHeading;
   }
   else
   {
      ok = false;
   }
   if (ok)
   {
      std::string next;
      aInput.ReadValue(next);
      if (next == "in") // units follow
      {
         std::string units;
         aInput.ReadValue(units);
         elementPtr->SetPersistentUnits(units);
      }
      else if (next == "inverted")
      {
         elementPtr->SetInverted(true);
      }
      else
      {
         ok = false;
      }
   }
   return ok;
}

// friend
/** Stream out operator (typically, to a file); units are converted on output. */
std::ostream& operator<<(std::ostream& aStream, const WsfTSPI& aTSPI)
{
   aStream << std::setprecision(10);
   aStream << aTSPI.mTime << " ";
   aStream << aTSPI.mLat << " ";
   aStream << aTSPI.mLon << " ";
   aStream << aTSPI.mAlt << " ";
   aStream << aTSPI.mSpeed << " ";
   aStream << aTSPI.mHeading << " ";
   aStream << aTSPI.mPitch << " ";
   aStream << aTSPI.mRoll << " " << std::endl;
   return aStream;
}

// friend
/** Stream in operator (typically, from a file); units are converted on input. */
std::istream& operator>>(std::istream& aStream, WsfTSPI& aTSPI)
{
   aStream >> aTSPI.mTime;
   aStream >> aTSPI.mLat;
   aStream >> aTSPI.mLon;
   aStream >> aTSPI.mAlt;
   aStream >> aTSPI.mSpeed;
   aStream >> aTSPI.mHeading;
   aStream >> aTSPI.mPitch;
   aStream >> aTSPI.mRoll;
   return aStream;
}

WsfTSPI::Element::Element(double aValue, unsigned int aValueType)
   : mAttribute(aValue)
   , mValueType(aValueType)
   , mPersistentUnits()
   , mHasConversion(false)
   , mIsInverted(false)
{
}

/** Assign the persistent units (i.e., the units in which the value is saved). */
void WsfTSPI::Element::SetPersistentUnits(const std::string& aConversion)
{
   mPersistentUnits = aConversion;
   mHasConversion   = true;
}

void WsfTSPI::Element::ConvertInput()
{
   if (mHasConversion)
   {
      mAttribute = UtInput::ConvertValueFrom(mAttribute, mPersistentUnits, static_cast<UtInput::ValueType>(mValueType));
   }
   if (mIsInverted)
   {
      mAttribute = -mAttribute;
   }
}

// private
double WsfTSPI::Element::GetOutputValue() const
{
   double outputValue = mAttribute;
   if (mHasConversion)
   {
      outputValue = UtInput::ConvertValueTo(mAttribute, mPersistentUnits, static_cast<UtInput::ValueType>(mValueType));
   }
   if (mIsInverted)
   {
      outputValue = -outputValue;
   }
   return outputValue;
}

// friend
/** Stream out operator (typically, to a file); units are converted on output. */
std::ostream& operator<<(std::ostream& aStream, const WsfTSPI::Element& aElement)
{
   aStream << aElement.GetOutputValue();
   return aStream;
}

// friend
/** Stream in operator (typically, from a file); units are converted on input. */
std::istream& operator>>(std::istream& aStream, WsfTSPI::Element& aElement)
{
   aStream >> aElement.mAttribute;
   aElement.ConvertInput();
   return aStream;
}
