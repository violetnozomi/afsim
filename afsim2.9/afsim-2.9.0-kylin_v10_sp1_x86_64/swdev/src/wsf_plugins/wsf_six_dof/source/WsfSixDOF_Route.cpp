// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Route.hpp"

#include <algorithm> // std::find
#include <iterator>
#include <limits>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"

// Set a 1 deg angular error limit
const double CA_ROUTE_ALLOWABLE_ANGLE_ERROR_RAD = 1.0 * UtMath::cRAD_PER_DEG;

wsf::six_dof::Route::Route(const Route& aSrc)
{
   // Copy the route
   for (auto& waypoint : aSrc.mRoute)
   {
      mRoute.emplace_back(waypoint->Clone());
   }

   ComputeSegmentMap();
}

wsf::six_dof::Route* wsf::six_dof::Route::Clone() const
{
   return new Route(*this);
}

const wsf::six_dof::Waypoint* wsf::six_dof::Route::GetNextWaypoint(const Waypoint* aWaypoint) const
{
   if (aWaypoint != nullptr)
   {
      std::string goToWpt = aWaypoint->GetGoTo();

      // Next waypoint is specified by goto
      if (goToWpt != "")
      {
         for (auto& waypoint : mRoute)
         {
            if (waypoint->GetLabel() == goToWpt)
            {
               return waypoint.get();
            }
         }
      }
      else // Goto is empty so return the next waypoint in the route
      {
         auto findIt = mRoute.begin();
         while (findIt != mRoute.end())
         {
            if ((*findIt).get() == aWaypoint)
            {
               ++findIt;
               if (findIt != mRoute.end())
               {
                  return (*findIt).get();
               }
            }
            else
            {
               ++findIt;
            }
         }
      }
   }

   // End of route or waypoint passed in is invalid
   return nullptr;
}

const wsf::six_dof::Waypoint* wsf::six_dof::Route::GetWaypointAtIndex(size_t aIndex) const
{
   if (aIndex < GetNumberOfWaypoints())
   {
      return mRoute.at(aIndex).get();
   }
   else
   {
      return nullptr;
   }
}

size_t wsf::six_dof::Route::GetWaypointIndex(const Waypoint* aWaypoint) const
{
   if (aWaypoint != nullptr)
   {
      auto iter = std::find_if(mRoute.begin(),
                               mRoute.end(),
                               [&](const std::unique_ptr<Waypoint>& wp) { return wp.get() == aWaypoint; });

      size_t routeIndex = iter - mRoute.begin();

      // valid index has been found
      if (routeIndex < mRoute.size())
      {
         return routeIndex;
      }
   }

   // Waypoint passed in is invalid
   return static_cast<size_t>(-1);
}

std::unique_ptr<wsf::six_dof::Route::sRouteSegment> wsf::six_dof::Route::CalcSegmentGeometry(const Waypoint* aPrevWpt,
                                                                                             const Waypoint* aCurrWpt)
{
   if (aPrevWpt != nullptr && aCurrWpt != nullptr)
   {
      auto segment             = ut::make_unique<sRouteSegment>();
      segment->trackDistance_m = GetDistanceBetweenWaypoints_m(aPrevWpt->GetLLA(),
                                                               aCurrWpt->GetLLA(),
                                                               segment->trackStartHdg_rad,
                                                               segment->trackEndHdg_rad);

      double dAlt_m      = aCurrWpt->GetLLA().GetAlt() - aPrevWpt->GetLLA().GetAlt();
      segment->slope_rad = atan2(dAlt_m, segment->trackDistance_m);

      double cos_slope = cos(segment->slope_rad);

      if (segment->trackDistance_m > 0.0 && (cos_slope != 0.0))
      {
         segment->slantRange_m = segment->trackDistance_m / cos_slope;
      }
      else
      {
         segment->slantRange_m = fabs(dAlt_m);
      }

      UtEllipsoidalEarth::ConvertLLAToECEF(aPrevWpt->GetLLA().GetLat(),
                                           aPrevWpt->GetLLA().GetLon(),
                                           aPrevWpt->GetLLA().GetAlt(),
                                           segment->prevWptVector.GetData());

      UtEllipsoidalEarth::ComputeNEDTransform(aCurrWpt->GetLLA().GetLat(),
                                              aCurrWpt->GetLLA().GetLon(),
                                              aCurrWpt->GetLLA().GetAlt(),
                                              segment->earthNED,
                                              segment->currWptVector.GetData());

      UtVec3d trackVector = segment->prevWptVector - segment->currWptVector;

      UtMat3d::Transform(segment->trackVector.GetData(), segment->earthNED, trackVector.GetData());
      return segment;
   }

   // There is no segment because one or both of the waypoints are invalid,
   // so return a null pointer.
   return nullptr;
}

void wsf::six_dof::Route::CalcAimHeadingAndBankAngle(const Waypoint*                    aPrevWpt,
                                                     const Waypoint*                    aCurrWpt,
                                                     const Waypoint*                    aNextWpt,
                                                     const sRouteSegment*               aCurrSegment,
                                                     const sRouteSegment*               aNextSegment,
                                                     const UtLLAPos&                    aCurrentPos_lla,
                                                     const UtVec2d&                     aCurrentVel_mps,
                                                     CommonController::WaypointNavData& aNavData,
                                                     double                             aTurnRollInMultiplier,
                                                     double                             aRouteAllowableAngleError_rad,
                                                     double                             aHeading_rad,
                                                     double                             aSpeed_mps,
                                                     double                             aMaxBankAngle_rad,
                                                     double                             aMaxBankRate_rad_s,
                                                     double                             aMaxG_g,
                                                     double                             aDeltaT_sec,
                                                     bool&                              aAchievedWaypoint)
{
   // No waypoint is given as a target, so exit the function
   if (aCurrWpt == nullptr)
   {
      aNavData.aimHeading_rad = aCurrSegment->trackEndHdg_rad;
      aNavData.executeTurn    = false;
      return;
   }

   // The typical track approach angle is pi/4 (45 deg)
   double trackApproachAngle = UtMath::cPI_OVER_4; // *(aMaxBankAngle_rad / aMaxBankRate_rad_s);
   if (aPrevWpt->WaypointOnPassing())
   {
      // We have a "fly-over" waypoint, so our track approach angle is pi/3 (60 deg)
      trackApproachAngle = UtMath::cPI / 3.0;
   }

   double aimHeading_rad                 = aHeading_rad;
   double rangeRate_mps                  = 0.0;
   double hdgError_rad                   = 0.0;
   double curPosToCurrentWptStartHdg_rad = 0.0;
   double curPosToCurrentWptEndHdg_rad   = 0.0;
   double crossTrackDist_m               = 0.0;
   double minTurnRadius_m                = 10.0;
   double turnLeadDist_m                 = 0.0;
   double nextTrackStartHdg_rad          = 0;
   double turnAngle_rad                  = 0.0;

   if (aNextWpt != nullptr)
   {
      nextTrackStartHdg_rad = aNextSegment->trackStartHdg_rad;
   }
   else
   {
      // The current waypoint is the last waypoint, so the target heading
      // will be the current track end heading
      nextTrackStartHdg_rad = aCurrSegment->trackEndHdg_rad;
   }

   // Only check the lat/lon equality at float precision, don't care about altitude
   if (fabs(aCurrentPos_lla.GetLat() - aCurrWpt->GetLLA().GetLat()) < std::numeric_limits<double>::epsilon() &&
       fabs(aCurrentPos_lla.GetLon() - aCurrWpt->GetLLA().GetLon()) <
          std::numeric_limits<double>::epsilon()) // same point, no distance
   {
      // We are exactly over the waypoint
      // TODO -- Should we really do nothing and just return?
      return;
   }

   UtVec3d currPosVector;
   UtEllipsoidalEarth::ConvertLLAToECEF(aCurrentPos_lla.GetLat(),
                                        aCurrentPos_lla.GetLon(),
                                        aCurrentPos_lla.GetAlt(),
                                        currPosVector.GetData());

   UtVec3d deltaCurrPosCurrWpt = currPosVector - aCurrSegment->currWptVector;

   // Determine if waypoint has been passed already
   UtVec3d worldToNEDpos;
   UtMat3d::Transform(worldToNEDpos.GetData(), aCurrSegment->earthNED, deltaCurrPosCurrWpt.GetData());
   UtVec2d pos2d(worldToNEDpos[0], worldToNEDpos[1]);
   UtVec2d trk2d(aCurrSegment->trackVector[0], aCurrSegment->trackVector[1]);
   double  vectorRng = UtVec2d::DotProduct(pos2d, trk2d);

   bool passedCurrWpt = false;
   if (vectorRng < 0.0)
   {
      passedCurrWpt = true;
   }

   aNavData.rangeTrack_m = GetDistanceBetweenWaypoints_m(aCurrentPos_lla,
                                                         aCurrWpt->GetLLA(),
                                                         curPosToCurrentWptStartHdg_rad,
                                                         curPosToCurrentWptEndHdg_rad);
   if (passedCurrWpt)
   {
      aNavData.rangeTrack_m = -aNavData.rangeTrack_m;
   }

   // Calculate rate at which vehicle is approaching the waypoint
   UtVec2d trk2dUnitVec;
   trk2d.Normalize(trk2dUnitVec);
   rangeRate_mps = -UtVec2d::DotProduct(trk2dUnitVec, aCurrentVel_mps);

   // Calculate cross track distance from the aircraft to the track
   UtVec3d deltaCurrPosPrev = currPosVector - aCurrSegment->prevWptVector;
   UtVec3d prevWptCurrWptCross;
   prevWptCurrWptCross.CrossProduct(aCurrSegment->prevWptVector, aCurrSegment->currWptVector);
   crossTrackDist_m = -UtVec3d::DotProduct(deltaCurrPosPrev, prevWptCurrWptCross.GetNormal());

   double bankAngle_rad = 0.0;
   double pilotG_g      = 0.0;

   // Calculate the desired bank angle, given the turn g-load
   if (aCurrWpt->GetMaxTurnG().type == Waypoint::TURN_G_TYPE_LATERAL)
   {
      // Limit the lateral G by converting to pitch G and applying the max g limit
      pilotG_g = sqrt(aCurrWpt->GetMaxTurnG().val * aCurrWpt->GetMaxTurnG().val + 1.0);
      pilotG_g = UtMath::Limit(pilotG_g, aMaxG_g);
   }
   else if (aCurrWpt->GetMaxTurnG().type == Waypoint::TURN_G_TYPE_PILOT)
   {
      pilotG_g = UtMath::Limit(aCurrWpt->GetMaxTurnG().val, aMaxG_g);
   }
   bankAngle_rad = UtMath::Limit((acos(1 / pilotG_g)), aMaxBankAngle_rad);

   double turnRadius_m = GetTurnRadius_m(aSpeed_mps, bankAngle_rad);

   if (turnRadius_m < minTurnRadius_m)
   {
      turnRadius_m = minTurnRadius_m;
   }

   // Since it takes time to roll into the turn, we must provide for the extra distance that this requires
   double rollTimeLeadDistance_m = (bankAngle_rad / (0.5 * aMaxBankRate_rad_s)) * rangeRate_mps;

   // We use the turn roll-in multiplier from the autopilot to adjust the roll-in distance
   rollTimeLeadDistance_m *= aTurnRollInMultiplier;

   turnAngle_rad = UtMath::NormalizeAngleMinusPi_Pi(nextTrackStartHdg_rad - curPosToCurrentWptStartHdg_rad);

   turnLeadDist_m = GetTurnLeadDistance_m(turnAngle_rad, turnRadius_m) + rollTimeLeadDistance_m;

   double boundedXTrack   = UtMath::Limit(crossTrackDist_m, turnRadius_m);
   double offsetAngle_rad = 0.0;

   if (turnRadius_m > 0.0)
   {
      double approachRatio = boundedXTrack / turnRadius_m;
      if (approachRatio < 0.0)
      {
         approachRatio *= (2.0 + approachRatio);
      }
      else
      {
         approachRatio *= (2.0 - approachRatio);
      }

      offsetAngle_rad = trackApproachAngle * approachRatio;
   }

   double distTraveled = aCurrSegment->trackDistance_m - aNavData.rangeTrack_m;
   if (!passedCurrWpt)
   {
      double tempLat, tempLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(aPrevWpt->GetLLA().GetLat(),
                                                          aPrevWpt->GetLLA().GetLon(),
                                                          aCurrSegment->trackStartHdg_rad * UtMath::cDEG_PER_RAD,
                                                          distTraveled,
                                                          tempLat,
                                                          tempLon);
      double tempAlt_m = aCurrentPos_lla.GetAlt();

      double trackCurrentHeading_rad = GetInitialHeading_rad(UtLLAPos(tempLat, tempLon, tempAlt_m), aCurrWpt->GetLLA());

      hdgError_rad = UtMath::NormalizeAngleMinusPi_Pi(trackCurrentHeading_rad - aHeading_rad);

      // Keep the aircraft aligned with the horizontal track
      if (rangeRate_mps > 0.0)
      {
         if (!aNavData.executeTurn)
         {
            aimHeading_rad = trackCurrentHeading_rad - offsetAngle_rad;
         }
      }
      else
      {
         // Moving away from current waypoint or not moving, so maintain track heading
         if (!aNavData.executeTurn)
         {
            aimHeading_rad = trackCurrentHeading_rad;
         }
      }
   }

   // Populate data needed for heading nav
   aNavData.turnLeadDist_m = turnLeadDist_m;
   aNavData.rangeRate_mps  = rangeRate_mps;
   aNavData.deltaAlt_m     = aCurrWpt->GetLLA().GetAlt() - aCurrentPos_lla.GetAlt();
   if (!aNavData.executeTurn)
   {
      aNavData.aimHeading_rad    = aimHeading_rad;
      aNavData.commandedBank_rad = bankAngle_rad;
   }

   if (fabs(hdgError_rad) < aRouteAllowableAngleError_rad)
   {
      if (aNavData.executeTurn)
      {
         aNavData.commandedBank_rad = bankAngle_rad;
         aNavData.executeTurn       = false;
      }
   }

   // If the waypoint is achieved, execute the turn
   if (AchievedWaypoint(static_cast<float>(aDeltaT_sec), aNavData, aCurrWpt, aNextWpt))
   {
      aAchievedWaypoint          = true;
      aNavData.executeTurn       = true;
      aimHeading_rad             = nextTrackStartHdg_rad;
      aNavData.aimHeading_rad    = aimHeading_rad;
      aNavData.commandedBank_rad = bankAngle_rad;
   }
   else
   {
      aAchievedWaypoint = false;
   }
}

void wsf::six_dof::Route::CalcYawAimHeadingAngle(const Waypoint*                    aPrevWpt,
                                                 const Waypoint*                    aCurrWpt,
                                                 const Waypoint*                    aNextWpt,
                                                 const sRouteSegment*               aCurrSegment,
                                                 const sRouteSegment*               aNextSegment,
                                                 const UtLLAPos&                    aCurrentPos_lla,
                                                 const UtVec2d&                     aCurrentVel_mps,
                                                 CommonController::WaypointNavData& aNavData,
                                                 double                             aRouteAllowableAngleError_rad,
                                                 double                             aHeading_rad,
                                                 double                             aSpeed_mps,
                                                 double                             aMaxG_g,
                                                 double                             aDeltaT_sec,
                                                 bool&                              aAchievedWaypoint)
{
   // No waypoint is given as a target, so exit the function
   if (aCurrWpt == nullptr)
   {
      return;
   }

   double trackApproachAngle = UtMath::cPI_OVER_4;

   double aimHeading_rad                 = aHeading_rad;
   double rangeRate_mps                  = 0.0;
   double hdgError_rad                   = 0.0;
   double curPosToCurrentWptStartHdg_rad = 0.0;
   double curPosToCurrentWptEndHdg_rad   = 0.0;
   double crossTrackDist_m               = 0.0;
   double minTurnRadius_m                = 10.0;
   double turnLeadDist_m                 = 0.0;
   double nextTrackStartHdg_rad          = 0;
   double turnAngle_rad                  = 0.0;

   if (aNextWpt != nullptr)
   {
      nextTrackStartHdg_rad = aNextSegment->trackStartHdg_rad;
   }
   else
   {
      // The current waypoint is the last waypoint, so the target heading
      // will be the current track end heading
      nextTrackStartHdg_rad = aCurrSegment->trackEndHdg_rad;
   }

   // only check the lat/lon equality at float precision, don't care about altitude
   if (fabs(aCurrentPos_lla.GetLat() - aCurrWpt->GetLLA().GetLat()) < std::numeric_limits<double>::epsilon() &&
       fabs(aCurrentPos_lla.GetLon() - aCurrWpt->GetLLA().GetLon()) <
          std::numeric_limits<double>::epsilon()) // same point, no distance
   {
      return;
   }

   UtVec3d currPosVector;
   UtEllipsoidalEarth::ConvertLLAToECEF(aCurrentPos_lla.GetLat(),
                                        aCurrentPos_lla.GetLon(),
                                        aCurrentPos_lla.GetAlt(),
                                        currPosVector.GetData());

   UtVec3d deltaCurrPosCurrWpt = currPosVector - aCurrSegment->currWptVector;

   // Determine if waypoint has been passed already
   UtVec3d worldToNEDpos;
   UtMat3d::Transform(worldToNEDpos.GetData(), aCurrSegment->earthNED, deltaCurrPosCurrWpt.GetData());
   UtVec2d pos2d(worldToNEDpos[0], worldToNEDpos[1]);
   UtVec2d trk2d(aCurrSegment->trackVector[0], aCurrSegment->trackVector[1]);
   double  vectorRng     = UtVec2d::DotProduct(pos2d, trk2d);
   bool    passedCurrWpt = vectorRng < 0.0;

   aNavData.rangeTrack_m = GetDistanceBetweenWaypoints_m(aCurrentPos_lla,
                                                         aCurrWpt->GetLLA(),
                                                         curPosToCurrentWptStartHdg_rad,
                                                         curPosToCurrentWptEndHdg_rad);
   if (passedCurrWpt)
   {
      aNavData.rangeTrack_m = -aNavData.rangeTrack_m;
   }

   // Calculate rate at which vehicle is approaching the waypoint
   UtVec2d trk2dUnitVec;
   trk2d.Normalize(trk2dUnitVec);
   rangeRate_mps = -UtVec2d::DotProduct(trk2dUnitVec, aCurrentVel_mps);

   // Calculate cross track distance from the aircraft to the track
   UtVec3d deltaCurrPosPrev = currPosVector - aCurrSegment->prevWptVector;
   UtVec3d prevWptCurrWptCross;
   prevWptCurrWptCross.CrossProduct(aCurrSegment->prevWptVector, aCurrSegment->currWptVector);
   crossTrackDist_m = -UtVec3d::DotProduct(deltaCurrPosPrev, prevWptCurrWptCross.GetNormal());

   double commandedG_g = aCurrWpt->GetMaxTurnG().val;
   // Yaw control uses the yaw g limit, so the turn g type should always be lateral
   if (aCurrWpt->GetMaxTurnG().type == Waypoint::TURN_G_TYPE_LATERAL)
   {
      // Limit the yaw g
      commandedG_g = UtMath::Limit(commandedG_g, static_cast<double>(aMaxG_g));
   }

   double turnRadius_m = GetTurnRadiusFromLateralG_m(aSpeed_mps, commandedG_g);

   if (minTurnRadius_m > turnRadius_m)
   {
      turnRadius_m = minTurnRadius_m;
   }

   // Calculate the ideal-case distance between the aircraft and the waypoint needed to make the turn to the next track
   // double rollTimeLeadDistance_m = (bankAngle_rad / aMaxBankRate_rad_s) * rangeRate_mps;

   turnAngle_rad = UtMath::NormalizeAngleMinusPi_Pi(nextTrackStartHdg_rad - curPosToCurrentWptStartHdg_rad);

   turnLeadDist_m = GetTurnLeadDistance_m(turnAngle_rad, turnRadius_m);

   double boundedXTrack   = UtMath::Limit(crossTrackDist_m, turnRadius_m);
   double offsetAngle_rad = 0.0;

   if (turnRadius_m > 0.0)
   {
      double approachRatio = boundedXTrack / turnRadius_m;
      if (approachRatio < 0.0)
      {
         approachRatio *= (2.0 + approachRatio);
      }
      else
      {
         approachRatio *= (2.0 - approachRatio);
      }

      offsetAngle_rad = trackApproachAngle * approachRatio;
   }

   double distTraveled = aCurrSegment->trackDistance_m - aNavData.rangeTrack_m;
   if (!passedCurrWpt)
   {
      double tempLat, tempLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(aPrevWpt->GetLLA().GetLat(),
                                                          aPrevWpt->GetLLA().GetLon(),
                                                          aCurrSegment->trackStartHdg_rad * UtMath::cDEG_PER_RAD,
                                                          distTraveled,
                                                          tempLat,
                                                          tempLon);

      double trackCurrentHeading_rad =
         GetInitialHeading_rad(UtLLAPos(tempLat, tempLon, aCurrentPos_lla.GetAlt()), aCurrWpt->GetLLA());
      hdgError_rad = UtMath::NormalizeAngleMinusPi_Pi(trackCurrentHeading_rad - aHeading_rad);

      if (rangeRate_mps > 0.0)
      {
         // Need to turn to new waypoint
         if (aNavData.executeTurn)
         {
            if (fabs(hdgError_rad) > aRouteAllowableAngleError_rad)
            {
               if (hdgError_rad > aRouteAllowableAngleError_rad)
               {
                  aimHeading_rad = aHeading_rad + UtMath::cPI_OVER_2;
               }
               else
               {
                  aimHeading_rad       = aHeading_rad - UtMath::cPI_OVER_2;
                  aNavData.executeTurn = false;
               }
            }
            else
            {
               aimHeading_rad = trackCurrentHeading_rad - offsetAngle_rad;
            }
         }
         // Need to maintain alignment with track
         else
         {
            aimHeading_rad = trackCurrentHeading_rad - offsetAngle_rad;
         }
      }
      else
      {
         // Moving away from current waypoint or not moving,
         // so just align self with track.
         aimHeading_rad = trackCurrentHeading_rad;
      }
   }

   // Populate data needed for heading nav
   aNavData.turnLeadDist_m = turnLeadDist_m;
   aNavData.rangeRate_mps  = rangeRate_mps;
   aNavData.deltaAlt_m     = aCurrWpt->GetLLA().GetAlt() - aCurrentPos_lla.GetAlt();
   aNavData.aimHeading_rad = aimHeading_rad;

   // If you've cleared the previous turn, set the new bank angle
   if (fabs(hdgError_rad) < aRouteAllowableAngleError_rad)
   {
      //  aNavData.commandedBank_rad = bankAngle_rad;
      aNavData.executeTurn = false;
   }

   // If the waypoint is achieved, execute the turn
   if (AchievedWaypoint(static_cast<float>(aDeltaT_sec), aNavData, aCurrWpt, aNextWpt))
   {
      aAchievedWaypoint    = true;
      aNavData.executeTurn = true;
   }
   else
   {
      aAchievedWaypoint = false;
   }
}

void wsf::six_dof::Route::CalcTaxiAimHeadingAngle(const Waypoint*                    aPrevWpt,
                                                  const Waypoint*                    aCurrWpt,
                                                  const Waypoint*                    aNextWpt,
                                                  const sRouteSegment*               aCurrSegment,
                                                  const sRouteSegment*               aNextSegment,
                                                  const UtLLAPos&                    aCurrentPos_lla,
                                                  const UtVec2d&                     aCurrentVel_mps,
                                                  CommonController::WaypointNavData& aNavData,
                                                  float                              aHeading_rad,
                                                  double                             aTurnRadius_ft,
                                                  double                             aDeltaT_sec,
                                                  bool&                              aAchievedWaypoint)
{
   // No waypoint is given as a target, so exit the function
   if (aCurrWpt == nullptr)
   {
      return;
   }

   float trackApproachAngle = static_cast<float>(UtMath::cPI_OVER_4);

   double aimHeading_rad                 = aHeading_rad;
   double rangeRate_mps                  = 0.0;
   double hdgError_rad                   = 0.0;
   double curPosToCurrentWptStartHdg_rad = 0.0;
   double curPosToCurrentWptEndHdg_rad   = 0.0;
   double crossTrackDist_m               = 0.0;
   double turnLeadDist_m                 = 0.0;
   double nextTrackStartHdg_rad          = 0;
   double turnAngle_rad                  = 0.0;

   if (aNextWpt != nullptr)
   {
      nextTrackStartHdg_rad = aNextSegment->trackStartHdg_rad;
   }
   else
   {
      // The current waypoint is the last waypoint, so the target heading
      // will be the current track end heading
      nextTrackStartHdg_rad = aCurrSegment->trackEndHdg_rad;
   }

   // only check the lat/lon equality at float precision, don't care about altitude
   if (fabs(aCurrentPos_lla.GetLat() - aCurrWpt->GetLLA().GetLat()) < std::numeric_limits<float>::epsilon() &&
       fabs(aCurrentPos_lla.GetLon() - aCurrWpt->GetLLA().GetLon()) <
          std::numeric_limits<float>::epsilon()) // same point, no distance
   {
      return;
   }

   UtVec3d currPosVector;
   UtEllipsoidalEarth::ConvertLLAToECEF(aCurrentPos_lla.GetLat(),
                                        aCurrentPos_lla.GetLon(),
                                        aCurrentPos_lla.GetAlt(),
                                        currPosVector.GetData());

   UtVec3d deltaCurrPosCurrWpt = currPosVector - aCurrSegment->currWptVector;

   // Determine if waypoint has been passed already
   UtVec3d worldToNEDpos;
   UtMat3d::Transform(worldToNEDpos.GetData(), aCurrSegment->earthNED, deltaCurrPosCurrWpt.GetData());
   UtVec2d pos2d(worldToNEDpos[0], worldToNEDpos[1]);
   UtVec2d trk2d(aCurrSegment->trackVector[0], aCurrSegment->trackVector[1]);
   double  vectorRng     = UtVec2d::DotProduct(pos2d, trk2d);
   bool    passedCurrWpt = vectorRng < 0.0;

   aNavData.rangeTrack_m = GetDistanceBetweenWaypoints_m(aCurrentPos_lla,
                                                         aCurrWpt->GetLLA(),
                                                         curPosToCurrentWptStartHdg_rad,
                                                         curPosToCurrentWptEndHdg_rad);
   if (passedCurrWpt)
   {
      aNavData.rangeTrack_m = -aNavData.rangeTrack_m;
   }

   // Calculate rate at which vehicle is approaching the waypoint
   UtVec2d trk2dUnitVec;
   trk2d.Normalize(trk2dUnitVec);
   rangeRate_mps = -UtVec2d::DotProduct(trk2dUnitVec, aCurrentVel_mps);

   // Calculate cross track distance from the aircraft to the track
   UtVec3d deltaCurrPosPrev = currPosVector - aCurrSegment->prevWptVector;
   UtVec3d prevWptCurrWptCross;
   prevWptCurrWptCross.CrossProduct(aCurrSegment->prevWptVector, aCurrSegment->currWptVector);
   crossTrackDist_m = -UtVec3d::DotProduct(deltaCurrPosPrev, prevWptCurrWptCross.GetNormal());

   // Set the turn radius
   double turnRadius_m = aTurnRadius_ft * UtMath::cM_PER_FT;

   // Calculate the ideal-case distance between the aircraft and the waypoint needed to make the turn to the next track
   // double rollTimeLeadDistance_m = (bankAngle_rad / aMaxBankRate_rad_s) * rangeRate_mps;

   turnAngle_rad = UtMath::NormalizeAngleMinusPi_Pi(nextTrackStartHdg_rad - curPosToCurrentWptStartHdg_rad);

   turnLeadDist_m = GetTurnLeadDistance_m(turnAngle_rad, turnRadius_m);

   // Give a "gain" to the lead-in distance to give time to start making the turn  and ramp-in control
   turnLeadDist_m *= 1.05;

   double boundedXTrack   = UtMath::Limit(crossTrackDist_m, turnRadius_m);
   double offsetAngle_rad = 0.0;

   if (turnRadius_m > 0.0)
   {
      double approachRatio = boundedXTrack / turnRadius_m;
      if (approachRatio < 0.0)
      {
         approachRatio *= (2.0 + approachRatio);
      }
      else
      {
         approachRatio *= (2.0 - approachRatio);
      }

      offsetAngle_rad = trackApproachAngle * approachRatio;
   }

   float distTraveled = static_cast<float>(aCurrSegment->trackDistance_m - aNavData.rangeTrack_m);
   if (!passedCurrWpt)
   {
      double tempLat, tempLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(aPrevWpt->GetLLA().GetLat(),
                                                          aPrevWpt->GetLLA().GetLon(),
                                                          aCurrSegment->trackStartHdg_rad * UtMath::cDEG_PER_RAD,
                                                          distTraveled,
                                                          tempLat,
                                                          tempLon);

      double trackCurrentHeading_rad =
         GetInitialHeading_rad(UtLLAPos(tempLat, tempLon, aCurrentPos_lla.GetAlt()), aCurrWpt->GetLLA());
      hdgError_rad = UtMath::NormalizeAngleMinusPi_Pi(trackCurrentHeading_rad - aHeading_rad);

      if (rangeRate_mps > 0.0)
      {
         // Need to turn to new waypoint
         if (aNavData.executeTurn)
         {
            if (fabs(hdgError_rad) > CA_ROUTE_ALLOWABLE_ANGLE_ERROR_RAD)
            {
               if (hdgError_rad > CA_ROUTE_ALLOWABLE_ANGLE_ERROR_RAD)
               {
                  aimHeading_rad       = aHeading_rad + UtMath::cPI_OVER_2;
                  aNavData.executeTurn = false;
               }
               else
               {
                  aimHeading_rad       = aHeading_rad - UtMath::cPI_OVER_2;
                  aNavData.executeTurn = false;
               }
            }
            else
            {
               aimHeading_rad = trackCurrentHeading_rad - offsetAngle_rad;
            }
         }
         // Need to maintain alignment with track
         else
         {
            aimHeading_rad = trackCurrentHeading_rad - offsetAngle_rad;
         }
      }
      else
      {
         // Moving away from current waypoint or not moving,
         // so just align self with track.
         aimHeading_rad = trackCurrentHeading_rad;
      }
   }

   // Populate data needed for heading nav
   aNavData.turnLeadDist_m = turnLeadDist_m;
   aNavData.rangeRate_mps  = rangeRate_mps;
   aNavData.deltaAlt_m     = aCurrWpt->GetLLA().GetAlt() - aCurrentPos_lla.GetAlt();
   aNavData.aimHeading_rad = aimHeading_rad;

   // If we are within a small angle, do not bother executing the turn
   if (fabs(hdgError_rad) < CA_ROUTE_ALLOWABLE_ANGLE_ERROR_RAD)
   {
      aNavData.executeTurn = false;
   }

   // If the waypoint is achieved, execute the turn
   if (AchievedWaypoint(static_cast<float>(aDeltaT_sec), aNavData, aCurrWpt, aNextWpt))
   {
      aAchievedWaypoint    = true;
      aNavData.executeTurn = true;
   }
   else
   {
      aAchievedWaypoint = false;
   }
}

void wsf::six_dof::Route::CalcVerticalSpeed(CommonController::WaypointNavData& aNavData)
{
   double travelTime_s = std::abs(aNavData.rangeTrack_m) / aNavData.rangeRate_mps;

   // Already at altitude, so stop climb/descent
   if (fabs(aNavData.deltaAlt_m) <= std::numeric_limits<float>::epsilon())
   {
      aNavData.vertSpeed_mps = 0.0;
   }
   // Follow a slope between waypoints to the current waypoint altitude
   else
   {
      aNavData.vertSpeed_mps = aNavData.deltaAlt_m / travelTime_s;
   }
}

bool wsf::six_dof::Route::PassedWaypoint(float aDT, const CommonController::WaypointNavData& aNavData)
{
   if (aNavData.rangeTrack_m <= std::numeric_limits<float>::epsilon())
   {
      return true; // already passed it
   }
   else if (aNavData.rangeTrack_m <= (aNavData.rangeRate_mps * aDT + std::numeric_limits<float>::epsilon()))
   {
      return true; // passing it
   }
   else
   {
      return false; // did not pass it
   }
}

bool wsf::six_dof::Route::AchievedWaypoint(float                                    aDT,
                                           const CommonController::WaypointNavData& aNavData,
                                           const Waypoint*                          aWaypoint,
                                           const Waypoint*                          aNextWaypoint)
{
   // no next waypoint or commanded to fly over it
   if (aNextWaypoint == nullptr || aWaypoint->WaypointOnPassing())
   {
      return PassedWaypoint(aDT, aNavData);
   }
   else if (aNavData.rangeTrack_m <= aNavData.turnLeadDist_m)
   {
      return true; // waypoint achieved
   }
   else if (aNavData.rangeTrack_m + aNavData.rangeRate_mps * aDT <= aNavData.turnLeadDist_m)
   {
      return true; // waypoint will be achieved in next time step
   }
   else
   {
      return false; // waypoint not achieved
   }
}

double wsf::six_dof::Route::GetInitialHeading_rad(const UtLLAPos& aLLAStart, const UtLLAPos& aLLAEnd)
{
   if (aLLAStart == aLLAEnd) // same point, no distance
   {
      return 0.0;
   }

   double sLat_rad = aLLAStart.GetLat() * UtMath::cRAD_PER_DEG;
   double sLon_rad = aLLAStart.GetLon() * UtMath::cRAD_PER_DEG;
   double eLat_rad = aLLAEnd.GetLat() * UtMath::cRAD_PER_DEG;
   double eLon_rad = aLLAEnd.GetLon() * UtMath::cRAD_PER_DEG;

   double dlon     = eLon_rad - sLon_rad;
   double sin_dlon = sin(dlon);
   double cos_dlon = cos(dlon);
   double sin_sLat = sin(sLat_rad);
   double cos_sLat = cos(sLat_rad);
   double sin_eLat = sin(eLat_rad);
   double cos_eLat = cos(eLat_rad);

   return atan2((sin_dlon * cos_eLat), (cos_sLat * sin_eLat - sin_sLat * cos_eLat * cos_dlon));
}

double wsf::six_dof::Route::GetTurnRadius_m(double aSpeed_mps, double aBankAngle_rad)
{
   return aSpeed_mps * aSpeed_mps / (UtEarth::cACCEL_OF_GRAVITY * tan(fabs(aBankAngle_rad)));
}

double wsf::six_dof::Route::GetTurnRadiusFromLateralG_m(double aSpeed_mps, double aLateralG_g)
{
   return (aSpeed_mps * aSpeed_mps / (UtEarth::cACCEL_OF_GRAVITY * aLateralG_g));
}

double wsf::six_dof::Route::GetTurnLeadDistance_m(double aTurnAngle_rad, double aTurnRadius_m)
{
   return tan(std::abs(aTurnAngle_rad * 0.5)) * aTurnRadius_m;
}

double wsf::six_dof::Route::GetDistanceBetweenWaypoints_m(const UtLLAPos& aStartLLA,
                                                          const UtLLAPos& aEndLLA,
                                                          double&         aStartHdg_rad,
                                                          double&         aEndHdg_rad)
{
   double trackDist_m = UtEllipsoidalEarth::GetVincentyDistance(aStartLLA.GetLat(),
                                                                aStartLLA.GetLon(),
                                                                aEndLLA.GetLat(),
                                                                aEndLLA.GetLon(),
                                                                aStartHdg_rad,
                                                                aEndHdg_rad);
   aStartHdg_rad *= UtMath::cRAD_PER_DEG;
   aEndHdg_rad *= UtMath::cRAD_PER_DEG;

   return trackDist_m;
}

size_t wsf::six_dof::Route::GetNumberOfWaypoints() const
{
   return mRoute.size();
}

const wsf::six_dof::Waypoint* wsf::six_dof::Route::GetFirstElement() const
{
   if (!mRoute.empty())
   {
      return mRoute[0].get();
   }
   else
   {
      return nullptr;
   }
}

void wsf::six_dof::Route::ComputeSegmentMap()
{
   for (auto& iter : mRoute)
   {
      const Waypoint* next = GetNextWaypoint(iter.get());

      if (next != nullptr)
      {
         mSegments[iter.get()] = CalcSegmentGeometry(iter.get(), next);
      }
   }
}

wsf::six_dof::Route::sRouteSegment* wsf::six_dof::Route::GetRouteSegment(const Waypoint* aWaypoint) const
{
   auto iter = std::find_if(mRoute.begin(),
                            mRoute.end(),
                            [&](const std::unique_ptr<Waypoint>& wp) { return wp.get() == aWaypoint; });

   if (iter != mRoute.end())
   {
      Waypoint* wpt = (*iter).get();
      if (mSegments.count(wpt) > 0)
      {
         return mSegments.at(wpt).get();
      }
   }

   return nullptr;
}

void wsf::six_dof::Route::AddWaypointToRouteEnd(std::unique_ptr<Waypoint>& aWaypoint)
{
   if (aWaypoint != nullptr)
   {
      for (auto& waypoint : mRoute)
      {
         if (waypoint.get() == aWaypoint.get())
         {
            return;
         }
      }

      mRoute.push_back(std::move(aWaypoint));
   }
}

void wsf::six_dof::Route::AddWaypointToRouteStart(std::unique_ptr<Waypoint>& aWaypoint)
{
   if (aWaypoint != nullptr)
   {
      for (auto& waypoint : mRoute)
      {
         if (waypoint.get() == aWaypoint.get())
         {
            return;
         }
      }

      mRoute.insert(mRoute.begin(), std::move(aWaypoint));
   }
}

void wsf::six_dof::Route::AddSegment(Waypoint* aWaypoint, std::unique_ptr<sRouteSegment>& aSegment)
{
   mSegments[aWaypoint] = std::move(aSegment);
}
