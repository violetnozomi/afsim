// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceEventUtils.hpp"

#include <iomanip>
#include <ostream>

#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfEventUtils.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace wsf
{
namespace event
{
namespace utils
{
void PrintOrbitalElementsData(std::ostream& aStream, const UtOrbitalElements& aOE)
{
   aStream << " Orbital Elements: ";
   std::streamsize oldPrecision = aStream.precision(18);
   aStream << std::setprecision(10);
   // clang-format off
   aStream << "SMA: " << aOE.GetSemiMajorAxis() / 1000.0 << " km "
           << "ECC: " << aOE.GetEccentricity() << "  "
           << "INC: " << aOE.GetInclination() / UtMath::cRAD_PER_DEG << " deg "
           << "RAAN: " << aOE.GetRAAN() / UtMath::cRAD_PER_DEG << " deg "
           << "ARGP: " << aOE.GetArgumentOfPeriapsis() / UtMath::cRAD_PER_DEG << " deg "
           << "MA: " << aOE.GetMeanAnomaly() / UtMath::cRAD_PER_DEG << " deg "
           << "TA: " << aOE.GetTrueAnomaly() / UtMath::cRAD_PER_DEG << " deg ";
   // clang-format on
   aStream.precision(oldPrecision);
}

void PrintOrbitalManeuverData(std::ostream&          aStream,
                              WsfSpaceMoverBase*     aSpaceMoverPtr,
                              const WsfOrbitalEvent& aManeuver,
                              bool                   aPrintSingleLinePerEvent)
{
   aStream << aSpaceMoverPtr->GetPlatform()->GetName() << " ID: " << aManeuver.GetUniqueId()
           << ContinueChar(aPrintSingleLinePerEvent) << " Type: " << aManeuver.GetType()
           << " Constraint: " << aManeuver.GetConditionString();
   if (aManeuver.GetCondition().GetType() == wsf::space::RelativeTimeCondition::cTYPE)
   {
      const auto& relTime = dynamic_cast<const wsf::space::RelativeTimeCondition&>(aManeuver.GetCondition());
      aStream << " " << relTime.GetOffsetTime();
   }
   else if (aManeuver.GetCondition().GetType() == wsf::space::AscendingRadiusCondition::cTYPE)
   {
      const auto& ascRad = dynamic_cast<const wsf::space::AscendingRadiusCondition&>(aManeuver.GetCondition());
      aStream << " " << ascRad.GetRadius() / 1000.0 << " km";
   }
   else if (aManeuver.GetCondition().GetType() == wsf::space::DescendingRadiusCondition::cTYPE)
   {
      const auto& descRad = dynamic_cast<const wsf::space::DescendingRadiusCondition&>(aManeuver.GetCondition());
      aStream << " " << descRad.GetRadius() / 1000.0 << " km";
   }
   aStream << " Duration: " << aManeuver.GetDuration();
   aStream << ContinueChar(aPrintSingleLinePerEvent);
   const UtOrbitalElements& oe = aSpaceMoverPtr->GetOrbitalState().GetOrbitalElements();
   PrintOrbitalElementsData(aStream, oe);
   auto circular   = aSpaceMoverPtr->GetPropagator().OrbitIsCircular();
   auto equatorial = aSpaceMoverPtr->GetPropagator().OrbitIsEquatorial();
   if (circular || equatorial)
   {
      if (circular && !equatorial)
      {
         aStream << " (Circular)";
      }
      else if (equatorial && !circular)
      {
         aStream << " (Equatorial)";
      }
      else
      {
         aStream << " (Circular, Equatorial)";
      }
   }
}

} // namespace utils

namespace utilsCSV
{
void PrintOrbitalElementsData(std::ostream& aStream, const UtOrbitalElements& aOE)
{
   std::streamsize oldPrecision = aStream.precision(18);
   aStream << std::setprecision(10);
   aStream << ',' << aOE.GetSemiMajorAxis() / 1000.0 << ',' << aOE.GetEccentricity() << ','
           << aOE.GetInclination() / UtMath::cRAD_PER_DEG << ',' << aOE.GetRAAN() / UtMath::cRAD_PER_DEG << ','
           << aOE.GetArgumentOfPeriapsis() / UtMath::cRAD_PER_DEG << ',' << aOE.GetMeanAnomaly() / UtMath::cRAD_PER_DEG
           << ',' << aOE.GetTrueAnomaly() / UtMath::cRAD_PER_DEG;
   aStream.precision(oldPrecision);
}

void PrintOrbitalManeuverData(std::ostream& aStream, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver)
{
   aStream << ',' << aSpaceMoverPtr->GetPlatform()->GetName() << ',' << aManeuver.GetUniqueId() << ','
           << aManeuver.GetType() << ',' << aManeuver.GetConditionString();
   if (aManeuver.GetCondition().GetType() == wsf::space::RelativeTimeCondition::cTYPE)
   {
      const auto& relTime = dynamic_cast<const wsf::space::RelativeTimeCondition&>(aManeuver.GetCondition());
      aStream << ',' << static_cast<double>(relTime.GetOffsetTime());
   }
   else if (aManeuver.GetCondition().GetType() == wsf::space::AscendingRadiusCondition::cTYPE)
   {
      const auto& ascRad = dynamic_cast<const wsf::space::AscendingRadiusCondition&>(aManeuver.GetCondition());
      aStream << ',' << ascRad.GetRadius() / 1000.0;
   }
   else if (aManeuver.GetCondition().GetType() == wsf::space::DescendingRadiusCondition::cTYPE)
   {
      const auto& descRad = dynamic_cast<const wsf::space::DescendingRadiusCondition&>(aManeuver.GetCondition());
      aStream << ',' << descRad.GetRadius() / 1000.0;
   }
   else
   {
      aStream << ','; // empty
   }
   aStream << ',' << static_cast<double>(aManeuver.GetDuration()) << ",";
   const UtOrbitalElements& oe = aSpaceMoverPtr->GetOrbitalState().GetOrbitalElements();
   PrintOrbitalElementsData(aStream, oe);
   bool circular   = aSpaceMoverPtr->GetPropagator().OrbitIsCircular();
   bool equatorial = aSpaceMoverPtr->GetPropagator().OrbitIsEquatorial();
   if (circular || equatorial)
   {
      if (circular && !equatorial)
      {
         aStream << ',' << " Circular";
      }
      else if (equatorial && !circular)
      {
         aStream << ',' << " Equatorial";
      }
      else
      {
         aStream << ',' << " Circular / Equatorial";
      }
   }
   else
   {
      aStream << ',';
   }
}

} // namespace utilsCSV

} // namespace event
} // namespace wsf
