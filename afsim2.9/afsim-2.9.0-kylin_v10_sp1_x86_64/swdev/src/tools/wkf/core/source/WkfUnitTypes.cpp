// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfUnitTypes.hpp"

#include <cassert>

void wkf::UnitBase::SetValue(double aValue)
{
   if (!mInitialized)
   {
      InitializeSubscriptions();
      mInitialized = true;
   }
   mValue = aValue;
   UpdateDisplay();
}

QString wkf::UnitBase::GetString(double aValue, const std::string& aUnitString)
{
   QString retVal;

   if (mPrecisionType == 'f')
   {
      retVal = QString::number(aValue, mPrecisionType, mPrecision);
   }
   else
   {
      // Have to manually add trailing zeros instead of using 'f' option in QString::number because we want 'g' option
      // for significant digits (which does not maintain trailing zeros), instead of 'f' for precision (which maintains
      // trailing zeros)
      retVal = QString::number(aValue, 'g', mPrecision);
      // if contains e, the number is in exponential format, if we need to pad zeros, they are to be added in front of the 'e'
      if (retVal.contains('e'))
      {
         int        eIndex       = retVal.indexOf('e');
         int        targetLength = mPrecision;
         QStringRef prefix(&retVal, 0, eIndex);
         if (prefix.contains('-'))
         {
            targetLength++;
         }
         if (prefix.contains('.'))
         {
            targetLength++;
         }
         else
         {
            if (prefix.length() != targetLength)
            {
               retVal.insert(eIndex, ".");
               eIndex++;
               targetLength++;
            }
         }
         int numSpacesToPad = targetLength - eIndex;
         for (int i = 0; i < numSpacesToPad; ++i)
         {
            retVal.insert(eIndex, "0");
         }
      }
      else
      {
         // Decimal format, add '.' and trailing zeros if required to display a number of digits equal to mPrecision
         int targetLength = mPrecision;
         if (retVal.contains('-'))
         {
            targetLength++;
         }
         if (retVal.contains('.'))
         {
            targetLength++;
         }
         else
         {
            if (retVal.length() != targetLength)
            {
               retVal.append(".");
               targetLength++;
            }
         }
         int numSpacesToPad = targetLength - retVal.length();
         for (int i = 0; i < numSpacesToPad; ++i)
         {
            retVal.append("0");
         }
      }
   }

   return retVal + " " + QString::fromStdString(aUnitString);
}

void wkf::UnitBase::SetUnitText(const QString& aText)
{
   mUnitInterface->SetUnitText(aText);
}

wkf::UnitInterface::UnitInterface(std::unique_ptr<UnitBase> aUnit)
   : mUnit(std::move(aUnit))
{
   // At this point if mUnit is null, it is an error
   assert(mUnit != nullptr);
   mUnit->SetInterface(this);
}
