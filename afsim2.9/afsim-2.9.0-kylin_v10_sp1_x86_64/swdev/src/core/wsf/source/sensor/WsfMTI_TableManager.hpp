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

#ifndef WSFMTI_TABLEMANAGER_HPP
#define WSFMTI_TABLEMANAGER_HPP

#include "wsf_export.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "TblLookup.hpp"
#include "UtMath.hpp"

//! For MTI there can be a specific table for each possible mode,
//! i.e., staggered or fixed PRI, dual or single.  In WSF, you configure
//! a mode simply by name. In that element, you can dictate a MTI table.  It
//! already supports in-line tables.  This class allows specifying
//! an external (CSV) table.  The original table is in dB, and the note from
//! WsfRadarSensor is "The adjustment table is in dB, and the interpolation is in dB-space."
//! So, no conversions in this class are required.
class WSF_EXPORT WsfMTI_TableManager
{
public:
   //! A nested class for the MTI adjustment table.  This was extracted from WsfRadarSensor
   class WSF_EXPORT MTI_AdjustmentTable
   {
   public:
      MTI_AdjustmentTable();
      virtual ~MTI_AdjustmentTable() = default;

      virtual double ComputeAdjustmentFactor(double aClosingSpeed, double aFrequency);

      // For External Services.
      bool                IsDopplerFrequency() const { return mIV_IsDopplerFrequency; }
      TblIndVarU<double>& GetSpeedFrequencyTable() { return mSpeedFrequencyTable; }
      TblDepVar1<double>& GetAdjustmentTable() { return mAdjustmentTable; }

      //! If read from external table, the independent variable is speed.
      void SetDopplerFrequency(bool aIsDopplerFrequency) { mIV_IsDopplerFrequency = aIsDopplerFrequency; }

   protected:
      //! 'true' if the independent variable is closing speed, 'false' if it is Doppler frequency
      bool               mIV_IsDopplerFrequency;
      TblIndVarU<double> mSpeedFrequencyTable; // closing speed or Doppler frequency
      TblDepVar1<double> mAdjustmentTable;
   };

   //! A version that uses the given frequency to look up a table that will look up
   //! using speed.
   class MTI_AdjustmentTableCompound : public MTI_AdjustmentTable
   {
   public:
      struct FreqTablePair
      {
         FreqTablePair()
            : mFreqencyHz(0.0)
            , mMTI_TablePtr(nullptr)
         {
         }

         double               mFreqencyHz;
         MTI_AdjustmentTable* mMTI_TablePtr;
      };

      MTI_AdjustmentTableCompound();
      ~MTI_AdjustmentTableCompound() override;

      void AddTable(double aFrequencyHz, std::vector<double>& aSpeedValues, std::vector<double>& aAdjustmentValues);

      WsfMTI_TableManager::MTI_AdjustmentTable* GetTable(double aFrequencyHz);

      double ComputeAdjustmentFactor(double aClosingSpeed, double aFrequency) override;

   protected:
      std::vector<FreqTablePair> mFreqTablePairList;
   };

   // Reads a MTI table, where the data is separated by commas
   // @param aFileName The name of the file to be read
   // @param aAdjustmentTable The table of data to be read into.
   // @return If the given file was read without incidence
   bool ReadCsv(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable);

   // Reads a MTI table, where the data is separated by commas.  This
   // is for a different table format, where mulitple independent columns map to
   // the same last dependent (attenuation) column.  Each independent
   // column has a specific frequency, which is indicated in its header.
   // @param aFileName The name of the file to be read
   MTI_AdjustmentTable* ReadCsvCompound(const std::string& aFileName);

   // Reads a MTI table, where the data is binary AND created by WriteBinary
   // @param aFileName The name of the file to be read.
   // @param aAdjustmentTable The table of data to be read into.
   // @return If the given file was read without incidence
   bool ReadBinary(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable);

   // Writes a table in memory to a binary file
   // @param aFileName The name of the file to be written
   // @param aAdjustmentTable The table of data to be written.
   void WriteBinary(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable);

   // Writes a table in memory to a CSV text file, which could be compared to the original
   // @param aFileName The name of the file to be written
   // @param aAdjustmentTable The table of data to be written.
   void WriteCsv(const std::string& aFileName, MTI_AdjustmentTable& aAdjustmentTable);

   //! Accesses the singleton
   //! @return The singleton of this class
   static WsfMTI_TableManager& GetManager();

   //! Enables debugging messages.
   //! @param aTruth  True to turn on, false oterwise
   void SetDebug(bool aTruth) { mDebug = aTruth; }
   //! A data structure used for parsing the compound tables.

protected:
   struct FreqSpeedListPair
   {
      double mFrequencyHz;
      // WsfMTI_TableManager::MTI_AdjustmentTable* mAdjustmentTable;
      std::vector<double> speedValues;
      // std::vector<double> adjustmentValues;
   };

private:
   //! Singleton, prevent external construction
   WsfMTI_TableManager();
   WsfMTI_TableManager(const WsfMTI_TableManager& aSrc);
   virtual ~WsfMTI_TableManager();

   WsfMTI_TableManager operator=(const WsfMTI_TableManager& aSrc) = delete;

   bool mDebug;

   static WsfMTI_TableManager* sTheManager;
};
#endif
