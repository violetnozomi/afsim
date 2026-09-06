// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_Assess.hpp"

#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfAirCombatObserver.hpp"
#include "WsfAirMover.hpp"
#include "WsfAirToAirLaunchComputer.hpp"
#include "WsfBrawlerFuel.hpp"
#include "WsfBrawlerMover.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfFuel.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_GroupManager.hpp"
#include "WsfSA_GroupUtils.hpp"
#include "WsfSA_PerceivedItem.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"

namespace
{
double TargetCrossSection(const WsfSA_Assess::AircraftSignatureParameters& aTargetSignature, double aAzimuth, double aElevation)
{
   // Simple assumption that there is a symmetrical result (not often true, but simple)
   double azimuth   = std::abs(aAzimuth);
   double elevation = std::abs(aElevation);

   // Limit azimuth
   if (azimuth > UtMath::cPI)
   {
      azimuth = UtMath::cPI;
   }

   // Limit elevation
   if (elevation > UtMath::cPI_OVER_2)
   {
      elevation = UtMath::cPI_OVER_2;
   }

   // This calculates a *very* simple cross section based on shapes

   // Assume no smaller than fuselage cross section
   double crossSectionFront = aTargetSignature.fuselageFrontalArea;

   // Calc the vertical component for the wing
   double crossSectionWing = sin(elevation) * aTargetSignature.wingArea;

   // Calc the component for the tail
   double crossSectionTail = sin(azimuth) * cos(elevation) * aTargetSignature.tailArea;

   // Calc the component for the inlet(s)
   double crossSectionInlet = 0.0;
   if (azimuth < UtMath::cPI_OVER_2) // Need forward
   {
      double inletAngle = azimuth;
      if (elevation > azimuth)
      {
         inletAngle = elevation;
      }

      crossSectionInlet = aTargetSignature.engineInletArea * cos(inletAngle);
   }

   // Calc the component for the exit(s)
   double crossSectionExit = 0.0;
   if (azimuth > UtMath::cPI_OVER_2) // Need aft
   {
      double tempAz = UtMath::cPI - azimuth;

      double exitAngle = tempAz;
      if (elevation > tempAz)
      {
         exitAngle = elevation;
      }

      crossSectionExit = aTargetSignature.engineExitArea * cos(exitAngle);
   }

   // Calc the component for sweep
   double cosineEffect      = cos(azimuth - aTargetSignature.sweepAngle);
   double cosineEffect2     = cosineEffect * cosineEffect;
   double crossSectionSweep = (cosineEffect2 * cosineEffect2) * aTargetSignature.wingArea * cos(elevation);

   // Total cross section
   double crossSection =
      crossSectionFront + crossSectionWing + crossSectionTail + crossSectionInlet + crossSectionExit + crossSectionSweep;

   // Nose reduction
   if ((azimuth < aTargetSignature.noseSignatureReductionAngle) &&
       (elevation < aTargetSignature.noseSignatureReductionAngle))
   {
      // Adjust the cross section
      crossSection *= aTargetSignature.signatureReductionFactorNose;
   }

   // Overall reduction
   crossSection *= aTargetSignature.signatureReductionFactorOverall;

   // Limit cross section to positive value
   if (crossSection < 0.0)
   {
      crossSection = 0.0;
   }

   return crossSection;
}
} // namespace

WsfSA_Assess::WsfSA_Assess(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr)
   : WsfSA_Module(aSA_ProcPtr, aDataPtr)
   , mP6DofMover(aSA_ProcPtr->GetP6DofMover())
   , mSixDOF_Mover(aSA_ProcPtr->GetSixDOF_Mover())
   , mBrawlerMover(aSA_ProcPtr->GetBrawlerMover())
   , mBrawlerFuel(aSA_ProcPtr->GetBrawlerFuel())
   , mAirMover(aSA_ProcPtr->GetAirMover())
   , mAirMoverFuel(aSA_ProcPtr->GetAirMoverFuel())
   , mBlueMissileParametersMRM(550.0, 120.0, 80.0 * UtMath::cRAD_PER_DEG, 3.5, 0.80, 0.0, 0.2)
   , mBlueMissileParametersSRM(550.0, 20.0, 120.0 * UtMath::cRAD_PER_DEG, 1.5, 0.95, 0.3, 0.2)
   , mRedMissileParametersMRM(550.0, 100.0, 72.0 * UtMath::cRAD_PER_DEG, 4.5, 0.75, 0.0, 0.3)
   , mRedMissileParametersSRM(550.0, 18.0, 132.0 * UtMath::cRAD_PER_DEG, 1.5, 0.90, 0.3, 0.2)
   , mBlueAircraftSignature(3.0, 30.0, 4.0, 2.0, 2.0, 45.0 * UtMath::cRAD_PER_DEG, 1.0, 1.0, 45.0 * UtMath::cRAD_PER_DEG)
   , mRedAircraftSignature(3.0, 30.0, 4.0, 2.0, 2.0, 45.0 * UtMath::cRAD_PER_DEG, 1.0, 1.0, 45.0 * UtMath::cRAD_PER_DEG)
{
}

WsfSA_Assess::~WsfSA_Assess() {}

double WsfSA_Assess::AngleOffTarget2D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform)
{
   double targetHeading_rad = 0.0;
   double targetPitch_rad   = 0.0;
   double targetRoll_rad    = 0.0;
   aTargetPlatform.GetOrientationNED(targetHeading_rad, targetPitch_rad, targetRoll_rad);

   double ownshipHeading_rad = 0.0;
   double ownshipPitch_rad   = 0.0;
   double ownshipRoll_rad    = 0.0;
   aPlatform.GetOrientationNED(ownshipHeading_rad, ownshipPitch_rad, ownshipRoll_rad);

   return UtMath::NormalizeAngleMinusPi_Pi(targetHeading_rad - ownshipHeading_rad);
}

double WsfSA_Assess::AngleOffTarget2D(const WsfPlatform& aTargetPlatform) const
{
   return AngleOffTarget2D(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::AngleOffTarget3D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform)
{
   UtVec3dX ownshipVelocityWCS;
   aPlatform.GetVelocityWCS(ownshipVelocityWCS.GetData());

   double targetVelocityWCS[3];
   aTargetPlatform.GetVelocityWCS(targetVelocityWCS);
   UtVec3dX targetVel(targetVelocityWCS);

   double angle_off_rad = ownshipVelocityWCS.AngleBetween(targetVel);

   double psi   = 0.0;
   double theta = 0.0;
   double phi   = 0.0;
   aPlatform.GetOrientationWCS(psi, theta, phi);
   UtDCM    dcm(psi, theta, phi);
   UtVec3dX rightWingVec(0.0, 1.0, 0.0);
   UtVec3dX rightSideVec = dcm.InverseTransform(rightWingVec);

   double dot = targetVel.DotProduct(rightSideVec);
   if (dot < 0.0)
   {
      angle_off_rad *= -1.0;
   }

   return angle_off_rad;
}

double WsfSA_Assess::AngleOffTarget3D(const WsfPlatform& aTargetPlatform) const
{
   return AngleOffTarget3D(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::AspectAngleForTarget2D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform)
{
   UtVec3dX targetPositionWCS;
   aTargetPlatform.GetLocationWCS(targetPositionWCS.GetData());

   UtVec3dX targetVelocityWCS;
   aTargetPlatform.GetVelocityWCS(targetVelocityWCS.GetData());

   double targetHeading_rad = 0.0;
   double targetPitch_rad   = 0.0;
   double targetRoll_rad    = 0.0;
   aTargetPlatform.GetOrientationNED(targetHeading_rad, targetPitch_rad, targetRoll_rad);

   UtEntity targetEntity;
   targetEntity.SetLocationWCS(targetPositionWCS.GetData());
   targetEntity.SetVelocityWCS(targetPositionWCS.GetData());
   targetEntity.SetOrientationNED(targetHeading_rad, targetPitch_rad, targetRoll_rad);

   double platformPositionWCS[3];
   aPlatform.GetLocationWCS(platformPositionWCS);

   // This returns a bearing measured from the nose
   double bearing_rad = targetEntity.RelativeBearing(platformPositionWCS);

   // We need a bearing measured from the tail

   // If the bearing is positive, subtract the angle from Pi
   if (bearing_rad >= 0)
   {
      bearing_rad = UtMath::cPI - bearing_rad;
   }
   else
   {
      // Since the bearing is negative, subtract the angle from -Pi
      bearing_rad = -UtMath::cPI - bearing_rad;
   }

   return bearing_rad;
}

double WsfSA_Assess::AspectAngleForTarget2D(const WsfPlatform& aTargetPlatform) const
{
   return AspectAngleForTarget2D(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::AspectAngleForTarget3D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform)
{
   double targetVelocityWCS[3];
   aTargetPlatform.GetVelocityWCS(targetVelocityWCS);
   UtVec3dX targetVel(targetVelocityWCS);

   // Negate the velocity
   UtVec3dX negTgtVel = -targetVel;

   double ownshipPositionWCS[3];
   aPlatform.GetLocationWCS(ownshipPositionWCS);
   UtVec3dX ownshipPos(ownshipPositionWCS);

   double targetPositionWCS[3];
   aTargetPlatform.GetLocationWCS(targetPositionWCS);
   UtVec3dX targetPos(targetPositionWCS);

   UtVec3dX vecFromTargetToOwnship = ownshipPos - targetPos;

   double aspect_angle_rad = negTgtVel.AngleBetween(vecFromTargetToOwnship);

   double psi   = 0.0;
   double theta = 0.0;
   double phi   = 0.0;
   aTargetPlatform.GetOrientationWCS(psi, theta, phi);
   UtDCM    dcm(psi, theta, phi);
   UtVec3dX rightWingVec(0.0, 1.0, 0.0);
   UtVec3dX rightSideVec = dcm.InverseTransform(rightWingVec);

   double dot = vecFromTargetToOwnship.DotProduct(rightSideVec);
   if (dot < 0.0)
   {
      aspect_angle_rad *= -1.0;
   }

   return aspect_angle_rad;
}

double WsfSA_Assess::AspectAngleForTarget3D(const WsfPlatform& aTargetPlatform) const
{
   return AspectAngleForTarget3D(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::OffBoresightAngleForTarget(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform)
{
   double relLocationECS[3];
   aPlatform.GetRelativeLocationECS(&aTargetPlatform, relLocationECS);

   UtVec3dX vecToTgt(relLocationECS);
   UtVec3dX vecBoresight(1.0, 0.0, 0.0);

   return vecBoresight.AngleBetween(vecToTgt);
}

double WsfSA_Assess::OffBoresightAngleForTarget(const WsfPlatform& aTargetPlatform) const
{
   return OffBoresightAngleForTarget(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::OffBoresightAngleForTarget(const double aLocationWCS[3]) const
{
   double locationECS[3];
   GetPlatform()->ConvertWCSToECS(aLocationWCS, locationECS);

   UtVec3dX vecToTgt(locationECS);
   UtVec3dX vecBoresight(1.0, 0.0, 0.0);

   return vecBoresight.AngleBetween(vecToTgt);
}

double WsfSA_Assess::ExpectedDetectionRange(const WsfPlatform& aSourePlatform, const WsfPlatform& aTargetPlatform) const
{
   // Assumption:
   //     Radar range (in km) is (constant * signature)^0.25
   //        where:
   //           signature is the RCS in sq meters
   //           constant is a constant
   //     We adjust the constant to produce the desired range for a specified RCS
   //     We will determine the constant to produce a 200km range for a 10 sq-meter target
   //     This results in a constant of 160000000

   const double constant = 160000000;

   double locationSrc[3];
   aSourePlatform.GetLocationWCS(locationSrc);
   double locationTgt[3];
   aTargetPlatform.GetLocationWCS(locationTgt);
   double relativeLocationTgt[3] = {locationSrc[0] - locationTgt[0],
                                    locationSrc[1] - locationTgt[1],
                                    locationSrc[2] - locationTgt[2]};
   double az                     = 0.0;
   double el                     = 0.0;
   aTargetPlatform.ComputeAspect(relativeLocationTgt, az, el);

   // Determine the signature
   // Previously, we used the simple equation: crossSection = 5.0 + (20.0 * fabs(sin(az)))
   // Now, we use the TargetCrossSection function
   double crossSection = TargetCrossSection(aTargetPlatform, az, el);

   // Calc the range
   double range_km = pow(constant * crossSection, 0.25);

   // Convert to meters and return
   return range_km * 1000.0;
}

double WsfSA_Assess::TargetCrossSection(const WsfPlatform& aTargetPlatform, double aAzimuth, double aElevation) const
{
   if (aTargetPlatform.GetSide() == "blue")
   {
      return ::TargetCrossSection(mBlueAircraftSignature, aAzimuth, aElevation);
   }
   else
   {
      return ::TargetCrossSection(mRedAircraftSignature, aAzimuth, aElevation);
   }
}

double WsfSA_Assess::ExpectedDetectionRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   return ExpectedDetectionRange(*GetPlatform(), aTargetPlatform);
}

double WsfSA_Assess::ExpectedDetectionRangeAgainstTarget(size_t aIndex) const
{
   const WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedDetectionRangeAgainstTarget(*platform);
}

double WsfSA_Assess::ExpectedDetectionRangeByTarget(const WsfPlatform& aTargetPlatform) const
{
   return ExpectedDetectionRange(aTargetPlatform, *GetPlatform());
}

double WsfSA_Assess::ExpectedDetectionRangeByTarget(size_t aIndex) const
{
   const WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedDetectionRangeByTarget(*tgtPlatform);
}

double WsfSA_Assess::ExpectedNormalizedDetectionRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(aIndex);
   if (platform != nullptr)
   {
      return ExpectedNormalizedDetectionRangeAgainstTarget(*platform);
   };

   return 0.0;
}

double WsfSA_Assess::ExpectedNormalizedDetectionRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   WsfPlatform* ownPlatform = GetPlatform();
   double       slantRange  = CalcSlantRange(*ownPlatform, aTargetPlatform);

   if (slantRange > std::numeric_limits<double>::epsilon())
   {
      double normalizedRange =
         ExpectedDetectionRangeAgainstTarget(aTargetPlatform) / CalcSlantRange(*ownPlatform, aTargetPlatform);

      return UtMath::Limit(normalizedRange, 0.0, 1.0);
   }

   return 1.0;
}

double WsfSA_Assess::ExpectedNormalizedWezRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(aIndex);
   if (platform != nullptr)
   {
      return ExpectedNormalizedWezRangeAgainstTarget(*platform);
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedNormalizedWezRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   WsfPlatform* ownPlatform = GetPlatform();
   double       slantRange  = CalcSlantRange(*ownPlatform, aTargetPlatform);

   if (slantRange > std::numeric_limits<double>::epsilon())
   {
      double normalizedRange =
         ExpectedWezRangeAgainstTarget(aTargetPlatform) / CalcSlantRange(*ownPlatform, aTargetPlatform);

      return UtMath::Limit(normalizedRange, 0.0, 1.0);
   }

   return 1.0;
}

double WsfSA_Assess::ExpectedWezRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   MissileRangeParameters Rmax;
   MissileRangeParameters Rne;
   MissileRangeParameters Rmin;
   double                 currentRange = 0.0;
   bool                   engageable   = false;

   bool valid = SimpleWezCalculationAircraft(aTargetPlatform, Rmax, Rne, Rmin, currentRange, engageable);

   if (valid)
   {
      return Rmax.range;
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedWezRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* tgtPlatform = mSA_ProcessorPtr->GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedWezRangeAgainstTarget(*tgtPlatform);
}

double WsfSA_Assess::ExpectedMinimumEngagementRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   MissileRangeParameters Rmax;
   MissileRangeParameters Rne;
   MissileRangeParameters Rmin;
   double                 currentRange = 0.0;
   bool                   engageable   = false;

   bool valid = SimpleWezCalculationAircraft(aTargetPlatform, Rmax, Rne, Rmin, currentRange, engageable);

   if (valid)
   {
      return Rmin.range;
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedMinimumEngagementRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* tgtPlatform = mSA_ProcessorPtr->GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedMinimumEngagementRangeAgainstTarget(*tgtPlatform);
}


double WsfSA_Assess::ExpectedMaximumEngagementRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   MissileRangeParameters Rmax;
   MissileRangeParameters Rne;
   MissileRangeParameters Rmin;
   double                 currentRange = 0.0;
   bool                   engageable   = false;

   bool valid = SimpleWezCalculationAircraft(aTargetPlatform, Rmax, Rne, Rmin, currentRange, engageable);

   if (valid)
   {
      return Rmax.range;
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedMaximumEngagementRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* tgtPlatform = mSA_ProcessorPtr->GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedMaximumEngagementRangeAgainstTarget(*tgtPlatform);
}

double WsfSA_Assess::ExpectedNoEscapeRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const
{
   MissileRangeParameters Rmax;
   MissileRangeParameters Rne;
   MissileRangeParameters Rmin;
   double                 currentRange = 0.0;
   bool                   engageable   = false;

   bool valid = SimpleWezCalculationAircraft(aTargetPlatform, Rmax, Rne, Rmin, currentRange, engageable);

   if (valid)
   {
      return Rne.range;
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedNoEscapeRangeAgainstTarget(size_t aIndex) const
{
   WsfPlatform* tgtPlatform = mSA_ProcessorPtr->GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedNoEscapeRangeAgainstTarget(*tgtPlatform);
}

double WsfSA_Assess::ExpectedWezRangeByTarget(const WsfPlatform& aTargetPlatform) const
{
   WsfPlatform* ownPlatform = GetPlatform();

   MissileRangeParameters Rmax;
   MissileRangeParameters Rne;
   MissileRangeParameters Rmin;
   double                 currentRange = 0.0;
   bool                   engageable   = false;

   bool valid = SimpleWezCalculationAircraft(aTargetPlatform, *ownPlatform, Rmax, Rne, Rmin, currentRange, engageable);

   if (valid)
   {
      return Rmax.range;
   }

   return 0.0;
}

double WsfSA_Assess::ExpectedWezRangeByTarget(size_t aIndex) const
{
   WsfPlatform* tgtPlatform = mSA_ProcessorPtr->GetSimulation()->GetPlatformByIndex(aIndex);
   return ExpectedWezRangeByTarget(*tgtPlatform);
}

double WsfSA_Assess::CalcSlantRange(const WsfPlatform& aSourcePlatform, const WsfPlatform& aTargetPlatform) const
{
   UtVec3dX relativeLocationWCS;
   aSourcePlatform.GetRelativeLocationWCS(&aTargetPlatform, relativeLocationWCS.GetData());

   return relativeLocationWCS.Magnitude();
}

double WsfSA_Assess::CalcSlantRange(const WsfPlatform& aTargetPlatform) const
{
   WsfPlatform* ownPlatform = GetPlatform();
   if (ownPlatform != nullptr)
   {
      return CalcSlantRange(*ownPlatform, aTargetPlatform);
   }

   return 0.0;
}

double WsfSA_Assess::CalcDeltaAltitude(const WsfPlatform& aSourcePlatform, const WsfPlatform& aTargetPlatform) const
{
   return aTargetPlatform.GetAltitude() - aSourcePlatform.GetAltitude();
}

double WsfSA_Assess::CalcDeltaAltitude(const WsfPlatform& aTargetPlatform) const
{
   WsfPlatform* ownPlatform = GetPlatform();
   if (ownPlatform != nullptr)
   {
      return CalcDeltaAltitude(*ownPlatform, aTargetPlatform);
   }

   return 0.0;
}

std::vector<WsfSA_PerceivedItem*> WsfSA_Assess::PerceivedThreatItems(double aSimTime)
{
   std::vector<WsfSA_PerceivedItem*> ret{GetSAP()->GetData()->mPerceivedBandits.begin(),
                                         GetSAP()->GetData()->mPerceivedBandits.end()};
   for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
   {
      // Note: Although the bogie list may contain friendly and neutral entities,
      // the perceived threat list should not include friendlies and neutrals

      // Only add to the prioritized list if it is *NOT* a friendly or neutral
      if ((bogie->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (bogie->GetIdentification() != WsfSA_EntityPerception::NEUTRAL))
      {
         ret.emplace_back(bogie);
      }
   }
   for (auto& group : mPerceivedDataSummary->mCurrentlyPerceivedGroups)
   {
      if ((group->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (group->GetIdentification() != WsfSA_EntityPerception::NEUTRAL) && !group->GetFocus())
      {
         ret.emplace_back(group);
      }
   }
   return ret;
}

WsfSA_PerceivedItem* WsfSA_Assess::HighestThreat(double aSimTime)
{
   // Return the entity that poses the highest threat level. This may be a bandit or bogie.

   double               highestThreat = 0.0;
   WsfSA_PerceivedItem* bestThreat    = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double threatLevel = bogie->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double threatLevel = bandit->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bandit;
         }
      }
   }

   // Check groups
   for (const auto& group : GetSAP()->GetData()->mCurrentlyPerceivedGroups)
   {
      double threatLevel = group->GetThreatLevel();
      if (threatLevel > highestThreat)
      {
         highestThreat = threatLevel;
         bestThreat    = group;
      }
   }

   return bestThreat;
}

WsfSA_EntityPerception* WsfSA_Assess::HighestThreatEntity(double aSimTime)
{
   // Return the entity that poses the highest threat level. This may be a bandit or bogie.

   double                  highestThreat = 0.0;
   WsfSA_EntityPerception* bestThreat    = nullptr;

   // Check bogies
   for (const auto& bogie : GetSAP()->GetData()->mPerceivedBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double threatLevel = bogie->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : GetSAP()->GetData()->mPerceivedBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double threatLevel = bandit->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bandit;
         }
      }
   }

   return bestThreat;
}

WsfSA_EntityPerception* WsfSA_Assess::HighestThreatAircraft(double aSimTime)
{
   // Return the aircraft entity that poses the highest threat level. This may be a bandit or bogie.

   double                  highestThreat = 0.0;
   WsfSA_EntityPerception* bestThreat    = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedAircraftBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double threatLevel = bogie->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedAircraftBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double threatLevel = bandit->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bandit;
         }
      }
   }

   return bestThreat;
}

WsfSA_EntityPerception* WsfSA_Assess::HighestThreatMissile(double aSimTime)
{
   // Return the missile entity that poses the highest threat level. This may be a bandit or bogie.

   double                  highestThreat = 0.0;
   WsfSA_EntityPerception* bestThreat    = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedMissileBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double threatLevel = bogie->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedMissileBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double threatLevel = bandit->GetThreatLevel();
         if (threatLevel > highestThreat)
         {
            highestThreat = threatLevel;
            bestThreat    = bandit;
         }
      }
   }

   return bestThreat;
}

WsfSA_Group* WsfSA_Assess::HighestThreatGroup(double aSimTime)
{
   // Return the group that poses the highest threat level. This may be a bandit or bogie.

   double       highestThreat = 0.0;
   WsfSA_Group* bestThreat    = nullptr;

   for (const auto& group : GetSAP()->GetData()->mCurrentlyPerceivedGroups)
   {
      double threatLevel = group->GetThreatLevel();
      if (threatLevel > highestThreat && !group->GetFocus())
      {
         highestThreat = threatLevel;
         bestThreat    = group;
      }
   }

   return bestThreat;
}

WsfSA_PerceivedItem* WsfSA_Assess::BestTarget(double aSimTime)
{
   // Return the entity that poses the highest target score. This may be a bandit or bogie.

   double               highestTargetScore = 0.0;
   WsfSA_PerceivedItem* bestTarget         = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double targetValue = bogie->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double targetValue = bandit->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bandit;
         }
      }
   }

   for (const auto& group : GetSAP()->GetData()->mCurrentlyPerceivedGroups)
   {
      double targetValue = group->GetTargetValue();
      if (targetValue > highestTargetScore)
      {
         highestTargetScore = targetValue;
         bestTarget         = group;
      }
   }

   return bestTarget;
}

WsfSA_EntityPerception* WsfSA_Assess::BestTargetEntity(double aSimTime)
{
   // Return the entity that poses the highest target score. This may be a bandit or bogie.

   double                  highestTargetScore = 0.0;
   WsfSA_EntityPerception* bestTarget         = nullptr;

   // Check bogies
   for (const auto& bogie : GetSAP()->GetData()->mPerceivedBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double targetValue = bogie->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : GetSAP()->GetData()->mPerceivedBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double targetValue = bandit->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bandit;
         }
      }
   }

   return bestTarget;
}

WsfSA_EntityPerception* WsfSA_Assess::BestTargetAircraft(double aSimTime)
{
   // Return the aircraft entity that poses the highest threat level. This may be a bandit or bogie.

   double                  highestTargetScore = 0.0;
   WsfSA_EntityPerception* bestTarget         = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedAircraftBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double targetValue = bogie->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedAircraftBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double targetValue = bandit->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bandit;
         }
      }
   }

   return bestTarget;
}

WsfSA_EntityPerception* WsfSA_Assess::BestTargetMissile(double aSimTime)
{
   // Return the missile entity that poses the highest threat level. This may be a bandit or bogie.

   double                  highestTargetScore = 0.0;
   WsfSA_EntityPerception* bestTarget         = nullptr;

   // Check bogies
   for (const auto& bogie : mDataPtr->mPerceivedMissileBogies)
   {
      if (!bogie->GetParentGroup() || bogie->GetParentGroup()->GetFocus())
      {
         double targetValue = bogie->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bogie;
         }
      }
   }

   // Check bandits
   for (const auto& bandit : mDataPtr->mPerceivedMissileBandits)
   {
      if (!bandit->GetParentGroup() || bandit->GetParentGroup()->GetFocus())
      {
         double targetValue = bandit->GetTargetValue();
         if (targetValue > highestTargetScore)
         {
            highestTargetScore = targetValue;
            bestTarget         = bandit;
         }
      }
   }

   return bestTarget;
}

WsfSA_Group* WsfSA_Assess::BestTargetGroup(double aSimTime)
{
   // Return the group that poses the highest threat level. This may be a bandit or bogie.

   double       highestTargetScore = 0.0;
   WsfSA_Group* bestTarget         = nullptr;

   for (const auto& group : GetSAP()->GetData()->mCurrentlyPerceivedGroups)
   {
      double targetValue = group->GetTargetValue();
      if (targetValue > highestTargetScore && !group->GetFocus())
      {
         highestTargetScore = targetValue;
         bestTarget         = group;
      }
   }

   return bestTarget;
}

bool WsfSA_Assess::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command = aInput.GetCommand();

   if (command == "bogie_threat_score_multiplier")
   {
      aInput.ReadValue(mBogieThreatScoreMultiplier);
   }
   else if (command == "bogie_target_score_multiplier")
   {
      aInput.ReadValue(mBogieTargetScoreMultiplier);
   }
   else if (command == "mission_task")
   {
      aInput.ReadValue(mMissionTask);
   }
   else if (command == "max_prioritized_threats")
   {
      int tempmMaxPrioritizedThreats;
      aInput.ReadValue(tempmMaxPrioritizedThreats);
      aInput.ValueGreaterOrEqual(tempmMaxPrioritizedThreats, 0);
      mMaxPrioritizedThreatsInitial = tempmMaxPrioritizedThreats;
      mMaxPrioritizedThreats        = mMaxPrioritizedThreatsInitial;
   }
   else if (command == "max_prioritized_targets")
   {
      int tempmMaxPrioritizedTargets;
      aInput.ReadValue(tempmMaxPrioritizedTargets);
      aInput.ValueGreaterOrEqual(tempmMaxPrioritizedTargets, 0);
      mMaxPrioritizedTargetsInitial = tempmMaxPrioritizedTargets;
      mMaxPrioritizedTargets        = mMaxPrioritizedTargetsInitial;
   }
   else if (command == "ignore_missiles_as_threats")
   {
      aInput.ReadValue(mIgnoreMissilesAsThreats);
   }
   else if (command == "ignore_bogies_as_threats")
   {
      aInput.ReadValue(mIgnoreBogiesAsThreats);
   }
   else if (command == "ignore_missiles_as_targets")
   {
      aInput.ReadValue(mIgnoreMissilesAsTargets);
   }
   else if (command == "ignore_bogies_as_targets")
   {
      aInput.ReadValue(mIgnoreBogiesAsTargets);
   }
   else if (command == "missile_wez_parameters")
   {
      MissileWezParameters tempParameters(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      std::string          missileSide;
      std::string          missileType;

      bool avgSpeedSet                  = false;
      bool maxTimeFlightSet             = false;
      bool maxOffBoresightAngleSet      = false;
      bool minTimeFlightSet             = false;
      bool pkData_nominalPkSet          = false;
      bool pkData_headOnReductionSet    = false;
      bool pkData_broadsideReductionSet = false;

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "side")
         {
            aInput.ReadValue(missileSide);
            if (missileSide != "blue")
            {
               if (missileSide != "red")
               {
                  // Error
                  auto out = ut::log::error() << "Missile WEZ 'side' must be 'blue' or 'red'";
               }
            }
         }
         else if (command == "type")
         {
            aInput.ReadValue(missileType);
            if (missileType != "mrm")
            {
               if (missileType != "srm")
               {
                  // Error
                  auto out = ut::log::error() << "Missile WEZ 'type' must be 'mrm' or 'srm'";
               }
            }
         }
         else if (command == "avg_speed")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cSPEED);
            if (value >= 0)
            {
               tempParameters.avgSpeed = value;
               avgSpeedSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'avg_speed' must be positive";
            }
         }
         else if (command == "max_time_of_flight")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cTIME);
            if (value >= 0)
            {
               tempParameters.maxTimeFlight = value;
               maxTimeFlightSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'max_time_of_flight' must be positive";
            }
         }
         else if (command == "max_off_boresight_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            if ((value >= 0.0) && (value <= UtMath::cPI))
            {
               tempParameters.maxOffBoresightAngle = value;
               maxOffBoresightAngleSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'max_off_boresight_angle' must be between 0 and Pi";
            }
         }
         else if (command == "min_time_of_flight")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cTIME);
            if (value >= 0)
            {
               tempParameters.minTimeFlight = value;
               minTimeFlightSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'min_time_of_flight' must be positive";
            }
         }
         else if (command == "nominal_pk")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            if ((value >= 0.0) && (value <= 1.0))
            {
               tempParameters.pkData.nominalPk = value;
               pkData_nominalPkSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'nominal_pk' must be between 0 and 1.";
            }
         }
         else if (command == "head_on_pk_reduction")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            if ((value >= 0.0) && (value <= 1.0))
            {
               tempParameters.pkData.headOnReduction = value;
               pkData_headOnReductionSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'head_on_pk_reduction' must be between 0 and 1.";
            }
         }
         else if (command == "broadside_pk_reduction")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            if ((value >= 0.0) && (value <= 1.0))
            {
               tempParameters.pkData.broadsideReduction = value;
               pkData_broadsideReductionSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Missile WEZ 'broadside_pk_reduction' must be between 0 and 1.";
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Load the data into the appropriate MissileWezParameters object
      if ((missileSide == "blue") && (missileType == "mrm"))
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (avgSpeedSet)
         {
            mBlueMissileParametersMRM.avgSpeed = tempParameters.avgSpeed;
         }
         if (maxTimeFlightSet)
         {
            mBlueMissileParametersMRM.maxTimeFlight = tempParameters.maxTimeFlight;
         }
         if (maxOffBoresightAngleSet)
         {
            mBlueMissileParametersMRM.maxOffBoresightAngle = tempParameters.maxOffBoresightAngle;
         }
         if (minTimeFlightSet)
         {
            mBlueMissileParametersMRM.minTimeFlight = tempParameters.minTimeFlight;
         }
         if (pkData_nominalPkSet)
         {
            mBlueMissileParametersMRM.pkData.nominalPk = tempParameters.pkData.nominalPk;
         }
         if (pkData_headOnReductionSet)
         {
            mBlueMissileParametersMRM.pkData.headOnReduction = tempParameters.pkData.headOnReduction;
         }
         if (pkData_broadsideReductionSet)
         {
            mBlueMissileParametersMRM.pkData.broadsideReduction = tempParameters.pkData.broadsideReduction;
         }
      }
      else if ((missileSide == "blue") && (missileType == "srm"))
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (avgSpeedSet)
         {
            mBlueMissileParametersSRM.avgSpeed = tempParameters.avgSpeed;
         }
         if (maxTimeFlightSet)
         {
            mBlueMissileParametersSRM.maxTimeFlight = tempParameters.maxTimeFlight;
         }
         if (maxOffBoresightAngleSet)
         {
            mBlueMissileParametersSRM.maxOffBoresightAngle = tempParameters.maxOffBoresightAngle;
         }
         if (minTimeFlightSet)
         {
            mBlueMissileParametersSRM.minTimeFlight = tempParameters.minTimeFlight;
         }
         if (pkData_nominalPkSet)
         {
            mBlueMissileParametersSRM.pkData.nominalPk = tempParameters.pkData.nominalPk;
         }
         if (pkData_headOnReductionSet)
         {
            mBlueMissileParametersSRM.pkData.headOnReduction = tempParameters.pkData.headOnReduction;
         }
         if (pkData_broadsideReductionSet)
         {
            mBlueMissileParametersSRM.pkData.broadsideReduction = tempParameters.pkData.broadsideReduction;
         }
      }
      else if ((missileSide == "red") && (missileType == "mrm"))
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (avgSpeedSet)
         {
            mRedMissileParametersMRM.avgSpeed = tempParameters.avgSpeed;
         }
         if (maxTimeFlightSet)
         {
            mRedMissileParametersMRM.maxTimeFlight = tempParameters.maxTimeFlight;
         }
         if (maxOffBoresightAngleSet)
         {
            mRedMissileParametersMRM.maxOffBoresightAngle = tempParameters.maxOffBoresightAngle;
         }
         if (minTimeFlightSet)
         {
            mRedMissileParametersMRM.minTimeFlight = tempParameters.minTimeFlight;
         }
         if (pkData_nominalPkSet)
         {
            mRedMissileParametersMRM.pkData.nominalPk = tempParameters.pkData.nominalPk;
         }
         if (pkData_headOnReductionSet)
         {
            mRedMissileParametersMRM.pkData.headOnReduction = tempParameters.pkData.headOnReduction;
         }
         if (pkData_broadsideReductionSet)
         {
            mRedMissileParametersMRM.pkData.broadsideReduction = tempParameters.pkData.broadsideReduction;
         }
      }
      else if ((missileSide == "red") && (missileType == "srm"))
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (avgSpeedSet)
         {
            mRedMissileParametersSRM.avgSpeed = tempParameters.avgSpeed;
         }
         if (maxTimeFlightSet)
         {
            mRedMissileParametersSRM.maxTimeFlight = tempParameters.maxTimeFlight;
         }
         if (maxOffBoresightAngleSet)
         {
            mRedMissileParametersSRM.maxOffBoresightAngle = tempParameters.maxOffBoresightAngle;
         }
         if (minTimeFlightSet)
         {
            mRedMissileParametersSRM.minTimeFlight = tempParameters.minTimeFlight;
         }
         if (pkData_nominalPkSet)
         {
            mRedMissileParametersSRM.pkData.nominalPk = tempParameters.pkData.nominalPk;
         }
         if (pkData_headOnReductionSet)
         {
            mRedMissileParametersSRM.pkData.headOnReduction = tempParameters.pkData.headOnReduction;
         }
         if (pkData_broadsideReductionSet)
         {
            mRedMissileParametersSRM.pkData.broadsideReduction = tempParameters.pkData.broadsideReduction;
         }
      }
      else
      {
         // Error
         ut::log::error()
            << "Missile WEZ parameters must include 'side' (blue or red) and 'type' (mrm or srm) -- no values updated";
      }
   }
   else if (command == "aircraft_signature_parameters")
   {
      AircraftSignatureParameters tempParameters(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      std::string                 aircraftSide;

      bool fuselageFrontalAreaSet             = false;
      bool wingAreaSet                        = false;
      bool tailAreaSet                        = false;
      bool engineInletAreaSet                 = false;
      bool engineExitAreaSet                  = false;
      bool sweepAngleSet                      = false;
      bool signatureReductionFactorOverallSet = false;
      bool signatureReductionFactorNoseSet    = false;
      bool noseSignatureReductionAngleSet     = false;

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "side")
         {
            aInput.ReadValue(aircraftSide);
            if (aircraftSide != "blue")
            {
               if (aircraftSide != "red")
               {
                  // Error
                  auto out = ut::log::error() << "Aircraft 'side' must be 'blue' or 'red'";
               }
            }
         }
         else if (command == "fuselage_frontal_area")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            if (value >= 0)
            {
               tempParameters.fuselageFrontalArea = value;
               fuselageFrontalAreaSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'fuselage_frontal_area' must be positive";
            }
         }
         else if (command == "wing_area")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            if (value >= 0)
            {
               tempParameters.wingArea = value;
               wingAreaSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'wing_area' must be positive";
            }
         }
         else if (command == "tail_area")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            if (value >= 0)
            {
               tempParameters.tailArea = value;
               tailAreaSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'tail_area' must be positive";
            }
         }
         else if (command == "engine_inlet_area")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            if (value >= 0)
            {
               tempParameters.engineInletArea = value;
               engineInletAreaSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'engine_inlet_area' must be positive";
            }
         }
         else if (command == "engine_exit_area")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            if (value >= 0)
            {
               tempParameters.engineExitArea = value;
               engineExitAreaSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'engine_exit_area' must be positive";
            }
         }
         else if (command == "sweep_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            if (value >= 0)
            {
               tempParameters.sweepAngle = value;
               sweepAngleSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'sweep_angle' must be positive";
            }
         }
         else if (command == "signature_reduction_factor_overall")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            if ((value >= 0) && (value <= 1.0))
            {
               tempParameters.signatureReductionFactorOverall = value;
               signatureReductionFactorOverallSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'signature_reduction_factor_overall' must be between 0 and 1";
            }
         }
         else if (command == "signature_reduction_factor_nose")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            if ((value >= 0) && (value <= 1.0))
            {
               tempParameters.signatureReductionFactorNose = value;
               signatureReductionFactorNoseSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'signature_reduction_factor_nose' must be between 0 and 1";
            }
         }
         else if (command == "nose_signature_reduction_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            if ((value >= 0) && (value <= UtMath::cPI))
            {
               tempParameters.noseSignatureReductionAngle = value;
               noseSignatureReductionAngleSet             = true;
            }
            else
            {
               auto out = ut::log::error() << "Aircraft 'nose_signature_reduction_angle' must be between 0 and Pi";
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Load the data into the appropriate AircraftSignatureParameters object
      if (aircraftSide == "blue")
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (fuselageFrontalAreaSet)
         {
            mBlueAircraftSignature.fuselageFrontalArea = tempParameters.fuselageFrontalArea;
         }
         if (wingAreaSet)
         {
            mBlueAircraftSignature.wingArea = tempParameters.wingArea;
         }
         if (tailAreaSet)
         {
            mBlueAircraftSignature.tailArea = tempParameters.tailArea;
         }
         if (engineInletAreaSet)
         {
            mBlueAircraftSignature.engineInletArea = tempParameters.engineInletArea;
         }
         if (engineExitAreaSet)
         {
            mBlueAircraftSignature.engineExitArea = tempParameters.engineExitArea;
         }
         if (sweepAngleSet)
         {
            mBlueAircraftSignature.sweepAngle = tempParameters.sweepAngle;
         }
         if (signatureReductionFactorOverallSet)
         {
            mBlueAircraftSignature.signatureReductionFactorOverall = tempParameters.signatureReductionFactorOverall;
         }
         if (signatureReductionFactorNoseSet)
         {
            mBlueAircraftSignature.signatureReductionFactorNose = tempParameters.signatureReductionFactorNose;
         }
         if (noseSignatureReductionAngleSet)
         {
            mBlueAircraftSignature.noseSignatureReductionAngle = tempParameters.noseSignatureReductionAngle;
         }
      }
      if (aircraftSide == "red")
      {
         // We will only overwrite with valid data, otherwise will use the default settings
         if (fuselageFrontalAreaSet)
         {
            mRedAircraftSignature.fuselageFrontalArea = tempParameters.fuselageFrontalArea;
         }
         if (wingAreaSet)
         {
            mRedAircraftSignature.wingArea = tempParameters.wingArea;
         }
         if (tailAreaSet)
         {
            mRedAircraftSignature.tailArea = tempParameters.tailArea;
         }
         if (engineInletAreaSet)
         {
            mRedAircraftSignature.engineInletArea = tempParameters.engineInletArea;
         }
         if (engineExitAreaSet)
         {
            mRedAircraftSignature.engineExitArea = tempParameters.engineExitArea;
         }
         if (sweepAngleSet)
         {
            mRedAircraftSignature.sweepAngle = tempParameters.sweepAngle;
         }
         if (signatureReductionFactorOverallSet)
         {
            mRedAircraftSignature.signatureReductionFactorOverall = tempParameters.signatureReductionFactorOverall;
         }
         if (signatureReductionFactorNoseSet)
         {
            mRedAircraftSignature.signatureReductionFactorNose = tempParameters.signatureReductionFactorNose;
         }
         if (noseSignatureReductionAngleSet)
         {
            mRedAircraftSignature.noseSignatureReductionAngle = tempParameters.noseSignatureReductionAngle;
         }
      }
      else
      {
         // Error
         ut::log::error() << "Aircraft signature parameters must include 'side' (blue or red) -- no values updated";
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void WsfSA_Assess::Initialize(double aSimTime)
{
   // Get the pointer to the perceived model data
   WsfSA_Perceive* perceive = GetSAP()->Perceive();
   if (perceive != nullptr)
   {
      mPerceivedDataSummary = GetSAP()->GetData();
   }

   // Get function pointers to optional script functions

   mCalculateRiskScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateRisk");
   bool ok                 = GetSAP()->GetScriptContext().ValidateScript(mCalculateRiskScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateRiskScriptPtr = nullptr;
   }

   mCalculateSelfRiskScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateSelfRisk");
   ok                          = GetSAP()->GetScriptContext().ValidateScript(mCalculateSelfRiskScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateSelfRiskScriptPtr = nullptr;
   }

   mCalculateFlightRiskScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateFlightRisk");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateFlightRiskScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateFlightRiskScriptPtr = nullptr;
   }

   mCalculatePackageRiskScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculatePackageRisk");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculatePackageRiskScriptPtr, "double", "");
   if (!ok)
   {
      mCalculatePackageRiskScriptPtr = nullptr;
   }

   mCalculateMissionRiskScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateMissionRisk");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateMissionRiskScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateMissionRiskScriptPtr = nullptr;
   }

   mCalculateDefensivenessScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateDefensiveness");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateDefensivenessScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateDefensivenessScriptPtr = nullptr;
   }

   mCalculateUrgencyScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateUrgency");
   ok                         = GetSAP()->GetScriptContext().ValidateScript(mCalculateUrgencyScriptPtr, "double", "");
   if (!ok)
   {
      mCalculateUrgencyScriptPtr = nullptr;
   }

   mCalculateEntityThreatLevelScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateThreatLevel");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateEntityThreatLevelScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception, bool");
   if (!ok)
   {
      mCalculateEntityThreatLevelScriptPtr = nullptr;
   }

   mCalculateEntityTargetValueScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateTargetValue");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateEntityTargetValueScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception, bool");
   if (!ok)
   {
      mCalculateEntityTargetValueScriptPtr = nullptr;
   }

   mCalculateMissileThreatLevelScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateMissileThreatLevel");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateMissileThreatLevelScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception, bool");
   if (!ok)
   {
      mCalculateMissileThreatLevelScriptPtr = nullptr;
   }

   mCalculateMissileTargetValueScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateMissileTargetValue");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateMissileTargetValueScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception, bool");
   if (!ok)
   {
      mCalculateMissileTargetValueScriptPtr = nullptr;
   }

   mCalculateGroupThreatLevelScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateGroupThreatLevel");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateGroupThreatLevelScriptPtr, "double", "WsfSA_Group, bool");
   if (!ok)
   {
      mCalculateGroupThreatLevelScriptPtr = nullptr;
   }

   mCalculateGroupTargetValueScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateGroupTargetValue");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateGroupTargetValueScriptPtr, "double", "WsfSA_Group, bool");
   if (!ok)
   {
      mCalculateGroupTargetValueScriptPtr = nullptr;
   }

   mCalculateRiskPosedByEntityScriptPtr = GetSAP()->GetScriptContext().FindScript("CalculateRiskPosedByEntity");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateRiskPosedByEntityScriptPtr, "double", "WsfSA_EntityPerception");
   if (!ok)
   {
      mCalculateRiskPosedByEntityScriptPtr = nullptr;
   }

   mCalculateDefensivenessInducedByEntityScriptPtr =
      GetSAP()->GetScriptContext().FindScript("CalculateDefensivenessInducedByEntity");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateDefensivenessInducedByEntityScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception");
   if (!ok)
   {
      mCalculateDefensivenessInducedByEntityScriptPtr = nullptr;
   }

   mCalculateUrgencyInducedByEntityScriptPtr =
      GetSAP()->GetScriptContext().FindScript("CalculateUrgencyInducedByEntity");
   ok = GetSAP()->GetScriptContext().ValidateScript(mCalculateUrgencyInducedByEntityScriptPtr,
                                                    "double",
                                                    "WsfSA_EntityPerception");
   if (!ok)
   {
      mCalculateUrgencyInducedByEntityScriptPtr = nullptr;
   }

   mCalculateWeaponSupport = GetSAP()->GetScriptContext().FindScript("CalculateWeaponSupport");
   ok                      = GetSAP()->GetScriptContext().ValidateScript(mCalculateWeaponSupport, "bool", "");
   if (!ok)
   {
      mCalculateWeaponSupport = nullptr;
   }
}

double WsfSA_Assess::TryUpdateEngagements(double aSimTime)
{
   // Perform the update
   UpdateEngagements(aSimTime);

   return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA));
}

double WsfSA_Assess::TryUpdatePrioritizedItemData(double aSimTime)
{
   // Perform the update
   UpdatePrioritizedItemData(aSimTime);

   return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA));
}

double WsfSA_Assess::TryUpdatePrioritizedItemCalculation(double aSimTime)
{
   // Perform the update
   UpdatePrioritizedItemCalculation(aSimTime);

   return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION));
}

void WsfSA_Assess::UpdateEngagements(double aSimTime)
{
   if (aSimTime > GetSAP()->GetLastUpdateTime(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA))
   {
      // Update the engagement data
      UpdateEngagementsData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendEngagementSummaryData(GetSimulation())(aSimTime, *GetPlatform(), *GetSAP());
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA, aSimTime);
   }
}

void WsfSA_Assess::UpdatePrioritizedItemData(double aSimTime)
{
   if (aSimTime > GetSAP()->GetLastUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA))
   {
      // Send the data
      WsfObserver::AirCombatSendPrioritizedThreatsAndTargetsData(GetSimulation())(aSimTime, *GetPlatform(), *GetSAP());

      // Update the last update time
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA, aSimTime);
   }
}

void WsfSA_Assess::UpdatePrioritizedItemCalculation(double aSimTime)
{
   if (aSimTime > GetSAP()->GetLastUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION))
   {
      // Use the perceived bogies/threats to create prioritized targets and threats
      UpdatePrioritizedThreatsAndTargetsData(aSimTime);

      // Update the last update time
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION, aSimTime);
   }
}

void WsfSA_Assess::UpdateEngagementsData(double aSimTime)
{
   const std::vector<size_t> platformIdList = GetSAP()->GetFilteredPlatformIdList();

   // This will update the list of currently tracked platforms (mCurrentlyTrackedPlatformNames and
   // mCurrentlyTrackedPlatformIds)
   CalculateTrackedPlatforms(aSimTime);

   // Clear old data
   mEngagementDataList.clear();

   // Loop through the filtered platforms
   for (const auto& platformId : platformIdList)
   {
      // Get the current target platform
      WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(platformId);

      if (tgtPlatform != nullptr)
      {
         if (tgtPlatform->GetIndex() != GetPlatform()->GetIndex()) // Be sure to exclude "self"
         {
            AssessEngagementData engagementData;

            // First, calc the range
            engagementData.currentRange_nm = static_cast<float>(CalcSlantRange(*tgtPlatform) * UtMath::cNM_PER_M);

            // Next, check for filtering by range
            bool filteredByRange = false;
            if (GetSAP()->UseMaxRangeForEngagementData())
            {
               if (engagementData.currentRange_nm > GetSAP()->MaxRangeForEngagementData())
               {
                  filteredByRange = true;
               }
            }

            if (!filteredByRange)
            {
               engagementData.targetIndex  = tgtPlatform->GetIndex();
               engagementData.targetName   = tgtPlatform->GetName();
               engagementData.targetType   = tgtPlatform->GetType();
               engagementData.angleOff_deg = static_cast<float>(AngleOffTarget2D(*tgtPlatform) * UtMath::cDEG_PER_RAD);
               engagementData.aspectAngle_deg =
                  static_cast<float>(AspectAngleForTarget2D(*tgtPlatform) * UtMath::cDEG_PER_RAD);
               engagementData.currentDeltaAlt_ft = static_cast<float>(CalcDeltaAltitude(*tgtPlatform) * UtMath::cFT_PER_M);
               engagementData.detectionRange_nm =
                  static_cast<float>(ExpectedDetectionRangeAgainstTarget(*tgtPlatform) * UtMath::cNM_PER_M);
               engagementData.wezDesiredRange_nm =
                  static_cast<float>(ExpectedWezRangeAgainstTarget(*tgtPlatform) * UtMath::cNM_PER_M);
               engagementData.wezMinimum_nm =
                  static_cast<float>(ExpectedMinimumEngagementRangeAgainstTarget(*tgtPlatform) * UtMath::cNM_PER_M);
               engagementData.wezMaximum_nm =
                  static_cast<float>(ExpectedMaximumEngagementRangeAgainstTarget(*tgtPlatform) * UtMath::cNM_PER_M);
               engagementData.wezNoEscape_nm =
                  static_cast<float>(ExpectedNoEscapeRangeAgainstTarget(*tgtPlatform) * UtMath::cNM_PER_M);

               // TODO - May want an acceptable WEZ that is not just Rmax
               if (engagementData.currentRange_nm < engagementData.wezDesiredRange_nm)
               {
                  engagementData.acceptableWez = true;
               }
               else
               {
                  engagementData.acceptableWez = false;
               }

               // This flag determines if the entity is considered a threat (used below)
               bool isThreat = false;

               engagementData.tgtTracked = PlatformIsCurrentlyBeingTracked(tgtPlatform->GetIndex());
               if (engagementData.tgtTracked)
               {
                  // Get the track
                  WsfLocalTrack* track = GetPerceivedTrackedPlatformWithTarget(engagementData.targetIndex);
                  if (track != nullptr)
                  {
                     // Target platform must be valid (non-null) or it must be a false target
                     WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(track->GetTargetIndex());
                     if ((tgtPlatform != nullptr) || (track->IsFalseTarget()))
                     {
                        engagementData.trackQuality = static_cast<float>(track->GetTrackQuality());

                        // TODO -- There may be a CID associated with tracks in the future, but for now,
                        // simply use the following id technique:

                        bool idAchieved = false;

                        // First attempt a type-based identification
                        if (track->TypeIdValid())
                        {
                           std::string trackType = track->GetTypeId().GetString();

                           if (GetSAP()->UseSimpleIdByType())
                           {
                              // This provides a simulated, simple identification by type. Basically,
                              // when using the "simple" approach, we confirm that the track supports
                              // type ID, and then we check the target platform truth "side", even if
                              // the side data is not necessarily provided in the track.

                              size_t       tgtIndex    = track->GetTargetIndex();
                              WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(tgtIndex);
                              if (tgtPlatform != nullptr)
                              {
                                 std::string truthSide = tgtPlatform->GetSide();

                                 if (GetSAP()->IsEnemySide(truthSide))
                                 {
                                    idAchieved               = true;
                                    isThreat                 = true;
                                    engagementData.targetCID = trackType;
                                 }
                                 else if (GetSAP()->IsFriendlySide(truthSide))
                                 {
                                    idAchieved               = true;
                                    engagementData.targetCID = trackType;
                                 }
                                 else if (GetSAP()->IsNeutralSide(truthSide))
                                 {
                                    idAchieved               = true;
                                    engagementData.targetCID = trackType;
                                 }
                              }
                           }
                           else
                           {
                              if (GetSAP()->IsEnemyType(trackType))
                              {
                                 idAchieved               = true;
                                 isThreat                 = true;
                                 engagementData.targetCID = trackType;
                              }
                              else if (GetSAP()->IsFriendlyType(trackType))
                              {
                                 idAchieved               = true;
                                 engagementData.targetCID = trackType;
                              }
                              else if (GetSAP()->IsNeutralType(trackType))
                              {
                                 idAchieved               = true;
                                 engagementData.targetCID = trackType;
                              }
                           }
                        }

                        if (!idAchieved && track->SideIdValid())
                        {
                           if (GetSAP()->IsEnemySide(track->GetSideId().GetString()))
                           {
                              idAchieved               = true;
                              isThreat                 = true;
                              engagementData.targetCID = "SIDE-FOE";
                           }
                           else if (GetSAP()->IsFriendlySide(track->GetSideId().GetString()))
                           {
                              idAchieved               = true;
                              engagementData.targetCID = "SIDE-FRIEND";
                           }
                           else if (GetSAP()->IsNeutralSide(track->GetSideId().GetString()))
                           {
                              idAchieved               = true;
                              engagementData.targetCID = "SIDE-NEUTRAL";
                           }
                        }

                        if (!idAchieved && GetSAP()->UseIFF() && track->IFF_StatusValid())
                        {
                           if (track->GetIFF_Status() == WsfTrack::cIFF_FOE)
                           {
                              idAchieved               = true;
                              isThreat                 = true;
                              engagementData.targetCID = "IFF-FOE";
                           }
                           else if (track->GetIFF_Status() == WsfTrack::cIFF_FRIEND)
                           {
                              idAchieved               = true;
                              engagementData.targetCID = "IFF-FRIEND";
                           }
                           else if (track->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL)
                           {
                              idAchieved               = true;
                              engagementData.targetCID = "IFF-NEUTRAL";
                           }
                        }

                        if (!idAchieved)
                        {
                           // Unable to identify
                           engagementData.targetCID = "";
                        }
                     }
                     else
                     {
                        // Target platform is not valid (non-null)
                        engagementData.tgtTracked   = false;
                        engagementData.trackQuality = 0.0f;
                        engagementData.targetCID    = "";
                     }
                  }
                  else
                  {
                     // Track is not valid
                     engagementData.tgtTracked   = false;
                     engagementData.trackQuality = 0.0f;
                     engagementData.targetCID    = "";
                  }
               }
               else
               {
                  // Target is not being tracked
                  engagementData.tgtTracked   = false;
                  engagementData.trackQuality = 0.0f;
                  engagementData.targetCID    = "";
               }

               // Create a temporary SA entity perception and use it to calc threat, target, risk, defensiveness, and urgency
               WsfSA_EntityPerception tempEntity(GetSimulation(), aSimTime, GetPlatform(), *tgtPlatform, false);
               engagementData.threatLevel = static_cast<float>(CalculateThreatLevel(aSimTime, &tempEntity, !isThreat));
               engagementData.targetValue = static_cast<float>(CalculateTargetValue(aSimTime, &tempEntity, !isThreat));
               engagementData.risk        = static_cast<float>(CalculateRisk(aSimTime, &tempEntity));
               engagementData.defensiveness = static_cast<float>(CalculateDefensiveness(aSimTime, &tempEntity));
               engagementData.urgency       = static_cast<float>(CalculateUrgency(aSimTime, &tempEntity));

               // Add this item to the list
               mEngagementDataList.push_back(engagementData);
            }
         }
      }
   }

   // Calculate various ancillary data
   GetSAP()->CalculateEmissions();   // Updates mRadarEmitting, mJammerEmitting, and mOtherSystemsEmitting
   GetSAP()->CalculateObservables(); // Updates mAfterburnerOn and mIsContrailing
}

void WsfSA_Assess::UpdatePrioritizedThreatsAndTargetsData(double aSimTime)
{
   // Note: The bogies and bandits have already had an assessment of their threat level,
   // risk, defensiveness and urgency when the lists were created, calling AssessBogiesBandits().
   // TODO - Review this approach to determine if it is better to assess these when bogies/bandits
   // lists are created (in perceive) or when threats/targets are prioritized (here)

   // Use the mPerceivedBogies and mPerceivedBandits to create prioritized lists of threats and targets
   CalculatePrioritizedThreats(aSimTime);
   CalculatePrioritizedTargets(aSimTime);

   CalculateOverallRisks(aSimTime);         // Updates mRisk, mSelfRisk, mFlightRisk, mPackageRisk, and mMissionRisk
   CalculateOverallDefensiveness(aSimTime); // Updates mDefensiveness
   CalculateOverallUrgency(aSimTime);       // Updates mUrgency
}

void WsfSA_Assess::CalculateTrackedPlatforms(double aSimTime)
{
   if (aSimTime > mLastTimeCalculatedTrackedPlatforms)
   {
      mCurrentlyTrackedPlatformNames.clear();
      mCurrentlyTrackedPlatformIds.clear();

      std::string pmtpName = GetSAP()->PerceptionMasterTrackProcessor();

      const std::vector<WsfSA_Processor::SA_TrackManagerData>& trkMgrData = GetSAP()->GetTrackManagerList();
      for (auto& trkMgr : trkMgrData)
      {
         bool validTrackManager = false;
         if (GetSAP()->UsingPerceptionMasterTrackProcessor() && (trkMgr.name == pmtpName))
         {
            validTrackManager = true;
         }
         else if (trkMgr.isMasterProcessor)
         {
            validTrackManager = true;
         }

         if (validTrackManager)
         {
            for (auto& trk : trkMgr.trackList)
            {
               size_t index = trk.targetPlatformIndex;

               // Check if index is already present in list
               std::vector<size_t>::iterator it =
                  std::find(mCurrentlyTrackedPlatformIds.begin(), mCurrentlyTrackedPlatformIds.end(), index);

               // If it is not in the list add it, else skip it
               if (it == mCurrentlyTrackedPlatformIds.end())
               {
                  // Not in the list, so add it
                  mCurrentlyTrackedPlatformIds.emplace_back(index);
                  mCurrentlyTrackedPlatformNames.emplace_back(trk.tgtPlatformName);
               }
            }
            break; // Exit the trkMgrData loop
         }
      }

      // Update the time
      mLastTimeCalculatedTrackedPlatforms = aSimTime;
   }
}

bool WsfSA_Assess::PlatformIsCurrentlyBeingTracked(size_t aTgtPlatformIndex) const
{
   for (auto& id : mCurrentlyTrackedPlatformIds)
   {
      if (id == aTgtPlatformIndex)
      {
         return true;
      }
   }

   return false;
}

WsfLocalTrack* WsfSA_Assess::GetPerceivedTrackedPlatformWithTarget(size_t aTgtPlatformIndex)
{
   WsfTrackManager&   trackMgr   = GetPlatform()->GetTrackManager();
   WsfLocalTrackList& tmpTrkList = trackMgr.GetTrackList();

   // Note: The track list will either use the MTP list (default) or the PMTP (optional)
   WsfLocalTrackList* trackList = &tmpTrkList;
   if (GetSAP()->UsingPerceptionMasterTrackProcessor())
   {
      // Use PMTP instead of MTP
      trackList = GetSAP()->GetPerceptionMasterTrackProcessorTrackList();
   }

   unsigned int numTracks = trackList->GetTrackCount();
   for (unsigned int i = 0; i < numTracks; ++i)
   {
      WsfLocalTrack* trk = trackList->GetTrackEntry(i);
      if (trk != nullptr)
      {
         if (trk->GetTargetIndex() == aTgtPlatformIndex)
         {
            return trk;
         }
      }
   }

   return nullptr;
}

void WsfSA_Assess::AssessBogiesBandits(double aSimTime)
{
   // Assess the threat level, risk, defensiveness and urgency presented by each bogie/threat

   for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
   {
      CalculateThreatLevel(aSimTime, bogie, true);
      CalculateTargetValue(aSimTime, bogie, true);
      CalculateRisk(aSimTime, bogie);
      CalculateDefensiveness(aSimTime, bogie);
      CalculateUrgency(aSimTime, bogie);
   }

   for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
   {
      CalculateThreatLevel(aSimTime, bandit, false);
      CalculateTargetValue(aSimTime, bandit, false);
      CalculateRisk(aSimTime, bandit);
      CalculateDefensiveness(aSimTime, bandit);
      CalculateUrgency(aSimTime, bandit);
   }
}

void WsfSA_Assess::CalculatePrioritizedThreats(double aSimTime)
{
   // Use the perceived bogies/threats to create prioritized threats
   // These are temporary lists of entities for scoring
   mPrioritizedThreatItems.clear();
   for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
   {
      CalculateThreatLevel(aSimTime, bogie, true);

      // Note: Although the bogie list may contain friendly and neutral entities,
      // the prioritized target and threat lists should not include friendlies and neutrals

      // Only add to the prioritized list if it is *NOT* a friendly or neutral
      if ((bogie->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (bogie->GetIdentification() != WsfSA_EntityPerception::NEUTRAL) &&
          (bogie->GetParentGroup() && bogie->GetParentGroup()->GetFocus()))
      {
         mPrioritizedThreatItems.emplace_back(bogie);
      }
   }

   for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
   {
      CalculateThreatLevel(aSimTime, bandit, false);

      if (bandit->GetParentGroup() && bandit->GetParentGroup()->GetFocus())
      {
         mPrioritizedThreatItems.emplace_back(bandit);
      }
   }

   for (auto& group : mPerceivedDataSummary->mCurrentlyPerceivedGroups)
   {
      CalculateThreatLevel(aSimTime, group, group->GetIdentification() != WsfSA_PerceivedItem::BANDIT);

      if ((group->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (group->GetIdentification() != WsfSA_EntityPerception::NEUTRAL) && (!group->GetFocus()))
      {
         mPrioritizedThreatItems.emplace_back(group);
      }
   }

   // Sort the list using bandit level for scoring
   auto sorting = [](const UtWeakReference<WsfSA_PerceivedItem>& a, const UtWeakReference<WsfSA_PerceivedItem>& b)
   { return WsfSA_PerceivedItem::CompareThreatLevelHighGood(a.Get(), b.Get()); };
   mPrioritizedThreatItems.sort(sorting);

   if (mMaxPrioritizedThreats > 0 && mPrioritizedThreatItems.size() > mMaxPrioritizedThreats)
   {
      mPrioritizedThreatItems.resize(mMaxPrioritizedThreats);
   }
   // Now, place the prioritized platforms into the two lists and Populate the aircraft and missile sublists
   mPrioritizedThreatNames.clear();
   mPrioritizedThreatIds.clear();
   mPrioritizedThreatEntities.clear();
   mPrioritizedAircraftThreatEntities.clear();
   mPrioritizedMissileThreatEntities.clear();
   mPrioritizedThreatGroups.clear();
   for (auto& item : mPrioritizedThreatItems)
   {
      size_t      tgtIndex = 0;
      std::string name     = "";

      if (item->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         WsfSA_EntityPerception* entity = dynamic_cast<WsfSA_EntityPerception*>(item.Get());
         if (entity->GetTrackPtr() != nullptr)
         {
            // This is track-based
            tgtIndex = entity->GetTrackPtr()->GetTargetIndex();
            name     = entity->GetTrackPtr()->GetTargetName();
         }
         else
         {
            // This is platform-based
            tgtIndex              = entity->GetEntityPlatformIndex();
            WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(tgtIndex);
            if (platform != nullptr)
            {
               name = platform->GetName();
            }
         }

         mPrioritizedThreatEntities.emplace_back(entity);
         if (entity->GetIsMissile())
         {
            mPrioritizedMissileThreatEntities.emplace_back(entity);
         }
         else
         {
            mPrioritizedAircraftThreatEntities.emplace_back(entity);
         }

         mPrioritizedThreatNames.emplace_back(name);
         mPrioritizedThreatIds.emplace_back(tgtIndex);
      }
      else
      {
         name     = item->GetPerceivedName();
         tgtIndex = 0; // Using 0 here to denote that this is a group, to prevent it from being mixed up with a platform.
         mPrioritizedThreatGroups.emplace_back(dynamic_cast<WsfSA_Group*>(item.Get()));
      }

      mPrioritizedThreatNames.emplace_back(name);
      mPrioritizedThreatIds.emplace_back(tgtIndex);
   }
}

void WsfSA_Assess::CalculatePrioritizedTargets(double aSimTime)
{
   // Use the perceived bogies/threats to create prioritized targets
   // These are temporary lists of entities for scoring
   mPrioritizedTargetItems.clear();
   for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
   {
      CalculateTargetValue(aSimTime, bogie, true);
      // Note: Although the bogie list may contain friendly and neutral entities,
      // the prioritized target and threat lists should not include friendlies and neutrals.

      // Only add to the prioritized list if it is *NOT* a friendly or neutral
      if ((bogie->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (bogie->GetIdentification() != WsfSA_EntityPerception::NEUTRAL) &&
          (bogie->GetParentGroup() && bogie->GetParentGroup()->GetFocus()))
      {
         mPrioritizedTargetItems.emplace_back(bogie);
      }
   }

   for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
   {
      CalculateTargetValue(aSimTime, bandit, false);

      if (bandit->GetParentGroup() && bandit->GetParentGroup()->GetFocus())
      {
         mPrioritizedTargetItems.emplace_back(bandit);
      }
   }

   for (auto& group : mPerceivedDataSummary->mCurrentlyPerceivedGroups)
   {
      CalculateTargetValue(aSimTime, group, group->GetIdentification() != WsfSA_PerceivedItem::BANDIT);

      if ((group->GetIdentification() != WsfSA_EntityPerception::FRIENDLY) &&
          (group->GetIdentification() != WsfSA_EntityPerception::NEUTRAL) && (!group->GetFocus()))
      {
         mPrioritizedTargetItems.emplace_back(group);
      }
   }

   // Sort the list using target value for scoring
   auto sorting = [](const UtWeakReference<WsfSA_PerceivedItem>& a, const UtWeakReference<WsfSA_PerceivedItem>& b)
   { return WsfSA_PerceivedItem::CompareTargetValueHighGood(a.Get(), b.Get()); };
   mPrioritizedTargetItems.sort(sorting);

   if (mMaxPrioritizedTargets > 0 && mPrioritizedTargetItems.size() > mMaxPrioritizedTargets)
   {
      mPrioritizedTargetItems.resize(mMaxPrioritizedTargets);
   }
   // Now, place the prioritized platforms into the two lists and populate the aircraft and missile sublists
   mPrioritizedTargetNames.clear();
   mPrioritizedTargetIds.clear();
   mPrioritizedTargetEntities.clear();
   mPrioritizedAircraftTargetEntities.clear();
   mPrioritizedMissileTargetEntities.clear();
   mPrioritizedTargetGroups.clear();
   for (auto item : mPrioritizedTargetItems)
   {
      size_t      tgtIndex = 0;
      std::string name     = "";
      if (item->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         auto entity = dynamic_cast<WsfSA_EntityPerception*>(item.Get());
         if (entity->GetTrackPtr() != nullptr)
         {
            // This is track-based
            tgtIndex = entity->GetTrackPtr()->GetTargetIndex();
            name     = entity->GetTrackPtr()->GetTargetName();
         }
         else
         {
            // This is platform-based
            tgtIndex              = entity->GetEntityPlatformIndex();
            WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(tgtIndex);
            if (platform != nullptr)
            {
               name = platform->GetName();
            }
         }

         mPrioritizedTargetEntities.emplace_back(entity);
         if (entity->GetIsMissile())
         {
            mPrioritizedMissileTargetEntities.emplace_back(entity);
         }
         else
         {
            mPrioritizedAircraftTargetEntities.emplace_back(entity);
         }
      }
      else
      {
         name     = item->GetPerceivedName();
         tgtIndex = 0;
         mPrioritizedTargetGroups.emplace_back(dynamic_cast<WsfSA_Group*>(item.Get()));
      }

      mPrioritizedTargetNames.emplace_back(name);
      mPrioritizedTargetIds.emplace_back(tgtIndex);
   }
}

void WsfSA_Assess::UpdateWeaponSupport(double aSimTime)
{
   // CalcWeaponSupport - Use the script-based function, if available
   if (mCalculateWeaponSupport != nullptr)
   {
      mSupportingWeapon = ExecuteCalcWeaponSupport(aSimTime);
   }
   else
   {
      mSupportingWeapon = false;
   }
}

void WsfSA_Assess::CalculateOverallRisks(double aSimTime)
{
   // The *overall* risk is the highest risk posed by any bogie or threat

   // CalculateRisk - Use the script-based function, if available
   if (mCalculateRiskScriptPtr != nullptr)
   {
      mRisk = ExecuteCalculateRisk(aSimTime);
   }
   else
   {
      double overallRisk = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetRisk() > overallRisk)
         {
            overallRisk = bogie->GetRisk();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetRisk() > overallRisk)
         {
            overallRisk = bandit->GetRisk();
         }
      }

      mRisk = overallRisk;
   }

   // CalculateSelfRisk - Use the script-based function, if available
   if (mCalculateSelfRiskScriptPtr != nullptr)
   {
      mSelfRisk = ExecuteCalculateSelfRisk(aSimTime);
   }
   else
   {
      double overallRisk = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetRisk() > overallRisk)
         {
            overallRisk = bogie->GetRisk();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetRisk() > overallRisk)
         {
            overallRisk = bandit->GetRisk();
         }
      }

      mSelfRisk = overallRisk;
   }

   // CalculateFlightRisk - Use the script-based function, if available
   if (mCalculateFlightRiskScriptPtr != nullptr)
   {
      mFlightRisk = ExecuteCalculateFlightRisk(aSimTime);
   }
   else
   {
      double overallRisk = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetRisk() > overallRisk)
         {
            overallRisk = bogie->GetRisk();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetRisk() > overallRisk)
         {
            overallRisk = bandit->GetRisk();
         }
      }

      mFlightRisk = overallRisk;
   }

   // CalculatePackageRisk - Use the script-based function, if available
   if (mCalculatePackageRiskScriptPtr != nullptr)
   {
      mPackageRisk = ExecuteCalculatePackageRisk(aSimTime);
   }
   else
   {
      double overallRisk = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetRisk() > overallRisk)
         {
            overallRisk = bogie->GetRisk();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetRisk() > overallRisk)
         {
            overallRisk = bandit->GetRisk();
         }
      }

      mPackageRisk = overallRisk;
   }

   // CalculateMissionRisk - Use the script-based function, if available
   if (mCalculateMissionRiskScriptPtr != nullptr)
   {
      mMissionRisk = ExecuteCalculateMissionRisk(aSimTime);
   }
   else
   {
      double overallRisk = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetRisk() > overallRisk)
         {
            overallRisk = bogie->GetRisk();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetRisk() > overallRisk)
         {
            overallRisk = bandit->GetRisk();
         }
      }

      mMissionRisk = overallRisk;
   }

   // Ensure limits are imposed
   mRisk        = UtMath::Limit(mRisk, 0.0, 1.0);
   mSelfRisk    = UtMath::Limit(mSelfRisk, 0.0, 1.0);
   mFlightRisk  = UtMath::Limit(mFlightRisk, 0.0, 1.0);
   mPackageRisk = UtMath::Limit(mPackageRisk, 0.0, 1.0);
   mMissionRisk = UtMath::Limit(mMissionRisk, 0.0, 1.0);
}

void WsfSA_Assess::CalculateOverallDefensiveness(double aSimTime)
{
   // Use the script-based function, if available
   if (mCalculateDefensivenessScriptPtr != nullptr)
   {
      mDefensiveness = ExecuteCalculateDefensiveness(aSimTime);
   }
   else
   {
      // The *overall* defensiveness is the highest defensiveness posed by any bogie or threat

      double overallDefensiveness = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetDefensiveness() > overallDefensiveness)
         {
            overallDefensiveness = bogie->GetDefensiveness();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetDefensiveness() > overallDefensiveness)
         {
            overallDefensiveness = bandit->GetDefensiveness();
         }
      }

      mDefensiveness = overallDefensiveness;
   }

   // Ensure limits are imposed
   mDefensiveness = UtMath::Limit(mDefensiveness, 0.0, 1.0);
}

void WsfSA_Assess::CalculateOverallUrgency(double aSimTime)
{
   // Use the script-based function, if available
   if (mCalculateUrgencyScriptPtr != nullptr)
   {
      mUrgency = ExecuteCalculateUrgency(aSimTime);
   }
   else
   {
      // The *overall* urgency is the highest urgency posed by any bogie or threat

      double overallUrgency = 0.0;
      for (auto& bogie : mPerceivedDataSummary->mPerceivedBogies)
      {
         if (bogie->GetUrgency() > overallUrgency)
         {
            overallUrgency = bogie->GetUrgency();
         }
      }

      for (auto& bandit : mPerceivedDataSummary->mPerceivedBandits)
      {
         if (bandit->GetUrgency() > overallUrgency)
         {
            overallUrgency = bandit->GetUrgency();
         }
      }

      mUrgency = overallUrgency;
   }

   // Ensure limits are imposed
   mUrgency = UtMath::Limit(mUrgency, 0.0, 1.0);
}

double WsfSA_Assess::CalculateThreatLevel(double aSimTime, WsfSA_PerceivedItem* aThreatItem, bool aIsBogie)
{
   if (aThreatItem != nullptr)
   {
      // Ignore any friendly or neutral platforms (regardless of whether they are missiles or aircraft)
      enum WsfSA_EntityPerception::Identification ident = aThreatItem->GetIdentification();
      if ((ident == WsfSA_EntityPerception::FRIENDLY) || (ident == WsfSA_EntityPerception::NEUTRAL))
      {
         // Friendly or neutral entities offer no threat, so use 0.0
         // Set the threat value (this function will ensure a normalized value)
         aThreatItem->SetThreatLevel(0.0);

         // Return the score, that has been normalized
         return aThreatItem->GetThreatLevel();
      }

      // Possibly ignore bogies as threats
      if (mIgnoreBogiesAsThreats)
      {
         if (ident == WsfSA_EntityPerception::BOGIE)
         {
            // We want to ignore bogies as a threat, so use 0.0
            // Set the threat value (this function will ensure a normalized value)
            aThreatItem->SetThreatLevel(0.0);

            // Return the score, that has been normalized
            return aThreatItem->GetThreatLevel();
         }
      }

      // Only consider platforms that are perceived as a missile if we are not ignoring them
      if (aThreatItem->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         auto threatEntity = dynamic_cast<WsfSA_EntityPerception*>(aThreatItem);
         if (threatEntity->GetIsMissile())
         {
            if (mIgnoreMissilesAsThreats)
            {
               // We will ignore this since it is a missile

               // Set the threat value to zero
               threatEntity->SetThreatLevel(0.0);

               // Return the score, that has been normalized
               return threatEntity->GetThreatLevel();
            }
            else
            {
               // If the entity is perceived as a missile, use the missile function instead
               return CalculateMissileThreatLevel(aSimTime, threatEntity, aIsBogie);
            }
         }

         // Determine the threat this platform poses

         // Use the script-based function, if available
         if (mCalculateEntityThreatLevelScriptPtr != nullptr)
         {
            double score = ExecuteCalculateEntityThreatLevel(aSimTime, threatEntity, aIsBogie);

            // Set the threat level (this function will ensure a normalized value)
            threatEntity->SetThreatLevel(score);

            // Return the score, that has been normalized
            return threatEntity->GetThreatLevel();
         }
      }
      else
      {
         auto threatGroup = dynamic_cast<WsfSA_Group*>(aThreatItem);
         // Use the script-based function, if available
         if (mCalculateGroupThreatLevelScriptPtr != nullptr)
         {
            double score = ExecuteCalculateGroupThreatLevel(aSimTime, threatGroup, aIsBogie);

            // Set the threat level (this function will ensure a normalized value)
            threatGroup->SetThreatLevel(score);

            // Return the score, that has been normalized
            return threatGroup->GetThreatLevel();
         }
      }

      double alt_m = 0.0;
      if (aThreatItem->GetAltitudeValid())
      {
         alt_m = aThreatItem->GetAltitude_ft();
      }

      UtVec3dX tgtLocationWCS;
      UtEntity::ConvertLLAToWCS(aThreatItem->GetLat_deg(), aThreatItem->GetLon_deg(), alt_m, tgtLocationWCS.GetData());

      UtVec3dX relativeLocationWCS;
      double   offBoresight_rad = OffBoresightAngleForTarget(tgtLocationWCS.GetData());

      GetPlatform()->GetRelativeLocationWCS(tgtLocationWCS.GetData(), relativeLocationWCS.GetData());

      double slantRange_nm = relativeLocationWCS.Magnitude() * UtMath::cNM_PER_M;

      double rangeScore = (slantRange_nm * slantRange_nm) / 10000.0;
      if (rangeScore > 1.0)
      {
         rangeScore = 1.0;
      }

      double offboresightScore = (offBoresight_rad * offBoresight_rad) / (UtMath::cPI * UtMath::cPI);
      if (offboresightScore > 1.0)
      {
         offboresightScore = 1.0;
      }

      double totalScore = rangeScore * 100.0 + offboresightScore * 200.0;

      // Adjustment for bogie (if applicable)
      if (aIsBogie)
      {
         totalScore *= mBogieThreatScoreMultiplier;
      }

      // Set the threat level (this function will ensure a normalized value)
      aThreatItem->SetThreatLevel(totalScore);

      // Return the score, that has been normalized
      return aThreatItem->GetThreatLevel();
   }

   // We should not arrive here, but if we do, use a value of zero
   return 0.0;
}

double WsfSA_Assess::CalculateTargetValue(double aSimTime, WsfSA_PerceivedItem* aTargetItem, bool aIsBogie)
{
   if (aTargetItem != nullptr)
   {
      // Ignore any friendly or neutral platforms (regardless of whether they are missiles or aircraft)
      enum WsfSA_EntityPerception::Identification ident = aTargetItem->GetIdentification();
      if ((ident == WsfSA_EntityPerception::FRIENDLY) || (ident == WsfSA_EntityPerception::NEUTRAL))
      {
         // Friendly or neutral entities are never valid targets, so use 0.0
         // Set the target value (this function will ensure a normalized value)
         aTargetItem->SetTargetValue(0.0);

         // Return the score, that has been normalized
         return aTargetItem->GetTargetValue();
      }

      // Possibly ignore bogies as targets
      if (mIgnoreBogiesAsTargets)
      {
         if (ident == WsfSA_EntityPerception::BOGIE)
         {
            // We want to ignore bogies as a viable target, so use 0.0
            // Set the threat value (this function will ensure a normalized value)
            aTargetItem->SetTargetValue(0.0);

            // Return the score, that has been normalized
            return aTargetItem->GetTargetValue();
         }
      }

      // Only consider platforms that are perceived as a missile if we are not ignoring them
      if (aTargetItem->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         auto targetEntity = dynamic_cast<WsfSA_EntityPerception*>(aTargetItem);
         if (targetEntity->GetIsMissile())
         {
            if (mIgnoreMissilesAsTargets)
            {
               // We will ignore this since it is a missile

               // Set the target value to zero
               targetEntity->SetTargetValue(0.0);

               // Return the score, that has been normalized
               return targetEntity->GetTargetValue();
            }
            else
            {
               // If the entity is perceived as a missile, use the missile function instead
               return CalculateMissileTargetValue(aSimTime, targetEntity, aIsBogie);
            }
         }


         // Determine the target value this platform presents

         // Use the script-based function, if available
         if (mCalculateEntityTargetValueScriptPtr != nullptr)
         {
            double score = ExecuteCalculateEntityTargetValue(aSimTime, targetEntity, aIsBogie);

            // Set the target value (this function will ensure a normalized value)
            targetEntity->SetTargetValue(score);

            // Return the score, that has been normalized
            return targetEntity->GetTargetValue();
         }
      }
      else
      {
         auto targetGroup = dynamic_cast<WsfSA_Group*>(aTargetItem);
         if (mCalculateGroupTargetValueScriptPtr != nullptr)
         {
            double score = ExecuteCalculateGroupTargetValue(aSimTime, targetGroup, aIsBogie);

            // Set the target value (this function will ensure a normalized value)
            targetGroup->SetTargetValue(score);

            // Return the score, that has been normalized
            return targetGroup->GetTargetValue();
         }
      }

      double alt_m = 0.0;
      if (aTargetItem->GetAltitudeValid())
      {
         alt_m = aTargetItem->GetAltitude_ft();
      }

      UtVec3dX tgtLocationWCS;
      UtEntity::ConvertLLAToWCS(aTargetItem->GetLat_deg(), aTargetItem->GetLon_deg(), alt_m, tgtLocationWCS.GetData());

      UtVec3dX relativeLocationWCS;
      double   offBoresight_rad = OffBoresightAngleForTarget(tgtLocationWCS.GetData());

      GetPlatform()->GetRelativeLocationWCS(tgtLocationWCS.GetData(), relativeLocationWCS.GetData());

      double slantRange_nm = relativeLocationWCS.Magnitude() * UtMath::cNM_PER_M;

      double rangeScore = 1.0 - ((slantRange_nm * slantRange_nm) / 100000.0);
      if (rangeScore > 1.00)
      {
         rangeScore = 1.00;
      }
      if (rangeScore < 0.01)
      {
         rangeScore = 0.01;
      }

      double offboresightScore = 1.0 - ((offBoresight_rad * offBoresight_rad) / (UtMath::cPI * UtMath::cPI));
      if (offboresightScore > 1.00)
      {
         offboresightScore = 1.00;
      }
      if (offboresightScore < 0.01)
      {
         offboresightScore = 0.01;
      }

      double totalScore = rangeScore * 100.0 + offboresightScore * 200.0;

      // Adjustment for bogie (if applicable)
      if (aIsBogie)
      {
         totalScore *= mBogieTargetScoreMultiplier;
      }

      // Set the target value (this function will ensure a normalized value)
      aTargetItem->SetTargetValue(totalScore);

      // Return the score, that has been normalized
      return aTargetItem->GetTargetValue();
   }

   // We should not arrive here, but if we do, use a value of zero
   return 0.0;
}

double WsfSA_Assess::CalculateMissileThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatEntity, bool aIsBogie)
{
   if (aThreatEntity != nullptr)
   {
      // Prior to calling this in CalculateThreatLevel(), we have ignored any friendly or neutral platforms
      // (regardless of whether they are missiles or aircraft), and may have also ignored bogies as threats.

      // Verify that this is a missile, if not, return zero
      if (!aThreatEntity->GetIsMissile())
      {
         // Set the threat level to zero
         aThreatEntity->SetThreatLevel(0.0);

         // Return the score, that has been normalized
         return aThreatEntity->GetThreatLevel();
      }

      // Determine the threat this platform poses

      // Use the missile-variant script-based function, if available
      if (mCalculateMissileThreatLevelScriptPtr != nullptr)
      {
         double score = ExecuteCalculateMissileThreatLevel(aSimTime, aThreatEntity, aIsBogie);

         // Set the threat level (this function will ensure a normalized value)
         aThreatEntity->SetThreatLevel(score);

         // Return the score, that has been normalized
         return aThreatEntity->GetThreatLevel();
      }

      double alt_m = 0.0;
      if (aThreatEntity->GetAltitudeValid())
      {
         alt_m = aThreatEntity->GetAltitude_ft();
      }

      UtVec3dX tgtLocationWCS;
      UtEntity::ConvertLLAToWCS(aThreatEntity->GetLat_deg(), aThreatEntity->GetLon_deg(), alt_m, tgtLocationWCS.GetData());

      UtVec3dX relativeLocationWCS;
      double   offBoresight_rad = OffBoresightAngleForTarget(tgtLocationWCS.GetData());

      GetPlatform()->GetRelativeLocationWCS(tgtLocationWCS.GetData(), relativeLocationWCS.GetData());

      double slantRange_nm = relativeLocationWCS.Magnitude() * UtMath::cNM_PER_M;

      double rangeScore = (slantRange_nm * slantRange_nm) / 10000.0;
      if (rangeScore > 1.0)
      {
         rangeScore = 1.0;
      }

      double offboresightScore = (offBoresight_rad * offBoresight_rad) / (UtMath::cPI * UtMath::cPI);
      if (offboresightScore > 1.0)
      {
         offboresightScore = 1.0;
      }

      double totalScore = rangeScore * 100.0 + offboresightScore * 200.0;

      // Adjustment for bogie (if applicable)
      if (aIsBogie)
      {
         totalScore *= mBogieThreatScoreMultiplier;
      }

      // Set the threat level (this function will ensure a normalized value)
      aThreatEntity->SetThreatLevel(totalScore);

      // Return the score, that has been normalized
      return aThreatEntity->GetThreatLevel();
   }

   // We should not arrive here, but if we do, use a value of zero
   return 0.0;
}

double WsfSA_Assess::CalculateMissileTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetEntity, bool aIsBogie)
{
   if (aTargetEntity != nullptr)
   {
      // Prior to calling this in CalculateThreatLevel(), we have ignored any friendly or neutral platforms
      // (regardless of whether they are missiles or aircraft), and may have also ignored bogies as threats.

      // Verify that this is a missile, if not, return zero
      if (!aTargetEntity->GetIsMissile())
      {
         // Set the target value to zero
         aTargetEntity->SetTargetValue(0.0);

         // Return the score, that has been normalized
         return aTargetEntity->GetTargetValue();
      }

      // Determine the target value this platform poses

      // Use the missile-variant script-based function, if available
      if (mCalculateMissileTargetValueScriptPtr != nullptr)
      {
         double score = ExecuteCalculateMissileTargetValue(aSimTime, aTargetEntity, aIsBogie);

         // Set the target value (this function will ensure a normalized value)
         aTargetEntity->SetTargetValue(score);

         // Return the score, that has been normalized
         return aTargetEntity->GetTargetValue();
      }

      double alt_m = 0.0;
      if (aTargetEntity->GetAltitudeValid())
      {
         alt_m = aTargetEntity->GetAltitude_ft();
      }

      UtVec3dX tgtLocationWCS;
      UtEntity::ConvertLLAToWCS(aTargetEntity->GetLat_deg(), aTargetEntity->GetLon_deg(), alt_m, tgtLocationWCS.GetData());

      UtVec3dX relativeLocationWCS;
      double   offBoresight_rad = OffBoresightAngleForTarget(tgtLocationWCS.GetData());

      GetPlatform()->GetRelativeLocationWCS(tgtLocationWCS.GetData(), relativeLocationWCS.GetData());

      double slantRange_nm = relativeLocationWCS.Magnitude() * UtMath::cNM_PER_M;

      double rangeScore = 1.0 - ((slantRange_nm * slantRange_nm) / 100000.0);
      if (rangeScore > 1.00)
      {
         rangeScore = 1.00;
      }
      if (rangeScore < 0.01)
      {
         rangeScore = 0.01;
      }

      double offboresightScore = 1.0 - ((offBoresight_rad * offBoresight_rad) / (UtMath::cPI * UtMath::cPI));
      if (offboresightScore > 1.00)
      {
         offboresightScore = 1.00;
      }
      if (offboresightScore < 0.01)
      {
         offboresightScore = 0.01;
      }

      double totalScore = rangeScore * 100.0 + offboresightScore * 200.0;

      // Adjustment for bogie (if applicable)
      if (aIsBogie)
      {
         totalScore *= mBogieTargetScoreMultiplier;
      }

      // Set the target value (this function will ensure a normalized value)
      aTargetEntity->SetTargetValue(totalScore);

      // Return the score, that has been normalized
      return aTargetEntity->GetTargetValue();
   }

   // We should not arrive here, but if we do, use a value of zero
   return 0.0;
}

double WsfSA_Assess::CalculateRisk(double aSimTime, WsfSA_EntityPerception* aThreatEntity)
{
   // Use the script-based function, if available
   if (mCalculateRiskPosedByEntityScriptPtr != nullptr)
   {
      double score = -1.0;

      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatEntity;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateRiskPosedByEntityScriptPtr, scriptRetVal, scriptArgs);
         score = scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::CalculateRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }

      // Limit score to +/- 1.0
      score = UtMath::Limit(score, 1.0);

      // Set the risk (this function will ensure a normalized value)
      aThreatEntity->SetRisk(score);

      // Return the risk, that has been normalized
      return aThreatEntity->GetRisk();
   }

   // TODO -- Provide a default calculation of risk
   // Set the risk (this function will ensure a normalized value)
   aThreatEntity->SetRisk(0.0);

   // Return the risk, that has been normalized
   return aThreatEntity->GetRisk();
}

double WsfSA_Assess::CalculateDefensiveness(double aSimTime, WsfSA_EntityPerception* aThreatEntity)
{
   // Use the script-based function, if available
   if (mCalculateDefensivenessInducedByEntityScriptPtr != nullptr)
   {
      double score = -1.0;

      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatEntity;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateDefensivenessInducedByEntityScriptPtr, scriptRetVal, scriptArgs);
         score = scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::CalculateDefensiveness.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }

      // Limit score to +/- 1.0
      score = UtMath::Limit(score, 1.0);

      // Set the defensiveness (this function will ensure a normalized value)
      aThreatEntity->SetDefensiveness(score);

      // Return the defensiveness, that has been normalized
      return aThreatEntity->GetDefensiveness();
   }

   // TODO -- Provide a default calculation of defensiveness
   // Set the defensiveness (this function will ensure a normalized value)
   aThreatEntity->SetDefensiveness(0.0);

   // Return the defensiveness, that has been normalized
   return aThreatEntity->GetDefensiveness();
}

double WsfSA_Assess::CalculateUrgency(double aSimTime, WsfSA_EntityPerception* aThreatEntity)
{
   // Use the script-based function, if available
   if (mCalculateUrgencyInducedByEntityScriptPtr != nullptr)
   {
      double score = -1.0;

      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatEntity;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateUrgencyInducedByEntityScriptPtr, scriptRetVal, scriptArgs);
         score = scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::CalculateUrgency.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }

      // Limit score to +/- 1.0
      score = UtMath::Limit(score, 1.0);

      // Set the urgency (this function will ensure a normalized value)
      aThreatEntity->SetUrgency(score);

      // Return the urgency, that has been normalized
      return aThreatEntity->GetUrgency();
   }

   // TODO -- Provide a default calculation of urgency
   // Set the urgency (this function will ensure a normalized value)
   aThreatEntity->SetUrgency(0.0);

   // Return the urgency, that has been normalized
   return aThreatEntity->GetUrgency();
}

bool WsfSA_Assess::CompareScore(const PlatormScorePairing& first, const PlatormScorePairing& second)
{
   if (first.score > second.score)
   {
      return true;
   }

   return false;
}

double WsfSA_Assess::ExecuteCalculateRisk(double aSimTime)
{
   if (mCalculateRiskScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal(0.0);
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateRiskScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateSelfRisk(double aSimTime)
{
   if (mCalculateSelfRiskScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateSelfRiskScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateSelfRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateFlightRisk(double aSimTime)
{
   if (mCalculateFlightRiskScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateFlightRiskScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateFlightRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculatePackageRisk(double aSimTime)
{
   if (mCalculatePackageRiskScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculatePackageRiskScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculatePackageRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateMissionRisk(double aSimTime)
{
   if (mCalculateMissionRiskScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateMissionRiskScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateMissionRisk.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateDefensiveness(double aSimTime)
{
   if (mCalculateDefensivenessScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateDefensivenessScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateDefensiveness.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateUrgency(double aSimTime)
{
   if (mCalculateUrgencyScriptPtr != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateUrgencyScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateUrgency.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateEntityThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatEntity, bool aIsBogie)
{
   if (mCalculateEntityThreatLevelScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatEntity << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateEntityThreatLevelScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateThreatLevel.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateEntityTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetEntity, bool aIsBogie)
{
   if (mCalculateEntityTargetValueScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aTargetEntity << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateEntityTargetValueScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateEntityTargetValue.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateMissileThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatEntity, bool aIsBogie)
{
   if (mCalculateMissileThreatLevelScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatEntity << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateMissileThreatLevelScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateMissileThreatLevel.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateMissileTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetEntity, bool aIsBogie)
{
   if (mCalculateMissileTargetValueScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aTargetEntity << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateMissileTargetValueScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalculateMissileTargetValue.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateGroupThreatLevel(double aSimTime, WsfSA_Group* aThreatGroup, bool aIsBogie)
{
   if (mCalculateGroupThreatLevelScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aThreatGroup << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateGroupThreatLevelScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Assess::ExecuteCalculateGroupThreatLevel on platform: "
                          << GetPlatform()->GetName() << ".";
         return 0.0;
      }
   }

   // Return zero if no function is available
   return 0.0;
}

double WsfSA_Assess::ExecuteCalculateGroupTargetValue(double aSimTime, WsfSA_Group* aTargetGroup, bool aIsBogie)
{
   if (mCalculateGroupTargetValueScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aTargetGroup << aIsBogie;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateGroupTargetValueScriptPtr, scriptRetVal, scriptArgs);
         return scriptRetVal.GetDouble();
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Assess::ExecuteCalculateGroupTargetValue on platform: "
                          << GetPlatform()->GetName() << ".";
         return 0.0;
      }
   }

   // Return zero if no function is available
   return 0.0;
}

bool WsfSA_Assess::ExecuteCalcWeaponSupport(double aSimTime)
{
   if (mCalculateWeaponSupport != nullptr)
   {
      try
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         GetScriptContext().ExecuteScript(aSimTime, mCalculateWeaponSupport, scriptRetVal, scriptArgs);
         return scriptRetVal.GetBool();
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Assess::ExecuteCalcWeaponSupport.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   // Return false if no function is available
   return false;
}

bool WsfSA_Assess::SimpleWezCalculationAircraft(const WsfPlatform&      aTargetPlatform,
                                                MissileRangeParameters& aRmax,
                                                MissileRangeParameters& aRne,
                                                MissileRangeParameters& aRmin,
                                                double&                 aCurrentRange,
                                                bool&                   aEngageable) const
{
   WsfPlatform* aircraftPlatform = GetPlatform();

   return SimpleWezCalculationAircraft(*aircraftPlatform, aTargetPlatform, aRmax, aRne, aRmin, aCurrentRange, aEngageable);
}

bool WsfSA_Assess::SimpleWezCalculationAircraft(const WsfPlatform&      aAircraftPlatform,
                                                const WsfPlatform&      aTargetPlatform,
                                                MissileRangeParameters& aRmax,
                                                MissileRangeParameters& aRne,
                                                MissileRangeParameters& aRmin,
                                                double&                 aCurrentRange,
                                                bool&                   aEngageable) const
{
   // This is used below to indicate which type of missile wez should be used
   const MissileWezParameters* missileParameters = nullptr;

   if (IsMediumRangeMissile(mSelectedWeapon))
   {
      std::string side = aAircraftPlatform.GetSide();
      UtStringUtil::ToUpper(side);
      if (side == "RED")
      {
         missileParameters = &mRedMissileParametersMRM;
      }
      else
      {
         missileParameters = &mBlueMissileParametersMRM;
      }

      // Perform a simple launch computer lookup
      if (LaunchComputerMissileWezLookup(aAircraftPlatform,
                                         aTargetPlatform,
                                         *missileParameters,
                                         aRmax,
                                         aRne,
                                         aRmin,
                                         aCurrentRange,
                                         aEngageable))
      {
         return true;
      }

      // The lookup failed so calculate the missile WEZ
      MissileRangeParameters mrmRmax;
      MissileRangeParameters mrmRne;
      MissileRangeParameters mrmRmin;
      double                 mrmCurrentRange = 0.0;
      bool                   mrmEngageable   = false;

      bool resultMRM = SimpleMissileWezCalculation(aAircraftPlatform,
                                                   aTargetPlatform,
                                                   *missileParameters,
                                                   mrmRmax,
                                                   mrmRne,
                                                   mrmRmin,
                                                   mrmCurrentRange,
                                                   mrmEngageable);

      if (resultMRM)
      {
         aRmax.range   = mrmRmax.range;
         aRmax.time    = mrmRmax.time;
         aRmax.pk      = mrmRmax.pk;
         aRne.range    = mrmRne.range;
         aRne.time     = mrmRne.time;
         aRne.pk       = mrmRne.pk;
         aRmin.range   = mrmRmin.range;
         aRmin.time    = mrmRmin.time;
         aRmin.pk      = mrmRmin.pk;
         aCurrentRange = mrmCurrentRange;
         aEngageable   = mrmEngageable;
         return true;
      }

      return false;
   }
   else if (IsShortRangeMissile(mSelectedWeapon))
   {
      std::string side = aAircraftPlatform.GetSide();
      UtStringUtil::ToUpper(side);
      if (side == "RED")
      {
         missileParameters = &mRedMissileParametersSRM;
      }
      else
      {
         missileParameters = &mBlueMissileParametersSRM;
      }

      // Perform a simple launch computer lookup
      if (LaunchComputerMissileWezLookup(aAircraftPlatform,
                                         aTargetPlatform,
                                         *missileParameters,
                                         aRmax,
                                         aRne,
                                         aRmin,
                                         aCurrentRange,
                                         aEngageable))
      {
         return true;
      }

      // The lookup failed so calculate the missile WEZ
      MissileRangeParameters srmRmax;
      MissileRangeParameters srmRne;
      MissileRangeParameters srmRmin;
      double                 srmCurrentRange = 0.0;
      bool                   srmEngageable   = false;

      bool resultSRM = SimpleMissileWezCalculation(aAircraftPlatform,
                                                   aTargetPlatform,
                                                   *missileParameters,
                                                   srmRmax,
                                                   srmRne,
                                                   srmRmin,
                                                   srmCurrentRange,
                                                   srmEngageable);

      if (resultSRM)
      {
         // Only the SRM may be used
         aRmax.range   = srmRmax.range;
         aRmax.time    = srmRmax.time;
         aRmax.pk      = srmRmax.pk;
         aRne.range    = srmRne.range;
         aRne.time     = srmRne.time;
         aRne.pk       = srmRne.pk;
         aRmin.range   = srmRmin.range;
         aRmin.time    = srmRmin.time;
         aRmin.pk      = srmRmin.pk;
         aCurrentRange = srmCurrentRange;
         aEngageable   = srmEngageable;
         return true;
      }

      return false;
   }
   else
   {
      std::string side = aAircraftPlatform.GetSide();
      UtStringUtil::ToUpper(side);

      // Test using the MRM
      MissileRangeParameters mrmRmax;
      MissileRangeParameters mrmRne;
      MissileRangeParameters mrmRmin;
      double                 mrmCurrentRange = 0.0;
      bool                   mrmEngageable   = false;

      if (side == "RED")
      {
         missileParameters = &mRedMissileParametersMRM;
      }
      else
      {
         missileParameters = &mBlueMissileParametersMRM;
      }

      // Perform a simple launch computer lookup
      bool resultMRM = LaunchComputerMissileWezLookup(aAircraftPlatform,
                                                      aTargetPlatform,
                                                      *missileParameters,
                                                      mrmRmax,
                                                      mrmRne,
                                                      mrmRmin,
                                                      mrmCurrentRange,
                                                      mrmEngageable);

      if (!resultMRM)
      {
         // The lookup failed so calculate the missile WEZ
         resultMRM = SimpleMissileWezCalculation(aAircraftPlatform,
                                                 aTargetPlatform,
                                                 *missileParameters,
                                                 mrmRmax,
                                                 mrmRne,
                                                 mrmRmin,
                                                 mrmCurrentRange,
                                                 mrmEngageable);
      }

      // Test using the SRM
      MissileRangeParameters srmRmax;
      MissileRangeParameters srmRne;
      MissileRangeParameters srmRmin;
      double                 srmCurrentRange = 0.0;
      bool                   srmEngageable   = false;

      if (side == "RED")
      {
         missileParameters = &mRedMissileParametersSRM;
      }
      else
      {
         missileParameters = &mBlueMissileParametersSRM;
      }

      // Perform a simple launch computer lookup
      bool resultSRM = LaunchComputerMissileWezLookup(aAircraftPlatform,
                                                      aTargetPlatform,
                                                      *missileParameters,
                                                      srmRmax,
                                                      srmRne,
                                                      srmRmin,
                                                      srmCurrentRange,
                                                      srmEngageable);

      if (!resultSRM)
      {
         // The lookup failed so calculate the missile WEZ
         resultSRM = SimpleMissileWezCalculation(aAircraftPlatform,
                                                 aTargetPlatform,
                                                 *missileParameters,
                                                 srmRmax,
                                                 srmRne,
                                                 srmRmin,
                                                 srmCurrentRange,
                                                 srmEngageable);
      }

      bool useMRM = false;
      bool useSRM = false;

      // Now, we want to find the appropriate weapon WEZ
      if (resultSRM)
      {
         // Check the SRM -- if within wez, this will be used (favor SRM over MRM)
         if ((srmCurrentRange >= srmRmin.range) && (srmCurrentRange <= srmRmax.range))
         {
            useSRM = true;
         }
         else if (resultMRM)
         {
            // Check the MRM -- if within wez, this will be used
            if ((mrmCurrentRange >= mrmRmin.range) && (mrmCurrentRange <= mrmRmax.range))
            {
               useMRM = true;
            }
            else if ((mrmRmax.range > srmRmax.range) && (mrmCurrentRange >= mrmRmin.range))
            {
               // Use the MRM if it has greater range than SRM and if greater than min range
               useMRM = true;
            }
            else if (srmCurrentRange >= srmRmin.range) // Use SRM if greater than min range
            {
               useSRM = true;
            }
            else
            {
               // Too close -- but use the shorter ranged weapon
               if (srmRmin.range <= mrmRmin.range)
               {
                  useSRM = true;
               }
               else
               {
                  useMRM = true;
               }
            }
         }
         else
         {
            useSRM = true;
         }
      }
      else if (resultMRM)
      {
         useMRM = true;
      }
      else
      {
         // Neither missile may be used
         return false;
      }

      if (useMRM)
      {
         // Use the MRM
         aRmax.range   = mrmRmax.range;
         aRmax.time    = mrmRmax.time;
         aRmax.pk      = mrmRmax.pk;
         aRne.range    = mrmRne.range;
         aRne.time     = mrmRne.time;
         aRne.pk       = mrmRne.pk;
         aRmin.range   = mrmRmin.range;
         aRmin.time    = mrmRmin.time;
         aRmin.pk      = mrmRmin.pk;
         aCurrentRange = mrmCurrentRange;
         aEngageable   = mrmEngageable;
         return true;
      }
      else if (useSRM)
      {
         // Use the SRM
         aRmax.range   = srmRmax.range;
         aRmax.time    = srmRmax.time;
         aRmax.pk      = srmRmax.pk;
         aRne.range    = srmRne.range;
         aRne.time     = srmRne.time;
         aRne.pk       = srmRne.pk;
         aRmin.range   = srmRmin.range;
         aRmin.time    = srmRmin.time;
         aRmin.pk      = srmRmin.pk;
         aCurrentRange = srmCurrentRange;
         aEngageable   = srmEngageable;
         return true;
      }
   }

   return false;
}

bool WsfSA_Assess::SimpleMissileWezCalculation(const WsfPlatform&          aTargetPlatform,
                                               const MissileWezParameters& aMissileParameters,
                                               MissileRangeParameters&     aRmax,
                                               MissileRangeParameters&     aRne,
                                               MissileRangeParameters&     aRmin,
                                               double&                     aCurrentRange,
                                               bool&                       aEngageable) const
{
   WsfPlatform* ownPlatform = GetPlatform();

   return SimpleMissileWezCalculation(*ownPlatform, aTargetPlatform, aMissileParameters, aRmax, aRne, aRmin, aCurrentRange, aEngageable);
}

bool WsfSA_Assess::SimpleMissileWezCalculation(const WsfPlatform&          aAircraftPlatform,
                                               const WsfPlatform&          aTargetPlatform,
                                               const MissileWezParameters& aMissileParameters,
                                               MissileRangeParameters&     aRmax,
                                               MissileRangeParameters&     aRne,
                                               MissileRangeParameters&     aRmin,
                                               double&                     aCurrentRange,
                                               bool&                       aEngageable) const
{
   // Calc "angles" in radians
   double angleOff  = fabs(AngleOffTarget3D(aAircraftPlatform, aTargetPlatform));
   double noseAngle = OffBoresightAngleForTarget(aTargetPlatform);

   if (noseAngle > aMissileParameters.maxOffBoresightAngle)
   {
      // Outside of boresight-limited envelope -- cannot engage
      return false;
   }

   // Calculate max range
   double maxRange = aMissileParameters.avgSpeed * aMissileParameters.maxTimeFlight;

   // Assume range is reduced according to off-boresight (nose angle) as follows
   double offboresightRangeMultiplier = 1.0;
   if (noseAngle < UtMath::cPI_OVER_2)
   {
      offboresightRangeMultiplier = 1.0 - 0.5 * sin(noseAngle);
   }
   else
   {
      offboresightRangeMultiplier = 0.5 - 0.3 * sin(noseAngle - UtMath::cPI_OVER_2);
   }

   // Calc the range due to off-boresight
   double offboresightRange = maxRange * offboresightRangeMultiplier;

   // Calc the new time of flight (TOF)
   double newTOF = offboresightRange / aMissileParameters.avgSpeed;

   // Get the speed of the target
   double tgtSpeed = aTargetPlatform.GetSpeed();

   // Calc the effect of the target motion -- a positive number
   // will reduce range, since the distance is increasing as
   // the target recedes
   double tgtRangeEffect = tgtSpeed * newTOF * cos(angleOff);

   // Calc the maximum "effective" range
   double effectiveRange = offboresightRange - tgtRangeEffect;

   // We have calculated the Rmax data
   aRmax.range = effectiveRange;
   aRmax.time  = newTOF;
   aRmax.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, angleOff);

   // For Rne, we begin by examining how long it takes to turn and run. Assuming target speed of roughly Mach 1
   // (~1000 ft/sec, ~300 m/sec) and assuming a 6g turn, we have a = V^2/r. Thus, the radius is r = V^2/a.
   // With these parameters, r = (300*300)/(9.81*6) = 90000/58.86 = 1529 meters. The distance covered in the
   // turn to reverse course is d = Pi*r = 4,803 m. This requires 4803/V -> 4803/300 = 16.01 sec. Thus, to turn
   // and run (facing away) generally takes 16 seconds. Note that, at the end of the 16 seconds, the target is
   // no closer/farther than it was at the start. Also note that, at the end of 8 seconds, the target is a radius
   // (1529m) closer than at the start.

   double tgtRangeDecrease  = 0.0;
   double effectiveAngleOff = angleOff;
   if (newTOF < 8.0) // Target is still turning half way...
   {
      tgtRangeDecrease  = -1529.0 * (newTOF / 8.0);
      effectiveAngleOff = UtMath::cPI_OVER_2 * (newTOF / 8.0);
   }
   else if (newTOF < 16.0) // Target still making a 180deg turn...
   {
      tgtRangeDecrease  = -1529.0 + (1529.0 * ((newTOF - 8.0) / 8.0));
      effectiveAngleOff = UtMath::cPI_OVER_2 * (1.0 - ((newTOF - 8.0) / 8.0));
   }
   else // Target has turned around and can now run away
   {
      tgtRangeDecrease  = tgtSpeed * (newTOF - 16.0);
      effectiveAngleOff = 0.0;
   }

   // Calc the maximum "effective" range for Rne
   effectiveRange = offboresightRange - tgtRangeDecrease;

   // We have calculated the Rne data
   aRne.range = effectiveRange;
   aRne.time  = newTOF;
   aRne.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, effectiveAngleOff);

   // For Rmin, simply check for the minimum range. Start with min time.
   newTOF          = aMissileParameters.minTimeFlight;
   double minRange = aMissileParameters.avgSpeed * newTOF;

   // Calc the effect of the target motion
   tgtRangeDecrease = tgtSpeed * newTOF * cos(angleOff);

   // Calc the maximum "effective" range
   double effectiveMinRange = minRange - tgtRangeDecrease;

   // We have calculated the Rmin data
   aRmin.range = effectiveMinRange;
   aRmin.time  = newTOF;
   aRmin.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, effectiveAngleOff);

   // Confirm that Rmin < Rne < Rmax
   bool wezValid = true;
   if (aRne.range < aRmin.range)
   {
      // Rne cannot be lass than Rmin
      aRne = aRmin;
   }
   if (aRne.range > aRmax.range)
   {
      // Rne cannot be more than Rmax
      aRne = aRmax;
   }
   if (aRmin.range >= aRmax.range)
   {
      // Rmin *must* be less than Rmax
      // Since it is not, we do not have a solution
      aRmin    = aRmax;
      wezValid = false;
   }

   // Calc range data
   double slant_range = CalcSlantRange(aAircraftPlatform, aTargetPlatform);

   // Is this within range, and hence engageable?
   aEngageable = false;
   if ((slant_range >= aRmin.range) && (slant_range <= aRmax.range) && wezValid)
   {
      aEngageable = true;
   }

   // Set return parameters
   aCurrentRange = slant_range;
   aEngageable   = true; // TODO - Examine continued use of aEngageable flag (is it worthwhile?)

   return true;
}

bool WsfSA_Assess::LaunchComputerMissileWezLookup(const WsfPlatform&          aAircraftPlatform,
                                                  const WsfPlatform&          aTargetPlatform,
                                                  const MissileWezParameters& aMissileParameters,
                                                  MissileRangeParameters&     aRmax,
                                                  MissileRangeParameters&     aRne,
                                                  MissileRangeParameters&     aRmin,
                                                  double&                     aCurrentRange,
                                                  bool&                       aEngageable) const
{
   const WsfWeapon* weaponPtr = mSA_ProcessorPtr->GetPlatform()->GetComponent<WsfWeapon>(mSelectedWeapon);
   if (weaponPtr)
   {
      WsfAirToAirLaunchComputer* computerPtr = dynamic_cast<WsfAirToAirLaunchComputer*>(weaponPtr->GetLaunchComputer());
      if (computerPtr)
      {
         // Create a track with target platform data for use in the LookupResult method
         // Location and velocity must be valid
         WsfTrack track;

         double locWCS[3] = {0.0};
         aTargetPlatform.GetLocationWCS(locWCS);
         track.SetLocationWCS(locWCS);

         double velWCS[3] = {0.0};
         aTargetPlatform.GetVelocityWCS(velWCS);
         track.SetVelocityWCS(velWCS);

         // Set default return values
         double Rmax(-1.0);
         double RmaxTOF(-1.0);
         double Rne(-1.0);
         double RneTOF(-1.0);
         double Rmin(-1.0);
         double RminTOF(-1.0);

         if (computerPtr->LookupResult(track, Rmax, RmaxTOF, Rne, RneTOF, Rmin, RminTOF))
         {
            double angleOff = fabs(AngleOffTarget3D(aAircraftPlatform, aTargetPlatform));

            // Rmax
            aRmax.range = Rmax;
            aRmax.time  = RmaxTOF;
            aRmax.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, angleOff);

            double effectiveAngleOff = angleOff;
            if (RmaxTOF < 8.0) // Target is still turning half way...
            {
               effectiveAngleOff = UtMath::cPI_OVER_2 * (RmaxTOF / 8.0);
            }
            else if (RmaxTOF < 16.0) // Target still making a 180deg turn...
            {
               effectiveAngleOff = UtMath::cPI_OVER_2 * (1.0 - ((RmaxTOF - 8.0) / 8.0));
            }
            else // Target has turned around and can now run away
            {
               effectiveAngleOff = 0.0;
            }

            // Rne
            aRne.range = Rne;
            aRne.time  = RneTOF;
            aRne.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, effectiveAngleOff);

            // Rmin
            aRmin.range = Rmin;
            aRmin.time  = RminTOF;
            aRmin.pk    = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, effectiveAngleOff);

            aCurrentRange = CalcSlantRange(aAircraftPlatform, aTargetPlatform);

            aEngageable = true;

            return true;
         }
      }
   }
   return false;
}

bool WsfSA_Assess::SimpleMissileWezCalculationInflight(const WsfPlatform&          aMissilePlatform,
                                                       WsfPlatform&                aTargetPlatform,
                                                       const MissileWezParameters& aMissileParameters,
                                                       double&                     aRemainingRange,
                                                       double&                     aRemainingTime,
                                                       double&                     aPk) const
{
   // Calc "angles" in radians
   // double aspect = AspectAngleForTarget3D(aMissilePlatform, aTargetPlatform);
   double angleOff  = fabs(AngleOffTarget3D(aMissilePlatform, aTargetPlatform));
   double noseAngle = OffBoresightAngleForTarget(aMissilePlatform, aTargetPlatform);

   if (noseAngle > aMissileParameters.maxOffBoresightAngle)
   {
      // Outside of boresight-limited envelope -- cannot engage
      return false;
   }

   // Calculate max range
   double maxRange = aMissileParameters.avgSpeed * aMissileParameters.maxTimeFlight;

   // Assume range is reduced according to off-boresight (nose angle) as follows
   double offboresightRangeMultiplier = 1.0;
   if (noseAngle < UtMath::cPI_OVER_2)
   {
      offboresightRangeMultiplier = 1.0 - 0.5 * sin(noseAngle);
   }
   else
   {
      offboresightRangeMultiplier = 0.5 - 0.3 * sin(noseAngle - UtMath::cPI_OVER_2);
   }

   // Calc the range due to off-boresight
   double offboresightRange = maxRange * offboresightRangeMultiplier;

   // Calc the new time of flight (TOF)
   double newTOF = offboresightRange / aMissileParameters.avgSpeed;

   // Get the speed of the target
   double tgtSpeed = aTargetPlatform.GetSpeed();

   // Calc the effect of the target motion -- a positive number will reduce range, since the distance is
   // increasing as the target recedes
   double tgtRangeEffect = tgtSpeed * newTOF * cos(angleOff);

   // Calc the "effective" range
   double effectiveRange = offboresightRange - tgtRangeEffect;

   // Calc range data
   double slant_range = CalcSlantRange(aTargetPlatform, aMissilePlatform);

   // Is this within range?
   bool withinRange = false;
   if (slant_range <= effectiveRange)
   {
      withinRange = true;
   }

   // Not in range, then return false
   if (!withinRange)
   {
      return false;
   }

   // Set return parameters
   aRemainingRange = effectiveRange;
   aRemainingTime  = newTOF;
   aPk             = SimpleCalcPkBasedOnAngleOff(aMissileParameters.pkData, angleOff);

   return true;
}

double WsfSA_Assess::SimpleCalcPkBasedOnAngleOff(const MissilePkParameters& aParameters, double aAngleOff) const
{
   double pk = aParameters.nominalPk *                                             // Base Pk
               (1.0 - (aParameters.headOnReduction * (aAngleOff / UtMath::cPI))) * // Head-on effects
               (1.0 - (aParameters.broadsideReduction * sin(aAngleOff)));          // Broadside effects

   // Return pk if in valid range
   if ((pk <= 1.0) && (pk >= 0.0))
   {
      return pk;
   }

   return 0.0;
}

bool WsfSA_Assess::IsMediumRangeMissile(const std::string& aName) const
{
   std::string upperCaseName = aName;
   UtStringUtil::ToUpper(upperCaseName);

   if (upperCaseName.find("MRM") != std::string::npos)
   {
      return true;
   }

   if (upperCaseName.find("FOX1") != std::string::npos)
   {
      return true;
   }

   if (upperCaseName.find("FOX3") != std::string::npos)
   {
      return true;
   }

   return false;
}

bool WsfSA_Assess::IsShortRangeMissile(const std::string& aName) const
{
   std::string upperCaseName = aName;
   UtStringUtil::ToUpper(upperCaseName);

   if (upperCaseName.find("SRM") != std::string::npos)
   {
      return true;
   }

   if (upperCaseName.find("SRM-TVC") != std::string::npos)
   {
      return true;
   }

   if (upperCaseName.find("SVM") != std::string::npos)
   {
      return true;
   }

   if (upperCaseName.find("FOX2") != std::string::npos)
   {
      return true;
   }

   return false;
}

bool WsfSA_Assess::IsCurrentlyBeingTracked(size_t aPlatformId) const
{
   for (auto& id : mCurrentlyTrackedPlatformIds)
   {
      if (aPlatformId == id)
      {
         return true;
      }
   }

   return false;
}

void WsfSA_Assess::UpdateGroupFocusLists()
{
   mFocusedGroups.clear();
   mUnfocusedGroups.clear();

   for (auto group : mDataPtr->mCurrentlyPerceivedGroups)
   {
      if (group->GetFocus())
      {
         mFocusedGroups.push_back(group);
      }
      else
      {
         mUnfocusedGroups.push_back(group);
      }
   }
}

void WsfSA_Assess::SortedGroups(int aSortingEnum, std::vector<WsfSA_Group*>& aGroups) const
{
   WsfSA_GroupUtils::SortedGroups(aSortingEnum, aGroups, mDataPtr->mCurrentlyPerceivedGroups, GetPlatform());
}

WsfSA_Group* WsfSA_Assess::GetGroup(const std::string& aName) const
{
   for (auto& group : GetSAP()->GetData()->mCurrentlyPerceivedGroups)
   {
      if (group->GetPerceivedName() == aName)
      {
         return group;
      }
   }

   return nullptr;
}
