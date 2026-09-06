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

#ifndef AERO_BODY_HPP
#define AERO_BODY_HPP

#include <string>
#include <unordered_map>

#include "AeroObject.hpp"
#include "GeometryBody.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// AeroBody is derived from AeroObject and provides aerodynamics for
// any body of revolution such as a fuselage or missile body.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroBody : public AeroObject
   {
      public:
         AeroBody(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroBody() = default;

         // Calculate F&M using alpha, beta, q, and Mach.
         // SetVehicleAlphaBeta should be called prior to calling this function.
         void CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                   double    aMach,
                                   UtVec3dX& aLiftVector_lbs,
                                   UtVec3dX& aDragVector_lbs,
                                   UtVec3dX& aSideVector_lbs,
                                   UtVec3dX& aMoment_ftlbs,
                                   UtVec3dX& aForcePoint_ft) override;

         // This returns the location where aero forces are acting (as a function of Mach)
         UtVec3dX CalcAeroForcePoint_ft(double aMach = 0.0) const override;

         double GetLength_ft() { return mLength_ft; }
         double GetHeight_ft() { return mHeight_ft; }
         double GetWidth_ft() { return mWidth_ft; }
         double GetCrossSectionalArea_sqft() { return mCrossSectionalArea_sqft; }

         double GetYawAngle_deg() { return mYaw_rad * UtMath::cDEG_PER_RAD; }
         double GetPitchAngle_deg() { return mPitch_rad * UtMath::cDEG_PER_RAD; }
         double GetRollAngle_deg() { return mRoll_rad * UtMath::cDEG_PER_RAD; }

         void SetYawAngle(double aYaw_deg) { mYaw_rad = aYaw_deg * UtMath::cRAD_PER_DEG; }
         void SetPitchAngle(double aPitch_deg) { mPitch_rad = aPitch_deg * UtMath::cRAD_PER_DEG; }
         void SetRollAngle(double aRoll_deg) { mRoll_rad = aRoll_deg * UtMath::cRAD_PER_DEG; }

         void SetData(const std::string&             aName,
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
                      double                         aSurfaceArea_ft2);

      protected:

         void CalcLiftVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             UtVec3dX& aLiftVector_lbs);

         void CalcDragVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aDragVector_lbs);

         void CalcSideForceVector(double    aAlpha_rad,
                                  double    aBeta_rad,
                                  double    aDynamicPressure_lbssqft,
                                  UtVec3dX& aSideForceVector_lbs);

         void CalcMomentVector(double    aAlpha_rad,
                               double    aBeta_rad,
                               double    aDynamicPressure_lbssqft,
                               UtVec3dX& aMoment_ftlbs);

         double CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad);
         double CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach);
         double CalcSideForceCoefficient(double aAlpha_rad, double aBeta_rad);
         double CalcZeroLiftPitchingMoment(double aAlpha_rad, double aBeta_rad);

         void CalcGeometryDCM() override;

         void GenerateDragTable();

         double      mLength_ft                  = 0.0;
         double      mHeight_ft                  = 0.0;
         double      mWidth_ft                   = 0.0;
         double      mYaw_rad                    = 0.0;
         double      mPitch_rad                  = 0.0;
         double      mRoll_rad                   = 0.0;
         double      mCrossSectionalArea_sqft    = 0.0;
         double      mEffectiveRadius_ft         = 0.0;
         double      mFinenessRatio              = 0.0;
         double      mNoseLength_ft              = 0.0;
         double      mNoseFinenessRatio          = 0.0;
         double      mConeRelativeTotalAngle_deg = 0.0;
         double      mAftShapeLength_ft          = 0.0;
         double      mAftShapeDiam_ft            = 0.0;
         double      mMaxTableMach               = 0.0;

         // Forward shapes include Cone, Ogive, Round, Blunt
         GeometryBody::ForwardShapeType mForwardShapeType = GeometryBody::ForwardShapeType::cOGIVE;

         // Aft shapes include Cone, Ogive, Round, BoatTail, Blunt
         GeometryBody::AftShapeType mAftShapeType = GeometryBody::AftShapeType::cBLUNT;

         static const double cDragMultiplier;

      private:
         struct FowardShapeConstants
         {
            std::unordered_map<GeometryBody::ForwardShapeType, double> mSubsonicDragCoeff; // Base drag coefficient at Mach = 0
            std::unordered_map<GeometryBody::ForwardShapeType, double> mPeakDragCoeff; // Drag coefficient at Mach = 1
            std::unordered_map<GeometryBody::ForwardShapeType, double> mSupersonicDragCoeff; // Drag coefficient at ~ Mach > 1.5
            std::unordered_map<GeometryBody::ForwardShapeType, double> mPeakMach; // Mach at which peak occurs
            std::unordered_map<GeometryBody::ForwardShapeType, double> mRiseFactor; // [0 - 1] How quickly the drag rises up to Mach 1
            std::unordered_map<GeometryBody::ForwardShapeType, double> mDropFactor; // [0 - 1] How quickly the drag drops after Mach 1
         };

         double GetSubsonicDragCoeff()   { return mForwardShapeConstants.mSubsonicDragCoeff.at(mForwardShapeType); }
         double GetPeakDragCoeff()       { return mForwardShapeConstants.mPeakDragCoeff.at(mForwardShapeType); }
         double GetSupersonicDragCoeff() { return mForwardShapeConstants.mSupersonicDragCoeff.at(mForwardShapeType); }
         double GetPeakMach()            { return mForwardShapeConstants.mPeakMach.at(mForwardShapeType); }
         double GetRiseFactor()          { return mForwardShapeConstants.mRiseFactor.at(mForwardShapeType); }
         double GetDropFactor()          { return mForwardShapeConstants.mDropFactor.at(mForwardShapeType); }

         double CalcSkinFrictionCoefficient(double aMach);
         double CalcShapeDragCoefficient(double aMach);
         double CalcBodyFinenessFactor();
         double CalcSubsonicNoseFinenessFactor();
         double CalcSupersonicNoseFinenessFactor();

         FowardShapeConstants mForwardShapeConstants;
         std::map<double, double> mMachDragCoeffMap;
   };
}
#endif // ! AERO_BODY_HPP
