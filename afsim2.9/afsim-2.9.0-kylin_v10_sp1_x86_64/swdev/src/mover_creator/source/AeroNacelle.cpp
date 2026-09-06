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

#include "AeroNacelle.hpp"

#include "P6DofUtils.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace Designer
{

AeroNacelle::AeroNacelle(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroBody(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroBody type
   mAeroTypeString.assign("AeroNacelle");
}

void AeroNacelle::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
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
      ut::log::error() << "Non-positive cross sectional area in AeroNacelle::CalcForcesAndMoments().";
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

UtVec3dX AeroNacelle::CalcAeroForcePoint_ft(double aMach) const
{
   // Determine the fore/aft location - not flat
   // Aft section length (not flat)

   if (mOverallShapeType == "Rounded")
   {
      // Adjust fore/aft for tapered
      if (mAftSectionType == "Tapered")
      {
         if (mAftSectionLength_ft <= 0.0) { return mRefPoint_ft; }         // No fore/aft effects - just return the ref pt
         if (mAftSectionLength_ft > mLength_ft) { return mRefPoint_ft; }   // Error condition, so ignore fore/aft effects - just return the ref pt

         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         UtVec3dX foreAftAdjustment(xOffset_ft, 0.0, 0.0);
         return mRefPoint_ft + foreAftAdjustment;
      }
      else
      {
         // Aft section is blunt, so no fore/aft adjustment
         return mRefPoint_ft;
      }

      // Default is simply the ref pt
      return mRefPoint_ft;
   }
   else if (mOverallShapeType == "Half-Round-Right")
   {
      double width_ft = mWidth_ft;
      double r = width_ft * 0.5;
      double distFromCenter_ft = 4.0 * r / (3.0 * UtMath::cPI);
      UtVec3dX vector(0.0, distFromCenter_ft, 0.0);

      // Adjust fore/aft for tapered
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);
      if (mAftSectionType == "Tapered")
      {
         if ((mAftSectionLength_ft <= 0.0) || (mAftSectionLength_ft > mLength_ft))
         {
            // Error condition -- do not use any fore/aft effects
         }

         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }
   else if (mOverallShapeType == "Half-Round-Left")
   {
      double width_ft = mWidth_ft;
      double r = width_ft * 0.5;
      double distFromCenter_ft = 4.0 * r / (3.0 * UtMath::cPI);

      UtVec3dX vector(0.0, -distFromCenter_ft, 0.0);

      // Adjust fore/aft for tapered
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);
      if (mAftSectionType == "Tapered")
      {
         if ((mAftSectionLength_ft <= 0.0) || (mAftSectionLength_ft > mLength_ft))
         {
            // Error condition -- do not use any fore/aft effects
         }

         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }
   else if (mOverallShapeType == "Half-Round-Top")
   {
      double r = mHeight_ft * 0.5;
      double distFromCenter_ft = 4.0 * r / (3.0 * UtMath::cPI);

      UtVec3dX vector(0.0, 0.0, -distFromCenter_ft);

      // Adjust fore/aft for tapered
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);
      if (mAftSectionType == "Tapered")
      {
         if ((mAftSectionLength_ft <= 0.0) || (mAftSectionLength_ft > mLength_ft))
         {
            // Error condition -- do not use any fore/aft effects
         }

         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }
   else if (mOverallShapeType == "Half-Round-Bottom")
   {
      double r = mHeight_ft * 0.5;
      double distFromCenter_ft = 4.0 * r / (3.0 * UtMath::cPI);

      UtVec3dX vector(0.0, 0.0, distFromCenter_ft);

      // Adjust fore/aft for tapered
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);
      if (mAftSectionType == "Tapered")
      {
         if ((mAftSectionLength_ft <= 0.0) || (mAftSectionLength_ft > mLength_ft))
         {
            // Error condition -- do not use any fore/aft effects
         }

         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }
   else if (mOverallShapeType == "Flat-Sided")
   {
      // Flat-sided lack a tapered aft section
      return mRefPoint_ft;
   }
   else if (mOverallShapeType == "Flat-Swept-Right")
   {
      double h = mWidth_ft;
      double a = mHeightOuter_ft;
      double b = mHeightInner_ft;
      double distFromCenter_ft = (h / 3.0) * (((2.0 * a) + b) / (a + b));

      UtVec3dX vector(0.0, distFromCenter_ft, 0.0);

      // Adjust fore/aft for tapered
      bool aftEffectsOnly  = false;
      bool foreEffectsOnly = false;
      bool bothEffects     = false;
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);

      if (mAftSectionType == "Tapered")
      {
         // Both forward and aft effects

         if (mAftSectionLength_ft <= 0.0)
         {
            // No aft effects, check for fore effects
            if (mForwardSweepLength_ft > 0.0)
            {
               foreEffectsOnly = true;
            }
         }
         else if ((mForwardSweepLength_ft + mAftSectionLength_ft) > mLength_ft)
         {
            // Error condition - neither effect
         }
         else if (mForwardSweepLength_ft <= 0.0)
         {
            // No aft effects, check for fore effects
            if (mAftSectionLength_ft <= mLength_ft)
            {
               aftEffectsOnly = true;
            }
         }
         else
         {
            bothEffects = true;
         }
      }
      else
      {
         if (mForwardSweepLength_ft > 0.0)
         {
            foreEffectsOnly = true;
         }
         else
         {
            // Neither effect
         }
      }

      if (aftEffectsOnly)
      {
         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else if (foreEffectsOnly)
      {
         double baseLength_ft = mLength_ft - mForwardSweepLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double foreX_ft = halfLength_ft - mForwardSweepLength_ft * 0.6666667;
         double baseHalfX_ft = foreX_ft - baseLength_ft * 0.5;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double foreArea = 0.5 * avgHgt * mForwardSweepLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + foreArea * foreX_ft) / (baseArea + foreArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else if (bothEffects)
      {
         double baseLength_ft = mLength_ft - mAftSectionLength_ft - mForwardSweepLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double foreX_ft = halfLength_ft - mForwardSweepLength_ft * 0.6666667;
         double baseHalfX_ft = foreX_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double foreArea = 0.5 * avgHgt * mForwardSweepLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + foreArea * foreX_ft + aftArea * aftX_ft) / (baseArea + foreArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else
      {
         // No effects
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }
   else if (mOverallShapeType == "Flat-Swept-Left")
   {
      double h = mWidth_ft;
      double a = mHeightOuter_ft;
      double b = mHeightInner_ft;
      double distFromCenter_ft = (h / 3.0) * (((2.0 * a) + b) / (a + b));

      UtVec3dX vector(0.0, -distFromCenter_ft, 0.0);

      // Adjust fore/aft for tapered
      bool aftEffectsOnly = false;
      bool foreEffectsOnly = false;
      bool bothEffects = false;
      UtVec3dX foreAftAdjustment(0.0, 0.0, 0.0);

      if (mAftSectionType == "Tapered")
      {
         // Both forward and aft effects

         if (mAftSectionLength_ft <= 0.0)
         {
            // No aft effects, check for fore effects
            if (mForwardSweepLength_ft > 0.0)
            {
               foreEffectsOnly = true;
            }
         }
         else if ((mForwardSweepLength_ft + mAftSectionLength_ft) > mLength_ft)
         {
            // Error condition - neither effect
         }
         else if (mForwardSweepLength_ft <= 0.0)
         {
            // No aft effects, check for fore effects
            if (mAftSectionLength_ft <= mLength_ft)
            {
               aftEffectsOnly = true;
            }
         }
         else
         {
            bothEffects = true;
         }
      }
      else
      {
         if (mForwardSweepLength_ft > 0.0)
         {
            foreEffectsOnly = true;
         }
         else
         {
            // Neither effect
         }
      }

      if (aftEffectsOnly)
      {
         double baseLength_ft = mLength_ft - mAftSectionLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double baseHalfX_ft = halfLength_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + aftArea * aftX_ft) / (baseArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else if (foreEffectsOnly)
      {
         double baseLength_ft = mLength_ft - mForwardSweepLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double foreX_ft = halfLength_ft - mForwardSweepLength_ft * 0.6666667;
         double baseHalfX_ft = foreX_ft - baseLength_ft * 0.5;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double foreArea = 0.5 * avgHgt * mForwardSweepLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + foreArea * foreX_ft) / (baseArea + foreArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else if (bothEffects)
      {
         double baseLength_ft = mLength_ft - mAftSectionLength_ft - mForwardSweepLength_ft;
         double halfLength_ft = 0.5 * mLength_ft;
         double foreX_ft = halfLength_ft - mForwardSweepLength_ft * 0.6666667;
         double baseHalfX_ft = foreX_ft - baseLength_ft * 0.5;
         double aftX_ft = baseHalfX_ft - mAftSectionLength_ft / 3.0;
         double avgHgt = (mHeight_ft + mWidth_ft) * 0.5;
         double baseArea = avgHgt * baseLength_ft;
         double foreArea = 0.5 * avgHgt * mForwardSweepLength_ft;
         double aftArea = 0.5 * avgHgt * mAftSectionLength_ft;
         double xOffset_ft = (baseArea * baseHalfX_ft + foreArea * foreX_ft + aftArea * aftX_ft) / (baseArea + foreArea + aftArea);

         foreAftAdjustment.Set(xOffset_ft, 0.0, 0.0);
      }
      else
      {
         // No effects
      }

      return mRefPoint_ft + vector + foreAftAdjustment;
   }

   return mRefPoint_ft;
}

void AeroNacelle::SetData(const std::string& aName,
                          UtVec3dX           aRefPoint_ft,
                          double             aLength_ft,
                          double             aHeight_ft,
                          double             aHeightInner_ft,
                          double             aHeightOuter_ft,
                          double             aWidth_ft,
                          double             aThickness_ft,
                          double             aYaw_rad,
                          double             aPitch_rad,
                          double             aRoll_rad,
                          const std::string& aOverallShapeType,
                          double             aForwardSweepLength_ft,
                          const std::string& aAftSectionType,
                          double             aAftSectionLength_ft,
                          double             aSurfaceArea_ft2)
{
   mName = aName;

   mRefPoint_ft = aRefPoint_ft;

   mLength_ft = aLength_ft;
   mHeight_ft = aHeight_ft;
   mHeightInner_ft = aHeightInner_ft;
   mHeightOuter_ft = aHeightOuter_ft;
   mWidth_ft  = aWidth_ft;
   mThickness_ft = aThickness_ft;
   mYaw_rad   = aYaw_rad;
   mPitch_rad = aPitch_rad;
   mRoll_rad  = aRoll_rad;

   mDCM.Set(aYaw_rad, aPitch_rad, aRoll_rad);

   // Overall shapes include Rounded, Half-Round-Right, Half-Round-Left, Half-Round-Top,
   // Half-Round-Bottom, Flat-Sided, Flat-Swept-Right, and Flat-Swept-Left
   mOverallShapeType = aOverallShapeType;

   // Aft section types include Blunt and Tapered
   mAftSectionType = aAftSectionType;

   mForwardSweepLength_ft = aForwardSweepLength_ft;
   mAftSectionLength_ft = aAftSectionLength_ft;

   // Unused AeroBody vars
   mForwardShapeType = GeometryBody::ForwardShapeType::cBLUNT;
   mAftShapeType = GeometryBody::AftShapeType::cBLUNT;
   mNoseLength_ft = 0.0;
   mAftShapeLength_ft = 0.0;
   mAftShapeDiam_ft = 0.0;

   mSurfaceArea_ft2 = aSurfaceArea_ft2;

   // Note:  A possible future improvement would be to include an aero-center for each of these (below)

   // Calculate frontal area (cross-sectional area), depending on type of nacelle
   if (mOverallShapeType == "Rounded")
   {
      // Calculate frontal area, assuming an ellipse
      double outerArea_sqft = (mHeight_ft * 0.5) * (mWidth_ft * 0.5) * UtMath::cPI;
      double innerArea_sqft = ((mHeight_ft * 0.5) - mThickness_ft) * ((mWidth_ft * 0.5) - mThickness_ft) * UtMath::cPI;
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;
   }
   else if (mOverallShapeType == "Half-Round-Right")
   {
      // Calculate frontal area, assuming an ellipse
      double outerArea_sqft = (mHeight_ft * 0.5) * (mWidth_ft * 0.5) * UtMath::cPI;
      double innerArea_sqft = ((mHeight_ft * 0.5) - mThickness_ft) * ((mWidth_ft * 0.5) - mThickness_ft) * UtMath::cPI;
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;

      // This is a half-circle, so cut area in half
      mCrossSectionalArea_sqft *= 0.5;
   }
   else if (mOverallShapeType == "Half-Round-Left")
   {
      // Calculate frontal area, assuming an ellipse
      double outerArea_sqft = (mHeight_ft * 0.5) * (mWidth_ft * 0.5) * UtMath::cPI;
      double innerArea_sqft = ((mHeight_ft * 0.5) - mThickness_ft) * ((mWidth_ft * 0.5) - mThickness_ft) * UtMath::cPI;
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;

      // This is a half-circle, so cut area in half
      mCrossSectionalArea_sqft *= 0.5;
   }
   else if (mOverallShapeType == "Half-Round-Top")
   {
      // Calculate frontal area, assuming an ellipse
      double outerArea_sqft = (mHeight_ft * 0.5) * (mWidth_ft * 0.5) * UtMath::cPI;
      double innerArea_sqft = ((mHeight_ft * 0.5) - mThickness_ft) * ((mWidth_ft * 0.5) - mThickness_ft) * UtMath::cPI;
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;

      // This is a half-circle, so cut area in half
      mCrossSectionalArea_sqft *= 0.5;
   }
   else if (mOverallShapeType == "Half-Round-Bottom")
   {
      // Calculate frontal area, assuming an ellipse
      double outerArea_sqft = (mHeight_ft * 0.5) * (mWidth_ft * 0.5) * UtMath::cPI;
      double innerArea_sqft = ((mHeight_ft * 0.5) - mThickness_ft) * ((mWidth_ft * 0.5) - mThickness_ft) * UtMath::cPI;
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;

      // This is a half-circle, so cut area in half
      mCrossSectionalArea_sqft *= 0.5;
   }
   else if (mOverallShapeType == "Flat-Sided")
   {
      double outerArea_sqft = mHeight_ft * mWidth_ft;
      double innerArea_sqft = (mHeight_ft - mThickness_ft * 2.0) * (mWidth_ft - mThickness_ft * 2.0);
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;
   }
   else if (mOverallShapeType == "Flat-Swept-Right")
   {
      double outerArea_sqft = mHeight_ft * mWidth_ft;
      double innerArea_sqft = (mHeight_ft - mThickness_ft * 2.0) * (mWidth_ft - mThickness_ft * 2.0);
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;
   }
   else if (mOverallShapeType == "Flat-Swept-Left")
   {
      double outerArea_sqft = mHeight_ft * mWidth_ft;
      double innerArea_sqft = (mHeight_ft - mThickness_ft * 2.0) * (mWidth_ft - mThickness_ft * 2.0);
      mCrossSectionalArea_sqft = outerArea_sqft - innerArea_sqft;
   }

   mEffectiveRadius_ft = sqrt(mCrossSectionalArea_sqft / UtMath::cPI);

   double effectiveDiam_ft = mEffectiveRadius_ft * 2.0;
   mFinenessRatio = mLength_ft / effectiveDiam_ft;

   if (mForwardSweepLength_ft < std::numeric_limits<double>::epsilon())
   {
      mConeRelativeTotalAngle_deg = 180.0;
   }
   else
   {
      mConeRelativeTotalAngle_deg = 2.0 * atan(mEffectiveRadius_ft / mForwardSweepLength_ft) * UtMath::cDEG_PER_RAD;
   }

   GenerateDragTable();
}

} // namespace Designer
