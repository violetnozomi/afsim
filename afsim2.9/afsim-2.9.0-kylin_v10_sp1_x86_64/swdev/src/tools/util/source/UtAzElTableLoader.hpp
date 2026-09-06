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

#ifndef UTAZELTABLELOADER_HPP
#define UTAZELTABLELOADER_HPP

#include "ut_export.h"

#include <map>
#include <string>

class UtAzElTable;
class UtInput;

//! A utility class for loading a UtAzElTable from a UtInput object.
//!
//! This could have been put directly in UtAzElTable, but it was decided to
//! keep it separate as the UtAzElTable is a nice compact class that has
//! some general utility.  This class, on the other hand, could get messy if
//! the ability to read different signature tables is added.

class UT_EXPORT UtAzElTableLoader
{
public:
   UtAzElTableLoader()          = default;
   virtual ~UtAzElTableLoader() = default;

   static bool ProcessTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits);

   static bool ProcessTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits, std::string& aFileName);

   // PLT signature table format
   // Polarization -> Frequency -> AzEl Table
   using PolFreqTableMapType = std::map<std::string, std::map<double, UtAzElTable*>>;

   static bool ProcessTable(UtInput& aInput, PolFreqTableMapType& aTableMap, std::string& aTableUnits);

   static bool ProcessTable(UtInput& aInput, PolFreqTableMapType& aTableMap, std::string& aTableUnits, std::string& aFileName);

private:
   static void LoadConstantTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits);

   static void LoadInlineTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits);

   static void LoadTableFromFile(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits, std::string& aFileName);

   static void LoadTableFromPLT_File(UtInput& aInput, PolFreqTableMapType& aTableMap, std::string& aFileName);
};

#endif
