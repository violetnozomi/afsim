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

#ifndef WSFLASERDESIGNATIONS_HPP
#define WSFLASERDESIGNATIONS_HPP

#include <vector>

class UtInput;
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
class WsfDisDesignator;
#include "wsf_mil_export.h"

#include "WsfEvent.hpp"
class WsfPlatform;
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"

class WSF_MIL_EXPORT WsfLaserDesignationsData
{
public:
   WsfLaserDesignationsData()                                     = default;
   WsfLaserDesignationsData(const WsfLaserDesignationsData& aSrc) = default;
   ~WsfLaserDesignationsData()                                    = default;

   void SetInputProcessed(bool aValue) { mProcessedInput = aValue; }
   bool InputIsProcessed() const { return mProcessedInput; }

   void SetDebug(bool aValue) { mDebug = aValue; }
   bool DebugEnabled() const { return mDebug; }

protected:
   bool         mDebug{false};
   bool         mProcessedInput{false};
   bool         mSendExternal{true};
   unsigned int mUpdatePrintModulus{157};
   double       mStaleOutInterval{5.0}; // Remove a designation that is stale.
   double       mUpdateInterval{3.0};
   double       mPlatformRadius{1.5}; // Relocate the laser spot to place in front of a lased body.
};

//! A singleton class to obtain information about Laser Designations that
//! exist in the Simulation, produced either locally, or received externally
//! via DIS connection.  Currently makes several simplifying assumptions:
//! Only one emitter per platform, one designation per emitter, and one
//! designation per target platform (no overspill or underspill).  As class
//! is fleshed out, the assumptions may be removed.  Also, the DIS dead
//! reckoning algorithm enumeration is not used, nor is the spot acceleration.

//! When a spot is placed in the environment, a flag is set to permit target
//! occlusion of the spot to be internally considered by this class or not.
//! A high-fidelity laser designator model may itself consider target
//! occlusion, with a detailed target facet model available for this purpose.
//! If so, the Spot it would place should have its SetAddLocalOcclusion(false).
//! Note that the default is true.  If true, the non-occulted "ground-lase"
//! WCS location is always set into the spot, and is available as
//! GetLocationWCS(), if there is no target platform association.  But if a
//! target entity is associated with the spot, the provided WCS laser spot
//! is relocated in target-relative coordinates if appropriate.  The relo-
//! cation is to place the spot on the surface of a sphere at radius R toward
//! the designator from the target center reference.  A target ECS-relative
//! offset is computed, using the relocated spot position.  Then the "true"
//! WCS spot location is obtainable via the GetTargetAdjustedLocationWCS()
//! function.  To prevent latency or coordinate frame mismatches from causing
//! a designation position error, when the designation is associated with an
//! entity, the local ECS offset should be used, rather than WCS.  The
//! GetTargetAdjustedLocationWCS() function does this automatically.
//! WHAT CONSITUTES A "RADIUS"? - For each target, if Length, Width, and Height
//! values are set in the platform, then R is computed as the cube root of
//! (L * L + W * W + H * H).  But if any of (L,W,H) are zero, we will use the
//! default radius mPlatformRadius provided to this class.

class WSF_MIL_EXPORT WsfLaserDesignations : public WsfSimulationExtension, public WsfLaserDesignationsData
{
public:
   class Spot
   {
   public:
      enum DeadReckoningAlgorithm
      {
         // Based upon DIS Enumerations, but does not introduce a dependency
         // upon its types.  See SISO-REF-010-2011.1 or later:
         cOTHER   = 0,
         cSTATIC  = 1,
         cDRM_FPW = 2,
         cDRM_RPW = 3,
         cDRM_RVW = 4,
         cDRM_FVW = 5,
         cDRM_FPB = 6,
         cDRM_RPB = 7,
         cDRM_RVB = 8,
         cDRM_FVB = 9
      };

      explicit Spot();
      Spot(const Spot& aSrc);
      ~Spot() = default;

      Spot& operator=(const Spot& aSrc) = delete; // Do not define, as assignment forbidden.

      void UpdateTo(const Spot* aSrcPtr, WsfLaserDesignations* aDesignationsPtr);
      void UpdateTo(const Spot& aSrc, WsfLaserDesignations* aDesignationsPtr);

      // This should not be called outside of the WsfLaserDesignations class.
      void LocalUpdate(WsfSimulation& aSimulation, const double aEmitterLocationWCS[3], double aPlatformRadius);

      bool IsNulled() { return mPower == 0.0; }
      void NullPower() { mPower = 0.0; }

      unsigned int GetUpdateCount() const { return mSpotUpdateCount; }
      double       GetSimTimeStamp() const { return mSimTimeStamp; }
      size_t       GetDesignatedIndex() const { return mDesignatedPlatIndex; }
      size_t       GetDesignatingIndex() const { return mDesignatingPlatIndex; }
      unsigned int GetCode() const { return mLaserCode; }
      unsigned int GetCodeName() const { return mLaserCode; }
      unsigned int GetDeadReckoningAlgorithm() const { return mDeadReckoningAlgorithm; }
      double       GetWavelength() const { return mWavelength; }
      double       GetFrequency() const;
      double       GetPower() const { return mPower; }
      void         GetLocationWCS(double aLocWCS[3]) const;
      void         GetTargetAdjustedLocationWCS(double aLocWCS[3], WsfLaserDesignations* aDesignationsPtr) const;
      void         GetOffsetECS(double aOffsetECS[3]) const;
      void         GetAcceleration(double aAccel[3]) const;
      double       GetOffsetDistSquared() const;
      bool         IsExternal() const { return mIsExternal; }
      bool         IsLocal() const { return !mIsExternal; }
      bool         AddLocalOcclusion() const { return mAddLocalOcclusion; }

      void SetSimTimeStamp(double aValue) { mSimTimeStamp = aValue; }
      void SetCode(unsigned int aCode) { mLaserCode = aCode; }
      void SetCodeName(unsigned int aCodeName) { mLaserCodeName = aCodeName; }
      void SetLocationWCS(const double aLocWCS[3]);
      void SetOffsetECS(const double aOffsetECS[3]);
      void SetAcceleration(const double aAccel[3]);
      void SetPower(double aValue) { mPower = aValue; }
      void SetWavelength(double aValue) { mWavelength = aValue; }
      void SetFrequency(double aValue);
      void SetAddLocalOcclusion(bool aValue) { mAddLocalOcclusion = aValue; }
      void ClearDesignatedPlatform();
      void SetDesignatedIndex(WsfSimulation& aSimulation, size_t aDesignatedIndex);
      void SetDesignatingIndex(size_t aDesignatingIndex) { mDesignatingPlatIndex = aDesignatingIndex; }
      void SetIsExternal(bool aValue) { mIsExternal = aValue; }
      void SetIsLocal(bool aValue) { mIsExternal = !aValue; }
      void SetDeadReckoningAlgorithm(DeadReckoningAlgorithm aValue) { mDeadReckoningAlgorithm = aValue; }

   private:
      void AddLocalOcclusion(WsfPlatform* aTargetPtr, double aPlatformRadius, const double aEmitterLocationWCS[3]);

      void ComputeECS_OffsetFrom(WsfPlatform* aTargetPtr, const double aLocWCS[3]);

      size_t       mDesignatingPlatIndex;
      size_t       mDesignatedPlatIndex;
      unsigned int mLaserCodeName;
      unsigned int mLaserCode;
      unsigned int mDeadReckoningAlgorithm;
      unsigned int mSpotUpdateCount;

      double mSimTimeStamp; // (Not in the DIS PDU)
      double mPower;
      double mWavelength;

      double mOffsetECS[3]; // Offset valid only if mDesignatedPlatIndex != 0
      double mLocationWCS[3];
      double mAcceleration[3]; // (In DIS PDU, but not set currently.)

      bool mIsExternal;        // (Not in the DIS PDU)
      bool mAddLocalOcclusion; // If true, apply a (crude) local occlusion model.
   };

   UtCallbackListN<void(const Spot&)> SpotUpdated;

   WsfLaserDesignations() = default;
   WsfLaserDesignations(WsfLaserDesignationsData& aSetupData);
   ~WsfLaserDesignations() override;

   void Update(double aSimTime);
   bool Initialize() override;
   void UpdateCallbacks();

   // A newly added Spot returns true.  This function may move the spot
   // to a location nearer the emitter it is determined that the spot
   // is occluded by the target.  (The target is assumed a spherical ball.)
   bool AddOrUpdate(const Spot& aDesignator, const double aEmitterLocationWCS[3]);

   // Returns true if a Spot was removed.
   bool Remove(const Spot& aDesignator);

   // ======== Accessors =============
   // These references to existing spots are not guaranteed
   // to be persistent, and should be used immediately.

   void GetAllSpots(std::vector<const Spot*>& aSpotPtrs);

   void GetSpecified(unsigned int aLaserCode, double aWaveLength, std::vector<const Spot*>& aSpotPtrs);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatPtr);


   class UpdateEvent : public WsfEvent
   {
   public:
      UpdateEvent() = default;
      UpdateEvent(double aUpdateInterval, WsfLaserDesignations* aDesignationsPtr);
      EventDisposition Execute() override;

   private:
      WsfLaserDesignations* mDesignationsPtr;
      double                mUpdateInterval;
   };

private:
   void RemoveP(std::list<Spot>::iterator aDesignator, std::list<Spot>::iterator& aResultIter);


   void Register();

   bool FindDesignationOf(size_t aDesignatedPlatIndex, std::list<Spot>::iterator& aIter);

   bool FindDesignationFrom(size_t aDesignatingPlatIndex, std::list<Spot>::iterator& aIter);

   bool   mIsInitialized = false;
   bool   mIsRegistered  = false;
   double mLastUpdate    = -100.0;

   std::list<Spot>  mSpots     = std::list<Spot>();
   UtCallbackHolder mCallbacks = UtCallbackHolder();
};

#endif
