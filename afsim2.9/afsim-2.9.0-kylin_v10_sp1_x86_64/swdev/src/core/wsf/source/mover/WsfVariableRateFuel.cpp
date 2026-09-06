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

#include "WsfVariableRateFuel.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"

namespace
{
const double cHUGE_ALTITUDE_OR_SPEED = 1.0E+10;
}

WsfVariableRateFuel::WsfVariableRateFuel(WsfScenario& aScenario)
   : WsfFuel(aScenario)
   , mTables()
   , mInputMode(nullptr)
{
}

// virtual
WsfVariableRateFuel::~WsfVariableRateFuel() {}

//! Copy constructor (for Clone()).
// protected
WsfVariableRateFuel::WsfVariableRateFuel(const WsfVariableRateFuel& aSrc)
   : WsfFuel(aSrc)
   , mTables(aSrc.mTables)
   , mInputMode(aSrc.mInputMode)
{
}

//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuel* WsfVariableRateFuel::Clone() const
{
   return new WsfVariableRateFuel(*this);
}

// virtual
void WsfVariableRateFuel::SetMode(WsfStringId aMode)
{
   WsfFuel::SetMode(aMode);
}

// ============================================================================
//! Initialize the fuel object.
//!
//! @param aSimTime       [input] The current simulation time.
//! @return 'true' if the object was successfully initialized or 'false'
//! if initialization failed.
// virtual
bool WsfVariableRateFuel::Initialize(double aSimTime)
{
   bool success = true;

   if (mTables.empty())
   {
      success  = false;
      auto out = ut::log::error() << "Fuel did not define a Fuel Rate Table.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel: " << GetName();
   }
   else
   {
      for (RateTableIter i(mTables.begin()); i != mTables.end(); ++i)
      {
         if (i->second.mAltitudes.empty())
         {
            auto out = ut::log::error() << "Fuel Rate Table does not contain any defined altitudes.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Fuel: " << GetName();
            success = false;
         }
      }
   }

   // Call the base class
   success &= WsfFuel::Initialize(aSimTime);

   if (!GetCurrentTable())
   {
      auto out = ut::log::error() << "There is no Fuel Rate Table matching the current 'mode'.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel: " << GetName();
      out.AddNote() << "Mode: " << GetMode();
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
bool WsfVariableRateFuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "consumption_rate")
   {
      throw UtInput::BadValue(aInput, "A constant consumption rate is not permitted in this implementation.");
   }
   else if ((command == "rate") || (command == "rates"))
   {
      RateTableConstIter i(mTables.find(mInputMode));
      if (i != mTables.end())
      {
         throw UtInput::BadValue(aInput,
                                 "Cannot input multiple default fuel tables, or two tables for the same fuel mode.");
      }

      RateTablePair pair(mInputMode, Table());

      if (ProcessTablePairInput(aInput, pair))
      {
         mTables.insert(pair);
      }
   }
   else if (command == "table_for_mode")
   {
      std::string mode;
      aInput.ReadValue(mode);
      if (mode == "0")
      {
         mInputMode.Clear();
      }
      else
      {
         mInputMode = mode;
      }
   }
   else
   {
      myCommand = WsfFuel::ProcessInput(aInput);
   }

   return myCommand;
}

// ============================================================================
//! Process the fuel object block.
//!
//! @param aInput    [input]  The input stream.
//! @param aPair     [update] The rate table be loaded
//!
//! @return 'true' if the command was recognized (and processed) or 'false'
//! if the command was not one recognized by this class.
// virtual
bool WsfVariableRateFuel::ProcessTablePairInput(UtInput& aInput, RateTablePair& aPair)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   Table& table = aPair.second;

   if (command == "rate")
   {
      if (table.mRateTableUsed)
      {
         throw UtInput::BadValue(aInput, "'rate' cannot be used after a fuel rate table has been defined");
      }
      table.mConstantRateUsed = true;

      // Get the constant value
      double value(0.0);
      aInput.ReadValueOfType(value, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(value, 0.0);

      // Load into a constant table
      CreateConstantTable(value, table);
   }
   else if (command == "rates")
   {
      if (table.mConstantRateUsed)
      {
         throw UtInput::BadValue(aInput, "fuel rate table cannot be used after 'rate' has been defined.");
      }
      table.mRateTableUsed = true;

      double              altitude(0.0);
      bool                haveAltitude(false);
      std::vector<double> speeds;
      std::vector<double> rates;

      UtInputBlock block(aInput, "end_rates");
      while (block.ReadCommand())
      {
         command = block.GetCommand();
         if (command == "altitude")
         {
            // Check data and write data
            if (!speeds.empty())
            {
               // speeds and rates
               if (speeds.size() != rates.size())
               {
                  throw UtInput::BadValue(aInput, "Number of speed and rate entries must match in fuel table.");
               }
               AddAltitudeEntry(altitude, speeds, rates, table);
               speeds.clear();
               rates.clear();
            }
            else if (!rates.empty())
            {
               // rates only

               // Create speed entries
               speeds.push_back(-1.0);
               speeds.push_back(cHUGE_ALTITUDE_OR_SPEED);

               // Repeat the rate entry
               rates.push_back(rates[0]);

               AddAltitudeEntry(altitude, speeds, rates, table);
               speeds.clear();
               rates.clear();
            }
            haveAltitude = true;

            // Read
            aInput.ReadValueOfType(altitude, UtInput::cLENGTH);
         }
         else if (command == "speed")
         {
            // Check
            if (!haveAltitude)
            {
               throw UtInput::BadValue(aInput, "altitude has not been defined");
            }

            // Read
            double value(0.0);
            aInput.ReadValueOfType(value, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(value, 0.0);
            speeds.push_back(value);
         }
         else if (command == "rate")
         {
            // Check
            if (!haveAltitude)
            {
               throw UtInput::BadValue(aInput, "altitude has not been defined.");
            }

            // Read
            double value(0.0);
            aInput.ReadValueOfType(value, UtInput::cMASS_TRANSFER);
            aInput.ValueGreaterOrEqual(value, 0.0);
            rates.push_back(value);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Error reading fuel rates.");
         }
      }

      // Check data and write data
      if (!speeds.empty())
      {
         // speeds and rates
         if (speeds.size() != rates.size())
         {
            throw UtInput::BadValue(aInput, "Number of speed and rate entries must match.");
         }
         // Table lookup will fail without at least 2 entries
         if (speeds.size() == 1)
         {
            speeds.push_back(speeds[0]);
            rates.push_back(rates[0]);
         }
         AddAltitudeEntry(altitude, speeds, rates, table);
         speeds.clear();
         rates.clear();
      }
      else if (!rates.empty())
      {
         // rates only

         // Create speed entries
         speeds.push_back(-1.0);
         speeds.push_back(cHUGE_ALTITUDE_OR_SPEED);

         // Repeat the rate entry
         rates.push_back(rates[0]);

         AddAltitudeEntry(altitude, speeds, rates, table);
         speeds.clear();
         rates.clear();
      }
      if (table.mAltitudes.size() < 2)
      {
         // Just copy the first entry
         table.mAltitudes.push_back(table.mAltitudes[0]);
      }
   }

   return myCommand;
}

// ============================================================================
//! Gets the fuel consumption rate at the platform's current altitude and speed
//!
//! @return A fuel consumption rate.
// virtual
double WsfVariableRateFuel::CalcConsumptionRate()
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform speed
   double speed(platformPtr->GetSpeed());

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return CalcConsumptionRateAltitudeSpeed(altitude, speed);
}

// ============================================================================
//! Gets the fuel consumption rate at the given altitude and speed
//!
//! @return A fuel consumption rate.
// virtual
double WsfVariableRateFuel::CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed)
{
   // Return immediately if the rate table is empty for some reason
   Table* tblPtr = GetCurrentTable();
   if (tblPtr == nullptr)
   {
      auto out = ut::log::error() << "Platform has no table in variable rate fuel for mode.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Mode: " << GetMode();
      return 0.0;
   }

   Table& table = *tblPtr;

   if (table.mAltitudes.empty())
   {
      // return the base class value in case it has been set and the table is empty, will be 0.0 if neither have been set
      return WsfFuel::CalcConsumptionRate();
   }

   // Find the index to use
   int    index(0);
   double ratio(0.0);
   if (aAltitude <= table.mAltitudes[0].mAltitude)
   {
      index = 0;
      ratio = 0.0;
   }
   else if (aAltitude >= table.mAltitudes[table.mAltitudes.size() - 1].mAltitude)
   {
      index = static_cast<int>(table.mAltitudes.size()) - 2;
      ratio = 1.0;
   }
   else
   {
      while (aAltitude > table.mAltitudes[index].mAltitude)
      {
         ++index;
      }
      while (aAltitude < table.mAltitudes[index].mAltitude)
      {
         --index;
      }
      ratio = (aAltitude - table.mAltitudes[index].mAltitude) /
              (table.mAltitudes[index + 1].mAltitude - table.mAltitudes[index].mAltitude);
   }

   // Find the first interpolated rate
   table.mAltitudes[index].mSpeedLookup.Lookup(table.mAltitudes[index].mSpeeds, aSpeed);
   double rate1 = TblEvaluate(table.mAltitudes[index].mRates, table.mAltitudes[index].mSpeedLookup);

   // Find the second interpolated rate
   table.mAltitudes[index + 1].mSpeedLookup.Lookup(table.mAltitudes[index + 1].mSpeeds, aSpeed);
   double rate2 = TblEvaluate(table.mAltitudes[index + 1].mRates, table.mAltitudes[index + 1].mSpeedLookup);

   /*
      if (DebugEnabled())
      {
         ut::log::debug() << "WsfVariableRateFuel::GetConsumptionRate() " << GetPlatform()->GetName() << '.' << GetName()
            << " Altitude=" << aAltitude
            << " Speed=" << aSpeed
            << " Rate=" << (rate1 + ratio * (rate2 - rate1));

      }
   */

   // Finally calculate the fuel rate value and return it
   return (rate1 + ratio * (rate2 - rate1));
}

// ============================================================================
//! The following is a convenience method to create a minimal
//! table that contains a constant value.
//!
//! @param aSimTime       [input] The constant fuel rate.
void WsfVariableRateFuel::CreateConstantTable(double aValue, Table& aTable)
{
   std::vector<double> speedValues;
   speedValues.push_back(-1.0);
   speedValues.push_back(cHUGE_ALTITUDE_OR_SPEED);

   std::vector<double> rates;
   rates.push_back(aValue);
   rates.push_back(aValue);

   AddAltitudeEntry(-cHUGE_ALTITUDE_OR_SPEED, speedValues, rates, aTable);
   AddAltitudeEntry(cHUGE_ALTITUDE_OR_SPEED, speedValues, rates, aTable);
}

// ============================================================================
//! Add an entry to the fuel table.
//!
//! @param aAltitude     [input] The altitude.
//! @param aSpeeds       [input] Speed values for this altitude.
//! @param aRates        [input] Fuel rates for this altitude.
void WsfVariableRateFuel::AddAltitudeEntry(double               aAltitude,
                                           std::vector<double>& aSpeeds,
                                           std::vector<double>& aRates,
                                           Table&               aTable)
{
   AltitudeEntry altEntry;
   altEntry.mAltitude = aAltitude;
   altEntry.mSpeeds.SetValues(aSpeeds);

   altEntry.mRates.Resize(altEntry.mSpeeds.GetSize());
   for (size_t j = 0; j < altEntry.mSpeeds.GetSize(); ++j)
   {
      altEntry.mRates.Set(aRates[j], j);
   }

   aTable.mAltitudes.push_back(altEntry);
}

WsfVariableRateFuel::Table* WsfVariableRateFuel::GetCurrentTable()
{
   RateTables::iterator i = mTables.find(GetModeId());
   if (i != mTables.end())
   {
      return &i->second;
   }
   if (mTables.empty())
   {
      return nullptr;
   }
   else
   {
      return &mTables.begin()->second;
   }
}
