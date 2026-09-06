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

#include "DisBeam.hpp"

#include <limits>

#include "Dis.hpp"
#include "DisBaseSizes.hpp"
#include "DisBeamEnums.hpp"
#include "DisEmission.hpp"
#include "DisJammingTechniqueEnums.hpp"
#include "DisSystem.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMath.hpp"

namespace
{
// Size in octets
const DisUint16 sDisBeamTargetSize = 8;
} // namespace

DisBeam::DisBeam()
   : mReportedDataLength(0)
   , mNumber(0)
   , mParameterIndex(0)
   , mFrequency(0.0F)
   , mFrequencyRange(0.0F)
   , mEffectiveRadiatedPower(0.0F)
   , mPulseRepetitionFrequency(0.0F)
   , mPulseWidth(0.0F)
   , mFunction(DisEnum::beam::function::Other)
   , mReportedNumberOfTargets(0)
   , mHighDensityTrackJam(DisEnum::beam::hdtj::NotSelected)
   , mStatus(DisEnum::beam::status::Active)
   , mJammingTechniqueRecord()
   , mTargetSet()
   , mTrackJamList()
   , mLengthRead(0)
   , mParentSystem(nullptr)
{
}

DisBeam::DisBeam(const DisBeam& aSrc)
   : mReportedDataLength(aSrc.mReportedDataLength)
   , mNumber(aSrc.mNumber)
   , mParameterIndex(aSrc.mParameterIndex)
   , mFrequency(aSrc.mFrequency)
   , mFrequencyRange(aSrc.mFrequencyRange)
   , mEffectiveRadiatedPower(aSrc.mEffectiveRadiatedPower)
   , mPulseRepetitionFrequency(aSrc.mPulseRepetitionFrequency)
   , mPulseWidth(aSrc.mPulseWidth)
   , mBeamDataRecord(aSrc.mBeamDataRecord)
   , mFunction(aSrc.mFunction)
   , mReportedNumberOfTargets(aSrc.mReportedNumberOfTargets)
   , mHighDensityTrackJam(aSrc.mHighDensityTrackJam)
   , mStatus(aSrc.mStatus)
   , mJammingTechniqueRecord(aSrc.mJammingTechniqueRecord)
   , mTargetSet(aSrc.mTargetSet)
   , mTrackJamList(aSrc.mTrackJamList)
   , mLengthRead(aSrc.mLengthRead)
   , mParentSystem(nullptr) // Do not copy parent pointer!
{
}

DisUint8 DisBeam::GetDataLength() const
{
   // convert to 32-bit words
   DisUint16 length = GetLengthOctets() / 4;
   return length > 255 ? DisUint8(0) : DisUint8(length);
}

DisUint16 DisBeam::GetLengthOctets() const
{
   // Include the track-jam list in the length only if high-density track-jam
   // is not selected.
   //
   // Note: This check must be consistent with the check in Put().
   if (mHighDensityTrackJam == DisEnum::beam::hdtj::Selected)
   {
      return static_cast<DisUint16>(cBASE_BEAM_SIZE);
   }

   DisUint16 octetCount = cBASE_BEAM_SIZE + (GetNumberOfTargets() * sDisBeamTargetSize);
   return octetCount;
}

bool DisBeam::IsActive() const
{
   return ((mStatus & 0x01) == DisEnum::beam::status::Active);
}

void DisBeam::SetStatus(DisEnum8 aStatus)
{
   mStatus = aStatus;
   if (!IsActive())
   {
      mTargetSet.clear();
      mTrackJamList.RemoveAll();
   }
}

void DisBeam::Get(GenI& aGenI)
{
   mTargetSet.clear();
   mTrackJamList.RemoveAll();

   aGenI >> mReportedDataLength;
   aGenI >> mNumber;
   aGenI >> mParameterIndex;
   aGenI >> mFrequency;
   aGenI >> mFrequencyRange;
   aGenI >> mEffectiveRadiatedPower;
   aGenI >> mPulseRepetitionFrequency;
   aGenI >> mPulseWidth;
   aGenI >> mBeamDataRecord;
   aGenI >> mFunction;
   aGenI >> mReportedNumberOfTargets;
   aGenI >> mHighDensityTrackJam;
   aGenI >> mStatus;
   aGenI >> mJammingTechniqueRecord;
   mLengthRead = static_cast<DisUint16>(cBASE_BEAM_SIZE);

   mTrackJamList.Reserve(mReportedNumberOfTargets);
   for (int i = 0; i < mReportedNumberOfTargets; ++i)
   {
      DisTrackJam newTarget;
      aGenI >> newTarget;
      mTargetSet.insert(newTarget);
      mLengthRead += 8;

      // To keep legacy interface working.  Delete these 2 lines when deprecated
      // functionality is removed.
      DisTrackJam* trackJam = new DisTrackJam(newTarget);
      mTrackJamList.Add(trackJam); // List takes ownership
   }

   if (mReportedDataLength > 0)
   {
      // Read and ignore any 'extra' data...
      DisUint16 extraOctets = (mReportedDataLength * 4) - mLengthRead;

      // Note: it is legal for mReportedDataLength to be 0 for large Beams. So only
      // adjust the amount we have read if the extra octets count is positive.
      mLengthRead += extraOctets;
      while (extraOctets > 0)
      {
         DisUint8 tempOctet;
         aGenI >> tempOctet;
         --extraOctets;
      }
   }
}

void DisBeam::Put(GenO& aGenO) const
{
   aGenO << GetDataLength();
   aGenO << mNumber;
   aGenO << mParameterIndex;
   if (IsActive())
   {
      aGenO << mFrequency;
      aGenO << mFrequencyRange;
      aGenO << mEffectiveRadiatedPower;
      aGenO << mPulseRepetitionFrequency;
      aGenO << mPulseWidth;
      aGenO << mBeamDataRecord;
   }
   else
   {
      // Beam is deactivated, zero out key values
      aGenO << 0.0F; // mFrequency
      aGenO << 0.0F; // mFrequencyRange
      aGenO << 0.0F; // mEffectiveRadiatedPower
      aGenO << 0.0F; // mPulseRepetitionFrequency
      aGenO << 0.0F; // mPulseWidth
      aGenO << 0.0F; // mAzimuthCenter
      aGenO << 0.0F; // mAzimuthSweep
      aGenO << 0.0F; // mElevationCenter
      aGenO << 0.0F; // mElevationSweep
      aGenO << 0.0F; // mSweepSync
   }
   aGenO << mFunction;
   // Only count track jam as part of the size if it is within the threshold.
   DisUint8 numTargets = GetNumberOfTargets();
   aGenO << numTargets;
   aGenO << GetHighDensityTrackJam();
   aGenO << mStatus;
   aGenO << mJammingTechniqueRecord;

   if (numTargets > 0)
   {
      for (const auto& it : mTargetSet)
      {
         aGenO << it;
      }
   }
}

void DisBeam::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisBeam::ToString() const
{
   using namespace std;
   using namespace DisEnum;
   std::ostringstream ss("");

   static const string cUNITS[] = {" Hz)", " KHz)", " MHz)", " GHz)", " THz)"};

   float        frequency  = GetFrequency();
   float        freq_short = frequency;
   unsigned int freq_units = 0;
   while (freq_short > 1000.0F && freq_units < 5)
   {
      freq_short /= 1000.0F;
      ++freq_units;
   }
   float        range       = GetFrequencyRange();
   float        range_short = range;
   unsigned int range_units = 0;
   while (range_short > 1000.0F && freq_units < 5)
   {
      range_short /= 1000.0F;
      ++range_units;
   }
   float        prf       = GetPulseRepetitionFrequency();
   float        prf_short = prf;
   unsigned int prf_units = 0;
   while (prf_short > 1000.0F && prf_units < 5)
   {
      prf_short /= 1000.0F;
      ++prf_units;
   }
   DisUint16 beamNumber = GetNumber();
   ss << "***** Beam " << beamNumber << " Data *****" << '\n'
      << "Beam Data Length:     " << static_cast<DisUint16>(GetDataLength());
   if (GetDataLength() > 0)
   {
      ss << " (32-bit words)\n";
   }
   else
   {
      ss << " (Large Beam)\n";
   }
   ss << "Beam Number:          " << beamNumber << '\n'
      << "Beam Parameter Index: " << GetParameterIndex() << '\n'
      << "***** Fundamental Parameter Data *****" << '\n';
   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Frequency:                        " << fixed << setprecision(0) << frequency << " Hz";
   if (freq_units > 0)
   {
      ss << " (" << setprecision(3) << freq_short << cUNITS[freq_units];
   }
   ss << '\n';
   ss << "Frequency Range:                  " << fixed << setprecision(0) << frequency << " Hz";
   if (range_units > 0)
   {
      ss << " (" << setprecision(3) << range_short << cUNITS[range_units];
   }
   ss << '\n';
   float power_dBm = GetEffectiveRadiatedPower();
   float power_W   = (float)pow(10.0, ((power_dBm - 30.0) / 10.0));
   ss << "Effective Radiated Power (ERP):   " << fixed << setprecision(3) << power_dBm << " dBm (" << setprecision(3)
      << power_W << " W)\n";

   ss << "Pulse Repetition Frequency (PRF): " << fixed << setprecision(0) << prf << " Hz";
   if (prf_units > 0)
   {
      ss << " (" << setprecision(3) << prf_short << cUNITS[prf_units];
   }
   static const float LOW_PRF_UPPER_LIMIT = 3000.0F;  // 3 KHz
   static const float MED_PRF_UPPER_LIMIT = 30000.0F; // 30 KHz
   if (prf == 0.0F)
   {
      if (GetPulseWidth() == 0.0F)
      {
         ss << "(Continuous Wave)";
      }
      ss << '\n';
   }
   else if (prf > 0.0F && prf < LOW_PRF_UPPER_LIMIT)
   {
      // Low PRF is less than 3 kHz
      ss << " (Low PRF)\n";
   }
   else if (prf >= LOW_PRF_UPPER_LIMIT && prf < MED_PRF_UPPER_LIMIT)
   {
      // Medium PRF is between 3 kHz and 30 kHz
      ss << " (Medium PRF)\n";
   }
   else if (prf >= MED_PRF_UPPER_LIMIT)
   {
      // High PRF is above 30 kHz
      ss << " (High PRF)\n";
   }
   else
   {
      ss << '\n';
   }

   ss << "Pulse Width:                      " << fixed << setprecision(1) << GetPulseWidth() << " microseconds\n";
   // Max Unambiguous Range
   //   RMax = c / (2*PRF)
   if (prf > 0.0F)
   {
      ss << "***** Derived Parameters (from PRF and Frequency) *****\n";
      const float C         = 299792458.0F; // Speed of Light in meters per second
      float       max_range = 0.5F * C / prf;
      ss << "Max Unambiguous Range:            " << max_range << " m (" << max_range / UtMath::cM_PER_NM << " nmile)\n";
      // Max Unambiguous Radial Velocity
      //   VMax = (PRF * C)/(2 * F)
      if (frequency > 0.0F)
      {
         float max_vel = 0.5F * prf * C / frequency;
         ss << "Max Unambiguous Radial Velocity:  " << max_vel << " m/s (" << max_vel * UtMath::cNMPH_PER_MPS
            << " knots)\n";
      }
   }
   ss << "***** Beam Data Record *****\n"
      << "Beam Azimuth Center:   " << fixed << setprecision(3) << GetAzimuthCenter() << " (" << fixed << setprecision(1)
      << GetAzimuthCenter() * UtMath::cDEG_PER_RAD << " deg)\n"
      << "Beam Azimuth Sweep:    " << fixed << setprecision(3) << GetAzimuthSweep() << " (" << fixed << setprecision(1)
      << GetAzimuthSweep() * UtMath::cDEG_PER_RAD << " deg)\n"
      << "Beam Elevation Center: " << fixed << setprecision(3) << GetElevationCenter() << " (" << fixed
      << setprecision(1) << GetElevationCenter() * UtMath::cDEG_PER_RAD << " deg)\n"
      << "Beam Elevation Sweep:  " << fixed << setprecision(3) << GetElevationSweep() << " (" << fixed
      << setprecision(1) << GetElevationSweep() * UtMath::cDEG_PER_RAD << " deg)\n"
      << "Beam Sweep Sync:       " << fixed << setprecision(1) << GetSweepSync() << " % (of frame)\n"
      << "***** Beam Data Record End *****\n";
   ss.imbue(tmp);
   ss << "Beam Function:          " << static_cast<DisUint16>(GetFunction()) << " ("
      << DisEnum::beam::function::ToString(GetFunction()) << ")\n"
      << "Number of Targets:      " << static_cast<DisUint16>(GetNumberOfTargets()) << '\n'
      << "High-Density Track/Jam: " << DisEnum::beam::hdtj::ToString(GetHighDensityTrackJam()) << '\n'
      << "Beam Status:            " << DisEnum::beam::status::ToString(GetStatus()) << '\n'
      << "Jamming Technique:      " << DisEnum::jamming_technique::ToString(GetJammingTechniqueRecord()) << '\n';

   typedef vector<DisTrackJam> TJVec;
   TJVec                       v = GetTargetVector();
   if (v.size() > 0 && GetHighDensityTrackJam() == DisEnum::beam::hdtj::Selected)
   {
      ss << "Dis Track/Jam List will not be sent due to High-Density "
         << "Track/Jam.\nDetailed list included for informational "
         << "purposes.\n";
   }
   for (const auto& itr : v)
   {
      ss << itr.ToString() << '\n';
   }
   ss << "***** Beam " << beamNumber << " Data End *****";
   return ss.str();
}

bool DisBeam::CanAddTrackJamEntry(const DisSystem* aSystemPtr, const DisEmission* aEmissionPtr) const
{
   if (!IsActive())
   {
      // Not an active beam, can't add a target
      return false;
   }
   if (GetHighDensityTrackJam() == DisEnum::beam::hdtj::Selected)
   {
      // Not reporting targets
      return false;
   }
   if (GetNumberOfTargets() == Dis::GetEE_HIGH_DENSITY_THRSH())
   {
      // No more room
      return false;
   }
   // Calculate Size limit
   DisUint32 max_size = Dis::GetMAX_PDU_SIZE_OCTETS();
   if (!aSystemPtr && mParentSystem)
   {
      aSystemPtr = mParentSystem;
      // Don't factor in size of this beam already in the current system
      max_size += GetLengthOctets();
   }
   if (!aEmissionPtr && aSystemPtr && aSystemPtr->GetParentEmission())
   {
      aEmissionPtr = aSystemPtr->GetParentEmission();
      // Don't factor in size of this system already in the current Emission
   }

   if (aEmissionPtr)
   {
      // Use Current PDU size
      max_size -= aEmissionPtr->GetLengthOctets();
   }
   else if (aSystemPtr)
   {
      // Use Base Emission PDU size plus Current System Size
      max_size -= cBASE_EMISSION_SIZE;
      max_size -= aSystemPtr->GetLengthOctets();
   }
   else
   {
      max_size -= cBASE_EMISSION_SIZE;
      max_size -= cBASE_SYSTEM_SIZE; // best case, only 1 system
   }
   // Subtract out this current beam's size
   max_size -= GetLengthOctets();
   // True if we have room for another Target.
   return (max_size >= sDisBeamTargetSize);
}

bool DisBeam::IsValid() const
{
   // Allow a floating point error epsilon to limits.
   static const float ZERO_LIMIT = std::numeric_limits<float>::epsilon();

   if (mNumber == Dis::cNO_BEAM || mNumber == Dis::cALL_BEAMS)
   {
      // Beam Number is required to be a single beam
      return false;
   }
   if (!ValidateScalar(mFrequency) || mFrequency < -ZERO_LIMIT)
   {
      // Frequencies must be positive
      return false;
   }
   if (!ValidateScalar(mFrequencyRange) || mFrequencyRange < -ZERO_LIMIT)
   {
      // Frequencies must be positive
      return false;
   }
   if (!ValidateScalar(mEffectiveRadiatedPower))
   {
      // Make sure power is a finite number
      return false;
   }
   if (!ValidateScalar(mPulseRepetitionFrequency) || mPulseRepetitionFrequency < -ZERO_LIMIT)
   {
      // Frequencies must be positive
      return false;
   }
   if (!ValidateScalar(mPulseWidth) || mPulseWidth < -ZERO_LIMIT)
   {
      // Time length measurments must be positive
      return false;
   }
   static const float SEC_TO_USEC = 1.0e6F;
   if (mPulseRepetitionFrequency > ZERO_LIMIT && mPulseWidth > ZERO_LIMIT &&
       mPulseWidth >= (SEC_TO_USEC / mPulseRepetitionFrequency))
   {
      // Bad Pulse Width (greater than 1/PRF)
      return false;
   }
   if (!mBeamDataRecord.IsValid())
   {
      return false;
   }
   if (!DisEnum::beam::function::IsValid(mFunction))
   {
      return false;
   }
   if (!DisEnum::beam::hdtj::IsValid(mHighDensityTrackJam))
   {
      return false;
   }
   if (!DisEnum::beam::status::IsValid(mStatus))
   {
      return false;
   }
   // Some areas are using different enums from before the standard defined them
   // so don't check these for validity at this time.
   // if(!mJammingTechniqueRecord.IsValid())
   // {
   //    return false;
   // }

   typedef std::set<DisTrackJam> TJSet;
   for (TJSet::const_iterator itr = mTargetSet.begin(); itr != mTargetSet.end(); ++itr)
   {
      if (!itr->IsValid())
      {
         return false;
      }
   }
   return true;
}

DisUint8 DisBeam::GetNumberOfTargets() const
{
   if (!IsActive())
   {
      return static_cast<DisUint8>(0);
   }
   if (mHighDensityTrackJam == DisEnum::beam::hdtj::Selected)
   {
      return static_cast<DisUint8>(0);
   }
   if (mTargetSet.size() > Dis::GetEE_HIGH_DENSITY_THRSH())
   {
      return static_cast<DisUint8>(0);
   }
   return (DisUint8)mTargetSet.size();
}

DisEnum8 DisBeam::GetHighDensityTrackJam() const
{
   if (!IsActive())
   {
      return static_cast<DisEnum8>(DisEnum::beam::hdtj::NotSelected);
   }
   // mTargetSet.size() equals GetNumberOfTargets(), if Track/Jam list is within
   // publishable limits
   if (mTargetSet.size() == GetNumberOfTargets())
   {
      return mHighDensityTrackJam;
   }
   return static_cast<DisEnum8>(DisEnum::beam::hdtj::Selected);
}
