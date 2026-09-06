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

#include "AeroBody.hpp"

#include "P6DofUtils.hpp"
#include "UtMath.hpp"
#include "UtLog.hpp"
#include "Vehicle.hpp"

namespace Designer
{

// After extensive testing, it has been determined 
// that Mover Creator's predictive aerodynamic calculations
// consistently over-estimate the drag coefficients for bodies 
// of revolution. This multiplier was selected through trial and error
// such that no aero modifiers were necessary in Mover Creator .amc template files 
// for the majority of vehicles.
const double AeroBody::cDragMultiplier = 0.65;

AeroBody::AeroBody(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroObject(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroBody type
   mAeroTypeString.assign("AeroBody");

   // These values have been chosen to fit researched values Cd0's of the various forward shapes
   // Source: "Aerodynamic Characteristics of Blunt Bodies", JPL, 1964

   mForwardShapeConstants.mSubsonicDragCoeff =   { { GeometryBody::ForwardShapeType::cCONE,  0.3  },
      { GeometryBody::ForwardShapeType::cOGIVE, 0.2  },
      { GeometryBody::ForwardShapeType::cROUND, 0.1  },
      { GeometryBody::ForwardShapeType::cBLUNT, 0.8  }
   };

   mForwardShapeConstants.mPeakDragCoeff =       { { GeometryBody::ForwardShapeType::cCONE,  0.7 },
      { GeometryBody::ForwardShapeType::cOGIVE, 0.6 },
      { GeometryBody::ForwardShapeType::cROUND, 0.9 },
      { GeometryBody::ForwardShapeType::cBLUNT, 2.0 }
   };

   mForwardShapeConstants.mSupersonicDragCoeff = { { GeometryBody::ForwardShapeType::cCONE,  0.35 },
      { GeometryBody::ForwardShapeType::cOGIVE, 0.45 },
      { GeometryBody::ForwardShapeType::cROUND, 0.70 },
      { GeometryBody::ForwardShapeType::cBLUNT, 1.80 }
   };

   mForwardShapeConstants.mPeakMach =            { { GeometryBody::ForwardShapeType::cCONE,  1},
      { GeometryBody::ForwardShapeType::cOGIVE, 1 },
      { GeometryBody::ForwardShapeType::cROUND, 1 },
      { GeometryBody::ForwardShapeType::cBLUNT, 1 }
   };

   mForwardShapeConstants.mRiseFactor =          { { GeometryBody::ForwardShapeType::cCONE,  2.0 / 3.0 },
      { GeometryBody::ForwardShapeType::cOGIVE, 2.0 / 3.0 },
      { GeometryBody::ForwardShapeType::cROUND, 2.0 / 3.0 },
      { GeometryBody::ForwardShapeType::cBLUNT, 2.0 / 3.0 }
   };

   mForwardShapeConstants.mDropFactor =          { { GeometryBody::ForwardShapeType::cCONE,  2.0 / 3.0 },
      { GeometryBody::ForwardShapeType::cOGIVE, 2.0 / 3.0 },
      { GeometryBody::ForwardShapeType::cROUND, 0.5 },
      { GeometryBody::ForwardShapeType::cBLUNT, 1.0 / 3.0 }
   };

   mMaxTableMach = aVehicle->GetAeroDataMaxMach();
}

void AeroBody::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
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

   // Calculate the current aero force point, based on Mach
   aForcePoint_ft = CalcAeroForcePoint_ft(aMach);

   // Verify cross sectional area
   if (mCrossSectionalArea_sqft <= 0.0)
   {
      ut::log::error() << "Non-positive cross sectional area in AeroBody::CalcForcesAndMoments().";
   }

   double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;
   double beta_rad  = beta_deg * UtMath::cRAD_PER_DEG;

   // Add in the additional dynamic pressure
   aDynamicPressure_lbssqft += mAdditionalDynamicPressure_psf;

   CalcLiftVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aLiftVector_lbs);
   CalcDragVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
   CalcSideForceVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aSideVector_lbs);
   CalcMomentVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMoment_ftlbs);
}

UtVec3dX AeroBody::CalcAeroForcePoint_ft(double aMach) const
{
   const double epsilon   = std::numeric_limits<double>::epsilon();
   const double avgDiam   = 0.5 * (mHeight_ft + mWidth_ft);
   const double avgRadius = avgDiam * 0.5;
   const double pi        = UtMath::cPI;
   double foreLength_ft   = 0.0;
   double aftLength_ft    = 0.0;
   double foreArea        = 0.0;
   double foreCentroidX   = 0.0;
   double midArea         = 0.0;
   double midCentroidX    = 0.0;
   double aftArea         = 0.0;
   double aftCentroidX    = 0.0;

   // First, determine the fore data
   switch (mForwardShapeType)
   {
      case GeometryBody::ForwardShapeType::cROUND:
      {
         if (mNoseLength_ft > epsilon)
         {
            foreLength_ft = mNoseLength_ft;
            foreArea      = 0.5 * pi * avgRadius * foreLength_ft;
            foreCentroidX = foreLength_ft * 0.42;
         }
         break;
      }
      case GeometryBody::ForwardShapeType::cOGIVE:
      {
         if (mNoseLength_ft > epsilon)
         {
            foreLength_ft = mNoseLength_ft;

            // We assume the area and centroid of the ogive is roughly between a circle and cone
            foreArea      = 0.5 * ((0.5 * pi * avgRadius * foreLength_ft) + (foreLength_ft * avgRadius));
            foreCentroidX = foreLength_ft * 0.5 * (0.42 + 0.33333);
         }
         break;
      }
      case GeometryBody::ForwardShapeType::cCONE:
      {
         if (mNoseLength_ft > epsilon)
         {
            foreLength_ft = mNoseLength_ft;
            foreArea      = foreLength_ft * avgRadius;
            foreCentroidX = foreLength_ft * 0.33333;
         }
         break;
      }
      default: // Includes cBLUNT
      {
         // Do nothing
         break;
      }
   }

   // Next, determine the aft data
   switch (mAftShapeType)
   {
      case GeometryBody::AftShapeType::cROUND:
      {
         if (mAftShapeLength_ft > epsilon)
         {
            aftLength_ft = mAftShapeLength_ft;
            aftArea = 0.5 * pi * avgRadius * aftLength_ft;
            aftCentroidX = aftLength_ft * 0.42;
         }
         break;
      }
      case GeometryBody::AftShapeType::cOGIVE:
      {
         if (mAftShapeLength_ft > epsilon)
         {
            aftLength_ft = mAftShapeLength_ft;

            // We assume the area and centroid of the ogive is roughly between a circle and cone
            aftArea = 0.5 * ((0.5 * pi * avgRadius * aftLength_ft) + (aftLength_ft * avgRadius));
            aftCentroidX = aftLength_ft * 0.5 * (0.42 + 0.33333);
         }
         break;
      }
      case GeometryBody::AftShapeType::cCONE:
      {
         if (mAftShapeLength_ft > epsilon)
         {
            aftLength_ft = mAftShapeLength_ft;
            aftArea = aftLength_ft * avgRadius;
            aftCentroidX = aftLength_ft * 0.33333;
         }
         break;
      }
      case GeometryBody::AftShapeType::cBOATTAIL:
      {
         if (mAftShapeLength_ft > epsilon)
         {
            aftLength_ft = mAftShapeLength_ft;
            aftArea      = 0.5 * aftLength_ft * (avgDiam + mAftShapeDiam_ft);
            aftCentroidX = aftLength_ft * (2.0 * mAftShapeDiam_ft + avgDiam) / (3.0 * (mAftShapeDiam_ft + avgDiam));
         }
         break;
      }
      default: // Includes cBLUNT
      {
         // Do nothing
         break;
      }
   }

   // Now, we deal with the mid section, with Mach effects
   double midLength = mLength_ft - foreLength_ft - aftLength_ft;
   midCentroidX = 0.5 * midLength;
   midArea = avgDiam * midLength;

   // Next, we adjust the centroidX values
   double mostForwardX = mLength_ft * 0.5;
   double mostAftX     = -mLength_ft * 0.5;
   foreCentroidX = (mostForwardX - foreLength_ft) + foreCentroidX;
   midCentroidX  = (mostForwardX - foreLength_ft) - midCentroidX;
   aftCentroidX  = (mostAftX + aftLength_ft) - aftCentroidX;

   double denominator = (foreArea + midArea + aftArea);
   double xOffset_ft  = 0.0;
   if (denominator > 0.0)
   {
      xOffset_ft = ((foreArea * foreCentroidX) + (midArea * midCentroidX) + (aftArea * aftCentroidX)) / denominator;
   }

   UtVec3dX foreAftAdjustment(xOffset_ft, 0.0, 0.0);
   return mRefPoint_ft + foreAftAdjustment;
}

void AeroBody::CalcLiftVector(double    aAlpha_rad,
                              double    aBeta_rad,
                              double    aDynamicPressure_lbssqft,
                              UtVec3dX& aLiftVector_lbs)
{
   double CL       = CalcLiftCoefficient(aAlpha_rad, aBeta_rad);
   double lift_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CL;
   aLiftVector_lbs = lift_lbs * mLiftForceUnitVector;
}

void AeroBody::CalcDragVector(double    aAlpha_rad,
                              double    aBeta_rad,
                              double    aDynamicPressure_lbssqft,
                              double    aMach,
                              UtVec3dX& aDragVector_lbs)
{
   double CD       = CalcDragCoefficient(aAlpha_rad, aBeta_rad, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

void AeroBody::CalcSideForceVector(double    aAlpha_rad,
                                   double    aBeta_rad,
                                   double    aDynamicPressure_lbssqft,
                                   UtVec3dX& aSideForceVector_lbs)
{
   double CY            = CalcSideForceCoefficient(aAlpha_rad, aBeta_rad);
   double sideForce_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CY;
   aSideForceVector_lbs = sideForce_lbs * mSideForceUnitVector;
}

void AeroBody::CalcMomentVector(double    aAlpha_rad,
                                double    aBeta_rad,
                                double    aDynamicPressure_lbssqft,
                                UtVec3dX& aMoment_ftlbs)
{
   double Cm           = CalcZeroLiftPitchingMoment(aAlpha_rad, aBeta_rad);
   double moment_ftlbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * Cm;

   // Assume that pitching moment is about the side force vector
   aMoment_ftlbs = moment_ftlbs * mSideForceUnitVector;
}

double AeroBody::CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad)
{
   double betaFactor = std::cos(std::abs(aBeta_rad));
   return std::sin(2.0 * aAlpha_rad) * betaFactor;
}

double AeroBody::CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach)
{
   // Drag due to alpha and beta
   // sin^2(alpha)*(cos^2(beta) + 1) + sin^2(beta)*(cos^2(alpha) + 1)
   // The drag coefficient should be the same for all values of alpha when beta = 90 deg
   // When beta = 90 deg, the expression above reduces to sin^2(alpha) + cos^2(alpha) + 1,
   // which is a constant value. This expression is also symmetrical in alpha/beta.
   double alphaBetaDrag = std::pow(std::sin(aAlpha_rad), 2) * (std::pow(std::cos(aBeta_rad),  2) + 1)
                          + std::pow(std::sin(aBeta_rad),  2) * (std::pow(std::cos(aAlpha_rad), 2) + 1);

   // Drag induced by lift
   double liftCoeff   = CalcLiftCoefficient(aAlpha_rad, aBeta_rad);
   double inducedDragFromLift = liftCoeff * liftCoeff / UtMath::cPI; // assume AR = 1

   // Drag induced by side force
   double sideCoeff   = CalcSideForceCoefficient(aAlpha_rad, aBeta_rad);
   double inducedDragFromSide = sideCoeff * sideCoeff / UtMath::cPI; // assume AR = 1

   // Drag due to skin friction
   double Cf           = CalcSkinFrictionCoefficient(aMach);
   double skinFriction = Cf * mSurfaceArea_ft2 / mCrossSectionalArea_sqft;

   // Drag due to the forward shape and mach
   double shapeDrag = CalcShapeDragCoefficient(aMach); // takes into account transonic effects

   return cDragMultiplier * (shapeDrag + alphaBetaDrag + inducedDragFromLift + inducedDragFromSide + skinFriction);
}

void Designer::AeroBody::CalcGeometryDCM()
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM yawDCM(-mYaw_rad, 0, 0);
   UtDCM pitchDCM(0, -mPitch_rad, 0);
   UtDCM rollDCM(0, 0, -mRoll_rad);

   mGeometryDCM = yawDCM * pitchDCM * rollDCM * axesDCM;
}

void Designer::AeroBody::GenerateDragTable()
{
   double subsonicScaleFactor   = GetPeakDragCoeff() / GetSubsonicDragCoeff(); // 3
   double supersonicScaleFactor = GetSupersonicDragCoeff() / GetSubsonicDragCoeff(); //2
   double subsonicMult          = subsonicScaleFactor - 1;
   double supersonicMult        = subsonicScaleFactor - supersonicScaleFactor;
   double subsonicBase          = 10;
   double supersonicBase        = 10;
   double subsonicExp           = (GetRiseFactor() * -29) - 1; // [-1, -30]
   double supersonicExp         = (GetDropFactor() * -29) - 1; // [-1, -30]
   double subsonicMachOffset    = 0.9;
   double supersonicMachOffset  = 1.1;

   std::vector<std::pair<double, double>> machMultiplierVector;

   for (double m = 0; m <= mMaxTableMach + 0.01; m = m + 0.01)
   {
      double subsonicVal   = (CalcSubsonicNoseFinenessFactor() * subsonicMult) / (1 + std::pow(subsonicBase, subsonicExp * (m - subsonicMachOffset)));
      double supersonicVal = subsonicMult - ((CalcSupersonicNoseFinenessFactor() * supersonicMult) / (1 + std::pow(supersonicBase, supersonicExp * (m - supersonicMachOffset))));

      machMultiplierVector.emplace_back(m, subsonicVal * supersonicVal);
   }

   // Go through the table and find the peak value and the mach at which it occurs
   double maxVal       = 0.0;
   double machAtMaxVal = 0.0;
   for (const auto& m : machMultiplierVector)
   {
      if (m.second > maxVal)
      {
         maxVal = m.second;
         machAtMaxVal = m.first;
      }
   }

   // Find how much we need to shift the curve vertically and horizontally
   double multiplier = subsonicMult / maxVal; // Global multiplier
   double shift      = GetPeakMach() - machAtMaxVal; // Global shift

   // Calculate Cd
   for (const auto& m : machMultiplierVector)
   {
      double mach       = m.first + shift;
      double dragFactor = 1 + (m.second * multiplier);
      double dragCoeff  = GetSubsonicDragCoeff() * dragFactor * CalcBodyFinenessFactor();

      mMachDragCoeffMap.emplace(mach, dragCoeff);
   }
}

double AeroBody::CalcSkinFrictionCoefficient(double aMach)
{
   double reynoldsNumber = aMach * cSpeedOfSound * mLength_ft / cKinematicViscosity;

   // Surface roughness
   // Source: "The Practical Calculation of the Aerodynamic Characteristics of Slender Finned Vehicles", NASA 1967
   double surfaceRoughnessHeight = 0.001; // In inches. Used "paint in aircraft-mass production" from above source, page 46.
   double length                 = 12 * mLength_ft; // Length in inches;
   double roughnessRatio         = surfaceRoughnessHeight / length;

   double ReCrit = 51 * std::pow(roughnessRatio, -1.039);

   if (reynoldsNumber < ReCrit)
   {
      if (reynoldsNumber < 5e5)
      {
         return 1.328 / std::sqrt(reynoldsNumber); // Laminar flow
      }
      return 1 / std::pow(3.46 * std::log10(reynoldsNumber) - 5.6, 2); // Turbulent flow
   }

   // Reynolds number is very high, skin friction not a function of Reynolds number.
   return 0.032 * std::pow(roughnessRatio, 0.2);

}

double Designer::AeroBody::CalcShapeDragCoefficient(double aMach)
{
   auto it = mMachDragCoeffMap.lower_bound(aMach);
   if (it != mMachDragCoeffMap.end())
   {
      if (it->first == aMach)
      {
         return it->second;
      }

      auto it_prev = std::prev(it, 1);

      // Interpolation:
      double mach0 = it_prev->first;
      double mach1 = it->first;

      double cd0 = it_prev->second;
      double cd1 = it->second;

      double slope = (cd1 - cd0) / (mach1 - mach0);

      return cd0 + slope * (aMach - mach0);
   }
   return mMachDragCoeffMap.rbegin()->second;
}

double Designer::AeroBody::CalcBodyFinenessFactor()
{
   return 1 + (0.02 * std::pow(mFinenessRatio - 2, 2) / std::sqrt(mFinenessRatio));
}

double Designer::AeroBody::CalcSubsonicNoseFinenessFactor()
{
   return std::pow(mNoseFinenessRatio + 0.5, -0.1);
}

double Designer::AeroBody::CalcSupersonicNoseFinenessFactor()
{
   return 0.5 + 0.25 * std::sqrt(mNoseFinenessRatio);
}

double AeroBody::CalcSideForceCoefficient(double aAlpha_rad, double aBeta_rad)
{
   double alphaFactor = cos(std::abs(aAlpha_rad));
   // Note: A positive beta results in a negative side force
   return -std::sin(2 * aBeta_rad) * alphaFactor;
}

double AeroBody::CalcZeroLiftPitchingMoment(double, //aAlpha_rad,
                                            double) //aBeta_rad)
{
   return 0.0;
}

void AeroBody::SetData(const std::string&             aName,
                       UtVec3dX                       aRefPoint_ft,
                       double                         aLength_ft,
                       double                         aHeight_ft,
                       double                         aWidth_ft,
                       double                         aArea_ft2,
                       double                         aYaw_rad,
                       double                         aPitch_rad,
                       double                         aRoll_rad,
                       double                         aNoseLength_ft,
                       GeometryBody::ForwardShapeType aForwardShapeType,
                       double                         aAftLength_ft,
                       double                         aAftDiam_ft,
                       GeometryBody::AftShapeType     aAftShapeType,
                       double                         aSurfaceArea_ft2)
{
   mName        = aName;
   mRefPoint_ft = aRefPoint_ft;
   mLength_ft   = aLength_ft;
   mHeight_ft   = aHeight_ft;
   mWidth_ft    = aWidth_ft;
   mYaw_rad     = aYaw_rad;
   mPitch_rad   = aPitch_rad;
   mRoll_rad    = aRoll_rad;

   mDCM.Set(aYaw_rad, aPitch_rad, aRoll_rad);

   mCrossSectionalArea_sqft = aArea_ft2;
   mNoseLength_ft           = aNoseLength_ft;
   mAftShapeLength_ft       = aAftLength_ft;
   mAftShapeDiam_ft         = aAftDiam_ft;
   mForwardShapeType        = aForwardShapeType;
   mAftShapeType            = aAftShapeType;
   mSurfaceArea_ft2         = aSurfaceArea_ft2;
   mEffectiveRadius_ft      = sqrt(mCrossSectionalArea_sqft / UtMath::cPI);

   double effectiveDiam_ft  = mEffectiveRadius_ft * 2.0;
   mFinenessRatio           = mLength_ft / effectiveDiam_ft;
   mNoseFinenessRatio       = mNoseLength_ft / effectiveDiam_ft;

   if (mNoseLength_ft < std::numeric_limits<double>::epsilon())
   {
      mConeRelativeTotalAngle_deg = 180.0;
   }
   else
   {
      mConeRelativeTotalAngle_deg = 2.0 * atan(mEffectiveRadius_ft / mNoseLength_ft) * UtMath::cDEG_PER_RAD;
   }

   GenerateDragTable();
}

} // namespace Designer
