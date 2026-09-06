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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFEW_TRACKEFFECT_HPP
#define WSFEW_TRACKEFFECT_HPP

#include "wsf_mil_export.h"

#include <cfloat>

#include "UtCallbackHolder.hpp"
#include "UtMath.hpp"
class UtInput;

class WsfEM_Interaction;
#include "WsfEW_PulseEffect.hpp"
class WsfSensor;
class WsfSensorMode;
class WsfTrack;

//! WsfEW_TrackEffect models and applies track/tracking effects (tracking errors, track drops) to tracks of radars.

class WSF_MIL_EXPORT WsfEW_TrackEffect : public WsfEW_PulseEffect
{
public:
   WsfEW_TrackEffect();
   WsfEW_TrackEffect& operator=(const WsfEW_TrackEffect&) = delete;
   ~WsfEW_TrackEffect() override                          = default;

   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   void UpdateEffects(double                                 aSimTime,
                      const WsfEM_Interaction&               aTargetInteraction,
                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                      WsfEW_Effects&                         aEW_Effects,
                      WsfEW_Effect*                          aEW_EffectPtr = nullptr) override;

   void UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr) override;

protected:
   class WSF_MIL_EXPORT TrackEffectData : public WsfEW_PulseEffect::PulseData
   {
   public:
      enum
      {
         cRANGE,
         cAZIMUTH,
         cELEVATION,
         cVELOCITY,
         cMAINTAIN_DROP,
         cLAST_ERROR_TYPE
      };

      struct WalkoffData
      {
         struct JtoSTimeTable
         {
            TblIndVarU<double> mJtoS;
            TblDepVar1<double> mMinTime;
            TblDepVar1<double> mMaxTime;
         };

         WalkoffData()
            : mTrackError(0.0)
            , mWalkoffRate(0.0)
            , mHoldoutTarget(DBL_MAX)
            , mHoldoutTime(DBL_MAX)
            , mRecycleTime(DBL_MAX)
            , mDelayTime(0.0)
            , mLastCycleTime(0.0)
            , mRecycle(false)
            , mDelayTable()
            , mHoldOutTable()
            , mRecycleTable()
         {
         }

         void Recycle(double aSimTime)
         {
            mTrackError    = 0.0;
            mLastCycleTime = aSimTime;
         }

         double GetDelayTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal);
         double GetHoldoutTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal);
         double GetRecycleTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal);

         double mTrackError;    //! Error to induce on a track if conditions are met.
         double mWalkoffRate;   //! Rate at which error is increased
         double mHoldoutTarget; //! Specifies maximum error to walk-off ProcessInput forces either a cycle/holdout time
                                //! or maximum error, but not both
         double mHoldoutTime;   //! Time duration each walk-off. ProcessInput forces either a cycle/holdout time or
                                //! maximum error, but not both
         double mRecycleTime; //! Recycle time for each walk-off cycle. ProcessInput forces either a cycle/holdout time
                              //! or maximum error, but not both
         double mDelayTime;   //! Time to delay effect if conditions are met
         double mLastCycleTime; //! Time the last cycle started
         bool mRecycle; //! Indicates whether the walk-off should recycle to the minimum error or stay at holdout (hook)

         JtoSTimeTable mDelayTable;
         JtoSTimeTable mHoldOutTable;
         JtoSTimeTable mRecycleTable;
      };

      TrackEffectData();
      TrackEffectData& operator=(const TrackEffectData&) = delete;
      ~TrackEffectData() override                        = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      double      mLastUpdateTime;
      double      mRequiredJtoS;
      double      mPreviousJtoS;
      double      mRequiredStoN;
      double      mPreviousStoN;
      WalkoffData mWalkoffData[cLAST_ERROR_TYPE];

   protected:
      TrackEffectData(const TrackEffectData& aSrc);
   };

   WsfEW_TrackEffect(const WsfEW_TrackEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   double GetLastUpdateTime(WsfStringId aSystemTypeId) const;
   void   SetLastUpdateTime(WsfStringId aSystemTypeId, double aLastUpdateTime);

   double GetRequiredJtoS(WsfStringId aSystemTypeId) const;

   double GetPreviousJtoS(WsfStringId aSystemTypeId) const;
   void   SetPreviousJtoS(WsfStringId aSystemTypeId, double aPreviousJtoS);

   double GetRequiredStoN(WsfStringId aSystemTypeId) const;
   void   SetRequiredStoN(WsfStringId aSystemTypeId, double aRequiredStoN);

   double GetPreviousStoN(WsfStringId aSystemTypeId) const;
   void   SetPreviousStoN(WsfStringId aSystemTypeId, double aPreviousStoN);

   double GetRangeError(WsfStringId aSystemTypeId) const;
   void   SetRangeError(WsfStringId aSystemTypeId, double aRange);

   double GetRangeWalkoffRate(WsfStringId aSystemTypeId) const;
   void   SetRangeWalkoffRate(WsfStringId aSystemTypeId, double aWalkoffRate);

   double GetAzimuthError(WsfStringId aSystemTypeId) const;
   void   SetAzimuthError(WsfStringId aSystemTypeId, double aAzimuth);

   double GetElevationError(WsfStringId aSystemTypeId) const;
   void   SetElevationError(WsfStringId aSystemTypeId, double aElevation);

   double GetVelocityError(WsfStringId aSystemTypeId) const;
   void   SetVelocityError(WsfStringId aSystemTypeId, double aVelocity);

   TrackEffectData::WalkoffData& GetWalkoffData(WsfStringId aSystemTypeId, int aType) const;

   EffectData* NewEffectData() override;
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}


   void SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

private:
   using TrackEffectDataMap =
      std::map<WsfStringId, TrackEffectData*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.
   TrackEffectDataMap mTrackEffectDataMap;     //!< Map of the System Type Id to the data required for this technique.
   bool               mApplyCurrentEffect;     //!< Flag as to whether the current effect is to be applied or not

   UtCallbackHolder mCallbacks;
};

#endif
