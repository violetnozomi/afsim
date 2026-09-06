// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTORBITALELEMENTS_HPP
#define UTORBITALELEMENTS_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>
#include <type_traits>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtMat3.hpp"
#include "UtVec3.hpp"

//! An encapsulation of the 6+1 orbital elements needed to fix the location of an orbiting body.
//! This is the basis of the standard 'two-line element' (TLE) descriptor.  It is used by the orbital
//! propagators to keep track of the satellite's location.  It also can translate between a standard
//! position + velocity description and the set of orbital elements.
//!@note This and derived classes are used by orbital propagators (see UtOrbitalPropagator).
class UT_EXPORT UtOrbitalElements
{
public:
   //! The type of orbital elements or ephemeris indicative of this class type.
   enum EphemerisType
   {
      cNORAD           = 0, //!< The standard TLE type used with SGP*, SDP* algorithms
      cASTRO_STANDARDS = 6, //!< Not currently supported, this alternate TLE form is used with the Astrodynamics
                            //!< Standards' SP propagator.
      cOSCULATING = 9 //!< Indicates that data were not initially read from a TLE, but either converted to orbital elements,
                      //!  or input as separate osculating orbital elements.
   };

   enum class OrbitSizeSpecification
   {
      // The order of these enumerated values is important and should not be modified.
      cNULL            = 0,
      cECCENTRICITY    = 1,
      cSEMI_MAJOR_AXIS = 2, // semi-major axis OR mean motion
      cPERIAPSIS       = 4, // periapsis radius OR periapsis altitude
      cAPOAPSIS        = 8  // apoapsis radius OR apoapsis altitude
   };


   explicit UtOrbitalElements(const ut::CentralBody& aCentralBody);

   UtOrbitalElements(const ut::CentralBody& aCentralBody,
                     double                 aSemiMajorAxis,
                     double                 aEccentricity,
                     double                 aMeanAnomaly,
                     double                 aInclination,
                     double                 aRAAN, // Right Ascension of Ascending node.
                     double                 aArgumentOfPeriapsis);

   UtOrbitalElements(const UtCalendar&      aEpoch,
                     const ut::CentralBody& aCentralBody,
                     double                 aSemiMajorAxis,
                     double                 aEccentricity,
                     double                 aMeanAnomaly,
                     double                 aInclination,
                     double                 aRAAN, // Right Ascension of Ascending node.
                     double                 aArgumentOfPeriapsis);

   UtOrbitalElements(const UtOrbitalElements& aSrc) = default;
   UtOrbitalElements& operator                      =(const UtOrbitalElements& aRhs);

   virtual ~UtOrbitalElements() = default;

   virtual UtOrbitalElements* Clone() const { return new UtOrbitalElements(*this); }

   virtual bool ProcessInput(UtInput& aInput);

   virtual bool IsValid() const;

   virtual bool Initialize(const UtCalendar& aEpoch);
   virtual bool Initialize();

   //! Return the central body object
   const ut::CentralBody& GetCentralBody() const { return *mCentralBodyPtr; }

   //! Get the designator for the satellite
   const std::string& GetDesignator() const { return mDesignator; }

   //! Return the default designator that is used when one is not provided.
   static constexpr const char* GetDefaultDesignator() { return cDEFAULT_DESIGNATOR; }

   //! Get a name (descriptor) for the satellite.
   const std::string& GetDescriptor() const { return mDescriptor; }

   //! Set a name (descriptor) for the satellite.
   void SetDescriptor(const std::string& aDescriptor) { mDescriptor = aDescriptor; }

   void   SetSemiMajorAxis(double aSemiMajorAxis);
   double GetSemiMajorAxis() const;

   void   SetMeanMotion(double aMeanMotion);
   double GetMeanMotion() const;

   void   SetEccentricity(double aEccentricity);
   double GetEccentricity() const;

   double GetMeanAnomaly() const;
   void   SetMeanAnomaly(double aMeanAnomaly);

   double GetTrueAnomaly(double aTolerance = 1.0e-8, double aMaxIterations = 1000) const;
   void   SetTrueAnomaly(double aTrueAnomaly);

   double GetInclination() const { return mInclination; }
   void   SetInclination(double aInclination);

   double GetRAAN() const { return mRAAN; }
   void   SetRAAN(double aRAAN);

   double GetArgumentOfPeriapsis() const { return mArgumentOfPeriapsis; }
   void   SetArgumentOfPeriapsis(double aArgumentOfPeriapsis);

   void   SetPeriapsisRadius(double aPeriapsisRadius);
   double GetPeriapsisRadius() const;

   void   SetApoapsisRadius(double aRadiusOfApoapsis);
   double GetApoapsisRadius() const;

   void   SetPeriapsisAltitude(double aAltitudeOfPeriapsis);
   double GetPeriapsisAltitude() const;

   void   SetApoapsisAltitude(double aAltitudeOfApoapsis);
   double GetApoapsisAltitude() const;

   //! Return the ephemeris type.
   unsigned GetEphemerisType() const { return mEphemerisType; }
   void     SetEphemerisType(unsigned aEphemerisType);

   //! Return whether the orbital elements contain mean (averaged) orbital elements.
   //! @return true if the elements are mean elements; false if they are instantaneous elements.
   bool HasMeanElements() const { return mAreAveraged; }

   //! Set whether the orbital elements represent mean (averaged) orbital elements.
   void SetHasMeanElements(bool aAreAveraged) { mAreAveraged = aAreAveraged; }

   double GetAngularMomentum() const;

   const UtMat3d& GetPerifocalToInertialTransform() const;

   virtual void Set(double aSemiMajorAxis,
                    double aEccentricity,
                    double aStartMeanAnomaly,
                    double aInclination,
                    double aRAAN, // Right Ascension of Ascending node.
                    double aArgumentOfPeriapsis);

   virtual void SetSunSynchronous(const UtCalendar& aEpoch, double aSolarHour, double aCircularAltitude, double aJ2);

   virtual void Publish(const double aPosition[3], const double aVelocity[3], const UtCalendar& aEpoch);

   virtual void Publish(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtCalendar& aEpoch);

   virtual void ComputeState(UtVec3d& aPosition, UtVec3d& aVelocity) const;

   UtOrbitalElements FastForward(const double aSeconds) const;

   virtual bool OrbitIsCircular() const;

   virtual bool OrbitIsPrograde() const;

   virtual bool OrbitIsEquatorial() const;

   virtual bool OrbitIsHyperbolic() const;

   //! Get the calendar object corresponding to the epoch.
   const UtCalendar& GetEpoch() const { return mEpoch; }

   void SetEpoch(const UtCalendar& aEpoch);

   void SetEpoch(double aEpoch);

   void SetEpoch(int aYear, int aMonth, int aDay, int aHour, int aMinute, double aSecond);

   bool HasEpoch() const;

   void SetCentralBody(const ut::CentralBody& aCentralBody);

   //! Convenience methods
   //@{
   double GetPeriod() const;

   double GetRevolutionsPerDay() const;

   //@}

   enum class OrbitalRegime
   {
      cLEO,
      cMEO,
      cGEO,
      cHEO,
      cNO_LABEL
   };

   OrbitalRegime GetOrbitalRegime() const;

   static double ComputeMeanAnomaly(double aTrueAnomaly, double aEccentricity);

   static double ComputeTrueAnomaly(double aMeanAnomaly,
                                    double aEccentricity,
                                    double aTolerance     = 1.0e-8,
                                    double aMaxIterations = 1000);

   static double ComputeSemiMajorAxis(double aMeanMotion, double aGravitationalParameter);

   static double ComputeMeanMotion(double aSemiMajorAxis, double aGravitationalParameter);

   static double ComputePeriapsisRadius(double aPeriapsisAltitude, double aCentralBodyRadius);

   static double ComputePeriapsisAltitude(double aPeriapsisRadius, double aCentralBodyRadius);

   static double ComputeApoapsisRadius(double aApoapsisAltitude, double aCentralBodyRadius);

   static double ComputeApoapsisAltitude(double aApoapsisRadius, double aCentralBodyRadius);

   UT_DEPRECATED static void ComputePerifocalToInertialTransform(double   aRAAN,
                                                                 double   aInclination,
                                                                 double   aArgumentOfPeriapsis,
                                                                 UtMat3d& aRotationMatrix);

protected:
   double GetGravitationalParameter() const { return mCentralBodyPtr->GetGravitationalParameter(); }

   ut::CloneablePtr<ut::CentralBody> mCentralBodyPtr;

   // Define the 2-D shape of the orbit (3 elements)
   mutable double mMeanMotion{0.0};    // redundant with mSemiMajorAxis (using gravitational parameter)
   mutable double mSemiMajorAxis{1.0}; // redundant with mMeanMotion    (using gravitational parameter)
   mutable double mEccentricity{0.0};

   mutable double mPeriapsisRadius{1.0};   // redundant with mPeriapsisAltitude (using central body radius)
   mutable double mApoapsisRadius{1.0};    // redundant with mApoapsisAltitude  (using central body radius)
   mutable double mPeriapsisAltitude{1.0}; // redundant with mPeriapsisRadius   (using central body radius)
   mutable double mApoapsisAltitude{1.0};  // redundant with mApoapsisRadius    (using central body radius)

   // Define the 3-D rotation of the orbit (3 elements)
   double mInclination{0.0};
   double mRAAN{0.0}; // Right Ascension of Ascending node.
   double mArgumentOfPeriapsis{0.0};

   mutable UtMat3d mPerifocalToInertialTransform; // On demand, produce a transformation matrix using the rotational elements

   static constexpr const char* cDEFAULT_DESIGNATOR{"00001A"};
   std::string                  mDesignator{cDEFAULT_DESIGNATOR};
   mutable std::string          mDescriptor;

   mutable double mTrueAnomaly{0.0};          // redundant with mMeanAnomaly, radians per second
   mutable double mMeanAnomaly{0.0};          // redundant with mTrueAnomaly, radians per second
   double         mNodalVectorMagnitude{0.0}; // For use in fine determinations of equatorial orbits.
   mutable double mAngularMomentum{0.0};

private:
   // Resets the validity flags for elements pertaining to orbit size.
   void ResetValidityFlags();
   bool ValidateOrbitalElements();

   void SetOrbitSizeSpecification(const OrbitSizeSpecification& aOrbitSizeSpecification);

   UtCalendar mEpoch;
   bool       mEpochSet{false};
   bool       mAreAveraged{false};

   unsigned mEphemerisType{cOSCULATING};

   mutable bool mSemiMajorAxisValid{false};
   mutable bool mMeanMotionValid{false};

   mutable bool mPeriapsisRadiusValid{false};
   mutable bool mPeriapsisAltitudeValid{false};

   mutable bool mApoapsisRadiusValid{false};
   mutable bool mApoapsisAltitudeValid{false};

   mutable bool mEccentricityValid{false};

   mutable bool mMeanAnomalyValid{false};
   mutable bool mTrueAnomalyValid{false};

   mutable bool mNodalVectorMagnitudeValid{false};
   mutable bool mPerifocalToInertialTransformValid{false};

   std::pair<OrbitSizeSpecification, OrbitSizeSpecification> mOrbitSizeElementsPair{OrbitSizeSpecification::cNULL,
                                                                                    OrbitSizeSpecification::cNULL};
   OrbitSizeSpecification                                    mOrbitSizeElements{OrbitSizeSpecification::cNULL};
};

inline constexpr UtOrbitalElements::OrbitSizeSpecification operator|(UtOrbitalElements::OrbitSizeSpecification aLhs,
                                                                     UtOrbitalElements::OrbitSizeSpecification aRhs)
{
   return static_cast<UtOrbitalElements::OrbitSizeSpecification>(
      static_cast<std::underlying_type<UtOrbitalElements::OrbitSizeSpecification>::type>(aLhs) |
      static_cast<std::underlying_type<UtOrbitalElements::OrbitSizeSpecification>::type>(aRhs));
}

inline constexpr UtOrbitalElements::OrbitSizeSpecification operator&(UtOrbitalElements::OrbitSizeSpecification aLhs,
                                                                     UtOrbitalElements::OrbitSizeSpecification aRhs)
{
   return static_cast<UtOrbitalElements::OrbitSizeSpecification>(
      static_cast<std::underlying_type<UtOrbitalElements::OrbitSizeSpecification>::type>(aLhs) &
      static_cast<std::underlying_type<UtOrbitalElements::OrbitSizeSpecification>::type>(aRhs));
}
#endif
