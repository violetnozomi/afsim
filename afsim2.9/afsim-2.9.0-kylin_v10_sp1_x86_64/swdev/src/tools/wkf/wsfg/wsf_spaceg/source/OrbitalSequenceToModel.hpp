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

#ifndef ORBITALSEQUENCETOMODEL_HPP
#define ORBITALSEQUENCETOMODEL_HPP

#include <iomanip>
#include <memory>
#include <sstream>

#include <QJsonObject>

#include "WsfOrbitalEventVisitor.hpp"
class WsfOrbitalMissionSequence;

namespace wsf
{
namespace space
{
class TargetPointOptions;

}
} // namespace wsf

#include "wsf_spaceg_export.h"

#include "OrbitalEventConfigKeys.hpp"
#include "OrbitalMissionItem.hpp"

namespace wsfg
{
namespace spaceg
{
class OrbitalMissionModel;

//! Helper function to convert a unit-bearing real value into a string for use in the UI mission model.
template<typename UnitValueType>
QString FromValue(const UnitValueType& aValue)
{
   std::stringstream ss{};
   ss << std::setprecision(9) << aValue;
   return QString::fromStdString(ss.str());
}

//! This object transforms a mission sequence into the Astrolabe-adapted mission model.
class WSF_SPACEG_EXPORT OrbitalSequenceToModel : public WsfOrbitalEventVisitor
{
public:
   OrbitalSequenceToModel()
      : mItem{QJsonObject{}}
      , mData{}
      , mModelPtr{nullptr}
   {
   }
   ~OrbitalSequenceToModel() override = default;

   //! Return the configured model item produced from the orbital event.
   const OrbitalMissionItem&            GetItem();
   std::unique_ptr<OrbitalMissionModel> GetModel();

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
   void        CommonSetup(WsfOrbitalEvent* aEventPtr);
   void        SetupEventOption(WsfOrbitalEvent* aEventPtr);
   void        CommonSetupNoFinite(WsfOrbitalEvent* aEventPtr);
   void        ProcessCondition(wsf::space::OrbitalPropagatorCondition& aCondition);
   void        ProcessFinite(const WsfOrbitalEvent* aEventPtr);
   void        ProcessTargetOptions(const wsf::space::TargetPointOptions& aOptions);
   void        ProcessOffset(const wsf::space::TargetPointOptions& aOptions);
   static void ProcessOptimizationCost(const wsf::space::OrbitalTargetingCost* aCostPtr, QJsonObject& aOptimizeObject);

   template<typename TargetingEventType>
   void ProcessTargetingEvent(const TargetingEventType* aEventPtr)
   {
      QJsonObject optimizeObject{};

      // Set the optimization option
      QString                             optimizeOption{};
      WsfOrbitalManeuvers::OptimizeOption opt = aEventPtr->GetOptimizeOption();
      switch (opt)
      {
      case WsfOrbitalManeuvers::OptimizeOption::cOPTIMIZE_TIME:
         optimizeOption = EventConfigKeys::cDELTA_T;
         break;
      case WsfOrbitalManeuvers::OptimizeOption::cOPTIMIZE_DELTA_V:
         optimizeOption = EventConfigKeys::cDELTA_V;
         break;
      case WsfOrbitalManeuvers::OptimizeOption::cOPTIMIZE_NONE:
         optimizeOption = EventConfigKeys::cNONE;
         break;
      case WsfOrbitalManeuvers::OptimizeOption::cOPTIMIZE_COST:
         optimizeOption = EventConfigKeys::cCOST_OPTIMIZATION;
         ProcessOptimizationCost(aEventPtr->GetOptimizationCost(), optimizeObject);
         break;
      }
      optimizeObject[EventConfigKeys::cOPTION] = optimizeOption;

      if (aEventPtr->GetDeltaTime() > 0.0)
      {
         optimizeObject[EventConfigKeys::cDELTA_TIME] = FromValue(aEventPtr->GetDeltaTime());
      }
      if (aEventPtr->GetMaximumDeltaT() > 0.0)
      {
         optimizeObject[EventConfigKeys::cMAXIMUM_DELTA_TIME] = FromValue(aEventPtr->GetMaximumDeltaT());
      }
      if (aEventPtr->GetMaximumDeltaV() > 0.0)
      {
         optimizeObject[EventConfigKeys::cMAXIMUM_DELTA_V] = FromValue(aEventPtr->GetMaximumDeltaV());
      }

      mData[EventConfigKeys::cOPTIMIZE] = optimizeObject;

      mData[EventConfigKeys::cTOLERANCE] = QString::number(aEventPtr->GetTolerance());
   }

   QString StripBlock(const QString& aBlockName, const QString& aInput);

   OrbitalMissionItem                   mItem;
   QJsonObject                          mData;
   std::unique_ptr<OrbitalMissionModel> mModelPtr;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALSEQUENCETOMODEL_HPP
