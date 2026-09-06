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
//
//                    P R O G R A M M I N G    N O T E S
//
// *) Spherical Earth coordinates are used for all internal computations (target
//    propagation, etc.). This is because the tables are produced assuming
//    spherical coordinates and WsfGuidedMover internally operates in spherical
//    coordinates. Ellipsoidal coordinates are not used because it make it
//    impossible to accurately predict the loft angle and burn time for all
//    any target location.
//
// *) Unless stated otherwise, all use of the term 'range' means 'ground range'.
//    Any use of 'slant range' should explicitly say 'slant range'.
//
// ****************************************************************************

#include "WsfBallisticMissileLaunchComputer.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLLPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfAero.hpp"
#include "WsfDraw.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

// static
namespace
{
const char* cBALLISTIC_SHARED_EXTENSION_NAME = "ballistic_launch_computer_share";

//! A class used to hold the trajectory.
//!
//! Objects of this type are only used while the tables used during run-time are being created.
class Trajectory
{
public:
   TblIndVarU<double> mRange;
   TblDepVar1<double> mAltitude;
   TblDepVar1<double> mTimeOfFlight;
   TblDepVar1<double> mSpeed;
   TblDepVar1<double> mFlightPathAngle;

   double mLoftAngle;
   double mMinRange;
   double mMaxRange;
   double mMinAltitude;
   double mMaxAltitude;
   double mMinTimeOfFlight;
   double mMaxTimeOfFlight;
};

// How far in the past a solution can be and still be called 'acceptable'.
const double cPAST_SOLUTION_TOLERANCE = 0.5;
} // namespace

class WSF_MIL_EXPORT WsfBallisticMissileLaunchComputerSharedSetup : public WsfScenarioExtension
{
public:
   std::map<WsfStringId, WsfStringId> mAeroMap;
   std::map<WsfStringId, double>      mMassMap;

   WsfBallisticMissileLaunchComputerSharedSetup() = default;

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      auto shared            = ut::make_unique<WsfBallisticMissileLaunchComputerSharedData>();
      shared->mTargetAeroMap = mAeroMap;
      shared->mTargetMassMap = mMassMap;
      aSimulation.RegisterExtension(GetExtensionName(), std::move(shared));
   }

   bool ProcessInput(UtInput& aInput) override
   {
      bool myCommand = false;
      if (aInput.GetCommand() == "ballistic_missile_launch_computer")
      {
         myCommand = true;
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            if (!ProcessBallisticTargetInput(aInput))
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      return myCommand;
   }

   bool ProcessBallisticTargetInput(UtInput& aInput)
   {
      bool        myCommand = false;
      std::string command   = aInput.GetCommand();
      if (command == "target_data")
      {
         UtInputBlock inputBlock(aInput);
         std::string  type;
         aInput.ReadValue(type);
         bool hasAero = false;
         bool hasMass = false;
         while (inputBlock.ReadCommand(command))
         {
            if (command == "aero")
            {
               std::string aeroType;
               aInput.ReadCommand(aeroType);
               mAeroMap[type] = aeroType;
               hasAero        = true;
            }
            else if (command == "mass")
            {
               double mass;
               aInput.ReadValueOfType(mass, UtInput::cMASS);
               mMassMap[type] = mass;
               hasMass        = true;
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         if ((!hasAero) || (!hasMass))
         {
            throw UtInput::BadValue(aInput, "Incomplete target_data block (include both aero mapping and mass).");
         }
         myCommand = true;
      }
      return myCommand;
   }
};

void Register_BallisticMissileLaunchComputer(WsfScenario& aScenario)
{
   aScenario.RegisterExtension(cBALLISTIC_SHARED_EXTENSION_NAME,
                               ut::make_unique<WsfBallisticMissileLaunchComputerSharedSetup>());
}

// =================================================================================================
//! Constructor
WsfBallisticMissileLaunchComputer::WsfBallisticMissileLaunchComputer()
   : WsfLaunchComputer()
   , mSA_TablePtr()
   , mSS_TablePtr()
   , mMaxLaunchSlantRange(0.0)
   , mMaxInterceptSlantRange(0.0)
   , mMinInterceptAltitude(0.0)
   , mMaxInterceptAltitude(DBL_MAX)
   , mMinInterceptVelocity(0.0)
   , mMaxInterceptAngle(0.0)
   , mIntegrationTimeStep(0.05)
   , mGroundRangeSampleInterval(500.0)
   , mSolutionValidityTime(cPAST_SOLUTION_TOLERANCE + 0.1)
   , // Should be bigger than cNEGATIVE_SOLUTION_TOLERANCE
   mPredictedTrajectoryErrorLimit(250.0)
   , mLastEvaluatedGroundRange(0.0)
   , mAllowBoostPhaseIntercept(false)
   , mShowGraphics(false)
   , mDebugFileLoading(false)
   , mInterceptTruth(true)
   , mInputComputeEndPoint(false)
   , mComputeEndPoint(false)
   , mComputeLaunchWindows(false)
   , mLaunchWindows()
   , mTargetApogeeTime(0.0)
   , mSA_SolutionCache()
   , mCallbacks()
   , mSharedDataPtr(nullptr)
{
   UtVec3d::Set(mTargetApogeeLocWCS, 0.0);
}

// =================================================================================================
//! Copy constructor
WsfBallisticMissileLaunchComputer::WsfBallisticMissileLaunchComputer(const WsfBallisticMissileLaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mSA_TablePtr(aSrc.mSA_TablePtr)
   , mSS_TablePtr(aSrc.mSS_TablePtr)
   , mMaxLaunchSlantRange(aSrc.mMaxLaunchSlantRange)
   , mMaxInterceptSlantRange(aSrc.mMaxInterceptSlantRange)
   , mMinInterceptAltitude(aSrc.mMinInterceptAltitude)
   , mMaxInterceptAltitude(aSrc.mMaxInterceptAltitude)
   , mMinInterceptVelocity(aSrc.mMinInterceptVelocity)
   , mMaxInterceptAngle(aSrc.mMaxInterceptAngle)
   , mIntegrationTimeStep(aSrc.mIntegrationTimeStep)
   , mGroundRangeSampleInterval(aSrc.mGroundRangeSampleInterval)
   , mSolutionValidityTime(aSrc.mSolutionValidityTime)
   , mPredictedTrajectoryErrorLimit(aSrc.mPredictedTrajectoryErrorLimit)
   , mLastEvaluatedGroundRange(aSrc.mLastEvaluatedGroundRange)
   , mAllowBoostPhaseIntercept(aSrc.mAllowBoostPhaseIntercept)
   , mShowGraphics(aSrc.mShowGraphics)
   , mDebugFileLoading(aSrc.mDebugFileLoading)
   , mInterceptTruth(aSrc.mInterceptTruth)
   , mInputComputeEndPoint(aSrc.mInputComputeEndPoint)
   , mComputeEndPoint(aSrc.mComputeEndPoint)
   , mComputeLaunchWindows(aSrc.mComputeLaunchWindows)
   , mLaunchWindows(aSrc.mLaunchWindows)
   , mTargetApogeeTime(0.0)
   , mSA_SolutionCache(aSrc.mSA_SolutionCache)
   , mCallbacks(aSrc.mCallbacks)
   , mSharedDataPtr(aSrc.mSharedDataPtr)
{
   UtVec3d::Set(mTargetApogeeLocWCS, 0.0);
}

// =================================================================================================
//! Destructor
// virtual
WsfBallisticMissileLaunchComputer::~WsfBallisticMissileLaunchComputer()
{
   // NOTE: We don't have to worry about cleaning up target data because it is deleted by the
   //       PlatformDeleted callback.
}

// =================================================================================================
// virtual
WsfLaunchComputer* WsfBallisticMissileLaunchComputer::Clone() const
{
   return new WsfBallisticMissileLaunchComputer(*this);
}

/// =================================================================================================
// virtual
bool WsfBallisticMissileLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool ok = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);

   // Locate or create the shared data store
   mSharedDataPtr = static_cast<WsfBallisticMissileLaunchComputerSharedData*>(
      GetSimulation()->FindExtension(cBALLISTIC_SHARED_EXTENSION_NAME));

   if (mShowGraphics)
   {
      mLineDrawPtr = ut::make_unique<WsfDraw>(*GetSimulation());
      mDrawPtr     = ut::make_unique<WsfDraw>(*GetSimulation());
   }

   if (ComputerGenerationMode())
   {
      return ok;
   }

   // The maximum_time_of_flight parameter is only needed for surface-to-air shots.
   // If a surface-to-surface table has been defined then it is assumed that only surface-to-surface
   // shots are allowed.
   if (!mSS_TablePtr && !mSA_TablePtr)
   {
      ut::log::error() << "Surface_to_surface or surface_to_air table must be provided.";
      ok = false;
   }

   if (mShowGraphics)
   {
      DrawRanges();
   }

   if (DebugEnabled())
   {
      mShowResults = true;
   }

   // Register callbacks so we can clean up objects when necessary.
   mCallbacks.Add(
      WsfObserver::PlatformDeleted(GetSimulation()).Connect(&WsfBallisticMissileLaunchComputer::PlatformDeleted, this));
   mCallbacks.Add(WsfObserver::WeaponFired(GetSimulation()).Connect(&WsfBallisticMissileLaunchComputer::WeaponFired, this));
   mCallbacks.Add(
      WsfObserver::WeaponFireAborted(GetSimulation()).Connect(&WsfBallisticMissileLaunchComputer::WeaponFireAborted, this));

   return ok;
}

// =================================================================================================
// virtual
bool WsfBallisticMissileLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "maximum_launch_slant_range")
   {
      aInput.ReadValueOfType(mMaxLaunchSlantRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaxLaunchSlantRange, 0.0); // A value of zero disables the check
   }
   else if (command == "maximum_intercept_slant_range")
   {
      aInput.ReadValueOfType(mMaxInterceptSlantRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaxInterceptSlantRange, 0.0); // A value of zero disables the check
   }
   else if (command == "minimum_intercept_altitude")
   {
      aInput.ReadValueOfType(mMinInterceptAltitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMinInterceptAltitude, 0.0);
      aInput.ValueLess(mMinInterceptAltitude, mMaxInterceptAltitude);
   }
   else if (command == "maximum_intercept_altitude")
   {
      aInput.ReadValueOfType(mMaxInterceptAltitude, UtInput::cLENGTH);
      aInput.ValueGreater(mMaxInterceptAltitude, mMinInterceptAltitude);
   }
   else if (command == "maximum_intercept_angle")
   {
      aInput.ReadValueOfType(mMaxInterceptAngle, UtInput::cANGLE);
      // For backward compatibility, allow 0-180 degrees, but
      // if > 90 degrees, override, and limit it to 90 degrees
      aInput.ValueInClosedRange(mMaxInterceptAngle, 0.0, UtMath::cPI);
      if (mMaxInterceptAngle > UtMath::cPI_OVER_2)
      {
         mMaxInterceptAngle = UtMath::cPI_OVER_2;
      }
   }
   else if ((command == "minimum_intercept_velocity") || (command == "minimum_impact_velocity"))
   {
      aInput.ReadValueOfType(mMinInterceptVelocity, UtInput::cSPEED);
      aInput.ValueGreater(mMinInterceptVelocity, 0.0);
   }
   else if ((command == "integration_timestep") || (command == "integration_time_step"))
   {
      aInput.ReadValueOfType(mIntegrationTimeStep, UtInput::cTIME);
      aInput.ValueGreater(mIntegrationTimeStep, 0.0);
   }
   else if (command == "ground_range_sample_interval")
   {
      aInput.ReadValueOfType(mGroundRangeSampleInterval, UtInput::cLENGTH);
      aInput.ValueGreater(mGroundRangeSampleInterval, 0.0);
   }
   else if (command == "solution_validity_time")
   {
      aInput.ReadValueOfType(mSolutionValidityTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSolutionValidityTime, 0.0);
   }
   else if (command == "predicted_trajectory_error_limit")
   {
      aInput.ReadValueOfType(mPredictedTrajectoryErrorLimit, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mPredictedTrajectoryErrorLimit, 0.0);
   }
   else if (command == "allow_boost_phase_intercept")
   {
      aInput.ReadValue(mAllowBoostPhaseIntercept);
   }
   else if (command == "compute_end_point")
   {
      aInput.ReadValue(mInputComputeEndPoint);
      mComputeEndPoint = mInputComputeEndPoint;
   }
   else if (command == "show_graphics")
   {
      mShowGraphics = true;
   }
   else if (command == "debug_file_loading")
   {
      mDebugFileLoading = true;
   }
   else if (command == "intercept_track_location")
   {
      mInterceptTruth = false;
   }
   else if (command == "intercept_truth_location")
   {
      mInterceptTruth = true;
   }
   else if (command == "surface_to_air_table")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName     = aInput.LocateFile(fileName);
      mSA_TablePtr = std::make_shared<SA_Table>(mGroundRangeSampleInterval);
      if (!mSA_TablePtr->Read(fileName, DebugEnabled(), mDebugFileLoading))
      {
         throw UtInput::BadValue(aInput, "Unable to process file " + fileName);
      }
      WsfScenario::FromInput(aInput).GetSystemLog().WriteLogEntry("file " + fileName);
   }
   else if (command == "surface_to_surface_table")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName     = aInput.LocateFile(fileName);
      mSS_TablePtr = std::make_shared<SS_Table>();
      if (!mSS_TablePtr->Read(fileName))
      {
         throw UtInput::BadValue(aInput, "Unable to process file " + fileName);
      }
      WsfScenario::FromInput(aInput).GetSystemLog().WriteLogEntry("file " + fileName);
   }
   else
   {
      // Check for any 'target_data' blocks and base class input.
      WsfScenarioExtension* extPtr = WsfScenario::FromInput(aInput).FindExtension(cBALLISTIC_SHARED_EXTENSION_NAME);
      WsfBallisticMissileLaunchComputerSharedSetup* sharedPtr =
         dynamic_cast<WsfBallisticMissileLaunchComputerSharedSetup*>(extPtr);
      if ((sharedPtr != nullptr) && sharedPtr->ProcessBallisticTargetInput(aInput))
      {
      }
      else
      {
         myCommand = WsfLaunchComputer::ProcessInput(aInput);
      }
   }
   return myCommand;
}

// =================================================================================================
// virtual
double WsfBallisticMissileLaunchComputer::EstimatedTimeToIntercept(double          aSimTime,
                                                                   const WsfTrack& aTrack,
                                                                   double          aLaunchDelayTime)
{
   // If computer generation mode simply call the base class, which will apply the commanded values.
   // We don't care about estimating the time because that is what computer generation is for!
   if (ComputerGenerationMode())
   {
      mComputeLaunchWindows = false; // This is a one shot request, so disable after use
      mComputeEndPoint      = mInputComputeEndPoint;
      return WsfLaunchComputer::EstimatedTimeToIntercept(aSimTime, aTrack, aLaunchDelayTime);
   }

   InitializeResults(aSimTime, aTrack);

   bool         airTarget = false;
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
   if (targetPtr != nullptr)
   {
      if ((targetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR) ||
          (targetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE))
      {
         airTarget = true;
      }
   }

   if (airTarget)
   {
      // Surface-to-air shot.
      ComputeAirIntercept(aSimTime, targetPtr, &aTrack, aLaunchDelayTime);
   }
   else
   {
      // Possible surface-to-surface shot.
      double targetLocWCS[3] = {0.0, 0.0, 0.0};
      if ((!aTrack.GetExtrapolatedLocationWCS(aSimTime, targetLocWCS)) && (targetPtr != nullptr))
      {
         // No location in the track, but we have a truth target. So use it...
         targetPtr->GetLocationWCS(targetLocWCS);
      }
      if (!UtVec3d::Equals(targetLocWCS, 0.0))
      {
         ComputeGroundIntercept(aSimTime, aLaunchDelayTime, targetLocWCS);
      }
   }

   double timeToIntercept = cFOREVER;
   if (InterceptTimeIsValid())
   {
      timeToIntercept = mInterceptTime - aSimTime;
   }
   UpdateResults(aSimTime, timeToIntercept, aTrack);

   if (ShowResults())
   {
      std::ostringstream oss;
      oss << "T=" << aSimTime << ' ' << GetPlatform()->GetName() << '.' << mWeaponPtr->GetName() << " ("
          << mWeaponPtr->GetType() << ") Computed firing solution vs. ";
      auto out = ut::log::info() << "Computed firing solution:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << mWeaponPtr->GetName();
      out.AddNote() << "Weapon Type: " << mWeaponPtr->GetType();
      if (targetPtr != nullptr)
      {
         oss << targetPtr->GetName() << " (" << targetPtr->GetType() << ")";
         out.AddNote() << "Target: " << targetPtr->GetName();
         out.AddNote() << "Target Type: " << targetPtr->GetType();
      }
      else
      {
         oss << "<UNKNOWN-TARGET>";
         out.AddNote() << "Target: "
                       << "<UNKNOWN-TARGET>";
      }
      if (InterceptTimeIsValid())
      {
         oss << ": TTI=" << mInterceptTime - aSimTime << " (T=" << mInterceptTime << ").";
         out.AddNote() << "Time to intercept: " << mInterceptTime - aSimTime << "(T = " << mInterceptTime << ").";
      }
      else
      {
         oss << "; No solution.";
         out.AddNote() << "No solution.";
      }
      GetPlatform()->Comment(aSimTime, oss.str());
   }
   mComputeLaunchWindows = false; // This is a one shot request, so disable after use
   mComputeEndPoint      = mInputComputeEndPoint;
   return timeToIntercept;
}

// =================================================================================================
//! Get the lateral offset from the launching weapon system to the target trajectory.
//! @param aSimTime The current simulation time.
//! @param aTrack   The track that represents the target.
//! @note This is valid only for surface-to-air intercepts.
// virtual
double WsfBallisticMissileLaunchComputer::ComputeLateralTargetOffset(double aSimTime, const WsfTrack& aTrack)
{
   double lateralOffset = cOUT_OF_REACH;

   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
   if (targetPtr == nullptr)
   {
      return lateralOffset;
   }
   if ((targetPtr->GetSpatialDomain() != WSF_SPATIAL_DOMAIN_AIR) &&
       (targetPtr->GetSpatialDomain() != WSF_SPATIAL_DOMAIN_SPACE))
   {
      return lateralOffset;
   }
   if (!mSA_TablePtr ||               // No data provided
       (targetPtr->GetSpeed() < 1.0)) // target is not moving
   {
      return lateralOffset;
   }

   // In a typical engagement intercept calculations are not performed during boost phase
   // because the trajectory prediction would be very inaccurate.

   if (!mAllowBoostPhaseIntercept)
   {
      WsfMover* moverPtr = targetPtr->GetMover();
      if ((moverPtr != nullptr) && (!moverPtr->IsExtrapolating())) // IsExtrapolating means boost phase is complete
      {
         return lateralOffset;
      }
   }

   // Get the predicted target trajectory.

   if (mLineDrawPtr != nullptr)
   {
      mLineDrawPtr->SetColor(1.0, 0.0, 0.0);
   }

   size_t      targetIndex = targetPtr->GetIndex();
   WsfStringId typeId      = targetPtr->GetTypeId();
   targetPtr->Update(aSimTime);
   double locWCS[3];
   double velWCS[3];
   targetPtr->GetLocationWCS(locWCS);
   targetPtr->GetVelocityWCS(velWCS);
   WsfMover::KinematicModel* targetDataPtr =
      GetTargetData(aSimTime, targetIndex, typeId, locWCS, velWCS, targetPtr->GetMover());
   if (targetDataPtr == nullptr)
   {
      return 0.0;
   }

   double     launchLocWCS[3];
   WsfWeapon* weaponPtr = GetWeapon();
   weaponPtr->GetLocationWCS(launchLocWCS);
   double launchLocXYZ[3];
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(launchLocWCS, launchLocXYZ);

   // This is brute force... follow the trajectory and find when the ground range from the launcher to
   // the weapon is the smallest. This will be 'lateral offset'.
   //
   // Note that this assumes that the initial trajectory estimation for the target was done while it
   // was inbound. If the initial estimation was done while it was outbound then the first point will
   // be the closest.

   double targetLocXYZ[3];
   double range;
   double altitude;
   double t = targetDataPtr->GetStartTime() - mIntegrationTimeStep;
   while (t <= targetDataPtr->GetTimeAtLocation())
   {
      t += mIntegrationTimeStep;
      if (!targetDataPtr->GetLocationXYZ(t, targetLocXYZ))
      {
         break;
      }
      WsfMover::KinematicModel::GetRangeAndAltitude(launchLocXYZ, targetLocXYZ, range, altitude);
      if (range > lateralOffset)
      {
         break;
      }
      lateralOffset = range;
   }

   return lateralOffset;
}

// =================================================================================================
double WsfBallisticMissileLaunchComputer::GetFirstLaunchTime(unsigned int aIndex) const
{
   double value = -1.0;
   if (aIndex < mLaunchWindows.size())
   {
      value = mLaunchWindows[aIndex].mFirstLaunchTime;
   }
   return value;
}

// =================================================================================================
double WsfBallisticMissileLaunchComputer::GetFirstInterceptTime(unsigned int aIndex) const
{
   double value = -1.0;
   if (aIndex < mLaunchWindows.size())
   {
      value = mLaunchWindows[aIndex].mFirstInterceptTime;
   }
   return value;
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::GetFirstInterceptLocationWCS(unsigned int aIndex, double aLocWCS[3]) const
{
   if (aIndex < mLaunchWindows.size())
   {
      UtVec3d::Set(aLocWCS, mLaunchWindows[aIndex].mFirstInterceptLocWCS);
   }
   else
   {
      UtVec3d::Set(aLocWCS, 0.0);
   }
}

// =================================================================================================
double WsfBallisticMissileLaunchComputer::GetLastLaunchTime(unsigned int aIndex) const
{
   double value = -1.0;
   if (aIndex < mLaunchWindows.size())
   {
      value = mLaunchWindows[aIndex].mLastLaunchTime;
   }
   return value;
}

// =================================================================================================
double WsfBallisticMissileLaunchComputer::GetLastInterceptTime(unsigned int aIndex) const
{
   double value = -1.0;
   if (aIndex < mLaunchWindows.size())
   {
      value = mLaunchWindows[aIndex].mLastInterceptTime;
   }
   return value;
}

// =================================================================================================
double WsfBallisticMissileLaunchComputer::GetTargetApogeeTime() const
{
   return mTargetApogeeTime;
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::GetTargetApogeeLocationWCS(double aLocWCS[3]) const
{
   UtVec3d::Set(aLocWCS, mTargetApogeeLocWCS);
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::GetLastInterceptLocationWCS(unsigned int aIndex, double aLocWCS[3]) const
{
   if (aIndex < mLaunchWindows.size())
   {
      UtVec3d::Set(aLocWCS, mLaunchWindows[aIndex].mLastInterceptLocWCS);
   }
   else
   {
      UtVec3d::Set(aLocWCS, 0.0);
   }
}

// =================================================================================================
bool WsfBallisticMissileLaunchComputer::GetPrecomputedLaunchWindowsFor(double                        aSimTime,
                                                                       unsigned int                  aThreatIndex,
                                                                       std::vector<SA_LaunchWindow>& aWindows)
{
   SA_SolutionCache::const_iterator smi = mSA_SolutionCache.find(aThreatIndex);
   if (smi != mSA_SolutionCache.end())
   {
      const SA_Solution& cachedSolution = smi->second;
      // (Here we ignore solution validity time.)
      // if (fabs(cachedSolution.mLaunchTime - aSimTime) <= mSolutionValidityTime)
      //{
      aWindows = cachedSolution.mLaunchWindows;
      return true;
      //}
   }
   return false;
}

// =================================================================================================
// protected
bool WsfBallisticMissileLaunchComputer::ComputeAirIntercept(double          aSimTime,
                                                            WsfPlatform*    aTargetPtr,
                                                            const WsfTrack* aTrackPtr,
                                                            double          aLaunchDelayTime)
{
   if (!mSA_TablePtr ||                // No data provided
       (aTargetPtr->GetSpeed() < 1.0)) // target is not moving
   {
      return false;
   }

   // In a typical engagement intercept calculations are not performed during boost phase
   // because the trajectory prediction would be very inaccurate.

   if (!mAllowBoostPhaseIntercept)
   {
      WsfMover* moverPtr = aTargetPtr->GetMover();
      // NOTE: IsExtrapolating is not maintained by the space movers (so the default of false is used).
      //       If we want to be able to intercept targets using the space mover then we must ignore
      //       this check.
      if ((moverPtr != nullptr) && (!moverPtr->IsExtrapolating())) // IsExtrapolating means boost phase is complete
      {
         return false;
      }
   }

   size_t      targetIndex = 0;
   WsfStringId typeId      = aTargetPtr->GetTypeId();
   double      targetLocWCS[3];
   double      targetVelWCS[3];

   if (mInterceptTruth)
   {
      aTargetPtr->Update(aSimTime);
      targetIndex = aTargetPtr->GetIndex();
      aTargetPtr->GetLocationWCS(targetLocWCS);
      aTargetPtr->GetVelocityWCS(targetVelWCS);
   }
   else // intercept Track
   {
      if (aTrackPtr->LocationValid() && aTrackPtr->VelocityValid())
      {
         targetIndex = aTrackPtr->GetTargetIndex();
         aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, targetLocWCS);
         aTrackPtr->GetVelocityWCS(targetVelWCS);
      }
      else
      {
         return false;
      }
   }

   // Get the predicted target trajectory.
   if (mLineDrawPtr != nullptr)
   {
      mLineDrawPtr->SetColor(1.0, 0.0, 0.0);
   }

   WsfMover::KinematicModel* targetDataPtr =
      GetTargetData(aSimTime, targetIndex, typeId, targetLocWCS, targetVelWCS, aTargetPtr->GetMover());
   if (targetDataPtr == nullptr)
   {
      return false;
   }

   // Capture the target impact time and location for external use.

   double impactLocXYZ[3];
   double impactLocWCS[3];
   bool   impactLocValid = targetDataPtr->GetLocationXYZ(targetDataPtr->GetTimeAtLocation(), impactLocXYZ);
   if (impactLocValid)
   {
      UtSphericalEarth::ConvertSphericalToEllipsoidal(impactLocXYZ, impactLocWCS);
      SetTargetImpactTime(targetDataPtr->GetTimeAtLocation());
      SetTargetImpactPointWCS(impactLocWCS);
   }

   // Also capture the time and location of the target apogee.
   mTargetApogeeTime = targetDataPtr->GetApogeeTime();
   double targetApogeeLocXYZ[3];
   bool   apogeeLocValid = targetDataPtr->GetLocationXYZ(mTargetApogeeTime, targetApogeeLocXYZ);
   if (apogeeLocValid)
   {
      UtSphericalEarth::ConvertSphericalToEllipsoidal(targetApogeeLocXYZ, mTargetApogeeLocWCS);
   }

   // If a previous solution exists for this target and we are within the 'solution commit time',
   // the saved solution is used. The reason is two-fold:
   //
   // 1) Real systems often lock in a solution some time before the real launch time.
   // 2) Recomputing could cause a no solution, thus causing the launch to fail. In most cases
   //    these failures are due to numerical issues.

   double timeToLaunch             = 0.0;
   double timeToIntercept          = -1.0;
   double timeOfFlight             = -1.0;
   double loftAngle                = 0.0;
   double interceptSpeed           = 0.0;
   double interceptFlightPathAngle = 0.0;
   double endTime                  = -1.0;
   double endPointWCS[3]           = {0.0, 0.0, 0.0};
   mLaunchWindows.clear();
   SA_Solution                      solution;
   SA_SolutionCache::const_iterator smi = mSA_SolutionCache.find(aTargetPtr->GetIndex());
   if (smi != mSA_SolutionCache.end())
   {
      const SA_Solution& cachedSolution = smi->second;
      if (fabs(cachedSolution.mLaunchTime - aSimTime) <= mSolutionValidityTime)
      {
         solution                 = cachedSolution;
         timeToLaunch             = solution.mLaunchTime - (aSimTime + aLaunchDelayTime);
         timeToIntercept          = solution.mInterceptTime - (aSimTime + aLaunchDelayTime);
         timeOfFlight             = solution.mTimeOfFlight;
         loftAngle                = solution.mLoftAngle;
         interceptSpeed           = solution.mInterceptSpeed;
         interceptFlightPathAngle = solution.mInterceptFlightPathAngle;
         endTime                  = solution.mEndTime;
         UtVec3d::Set(endPointWCS, solution.mEndPointWCS);
         mLaunchWindows = solution.mLaunchWindows;
      }
   }

   // Compute a solution if a cached solution is not being used.

   if (timeToIntercept < 0.0)
   {
      // No previously cached solution... compute one
      SA_Evaluator evaluator(aSimTime + aLaunchDelayTime, this, targetDataPtr);
      evaluator.FindLaunchWindowStart(timeToLaunch,
                                      timeToIntercept,
                                      timeOfFlight,
                                      loftAngle,
                                      interceptSpeed,
                                      interceptFlightPathAngle);
      if (mComputeLaunchWindows)
      {
         ComputeLaunchWindows(aSimTime, aLaunchDelayTime, timeToLaunch, timeToIntercept, evaluator, mLaunchWindows);
      }
   }
   else if (mComputeLaunchWindows && mLaunchWindows.empty())
   {
      // Previously cached solution exists, but it didn't contain launch windows and now launch windows
      // are being requested. So... compute the launch windows.
      SA_Evaluator evaluator(aSimTime + aLaunchDelayTime + timeToIntercept, this, targetDataPtr);
      ComputeLaunchWindows(aSimTime, aLaunchDelayTime, timeToLaunch, timeToIntercept, evaluator, mLaunchWindows);
   }

   bool canIntercept = (timeToIntercept >= 0.0);
   if (canIntercept)
   {
      SetLaunchTime(aSimTime + aLaunchDelayTime + timeToLaunch);
      SetTimeOfFlight(timeOfFlight);
      SetLoftAngle(loftAngle);

      // The updated target location becomes the intercept point.

      SetInterceptTime(aSimTime + aLaunchDelayTime + timeToIntercept);
      double interceptLocXYZ[3];
      if (!targetDataPtr->GetLocationXYZ(mInterceptTime, interceptLocXYZ))
      {
         return false; // Shouldn't happen, but just in case...
      }
      double interceptLocWCS[3];
      UtSphericalEarth::ConvertSphericalToEllipsoidal(interceptLocXYZ, interceptLocWCS);
      SetInterceptPointWCS(interceptLocWCS);

      if (mComputeEndPoint && (endTime <= 0.0))
      {
         // Compute the approximate point where the interceptor would hit the ground if it
         // simply flew without hitting the target.
         //
         // Assume the interceptor flies in a straight path to the intercept point.
         // Get the unit vector that points from the launcher to the intercept pointer.

         double launchLocWCS[3];
         mWeaponPtr->GetLocationWCS(launchLocWCS);
         double lauToIntLocWCS[3];
         UtVec3d::Subtract(lauToIntLocWCS, interceptLocWCS, launchLocWCS);
         UtVec3d::Normalize(lauToIntLocWCS);

         // Transform the launcher->intercept unit vector from the WCS frame to a local NED frame
         // whose origin is at the intercept point.

         double lat;
         double lon;
         double alt;
         UtEllipsoidalEarth::ConvertECEFToLLA(interceptLocWCS, lat, lon, alt);
         double WCS_ToNED_Transform[3][3];
         double intLocWCS[3];
         UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, intLocWCS);

         double vecNED[3];
         UtMat3d::Transform(vecNED, WCS_ToNED_Transform, lauToIntLocWCS);

         // Form the local NED unit velocity vector of the weapon using the horizontal components of
         // of launcher->intercept unit vector and the flight path angle from the interceptor tables.

         double heading      = atan2(vecNED[1], vecNED[0]);
         double wpnVelNE     = cos(interceptFlightPathAngle) * interceptSpeed;
         double wpnVelD      = -sin(interceptFlightPathAngle) * interceptSpeed;
         double wpnVelNED[3] = {wpnVelNE * cos(heading), wpnVelNE * sin(heading), wpnVelD};

         // Transform the NED velocity back to the WCS frame.

         double wpnVelWCS[3];
         UtMat3d::InverseTransform(wpnVelWCS, WCS_ToNED_Transform, wpnVelNED);

         // Propagate the interceptor forward from the intercept point until it hits the ground.
         WsfMover::KinematicModel* weaponDataPtr = CreateTarget(mInterceptTime,
                                                                aTargetPtr->GetTypeId(),
                                                                interceptLocWCS,
                                                                wpnVelWCS,
                                                                mWeaponPtr->GetPlatform()->GetMover());
         if (weaponDataPtr != nullptr)
         {
            endTime = weaponDataPtr->GetTimeAtLocation();
            double endPointXYZ[3];
            if (weaponDataPtr->GetLocationXYZ(endTime, endPointXYZ))
            {
               UtSphericalEarth::ConvertSphericalToEllipsoidal(endPointXYZ, endPointWCS);
            }
            delete weaponDataPtr;
         }
      }

      // If an endpoint was computed or retrieved from the cache, push it into the solution.

      if (endTime > 0.0)
      {
         SetEndTime(endTime);
         SetEndPointWCS(endPointWCS);
      }

      if (mShowGraphics)
      {
         double locXYZ[3];
         double locWCS[3];

         // Show a big orange dot at the time of evaluation.
         if (targetDataPtr->GetLocationXYZ(aSimTime, locXYZ))
         {
            UtSphericalEarth::ConvertSphericalToEllipsoidal(locXYZ, locWCS);
            ShowPoint(locWCS, 1.0, 0.5, 0.0);
         }

         // Show a big green dot at the target location at time of launch.
         if (targetDataPtr->GetLocationXYZ(mLaunchTime, locXYZ))
         {
            UtSphericalEarth::ConvertSphericalToEllipsoidal(locXYZ, locWCS);
            ShowPoint(locWCS, 0.0, 1.0, 0.0, 15);
         }

         // Show a big red dot at the point of intercept
         mInterceptPoint.GetLocationWCS(impactLocWCS);
         ShowPoint(impactLocWCS, 1.0, 0.0, 0.0, 15);

         // Show a big white dot at the last possible intercept location.
         if (!mLaunchWindows.empty())
         {
            ShowPoint(mLaunchWindows[0].mLastInterceptLocWCS, 1.0, 1.0, 1.0, 15);
         }

         // Show a big yellow down at the 'final' location of the weapon.
         if (endTime > 0.0)
         {
            ShowPoint(endPointWCS, 1.0, 1.0, 0.5, 15);
         }
      }

      // Cache the solution. Don't overwrite the cached solution if the solution came from the cache.

      if (solution.mLaunchTime < 0.0)
      {
         solution.mLaunchTime               = mLaunchTime;
         solution.mInterceptTime            = mInterceptTime;
         solution.mTimeOfFlight             = mTimeOfFlight;
         solution.mLoftAngle                = mLoftAngle;
         solution.mInterceptSpeed           = interceptSpeed;
         solution.mInterceptFlightPathAngle = interceptFlightPathAngle;
         solution.mEndTime                  = endTime;
         UtVec3d::Set(solution.mEndPointWCS, endPointWCS);
      }
      if (mComputeLaunchWindows && solution.mLaunchWindows.empty())
      {
         solution.mLaunchWindows = mLaunchWindows;
      }
      assert(aTargetPtr->GetIndex() == targetIndex);
      mSA_SolutionCache[aTargetPtr->GetIndex()] = solution;
   }

   return canIntercept;
}

// =================================================================================================
// protected
bool WsfBallisticMissileLaunchComputer::ComputeGroundIntercept(double       aSimTime,
                                                               double       aLaunchDelayTime,
                                                               const double aTargetLocWCS[3])
{
   bool canIntercept = false;

   // This is a simple table lookup based on weapon and target location.
   if (mSS_TablePtr)
   {
      double launchLocWCS[3];
      GetPlatform()->GetLocationWCS(launchLocWCS);
      double weaponLat;
      double weaponLon;
      double weaponAlt;
      UtEllipsoidalEarth::ConvertECEFToLLA(launchLocWCS, weaponLat, weaponLon, weaponAlt);
      double WCS_ToNED_Transform[3][3];
      UtEllipsoidalEarth::ComputeNEDTransform(weaponLat, weaponLon, weaponAlt, WCS_ToNED_Transform, launchLocWCS);
      double targetLocNED[3];
      UtEllipsoidalEarth::ConvertECEFToLocal(launchLocWCS, WCS_ToNED_Transform, aTargetLocWCS, targetLocNED);
      double bearing = atan2(targetLocNED[1], targetLocNED[0]);
      SetLauncherBearingOnly(bearing);

      // Ground range is estimated using a spherical earth. WsfGuidedMover uses a spherical earth
      // for its internal model, so we should do the same.

      double targetLat;
      double targetLon;
      double targetAlt;
      UtEllipsoidalEarth::ConvertECEFToLLA(aTargetLocWCS, targetLat, targetLon, targetAlt);
      double targetLocWCS[3];
      UtSphericalEarth::ConvertLLAToECEF(weaponLat, weaponLon, weaponAlt, launchLocWCS);
      UtSphericalEarth::ConvertLLAToECEF(targetLat, targetLon, targetAlt, targetLocWCS);
      double range =
         UtSphericalEarth::cEARTH_RADIUS * acos(UtVec3d::DotProduct(launchLocWCS, targetLocWCS) /
                                                UtVec3d::Magnitude(launchLocWCS) / UtVec3d::Magnitude(targetLocWCS));
      double minRange = mSS_TablePtr->mRange.Get(0);
      size_t maxIndex = mSS_TablePtr->mRange.GetSize() - 1;
      double maxRange = mSS_TablePtr->mRange.Get(maxIndex);
      if ((range >= minRange) && (range <= maxRange))
      {
         SetLaunchTime(aSimTime);
         SetLauncherBearing(bearing);

         TblLookupLUX<double> rangeLookup;
         rangeLookup.Lookup(mSS_TablePtr->mRange, range);
         mLastEvaluatedGroundRange = range;

         SetLoftAngle(TblEvaluate(mSS_TablePtr->mLoftAngle, rangeLookup));
         SetBurnTime(TblEvaluate(mSS_TablePtr->mBurnTime, rangeLookup));
         SetTimeOfFlight(TblEvaluate(mSS_TablePtr->mTimeOfFlight, rangeLookup));

         // Note:  Some confusion here over whether this would be an "Impact" or an "Intercept",
         // but just stayed with the established convention anyway.
         SetInterceptTime(aSimTime + mTimeOfFlight);
         SetInterceptPointWCS(aTargetLocWCS);

         // Also set the 'end' time/position
         SetEndTime(aSimTime + mTimeOfFlight);
         SetEndPointWCS(aTargetLocWCS);

         if (ShowResults())
         {
            std::ostringstream oss;
            oss << std::fixed;
            oss << "  Bearing: " << std::setprecision(4) << mLauncherBearing * UtMath::cDEG_PER_RAD << " deg";
            oss << ", Ground Range: " << std::setprecision(2) << range << " m";
            oss << "; Loft angle: " << std::setprecision(4) << mLoftAngle * UtMath::cDEG_PER_RAD << " deg";
            oss << ", Burn Time: " << std::setprecision(4) << mBurnTime << " sec";
            GetPlatform()->Comment(aSimTime, oss.str());

            size_t index = rangeLookup.GetIndex();

            auto out = ut::log::info() << "Ground Intercept Results:" << std::fixed;
            out.AddNote() << "Bearing: " << std::setprecision(4) << mLauncherBearing * UtMath::cDEG_PER_RAD << " deg";
            out.AddNote() << "Ground Range: " << std::setprecision(2) << range << " m";
            out.AddNote() << "Loft angle: " << std::setprecision(4) << mLoftAngle * UtMath::cDEG_PER_RAD << " deg";
            out.AddNote() << "Burn Time: " << std::setprecision(4) << mBurnTime << " sec";
            auto note = out.AddNote() << "Table Bounds:";
            note.AddNote() << "Range: " << std::setprecision(2) << mSS_TablePtr->mRange.Get(index) << ' '
                           << mSS_TablePtr->mRange.Get(index + 1);
            note.AddNote() << "Burn Time: " << std::setprecision(4) << mSS_TablePtr->mBurnTime.Get(index) << ' '
                           << mSS_TablePtr->mBurnTime.Get(index + 1);
         }

         if (mShowGraphics)
         {
            ShowPoint(aTargetLocWCS);
         }
      }
      else if (ShowResults())
      {
         std::ostringstream oss;
         oss << std::fixed;
         oss << "***** WARNING: Target range " << std::setprecision(4) << range
             << " m is not within the range limits [ " << minRange << ' ' << maxRange << " ]";
         GetPlatform()->Comment(aSimTime, oss.str());
         auto out = ut::log::warning() << std::fixed << "Target range is not within the range limits.";
         out.AddNote() << "Range: " << std::setprecision(4) << range << " m";
         out.AddNote() << "Minimum Range: " << minRange;
         out.AddNote() << "Maximum Range: " << maxRange;
      }
   }
   return canIntercept;
}

// =================================================================================================
// protected static
void WsfBallisticMissileLaunchComputer::ComputeLaunchWindows(double                        aSimTime,
                                                             double                        aLaunchDelayTime,
                                                             double                        aTimeToLaunch,
                                                             double                        aTimeToIntercept,
                                                             SA_Evaluator&                 aEvaluator,
                                                             std::vector<SA_LaunchWindow>& aLaunchWindows)
{
   double          targetLocXYZ[3];
   SA_LaunchWindow launchWindow;

   double timeToLaunch    = aTimeToLaunch;
   double timeToIntercept = aTimeToIntercept;
   double timeOfFlight;
   double loftAngle;
   double interceptSpeed;
   double interceptFlightPathAngle;

   while (timeToIntercept >= 0.0)
   {
      // Capture the start of the launch window.
      launchWindow.mFirstLaunchTime    = aSimTime + aLaunchDelayTime + timeToLaunch;
      launchWindow.mFirstInterceptTime = aSimTime + aLaunchDelayTime + timeToIntercept;
      if (aEvaluator.mTargetDataPtr->GetLocationXYZ(launchWindow.mFirstInterceptTime, targetLocXYZ))
      {
         UtSphericalEarth::ConvertSphericalToEllipsoidal(targetLocXYZ, launchWindow.mFirstInterceptLocWCS);
      }

      // Determine the end of the launch window.
      aEvaluator.FindLaunchWindowEnd(timeToLaunch, timeToIntercept, timeOfFlight, loftAngle, interceptSpeed, interceptFlightPathAngle);

      // Capture the end of the launch window and store the completed launch window.
      if (timeOfFlight > 0.0)
      {
         launchWindow.mLastLaunchTime    = aSimTime + aLaunchDelayTime + timeToLaunch;
         launchWindow.mLastInterceptTime = aSimTime + aLaunchDelayTime + timeToIntercept;
         if (aEvaluator.mTargetDataPtr->GetLocationXYZ(launchWindow.mLastInterceptTime, targetLocXYZ))
         {
            UtSphericalEarth::ConvertSphericalToEllipsoidal(targetLocXYZ, launchWindow.mLastInterceptLocWCS);
         }
      }
      else
      {
         // Couldn't find the end of the launch window (which shouldn't happen). Issue a warning and simply use the start point.
         auto out = ut::log::warning() << "Could not determine end of launch window.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         launchWindow.mLastLaunchTime    = launchWindow.mFirstLaunchTime;
         launchWindow.mLastInterceptTime = launchWindow.mFirstInterceptTime;
         UtVec3d::Set(launchWindow.mLastInterceptLocWCS, launchWindow.mFirstInterceptLocWCS);
      }
      aLaunchWindows.push_back(launchWindow);

      // Locate the start of the next launch window.
      aEvaluator.FindLaunchWindowStart(timeToLaunch,
                                       timeToIntercept,
                                       timeOfFlight,
                                       loftAngle,
                                       interceptSpeed,
                                       interceptFlightPathAngle);
   }
}

// =================================================================================================
// protected
void WsfBallisticMissileLaunchComputer::DrawRanges()
{
   if (mLineDrawPtr == nullptr)
   {
      return;
   }
   double minAlt = std::max(mSA_TablePtr->mMinAltitude, mMinInterceptAltitude);
   double maxAlt = std::min(mSA_TablePtr->mMaxAltitude, mMaxInterceptAltitude);

   int                 altCount = 30;
   double              altStep  = (maxAlt - minAlt) / altCount;
   std::vector<double> alt(altCount);
   std::vector<double> minRange(altCount, DBL_MAX);
   std::vector<double> maxRange(altCount, -DBL_MAX);
   for (int i = 0; i < altCount; ++i)
   {
      alt[i] = minAlt + (i * altStep);
   }

   for (int ri = 0; ri < mSA_TablePtr->mRangePointCount; ++ri)
   {
      double range = mSA_TablePtr->RangeOfIndex(ri);
      for (size_t ai = 0; ai < alt.size(); ++ai)
      {
         double altitude = alt[ai];
         if ((altitude >= mSA_TablePtr->mRangeTable[ri].mIncreasingLine.mMinAltitude) &&
             (altitude <= mSA_TablePtr->mRangeTable[ri].mIncreasingLine.mMaxAltitude))
         {
            minRange[ai] = std::min(minRange[ai], range);
            maxRange[ai] = std::max(maxRange[ai], range);
         }
         else if ((altitude >= mSA_TablePtr->mRangeTable[ri].mDecreasingLine.mMinAltitude) &&
                  (altitude <= mSA_TablePtr->mRangeTable[ri].mDecreasingLine.mMaxAltitude))
         {
            minRange[ai] = std::min(minRange[ai], range);
            maxRange[ai] = std::max(maxRange[ai], range);
         }
      }
   }

   double refLat;
   double refLon;
   double refAlt;
   GetPlatform()->GetLocationLLA(refLat, refLon, refAlt);

   mLineDrawPtr->SetId(mLineDrawPtr->GetNewId());
   mLineDrawPtr->SetDuration(WsfDraw::cFOREVER);
   mLineDrawPtr->SetColor(0.0, 1.0, 1.0);
   mLineDrawPtr->SetEllipseMode(WsfDraw::cELLIPSE_LINE);
   double a = UtEarth::cA;

   for (int ai = 0; ai < altCount; ++ai)
   {
      if (minRange[ai] < DBL_MAX)
      {
         double b    = a + alt[ai];
         double rnew = b * sin(minRange[ai] / a);
         double anew = sqrt(b * b - rnew * rnew) - a;
         mLineDrawPtr->BeginEllipse(0, rnew, rnew);
         mLineDrawPtr->VertexLLA(refLat, refLon, anew);
         mLineDrawPtr->End();
      }
   }

   mLineDrawPtr->SetId(mLineDrawPtr->GetNewId());
   for (int ai = 0; ai < altCount; ++ai)
   {
      if (maxRange[ai] > 0.0)
      {
         double b    = a + alt[ai];
         double rnew = b * sin(maxRange[ai] / a);
         double anew = sqrt(b * b - rnew * rnew) - a;
         mLineDrawPtr->BeginEllipse(0, rnew, rnew);
         mLineDrawPtr->VertexLLA(refLat, refLon, anew);
         mLineDrawPtr->End();
      }
   }
}

// =================================================================================================
// protected
WsfMover::KinematicModel* WsfBallisticMissileLaunchComputer::GetTargetData(double       aSimTime,
                                                                           size_t       aTargetIndex,
                                                                           WsfStringId  aTargetTypeId,
                                                                           const double aTargetLocWCS[3],
                                                                           const double aTargetVelWCS[3],
                                                                           WsfMover*    aMoverPtr)
{
   WsfMover::KinematicModel* targetDataPtr = nullptr;
   auto                      tmi           = mSharedDataPtr->mTargetDataMap.find(aTargetIndex);
   if (tmi != mSharedDataPtr->mTargetDataMap.end())
   {
      targetDataPtr = (*tmi).second;
      double trueLocXYZ[3];
      UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(aTargetLocWCS, trueLocXYZ);
      double predLocXYZ[3];
      if (targetDataPtr->GetLocationXYZ(aSimTime, predLocXYZ))
      {
         double deltaLocXYZ[3];
         UtVec3d::Subtract(deltaLocXYZ, predLocXYZ, trueLocXYZ);
         double delta = UtVec3d::Magnitude(deltaLocXYZ);

         if (delta > mPredictedTrajectoryErrorLimit)
         {
            delete targetDataPtr;
            targetDataPtr = nullptr;
            mSharedDataPtr->mTargetDataMap.erase(tmi);
            if (mShowResults)
            {
               std::string  tgtname = "unknown";
               WsfPlatform* tgtPtr  = GetSimulation()->GetPlatformByIndex(aTargetIndex);
               if (tgtPtr != nullptr)
               {
                  tgtname = tgtPtr->GetName();
               }
               auto out = ut::log::error() << "Ballistic target data for target is obsolete.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Error: " << delta << " m";
            }
         }
      }
   }
   if (targetDataPtr == nullptr)
   {
      std::string  tgtname("unknown");
      WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(aTargetIndex);
      if (tgtPtr != nullptr)
      {
         tgtname = tgtPtr->GetName();
      }
      if (mShowResults)
      {
         auto out = ut::log::info() << "Creating ballistic target data for target.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Target: " << tgtname;
      }
      targetDataPtr = CreateTarget(aSimTime, aTargetTypeId, aTargetLocWCS, aTargetVelWCS, aMoverPtr);
      if (targetDataPtr != nullptr)
      {
         mSharedDataPtr->mTargetDataMap[aTargetIndex] = targetDataPtr;
      }
      else
      {
         auto out = ut::log::info() << "Unable to create ballistic target data for target.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Target: " << tgtname;
      }
   }
   return targetDataPtr;
}

WsfMover::KinematicModel* WsfBallisticMissileLaunchComputer::CreateTarget(double       aSimTime,
                                                                          WsfStringId  aTargetTypeId,
                                                                          const double aTargetLocWCS[3],
                                                                          const double aTargetVelWCS[3],
                                                                          WsfMover*    aMoverPtr)
{
   WsfMover::KinematicModel* targetDataPtr = nullptr;
   if (aMoverPtr != nullptr)
   {
      targetDataPtr =
         aMoverPtr->CreateKinematicModel(aSimTime, aTargetTypeId, aTargetLocWCS, aTargetVelWCS, nullptr, mLineDrawPtr.get());
      if (targetDataPtr == nullptr)
      {
         // Check if the mover has been swapped, as with a DIS Mover.  If so check whether we can create target data
         // based on the original mover type.
         WsfPlatform* typePtr =
            GetSimulation()->GetScenario().GetPlatformTypes().Find(aMoverPtr->GetPlatform()->GetType());
         if ((typePtr->GetMover() != nullptr) && (typePtr->GetMover()->GetType() != aMoverPtr->GetType()))
         {
            targetDataPtr = typePtr->GetMover()->CreateKinematicModel(aSimTime,
                                                                      aTargetTypeId,
                                                                      aTargetLocWCS,
                                                                      aTargetVelWCS,
                                                                      aMoverPtr,
                                                                      mLineDrawPtr.get());
         }
      }
   }

   if (targetDataPtr == nullptr)
   {
      // Assume ballistic target modeled by WsfGuidedMover or some other mover.
      targetDataPtr = new BallisticModel(this,
                                         aSimTime,
                                         aTargetTypeId,
                                         aTargetLocWCS,
                                         aTargetVelWCS,
                                         mIntegrationTimeStep,
                                         mLineDrawPtr.get());
   }
   return targetDataPtr;
}

// =================================================================================================
//! A simulation observer for platform deletions.
//!
//! This method is registered as a simulation callback and is called whenever a platform is deleted.
//! The method will delete any cached launch computer solutions associated with the target.
// protected
void WsfBallisticMissileLaunchComputer::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   PurgeSolution(aSimTime, aPlatformPtr->GetIndex(), "PlatformDeleted");
}

// =================================================================================================
//! Queue a purge a solution from the cache.
// protected
void WsfBallisticMissileLaunchComputer::PurgeSolution(double aSimTime, size_t aTargetIndex, const std::string& aCaller)
{
   GetSimulation()->AddEvent(ut::make_unique<PurgeSolutionEvent>(aSimTime + 0.001, this, aTargetIndex, aCaller));
}

// =================================================================================================
//! Purge a solution from the cache.
// protected
void WsfBallisticMissileLaunchComputer::QueuedSolutionPurge(double aSimTime, size_t aTargetIndex, const std::string& aCaller)
{
   auto sci = mSA_SolutionCache.find(aTargetIndex);
   if (sci != mSA_SolutionCache.end())
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Deleting cached solution for target platform.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << GetWeapon()->GetName();
         out.AddNote() << "Target Platform: " << GetSimulation()->GetPlatformNameId(aTargetIndex);
         out.AddNote() << "Caller: " << aCaller;
      }
      mSA_SolutionCache.erase(sci);
   }
}

// =================================================================================================
// protected
void WsfBallisticMissileLaunchComputer::ShowPoint(const double aLocWCS[3],
                                                  double       aRedValue,
                                                  double       aGreenValue,
                                                  double       aBlueValue,
                                                  int          aPointSize)
{
   if (mDrawPtr == nullptr)
   {
      return;
   }
   mDrawPtr->SetId(mDrawPtr->GetNewId());
   mDrawPtr->SetPointSize(aPointSize);
   mDrawPtr->SetDuration(WsfDraw::cFOREVER);
   mDrawPtr->SetColor(aRedValue, aGreenValue, aBlueValue);
   mDrawPtr->BeginPoints();
   mDrawPtr->VertexWCS(aLocWCS);
   mDrawPtr->VertexWCS(aLocWCS); // Need two point because of VESPA bug
   mDrawPtr->End();
}

// =================================================================================================
// protected
void WsfBallisticMissileLaunchComputer::WeaponFireAborted(double          aSimTime,
                                                          WsfWeapon*      aWeaponPtr,
                                                          const WsfTrack* aTrackPtr,
                                                          double          aQuantity)
{
   if ((aWeaponPtr == GetWeapon()) && (aTrackPtr != nullptr))
   {
      PurgeSolution(aSimTime, aTrackPtr->GetTargetIndex(), "WeaponFireAborted");
   }
}

// =================================================================================================
//! Simulation observer for a weapon firing.
// protected
void WsfBallisticMissileLaunchComputer::WeaponFired(double                     aSimTime,
                                                    const WsfWeaponEngagement* aEngagementPtr,
                                                    const WsfTrack*            aTrackPtr)
{
   // Only process if associated with the weapon for which I am the computer.

   if ((aEngagementPtr != nullptr) && (aEngagementPtr->GetWeaponSystem() == GetWeapon()))
   {
      PurgeSolution(aSimTime, aEngagementPtr->GetTargetPlatformIndex(), "WeaponFired");
   }
}

// =================================================================================================
// virtual
bool WsfBallisticMissileLaunchComputer::InitializeTTIData()
{
   // No TTI data here... must return true to prevent base class failure.
   return true;
}

// =================================================================================================
// Nested class 'SS_Table'
// =================================================================================================
bool WsfBallisticMissileLaunchComputer::SS_Table::Read(const std::string& aFileName)
{
   bool          success = false;
   std::ifstream ifs(aFileName.c_str());
   if (ifs.is_open())
   {
      success = true;

      std::string line;
      // Ignore any comments
      while (ifs.peek() == '#')
      {
         getline(ifs, line);
      }

      std::vector<double> ranges;
      std::vector<double> loftAngles;
      std::vector<double> burnTimes;
      std::vector<double> timeOfFlights;
      std::vector<double> speeds;
      std::vector<double> flightPathAngles;

      std::vector<double> bo_timeOfFlights;
      std::vector<double> bo_ranges;
      std::vector<double> bo_speeds;
      std::vector<double> bo_flightPathAngles;

      size_t initialCapacity(4096U);
      ranges.reserve(initialCapacity);
      loftAngles.reserve(initialCapacity);
      burnTimes.reserve(initialCapacity);
      timeOfFlights.reserve(initialCapacity);
      speeds.reserve(initialCapacity);
      flightPathAngles.reserve(initialCapacity);

      bo_timeOfFlights.reserve(initialCapacity);
      bo_ranges.reserve(initialCapacity);
      bo_speeds.reserve(initialCapacity);
      bo_flightPathAngles.reserve(initialCapacity);

      double loftAngle       = 0.0;
      double range           = 0.0;
      double burnTime        = 0.0;
      double timeOfFlight    = 0.0;
      double speed           = 1000.0; // Default if not provided
      double flightPathAngle = 50.0;   // Default if not provided
      bool   firstPoint      = true;

      double bo_timeOfFlight    = 0.0;
      double bo_range           = 0.0;
      double bo_speed           = 0.0;
      double bo_flightPathAngle = 0.0;

      while ((ifs >> loftAngle >> burnTime >> range >> timeOfFlight))
      {
         // Read the optional speed and flight path angle data if it is present.
         // If the first point has it then all points must have it.
         // ... The same also goes for burnout data after the velocity data.

         if (firstPoint)
         {
            firstPoint       = false;
            mHasVelocityData = false;
            mHasBurnoutData  = false;

            getline(ifs, line); // Get the remainder of the line
            std::istringstream iss(line);
            if ((iss >> speed >> flightPathAngle))
            {
               mHasVelocityData = true;

               if ((iss >> bo_timeOfFlight >> bo_range >> bo_speed >> bo_flightPathAngle))
               {
                  mHasBurnoutData = true;
               }
            }
         }
         else if (mHasVelocityData)
         {
            if (!(ifs >> speed >> flightPathAngle))
            {
               auto out = ut::log::error() << "Input error encountered.";
               out.AddNote() << "File: " << aFileName;
               return false;
            }

            if (mHasBurnoutData)
            {
               if (!(ifs >> bo_timeOfFlight >> bo_range >> bo_speed >> bo_flightPathAngle))
               {
                  auto out = ut::log::error() << "Input error encountered.";
                  out.AddNote() << "File: " << aFileName;
                  return false;
               }
            }

            getline(ifs, line); // Ignore the remainder of the line
         }
         else
         {
            getline(ifs, line); // Ignore the remainder of the line
         }

         ranges.push_back(range);

         loftAngles.push_back(loftAngle * UtMath::cRAD_PER_DEG);
         burnTimes.push_back(burnTime);

         timeOfFlights.push_back(timeOfFlight);
         speeds.push_back(speed);
         flightPathAngles.push_back(flightPathAngle * UtMath::cRAD_PER_DEG);

         bo_timeOfFlights.push_back(bo_timeOfFlight);
         bo_ranges.push_back(bo_range);
         bo_speeds.push_back(bo_speed);
         bo_flightPathAngles.push_back(bo_flightPathAngle * UtMath::cRAD_PER_DEG);
      }

      mRange.SetValues(ranges);

      mLoftAngle.SetValues(loftAngles);
      mBurnTime.SetValues(burnTimes);
      mTimeOfFlight.SetValues(timeOfFlights);
      mSpeed.SetValues(speeds);
      mFlightPathAngle.SetValues(flightPathAngles);

      mBO_TimeOfFlight.SetValues(bo_timeOfFlights);
      mBO_Range.SetValues(bo_ranges);
      mBO_Speed.SetValues(bo_speeds);
      mBO_FlightPathAngle.SetValues(bo_flightPathAngles);
   }
   return success;
}

// =================================================================================================
// Nested class 'SA_Table'
// =================================================================================================

WsfBallisticMissileLaunchComputer::SA_Table::SA_Table(double aRangeInterval)
   : mMinRange(DBL_MAX)
   , mMaxRange(DBL_MIN)
   , mMinAltitude(DBL_MAX)
   , mMaxAltitude(DBL_MIN)
   , mMinTimeOfFlight(DBL_MAX)
   , mMaxTimeOfFlight(DBL_MIN)
   , mRangePointCount(0)
   , mHasVelocityData(false)
{
   mRangeInterval = aRangeInterval;
}

// =================================================================================================
WsfBallisticMissileLaunchComputer::SA_Table::~SA_Table() = default;

// =================================================================================================
void WsfBallisticMissileLaunchComputer::SA_Table::ComputeRoundedLimits(double  aInterval,
                                                                       double& aMinValue,
                                                                       double& aMaxValue,
                                                                       int&    aPointCount)
{
   int minIntValue = static_cast<int>(aMinValue / aInterval);
   if ((minIntValue * aInterval) > aMinValue)
   {
      --minIntValue;
   }
   aMinValue = minIntValue * aInterval;

   int maxIntValue = static_cast<int>(aMaxValue / aInterval);
   if ((maxIntValue * aInterval) < aMaxValue)
   {
      ++maxIntValue;
   }
   aMaxValue   = maxIntValue * aInterval;
   aPointCount = maxIntValue - minIntValue + 1;
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::SA_Table::ConvertRangeLine(TempRangeLine& aTempRangeLine, RangeLine& aRealRangeLine)
{
   int count = static_cast<int>(aTempRangeLine.size());
   if (count != 0)
   {
      // Resize the output lines to the proper size. For a case where there is exactly one point,
      // allocate space for two so the lookups will work properly.

      int pointCount = (count == 1) ? 2 : count;
      aRealRangeLine.mAltitude.Resize(pointCount);
      aRealRangeLine.mTimeOfFlight.Resize(pointCount);
      aRealRangeLine.mLoftAngle.Resize(pointCount);

      // Copy the temporary values to the permanent values.

      for (int i = 0; i < count; ++i)
      {
         aRealRangeLine.mAltitude.Set(aTempRangeLine[i].mAltitude, i);
         aRealRangeLine.mTimeOfFlight.Set(aTempRangeLine[i].mTimeOfFlight, i);
         aRealRangeLine.mLoftAngle.Set(aTempRangeLine[i].mLoftAngle, i);
      }

      // If only one temporary value, create a second permanent value.
      // An altitude interval is created so interpolation works properly.

      if (count == 1)
      {
         aRealRangeLine.mAltitude.Set(aTempRangeLine[0].mAltitude - 500.0, 0);
         aRealRangeLine.mAltitude.Set(aTempRangeLine[0].mAltitude + 500.0, 1);
         aRealRangeLine.mTimeOfFlight.Set(aTempRangeLine[0].mTimeOfFlight, 1);
         aRealRangeLine.mLoftAngle.Set(aTempRangeLine[0].mLoftAngle, 1);
      }

      // Repeat the above steps for the optional speed and flight path angle
      if (mHasVelocityData)
      {
         aRealRangeLine.mSpeed.Resize(pointCount);
         aRealRangeLine.mFlightPathAngle.Resize(pointCount);

         for (int i = 0; i < count; ++i)
         {
            aRealRangeLine.mSpeed.Set(aTempRangeLine[i].mSpeed, i);
            aRealRangeLine.mFlightPathAngle.Set(aTempRangeLine[i].mFlightPathAngle, i);
         }

         if (count == 1)
         {
            aRealRangeLine.mSpeed.Set(aTempRangeLine[0].mSpeed, 1);
            aRealRangeLine.mFlightPathAngle.Set(aTempRangeLine[0].mFlightPathAngle, 1);
         }
      }

      // Set the min/max limits for quick comparison.

      size_t lastIndex = aRealRangeLine.mAltitude.GetSize() - 1;
      if (aRealRangeLine.mAltitude.Get(0) < aRealRangeLine.mAltitude.Get(1))
      {
         aRealRangeLine.mMinAltitude = aRealRangeLine.mAltitude.Get(0);
         aRealRangeLine.mMaxAltitude = aRealRangeLine.mAltitude.Get(lastIndex);
      }
      else
      {
         aRealRangeLine.mMinAltitude = aRealRangeLine.mAltitude.Get(lastIndex);
         aRealRangeLine.mMaxAltitude = aRealRangeLine.mAltitude.Get(0);
      }
   }
   else
   {
      aRealRangeLine.mMinAltitude = DBL_MAX;
      aRealRangeLine.mMaxAltitude = -DBL_MAX;
   }
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::SA_Table::PrintRangeLine(const RangeLine&        aRangeLine,
                                                                 ut::log::MessageStream& aStream) const
{
   size_t pointCount = aRangeLine.mAltitude.GetSize();
   aStream.AddNote() << "Number of points: " << pointCount;
   if (pointCount > 0)
   {
      aStream.AddNote() << "Minimum Altitude: " << aRangeLine.mMinAltitude;
      aStream.AddNote() << "Maximum Altitude: " << aRangeLine.mMaxAltitude;
   }
   for (size_t i = 0; i < pointCount; ++i)
   {
      auto point = aStream.AddNote() << "Point #" << i << ":";
      point.AddNote() << "Altitude: " << aRangeLine.mAltitude.Get(i);
      point.AddNote() << "TOF: " << aRangeLine.mTimeOfFlight.Get(i);
      point.AddNote() << "Loft: " << aRangeLine.mLoftAngle.Get(i) * UtMath::cDEG_PER_RAD << " deg";
   }
}

// =================================================================================================
bool WsfBallisticMissileLaunchComputer::SA_Table::Read(const std::string& aFileName, bool aDebugEnabled, bool aDebugFileLoading)
{
   std::ifstream ifs(aFileName);
   if (!ifs.is_open())
   {
      auto out = ut::log::error() << "Unable to open file.";
      out.AddNote() << "File: " << aFileName;
      return false;
   }

   std::string line;
   // Ignore any comments
   while (ifs.peek() == '#')
   {
      getline(ifs, line);
   }

   // Read and store each trajectory

   std::list<Trajectory> trajectories;

   size_t              initialCapacity(4096U);
   std::vector<double> ranges;
   std::vector<double> altitudes;
   std::vector<double> timeOfFlights;
   std::vector<double> speeds;
   std::vector<double> flightPathAngles;
   ranges.reserve(initialCapacity);
   altitudes.reserve(initialCapacity);
   timeOfFlights.reserve(initialCapacity);
   speeds.reserve(initialCapacity);
   flightPathAngles.reserve(initialCapacity);

   double loftAngle;
   bool   firstPoint = true;
   while ((ifs >> loftAngle))
   {
      getline(ifs, line); // Ignore remainder of the line
      ranges.clear();
      altitudes.clear();
      speeds.clear();
      flightPathAngles.clear();
      timeOfFlights.clear();
      double minAltitude = DBL_MAX;
      double maxAltitude = DBL_MIN;
      double range;
      double altitude;
      double timeOfFlight;
      double speed           = 0.0;
      double flightPathAngle = 0.0;
      while ((ifs >> range >> altitude >> timeOfFlight) && (range >= 0.0))
      {
         // Read the optional speed and flight path angle data if it is present.
         // If the first point has it then all points must have it.

         if (firstPoint)
         {
            firstPoint       = false;
            mHasVelocityData = false;
            getline(ifs, line); // Get the remainder of the line
            std::istringstream iss(line);
            if ((iss >> speed >> flightPathAngle))
            {
               mHasVelocityData = true;
            }
         }
         else if (mHasVelocityData)
         {
            if (!(ifs >> speed >> flightPathAngle))
            {
               auto out = ut::log::error() << "Input error encountered.";
               out.AddNote() << "File: " << aFileName;
               return false;
            }
            getline(ifs, line); // Ignore the remainder of the line
         }
         else
         {
            getline(ifs, line); // Ignore the remainder of the line
         }

         // Make ground range and time of flight values are ascending...
         if (!ranges.empty())
         {
            if (range < ranges.back())
            {
               ut::log::error() << "Ground ranges must be ascending.";
               return false;
            }
            if (timeOfFlight <= timeOfFlights.back())
            {
               ut::log::error() << "Time of flight values must be monotonically ascending.";
               return false;
            }
         }

         ranges.push_back(range);
         altitudes.push_back(altitude);
         timeOfFlights.push_back(timeOfFlight);
         minAltitude = std::min(altitude, minAltitude);
         maxAltitude = std::max(altitude, maxAltitude);
         if (mHasVelocityData)
         {
            speeds.push_back(speed);
            flightPathAngles.push_back(flightPathAngle * UtMath::cRAD_PER_DEG);
         }
      }
      getline(ifs, line); // Ignore remainder of the termination line

      if (ranges.empty())
      {
         auto out = ut::log::error() << "File contains an empty trajectory.";
         out.AddNote() << "File: " << aFileName;
         return false;
      }

      // Populate the 'Trajectory' object and store in the list of trajectories.

      trajectories.emplace_back();
      Trajectory& trajectory = trajectories.back();
      trajectory.mRange.SetValues(ranges);
      trajectory.mAltitude.SetValues(altitudes);
      trajectory.mTimeOfFlight.SetValues(timeOfFlights);
      trajectory.mSpeed.SetValues(speeds);
      trajectory.mFlightPathAngle.SetValues(flightPathAngles);

      trajectory.mLoftAngle       = loftAngle * UtMath::cRAD_PER_DEG;
      trajectory.mMinRange        = ranges.front();
      trajectory.mMaxRange        = ranges.back();
      trajectory.mMinAltitude     = minAltitude;
      trajectory.mMaxAltitude     = maxAltitude;
      trajectory.mMinTimeOfFlight = timeOfFlights.front();
      trajectory.mMaxTimeOfFlight = timeOfFlights.back();

      if (aDebugFileLoading)
      {
         auto out = ut::log::debug() << "Trajectory Statistics:";
         out.AddNote() << "Number of Trajectories: " << trajectories.size();
         out.AddNote() << "Number of points: " << trajectory.mRange.GetSize();
         out.AddNote() << "Loft Angle: " << trajectory.mLoftAngle * UtMath::cDEG_PER_RAD;
         out.AddNote() << "Min Ground Range: " << trajectory.mMinRange;
         out.AddNote() << "Max Ground Range: " << trajectory.mMaxRange;
         out.AddNote() << "Min Altitude: " << trajectory.mMinAltitude;
         out.AddNote() << "Max Altitude: " << trajectory.mMaxAltitude;
         out.AddNote() << "Min Time Of Flight: " << trajectory.mMinTimeOfFlight;
         out.AddNote() << "Max Time Of Flight: " << trajectory.mMaxTimeOfFlight;
      }

      // Update the global minimum/maximum values.

      mMinRange        = std::min(mMinRange, trajectory.mMinRange);
      mMaxRange        = std::max(mMaxRange, trajectory.mMaxRange);
      mMinAltitude     = std::min(mMinAltitude, trajectory.mMinAltitude);
      mMaxAltitude     = std::max(mMaxAltitude, trajectory.mMaxAltitude);
      mMinTimeOfFlight = std::min(mMinTimeOfFlight, trajectory.mMinTimeOfFlight);
      mMaxTimeOfFlight = std::max(mMaxTimeOfFlight, trajectory.mMaxTimeOfFlight);

   } // while ((ifs << loftAngle ...

   if (trajectories.empty())
   {
      auto out = ut::log::error() << "No trajectories in file.";
      out.AddNote() << "File: " << aFileName;
      return false;
   }

   if (aDebugEnabled || aDebugFileLoading)
   {
      auto out = ut::log::debug() << "Global Trajectory Statistics:";
      out.AddNote() << "Min Ground Range: " << mMinRange;
      out.AddNote() << "Max Ground Range: " << mMaxRange;
      out.AddNote() << "Min Altitude: " << mMinAltitude;
      out.AddNote() << "Max Altitude: " << mMaxAltitude;
      out.AddNote() << "Min Time Of Flight: " << mMinTimeOfFlight;
      out.AddNote() << "Max Time Of Flight: " << mMaxTimeOfFlight;
   }

   // Determine the rounded values that total encompass the values in the trajectories

   ComputeRoundedLimits(mRangeInterval, mMinRange, mMaxRange, mRangePointCount);
   int tempPointCount;
   ComputeRoundedLimits(mRangeInterval, mMinAltitude, mMaxAltitude, tempPointCount);

   mRangeTable.resize(mRangePointCount);
   std::vector<TempRangeEntry> tempRangeTable(mRangePointCount);

   // For each trajectory, create the intersection objects for point where the trajectory
   // intersects one of our lines of constant ground range.

   TempPoint intersection;
   for (Trajectory& trajectory : trajectories)
   {
      int minRangeIndex = IndexOfRange(trajectory.mMinRange);
      int maxRangeIndex = IndexOfRange(trajectory.mMaxRange);

      // The indices should be wholly contained within the trajectory range. RangeIndex will return
      // the index of the line whose range is LESS THAN OR EQUAL to the supplied range. For the upper bound
      // this is no problem, but for the lower bound we may need to adjust.

      if ((mMinRange + (minRangeIndex * mRangeInterval)) < trajectory.mMinRange)
      {
         ++minRangeIndex;
      }

      TblLookupLU<double> rangeLU;
      for (int rangeIndex = minRangeIndex; rangeIndex <= maxRangeIndex; ++rangeIndex)
      {
         // Determine the altitude, time of flight and loft angle at this range.

         double range = mMinRange + (rangeIndex * mRangeInterval);
         rangeLU.Lookup(trajectory.mRange, range);
         intersection.mAltitude     = TblEvaluate(trajectory.mAltitude, rangeLU);
         intersection.mAltitude     = static_cast<int>(intersection.mAltitude + 0.5); // See reason below
         intersection.mTimeOfFlight = TblEvaluate(trajectory.mTimeOfFlight, rangeLU);
         intersection.mLoftAngle    = trajectory.mLoftAngle;
         if (mHasVelocityData)
         {
            intersection.mSpeed           = TblEvaluate(trajectory.mSpeed, rangeLU);
            intersection.mFlightPathAngle = TblEvaluate(trajectory.mFlightPathAngle, rangeLU);
         }

         // Decide which line into which to insert the point.
         //
         // Note that the altitude has been rounded to the nearest integer and that we allow a little slop when
         // determining if we're on the ascending or descending line. This is because many trajectories pass through
         // the same point very early in the flight..

         bool increasingLineEmpty = tempRangeTable[rangeIndex].mIncreasingLine.empty();
         bool decreasingLineEmpty = tempRangeTable[rangeIndex].mDecreasingLine.empty();
         if (increasingLineEmpty && decreasingLineEmpty)
         {
            tempRangeTable[rangeIndex].mIncreasingLine.push_back(intersection);
         }
         else if (!decreasingLineEmpty)
         {
            // Once the 'decreasing' altitude line is started, all subsequent points should be decreasing altitude.
            double lastAltitude = tempRangeTable[rangeIndex].mDecreasingLine.back().mAltitude;
            if (intersection.mAltitude < lastAltitude)
            {
               tempRangeTable[rangeIndex].mDecreasingLine.push_back(intersection);
            }
            else if (intersection.mAltitude > (lastAltitude + 1.1))
            {
               ut::log::warning() << "Unexpected transition from decreasing to increasing altitudes in launch table.";
            }
         }
         else
         {
            double lastAltitude = tempRangeTable[rangeIndex].mIncreasingLine.back().mAltitude;
            if (intersection.mAltitude > lastAltitude)
            {
               tempRangeTable[rangeIndex].mIncreasingLine.push_back(intersection);
            }
            else if (intersection.mAltitude < (lastAltitude - 1.1))
            {
               // Starting the decreasing altitude section.
               const TempPoint& lastIntersection = tempRangeTable[rangeIndex].mIncreasingLine.back();
               tempRangeTable[rangeIndex].mDecreasingLine.push_back(lastIntersection);
               tempRangeTable[rangeIndex].mDecreasingLine.push_back(intersection);
            }
         }
      }
   }

   // The temporary tables have been created. Go back and construct the tables that will be used at run-time.
   // The run-time tables are optimized for speed.

   for (size_t gri = 0; gri < tempRangeTable.size(); ++gri)
   {
      TempRangeEntry& tempRangeEntry = tempRangeTable[gri];
      RangeEntry&     realRangeEntry = mRangeTable[gri];

      // If the 'increasing line' has only a single entry and the 'decreasing line' is NOT empty, then
      // there is no need for an 'increasing line' because the first entry in the 'decreasing line' is
      // a replication of the single entry.
      //
      // If the 'increasing line' has only a single entry but there are no entries in the 'decreasing line',
      // then the temporary version is left as-is, where it will then be converted to two points on output.
      //
      // Further note that the 'decreasing line' will never have exactly one entry because of the way it
      // is created.

      if ((tempRangeEntry.mIncreasingLine.size() == 1) && (tempRangeEntry.mDecreasingLine.size() > 1))
      {
         tempRangeEntry.mIncreasingLine.clear();
      }
      ConvertRangeLine(tempRangeEntry.mIncreasingLine, realRangeEntry.mIncreasingLine);
      ConvertRangeLine(tempRangeEntry.mDecreasingLine, realRangeEntry.mDecreasingLine);
   }

   if (aDebugFileLoading)
   {
      size_t totalCount = 0;
      auto   out        = ut::log::debug() << "Range Table:";
      for (size_t rti = 0; rti < mRangeTable.size(); ++rti)
      {
         auto note = out.AddNote() << "Range Entry " << rti << ":";
         note.AddNote() << "Range: " << RangeOfIndex(static_cast<int>(rti));
         { // RAII block
            auto inc = note.AddNote() << "Increasing line:";
            PrintRangeLine(mRangeTable[rti].mIncreasingLine, inc);
         }
         { // RAII block
            auto dec = note.AddNote() << "Decreasing line:";
            PrintRangeLine(mRangeTable[rti].mDecreasingLine, dec);
         }
         totalCount +=
            (mRangeTable[rti].mIncreasingLine.mAltitude.GetSize() + mRangeTable[rti].mDecreasingLine.mAltitude.GetSize());
      }
      out.AddNote() << "Total Point Count: " << totalCount;
   }

   if (aDebugEnabled || aDebugFileLoading)
   {
      auto out = ut::log::debug() << "Rounded Trajectory Limits:";
      out.AddNote() << "Min Ground Range: " << mMinRange;
      out.AddNote() << "Max Ground Range: " << mMaxRange;
      out.AddNote() << "Ground Range Count: " << mRangePointCount;
      out.AddNote() << "Min Altitude: " << mMinAltitude;
      out.AddNote() << "Max Altitude: " << mMaxAltitude;
   }
   return true;
}

// =================================================================================================
// Nested class 'SA_Evaluator'
// =================================================================================================

WsfBallisticMissileLaunchComputer::SA_Evaluator::SA_Evaluator(double                             aBaseTime,
                                                              WsfBallisticMissileLaunchComputer* aComputerPtr,
                                                              WsfMover::KinematicModel*          aTargetDataPtr)
   : mComputerPtr(aComputerPtr)
   , mTargetDataPtr(aTargetDataPtr)
   , mSA_Table(*(mComputerPtr->mSA_TablePtr))
   , mBaseTime(aBaseTime)
   , mCheckGeometry(false)
   , mBegTime(aBaseTime)
   , mBegRange(DBL_MAX)
   , mBegAltitude(0.0)
   , mBegRangeIndex(-1)
   , mEndTime(aBaseTime)
   , mEndRange(DBL_MAX)
   , mEndAltitude(0.0)
   , mEndRangeIndex(-1)
   , mLastRange(DBL_MAX)
   , mIterCount(0)
   , mClosing(false)
   , mAltitudeLU()
{
   if (mSA_Table.mHasVelocityData)
   {
      // Enable intercept angle checking if:
      // 1) The interceptor data tables include velocity data ... AND ...
      // 2a) A maximum allowable intercept angle or
      // 2b) A minimum intercept velocity has been defined.
      mCheckGeometry = (mComputerPtr->mMaxInterceptAngle > 0.0) || (mComputerPtr->mMinInterceptVelocity > 0.0);
   }

   // Get the location of the launch platform and convert to spherical Earth WCS.
   WsfWeapon* weaponPtr = mComputerPtr->GetWeapon();
   double     launchLocWCS[3];
   weaponPtr->GetLocationWCS(launchLocWCS);
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(launchLocWCS, mLaunchLocXYZ);

   UtVec3d::Set(mBegLocXYZ, 0.0);
   UtVec3d::Set(mEndLocXYZ, 0.0);
}

// =================================================================================================
void WsfBallisticMissileLaunchComputer::SA_Evaluator::FindLaunchWindowStart(double& aTimeToLaunch,
                                                                            double& aTimeToIntercept,
                                                                            double& aTimeOfFlight,
                                                                            double& aLoftAngle,
                                                                            double& aInterceptSpeed,
                                                                            double& aInterceptFlightPathAngle)
{
   aTimeToLaunch             = -1.0;
   aTimeToIntercept          = -1.0;
   aTimeOfFlight             = -1.0;
   aLoftAngle                = 0.0;
   aInterceptSpeed           = 0.0;
   aInterceptFlightPathAngle = 0.0;

   // Cache the intercept limits for performance

   double minRange        = mSA_Table.mMinRange;
   double maxRange        = mSA_Table.mMaxRange;
   double minAltitude     = mSA_Table.mMinAltitude;
   double maxAltitude     = mSA_Table.mMaxAltitude;
   double maxSlantRangeSq = mComputerPtr->mMaxInterceptSlantRange * mComputerPtr->mMaxInterceptSlantRange;

   // We simply continue from the last point
   mBegRangeIndex  = -1;
   double timeStep = mComputerPtr->mIntegrationTimeStep;
   while (mEndTime < mTargetDataPtr->GetTimeAtLocation())
   {
      mLastRange = mEndRange;
      mEndTime += timeStep;

      double tempEndLocXYZ[3];
      if (mTargetDataPtr->GetLocationXYZ(mEndTime, tempEndLocXYZ))
      {
         WsfMover::KinematicModel::GetRangeAndAltitude(mLaunchLocXYZ, tempEndLocXYZ, mEndRange, mEndAltitude);
         UtVec3d::Set(mEndLocXYZ, tempEndLocXYZ);
      }
      else
      {
         break;
      }

      // Keep track if target is closing or receding.
      bool previouslyClosing = mClosing;
      mClosing               = (mEndRange < mLastRange);
      ++mIterCount;

      // If not within range of the table then simply continue advancing, or if possible, exit.
      if (mEndRange > maxRange)
      {
         if (mEndRange > mLastRange)
         {
            // The target is receding from the launcher and is beyond maximum range.
            // No intercept is possible.
            return;
         }
         mBegRangeIndex = -1;
         continue;
      }
      if (mEndAltitude > maxAltitude)
      {
         // No special early-exit checks here because the target could come down.
         mBegRangeIndex = -1;
         continue;
      }
      if (mEndRange < minRange)
      {
         // No special checks here. Once it goes into the min range ring, it is theoretically
         // possible to come out the other side.
         mBegRangeIndex = -1;
         continue;
      }
      if (mEndAltitude < minAltitude)
      {
         if (mEndTime > mTargetDataPtr->GetApogeeTime())
         {
            // The target is past apogee and has descended below minimum intercept altitude.
            // No intercept is possible.
            return;
         }
         mBegRangeIndex = -1;
         continue;
      }

      // If 'maximum_intercept_slant_range' is specified, make sure the target is within the range.
      if (maxSlantRangeSq > 0.0)
      {
         double deltaLocXYZ[3];
         UtVec3d::Subtract(deltaLocXYZ, mEndLocXYZ, mLaunchLocXYZ);
         double slantRangeSq = UtVec3d::MagnitudeSquared(deltaLocXYZ);
         if (slantRangeSq > maxSlantRangeSq)
         {
            mBegRangeIndex = -1;
            continue;
         }
      }

      // If the target has changed from incoming and outgoing (or the other way around), mark the starting
      // point as 'not defined' (index=-1) to force the current point to be captured as the starting point.
      if ((mIterCount > 2) && (mClosing != previouslyClosing))
      {
         mBegRangeIndex = -1;
      }

      // If this is the first valid point found capture it as the starting point.
      mEndRangeIndex = mSA_Table.IndexOfRange(mEndRange);
      if (mBegRangeIndex < 0)
      {
         UtVec3d::Set(mBegLocXYZ, mEndLocXYZ);
         mBegTime       = mEndTime;
         mBegRange      = mEndRange;
         mBegAltitude   = mEndAltitude;
         mBegRangeIndex = mEndRangeIndex;
         continue;
      }
      else if (mEndRangeIndex == mBegRangeIndex)
      {
         continue;
      }

      // The beginning point is in one segment and the ending point is in another segment.
      // Check each crossing point to see if there is an intercept.

      bool   found                    = false;
      double timeOfFlight             = 0.0;
      double timeToLaunch             = 0.0;
      double timeToIntercept          = 0.0;
      double loftAngle                = 0.0;
      double interceptSpeed           = 0.0;
      double interceptFlightPathAngle = 0.0;
      if (mBegRangeIndex > mEndRangeIndex)
      {
         // Inbound target (the normal case)
         while ((mBegRangeIndex > mEndRangeIndex) && (!found))
         {
            found = CheckForInterceptOnRangeLine(mBegRangeIndex,
                                                 timeToLaunch,
                                                 timeToIntercept,
                                                 timeOfFlight,
                                                 loftAngle,
                                                 interceptSpeed,
                                                 interceptFlightPathAngle);
            --mBegRangeIndex;
         }
      }
      else if (mBegRangeIndex < mEndRangeIndex)
      {
         // Outbound target
         while ((mBegRangeIndex < mEndRangeIndex) && (!found))
         {
            found = CheckForInterceptOnRangeLine((mBegRangeIndex + 1),
                                                 timeToLaunch,
                                                 timeToIntercept,
                                                 timeOfFlight,
                                                 loftAngle,
                                                 interceptSpeed,
                                                 interceptFlightPathAngle);
            ++mBegRangeIndex;
         }
      }

      if (found)
      {
         aTimeToLaunch             = timeToLaunch;
         aTimeToIntercept          = timeToIntercept;
         aTimeOfFlight             = timeOfFlight;
         aLoftAngle                = loftAngle;
         aInterceptSpeed           = interceptSpeed;
         aInterceptFlightPathAngle = interceptFlightPathAngle;

         // Set up to resume search at next point
         mEndTime = mBaseTime + timeToIntercept;
         if (mTargetDataPtr->GetLocationXYZ(mEndTime, mEndLocXYZ))
         {
            WsfMover::KinematicModel::GetRangeAndAltitude(mLaunchLocXYZ, mEndLocXYZ, mEndRange, mEndAltitude);
            break;
         }
      }
      else
      {
         UtVec3d::Set(mBegLocXYZ, mEndLocXYZ);
         mBegTime       = mEndTime;
         mBegRange      = mEndRange;
         mBegAltitude   = mEndAltitude;
         mBegRangeIndex = mEndRangeIndex;
      }
   } // while (mEndTime < mTargetDataPtr->GetImpactTime ...
}

// =================================================================================================
//! Find the last possible intercept within the current launch window.
//! This method should only be called after a previously successful call to FindLaunchWindowStart.
//! It continues to iterate forward in time from the start of the launch window (as determined by
//! FindLaunchWindowStart) until it finds a point at which an intercept cannot occur. The values
//! returned represent the last time at which a successful intercept can occur.
void WsfBallisticMissileLaunchComputer::SA_Evaluator::FindLaunchWindowEnd(double& aTimeToLaunch,
                                                                          double& aTimeToIntercept,
                                                                          double& aTimeOfFlight,
                                                                          double& aLoftAngle,
                                                                          double& aInterceptSpeed,
                                                                          double& aInterceptFlightPathAngle)
{
   aTimeToLaunch             = -1.0;
   aTimeToIntercept          = -1.0;
   aTimeOfFlight             = 0.0;
   aLoftAngle                = 0.0;
   aInterceptSpeed           = 0.0;
   aInterceptFlightPathAngle = 0.0;

   // Continue looking forward from the last solution to find where a solution no longer exists

   double timeToLaunch;
   double timeToIntercept;
   double timeOfFlight;
   double loftAngle;
   double interceptSpeed;
   double interceptFlightPathAngle;

   // Cache the intercept limits for performance

   double minRange        = mSA_Table.mMinRange;
   double maxRange        = mSA_Table.mMaxRange;
   double minAltitude     = mSA_Table.mMinAltitude;
   double maxAltitude     = mSA_Table.mMaxAltitude;
   double maxSlantRangeSq = mComputerPtr->mMaxInterceptSlantRange * mComputerPtr->mMaxInterceptSlantRange;

   // Continue searching from last solution.
   mBegRangeIndex  = -1;
   double timestep = mComputerPtr->mIntegrationTimeStep;
   while (mEndTime < mTargetDataPtr->GetTimeAtLocation())
   {
      double tempEndTime = mEndTime + timestep;
      double tempEndLocXYZ[3];

      if (mTargetDataPtr->GetLocationXYZ(tempEndTime, tempEndLocXYZ))
      {
         UtVec3d::Set(mEndLocXYZ, tempEndLocXYZ);
         WsfMover::KinematicModel::GetRangeAndAltitude(mLaunchLocXYZ, tempEndLocXYZ, mEndRange, mEndAltitude);
         mLastRange = mEndRange;
      }
      else
      {
         return;
      }

      mEndTime += timestep;

      bool previouslyClosing = mClosing;
      mClosing               = (mEndRange <= mLastRange);
      ++mIterCount;

      // If not within range of the table then we've found the end of this window.
      if ((mEndRange < minRange) || (mEndRange > maxRange) || (mEndAltitude < minAltitude) || (mEndAltitude > maxAltitude))
      {
         break;
      }

      // If 'maximum_intercept_slant_range' is specified, make sure the target is within the range.
      if (maxSlantRangeSq > 0.0)
      {
         double deltaLocXYZ[3];
         UtVec3d::Subtract(deltaLocXYZ, mEndLocXYZ, mLaunchLocXYZ);
         double slantRangeSq = UtVec3d::MagnitudeSquared(deltaLocXYZ);
         if (slantRangeSq > maxSlantRangeSq)
         {
            break;
         }
      }

      // If the target has changed from incoming and outgoing (or the other way around), mark the starting
      // point as 'not defined' (index=-1) to force the current point to be captured as the starting point.
      if ((mIterCount > 2) && (mClosing != previouslyClosing))
      {
         mBegRangeIndex = -1;
      }

      // If this is the first valid point found capture it as the starting point.
      mEndRangeIndex = mSA_Table.IndexOfRange(mEndRange);
      if (mBegRangeIndex < 0)
      {
         UtVec3d::Set(mBegLocXYZ, mEndLocXYZ);
         mBegTime       = mEndTime;
         mBegRange      = mEndRange;
         mBegAltitude   = mEndAltitude;
         mBegRangeIndex = mEndRangeIndex;
         continue;
      }
      else if (mEndRangeIndex == mBegRangeIndex)
      {
         continue;
      }

      // The beginning point is in one segment and the ending point is in another segment.
      // Check each crossing point to see if there is an intercept.

      bool found = true;
      if (mBegRangeIndex > mEndRangeIndex)
      {
         // Inbound target (the normal case)
         while ((mBegRangeIndex > mEndRangeIndex) && (found))
         {
            found = CheckForInterceptOnRangeLine(mBegRangeIndex,
                                                 timeToLaunch,
                                                 timeToIntercept,
                                                 timeOfFlight,
                                                 loftAngle,
                                                 interceptSpeed,
                                                 interceptFlightPathAngle);
            if (found)
            {
               // Still have a solution - retain this as the last good solution.
               aTimeToLaunch             = timeToLaunch;
               aTimeToIntercept          = timeToIntercept;
               aTimeOfFlight             = timeOfFlight;
               aLoftAngle                = loftAngle;
               aInterceptSpeed           = interceptSpeed;
               aInterceptFlightPathAngle = interceptFlightPathAngle;
            }
            --mBegRangeIndex;
         }
      }
      else if (mBegRangeIndex < mEndRangeIndex)
      {
         // Outbound target
         while ((mBegRangeIndex < mEndRangeIndex) && (found))
         {
            found = CheckForInterceptOnRangeLine((mBegRangeIndex + 1),
                                                 timeToLaunch,
                                                 timeToIntercept,
                                                 timeOfFlight,
                                                 loftAngle,
                                                 interceptSpeed,
                                                 interceptFlightPathAngle);
            if (found)
            {
               // Still have a solution - retain this as the last good solution.
               aTimeToLaunch             = timeToLaunch;
               aTimeToIntercept          = timeToIntercept;
               aTimeOfFlight             = timeOfFlight;
               aLoftAngle                = loftAngle;
               aInterceptSpeed           = interceptSpeed;
               aInterceptFlightPathAngle = interceptFlightPathAngle;
            }
            ++mBegRangeIndex;
         }
      }

      UtVec3d::Set(mBegLocXYZ, mEndLocXYZ);
      mBegTime       = mEndTime;
      mBegRange      = mEndRange;
      mBegAltitude   = mEndAltitude;
      mBegRangeIndex = mEndRangeIndex;

      if (!found)
      {
         break;
      }
   } // while (mEndTime < mTargetDataPtr->GetImpactTime ...
}

// =================================================================================================
//! Check for an intercept on a line of constant of constant range.
//! returns true if an intercept is possible or false if not.
bool WsfBallisticMissileLaunchComputer::SA_Evaluator::CheckForInterceptOnRangeLine(int     aRangeIndex,
                                                                                   double& aTimeToLaunch,
                                                                                   double& aTimeToIntercept,
                                                                                   double& aTimeOfFlight,
                                                                                   double& aLoftAngle,
                                                                                   double& aInterceptorSpeed,
                                                                                   double& aInterceptorFlightPathAngle)
{
   double range         = mSA_Table.RangeOfIndex(aRangeIndex);
   double frac          = (range - mBegRange) / (mEndRange - mBegRange);
   double altitude      = mBegAltitude + frac * (mEndAltitude - mBegAltitude);
   double interceptTime = mBegTime + frac * (mEndTime - mBegTime);
   aTimeToIntercept     = interceptTime - mBaseTime;

   // Determine if the altitude is within the intercept limits.
   //
   // The tables contain raw kinematic data that may include points that are outside the defined
   // intercept limits. The points that outside are not eliminated because they may be needed to
   // get an interpolated value for an altitude that is within the table.
   if ((altitude < mComputerPtr->mMinInterceptAltitude) || (altitude > mComputerPtr->mMaxInterceptAltitude))
   {
      return false;
   }

   // Retrieve interceptor data for this possible intercept range and altitude:
   //
   // 1) Time of flight to the point.
   // 2) Loft angle for guidance.
   // 3) Speed at the point.
   // 4) Flight path angle at the point.
   //
   // The later two will be retrieved only if performing intercept geometry checks.

   const RangeEntry& rangeEntry = mSA_Table.mRangeTable[aRangeIndex];

   aTimeOfFlight               = -1.0;
   aLoftAngle                  = 0.0;
   aInterceptorSpeed           = 0.0;
   aInterceptorFlightPathAngle = 0.0;

   // Try the increasing line first.

   const RangeLine& rangeLine1 = rangeEntry.mIncreasingLine;
   if ((altitude >= rangeLine1.mMinAltitude) && (altitude <= rangeLine1.mMaxAltitude))
   {
      mAltitudeLU.Reset();
      mAltitudeLU.Lookup(rangeLine1.mAltitude, altitude);
      aTimeOfFlight               = TblEvaluate(rangeLine1.mTimeOfFlight, mAltitudeLU);
      aLoftAngle                  = TblEvaluate(rangeLine1.mLoftAngle, mAltitudeLU);
      aInterceptorSpeed           = TblEvaluate(rangeLine1.mSpeed, mAltitudeLU);
      aInterceptorFlightPathAngle = TblEvaluate(rangeLine1.mFlightPathAngle, mAltitudeLU);
   }

   // Try the decreasing line last.

   const RangeLine& rangeLine2 = rangeEntry.mDecreasingLine;
   if (aTimeOfFlight < 0.0)
   {
      if ((altitude >= rangeLine2.mMinAltitude) && (altitude <= rangeLine2.mMaxAltitude))
      {
         mAltitudeLU.Reset();
         mAltitudeLU.Lookup(rangeLine2.mAltitude, altitude);
         aTimeOfFlight               = TblEvaluate(rangeLine2.mTimeOfFlight, mAltitudeLU);
         aLoftAngle                  = TblEvaluate(rangeLine2.mLoftAngle, mAltitudeLU);
         aInterceptorSpeed           = TblEvaluate(rangeLine2.mSpeed, mAltitudeLU);
         aInterceptorFlightPathAngle = TblEvaluate(rangeLine2.mFlightPathAngle, mAltitudeLU);
      }
   }

   // Simply return if the interceptor cannot achieve this range and altitude.
   if (aTimeOfFlight < 0.0)
   {
      return false;
   }

   // Compute the time to launch. As long as it is in the future then we have a possible solution.
   // A little slop is allowed here because we don't want to miss a solution that is very close.

   double launchTime = interceptTime - aTimeOfFlight;
   aTimeToLaunch     = launchTime - mBaseTime;
   bool canIntercept = (aTimeToLaunch > (-cPAST_SOLUTION_TOLERANCE));

   // If an intercept is possible, determine if the conditions at launch are appropriate
   if (canIntercept && (mComputerPtr->mMaxLaunchSlantRange > 0.0))
   {
      double tgtLocXYZ[3]; // Location of target at time of launch
      if (mTargetDataPtr->GetLocationXYZ(launchTime, tgtLocXYZ))
      {
         double deltaLocXYZ[3];
         UtVec3d::Subtract(deltaLocXYZ, tgtLocXYZ, mLaunchLocXYZ);
         double slantRangeSq          = UtVec3d::MagnitudeSquared(deltaLocXYZ);
         double maxLaunchSlantRangeSq = mComputerPtr->mMaxLaunchSlantRange * mComputerPtr->mMaxLaunchSlantRange;
         canIntercept                 = (slantRangeSq <= maxLaunchSlantRangeSq);
      }
   }

   // If an intercept is possible, determine if the intercept geometry is acceptable.

   if (canIntercept && mCheckGeometry)
   {
      // Get the position and velocity vectors of the target at the time of intercept.
      double tgtLocXYZ[3];
      double tgtVelXYZ[3];

      mTargetDataPtr->GetLocationXYZ(interceptTime, tgtLocXYZ);
      mTargetDataPtr->GetVelocityXYZ(interceptTime, tgtVelXYZ);

      // Assume the interceptor flies in a straight path to the intercept point.
      // Get the unit vector that points from the launcher to the intercept pointer.

      double lauToIntLocXYZ[3];
      UtVec3d::Subtract(lauToIntLocXYZ, tgtLocXYZ, mLaunchLocXYZ);
      double tgtLocWCS[3];
      UtSphericalEarth::ConvertSphericalToEllipsoidal(tgtLocXYZ, tgtLocWCS);
      UtVec3d::Normalize(lauToIntLocXYZ);

      // Transform the launcher->intercept unit vector from the WCS frame to a local NED frame
      // whose origin is at the intercept point.

      double lat;
      double lon;
      double alt;
      UtSphericalEarth::ConvertECEFToLLA(tgtLocWCS, lat, lon, alt);
      double WCS_ToNED_Transform[3][3];
      double intLocWCS[3];
      UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, intLocWCS);

      double vecNED[3];
      UtMat3d::Transform(vecNED, WCS_ToNED_Transform, lauToIntLocXYZ);

      // Form the local NED unit velocity vector of the weapon using the horizontal components of
      // of launcher->intercept unit vector and the flight path angle from the interceptor tables.

      double heading          = atan2(vecNED[1], vecNED[0]);
      double vecNE            = cos(aInterceptorFlightPathAngle);
      double vecD             = -sin(aInterceptorFlightPathAngle);
      double wpnUnitVelNED[3] = {vecNE * cos(heading), vecNE * sin(heading), vecD};

      // Transform the NED vector back to the WCS frame.

      double wpnUnitVelWCS[3];
      UtMat3d::InverseTransform(wpnUnitVelWCS, WCS_ToNED_Transform, wpnUnitVelNED);

      // Using the definition of the dot product, get the cosine of the angle between the velocity vectors.
      // (Note that the weapon vector is already a unit vector).
      //
      // The negative sign accounts for the fact that we want 0 degrees to be head-on and 180 degrees
      // to be tail-chase.

      double tgtSpeed     = UtVec3d::Magnitude(tgtVelXYZ);
      double cosTheta     = UtVec3d::DotProduct(tgtVelXYZ, wpnUnitVelWCS) / tgtSpeed;
      double interceptVel = aInterceptorSpeed - cosTheta * tgtSpeed;

      // Check for two constraints:
      // A.  The intercept angle is smaller than the constraint.
      canIntercept = fabs(cos(mComputerPtr->mMaxInterceptAngle)) <= fabs(cosTheta);

      // B.  The relative velocity is greater than the specified minimum
      if (canIntercept)
      {
         canIntercept = interceptVel > mComputerPtr->mMinInterceptVelocity;
      }

      if (mComputerPtr->DebugEnabled())
      {
         auto out = ut::log::debug() << "BallisticMissleLaunchComputer::CheckForInterceptOnRangeLine():";
         out.AddNote() << "Impact angle: " << acos(cosTheta) * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Limit: " << mComputerPtr->mMaxInterceptAngle * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "CanIntercept: " << std::boolalpha << canIntercept;

         double pitch;
         double tgtVelNED[3];
         UtMat3d::Transform(tgtVelNED, WCS_ToNED_Transform, tgtVelXYZ);

         heading = atan2(tgtVelNED[1], tgtVelNED[0]);
         heading = UtMath::NormalizeAngle0_TwoPi(heading);
         pitch   = atan2(-tgtVelNED[2], sqrt(tgtVelNED[0] * tgtVelNED[0] + tgtVelNED[1] * tgtVelNED[1]));

         out.AddNote() << "Target Heading: " << heading * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Speed: " << UtVec3d::Magnitude(tgtVelXYZ);

         heading = atan2(wpnUnitVelNED[1], wpnUnitVelNED[0]);
         heading = UtMath::NormalizeAngle0_TwoPi(heading);
         pitch = atan2(-wpnUnitVelNED[2], sqrt(wpnUnitVelNED[0] * wpnUnitVelNED[0] + wpnUnitVelNED[1] * wpnUnitVelNED[1]));
         out.AddNote() << "Weapon Heading: " << heading * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Speed: " << aInterceptorSpeed;
         out.AddNote() << "Rel Impact Velocity: " << interceptVel << " m/s";
         out.AddNote() << "Constraint: " << mComputerPtr->mMinInterceptVelocity << " m/s";
      }
   }

   return canIntercept;
}

// =================================================================================================
// Nested class 'BallisticModel'
// =================================================================================================

// =================================================================================================
WsfBallisticMissileLaunchComputer::BallisticModel::BallisticModel(WsfBallisticMissileLaunchComputer* aParentPtr,
                                                                  double                             aSimTime,
                                                                  WsfStringId                        aTypeId,
                                                                  const double                       aLocationWCS[3],
                                                                  const double                       aVelocityWCS[3],
                                                                  double   aIntegrationInterval,
                                                                  WsfDraw* aDrawPtr) //= 0
   : WsfMover::KinematicModel(aSimTime, aTypeId)
   , mStatePtr(nullptr)
   , mMass(0.0)
   , mAeroPtr(nullptr)
   , mIntegrationInterval(aIntegrationInterval)
   , mPoints()
{
   auto& scenario = aParentPtr->GetPlatform()->GetScenario();
   mStatePtr      = new WsfPlatform(scenario);

   mStatePtr->SetLocationWCS(aLocationWCS);
   mStatePtr->SetVelocityWCS(aVelocityWCS);

   // Integration is internally done in spherical Earth coordinates to be consistent with WsfGuidedMover.

   double lat;
   double lon;
   double alt;
   mStatePtr->GetLocationLLA(lat, lon, alt);
   double locWCS[3];
   UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, mIntegWCS_ToNED_Transform, locWCS);

   // The velocity vector must first be converted into an NED frame and then converted to the spherical frame.
   double velNED[3];
   mStatePtr->GetVelocityNED(velNED);

   // Rotate the vector in the NED to the WCS frame
   double velWCS[3];
   UtMat3d::InverseTransform(velWCS, mIntegWCS_ToNED_Transform, velNED);

   // Set the initial integrated location and velocity to these values
   UtVec3d::Set(mIntegLocWCS, locWCS);
   UtVec3d::Set(mIntegVelWCS, velWCS);

   auto iter = aParentPtr->mSharedDataPtr->mTargetAeroMap.find(mModelTypeId);
   if (iter != aParentPtr->mSharedDataPtr->mTargetAeroMap.end())
   {
      WsfAero* prototypePtr = dynamic_cast<WsfAero*>(scenario.FindType("aero", iter->second.GetString()));
      if (prototypePtr != nullptr)
      {
         mAeroPtr = prototypePtr->Clone();
         mAeroPtr->Initialize(aSimTime, mStatePtr);
         mMass = aParentPtr->mSharedDataPtr->mTargetMassMap[mModelTypeId];
      }
      else
      {
         auto out =
            ut::log::info()
            << "WsfBallisticMissileLaunchComputer::BallisticTarget: Could not find aero object of specified type.";
         out.AddNote() << "Type: " << mModelTypeId;
      }
   }

   // Compute the entire trajectory.
   //
   // The trajectory is first stored in a vector whose capacity has been set to be large (about 1800 seconds
   // of flight). The result is copied to a permanent vector to eliminate any unused entries. This should be
   // faster because it eliminated reallocations.

   std::vector<Point> points;
   size_t             initialCapacity = static_cast<size_t>(1800.0 / mIntegrationInterval);
   points.reserve(initialCapacity);

   double locXYZ[3];
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(locWCS, locXYZ);
   double altitude       = UtVec3d::Magnitude(locXYZ) - UtSphericalEarth::cEARTH_RADIUS;
   double apogeeAltitude = altitude;
   mApogeeTime           = aSimTime;
   mTimeAtLocation       = aSimTime;

   Point point;
   UtVec3d::Set(point.mLocXYZ, mIntegLocWCS);
   UtVec3d::Set(point.mVelXYZ, mIntegVelWCS);
   points.push_back(point);

   while (altitude > 0.0)
   {
      mTimeAtLocation += mIntegrationInterval;
      altitude = Update();
      UtVec3d::Set(point.mLocXYZ, mIntegLocWCS);
      UtVec3d::Set(point.mVelXYZ, mIntegVelWCS);
      points.push_back(point);
      if (altitude > apogeeAltitude)
      {
         apogeeAltitude = altitude;
         mApogeeTime    = mTimeAtLocation;
      }

      // The target may be an space object that never hits the surface OR it may be the weapon
      // and it may have enough energy to go into orbit ('Target' data will created for the weapon
      // if compute_end_point is true. It propagates the weapon from the predicted intercept point
      // to where it would hit the surface).
      //
      // If this is the case then the trajectory computation will be truncated in order to avoid
      // an infinite loop that fills memory.

      if (mTimeAtLocation > (mStartTime + 3600.0))
      {
         break;
      }
   }
   mPoints = points; // Copy to permanent vector, eliminating unused space

   // Set 'mImpactTime' to the approximate time when the target passes through the surface.
   // (The integration goes one step past).
   //
   // Note that the integration loop may abbreviate the trajectory if it doesn't hit the surface
   // within a reasonable period of time. (The target may be in space or the weapon itself may
   // escape.

   size_t n = mPoints.size();
   if (n >= 2)
   {
      double alt1 = UtVec3d::Magnitude(mPoints[n - 2].mLocXYZ) - UtSphericalEarth::cEARTH_RADIUS;
      double alt2 = UtVec3d::Magnitude(mPoints[n - 1].mLocXYZ) - UtSphericalEarth::cEARTH_RADIUS;
      if (alt2 <= 0.0)
      {
         double frac     = (0.0 - alt1) / (alt2 - alt1);
         double t2       = mTimeAtLocation;
         double t1       = mTimeAtLocation - mIntegrationInterval;
         mTimeAtLocation = t1 + frac * (t2 - t1);
      }
   }

   // Draw the trajectory if requested.
   if (aDrawPtr != nullptr)
   {
      double sampler = 5.0;
      aDrawPtr->BeginPolyline();
      for (const auto& point : mPoints)
      {
         sampler += mIntegrationInterval;
         if (sampler >= 5.0)
         {
            double tempLocWCS[3];
            UtSphericalEarth::ConvertSphericalToEllipsoidal(point.mLocXYZ, tempLocWCS);
            aDrawPtr->VertexWCS(tempLocWCS);
            sampler = 0.0;
         }
      }
      aDrawPtr->End();
   }
}

// =================================================================================================
WsfBallisticMissileLaunchComputer::BallisticModel::~BallisticModel()
{
   delete mAeroPtr;
}

// =================================================================================================
bool WsfBallisticMissileLaunchComputer::BallisticModel::GetLocationXYZ(double aTime, double aLocXYZ[3]) const
{
   bool ok        = true;
   int  index     = static_cast<int>((aTime - mStartTime) / mIntegrationInterval);
   int  lastIndex = static_cast<int>(mPoints.size() - 1);
   if (index < 0)
   {
      // Requested time is before the first point.
      ok = false;
      UtVec3d::Set(aLocXYZ, mPoints[0].mLocXYZ);
   }
   else if (index >= lastIndex)
   {
      // Requested time is at or after the last point.
      ok = (index == lastIndex);
      UtVec3d::Set(aLocXYZ, mPoints[lastIndex].mLocXYZ);
   }
   else
   {
      // Requested time is within a valid interval.
      double frac = ((aTime - mStartTime) - (index * mIntegrationInterval)) / mIntegrationInterval;
      double diffLocXYZ[3];
      UtVec3d::Subtract(diffLocXYZ, mPoints[index + 1].mLocXYZ, mPoints[index].mLocXYZ);
      UtVec3d::AddProduct(aLocXYZ, mPoints[index].mLocXYZ, frac, diffLocXYZ);
   }
   return ok;
}

// =================================================================================================
bool WsfBallisticMissileLaunchComputer::BallisticModel::GetVelocityXYZ(double aTime, double aVelXYZ[3]) const
{
   bool ok        = true;
   int  index     = static_cast<int>((aTime - mStartTime) / mIntegrationInterval);
   int  lastIndex = static_cast<int>(mPoints.size() - 1);
   if (index < 0)
   {
      // Requested time is before the first point
      ok = false;
      UtVec3d::Set(aVelXYZ, mPoints[0].mVelXYZ);
   }
   else if (index >= lastIndex)
   {
      // Requested time is at or after the last point.
      ok = (index == lastIndex);
      UtVec3d::Set(aVelXYZ, mPoints[lastIndex].mVelXYZ);
   }
   else
   {
      // Requested time is within a valid interval.
      double frac = ((aTime - mStartTime) - (index * mIntegrationInterval)) / mIntegrationInterval;
      double diffVelXYZ[3];
      UtVec3d::Subtract(diffVelXYZ, mPoints[index + 1].mVelXYZ, mPoints[index].mVelXYZ);
      UtVec3d::AddProduct(aVelXYZ, mPoints[index].mVelXYZ, frac, diffVelXYZ);
   }
   return ok;
}

// =================================================================================================
// protected
double WsfBallisticMissileLaunchComputer::BallisticModel::Update()
{
   // Compute the drag force, if available
   double drag = 0.0;
   if (mAeroPtr != nullptr)
   {
      mAeroPtr->Update(0.0);
      drag = mAeroPtr->GetDrag();
   }

   // Use two-stage Runge-Kutta integration.

   double locWCS_1[3];
   double locWCS_2[3];
   double velWCS_1[3];
   double velWCS_2[3];
   double aclWCS_1[3];
   double aclWCS_2[3];
   double dragWCS[3];
   double r;
   double g;

   // Use derivatives at t to get position and velocity at t + dt/2.

   UtVec3d::Set(locWCS_1, mIntegLocWCS);
   UtVec3d::Set(velWCS_1, mIntegVelWCS);

   // Compute the derivatives at t.

   UtVec3d::Set(aclWCS_1, locWCS_1);
   r = UtVec3d::Magnitude(aclWCS_1);
   g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   UtVec3d::Multiply(aclWCS_1, -g / r);
   if (drag > 0.0)
   {
      // Drag is opposite the velocity vector
      UtVec3d::Set(dragWCS, velWCS_1);
      double speed = UtVec3d::Magnitude(velWCS_1);
      UtVec3d::Multiply(dragWCS, -drag / speed / mMass);
      UtVec3d::Add(aclWCS_1, aclWCS_1, dragWCS);
   }

   UtVec3d::AddProduct(locWCS_2, locWCS_1, 0.5 * mIntegrationInterval, velWCS_1);
   UtVec3d::AddProduct(velWCS_2, velWCS_1, 0.5 * mIntegrationInterval, aclWCS_1);

   // Compute derivatives at t + dt/2

   UtVec3d::Set(aclWCS_2, locWCS_2);
   r = UtVec3d::Magnitude(aclWCS_2);
   g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   UtVec3d::Multiply(aclWCS_2, -g / r);
   if (drag > 0.0)
   {
      // Drag is opposite the velocity vector
      UtVec3d::Set(dragWCS, velWCS_2);
      double speed = UtVec3d::Magnitude(velWCS_2);
      UtVec3d::Multiply(dragWCS, -drag / speed / mMass);
      UtVec3d::Add(aclWCS_2, aclWCS_2, dragWCS);
   }

   // Use derivatives at t + dt/2 to get position and velocity at t + dt.

   UtVec3d::AddProduct(mIntegLocWCS, locWCS_1, mIntegrationInterval, velWCS_2);
   UtVec3d::AddProduct(mIntegVelWCS, velWCS_1, mIntegrationInterval, aclWCS_2);

   // The ellipsoidal earth location and velocity is needed for aero computations.

   double lat;
   double lon;
   double alt;
   UtSphericalEarth::ConvertECEFToLLA(mIntegLocWCS, lat, lon, alt);
   double locWCS[3];
   UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, mIntegWCS_ToNED_Transform, locWCS);
   double velNED[3];
   UtSphericalEarth::ConvertECEFToLocal(mIntegWCS_ToNED_Transform, mIntegVelWCS, velNED);
   mStatePtr->SetLocationLLA(lat, lon, alt);
   mStatePtr->SetVelocityNED(velNED);

   return alt;
}

// ================== WsfBallisticMissileLaunchComputer::PurgeSolutionEvent ============================

WsfBallisticMissileLaunchComputer::PurgeSolutionEvent::PurgeSolutionEvent(double                             aSimTime,
                                                                          WsfBallisticMissileLaunchComputer* aLC_Ptr,
                                                                          size_t             aTargetIndex,
                                                                          const std::string& aCaller)
   : WsfEvent(aSimTime)
   , mPlatformIndex(aLC_Ptr->GetPlatform()->GetIndex())
   , mLC_Ptr(aLC_Ptr)
   , mTargetIndex(aTargetIndex)
   , mCaller(aCaller)
{
}

WsfEvent::EventDisposition WsfBallisticMissileLaunchComputer::PurgeSolutionEvent::Execute()
{
   if (GetSimulation()->PlatformExists(mPlatformIndex))
   {
      mLC_Ptr->QueuedSolutionPurge(GetTime(), mTargetIndex, mCaller);
   }
   return WsfEvent::cDELETE;
}

void WsfBallisticMissileLaunchComputerSharedData::GlobalPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Delete trajectory data.
   auto tmi = mTargetDataMap.find(aPlatformPtr->GetIndex());
   if (tmi != mTargetDataMap.end())
   {
      delete ((*tmi).second);
      mTargetDataMap.erase(tmi);
   }
}

WsfBallisticMissileLaunchComputerSharedData::~WsfBallisticMissileLaunchComputerSharedData()
{
   for (auto& tmi : mTargetDataMap)
   {
      delete tmi.second;
   }
}

WsfBallisticMissileLaunchComputerSharedData::WsfBallisticMissileLaunchComputerSharedData()
   : WsfSimulationExtension()
{
}

bool WsfBallisticMissileLaunchComputerSharedData::Initialize()
{
   mCallbacks.Add(WsfObserver::PlatformDeleted(&GetSimulation())
                     .Connect(&WsfBallisticMissileLaunchComputerSharedData::GlobalPlatformDeleted, this));
   return true;
}
