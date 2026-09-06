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

#include "AntennaPlotFunction.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfAntennaPatternTypes.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Util.hpp"
#include "WsfESA_AntennaPattern.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"

AntennaPlotFunction::AntennaPlotFunction(WsfScenario& aScenario)
   : Function(aScenario)
   , mSimulationPtr(nullptr)
   , mPatternName()
   , mOutputFile()
   , mGnuPlotFile()
   , mHeaderLine1()
   , mHeaderLine2()
   , mHeaderLine3()
   , mOutputColumnLimit(100)
   , mPlotType('v')
   , mAzimuthMin(-UtMath::cPI)
   , mAzimuthMax(UtMath::cPI)
   , mAzimuthStep(1.0 * UtMath::cRAD_PER_DEG)
   , mElevationMin(-UtMath::cPI_OVER_2)
   , mElevationMax(UtMath::cPI_OVER_2)
   , mElevationStep(1.0 * UtMath::cRAD_PER_DEG)
   , mTiltAngle(0.0)
   , mFrequency(0.0)
   , mPolarization(WsfEM_Types::cPOL_DEFAULT)
   , mEBS_Mode(WsfEM_Antenna::cEBS_NONE)
   , mEBS_AzCosSteeringLimit(0.0)
   , mEBS_ElCosSteeringLimit(0.0)
   , mEBS_AzLossExponent(1.0)
   , mEBS_ElLossExponent(1.0)
   , mEBS_Az(0.0)
   , mEBS_El(0.0)
{
}

// virtual
bool AntennaPlotFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   mSimulationPtr                = &aSimulation;
   WsfAntennaPattern* patternPtr = WsfAntennaPatternTypes::Get(GetScenario()).Find(mPatternName);
   if (patternPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find antenna_pattern.";
      out.AddNote() << "Pattern: " << mPatternName;
      return false;
   }

   WsfEM_Antenna* antennaPtr = new WsfEM_Antenna();
   if (antennaPtr == nullptr)
   {
      ut::log::error() << "Unable to find setup antenna.";
      return false;
   }

   WsfEM_Rcvr* rcvrPtr = new WsfEM_Rcvr(WsfEM_Rcvr::cRF_UNDEFINED, antennaPtr);
   if (rcvrPtr == nullptr)
   {
      ut::log::error() << "Unable to find setup articulated part.";
      return false;
   }
   patternPtr->Initialize(mSimulationPtr); // ESA patterns require initialization, since we aren't initializing the rcvr
   rcvrPtr->SetAntennaPattern(patternPtr, mPolarization, mFrequency);
   rcvrPtr->GetAntenna()->SetEBS_Mode(mEBS_Mode);
   rcvrPtr->GetAntenna()->SetEBS_AzCosSteeringLimit(mEBS_AzCosSteeringLimit);
   rcvrPtr->GetAntenna()->SetEBS_ElCosSteeringLimit(mEBS_ElCosSteeringLimit);
   rcvrPtr->GetAntenna()->SetEBS_AzLossExponent(mEBS_AzLossExponent);
   rcvrPtr->GetAntenna()->SetEBS_ElLossExponent(mEBS_ElLossExponent);

   if (((mEBS_Mode == WsfEM_Antenna::cEBS_NONE) || (mEBS_Mode == WsfEM_Antenna::cEBS_ELEVATION)) && (mEBS_Az > 0.0))
   {
      auto out = ut::log::warning() << "Ignoring azimuth_steering_angle due to the electronic_beam_steering input "
                                       "being incompatible with this input value.";
      out.AddNote() << "Azimuth Steering Angle: " << mEBS_Az * UtMath::cDEG_PER_RAD << " deg";
      mEBS_Az = 0.0;
   }

   if (((mEBS_Mode == WsfEM_Antenna::cEBS_NONE) || (mEBS_Mode == WsfEM_Antenna::cEBS_AZIMUTH)) && (mEBS_El > 0.0))
   {
      auto out = ut::log::warning() << "Ignoring elevation_steering_angle due to the electronic_beam_steering input "
                                       "being incompatible with this input value.";
      out.AddNote() << "Elevation Steering Angle: " << mEBS_El * UtMath::cDEG_PER_RAD << " deg";
      mEBS_El = 0.0;
   }

   bool ok = false;
   if (mPlotType == 'b')
   {
      ok = PlotBoth(rcvrPtr);
   }
   else if (mPlotType == 'h')
   {
      ok = PlotHorizontal(rcvrPtr);
   }
   else if (mPlotType == 'v')
   {
      ok = PlotVertical(rcvrPtr);
   }
   else
   {
      auto out = ut::log::error() << "Bad plot type.";
      out.AddNote() << "Type: " << mPlotType;
   }
   return ok;
}

bool AntennaPlotFunction::PlotBoth(WsfEM_Rcvr* rcvrPtr)
{
   std::vector<double> rowValues;
   std::vector<double> colValues;
   std::vector<double> dataValues;

   for (double azRad = mAzimuthMin; azRad <= (mAzimuthMax + 0.01 * mAzimuthStep); azRad += mAzimuthStep)
   {
      double value = azRad * UtMath::cDEG_PER_RAD;
      if (fabs(value) < 1.0E-8)
      {
         value = 0.0;
      }
      rowValues.push_back(value);
      for (double elRad = mElevationMin; elRad <= (mElevationMax + 0.01 * mElevationStep); elRad += mElevationStep)
      {
         if (azRad == mAzimuthMin)
         {
            value = elRad * UtMath::cDEG_PER_RAD;
            if (fabs(value) < 1.0E-8)
            {
               value = 0.0;
            }
            colValues.push_back(value);
         }
         // Use 'min' to limit angles because they may creep slightly outside the limits because of numerical issues.
         double azAngle = std::min(azRad, mAzimuthMax);
         double elAngle = std::min(elRad - mTiltAngle, mElevationMax);

         if (dynamic_cast<WsfESA_AntennaPattern*>(rcvrPtr->GetAntennaPattern(mPolarization, mFrequency)) != nullptr)
         {
            azAngle -= mEBS_Az;
            elAngle -= mEBS_El;
         }

         double gain = rcvrPtr->GetAntennaGain(mPolarization, mFrequency, azAngle, elAngle, mEBS_Az, mEBS_El);
         value       = UtMath::SafeLinearToDB(gain);
         if (fabs(value) < 1.0E-8)
         {
            value = 0.0;
         }
         dataValues.push_back(value);
      }
   }

   if (!mOutputFile.empty())
   {
      { // RAII block
         auto out = ut::log::info() << "Writing output file.";
         out.AddNote() << "File: " << mOutputFile;
      }
      WritePlotFile(mOutputFile, rowValues, colValues, dataValues);
   }

   if (!mGnuPlotFile.empty())
   {
      { // RAII block
         auto out = ut::log::info() << "Writing GNU Plot file.";
         out.AddNote() << "File: " << mGnuPlotFile;
      }

      WriteGnuPlotFile(rowValues, colValues, dataValues);
   }

   return true;
}

bool AntennaPlotFunction::PlotHorizontal(WsfEM_Rcvr* rcvrPtr)
{
   std::ofstream ofs(mOutputFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mOutputFile;
      return false;
   }
   else
   {
      auto out = ut::log::info() << "Writing output file.";
      out.AddNote() << "File: " << mOutputFile;
   }

   ofs << "# " << mPatternName << " - horizontal plot" << std::endl;
   double elRad = 0.0;
   for (double azRad = mAzimuthMin; azRad <= (mAzimuthMax + 0.01 * mAzimuthStep); azRad += mAzimuthStep)
   {
      // Use 'min' to limit angles because they may creep slightly outside the limits because of numerical issues.
      double azAngle = std::min(azRad, mAzimuthMax);
      double elAngle = std::min(elRad - mTiltAngle, mElevationMax);

      if (dynamic_cast<WsfESA_AntennaPattern*>(rcvrPtr->GetAntennaPattern(mPolarization, mFrequency)) != nullptr)
      {
         azAngle -= mEBS_Az;
         elAngle -= mEBS_El;
      }

      double gain = rcvrPtr->GetAntennaGain(mPolarization, mFrequency, azAngle, elAngle, mEBS_Az, mEBS_El);
      gain        = UtMath::SafeLinearToDB(gain);
      if (fabs(gain) < 1.0E-8)
      {
         gain = 0.0;
      }
      double angle = azRad * UtMath::cDEG_PER_RAD;
      if (fabs(angle) < 1.0E-8)
      {
         angle = 0.0;
      }
      ofs << angle << ' ' << gain << std::endl;
   }
   return true;
}

bool AntennaPlotFunction::PlotVertical(WsfEM_Rcvr* rcvrPtr)
{
   std::ofstream ofs(mOutputFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mOutputFile;
      return false;
   }
   else
   {
      auto out = ut::log::info() << "Writing output file.";
      out.AddNote() << "File: " << mOutputFile;
   }

   ofs << "# " << mPatternName << " - vertical plot" << std::endl;
   double azRad = 0.0;
   for (double elRad = mElevationMin; elRad <= (mElevationMax + 0.01 * mElevationStep); elRad += mElevationStep)
   {
      // Use 'min' to limit angles because they may creep slightly outside the limits because of numerical issues.
      double azAngle = std::min(azRad, mAzimuthMax);
      double elAngle = std::min(elRad - mTiltAngle, mElevationMax);

      if (dynamic_cast<WsfESA_AntennaPattern*>(rcvrPtr->GetAntennaPattern(mPolarization, mFrequency)) != nullptr)
      {
         azAngle -= mEBS_Az;
         elAngle -= mEBS_El;
      }

      double gain = rcvrPtr->GetAntennaGain(mPolarization, mFrequency, azAngle, elAngle, mEBS_Az, mEBS_El);
      gain        = UtMath::SafeLinearToDB(gain);
      if (fabs(gain) < 1.0E-8)
      {
         gain = 0.0;
      }
      double angle = elRad * UtMath::cDEG_PER_RAD;
      if (fabs(angle) < 1.0E-8)
      {
         angle = 0.0;
      }
      ofs << angle << ' ' << gain << std::endl;
   }
   return true;
}

bool AntennaPlotFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "pattern_name")
   {
      aInput.ReadValue(mPatternName);
   }
   else if (command == "axes")
   {
      std::string axes;
      aInput.ReadValue(axes);
      if (axes == "vertical")
      {
         mPlotType = 'v';
      }
      else if (axes == "horizontal")
      {
         mPlotType = 'h';
      }
      else if (axes == "both")
      {
         mPlotType = 'b';
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "azimuth_range")
   {
      aInput.ReadValueOfType(mAzimuthMin, UtInput::cANGLE);
      aInput.ReadValueOfType(mAzimuthMax, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mAzimuthMin, -UtMath::cPI);
      aInput.ValueLessOrEqual(mAzimuthMax, UtMath::cPI);
      aInput.ValueGreaterOrEqual(mAzimuthMin, mAzimuthMin);
   }
   else if (command == "azimuth_step")
   {
      aInput.ReadValueOfType(mAzimuthStep, UtInput::cANGLE);
      aInput.ValueGreater(mAzimuthStep, 0.0);
   }
   else if (command == "elevation_range")
   {
      aInput.ReadValueOfType(mElevationMin, UtInput::cANGLE);
      aInput.ReadValueOfType(mElevationMax, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mElevationMin, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mElevationMax, UtMath::cPI_OVER_2);
      aInput.ValueGreaterOrEqual(mElevationMin, mElevationMin);
   }
   else if (command == "elevation_step")
   {
      aInput.ReadValueOfType(mElevationStep, UtInput::cANGLE);
      aInput.ValueGreater(mElevationStep, 0.0);
   }
   else if (command == "tilt_angle")
   {
      aInput.ReadValueOfType(mTiltAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mTiltAngle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "azimuth_steering_angle")
   {
      aInput.ReadValueOfType(mEBS_Az, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEBS_Az, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "elevation_steering_angle")
   {
      aInput.ReadValueOfType(mEBS_El, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEBS_El, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "electronic_beam_steering")
   {
      std::string ebsMode;
      aInput.ReadValue(ebsMode);
      if (ebsMode == "azimuth")
      {
         mEBS_Mode = WsfEM_Antenna::cEBS_AZIMUTH;
      }
      else if (ebsMode == "elevation")
      {
         mEBS_Mode = WsfEM_Antenna::cEBS_ELEVATION;
      }
      else if ((ebsMode == "both") || (ebsMode == "azimuth_and_elevation"))
      {
         mEBS_Mode = WsfEM_Antenna::cEBS_BOTH;
      }
      else if (ebsMode == "none")
      {
         mEBS_Mode = WsfEM_Antenna::cEBS_NONE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid value for electronic_beam_steering: " + ebsMode);
      }
   }
   else if (command == "electronic_beam_steering_limit")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_AzCosSteeringLimit = cos(steeringLimit);
      mEBS_ElCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_limit_azimuth")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_AzCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_limit_elevation")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_ElCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_loss_exponent")
   {
      aInput.ReadValue(mEBS_AzLossExponent);
      mEBS_ElLossExponent = mEBS_AzLossExponent;
   }
   else if (command == "electronic_beam_steering_loss_exponent_azimuth")
   {
      aInput.ReadValue(mEBS_AzLossExponent);
   }
   else if (command == "electronic_beam_steering_loss_exponent_elevation")
   {
      aInput.ReadValue(mEBS_ElLossExponent);
   }
   else if (command == "frequency")
   {
      aInput.ReadValueOfType(mFrequency, UtInput::cFREQUENCY);
      aInput.ValueGreater(mFrequency, 0.0);
   }
   else if (command == "polarization")
   {
      std::string polarizationStr;
      aInput.ReadValue(polarizationStr);
      if (!WsfEM_Util::StringToEnum(mPolarization, polarizationStr))
      {
         throw UtInput::BadValue(aInput, "Invalid polarization: " + polarizationStr);
      }
   }
   else if (command == "output_file")
   {
      aInput.ReadValueQuoted(mOutputFile);
      mOutputFile = aInput.SubstitutePathVariables(mOutputFile);
   }
   else if (command == "gnuplot_file")
   {
      aInput.ReadValueQuoted(mGnuPlotFile);
      mGnuPlotFile = aInput.SubstitutePathVariables(mGnuPlotFile);
   }
   else if (command == "header_line_1")
   {
      aInput.ReadLine(mHeaderLine1, false);
      if (!mHeaderLine1.empty())
      {
         mHeaderLine1 = mHeaderLine1.substr(1);
      }
   }
   else if (command == "header_line_2")
   {
      aInput.ReadLine(mHeaderLine2, false);
      if (!mHeaderLine2.empty())
      {
         mHeaderLine2 = mHeaderLine2.substr(1);
      }
   }
   else if (command == "header_line_3")
   {
      aInput.ReadLine(mHeaderLine3, false);
      if (!mHeaderLine3.empty())
      {
         mHeaderLine3 = mHeaderLine3.substr(1);
      }
   }
   else if (command == "output_column_limit")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreater(value, 0);
      mOutputColumnLimit = static_cast<size_t>(value);
   }
   else if (Function::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Write the output file.
void AntennaPlotFunction::WritePlotFile(const std::string&         aFileName,
                                        const std::vector<double>& aRowValues,
                                        const std::vector<double>& aColValues,
                                        const std::vector<double>& aDataValues)
{
   std::ofstream ofs(aFileName);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << aFileName;
      return;
   }

   size_t rowCount = aRowValues.size();
   size_t colCount = aColValues.size();

   ofs << mHeaderLine1 << '\n';
   ofs << mHeaderLine2 << '\n';
   ofs << mHeaderLine3 << '\n';
   ofs << "   " << rowCount << "  " << colCount << '\n';

   size_t rowIndex;
   size_t colIndex;
   size_t itemsOnLine;
   ofs << "          ";
   itemsOnLine = 0;
   for (colIndex = 0; colIndex < colCount; ++colIndex)
   {
      if (itemsOnLine >= mOutputColumnLimit)
      {
         itemsOnLine = 0;
         ofs << "\n          ";
      }
      ofs << ' ' << std::setw(10) << aColValues[colIndex];
      ++itemsOnLine;
   }
   ofs << '\n';

   size_t rowColIndex = 0;
   for (rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      ofs << std::setw(10) << aRowValues[rowIndex];
      itemsOnLine = 0;
      for (colIndex = 0; colIndex < colCount; ++colIndex)
      {
         if (itemsOnLine >= mOutputColumnLimit)
         {
            itemsOnLine = 0;
            ofs << "\n          ";
         }
         ofs << ' ' << std::setw(10) << aDataValues[rowColIndex];
         ++itemsOnLine;
         ++rowColIndex;
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Antenna Plot", mOutputFile);
}

//! Write the output file.
void AntennaPlotFunction::WriteGnuPlotFile(const std::vector<double>& aRowValues,
                                           const std::vector<double>& aColValues,
                                           const std::vector<double>& aDataValues)
{
   std::ofstream ofs(mGnuPlotFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mGnuPlotFile;
      return;
   }

   size_t rowCount = aRowValues.size();
   size_t colCount = aColValues.size();

   if (!mHeaderLine1.empty())
   {
      ofs << "# " << mHeaderLine1 << '\n';
   }
   if (!mHeaderLine2.empty())
   {
      ofs << "# " << mHeaderLine2 << '\n';
   }
   if (!mHeaderLine3.empty())
   {
      ofs << "# " << mHeaderLine3 << '\n';
   }

   // clang-format off
   ofs << "# plot using: \n"
       << "#\n"
       //       << "# set contour\n"
       //       << "# set cntrparam levels auto 10\n"
       << "# unset surface\n"
       << "# set pm3d\n"
       << "# set view 0,0\n"
       << "# set zrange [-299:299] #ignore hard limits\n"
       << "# set xlabel \"Azimuth Angle\"\n"
       << "# set ylabel \"Elevation Angle\"\n"
       << "# splot \"" << mGnuPlotFile << "\" with pm3d\n";
   // clang-format on

   ofs << "#\n";
   ofs << "# Column 1: Azimuth Angle\n";
   ofs << "# Column 2: Elevation Angle\n";

   size_t rowIndex;
   size_t colIndex;
   size_t rowColIndex = 0;
   for (rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      for (colIndex = 0; colIndex < colCount; ++colIndex)
      {
         ofs << std::setw(10) << aRowValues[rowIndex];
         ofs << ' ' << std::setw(10) << aColValues[colIndex];
         ofs << ' ' << std::setw(10) << aDataValues[rowColIndex];
         ofs << "\n";
         ++rowColIndex;
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("GNU Plot", mGnuPlotFile);
}
