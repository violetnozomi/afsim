// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "OrbitalSequenceToModel.hpp"

#include "OrbitalMissionItem.hpp"
#include "OrbitalMissionModel.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfSpaceOrbitalPropagatorCondition.hpp"
#include "WsfSpaceOrbitalPropagatorConditionVisitor.hpp"
#include "WsfTargetPointOptions.hpp"

namespace
{
class ConditionToModel : public wsf::space::OrbitalPropagatorConditionVisitor
{
public:
   void Accept(wsf::space::ApoapsisCondition& aCondition) override { CommonConfiguration(aCondition); }
   void Accept(wsf::space::AscendingNodeCondition& aCondition) override { CommonConfiguration(aCondition); }

   void Accept(wsf::space::AscendingRadiusCondition& aCondition) override
   {
      CommonConfiguration(aCondition);
      mConfiguration[wsfg::spaceg::EventConfigKeys::cRADIUS] = wsfg::spaceg::FromValue(aCondition.GetRadius());
   }

   void Accept(wsf::space::DescendingNodeCondition& aCondition) override { CommonConfiguration(aCondition); }

   void Accept(wsf::space::DescendingRadiusCondition& aCondition) override
   {
      CommonConfiguration(aCondition);
      mConfiguration[wsfg::spaceg::EventConfigKeys::cRADIUS] = wsfg::spaceg::FromValue(aCondition.GetRadius());
   }

   void Accept(wsf::space::EclipseEntryCondition& aCondition) override { CommonConfiguration(aCondition); }
   void Accept(wsf::space::EclipseExitCondition& aCondition) override { CommonConfiguration(aCondition); }
   void Accept(wsf::space::NoneCondition& aCondition) override { CommonConfiguration(aCondition); }
   void Accept(wsf::space::NorthernIntersectionCondition& aCondition) override { CommonConfiguration(aCondition); }
   void Accept(wsf::space::PeriapsisCondition& aCondition) override { CommonConfiguration(aCondition); }

   void Accept(wsf::space::RelativeTimeCondition& aCondition) override
   {
      CommonConfiguration(aCondition);
      mConfiguration[wsfg::spaceg::EventConfigKeys::cRELATIVE_TIME] = wsfg::spaceg::FromValue(aCondition.GetOffsetTime());
   }

   void Accept(wsf::space::SouthernIntersectionCondition& aCondition) override { CommonConfiguration(aCondition); }

   const QJsonObject& GetConfiguration() const { return mConfiguration; }

private:
   void CommonConfiguration(wsf::space::OrbitalPropagatorCondition& aCondition)
   {
      mConfiguration[wsfg::spaceg::EventConfigKeys::cOPTION] = QString::fromStdString(aCondition.GetConditionString());

      unsigned int orbit = aCondition.GetOrbitNumber();
      if (orbit != 0)
      {
         mConfiguration[wsfg::spaceg::EventConfigKeys::cORBIT] = QString::number(orbit);
      }
   }

   QJsonObject mConfiguration{};
};

} // namespace

namespace wsfg
{
namespace spaceg
{
const OrbitalMissionItem& OrbitalSequenceToModel::GetItem()
{
   mItem.SetSavedData(mData);
   return mItem;
}

std::unique_ptr<OrbitalMissionModel> OrbitalSequenceToModel::GetModel()
{
   return std::move(mModelPtr);
}

void OrbitalSequenceToModel::VisitMissionSequence(WsfOrbitalMissionSequence* aSequencePtr)
{
   mModelPtr = ut::make_unique<OrbitalMissionModel>();

   CommonSetupNoFinite(aSequencePtr);
   mModelPtr->SetItemData(mModelPtr->GetSequenceIndex(), mData);

   auto& events = aSequencePtr->GetSequence();
   for (auto& eventPtr : events)
   {
      OrbitalSequenceToModel converter{};
      eventPtr->Accept(&converter);
      mModelPtr->AppendItem(converter.GetItem(), mModelPtr->GetSequenceIndex());
   }
}

void OrbitalSequenceToModel::VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cECCENTRICITY] = QString::number(aEventPtr->GetEccentricity());
}

void OrbitalSequenceToModel::VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cINCLINATION] = FromValue(aEventPtr->GetInclination());
}

void OrbitalSequenceToModel::VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cRAAN] = FromValue(aEventPtr->GetRAAN());
}

void OrbitalSequenceToModel::VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cRAAN]        = FromValue(aEventPtr->GetRAAN());
   mData[EventConfigKeys::cINCLINATION] = FromValue(aEventPtr->GetInclination());
}

void OrbitalSequenceToModel::VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr)
{
   CommonSetup(aEventPtr);
   auto option = aEventPtr->GetInputType();
   switch (option)
   {
   case WsfOrbitalManeuvers::ChangeSemiMajorAxis::InputOption::cFINAL_SMA: // NOTE the fall-through.
   case WsfOrbitalManeuvers::ChangeSemiMajorAxis::InputOption::cRADIUS:
      mData[EventConfigKeys::cSEMI_MAJOR_AXIS] = FromValue(aEventPtr->GetFinalSMA());
      break;
   case WsfOrbitalManeuvers::ChangeSemiMajorAxis::InputOption::cAPOAPSIS_ALT:
      mData[EventConfigKeys::cAPOAPSIS_ALTITUDE] = FromValue(aEventPtr->GetApoapsisAltitude());
      break;
   case WsfOrbitalManeuvers::ChangeSemiMajorAxis::InputOption::cPERIAPSIS_ALT:
      mData[EventConfigKeys::cPERIAPSIS_ALTITUDE] = FromValue(aEventPtr->GetPeriapsisAltitude());
      break;
   }
}

void OrbitalSequenceToModel::VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr)
{
   CommonSetup(aEventPtr);
}

void OrbitalSequenceToModel::VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr)
{
   SetupEventOption(aEventPtr);

   OrbitalSequenceToModel transformA{};
   aEventPtr->GetFirstEvent()->Accept(&transformA);

   OrbitalSequenceToModel transformB{};
   aEventPtr->GetSecondEvent()->Accept(&transformB);

   mItem.AddChild(transformA.GetItem().GetSavedData());
   mItem.AddChild(transformB.GetItem().GetSavedData());
}

void OrbitalSequenceToModel::VisitDeltaV(WsfOrbitalManeuvers::DeltaV* aEventPtr)
{
   CommonSetup(aEventPtr);

   QJsonObject delta{};
   if (WsfOrbitalManeuvers::DeltaV::IsFrameSupported(aEventPtr->GetDeltaV_Frame()))
   {
      delta[EventConfigKeys::cOPTION] =
         QString::fromStdString(wsf::space::OrbitalReferenceFrameToIdentifier(aEventPtr->GetDeltaV_Frame()));
      delta[EventConfigKeys::cDV_X] = FromValue<UtSpeedValue>(aEventPtr->GetDeltaV_Component(0));
      delta[EventConfigKeys::cDV_Y] = FromValue<UtSpeedValue>(aEventPtr->GetDeltaV_Component(1));
      delta[EventConfigKeys::cDV_Z] = FromValue<UtSpeedValue>(aEventPtr->GetDeltaV_Component(2));
   }
   mData[EventConfigKeys::cDELTA_V] = delta;
}

void OrbitalSequenceToModel::VisitDrift(WsfOrbitalManeuvers::Drift* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
   mData[EventConfigKeys::cDRIFT_RATE] = FromValue(aEventPtr->GetDriftRate());
}

void OrbitalSequenceToModel::VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (aEventPtr->IsRadiusInput())
   {
      mData[EventConfigKeys::cFINAL_RADIUS] = FromValue(aEventPtr->GetFinalRadius());
   }
   else
   {
      mData[EventConfigKeys::cFINAL_SMA] = FromValue(aEventPtr->GetFinalSMA());
   }
}

void OrbitalSequenceToModel::VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
   ProcessTargetOptions(aEventPtr->GetTargetPointOptions());
}

void OrbitalSequenceToModel::VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr)
{
   CommonSetup(aEventPtr);
   QJsonObject targetObject{};
   targetObject[EventConfigKeys::cPLATFORM] = QString::fromStdString(aEventPtr->GetPlatformName());
   mData[EventConfigKeys::cTARGET]          = targetObject;
}

void OrbitalSequenceToModel::VisitNaturalMotionCircumnavigation(WsfOrbitalManeuvers::NaturalMotionCircumnavigation* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
   mData[EventConfigKeys::cORBIT_SIZE]    = FromValue(aEventPtr->GetOrbitSize());
   mData[EventConfigKeys::cORBIT_PHASE]   = FromValue(aEventPtr->GetOrbitPhase());
   mData[EventConfigKeys::cOOP_AMPLITUDE] = FromValue(aEventPtr->GetOutOfPlaneAmplitude());
   mData[EventConfigKeys::cOOP_PHASE]     = FromValue(aEventPtr->GetOutOfPlanePhase());

   // NOTE: Will never have to convert from Track to model, so only platform is relevant here.
   QJsonObject targetObject{};
   targetObject[EventConfigKeys::cPLATFORM] = QString::fromStdString(aEventPtr->GetPlatformName());
   mData[EventConfigKeys::cTARGET]          = targetObject;
}

void OrbitalSequenceToModel::VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cDELTA_V] = FromValue(aEventPtr->GetManeuverDeltaV());
}

void OrbitalSequenceToModel::VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
   ProcessTargetOptions(aEventPtr->GetTargetPointOptions());
}

void OrbitalSequenceToModel::VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr)
{
   CommonSetup(aEventPtr);
   mData[EventConfigKeys::cDELTA_V] = FromValue(aEventPtr->GetManeuverDeltaV());
}

void OrbitalSequenceToModel::VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
   ProcessTargetOptions(aEventPtr->GetTargetPointOptions());
}

void OrbitalSequenceToModel::VisitTeardrop(WsfOrbitalManeuvers::Teardrop* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);

   mData[EventConfigKeys::cR_AT_POCA]    = FromValue(aEventPtr->GetRadialOffsetAtPOCA());
   mData[EventConfigKeys::cPERIOD]       = FromValue(aEventPtr->GetTeardropPeriod());
   mData[EventConfigKeys::cTIME_TO_POCA] = FromValue(aEventPtr->GetTimeToPOCA());
   mData[EventConfigKeys::cREPETITIONS]  = QString::number(aEventPtr->GetRepetitions());

   // NOTE: Will never have to convert from Track to model, so only platform is relevant here.
   QJsonObject targetObject{};
   targetObject[EventConfigKeys::cPLATFORM] = QString::fromStdString(aEventPtr->GetPlatformName());
   mData[EventConfigKeys::cTARGET]          = targetObject;
}

void OrbitalSequenceToModel::VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr)
{
   CommonSetupNoFinite(aEventPtr);
   if (aEventPtr->IsFinite())
   {
      mData[EventConfigKeys::cFINITE] = aEventPtr->IsFinite();
   }
   mData[EventConfigKeys::cDURATION]        = FromValue(aEventPtr->GetDuration());
   mData[EventConfigKeys::cUPDATE_INTERVAL] = FromValue(aEventPtr->GetUpdateInterval());
   mData[EventConfigKeys::cON_COMPLETE] =
      StripBlock(QString("on_complete"), QString::fromStdString(aEventPtr->GetOnCompleteInput()));
   mData[EventConfigKeys::cON_INITIALIZE] =
      StripBlock(QString("on_initialize"), QString::fromStdString(aEventPtr->GetOnInitializeInput()));
   mData[EventConfigKeys::cON_UPDATE] =
      StripBlock(QString("on_update"), QString::fromStdString(aEventPtr->GetOnUpdateInput()));
   mData[EventConfigKeys::cIS_COMPLETE] =
      StripBlock(QString("is_complete"), QString::fromStdString(aEventPtr->GetIsCompleteInput()));
}

void OrbitalSequenceToModel::VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr)
{
   CommonSetup(aEventPtr);
}

void OrbitalSequenceToModel::VisitOrbitalAttitudeChangeEvent(WsfOrbitalAttitudeChangeEvent* aEventPtr)
{
   CommonSetup(aEventPtr);

   QJsonObject orientationData{};

   wsf::space::Orientation* orientationPtr = aEventPtr->GetOrientation();
   if (orientationPtr)
   {
      orientationData[EventConfigKeys::cOPTION] = QString::fromStdString(orientationPtr->GetType());

      wsf::space::OrientationEntity* entityRefPtr = dynamic_cast<wsf::space::OrientationEntity*>(orientationPtr);
      if (entityRefPtr)
      {
         QJsonObject targetObject{};
         targetObject[EventConfigKeys::cPLATFORM]  = QString::fromStdString(entityRefPtr->GetEntityName());
         orientationData[EventConfigKeys::cTARGET] = targetObject;
      }
      else
      {
         wsf::space::OrientationGeoPoint* geoRefPtr = dynamic_cast<wsf::space::OrientationGeoPoint*>(orientationPtr);
         if (geoRefPtr)
         {
            orientationData[EventConfigKeys::cGEO_POINT] = QString::fromStdString(geoRefPtr->GetGeoPointName());
         }
      }
   }

   mData[EventConfigKeys::cORIENTATION] = orientationData;
}

void OrbitalSequenceToModel::CommonSetup(WsfOrbitalEvent* aEventPtr)
{
   CommonSetupNoFinite(aEventPtr);
   ProcessFinite(aEventPtr);
}

void OrbitalSequenceToModel::SetupEventOption(WsfOrbitalEvent* aEventPtr)
{
   mData                           = QJsonObject{};
   mData[EventConfigKeys::cOPTION] = QString::fromStdString(aEventPtr->GetTypeId()).toLower();
}

void OrbitalSequenceToModel::CommonSetupNoFinite(WsfOrbitalEvent* aEventPtr)
{
   SetupEventOption(aEventPtr);
   ProcessCondition(aEventPtr->GetCondition());
}

void OrbitalSequenceToModel::ProcessCondition(wsf::space::OrbitalPropagatorCondition& aCondition)
{
   ConditionToModel vtor{};
   aCondition.Accept(vtor);
   mData[EventConfigKeys::cCONDITION] = vtor.GetConfiguration();
}

void OrbitalSequenceToModel::ProcessFinite(const WsfOrbitalEvent* aEventPtr)
{
   if (aEventPtr->IsFinite())
   {
      QJsonObject obj{};
      obj[EventConfigKeys::cDURATION]        = FromValue(aEventPtr->GetDuration());
      obj[EventConfigKeys::cUPDATE_INTERVAL] = FromValue(aEventPtr->GetUpdateInterval());

      mData[EventConfigKeys::cFINITE] = obj;
   }
}

void OrbitalSequenceToModel::ProcessTargetOptions(const wsf::space::TargetPointOptions& aOptions)
{
   ProcessOffset(aOptions);

   QJsonObject targetObject{};
   if (aOptions.HasPlatformTarget())
   {
      targetObject[EventConfigKeys::cPLATFORM] = QString::fromStdString(aOptions.GetPlatformName());
   }
   if (aOptions.HasLibrationPointTarget())
   {
      targetObject[EventConfigKeys::cLIBRATION_SYSTEM] =
         QString::fromStdString(wsf::space::LibrationPoint::GetIdentifierFromSystem(aOptions.GetLP_System()));
      targetObject[EventConfigKeys::cLIBRATION_POINT] =
         QString::fromStdString(wsf::space::LibrationPoint::GetIdentifierFromPoint(aOptions.GetLP_Point()));
   }
   if (aOptions.HasKinematicStateTarget())
   {
      QJsonObject state;

      UtVec3d pos = aOptions.GetKinematicStatePositionInUnits();
      UtVec3d vel = aOptions.GetKinematicStateVelocityInUnits();
      if (!UtMath::NearlyZero(pos.Magnitude()))
      {
         QJsonObject pObj;
         pObj[EventConfigKeys::cX_COMPONENT] = FromValue(UtLengthValue{pos[0], aOptions.GetKinematicStatePositionUnits()});
         pObj[EventConfigKeys::cY_COMPONENT] = FromValue(UtLengthValue{pos[1], aOptions.GetKinematicStatePositionUnits()});
         pObj[EventConfigKeys::cZ_COMPONENT] = FromValue(UtLengthValue{pos[2], aOptions.GetKinematicStatePositionUnits()});
         state[EventConfigKeys::cSTATE_POSITION] = pObj;
      }
      if (!UtMath::NearlyZero(vel.Magnitude()))
      {
         QJsonObject vObj;
         vObj[EventConfigKeys::cX_COMPONENT] = FromValue(UtSpeedValue{vel[0], aOptions.GetKinematicStateVelocityUnits()});
         vObj[EventConfigKeys::cY_COMPONENT] = FromValue(UtSpeedValue{vel[1], aOptions.GetKinematicStateVelocityUnits()});
         vObj[EventConfigKeys::cZ_COMPONENT] = FromValue(UtSpeedValue{vel[2], aOptions.GetKinematicStateVelocityUnits()});
         state[EventConfigKeys::cSTATE_VELOCITY] = vObj;
      }

      targetObject[EventConfigKeys::cKINEMATIC_STATE] = state;
   }
   // Will never need to convert from an event with a track target

   mData[EventConfigKeys::cTARGET] = targetObject;
}

void OrbitalSequenceToModel::ProcessOffset(const wsf::space::TargetPointOptions& aOptions)
{
   if (aOptions.HasPositionOffset())
   {
      UtVec3d     offsetVec = aOptions.GetPositionOffsetInUnits();
      QJsonObject offset{};
      QString     units = QString(" ") + QString::fromStdString(UtUnitLength::FindUnitName(UtUnitLength::cMETERS));
      offset[EventConfigKeys::cOFFSET_X] = QString::number(offsetVec[0]) + units;
      offset[EventConfigKeys::cOFFSET_Y] = QString::number(offsetVec[1]) + units;
      offset[EventConfigKeys::cOFFSET_Z] = QString::number(offsetVec[2]) + units;
      auto frameId = wsf::space::OrbitalReferenceFrameToIdentifier(aOptions.GetPositionOffsetFrame());
      offset[EventConfigKeys::cFRAME]          = QString::fromStdString(frameId);
      mData[EventConfigKeys::cPOSITION_OFFSET] = offset;
   }

   if (aOptions.HasVelocityOffset())
   {
      UtVec3d     offsetVec = aOptions.GetVelocityOffsetInUnits();
      QJsonObject offset{};
      QString units = QString(" ") + QString::fromStdString(UtUnitSpeed::FindUnitName(UtUnitSpeed::cMETERS_PER_SECOND));
      offset[EventConfigKeys::cOFFSET_X] = QString::number(offsetVec[0]) + units;
      offset[EventConfigKeys::cOFFSET_Y] = QString::number(offsetVec[1]) + units;
      offset[EventConfigKeys::cOFFSET_Z] = QString::number(offsetVec[2]) + units;
      auto frameId = wsf::space::OrbitalReferenceFrameToIdentifier(aOptions.GetVelocityOffsetFrame());
      offset[EventConfigKeys::cFRAME]          = QString::fromStdString(frameId);
      mData[EventConfigKeys::cVELOCITY_OFFSET] = offset;
   }

   if (aOptions.HasOffsetTime())
   {
      mData[EventConfigKeys::cOFFSET_TIME] = FromValue(aOptions.GetOffsetTime());
   }

   if (aOptions.HasLagTime())
   {
      mData[EventConfigKeys::cLAG_TIME] = FromValue(aOptions.GetLagTime());
   }
}

void OrbitalSequenceToModel::ProcessOptimizationCost(const wsf::space::OrbitalTargetingCost* aCostPtr,
                                                     QJsonObject&                            aOptimizeObject)
{
   const auto* blendedPtr = dynamic_cast<const wsf::space::BlendedCost*>(aCostPtr);
   if (blendedPtr)
   {
      QJsonObject selection{};
      selection[EventConfigKeys::cOPTION] = EventConfigKeys::cBLENDED_COST;
      if (blendedPtr->GetA_Value() != 0.0)
      {
         selection[EventConfigKeys::cA_PARAMETER] = QString::number(blendedPtr->GetA_Value());
      }
      if (blendedPtr->GetB_Value() != 0.0)
      {
         selection[EventConfigKeys::cB_PARAMETER] = QString::number(blendedPtr->GetB_Value());
      }
      if (blendedPtr->GetC_Value() != 0.0)
      {
         selection[EventConfigKeys::cC_PARAMETER] = QString::number(blendedPtr->GetC_Value());
      }
      aOptimizeObject[EventConfigKeys::cCOST_SELECTION] = selection;
   }
}

QString OrbitalSequenceToModel::StripBlock(const QString& aBlockName, const QString& aInput)
{
   QString tester = aInput.trimmed();

   // Search for <aBlockName> at the start.
   int startIndex{0};
   int numToKeep{tester.size()};
   if (tester.startsWith(aBlockName))
   {
      startIndex = aBlockName.size();
      numToKeep -= aBlockName.size();
   }

   // Search for end_<aBlockName> at the end.
   QString endTag = QString("end_") + aBlockName;
   if (tester.endsWith(endTag))
   {
      numToKeep -= endTag.size();
   }

   // Return the trimmed stripped version of the input.
   return tester.mid(startIndex, numToKeep).trimmed();
}

} // namespace spaceg

} // namespace wsfg
