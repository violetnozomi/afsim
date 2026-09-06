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

#include "GeometrySurfaceInertia.hpp"

#include "GeometrySurface.hpp"

#include "UtLog.hpp"

namespace Designer
{

GeometrySurfaceInertia::GeometrySurfaceInertia()
{
}

GeometrySurfaceInertia::~GeometrySurfaceInertia()
{
}

void GeometrySurfaceInertia::SetParent(GeometrySurface* aParent)
{
   mParent = aParent;
}

void GeometrySurfaceInertia::CalculateRotationalInertia(double   aSpan_ft,
                                                        double   aArea_sqft,
                                                        double   aMass_slugs,
                                                        UtVec3dX aRefPoint_ft,
                                                        double&  aCalculatedIxx,
                                                        double&  aCalculatedIyy,
                                                        double&  aCalculatedIzz)
{
   // Zero the calculated/integrated inertias
   aCalculatedIxx = 0.0;
   aCalculatedIyy = 0.0;
   aCalculatedIzz = 0.0;

   // Clear any previous data
   for (auto& outer : mRectSet)
   {
      outer.clear();
   }
   mRectSet.clear();

   if (mParent == nullptr)
   {
      ut::log::error() << "No parent in GeometrySurfaceInertia::CalculateRotationalInertia().";
      return;
   }

   // Get parent data
   double  rootChord_ft        = mParent->GetRootChord_ft();
   double  tipChord_ft         = mParent->GetTipChord_ft();
   double  sweepAngle_rad      = mParent->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;
   double  dihedralAngle_rad   = mParent->GetDihedralAngle_deg() * UtMath::cRAD_PER_DEG;
   double  incidenceAngle_rad  = mParent->GetIncidenceAngle_deg() * UtMath::cRAD_PER_DEG;
   double  thicknessRatio      = mParent->GetThicknessRatio();

   UtVec3dX vec1(0.0, 0.0, 0.0);
   UtVec3dX vec2(0.0, 0.0, 0.0);
   UtVec3dX vec3(0.0, 0.0, 0.0);
   UtVec3dX vec4(0.0, 0.0, 0.0);

   double sweepBack = -aSpan_ft * tan(sweepAngle_rad);
   vec1.Set(vec1.X() + rootChord_ft * 0.25, vec1.Y(), vec1.Z());
   vec2.Set(vec2.X() - rootChord_ft * 0.75, vec2.Y(), vec2.Z());
   vec3.Set(vec3.X() + sweepBack - tipChord_ft * 0.75, vec3.Y() + aSpan_ft, vec3.Z());
   vec4.Set(vec4.X() + sweepBack + tipChord_ft * 0.25, vec4.Y() + aSpan_ft, vec4.Z());

   UtDCM dihedralDCM(0.0, 0.0, -dihedralAngle_rad);
   UtDCM incidenceDCM(0.0, incidenceAngle_rad, 0.0);
   UtDCM attitudeDCM = incidenceDCM * dihedralDCM;

   UtVec3dX transformedPt1 = attitudeDCM.InverseTransform(vec1);
   UtVec3dX transformedPt2 = attitudeDCM.InverseTransform(vec2);
   UtVec3dX transformedPt3 = attitudeDCM.InverseTransform(vec3);
   UtVec3dX transformedPt4 = attitudeDCM.InverseTransform(vec4);

   mRootFrontPt = transformedPt1 + aRefPoint_ft;
   mRootRearPt  = transformedPt2 + aRefPoint_ft;
   mTipRearPt   = transformedPt3 + aRefPoint_ft;
   mTipFrontPt  = transformedPt4 + aRefPoint_ft;

   // Set the number of points along the span -- minimum of 20
   int numSpanPts = static_cast<double>(aSpan_ft * 5.0);
   if (numSpanPts < 20)
   {
      numSpanPts = 20;
   }

   // Chord Slices
   double sliceWidth = 1.0 / static_cast<double>(numSpanPts);
   double integratedArea_sqft = 0.0;
   for (int iSpanPt = 0; iSpanPt < numSpanPts; ++iSpanPt)
   {
      double spanFractionRootSide = static_cast<double>(iSpanPt) / static_cast<double>(numSpanPts);
      double spanFractionTipSide = static_cast<double>(iSpanPt + 1) / static_cast<double>(numSpanPts);
      double spanFraction = 0.5 * (spanFractionRootSide + spanFractionTipSide);

      double pt1_y = spanFractionRootSide * aSpan_ft;
      double pt2_y = pt1_y;
      double pt3_y = spanFractionTipSide * aSpan_ft;
      double pt4_y = pt3_y;
      double ptCenter_y = spanFraction * aSpan_ft;

      double chordFractionFrontSide = 0.0;
      double chordFractionRearSide = 1.0;
      double chordFraction = 0.5;

      double deltaChord = tipChord_ft - rootChord_ft;
      double chordRootSide = rootChord_ft + deltaChord * spanFractionRootSide;
      double chordTipSide = rootChord_ft + deltaChord * spanFractionTipSide;
      double chordCenter = rootChord_ft + deltaChord * spanFraction;

      double sweepBackQuarterChordRootSide = tan(sweepAngle_rad) * -spanFractionRootSide * aSpan_ft;
      double sweepBackQuarterChordTipSide = tan(sweepAngle_rad) * -spanFractionTipSide * aSpan_ft;
      double sweepBackQuarterChord = tan(sweepAngle_rad) * -spanFraction * aSpan_ft;

      double pt1_x = (0.25 * chordRootSide) - (chordRootSide * chordFractionFrontSide) + sweepBackQuarterChordRootSide;
      double pt2_x = (0.25 * chordRootSide) - (chordRootSide * chordFractionRearSide) + sweepBackQuarterChordRootSide;
      double pt3_x = (0.25 * chordTipSide) - (chordTipSide * chordFractionRearSide) + sweepBackQuarterChordTipSide;
      double pt4_x = (0.25 * chordTipSide) - (chordTipSide * chordFractionFrontSide) + sweepBackQuarterChordTipSide;
      double ptCenter_x = (0.25 * chordCenter) - (chordCenter * chordFraction) + sweepBackQuarterChord;

      double pt1_z = 0;
      double pt2_z = 0;
      double pt3_z = 0;
      double pt4_z = 0;
      double ptCenter_z = 0;

      double tempFrontSide = 0.5 * (pt1_x + pt4_x);
      double tempRearSide = 0.5 * (pt2_x + pt3_x);
      double tempHalfChord = 0.5 * (tempFrontSide - tempRearSide);

      pt1_x = tempFrontSide;
      pt2_x = tempRearSide;
      pt3_x = tempRearSide;
      pt4_x = tempFrontSide;

      UtVec3dX v1(pt1_x, pt1_y, pt1_z);
      UtVec3dX v2(pt2_x, pt2_y, pt2_z);
      UtVec3dX v3(pt3_x, pt3_y, pt3_z);
      UtVec3dX v4(pt4_x, pt4_y, pt4_z);
      UtVec3dX vecCenter(ptCenter_x, ptCenter_y, ptCenter_z);

      UtVec3dX tempCenterPt = attitudeDCM.InverseTransform(vecCenter);

      UtVec3dX myPt1(tempHalfChord, sliceWidth * aSpan_ft * -0.5, 0.0);
      UtVec3dX myPt2(-tempHalfChord, sliceWidth * aSpan_ft * -0.5, 0.0);
      UtVec3dX myPt3(-tempHalfChord, sliceWidth * aSpan_ft * 0.5, 0.0);
      UtVec3dX myPt4(tempHalfChord, sliceWidth * aSpan_ft * 0.5, 0.0);

      UtVec3dX rectPt1 = attitudeDCM.InverseTransform(myPt1);
      UtVec3dX rectPt2 = attitudeDCM.InverseTransform(myPt2);
      UtVec3dX rectPt3 = attitudeDCM.InverseTransform(myPt3);
      UtVec3dX rectPt4 = attitudeDCM.InverseTransform(myPt4);

      rectPt1 += tempCenterPt + aRefPoint_ft;
      rectPt2 += tempCenterPt + aRefPoint_ft;
      rectPt3 += tempCenterPt + aRefPoint_ft;
      rectPt4 += tempCenterPt + aRefPoint_ft;

      UtVec3dX centerPt = tempCenterPt + aRefPoint_ft;

      // Determine the size of the elemental inertia strip
      double deltaLength = tempFrontSide - tempRearSide;
      double deltaWidth = pt3_y - pt1_y;
      double deltaThickness = thicknessRatio * deltaLength;
      double currentArea_sqft = deltaLength * deltaWidth;
      double massFraction = currentArea_sqft / aArea_sqft;

      // Integrate the area (used to confirm proper calculations)
      integratedArea_sqft += currentArea_sqft;

      double deltaLengthSquared = deltaLength * deltaLength;
      double deltaWidthSquared = deltaWidth * deltaWidth;
      double deltaThicknessSquared = deltaThickness * deltaThickness;

      // Calculate the rotational inertia values when the element is at the mass centroid
      double oneTwelthMass = (aMass_slugs * massFraction) / 12.0;
      double ixx = oneTwelthMass * (deltaWidthSquared + deltaThicknessSquared);
      double iyy = oneTwelthMass * (deltaLengthSquared + deltaThicknessSquared);
      double izz = oneTwelthMass * (deltaLengthSquared + deltaWidthSquared);

      // Modify inertia for attitude
      UtVec3dX inertiaVector(ixx, iyy, izz);
      UtVec3dX transformedInertiaiVector = attitudeDCM.InverseTransform(inertiaVector);

      // The current rotational inertia values act at the mass centroid. Now, let's correct
      // for location, using parallel axis theorem
      double distX = sqrt(centerPt.Y() * centerPt.Y() + centerPt.Z() * centerPt.Z());
      double distY = sqrt(centerPt.X() * centerPt.X() + centerPt.Z() * centerPt.Z());
      double distZ = sqrt(centerPt.X() * centerPt.X() + centerPt.Y() * centerPt.Y());
      double dX2 = distX * distX;
      double dY2 = distY * distY;
      double dZ2 = distZ * distZ;

      double fractionalMass = massFraction * aMass_slugs;
      UtVec3dX parallelAxisVector(fractionalMass * dX2, fractionalMass * dY2, fractionalMass * dZ2);
      transformedInertiaiVector += parallelAxisVector;

      aCalculatedIxx += transformedInertiaiVector.X();
      aCalculatedIyy += transformedInertiaiVector.Y();
      aCalculatedIzz += transformedInertiaiVector.Z();

      std::vector<UtVec3dX> myVector;

      myVector.push_back(rectPt1);
      myVector.push_back(rectPt2);
      myVector.push_back(rectPt3);
      myVector.push_back(rectPt4);

      mRectSet.push_back(myVector);
   }

   // Confirm that area is approximately as expected
   double areaFraction = integratedArea_sqft / aArea_sqft;
   if ((areaFraction < 0.99) || (areaFraction > 1.01))
   {
      auto outStream = ut::log::error() << "Integrated area is not as expected.";
      outStream.AddNote() << "integratedArea_sqft: " << integratedArea_sqft;
      outStream.AddNote() << "aArea_sqft: " << aArea_sqft;
   }

   // Show that the object has been initialized
   mDataInitialized = true;
}

bool GeometrySurfaceInertia::GetInertiaCompositionData(UtVec3dX&                           aRootFrontPt,
                                                       UtVec3dX&                           aRootRearPt,
                                                       UtVec3dX&                           aTipRearPt,
                                                       UtVec3dX&                           aTipFrontPt,
                                                       std::vector<std::vector<UtVec3dX>>& aRectSet) const
{
   if (!mDataInitialized)
   {
      return false;
   }

   aRootFrontPt = mRootFrontPt;
   aRootRearPt  = mRootRearPt;
   aTipRearPt   = mTipRearPt;
   aTipFrontPt  = mTipFrontPt;
   aRectSet     = mRectSet;
   return true;
}

} // namespace Designer
