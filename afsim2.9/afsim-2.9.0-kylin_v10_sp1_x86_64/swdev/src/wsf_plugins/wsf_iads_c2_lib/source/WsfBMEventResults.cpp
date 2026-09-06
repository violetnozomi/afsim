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

#include "WsfBMEventResults.hpp"

#include <ostream>

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/moeLoggingInterface.hpp>
#include <iadsLib/util.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"
#include "WsfEventUtils.hpp"
#include "WsfBMUtils.hpp"

namespace wsf
{
namespace event
{

void BMAssignment::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_ASSIGNMENT_CREATION " << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
           << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " ("
           << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown") << ") "
           << "TrackId: " << mAssignment->Assignment()->getLocalTrackID() << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Start_Time: ";
   utils::PrintTime(aStream, mAssignment->Assignment()->getAssignTime(), mSettings.GetTimeFormat());
   aStream << mAssignmentFlag << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Truth:";
   utils::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2], mSettings.GetLatLonFormat());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Sensor_Type: " << mAssignment->Track()->getReportingSensorType()
           << " Weapon_Type: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown") << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << " Source_TrackId: " << mAssignment->Assignment()->getReferenceTrackID() << " "
           << il::ShotDoctrineEnumToString(mAssignment->Assignment()->getShotDoctrine()) << "\n";
}

void BMAssignment::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "BM_ASSIGNMENT_CREATION" 
           << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
           << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
           << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
           << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
           << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
           << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
           << ',' << il::ShotDoctrineEnumToString(mAssignment->Assignment()->getShotDoctrine())
           << ',' << mAssignment->Track()->getReportingSensorType()
           << ',' << (mIsDelegate ? "DELEGATE" : "COMMANDED")
           << ',' << mTargetLLA[0]
           << ',' << mTargetLLA[1]
           << ',' << mTargetLLA[2]
           << ',' << mAssignment->Assignment()->getAssignTime()
           << ',' << mAssignment->Assignment()->getReferenceTrackID()
           << ',' << mAssignment->Assignment()->getLocalTrackID()
           << '\n';
}

void AssignmentStatus::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   if (mStatus->getStatus() == il::assignmentACKMessage::CHANGED_ASSIGNED_UNIT)
   {
      auto new_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(&mSimulation, mStatus->getNewlyAssignedID().getID());
      WsfWeapon* new_wpn = nullptr;
      if (new_plat)
      {
         new_wpn = new_plat->GetComponentEntry<WsfWeapon>(mStatus->getNewlyAssignedID().getSubID());
      }

      aStream << "BM_ASSIGNMENT_OVERRIDE " << mStatusType << " " << (new_plat ? new_plat->GetName() : "unknown") << " "
              << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
              << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
              << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " ("
              << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown") << ") "
              << "TrackId: " << mRelatedAssignment->Assignment()->getLocalTrackID() << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Start_Time: ";
      utils::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime(), mSettings.GetTimeFormat());
      aStream << " Weapon_Type: " << (new_wpn ? new_wpn->GetName() : "unknown") << " "
              << "Prev_Platform_Name: " << (mAssignedPlatform ? mAssignedWeapon->GetName() : "unknown") << " "
              << "Prev_Weapon_Name: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown") << " "
              << "Ack_Message_Time: " << mStatus->getAckTime() << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Systemic_Cantco: " << il::BoolToString(mStatus->isSystemicCantco()) << " " << mSystemicScope << " "
              << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine()) << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << " SourceTrackId: " << mRelatedAssignment->Assignment()->getReferenceTrackID()
              << "\n";
   }
   else if (mStatus->getStatus() == il::assignmentACKMessage::UPDATED_SHOT_DOCTRINE)
   {
      aStream << "BM_ASSIGNMENT_DOCTRINE_OVERRIDE " << mStatusType << " "
              << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown") << " "
              << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
              << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
              << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " ("
              << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown") << ") "
              << "TrackId: " << mRelatedAssignment->Assignment()->getLocalTrackID() << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Start_Time: ";
      utils::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime(), mSettings.GetTimeFormat());
      aStream << " Weapon_Type: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown") << " "
              << "Ack_Message_Time: " << mStatus->getAckTime() << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Systemic_Cantco: " << il::BoolToString(mStatus->isSystemicCantco()) << " " << mSystemicScope << " "
              << il::ShotDoctrineEnumToString(mStatus->getUpdatedShotDoctrine()) << " Prev_Shot_Doctrine: "
              << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine()) << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " SourceTrackId: "
              << mRelatedAssignment->Assignment()->getReferenceTrackID()
              << "\n";
   }
   else
   {
      aStream << "BM_ASSIGNMENT_STATUS " << mStatusType << " "
              << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown ") << " "
              << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown ") << " ("
              << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown ") << ") "
              << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown ") << " ("
              << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown ") << ") "
              << "TrackId: " << mRelatedAssignment->Assignment()->getLocalTrackID() << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Start_Time: ";
      utils::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime(), mSettings.GetTimeFormat());
      aStream << " Weapon_Type: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown ") << " "
              << "Ack_Message_Time: " << mStatus->getAckTime() << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Systemic_Cantco: " << il::BoolToString(mStatus->isSystemicCantco()) << " " << mSystemicScope << " "
              << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine()) << " "
              << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " SourceTrackId: "
              << mRelatedAssignment->Assignment()->getReferenceTrackID()
              << "\n";
   }
}

void AssignmentStatus::PrintCSV(std::ostream& aStream) const
{
   if (mStatus->getStatus() == il::assignmentACKMessage::CHANGED_ASSIGNED_UNIT)
   {
      auto new_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(&mSimulation, mStatus->getNewlyAssignedID().getID());
      WsfWeapon* new_wpn = nullptr;
      if (new_plat)
      {
         new_wpn = new_plat->GetComponentEntry<WsfWeapon>(mStatus->getNewlyAssignedID().getSubID());
      }

      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "BM_ASSIGNMENT_OVERRIDE"
         << ',' << mStatusType
         << ',' << (new_plat ? new_plat->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
         << ',' << mRelatedAssignment->Assignment()->getLocalTrackID()
         << ',';
      utilsCSV::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime());
      aStream << ',' << (new_wpn ? new_wpn->GetName() : "unknown")
         << ',' << mStatus->getAckTime()
         << ',' << il::BoolToString(mStatus->isSystemicCantco())
         << ',' << mSystemicScope
         << ',' << mRelatedAssignment->Assignment()->getReferenceTrackID()
         << ',' << il::ShotDoctrineEnumToString(mStatus->getUpdatedShotDoctrine())
         << ',' << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine())
         << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
         << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
         << '\n';
   }
   else if (mStatus->getStatus() == il::assignmentACKMessage::UPDATED_SHOT_DOCTRINE)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "BM_ASSIGNMENT_DOCTRINE_OVERRIDE"
         << ',' << mStatusType
         << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
         << ',' << mRelatedAssignment->Assignment()->getLocalTrackID()
         << ',';
      utilsCSV::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime());
      aStream << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
         << ',' << mStatus->getAckTime()
         << ',' << il::BoolToString(mStatus->isSystemicCantco())
         << ',' << mSystemicScope
         << ',' << mRelatedAssignment->Assignment()->getReferenceTrackID()
         << ',' << il::ShotDoctrineEnumToString(mStatus->getUpdatedShotDoctrine())
         << ',' << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine())
         << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
         << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
         << '\n';
   }
   else
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "BM_ASSIGNMENT_STATUS"
         << ',' << mStatusType
         << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
         << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
         << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
         << ',' << mRelatedAssignment->Assignment()->getLocalTrackID()
         << ',';
      utilsCSV::PrintTime(aStream, mRelatedAssignment->Assignment()->getAssignTime());
      aStream << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
         << ',' << mStatus->getAckTime()
         << ',' << il::BoolToString(mStatus->isSystemicCantco())
         << ',' << mSystemicScope
         << ',' << mRelatedAssignment->Assignment()->getReferenceTrackID()
         << ',' << il::ShotDoctrineEnumToString(mStatus->getUpdatedShotDoctrine())
         << ',' << il::ShotDoctrineEnumToString(mRelatedAssignment->Assignment()->getShotDoctrine())
         << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
         << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
         << '\n';
   }
}

void AssignmentCancel::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_ASSIGNMENT_CANCEL " << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown") << " "
      << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
      << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
      << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " ("
      << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown") << ") "
      << "TrackId: " << mActiveAssignment->Assignment()->getLocalTrackID() << " "
      << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
      << "  Start_Time: ";
   utils::PrintTime(aStream, mActiveAssignment->Assignment()->getAssignTime(), mSettings.GetTimeFormat());
   aStream << " Weapon_Type: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown ") << " "
      << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
      << "  Canceling_Platform: " << (mCancellingPlatform ? mCancellingPlatform->GetName() : "unknown") << " ("
      << (mCancellingPlatform ? mCancellingPlatform->GetSide() : "unknown") << ") "
      << "mReason: " << mCancelReason << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
      << " SourceTrackId: " << mActiveAssignment->Assignment()->getReferenceTrackID() << "\n";
}

void AssignmentCancel::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_ASSIGNMENT_CANCEL"
      << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
      << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
      << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
      << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
      << ',' << (mCancellingPlatform ? mCancellingPlatform->GetName() : "unknown")
      << ',' << (mCancellingPlatform ? mCancellingPlatform->GetSide() : "unknown")
      << ',' << mCancelReason
      << ',' << mActiveAssignment->Assignment()->getAssignTime()
      << ',' << mActiveAssignment->Assignment()->getReferenceTrackID()
      << ',' << mActiveAssignment->Assignment()->getLocalTrackID()
      << '\n';
}

void CANTCOAssignment::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_CANTCO_ASSIGNMENT " << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown") << " "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
           << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown") << " ("
           << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown") << ") "
           << "TrackId: " << mAssignment->getLocalTrackID() << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Start_Time: ";
   utils::PrintTime(aStream, mAssignment->getAssignTime(), mSettings.GetTimeFormat());
   aStream << " Weapon_Type: " << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown") << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << "  mReportingPlatformform: " << (mReportingPlatform ? mReportingPlatform->GetName() : "unknown") << " ("
           << (mReportingPlatform ? mReportingPlatform->GetSide() : "unknown") << ") "
           << "mReason: " << mReason << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << " SourceTrackId: " << mAssignment->getReferenceTrackID() << "\n";
}

void CANTCOAssignment::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_CANTCO_ASSIGNMENT"
      << ',' << (mAssignedPlatform ? mAssignedPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
      << ',' << (mAssigningPlatform ? mAssigningPlatform->GetName() : "unknown")
      << ',' << (mAssigningPlatform ? mAssigningPlatform->GetSide() : "unknown")
      << ',' << (mAssignedWeapon ? mAssignedWeapon->GetName() : "unknown")
      << ',' << (mReportingPlatform ? mReportingPlatform->GetName() : "unknown")
      << ',' << (mReportingPlatform ? mReportingPlatform->GetSide() : "unknown")
      << ',' << mReason
      << ',' << mAssignment->getAssignTime()
      << ',' << mAssignment->getReferenceTrackID()
      << ',' << mAssignment->getLocalTrackID()
      << '\n';
}

void Allocate::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_ALLOCATE " << (mAllocatingPlatform ? mAllocatingPlatform->GetName() : "unknown") << " ("
           << (mAllocatingPlatform ? mAllocatingPlatform->GetSide() : "unknown") << ") "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
           << "TrackId: " << mAssessment->getAssessedThreatRecord()->getTrack()->getID() << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << "  Zone_Types: " << mZoneTypes << "\n";
}

void Allocate::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_ALLOCATE"
      << ',' << (mAllocatingPlatform ? mAllocatingPlatform->GetName() : "unknown")
      << ',' << (mAllocatingPlatform ? mAllocatingPlatform->GetSide() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
      << ',' << mZoneTypes
      << ',' << mAssessment->getAssessedThreatRecord()->getTrack()->getID()
      << '\n';
}

void SensorCue::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_SENSOR_CUE "
           << il::getSensorTypeStringFromEnum(mSensorToAssign->GetSensorType(),
                                              IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
           << " " << mSensorToAssign->GetParentAsset()->getName() << " ("
           << mSensorToAssign->GetParentAsset()->getSide() << ") "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") " << mCurrentUnit->getName() << " ("
           << mCurrentUnit->getSide() << ") " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << "  Type: " << mSensorToAssign->GetSensorID().getIDString() << " "
           << "TrackId: " << mMasterTrack->getID() << " "
           << "TruthId: " << mMasterTrack->getTargetTruthID().getID() << "\n";
}

void SensorCue::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_SENSOR_CUE"
           << ',' << il::getSensorTypeStringFromEnum(mSensorToAssign->GetSensorType(),
                                                     IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
           << ',' << mSensorToAssign->GetParentAsset()->getName()
           << ',' << mSensorToAssign->GetParentAsset()->getSide()
           << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
           << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
           << ',' << mCurrentUnit->getName()
           << ',' << mCurrentUnit->getSide()
           << ',' << mSensorToAssign->GetSensorID().getIDString()
           << ',' << mMasterTrack->getID() << ',' << mMasterTrack->getTargetTruthID().getID()
           << '\n';
}

void SensorDropCue::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_SENSOR_DROP_CUE "
           << il::getSensorTypeStringFromEnum(mSensorToAssign->GetSensorType(),
                                              IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
           << " " << mSensorToAssign->GetParentAsset()->getName() << " ("
           << mSensorToAssign->GetParentAsset()->getSide() << ") "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") " << mCurrentUnit->getName() << " ("
           << mCurrentUnit->getSide() << ") " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << "  Type: " << mSensorToAssign->GetSensorID().getIDString() << " "
           << "TrackId: " << mMasterTrack->getID() << " "
           << "TruthId: " << mMasterTrack->getTargetTruthID().getID() << "\n";
}

void SensorDropCue::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_SENSOR_DROP_CUE"
      << ',' << il::getSensorTypeStringFromEnum(mSensorToAssign->GetSensorType(),
                                                IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
      << ',' << mSensorToAssign->GetParentAsset()->getName() << ',' << mSensorToAssign->GetParentAsset()->getSide()
      << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
      << ',' << mCurrentUnit->getName()
      << ',' << mCurrentUnit->getSide()
      << ',' << mSensorToAssign->GetSensorID().getIDString()
      << ',' << mMasterTrack->getID()
      << ',' << mMasterTrack->getTargetTruthID().getID()
      << '\n';
}

void SensorTracking::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_SENSOR_TRACKING "
           << il::getSensorTypeStringFromEnum(mAssignedSensor->GetSensorType(),
                                              IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
           << " " << mAssignedSensor->GetParentAsset()->getName() << " ("
           << mAssignedSensor->GetParentAsset()->getSide() << ") "
           << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown") << " ("
           << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown") << ") "
           << "PlatformId: " << mAssignedSensor->GetParentAsset()->getID() << " "
           << "SensorId: " << mAssignedSensor->GetSensorID() << " "
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Track: ";
   utils::PrintLocationDataLLA(aStream, mTrackingLocation.getLatDegs(),
                               mTrackingLocation.getLonDegs(),
                               mTrackingLocation.getAltM(),
                               mSettings.GetLatLonFormat());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Truth: ";
   utils::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2], mSettings.GetLatLonFormat());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  TrackId: " << mMasterTrack->getID()
           << " TruthId: " << mMasterTrack->getTargetTruthID().getID() << "\n";
}

void SensorTracking::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_SENSOR_TRACKING"
      << ',' << il::getSensorTypeStringFromEnum(mAssignedSensor->GetSensorType(),
                                                IADSC2ScenarioExtension::GetGlobalLogger(mSimulation.GetScenario()))
      << ',' << mAssignedSensor->GetParentAsset()->getName() << ',' << mAssignedSensor->GetParentAsset()->getSide()
      << ',' << (mTargetPlatform ? mTargetPlatform->GetName() : "unknown")
      << ',' << (mTargetPlatform ? mTargetPlatform->GetSide() : "unknown")
      << ',' << mAssignedSensor->GetParentAsset()->getID()
      << ',' << mAssignedSensor->GetSensorID();
   utilsCSV::PrintLocationDataLLA(aStream, mTrackingLocation.getLatDegs(),
                                  mTrackingLocation.getLonDegs(),
                                  mTrackingLocation.getAltM());
   utilsCSV::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2]);
   aStream << ',' << mMasterTrack->getID() << ',' << mMasterTrack->getTargetTruthID().getID()
      << '\n';
}

void AIAddBehavior::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_AI_ADD_BEHAVIOR: " << mBehavior << ','
           << "This AI: " << mAIAsset->getName() << ',' << mAIAsset->getID() << ',' << mAIAsset->getSide();
   utils::PrintLocationDataLLA(aStream,
                               mAIAsset->getPosition().getLatDegs(),
                               mAIAsset->getPosition().getLonDegs(),
                               mAIAsset->getPosition().getAltM(),
                               mSettings.GetLatLonFormat());
   aStream << ','
           << "Target: " << mMasterTrack->getTargetTruthName() << ',' << mMasterTrack->getTargetTruthID() << ','
           << mMasterTrack->getTargetTruthSide();
   utils::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2], mSettings.GetLatLonFormat());
   aStream << ','
           << "Range to target: " << mRange << ','
           << "Range to zone: " << mRangeToZone << ','
           << "Track ID: " << mMasterTrack->getID() << '\n';
}

void AIAddBehavior::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_AI_ADD_BEHAVIOR" 
           << ',' << mBehavior
           << ',' << mAIAsset->getName() 
           << ',' << mAIAsset->getID()
           << ',' << mAIAsset->getSide();
   utilsCSV::PrintLocationDataLLA(aStream,
                                  mAIAsset->getPosition().getLatDegs(),
                                  mAIAsset->getPosition().getLonDegs(),
                                  mAIAsset->getPosition().getAltM());
   aStream << ',' << mMasterTrack->getTargetTruthName() 
           << ',' << mMasterTrack->getTargetTruthID()
           << ',' << mMasterTrack->getTargetTruthSide();
   utilsCSV::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2]);
   aStream << ',' << mRange 
           << ',' << mRangeToZone 
           << ',' << mMasterTrack->getID()
           << '\n';
}

void AITakeAction::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BM_AI_TAKE_ACTION"
           << ','
           << "Action type: " << mActionType << ','
           << "This AI: " << mAIAsset->getName() << ',' << mAIAsset->getID() << ',' << mAIAsset->getSide();
   utils::PrintLocationDataLLA(aStream,
                               mAIAsset->getPosition().getLatDegs(),
                               mAIAsset->getPosition().getLonDegs(),
                               mAIAsset->getPosition().getAltM(), mSettings.GetLatLonFormat());
   aStream << ','
           << "Target: " << mMasterTrack->getTargetTruthName() << ',' << mMasterTrack->getTargetTruthID() << ','
           << mMasterTrack->getTargetTruthSide();
   utils::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2], mSettings.GetLatLonFormat());
   aStream << ','
           << "Action end time: " << mEndTime << ','
           << "Range to target: " << mRange << ','
           << "Range to zone: " << mRangeToZone << ','
           << "Track ID: " << mMasterTrack->getID() << '\n';
}

void AITakeAction::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_AI_TAKE_ACTION"
           << ',' << mActionType
           << ',' << mAIAsset->getName()
           << ',' << mAIAsset->getID()
           << ',' << mAIAsset->getSide();
   utilsCSV::PrintLocationDataLLA(aStream,
                                  mAIAsset->getPosition().getLatDegs(),
                                  mAIAsset->getPosition().getLonDegs(),
                                  mAIAsset->getPosition().getAltM());
   aStream << ',' << mMasterTrack->getTargetTruthName()
           << ',' << mMasterTrack->getTargetTruthID()
           << ',' << mMasterTrack->getTargetTruthSide();
   utilsCSV::PrintLocationDataLLA(aStream, mTargetLLA[0], mTargetLLA[1], mTargetLLA[2]);
   aStream << ',' << mEndTime
           << ',' << mRange
           << ',' << mRangeToZone
           << ',' << mMasterTrack->getID()
           << '\n';
}

void AIEgress::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());

   aStream << "BM_AI_EGRESS"
           << ',' << "This AI: " << mAIAsset->getName() 
           << ',' << mAIAsset->getID()
           << ',' << mAIAsset->getSide();
   utils::PrintLocationDataLLA(aStream,
                               mAIAsset->getPosition().getLatDegs(),
                               mAIAsset->getPosition().getLonDegs(),
                               mAIAsset->getPosition().getAltM(), mSettings.GetLatLonFormat());
   aStream << ',' << "Land or CAP?: " << mEgressMethod 
           << ',' << "Corridor: " << mCorridorName 
           << '\n';
}

void AIEgress::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "BM_AI_EGRESS" << ',' << mAIAsset->getName()
           << ',' << mAIAsset->getID()
           << ',' << mAIAsset->getSide();
   utilsCSV::PrintLocationDataLLA(aStream,
                                  mAIAsset->getPosition().getLatDegs(),
                                  mAIAsset->getPosition().getLonDegs(),
                                  mAIAsset->getPosition().getAltM());
   aStream << ',' << mEgressMethod
           << ',' << mCorridorName
           << '\n';
}

}
}
