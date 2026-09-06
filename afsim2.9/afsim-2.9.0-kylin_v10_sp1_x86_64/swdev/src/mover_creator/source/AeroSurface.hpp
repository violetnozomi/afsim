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

#ifndef AERO_SURFACE_HPP
#define AERO_SURFACE_HPP

#include <string>

#include "AeroData.hpp"
#include "AeroObject.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// AeroSurface is derived from AeroObject and provides aerodynamics for
// any surface such as stabilizers, tails, and fins. Wings are derived from
// surfaces and add additional capabilities.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroSurface : public AeroObject
   {
      public:
         AeroSurface(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroSurface() = default;


         // Returns a vector in the direction of quarter-chord sweep
         UtVec3dX GetSweepUnitVector() { return mSweepUnitVector; }

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

         virtual void CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg) override;

         // Provides the local alpha and beta angles.
         // SetVehicleAlphaBeta should be called prior to calling this function.
         double CalcSweep_deg() const;

         double GetSpan_ft() { return mSpan_ft; }
         double GetRootChord_ft() { return mRootChord_ft; }
         double GetTipChord_ft() { return mTipChord_ft; }
         double GetSweepAngle_deg() { return mSweepAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetDihedralAngle_deg() { return mDihedralAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetIncidenceAngle_deg() { return mIncidenceAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetThicknessRatio() { return mThicknessRatio; }
         double GetOswaldsEfficiency() { return  mOswaldsEfficiency; }
         virtual double GetAspectRatio() { return mAspectRatio; }
         virtual double GetPlanformArea_ft2() { return mArea_ft2; }

         bool InvertVectors() { return mInvertVectors; }

         void SetData(const std::string& aName,
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
                      double             aSurfaceArea_ft2);

         void SetAdditionalData(const std::string&    aAirfoilFilename,
                                bool                  aUseExponentialAngleMapping);

         // This should be overridden by derived classes, if appropriate
         bool IsMovable() const override { return false; }

         QStringList Audit() override;

      protected:

         void LoadAirfoil();

         virtual void CalcLiftVector(double    aAlpha_deg,
                                     double    aBeta_deg,
                                     double    aEffectiveSweep_deg,
                                     double    aDynamicPressure_lbssqft,
                                     double    aMach,
                                     UtVec3dX& aLiftVector_lbs);

         virtual void CalcDragVector(double    aAlpha_deg,
                                     double    aBeta_deg,
                                     double    aEffectiveSweep_deg,
                                     double    aDynamicPressure_lbssqft,
                                     double    aMach,
                                     UtVec3dX& aDragVector_lbs);

         virtual void CalcSideForceVector(double    aAlpha_deg,
                                          double    aBeta_deg,
                                          double    aEffectiveSweep_deg,
                                          double    aDynamicPressure_lbssqft,
                                          double    aMach,
                                          UtVec3dX& aSideForceVector_lbs);

         virtual void CalcMomentVector(double    aAlpha_deg,
                                       double    aBeta_deg,
                                       double    aEffectiveSweep_deg,
                                       double    aDynamicPressure_lbssqft,
                                       double    aMach,
                                       UtVec3dX& aLiftVector_lbs,
                                       UtVec3dX& aDragVector_lbs,
                                       UtVec3dX& aSideForceVector_lbs,
                                       UtVec3dX& aMoment_ftlbs);

         virtual double CalcLiftCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach);
         virtual double CalcDragCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach);
         virtual double CalcSideForceCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach);
         virtual double CalcZeroLiftPitchingMoment(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach);
         void           CalcTransonicMachEndpoints(double& aMachDragDivergence, double& aMachPeakPressure) const;
         double         CalcTransonicMultiplier(double aMach) const;
         double         CalcSkinFrictionCoeff(double aMach);

         void CalcGeometryDCM() override;

         // Calculates the vector in the direction of quarter-chord sweep
         void CalcSweepUnitVector();

         // Return the effective incidence angle, which includes incidence plus any "full flying" control angle
         virtual double CombinedControlAndIncidenceAngle_rad() const;

         double                mSpan_ft                        = 0.0;
         double                mRootChord_ft                   = 0.0;
         double                mTipChord_ft                    = 0.0;
         double                mThicknessRatio                 = 0.1;
         double                mSweepAngle_rad                 = 0.0;
         double                mDihedralAngle_rad              = 0.0;
         double                mIncidenceAngle_rad             = 0.0;
         double                mOswaldsEfficiency              = 0.85;
         bool                  mInvertVectors                  = false;   // This is true if some vectors should be inverted
         double                mAspectRatio                    = 0.0;
         double                mArea_ft2                       = 0.0;
         std::string           mAirfoilFilename;
         int                   mNumAeroSections                = 1;

         // Angle convention is "trailing edge down" (TED) is positive

         //eSurfaceControlMotion mSurfaceControlMotion           = cControlMotionFixed; // eControlMotionFixed, eControlMotionPanel, or eControlMotionFullFlying
         //bool                  mIsMovable                      = false;
         //bool                  mUseExponentialAngleMapping     = false;
         //double                mControlSurfaceChordFraction    = 0.0;
         //double                mActuatorMinRate_dps            = -300.00;
         //double                mActuatorMaxRate_dps            = 300.00;
         //double                mControlSurfaceMinAngle_deg     = 0.0;
         //double                mControlSurfaceMaxAngle_deg     = 0.0;
         //double                mActuatorMinAngle_deg           = 0.0;
         //double                mActuatorMaxAngle_deg           = 0.0;
         //double                mCurrentControlSurfaceAngle_deg = 0.0;
         UtVec3dX              mSweepUnitVector                = UtVec3dX(0.0, 0.0, 0.0); // sweep vector in vehicle coordinates
         UtDCM                 mDCM_DihedralIncidenceAndSweep  = UtDCM(0.0, 0.0, 0.0);
         AeroData              mAeroData;

      private:
         void CalcForwardUpSideUnitVectors() override;
         void CalcWindUnitVectors() override;
   };
}
#endif // !AERO_SURFACE_HPP
