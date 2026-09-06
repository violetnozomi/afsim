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

#include "WsfTabularAttenuation.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"

namespace
{
// Independent variable indices for table lookups.
enum
{
   cIV_FREQUENCY,
   cIV_ALTITUDE,
   cIV_ELEVATION_ANGLE,
   cIV_SLANT_RANGE,
   cIV_ALTITUDE_1,
   cIV_ALTITUDE_2,
   cIV_GROUND_RANGE,
   cIV_COUNT // Number of independent variables
};

UtTable::Parameters sParameters;

// Helper function to check for presence of a variable name.
bool VariableExists(const std::vector<std::string>& aNames, const std::string& aName)
{
   return (std::find(aNames.begin(), aNames.end(), aName) != aNames.end());
}
} // namespace

// =================================================================================================
WsfTabularAttenuation::WsfTabularAttenuation()
   : WsfEM_Attenuation()
   , mAttenuationPtr()
   , mAdjustmentFactor(1.0)
   , mTwoWayAttenuation(false)
   , mNeedFrequency(false)
   , mNeedGroundRange(false)
   , mNeedSlantRange(false)
{
   // Change the default of end point sorting from the base class default of 'true'.
   mSortEndPoints = false;

   if (sParameters.Empty())
   {
      // Register allowable independent variables.
      sParameters.AddRealParameter("frequency", UtInput::cFREQUENCY, UtTable::ValueGE(0.0), cIV_FREQUENCY);
      sParameters.AddRealParameter("altitude", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE);
      sParameters.AddRealParameter("elevation_angle", UtInput::cANGLE, UtTable::NoCheck(), cIV_ELEVATION_ANGLE);
      sParameters.AddRealParameter("slant_range", UtInput::cLENGTH, UtTable::NoCheck(), cIV_SLANT_RANGE);
      sParameters.AddRealParameter("altitude_1", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE_1);
      sParameters.AddRealParameter("altitude_2", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE_2);
      sParameters.AddRealParameter("ground_range", UtInput::cLENGTH, UtTable::NoCheck(), cIV_GROUND_RANGE);
   }
}

// =================================================================================================
WsfTabularAttenuation::WsfTabularAttenuation(const WsfTabularAttenuation& aSrc)
   : WsfEM_Attenuation(aSrc)
   , mAttenuationPtr(aSrc.mAttenuationPtr)
   , mAdjustmentFactor(aSrc.mAdjustmentFactor)
   , mTwoWayAttenuation(aSrc.mTwoWayAttenuation)
   , mNeedFrequency(aSrc.mNeedFrequency)
   , mNeedGroundRange(aSrc.mNeedGroundRange)
   , mNeedSlantRange(aSrc.mNeedSlantRange)
{
}

// =================================================================================================
//! Factory method called by WsfEM_AttenuationTypes.
// static
WsfEM_Attenuation* WsfTabularAttenuation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Attenuation* instancePtr = nullptr;
   if ((aTypeName == "WSF_TABULAR_ATTENUATION") || (aTypeName == "tabular"))
   {
      instancePtr = new WsfTabularAttenuation();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Attenuation* WsfTabularAttenuation::Clone() const
{
   return new WsfTabularAttenuation(*this);
}

// =================================================================================================
// virtual
bool WsfTabularAttenuation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   bool ok = WsfEM_Attenuation::Initialize(aXmtrRcvrPtr);

   // Make sure the table is defined and that one of the following is true:
   // 1) A 'constant' table is used (not very useful, but allowed).
   // 2) function of altitude, elevation_angle, slant_range
   // 3) function of altitude_1, altitude_2, ground_range

   if (mAttenuationPtr != nullptr)
   {
      std::vector<std::string> ivNames;
      mAttenuationPtr->GetVariableNames(ivNames);

      mNeedFrequency = VariableExists(ivNames, "frequency");
      if (ivNames.empty())
      {
         // constant form
      }
      else if (VariableExists(ivNames, "altitude") && VariableExists(ivNames, "elevation_angle") &&
               VariableExists(ivNames, "slant_range") && (!VariableExists(ivNames, "altitude_1")) &&
               (!VariableExists(ivNames, "altitude_2")) && (!VariableExists(ivNames, "ground_range")))
      {
         mNeedSlantRange = true;
      }
      else if (VariableExists(ivNames, "altitude_1") && VariableExists(ivNames, "altitude_2") &&
               VariableExists(ivNames, "ground_range") && (!VariableExists(ivNames, "altitude")) &&
               (!VariableExists(ivNames, "elevation_angle")) && (!VariableExists(ivNames, "slant_range")))
      {
         mNeedGroundRange = true;
      }
      else
      {
         ut::log::error() << "Insufficient or inconsistent independent variables.";
         ok = false;
      }
   }
   else
   {
      ut::log::error() << "Attenuation table not defined.";
      ok = false;
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfTabularAttenuation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "attenuation")
   {
      mAttenuationPtr = UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), sParameters);
   }
   else if (command == "sort_end_points")
   {
      aInput.ReadValue(mSortEndPoints);
   }
   else if (command == "two_way_attenuation")
   {
      aInput.ReadValue(mTwoWayAttenuation);
   }
   else if (command == "query") // NO_DOC | TESTING
   {
      ProcessQueryCommand(aInput);
   }
   else if (command == "spectral_data_conversion")
   {
      SpectralDataConversion(aInput);
   }
   else
   {
      myCommand = WsfEM_Attenuation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// This overrides the base class 3-argument implementation.
// virtual
double WsfTabularAttenuation::ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                                       WsfEnvironment&             aEnvironment,
                                                       WsfEM_Interaction::Geometry aGeometry)
{
   double lookupArgs[cIV_COUNT] = {0.0};

   // Populate the lookup arguments with only the needed variables...

   if (mNeedFrequency)
   {
      WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
      WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();
      if (xmtrPtr != nullptr)
      {
         lookupArgs[cIV_FREQUENCY] = xmtrPtr->GetFrequency();
      }
      else if (rcvrPtr != nullptr)
      {
         lookupArgs[cIV_FREQUENCY] = rcvrPtr->GetFrequency();
      }
   }

   if (mNeedSlantRange)
   {
      GetRangeElevationAltitude(aInteraction,
                                aGeometry,
                                lookupArgs[cIV_SLANT_RANGE],
                                lookupArgs[cIV_ELEVATION_ANGLE],
                                lookupArgs[cIV_ALTITUDE]);
   }
   else if (mNeedGroundRange)
   {
      GetAltitudesAndGroundRange(aInteraction,
                                 aGeometry,
                                 lookupArgs[cIV_ALTITUDE_1],
                                 lookupArgs[cIV_ALTITUDE_2],
                                 lookupArgs[cIV_GROUND_RANGE]);
   }

   double atten = mAttenuationPtr->Lookup(lookupArgs); // Get the attenuation from the table.

   // If the table values represent two-way values, convert the result to a one-way value.
   if (mTwoWayAttenuation)
   {
      atten = sqrt(atten);
   }

   // Return linear attenuation factor
   return (atten * mAdjustmentFactor);
}

// =================================================================================================
//! Process the 'query' command.
//! This must occur after the 'attenuation' command.
// private
void WsfTabularAttenuation::ProcessQueryCommand(UtInput& aInput)
{
   // Note: The user is responsible for ensuring they provide enough input.

   double       lookupArgs[cIV_COUNT] = {-1.0};
   double       expectedValue         = -1.0;
   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "frequency")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_FREQUENCY], UtInput::cFREQUENCY);
         aInput.ValueGreater(lookupArgs[cIV_FREQUENCY], 0.0);
      }
      else if (command == "altitude")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_ALTITUDE], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(lookupArgs[cIV_ALTITUDE], 0.0);
      }
      else if (command == "slant_range")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_SLANT_RANGE], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(lookupArgs[cIV_SLANT_RANGE], 0.0);
      }
      else if (command == "elevation_angle")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_ELEVATION_ANGLE], UtInput::cANGLE);
         aInput.ValueInClosedRange(lookupArgs[cIV_ELEVATION_ANGLE], -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if (command == "altitude_1")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_ALTITUDE_1], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(lookupArgs[cIV_ALTITUDE_1], 0.0);
      }
      else if (command == "altitude_2")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_ALTITUDE_2], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(lookupArgs[cIV_ALTITUDE_2], 0.0);
      }
      else if (command == "ground_range")
      {
         aInput.ReadValueOfType(lookupArgs[cIV_GROUND_RANGE], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(lookupArgs[cIV_GROUND_RANGE], 0.0);
      }
      else if (command == "expected_value")
      {
         aInput.ReadValue(expectedValue);
         aInput.ValueGreaterOrEqual(expectedValue, 0.0);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   double atten = mAttenuationPtr->Lookup(lookupArgs); // Get the attenuation from the table.

   // If the table values represent two-way values, convert the result to a one-way value.
   if (mTwoWayAttenuation)
   {
      atten = sqrt(atten);
   }

   { // RAII block
      auto out = ut::log::debug() << "WsfTabularAttenuation query: ";
      out.AddNote() << "Attenuation: " << UtMath::SafeLinearToDB(atten) << " dB (" << atten << " abs)";
   }
   if (expectedValue >= 0.0)
   {
      if (fabs(expectedValue - atten) > 0.0001)
      {
         auto out = ut::log::info("test_fail") << "-FAIL- attenuation query:";
         out.AddNote() << "Actual: " << atten;
         out.AddNote() << "Expected: " << expectedValue;
      }
   }
}

// =================================================================================================
//! Convert raw MODTRAN results into a form that can be used as input to this model.
// private
void WsfTabularAttenuation::SpectralDataConversion(UtInput& aInput)
{
   std::string    sttFileName;         // Name of sensor-to-target transmittance file
   std::string    tbrFileName;         // Name of target-to-background radiance file.
   UtTable::Curve sensorResponseCurve; // Sensor response
   std::string    outputFileName;      // Name of the output file

   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      UtInput& input(inputBlock.GetInput());
      if (command == "sensor_to_target_transmittance")
      {
         input.ReadValue(sttFileName);
      }
      else if (command == "target_to_background_radiance")
      {
         input.ReadValue(tbrFileName);
      }
      else if (command == "sensor_response_curve")
      {
         sensorResponseCurve.ProcessInput(input,
                                          UtInput::cLENGTH,
                                          "",
                                          UtTable::ValueGT(0.0),
                                          UtInput::cNON_DIMENSIONAL,
                                          "",
                                          UtTable::ValueGE_LE(0.0, 1.0));
         double minValue, maxValue;
         sensorResponseCurve.GetVariableLimits(minValue, maxValue);
      }
      else if (command == "output")
      {
         input.ReadValue(outputFileName);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (sttFileName.empty())
   {
      throw UtInput::BadValue(aInput, "'sensor_to_target_transmittance' is required");
   }
   if (outputFileName.empty())
   {
      throw UtInput::BadValue(aInput, "'output' is required");
   }
   std::ifstream sttFile(sttFileName.c_str());
   if (!sttFile.is_open())
   {
      throw UtInput::BadValue(aInput, "Unable to open " + sttFileName);
   }
   std::ifstream tbrFile;
   if (!tbrFileName.empty())
   {
      tbrFile.open(tbrFileName.c_str());
      if (!tbrFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + tbrFileName);
      }
   }
   std::ofstream outputFile(outputFileName.c_str());
   if (!outputFile.is_open())
   {
      throw UtInput::BadValue(aInput, "Unable to open " + outputFileName);
   }

   { // RAII block
      auto out = ut::log::info() << "Starting spectral data conversion.";
      out.AddNote() << "Output File: " << outputFileName;
      out.AddNote() << "Sensor-to-Target Transmittance File: " << sttFileName;
      if (!tbrFileName.empty())
      {
         out.AddNote() << "Target-to-Background Radiance File: " << tbrFileName;
      }
   }

   std::vector<double> altitudeValues;
   std::vector<double> elevationValues;
   std::vector<double> rangeValues;
   std::vector<double> firstElevationValues;
   std::vector<double> firstRangeValues;
   std::vector<double> resultValues;
   std::vector<double> sensorResponseVector;

   std::string headers[3];
   ReadSpectralHeaders(sttFile, headers);
   if (!tbrFileName.empty())
   {
      ReadSpectralHeaders(tbrFile, headers);
   }

   double              sttAltitude, sttElevation, sttRange;
   std::vector<double> sttWavenumbers, sttValues;
   double              tbrAltitude, tbrElevation, tbrRange;
   std::vector<double> tbrWavenumbers, tbrValues;

   double lastResult                 = -1.0;
   double lastAltitude               = -9999.0;
   double lastElevation              = -9999.0;
   double lastRange                  = -9999.0;
   bool   savingFirstElevationValues = false;
   bool   savingFirstRangeValues     = false;

   while (true)
   {
      bool sttEOF = ReadSpectralData(sttFile, sttAltitude, sttElevation, sttRange, sttWavenumbers, sttValues);
      if (tbrFileName.empty())
      {
         if (sttEOF)
         {
            break;
         }
      }
      else
      {
         bool tbrEOF = ReadSpectralData(tbrFile, tbrAltitude, tbrElevation, tbrRange, tbrWavenumbers, tbrValues);

         if (sttEOF && tbrEOF)
         {
            break;
         }
         else if (sttEOF != tbrEOF)
         {
            throw UtException("Spectral Data Error: mismatch end-of-files");
         }
         else if ((sttAltitude != tbrAltitude) || (sttElevation != tbrElevation) || (sttRange != tbrRange) ||
                  (sttWavenumbers != tbrWavenumbers))
         {
            throw UtException("Spectral Data Error: Inconsistent files");
         }
      }

      // Ensure independent values have correct relationships.
      // -) Must be monotonically increasing.
      // -) The breakpoints for elevation and range must always be the same.

      if (sttAltitude != lastAltitude)
      {
         std::ostringstream oss;
         oss << "\nAltitude=" << sttAltitude;
         if (sttAltitude <= lastAltitude)
         {
            throw UtException("Spectral Data Error: Non-ascending altitudes" + oss.str());
         }

         // Make sure the elevation values in the previous line are consistent with the first set of altitudes
         if (elevationValues != firstElevationValues)
         {
            throw UtException("Spectral Data Error: Mis-matched elevation breakpoints");
         }
         elevationValues.clear();

         savingFirstElevationValues = altitudeValues.empty();
         altitudeValues.push_back(sttAltitude);
         lastElevation = -9999.0;
      }

      // Check for change in elevation...
      if (sttElevation != lastElevation)
      {
         // Make sure elevation values are monotonically increasing.
         if (sttElevation <= lastElevation)
         {
            throw UtException("Spectral Data Error: Non-ascending elevation values");
         }

         // Make sure the range breakpoints are the same for every elevation
         if (rangeValues != firstRangeValues)
         {
            throw UtException("Spectral Data Error: Mis-matched range breakpoints");
         }
         rangeValues.clear();

         // If we are processing the first altitude then save the baseline elevation breakpoints.
         savingFirstRangeValues = false;
         if (savingFirstElevationValues)
         {
            savingFirstRangeValues = firstElevationValues.empty();
            firstElevationValues.push_back(sttElevation);
         }
         elevationValues.push_back(sttElevation);
         lastRange = -9999.0;
      }

      if (sttRange <= lastRange)
      {
         throw UtException("Spectral Data Error: Non-ascending ranges");
      }
      // Collect the baseline range breakpoints when processing the first altitude/elevation.
      if (savingFirstRangeValues)
      {
         firstRangeValues.push_back(sttRange);
      }
      rangeValues.push_back(sttRange);

      lastAltitude  = sttAltitude;
      lastElevation = sttElevation;
      lastRange     = sttRange;

      // Compute the Line-of-sight atmospheric transmissivity (LOSAT from the algorithm description)

      double result = lastResult;
      if (!sttWavenumbers.empty())
      {
         // Build the sensor response as a function of wavenumber.
         if (sensorResponseVector.empty())
         {
            BuildResponseVector(sensorResponseCurve, sttWavenumbers, sensorResponseVector);
         }
         if (tbrFileName.empty())
         {
            result = ComputeAverageTransmittance(sttWavenumbers, sttValues, sensorResponseVector);
         }
         else
         {
            result = ComputeAverageContrastTransmittance(sttWavenumbers, sttValues, tbrValues, sensorResponseVector);
         }
         lastResult = result;
      }
      resultValues.push_back(result);
   }
   sttFile.close();
   if (!tbrFileName.empty())
   {
      tbrFile.close();
   }

   // Make sure the last set of elevation and range breakpoints match. They must are checked when the NEXT change
   // occurs, and in this case the end-of-file is the change!
   if (elevationValues != firstElevationValues)
   {
      throw UtException("Spectral Data Error: Mis-matched elevation breakpoints");
   }
   if (rangeValues != firstRangeValues)
   {
      throw UtException("Spectral Data Error: Mis-matched range breakpoints");
   }

   { // RAII block
      auto out = ut::log::info() << "Input processing complete.";
      out.AddNote() << "Altitudes: " << altitudeValues.size();
      out.AddNote() << "Elevations: " << elevationValues.size();
      out.AddNote() << "Ranges: " << rangeValues.size();
      out.AddNote() << "Total Points: " << resultValues.size();
   }

   size_t altitudeCount  = altitudeValues.size();
   size_t elevationCount = elevationValues.size();
   size_t rangeCount     = rangeValues.size();
   size_t resultCount    = resultValues.size();
   if ((altitudeCount < 2) || (elevationCount < 2) || (rangeCount < 2))
   {
      throw UtException("Spectral Data Error: Must have at least 2 breakpoints for each dimension");
   }
   if ((altitudeCount * elevationCount * rangeCount) != resultCount)
   {
      throw UtException("Spectral Data Error: Internal error");
   }

   // Write out the file
   for (size_t i = 0; i < 3; ++i)
   {
      if (!headers[i].empty())
      {
         outputFile << "# " + headers[i] << "\n";
      }
   }
   outputFile << "\n";
   outputFile << "# Approximate size in memory=" << altitudeCount + elevationCount + rangeCount + resultCount
              << "  * sizeof(float)\n";
   outputFile << "\n";

   outputFile << "regular_table\n";
   outputFile << "   # Number of altitudes = " << altitudeValues.size() << "\n";
   outputFile << "   independent_variable altitude  units meters  precision float";
   for (size_t i = 0; i < altitudeValues.size(); ++i)
   {
      if ((i % 7) == 0)
      {
         outputFile << "\n     ";
      }
      outputFile << " " << std::setw(10) << altitudeValues[i];
   }
   outputFile << "\n   end_independent_variable\n";
   outputFile << "\n";
   outputFile << "   # Number of elevation angles = " << elevationValues.size() << "\n";
   outputFile << "   independent_variable  elevation_angle  units degrees  precision float";
   for (size_t i = 0; i < elevationValues.size(); ++i)
   {
      if ((i % 7) == 0)
      {
         outputFile << "\n     ";
      }
      outputFile << " " << std::setw(10) << elevationValues[i];
   }
   outputFile << "\n   end_independent_variable\n";
   outputFile << "\n";
   outputFile << "   # Number of slant ranges = " << rangeValues.size() << "\n";
   outputFile << "   independent_variable  slant_range  units meters  precision float";
   for (size_t i = 0; i < rangeValues.size(); ++i)
   {
      if ((i % 7) == 0)
      {
         outputFile << "\n     ";
      }
      outputFile << " " << std::setw(10) << rangeValues[i];
   }
   outputFile << "\n   end_independent_variable\n";
   outputFile << "\n";
   outputFile << "   # Number of values = " << resultValues.size() << "\n";
   outputFile << "   dependent_variable  precision float\n";
   size_t resultIndex = 0;
   for (size_t iAlt = 0; iAlt < altitudeValues.size(); ++iAlt)
   {
      for (size_t iEl = 0; iEl < elevationValues.size(); ++iEl)
      {
         outputFile << "\n      # Altitude=" << altitudeValues[iAlt] << " meters";
         outputFile << ", Elevation angle=" << elevationValues[iEl] << " degrees";
         for (size_t i = 0; i < rangeValues.size(); ++i)
         {
            if ((i % 7) == 0)
            {
               outputFile << "\n     ";
            }
            outputFile << " " << std::setw(10) << resultValues[resultIndex];
            ++resultIndex;
         }
         outputFile << "\n";
      }
   }
   outputFile << "   end_dependent_variable\n";
   outputFile << "end_regular_table\n";
   outputFile.close();
   ut::log::info() << "Conversion complete.";
}

// =================================================================================================
//! Build the sensor response vector.
//! @param aResponseCurve  [input] The sensor response curve as a function of wavelength
//! @param aWavenumbers    [input] The wavenumber of the bin centers.
//! @param aResponseVector [output] The sensor response the wavenumber bin centers.
// public static
void WsfTabularAttenuation::BuildResponseVector(UtTable::Curve&            aResponseCurve,
                                                const std::vector<double>& aWavenumbers,
                                                std::vector<double>&       aResponseVector)
{
   aResponseVector.resize(aWavenumbers.size());
   if (aResponseCurve.IsDefined())
   {
      double minWavelength;
      double maxWavelength;
      aResponseCurve.GetVariableLimits(minWavelength, maxWavelength);
      for (size_t i = 0; i < aWavenumbers.size(); ++i)
      {
         double wavelength = 1.0 / aWavenumbers[i]; // Wavenumber (cm-1) -> wavelength (cm)
         wavelength *= 1.0E-2;                      // wavelength (m)
         if ((wavelength >= minWavelength) && (wavelength <= maxWavelength))
         {
            aResponseVector[i] = aResponseCurve.Lookup(wavelength);
         }
         else
         {
            // Setting the element to a negative value forces it to be excluded from calculations
            aResponseVector[i] = -1.0E-10;
         }
      }
   }
   else
   {
      for (size_t i = 0; i < aWavenumbers.size(); ++i)
      {
         aResponseVector[i] = 1.0;
      }
   }
}

// =================================================================================================
//! Compute the average transmittance over the band.
//! @param aWavenumbers                   [input] The wavenumber of the bin centers.
//! @param aSensorToTargetTransmittances  [input] The transmittance in each bin.
//! @param aTargetToBackgroundRadiances   [input] The radiance of background at the target in each bin.
//! @param aSensorResponse                [input] The sensor response in each bin.
//! @returns The contrast transmittance.
// public static
double WsfTabularAttenuation::ComputeAverageContrastTransmittance(const std::vector<double>& aWavenumbers,
                                                                  const std::vector<double>& aSensorToTargetTransmittances,
                                                                  const std::vector<double>& aTargetToBackgroundRadiances,
                                                                  const std::vector<double>& aSensorResponse)
{
   double numer = 0.0;
   double denom = 0.0;
   size_t n     = aWavenumbers.size();
   for (size_t i = 0; i < n; ++i)
   {
      // Compute the size of the interval.
      double dLambda = 0.0;
      if (i == 0)
      {
         // The first interval is a half-sized interval where the left endpoint is first wavenumber.
         dLambda = 0.5 * (aWavenumbers[1] - aWavenumbers[0]);
      }
      else if ((i + 1) == n)
      {
         // The last interval is a half-interval whose right endpoint is the last wavenumber.
         dLambda = 0.5 * (aWavenumbers[n - 1] - aWavenumbers[n - 2]);
      }
      else
      {
         // The other intervals are all the same size whose mid-point is the current wavenumber.
         dLambda = 0.5 * (aWavenumbers[i + 1] - aWavenumbers[i - 1]);
      }
      // If the response vector element is less than zero then the bin is outside the range of the
      // sensor's response and is not included in the computation.
      double V = aSensorResponse[i];
      if (V >= 0.0)
      {
         double L_B   = aTargetToBackgroundRadiances[i];
         double tau_a = aSensorToTargetTransmittances[i];
         numer += L_B * V * tau_a * dLambda;
         denom += L_B * V * dLambda;
      }
   }
   double result = numer / denom;
   return result;
}

// =================================================================================================
//! Compute the average transmittance over the band.
//! @param aWavenumbers     [input] The wavenumber of the bin centers.
//! @param aTransmittances  [input] The transmittance in each bin.
//! @param aSensorResponse  [input] The sensor response in each bin.
//! @returns The average transmittance.
// public static
double WsfTabularAttenuation::ComputeAverageTransmittance(const std::vector<double>& aWavenumbers,
                                                          const std::vector<double>& aTransmittances,
                                                          const std::vector<double>& aSensorResponse)
{
   size_t n = aWavenumbers.size();
   if (n == 1)
   {
      return aTransmittances[0]; // Only a single bin
   }
   else if (n == 2)
   {
      // Monochromatic... if the wavelengths (in nm) cross an integer boundary then assume we want the integer value.
      double lowerWavelength      = 10000000.0 / aWavenumbers[1]; // cm-1 -> nm
      double upperWavelength      = 10000000.0 / aWavenumbers[0]; // cm-1 -> nm
      double floorLowerWavelength = std::floor(lowerWavelength);
      double floorUpperWavelength = std::floor(upperWavelength);
      if (floorLowerWavelength < floorUpperWavelength)
      {
         double f        = (floorUpperWavelength - lowerWavelength) / (upperWavelength - lowerWavelength);
         double lowerTau = aTransmittances[1];
         double upperTau = aTransmittances[0];
         double tau      = lowerTau + (f * (upperTau - lowerTau));
         return tau;
      }
      // Doesn't span an integer wavelength, so just use the normal algorithm
   }

   // Calculate the average transmittance using the same method as MODTRAN.

   double numer      = 0.0;
   double denom      = 0.0;
   double absorption = 0.0;
   for (size_t i = 0; i < n; ++i)
   {
      // Compute the size of the interval.
      double dLambda = 0.0;
      if (i == 0)
      {
         // The first interval is a half-sized interval where the left endpoint is first wavenumber.
         dLambda = 0.5 * (aWavenumbers[1] - aWavenumbers[0]);
      }
      else if ((i + 1) == n)
      {
         // The last interval is a half-interval whose right endpoint is the last wavenumber.
         dLambda = 0.5 * (aWavenumbers[n - 1] - aWavenumbers[n - 2]);
      }
      else
      {
         // The other intervals are all the same size whose mid-point is the current wavenumber.
         dLambda = 0.5 * (aWavenumbers[i + 1] - aWavenumbers[i - 1]);
      }
      // If the response vector element is less than zero then the bin is outside the range of the
      // sensor's response and is not included in the computation.
      double V = aSensorResponse[i];
      if (V >= 0.0)
      {
         absorption = 1.0 - aTransmittances[i];
         numer += (absorption * V * dLambda);
         denom += dLambda;
      }
   }
   if (denom != 0.0)
   {
      absorption = numer / denom;
   }
   double transmittance = 1.0 - absorption;
   return transmittance;
}

// =================================================================================================
//! Read the file headers (first three lines) from a spectral data file.
//! @param aFile      [input] file being read
//! @param aHeaders   [output] The headers.
// public static
void WsfTabularAttenuation::ReadSpectralHeaders(std::istream& aFile, std::string aHeaders[3])
{
   if ((!getline(aFile, aHeaders[0])) || (!getline(aFile, aHeaders[1])) || (!getline(aFile, aHeaders[2])))
   {
      throw UtException("Spectral Data Error: Error reading header data");
   }
}

// =================================================================================================
//! Read one set of spectral data for a given observation condition from MODTRAN (or some other similar program)
//! @param aFile        [input] file being read
//! @param aAltitude    [output] The observation altitude.
//! @param aElevation   [output] The observation (look) angle (relative to horizontal)
//! @param aSlantRange  [output] The observation slant range.
//! @param aWavenumbers [output] The wavenumbers of the bins (cm-1)
//! @param aValues      [output] The transmittance values [0..1] or radiance values (W/cm^2/sr).
//! @returns true if data successfully read or false if end-of-file encountered.
//! @throws UtException if there is an error in the data.
// public static
bool WsfTabularAttenuation::ReadSpectralData(std::istream&        aFile,
                                             double&              aAltitude,
                                             double&              aElevation,
                                             double&              aSlantRange,
                                             std::vector<double>& aWavenumbers,
                                             std::vector<double>& aValues)
{
   aAltitude   = -1000.0;
   aElevation  = -1000.0;
   aSlantRange = -1000.0;
   aWavenumbers.clear();
   aValues.clear();

   std::string line;
   bool        blockOpen = false;

   while (getline(aFile, line))
   {
      char ch = line[0];
      if (line.length() < 2)
      {
      }
      else if (line[0] == ' ')
      {
         // Radiance/transmittance for a wavenumber (which is the center of a bin)
         std::istringstream iss(line);
         double             tempWavenumber;
         double             tempValue;
         if (!(iss >> tempWavenumber >> tempValue))
         {
            throw UtException("Spectral Data Error: Error reading spectral data");
         }
         aWavenumbers.push_back(tempWavenumber);
         aValues.push_back(tempValue);
      }
      else if (ch == '%')
      {
         if (line[1] == '{')
         {
            // Start of a block for a single altitude/elevation/range.
            if (!aWavenumbers.empty())
            {
               // We are already processing a block (can't have a block within a block)!
               throw UtException("Spectral Data Error: Data sequence error");
            }
            std::istringstream iss(line);
            std::string        dummy;
            if (!(iss >> dummy >> aAltitude >> aElevation >> aSlantRange))
            {
               throw UtException("Spectral Data Error: Geometry input error");
            }
            if ((aAltitude < 0.0) || (aElevation < -90.0) || (aElevation > 90.0) || (aSlantRange < 0.0))
            {
               throw UtException("Spectral Data Error: Invalid geometry values");
            }
            blockOpen = true;
         }
         else if (line[1] == '}')
         {
            // End of the block for a single altitude/elevation/range
            blockOpen = false;
            break;
         }
         else
         {
            throw UtException("Spectral Data Error: Unknown data");
         }
      }
   }
   if (blockOpen)
   {
      throw UtException("Spectral Data Error: Unclosed block");
   }
   return (aAltitude < 0.0);
}
