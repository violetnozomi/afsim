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

#if defined(_MSC_VER)
//  warning C4503: '.....' : decorated name length exceeded, name was truncated
#pragma warning(disable : 4503)
#endif

#include "UtAzElTableLoader.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "UtAzElTable.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"

//! Determine if the current input command is one that loads a table.
//!
//!  @param aInput      The input stream.
//!  @param aTablePtr   [output] If a table is created then this will be a
//!                     pointer to the table (only if the function return value is 'true')
//!  @param aTableUnits On input this should be the default units for the dependent
//!                     variable data values in the table ("m^2", "dBsm", etc.).
//!                     On output this will be the actual units requested.
//!
//!  @returns 'true' if the current command loads a table or 'false' if not.
//!  @throws UtInput::ExceptionBase If an error occurred while processing the table definition.
//!  @note If a table was read from a file, the file name logging callback on the input object will be invoked.
bool UtAzElTableLoader::ProcessTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits)
{
   std::string fileName;
   bool        myCommand = ProcessTable(aInput, aTablePtr, aTableUnits, fileName);
   // If a table was read from a file, invoke the file open callback (typically used for file name logging)
   if (myCommand && (!fileName.empty()))
   {
      aInput.DoFileOpenCallback(fileName);
   }
   return myCommand;
}

//! Determine if the current input command is one that loads a table.
//!
//!  @param aInput      The input stream.
//!  @param aTablePtr   [output] If a table is created then this will be a
//!                     pointer to the table (only if the function return value is 'true')
//!  @param aTableUnits On input this should be the default units for the dependent
//!                     variable data values in the table ("m^2", "dBsm", etc.).
//!                     On output this will be the actual units requested.
//!  @param aFileName   The name of the file from which the table was loaded. This will be an
//!                     empty string if the table was not loaded from a file.
//!
//!  @returns 'true' if the current command loads a table or 'false' if not.
//!  @throws UtInput::ExceptionBase If an error occurred while processing the table definition.
bool UtAzElTableLoader::ProcessTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits, std::string& aFileName)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   aTablePtr = nullptr;
   aFileName = "";
   if (command == "inline_table")
   {
      LoadInlineTable(aInput, aTablePtr, aTableUnits);
   }
   else if (command == "constant")
   {
      LoadConstantTable(aInput, aTablePtr, aTableUnits);
   }
   else if ((command == "file") || (command == "salram_file")) // NO_DOC | DEPRECATED
   {
      LoadTableFromFile(aInput, aTablePtr, aTableUnits, aFileName);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Determine if the current input command is one that loads a table.
//!
//!  @param aInput      The input stream.
//!  @param aTableMap   [output] If a table is created then this will be a
//!                     reference to the map of tables (only if the function return value is 'true')
//!  @param aTableUnits On input this should be the default units for the dependent
//!                     variable data values in the table ("m^2", "dBsm", etc.).
//!                     On output this will be the actual units requested.
//!
//!  @returns 'true' if the current command loads a table or 'false' if not.
//!  @throws UtInput::ExceptionBase If an error occurred while processing the table definition.
//!  @note If a table was read from a file, the file name logging callback on the input object will be invoked.
bool UtAzElTableLoader::ProcessTable(UtInput& aInput, PolFreqTableMapType& aTableMap, std::string& aTableUnits)
{
   std::string fileName;
   bool        myCommand = ProcessTable(aInput, aTableMap, aTableUnits, fileName);
   // If a table was read from a file, invoke the file open callback (typically used for file name logging)
   if (myCommand && (!fileName.empty()))
   {
      aInput.DoFileOpenCallback(fileName);
   }
   return myCommand;
}

//! Determine if the current input command is one that loads a table.
//!
//!  @param aInput      The input stream.
//!  @param aTableMap   [output] If a table is created then this will be a
//!                     reference to the map of tables (only if the function return value is 'true')
//!  @param aTableUnits On input this should be the default units for the dependent
//!                     variable data values in the table ("m^2", "dBsm", etc.).
//!                     On output this will be the actual units requested.
//!  @param aFileName   The name of the file from which the table was loaded. This will be an
//!                     empty string if the table was not loaded from a file.
//!
//!  @returns 'true' if the current command loads a table or 'false' if not.
//!  @throws UtInput::ExceptionBase If an error occurred while processing the table definition.
bool UtAzElTableLoader::ProcessTable(UtInput&             aInput,
                                     PolFreqTableMapType& aTableMap,
                                     std::string&         aTableUnits,
                                     std::string&         aFileName)
{
   bool myCommand = true;
   aTableMap.clear();
   std::string command(aInput.GetCommand());

   if (command == "plt_file")
   {
      aTableUnits = "dBsm";
      LoadTableFromPLT_File(aInput, aTableMap, aFileName);
   }
   else
   {
      UtAzElTable* tablePtr;
      myCommand = ProcessTable(aInput, tablePtr, aTableUnits, aFileName);
      if (myCommand && tablePtr != nullptr)
      {
         aTableMap[""][0] = tablePtr;
      }
   }
   return myCommand;
}

// private
void UtAzElTableLoader::LoadConstantTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits)
{
   float value;
   aInput.ReadValue(value);
   // Don't read the table units if none are expected
   if (aTableUnits != "dummy")
   {
      aInput.ReadValue(aTableUnits);
   }
   aTablePtr = new UtAzElTable();
   aTablePtr->CreateConstantTable(value);
}

// private
void UtAzElTableLoader::LoadInlineTable(UtInput& aInput, UtAzElTable*& aTablePtr, std::string& aTableUnits)
{
   int azCount;
   int elCount;

   aInput.ReadValue(aTableUnits);
   aInput.ReadValue(azCount);
   aInput.ReadValue(elCount);
   aInput.ValueGreaterOrEqual(azCount, 2);
   aInput.ValueGreaterOrEqual(elCount, 2);
   auto tablePtr = ut::make_unique<UtAzElTable>();
   tablePtr->mDataValues.Resize(azCount, elCount);

   std::vector<float> azValues;
   std::vector<float> elValues;
   float              azValue;
   float              elValue;
   float              sigValue;
   int                azIndex;
   int                elIndex;

   // Read the elevations...

   for (elIndex = 0; elIndex < elCount; ++elIndex)
   {
      aInput.ReadValue(elValue);
      elValues.push_back(static_cast<float>(elValue * UtMath::cRAD_PER_DEG));
   }

   // Read the values for each azimuth...

   for (azIndex = 0; azIndex < azCount; ++azIndex)
   {
      aInput.ReadValue(azValue);
      azValues.push_back(static_cast<float>(azValue * UtMath::cRAD_PER_DEG));
      for (elIndex = 0; elIndex < elCount; ++elIndex)
      {
         aInput.ReadValue(sigValue);
         tablePtr->mDataValues.Set(sigValue, azIndex, elIndex);
      }
   }

   // Make sure we have the 'end_inline_table' string
   std::string command;
   aInput.ReadCommand(command);
   if (command == "end_inline_table")
   {
      tablePtr->mAzValues.SetValues(azValues);
      tablePtr->mElValues.SetValues(elValues);
      if (!tablePtr->Validate(azCount, elCount))
      {
         throw UtInput::BadValue(aInput, "Inline table validation error");
      }
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }

   // Table is good... take ownership of the table object from the unique_ptr

   aTablePtr = tablePtr.get();
   tablePtr.release();
}

// private
void UtAzElTableLoader::LoadTableFromFile(UtInput&      aInput,
                                          UtAzElTable*& aTablePtr,
                                          std::string&  aTableUnits,
                                          std::string&  aFileName)
{
   aTablePtr = nullptr;
   aInput.ReadValueQuoted(aFileName);
   aFileName = aInput.LocateFile(aFileName);

   // Read in required units <units>' specified after the file filename.

   std::string units;
   aInput.ReadValue(units);
   if (units == "units")
   {
      aInput.ReadValue(aTableUnits);
   }
   else
   {
      throw UtInput::BadValue(aInput, "units input not specified for az-el file " + aFileName);
   }

   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtInput::BadValue(aInput, "Unable to open file " + aFileName);
   }

   char line[2048];

   // Skip the first three lines...
   for (int i = 0; i < 3; ++i)
   {
      if (!ifs.getline(line, sizeof(line)))
      {
         throw UtInput::BadValue(aInput, "Error skipping first three lines in file " + aFileName);
      }
   }

   auto readAzElCounts = [&]()
   {
      // This is a float to support multiple az/el data table formats.
      // This allows for flat tables to be read in as well as traditional AFSIM az/el tables.
      float azCount;
      float elCount;

      if (!(ifs >> azCount >> elCount))
      {
         throw UtInput::BadValue(aInput, "Error reading az/el counts from file " + aFileName);
      }
      if ((azCount <= 0) || (elCount <= 0))
      {
         throw UtInput::BadValue(aInput, "Invalid az/el counts file " + aFileName);
      }
      return std::make_pair(static_cast<int>(azCount), static_cast<int>(elCount));
   };

   int azCount;
   int elCount;

   std::tie(azCount, elCount) = readAzElCounts();

   auto tablePtr = ut::make_unique<UtAzElTable>();
   tablePtr->mDataValues.Resize(azCount, elCount);

   std::vector<float> azValues;
   std::vector<float> elValues;
   float              azValue;
   float              elValue;
   float              sigValue;
   int                azIndex;
   int                elIndex;

   // Read the elevations...

   for (elIndex = 0; elIndex < elCount; ++elIndex)
   {
      if (!(ifs >> elValue))
      {
         throw UtInput::BadValue(aInput, "Error reading elevation file " + aFileName);
      }
      elValues.push_back(static_cast<float>(elValue * UtMath::cRAD_PER_DEG));
   }

   // Read the values for each azimuth...

   for (azIndex = 0; azIndex < azCount; ++azIndex)
   {
      if (!(ifs >> azValue))
      {
         throw UtInput::BadValue(aInput, "Error reading azimuth from file " + aFileName);
      }
      azValues.push_back(static_cast<float>(azValue * UtMath::cRAD_PER_DEG));
      for (elIndex = 0; elIndex < elCount; ++elIndex)
      {
         if (!(ifs >> sigValue))
         {
            throw UtInput::BadValue(aInput, "Error reading data value from file " + aFileName);
         }
         tablePtr->mDataValues.Set(sigValue, azIndex, elIndex);
      }
   }

   // The following code handles the situation in which only half of the azimuth values are given.
   // The validation checks are written so that the az and el values must increase (-180, -170, -160 or 10, 20, 30, etc).
   // So we need to handle the case when the range -180 to 0 deg is given for az, as well as when 0 to 180 deg is given.

   // Case where -180 to 0 deg is given
   if (azValues[azCount - 1] == 0.0)
   {
      tablePtr->mDataValues.Resize(azCount * 2 - 1, elCount);
      for (azIndex = 1; azIndex < azCount; ++azIndex)
      {
         // mirror the data
         azValues.push_back(static_cast<float>(-azValues[azCount - azIndex - 1]));
         for (elIndex = 0; elIndex < elCount; ++elIndex)
         {
            tablePtr->mDataValues.Set(tablePtr->mDataValues.Get(azCount - azIndex - 1, elIndex),
                                      azIndex + azCount - 1,
                                      elIndex);
         }
      }
   }
   else if (azValues[0] == 0.0) // case where 0 to 180 deg is given
   {
      tablePtr->mDataValues.Resize(azCount * 2 - 1, elCount);

      // Move the 0 to 180 deg data, that is already read in, to the bottom half of the new table.
      // Validate() method will only pass if the azValues are increasing (-180, -170, -160, etc), so
      // just throwing the mirrored data on the bottom half of the table won't work in this case.
      for (azIndex = azCount - 1; azIndex >= 0; --azIndex)
      {
         for (elIndex = 0; elIndex < elCount; ++elIndex)
         {
            tablePtr->mDataValues.Set(tablePtr->mDataValues.Get(azIndex, elIndex), azIndex + azCount - 1, elIndex);
         }
      }
      // Now, fill in the mirrored data on the top half of the table, so as to pass validation
      std::vector<float> mirror;
      for (azIndex = azCount * 2 - 2; azIndex >= azCount; --azIndex)
      {
         // mirror the data
         mirror.push_back(-azValues[azIndex - azCount + 1]);
         for (elIndex = 0; elIndex < elCount; ++elIndex)
         {
            tablePtr->mDataValues.Set(tablePtr->mDataValues.Get(azIndex, elIndex), azCount * 2 - 2 - azIndex, elIndex);
         }
      }
      azValues.insert(azValues.begin(), mirror.begin(), mirror.end());
   }

   tablePtr->mAzValues.SetValues(azValues);
   tablePtr->mElValues.SetValues(elValues);
   if (!tablePtr->Validate(azValues.size(), elValues.size()))
   {
      throw UtInput::BadValue(aInput, "Error validating file " + aFileName);
   }

   // Table is good... take ownership of the table object from the unique_ptr
   aTablePtr = tablePtr.get();
   tablePtr.release();
}

// private
void UtAzElTableLoader::LoadTableFromPLT_File(UtInput& aInput, PolFreqTableMapType& aTableMap, std::string& aFileName)
{
   static const std::string newTableString("NPTS=");
   static const std::string elevString("ELEV=");
   static const std::string freqString("FREQ=");
   static const std::string altFreqString("F="); // old versions of table used this.
   static const std::string pol1String("POL1=");
   static const std::string pol2String("POL2=");
   static const std::string eodString("END OF DATA");

   aInput.ReadValueQuoted(aFileName);
   aFileName = aInput.LocateFile(aFileName);

   std::string defaultPol; // Read which polarization to use as default, if any

   try
   {
      aInput.ReadValue(defaultPol);

      if (defaultPol == "default")
      {
         aInput.ReadValue(defaultPol);
         if ((defaultPol != "vertical") && (defaultPol != "horizontal"))
         {
            throw UtInput::BadValue(aInput,
                                    "Unrecognized default polarization " + defaultPol + " specified for PLT file " +
                                       aFileName);
         }
      }
      else
      {
         aInput.PushBack(defaultPol);
      }
   }
   catch (UtInput::EndOfData&)
   {
   }

   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtInput::BadValue(aInput, "Unable to open PLT file " + aFileName);
   }

   // Read all the data from the file into a temporary map structure in order to maintain
   // the elevations and azimuths in sorted (increasing) order, as required by UtAzElTable
   // After file is read in, the data is copied to aTableMap
   // polarization -> frequency -> elevation -> azimuth -> RCS in dB
   using TempMapType = std::map<std::string, std::map<double, std::map<float, std::map<float, float>>>>;
   TempMapType tempMap;

   while (ifs.peek() != EOF)
   {
      std::string line;

      // Parse any optional comments, lines that do not contain string 'NPTS'
      size_t found = std::string::npos;
      while (found == std::string::npos)
      {
         if (!std::getline(ifs, line))
         {
            throw UtInput::BadValue(aInput, "Error parsing comments in PLT file " + aFileName);
         }
         found = line.find(newTableString);
      }

      // Read number of points (line of data in the table)
      std::stringstream iss(line);
      iss.seekg(found + newTableString.length());
      unsigned int npts;
      iss >> npts;

      // Read elevation (in degrees)
      iss.seekg(line.find(elevString) + elevString.length());
      ;
      float elevation;
      iss >> elevation;
      elevation *= static_cast<float>(UtMath::cRAD_PER_DEG); // convert to radians

      // Read frequency (in GHz)
      size_t pos = line.find(freqString);
      if (pos == std::string::npos) // allow for either "F=" or "FREQ=" to be parsed.
      {
         pos = line.find(altFreqString);
         pos += altFreqString.length();
      }
      else
      {
         pos += freqString.length();
      }
      iss.seekg(pos);
      float freqGHz;
      iss >> freqGHz;
      double frequency = aInput.ConvertValue(freqGHz, "ghz", UtInput::cFREQUENCY);

      // Read polarizations (assume only VV and HH for now)       // EDIT: Do not enforce polarization labels
      /*iss.seekg(line.find(pol1String) + pol1String.length());
      string pol;
      iss >> pol;
      if (pol != "VV")
      {
         throw UtInput::BadValue(aInput, "Expecting POL1=VV in PLT file " + aFileName);
      }
      iss.seekg(line.find(pol2String) + pol2String.length());
      iss >> pol;
      if (pol != "HH")
      {
         throw UtInput::BadValue(aInput, "Expecting POL2=HH in PLT file " + aFileName);
      }*/

      // Check that NPTS value matches other elevations at same frequency
      for (auto& it : tempMap)
      {
         if (it.second.find(frequency) != it.second.end() && it.second[frequency].begin()->second.size() != npts)
         {
            throw UtInput::BadValue(
               aInput,
               "Two different elevations with same frequency encountered with different NPTS in PLT file " + aFileName);
         }
      }

      bool endOfData = false;
      while (!endOfData)
      {
         if (!std::getline(ifs, line))
         {
            throw UtInput::BadValue(aInput, "Error reading line in PLT file " + aFileName);
         }

         endOfData = (line.find(eodString) != std::string::npos);

         if (!endOfData)
         {
            if (tempMap["vertical"][frequency][elevation].size() > npts ||
                tempMap["horizontal"][frequency][elevation].size() > npts)
            {
               throw UtInput::BadValue(aInput, "Number of lines in table greater than NPTS value in PLT file " + aFileName);
            }

            iss.clear();
            iss.str(line);
            float azimuth;
            float vReal;
            float vImag;
            float hReal;
            float hImag; // 5-column format
            float v_dB;
            float h_dB; // read directly in 3-column format, computed in 5-column
            if (iss >> azimuth >> vReal >> vImag >> hReal >> hImag)
            {
               static const float FCONST = 1.129409075F;
               float              fps    = FCONST / (freqGHz * freqGHz);
               v_dB                      = 10 * log10(fps * (vReal * vReal + vImag * vImag));
               h_dB                      = 10 * log10(fps * (hReal * hReal + hImag * hImag));
            }
            else
            {
               iss.clear();
               iss.seekg(0);
               if (!(iss >> azimuth >> v_dB >> h_dB))
               {
                  throw UtInput::BadValue(
                     aInput,
                     "Neither 5-column nor 3-column format encountered when reading Vpol and Hpol values in PLT file " +
                        aFileName);
               }
            }

            azimuth *= static_cast<float>(UtMath::cRAD_PER_DEG); // convert to radians
            tempMap["vertical"][frequency][elevation][azimuth]   = v_dB;
            tempMap["horizontal"][frequency][elevation][azimuth] = h_dB;
         }
      } // END OF DATA

      if ((tempMap["vertical"][frequency][elevation].size() != npts) ||
          (tempMap["horizontal"][frequency][elevation].size() != npts)) // check that the correct number of lines were present
      {
         throw UtInput::BadValue(aInput, "Number of lines in table less than NPTS value in PLT file " + aFileName);
      }
   } // End of file reached

   // Write data from temporary map into UtAzElTable's
   for (const auto& pol : tempMap)
   {
      // Rework frequency limits so that the frequency limits are computed to be
      // between the given frequencies (or, in other words, the given frequencies are
      // midway between the limits.
      double                   lowFreq  = 0.0;
      double                   highFreq = 0.0;
      double                   newFreq  = 0.0;
      std::map<double, double> freqMap;
      for (const auto& freq : pol.second)
      {
         highFreq = freq.first;
         if (freq.first != pol.second.begin()->first)
         {
            newFreq          = (lowFreq + highFreq) / 2.0;
            freqMap[lowFreq] = newFreq;
         }
         else
         {
            newFreq = highFreq;
         }
         lowFreq = highFreq;
      }
      freqMap[highFreq] = highFreq + newFreq;

      for (const auto& freq : pol.second)
      {
         UtAzElTable* tablePtr                     = new UtAzElTable();
         aTableMap[pol.first][freqMap[freq.first]] = tablePtr; // tablePtr is populated below.
         tablePtr->mElValues.Resize(static_cast<int>(freq.second.size()));
         int elIndex = 0;
         int azIndex = 0;

         for (const auto& el : freq.second)
         {
            tablePtr->mElValues.Set(el.first, elIndex);
            tablePtr->mAzValues.Resize(static_cast<int>(el.second.size()));
            tablePtr->mDataValues.Resize(static_cast<int>(el.second.size()), static_cast<int>(freq.second.size()));
            azIndex = 0;

            for (const auto& az : el.second)
            {
               tablePtr->mAzValues.Set(az.first, azIndex);
               tablePtr->mDataValues.Set(az.second, azIndex, elIndex);
               azIndex++;
            }

            elIndex++;
         }

         // Validate the table
         if (!tablePtr->Validate(azIndex, elIndex))
         {
            throw UtInput::BadValue(aInput, "Error validating UtAzElTable in PLT file " + aFileName);
         }
      }
   }

   // If default polarization is given, copy the tables into a new entry in the map
   if (defaultPol == "vertical" || defaultPol == "horizontal")
   {
      aTableMap.insert(make_pair(std::string("default"), aTableMap[defaultPol]));
   }
}
