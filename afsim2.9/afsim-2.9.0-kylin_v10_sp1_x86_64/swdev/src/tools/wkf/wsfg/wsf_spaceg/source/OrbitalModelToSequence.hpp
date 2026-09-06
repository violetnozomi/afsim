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

#ifndef ORBITALMODELTOSEQUENCE_HPP
#define ORBITALMODELTOSEQUENCE_HPP

#include <memory>
#include <sstream>

#include <QJsonObject>
class QModelIndex;

#include "WsfOrbitalEventVisitor.hpp"
class WsfOrbitalMissionSequence;
#include "wsf_spaceg_export.h"

#include "OrbitalEventConfigKeys.hpp"
#include "OrbitalMissionData.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace space
{
class OrbitalTargetingCost;

}
} // namespace wsf

namespace wsfg
{
namespace spaceg
{
//! A helper function used to interpret the string values saved in the UI's model into the
//! unit bearing types stored in AFSIM's mission sequence.
template<typename UnitClass>
UnitClass StringToUnitHelper(const QString& aString)
{
   std::stringstream ss{};
   ss.str(aString.toStdString());
   UnitClass retval{};
   ss >> retval;
   return retval;
}

//! An object that transforms the UI representation of the mission model into the AFSIM representation.
//!
//! This makes use of a WsfOrbitalEventVisitor subclass to perform the transformation from
//! Astrolabe's OrbitalMissionModel into AFSIM's WsfOrbitalMissionSequence. This makes use of
//! an AFSIM scenario's type list for orbital mission events.
class WSF_SPACEG_EXPORT OrbitalModelToSequence
{
public:
   explicit OrbitalModelToSequence(const WsfScenario& aScenario);
   ~OrbitalModelToSequence();

   const WsfScenario& GetScenario() const;

   std::unique_ptr<WsfOrbitalMissionSequence> Transform(const OrbitalMissionData& aMission);

private:
   class Visitor : public WsfOrbitalEventVisitor
   {
   public:
      Visitor(OrbitalModelToSequence& aTransformer, const OrbitalMissionData& aMission, int aOrderedIndex)
         : mTransformer(aTransformer)
         , mMission(aMission)
         , mOrderedIndex{aOrderedIndex}
         , mData{aMission.GetItemData(aOrderedIndex)}
      {
      }
      ~Visitor() override = default;

      void VisitMissionSequence(WsfOrbitalMissionSequence* aSequencePtr) override;
      void VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr) override;
      void VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr) override;
      void VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr) override;
      void VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr) override;
      void VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr) override;
      void VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr) override;
      void VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr) override;
      void VisitDeltaV(WsfOrbitalManeuvers::DeltaV* aEventPtr) override;
      void VisitDrift(WsfOrbitalManeuvers::Drift* aEventPtr) override;
      void VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr) override;
      void VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr) override;
      void VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr) override;
      void VisitNaturalMotionCircumnavigation(WsfOrbitalManeuvers::NaturalMotionCircumnavigation* aEventPtr) override;
      void VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr) override;
      void VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr) override;
      void VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr) override;
      void VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr) override;
      void VisitTeardrop(WsfOrbitalManeuvers::Teardrop* aEventPtr) override;
      void VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr) override;
      void VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr) override;
      void VisitOrbitalAttitudeChangeEvent(WsfOrbitalAttitudeChangeEvent* aEventPtr) override;

   private:
      void                                              CommonSetup(WsfOrbitalEvent* aEventPtr) const;
      void                                              ConditionSetup(WsfOrbitalEvent* aEventPtr) const;
      static std::string                                ConditionTypeFromString(const QString& aType);
      void                                              FiniteSetup(WsfOrbitalEvent* aEventPtr) const;
      static WsfTrackId                                 TrackIdFromString(const QString& aTrackString);
      std::unique_ptr<wsf::space::OrbitalTargetingCost> ProcessCostFunction(const QJsonObject& aStateObject) const;

      //! Process the time and delta-v constraints from the optimization selection.
      template<typename TargetingEvent>
      void ProcessTargetConstraints(TargetingEvent* aEventPtr, const QJsonObject& aStateObject) const
      {
         if (aStateObject.contains(EventConfigKeys::cDELTA_TIME))
         {
            aEventPtr->SetDeltaTime(
               StringToUnitHelper<UtTimeValue>(aStateObject.constFind(EventConfigKeys::cDELTA_TIME)->toString()));
         }
         if (aStateObject.contains(EventConfigKeys::cMAXIMUM_DELTA_TIME))
         {
            aEventPtr->SetMaximumDeltaT(
               StringToUnitHelper<UtTimeValue>(aStateObject.constFind(EventConfigKeys::cMAXIMUM_DELTA_TIME)->toString()));
         }
         if (aStateObject.contains(EventConfigKeys::cMAXIMUM_DELTA_V))
         {
            aEventPtr->SetMaximumDeltaV(
               StringToUnitHelper<UtSpeedValue>(aStateObject.constFind(EventConfigKeys::cMAXIMUM_DELTA_V)->toString()));
         }
      }

      //! All targeting-class events use these options, including NMC and Drift.
      template<typename TargetingEvent>
      void ProcessCommonTargetingOptions(TargetingEvent* aEventPtr) const
      {
         auto optimize = mData.constFind(EventConfigKeys::cOPTIMIZE);
         if (optimize != mData.constEnd() && optimize->isObject())
         {
            QJsonObject optimizeObject = optimize->toObject();

            if (optimizeObject.contains(EventConfigKeys::cOPTION))
            {
               QString option = optimizeObject.constFind(EventConfigKeys::cOPTION)->toString();
               if (option == EventConfigKeys::cDELTA_T)
               {
                  ProcessTargetConstraints(aEventPtr, optimizeObject);
                  aEventPtr->SetOptimizeOption(WsfOrbitalManeuvers::cOPTIMIZE_TIME);
               }
               else if (option == EventConfigKeys::cDELTA_V)
               {
                  ProcessTargetConstraints(aEventPtr, optimizeObject);
                  aEventPtr->SetOptimizeOption(WsfOrbitalManeuvers::cOPTIMIZE_DELTA_V);
               }
               else if (option == EventConfigKeys::cCOST_OPTIMIZATION)
               {
                  ProcessTargetConstraints(aEventPtr, optimizeObject);
                  auto costPtr = ProcessCostFunction(optimizeObject);
                  aEventPtr->SetOptimizationCost(std::move(costPtr));
               }
               else // option == "none"
               {
                  ProcessTargetConstraints(aEventPtr, optimizeObject);
                  aEventPtr->SetOptimizeOption(WsfOrbitalManeuvers::cOPTIMIZE_NONE);
               }
            }
         }
         if (mData.contains(EventConfigKeys::cTOLERANCE))
         {
            bool   ok;
            double tolerance = mData.constFind(EventConfigKeys::cTOLERANCE)->toString().toDouble(&ok);
            if (ok)
            {
               aEventPtr->SetTolerance(tolerance);
            }
         }
      }

      //! A helper function used to handle the three targeting events: Target, Intercept and Rendezvous.
      template<typename TargetingEvent>
      void ProcessTargetingEvent(TargetingEvent* aEventPtr)
      {
         ProcessCommonTargetingOptions(aEventPtr);
         if (mData.contains(EventConfigKeys::cPOSITION_OFFSET))
         {
            QJsonObject   offsetObj = mData.constFind(EventConfigKeys::cPOSITION_OFFSET)->toObject();
            UtVec3d       offsetValue{};
            UtLengthValue length{};

            auto frameIdentifier = offsetObj.constFind(EventConfigKeys::cFRAME)->toString().toStdString();
            auto frame           = wsf::space::OrbitalReferenceFrameFromIdentifier(frameIdentifier);

            length = StringToUnitHelper<UtLengthValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_X)->toString());
            offsetValue[0] = length.GetAsUnit(UtUnitLength::cMETERS);

            length = StringToUnitHelper<UtLengthValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_Y)->toString());
            offsetValue[1] = length.GetAsUnit(UtUnitLength::cMETERS);

            length = StringToUnitHelper<UtLengthValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_Z)->toString());
            offsetValue[2] = length.GetAsUnit(UtUnitLength::cMETERS);

            std::string offsetUnits = UtUnitLength::FindUnitName(UtUnitLength::cMETERS);

            if (offsetValue[0] != 0.0 || offsetValue[1] != 0.0 || offsetValue[2] != 0.0)
            {
               aEventPtr->SetOffsetInUnits(offsetValue, offsetUnits, frame);
            }
         }
         if (mData.contains(EventConfigKeys::cVELOCITY_OFFSET))
         {
            QJsonObject  offsetObj = mData.constFind(EventConfigKeys::cVELOCITY_OFFSET)->toObject();
            UtVec3d      offsetValue{};
            UtSpeedValue speed{};

            auto frameIdentifier = offsetObj.constFind(EventConfigKeys::cFRAME)->toString().toStdString();
            auto frame           = wsf::space::OrbitalReferenceFrameFromIdentifier(frameIdentifier);

            speed = StringToUnitHelper<UtSpeedValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_X)->toString());
            offsetValue[0] = speed.GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND);

            speed = StringToUnitHelper<UtSpeedValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_Y)->toString());
            offsetValue[1] = speed.GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND);

            speed = StringToUnitHelper<UtSpeedValue>(offsetObj.constFind(EventConfigKeys::cOFFSET_Z)->toString());
            offsetValue[2] = speed.GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND);

            std::string offsetUnits = UtUnitSpeed::FindUnitName(UtUnitSpeed::cMETERS_PER_SECOND);

            if (offsetValue[0] != 0.0 || offsetValue[1] != 0.0 || offsetValue[2] != 0.0)
            {
               aEventPtr->SetVelocityOffsetInUnits(offsetValue, offsetUnits, frame);
            }
         }
         if (mData.contains(EventConfigKeys::cOFFSET_TIME))
         {
            aEventPtr->SetTargetOffsetTime(
               StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cOFFSET_TIME)->toString()));
         }
         if (mData.contains(EventConfigKeys::cLAG_TIME))
         {
            aEventPtr->SetTargetLagTime(
               StringToUnitHelper<UtTimeValue>(mData.constFind(EventConfigKeys::cLAG_TIME)->toString()));
         }
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
            else if (targetObject.contains(EventConfigKeys::cLIBRATION_SYSTEM) &&
                     targetObject.contains(EventConfigKeys::cLIBRATION_POINT))
            {
               auto systemString = targetObject.constFind(EventConfigKeys::cLIBRATION_SYSTEM)->toString().toStdString();
               auto pointString  = targetObject.constFind(EventConfigKeys::cLIBRATION_POINT)->toString().toStdString();
               aEventPtr->SetLibrationPoint(wsf::space::LibrationPoint::GetSystemFromIdentifier(systemString),
                                            wsf::space::LibrationPoint::GetPointFromIdentifier(pointString));
            }
            else if (targetObject.contains(EventConfigKeys::cKINEMATIC_STATE))
            {
               ProcessKinematicStateTarget(aEventPtr,
                                           targetObject.constFind(EventConfigKeys::cKINEMATIC_STATE)->toObject());
            }
         }
      }

      std::pair<UtVec3d, std::string> ProcessKinematicStatePosition(const QJsonObject& aStateObject) const;
      std::pair<UtVec3d, std::string> ProcessKinematicStateVelocity(const QJsonObject& aStateObject) const;

      template<typename TargetingEvent>
      void ProcessKinematicStateTarget(TargetingEvent* aEventPtr, const QJsonObject& aStateObject)
      {
         UtVec3d     pVec{};
         std::string pUnit;
         std::tie(pVec, pUnit) = ProcessKinematicStatePosition(aStateObject);

         UtVec3d     vVec{};
         std::string vUnit;
         std::tie(vVec, vUnit) = ProcessKinematicStateVelocity(aStateObject);

         aEventPtr->SetKinematicStateTarget(pVec, pUnit, vVec, vUnit);
      }

      OrbitalModelToSequence&   mTransformer;
      const OrbitalMissionData& mMission;
      int                       mOrderedIndex;
      QJsonObject               mData;
   };

   const WsfScenario& mScenario;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALMODELTOSEQUENCE_HPP
