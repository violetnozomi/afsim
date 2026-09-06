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

#include "WsfEM_SurfaceClutterTable.hpp"

#include <set>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfUtil.hpp"

// =================================================================================================
WsfEM_SurfaceClutterTable::WsfEM_SurfaceClutterTable()
   : WsfEM_Clutter()
   , mAltitudesPtr(std::make_shared<ClutterTable>())
   , mAltitudes(*mAltitudesPtr)
   , mType(cUNDEFINED)
   , mConstantClutter(0.0)
   , mReceiver()
   , mTarget()
{
}

// =================================================================================================
WsfEM_SurfaceClutterTable::WsfEM_SurfaceClutterTable(const WsfEM_SurfaceClutterTable& aSrc)
   : WsfEM_Clutter(aSrc)
   , mAltitudesPtr(aSrc.mAltitudesPtr)
   , mAltitudes(*mAltitudesPtr)
   , mType(aSrc.mType)
   , mConstantClutter(aSrc.mConstantClutter)
   , mReceiver(aSrc.mReceiver)
   , mTarget(aSrc.mTarget)
{
   // TODO need to make the table a shared ptr
}

WsfEM_SurfaceClutterTable& WsfEM_SurfaceClutterTable::operator=(const WsfEM_SurfaceClutterTable& aRhs)
{
   if (this != &aRhs)
   {
      WsfEM_Clutter::operator=(aRhs);
      mAltitudesPtr          = aRhs.mAltitudesPtr;
      mAltitudes             = *mAltitudesPtr;
      mType                  = aRhs.mType;
      mConstantClutter       = aRhs.mConstantClutter;
      mReceiver              = aRhs.mReceiver;
      mTarget                = aRhs.mTarget;
   }
   return *this;
}

// virtual
WsfEM_SurfaceClutterTable::ClutterTable::~ClutterTable()
{
   ClutterTable::iterator iter = begin();
   while (iter != end())
   {
      delete *iter;
      ++iter;
   }
}

// =================================================================================================
// virtual
WsfEM_SurfaceClutterTable::~WsfEM_SurfaceClutterTable() {}

// =================================================================================================
//! Factory method called by WsfEM_ClutterTypes.
// static
WsfEM_Clutter* WsfEM_SurfaceClutterTable::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Clutter* instancePtr = nullptr;
   if ((aTypeName == "WSF_SURFACE_CLUTTER_TABLE") || (aTypeName == "surface_clutter_table"))
   {
      instancePtr = new WsfEM_SurfaceClutterTable();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Clutter* WsfEM_SurfaceClutterTable::Clone() const
{
   return new WsfEM_SurfaceClutterTable(*this);
}

// =================================================================================================
// virtual
bool WsfEM_SurfaceClutterTable::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   bool        inlineTable = false;

   // new syntax; accept either "inline_table" or "file"
   if (command == "inline_table")
   {
      aInput.ReadCommand(command);
      inlineTable = true;
   }

   if ((command == "clutter") || // deprecate
       ((command == "constant") && inlineTable))
   {
      if (mType != cUNDEFINED)
      {
         throw UtInput::BadValue(aInput, "'clutter' cannot be used after a clutter table has been defined");
      }
      mType = cCONSTANT;

      // Get the constant value
      aInput.ReadValueOfType(mConstantClutter, UtInput::cPOWER_DB);
      aInput.ValueGreater(mConstantClutter, 0.0);

      // Load into a constant table
      // CreateConstantTable(value);
   }
   else if ((command == "clutters") || // deprecate
            inlineTable)
   {
      if (mType != cUNDEFINED)
      {
         throw UtInput::BadValue(aInput, "clutter table cannot be used after 'clutter' has been defined.");
      }
      mType = cGENERIC;
      Load(aInput);
   }
   else if (command == "file" && (!inlineTable))
   {
      std::string filename;
      aInput.ReadCommand(filename);
      UtInput fileInput;
      fileInput.PushInput(ut::make_unique<UtInputFile>(filename));
      Load(fileInput);
   }
   else
   {
      myCommand = WsfEM_Clutter::ProcessInput(aInput);
   }

   return myCommand;
}

bool WsfEM_SurfaceClutterTable::Load(UtInput& aInput)
{
   std::string         command;
   double              altitude(0.0);
   double              bearing(0.0);
   bool                haveAltitude(false);
   bool                haveRange(false);
   std::vector<double> ranges;
   std::vector<double> clutters;
   std::vector<double> bearings;
   std::set<double>    rangeSet;
   std::set<double>    bearingSet;

   UtInputBlock block(aInput, "end_clutters");
   while (block.ReadCommand())
   {
      command = block.GetCommand();
      if (command == "altitude")
      {
         if (haveAltitude) // don't execute this block the first time in the loop
         {
            // ranges and clutters
            if (mType == cGENERIC)
            {
               if (ranges.size() != clutters.size())
               {
                  throw UtInput::BadValue(aInput, "Number of ranges and clutters entries must match in clutter table.");
               }
               AddAltitudeEntry(altitude, ranges, clutters);
            }
            else // (mType == cSITE_SPECIFIC)
            {
               if (bearingSet.size() * rangeSet.size() != clutters.size())
               {
                  throw UtInput::BadValue(
                     aInput,
                     "Number of bearings and clutters entries must match in a site-specific clutter table.");
               }
               AddAltitudeEntry(altitude, ranges, bearings, clutters);
               bearings.clear();
               rangeSet.clear();
               bearingSet.clear();
            }

            ranges.clear();
            clutters.clear();
         }
         haveAltitude = true;

         // Read
         aInput.ReadValueOfType(altitude, UtInput::cLENGTH);
      }
      else if (command == "bearing")
      {
         // Check
         if (!haveAltitude)
         {
            throw UtInput::BadValue(aInput, "Altitude has not been defined.");
         }

         // site-specific maps only.
         if (mType == cGENERIC)
         {
            mType = cSITE_SPECIFIC;
         }

         // Read
         aInput.ReadValueOfType(bearing, UtInput::cANGLE);
         aInput.ValueInClosedRange(bearing, -UtMath::cPI, UtMath::cPI);
         if (bearingSet.find(bearing) == bearingSet.end())
         {
            bearings.push_back(bearing);
            bearingSet.insert(bearing);
         }
      }
      else if (command == "range")
      {
         // Check
         if (!haveAltitude)
         {
            throw UtInput::BadValue(aInput, "Altitude has not been defined.");
         }

         // Read
         double value(0.0);
         aInput.ReadValueOfType(value, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(value, 0.0);

         if (mType == cGENERIC)
         {
            ranges.push_back(value);
            if (!haveRange)
            {
               rangeSet.insert(value);
            }
         }
         else if (rangeSet.find(value) == rangeSet.end())
         {
            ranges.push_back(value);
            rangeSet.insert(value);
         }

         haveRange = true;
      }
      else if (command == "clutter")
      {
         // Check
         if (!haveAltitude)
         {
            throw UtInput::BadValue(aInput, "altitude has not been defined.");
         }

         // Read
         double value(0.0);
         aInput.ReadValueOfType(value, UtInput::cPOWER_DB);
         aInput.ValueGreater(value, 0.0);
         clutters.push_back(value);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Error reading clutters.");
      }
   }

   // ranges and clutters
   if (mType == cGENERIC)
   {
      if (ranges.size() != clutters.size())
      {
         throw UtInput::BadValue(aInput, "Number of ranges and clutters entries must match in clutter table.");
      }
      AddAltitudeEntry(altitude, ranges, clutters);
   }
   else // (mType == cSITE_SPECIFIC)
   {
      if (bearingSet.size() * rangeSet.size() != clutters.size())
      {
         throw UtInput::BadValue(aInput, "Number of bearings and clutters entries must match in a site-specific clutter table.");
      }
      AddAltitudeEntry(altitude, ranges, bearings, clutters);
      bearings.clear();
      rangeSet.clear();
      bearingSet.clear();
   }

   ranges.clear();
   clutters.clear();

   if (mAltitudes.size() < 2)
   {
      // Just copy the first entry
      mAltitudes.push_back(mAltitudes[0]);
   }

   return true;
}
// =================================================================================================
double WsfEM_SurfaceClutterTable::ComputeClutterPower(WsfEM_Interaction& aInteraction,
                                                      WsfEnvironment&    aEnvironment,
                                                      double             aProcessingFactor)
{
   // Return immediately if the altitude table is empty for some reason
   if (mAltitudes.empty())
   {
      return 0.0;
   }

   // It's easier to use msl because of the quirky way sensor_plot does its geometry calculations
   // This code had been using AGL, which has some advantages (no points underground!),
   // but threre is no easy way to make it consistent.
   double altitude = aInteraction.mTgtLoc.mAlt;

   // get range to target
   // double aRange = UtMeasurementUtil::GroundRange(aInteraction.mRcvrLoc.mLocWCS,
   //                                     aInteraction.mTgtLoc.mLocWCS);
   mReceiver.SetLocationWCS(aInteraction.mRcvrLoc.mLocWCS);
   mTarget.SetLocationWCS(aInteraction.mTgtLoc.mLocWCS);
   double locNED[3];
   mReceiver.GetRelativeLocationNED(&mTarget, locNED);
   double range = sqrt(locNED[0] * locNED[0] + locNED[1] * locNED[1]);

   // Find the index to use
   int    index_(0);
   double ratio_(0.0);
   if (altitude <= mAltitudes[0]->mAltitude)
   {
      index_ = 0;
      ratio_ = 0.0;
   }
   else if (altitude >= mAltitudes[mAltitudes.size() - 1]->mAltitude)
   {
      index_ = static_cast<int>(mAltitudes.size()) - 2;
      ratio_ = 1.0;
   }
   else
   {
      while (altitude > mAltitudes[index_]->mAltitude)
      {
         ++index_;
      }
      while (altitude < mAltitudes[index_]->mAltitude)
      {
         --index_;
      }
      ratio_ = (altitude - mAltitudes[index_]->mAltitude) /
               (mAltitudes[index_ + 1]->mAltitude - mAltitudes[index_]->mAltitude);
   }

   // Find the first interpolated clutter
   double bearing  = 0.0;
   double clutter1 = 0.0;
   mAltitudes[index_]->mRangeLookup.Lookup(mAltitudes[index_]->mRanges, range);
   if (mType == cSITE_SPECIFIC)
   {
      // find the true target bearing from true north.
      double targetVecNED[3];
      aInteraction.GetReceiver()->GetPlatform()->ConvertWCSVectorToNED(targetVecNED,
                                                                       aInteraction.mRcvrToTgt.mTrueUnitVecWCS);
      bearing = atan2(targetVecNED[1], targetVecNED[0]);
      mAltitudes[index_]->mBearingLookup.Lookup(mAltitudes[index_]->mBearings, bearing);
      mAltitudes[index_]->mRangeLookup.Lookup(mAltitudes[index_]->mRanges, range);
      clutter1 =
         TblEvaluate(mAltitudes[index_]->mClutters2, mAltitudes[index_]->mBearingLookup, mAltitudes[index_]->mRangeLookup);
   }
   else
   {
      mAltitudes[index_]->mRangeLookup.Lookup(mAltitudes[index_]->mRanges, range);
      clutter1 = TblEvaluate(mAltitudes[index_]->mClutters1, mAltitudes[index_]->mRangeLookup);
   }

   // Find the second interpolated clutter
   double clutter2 = 0.0;
   mAltitudes[index_ + 1]->mRangeLookup.Lookup(mAltitudes[index_ + 1]->mRanges, range);
   if (mType == cSITE_SPECIFIC)
   {
      mAltitudes[index_ + 1]->mBearingLookup.Lookup(mAltitudes[index_ + 1]->mBearings, bearing);
      mAltitudes[index_ + 1]->mRangeLookup.Lookup(mAltitudes[index_ + 1]->mRanges, range);
      clutter2 = TblEvaluate(mAltitudes[index_ + 1]->mClutters2,
                             mAltitudes[index_ + 1]->mBearingLookup,
                             mAltitudes[index_ + 1]->mRangeLookup);
   }
   else
   {
      clutter2 = TblEvaluate(mAltitudes[index_ + 1]->mClutters1, mAltitudes[index_ + 1]->mRangeLookup);
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Clutter table lookup-data:";
      out.AddNote() << "Altitude: " << altitude;
      out.AddNote() << "Range: " << range;
      out.AddNote() << "Clutter Power: " << 10.0 * log10(clutter1 + ratio_ * (clutter2 - clutter1)) << " dBW";
   }

   // Finally calculate the clutter value
   return (clutter1 + ratio_ * (clutter2 - clutter1)) * aProcessingFactor;
}

// =================================================================================================
//! The following is a convenience method to create a minimal
//! table that contains a constant value.
//!
//! @param aSimTime       [input] The constant fuel clutter.
/*
void WsfEM_SurfaceClutterTable::CreateConstantTable(double aValue)
{
   std::vector<double> rangeValues;
   rangeValues.push_back(-1.0);
   rangeValues.push_back(cHUGE_VALUE);

   std::vector<double> bearingValues;
   bearingValues.push_back(-180.0);
   bearingValues.push_back(180.0);

   std::vector<double> clutters;
   clutters.push_back(aValue);
   clutters.push_back(aValue);

   AddAltitudeEntry(-cHUGE_VALUE, rangeValues, bearingValues, clutters);
   AddAltitudeEntry( cHUGE_VALUE, rangeValues, bearingValues, clutters);
}
*/

// =================================================================================================
//! Add an entry to the clutter table.
//!
//! @param aAltitude  [input] The altitude.
//! @param aRanges    [input] Range values for this altitude.
//! @param aClutters  [input] Clutters for this altitude.
void WsfEM_SurfaceClutterTable::AddAltitudeEntry(double               aAltitude,
                                                 std::vector<double>& aRanges,
                                                 std::vector<double>& aBearings,
                                                 std::vector<double>& aClutters)
{
   AltitudeEntry* altEntryPtr = new AltitudeEntry();
   altEntryPtr->mAltitude     = aAltitude;
   altEntryPtr->mRanges.SetValues(aRanges);
   altEntryPtr->mBearings.SetValues(aBearings);
   size_t numRanges   = altEntryPtr->mRanges.GetSize();
   size_t numBearings = altEntryPtr->mBearings.GetSize();

   altEntryPtr->mClutters2.Resize((int)numBearings, (int)numRanges);
   // insert based on range and bearing; convention is that bearing is the outer loop,
   // so we should have in the table:
   // (b1, r1, c)
   // (b1, r2, c)
   // ...
   // (b2, r1, c) ... etc.
   for (size_t i = 0; i < numBearings; ++i)
   {
      for (size_t j = 0; j < numRanges; ++j)
      {
         altEntryPtr->mClutters2.Set(aClutters[i * numRanges + j], i, j);
      }
   }
   mAltitudes.push_back(altEntryPtr);
}

// =================================================================================================
//! Add an entry to the clutter table.
//!
//! @param aAltitude [input] The altitude.
//! @param aRanges   [input] Range values for this altitude.
//! @param aClutters [input] Clutters for this altitude.
void WsfEM_SurfaceClutterTable::AddAltitudeEntry(double aAltitude, std::vector<double>& aRanges, std::vector<double>& aClutters)
{
   AltitudeEntry* altEntryPtr = new AltitudeEntry();
   altEntryPtr->mAltitude     = aAltitude;
   altEntryPtr->mRanges.SetValues(aRanges);
   size_t numRanges = altEntryPtr->mRanges.GetSize();

   altEntryPtr->mClutters1.Resize(numRanges);
   for (size_t j = 0; j < numRanges; ++j)
   {
      altEntryPtr->mClutters1.Set(aClutters[j], j);
   }
   mAltitudes.push_back(altEntryPtr);
}
