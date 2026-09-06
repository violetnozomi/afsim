// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBMEVENTRESULTS_HPP
#define WSFBMEVENTRESULTS_HPP

#include <ostream>

#include "wsf_iads_c2_lib_export.h"

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/moeLoggingInterface.hpp>
#include <iadsLib/util.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"
#include "WsfEventResult.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"

namespace wsf
{
namespace event
{

// ============================================= BMAssignment =============================================
class WSF_IADS_C2_LIB_EXPORT BMAssignment : public Result
{
   public:
      BMAssignment(double                          aSimTime, 
                   const il::evaluationAssignment* aAssignment, 
                   const std::string               aAssignmentFlag,
                   WsfPlatform*                    aAssigningPlatform,
                   WsfPlatform*                    aAssignedPlatform,
                   WsfPlatform*                    aTargetPlatform,
                   WsfWeapon*                      aAssignedWeapon,
                   double                          aTargetLLA[3],
                   Settings&                       aSettings,
                   bool                            aIsDelegate = false)
       : Result(aSimTime, aSettings), 
         mAssignment(aAssignment),
         mAssignmentFlag(aAssignmentFlag),
         mAssigningPlatform(aAssigningPlatform),
         mAssignedPlatform(aAssignedPlatform),
         mTargetPlatform(aTargetPlatform),
         mAssignedWeapon(aAssignedWeapon),
         mIsDelegate(aIsDelegate)
      { UtVec3d::Set(mTargetLLA, aTargetLLA); }
      ~BMAssignment() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::evaluationAssignment* mAssignment; 
      const std::string                 mAssignmentFlag;      
      WsfPlatform*                    mAssigningPlatform;
      WsfPlatform*                    mAssignedPlatform;
      WsfPlatform*                    mTargetPlatform;   
      WsfWeapon*                       mAssignedWeapon;
      double                          mTargetLLA[3];
      bool                             mIsDelegate;
};

// ============================================= AssignmentStatus =========================================
class WSF_IADS_C2_LIB_EXPORT AssignmentStatus : public Result
{
   public:
      AssignmentStatus(double                            aSimTime,
                        const il::assignmentACKMessage* aStatus,
                        const il::evaluationAssignment* aRelatedAssignment,
                        std::string                      aStatusType,
                        WsfPlatform*                      aAssigningPlatform,
                        WsfPlatform*                      aAssignedPlatform,
                        WsfPlatform*                      aTargetPlatform,
                        WsfWeapon*                      aAssignedWeapon,
                        std::string                      aSystemicScope,
                        WsfSimulation&                  aSimulation,
                        Settings&                         aSettings)
       : Result(aSimTime, aSettings),
         mStatus(aStatus),
         mRelatedAssignment(aRelatedAssignment),
         mStatusType(aStatusType),
         mAssigningPlatform(aAssigningPlatform),
         mAssignedPlatform(aAssignedPlatform),
         mTargetPlatform(aTargetPlatform),
         mAssignedWeapon(aAssignedWeapon),
         mSystemicScope(aSystemicScope),
         mSimulation(aSimulation)
      {}
      ~AssignmentStatus() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::assignmentACKMessage* mStatus;
      const il::evaluationAssignment* mRelatedAssignment;
      std::string                       mStatusType;
      WsfPlatform*                    mAssigningPlatform;
      WsfPlatform*                    mAssignedPlatform;
      WsfPlatform*                    mTargetPlatform;
      WsfWeapon*                       mAssignedWeapon;
      std::string                     mSystemicScope;
      WsfSimulation&                    mSimulation;
};

// ============================================= AssignmentCancel =========================================
class WSF_IADS_C2_LIB_EXPORT AssignmentCancel : public Result
{
   public:
      AssignmentCancel(double                            aSimTime,
                        const il::idRecord*             aCancellingUnit,
                        const il::evaluationAssignment* aActiveAssignment,
                        const std::string&              aCancelReason,
                        WsfPlatform*                      aCancellingPlatform,
                        WsfPlatform*                    aAssigningPlatform,
                        WsfPlatform*                      aAssignedPlatform,
                        WsfPlatform*                      aTargetPlatform,
                        WsfWeapon*                      aAssignedWeapon,
                        Settings&                         aSettings)
       : Result(aSimTime, aSettings),
         mCancellingUnit(aCancellingUnit),
         mActiveAssignment(aActiveAssignment),
         mCancelReason(aCancelReason),
         mCancellingPlatform(aCancellingPlatform),
         mAssigningPlatform(aAssigningPlatform),
         mAssignedPlatform(aAssignedPlatform),
         mTargetPlatform(aTargetPlatform),
         mAssignedWeapon(aAssignedWeapon)
      {}
      ~AssignmentCancel() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::idRecord*             mCancellingUnit;
      const il::evaluationAssignment* mActiveAssignment;
      const std::string&              mCancelReason;
      WsfPlatform*                    mCancellingPlatform;
      WsfPlatform*                    mAssigningPlatform;
      WsfPlatform*                    mAssignedPlatform;
      WsfPlatform*                    mTargetPlatform;
      WsfWeapon*                       mAssignedWeapon;
};

// ============================================= CANTCOAssignment =========================================
class WSF_IADS_C2_LIB_EXPORT CANTCOAssignment : public Result
{
   public:
      CANTCOAssignment(double                         aSimTime, 
                        const il::idRecord*          aRespondingUnit, 
                        const il::assignmentMessage* aAssignment, 
                        const il::trackRecord*       aOptTrack, 
                        const std::string&             aReason,
                        WsfPlatform*                 aAssigningPlatform,
                        WsfPlatform*                   aAssignedPlatform,
                        WsfPlatform*                   aTargetPlatform,
                        WsfPlatform*                   aReportingPlatform,
                        WsfWeapon*                   aAssignedWeapon,
                        Settings&                      aSettings)
      : Result(aSimTime, aSettings),
         mRespondingUnit(aRespondingUnit),
         mAssignment(aAssignment),
         mOptTrack(aOptTrack),
         mReason(aReason),
         mAssigningPlatform(aAssigningPlatform),
         mAssignedPlatform(aAssignedPlatform),
         mTargetPlatform(aTargetPlatform),
         mReportingPlatform(aReportingPlatform),
         mAssignedWeapon(aAssignedWeapon)

      {}
      ~CANTCOAssignment() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::idRecord*           mRespondingUnit;
      const il::assignmentMessage* mAssignment;
      const il::trackRecord*        mOptTrack;
      const std::string&           mReason;
      WsfPlatform*                 mAssigningPlatform;
      WsfPlatform*                 mAssignedPlatform;
      WsfPlatform*                 mTargetPlatform;
      WsfPlatform*                 mReportingPlatform;
      WsfWeapon*                    mAssignedWeapon;
};

// ============================================= Allocate =================================================
class WSF_IADS_C2_LIB_EXPORT Allocate : public Result
{
   public:
      Allocate(double                      aSimTime,
               WsfPlatform*                aAllocatingPlatform,
               const il::assessmentRecord* aAssessment,
               WsfPlatform*                aTargetPlatform,
               std::string                 aZoneTypes,
               Settings&                   aSettings)
       : Result(aSimTime, aSettings),
         mAllocatingPlatform(aAllocatingPlatform),
         mAssessment(aAssessment),
         mTargetPlatform(aTargetPlatform),
         mZoneTypes(aZoneTypes)
      {
      }
      ~Allocate() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      WsfPlatform* mAllocatingPlatform;
      const il::assessmentRecord* mAssessment;
      WsfPlatform* mTargetPlatform;
      std::string                 mZoneTypes;
};

// ============================================= SensorCue ================================================
class WSF_IADS_C2_LIB_EXPORT SensorCue : public Result
{
   public:
      SensorCue(double                        aSimTime, 
                  const il::assetRecord*         aCurrentUnit, 
                  const il::trackRecord*         aMasterTrack, 
                  const il::assignmentMessage* aAssignment, 
                  const il::sensorRecord*      aSensorToAssign,
                  WsfPlatform*                  aTargetPlatform,
                  WsfSimulation&               aSimulation,
                  Settings&                     aSettings)
       : Result(aSimTime, aSettings),
         mCurrentUnit(aCurrentUnit), 
         mMasterTrack(aMasterTrack), 
         mAssignment(aAssignment), 
         mSensorToAssign(aSensorToAssign), 
         mTargetPlatform(aTargetPlatform), 
         mSimulation(aSimulation)
      {}
      ~SensorCue() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::assetRecord*        mCurrentUnit;
      const il::trackRecord*        mMasterTrack; 
      const il::assignmentMessage* mAssignment;
      const il::sensorRecord*        mSensorToAssign;
      WsfPlatform*                 mTargetPlatform;
      WsfSimulation&                 mSimulation;
};

// ============================================= SensorDropCue ============================================
class WSF_IADS_C2_LIB_EXPORT SensorDropCue : public Result
{
   public:
      SensorDropCue(double                         aSimTime, 
                     const il::assetRecord*       aCurrentUnit, 
                     const il::trackRecord*       aMasterTrack, 
                     const il::assignmentMessage* aAssignment, 
                     const il::sensorRecord*       aSensorToAssign,
                     WsfPlatform*                   aTargetPlatform,
                     WsfSimulation&                aSimulation,
                     Settings&                      aSettings)
       : Result(aSimTime, aSettings),
         mCurrentUnit(aCurrentUnit), 
         mMasterTrack(aMasterTrack), 
         mAssignment(aAssignment), 
         mSensorToAssign(aSensorToAssign), 
         mTargetPlatform(aTargetPlatform), 
         mSimulation(aSimulation)
      {}
      ~SensorDropCue() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::assetRecord*        mCurrentUnit;
      const il::trackRecord*        mMasterTrack; 
      const il::assignmentMessage* mAssignment;
      const il::sensorRecord*        mSensorToAssign;
      WsfPlatform*                 mTargetPlatform;
      WsfSimulation&                 mSimulation;
};

// ============================================= SensorTracking ===========================================
class WSF_IADS_C2_LIB_EXPORT SensorTracking : public Result
{
   public:
      SensorTracking(double                   aSimTime, 
                     const il::trackRecord*   aMasterTrack, 
                     const il::sensorRecord*  aAssignedSensor,
                     const il::positionRecord aTrackingLocation,
                     WsfPlatform*             aTargetPlatform,
                     double                   aTargetLLA[3],
                     WsfSimulation&           aSimulation,
                     Settings&                aSettings)
       : Result(aSimTime, aSettings),
         mMasterTrack(aMasterTrack), 
         mAssignedSensor(aAssignedSensor), 
         mTrackingLocation(aTrackingLocation), 
         mTargetPlatform(aTargetPlatform), 
         mSimulation(aSimulation)
      { UtVec3d::Set(mTargetLLA, aTargetLLA); }
      ~SensorTracking() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::trackRecord*   mMasterTrack;
      const il::sensorRecord*  mAssignedSensor;
      const il::positionRecord mTrackingLocation;
      WsfPlatform*             mTargetPlatform;
      double                   mTargetLLA[3];
      WsfSimulation&             mSimulation;
};

// ============================================= AIAddBehavior ============================================
class WSF_IADS_C2_LIB_EXPORT AIAddBehavior : public Result
{
   public:
      AIAddBehavior(double                 aSimTime, 
                    const std::string      aBehavior, 
                    const il::trackRecord* aMasterTrack, 
                    const il::assetRecord* aAIAsset, 
                    const double           aRange, 
                    const double           aRangeToZone,
                    double                 aTargetLLA[3],
                    Settings&              aSettings)
       : Result(aSimTime, aSettings),
         mBehavior(aBehavior), 
         mMasterTrack(aMasterTrack), 
         mAIAsset(aAIAsset), 
         mRange(aRange),
         mRangeToZone(aRangeToZone)
      { UtVec3d::Set(mTargetLLA, aTargetLLA); }
      ~AIAddBehavior() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const std::string      mBehavior;
      const il::trackRecord* mMasterTrack; 
      const il::assetRecord* mAIAsset;
      const double           mRange;
      const double           mRangeToZone;
      double                 mTargetLLA[3];
};

// ============================================= AITakeAction =============================================
class WSF_IADS_C2_LIB_EXPORT AITakeAction : public Result
{
   public:
      AITakeAction(double                 aSimTime, 
                   const std::string&     aActionType, 
                   const double           aEndTime,
                   const il::trackRecord* aMasterTrack, 
                   const il::assetRecord* aAIAsset, 
                   const double           aRange, 
                   const double           aRangeToZone,
                   double                  aTargetLLA[3],
                   Settings&               aSettings)
       : Result(aSimTime, aSettings),
         mActionType(aActionType), 
         mEndTime(aEndTime),
         mMasterTrack(aMasterTrack), 
         mAIAsset(aAIAsset), 
         mRange(aRange),
         mRangeToZone(aRangeToZone)
      { UtVec3d::Set(mTargetLLA, aTargetLLA); }
      ~AITakeAction() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const std::string&     mActionType;
      const double           mEndTime;
      const il::trackRecord* mMasterTrack; 
      const il::assetRecord* mAIAsset;
      const double           mRange;
      const double           mRangeToZone;
      double                 mTargetLLA[3];
};

// ============================================= AIEgress =================================================
class WSF_IADS_C2_LIB_EXPORT AIEgress : public Result
{
   public:
      AIEgress(double                 aSimTime,
               const il::assetRecord* aAIAsset,
               const std::string&     aEgressMethod,
               const std::string&     aCorridorName,
               Settings&              aSettings)
       : Result(aSimTime, aSettings),
         mAIAsset(aAIAsset),
         mEgressMethod(aEgressMethod),
         mCorridorName(aCorridorName)
      {
      }
      ~AIEgress() override = default;

      void Print(std::ostream& aStream) const override;
      void PrintCSV(std::ostream& aStream) const override;

   private:
      const il::assetRecord* mAIAsset;
      const std::string& mEgressMethod;
      const std::string& mCorridorName;
};

}
}

#endif
