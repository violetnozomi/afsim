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

#ifndef AERO_OBJECT_HPP
#define AERO_OBJECT_HPP

#include <QStringList>
#include <string>

#include "AeroData.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "UtDCM.hpp"
#include "UtVec3dX.hpp"

// ****************************************************************************
// AeroObject is the base class of all objects that have aerodynamic
// characteristics such as bodies, fuselages, surfaces, wings, and drag
// producers such as speed brakes, and landing gear.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroObject //: public QObject
   {
      public:
         AeroObject(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         ~AeroObject() = default;

         std::string AeroObjType() { return mAeroTypeString; }

         virtual std::string GetName();

         // Returns the name of the geometry object from which this aero object was generated
         std::string GetGeneratedFromName() const;

         // This allows the reference point to be set
         virtual void SetRefPoint(UtVec3dX aRefPoint_ft);

         // This is used to set a normalized control value for any movement that
         // may be supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         virtual void SetNormalizedControlValue(double aControlValue);

         // This is used to set a control angle for any movement that may be
         // supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         virtual void SetControlAngle_deg(double aControlAngle_deg);

         // This is used to get the control angle for any movement that may be
         // supported by the object. This may be overridden by derived classes.
         virtual double GetControlAngle_deg() const { return 0.0; }

         // This sets the vehicle alpha/beta in degrees
         void SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg);

         // This sets the vehicle yaw, pitch, and roll rates in degrees/sec
         void SetVehicleYawPitchRollRates(double aYawRate_dps,
                                          double aPitchRate_dps,
                                          double aRollRate_dps);

         // This sets the vehicle parameters pertaining to speed
         void SetVehicleSpeedParameters(double aSpeed_fps,
                                        double aMach,
                                        double aKTAS,
                                        double aKCAS,
                                        double aDynamicPressure_psf,
                                        double aAirDensity_slugs_ft3);

         // This should be called after calling SetVehicleAlphaBeta, SetVehicleYawPitchRollRates,
         // and SetVehicleSpeedParameters. It updates the main aero calculation vectors, which are
         // based on vehicle attitude along with the characteristics of the aero object.
         void CalculateAeroAttitudeVectors();

         // Returns a vector in the positive "forward" direction.
         UtVec3dX GetForwardUnitVector() { return mGeometryDCM.GetCol(0); }

         // Returns a vector in the positive "up" direction. This is not the direction of actual lift,
         // but is used to calculate the actual lift vector.
         UtVec3dX GetUpUnitVector() { return mGeometryDCM.GetCol(2); }

         // Returns a vector in the positive "side" direction.
         UtVec3dX GetSideUnitVector() { return mGeometryDCM.GetCol(1); }

         // Returns a vector in the positive forward direction. Must be overridden by derived classes.
         UtVec3dX GetVelocityUnitVector() { return mVelocityUnitVector; }

         // Returns a vector in the drag direction. This is the "opposite" of the forward vector.
         UtVec3dX GetDragForceUnitVector() { return mDragForceUnitVector; }

         // Returns a vector in the lift direction. It is calculated using the forward unit vector
         // and the lifting normal unit vector.
         UtVec3dX GetLiftForceUnitVector() { return mLiftForceUnitVector; }

         // Returns a vector in the side force direction
         UtVec3dX GetSideForceUnitVector() { return mSideForceUnitVector; }

         // Calculate F&M using alpha, beta, q, and Mach. SetVehicleAlphaBeta should be called
         // prior to calling this function. Must be overridden by derived classes.
         virtual void CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                           double    aMach,
                                           UtVec3dX& aLiftVector_lbs,
                                           UtVec3dX& aDragVector_lbs,
                                           UtVec3dX& aSideVector_lbs,
                                           UtVec3dX& aMoment_ftlbs,
                                           UtVec3dX& aForcePoint_ft) = 0;

         // This returns the location of the reference point
         UtVec3dX GetRefPoint_ft() const;

         // This returns the location where aero forces are acting (as a function of Mach).
         // May be overridden by derived classes.
         virtual UtVec3dX CalcAeroForcePoint_ft(double aMach = 0.0) const = 0;

         // Provides the local alpha and beta angles. SetVehicleAlphaBeta should be called
         // prior to calling this function. May be overridden by derived classes.
         virtual void CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg);

         // This returns the surface area of the object (in sqft)
         double GetSurfaceArea_ft2() { return mSurfaceArea_ft2; }

         // This should be overridden by derived classes, if appropriate
         virtual bool IsMovable() const { return false; }

         virtual QStringList Audit() { return QStringList(); }

         void UpdateGeometryDCM() { CalcGeometryDCM(); }

         void CalcAeroData(double    aAltitude_ft,
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
                           UtVec3dX& aTotalMomentVector_ftlbs);

         virtual void CalcAeroCoeffDelta(const std::string& aControlSurfaceName,
                                         double             aControlAngle_deg,
                                         double             aAltitude_ft,
                                         double             aSpeed_fps,
                                         double             aMach,
                                         double             aKTAS,
                                         double             aKCAS,
                                         double             aDynamicPressure_psf,
                                         double             aDensity_slugs_ft3,
                                         double             aVehicleAlpha_deg,
                                         double             aVehicleBeta_deg,
                                         double             aVehicleYawRate_dps,
                                         double             aVehiclePitchRate_dps,
                                         double             aVehicleRollRate_dps,
                                         double&            aDeltaCL,
                                         double&            aDeltaCd,
                                         double&            aDeltaCY,
                                         double&            aDeltaCm,
                                         double&            aDeltaCn,
                                         double&            aDeltaCl);

      protected:

         // Calculates the vectors in the forward, up, and side directions.
         // This can be overriden, but typically is not.
         virtual void CalcForwardUpSideUnitVectors();

         // Calculates mAlphaBetaDCM, which is from body to wind frame. Takes into account induced velocity.
         // Used in conjunction with mGeometryDCM to calculate the velocity, drag, side, and lift unit vectors.
         void CalcAlphaBetaDCM();

         // Calculates mGeometryDCM, which is from sub-object body coordinates to vehicle body coordinates.
         // Used in conjunction with mAlphaBetaDCM to calculate the velocity, drag, side, and lift unit vectors.
         virtual void CalcGeometryDCM() = 0;

         // Calculates the velocity, drag, side, and lift unit vectors.
         // This can be overriden, but typically is not.
         virtual void CalcWindUnitVectors();

         void InvertGeometryVectors();
         void InvertWindVectors();

         std::string  mName                          = std::string("UNDEFINED");
         std::string  mAeroTypeString                = std::string("UNDEFINED"); // Defines the type of aero object (used with GUI elements)
         std::string  mGeneratedFromName;                                        // Name of the geometry object from which this aero object was generated
         UtVec3dX     mRefPoint_ft                   = UtVec3dX(0.0, 0.0, 0.0);  // Reference point of the aero object
         UtVec3dX     mForcePoint_ft                 = UtVec3dX(0.0, 0.0, 0.0);  // Position at which aero forces/moments are acting
         UtDCM        mDCM                           = UtDCM(0.0, 0.0, 0.0);
         double       mSurfaceArea_ft2               = 0.0;
         Vehicle*     mVehicle                       = nullptr;

         UtVec3dX     mVelocityUnitVector            = UtVec3dX(0.0, 0.0, 0.0);  // This points in the direction of travel (velocity vector) in vehicle coordinates
         UtVec3dX     mLiftForceUnitVector           = UtVec3dX(0.0, 0.0, 0.0);  // This is the direction in which the lift force acts in vehicle coordinates
         UtVec3dX     mDragForceUnitVector           = UtVec3dX(0.0, 0.0, 0.0);  // This is the direction in which the drag force acts (opposite of the velocity vector) in vehicle coordinates
         UtVec3dX     mSideForceUnitVector           = UtVec3dX(0.0, 0.0, 0.0);  // This is the direction in which the side force acts in vehicle coordinates

         UtDCM        mAlphaBetaDCM                  = UtDCM(0.0, 0.0, 0.0); // From body to wind frame. Takes into account induced velocity
         UtDCM        mGeometryDCM                   = UtDCM(0.0, 0.0, 0.0);// From sub-object body coordinates to vehicle coordinates.

         double       mVehicleAlpha_deg              = 0.0;
         double       mVehicleBeta_deg               = 0.0;
         double       mVehicleYawRate_rps            = 0.0;
         double       mVehiclePitchRate_rps          = 0.0;
         double       mVehicleRollRate_rps           = 0.0;
         double       mVehicleSpeed_fps              = 0.0;
         double       mVehicleMach                   = 0.0;
         double       mVehicleKTAS                   = 0.0;
         double       mVehicleKCAS                   = 0.0;
         double       mVehicleDynamicPressure_psf    = 0.0;
         double       mAirDensity_slugs_ft3          = 0.0;
         double       mAdditionalDynamicPressure_psf = 0.0;

         static const double cEpsilon;
         static const double cAxesTransformDCM[3][3];
   };
}
#endif // ! AERO_OBJECT_HPP
