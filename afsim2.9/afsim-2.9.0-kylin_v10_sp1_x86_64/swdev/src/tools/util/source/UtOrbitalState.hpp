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

#ifndef UTORBITALSTATE_HPP
#define UTORBITALSTATE_HPP

#include "ut_export.h"

#include "UtCalendar.hpp"
#include "UtCentralPoint.hpp"
#include "UtCloneablePtr.hpp"
#include "UtECI_Conversion.hpp"
class UtInput;
#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
class UtTwoLineElement;
#include "UtVec3.hpp"

namespace ut
{
//! A class representing an orbital state vector (position + velocity).
//! Other information about the state (inertial coordinate system, reference frame, and epoch)
//! are stored in the orbital state.
//! @note This class is defined outside of OrbitalState to allow forward declaration.
class OrbitalStateVector
{
public:
   OrbitalStateVector() = default;
   OrbitalStateVector(const UtVec3d& aPosition, const UtVec3d& aVelocity)
      : mPosition(aPosition)
      , mVelocity(aVelocity)
   {
   }
   OrbitalStateVector(double aPosition[3], double aVelocity[3])
      : mPosition(aPosition)
      , mVelocity(aVelocity)
   {
   }
   const UtVec3d& GetLocation() const { return mPosition; }
   const UtVec3d& GetVelocity() const { return mVelocity; }

   void Set(const UtVec3d& aPosition, const UtVec3d& aVelocity)
   {
      mPosition = aPosition;
      mVelocity = aVelocity;
   }

private:
   UtVec3d mPosition;
   UtVec3d mVelocity;
};

//! A representation of an orbital state.
//! In this class, orbital state representation can be either an orbital state
//! vector, which contains a position and velocity, or it can be a set of
//! orbital elements (see UtOrbitalElements).  Orbital states referenced to a particular epoch,
//! or instant in time, as represented by a UtCalendar object, as well as a particular coordinate
//! system (equatorial or ecliptic), inertial reference fame, and epoch.
//! Orbital state vectors are always considered instantaneous, in that they refer to the
//! true location of the object at epoch.  Orbital elements, however, can be considered to be either
//! instantaneous or averaged.  Instantaneous orbital elements (also called "osculating" orbital
//! elements) define a Keplerian elliptical orbit, which is the orbit defined by a spherical, homogeneous
//! gravitating central body in the absence of other forces.  These elements can be converted
//! into orbital state vectors, such that instantaneous orbital elements and state vectors are equivalent.
//! Because of this, we can set the orbital elements and get the orbital state vector, and vice-versa.
//! In contrast, averaged orbital elements cannot be converted to orbital state vectors without use of
//! a specific propagator (NORAD two-line elements is an example of an averaged element set).
//! Attempting to retrieve an orbital state vector based on an averaged orbital element set
//! results in an exception.
class UT_EXPORT OrbitalState
{
public:
   // Alias OrbitalState::Vector to be OrbitalStateVector.
   using Vector = OrbitalStateVector;

   //! Define the coordinate system.
   //! Usually this will be equatorial, except in the case of planetary orbital states.
   enum class CoordinateSystem
   {
      cUNDEFINED_COORDINATE_SYSTEM,
      cEQUATORIAL,
      cECLIPTIC
   };

   //! Define the reference frame to which the orbital state is referenced.
   //! This implies both a reference epoch and reference frame.
   enum class ReferenceFrame
   {
      cUNDEFINED_REFERENCE_FRAME,
      cECI,   //!< Referenced to AFSIM ECI / GCRF (mean equator, equinox at J2000; very small angular offset from J2000)
      cJ2000, //!< Referenced to J2000 (mean equator, equinox at J2000)
      cMEAN_OF_DATE, //!< Referenced to the epoch without nutation
      cTRUE_OF_DATE, //!< Referenced to the epoch with nutation
      cTEME          //!< "True Equator, Mean Equinox" as used by AFSPC SGP propagators (WSF_NORAD_SPACE_MOVER).
   };

   //! Default constructor only to be used when the orbital state configuration is not known initially
   //! (such as when ProcessInput is called).
   //! @note OrbitalState::Initialize must be called if this constructor is used.
   OrbitalState();

   //! Constructor that defines the context of the state, but leaves setting
   //! the kinematics to one of the Set(...) methods.
   //! @note OrbitalState::Initialize must be called if this constructor is used.
   OrbitalState(const ut::CentralPoint& aCentralPoint, CoordinateSystem aCoordinateSystem, ReferenceFrame aReferenceFrame);

   //! Constructors that fully define a state.
   //! @note OrbitalState::Initialize should not be called if these constructors are used.
   //@{
   OrbitalState(CoordinateSystem aCoordinateSystem, ReferenceFrame aReferenceFrame, const UtOrbitalElements& aElements);

   OrbitalState(const UtCalendar&       aEpoch,
                const ut::CentralPoint& aCentralPoint,
                CoordinateSystem        aCoordinateSystem,
                ReferenceFrame          aReferenceFrame,
                const Vector&           aVector);
   //@}

   OrbitalState(const OrbitalState& aSrc) = default;
   ~OrbitalState()                        = default;
   OrbitalState& operator=(const OrbitalState& aRhs) = default;
   OrbitalState* Clone() const { return new OrbitalState(*this); }

   bool ProcessInput(UtInput& aInput);
   bool IsValid() const;
   bool Initialize(const UtCalendar& aEpoch);

   //! @name Methods to provide orbital elements or two-line elements.
   //@{
   const UtOrbitalElements& GetOrbitalElements() const;
   const UtTwoLineElement&  GetTwoLineElement() const;
   const UtOrbitalElements& GetOrbitalElementsTOD() const;

   bool OrbitalElementsValid() const { return mOrbitalElementsValid; }
   bool TwoLineElementValid() const;
   //@}

   //! @name Utility Methods with default implementations to catch special cases used in maneuvering.
   //@{
   bool OrbitIsHyperbolic() const;
   bool OrbitIsCircular() const;
   bool OrbitIsEquatorial() const;
   bool OrbitIsPrograde() const;
   //@}

   //! @name Mutators that atomically set the orbital state (i.e., nothing else needs to be called to set the state).
   //@{
   void Set(const UtCalendar& aEpoch, const Vector& aVector);
   void SetInertial(const UtCalendar& aEpoch, const Vector& aVector);
   void Set(const UtOrbitalElements& aElements);
   void SetTOD(const UtOrbitalElements& aElements);
   void Set(const OrbitalState& aState);
   //@}

   //! @name Methods that provide the orbital state vector in various reference frames.
   //@{
   const Vector& GetOrbitalStateVector() const;
   Vector        GetOrbitalStateVectorWCS() const;
   Vector        GetOrbitalStateVectorInertial() const;
   Vector        GetOrbitalStateVectorTOD() const;
   Vector        GetOrbitalStateVectorTEME() const;
   Vector        GetOrbitalStateVector(ReferenceFrame aReferenceFrame) const;
   Vector        GetOrbitalStateVectorInertial(const CentralPoint& aCentralPoint) const;
   //@}

   //! @name Methods related to the central point associated with the orbital state.
   //@{
   const ut::CentralPoint& GetCentralPoint() const;
   const ut::CentralBody&  GetCentralBody() const;

   void               SetCentralPoint(const ut::CentralPoint& aCentralPoint);
   UT_DEPRECATED void SetCentralBody(const ut::CentralBody& aCentralBody) { SetCentralPoint(aCentralBody); }
   void               TransformTo(const ut::CentralPoint& aNewCentralPoint);
   bool               OrbitIntersectsCentralBody() const;
   //@}

   void                         SetAccelerationInertial(const UtVec3d& aAcceleration) const;
   const UtVec3d&               GetAccelerationInertial() const;
   UT_DEPRECATED const UtVec3d& GetAcceleration() const;

   const UtCalendar& GetEpoch() const;
   void              SetEpoch(const UtCalendar& aEpoch);
   bool              HasEpoch() const { return mEpochValid; }
   bool              HasPlatformCreationEpoch() const { return mEpochPlatformCreation; }

   CoordinateSystem GetCoordinateSystem() const { return mCoordinateSystem; }
   void             SetCoordinateSystem(CoordinateSystem aCoordinateSystem);

   ReferenceFrame GetReferenceFrame() const { return mReferenceFrame; }
   void           SetReferenceFrame(ReferenceFrame aReferenceFrame);

   // gets and sets mTLE_Input, which specifies if tle input was used
   bool IsTLEInput() const noexcept { return mTLE_Input; }
   void SetTLEInput(bool aTLE_Input) noexcept { mTLE_Input = aTLE_Input; }

   bool IsDeclaredWithOrbitalState() const noexcept { return mDeclaredWithOrbitalState; }
   void SetDeclaredWithOrbitalState(bool aDeclaredWithOrbitalState) noexcept
   {
      mDeclaredWithOrbitalState = aDeclaredWithOrbitalState;
   }

   double GetEscapeVelocity() const;
   double GetGravitationalParameter() const { return GetCentralPoint().GetGravitationalParameter(); }

   void SetOrbitalStateVector(const Vector& aVector);

private:
   void              InitializeReferenceFrame();
   UtECI_Conversion& GetInertialConversion() const;
   void              UpdateInertialConversion() const;

   mutable bool mEpochValid{false};
   mutable bool mOrbitalElementsValid{false};
   mutable bool mOrbitalStateVectorValid{false};
   mutable bool mAccelerationValid{false};
   mutable bool mOrbitalElementsTOD_Valid{false};
   mutable bool mOrbitalElementsComputable{true};

   bool mEpochPlatformCreation{false};
   bool mDeclaredWithOrbitalState{false};
   bool mTLE_Input{false};

   mutable ut::CloneablePtr<UtOrbitalElements> mOrbitalElementsPtr;
   mutable ut::CloneablePtr<UtOrbitalElements> mOrbitalElementsTOD_Ptr{nullptr};
   mutable Vector                              mOrbitalStateVector;
   ut::CloneablePtr<ut::CentralPoint>          mCentralPointPtr;
   CoordinateSystem                            mCoordinateSystem{CoordinateSystem::cEQUATORIAL};
   mutable ReferenceFrame                      mReferenceFrame{ReferenceFrame::cUNDEFINED_REFERENCE_FRAME};
   mutable UtCalendar                          mEpoch;
   mutable UtVec3d                             mAcceleration;
   mutable ut::CloneablePtr<UtECI_Conversion>  mInertialConversionPtr{nullptr};
};

}; // namespace ut

#endif
