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

#include "WsfSAM_LaunchComputer.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"

namespace
{
const double cNOT_VALID = -1.0;

const double cEPSILON = 0.1;
bool         IsEqual(double a, double b)
{
   return fabs(a - b) < cEPSILON;
}

void Add(std::vector<double>& aValues, double aNewValue)
{
   if (aValues.empty())
   {
      aValues.push_back(aNewValue);
      return;
   }

   for (auto i = aValues.begin(); i != aValues.end(); ++i)
   {
      // if same, no need to add the value.
      if (IsEqual(*i, aNewValue))
         return;

      if (aNewValue < *i)
      {
         // Is less, insert before
         i = aValues.insert(i, aNewValue);
         return;
      }
   }

   aValues.push_back(aNewValue);
}

bool Set(std::vector<std::vector<double>>& aTOFs,
         const std::vector<double>&        aAltitudes,
         const std::vector<double>&        aRanges,
         double                            aAlt,
         double                            aRange,
         double                            aTOF)
{
   for (unsigned int iAlt = 0; iAlt != aAltitudes.size(); ++iAlt)
   {
      if (IsEqual(aAlt, aAltitudes[iAlt]))
      {
         for (unsigned int jRange = 0; jRange != aRanges.size(); ++jRange)
         {
            if (IsEqual(aRange, aRanges[jRange]))
            {
               aTOFs[iAlt][jRange] = aTOF;
               return true;
            }
         }
      }
   }
   return false;
}
} // namespace

WsfSAM_LaunchComputer::WsfSAM_LaunchComputer()
   : WsfLaunchComputer()
   , mInputOffsets()
   , mInputAltitudes()
   , mInputRanges()
   , mInputSpeeds()
   , mTestPoints()
   , mEnvelopePtr()
   , mInterpolator()
{
}

//! Copy constructor.
WsfSAM_LaunchComputer::WsfSAM_LaunchComputer(const WsfSAM_LaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mInputOffsets(aSrc.mInputOffsets)
   , mInputAltitudes(aSrc.mInputAltitudes)
   , mInputRanges(aSrc.mInputRanges)
   , mInputSpeeds(aSrc.mInputSpeeds)
   , mTestPoints(aSrc.mTestPoints)
   , mEnvelopePtr(aSrc.mEnvelopePtr)
   , mInterpolator()
{
}

//! Return a pointer to the intercept envelope.
WsfSAM_LaunchComputer::InterceptEnvelope* WsfSAM_LaunchComputer::GetEnvPtr()
{
   if (!mEnvelopePtr)
   {
      mEnvelopePtr = std::make_shared<InterceptEnvelope>();
   }
   return mEnvelopePtr.get();
}

// virtual
WsfLaunchComputer* WsfSAM_LaunchComputer::Clone() const
{
   return new WsfSAM_LaunchComputer(*this);
}

// static
bool WsfSAM_LaunchComputer::ValidateEnvelope(WsfSAM_LaunchComputer::InterceptEnvelope* aEnvelopePtr, bool aIgnoreTOFs)
{
   // If we can ignore the TOFs (some of the tool generators start out with zero TOFs),
   // then do not flag them as bad.
   return aEnvelopePtr->Initialize(aIgnoreTOFs);
}

// virtual
bool WsfSAM_LaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool success = InitializeTTIData();

   if (success)
   {
      success = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   }

   mInterpolator.mOffsetsPtr   = &(GetEnvPtr()->mOffsets);
   mInterpolator.mAltitudesPtr = &(GetEnvPtr()->mAltitudes);
   mInterpolator.mRangesPtr    = &(GetEnvPtr()->mRanges);
   mInterpolator.mSpeedsPtr    = &(GetEnvPtr()->mSpeeds);
   mInterpolator.mTOFsPtr      = &(GetEnvPtr()->mTOFs);

   if (!mTestPoints.empty())
   {
      TestInterpolationPoints();
   }

   return success;
}

void WsfSAM_LaunchComputer::TestInterpolationPoints()
{
   auto out = ut::log::info() << "SAM Test Interpolation Points:";
   for (unsigned int i = 0; i != mTestPoints.size(); ++i)
   {
      TestPoint& tp   = mTestPoints[i];
      double     tof  = InterpolatedTimeToIntercept(tp.latOffset, tp.altitude, tp.groundRange, tp.speed);
      auto       note = out.AddNote() << "Point " << i << " Test Results:";
      note.AddNote() << "Offset: " << tp.latOffset;
      note.AddNote() << "Altitude: " << tp.altitude;
      note.AddNote() << "GrndRange: " << tp.groundRange;
      note.AddNote() << "Speed: " << tp.speed;
      note.AddNote() << "Resulting Time of Flight: " << tof;
   }
}

// virtual
bool WsfSAM_LaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "intercept_envelope")
   {
      GetEnvPtr()->Resize(mInputOffsets, mInputAltitudes, mInputRanges, mInputSpeeds);

      // Do not need these any more, so clear them:
      mInputOffsets.clear();
      mInputAltitudes.clear();
      mInputRanges.clear();
      mInputSpeeds.clear();

      UtInputBlock inputBlock(aInput);
      GetEnvPtr()->ProcessInterceptEnvBlock(inputBlock);
   }
   else if (command == "lateral_offsets")
   {
      std::string terminator("end_" + command);
      if (!ProcessArray(aInput, terminator, UtInput::cLENGTH, mInputOffsets))
      {
         throw UtInput::BadValue(aInput, "Improper 'lateral_offsets' specification");
      }
   }
   else if (command == "altitudes")
   {
      std::string terminator("end_" + command);
      if (!ProcessArray(aInput, terminator, UtInput::cLENGTH, mInputAltitudes))
      {
         throw UtInput::BadValue(aInput, "Improper 'altitudes' specification");
      }
   }
   else if (command == "ranges")
   {
      std::string terminator("end_" + command);
      if (!ProcessArray(aInput, terminator, UtInput::cLENGTH, mInputRanges))
      {
         throw UtInput::BadValue(aInput, "Improper 'ranges' specification");
      }
   }
   else if (command == "speeds")
   {
      std::string terminator("end_" + command);
      if (!ProcessArray(aInput, terminator, UtInput::cSPEED, mInputSpeeds))
      {
         throw UtInput::BadValue(aInput, "Improper 'speeds' specification");
      }
   }
   else if (command == "interpolation_test")
   {
      UtInputBlock block(aInput);
      TestPoint    point;
      while (block.ReadCommand())
      {
         UtInput& in = block.GetInput();
         command     = in.GetCommand();
         if (command == "lateral_offset")
         {
            in.ReadValueOfType(point.latOffset, UtInput::cLENGTH);
         }
         else if (command == "altitude")
         {
            in.ReadValueOfType(point.altitude, UtInput::cLENGTH);
         }
         else if (command == "ground_range")
         {
            in.ReadValueOfType(point.groundRange, UtInput::cLENGTH);
         }
         else if (command == "target_speed")
         {
            in.ReadValueOfType(point.speed, UtInput::cSPEED);
         }
         else if (command == "test")
         {
            mTestPoints.push_back(point);
         }
      }
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }

   return myCommand;
}

// Implementation of base class method.
double WsfSAM_LaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   InitializeResults(aSimTime, aTrack);

   // Get the WCS unit position vector of the launcher.
   double launchLocWCS[3];
   GetPlatform()->GetLocationWCS(launchLocWCS);
   double launchLocWCS_UnitVec[3];
   UtVec3d::Normalize(launchLocWCS_UnitVec, launchLocWCS);

   // Get the WCS unit position vector and altitude of the last reported location of the target.
   // This is used for two purposes:
   //
   // 1) To get the expected maximum time of flight.
   // 2) For correcting extrapolated tracks due to earth curvature (we often extrapolate
   //    tracks many seconds into the future and the error may be a little much).

   double tempTgtLocWCS[3];
   if (!aTrack.GetExtrapolatedLocationWCS(aSimTime, tempTgtLocWCS))
   {
      return cFOREVER; // Unable to determine the target location
   }
   double baseTgtLocWCS_UnitVec[3];
   UtVec3d::Normalize(baseTgtLocWCS_UnitVec, tempTgtLocWCS);
   double tgtLat;
   double tgtLon;
   double tgtAlt;
   UtEntity::ConvertWCSToLLA(tempTgtLocWCS, tgtLat, tgtLon, tgtAlt);

   double trackLateralOffset = 0.0;
   double tgtSpeed           = 0.0;
   double tgtGroundRange     = 0.0;
   double hdgNotUsed;

   double shooterLat;
   double shooterLon;
   double shooterAlt;
   GetPlatform()->GetLocationLLA(shooterLat, shooterLon, shooterAlt);
   UtSphericalEarth::GreatCircleHeadingAndDistance(shooterLat, shooterLon, tgtLat, tgtLon, hdgNotUsed, tgtGroundRange);

   if (aTrack.VelocityValid())
   {
      double tgtVelWCS[3] = {0.0, 0.0, 0.0};
      aTrack.GetVelocityWCS(tgtVelWCS);

      // Since we have a valid target velocity, calculate what the lateral flyover offset will be:
      // Process:  A. Cross targetLocWCS with targetVelWCS yields orthogonal earth axis to flyout.
      //           B. Dot (result A) with shooterLocWCS.  Normalize.  Take acos for offset sector angle.
      //           C. Multiply by earth radius to get great circle flyover offset.
      double tgtUnitVelWCS[3];
      UtVec3d::Set(tgtUnitVelWCS, tgtVelWCS);
      tgtSpeed = UtVec3d::Normalize(tgtUnitVelWCS);
      double orthogonalUnitVect[3];
      UtVec3d::CrossProduct(orthogonalUnitVect, baseTgtLocWCS_UnitVec, tgtUnitVelWCS);
      /*double mag2 =*/UtVec3d::Normalize(orthogonalUnitVect);
      double dotProduct = UtVec3d::DotProduct(orthogonalUnitVect, launchLocWCS_UnitVec);
      // Since all inputs are unit vectors, the dotProduct == cosine value
      double sectorAngle = fabs(acos(dotProduct) - UtMath::cPI_OVER_2);
      trackLateralOffset = sectorAngle * UtSphericalEarth::cEARTH_RADIUS;
   }

   double timeToIntercept = mInterpolator.TimeOfFlightFor(trackLateralOffset, tgtAlt, tgtGroundRange, tgtSpeed);
   if (timeToIntercept <= 0.0)
   {
      return cFOREVER;
   }
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "SAM Launch Computer: Estimated Time To Intercept:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "TTI: " << timeToIntercept;
   }
   UpdateResults(aSimTime, timeToIntercept, aTrack);
   return timeToIntercept;
}

// virtual
bool WsfSAM_LaunchComputer::InitializeTTIData()
{
   return ValidateEnvelope(GetEnvPtr(), false);
}

// ==============================================================================================
//                                    Sub-Class InterceptInvelope
// ==============================================================================================

void WsfSAM_LaunchComputer::InterceptEnvelope::ProcessInterceptEnvBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      if (!ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::BadValue(aInputBlock.GetInput(),
                                 "Unknown keyword " + aInputBlock.GetCommand() + " in intercept_envelope block.");
      }
   }
}

bool WsfSAM_LaunchComputer::InterceptEnvelope::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "intercept_envelope")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!ProcessInput(inputBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
   }
   else if (command == "lateral_symmetry")
   {
      aInput.ReadValue(mLaterallySymmetric);
   }
   else if (command == "intercept")
   {
      double offset;
      double alt;
      double range;
      double speed;
      double TOF;
      int    iIntercept;
      int    iOffset;
      int    iAlt;
      int    iRange;
      int    iSpeed;

      aInput.ReadValue(iIntercept);

      aInput.ReadValue(iOffset);
      aInput.ReadValueOfType(offset, UtInput::cLENGTH);

      aInput.ReadValue(iAlt);
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);

      aInput.ReadValue(iRange);
      aInput.ReadValueOfType(range, UtInput::cLENGTH);

      aInput.ReadValue(iSpeed);
      aInput.ReadValueOfType(speed, UtInput::cSPEED);

      aInput.ReadValueOfType(TOF, UtInput::cTIME);

      SetTOF_Value(iOffset, iAlt, iRange, iSpeed, TOF);
   }
   // ================================ START DEPRECATED INPUT ===========================
   else if (command == "target_altitude")
   {
      AltSet set;
      double range;
      double tof;
      aInput.ReadValueOfType(set.alt, UtInput::cLENGTH);
      aInput.ReadCommand(command);
      while (ReadRngAndTOF(aInput, range, tof))
      {
         set.mRangeAndTOFs.emplace_back(range, tof);
         aInput.ReadCommand(command);
      }
      aInput.PushBack(command);
      mObsoleteInput.push_back(set);
   }
   // ================================ END DEPRECATED INPUT ===========================
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfSAM_LaunchComputer::InterceptEnvelope::SetTOF_Value(unsigned int aOffsetIndex,
                                                            unsigned int aAltitudeIndex,
                                                            unsigned int aRangeIndex,
                                                            unsigned int aSpeedIndex,
                                                            double       aTOF_Value)
{
   mTOFs.Set(aTOF_Value, aOffsetIndex, aAltitudeIndex, aRangeIndex, aSpeedIndex);
}

void WsfSAM_LaunchComputer::InterceptEnvelope::ResizeTOFsAndSetDefaults()
{
   mTOFs.Resize(mOffsets.GetSize(), mAltitudes.GetSize(), mRanges.GetSize(), mSpeeds.GetSize());
   for (size_t i = 0; i != mOffsets.GetSize(); ++i)
   {
      for (size_t j = 0; j != mAltitudes.GetSize(); ++j)
      {
         for (size_t k = 0; k != mRanges.GetSize(); ++k)
         {
            for (size_t m = 0; m != mSpeeds.GetSize(); ++m)
            {
               mTOFs.Set(cNOT_VALID, i, j, k, m);
            }
         }
      }
   }
}

unsigned int WsfSAM_LaunchComputer::InterceptEnvelope::CountValidTOFs()
{
   unsigned int result = 0;
   mTOFs.Resize(mOffsets.GetSize(), mAltitudes.GetSize(), mRanges.GetSize(), mSpeeds.GetSize());
   for (size_t i = 0; i != mOffsets.GetSize(); ++i)
   {
      for (size_t j = 0; j != mAltitudes.GetSize(); ++j)
      {
         for (size_t k = 0; k != mRanges.GetSize(); ++k)
         {
            for (size_t m = 0; m != mSpeeds.GetSize(); ++m)
            {
               if (mTOFs.Get(i, j, k, m) != cNOT_VALID)
               {
                  ++result;
               }
            }
         }
      }
   }
   return result;
}
//! Accept an array of offsets, altitudes, and ranges, and set the independent variables to the array contents.
//! This method is used only during launch computer generation, as it does not set the corresponding
//! times-of-flight, which will be calculated later.
//! @param aOffsets       [input] The desired array of lateral offsets.  Should be in ascending order.
//! @param aAltitudes     [input] The desired array of altitudes.  Should be in ascending order.
//! @param aRanges        [input] The desired array of target ranges.  Should be in ascending order.
//! @param aSpeeds        [input] The desired array of target speeds.  Should be in ascending order.
//! @returns Returns true if the initialization was successful.
void WsfSAM_LaunchComputer::InterceptEnvelope::Resize(const std::vector<double>& aOffsets,
                                                      const std::vector<double>& aAltitudes,
                                                      const std::vector<double>& aRanges,
                                                      const std::vector<double>& aSpeeds)
{
   mOffsets.Resize(static_cast<int>(aOffsets.size()));
   mOffsets.SetValues(aOffsets);

   mAltitudes.Resize(static_cast<int>(aAltitudes.size()));
   mAltitudes.SetValues(aAltitudes);

   mRanges.Resize(static_cast<int>(aRanges.size()));
   mRanges.SetValues(aRanges);

   mSpeeds.Resize(static_cast<int>(aSpeeds.size()));
   mSpeeds.SetValues(aSpeeds);

   ResizeTOFsAndSetDefaults();
}

//! Initialize the intercept envelope, and assure that the data it contains is valid.
//! @param aIgnoreTOFs    [input] Do not evaluate the Times of Flight for Correctness, as they are not set yet.
//! @returns Returns true if the initialization was successful.
bool WsfSAM_LaunchComputer::InterceptEnvelope::Initialize(bool aIgnoreTOFs)
{
   bool success = true;

   if (!mObsoleteInput.empty())
   {
      // Resolve old-style input into current format:
      // Can only do this if all input arrays are "rectangular" and equivalent valued IVs:
      success = ResolveDeprecatedInput();
   }

   if (mOffsets.GetSize() < 2)
   {
      ut::log::error() << "WsfSAM_LC size of lateral offsets array must be greater than 1.";
      success = false;
   }
   else
   {
      double prevOffset = -DBL_MAX;
      for (size_t i = 0; i != mOffsets.GetSize(); ++i)
      {
         double offset = mOffsets.Get(i);
         if (offset <= prevOffset)
         {
            ut::log::error() << "WsfSAM_LC must receive a monotonically increasing array of lateral offsets.";
            success = false;
         }
         prevOffset = offset;
      }
   }

   if (mAltitudes.GetSize() < 2)
   {
      ut::log::error() << "WsfSAM_LC size of altitudes array must be greater than 1.";
      success = false;
   }
   else
   {
      double prevAltitude = -DBL_MAX;
      for (size_t i = 0; i != mAltitudes.GetSize(); ++i)
      {
         double alt = mAltitudes.Get(i);
         if (alt <= prevAltitude)
         {
            ut::log::error() << "WsfSAM_LC must receive a monotonically increasing array of altitudes.";
            success = false;
         }
         prevAltitude = alt;
      }
   }

   if (mRanges.GetSize() < 2)
   {
      ut::log::error() << "WsfSAM_LC size of target ranges array must be greater than 1.";
      success = false;
   }
   else
   {
      double prevRange = -DBL_MAX;
      for (size_t i = 0; i != mRanges.GetSize(); ++i)
      {
         double range = mRanges.Get(i);
         if (range <= prevRange)
         {
            ut::log::error() << "WsfSAM_LC must receive a monotonically increasing array of target ranges.";
            success = false;
         }
         prevRange = range;
      }
   }

   if (mSpeeds.GetSize() < 2)
   {
      ut::log::error() << "WsfSAM_LC size of target speeds array must be greater than 1.";
      success = false;
   }
   else
   {
      double prevSpeed = -DBL_MAX;
      for (size_t i = 0; i != mSpeeds.GetSize(); ++i)
      {
         double speed = mSpeeds.Get(i);
         if (speed <= prevSpeed)
         {
            ut::log::error() << "WsfSAM_LC must receive a monotonically increasing array of target speeds.";
            success = false;
         }
         prevSpeed = speed;
      }
   }

   return success;
}

bool WsfSAM_LaunchComputer::InterceptEnvelope::ResolveDeprecatedInput()
{
   unsigned int        iAlt;
   unsigned int        iRng;
   std::vector<double> altitudes;
   std::vector<double> ranges;

   // Initialize to provide a -1.0 "edge" value to stop the low-end extrapolation of TOF.
   altitudes.push_back(-1.0);
   ranges.push_back(-1.0);

   // First amass an array for the altitudes and ranges of all successful engagements.
   for (iAlt = 0; iAlt != mObsoleteInput.size(); ++iAlt)
   {
      altitudes.push_back(mObsoleteInput[iAlt].alt);
      for (iRng = 0; iRng != mObsoleteInput[iAlt].mRangeAndTOFs.size(); ++iRng)
      {
         Add(ranges, mObsoleteInput[iAlt].mRangeAndTOFs[iRng].first);
      }
   }

   // Initialize to provide a -1.0 "edge" value to stop the high-end extrapolation of TOF.
   altitudes.push_back(altitudes[altitudes.size() - 1] + 10.0);
   ranges.push_back(ranges[ranges.size() - 1] + 10.0);

   // Allocate an array to hold all the TOFs:
   std::vector<std::vector<double>> tofs;
   tofs.resize(altitudes.size());

   // Initialize all values:
   for (iAlt = 0; iAlt != altitudes.size(); ++iAlt)
   {
      tofs[iAlt].resize(ranges.size());
      for (iRng = 0; iRng != ranges.size(); ++iRng)
      {
         tofs[iAlt][iRng] = cNOT_VALID;
      }
   }

   bool allSet = true;
   // Pick off all the TOFs previously input, and place them in the array as needed
   for (iAlt = 0; iAlt != mObsoleteInput.size(); ++iAlt)
   {
      for (iRng = 0; iRng != mObsoleteInput[iAlt].mRangeAndTOFs.size(); ++iRng)
      {
         if (!Set(tofs,                                             // TOF Array
                  altitudes,                                        // Array to match an Altitude value for
                  ranges,                                           // Array to match a Range value for
                  mObsoleteInput[iAlt].alt,                         // Corresponding Altitude
                  mObsoleteInput[iAlt].mRangeAndTOFs[iRng].first,   // Corresponding Range
                  mObsoleteInput[iAlt].mRangeAndTOFs[iRng].second)) // TOF Value to Insert
         {
            allSet = false;
         }
      }
   }
   if (!allSet)
   {
      ut::log::info() << "FYI Concern: Not all times-of-flight values were located into the altitude and range matrix.";
   }

   // Now we have to "fake" velocity and offset data that is simply not there.
   // Offset is easy, just clone two tables values one unit apart, and call it good (a 3D rep of 2D data).
   // We will do the same for velocity as we do for offset, to see if it works okay...
   mOffsets.Resize(2);
   mOffsets.Set(0.0, 0);
   mOffsets.Set(20000, 1);

   mAltitudes.Resize(static_cast<int>(altitudes.size()));
   for (iAlt = 0; iAlt != altitudes.size(); ++iAlt)
   {
      mAltitudes.Set(altitudes[iAlt], iAlt);
   }

   mRanges.Resize(static_cast<int>(ranges.size()));
   for (iRng = 0; iRng != ranges.size(); ++iRng)
   {
      mRanges.Set(ranges[iRng], iRng);
   }

   mSpeeds.Resize(2);
   mSpeeds.Set(0.0, 0);
   mSpeeds.Set(100, 1);

   ResizeTOFsAndSetDefaults();

   // Pick off all the TOFs previously input, and place them in the array as needed
   for (iAlt = 0; iAlt != altitudes.size(); ++iAlt)
   {
      for (iRng = 0; iRng != ranges.size(); ++iRng)
      {
         // IV Order = Offsets, Altitudes, Ranges, Speeds
         mTOFs.Set(tofs[iAlt][iRng], 0, iAlt, iRng, 0);
         mTOFs.Set(tofs[iAlt][iRng], 0, iAlt, iRng, 1);
         mTOFs.Set(tofs[iAlt][iRng], 1, iAlt, iRng, 0);
         mTOFs.Set(tofs[iAlt][iRng], 1, iAlt, iRng, 1);
      }
   }

   mObsoleteInput.clear();

   return true;
}

bool WsfSAM_LaunchComputer::InterceptEnvelope::ReadRngAndTOF(UtInput& aInput, double& aRange, double& aTOF)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "ground_range")
   {
      // ground_range 10000 m   time_of_flight 25.2 sec
      aInput.ReadValueOfType(aRange, UtInput::cLENGTH);
      std::string token;
      aInput.ReadValue(token);
      aInput.ReadValueOfType(aTOF, UtInput::cTIME);
      myCommand = true;
   }
   return myCommand;
}

WsfSAM_LaunchComputer::InterceptEnvelope::InterceptEnvelope()
   : mLaterallySymmetric(true)
   , mOffsets()
   , mAltitudes()
   , mRanges()
   , mSpeeds()
   , mTOFs()
{
}

double WsfSAM_LaunchComputer::Interpolator::TimeOfFlightFor(double aOffset, double aAltitude, double aRange, double aSpeed)
{
   // Ensure the data has enough entries
   // Interpolation cannot be performed if tables are not large enough
   // Extrapolation is not allowed
   if ((mOffsetsPtr->GetSize() < 2) || (mAltitudesPtr->GetSize() < 2) || (mRangesPtr->GetSize() < 2) ||
       (mSpeedsPtr->GetSize() < 2))
   {
      auto out = ut::log::error() << "Time of Flight interpolation cannot be performed.";
      out.AddNote() << "All tables must have at least two entries.";
      out.AddNote() << "Offsets: " << mOffsetsPtr->GetSize();
      out.AddNote() << "Altitudes: " << mAltitudesPtr->GetSize();
      out.AddNote() << "Ranges: " << mRangesPtr->GetSize();
      out.AddNote() << "Speeds: " << mSpeedsPtr->GetSize();

      return -1;
   }

   // First, set the indexes into the IV arrays:
   mOffsetLU.Lookup(*mOffsetsPtr, aOffset);
   mAltLU.Lookup(*mAltitudesPtr, aAltitude);
   mRangeLU.Lookup(*mRangesPtr, aRange);
   mSpeedLU.Lookup(*mSpeedsPtr, aSpeed);

   // The generator for this object will write a -1 into any non-valid TOF slot.
   // So if any of the "corner" values (above) to interpolate from do not have
   // values greater than zero, we must force the answer to -1, because the
   // interpolation would be invalid.    // binary-type permutation of indices...

   if ((mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex(), mRangeLU.GetIndex(), mSpeedLU.GetIndex()) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex(), mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex(), mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex()) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex(), mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex() + 1) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex() + 1, mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex() + 1, mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex() + 1, mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex()) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex(), mAltLU.GetIndex() + 1, mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex() + 1) ==
        -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex(), mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex(), mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex()) == -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex(), mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex() + 1) ==
        -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex() + 1, mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) ==
        -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex() + 1, mRangeLU.GetIndex(), mSpeedLU.GetIndex() + 1) ==
        -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex() + 1, mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex()) ==
        -1.0) ||
       (mTOFsPtr->Get(mOffsetLU.GetIndex() + 1, mAltLU.GetIndex() + 1, mRangeLU.GetIndex() + 1, mSpeedLU.GetIndex() + 1) ==
        -1.0))
   {
      return -1.0;
   }

   return TblEvaluate(*mTOFsPtr, mOffsetLU, mAltLU, mRangeLU, mSpeedLU);
}

//! Parse an input stream and extract from it an array of unit values.
//! @param aInput           The input stream reference.
//! @param aBlockTerminator The string to search for end-of-block.
//! @param aUnitType        The type of unit to be read in and added to the array contents.
//! @param aValues          An array of floating point values to hold the results of parsing.
//! @returns 'True' if the aValues array was added to in any way.
bool WsfSAM_LaunchComputer::ProcessArray(UtInput&             aInput,
                                         const std::string&   aBlockTerminator,
                                         UtInput::ValueType   aUnitType,
                                         std::vector<double>& aValues)
{
   // Throw away the obsolete command that got us in here...
   std::string newCommand;
   aInput.ReadCommand(newCommand);

   bool valuesAdded = false;
   bool done        = false;

   // The implementation guarantees that the array is null coming in.
   // The idea is to read in an array of distance values, either with:
   // a) A "from" <value> "to" <value> "by" <value> syntax, or
   // b) A continuous stream of values in increasing order, delimited by white space.
   std::string command = aInput.GetCommand();

   while (!done)
   {
      if (command == aBlockTerminator)
      {
         done = true;
      }
      else if (command == "from")
      {
         double startDistValue;
         aInput.ReadValueOfType(startDistValue, aUnitType);

         std::string tokenTo;
         aInput.ReadValue(tokenTo); // throw away required (but unused) token "to"

         double endDistValue;
         aInput.ReadValueOfType(endDistValue, aUnitType);
         aInput.ValueGreater(endDistValue, startDistValue);

         std::string tokenBy;
         aInput.ReadValue(tokenBy); // throw away required (but unused) token "by"

         double deltaDistValue;
         aInput.ReadValueOfType(deltaDistValue, aUnitType);

         unsigned int numDists = static_cast<unsigned int>((endDistValue - startDistValue) / deltaDistValue + 1);

         for (unsigned int iDist = 0; iDist != numDists; ++iDist)
         {
            aValues.push_back(startDistValue + iDist * deltaDistValue);
         }
         valuesAdded = true;

         aInput.ReadCommand(command);
      }
      else
      {
         if (UtStringUtil::ContainsNonNumeric(command))
         {
            done = true;
         }
         else
         {
            aInput.PushBack(command);
            double distanceValue;
            aInput.ReadValueOfType(distanceValue, aUnitType);
            aValues.push_back(distanceValue);
            valuesAdded = true;
            aInput.ReadCommand(command);
         }
      }
   }

   return valuesAdded;
}

//! This method is for testing of the internal interpolation routines only.
double WsfSAM_LaunchComputer::InterpolatedTimeToIntercept(double aOffset, double aAltitude, double aRange, double aSpeed)
{
   return mInterpolator.TimeOfFlightFor(aOffset, aAltitude, aRange, aSpeed);
}
