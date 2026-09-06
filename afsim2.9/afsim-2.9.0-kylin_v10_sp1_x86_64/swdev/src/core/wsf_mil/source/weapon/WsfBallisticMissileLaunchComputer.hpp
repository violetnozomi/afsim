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

#ifndef WSFBALLISTICMISSILELAUNCHCOMPUTER_HPP
#define WSFBALLISTICMISSILELAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <map>
#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "UtCallbackHolder.hpp"
#include "UtEntity.hpp"
#include "UtLog.hpp"

class WsfAero;
class WsfDraw;
#include "WsfEvent.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulationExtension.hpp"

class WsfBallisticMissileLaunchComputerSharedData;

//! A launch computer for ballistic missiles.
//! This launch computer is intended to be used for computing intercept solutions for
//! a ballistic missile interceptor versus a ballistic missile target.
class WSF_MIL_EXPORT WsfBallisticMissileLaunchComputer : public WsfLaunchComputer
{
protected:
   class Target;

public:
   using TargetMap = std::map<size_t, WsfMover::KinematicModel*>;

   class SA_LaunchWindow
   {
   public:
      double mFirstLaunchTime;
      double mFirstInterceptTime;
      double mFirstInterceptLocWCS[3];
      double mLastLaunchTime;
      double mLastInterceptTime;
      double mLastInterceptLocWCS[3];
   };

   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_BALLISTIC_MISSILE_LAUNCH_COMPUTER"; }

   WsfBallisticMissileLaunchComputer();
   WsfBallisticMissileLaunchComputer(const WsfBallisticMissileLaunchComputer& aSrc);
   WsfBallisticMissileLaunchComputer& operator=(const WsfBallisticMissileLaunchComputer&) = delete;
   ~WsfBallisticMissileLaunchComputer() override;

   WsfLaunchComputer* Clone() const override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;
   bool               ProcessInput(UtInput& aInput) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   virtual double ComputeLateralTargetOffset(double aSimTime, const WsfTrack& aTrack);

   double GetTargetApogeeTime() const;
   void   GetTargetApogeeLocationWCS(double aLocWCS[3]) const;

   //! Indicate the weapon 'end point and time' should be computed on the next EstimatedTimeToIntercept call.
   //! The 'end point and time' is an estimate of where the weapon/interceptor would hit the ground if
   //! it continued on a ballistic trajectory after missing the target.
   //! @note This only applies to the next call to EstimatedTimeToIntercept. It must be called for each call
   //! where this data is required, or the input value 'compute_end_point' must be specified as true.
   void ComputeEndPoint() { mComputeEndPoint = true; }

   //! @name Special methods for determination of launch windows.
   //! Calling 'ComputeLaunchWindows' enables these calculations.
   //! @note Computing launch windows is compute intensive.
   //@{

   void         ComputeLaunchWindows() { mComputeLaunchWindows = true; }
   unsigned int GetLaunchWindowCount() const { return static_cast<unsigned int>(mLaunchWindows.size()); }
   double       GetFirstLaunchTime(unsigned int aIndex) const;
   double       GetFirstInterceptTime(unsigned int aIndex) const;
   void         GetFirstInterceptLocationWCS(unsigned int aIndex, double aLocWCS[3]) const;
   double       GetLastLaunchTime(unsigned int aIndex) const;
   double       GetLastInterceptTime(unsigned int aIndex) const;
   void         GetLastInterceptLocationWCS(unsigned int aIndex, double aLocWCS[3]) const;
   //@}

   // The following function was written specifically to get access to the launch windows
   // encapsulated within, for the purposes of automating multiple timed SA defensive launches
   // in response to a given threat SS BM launch:
   bool GetPrecomputedLaunchWindowsFor(double aSimTime, unsigned int aThreatIndex, std::vector<SA_LaunchWindow>& aWindows);

   void QueuedSolutionPurge(double aSimTime, size_t aTargetIndex, const std::string& aCaller);

protected:
   //! A class encapsulating a simple kinematic model of the ballistic target.
   //! It is used to predict ahead in order to compute an intercept point or time.
   class BallisticModel : public WsfMover::KinematicModel
   {
   public:
      BallisticModel() = delete;
      BallisticModel(WsfBallisticMissileLaunchComputer* aParentPtr,
                     double                             aSimTime,
                     WsfStringId                        aModelTypeId,
                     const double                       aLocationWCS[3],
                     const double                       aVelocityWCS[3],
                     double                             aIntegrationInterval,
                     WsfDraw*                           aDrawPtr = nullptr); // If we want to draw the trajectory.
      ~BallisticModel() override;

      bool GetLocationXYZ(double aTime, double aLocXYZ[3]) const override;

      bool GetVelocityXYZ(double aTime, double aVelXYZ[3]) const override;

   private:
      struct Point
      {
         double mLocXYZ[3]; // Spherical Earth
         double mVelXYZ[3]; // Spherical Earth
      };

      double Update();

      WsfPlatform* mStatePtr;
      double       mMass;
      WsfAero*     mAeroPtr;
      double       mIntegrationInterval;

      // The following are Ellipsoidal Earth values, but integration is done is done in the spherical
      // frame to correspond with the frame used by WsfGuidedMover. Results are converted to ellipsoidal
      // earth values for external use.
      double mIntegLocWCS[3];
      double mIntegVelWCS[3];
      double mIntegWCS_ToNED_Transform[3][3];

      //! The points that define the trajectory.
      std::vector<Point> mPoints;
   };

   double LastEvaluatedGroundRange() const { return mLastEvaluatedGroundRange; }

   class PurgeSolutionEvent : public WsfEvent
   {
   public:
      PurgeSolutionEvent(double                             aSimTime,
                         WsfBallisticMissileLaunchComputer* aLC_Ptr,
                         size_t                             aTargetIndex,
                         const std::string&                 aCaller);
      WsfEvent::EventDisposition Execute() override;

   private:
      size_t                             mPlatformIndex;
      WsfBallisticMissileLaunchComputer* mLC_Ptr;
      size_t                             mTargetIndex;
      std::string                        mCaller;
   };

   // Intentionally does not set the corresponding validity flag.
   void SetLauncherBearingOnly(double aValue) { mLauncherBearing = aValue; }

   //! @name Temporary classes
   //! The following are temporary classes used during the creation of the surface-to-air tables.
   //@{
   //! A temporary class for holding an intersection point.
   class TempPoint
   {
   public:
      double mAltitude;
      double mTimeOfFlight;
      double mLoftAngle;
      double mSpeed;
      double mFlightPathAngle;
   };

   //! A temporary form of 'RangeLine' used during creation of the final tables.
   using TempRangeLine = std::vector<TempPoint>;

   //! A temporary form of 'RangeEntry' used during creation of the final tables.
   class TempRangeEntry
   {
   public:
      TempRangeLine mIncreasingLine;
      TempRangeLine mDecreasingLine;
   };
   //@}

   //! Time of flight and loft angle as a function of altitude.
   //! Each line of constant ground range will have two instances of this:
   //!
   //! - One where increasing loft angle causes an increase in altitude.
   //! - One where increasing loft angle causes a decrease in altitude.
   //!
   //! In either case the curve may be empty.
   class RangeLine
   {
   public:
      TblIndVarU<double> mAltitude;
      TblDepVar1<double> mTimeOfFlight;
      TblDepVar1<double> mLoftAngle;
      TblDepVar1<double> mSpeed;
      TblDepVar1<double> mFlightPathAngle;

      double mMinAltitude;
      double mMaxAltitude;
   };

   class RangeEntry
   {
   public:
      //! The line where increasing loft angle results in increasing altitude.
      RangeLine mIncreasingLine;
      //! The line where increasing loft angle results in decreasing altitude.
      RangeLine mDecreasingLine;
   };

   //! The data used for surface-to-air launches.
   class SA_Table
   {
   public:
      SA_Table(double aRangeInterval);
      ~SA_Table();

      void ComputeRoundedLimits(double aInterval, double& aMinValue, double& aMaxValue, int& aPointCount);

      void ConvertRangeLine(TempRangeLine& aTempRangeLine, RangeLine& aRealRangeLine);

      int IndexOfRange(double aRange) const
      {
         int groundRangeIndex = static_cast<int>((aRange - mMinRange) / mRangeInterval);
         if (groundRangeIndex < 0)
         {
            groundRangeIndex = 0;
         }
         else if (groundRangeIndex >= mRangePointCount)
         {
            groundRangeIndex = mRangePointCount - 1;
         }
         return groundRangeIndex;
      }

      double RangeOfIndex(size_t aRangeIndex) const { return mMinRange + (aRangeIndex * mRangeInterval); }

      void PrintRangeLine(const RangeLine& aRangeLine, ut::log::MessageStream& aStream) const;

      bool Read(const std::string& aFileName, bool aDebugEnabled, bool aPrintDetail);

      double mMinRange;
      double mMaxRange;
      double mMinAltitude;
      double mMaxAltitude;
      double mMinTimeOfFlight;
      double mMaxTimeOfFlight;

      double mRangeInterval;
      int    mRangePointCount;

      bool mHasVelocityData;

      // The vector is indexed by ground range index.
      std::vector<RangeEntry> mRangeTable;
   };

   //! The data used for surface-to-surface launches.
   class SS_Table
   {
   public:
      SS_Table()  = default;
      ~SS_Table() = default;

      bool Read(const std::string& aFileName);

      // Independent variable range.
      TblIndVarU<double> mRange;

      // Dependent variables are loft angle, burn time, time of flight
      TblDepVar1<double> mLoftAngle;
      TblDepVar1<double> mBurnTime;
      TblDepVar1<double> mTimeOfFlight;
      TblDepVar1<double> mSpeed;
      TblDepVar1<double> mFlightPathAngle;

      TblDepVar1<double> mBO_TimeOfFlight;
      TblDepVar1<double> mBO_Range;
      TblDepVar1<double> mBO_Speed;
      TblDepVar1<double> mBO_FlightPathAngle;

      bool mHasVelocityData = false;
      bool mHasBurnoutData  = false;
   };

   //! The 'evaluator' for surface-to-air intercepts.
   //! This class exists to maintain a 'context' for determining an intercept solutiion.
   //! It eliminates the need to pass a lot of variables between routines. And by not
   //! storing the data as member variables in the launch computer object, it will enable
   //! multiple evaluations (threads) to be performed in parallel.
   class SA_Evaluator
   {
   public:
      SA_Evaluator(double                             aBaseTime,
                   WsfBallisticMissileLaunchComputer* aComputerPtr,
                   WsfMover::KinematicModel*          aTargetDataPtr);

      void FindLaunchWindowStart(double& aTimeToLaunch,
                                 double& aTimeToIntercept,
                                 double& aTimeOfFlight,
                                 double& aLoftAngle,
                                 double& aInterceptSpeed,
                                 double& aInterceptFlightPathAngle);

      void FindLaunchWindowEnd(double& aTimeToLaunch,
                               double& aTimeToIntercept,
                               double& aTimeOfFlight,
                               double& aLoftAngle,
                               double& aInterceptSpeed,
                               double& aInterceptFlightPathAngle);

      bool CheckForInterceptOnRangeLine(int     aRangeIndex,
                                        double& aTimeToLaunch,
                                        double& aTimeToIntercept,
                                        double& aTimeOfFlight,
                                        double& aLoftAngle,
                                        double& aInterceptSpeed,
                                        double& aInterceptFlightPathAngle);

      const WsfBallisticMissileLaunchComputer* mComputerPtr;
      const WsfMover::KinematicModel*          mTargetDataPtr;
      const SA_Table&                          mSA_Table;
      double                                   mBaseTime;
      double                                   mLaunchLocXYZ[3]; // spherical

      bool mCheckGeometry;

      double mBegTime;
      double mBegLocXYZ[3]; // spherical
      double mBegRange;
      double mBegAltitude;
      int    mBegRangeIndex;

      double mEndTime;
      double mEndLocXYZ[3]; // spherical
      double mEndRange;
      double mEndAltitude;
      int    mEndRangeIndex;

      double mLastRange;
      int    mIterCount;
      bool   mClosing;

      TblLookupLUBX<double> mAltitudeLU;
   };

   class SA_Solution
   {
   public:
      SA_Solution()
         : mLaunchTime(-1.0)
         , mInterceptTime(-1.0)
         , mTimeOfFlight(-1.0)
         , mLoftAngle(0.0)
         , mInterceptSpeed(0.0)
         , mInterceptFlightPathAngle(0.0)
         , mEndTime(-1.0)
         , mLaunchWindows()
      {
         UtVec3d::Set(mEndPointWCS, 0.0);
      }

      double mLaunchTime;
      double mInterceptTime;
      double mTimeOfFlight;
      double mLoftAngle;
      double mInterceptSpeed;           // Of the interceptor
      double mInterceptFlightPathAngle; // Of the interceptor
      double mEndTime;
      double mEndPointWCS[3];

      std::vector<SA_LaunchWindow> mLaunchWindows;
   };

   bool ComputeAirIntercept(double aSimTime, WsfPlatform* aTargetPtr, const WsfTrack* aTrackPtr, double aLaunchDelayTime);

   bool ComputeGroundIntercept(double aSimTime, double aLaunchDelayTime, const double aTargetLocWCS[3]);

   void ComputeLaunchWindows(double                        aSimTime,
                             double                        aLaunchDelayTime,
                             double                        aTimeToLaunch,
                             double                        aTimeToIntercept,
                             SA_Evaluator&                 aEvaluator,
                             std::vector<SA_LaunchWindow>& aLaunchWindows);

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);

   void WeaponFireAborted(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTrackPtr, double aQuantity);

   bool InitializeTTIData() override;

private:
   void DrawRanges();

   // This accessor will always return a base Target class pointer.
   WsfMover::KinematicModel* GetTargetData(double       aSimTime,
                                           size_t       aTargetIndex,
                                           WsfStringId  aTargetTypeId,
                                           const double aTargetLocWCS[3],
                                           const double aTargetVelWCS[3],
                                           WsfMover*    aMoverPtr);

   WsfMover::KinematicModel* CreateTarget(double       aSimTime,
                                          WsfStringId  aTargetTypeId,
                                          const double aTargetLocWCS[3],
                                          const double aTargetVelWCS[3],
                                          WsfMover*    aMoverPtr);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   void PurgeSolution(double aSimTime, size_t aTargetIndex, const std::string& aCaller);

   void ShowPoint(const double aLocWCS[3],
                  double       aRedValue   = 1.0,
                  double       aGreenValue = 1.0,
                  double       aBlueValue  = 0.0,
                  int          aPointSize  = 8);


   std::shared_ptr<SA_Table> mSA_TablePtr;
   std::shared_ptr<SS_Table> mSS_TablePtr;

   double                   mMaxLaunchSlantRange;
   double                   mMaxInterceptSlantRange;
   double                   mMinInterceptAltitude;
   double                   mMaxInterceptAltitude;
   double                   mMinInterceptVelocity;
   double                   mMaxInterceptAngle;
   double                   mIntegrationTimeStep;
   double                   mGroundRangeSampleInterval;
   double                   mSolutionValidityTime;
   double                   mPredictedTrajectoryErrorLimit;
   double                   mLastEvaluatedGroundRange;
   bool                     mAllowBoostPhaseIntercept;
   bool                     mShowGraphics;
   bool                     mDebugFileLoading;
   bool                     mInterceptTruth; // else Intercept Track location.
   std::unique_ptr<WsfDraw> mDrawPtr;
   std::unique_ptr<WsfDraw> mLineDrawPtr;

   bool                         mInputComputeEndPoint;
   bool                         mComputeEndPoint;
   bool                         mComputeLaunchWindows;
   std::vector<SA_LaunchWindow> mLaunchWindows;

   double mTargetApogeeTime;
   double mTargetApogeeLocWCS[3];

   //! The SA_SolutionCache stores the last computed solution for each target.
   using SA_SolutionCache = std::map<size_t, SA_Solution>;
   SA_SolutionCache mSA_SolutionCache;

   UtCallbackHolder                             mCallbacks;
   WsfBallisticMissileLaunchComputerSharedData* mSharedDataPtr;
};


class WSF_MIL_EXPORT WsfBallisticMissileLaunchComputerSharedData : public WsfSimulationExtension
{
public:
   WsfBallisticMissileLaunchComputerSharedData();
   ~WsfBallisticMissileLaunchComputerSharedData() override;

   // =================================================================================================
   //! A simulation observer for platform deletions.
   //!
   //! This method is registered as a simulation callback and is called whenever a platform is deleted.
   //! The method will delete any trajectory that exists in the static shared target data table.
   //!
   //! @note This cannot be done in the instance-specific PlatformDeleted method because all of the
   //! launch computer objects may get deleted before the target platform. In that case the callback
   //! would never get called with the target platform as the argument and the target data would never
   //! be cleaned up.
   // protected
   void GlobalPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   bool Initialize() override;


   //! The target map contains predicted trajectory data for targets being evaluated.
   //! This data is shared among all instances of this class because the results are
   //! independent of the evaluator (we are using truth data for the prediction, so
   //! all evaluators will see the same predicted results).

   WsfBallisticMissileLaunchComputer::TargetMap mTargetDataMap;
   std::map<WsfStringId, WsfStringId>           mTargetAeroMap;
   std::map<WsfStringId, double>                mTargetMassMap;
   UtCallbackHolder                             mCallbacks;
};

#endif
