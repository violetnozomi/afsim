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

#include "WsfMTI_TableManager.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <string>

#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtTypeInfo.hpp"
#include "UtWallClock.hpp"

namespace
{

static const int    cINIT_VECTOR_SIZE  = 1000;
static const double cINIT_DATA_EPSILON = 1E-3;

// void AssertLocal(bool        aTruth,
//                  std::string aMessage)
//{
//    if (!aTruth)
//    {
//       ut::log::error() << "WsfMTI_TableManager:AssertLocal: " << aMessage << endl;
//       assert(false);
//    }
// }
} // namespace

//! Static Definitions
WsfMTI_TableManager* WsfMTI_TableManager::sTheManager = nullptr;

//! Static
WsfMTI_TableManager& WsfMTI_TableManager::GetManager()
{
   if (sTheManager == nullptr)
   {
      sTheManager = new WsfMTI_TableManager;
   }
   return *sTheManager;
}

// =================================================================================================
WsfMTI_TableManager::WsfMTI_TableManager()
   : mDebug(false)
{
}

// =================================================================================================
WsfMTI_TableManager::WsfMTI_TableManager(const WsfMTI_TableManager& aSrc)
   : mDebug(aSrc.mDebug)
{
}

// =================================================================================================
// virtual
WsfMTI_TableManager::~WsfMTI_TableManager() {}

// =================================================================================================
bool WsfMTI_TableManager::ReadCsv(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable)
{
   std::ifstream ifs(aFileName);
   if (!ifs.is_open() || ifs.fail())
   {
      auto out = ut::log::error() << "MTI_Table::ReadCsv: Unable to open MTI table file.";
      out.AddNote() << "File: " << aFileName;
      throw UtException("MTI_TABLE::ReadCsv: Unable to open MTI table file: " + aFileName);
   }

   aAdjustmentTable.SetDopplerFrequency(false);

   if (mDebug)
   {
      auto out = ut::log::debug() << "MTI_Table::ReadCsv: Parsing MTI table file.";
      out.AddNote() << "File: " << aFileName;
   }

   UtWallClock runClock;
   runClock.ResetClock();
   double startTime = runClock.GetClock();

   std::string line;
   int         row = 1;

   // 1 lines of comments (header)
   for (int i = 0; i < 1; ++i)
   {
      getline(ifs, line);
      row++; // For debugging
   }
   // GetLine pulls the line off and leaves the
   // stream at the beginning at the next line.  So the
   // next redirection below pulls the data off the new line

   const static char delim = ',';

   std::vector<double> speedValues;
   std::vector<double> adjustmentValues;

   double closingRate = 0.0;
   double adjustment  = 0.0;

   // Create an iterable array to access the structure
   double* dataPtr[] = {&closingRate, &adjustment};

   try
   {
      std::string cell;
      while (std::getline(ifs, line))
      {
         std::stringstream lineStream(line);

         int i = 0;
         while (std::getline(lineStream, cell, delim))
         {
            *dataPtr[i++] = atof(cell.c_str());
         }

         if (i == 2)
         {
            if ((!speedValues.empty()) && (closingRate <= speedValues.back()))
            {
               ut::log::error() << "MTI_Table::ReadCsv MTI_AdjustmentTable2: Values must be monotonically ascending.";
               throw UtException("MTI_Table::ReadCsv MTI_AdjustmentTable2: Values must be monotonically ascending.");
            }

            speedValues.push_back(closingRate);
            adjustmentValues.push_back(adjustment);
         }
         else
         {
            auto out = ut::log::error() << "MTI_Table::ReadCsv: Failed reading row.";
            out.AddNote() << "Row: " << row;
            out.AddNote() << "Values in Row: " << i;
            throw UtException("MTI_Table::- reading row failed!  Numbers fields");
         }

         // Progress indication
         if (mDebug && (row % 100000 == 0))
         {
            auto out = ut::log::debug() << "MTI_Table::ReadCsv: Parsing row.";
            out.AddNote() << "Row: " << row;
         }
         row++;
      }
   }
   catch (std::exception&)
   {
      auto out = ut::log::error() << "MTI_Table::ReadCsv: Caught exception!!!";
      out.AddNote() << "Row: " << row;
      throw;
   }

   ifs.close();

   aAdjustmentTable.GetSpeedFrequencyTable().SetValues(speedValues);
   aAdjustmentTable.GetAdjustmentTable().SetValues(adjustmentValues);

   if (mDebug)
   {
      double rt  = runClock.GetClock() - startTime;
      auto   out = ut::log::debug() << "MTI_Table::ReadCsv: Finished parsing file.";
      out.AddNote() << "Rows Read(including header): " << row;
      out.AddNote() << "Run Time(sec): " << rt;
   }
   return true;
}

//! This table-version has mutiple columns mapped to a single function outputs/attenuations.
WsfMTI_TableManager::MTI_AdjustmentTable* WsfMTI_TableManager::ReadCsvCompound(const std::string& aFileName)
{
   std::ifstream ifs(aFileName);
   if (!ifs.is_open() || ifs.fail())
   {
      auto out = ut::log::error() << "MTI_Table::ReadCsvCompound: Unable to open MTI table file.";
      out.AddNote() << "File: " << aFileName;
      throw UtException("MTI_Table::ReadCsvCompound: Unable to open MTI table file: " + aFileName);
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "MTI_Table::ReadCsvCompound: Parsing file.";
      out.AddNote() << "File: " << aFileName;
   }

   UtWallClock runClock;
   runClock.ResetClock();
   double startTime = runClock.GetClock();

   std::string line;
   int         row = 1;

   std::vector<FreqSpeedListPair> pairList;

   const static char delim = ',';

   double lastFreq = 0;
   for (int i = 0; i < 1; ++i)
   {
      getline(ifs, line);

      std::stringstream lineStream(line);
      std::string       cell;
      double            units = 1E6; // mhz
      while (std::getline(lineStream, cell, delim))
      {
         if (cell.substr(0, 9) != "ATTN (db)")
         {
            FreqSpeedListPair freqSpeeds;
            try
            {
               freqSpeeds.mFrequencyHz = atof(cell.substr(11, std::string::npos).c_str());
            }
            catch (std::exception& e)
            {
               auto out = ut::log::error() << "Exception thrown in MTI_Table::ReadCsvCompound. Error parsing table.";
               out.AddNote() << "Type: " << ut::TypeNameOf(e);
               out.AddNote() << "What: " << e.what();
               throw;
            }

            freqSpeeds.mFrequencyHz *= units;
            if (freqSpeeds.mFrequencyHz < lastFreq)
            {
               ut::log::error()
                  << "MTI_TABLE::ReadCsvCompound: Frequencies must be monotonically ascending (in columns).";
               throw UtException(
                  "MTI_TABLE::ReadCsvCompound: Frequencies must be monotonically ascending (in columns)!");
            }
            lastFreq = freqSpeeds.mFrequencyHz;
            pairList.push_back(freqSpeeds);
         }
      }
      row++; // For debugging
   }

   // GetLine pulls the line off and leaves the
   // stream at the beginning at the next line.  So the
   // next redirection below pulls the data off the new line

   std::vector<double> adjustmentValues;

   size_t              numCol = pairList.size() + 1;
   std::vector<double> dataPtr(numCol);

   try
   {
      std::string cell;
      while (std::getline(ifs, line))
      {
         std::stringstream lineStream(line);

         size_t i = 0;
         while (std::getline(lineStream, cell, delim))
         {
            dataPtr[i++] = atof(cell.c_str());
         }

         if (i == numCol)
         {
            for (unsigned int j = 0; j < pairList.size(); ++j)
            {
               double               closingRate = dataPtr[j];
               std::vector<double>& speedValues = pairList[j].speedValues;
               if ((!speedValues.empty()) && (closingRate <= speedValues.back()))
               {
                  ut::log::error()
                     << "MTI_TABLE::ReadCsvCompound MTI_AdjustmentTable2: Values must be monotonically ascending.";
                  throw UtException("MTI_TABLE::ReadCsv MTI_AdjustmentTable2: Values must be monotonically ascending!");
               }

               speedValues.push_back(closingRate);
            }
            double adjustment = dataPtr[numCol - 1];
            adjustmentValues.push_back(adjustment);
         }
         else
         {
            auto out = ut::log::error() << "MTI_TABLE::ReadCsvCompound: Failed reading row.";
            out.AddNote() << "Row: " << row;
            out.AddNote() << "Values in Row: " << i;
            throw UtException("MTI_TABLE::ReadCsvCompound reading row failed!");
         }

         // Progress indication
         if (mDebug && (row % 100000 == 0))
         {
            auto out = ut::log::debug() << "MTI_TABLE::ReadCsvCompound: Parsing row.";
            out.AddNote() << "Row: " << row;
         }
         row++;
      }
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception thrown in MTI_TABLE::ReadCsvCompound.";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      out.AddNote() << "Row: " << row;
      throw;
   }

   ifs.close();

   WsfMTI_TableManager::MTI_AdjustmentTableCompound* compoundTablePtr =
      new WsfMTI_TableManager::MTI_AdjustmentTableCompound();

   for (unsigned int j = 0; j < pairList.size(); ++j)
   {
      if (pairList[j].speedValues.size() != adjustmentValues.size())
      {
         ut::log::error()
            << "MTI_TABLE::ReadCsvCompound MTI_AdjustmentTable2: Number of speed values not same as adjustment values.";
         delete compoundTablePtr;
         throw UtException(
            "MTI_TABLE::ReadCsv MTI_AdjustmentTable2: Number of speed values not same as adjustment values!");
      }
      compoundTablePtr->AddTable(pairList[j].mFrequencyHz, pairList[j].speedValues, adjustmentValues);
   }

   if (mDebug)
   {
      double rt  = runClock.GetClock() - startTime;
      auto   out = ut::log::debug() << "MTI_TABLE::ReadCsvCompound: Finished parsing file.";
      out.AddNote() << "Independent Columns Read: " << pairList.size();
      out.AddNote() << "Rows Read (including header): " << row;
      out.AddNote() << "Run Time (sec): " << rt;
   }
   return compoundTablePtr;
}

// =================================================================================================
void WsfMTI_TableManager::WriteCsv(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable)
{
   std::ofstream ofs;
   ofs.open(aFileName, std::ios::out);

   if (!ofs.is_open())
   {
      auto out = ut::log::error() << "MTI_TABLE::WriteCsv: Unable to open file.";
      out.AddNote() << "File: " << aFileName;
      throw UtException("MTI_TABLE::WriteCsv: Unable to open file: " + aFileName);
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "MTI_TABLE::WriteCsv: Writing file.";
      out.AddNote() << "File: " << aFileName;
   }

   UtWallClock runClock;
   runClock.ResetClock();
   double startTime = runClock.GetClock();

   TblIndVarU<double>& speedTable      = aAdjustmentTable.GetSpeedFrequencyTable();
   TblDepVar1<double>& adjustmentTable = aAdjustmentTable.GetAdjustmentTable();

   std::vector<double>& speedAxis      = speedTable.GetValues();
   std::vector<double>& adjustmentAxis = adjustmentTable.GetValues();

   //--------------------------
   ofs << "RNRT (m/s),ATTN (dB)," << std::endl;

   size_t n = adjustmentAxis.size();

   for (size_t i = 0; i < n; ++i)
   {
      ofs << speedAxis[i] << ",";
      ofs << adjustmentAxis[i] << ",";
      ofs << std::endl;
   }
   ofs.close();

   if (mDebug)
   {
      double rt  = runClock.GetClock() - startTime;
      auto   out = ut::log::debug() << "MTI_TABLE::WriteCsv: Finished writing file.";
      out.AddNote() << "Run Time(sec): " << rt;
   }
}

// =================================================================================================
void WsfMTI_TableManager::WriteBinary(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable)
{
   std::ofstream ofs;
   ofs.open(aFileName, std::ios::out | std::ios::binary);

   if (!ofs.is_open())
   {
      auto out = ut::log::error() << "MTI_TABLE::WriteBinary: Unable to open file.";
      out.AddNote() << "File: " << aFileName;
      throw UtException("MTI_TABLE::WriteBinary: Unable to open file: " + aFileName);
   }

   TblIndVarU<double>& speedTable      = aAdjustmentTable.GetSpeedFrequencyTable();
   TblDepVar1<double>& adjustmentTable = aAdjustmentTable.GetAdjustmentTable();

   std::vector<double>& speedAxis      = speedTable.GetValues();
   std::vector<double>& adjustmentAxis = adjustmentTable.GetValues();

   size_t n = adjustmentAxis.size();

   ofs.write((char*)&n, sizeof(int));

   for (size_t i = 0; i < n; ++i)
   {
      ofs.write((char*)&speedAxis[i], sizeof(double));
      if (!ofs)
      {
         ut::log::error() << "MTI_TABLE::WriteBinary: Writing data speedAxis.";
         throw UtException("MTI_TABLE::WriteBinary: Writing data speedAxis");
      }
   }
   for (size_t i = 0; i < n; ++i)
   {
      ofs.write((char*)&adjustmentAxis[i], sizeof(double));
      if (!ofs)
      {
         ut::log::error() << "MTI_TABLE::WriteBinary: Writing data adjustmentAxis.";
         throw UtException("MTI_TABLE::WriteBinary: Writing data adjustmentAxis");
      }
   }
   ofs.close();
}

// =================================================================================================
bool WsfMTI_TableManager::ReadBinary(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable)
{
   std::ifstream ifs;
   ifs.open(aFileName, std::ios::in | std::ios::binary);
   if (!ifs.is_open())
   {
      auto out = ut::log::error() << "MTI_TABLE::ReadBinary: Unable to open file.";
      out.AddNote() << "File: " << aFileName;
      throw UtException("MTI_TABLE::ReadBinary: Unable to open file: " + aFileName);
      return false;
   }

   UtWallClock runClock;
   runClock.ResetClock();
   double startTime = runClock.GetClock();

   unsigned int n = 0;

   ifs.read((char*)&n, sizeof(int));

   //-----------------------------------
   std::vector<double> speedAxis(n);
   std::vector<double> adjustmentAxis(n);

   for (unsigned i = 0; i < n; ++i)
   {
      ifs.read((char*)&speedAxis[i], sizeof(double));
      if (!ifs)
      {
         ut::log::error() << "MTI_TABLE::ReadBinary: Reading data speedAxis.";
         ifs.close();
         throw UtException("MTI_TABLE::ReadBinary: Reading data speedAxis");
         return false;
      }
   }

   for (unsigned i = 0; i < n; ++i)
   {
      ifs.read((char*)&adjustmentAxis[i], sizeof(double));
      if (!ifs)
      {
         ut::log::error() << "MTI_TABLE::ReadBinary: Reading data adjustmentAxis.";
         ifs.close();
         throw UtException("MTI_TABLE::ReadBinary: Reading data adjustmentAxis");
         return false;
      }
   }

   ifs.close();

   aAdjustmentTable.GetSpeedFrequencyTable().SetValues(speedAxis);
   aAdjustmentTable.GetAdjustmentTable().SetValues(adjustmentAxis);

   if (mDebug)
   {
      double rt  = runClock.GetClock() - startTime;
      auto   out = ut::log::debug() << "MTI_TABLE::ReadBinary: Finished parsing file.";
      out.AddNote() << "Run Time(sec): " << rt;
   }
   return true;
}

// =================================================================================================
// Nested class MTI_AdjustmentTable
// =================================================================================================

// =================================================================================================
//! Compute the Signal-to-noise ratio adjustment due to closing speed.
//! @param aClosingSpeed The closing speed (possibly filtered to exclude ownship). (m/s).
//! Positive values are closing, negative values are receeding.
//! @param aFrequency The frequency of the radar signal (Hz). Needed if the table uses doppler frequency
//! instead of closing speed.
//! @returns The adjustment factor to the received signal (linear, not dB).
//! virtual
double WsfMTI_TableManager::MTI_AdjustmentTable::ComputeAdjustmentFactor(double aClosingSpeed, double aFrequency)
{
   double closingSpeed = aClosingSpeed;
   if (mSpeedFrequencyTable.Get(0) >= 0.0)
   {
      // No negative values in the table, so it is assumed symmetric.
      closingSpeed = fabs(closingSpeed);
   }
   double closingValue = closingSpeed;
   if (mIV_IsDopplerFrequency)
   {
      // Compute the Doppler frequency shift (Hz)
      closingValue = 2.0 * closingSpeed * aFrequency / UtMath::cLIGHT_SPEED;
   }
   TblLookupLU<double> luClosingValue;
   luClosingValue.Lookup(mSpeedFrequencyTable, closingValue);

   // The adjustment table is in dB, and the interpolation is in dB-space.
   double adjustmentDB = TblEvaluate(mAdjustmentTable, luClosingValue);
   // The return value is linear.
   return UtMath::DB_ToLinear(adjustmentDB);
}

WsfMTI_TableManager::MTI_AdjustmentTable::MTI_AdjustmentTable()
   : mIV_IsDopplerFrequency(false)
{
}

WsfMTI_TableManager::MTI_AdjustmentTableCompound::MTI_AdjustmentTableCompound()
   : mFreqTablePairList()
{
}

WsfMTI_TableManager::MTI_AdjustmentTableCompound::~MTI_AdjustmentTableCompound()
{
   for (unsigned int i = 0; i < mFreqTablePairList.size(); ++i)
   {
      delete mFreqTablePairList[i].mMTI_TablePtr;
   }
}

void WsfMTI_TableManager::MTI_AdjustmentTableCompound::AddTable(double               aFrequencyHz,
                                                                std::vector<double>& aSpeedValues,
                                                                std::vector<double>& aAdjustmentValues)
{
   WsfMTI_TableManager::MTI_AdjustmentTable* adjustmentTablePtr = new WsfMTI_TableManager::MTI_AdjustmentTable;
   adjustmentTablePtr->SetDopplerFrequency(false);

   // Internally it makes a copy of the Values.
   adjustmentTablePtr->GetSpeedFrequencyTable().SetValues(aSpeedValues);
   adjustmentTablePtr->GetAdjustmentTable().SetValues(aAdjustmentValues);

   FreqTablePair pair;
   pair.mFreqencyHz   = aFrequencyHz;
   pair.mMTI_TablePtr = adjustmentTablePtr;

   mFreqTablePairList.push_back(pair);
}

WsfMTI_TableManager::MTI_AdjustmentTable* WsfMTI_TableManager::MTI_AdjustmentTableCompound::GetTable(double aFrequencyHz)
{
   assert(mFreqTablePairList.size() > 0);

   // Find the table for the given frequency.
   // Default to the last table
   size_t useIndex = mFreqTablePairList.size() - 1;
   for (size_t i = 0; i < mFreqTablePairList.size(); ++i)
   {
      if (mFreqTablePairList[i].mFreqencyHz > aFrequencyHz)
      {
         useIndex = i;
         break;
      }
   }

   WsfMTI_TableManager::MTI_AdjustmentTable* mti_AdjustmentTablePtr = mFreqTablePairList[useIndex].mMTI_TablePtr;
   return mti_AdjustmentTablePtr;
}

//! virtual
double WsfMTI_TableManager::MTI_AdjustmentTableCompound::ComputeAdjustmentFactor(double aClosingSpeed, double aFrequencyHz)
{
   assert(mFreqTablePairList.size() > 0);

   WsfMTI_TableManager::MTI_AdjustmentTable* mti_AdjustmentTablePtr = GetTable(aFrequencyHz);

   if (mti_AdjustmentTablePtr != nullptr)
   {
      double a = mti_AdjustmentTablePtr->ComputeAdjustmentFactor(aClosingSpeed, aFrequencyHz);
      if (UtMath::SafeLinearToDB(a) != 0.0)
      {
         mti_AdjustmentTablePtr = GetTable(aFrequencyHz);
         // a = mti_AdjustmentTablePtr->ComputeAdjustmentFactor(aClosingSpeed, aFrequencyHz);
      }
      return mti_AdjustmentTablePtr->ComputeAdjustmentFactor(aClosingSpeed, aFrequencyHz);
   }
   else
   {
      auto out = ut::log::error() << "MTI_AdjustmentTableCompound: No MTI adjustment table was found for frequency.";
      out.AddNote() << "Frequency: " << aFrequencyHz;
      return 1.0;
   }
}
