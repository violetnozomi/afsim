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

#include "WsfAntennaPattern.hpp"

#include <algorithm>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

const char* WsfAntennaPattern::cTYPE_KIND = "antenna_pattern";

// =================================================================================================
WsfAntennaPattern::WsfAntennaPattern()
   : WsfObject()
   , mSharedDataPtr(new BaseData)
{
}

// =================================================================================================
WsfAntennaPattern::WsfAntennaPattern(BaseData* aBasePtr)
   : WsfObject()
   , mSharedDataPtr(aBasePtr)
{
}

// =================================================================================================
//! Copy Constructor for Clone().
// protected
WsfAntennaPattern::WsfAntennaPattern(const WsfAntennaPattern& aSrc)
   : WsfObject(aSrc)
   , mSharedDataPtr(aSrc.mSharedDataPtr)
{
   mSharedDataPtr->AddRef();
}

// =================================================================================================
// virtual
WsfAntennaPattern::~WsfAntennaPattern()
{
   mSharedDataPtr->Unref();
}

// =================================================================================================
// virtual
WsfAntennaPattern* WsfAntennaPattern::Clone() const
{
   return new WsfAntennaPattern(*this);
}

// =================================================================================================
//! Return the antenna gain at a specified azimuth and elevation.
//!
//! @param aFrequency The frequency at which to get the gain (Hz).
//! @param aTargetAz  Target azimuth with respect to the gain pattern boresight (radians).
//! @param aTargetEl  Target elevation with respect to the gain pattern boresight (radians).
//! @param aEBS_Az    The Electronic Beam Steering azimuth angle (radians).
//! @param aEBS_El    The Electronic Beam Steering elevation angle (radians).
//! @return The gain multiplier (NOT in dB!)
// virtual
double WsfAntennaPattern::GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El)
{
   return mSharedDataPtr->GetGain(aFrequency, aTargetAz, aTargetEl, aEBS_Az, aEBS_El);
}

// =================================================================================================
//! Return the electronically steered beamwidth.
//! @param aBeamwidth The beamwidth (radians).
//! @param aEBS_Az The azimuth's electronic beamwidth steering (radians).
//! @param aEBS_El The elevation's electronic beamwidth steering (radians).
//! @return The steered beamwidth (radians)
// virtual
double WsfAntennaPattern::ApplyEBS(double aBeamwidth, double aEBS_Az, double aEBS_El) const
{
   if (aEBS_Az != 0.0)
   {
      double EBS_Effect = std::cos(aEBS_Az);
      if (EBS_Effect > 0.0)
      {
         aBeamwidth /= EBS_Effect;
      }
   }
   if (aEBS_El != 0.0)
   {
      double EBS_Effect = std::cos(aEBS_El);
      if (EBS_Effect > 0.0)
      {
         aBeamwidth /= EBS_Effect;
      }
   }

   return aBeamwidth;
}

// =================================================================================================
//! Return the azimuth beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @return The azimuth beamwidth (radians)
// virtual
double WsfAntennaPattern::GetAzimuthBeamwidth(double aFrequency) const
{
   return GetAzimuthBeamwidth(aFrequency, 0.0, 0.0);
}

// =================================================================================================
//! Return the azimuth beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return The azimuth beamwidth (radians)
// virtual
double WsfAntennaPattern::GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(1.0 * UtMath::cRAD_PER_DEG, aEBS_Azimuth, 0.0);
}

// =================================================================================================
//! Return the elevation beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @return The azimuth beamwidth (radians)
// virtual
double WsfAntennaPattern::GetElevationBeamwidth(double aFrequency) const
{
   return GetElevationBeamwidth(aFrequency, 0.0, 0.0);
}

// =================================================================================================
//! Return the elevation beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return The azimuth beamwidth (radians)
// virtual
double WsfAntennaPattern::GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(1.0 * UtMath::cRAD_PER_DEG, 0.0, aEBS_Elevation);
}

// =================================================================================================
//! Return the minimum gain that will be returned by GetAntennaGain
//! @return The peak gain (as an absolute ratio, NOT dB)
// virtual
double WsfAntennaPattern::GetMinimumGain() const
{
   return mSharedDataPtr->mMinimumGain;
}

// =================================================================================================
//! Return the peak gain in the pattern at the specified frequency.
//! @param aFrequency The frequency in Hz.
//! @return The peak gain (as an absolute ratio, NOT dB)
// virtual
double WsfAntennaPattern::GetPeakGain(double aFrequency) const
{
   return 1.0;
}

// =================================================================================================
//! Return the simple gain adjustment
//! @return the simple gain adjustment to be used if the gain adjustment table is empty.
// virtual
double WsfAntennaPattern::GetGainAdjustment() const
{
   return mSharedDataPtr->mGainAdjustment;
}

// =================================================================================================
//! Return the gain adjustment table.
//! @return The gain adjustment table (which may be empty).
// virtual
const WsfAntennaPattern::GainAdjustmentTable& WsfAntennaPattern::GetGainAdjustmentTable() const
{
   return mSharedDataPtr->mGainAdjustmentTable;
}

// =================================================================================================
//! Initialize the antenna pattern.
//! This is called after all the input for an antenna pattern has been processed.
//! @param aSimulationPtr   Pointer to the simulation.
//!                         May be null if the antenna pattern is being checked for validity before
//!                         addition to the simulation.
//! @returns true if successful or false if not.
// virtual
bool WsfAntennaPattern::Initialize(WsfSimulation* aSimulationPtr)
{
   if (!mSharedDataPtr->mInitialized)
   {
      return mSharedDataPtr->Initialize(*this);
   }
   return true;
}

// =================================================================================================
// virtual
bool WsfAntennaPattern::ProcessInput(UtInput& aInput)
{
   return mSharedDataPtr->ProcessInput(*this, aInput);
}

// =================================================================================================
//! Get the fraction of the antenna pattern that meets or exceeds a certain gain threshold.
//!
//! This is a utility method used by the passive type sensor code to ESTIMATE the amount of
//! a azimuth scan pattern that exceeds specified gain threshold.
//!
//! @param aGainThreshold The gain threshold (absolute, not dB).
//! @param aPeakGain      The peak gain for the elevation in question (absolute, not dB).
//! This would typically be the value returned by GetGain(freq,0.0,el,0.0,0.0).
//! @param aMinAz         The minimum azimuth to consider [ -pi, pi ]
//! @param aMaxAz         The maximum azimuth to consider [ -pi, pi ]
//! @param aFrequency     The frequency (only needed for the steered array pattern)
//!
//! @returns The fraction of the scan pattern that meets or exceeds the gain threshold.
// static
double WsfAntennaPattern::GetGainThresholdFraction(double aGainThreshold,
                                                   double aPeakGain,
                                                   double aMinAz,
                                                   double aMaxAz,
                                                   double aFrequency)
{
   // Check to see if the threshold is greater than the peak gain. If it is then there
   // is no possibility of any of the pattern meeting the threshold. A little slop is
   // provided to address numerical issues when the threshold is very near the peak.

   double relGain = aGainThreshold / aPeakGain;
   if (relGain > 1.00001)
   {
      return 0.0;
   }
   double threshold = std::min(aGainThreshold, aPeakGain);

   mSharedDataPtr->InitializeAverageGain(aFrequency);

   // Determine the bins that need to be examined.
   //
   // Note that the bins are centered about integral angles. The extent of a bin is [ x-0.5, x+0.5).
   // (The upper limit does not include the endpoint!).

   int minAzIndex = static_cast<int>(aMinAz * UtMath::cDEG_PER_RAD + 180.500000);
   int maxAzIndex = static_cast<int>(aMaxAz * UtMath::cDEG_PER_RAD + 180.499999);
   minAzIndex     = std::min(std::max(minAzIndex, 0), 360);
   maxAzIndex     = std::min(std::max(maxAzIndex, 0), 360);

   // Determine the number of bins above the threshold.

   double gainScale = std::min(aPeakGain / mSharedDataPtr->mSampledPeakGain, 1.0);
   double minGain   = mSharedDataPtr->mMinimumGain;

   int count = 0;
   for (int azIndex = minAzIndex; azIndex <= maxAzIndex; ++azIndex)
   {
      double avgGain = gainScale * mSharedDataPtr->mAvgGain[azIndex];
      avgGain        = std::max(avgGain, minGain);
      if (avgGain >= threshold)
      {
         ++count;
      }
   }

   return static_cast<double>(count) / static_cast<double>(maxAzIndex - minAzIndex + 1);
}


// =================================================================================================
// Nested class 'BaseData'.
// =================================================================================================
WsfAntennaPattern::BaseData::BaseData()
   : mMinimumGain(1.0E-30)
   , mGainAdjustment(1.0)
   , mGainAdjustmentTable()
   , mInitialized(false)
   , mAvgGainInitialized(false)
   , mShowAvgGain(false)
   , mAvgGainMutex()
   , mAvgGain()
   , mSampledPeakGain(-1.0E+30)
{
}

// =================================================================================================
// virtual
bool WsfAntennaPattern::BaseData::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "minimum_gain")
   {
      aInput.ReadValueOfType(mMinimumGain, UtInput::cRATIO);
      aInput.ValueGreater(mMinimumGain, 0.0);
   }
   else if (command == "gain_adjustment")
   {
      aInput.ReadValueOfType(mGainAdjustment, UtInput::cRATIO);
   }
   else if (command == "gain_adjustment_table")
   {
      // The table is adjustment(db) .vs. log10(frequency)
      std::vector<double> frequencies;
      std::vector<double> adjustments;
      UtInputBlock        inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "frequency")
         {
            double frequency;
            double adjustment;
            aInput.ReadValueOfType(frequency, UtInput::cFREQUENCY);
            aInput.ReadValueOfType(adjustment, UtInput::cRATIO);
            aInput.ValueGreater(frequency, 0.0);
            aInput.ValueGreater(adjustment, 0.0);
            frequency  = log10(frequency);
            adjustment = UtMath::LinearToDB(adjustment);
            if ((!frequencies.empty()) && (frequency <= frequencies.back()))
            {
               throw UtInput::BadValue(aInput, "entries must be in order of ascending frequency");
            }
            frequencies.push_back(frequency);
            adjustments.push_back(adjustment);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (frequencies.size() == 1)
      {
         throw UtInput::BadValue(aInput, "At least two entries must be given");
      }
      mGainAdjustmentTable.mFrequency.SetValues(frequencies);
      mGainAdjustmentTable.mAdjustment.SetValues(adjustments);
   }
   else if (command == "show_average_gain_table")
   {
      mShowAvgGain = true;
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfAntennaPattern::BaseData::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   mInitialized = true;
   if (mShowAvgGain)
   {
      InitializeAverageGain(0.0);
   }
   return mInitialized;
}

// =================================================================================================
// virtual
double WsfAntennaPattern::BaseData::GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El)
{
   double gain = 1.0;
   return PerformGainAdjustment(aFrequency, gain);
}

// =================================================================================================
// virtual
double WsfAntennaPattern::BaseData::PerformGainAdjustment(double aFrequency, double aGain)
{
   double tableGainAdjustment = 1.0;
   if (mGainAdjustmentTable.mFrequency.GetSize() >= 2)
   {
      // The table is adjustment(db) .vs. log10(frequency)
      TblLookupLU<double> frequency;
      frequency.Lookup(mGainAdjustmentTable.mFrequency, log10(std::max(aFrequency, 1.0E-37)));
      tableGainAdjustment = TblEvaluate(mGainAdjustmentTable.mAdjustment, frequency);
      tableGainAdjustment = UtMath::DB_ToLinear(tableGainAdjustment);
   }
   // Apply scalar and tabular gain adjustments and prevent from going below the lower bound
   return std::max(aGain * mGainAdjustment * tableGainAdjustment, mMinimumGain);
}

// =================================================================================================
//! Initialize the average gain table if not already initialized.
void WsfAntennaPattern::BaseData::InitializeAverageGain(double aFrequency)
{
   if (mAvgGainInitialized)
   {
      return;
   }

   std::lock_guard<std::recursive_mutex> lock(mAvgGainMutex);
   if (!mAvgGainInitialized)
   {
      // Sample the pattern every 0.05 deg to generate RMS averages within a 1 degree window

      mAvgGain.resize(361); // -180 -> 180 by 1.
      double peakGain = -1.0E+30;
      for (int intAzDeg = -180; intAzDeg <= 180; ++intAzDeg)
      {
         double minAzDeg = std::max(intAzDeg - 0.5, -180.0);
         double maxAzDeg = std::min(intAzDeg + 0.5, 180.0);
         double sum      = 0.0;
         int    count    = 0;
         for (double azDeg = minAzDeg; azDeg <= maxAzDeg; azDeg += 0.05)
         {
            double az   = azDeg * UtMath::cRAD_PER_DEG;
            double gain = GetGain(aFrequency, az, 0.0, 0.0, 0.0);
            peakGain    = std::max(peakGain, gain);
            sum += (gain * gain);
            ++count;
         }
         double avgGain           = sqrt(sum / count);
         mAvgGain[intAzDeg + 180] = avgGain;
      }

      // Save off the peak gain of the sampled pattern (needed for eventual scaling);
      mSampledPeakGain = peakGain;

      mAvgGainInitialized = true; // Must be done AFTER the table is built.

      if (mShowAvgGain)
      {
         // Should be debug?
         auto out = ut::log::info() << "Average gain:";
         for (size_t i = 0; i < mAvgGain.size(); ++i)
         {
            out.AddNote() << static_cast<int>(i) - 180 << ": " << UtMath::LinearToDB(mAvgGain[i]);
         }
      }
   }
}
