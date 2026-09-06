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

#include "WsfEM_SimpleAttenuation.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"

// =================================================================================================
WsfEM_SimpleAttenuation::WsfEM_SimpleAttenuation()
   : WsfEM_Attenuation()
   , mAttenuationFactor(0.0)
   , mSpecificAttenuation(0.0)
{
}
// =================================================================================================
WsfEM_SimpleAttenuation::WsfEM_SimpleAttenuation(const WsfEM_SimpleAttenuation& aSrc)
   : WsfEM_Attenuation(aSrc)
   , mAttenuationFactor(aSrc.mAttenuationFactor)
   , mSpecificAttenuation(aSrc.mSpecificAttenuation)
{
}

// =================================================================================================
//! Factory method called by WsfEM_AttenuationTypes.
// static
WsfEM_Attenuation* WsfEM_SimpleAttenuation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Attenuation* instancePtr = nullptr;
   if ((aTypeName == "WSF_SIMPLE_ATTENUATION") || (aTypeName == "simple"))
   {
      instancePtr = new WsfEM_SimpleAttenuation();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Attenuation* WsfEM_SimpleAttenuation::Clone() const
{
   return new WsfEM_SimpleAttenuation(*this);
}

// =================================================================================================
// virtual
bool WsfEM_SimpleAttenuation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "specific_attenuation")
   {
      double      value;
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);

      // Parse the units.
      std::string::size_type slashPos = units.find('/');
      if ((slashPos != std::string::npos) && (slashPos != 0) && ((slashPos + 1) != units.size()))
      {
         std::string ratioUnits(units.substr(0, slashPos));
         std::string lengthUnits(units.substr(slashPos + 1));
         double      ratioValue    = aInput.ConvertValue(value, ratioUnits, UtInput::cRATIO);
         double      ratioValue_dB = UtMath::LinearToDB(ratioValue);
         double      lengthFactor  = aInput.ConvertValue(1.0, lengthUnits, UtInput::cLENGTH);
         mSpecificAttenuation      = ratioValue_dB / lengthFactor;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown specific attenuation units " + units);
      }
      mAttenuationFactor = 0.0; // specific_attenuation disables attenuation_factor
   }
   else if (command == "attenuation_factor")
   {
      aInput.ReadValueOfType(mAttenuationFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mAttenuationFactor, 0.0, 1.0);
   }
   else
   {
      myCommand = WsfEM_Attenuation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// protected virtual
double WsfEM_SimpleAttenuation::ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency)
{
   double atten = mAttenuationFactor;
   if (atten <= 0.0)
   {
      double atten_dB = mSpecificAttenuation * aRange; // dB/m * range(m) = dB
      atten           = pow(10.0, -0.1 * atten_dB);
   }
   return atten;
}
