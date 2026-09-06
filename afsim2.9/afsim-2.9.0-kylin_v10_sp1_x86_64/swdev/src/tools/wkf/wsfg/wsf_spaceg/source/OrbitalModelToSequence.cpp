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

#include "OrbitalModelToSequence.hpp"

#include <sstream>
#include <stdexcept>

#include "OrbitalMissionModel.hpp"
#include "UtMemory.hpp"
#include "WsfOrbitalMissionEventTypes.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfOrbitalTargetingCost.hpp"
#include "WsfOrbitalTargetingCostTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceOrbitalPropagatorConditionTypes.hpp"
#include "WsfSpaceOrbitalPropagatorConditionVisitor.hpp"

namespace
{
class ModelToCondition : public wsf::space::OrbitalPropagatorConditionVisitor
{
public:
   ModelToCondition(const QJsonObject& aConfiguration)
      : mConfiguration(aConfiguration)
   {
   }

   void Accept(wsf::space::ApoapsisCondition& aCondition) override { HandleOrbitNumber(aCondition); }
   void Accept(wsf::space::AscendingNodeCondition& aCondition) override { HandleOrbitNumber(aCondition); }

   void Accept(wsf::space::AscendingRadiusCondition& aCondition) override
   {
      HandleOrbitNumber(aCondition);
      QString radius = mConfiguration.constFind(wsfg::spaceg::EventConfigKeys::cRADIUS)->toString();
      aCondition.SetRadius(wsfg::spaceg::StringToUnitHelper<UtLengthValue>(radius));
   }

   void Accept(wsf::space::DescendingNodeCondition& aCondition) override { HandleOrbitNumber(aCondition); }

   void Accept(wsf::space::DescendingRadiusCondition& aCondition) override
   {
      HandleOrbitNumber(aCondition);
      QString radius = mConfiguration.constFind(wsfg::spaceg::EventConfigKeys::cRADIUS)->toString();
      aCondition.SetRadius(wsfg::spaceg::StringToUnitHelper<UtLengthValue>(radius));
   }

   void Accept(wsf::space::EclipseEntryCondition& aCondition) override { HandleOrbitNumber(aCondition); }
   void Accept(wsf::space::EclipseExitCondition& aCondition) override { HandleOrbitNumber(aCondition); }
   void Accept(wsf::space::NoneCondition& aCondition) override { HandleOrbitNumber(aCondition); }
   void Accept(wsf::space::NorthernIntersectionCondition& aCondition) override { HandleOrbitNumber(aCondition); }
   void Accept(wsf::space::PeriapsisCondition& aCondition) override { HandleOrbitNumber(aCondition); }

   void Accept(wsf::space::RelativeTimeCondition& aCondition) override
   {
      HandleOrbitNumber(aCondition);
      QString relTime = mConfiguration.constFind(wsfg::spaceg::EventConfigKeys::cRELATIVE_TIME)->toString();
      aCondition.SetOffsetTime(wsfg::spaceg::StringToUnitHelper<UtTimeValue>(relTime));
   }
   void Accept(wsf::space::SouthernIntersectionCondition& aCondition) override { HandleOrbitNumber(aCondition); }

private:
   void HandleOrbitNumber(wsf::space::OrbitalPropagatorCondition& aCondition)
   {
      int orbitNumber = mConfiguration.constFind(wsfg::spaceg::EventConfigKeys::cORBIT)->toString().toInt();
      if (orbitNumber != 0)
      {
         aCondition.SetOrbitNumber(orbitNumber);
      }
   }

   const QJsonObject& mConfiguration;
};

} // namespace

namespace wsfg
{
namespace spaceg
{
OrbitalModelToSequence::OrbitalModelToSequence(const WsfScenario& aScenario)
   : mScenario(aScenario)
{
}

OrbitalModelToSequence::~OrbitalModelToSequence() {}

//! Return the scenario associated with this object.
const WsfScenario& OrbitalModelToSequence::GetScenario() const
{
   return mScenario;
}

std::unique_ptr<WsfOrbitalMissionSequence> OrbitalModelToSequence::Transform(const OrbitalMissionData& aMission)
{
   std::unique_ptr<WsfOrbitalMissionSequence> sequencePtr = ut::make_unique<WsfOrbitalMissionSequence>(mScenario);

   Visitor transform{*this, aMission, 0};
   sequencePtr->Accept(&transform);

   return sequencePtr;
}

void OrbitalModelToSequence::Visitor::VisitMissionSequence(WsfOrbitalMissionSequence* aSequencePtr)
{
   ConditionSetup(aSequencePtr);

   const WsfOrbitalMissionEventTypes& eventFactory = WsfOrbitalMissionEventTypes::Get(mTransformer.GetScenario());

   auto operation = [&eventFactory, aSequencePtr, this](int aOrderedIndex, int aParent, int aRow, QJsonObject aData)
   {
      // There are a couple things that might go wrong here, but the end result would be that
      // currentEventType would be a string not containing a valid orbital event type name, so we
      // would enter the else branch below, and throw an exception.
      QString currentEventType = aData.constFind(EventConfigKeys::cOPTION)->toString().toUpper();
      std::unique_ptr<WsfOrbitalEvent> eventPtr{eventFactory.Clone(currentEventType.toStdString())};
      if (eventPtr != nullptr)
      {
         Visitor transform{mTransformer, mMission, aOrderedIndex};
         eventPtr->Accept(&transform);
         aSequencePtr->AddMissionEvent(std::move(eventPtr));
      }
      else
      {
         // Throw an exception here because something really has gone wrong if the factory fails
         // to produce the event.
         std::stringstream ss{};
         ss << "Unable to create Orbital Event of type '" << currentEventType.toStdString() << "'.";
         throw std::runtime_error{ss.str()};
      }
   };
   mMission.ApplyToChildrenOf(mOrderedIndex, operation);
}

void OrbitalModelToSequence::Visitor::VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cECCENTRICITY))
   {
      aEventPtr->SetEccentricity(mData.constFind(EventConfigKeys::cECCENTRICITY)->toString().toDouble());
   }
}

void OrbitalModelToSequence::Visitor::VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cINCLINATION))
   {
      aEventPtr->SetInclination(
         StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cINCLINATION)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cRAAN))
   {
      aEventPtr->SetRAAN(StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cRAAN)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cINCLINATION))
   {
      aEventPtr->SetInclination(
         StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cINCLINATION)->toString()));
   }
   if (mData.contains(EventConfigKeys::cRAAN))
   {
      aEventPtr->SetRAAN(StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cRAAN)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cAPOAPSIS_ALTITUDE))
   {
      aEventPtr->SetApoapsisAltitude(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cAPOAPSIS_ALTITUDE)->toString()));
   }
   else if (mData.contains(EventConfigKeys::cPERIAPSIS_ALTITUDE))
   {
      aEventPtr->SetPeriapsisAltitude(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cPERIAPSIS_ALTITUDE)->toString()));
   }
   else if (mData.contains(EventConfigKeys::cSEMI_MAJOR_AXIS))
   {
      aEventPtr->SetFinalSMA(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cSEMI_MAJOR_AXIS)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr)
{
   CommonSetup(aEventPtr);
}

void OrbitalModelToSequence::Visitor::VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr)
{
   CommonSetup(aEventPtr);

   const WsfOrbitalMissionEventTypes& eventFactory = WsfOrbitalMissionEventTypes::Get(mTransformer.GetScenario());

   auto operation = [&eventFactory, aEventPtr, this](int aOrderedIndex, int aParent, int aRow, QJsonObject aData)
   {
      // There are a couple things that might go wrong here, but the end result would be that
      // currentEventType would be a string not containing a valid orbital event type name, so we
      // would enter the else branch below, and throw an exception.
      QString currentEventType = aData.constFind(EventConfigKeys::cOPTION)->toString().toUpper();
      std::unique_ptr<WsfOrbitalEvent> subEventPtr{eventFactory.Clone(currentEventType.toStdString())};
      if (subEventPtr != nullptr)
      {
         Visitor transform{mTransformer, mMission, aOrderedIndex};
         subEventPtr->Accept(&transform);
         if (aRow == 0)
         {
            aEventPtr->SetFirstEvent(std::move(subEventPtr));
         }
         else if (aRow == 1)
         {
            aEventPtr->SetSecondEvent(std::move(subEventPtr));
         }
      }
      else
      {
         // Throw an exception here because something really has gone wrong if the factory fails
         // to produce the event.
         std::stringstream ss{};
         ss << "Unable to create Orbital Event of type '" << currentEventType.toStdString() << "'.";
         throw std::runtime_error{ss.str()};
      }
   };
   mMission.ApplyToChildrenOf(mOrderedIndex, operation);
}

void OrbitalModelToSequence::Visitor::VisitDeltaV(WsfOrbitalManeuvers::DeltaV* aEventPtr)
{
   CommonSetup(aEventPtr);

   auto delta = mData.constFind(EventConfigKeys::cDELTA_V);
   if (delta != mData.constEnd() && delta->isObject())
   {
      QJsonObject deltaObject = delta->toObject();

      if (deltaObject.contains(EventConfigKeys::cOPTION))
      {
         auto frame = wsf::space::OrbitalReferenceFrameFromIdentifier(
            deltaObject.constFind(EventConfigKeys::cOPTION)->toString().toStdString());
         aEventPtr->SetDeltaV_Frame(frame);

         UtSpeedValue component{};
         component = StringToUnitHelper<UtSpeedValue>(deltaObject.constFind(EventConfigKeys::cDV_X)->toString());
         aEventPtr->SetDeltaV_Component(0, component);

         component = StringToUnitHelper<UtSpeedValue>(deltaObject.constFind(EventConfigKeys::cDV_Y)->toString());
         aEventPtr->SetDeltaV_Component(1, component);

         component = StringToUnitHelper<UtSpeedValue>(deltaObject.constFind(EventConfigKeys::cDV_Z)->toString());
         aEventPtr->SetDeltaV_Component(2, component);
      }
   }
}

void OrbitalModelToSequence::Visitor::VisitDrift(WsfOrbitalManeuvers::Drift* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessCommonTargetingOptions(aEventPtr);
   if (mData.contains(EventConfigKeys::cDRIFT_RATE))
   {
      aEventPtr->SetDriftRate(
         StringToUnitHelper<UtAngularRateValue>(mData.constFind(EventConfigKeys::cDRIFT_RATE)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cFINAL_RADIUS))
   {
      aEventPtr->SetFinalRadius(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cFINAL_RADIUS)->toString()));
   }
   else if (mData.contains(EventConfigKeys::cFINAL_SMA))
   {
      aEventPtr->SetFinalSMA(StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cFINAL_SMA)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
}

void OrbitalModelToSequence::Visitor::VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cTARGET))
   {
      QJsonObject targetObj = mData.constFind(EventConfigKeys::cTARGET)->toObject();
      if (targetObj.contains(EventConfigKeys::cPLATFORM))
      {
         aEventPtr->SetPlatformName(targetObj.constFind(EventConfigKeys::cPLATFORM)->toString().toStdString());
      }
      else if (targetObj.contains(EventConfigKeys::cTRACK))
      {
         aEventPtr->SetLocalTrackId(TrackIdFromString(targetObj.constFind(EventConfigKeys::cTRACK)->toString()));
      }
   }
}

void OrbitalModelToSequence::Visitor::VisitNaturalMotionCircumnavigation(
   WsfOrbitalManeuvers::NaturalMotionCircumnavigation* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessCommonTargetingOptions(aEventPtr);
   if (mData.contains(EventConfigKeys::cTARGET))
   {
      QJsonObject targetObject = mData.constFind(EventConfigKeys::cTARGET)->toObject();
      if (targetObject.contains(EventConfigKeys::cPLATFORM))
      {
         aEventPtr->SetPlatformName(targetObject.constFind(EventConfigKeys::cPLATFORM)->toString().toStdString());
      }
      else if (targetObject.contains(EventConfigKeys::cTRACK))
      {
         aEventPtr->SetLocalTrackId(TrackIdFromString(targetObject.constFind(EventConfigKeys::cTRACK)->toString()));
      }
   }
   if (mData.contains(EventConfigKeys::cORBIT_SIZE))
   {
      aEventPtr->SetOrbitSize(StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cORBIT_SIZE)->toString()));
   }
   if (mData.contains(EventConfigKeys::cORBIT_PHASE))
   {
      aEventPtr->SetOrbitPhase(
         StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cORBIT_PHASE)->toString()));
   }
   if (mData.contains(EventConfigKeys::cOOP_AMPLITUDE))
   {
      aEventPtr->SetOutOfPlaneAmplitude(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cOOP_AMPLITUDE)->toString()));
   }
   if (mData.contains(EventConfigKeys::cOOP_PHASE))
   {
      aEventPtr->SetOutOfPlanePhase(
         StringToUnitHelper<UtAngleValue>(mData.constFind(EventConfigKeys::cOOP_PHASE)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cDELTA_V))
   {
      aEventPtr->SetManeuverDeltaV(
         StringToUnitHelper<UtSpeedValue>(mData.constFind(EventConfigKeys::cDELTA_V)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
}

void OrbitalModelToSequence::Visitor::VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr)
{
   CommonSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cDELTA_V))
   {
      aEventPtr->SetManeuverDeltaV(
         StringToUnitHelper<UtSpeedValue>(mData.constFind(EventConfigKeys::cDELTA_V)->toString()));
   }
}

void OrbitalModelToSequence::Visitor::VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessTargetingEvent(aEventPtr);
}

void OrbitalModelToSequence::Visitor::VisitTeardrop(WsfOrbitalManeuvers::Teardrop* aEventPtr)
{
   CommonSetup(aEventPtr);
   ProcessCommonTargetingOptions(aEventPtr);
   if (mData.contains(EventConfigKeys::cTARGET))
   {
      QJsonObject targetObject = mData.constFind(EventConfigKeys::cTARGET)->toObject();
      if (targetObject.contains(EventConfigKeys::cPLATFORM))
      {
         aEventPtr->SetPlatformName(targetObject.constFind(EventConfigKeys::cPLATFORM)->toString().toStdString());
      }
      else if (targetObject.contains(EventConfigKeys::cTRACK))
      {
         aEventPtr->SetLocalTrackId(TrackIdFromString(targetObject.constFind(EventConfigKeys::cTRACK)->toString()));
      }
   }
   if (mData.contains(EventConfigKeys::cR_AT_POCA))
   {
      aEventPtr->SetRadialOffsetAtPOCA(
         StringToUnitHelper<UtLengthValue>(mData.constFind(EventConfigKeys::cR_AT_POCA)->toString()));
   }
   if (mData.contains(EventConfigKeys::cPERIOD))
   {
      aEventPtr->SetTeardropPeriod(StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cPERIOD)->toString()));
   }
   if (mData.contains(EventConfigKeys::cTIME_TO_POCA))
   {
      aEventPtr->SetTimeToPOCA(
         StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cTIME_TO_POCA)->toString()));
   }
   if (mData.contains(EventConfigKeys::cREPETITIONS))
   {
      aEventPtr->SetRepetitions(mData.constFind(EventConfigKeys::cREPETITIONS)->toString().toUInt());
   }
}

void OrbitalModelToSequence::Visitor::VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr)
{
   ConditionSetup(aEventPtr);
   if (mData.contains(EventConfigKeys::cFINITE))
   {
      aEventPtr->SetFinite(mData.constFind(EventConfigKeys::cFINITE)->toBool());
   }
   if (mData.contains(EventConfigKeys::cDURATION))
   {
      aEventPtr->SetOnlyDuration(StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cDURATION)->toString()));
   }
   if (mData.contains(EventConfigKeys::cUPDATE_INTERVAL))
   {
      aEventPtr->SetUpdateInterval(
         StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cUPDATE_INTERVAL)->toString()));
   }
   if (mData.contains(EventConfigKeys::cON_COMPLETE))
   {
      aEventPtr->SetOnCompleteInput(mData.constFind(EventConfigKeys::cON_COMPLETE)->toString().toStdString());
   }
   if (mData.contains(EventConfigKeys::cON_INITIALIZE))
   {
      aEventPtr->SetOnInitializeInput(mData.constFind(EventConfigKeys::cON_INITIALIZE)->toString().toStdString());
   }
   if (mData.contains(EventConfigKeys::cON_UPDATE))
   {
      aEventPtr->SetOnUpdateInput(mData.constFind(EventConfigKeys::cON_UPDATE)->toString().toStdString());
   }
   if (mData.contains(EventConfigKeys::cIS_COMPLETE))
   {
      aEventPtr->SetIsCompleteInput(mData.constFind(EventConfigKeys::cIS_COMPLETE)->toString().toStdString());
   }
}

void OrbitalModelToSequence::Visitor::VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr)
{
   CommonSetup(aEventPtr);
}

void OrbitalModelToSequence::Visitor::VisitOrbitalAttitudeChangeEvent(WsfOrbitalAttitudeChangeEvent* aEventPtr)
{
   CommonSetup(aEventPtr);

   auto orientationIter = mData.constFind(EventConfigKeys::cORIENTATION);
   if (orientationIter->isObject())
   {
      QJsonObject orientationObj = orientationIter->toObject();

      if (orientationObj.contains(EventConfigKeys::cOPTION))
      {
         std::string orientationType = orientationObj.constFind(EventConfigKeys::cOPTION)->toString().toStdString();
         auto        orientationPtr  = wsf::space::Orientation::Factory(orientationType);

         // At most one of the two special classes of orientation will be relevant.
         wsf::space::OrientationEntity* entityPtr = dynamic_cast<wsf::space::OrientationEntity*>(orientationPtr.get());
         if (entityPtr)
         {
            auto targetIter = orientationObj.constFind(EventConfigKeys::cTARGET);
            if (targetIter != orientationObj.constEnd())
            {
               QJsonObject targetObject = targetIter->toObject();
               if (targetObject.contains(EventConfigKeys::cPLATFORM))
               {
                  entityPtr->SetEntityName(targetObject.constFind(EventConfigKeys::cPLATFORM)->toString().toStdString());
               }
               else if (targetObject.contains(EventConfigKeys::cTRACK))
               {
                  entityPtr->SetTrackId(TrackIdFromString(targetObject.constFind(EventConfigKeys::cTRACK)->toString()));
               }
            }
         }
         else
         {
            wsf::space::OrientationGeoPoint* geoPtPtr =
               dynamic_cast<wsf::space::OrientationGeoPoint*>(orientationPtr.get());
            if (geoPtPtr)
            {
               auto geoPtIter = orientationObj.constFind(EventConfigKeys::cGEO_POINT);
               if (geoPtIter != orientationObj.constEnd())
               {
                  geoPtPtr->SetGeoPointName(geoPtIter->toString().toStdString());
               }
            }
         }

         aEventPtr->SetOrientation(std::move(orientationPtr));
      }
   }
}

void OrbitalModelToSequence::Visitor::CommonSetup(WsfOrbitalEvent* aEventPtr) const
{
   ConditionSetup(aEventPtr);
   FiniteSetup(aEventPtr);
}

void OrbitalModelToSequence::Visitor::ConditionSetup(WsfOrbitalEvent* aEventPtr) const
{
   auto constraintIter = mData.constFind(EventConfigKeys::cCONDITION);
   if (constraintIter->isObject())
   {
      QJsonObject constraintObj = constraintIter->toObject();

      if (constraintObj.contains(EventConfigKeys::cOPTION))
      {
         auto conditionType = ConditionTypeFromString(constraintObj.constFind(EventConfigKeys::cOPTION)->toString());
         const auto& types  = wsf::space::OrbitalPropagatorConditionTypes::Get(mTransformer.GetScenario());
         auto        conditionPtr = std::unique_ptr<wsf::space::OrbitalPropagatorCondition>(types.Clone(conditionType));

         ModelToCondition vtor(constraintObj);
         conditionPtr->Accept(vtor);

         aEventPtr->SetCondition(std::move(conditionPtr));
      }
   }
}

std::string OrbitalModelToSequence::Visitor::ConditionTypeFromString(const QString& aType)
{
   // The possibility that this throws is intentional. It should be that this always get an existing type; if not it
   // should fail hard.
   return EventConfigKeys::cCONDITION_STRING_TO_TYPE.at(aType);
}

void OrbitalModelToSequence::Visitor::FiniteSetup(WsfOrbitalEvent* aEventPtr) const
{
   auto finiteIter = mData.constFind(EventConfigKeys::cFINITE);
   if (finiteIter->isObject())
   {
      QJsonObject finiteObj = finiteIter->toObject();
      aEventPtr->SetFinite(true);

      if (finiteObj.contains(EventConfigKeys::cDURATION))
      {
         aEventPtr->SetOnlyDuration(
            StringToUnitHelper<UtTimeValue>(finiteObj.constFind(EventConfigKeys::cDURATION)->toString()));
      }
      if (finiteObj.contains(EventConfigKeys::cUPDATE_INTERVAL))
      {
         aEventPtr->SetUpdateInterval(
            StringToUnitHelper<UtTimeValue>(finiteObj.constFind(EventConfigKeys::cUPDATE_INTERVAL)->toString()));
      }
   }
   else
   {
      aEventPtr->SetFinite(false);
   }
}

WsfTrackId OrbitalModelToSequence::Visitor::TrackIdFromString(const QString& aTrackString)
{
   WsfTrackId retval{};
   auto       parts = aTrackString.split(":");
   if (parts.size() == 2)
   {
      retval.SetOwningPlatformId(parts[0].toStdString());
      bool ok;
      int  ltid = parts[1].toInt(&ok);
      if (ok)
      {
         retval.SetLocalTrackNumber(ltid);
      }
      else
      {
         retval.Null();
      }
   }
   return retval;
}

std::unique_ptr<wsf::space::OrbitalTargetingCost>
OrbitalModelToSequence::Visitor::ProcessCostFunction(const QJsonObject& aStateObject) const
{
   std::unique_ptr<wsf::space::OrbitalTargetingCost> retvalPtr{};

   auto selection = aStateObject.constFind(EventConfigKeys::cCOST_SELECTION);
   if (selection != aStateObject.constEnd() && selection->isObject())
   {
      auto selectionObject = aStateObject.constFind(EventConfigKeys::cCOST_SELECTION)->toObject();

      if (selectionObject.contains(EventConfigKeys::cOPTION))
      {
         QString option = selectionObject.constFind(EventConfigKeys::cOPTION)->toString();
         if (option == EventConfigKeys::cBLENDED_COST)
         {
            auto blendedPtr = ut::make_unique<wsf::space::BlendedCost>(0.0, 0.0, 0.0);

            bool ok;
            if (selectionObject.contains(EventConfigKeys::cA_PARAMETER))
            {
               double aValue = selectionObject.constFind(EventConfigKeys::cA_PARAMETER)->toString().toDouble(&ok);
               if (ok)
               {
                  blendedPtr->SetA_Value(aValue);
               }
            }
            if (selectionObject.contains(EventConfigKeys::cB_PARAMETER))
            {
               double bValue = selectionObject.constFind(EventConfigKeys::cB_PARAMETER)->toString().toDouble(&ok);
               if (ok)
               {
                  blendedPtr->SetB_Value(bValue);
               }
            }
            if (selectionObject.contains(EventConfigKeys::cC_PARAMETER))
            {
               double cValue = selectionObject.constFind(EventConfigKeys::cC_PARAMETER)->toString().toDouble(&ok);
               if (ok)
               {
                  blendedPtr->SetC_Value(cValue);
               }
            }

            retvalPtr = std::move(blendedPtr);
         }
      }
   }

   return retvalPtr;
}

std::pair<UtVec3d, std::string> OrbitalModelToSequence::Visitor::ProcessKinematicStatePosition(const QJsonObject& aStateObject) const
{
   UtVec3d     pVec{};
   std::string pUnit;

   auto positionIter = aStateObject.constFind(EventConfigKeys::cSTATE_POSITION);
   if (positionIter != aStateObject.constEnd() && positionIter->isObject())
   {
      QJsonObject position = positionIter->toObject();
      auto xComp = StringToUnitHelper<UtLengthValue>(position.constFind(EventConfigKeys::cX_COMPONENT)->toString());
      auto yComp = StringToUnitHelper<UtLengthValue>(position.constFind(EventConfigKeys::cY_COMPONENT)->toString());
      auto zComp = StringToUnitHelper<UtLengthValue>(position.constFind(EventConfigKeys::cZ_COMPONENT)->toString());

      // Rather than always converting to the base unit, this attempts to use the provided units.
      int unit;
      if (!UtMath::NearlyZero(zComp))
      {
         unit = zComp.GetUnit();
      }
      else if (!UtMath::NearlyZero(yComp))
      {
         unit = yComp.GetUnit();
      }
      else if (!UtMath::NearlyZero(xComp))
      {
         unit = xComp.GetUnit();
      }
      else
      {
         unit = UtUnitLength::cMETERS;
      }

      pVec.Set(xComp.GetAsUnit(unit), yComp.GetAsUnit(unit), zComp.GetAsUnit(unit));
      pUnit = UtUnitLength::FindUnitName(unit);
   }

   return std::make_pair(pVec, pUnit);
}

std::pair<UtVec3d, std::string> OrbitalModelToSequence::Visitor::ProcessKinematicStateVelocity(const QJsonObject& aStateObject) const
{
   UtVec3d     vVec{};
   std::string vUnit;
   auto        velocityIter = aStateObject.constFind(EventConfigKeys::cSTATE_VELOCITY);
   if (velocityIter != aStateObject.constEnd() && velocityIter->isObject())
   {
      QJsonObject velocity = velocityIter->toObject();
      auto xComp = StringToUnitHelper<UtSpeedValue>(velocity.constFind(EventConfigKeys::cX_COMPONENT)->toString());
      auto yComp = StringToUnitHelper<UtSpeedValue>(velocity.constFind(EventConfigKeys::cY_COMPONENT)->toString());
      auto zComp = StringToUnitHelper<UtSpeedValue>(velocity.constFind(EventConfigKeys::cZ_COMPONENT)->toString());

      // Rather than always converting to the base unit, this attempts to use the provided units.
      int unit;
      if (!UtMath::NearlyZero(zComp))
      {
         unit = zComp.GetUnit();
      }
      else if (!UtMath::NearlyZero(yComp))
      {
         unit = yComp.GetUnit();
      }
      else if (!UtMath::NearlyZero(xComp))
      {
         unit = xComp.GetUnit();
      }
      else
      {
         unit = UtUnitSpeed::cMETERS_PER_SECOND;
      }

      vVec.Set(xComp.GetAsUnit(unit), yComp.GetAsUnit(unit), zComp.GetAsUnit(unit));
      vUnit = UtUnitSpeed::FindUnitName(unit);
   }

   return std::make_pair(vVec, vUnit);
}

} // namespace spaceg

} // namespace wsfg
