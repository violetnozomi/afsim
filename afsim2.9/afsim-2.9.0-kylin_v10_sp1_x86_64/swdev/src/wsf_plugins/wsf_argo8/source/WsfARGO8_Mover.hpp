// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFARGO_MOVER_HPP
#define WSFARGO_MOVER_HPP

class UtInput;
#include "UtEntity.hpp"
#include "UtPlugin.hpp"

#include "WsfMover.hpp"
#include "WsfWeaponEngagement.hpp"

#include "Argo8Missile.hpp"
#include "Argo8Structs.h"

class WsfARGO8_Mover : public WsfMover
{
   public:
      WsfARGO8_Mover(WsfScenario& aScenario);
      virtual ~WsfARGO8_Mover();
      WsfARGO8_Mover(const WsfARGO8_Mover& aSrc);

      virtual WsfMover* Clone() const;
      virtual bool Initialize(double aSimTime);
      virtual bool Initialize2(double aSimTime);
      virtual bool ProcessInput(UtInput& aInput);
      virtual void Update(double aSimTime);

      virtual WsfSpatialDomain GetSpatialDomain() const { return WSF_SPATIAL_DOMAIN_AIR; }
      virtual bool UseDeferredLaunching() const { return true; }
   private:
      void ContinueFlight(double aSimTime);
      void CoastFlight(double aSimTime);
      void UpdateMissile(double aSimTime);
      void UpdateARGO8(double aSimTime);
      void UpdateARGO8Coast(double aSimTime,
                            EngagementState aEngagementState);
      void SetRailData(bool aGuidanceMethod, double aSimTime);
      WsfPlatform * GetTargetPlatform();
      WsfPlatform * GetFiringPlatform();
      const WsfTrack * GetTargetTrack();
      void GetLocationESD(double aLocESD[3], WsfPlatform *aPlatformPtr);
      void GetLocationESD(double aLocESD[3], const WsfTrack *aTrackPtr, double aSimTime);
      void GetVelocityESD(double aVelESD[3], WsfPlatform *aPlatformPtr);
      void GetVelocityESD(double aLocESD[3], const WsfTrack *aTrackPtr);
      double GetEngagementDistance(double aSimTime,
                                   EngagementState aEngagementState);
      bool HasTrackUpdated(const WsfTrack* aTrackPtr);
      void TerminateMissile(double aSimTime,
                            int    aFuzeStateMask,
                            double aMissDistance);

      std::string  mMissileType;
      std::string  mMissileLibrary;
      bool         mGuidanceMethodTruth;
      bool         mSeekerMethodTruth;
      bool         mLoggingEnabled;
      std::string  mLogFilePath;
      Argo8Missile mMissile;
      UtEntity     mReference;
      EngagementState mEngagementState;
      double       mLastTrackLocation[3];
      double       mLaunchTime;
      double       mLastTrackUpdateTime;
      double       mEndgameGees;

      MissileState mMissileState;

      bool         mTerminationCompleted;
      bool         mSeekerOn;
      bool         mFuzeOutput;
      double       mMissileTimeStep;
      double       mGuidanceTime;
      double       mGuidanceStep;
      int          mMissileStep;
      double       mLastMissileUpdateTime;
      double       mLastGuidLoc[3];
      double       mLastGuidVel[3];
};

#endif
