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
#ifndef UTUNITVALUE_HPP
#define UTUNITVALUE_HPP

#include <iosfwd>
#include <string>

#include "UtInput.hpp"
#include "UtStringUtil.hpp"

template<UtInput::ValueType VALUE_TYPE>
class UtUnitValue
{
public:
   UtUnitValue()
      : mValue(0)
      , mDefaultUnit("")
   {
   }

   UtUnitValue(double aValue, const std::string& aDefaultUnit)
      : mValue(aValue)
      , mDefaultUnit(aDefaultUnit)
   {
   }

   operator double() const { return mValue; }

   friend std::istream& operator>>(std::istream& aIn, UtUnitValue<VALUE_TYPE>& aUnitary)
   {
      std::string unit;
      aIn >> aUnitary.mValue;
      if (aIn >> unit)
      {
         UtStringUtil::ToLower(unit);

         try
         {
            aUnitary.mValue = UtInput::ConvertValueFrom(aUnitary.mValue, unit, VALUE_TYPE);
         }
         catch (UtException& /* e */)
         {
            aIn.clear(std::ios_base::failbit);
         }

         if (!aIn.fail())
         {
            aUnitary.mDefaultUnit = unit;
         }
      }
      return aIn;
   }

   friend std::ostream& operator<<(std::ostream& aOut, const UtUnitValue<VALUE_TYPE>& aUnitary)
   {
      std::string units(aUnitary.mDefaultUnit);
      UtStringUtil::ToLower(units);
      double value = UtInput::ConvertValueTo(aUnitary.mValue, units, VALUE_TYPE);
      aOut << value << " " << aUnitary.mDefaultUnit;
      return aOut;
   }

protected:
   double      mValue;
   std::string mDefaultUnit;
};
// Note: these are obsolete, use UtUnitTypes.hpp
// typedef UtUnitValue<UtInput::cPOWER>      UtUnitPower;
// typedef UtUnitValue<UtInput::cPOWER_DB>   UtUnitPowerDB;
// typedef UtUnitValue<UtInput::cFREQUENCY>  UtUnitFrequency;
// typedef UtUnitValue<UtInput::cRATIO>      UtUnitRatio;
// typedef UtUnitValue<UtInput::cENERGY>     UtUnitEnergy;
// typedef UtUnitValue<UtInput::cFLUENCE>    UtUnitFluence;
// typedef UtUnitValue<UtInput::cIRRADIANCE> UtUnitIrradiance;
// typedef UtUnitValue<UtInput::cAREA>       UtUnitArea;
// typedef UtUnitValue<UtInput::cDATA_RATE>  UtUnitDataRate;

#endif
