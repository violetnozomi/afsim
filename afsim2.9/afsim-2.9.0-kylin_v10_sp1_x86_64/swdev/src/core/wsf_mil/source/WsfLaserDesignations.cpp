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

#include "WsfLaserDesignations.hpp"

#include <assert.h>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"

namespace
{
const double cLARGE_DISTANCE = 9999.0;
const double cZERO[]         = {0.0, 0.0, 0.0};
const double cHUGE_OFFSET[]  = {cLARGE_DISTANCE, cLARGE_DISTANCE, cLARGE_DISTANCE};
} // namespace

// ============================================================================
WsfLaserDesignations::WsfLaserDesignations(WsfLaserDesignationsData& aSetupData)
   : WsfLaserDesignationsData(aSetupData)
   , mIsInitialized(false)
   , mIsRegistered(false)
   , mLastUpdate(-100.0)
   , mSpots()
   , mCallbacks()
{
   Initialize();
}

// ============================================================================
WsfLaserDesignations::~WsfLaserDesignations()
{
   mCallbacks.Clear();
   mSpots.clear();
}

// ============================================================================
//! Function to refresh the laser designations, including getting rid of
//! stale/orphaned ones.  (Function is internally event-queued, so does not need
//! explicitly called by the user.)
//! @param aSimTime Current simulation time.
void WsfLaserDesignations::Update(double aSimTime)
{
   // Traverse all spots and remove any that have not been updated in <mStaleOutInterval>.
   double staleTime = aSimTime - mStaleOutInterval;

   std::list<Spot>::iterator iter = mSpots.begin();
   while (iter != mSpots.end())
   {
      if (iter->GetSimTimeStamp() < staleTime)
      {
         if (mDebug)
         {
            std::string qualifier = "internal ";
            if (iter->IsExternal())
            {
               qualifier = "external ";
            }
            auto out = ut::log::debug() << "Removing a stale " << qualifier << "laser spot.";
            out.AddNote() << "T = " << aSimTime;
         }
         iter = mSpots.erase(iter);
      }
      else
      {
         ++iter;
      }
   }

   mLastUpdate = aSimTime;
}

// ============================================================================
//! Function to refresh a particular laser designation.
//! @param aDesignation Laser designation to update.  (The designation itself will contain a time stamp.)
//! @param aEmitterLocationWCS Location of the designator.  Is used to discern LOS of sensor.
//! @returns true if the designation had to be created and added.
bool WsfLaserDesignations::AddOrUpdate(const Spot& aDesignation, const double aEmitterLocationWCS[3])
{
   // Do not accept a designation from "unknown" entity.
   size_t designatingIndex = aDesignation.GetDesignatingIndex();
   if (designatingIndex == 0)
   {
      return false;
   }

   // size_t designatedIndex = aDesignation.GetDesignatedIndex();
   // WsfPlatform* tgtPtr = 0;
   // if (designatedIndex != 0)
   //{
   //    tgtPtr = GetSimulation().GetPlatformByIndex(designatedIndex);
   // }

   bool updated = false;
   bool added   = false;

   std::list<Spot>::iterator iter = mSpots.begin();
   if (FindDesignationFrom(designatingIndex, iter))
   {
      iter->UpdateTo(aDesignation, this);

      if (iter->IsLocal())
      {
         iter->LocalUpdate(GetSimulation(), aEmitterLocationWCS, mPlatformRadius);
      }

      if (mDebug && (iter->GetUpdateCount() % mUpdatePrintModulus == 0))
      {
         std::string qualifier = "internal ";
         if (iter->IsExternal())
         {
            qualifier = "external ";
         }
         double simTime = GetSimulation().GetSimTime();
         auto   out     = ut::log::debug() << "Updating pre-existing " << qualifier << "laser spot.";
         out.AddNote() << "T = " << simTime;
      }
      updated = true;
   }

   if (!updated)
   {
      // Did not find this, so it must be a new designation.  Add it...
      mSpots.push_back(aDesignation);
      UpdateCallbacks();
      added = FindDesignationFrom(aDesignation.GetDesignatingIndex(), iter);
      assert(added);

      if (iter->IsLocal())
      {
         iter->LocalUpdate(GetSimulation(), aEmitterLocationWCS, mPlatformRadius);
      }

      if (mDebug)
      {
         std::string qualifier = "internal ";
         if (iter->IsExternal())
         {
            qualifier = "external ";
         }
         double simTime = GetSimulation().GetSimTime();
         auto   out     = ut::log::debug() << "Adding a new " << qualifier << " laser spot.";
         out.AddNote() << "T = " << simTime;
      }
   }

   if ((added || updated) && mSendExternal && aDesignation.IsLocal())
   {
      // This is an internal WSF designation change, notify subscribers
      // (if any) so the rest of the world (i.e. DIS) can react to it.
      SpotUpdated(*iter);

      // If we have not done it yet, register for periodic Update events, and PlatformDeleted;
      if (added && !mIsRegistered)
      {
         Register();
      }
   }

   return added;
}

// ============================================================================
// private
void WsfLaserDesignations::RemoveP(std::list<Spot>::iterator aDesignator, std::list<Spot>::iterator& aResultIter)
{
   // DIS requires that a Designation PDU be sent out with zero
   // power to tell external players that the Designation is removed.
   // If the power is already zero, assume the "Zero" send has been done.

   if (aDesignator->GetPower() > 0.0)
   {
      aDesignator->SetPower(0.0);
      SpotUpdated(*aDesignator);
   }

   aResultIter = mSpots.erase(aDesignator);
}

// ============================================================================
//! Function to remove a particular laser designation.
//! @param aDesignation Laser designation to remove.
//! @returns true if the designation was indeed removed.
bool WsfLaserDesignations::Remove(const Spot& aDesignation)
{
   // "Remove" commands will always be for only local designations.
   assert(!aDesignation.IsExternal());

   std::list<Spot>::iterator iter = mSpots.begin();
   if (FindDesignationFrom(aDesignation.GetDesignatingIndex(), iter))
   {
      if (mDebug)
      {
         std::string qualifier = "internal ";
         if (aDesignation.IsExternal())
         {
            qualifier = "external ";
         }
         double simTime = GetSimulation().GetSimTime();
         auto   out     = ut::log::debug() << "Removing a " << qualifier << "laser spot upon request.";
         out.AddNote() << "T = " << simTime;
      }

      std::list<Spot>::iterator resultIter;
      RemoveP(iter, resultIter);
      return true;
   }
   return false;
}

// ============================================================================
//! Function to allow users to get a laser designations matching certain attributes:
//! @param aDesignatedPlatIndex Seeking a designation which is associated with the
//!        platform of this index.
//! @returns true if the iterator is valid upon return.
bool WsfLaserDesignations::FindDesignationOf(size_t aDesignatedPlatIndex, std::list<Spot>::iterator& aIter)
{
   std::list<Spot>::iterator iter = mSpots.begin();
   while (iter != mSpots.end())
   {
      if (iter->GetDesignatedIndex() == aDesignatedPlatIndex)
      {
         if (GetSimulation().PlatformExists(aDesignatedPlatIndex))
         {
            aIter = iter;
            return true;
         }
      }
      ++iter;
   }
   return false;
}

// ============================================================================
//! Function to allow users to get a laser designations matching certain attributes:
//! @param aDesignatingPlatIndex Seeking a designation which is produced by the
//!        platform of this index.
//! @returns true if the iterator is valid upon return.
bool WsfLaserDesignations::FindDesignationFrom(size_t aDesignatingPlatIndex, std::list<Spot>::iterator& aIter)
{
   std::list<Spot>::iterator iter = mSpots.begin();
   while (iter != mSpots.end())
   {
      if (iter->GetDesignatingIndex() == aDesignatingPlatIndex)
      {
         if (GetSimulation().PlatformExists(aDesignatingPlatIndex))
         {
            aIter = iter;
            return true;
         }
      }
      ++iter;
   }
   return false;
}

// ============================================================================
void WsfLaserDesignations::UpdateCallbacks()
{
   if (mCallbacks.Empty() != mSpots.empty())
   {
      if (mCallbacks.Empty())
      {
         mCallbacks.Add(
            WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfLaserDesignations::PlatformDeleted, this));
      }
      else
      {
         mCallbacks.Clear();
      }
   }
}

// ============================================================================
void WsfLaserDesignations::Register()
{
   if (!mIsRegistered)
   {
      UpdateCallbacks();
      GetSimulation().AddEvent(ut::make_unique<UpdateEvent>(mUpdateInterval, this));

      mIsRegistered = true;
   }
}

// ============================================================================
//! Function to inform the object of simulation platform deletions.  Called by the
//! simulation, users need not call it explicitly.
//! @param aSimTime Current simulation time.
//! @param aPlatPtr Pointer to the platform to be deleted.
void WsfLaserDesignations::PlatformDeleted(double aSimTime, WsfPlatform* aPlatPtr)
{
   size_t index = aPlatPtr->GetIndex();

   std::list<Spot>::iterator spotIter = mSpots.begin();
   while (spotIter != mSpots.end())
   {
      if (spotIter->GetDesignatingIndex() == index)
      {
         // If the entity that produced the spot is destroyed, remove the designation.
         std::list<Spot>::iterator resultIter;
         RemoveP(spotIter, resultIter);
         spotIter = resultIter;
      }
      else if (spotIter->GetDesignatedIndex() == index)
      {
         // If the entity that is lased by the spot is destroyed, reset the "designated" index.
         spotIter->SetDesignatedIndex(GetSimulation(), 0);
         ++spotIter;
      }
      else
      {
         ++spotIter;
      }
   }
   // Remove callback if it's no longer needed
   if (mSpots.empty())
   {
      mCallbacks.Clear();
   }
}

// ============================================================================
//! Function to Initialize the laser designations.  (Function is internally
//! callback-queued, so does not need explicitly called by the user.)
//! @returns true if successful.
bool WsfLaserDesignations::Initialize()
{
   bool success = true;
   if (!mProcessedInput)
   {
      // This warning would be of value, but decided to remove it, as even
      // a blank input file caused it to be shown.  Decided it was too intrusive.
      // auto logger = ut::log::warning() << "The 'laser_designations' block was not specified.";
      // logger.AddNote() << "Default settings will be used.";
      mProcessedInput = true;
   }

   return success;
}

// ====================== UpdateEvent UpdateEvent UpdateEvent =================
// ============================================================================
WsfLaserDesignations::UpdateEvent::UpdateEvent(double aUpdateInterval, WsfLaserDesignations* aDesignationsPtr)
   : mDesignationsPtr(aDesignationsPtr)
   , mUpdateInterval(aUpdateInterval)
{
}

// ============================================================================
WsfEvent::EventDisposition WsfLaserDesignations::UpdateEvent::Execute()
{
   mDesignationsPtr->Update(GetTime());
   SetTime(GetTime() + mUpdateInterval);
   return WsfEvent::cRESCHEDULE;
}

// ==============================================================================
void WsfLaserDesignations::GetAllSpots(std::vector<const Spot*>& aSpotPtrs)
{
   aSpotPtrs.clear();
   std::list<Spot>::iterator it = mSpots.begin();
   while (it != mSpots.end())
   {
      aSpotPtrs.push_back(&(*it));
      ++it;
   }
}

// ==============================================================================
void WsfLaserDesignations::GetSpecified(unsigned int aLaserCode, double aWaveLength, std::vector<const Spot*>& aSpotPtrs)
{
   // Return all the spots in the simulation that currently contain this Code and Wavelength.

   aSpotPtrs.clear();
   std::list<Spot>::iterator it = mSpots.begin();
   while (it != mSpots.end())
   {
      if ((it->GetCode() == aLaserCode) && (fabs(it->GetWavelength() - aWaveLength) < 0.0001))
      {
         aSpotPtrs.push_back(&(*it));
      }
      ++it;
   }
}

// ====================== Spot Spot Spot ======================================
// ============================================================================
WsfLaserDesignations::Spot::Spot()
   : mDesignatingPlatIndex(0)
   , mDesignatedPlatIndex(0)
   , mLaserCodeName(0)
   , mLaserCode(0)
   , mDeadReckoningAlgorithm(cSTATIC) // TO-DO - Appropriate default?
   , mSpotUpdateCount(0)
   , mSimTimeStamp(0.0)
   , mPower(0.0)
   , mWavelength(0.0)
   //,mOffsetECS(),
   //,mLocationWCS(),
   //,mAcceleration()
   , mIsExternal(false)
   , mAddLocalOcclusion(true)
{
   UtVec3d::Set(mOffsetECS, cHUGE_OFFSET);
   UtVec3d::Set(mLocationWCS, cZERO);
   UtVec3d::Set(mAcceleration, cZERO);
}

// ============================================================================
WsfLaserDesignations::Spot::Spot(const Spot& aSrc)
   : mDesignatingPlatIndex(aSrc.mDesignatingPlatIndex)
   , mDesignatedPlatIndex(aSrc.mDesignatedPlatIndex)
   , mLaserCodeName(aSrc.mLaserCodeName)
   , mLaserCode(aSrc.mLaserCode)
   , mDeadReckoningAlgorithm(aSrc.mDeadReckoningAlgorithm)
   , mSpotUpdateCount(aSrc.mSpotUpdateCount)
   , mSimTimeStamp(aSrc.mSimTimeStamp)
   , mPower(aSrc.mPower)
   , mWavelength(aSrc.mWavelength)
   //,mOffsetECS()
   //,mLocationWCS()
   //,mAcceleration()
   , mIsExternal(aSrc.mIsExternal)
   , mAddLocalOcclusion(aSrc.mAddLocalOcclusion)
{
   UtVec3d::Set(mOffsetECS, aSrc.mOffsetECS);
   UtVec3d::Set(mLocationWCS, aSrc.mLocationWCS);
   UtVec3d::Set(mAcceleration, aSrc.mAcceleration);
}

// ============================================================================
void WsfLaserDesignations::Spot::SetFrequency(double aFrequency)
{
   mWavelength = UtMath::cLIGHT_SPEED / aFrequency;
}

// ============================================================================
double WsfLaserDesignations::Spot::GetFrequency() const
{
   return UtMath::cLIGHT_SPEED / mWavelength;
}

// ============================================================================
void WsfLaserDesignations::Spot::UpdateTo(const Spot& aSrc, WsfLaserDesignations* aDesignationsPtr)
{
   // Update the attributes of this spot to match that provided:

   // Unitary attributes first
   mDesignatedPlatIndex    = aSrc.mDesignatedPlatIndex;
   mDesignatingPlatIndex   = aSrc.mDesignatingPlatIndex;
   mLaserCode              = aSrc.mLaserCode;
   mLaserCodeName          = aSrc.mLaserCodeName;
   mDeadReckoningAlgorithm = aSrc.mDeadReckoningAlgorithm;
   mPower                  = aSrc.mPower;
   mWavelength             = aSrc.mWavelength;
   mIsExternal             = aSrc.mIsExternal;

   // Here we assume that if the spot is external to WSF,
   // it does not have a valid timestamp, and needs one,
   // otherwise, the local producer has already timestamped it.
   if (mIsExternal)
   {
      mSimTimeStamp = aDesignationsPtr->GetSimulation().GetSimTime();
   }
   else
   {
      mSimTimeStamp = aSrc.mSimTimeStamp;
   }

   // Now vector attributes:
   UtVec3d::Set(mOffsetECS, aSrc.mOffsetECS);
   UtVec3d::Set(mLocationWCS, aSrc.mLocationWCS);
   UtVec3d::Set(mAcceleration, aSrc.mAcceleration);

   ++mSpotUpdateCount;
}

// ============================================================================
void WsfLaserDesignations::Spot::UpdateTo(const Spot* aSrcPtr, WsfLaserDesignations* aDesignationsPtr)
{
   UpdateTo(*aSrcPtr, aDesignationsPtr);
}

// ============================================================================
void WsfLaserDesignations::Spot::GetLocationWCS(double aLocWCS[3]) const
{
   UtVec3d::Set(aLocWCS, mLocationWCS);
}

// ============================================================================
void WsfLaserDesignations::Spot::GetTargetAdjustedLocationWCS(double aLocWCS[3], WsfLaserDesignations* aDesignationsPtr) const
{
   // This function is critical to prevent latency.  Place the spot at its stated WCS
   // location, _unless_ it is associated with a particular target platform index that
   // we recognize.  If so, return the WCS location of the spot after an adjustment to
   // match the stated ECS offset from the target.
   UtVec3d::Set(aLocWCS, mLocationWCS);
   if (mDesignatedPlatIndex != 0)
   {
      WsfPlatform* tgtPtr = aDesignationsPtr->GetSimulation().GetPlatformByIndex(mDesignatedPlatIndex);
      if (tgtPtr != nullptr)
      {
         assert(mOffsetECS[0] != cLARGE_DISTANCE);
         assert(mOffsetECS[1] != cLARGE_DISTANCE);
         assert(mOffsetECS[2] != cLARGE_DISTANCE);

         double tgtLocWCS[3];
         tgtPtr->GetLocationWCS(tgtLocWCS);
         double deltaWCS[3];
         tgtPtr->ConvertECSVectorToWCS(deltaWCS, mOffsetECS);
         UtVec3d::Add(aLocWCS, deltaWCS, tgtLocWCS);
      }
   }
}

// ============================================================================
void WsfLaserDesignations::Spot::GetOffsetECS(double aOffsetECS[3]) const
{
   UtVec3d::Set(aOffsetECS, mOffsetECS);
}

// ============================================================================
void WsfLaserDesignations::Spot::GetAcceleration(double aAccel[3]) const
{
   UtVec3d::Set(aAccel, mAcceleration);
}

// ============================================================================
void WsfLaserDesignations::Spot::SetLocationWCS(const double aLocWCS[3])
{
   UtVec3d::Set(mLocationWCS, aLocWCS);
}

// ============================================================================
void WsfLaserDesignations::Spot::SetOffsetECS(const double aOffsetECS[3])
{
   UtVec3d::Set(mOffsetECS, aOffsetECS);
}

// ============================================================================
void WsfLaserDesignations::Spot::SetAcceleration(const double aAcceleration[3])
{
   UtVec3d::Set(mAcceleration, aAcceleration);
}

void WsfLaserDesignations::Spot::SetDesignatedIndex(WsfSimulation& aSimulation, size_t aDesignatedIndex)
{
   mDesignatedPlatIndex = aDesignatedIndex;
   if (mDesignatedPlatIndex == 0)
   {
      ClearDesignatedPlatform();
   }
   else
   {
      // This does not do an occulsion calculation, but will on next Update();
      WsfPlatform* tgtPtr = aSimulation.GetPlatformByIndex(mDesignatedPlatIndex);
      if (tgtPtr != nullptr)
      {
         ComputeECS_OffsetFrom(tgtPtr, mLocationWCS);
      }
   }
}

// ============================================================================
void WsfLaserDesignations::Spot::ClearDesignatedPlatform()
{
   mDesignatedPlatIndex = 0;
   UtVec3d::Set(mOffsetECS, cHUGE_OFFSET);
}

// ============================================================================
//! Function to refresh a particular laser designation, and consider target occlusion.
//! @param aSimulation The simulation object owning the platforms.
//! @param aEmitterLocationWCS Location of the designator.  Is used to discern LOS of sensor.
//! @param aPlatformRadius The radius assumed for the target volume.  This is used to approximate
//! the target (if no facets are available) to see where the spot impinges on the target geometry.
//! This function will move the target spot location into the ECS frame if the spot actually touches it.
void WsfLaserDesignations::Spot::LocalUpdate(WsfSimulation& aSimulation,
                                             const double   aEmitterLocationWCS[3],
                                             double         aPlatformRadius)
{
   WsfPlatform* tgtPtr = aSimulation.GetPlatformByIndex(mDesignatedPlatIndex);

   if (mAddLocalOcclusion && (tgtPtr != nullptr))
   {
      // Now if this designation has an associated target index, check for target occluding.
      // If true, this will involve relocating the spot to impinge on a front facet of the target.
      // The impingement point is very crudely placed on a sphere in front of the model reference
      // location, because here we have no knowledge of target geometry.

      // Use a default radius ("one size fits all") unless platform dimensions are given.
      double targetRadius = aPlatformRadius;
      double tgtVolume    = tgtPtr->GetLength() * tgtPtr->GetWidth() * tgtPtr->GetHeight();
      // Now compute an effective spherical radius that contains the same volume as the target box.
      // Since for a sphere V = 4/3 Pi R^3, then solve for R = cubeRoot( 3 V / (4Pi))
      if (tgtVolume > 0.0)
      {
         targetRadius = pow(0.75 * tgtVolume / UtMath::cPI, 1.0 / 3.0);
      }

      AddLocalOcclusion(tgtPtr, targetRadius, aEmitterLocationWCS);
   }
}

// ============================================================================
//! Function to refresh a particular laser designation, and consider target occlusion.
//! This function will move the target spot location in WCS and ECS frames if the spot touches the target.
//! @param aTargetPtr Target to be considered for occlusion.
//! @param aPlatformRadius The radius assumed for the target volume.  This is used to approximate
//! @param aEmitterLocationWCS Location of the designator.  Is used to discern LOS of sensor.
void WsfLaserDesignations::Spot::AddLocalOcclusion(WsfPlatform* aTargetPtr,
                                                   double       aPlatformRadius,
                                                   const double aEmitterLocationWCS[3])
{
   if (aTargetPtr == nullptr)
   {
      ClearDesignatedPlatform();
   }
   else
   {
      // Temporary spot location variable;
      double spotLocWCS[3];
      UtVec3d::Set(spotLocWCS, mLocationWCS);

      mDesignatedPlatIndex = aTargetPtr->GetIndex();

      // First consider the emitter line-of-sight vector, emitter to spot.
      double beamUnitLOS_WCS[3];
      UtVec3d::Subtract(beamUnitLOS_WCS, spotLocWCS, aEmitterLocationWCS);
      /*double fullBeamLength =*/UtVec3d::Normalize(beamUnitLOS_WCS);

      // Second, consider the target vector, emitter to target center.
      double targetLocWCS[3];
      aTargetPtr->Update(mSimTimeStamp);
      aTargetPtr->GetLocationWCS(targetLocWCS);

      // The formula for where a ray intersects a line reduces down to
      // a quadratic that can be solved with quadratic formula.

      // Given:  A ray with point of origin o, and direction vector d.
      // Given:  A sphere with center at c, and radius r.
      // In our problem
      //    o is aEmitterLocationWCS,
      //    d is beamUnitLOS_WCS,
      //    c is targetLocWCS,
      //    r is aPlatformRadius.
      // (d * d) t^2 + 2 * (o - c) * d * t + (o - c) * (o - c) - r^2 = 0
      //    or the more familiar At^2 + Bt + C = 0, so we substitute:
      // A = d * d
      // B = 2(o - c) * d
      // C = (o - c) * (o - c) - r^2

      double A = UtVec3d::DotProduct(beamUnitLOS_WCS, beamUnitLOS_WCS);
      double omc[3];
      UtVec3d::Subtract(omc, aEmitterLocationWCS, targetLocWCS);
      double tomc[3];
      UtVec3d::Multiply(tomc, omc, 2.0);
      double B = UtVec3d::DotProduct(tomc, beamUnitLOS_WCS);
      double C = UtVec3d::DotProduct(omc, omc) - aPlatformRadius * aPlatformRadius;

      // Now just use the standard formula:
      double radical = B * B - 4.0 * A * C;
      if (radical >= 0.0)
      {
         // Positive real roots:
         double root = sqrt(radical);
         // Here we use the shortest beam length, which means the smallest value for t
         double t = (-B - root) / (2.0 * A);
         double newWCS[3];
         UtVec3d::Multiply(newWCS, beamUnitLOS_WCS, t);
         UtVec3d::Add(newWCS, newWCS, aEmitterLocationWCS);
         UtVec3d::Set(spotLocWCS, newWCS);
      }

      // We placed a new or updated designation, compute a target-relative offset,
      // so that when this spot is exported to another simulation, there will be
      // a way to deal with latency issues.
      ComputeECS_OffsetFrom(aTargetPtr, spotLocWCS);
   }
}

// ============================================================================
void WsfLaserDesignations::Spot::ComputeECS_OffsetFrom(WsfPlatform* aTargetPtr, const double aLocWCS[3])
{
   // Take the WCS location of the beam, and express it in target-ECS relative offset.
   aTargetPtr->ConvertWCSToECS(aLocWCS, mOffsetECS);
}

// ========================================================================
double WsfLaserDesignations::Spot::GetOffsetDistSquared() const
{
   return UtVec3d::MagnitudeSquared(mOffsetECS);
}


class WSF_MIL_EXPORT WsfLaserDesignationsSetup : public WsfScenarioExtension, public WsfLaserDesignationsData
{
public:
   bool ProcessSubInput(UtInput& aInput)
   {
      std::string command   = aInput.GetCommand();
      bool        myCommand = true;
      if (command == "debug_laser_designations")
      {
         mDebug          = true;
         mProcessedInput = true;
      }
      else if (command == "debug_print_modulus")
      {
         int value;
         aInput.ReadValue(value);
         if (value > 0)
         {
            mUpdatePrintModulus = static_cast<unsigned int>(value);
         }
         mProcessedInput = true;
      }
      else if (command == "send_external")
      {
         mSendExternal = true;
      }
      else if (command == "do_not_send_external")
      {
         mSendExternal = false;
      }
      else if (command == "stale_out_interval")
      {
         aInput.ReadValueOfType(mStaleOutInterval, UtInput::cTIME);
         mProcessedInput = true;
      }
      else if (command == "update_interval")
      {
         aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
         mProcessedInput = true;
      }
      else if (command == "platform_radius")
      {
         aInput.ReadValueOfType(mPlatformRadius, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(mPlatformRadius, 0.0);
      }
      else
      {
         myCommand = false;
      }
      return myCommand;
   }

   // ============================================================================
   //! Function to parse the input stream to capture commands of interest.
   //! @returns true if successful.
   bool ProcessInput(UtInput& aInput) override
   {
      if (aInput.GetCommand() == "laser_designations")
      {
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            UtInput& input = inputBlock.GetInput();
            if (!ProcessSubInput(input))
            {
               throw UtInput::UnknownCommand(input);
            }
         }
         return true;
      }
      return false;
   }

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfLaserDesignations>(*this));
   }
};
//
// class WsfLaserDesignationsExtension : public WsfApplicationExtension
// {
//    WsfLaserDesignationsExtension(WsfApplication& aApplication)
//       : WsfApplicationExtension(aApplication)
//    {
//    }
//
//    virtual void ScenarioCreated(WsfScenario& aScenario)
//    {
//       aScenario.RegisterExtension(new WsfLaserDesignationsSetup(aScenario));
//    }
// };

void WSF_MIL_EXPORT Register_laser_designations(WsfApplication& aApplication)
{
   aApplication.RegisterExtension("laser_designations",
                                  ut::make_unique<WsfDefaultApplicationExtension<WsfLaserDesignationsSetup>>());
}
