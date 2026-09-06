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

#include "AeroSurface.hpp"

#include <QStringList>

#include "AeroData.hpp"
#include "GeometryTriangle.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "P6DofUtils.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"

namespace Designer
{

AeroSurface::AeroSurface(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroObject(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroObject type
   mAeroTypeString.assign("AeroSurface");
}

void AeroSurface::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                       double    aMach,
                                       UtVec3dX& aLiftVector_lbs,
                                       UtVec3dX& aDragVector_lbs,
                                       UtVec3dX& aSideVector_lbs,
                                       UtVec3dX& aMoment_ftlbs,
                                       UtVec3dX& aForcePoint_ft)
{
   // Calculate the current alpha/beta
   double alpha_deg = 0.0;
   double beta_deg  = 0.0;
   CalcAlphaBeta(alpha_deg, beta_deg);
 
   double effectiveSweep_deg = CalcSweep_deg();

   // Calculate the current aero force point, based on Mach
   aForcePoint_ft = CalcAeroForcePoint_ft(aMach);

   // Add in the additional dynamic pressure
   aDynamicPressure_lbssqft += mAdditionalDynamicPressure_psf;

   CalcLiftVector(alpha_deg, beta_deg, effectiveSweep_deg, aDynamicPressure_lbssqft, aMach, aLiftVector_lbs);
   CalcDragVector(alpha_deg, beta_deg, effectiveSweep_deg, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
   CalcSideForceVector(alpha_deg, beta_deg, effectiveSweep_deg, aDynamicPressure_lbssqft, aMach, aSideVector_lbs);
   CalcMomentVector(alpha_deg, beta_deg, effectiveSweep_deg, aDynamicPressure_lbssqft, aMach, aLiftVector_lbs,
                    aDragVector_lbs, aSideVector_lbs, aMoment_ftlbs);
}

double AeroSurface::CalcSweep_deg() const
{
   return mVelocityUnitVector.AngleBetween(mSweepUnitVector) * UtMath::cDEG_PER_RAD - 90.0;
}

void AeroSurface::CalcLiftVector(double    aAlpha_deg,
                                 double    aBeta_deg,
                                 double    aEffectiveSweep_deg,
                                 double    aDynamicPressure_lbssqft,
                                 double    aMach,
                                 UtVec3dX& aLiftVector_lbs)
{
   double CL = CalcLiftCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double lift_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CL;
   aLiftVector_lbs = lift_lbs * mLiftForceUnitVector;
}

void AeroSurface::CalcDragVector(double    aAlpha_deg,
                                 double    aBeta_deg,
                                 double    aEffectiveSweep_deg,
                                 double    aDynamicPressure_lbssqft,
                                 double    aMach,
                                 UtVec3dX& aDragVector_lbs)
{
   double CD = CalcDragCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

void AeroSurface::CalcSideForceVector(double    aAlpha_deg,
                                      double    aBeta_deg,
                                      double    aEffectiveSweep_deg,
                                      double    aDynamicPressure_lbssqft,
                                      double    aMach,
                                      UtVec3dX& aSideForceVector_lbs)
{
   double CY = CalcSideForceCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double sideForce_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CY;
   aSideForceVector_lbs = sideForce_lbs * mSideForceUnitVector;
}

void AeroSurface::CalcMomentVector(double    aAlpha_deg,
                                   double    aBeta_deg,
                                   double    aEffectiveSweep_deg,
                                   double    aDynamicPressure_lbssqft,
                                   double    aMach,
                                   UtVec3dX& aLiftVector_lbs,
                                   UtVec3dX& aDragVector_lbs,
                                   UtVec3dX& aSideForceVector_lbs,
                                   UtVec3dX& aMoment_ftlbs)
{
   // As the AR goes to inf, the multiplier goes to 1. In other words, it becomes equivalent to an infinite wing.
   // As the AR goes to 0, the multiplier goes to 0.
   // This comes from an approximation of the ratio between 2D and 3D lift.
   double finiteWingMultiplier = mAspectRatio / (mAspectRatio + 2.0);

   double Cm = finiteWingMultiplier * CalcZeroLiftPitchingMoment(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double moment_ftlbs = aDynamicPressure_lbssqft * mArea_ft2 * 0.5*(mRootChord_ft + mTipChord_ft) * Cm;

   // Assume that pitching moment is about the side force vector
   aMoment_ftlbs = moment_ftlbs * mSideForceUnitVector;
}

double AeroSurface::CalcLiftCoefficient(double aAlpha_deg,
                                        double aBeta_deg,
                                        double aEffectiveSweep_deg,
                                        double aMach)
{
   return mAeroData.GetLiftCoeff(aAlpha_deg, aEffectiveSweep_deg);
}

double AeroSurface::CalcDragCoefficient(double aAlpha_deg,
                                        double aBeta_deg,
                                        double aEffectiveSweep_deg,
                                        double aMach)
{
   // Base Cd with sweep effects
   double Cd_base = mAeroData.GetDragCoeff(aAlpha_deg, aEffectiveSweep_deg);

   // Calculate induced drag from lift.
   double liftCoeff = CalcLiftCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double inducedDrag = liftCoeff * liftCoeff / (UtMath::cPI * mOswaldsEfficiency * mAspectRatio);

   // Skin Friction
   double skinFriction = CalcSkinFrictionCoeff(aMach) * 2;

   // Transonic drag rise effects
   double transonicMultiplier = CalcTransonicMultiplier(aMach);

   return Cd_base * transonicMultiplier + inducedDrag + skinFriction;
}

void AeroSurface::CalcTransonicMachEndpoints(double& aMachDragDivergence, double& aMachPeakPressure) const
{
   double effective_sweep = CalcSweep_deg() * UtMath::cRAD_PER_DEG;

   double cosSweep = std::cos(std::abs(effective_sweep));
   double sinSweep = std::sin(std::abs(effective_sweep));

   double efficiency = mAeroData.GetAirfoilFactor();

   double effTerm = (1.0 / 3.0) * efficiency * sinSweep;
   double thicknessTerm = 0.5 * mThicknessRatio * sinSweep * sinSweep;

   // Drag divergence number at 0 sweep. From best fit with experimental data.
   double Mdd0 = efficiency - 0.5 * mThicknessRatio - 0.02;

   // Drag divergence number.
   // Decreasing the thickness ratio and increasing the sweep delays the divergence Mach.
   aMachDragDivergence = std::max(0.0, effTerm - thicknessTerm) + Mdd0;

   // From experimental data. 
   // Mpeak will be at least 0.01 more than Mdd
   aMachPeakPressure = aMachDragDivergence + (0.01 + 0.125*std::pow(cosSweep, 3));
}

double AeroSurface::CalcTransonicMultiplier(double aMach) const
{
   // Sources:
   // 1) W.H. Mason, "Configuration Aerodynamics", Chapter 7: Transonic Aerodynamics of Airfoils and Wings
   // 2) NASA SP-367 "Introduction to the Aerodynamics of Flight". Chapter V: Transonic Flow

   double effective_sweep = CalcSweep_deg() * UtMath::cRAD_PER_DEG;
   double sinSweep = std::sin(std::abs(effective_sweep));

   double Mdd   = 0.0;
   double Mpeak = 0.0;
   CalcTransonicMachEndpoints(Mdd, Mpeak);

   // For sweep [0, 90], peakMult ranges from [4, 1]
   double peakMult = 4 - 3 * sinSweep;

   double transonicSlope = (peakMult - 1.0) / (Mpeak - Mdd);

   double transonicMult = 1.0;
   if (peakMult > 1.0)
   {
      if (aMach >= Mdd && aMach <= Mpeak)
      {
         transonicMult = 1 + transonicSlope * (aMach - Mdd);
      }
      else if (aMach > Mpeak)
      {
         // Value that the transonic multiplier asymptotically approaches.
         double a = 0.6 * peakMult + 0.4; // weighted average between peakMult and 1.

         // Satisfy the boundary condition that transonicMult at Mpeak is equal to peakMult
         double b = std::log(peakMult - a) + Mpeak;

         transonicMult = std::exp(b - aMach) + a;
      }
   }
   return transonicMult;
}

void Designer::AeroSurface::CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg)
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM dihedralDCM(0.0, 0.0, mDihedralAngle_rad);

   UtDCM geometryDCM = dihedralDCM * axesDCM;

   UtDCM totalDCM = axesDCM.Transpose() * mGeometryDCM.Transpose() * mAlphaBetaDCM * geometryDCM * axesDCM;
   UtVec3dX velocity = totalDCM.GetCol(0);

   P6DofUtils::AlphaBetaFromBodyRelVel(velocity, aAlpha_deg, aBeta_deg);
}

double Designer::AeroSurface::CalcSkinFrictionCoeff(double aMach)
{
   double reynoldsNumber = aMach * cSpeedOfSound * mRootChord_ft / cKinematicViscosity;

   if (reynoldsNumber < 5e5)
   {
      return 1.328 / std::sqrt(reynoldsNumber); // laminar flow
   }
   return 0.0307 / std::pow(reynoldsNumber, (1.0 / 7.0)); // turbulent flow
}

void Designer::AeroSurface::CalcGeometryDCM()
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM dihedralDCM(0.0, 0.0, mDihedralAngle_rad);
   UtDCM incidenceDCM(0.0, -CombinedControlAndIncidenceAngle_rad(), 0.0);

   mGeometryDCM = dihedralDCM * incidenceDCM * axesDCM;
}

void Designer::AeroSurface::CalcForwardUpSideUnitVectors()
{
   AeroObject::CalcForwardUpSideUnitVectors();

   if (mInvertVectors)
   {
      InvertGeometryVectors();
   }
}

void Designer::AeroSurface::CalcWindUnitVectors()
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM incidenceDCMinv(0.0, CombinedControlAndIncidenceAngle_rad(), 0.0);
   UtDCM dihedralDCM(0.0, 0.0, mDihedralAngle_rad);

   UtDCM totalDCM = mAlphaBetaDCM * dihedralDCM * axesDCM;

   mVelocityUnitVector = totalDCM.GetCol(0);
   mSideForceUnitVector = totalDCM.GetCol(1);
   mLiftForceUnitVector = totalDCM.GetCol(2);
   mDragForceUnitVector = -mVelocityUnitVector;

   if (mInvertVectors)
   {
      InvertWindVectors();
   }

   CalcSweepUnitVector();
}

double AeroSurface::CalcSideForceCoefficient(double aAlpha_deg,
                                             double aBeta_deg,
                                             double aEffectiveSweep_deg,
                                             double aMach)
{
   return 0.0;
}

double AeroSurface::CalcZeroLiftPitchingMoment(double aAlpha_deg,
                                               double aBeta_deg,
                                               double aEffectiveSweep_deg,
                                               double aMach)
{
   return mAeroData.GetPitchMomentCoeff(aAlpha_deg, aEffectiveSweep_deg);
}

void AeroSurface::SetData(const std::string& aName,
                          UtVec3dX           aRefPoint_ft,
                          double             aSpan_ft,
                          double             aRootChord_ft,
                          double             aTipChord_ft,
                          double             aThicknessRatio,
                          double             aSweepAngle_rad,
                          double             aDihedralAngle_rad,
                          double             aIncidenceAngle_rad,
                          double             aOswaldsEfficiency,
                          bool               aInvertVectors,
                          double             aAspectRatio,
                          double             aArea_ft2,
                          int                aNumAeroSections,
                          double             aSurfaceArea_ft2)
{
   mName               = aName;
   mRefPoint_ft        = aRefPoint_ft;
   mSpan_ft            = aSpan_ft;
   mRootChord_ft       = aRootChord_ft;
   mTipChord_ft        = aTipChord_ft;
   mThicknessRatio     = aThicknessRatio;
   mSweepAngle_rad     = aSweepAngle_rad;
   mDihedralAngle_rad  = aDihedralAngle_rad;
   mIncidenceAngle_rad = aIncidenceAngle_rad;
   mOswaldsEfficiency  = aOswaldsEfficiency;
   mInvertVectors      = aInvertVectors;
   mAspectRatio        = aAspectRatio;
   mArea_ft2           = aArea_ft2;
   mNumAeroSections    = aNumAeroSections;
   mSurfaceArea_ft2    = aSurfaceArea_ft2;
}

void AeroSurface::SetAdditionalData(const std::string&    aAirfoilFilename,
                                    bool                  aUseExponentialAngleMapping)
{
   mAirfoilFilename             = aAirfoilFilename;
   //mUseExponentialAngleMapping  = aUseExponentialAngleMapping;

   LoadAirfoil();
}

QStringList AeroSurface::Audit()
{
   return mAeroData.Audit();
}

UtVec3dX AeroSurface::CalcAeroForcePoint_ft(double aMach) const
{
   // Determine the area for a single surface
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   double totalArea = mSpan_ft * averageChord;

   // Break the surface into two triangles (or a single triangle, if tip chord is zero)
   GeometryTriangle triangle1;
   GeometryTriangle triangle2;

   UtVec3dX centroid;
   UtVec3dX rootQuarterChordPt;
   UtVec3dX tipQuarterChordPt;

   // Check for the tip being zero length. We'll assume anything less than
   // a billionth of a foot is zero, due to numerical precision
   if (mTipChord_ft < 0.000000001)
   {
      // We have only a single triangle

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root quarter chord the tip quarter chord exists
      double offsetLength_ft = tan(mSweepAngle_rad) * mSpan_ft;

      UtVec3dX offsetVec(-offsetLength_ft, mSpan_ft, 0.0);
      tipQuarterChordPt = rootQuarterChordPt + offsetVec;

      // Set up the single triangle
      triangle1.SetPoints(FrontRootPt, AftRootPt, tipQuarterChordPt);

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle in AeroSurface::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      // Calculate the location of the cg
      centroid = triangle1.GetCentroid_ft();
   }
   else
   {
      // We have two triangles

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root leading edge the tip leading edge exists
      double offsetLength_ft = tan(mSweepAngle_rad) * mSpan_ft;

      UtVec3dX FrontTipPt(-offsetLength_ft, mSpan_ft, 0.0);
      UtVec3dX tipQuarterChordVec(-mTipChord_ft * 0.25, 0.0, 0.0);
      tipQuarterChordPt = FrontTipPt + tipQuarterChordVec;
      UtVec3dX tipAftVec(-mTipChord_ft, 0.0, 0.0);
      
      UtVec3dX AftTipPt = FrontTipPt + tipAftVec;

      // Set up the two triangles
      triangle1.SetPoints(FrontRootPt, AftRootPt, FrontTipPt);
      triangle2.SetPoints(AftTipPt, FrontTipPt, AftRootPt);

      double averageChord2   = 0.5 * mTipChord_ft;
      double area2           = mSpan_ft * averageChord2;
      double fractionalArea2 = area2 / totalArea;

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle1 in AeroSurface::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      if (!triangle2.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle2 in AeroSurface::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      // Calculate the location of the cg
      UtVec3dX cg1 = triangle1.GetCentroid_ft();
      UtVec3dX cg2 = triangle2.GetCentroid_ft();

      UtVec3dX delCg = cg2 - cg1;
      UtVec3dX cg    = cg1 + (delCg * fractionalArea2);

      // Calculate the location of the cg
      centroid = cg;
   }

   // Using the centroid, determine the centroid at the quarter-chord point
   UtVec3dX spanVect = tipQuarterChordPt - rootQuarterChordPt;
   double ySpanDist  = tipQuarterChordPt.Y() - rootQuarterChordPt.Y();
   double yFraction  = centroid.Y() / ySpanDist;
   spanVect = spanVect * yFraction;
   UtVec3dX aeroCenterQuarterChord = rootQuarterChordPt + spanVect;
   UtVec3dX aeroCenterHalfChord = centroid;

   // Adjust for root quarter chord
   UtVec3dX rootChordAdjustment(mRootChord_ft * 0.25, 0.0, 0.0);
   aeroCenterQuarterChord += rootChordAdjustment;
   aeroCenterHalfChord += rootChordAdjustment;

   // Mach effects
   UtVec3dX aeroCenter;
   if (aMach < 0.9)
   {
      aeroCenter = aeroCenterQuarterChord;
   }
   else if (aMach < 1.9)
   {
      double fraction = (aMach - 0.9);
      aeroCenter = aeroCenterQuarterChord;
      UtVec3dX deltaVec = aeroCenterHalfChord - aeroCenterQuarterChord;
      deltaVec = deltaVec * fraction;
      aeroCenter = aeroCenter + deltaVec;
   }
   else
   {
      aeroCenter = aeroCenterHalfChord;
   }

   // Now, adjust for orientation (dihedral and incidence)
   UtDCM dihedralDCM(0.0, 0.0, -mDihedralAngle_rad);
   UtDCM incidenceDCM(0.0, CombinedControlAndIncidenceAngle_rad(), 0.0);
   UtDCM attitudeDCM = incidenceDCM * dihedralDCM;
   aeroCenter = attitudeDCM.InverseTransform(aeroCenter);

   // Finally, add in ref pt offset
   aeroCenter += mRefPoint_ft;

   return aeroCenter;
}

void AeroSurface::CalcSweepUnitVector()
{
   double surfaceDihedral_rad = mDihedralAngle_rad;
   double surfaceIncidence_rad = CombinedControlAndIncidenceAngle_rad(); // Effective incidence angle is influenced by dihedral
   double surfaceSweep_rad = mSweepAngle_rad;                        // Effective sweep angle is influenced by dihedral

   UtDCM surfaceDihedralDCM(0.0, 0.0, -surfaceDihedral_rad); // For proper angle, use negative dihedral
   UtDCM surfaceIncidenceDCM(0.0, surfaceIncidence_rad, 0.0);
   UtDCM surfaceSweepDCM(surfaceSweep_rad, 0.0, 0.0);

   mDCM_DihedralIncidenceAndSweep = surfaceSweepDCM * surfaceIncidenceDCM * surfaceDihedralDCM;

   mSweepUnitVector = mDCM_DihedralIncidenceAndSweep.GetRow(1); // sweep unit vector in vehicle coordinates
}

void AeroSurface::LoadAirfoil()
{
   if (mAeroData.LoadFromFile(QString::fromStdString(mAirfoilFilename), QString::fromStdString(mName)))
   {
      if (mAeroData.ProcessAeroData())
      {
         for (int s = -90; s <= 90; s += 10)
         {
            mAeroData.CalculateNominalData(mAspectRatio, s);
         }
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Malformed airfoil data input in " + QString::fromStdString(mAirfoilFilename) + "!");
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Unable to load airfoil file for " + QString::fromStdString(mName) + "!");
   }
}

double AeroSurface::CombinedControlAndIncidenceAngle_rad() const
{
   return mIncidenceAngle_rad;
}

} // namespace Designer
