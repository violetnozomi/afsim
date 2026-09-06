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

#include "WsfFalseTarget.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtStringUtil.hpp"
#include "UtVec2.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_EffectManager.hpp"
#include "WsfEW_Result.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"

// static
std::map<WsfStringInt, size_t> WsfFalseTarget::sFT_NameCounter;
// Leaving the bottom half for the simulation unique id stuff when used in the sensor trackers.
size_t               WsfFalseTarget::sFT_UniqueId = std::numeric_limits<size_t>::max() / 2;
std::recursive_mutex WsfFalseTarget::sFT_Mutex;

// static
void WsfFalseTarget::ResetStatic()
{
   std::lock_guard<std::recursive_mutex> lock(sFT_Mutex);
   sFT_UniqueId = sizeof(size_t) / 2;
   sFT_NameCounter.clear();
}

// static
size_t WsfFalseTarget::AssignFT_BlipUniqueId()
{
   std::lock_guard<std::recursive_mutex> lock(sFT_Mutex);
   if (sFT_UniqueId == std::numeric_limits<size_t>::max())
   {
      // Leaving the bottom half for the simulation unique id stuff when used in the sensor trackers.
      sFT_UniqueId = std::numeric_limits<size_t>::max() / 2;
   }
   return (++sFT_UniqueId);
}

// =================================================================================================
WsfFalseTarget::WsfFalseTarget(WsfScenario& aScenario)
   : WsfObject()
   , mDebug(false)
   , mSimulationPtr(nullptr)
   , mBlipBehavior(cRANDOM_SCAN_TO_SCAN)
   , mRadarBehavior(cRANDOM_TO_MULTIPLE_RADARS)
   , mPlatformBehavior(cRANDOM_TO_MULTIPLE_PLATFORMS)
   , mInitialNumberOfFTs(0)
   , mScanInterval(20.0)
   , mPersistence(1.0)
   , mAzimuthExtent()
   , mRangeExtent()
   , mRangeConstrained(false)
   , mBearingReference(cNORTH)
   , mRangeDistribution(cUNIFORM)
   , mCentroidHeading(0.0)
   , mCentroidSpeed(0.0)
   , mLastCentroidUpdate(-DBL_MAX)
   , mCentroidNeedsAGL_Adjustment(false)
   , mExplicitReferenceCentroid(false)
   , mExplicitNumberOfFTs(false)
   , mExplicitScanInterval(false)
   , mExplicitAzimuthExtent(false)
   , mExplicitRangeConstraint(false)
   , mExplicitRangeExtent(false)
   , mExplicitBlipSpeeds(false)
   , mBlipReference(cBR_NONE)
   , mInitialized(false)
   , mInitializationTime(-DBL_MAX)
   , mChanged(false)
   , mBlipsMap()
   , mBaseLocations()
   , mFusedPlatformId(nullptr)
   , mJammerIdToStateMap()
   , mLastBaseLocsUpdate(-DBL_MAX)
   , mLastScan(-DBL_MAX)
   , mLastUpdate(-DBL_MAX)
   , mNewBlipsStateVec()
   , mState(cOn)
   , mToggled(false)
   , mFTNameExtensionId(0)
   , mBlipIdCounter(0)
   , mRandom()
   , mMutex()
{
   UtVec2d::Set(mBlipHeadings, 0.0);
   UtVec2d::Set(mBlipSpeeds, 0.0);
   UtVec3d::Set(mLastBaseLocWCS, 0.0);
   mCentroidPlatformPtr = new WsfPlatform(aScenario);
}

// =================================================================================================
WsfFalseTarget::WsfFalseTarget(const WsfFalseTarget& aSrc)
   : WsfObject(aSrc)
   , mDebug(aSrc.mDebug)
   , mSimulationPtr(aSrc.mSimulationPtr)
   , mBlipBehavior(aSrc.mBlipBehavior)
   , mRadarBehavior(aSrc.mRadarBehavior)
   , mPlatformBehavior(aSrc.mPlatformBehavior)
   , mInitialNumberOfFTs(aSrc.mInitialNumberOfFTs)
   , mScanInterval(aSrc.mScanInterval)
   , mPersistence(aSrc.mPersistence)
   , mAzimuthExtent(aSrc.mAzimuthExtent)
   , mRangeExtent(aSrc.mRangeExtent)
   , mRangeConstrained(aSrc.mRangeConstrained)
   , mBearingReference(aSrc.mBearingReference)
   , mRangeDistribution(aSrc.mRangeDistribution)
   , mCentroidHeading(aSrc.mCentroidHeading)
   , mCentroidSpeed(aSrc.mCentroidSpeed)
   , mLastCentroidUpdate(aSrc.mLastCentroidUpdate)
   , mCentroidNeedsAGL_Adjustment(aSrc.mCentroidNeedsAGL_Adjustment)
   , mExplicitReferenceCentroid(aSrc.mExplicitReferenceCentroid)
   , mExplicitNumberOfFTs(aSrc.mExplicitNumberOfFTs)
   , mExplicitScanInterval(aSrc.mExplicitScanInterval)
   , mExplicitAzimuthExtent(aSrc.mExplicitAzimuthExtent)
   , mExplicitRangeConstraint(aSrc.mExplicitRangeConstraint)
   , mExplicitRangeExtent(aSrc.mExplicitRangeExtent)
   , mExplicitBlipSpeeds(aSrc.mExplicitBlipSpeeds)
   , mBlipReference(aSrc.mBlipReference)
   , mInitialized(false)
   , mInitializationTime(aSrc.mInitializationTime)
   , mChanged(aSrc.mChanged)
   , mBlipsMap(aSrc.mBlipsMap)
   , mBaseLocations(aSrc.mBaseLocations)
   , mFusedPlatformId(aSrc.mFusedPlatformId)
   , mJammerIdToStateMap(aSrc.mJammerIdToStateMap)
   , mLastBaseLocsUpdate(aSrc.mLastBaseLocsUpdate)
   , mLastScan(aSrc.mLastScan)
   , mLastUpdate(aSrc.mLastUpdate)
   , mNewBlipsStateVec(aSrc.mNewBlipsStateVec)
   , mState(aSrc.mState)
   , mToggled(aSrc.mToggled)
   , mFTNameExtensionId(0)
   , mBlipIdCounter(0)
   , mRandom()
   , mMutex()
{
   UtVec2d::Set(mBlipHeadings, aSrc.mBlipHeadings);
   UtVec2d::Set(mBlipSpeeds, aSrc.mBlipSpeeds);
   UtVec3d::Set(mLastBaseLocWCS, aSrc.mLastBaseLocWCS);
   mCentroidPlatformPtr = aSrc.mCentroidPlatformPtr->Clone();

   std::lock_guard<std::recursive_mutex> lock(sFT_Mutex);
   mFTNameExtensionId = ++sFT_NameCounter[GetNameId()];
}

// =================================================================================================
// virtual
WsfFalseTarget::~WsfFalseTarget()
{
   if (!mBlipsMap.empty())
   {
      for (auto& bmi : mBlipsMap)
      {
         for (auto& bi : bmi.second)
         {
            delete bi;
         }
      }
   }
   delete mCentroidPlatformPtr;
}

// =================================================================================================
// virtual
WsfFalseTarget* WsfFalseTarget::Clone() const
{
   return new WsfFalseTarget(*this);
}

// =================================================================================================
// virtual
bool WsfFalseTarget::Initialize(WsfSimulation* aSimulationPtr)
{
   bool initialized = true;

   // Note: Initialize() is called on false targets added to the type list BEFORE the simulation is
   //        created -- therefore aSimulationPtr is null.
   //       When the false target instance is added to the sim, Initialize() is called with
   //       a valid simulation object
   if (aSimulationPtr != nullptr)
   {
      mSimulationPtr = aSimulationPtr;
      if (!mInitialized)
      {
         // Initialize the random number stream.  Each shot has a different seed.
         long int seed = mSimulationPtr->LockRandom().Uniform<long>();
         mSimulationPtr->UnlockRandom();
         mRandom.SetSeed(seed);

         mInitialized = true;
      }

      if ((mCentroidPlatformPtr != nullptr) && mCentroidNeedsAGL_Adjustment)
      {
         wsf::Terrain terrain(mSimulationPtr->GetTerrainInterface());
         double       lat;
         double       lon;
         double       alt;
         mCentroidPlatformPtr->GetLocationLLA(lat, lon, alt);
         float elev;
         terrain.GetElevInterp(lat, lon, elev);
         alt += elev;
         mCentroidPlatformPtr->SetLocationLLA(lat, lon, alt);
         mCentroidNeedsAGL_Adjustment = false;
      }
   }

   if (GetNameId().IsNull())
   {
      SetName(GetTypeId());
   }
   if ((mRadarBehavior == cRANDOM_TO_MULTIPLE_RADARS) && (mPlatformBehavior == cCONSISTENT_TO_MULTIPLE_PLATFORMS))
   {
      auto out = ut::log::error() << "Initializing false-target effect, the combination of 'random_to_multiple_radars' "
                                     "and 'consistent_to_multiple_platforms' is not allowed.";
      out.AddNote() << "False Target: " << GetNameId();
      initialized &= false;
   }


   if ((mRadarBehavior == cCONSISTENT_TO_MULTIPLE_RADARS) && !mExplicitReferenceCentroid)
   {
      auto out = ut::log::error() << "Initializing false-target effect, false target reference centroid not set for "
                                     "'consistent_to_multiple_radars' distribution.";
      out.AddNote() << "False Target: " << GetNameId();
      initialized &= false;
   }

   return initialized;
}

// =================================================================================================
bool WsfFalseTarget::Initialize(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr)
{
   bool ok = true;

   if (!mInitialized)
   {
      WsfSimulation* simPtr = aJammerXmtrPtr->GetPlatform()->GetSimulation();
      assert(simPtr);
      ok &= Initialize(simPtr);
   }

   mJammerIdToStateMap.emplace(aJammerXmtrPtr->GetUniqueId(), JammerState{});
   Update(aSimTime, aJammerXmtrPtr);
   mInitializationTime = aSimTime;

   return ok;
}

// =================================================================================================
// virtual
bool WsfFalseTarget::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "false_target_behavior")
   {
      std::string blipBehavior;
      aInput.ReadValue(blipBehavior);
      if (blipBehavior == "random_scan_to_scan")
      {
         mBlipBehavior = cRANDOM_SCAN_TO_SCAN;
      }
      else if (blipBehavior == "consistent_scan_to_scan")
      {
         mBlipBehavior = cCONSISTENT_SCAN_TO_SCAN;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "multi_radar_behavior")
   {
      std::string radarBehavior;
      aInput.ReadValue(radarBehavior);
      if (radarBehavior == "random_to_multiple_radars")
      {
         mRadarBehavior = cRANDOM_TO_MULTIPLE_RADARS;
      }
      else if (radarBehavior == "consistent_to_multiple_radars")
      {
         mRadarBehavior = cCONSISTENT_TO_MULTIPLE_RADARS;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "persistence")
   {
      aInput.ReadValue(mPersistence);
      aInput.ValueInClosedRange(mPersistence, 0.0, 1.0);
   }
   else if (command == "quantity")
   {
      unsigned int number;
      aInput.ReadValue(number);
      aInput.ValueGreaterOrEqual(number, 0U);
      mInitialNumberOfFTs  = static_cast<unsigned int>(number);
      mExplicitNumberOfFTs = true;
   }
   else if (command == "range_constrained")
   {
      aInput.ReadValue(mRangeConstrained);
      mExplicitRangeConstraint = true;
   }
   else if (command == "scan_rate")
   {
      double frameRate;
      aInput.ReadValueOfType(frameRate, UtInput::cFREQUENCY);
      aInput.ValueGreater(frameRate, 0.0);
      mScanInterval         = 1.0 / frameRate;
      mExplicitScanInterval = true;
   }
   else if (command == "scan_time")
   {
      aInput.ReadValueOfType(mScanInterval, UtInput::cTIME);
      aInput.ValueGreater(mScanInterval, 0.0);
      mExplicitScanInterval = true;
   }
   else if ((command == "track_movement") || (command == "false_target_movement"))
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "speeds") || (command == "track_speeds"))
         {
            aInput.ReadValueOfType(mBlipSpeeds[0], UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(mBlipSpeeds[0], 0.0);
            aInput.ReadValueOfType(mBlipSpeeds[1], UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(mBlipSpeeds[1], 0.0);
            aInput.ValueGreaterOrEqual(mBlipSpeeds[1], mBlipSpeeds[0]);
            mBlipReference      = cBR_NONE;
            mExplicitBlipSpeeds = true;
         }
         else if ((command == "track_headings") || (command == "headings"))
         {
            aInput.ReadValueOfType(mBlipHeadings[0], UtInput::cANGLE);
            aInput.ReadValueOfType(mBlipHeadings[1], UtInput::cANGLE);
            aInput.ValueGreaterOrEqual(mBlipHeadings[1], mBlipHeadings[0]);
            mBlipReference = cBR_NONE;
         }
         else if ((command == "reference") || (command == "movement_reference"))
         {
            std::string ref;
            aInput.ReadValue(ref);
            if (ref == "jammer")
            {
               mBlipReference = cBR_JAMMER;
            }
            else if (ref == "none")
            {
               mBlipReference = cBR_NONE;
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if ((command == "distribution") || (command == "circular_distribution") || (command == "sectored_distribution"))
   {
      mRangeExtent.clear();
      mAzimuthExtent.clear();
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         UtInput& input = inputBlock.GetInput();

         ExtentPair extent;
         if (command == "bearing_reference")
         {
            std::string ref;
            aInput.ReadValue(ref);
            if (ref == "jammer")
            {
               mBearingReference = cJAMMER;
            }
            else if (ref == "north")
            {
               mBearingReference = cNORTH;
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (command == "radius")
         {
            extent.mMin = 0;
            input.ReadValueOfType(extent.mMax, UtInput::cLENGTH);
            aInput.ValueGreater(extent.mMax, extent.mMin);
            mRangeExtent.push_back(extent);
            mExplicitRangeExtent = true;
         }
         else if (command == "sector_arc")
         {
            double azimuthExtent;
            input.ReadValueOfType(azimuthExtent, UtInput::cANGLE);
            extent.mMin = -azimuthExtent / 2.0;
            extent.mMax = azimuthExtent / 2.0;
            mAzimuthExtent.push_back(extent);
            mExplicitAzimuthExtent = true;
         }
         else if (command == "range_extent")
         {
            input.ReadValueOfType(extent.mMin, UtInput::cLENGTH);
            input.ReadValueOfType(extent.mMax, UtInput::cLENGTH);
            aInput.ValueGreater(extent.mMax, extent.mMin);
            mRangeExtent.push_back(extent);
            mExplicitRangeExtent = true;
         }
         else if (command == "azimuth_extent")
         {
            input.ReadValueOfType(extent.mMin, UtInput::cANGLE);
            input.ReadValueOfType(extent.mMax, UtInput::cANGLE);
            aInput.ValueGreater(extent.mMax, extent.mMin);
            mAzimuthExtent.push_back(extent);
            mExplicitAzimuthExtent = true;
         }
         else if (command == "range_distribution")
         {
            std::string distType;
            input.ReadValue(distType);
            if (distType == "uniform")
            {
               mRangeDistribution = cUNIFORM;
            }
            else if (distType == "log")
            {
               mRangeDistribution = cLOG;
            }
            else if (distType == "exponential")
            {
               mRangeDistribution = cEXPONENTIAL;
            }
            else
            {
               throw UtInput::BadValue(input, " unknown range_distribution type.");
            }
         }
         else
         {
            throw UtInput::UnknownCommand(input);
         }
      }
   }
   else if ((command == "reference_centroid") || (command == "distribution_centroid"))
   {
      UtInputBlock inputBlock(aInput);
      double       lat;
      double       lon;
      double       alt;
      bool         haveLat  = false;
      bool         haveLon  = false;
      bool         haveAlt  = false;
      bool         altIsAGL = false;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "speed")
         {
            aInput.ReadValueOfType(mCentroidSpeed, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(mCentroidSpeed, 0.0);
         }
         else if (command == "heading")
         {
            aInput.ReadValueOfType(mCentroidHeading, UtInput::cANGLE);
         }
         else if (command == "position")
         {
            aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
            haveLat = true;
            haveLon = true;
         }
         else if (command == "altitude")
         {
            aInput.ReadValueOfType(alt, UtInput::cLENGTH);
            haveAlt = true;
         }
         else if (command == "agl")
         {
            altIsAGL = true;
         }
         else if (command == "msl")
         {
            altIsAGL = false;
         }
         else if (command == "latitude")
         {
            aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
            haveLat = true;
         }
         else if (command == "longitude")
         {
            aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
            haveLon = true;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (haveLat && haveLon)
      {
         // If an altitude wasn't supplied then put it 1 meter above the ground
         if (!haveAlt)
         {
            alt      = 1.0;
            altIsAGL = true;
         }
         mCentroidNeedsAGL_Adjustment = altIsAGL;
         SetReferenceCentroid(lat, lon, alt);
         mExplicitReferenceCentroid = true;
      }
      else
      {
         if (!haveLat)
         {
            auto out = ut::log::error() << "In Input for false-target effect, false target reference centroid latitude "
                                           "or position not properly entered.";
            out.AddNote() << "False Target: " << GetNameId();
         }
         if (!haveLon)
         {
            auto out = ut::log::error() << "In Input for false-target effect, false target reference centroid "
                                           "longitude or position not properly entered.";
            out.AddNote() << "False Target: " << GetNameId();
         }
         myCommand = false;
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void WsfFalseTarget::SetScanInterval(double aScanInterval)
{
   if ((aScanInterval >= 0.0))
   {
      mScanInterval = aScanInterval;
   }
   else
   {
      ut::log::warning() << "WsfFalseTarget:SetScanInterval: Bad scan interval value.";
   }
}

// =================================================================================================
void WsfFalseTarget::SetPersistence(double aPersistence)
{
   if ((aPersistence >= 0.0) && (aPersistence <= 1.0))
   {
      mPersistence = aPersistence;
   }
   else
   {
      ut::log::warning() << "WsfFalseTarget:SetPersistence: Bad persistence value.";
   }
}

// =================================================================================================
void WsfFalseTarget::SetInitialNumberOfFTs(unsigned int aInitialNumberOfFTs, bool aIsExplicit /* = false*/)
{
   unsigned int numberOfFTs(0);
   if (mExplicitNumberOfFTs && (aInitialNumberOfFTs != mInitialNumberOfFTs))
   {
      auto out =
         ut::log::warning()
         << "WsfFalseTarget::SetInitialNumberOfFTs: Already had an initial number of false targets explicitly defined.";
      out.AddNote() << "False Target: " << GetName();
      out.AddNote() << "Type: " << GetType();
      auto note = out.AddNote() << "Will be resetting to the maximum of these values:";
      note.AddNote() << "Prev: " << mInitialNumberOfFTs;
      note.AddNote() << "New: " << aInitialNumberOfFTs;
      note.AddNote() << "Set: " << std::max(mInitialNumberOfFTs, aInitialNumberOfFTs);
      numberOfFTs = mInitialNumberOfFTs;
   }
   mInitialNumberOfFTs  = std::max(aInitialNumberOfFTs, numberOfFTs);
   mExplicitNumberOfFTs = aIsExplicit;
}

// =================================================================================================
void WsfFalseTarget::SetAzimuthExtent(double aAzLower, double aAzUpper)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);

   mAzimuthExtent.clear();
   ExtentPair extent;
   extent.mMin = aAzLower;
   extent.mMax = aAzUpper;
   mAzimuthExtent.push_back(extent);
}

// =================================================================================================
void WsfFalseTarget::SetRangeExtent(double aRangeLower, double aRangeUpper)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);

   mRangeExtent.clear();
   ExtentPair extent;
   extent.mMin = aRangeLower;
   extent.mMax = aRangeUpper;
   mRangeExtent.push_back(extent);
}

// =================================================================================================
void WsfFalseTarget::SetBlipSpeeds(double aMinSpeed, double aMaxSpeed)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);

   if ((aMinSpeed >= 0.0) && (aMinSpeed <= aMaxSpeed))
   {
      mBlipSpeeds[0] = aMinSpeed;
      mBlipSpeeds[1] = aMaxSpeed;
   }
}

// =================================================================================================
void WsfFalseTarget::GetBlipSpeeds(double& aMinBlipSpeed, double& aMaxBlipSpeed) const
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   aMinBlipSpeed = mBlipSpeeds[0];
   aMaxBlipSpeed = mBlipSpeeds[1];
}

// =================================================================================================
void WsfFalseTarget::GetBlipHeadings(double& aMinBlipHeading, double& aMaxBlipHeading) const
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   aMinBlipHeading = mBlipHeadings[0];
   aMaxBlipHeading = mBlipHeadings[1];
}

// =================================================================================================
bool WsfFalseTarget::PowerSufficiencyCheck(double      aSimTime,
                                           WsfEM_Xmtr* aJammerXmtrPtr,
                                           WsfEM_Xmtr* aRadarXmtrPtr,
                                           WsfEM_Rcvr* aRadarRcvrPtr)
{
   if ((aRadarRcvrPtr == nullptr) || (aJammerXmtrPtr == nullptr))
   {
      return false;
   }

   // algorithm for checking to see if false target jammer has enough power
   //  to be detectable by the radar's receiver and whether the received power
   //  resembles a specified type of target (via RCS value)

   // Get the jammer power to check thresholds, point the receiver at the jammer platform.
   WsfEM_Interaction jmrTgtInteraction;
   jmrTgtInteraction.BeginGenericInteraction(aRadarXmtrPtr, aJammerXmtrPtr->GetPlatform(), aRadarRcvrPtr);
   jmrTgtInteraction.ComputeUndefinedGeometry();
   jmrTgtInteraction.SetReceiverBeamPosition();
   jmrTgtInteraction.SetTransmitterBeamPosition();
   double            jammerPowers[3];
   WsfEM_Interaction jammerInteraction;
   WsfRF_Jammer::ComputeJammerPowers(aSimTime, aJammerXmtrPtr, jmrTgtInteraction, jammerInteraction, jammerPowers);

   WsfEW_Result* ewJammerResult = WsfEW_Result::Find(jammerInteraction);

   // Divide out repeater effects as it may be negatively impacting the power.
   // NOTE - not accounting for receiver noise multiplier, unless set in another interaction already.
   jammerPowers[2] *= ewJammerResult->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                      ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                      ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain /
                      ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor;

   double jammerSNR = aRadarRcvrPtr->ComputeSignalToNoise(jammerPowers[2], 0.0, 0.0);

   bool falseTargetSuccess = (jammerSNR >= aRadarRcvrPtr->GetDetectionThreshold());

   if (GetDebug() && (!falseTargetSuccess))
   {
      auto out = ut::log::debug() << "False target is unable to jam due to power sufficiency check.";
      out.AddNote() << "Jammer Platform: " << aJammerXmtrPtr->GetPlatform()->GetName();
      out.AddNote() << "Jammer Part: " << aJammerXmtrPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "False Target: " << GetName();
      out.AddNote() << "Radar Platform: " << aRadarRcvrPtr->GetPlatform()->GetName();
      out.AddNote() << "Radar Part: " << aRadarRcvrPtr->GetArticulatedPart()->GetName();

      if (jammerSNR > 0.0)
      {
         out.AddNote() << "Receiver Detection Threshold SNR: " << aRadarRcvrPtr->GetDetectionThreshold();
         out.AddNote() << "Effective JNR: " << UtMath::LinearToDB(jammerSNR) << " dB";
      }
      else
      {
         out.AddNote() << "Not detecting jamming power in main beam.";
      }

      if ((ewJammerResult->mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) != 0u)
      {
         out.AddNote() << " Calculated Jammer Power Adjusted By "
                       << UtMath::LinearToDB(ewJammerResult->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                                             ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                                             ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain)
                       << " dB due to EW Effects";
      }
   }
   return falseTargetSuccess;
}

// =================================================================================================
//! The Update method resolves the existence of targeted radar sites,
//! and calculates a reference location for the false targets.
// virtual
void WsfFalseTarget::Update(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr)
{
   if (aJammerXmtrPtr == nullptr)
   {
      return;
   }

   unsigned int jammerId = aJammerXmtrPtr->GetUniqueId();

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (aSimTime > mJammerIdToStateMap[jammerId].mLastUpdate)
   {
      if (aSimTime >= mJammerIdToStateMap[jammerId].mLastScan + mScanInterval)
      {
         // mLastScan < 0.0 => first time through create blips
         // mBehavior = cRANDOM_SCAN_TO_SCAN => create new blips every scan interval
         // mPersistance != 1 => purge some number of blips every scan
         if (mJammerIdToStateMap[jammerId].mLastScan < 0.0 || mBlipBehavior == cRANDOM_SCAN_TO_SCAN || mPersistence < 0.999)
         {
            Rescan(aSimTime, aJammerXmtrPtr);
         }
         else
         {
            mLastScan                               = aSimTime;
            mJammerIdToStateMap[jammerId].mLastScan = aSimTime;
            UpdateBlipPositions(aSimTime, jammerId);
         }
      }
      else
      {
         UpdateBlipPositions(aSimTime, jammerId);
      }

      mLastUpdate                               = aSimTime;
      mJammerIdToStateMap[jammerId].mLastUpdate = aSimTime;
   }
}

// =================================================================================================
void WsfFalseTarget::SetReferenceCentroid(double aLatitude, double aLongitude, double aAltitude)
{
   mCentroidPlatformPtr->SetLocationLLA(aLatitude, aLongitude, aAltitude);
}

// =================================================================================================
void WsfFalseTarget::GetReferenceCentroid(double& aLatitude, double& aLongitude, double& aAltitude)
{
   mCentroidPlatformPtr->GetLocationLLA(aLatitude, aLongitude, aAltitude);
}

// =================================================================================================
void WsfFalseTarget::RemoveJammer(WsfEM_Xmtr* aJammerXmtrPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   // erase from jammer state map
   JammerIdToStateMap::iterator jsi = mJammerIdToStateMap.find(aJammerXmtrPtr->GetUniqueId());
   if (jsi != mJammerIdToStateMap.end())
   {
      mJammerIdToStateMap.erase(jsi);
      if (mJammerIdToStateMap.empty())
      {
         Reset();
      }
      else
      {
         // erase blips for this jammer from the blip map
         ClearBlips(aJammerXmtrPtr->GetUniqueId());
      }
   }
}

// =================================================================================================
// protected
void WsfFalseTarget::UpdateBlipPositions(double aSimTime, unsigned int aJammerId)
{
   if (mJammerIdToStateMap[aJammerId].mLastUpdate < 0.0)
   {
      return;
   }

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   double                                deltaT = aSimTime - mJammerIdToStateMap[aJammerId].mLastBlipUpdate;
   if (deltaT >= 1.0E-5)
   {
      double          deltaLocWCS[3];
      Blips::iterator blipIter;
      if (mBlipReference == cBR_JAMMER)
      {
         WsfPlatform* jammerPtr = GetSimulation()->GetPlatformByName(mFusedPlatformId);
         assert(jammerPtr != nullptr);
         jammerPtr->Update(aSimTime);

         double currentLocWCS[3];
         jammerPtr->GetLocationWCS(currentLocWCS);
         UtVec3d::Subtract(deltaLocWCS, currentLocWCS, mJammerIdToStateMap[aJammerId].mLastBlipLocWCS);
         UtVec3d::Set(mJammerIdToStateMap[aJammerId].mLastBlipLocWCS, currentLocWCS);

         double velNED[3];
         jammerPtr->GetVelocityNED(velNED);
         UtVec3d::Set(velNED, velNED[0], velNED[1], 0.0);
         double orientNED[3];
         jammerPtr->GetOrientationNED(orientNED[0], orientNED[1], orientNED[2]);

         for (blipIter = mBlipsMap[aJammerId].begin(); blipIter != mBlipsMap[aJammerId].end(); ++blipIter)
         {
            (*blipIter)->SetVelocityNED(velNED);
            (*blipIter)->IncrementLocationWCS(deltaLocWCS);
            (*blipIter)->SetOrientationNED(orientNED[0], 0.0, 0.0);
         }
         mJammerIdToStateMap[aJammerId].mLastBlipUpdate = aSimTime;
      }
      else if ((mBlipSpeeds[1] > 1.0E-5) && (mBlipReference == cBR_NONE))
      {
         for (blipIter = mBlipsMap[aJammerId].begin(); blipIter != mBlipsMap[aJammerId].end(); ++blipIter)
         {
            (*blipIter)->GetVelocityWCS(deltaLocWCS);
            UtVec3d::Multiply(deltaLocWCS, deltaT);
            (*blipIter)->IncrementLocationWCS(deltaLocWCS);
         }
         mJammerIdToStateMap[aJammerId].mLastBlipUpdate = aSimTime;
      }
   }
}

// =================================================================================================
// protected
void WsfFalseTarget::UpdateBaseBlipPositions(double aSimTime)
{
   if ((mLastBaseLocsUpdate < 0.0) || mBaseLocations.empty())
   {
      return;
   }

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   double                                deltaT = aSimTime - mLastBaseLocsUpdate;
   if (deltaT >= 1.0E-5)
   {
      if (mBlipReference == cBR_JAMMER)
      {
         WsfPlatform* jammerPtr = GetSimulation()->GetPlatformByName(mFusedPlatformId);
         assert(jammerPtr != nullptr);
         jammerPtr->Update(aSimTime);

         double currentLocWCS[3];
         jammerPtr->GetLocationWCS(currentLocWCS);

         double deltaLocWCS[3];
         UtVec3d::Subtract(deltaLocWCS, currentLocWCS, mLastBaseLocWCS);
         UtVec3d::Set(mLastBaseLocWCS, currentLocWCS);

         double speed = jammerPtr->GetSpeed();

         double orientNED[3];
         jammerPtr->GetOrientationNED(orientNED[0], orientNED[1], orientNED[2]);

         BaseLocsVec::iterator bli;
         for (bli = mBaseLocations.begin(); bli != mBaseLocations.end(); ++bli)
         {
            (*bli).mSpeed = speed;
            UtVec3d::Add((*bli).mLocationWCS, (*bli).mLocationWCS, deltaLocWCS);
            (*bli).mHeading = orientNED[0];
         }
         mLastBaseLocsUpdate = aSimTime;
      }
      else if ((mBlipSpeeds[1] > 1.0E-5) && (mBlipReference == cBR_NONE))
      {
         UtEntity              tempEntity;
         double                deltaNED[3];
         BaseLocsVec::iterator bli;
         for (bli = mBaseLocations.begin(); bli != mBaseLocations.end(); ++bli)
         {
            tempEntity.SetLocationWCS(bli->mLocationWCS);
            UtVec3d::Set(deltaNED, cos(bli->mHeading), sin(bli->mHeading), 0.0);
            UtVec3d::Multiply(deltaNED, deltaT);
            tempEntity.IncrementLocationNED(deltaNED);
            tempEntity.GetLocationWCS(bli->mLocationWCS);
         }
         mLastBaseLocsUpdate = aSimTime;
      }
   }
}

// =================================================================================================
//! Produce a group of false target blips for a single scan of an observing sensor.
// virtual protected
void WsfFalseTarget::Rescan(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr)
{
   bool createBlips = true;

   unsigned int jammerId = aJammerXmtrPtr->GetUniqueId();

   const size_t blipsInitial = mBlipsMap[jammerId].size();
   size_t       blipsPlaced  = 0;
   size_t       blipsRemoved = 0;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (mBlipBehavior == cRANDOM_SCAN_TO_SCAN)
   {
      // If the false target behavior is completely random scan-to-scan,
      // then erase all existing blips, and place new ones (below).
      blipsRemoved += blipsInitial;
      if (aSimTime >= mLastScan + mScanInterval)
      {
         mBaseLocations.clear();
         mLastScan = aSimTime;
      }
      mJammerIdToStateMap[jammerId].mBlipsCreated = false;
      createBlips                                 = true;
   }
   else if ((mBlipBehavior == cCONSISTENT_SCAN_TO_SCAN) && (mJammerIdToStateMap[jammerId].mLastScan < 0.0))
   {
      // Have not generated a scan yet, so proceed with
      // generating blips from the first scan (below)...
      createBlips = true;
   }
   else if ((mBlipBehavior == cCONSISTENT_SCAN_TO_SCAN) && (mPersistence < 0.999))
   {
      // The blips have already been created, and they will be consistently
      // extrapolated, scan to scan, so there is no need to generate new ones...
      createBlips = false;

      // BUT, each time we scan, randomly reduce the number of blips by (1.0 - persistence) ratio.
      // if (mPersistence < 0.999)
      //{
      size_t blipsToRemoveThisScan = static_cast<size_t>((blipsInitial * (1.0 - mPersistence)) + 0.5);
      if (mBlipsMap[jammerId].size() > blipsToRemoveThisScan)
      {
         blipsRemoved += blipsToRemoveThisScan;
         for (size_t i = 0; i < blipsToRemoveThisScan; ++i)
         {
            size_t indx = mRandom.Uniform<size_t>(0, mBlipsMap[jammerId].size() - 1);
            delete mBlipsMap[jammerId].at(indx);
            mBlipsMap[jammerId].erase(mBlipsMap[jammerId].begin() + indx);
         }
      }
      else
      {
         blipsRemoved += mBlipsMap[jammerId].size();
         ClearBlips(jammerId);
      }
      //}
   }

   if (createBlips && (!mJammerIdToStateMap[jammerId].mBlipsCreated))
   {
      ClearBlips(jammerId);
      mBlipsMap[jammerId].reserve(mInitialNumberOfFTs);

      mJammerIdToStateMap[jammerId].mBlipsCreated = true;

      double nedOffset[3];
      double jammerLocWCS[3];
      double referenceHeading = 0.0;

      WsfPlatform* jammerPtr = aJammerXmtrPtr->GetPlatform();
      assert(jammerPtr != nullptr);
      jammerPtr->Update(aSimTime);
      jammerPtr->GetLocationWCS(jammerLocWCS);
      UpdateCentroidPosition(aSimTime);

      if (mBearingReference == cJAMMER)
      {
         mCentroidPlatformPtr->GetRelativeLocationNED(jammerPtr, nedOffset);
         referenceHeading = atan2(nedOffset[1], nedOffset[0]);
      }
      else if (mBearingReference == cNORTH)
      {
         referenceHeading = 0.0;
      }
      mCentroidPlatformPtr->SetOrientationNED(referenceHeading, 0.0, 0.0);
      nedOffset[2] = 0.0;

      if (!mBaseLocations.empty())
      {
         UpdateBaseBlipPositions(aSimTime);
      }
      else if (mRadarBehavior == cCONSISTENT_TO_MULTIPLE_RADARS)
      {
         mBaseLocations.reserve(mInitialNumberOfFTs);
      }

      double speed   = 0.0;
      double heading = 0.0;
      if (mBlipReference == cBR_JAMMER)
      {
         double temp[2];
         speed = jammerPtr->GetSpeed();
         jammerPtr->GetOrientationNED(heading, temp[0], temp[1]);

         if (mFusedPlatformId.IsNull())
         {
            mFusedPlatformId = jammerPtr->GetNameId();
            double locWCS[3];
            jammerPtr->GetLocationWCS(locWCS);
            UtVec3d::Set(mJammerIdToStateMap[jammerId].mLastBlipLocWCS, locWCS);
            UtVec3d::Set(mLastBaseLocWCS, locWCS);
         }
      }

      BaseData baseBlipData;
      double   radius;
      double   bearing;
      double   blipLocWCS[3];
      double   blipVelNED[3];
      Blip*    blipPtr = nullptr;

      for (size_t numPlaced = 0; numPlaced < mInitialNumberOfFTs; ++numPlaced)
      {
         blipPtr = new Blip();

         if ((mRadarBehavior == cRANDOM_TO_MULTIPLE_RADARS) ||
             ((mRadarBehavior == cCONSISTENT_TO_MULTIPLE_RADARS) && (mBaseLocations.size() < mInitialNumberOfFTs)))
         {
            CreateBlipNameId(blipPtr);

            assert(!mRangeExtent.empty());
            size_t rangeExtentIndex = static_cast<size_t>(mRandom.Uniform(0.0, static_cast<double>(mRangeExtent.size())));
            while (rangeExtentIndex >= mRangeExtent.size())
            {
               rangeExtentIndex = static_cast<size_t>(mRandom.Uniform(0.0, static_cast<double>(mRangeExtent.size())));
            }

            if (mRangeDistribution == cEXPONENTIAL)
            {
               radius = mRangeExtent[rangeExtentIndex].mMin +
                        (mRangeExtent[rangeExtentIndex].mMax - mRangeExtent[rangeExtentIndex].mMin) *
                           (1 + log(mRandom.Uniform(exp(-1.0), 1.0)));
            }
            else if (mRangeDistribution == cLOG)
            {
               radius = mRangeExtent[rangeExtentIndex].mMin +
                        (mRangeExtent[rangeExtentIndex].mMax - mRangeExtent[rangeExtentIndex].mMin) *
                           (1 + log10(mRandom.Uniform(0.1, 1.0)));
            }
            else
            {
               radius = mRandom.Uniform(mRangeExtent[rangeExtentIndex].mMin, mRangeExtent[rangeExtentIndex].mMax);
            }

            assert(mAzimuthExtent.size() != 0);
            size_t azExtentIndex = static_cast<size_t>(mRandom.Uniform(0.0, static_cast<double>(mAzimuthExtent.size())));
            while (azExtentIndex >= mAzimuthExtent.size())
            {
               azExtentIndex = static_cast<size_t>(mRandom.Uniform(0.0, static_cast<double>(mAzimuthExtent.size())));
            }
            bearing = mRandom.Uniform(mAzimuthExtent[azExtentIndex].mMin, mAzimuthExtent[azExtentIndex].mMax) +
                      referenceHeading;

            nedOffset[0] = radius * cos(bearing);
            nedOffset[1] = radius * sin(bearing);
            nedOffset[2] = 0;

            mCentroidPlatformPtr->ConvertNEDToWCS(nedOffset, blipLocWCS);

            // Get velocity and heading
            if (mBlipReference != cBR_JAMMER)
            {
               speed   = mRandom.Uniform(mBlipSpeeds[0], mBlipSpeeds[1]);
               heading = mRandom.Uniform(mBlipHeadings[0], mBlipHeadings[1]);
            }

            if (mRadarBehavior == cCONSISTENT_TO_MULTIPLE_RADARS)
            {
               baseBlipData.Clear();
               UtVec3d::Set(baseBlipData.mLocationWCS, blipLocWCS);
               baseBlipData.mSpeed    = speed;
               baseBlipData.mHeading  = heading;
               baseBlipData.mNameId   = blipPtr->GetNameId();
               baseBlipData.mUniqueId = blipPtr->GetUniqueId();
               mBaseLocations.push_back(baseBlipData);
            }
         }
         else
         {
            UtVec3d::Set(blipLocWCS, mBaseLocations[numPlaced].mLocationWCS);
            speed   = mBaseLocations[numPlaced].mSpeed;
            heading = mBaseLocations[numPlaced].mHeading;
            blipPtr->SetNameId(mBaseLocations[numPlaced].mNameId);
            blipPtr->SetUniqueId(mBaseLocations[numPlaced].mUniqueId);
         }

         blipPtr->SetLocationWCS(blipLocWCS);
         UtVec3d::Set(blipVelNED, cos(heading) * speed, sin(heading) * speed, 0.0);
         blipPtr->SetOrientationNED(heading, 0.0, 0.0);
         blipPtr->SetVelocityNED(blipVelNED);

         // UtVec3d::Subtract(deltaWCS, jammerWCS, radarWCS);

         blipPtr->SetFalseTarget(this);
         blipPtr->SetJammerXmtr(aJammerXmtrPtr); // set in false-target screener
         blipPtr->SetBehavior(GetBlipBehavior());

         // blipIter = find(mBlipsMap[jammerId].begin(), mBlipsMap[jammerId].end(), blipPtr);
         // if (blipIter == mBlipsMap[jammerId].end())
         //{
         mBlipsMap[jammerId].push_back(blipPtr);
         ++blipsPlaced;
         //}
      }
   } // if (createBlips  && !mBlipsCreated)
   else if ((!createBlips) && (mJammerIdToStateMap[jammerId].mBlipsCreated))
   {
      UpdateBlipPositions(aSimTime, jammerId);
   }

   if (blipsPlaced > 0)
   {
      // clear all screener jammer pairs from list so they
      //  know new blips have been created
      mNewBlipsStateVec.clear();
      mJammerIdToStateMap[jammerId].mLastBlipUpdate = aSimTime;
   }

   if ((!mBaseLocations.empty()) && (mLastBaseLocsUpdate < 0.0))
   {
      mLastBaseLocsUpdate = aSimTime;
   }

   if (mDebug)
   {
      auto logDebug = ut::log::debug() << "False Target scan report:";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Initial Blips Count: " << blipsInitial;
      logDebug.AddNote() << "Placed Blips Count: " << blipsPlaced;
      logDebug.AddNote() << "Removed Blips Count: " << blipsRemoved;
      logDebug.AddNote() << "Ending Blip Count: " << mBlipsMap[jammerId].size();
   }

   mJammerIdToStateMap[jammerId].mLastScan = aSimTime;
}

// =================================================================================================
void WsfFalseTarget::ClearAllBlips()
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (!mBlipsMap.empty())
   {
      JammerIdToBlipsMap::iterator bmi = mBlipsMap.begin();
      for (; bmi != mBlipsMap.end(); ++bmi)
      {
         Blips::iterator bi = bmi->second.begin();
         for (; bi != bmi->second.end(); ++bi)
         {
            delete *bi;
         }
      }
      mBlipsMap.clear();
   }
}

// =================================================================================================
void WsfFalseTarget::ClearBlips(unsigned int aJammerId)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   JammerIdToBlipsMap::iterator          bmi = mBlipsMap.find(aJammerId);
   if (bmi != mBlipsMap.end())
   {
      Blips::iterator bi = bmi->second.begin();
      for (; bi != bmi->second.end(); ++bi)
      {
         delete *bi;
      }
      mBlipsMap.erase(bmi);
   }
}

// =================================================================================================
WsfFalseTarget::Blips& WsfFalseTarget::GetBlips(unsigned int aJammerId)
{
   return mBlipsMap[aJammerId];
}

// =================================================================================================
WsfFalseTargetBlip* WsfFalseTarget::GetBlipPtr(WsfStringId aBlipNameId, unsigned int aJammerId)
{
   Blip* blipPtr = nullptr;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   JammerIdToBlipsMap::iterator          bmi = mBlipsMap.find(aJammerId);
   if (bmi != mBlipsMap.end())
   {
      Blips::iterator bi = bmi->second.begin();
      for (; bi != bmi->second.end(); ++bi)
      {
         if ((*bi)->GetNameId() == aBlipNameId)
         {
            blipPtr = *bi;
            break;
         }
      }
   }
   return blipPtr;
}


// =================================================================================================
WsfFalseTargetBlip* WsfFalseTarget::GetBlipPtr(size_t aBlipUniqueId, unsigned int aJammerId)
{
   Blip* blipPtr = nullptr;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   JammerIdToBlipsMap::iterator          bmi = mBlipsMap.find(aJammerId);
   if (bmi != mBlipsMap.end())
   {
      Blips::iterator bi = bmi->second.begin();
      for (; bi != bmi->second.end(); ++bi)
      {
         if ((*bi)->GetUniqueId() == aBlipUniqueId)
         {
            blipPtr = *bi;
            break;
         }
      }
   }
   return blipPtr;
}

// =================================================================================================
bool WsfFalseTarget::GetBlipsCreated(unsigned int aJammerId)
{
   bool blipsCreated(false);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   JammerIdToStateMap::iterator          jidsmi = mJammerIdToStateMap.find(aJammerId);
   if (jidsmi != mJammerIdToStateMap.end())
   {
      blipsCreated = mJammerIdToStateMap[aJammerId].mBlipsCreated;
   }
   return blipsCreated;
}

// =================================================================================================
void WsfFalseTarget::CreateBlipNameId(Blip* aBlipPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   std::ostringstream                    oss;
   oss << GetName() << "_" << mFTNameExtensionId << "." << mBlipIdCounter++;
   std::string name(oss.str());

   aBlipPtr->SetNameId(WsfStringId(name));

   aBlipPtr->SetUniqueId(WsfFalseTarget::AssignFT_BlipUniqueId());
}

// =================================================================================================
void WsfFalseTarget::UpdateCentroidPosition(double aSimTime)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   double                                deltaT = aSimTime - mLastCentroidUpdate;
   if ((mCentroidSpeed > 0.0001) && (deltaT > 0.0001))
   {
      double deltaNED[] = {cos(mCentroidHeading), sin(mCentroidHeading), 0.0};
      UtVec3d::Multiply(deltaNED, mCentroidSpeed * deltaT);
      mCentroidPlatformPtr->IncrementLocationNED(deltaNED);
   }
   mLastCentroidUpdate = aSimTime;
}

// =================================================================================================
void WsfFalseTarget::DropTracks(RadarSnsrPair aIdKey, WsfEM_Xmtr* aJammerXmtrPtr)
{
   unsigned int jammerId = aJammerXmtrPtr->GetUniqueId();

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   for (Blip* blipPtr : mBlipsMap[jammerId])
   {
      if ((blipPtr->GetJammerXmtr() != nullptr) && (blipPtr->GetJammerXmtr() == aJammerXmtrPtr))
      {
         blipPtr->UnDetect(aIdKey);
      }
   }
}

// =================================================================================================
void WsfFalseTarget::Reset()
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);

   mChanged = true;
   ClearAllBlips();
   mBaseLocations.clear();
   mLastUpdate         = -DBL_MAX;
   mLastScan           = -DBL_MAX;
   mLastBaseLocsUpdate = -DBL_MAX;
   mFusedPlatformId    = nullptr;
}

// =================================================================================================
bool WsfFalseTarget::GetNewBlipsState(WsfFalseTargetScreener* aScreenerPtr, WsfEM_Xmtr* aJammerXmtrPtr)
{
   bool                     newBlips = true;
   ScreenerPtrJammerXmtrPtr bsKey(std::make_pair(aScreenerPtr, aJammerXmtrPtr));

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipStateVec::const_iterator          bsi = find(mNewBlipsStateVec.begin(), mNewBlipsStateVec.end(), bsKey);
   if (bsi != mNewBlipsStateVec.end())
   {
      newBlips = false;
   }

   return newBlips;
}

// =================================================================================================
void WsfFalseTarget::SetNewBlipsState(WsfFalseTargetScreener* aScreenerPtr, WsfEM_Xmtr* aJammerXmtrPtr, bool aState)
{
   ScreenerPtrJammerXmtrPtr bsKey(std::make_pair(aScreenerPtr, aJammerXmtrPtr));

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipStateVec::iterator                bsi = find(mNewBlipsStateVec.begin(), mNewBlipsStateVec.end(), bsKey);
   if (bsi == mNewBlipsStateVec.end() && aState == false)
   {
      mNewBlipsStateVec.push_back(bsKey);
   }
   else if (bsi != mNewBlipsStateVec.end() && aState == true)
   {
      mNewBlipsStateVec.erase(bsi);
   }
}

// =================================================================================================
// =================================================================================================

// =================================================================================================
WsfFalseTargetBlip::WsfFalseTargetBlip()
   : UtEntity()
   , mNameId(nullptr)
   , mUniqueId(0)
   , mFT_BlipTypeId("FALSE_TARGET_BLIP")
   , mDataMap()
   , mFtBehavior(WsfFalseTarget::cRANDOM_SCAN_TO_SCAN)
   , mFtPtr(nullptr)
   , mJammerXmtrPtr(nullptr)
   , mPlatformPtr(nullptr)
   , mRadarSensorIds()
   , mMutex()
{
}

// =================================================================================================
WsfFalseTargetBlip::~WsfFalseTargetBlip()
{
   if ((mFtPtr != nullptr) && (mFtPtr->GetSimulation() != nullptr) && (mPlatformPtr != nullptr))
   {
      mFtPtr->GetSimulation()->DeletePlatform(mFtPtr->GetSimulation()->GetSimTime(), mPlatformPtr);
   }

   for (auto& bdi : mDataMap)
   {
      delete bdi.second.mInteractionPtr;
   }
}

// =================================================================================================
void WsfFalseTargetBlip::AddTargetedRadarId(RadarSnsrPair aIdKey)
{
   RadarSnsrVec::iterator radarIdIter;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   radarIdIter = find(mRadarSensorIds.begin(), mRadarSensorIds.end(), aIdKey);
   if (radarIdIter == mRadarSensorIds.end())
   {
      mRadarSensorIds.push_back(aIdKey);
      mDataMap[aIdKey].Clear();
   }
}

// =================================================================================================
void WsfFalseTargetBlip::ResetBlipData(RadarSnsrPair aIdKey)
{
   RadarSnsrVec::iterator radarIdIter;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   radarIdIter = find(mRadarSensorIds.begin(), mRadarSensorIds.end(), aIdKey);
   if (radarIdIter == mRadarSensorIds.end())
   {
      mDataMap[aIdKey].Clear();
   }
}

// =================================================================================================
void WsfFalseTargetBlip::UpdateBlipData(RadarSnsrPair            aIdKey,
                                        const WsfEM_Interaction& aBlipInteraction,
                                        const WsfEM_Interaction& aJammerInteraction)
{
   double equivalentRCS = ComputeEquivalentRCS(aBlipInteraction);
   AddEquivalentRCS(aIdKey, equivalentRCS);

   double jammerToRadarRange = aJammerInteraction.mXmtrToRcvr.mRange;
   AddJammerToRadarRange(aIdKey, jammerToRadarRange);

   double locationDelta[3];
   UtVec3d::Multiply(locationDelta,
                     aJammerInteraction.mXmtrToRcvr.mUnitVecWCS,
                     ((jammerToRadarRange - GetReferenceJammerToRadarRange()) / 2));
   AddLocationDelta(aIdKey, locationDelta);

   AddActualSNR(aIdKey, aBlipInteraction.mSignalToNoise);
   AddRadarToBlipRange(aIdKey, aBlipInteraction.mRcvrToTgt.mRange);

   // A successful detection occurs only if the beam with the best signal-to-noise had no failing criteria.
   if (((aBlipInteraction.mFailedStatus & WsfSensorResult::cSIGNAL_LEVEL) == 0) &&
       ((aBlipInteraction.mFailedStatus & WsfSensorResult::cDOPPLER_LIMITS) == 0) &&
       (aJammerInteraction.mFailedStatus == 0))
   {
      if (aBlipInteraction.mSignalToNoise >= (2 * aBlipInteraction.mDetectionThreshold))
      {
         AddStrength(aIdKey, cStrong);
      }
      else if (aBlipInteraction.mSignalToNoise >= aBlipInteraction.mDetectionThreshold)
      {
         AddStrength(aIdKey, cWeak);
      }
   }
   else
   {
      AddStrength(aIdKey, cUndetected);
   }

   if ((GetFalseTarget()->GetRangeConstrained()) &&
       (aBlipInteraction.mRcvrToTgt.mRange <= jammerToRadarRange)) // blip is inside jammer radius
   {
      AddStrength(aIdKey, cUndetected);
   }
}

// =================================================================================================
// Valid after initialization only
bool WsfFalseTargetBlip::IsValidTargetedRadarId(RadarSnsrPair aIdKey)
{
   bool                   isValid(false);
   RadarSnsrVec::iterator radarIter;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   radarIter = find(mRadarSensorIds.begin(), mRadarSensorIds.end(), aIdKey);
   if (radarIter != mRadarSensorIds.end())
   {
      isValid = true;
   }
   return isValid;
}

// =================================================================================================
void WsfFalseTargetBlip::AddLocationDelta(RadarSnsrPair aIdKey, const double aLocationDelta[3])
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   UtVec3d::Set(mDataMap[aIdKey].mLocationDelta, aLocationDelta);
}

// =================================================================================================
void WsfFalseTargetBlip::GetLocationDelta(RadarSnsrPair aIdKey, double aLocationDelta[3])
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   UtVec3d::Set(aLocationDelta, mDataMap[aIdKey].mLocationDelta);
}

// =================================================================================================
void WsfFalseTargetBlip::AddStrength(RadarSnsrPair aIdKey, BlipStrength aStrength)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mStrength = aStrength;
}

// =================================================================================================
WsfFalseTargetBlip::BlipStrength WsfFalseTargetBlip::GetStrength(RadarSnsrPair aIdKey)
{
   BlipStrength blipStrength(cUndetected);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      blipStrength = (*bdi).second.mStrength;
   }
   return blipStrength;
}

// =================================================================================================
void WsfFalseTargetBlip::UnDetect(RadarSnsrPair aIdKey)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mStrength = cUndetected;
}

// =================================================================================================
void WsfFalseTargetBlip::AddActualSNR(RadarSnsrPair aIdKey, double aActualSNR)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mActualSNR = aActualSNR;
}

// =================================================================================================
double WsfFalseTargetBlip::GetActualSNR(RadarSnsrPair aIdKey)
{
   double actualSNR(-DBL_MAX);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      actualSNR = (*bdi).second.mActualSNR;
   }
   return actualSNR;
}

// =================================================================================================
double WsfFalseTargetBlip::ComputeSNR(double aSimTime, WsfEM_Interaction& aBlipInteraction, WsfEM_Interaction& aJammerInteraction)
{
   // Does not account for signal processing gains

   aBlipInteraction.mSignalToNoise = 0.0; // set the return value

   aBlipInteraction.mRcvdPower = WsfFalseTargetBlip::ComputeRcvdPower(aSimTime, aBlipInteraction, aJammerInteraction);
   if (aBlipInteraction.mRcvdPower > 0.0)
   {
      // WsfEW_Result* ewResult = WsfEW_Result::Find(aJammerInteraction);

      WsfEM_Rcvr* rcvrPtr = aBlipInteraction.GetReceiver();
      assert(rcvrPtr != nullptr);
      aBlipInteraction.mSignalToNoise = rcvrPtr->ComputeSignalToNoise(/*ewResult.mCoherentJammerPower *
                                                                      ewResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain,*/
                                                                      aBlipInteraction.mRcvdPower,
                                                                      aBlipInteraction.mClutterPower,
                                                                      aBlipInteraction.mInterferencePower);
   }
   return aBlipInteraction.mSignalToNoise;
}


// =================================================================================================
double WsfFalseTargetBlip::ComputeRcvdPower(double             aSimTime,
                                            WsfEM_Interaction& aBlipInteraction,
                                            WsfEM_Interaction& aJammerInteraction)
{
   aBlipInteraction.mRcvdPower = 0.0; // set the return value

   WsfEM_Rcvr* rcvrPtr = aBlipInteraction.GetReceiver();
   assert(rcvrPtr != nullptr);

   WsfEM_Xmtr* jammerXmtrPtr = GetJammerXmtr();
   if (jammerXmtrPtr != nullptr)
   {
      // Need to setup the radar signature and received power in the interaction in case a
      // WSF_REPEATER_EFFECt is defined.
      // Set the radar cross section of the target.
      aBlipInteraction.mRadarSig = 1.0; // calculate wrt to a 1 m^2 target for scaling purposes.

      // Calculate the signal return.
      aBlipInteraction.ComputeRF_TwoWayPower(aBlipInteraction.mRadarSig);

      // Account for the receiver gains
      aBlipInteraction.mRcvdPower *= WsfRF_Jammer::ComputeReceiverGains(rcvrPtr);

      // Actual jamming power being delivered to the targeted radar is a function of the
      // jammer power through the antenna sidelobes assuming the mainlobe is oriented where
      // the false target is being placed.
      aJammerInteraction.BeginGenericInteraction(jammerXmtrPtr, nullptr, rcvrPtr);

      WsfRF_Jammer::ComputeSingleJammerEffect(aSimTime, jammerXmtrPtr, aBlipInteraction, aJammerInteraction, true);

      WsfEW_Result* jmrResult     = WsfEW_Result::Find(aJammerInteraction);
      aBlipInteraction.mRcvdPower = std::max(0.0, jmrResult->mCoherentJammerPower);
      if (jmrResult->mCoherentJammerPower > 0.0)
      {
         WsfRF_Jammer::ComputeTotalJammerEffects(aSimTime, aBlipInteraction, true);
      }
   }
   return aBlipInteraction.mRcvdPower;
}

// =================================================================================================
double WsfFalseTargetBlip::ComputeEquivalentRCS(const WsfEM_Interaction& aBlipInteraction) const
{
   double equivalentRCS = 0.0; // set the default return value

   WsfEM_Rcvr* radarRcvrPtr = aBlipInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);
   WsfEM_Xmtr* radarXmtrPtr = aBlipInteraction.GetTransmitter();
   assert(radarXmtrPtr != nullptr);
   WsfPlatform* tempPlatformPtr = aBlipInteraction.GetTarget();
   assert(tempPlatformPtr != nullptr);

   // compute power required by the jammer to generate the appropriate amplitude
   //  determined by mimicking the false target's RCS at the blip location
   //  and computing the power which would be reflected back to the radar.
   // Even if the two-way interaction fails go ahead and get the scaling SNR.
   WsfEM_Interaction blipInteraction; // create new interaction so the passed in result is not modified
   if (blipInteraction.BeginTwoWayInteraction(radarXmtrPtr, tempPlatformPtr, radarRcvrPtr) != 0)
   {
      blipInteraction.ComputeUndefinedGeometry();
   }
   blipInteraction.SetTransmitterBeamPosition();
   blipInteraction.SetReceiverBeamPosition();

   // Set the radar cross section of the target.
   blipInteraction.mRadarSig = 1.0; // calculate wrt to a 1 m^2 target for scaling purposes.
   // Calculate the signal return.
   blipInteraction.ComputeRF_TwoWayPower(blipInteraction.mRadarSig);
   // Account for the receiver gains
   blipInteraction.mRcvdPower *= WsfRF_Jammer::ComputeReceiverGains(radarRcvrPtr);

   double scalingSNR = radarRcvrPtr->ComputeSignalToNoise(blipInteraction.mRcvdPower, 0.0, 0.0);
   if (scalingSNR > 0.0)
   {
      equivalentRCS = blipInteraction.mRadarSig * (aBlipInteraction.mSignalToNoise / scalingSNR);
   }
   return equivalentRCS;
}

// =================================================================================================
void WsfFalseTargetBlip::AddEquivalentRCS(RadarSnsrPair aIdKey, double aEquivalentRCS)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mEquivalentRCS = aEquivalentRCS;
}

// =================================================================================================
double WsfFalseTargetBlip::GetEquivalentRCS(RadarSnsrPair aIdKey)
{
   double equivalentRCS(0.0);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      equivalentRCS = (*bdi).second.mEquivalentRCS;
   }
   return equivalentRCS;
}

// =================================================================================================
void WsfFalseTargetBlip::AddRadarToBlipRange(RadarSnsrPair aIdKey, double aRadar2BlipRange)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mRadarToBlipRange = aRadar2BlipRange;
}

// =================================================================================================
double WsfFalseTargetBlip::GetRadarToBlipRange(RadarSnsrPair aIdKey)
{
   double range(0.0);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      range = (*bdi).second.mRadarToBlipRange;
   }
   return range;
}

// =================================================================================================
void WsfFalseTargetBlip::AddJammerToRadarRange(RadarSnsrPair aIdKey, double aJammer2RadarRange)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mDataMap[aIdKey].mJammerToRadarRange = aJammer2RadarRange;
}

// =================================================================================================
double WsfFalseTargetBlip::GetJammerToRadarRange(RadarSnsrPair aIdKey)
{
   double range(0.0);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      range = (*bdi).second.mJammerToRadarRange;
   }
   return range;
}

// =================================================================================================
double WsfFalseTargetBlip::GetReferenceJammerToRadarRange()
{
   double range(0.0);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.begin();
   for (; bdi != mDataMap.end(); ++bdi)
   {
      WsfPlatform* platPtr = mFtPtr->GetSimulation()->GetPlatformByName(bdi->first.first);
      if ((platPtr != nullptr) && (GetJammerXmtr() != nullptr))
      {
         double relLocWCS[3];
         double jamLocWCS[3];
         GetJammerXmtr()->GetAntenna()->GetLocationWCS(jamLocWCS);
         platPtr->GetComponent<WsfSensor>((*bdi).first.second)->GetRelativeLocationWCS(jamLocWCS, relLocWCS);
         range = UtVec3d::Magnitude(relLocWCS);
         AddJammerToRadarRange((*bdi).first, range);
         break;
      }
   }
   return range;
}

// =================================================================================================
double WsfFalseTargetBlip::GetMinimumActualSNR(const RadarSnsrVec& aRadarSensorVec)
{
   double minimumActualSNR = DBL_MAX;

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   for (const auto& irdrSnsrPair : aRadarSensorVec)
   {
      double tempActualSNR = GetActualSNR(irdrSnsrPair);
      if (tempActualSNR >= 0.0)
      {
         minimumActualSNR = (minimumActualSNR < tempActualSNR) ? minimumActualSNR : tempActualSNR;
      }
   }
   return minimumActualSNR;
}

// =================================================================================================
void WsfFalseTargetBlip::SetInteraction(RadarSnsrPair aIdKey, WsfEM_Interaction* aInteractionPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      delete (*bdi).second.mInteractionPtr;
   }
   mDataMap[aIdKey].mInteractionPtr = aInteractionPtr;
}

// =================================================================================================
WsfEM_Interaction* WsfFalseTargetBlip::GetInteraction(RadarSnsrPair aIdKey)
{
   WsfEM_Interaction* interactionPtr(nullptr);

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   BlipDataMap::iterator                 bdi = mDataMap.find(aIdKey);
   if (bdi != mDataMap.end())
   {
      interactionPtr = (*bdi).second.mInteractionPtr;
   }
   return interactionPtr;
}
