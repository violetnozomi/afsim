// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuversChangeRAAN_Inclination.hpp"

#include <iostream>

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
// static
namespace // anonymous
{
double cCHANGE_RAAN_INCLINATION_INVALID_ANGLE = -999.0;
}

ChangeRAAN_Inclination::ChangeRAAN_Inclination(const WsfScenario& aScenario)
   : ChangeRAAN_Inclination::ChangeRAAN_Inclination(aScenario,
                                                    cCHANGE_RAAN_INCLINATION_INVALID_ANGLE,
                                                    cCHANGE_RAAN_INCLINATION_INVALID_ANGLE)
{
}

ChangeRAAN_Inclination::ChangeRAAN_Inclination(const WsfScenario&  aScenario,
                                               const UtAngleValue& aRAAN,
                                               const UtAngleValue& aInclination)
   : WsfOrbitalManeuver(aScenario)
   , mRAAN(aRAAN)
   , mInclination(aInclination)
{
   SetType(GetTypeId());
}

WsfStringId ChangeRAAN_Inclination::GetTypeId()
{
   return "CHANGE_RAAN_INCLINATION";
}

bool ChangeRAAN_Inclination::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if ((command == "raan") || (command == "right_ascension_of_ascending_node"))
   {
      aInput.ReadUnitValue(mRAAN);
      aInput.ValueInClosedRange<double>(mRAAN, 0.0, UtMath::cTWO_PI);
   }
   else if (command == "inclination")
   {
      aInput.ReadUnitValue(mInclination);
      aInput.ValueInClosedRange<double>(mInclination, 0.0, UtMath::cPI);
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::Initialize(const UtCalendar&                        aEpoch,
                                                             const wsf::space::OrbitalMissionContext& aContext)
{
   auto& cond = dynamic_cast<wsf::space::IntersectionCondition&>(GetCondition());
   cond.SetIntersectionRAAN_Inclination(mRAAN.GetAsUnit(UtUnitAngle::cRADIANS),
                                        mInclination.GetAsUnit(UtUnitAngle::cRADIANS));
   bool ok = WsfOrbitalEvent::Initialize(aEpoch, aContext);
   return ok;
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::VerifyCondition() const
{
   auto t = GetCondition().GetType();
   return ((t == wsf::space::NorthernIntersectionCondition::cTYPE) ||
           (t == wsf::space::SouthernIntersectionCondition::cTYPE));
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      ok &= aContext.GetPropagator().OrbitIsCircular();
      if (ok)
      {
         ok &= mRAAN != cCHANGE_RAAN_INCLINATION_INVALID_ANGLE;
         ok &= mInclination != cCHANGE_RAAN_INCLINATION_INVALID_ANGLE;
         if (!ok)
         {
            auto logger = ut::log::error() << "You must specify valid input for both RAAN and inclination.";
            logger.AddNote() << "Type: " << GetType();
         }
         else if ((mInclination == 0.0) || (mInclination == UtMath::cPI))
         {
            auto logger = ut::log::error() << "Maneuver invalid for equatorial orbits (RAAN is undefined).";
            logger.AddNote() << "Type: " << GetType();
            ok = false;
         }
      }
      else
      {
         auto logger = ut::log::error() << "Initial orbit must be circular to execute this maneuver.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = ValidateRAAN();
   retval      = ValidateInclination() && retval;
   retval      = WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::ValidateRAAN() const
{
   bool retval{true};
   if (mRAAN < 0.0 || mRAAN > UtMath::cTWO_PI)
   {
      auto logger = ut::log::error() << "RAAN must be in the range [0, 360.0 deg].";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   return retval;
}

bool WsfOrbitalManeuvers::ChangeRAAN_Inclination::ValidateInclination() const
{
   bool retval{true};
   if (mInclination < 0.0 || mInclination > UtMath::cPI)
   {
      auto logger = ut::log::error() << "inclination must be in the range [0, 180.0 deg].";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   return retval;
}

bool ChangeRAAN_Inclination::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   auto&            eleTOD             = aPropagator.GetOrbitalState().GetOrbitalElementsTOD();
   double           RAAN               = eleTOD.GetRAAN();
   double           inclination        = eleTOD.GetInclination();
   constexpr double sANGULAR_TOLERANCE = 0.001; // radians
   return ((aPropagator.OrbitIsCircular() || (fabs(mInclination - inclination) < sANGULAR_TOLERANCE)) &&
           ((fabs(mRAAN - RAAN) < sANGULAR_TOLERANCE) ||
            (fabs(mRAAN + UtMath::cTWO_PI - RAAN) < sANGULAR_TOLERANCE))); // special case of where numerical error
   // places RAAN near 2pi instead of 0.
}

// virtual
void ChangeRAAN_Inclination::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                           const UtCalendar&              aEvaluationTime,
                                           const UtOrbitalPropagatorBase& aPropagator,
                                           UtVec3d&                       aDeltaV) const
{
   static constexpr double sTOL = 1.0e-6;

   // Maneuvering using orbital elements MUST reference the True-of-Date frame (the current geo-referenced orientation).
   const UtOrbitalElements& currentElements = aPropagator.GetOrbitalState().GetOrbitalElementsTOD();

   double raan_i = currentElements.GetRAAN();
   double incl_i = currentElements.GetInclination();
   double raan_f = mRAAN.GetAsUnit("rad");
   double incl_f = mInclination.GetAsUnit("rad");

   double dRAAN = UtMath::NormalizeAngleMinusPi_Pi(raan_f - raan_i);
   double dInc  = incl_f - incl_i;

   if ((dRAAN != 0.0) || (dInc != 0.0))
   {
      auto opPtr = aPropagator.GetManeuveringPropagator();
      opPtr->Initialize(aCurrentTime);

      UtVec3d velECI = opPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
      UtVec3d posECI = opPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();

      double sini_i = std::sin(incl_i);
      double cosi_i = std::cos(incl_i);
      double sinW_i = std::sin(raan_i);
      double cosW_i = std::cos(raan_i);

      double sini_f = std::sin(incl_f);
      double cosi_f = std::cos(incl_f);
      double sinW_f = std::sin(raan_f);
      double cosW_f = std::cos(raan_f);

      // This maneuver is treated as a rotation about an axis parallel to the ECI position vector.
      //    The angle is computed as the arc-cosine of the dot product of the normal vectors
      //    of the initial and final orbits.
      //    In some situations, the angle of rotation needs to be negated.

      UtVec3d ni(sini_i * sinW_i, -sini_i * cosW_i, cosi_i);
      UtVec3d nf(sini_f * sinW_f, -sini_f * cosW_f, cosi_f);

      UtVec3d axis  = posECI.GetNormal();
      double  angle = std::acos(UtVec3d::DotProduct(ni, nf));
      UtVec3d cross;
      cross.UtVec3d::CrossProduct(ni, nf);

      bool northernIntersection = (GetCondition().GetType() == wsf::space::NorthernIntersectionCondition::cTYPE);

      // The z-component of the cross product will be zero if the intersection of the two orbits lies on the equatorial plane.
      //    This will happen in two situations:
      //       1. When the initial orbit is equatorial
      //       2. The RAANs of the two orbits are parallel (dRAAN == 180 deg).
      if (UtMath::NearlyZero(cross.Get(2), sTOL))
      {
         angle = UtMath::NormalizeAngleMinusPi_Pi(incl_f + incl_i);

         // if the initial orbit is equatorial.
         if (UtMath::NearlyZero(incl_i, sTOL) || UtMath::NearlyEqual(incl_i, UtMath::cPI, sTOL))
         {
            if (((northernIntersection && ((raan_f > UtMath::cPI) || UtMath::NearlyEqual(raan_f, UtMath::cPI, sTOL))) ||
                 (!northernIntersection && (raan_f < UtMath::cPI))))
            {
               angle *= -1.0;
            }
         }
         // else, if the RAANs of the orbits are parallel.
         else if (UtMath::NearlyEqual(std::fabs(dRAAN), UtMath::cPI, sTOL))
         {
            if (velECI[2] > 0)
            {
               angle *= -1.0;
            }
         }
      }
      else // The z-component of the cross product is non-zero
      {
         if ((cross.Get(2) < 0 && northernIntersection) || (cross.Get(2) > 0 && !northernIntersection))
         {
            angle *= -1.0;
         }
      }
      UtDCM   rotMat    = UtDCM(angle, axis.GetData());
      UtVec3d newVelECI = velECI.Magnitude() * rotMat.Transform(velECI.GetNormal());

      aDeltaV.Subtract(newVelECI, velECI);
   }
   else
   {
      aDeltaV.Set(0.0);
   }
}

void ChangeRAAN_Inclination::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitChangeRAAN_Inclination(this);
}

} // namespace WsfOrbitalManeuvers
