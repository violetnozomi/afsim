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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfTabularRateFuel.hpp"

#include <limits>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfFuelFlowTables.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"

WsfTabularRateFuel::WsfTabularRateFuel(WsfScenario& aScenario)
   : WsfFuel(aScenario)
   , mSpeedInMach(false)
   , mTables()
   , mAtmosphere(aScenario.GetAtmosphere())
{
}

// virtual
WsfTabularRateFuel::~WsfTabularRateFuel()
{
   for (auto& table : mTables)
   {
      delete table.second;
   }
}

//! Copy constructor (for Clone()).
// protected
WsfTabularRateFuel::WsfTabularRateFuel(const WsfTabularRateFuel& aSrc)
   : WsfFuel(aSrc)
   , mSpeedInMach(aSrc.mSpeedInMach)
   , mTables()
   , mAtmosphere(aSrc.mAtmosphere)
{
   RateTableConstIter cIter = aSrc.mTables.begin();
   for (; cIter != aSrc.mTables.end(); ++cIter)
   {
      mTables.insert(RateTablePair(cIter->first, cIter->second->Clone()));
   }
}


//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuel* WsfTabularRateFuel::Clone() const
{
   return new WsfTabularRateFuel(*this);
}

// virtual
void WsfTabularRateFuel::SetMode(WsfStringId aMode)
{
   WsfFuel::SetMode(aMode);
}

// ============================================================================
//! Initialize the fuel object.
//! @return 'true' if the object was successfully initialized or 'false'
//! if initialization failed.
// virtual
bool WsfTabularRateFuel::Initialize(double aSimTime)
{
   bool success = true;

   if (mTables.empty())
   {
      success  = false;
      auto out = ut::log::error() << "Fuel did not define a Fuel Rate Table.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel Rate Table: " << GetName();
   }
   else
   {
      for (auto& table : mTables)
      {
         success &= table.second->Initialize(GetPlatform());
      }
   }

   // Call the base class
   success &= WsfFuel::Initialize(aSimTime);

   if (GetCurrentTable() == nullptr)
   {
      auto out = ut::log::error() << "There is no table matching the current 'mode'.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel Rate Table: " << GetName();
      success = false;
   }

   return success;
}

// ============================================================================
//! Process the fuel object block.
//!
//! @param aInput    [input]  The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false'
//! if the command was not one recognized by this class.
// virtual
bool WsfTabularRateFuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "consumption_rate")
   {
      throw UtInput::BadValue(aInput, "A constant consumption rate is not permitted in this implementation.");
   }
   else if (command == "fuel_table")
   {
      UtInputBlock inputBlock(aInput);
      ProcessTableBlock(inputBlock);
   }
   else
   {
      myCommand = WsfFuel::ProcessInput(aInput);
   }

   return myCommand;
}

void WsfTabularRateFuel::ProcessTableBlock(UtInputBlock& aInputBlock)
{
   struct InputData temp;

   while (aInputBlock.ReadCommand())
   {
      UtInput&    input = aInputBlock.GetInput();
      std::string command(input.GetCommand());

      if (command == "mode")
      {
         std::string modeName;
         input.ReadValue(modeName);
         temp.Mode = modeName;
      }
      else if (command == "constant")
      {
         double rate;
         input.ReadValueOfType(rate, UtInput::cMASS_TRANSFER);
         input.ValueGreaterOrEqual(rate, 0.0);
         temp.Data.Values.push_back(rate);
      }
      else if ((command == "speeds") || (command == "altitudes") || (command == "weights") || (command == "masses"))
      {
         // First find out which of the IV arrays (first,
         // second, third) we are addressing:

         struct Array* ivArrayPtr = &(temp.FirstIV); // default only
         if (temp.FirstIV.Values.empty())
         {
            // Use default value
         }
         else if (temp.SecondIV.Values.empty())
         {
            ivArrayPtr = &(temp.SecondIV);
         }
         else // if (temp.ThirdIV.Values.empty())
         {
            ivArrayPtr = &(temp.ThirdIV);
         }

         // Now set the type of IV variable we are using:
         UtInput::ValueType type = UtInput::cSPEED; // default only
         if (command == "speeds")
         {
            // Use the default value
            ivArrayPtr->Variable = cSPEED;
            mSpeedInMach         = false;
         }
         else if (command == "altitudes")
         {
            ivArrayPtr->Variable = cALTITUDE;
            type                 = UtInput::cLENGTH;
         }
         else if ((command == "weights") || (command == "masses"))
         {
            ivArrayPtr->Variable = cMASS;
            type                 = UtInput::cMASS;
         }

         UtInputBlock inputBlock(input);
         ProcessVarBlock(inputBlock, type, *ivArrayPtr);
         ivArrayPtr->Check(aInputBlock.GetInput());
      }
      else if (command == "mach")
      {
         struct Array* ivArrayPtr = &(temp.FirstIV); // default only
         if (temp.FirstIV.Values.empty())
         {
            // Use default value
         }
         else if (temp.SecondIV.Values.empty())
         {
            ivArrayPtr = &(temp.SecondIV);
         }
         else // if (temp.ThirdIV.Values.empty())
         {
            ivArrayPtr = &(temp.ThirdIV);
         }
         ivArrayPtr->Variable = cSPEED;

         mSpeedInMach = true;
         UtInputBlock inputBlock(input);
         ProcessMachBlock(inputBlock, *ivArrayPtr);
         ivArrayPtr->Check(aInputBlock.GetInput());
      }
      else if (command == "rates")
      {
         temp.Data.Variable = cRATES;
         UtInputBlock inputBlock(input);
         ProcessVarBlock(inputBlock, UtInput::cMASS_TRANSFER, temp.Data);
      }
      else
      {
         throw UtInput::UnknownCommand(input);
      }
   }

   RateTableIter iter = mTables.find(temp.Mode);
   if (iter != mTables.end())
   {
      // Have already defined a mode table for this mode... not allowed:
      throw UtInput::BadValue(aInputBlock.GetInput(), "Not permitted to define multiple tables for the same fuel mode.");
   }

   // Now, instantiate the correct WsfFuelFlow type, and populate it with data:
   RateTablePair pair;
   pair.first = temp.Mode;

   if (!temp.ThirdIV.Values.empty())
   {
      // 3-D Table lookup
      unsigned int a = static_cast<unsigned int>(temp.FirstIV.Values.size());
      unsigned int b = static_cast<unsigned int>(temp.SecondIV.Values.size());
      unsigned int c = static_cast<unsigned int>(temp.ThirdIV.Values.size());
      unsigned int d = static_cast<unsigned int>(temp.Data.Values.size());

      if ((a * b * c) != d)
      {
         std::string msg = "Dimensions of IV1 * IV2 * IV3 do not equal Dimensions of DV.";
         throw UtInput::BadValue(aInputBlock.GetInput(), msg);
      }

      auto IVs1Ptr = std::make_shared<TblIndVarU<double>>();
      IVs1Ptr->SetValues(temp.FirstIV.Values);

      auto IVs2Ptr = std::make_shared<TblIndVarU<double>>();
      IVs2Ptr->SetValues(temp.SecondIV.Values);

      auto IVs3Ptr = std::make_shared<TblIndVarU<double>>();
      IVs3Ptr->SetValues(temp.ThirdIV.Values);

      auto DVsPtr = std::make_shared<TblDepVar3<double>>();

      size_t outerSize  = IVs1Ptr->GetSize();
      size_t middleSize = IVs2Ptr->GetSize();
      size_t innerSize  = IVs3Ptr->GetSize();

      DVsPtr->Resize(outerSize, middleSize, innerSize);

      size_t m = 0;
      for (size_t i = 0; i < outerSize; ++i)
      {
         for (size_t j = 0; j < middleSize; ++j)
         {
            for (size_t k = 0; k < innerSize; ++k)
            {
               m = i * middleSize * innerSize + j * innerSize + k;
               // ut::log::info() << " i = " << i << " j = " << j << " k = " << k << " m = " << m;
               DVsPtr->Set(temp.Data.Values[m], i, j, k);
            }
         }
      }

      if ((m + 1) != (innerSize * middleSize * outerSize))
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Incorrect number of Dependent values provided.");
      }

      // 3-D Table lookup
      WsfFuelFlow3D* ffPtr = new WsfFuelFlow3D(temp.FirstIV.Variable,
                                               temp.SecondIV.Variable,
                                               temp.ThirdIV.Variable,
                                               IVs1Ptr,
                                               IVs2Ptr,
                                               IVs3Ptr,
                                               DVsPtr,
                                               mAtmosphere);
      ffPtr->SetTableSpeedIsInMach(mSpeedInMach);
      pair.second = ffPtr;
   }
   else if (!temp.SecondIV.Values.empty())
   {
      // 2-D Table lookup
      if (temp.FirstIV.Values.size() * temp.SecondIV.Values.size() != temp.Data.Values.size())
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Dimensions of IV1 * IV2 do not equal DV.");
      }

      auto IVs1Ptr = std::make_shared<TblIndVarU<double>>();
      IVs1Ptr->SetValues(temp.FirstIV.Values);

      auto IVs2Ptr = std::make_shared<TblIndVarU<double>>();
      IVs2Ptr->SetValues(temp.SecondIV.Values);

      auto DVsPtr = std::make_shared<TblDepVar2<double>>();

      size_t outerSize = IVs1Ptr->GetSize();
      size_t innerSize = IVs2Ptr->GetSize();

      DVsPtr->Resize(outerSize, innerSize);

      size_t k = 0;
      for (size_t i = 0; i < outerSize; ++i)
      {
         for (size_t j = 0; j < innerSize; ++j)
         {
            k = i * innerSize + j;
            // ut::log::info() << " i = " << i << " j = " << j << " k = " << k;
            DVsPtr->Set(temp.Data.Values[k], i, j);
         }
      }

      if ((k + 1) != (innerSize * outerSize))
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Incorrect number of Dependent values provided.");
      }

      // 2-D Table lookup
      WsfFuelFlow2D* ffPtr =
         new WsfFuelFlow2D(temp.FirstIV.Variable, temp.SecondIV.Variable, IVs1Ptr, IVs2Ptr, DVsPtr, mAtmosphere);
      ffPtr->SetTableSpeedIsInMach(mSpeedInMach);
      pair.second = ffPtr;
   }
   else if (!temp.FirstIV.Values.empty())
   {
      // 1-D Table lookup
      if (temp.FirstIV.Values.size() != temp.Data.Values.size())
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Dimensions of IV and DV Arrays do not match.");
      }

      auto IVsPtr = std::make_shared<TblIndVarU<double>>();
      IVsPtr->SetValues(temp.FirstIV.Values);

      auto DVsPtr = std::make_shared<TblDepVar1<double>>();

      size_t outerSize = IVsPtr->GetSize();
      DVsPtr->Resize(outerSize);

      for (size_t i = 0; i != outerSize; ++i)
      {
         DVsPtr->Set(temp.Data.Values[i], i);
      }

      WsfFuelFlow1D* ffPtr = new WsfFuelFlow1D(temp.FirstIV.Variable, IVsPtr, DVsPtr, mAtmosphere);
      ffPtr->SetTableSpeedIsInMach(mSpeedInMach);
      pair.second = ffPtr;
   }
   else
   {
      // Constant (No Table lookup)
      if (temp.Table != cCONSTANT)
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Garbled Constant table type.");
      }
      WsfFuelFlow* ffPtr = new WsfFuelFlow(mAtmosphere, temp.Data.Values[0]);
      pair.second        = ffPtr;
   }

   mTables.insert(pair);
}

// ============================================================================
//! Calculates the fuel consumption rate.
//!
//! @return A fuel consumption rate.
// virtual
double WsfTabularRateFuel::CalcConsumptionRate()
{
   WsfFuelFlow* tablePtr = GetCurrentTable();
   if (tablePtr != nullptr)
   {
      return tablePtr->CalcConsumptionRate();
   }
   auto out = ut::log::error() << "No fuel_table for mode.";
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Mode: " << GetMode();
   return 0.0;
}

// ============================================================================
//! Calculates the fuel consumption rate.
//!
//! @return A fuel consumption rate.
// virtual
double WsfTabularRateFuel::CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed)
{
   WsfFuelFlow* tablePtr = GetCurrentTable();
   if (tablePtr != nullptr)
   {
      return tablePtr->CalcConsumptionRateAltitudeSpeed(aAltitude, aSpeed);
   }
   auto out = ut::log::error() << "No fuel_table for mode.";
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Mode: " << GetMode();
   return 0.0;
}

void WsfTabularRateFuel::ProcessVarBlock(UtInputBlock& aInputBlock, UtInput::ValueType aType, Array& aArray)
{
   bool valueRead    = false;
   bool unitsSet     = false;
   aArray.ConvFactor = 1.0;

   if (!aArray.Values.empty())
   {
      throw UtInput::BadValue(aInputBlock.GetInput(), "Array was already populated with values.");
   }

   while (aInputBlock.ReadCommand())
   {
      UtInput&    input = aInputBlock.GetInput();
      std::string command(input.GetCommand());

      if (command == "units")
      {
         if (valueRead)
         {
            throw UtInput::BadValue(input, "The 'units' converter keyword must be specified first in the list.");
         }

         std::string unitsString;
         input.ReadValue(unitsString);
         aArray.ConvFactor = input.ConvertValue(1.0, unitsString, aType);
         unitsSet          = true;
      }
      else if (unitsSet)
      {
         input.PushBack(command);
         double value;
         input.ReadValue(value);
         aArray.Values.push_back(value * aArray.ConvFactor);
         valueRead = true;
      }
      else
      {
         input.PushBack(command);
         double value;
         input.ReadValueOfType(value, aType);
         aArray.Values.push_back(value);
         valueRead = true;
      }
   }
}

void WsfTabularRateFuel::ProcessMachBlock(UtInputBlock& aInputBlock, Array& aArray)
{
   if (!aArray.Values.empty())
   {
      throw UtInput::BadValue(aInputBlock.GetInput(), "Array was already populated with values.");
   }

   std::string command = "";
   while (aInputBlock.ReadCommand(command))
   {
      UtInput& input = aInputBlock.GetInput();
      input.PushBack(command);
      double value;
      input.ReadValue(value);
      aArray.Values.push_back(value);
   }
}

WsfFuelFlow* WsfTabularRateFuel::GetCurrentTable()
{
   RateTableIter i = mTables.find(GetModeId());
   if (i != mTables.end())
   {
      return i->second;
   }
   return nullptr;
}


void WsfTabularRateFuel::Array::Check(UtInput& aInput)
{
   if (Values.size() < 2)
   {
      throw UtInput::BadValue(aInput, "Independent Variable Arrays must have at least 2 elements.");
   }
   for (unsigned int i = 1; i != Values.size(); ++i)
   {
      if (Values[i] <= Values[i - 1])
      {
         throw UtInput::BadValue(aInput, "Independent Variable Arrays must be ever increasing values.");
      }
   }
}
