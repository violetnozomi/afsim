// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AeroObject.hpp"

#include "P6DofUtils.hpp"
#include "UtMath.hpp"
#include "Vehicle.hpp"

namespace Designer
{

const double AeroObject::cEpsilon = std::numeric_limits<double>::epsilon();
const double AeroObject::cAxesTransformDCM[3][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0 } };

AeroObject::AeroObject(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : mGeneratedFromName(aGeneratedFromName)
   , mVehicle(aVehicle)
{
}

std::string AeroObject::GetName()
{
   return mName;
}

void AeroObject::SetRefPoint(UtVec3dX aRefPoint_ft)
{
   mRefPoint_ft = aRefPoint_ft;
}

UtVec3dX AeroObject::GetRefPoint_ft() const
{
   return mRefPoint_ft;
}

std::string AeroObject::GetGeneratedFromName() const
{
   return mGeneratedFromName;
}

void AeroObject::SetNormalizedControlValue(double) //aControlValue)
{
   // Default action is to do nothing
}

void AeroObject::SetControlAngle_deg(double) //aControlAngle_deg)
{
   // Default action is to do nothing
}

void AeroObject::SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg)
{
   mVehicleAlpha_deg = aAlpha_deg;
   mVehicleBeta_deg  = aBeta_deg;
}

void AeroObject::SetVehicleYawPitchRollRates(double aYawRate_dps, double aPitchRate_dps, double aRollRate_dps)
{
   mVehicleYawRate_rps   = aYawRate_dps * UtMath::cRAD_PER_DEG;
   mVehiclePitchRate_rps = aPitchRate_dps * UtMath::cRAD_PER_DEG;
   mVehicleRollRate_rps  = aRollRate_dps * UtMath::cRAD_PER_DEG;
}

void AeroObject::SetVehicleSpeedParameters(double aSpeed_fps,
                                           double aMach,
                                           double aKTAS,
                                           double aKCAS,
                                           double aDynamicPressure_psf,
                                           double aAirDensity_slugs_ft3)
{
   mVehicleSpeed_fps           = aSpeed_fps;
   mVehicleMach                = aMach;
   mVehicleKTAS                = aKTAS;
   mVehicleKCAS                = aKCAS;
   mVehicleDynamicPressure_psf = aDynamicPressure_psf;
   mAirDensity_slugs_ft3       = aAirDensity_slugs_ft3;
}

void AeroObject::CalculateAeroAttitudeVectors()
{
   // This calculates mForwardUnitVector, mUpUnitVector, mSideUnitVector, which are based
   // on geometry alone, in the absence of any air flow.
   CalcForwardUpSideUnitVectors();

   CalcAlphaBetaDCM();

   CalcWindUnitVectors();
}

void AeroObject::CalcAeroData(double    aAltitude_ft,
                              double    aSpeed_fps,
                              double    aMach,
                              double    aKTAS,
                              double    aKCAS,
                              double    aDynamicPressure_psf,
                              double    aDensity_slugs_ft3,
                              double    aVehicleAlpha_deg,
                              double    aVehicleBeta_deg,
                              double    aVehicleYawRate_dps,
                              double    aVehiclePitchRate_dps,
                              double    aVehicleRollRate_dps,
                              double&   aTotalLift_lbs,
                              double&   aTotalDrag_lbs,
                              double&   aTotalSide_lbs,
                              UtVec3dX& aTotalMomentVector_ftlbs)
{
   UtVec3dX liftBodyRefVector_lbs;
   UtVec3dX dragBodyRefVector_lbs;
   UtVec3dX sideBodyRefVector_lbs;
   UtVec3dX momentBodyRef_ftlbs;
   UtVec3dX forcePoint_ft;

   CalcForcesAndMoments(aDynamicPressure_psf,
                        aMach,
                        liftBodyRefVector_lbs,
                        dragBodyRefVector_lbs,
                        sideBodyRefVector_lbs,
                        momentBodyRef_ftlbs,
                        forcePoint_ft);

   P6DofForceAndMomentsObject liftBodyZeroVector_lbs;
   P6DofForceAndMomentsObject dragBodyZeroVector_lbs;
   P6DofForceAndMomentsObject sideBodyZeroVector_lbs;
   P6DofForceAndMomentsObject momentBodyZero_ftlbs;

   // Set the reference point at the object's aero reference point
   liftBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   dragBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   sideBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   momentBodyZero_ftlbs.MoveRefPoint_ft(forcePoint_ft);

   // Clear F&M data
   liftBodyZeroVector_lbs.ClearForcesAndMoments();
   dragBodyZeroVector_lbs.ClearForcesAndMoments();
   sideBodyZeroVector_lbs.ClearForcesAndMoments();
   momentBodyZero_ftlbs.ClearForcesAndMoments();

   // Add the body-relative data at the reference point
   UtVec3dX zeroVec(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(liftBodyRefVector_lbs, zeroVec);
   dragBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(dragBodyRefVector_lbs, zeroVec);
   sideBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(sideBodyRefVector_lbs, zeroVec);
   momentBodyZero_ftlbs.AddForceAndMomentAtReferencePoint(zeroVec, momentBodyRef_ftlbs);

   // Move the reference point to the origin as the "zero point, rather than using
   // each object's aero force center

   UtVec3dX zeroPointReference(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   dragBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   sideBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   momentBodyZero_ftlbs.MoveRefPoint_ft(zeroPointReference);

   // At this point, all F&M are based on a reference point of the origin, as desired

   // Now, we calculate the total moment, including all induced moments from all F&M contributors

   UtVec3dX partialMoment(0.0, 0.0, 0.0);
   UtVec3dX totalMomentBodyZero_ftlbs(0.0, 0.0, 0.0);

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempLiftBodyZeroVector_lbs(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempLiftBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempDragBodyZeroVector_lbs(0.0, 0.0, 0.0);
   dragBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempDragBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempSideBodyZeroVector_lbs(0.0, 0.0, 0.0);
   sideBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempSideBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   UtVec3dX zeroForce(0.0, 0.0, 0.0);
   partialMoment.Set(0.0, 0.0, 0.0);
   momentBodyZero_ftlbs.GetForceAndMomentAtCurrentRefPoint(zeroForce, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   // We now have the body-relative forces at the zero point. We also have the
   // total moment (in body coordinates) at the zero point.

   // We now need to determine the magnitude of the forces along the primary lift, drag, and
   // side force directions, rather than in body space.

   UtVec3dX liftVectorDirection(0.0, 0.0, -1.0);
   UtVec3dX dragVectorDirection(-1.0, 0.0, 0.0);
   UtVec3dX sideVectorDirection(0.0, 1.0, 0.0);

   // Transform the body relative vectors into the primary reference

   // Note negative beta
   UtDCM dcm(-aVehicleBeta_deg * UtMath::cRAD_PER_DEG, aVehicleAlpha_deg * UtMath::cRAD_PER_DEG, 0.0);

   UtVec3dX tempLiftVectorPrimary_lbs = dcm.InverseTransform(tempLiftBodyZeroVector_lbs);
   UtVec3dX tempDragVectorPrimary_lbs = dcm.InverseTransform(tempDragBodyZeroVector_lbs);
   UtVec3dX tempSideVectorPrimary_lbs = dcm.InverseTransform(tempSideBodyZeroVector_lbs);

   // Get forces along the primary lift vector direction
   aTotalLift_lbs = liftVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      liftVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      liftVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get forces along the primary drag vector direction
   aTotalDrag_lbs = dragVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      dragVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      dragVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get forces along the primary side vector direction
   aTotalSide_lbs = sideVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      sideVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      sideVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get the total moment (in body coordinates)
   aTotalMomentVector_ftlbs = totalMomentBodyZero_ftlbs;
}

void AeroObject::CalcAeroCoeffDelta(const std::string&/*aControlSurfaceName*/,
                                    double            /*aControlAngle_deg*/,
                                    double            /*aAltitude_ft*/,
                                    double            /*aSpeed_fps*/,
                                    double            /*aMach*/,
                                    double            /*aKTAS*/,
                                    double            /*aKCAS*/,
                                    double            /*aDynamicPressure_psf*/,
                                    double            /*aDensity_slugs_ft3*/,
                                    double            /*aVehicleAlpha_deg*/,
                                    double            /*aVehicleBeta_deg*/,
                                    double            /*aVehicleYawRate_dps*/,
                                    double            /*aVehiclePitchRate_dps*/,
                                    double            /*aVehicleRollRate_dps*/,
                                    double&           aDeltaCL,
                                    double&           aDeltaCd,
                                    double&           aDeltaCY,
                                    double&           aDeltaCm,
                                    double&           aDeltaCn,
                                    double&           aDeltaCl)
{
   aDeltaCL = 0.0;
   aDeltaCd = 0.0;
   aDeltaCY = 0.0;
   aDeltaCm = 0.0;
   aDeltaCn = 0.0;
   aDeltaCl = 0.0;
}

void Designer::AeroObject::CalcAlphaBetaDCM()
{
   UtVec3dX vectorX(1.0, 0.0, 0.0);
   mAlphaBetaDCM = UtDCM(-mVehicleBeta_deg * UtMath::cRAD_PER_DEG, mVehicleAlpha_deg * UtMath::cRAD_PER_DEG, 0.0);

   // Take into account induced velocity due to angular rates

   // Check for zero speed
   if (mVehicleSpeed_fps < cEpsilon)
   {
      // Speed is zero, so return
      return;
   }

   // velocity without including angular rates
   UtVec3dX velocityUnitVector = mAlphaBetaDCM.Transform(vectorX);

   UtVec3dX rotVec(mVehicleRollRate_rps, mVehiclePitchRate_rps, mVehicleYawRate_rps);

   // If we don't have any induced velocity, don't bother computing
   if (rotVec.Magnitude() > cEpsilon)
   {
      UtVec3dX aeroRefPt = CalcAeroForcePoint_ft(mVehicleMach);
      UtVec3dX relInducedVel_fps = aeroRefPt.Cross(rotVec);

      // Calc total relative velocity vector (ft/sec), which may increase/decrease the total speed
      UtVec3dX totalRelVelVector_fps = velocityUnitVector * mVehicleSpeed_fps - relInducedVel_fps;

      UtVec3dX totalRelVelUnitVector;
      totalRelVelVector_fps.Normalize(totalRelVelUnitVector);

      double beta_deg = 0;
      double alpha_deg = 0;

      P6DofUtils::AlphaBetaFromBodyRelVel(totalRelVelVector_fps, alpha_deg, beta_deg);

      mAlphaBetaDCM = UtDCM(-beta_deg * UtMath::cRAD_PER_DEG, alpha_deg * UtMath::cRAD_PER_DEG, 0.0);
   }

}

void Designer::AeroObject::CalcForwardUpSideUnitVectors()
{
   // Calculate the attitude dcm. From sub-object body coordinates to vehicle body coordinates. 
   // Each derived class must implement this function.
   CalcGeometryDCM();
}

void Designer::AeroObject::CalcWindUnitVectors()
{
   UtDCM windDCM = mAlphaBetaDCM * mGeometryDCM;
   mVelocityUnitVector = windDCM.GetCol(0);
   mSideForceUnitVector = windDCM.GetCol(1);
   mLiftForceUnitVector = windDCM.GetCol(2);
   mDragForceUnitVector = -mVelocityUnitVector;
}

void Designer::AeroObject::CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg)
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM totalDCM = axesDCM.Transpose() * mGeometryDCM.Transpose() * mAlphaBetaDCM * mGeometryDCM * axesDCM;
   UtVec3dX velocity = totalDCM.GetCol(0);

   P6DofUtils::AlphaBetaFromBodyRelVel(velocity, aAlpha_deg, aBeta_deg);
}

void Designer::AeroObject::InvertGeometryVectors()
{
   // Invert side vector
   mGeometryDCM.SetVal(0, 1, -mGeometryDCM[0][1]);
   mGeometryDCM.SetVal(1, 1, -mGeometryDCM[1][1]);
   mGeometryDCM.SetVal(2, 1, -mGeometryDCM[2][1]);

   // Invert up vector
   mGeometryDCM.SetVal(0, 2, -mGeometryDCM[0][2]);
   mGeometryDCM.SetVal(1, 2, -mGeometryDCM[1][2]);
   mGeometryDCM.SetVal(2, 2, -mGeometryDCM[2][2]);
}

void Designer::AeroObject::InvertWindVectors()
{
   mSideForceUnitVector *= -1;
   mLiftForceUnitVector *= -1;
}

} // namespace Designer
