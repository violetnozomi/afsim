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

#include "UtOrbitalState.hpp"

#include <cmath>
#include <string>

#include "UtCalendar.hpp"
#include "UtEntity.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSolarSystem.hpp"
#include "UtSun.hpp"
#include "UtTwoLineElement.hpp"

namespace ut
{

OrbitalState::OrbitalState()
   : mOrbitalElementsPtr(ut::make_unique<UtTwoLineElement>())
   , mCentralPointPtr(ut::make_unique<EarthWGS84>()) // Default when no more information can be provided.

{
}

OrbitalState::OrbitalState(const CentralPoint& aCentralPoint, CoordinateSystem aCoordinateSystem, ReferenceFrame aReferenceFrame)
   : mOrbitalElementsPtr(ut::make_unique<UtTwoLineElement>())
   , mCentralPointPtr(ut::clone(aCentralPoint))
   , mCoordinateSystem(aCoordinateSystem)
   , mReferenceFrame(aReferenceFrame)
{
}

OrbitalState::OrbitalState(CoordinateSystem aCoordinateSystem, ReferenceFrame aReferenceFrame, const UtOrbitalElements& aElements)
   : mEpochValid(true)
   , mOrbitalElementsValid(true)
   , mOrbitalElementsComputable(true)
   , mOrbitalElementsPtr(ut::clone(aElements))
   , mCentralPointPtr(ut::clone(aElements.GetCentralBody()))
   , mCoordinateSystem(aCoordinateSystem)
   , mReferenceFrame(aReferenceFrame)
   , mEpoch(aElements.GetEpoch())
{
}

OrbitalState::OrbitalState(const UtCalendar&   aEpoch,
                           const CentralPoint& aCentralPoint,
                           CoordinateSystem    aCoordinateSystem,
                           ReferenceFrame      aReferenceFrame,
                           const Vector&       aStateVector)
   : mEpochValid(true)
   , mOrbitalStateVectorValid(true)
   , mOrbitalElementsComputable(aCentralPoint.IsCentralBody())
   , mOrbitalElementsPtr(ut::make_unique<UtTwoLineElement>())
   , mOrbitalStateVector(aStateVector)
   , mCentralPointPtr(ut::clone(aCentralPoint))
   , mCoordinateSystem(aCoordinateSystem)
   , mReferenceFrame(aReferenceFrame)
   , mEpoch(aEpoch)
{
}

//! Process common commands using a UtInput object.
//! @param aInput The input object from which the commands are extracted.
bool OrbitalState::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;

   if (command == "orbit")
   {
      SetTLEInput(true);
      mOrbitalElementsPtr->ProcessInput(aInput);
   }
   else if (command == "position")
   {
      // this method is only allowed when inside the
      // orbital_state initialization block
      if (!mDeclaredWithOrbitalState)
      {
         throw UtInput::UnknownCommand(aInput);
      }

      double      posECI[3];
      std::string units;

      aInput.ReadValue(posECI[0]);
      aInput.ReadValue(posECI[1]);
      aInput.ReadValue(posECI[2]);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
      UtVec3d::Multiply(posECI, multiplier);

      std::string velIdentifier;
      aInput.ReadCommand(velIdentifier);
      if (velIdentifier != "velocity")
      {
         throw UtInput::UnknownCommand(aInput);
      }

      double velECI[3];
      aInput.ReadValue(velECI[0]);
      aInput.ReadValue(velECI[1]);
      aInput.ReadValue(velECI[2]);
      aInput.ReadValue(units);
      multiplier = aInput.ConvertValue(1.0, units, UtInput::cSPEED);
      UtVec3d::Multiply(velECI, multiplier);

      SetCentralPoint(ut::EarthWGS84());
      SetCoordinateSystem(ut::OrbitalState::CoordinateSystem::cEQUATORIAL);
      SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);

      // When 'position' declaration used in orbital state, it is usually preceded with an epoch.
      // SetOrbitalStateVector() sets mEpochValid to false, we want to avoid this.
      mOrbitalStateVector       = ut::OrbitalStateVector(posECI, velECI);
      mOrbitalStateVectorValid  = true;
      mOrbitalElementsValid     = false;
      mOrbitalElementsTOD_Valid = false;
      mAccelerationValid        = false;
   }
   else if (command == "reference_frame")
   {
      std::string referenceFrame;
      aInput.ReadValue(referenceFrame);
      if (referenceFrame == "eci")
      {
         mReferenceFrame = ReferenceFrame::cECI;
      }
      else if (referenceFrame == "j2000")
      {
         mReferenceFrame = ReferenceFrame::cJ2000;
      }
      else if (referenceFrame == "mean_of_date")
      {
         mReferenceFrame = ReferenceFrame::cMEAN_OF_DATE;
      }
      else if (referenceFrame == "true_of_date")
      {
         mReferenceFrame = ReferenceFrame::cTRUE_OF_DATE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown reference_frame: " + referenceFrame);
      }
   }
   else if (command == "epoch")
   {
      std::string epoch;
      aInput.ReadValue(epoch);

      if (epoch == "platform_creation_epoch")
      {
         mEpochPlatformCreation = true;
      }
      else
      {
         aInput.PushBack(epoch);
         double epochAsDouble;
         aInput.ReadValue(epochAsDouble);
         mEpoch.SetEpoch(epochAsDouble);
         mEpochValid = true;
         mOrbitalElementsPtr->SetEpoch(mEpoch);
      }
   }
   else if (command == "epoch_date_time")
   {
      aInput.ReadValue(mEpoch);
      mEpochValid = true;
      mOrbitalElementsPtr->SetEpoch(mEpoch);
   }
   else if (mOrbitalElementsPtr->ProcessInput(aInput))
   {
   }
   else if (command == "central_body")
   {
      std::string  centralBody;
      UtInputBlock block(aInput);
      block.ReadCommand(centralBody);
      mCentralPointPtr = CentralPoint::Create(centralBody);
      if (mCentralPointPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unrecognized central body type");
      }
      block.ProcessInput(mCentralPointPtr.get());
      if (mCentralPointPtr->IsCentralBody())
      {
         mOrbitalElementsComputable = true;
         mOrbitalElementsPtr->SetCentralBody(static_cast<const ut::CentralBody&>(*mCentralPointPtr));
      }
   }
   else if (command == "wgs_84") // deprecated
   {
      auto warning = ut::log::warning() << "Deprecated command.";
      warning.AddNote(command);
      mCentralPointPtr = ut::make_unique<EarthEGM96>();
   }
   else if (command == "egm_96") // deprecated
   {
      auto warning = ut::log::warning() << "Deprecated command.";
      warning.AddNote(command);
      mCentralPointPtr = ut::make_unique<EarthEGM96>();
   }
   else if (command == "heliocentric") // deprecated
   {
      mCentralPointPtr = ut::make_unique<Sun>();
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

//! Get the set of orbital elements as a two-line element.  If the underlying type is not a TLE, an exception will be thrown.
//! If the orbital elements did not initially contain a TLE, default values will be returned in all TLE-specific fields.
//! @note if a conversion is made from orbital state vector to orbital elements, mean values are not preserved.
const UtTwoLineElement& OrbitalState::GetTwoLineElement() const
{
   const auto tlePtr =
      dynamic_cast<const UtTwoLineElement*>(&GetOrbitalElements()); // Updates the orbital elements if necessary.
   if (tlePtr != nullptr)
   {
      return *tlePtr;
   }
   else
   {
      throw UtException("ut::OrbitalState::GetTwoLineElement: Could not cast to UtTwoLineElement.");
   }
}

//! Provide orbital elements in true-of-date format.
//! @note This is almost always the orbital element format desired, so this conversion is preferentially supported.
const UtOrbitalElements& OrbitalState::GetOrbitalElementsTOD() const
{
   if (!mOrbitalElementsComputable)
   {
      throw UtException(
         "ut::OrbitalState::GetOrbitalElementsTOD: Orbital elements cannot be produced from central point " +
         (std::string)mCentralPointPtr->GetName());
   }
   if (mOrbitalElementsTOD_Ptr == nullptr)
   {
      mOrbitalElementsTOD_Ptr = ut::make_unique<UtOrbitalElements>(mCentralPointPtr->GetAsCentralBody());
   }
   if (!mOrbitalElementsTOD_Valid)
   {
      if (IsValid())
      {
         Vector stateVectorTOD = GetOrbitalStateVectorTOD();
         mOrbitalElementsTOD_Ptr->Publish(stateVectorTOD.GetLocation(), stateVectorTOD.GetVelocity(), mEpoch);
         mOrbitalElementsTOD_Valid = true;
      }
      else
      {
         throw UtException("ut::OrbitalState::GetOrbitalElementsTOD: Orbital state is invalid");
      }
   }
   return *mOrbitalElementsTOD_Ptr;
}

//! Return whether the orbital elements are
bool OrbitalState::TwoLineElementValid() const
{
   return (mOrbitalElementsValid && (dynamic_cast<UtTwoLineElement*>(mOrbitalElementsPtr.get()) != nullptr));
}

//! Return the classical orbital elements for this state.
const UtOrbitalElements& OrbitalState::GetOrbitalElements() const
{
   if (!mOrbitalElementsComputable)
   {
      throw UtException(
         "ut::OrbitalState::GetOrbitalElements: Orbital elements cannot be produced from central point " +
         (std::string)mCentralPointPtr->GetName());
   }
   if (!mOrbitalElementsValid && IsValid())
   {
      mOrbitalElementsPtr->Publish(mOrbitalStateVector.GetLocation().GetData(),
                                   mOrbitalStateVector.GetVelocity().GetData(),
                                   mEpoch);
      if (mOrbitalElementsPtr->HasMeanElements())
      {
         mOrbitalElementsPtr->SetHasMeanElements(false);
         mOrbitalElementsPtr->SetEphemerisType(UtOrbitalElements::cOSCULATING);
      }
      mOrbitalElementsValid = true;
   }
   return *mOrbitalElementsPtr;
}

//! Return whether the orbit is hyperbolic (non-ellipsoidal).
bool OrbitalState::OrbitIsHyperbolic() const
{
   if (mOrbitalElementsValid)
   {
      return mOrbitalElementsPtr->OrbitIsHyperbolic();
   }
   else
   {
      return (mOrbitalStateVector.GetVelocity().Magnitude() >= GetEscapeVelocity());
   }
}

//! Define whether an orbit is "circular" (i.e., has nearly zero eccentricity).
bool OrbitalState::OrbitIsCircular() const
{
   return GetOrbitalElements().OrbitIsCircular();
}

//! Define whether an orbit is "equatorial" (i.e., the inclination is very close to 0 or 180 degrees).
bool OrbitalState::OrbitIsEquatorial() const
{
   return GetOrbitalElementsTOD().OrbitIsEquatorial();
}

//! Return whether the orbit is prograde (orbiting counterclockwise when viewed from above the north pole of the central
//! body; true when the orbital inclination is less than 90 degrees).
bool OrbitalState::OrbitIsPrograde() const
{
   return GetOrbitalElements().OrbitIsPrograde();
}

//! Return the velocity required to escape the gravitational pull of the central body.
double OrbitalState::GetEscapeVelocity() const
{
   return GetCentralBody().GetEscapeVelocity(GetOrbitalStateVectorInertial().GetLocation());
}

//! Return the orbital state vector that defines this orbital state.
//! If the orbital state vector is not currently valid, it will be converted from orbital elements, assuming they are
//! osculating; otherwise, if they are non-osculating, an exception will be thrown.
const OrbitalState::Vector& OrbitalState::GetOrbitalStateVector() const
{
   if (!mOrbitalStateVectorValid)
   {
      if (!mOrbitalElementsValid)
      {
         throw UtException("ut::OrbitalState::GetOrbitalStateVector: Cannot perform Mean orbital elements -> orbital "
                           "state vector conversion");
      }
      UtVec3d position;
      UtVec3d velocity;
      mOrbitalElementsPtr->ComputeState(position, velocity);
      mOrbitalStateVector.Set(position, velocity);
      mOrbitalStateVectorValid = true;
   }
   return mOrbitalStateVector;
}

//! Return the orbital state vector referenced to world coordinates (non-inertial).
OrbitalState::Vector OrbitalState::GetOrbitalStateVectorWCS() const
{
   auto& conv = GetInertialConversion();
   return Vector(conv.GetLocationWCS(), conv.GetVelocityWCS());
}

//! Return the orbital state vector referenced to inertial coordinates.
OrbitalState::Vector OrbitalState::GetOrbitalStateVectorInertial() const
{
   Vector vectorInertial = GetOrbitalStateVector();
   if (mReferenceFrame != ReferenceFrame::cECI)
   {
      auto& conv = GetInertialConversion();
      vectorInertial.Set(conv.GetLocationECI(), conv.GetVelocityECI());
   }
   return vectorInertial;
}

//! Return the orbital state vector referenced to True Of Date (TOD, quasi-inertial) coordinates.
OrbitalState::Vector OrbitalState::GetOrbitalStateVectorTOD() const
{
   Vector vectorTOD = GetOrbitalStateVector();
   if (mReferenceFrame != ReferenceFrame::cTRUE_OF_DATE)
   {
      auto& conv = GetInertialConversion();
      vectorTOD.Set(conv.GetLocationTOD(), conv.GetVelocityTOD());
   }
   return vectorTOD;
}

//! Return the orbital state vector referenced to True Equator, Mean ReferenceFrame (TEME, quasi-inertial) coordinates.
OrbitalState::Vector OrbitalState::GetOrbitalStateVectorTEME() const
{
   Vector vectorTEME = GetOrbitalStateVector();
   if (mReferenceFrame != ReferenceFrame::cTEME)
   {
      auto& conv = GetInertialConversion();
      vectorTEME.Set(conv.GetLocationTEME(), conv.GetVelocityTEME());
   }
   return vectorTEME;
}

//! Set the acceleration value in GCRF/J2000.
void ut::OrbitalState::SetAccelerationInertial(const UtVec3d& aAcceleration) const
{
   switch (mReferenceFrame)
   {
   case ReferenceFrame::cECI:
   case ReferenceFrame::cJ2000: // TODO J2000 acceleration.
      mAcceleration = aAcceleration;
      break;
   case ReferenceFrame::cTRUE_OF_DATE:
   case ReferenceFrame::cTEME: // TODO TEME acceleration
      GetInertialConversion().SetAccelerationECI(aAcceleration);
      mAcceleration = GetInertialConversion().GetAccelerationTOD();
      break;
   default:
      throw UtException("ut::OrbitalState::SetAccelerationInertial: Unsupported reference frame");
   }
   mAccelerationValid = true;
}

//! Given a valid state, get the acceleration due to the gravitational force of the central body (if any).
//! If the acceleration has not been set using SetAccelerationInertial, it will be calculated as the
//! acceleration due to the spherical central body.
const UtVec3d& OrbitalState::GetAccelerationInertial() const
{
   if (!mAccelerationValid && mCentralPointPtr->IsCentralBody())
   {
      SetAccelerationInertial(
         mCentralPointPtr->GetAsCentralBody().GetAccelerationInertial(mOrbitalStateVector.GetLocation()));
   }
   return mAcceleration;
}

// Deprecated
const UtVec3d& OrbitalState::GetAcceleration() const
{
   return GetAccelerationInertial();
}

//! Set the reference frame of the state.  If the state is currently valid, the orbital state vector will transform to
//! the new reference frame.
void OrbitalState::SetReferenceFrame(ReferenceFrame aReferenceFrame)
{
   if (aReferenceFrame != mReferenceFrame)
   {
      if (IsValid())
      {
         mOrbitalStateVector       = GetOrbitalStateVector(aReferenceFrame);
         mOrbitalStateVectorValid  = true;
         mOrbitalElementsValid     = false;
         mOrbitalElementsTOD_Valid = false;
      }
      mReferenceFrame = aReferenceFrame;
   }
}

const CentralPoint& OrbitalState::GetCentralPoint() const
{
   return *mCentralPointPtr;
}

//! Get the central point as a central body.  If the central point does not derive from ut::CentralBody, an exception
//! will be thrown.
const ut::CentralBody& OrbitalState::GetCentralBody() const
{
   return mCentralPointPtr->GetAsCentralBody();
}

//! Set the central body.
//! @note This method is to be called as part of input configuration only.
//! If dealing with an active state, call TransformTo() instead.
void OrbitalState::SetCentralPoint(const CentralPoint& aCentralPoint)
{
   mCentralPointPtr = ut::clone(aCentralPoint);
   if (mCentralPointPtr->IsCentralBody())
   {
      mOrbitalElementsPtr->SetCentralBody(static_cast<const CentralBody&>(aCentralPoint));
   }
   else
   {
      mOrbitalElementsComputable = false;
   }
   if (mInertialConversionPtr != nullptr)
   {
      mInertialConversionPtr->SetCentralPoint(aCentralPoint);
   }
}

//! Transform from one central body to another.
//! @param aNewCentralPoint The CentralPoint to which the state should be transformed.
void OrbitalState::TransformTo(const CentralPoint& aNewCentralPoint)
{
   SetCoordinateSystem(CoordinateSystem::cEQUATORIAL);
   SetReferenceFrame(ReferenceFrame::cECI);
   mOrbitalStateVector = GetOrbitalStateVectorInertial(aNewCentralPoint);
   SetCentralPoint(aNewCentralPoint);
   mOrbitalElementsValid     = false;
   mOrbitalElementsTOD_Valid = false;
}

//! Perform a simple check to determine if the osculating orbit defined by this state intersects the
//! central body.
bool OrbitalState::OrbitIntersectsCentralBody() const
{
   bool intersects = false;
   if (mCentralPointPtr->IsCentralBody())
   {
      intersects = GetOrbitalElements().GetPeriapsisRadius() <= mCentralPointPtr->GetMeanRadius();
   }
   return intersects;
}

//! Set the coordinate system.
void OrbitalState::SetCoordinateSystem(CoordinateSystem aCoordinateSystem)
{
   if (mCoordinateSystem != aCoordinateSystem)
   {
      if (IsValid())
      {
         if (mCoordinateSystem == CoordinateSystem::cECLIPTIC)
         {
            // Convert ecliptic to equatorial
            mOrbitalStateVector = ut::CentralBody::ConvertEclipticToEquatorial(mEpoch, GetOrbitalStateVector());
         }
         else
         {
            // convert equatorial to ecliptic
            mOrbitalStateVector = ut::CentralBody::ConvertEclipticToEquatorial(mEpoch, GetOrbitalStateVector());
         }
      }
      mCoordinateSystem = aCoordinateSystem;
   }
}

//! Return the orbital state vector for the current central body, and epoch, referenced to the given reference frame.
OrbitalState::Vector OrbitalState::GetOrbitalStateVector(ReferenceFrame aReferenceFrame) const
{
   Vector orbitalStateVector{GetOrbitalStateVector()};
   if (aReferenceFrame != mReferenceFrame)
   {
      UtECI_Conversion& conv = GetInertialConversion();
      switch (aReferenceFrame)
      {
      case ReferenceFrame::cECI:
         orbitalStateVector.Set(conv.GetLocationECI(), conv.GetVelocityECI());
         break;
      case ReferenceFrame::cTRUE_OF_DATE:
         orbitalStateVector.Set(conv.GetLocationTOD(), conv.GetVelocityTOD());
         break;
      case ReferenceFrame::cTEME:
         orbitalStateVector.Set(conv.GetLocationTEME(), conv.GetVelocityTEME());
         break;
      // case ReferenceFrame::cMEAN_OF_DATE:
      //    orbitalStateVector.Set(conv.GetLocationMOD(), conv.GetVelocityTOD());  // need getvelocitymod (TODO)
      //    break;
      case ReferenceFrame::cJ2000:
         orbitalStateVector.Set(conv.GetLocationJ2000(), conv.GetVelocityJ2000());
         break;
      default:
         throw UtException("ut::OrbitalState::GetOrbitalStateVector: Unsupported reference frame");
      }
   }
   return orbitalStateVector;
}

//! Convert the current inertial orbital state vector to reference the provided new central body reference and provide
//! the result in the returned orbital state vector. If the central body is earth, the coordinates will be ECI (GCRF);
//! otherwise they will be inertial frame that is offset and rotated with respect to ECI.
//! @param aCentralPoint The central body reference for the output orbital state vector.
//! @return The inertial, equatorial, orbital state vector referenced to aCentralPoint.
OrbitalState::Vector OrbitalState::GetOrbitalStateVectorInertial(const CentralPoint& aCentralPoint) const
{
   OrbitalState::Vector osvInertial = GetOrbitalStateVectorInertial();

   // De-rotate by polar angles to ECI
   if (!mCentralPointPtr->IsEarth())
   {
      auto xform = mCentralPointPtr->ComputeECI_ToCentralPointInertialTransform(mEpoch);
      xform.Transpose();
      UtVec3d eciLocation = xform.Multiply(osvInertial.GetLocation());
      UtVec3d eciVelocity = xform.Multiply(osvInertial.GetVelocity());
      osvInertial.Set(eciLocation, eciVelocity);
   }

   // Translate to new body
   UtVec3d r12; // vector from the current central body to the new one.
   r12.Subtract(aCentralPoint.GetLocationECI(mEpoch), mCentralPointPtr->GetLocationECI(mEpoch));

   UtVec3d r; // new location vector
   r.Subtract(osvInertial.GetLocation(), r12);

   UtVec3d v12; // relative velocity of the new body with respect to the current one.
   v12.Subtract(aCentralPoint.GetVelocityECI(mEpoch), mCentralPointPtr->GetVelocityECI(mEpoch));

   UtVec3d v; // new velocity vector
   v.Subtract(osvInertial.GetVelocity(), v12);
   osvInertial.Set(r, v);

   // Rotate by polar angles to new central body equatorial plane
   if (!aCentralPoint.IsEarth())
   {
      auto    xform       = aCentralPoint.ComputeECI_ToCentralPointInertialTransform(mEpoch);
      UtVec3d eciLocation = xform.Multiply(osvInertial.GetLocation());
      UtVec3d eciVelocity = xform.Multiply(osvInertial.GetVelocity());
      osvInertial.Set(eciLocation, eciVelocity);
   }

   return osvInertial;
}

//! Return whether this orbital state has enough information to be valid.
bool OrbitalState::IsValid() const
{
   return ((mCentralPointPtr != nullptr) && (mOrbitalElementsValid || mOrbitalStateVectorValid) &&
           (mCoordinateSystem != CoordinateSystem::cUNDEFINED_COORDINATE_SYSTEM) &&
           (mReferenceFrame != ReferenceFrame::cUNDEFINED_REFERENCE_FRAME)) &&
          (!mDeclaredWithOrbitalState || mEpochValid || mTLE_Input);
}

//! @brief Initialize the orbital state.
//! @param aEpoch The epoch to be used with the orbital state, in case an epoch has not
//! already been associated with it.  Otherwise, this epoch will not be used, and false will be
//! returned if the epoch has not been set in a constructor or through ProcessInput.
//! @return Whether the initialization was successful.
bool OrbitalState::Initialize(const UtCalendar& aEpoch)
{
   bool initialized = mCentralPointPtr->Initialize(aEpoch);
   if (initialized)
   {
      mOrbitalElementsComputable = mCentralPointPtr->IsCentralBody();
      mOrbitalElementsValid      = mOrbitalElementsComputable && mOrbitalElementsPtr->IsValid();

      // in an orbital_state block, epoch declaration is mandatory aside from TLE input
      if (mDeclaredWithOrbitalState && !mEpochValid && !mTLE_Input)
      {
         initialized = false;
      }
      else if (mOrbitalElementsValid)
      {
         if (mOrbitalElementsPtr->Initialize(aEpoch))
         {
            mEpoch      = mOrbitalElementsPtr->GetEpoch();
            mEpochValid = true;
            InitializeReferenceFrame();
         }
         else
         {
            initialized = false;
         }
      }
      else if (mOrbitalStateVectorValid)
      {
         if (!mEpochValid)
         {
            mEpoch      = aEpoch;
            mEpochValid = true;
         }
      }
      else
      {
         initialized = false;
      }
   }
   return initialized && IsValid();
}

//! Set the orbital state vector.
//! @param aVector The orbital state vector.
//! @note This method is supplied for special cases where the vector is known before
//! the epoch can be provided.
void OrbitalState::SetOrbitalStateVector(const Vector& aVector)
{
   mOrbitalStateVector       = aVector;
   mEpochValid               = false;
   mOrbitalStateVectorValid  = true;
   mOrbitalElementsValid     = false;
   mOrbitalElementsTOD_Valid = false;
   mAccelerationValid        = false;
}

//! Return the epoch at which the orbital state is valid.
const UtCalendar& OrbitalState::GetEpoch() const
{
   return mEpoch;
}

// TODO This should be removed as it means a two-step initialization.
void OrbitalState::SetEpoch(const UtCalendar& aEpoch)
{
   mEpoch = aEpoch;
   if (mOrbitalElementsValid)
   {
      mOrbitalElementsPtr->SetEpoch(aEpoch);
      mCentralPointPtr->Initialize(aEpoch);
   }
   mEpochValid = true;
}

//! Set the state from an orbital state vector.
//! @note It is the responsibility of the caller to insure
//! that the reference frame and coordinate frame are consistent.
void OrbitalState::Set(const UtCalendar& aEpoch, const Vector& aVector)
{
   mEpoch                    = aEpoch;
   mEpochValid               = true;
   mOrbitalStateVector       = aVector;
   mOrbitalElementsValid     = false;
   mOrbitalElementsTOD_Valid = false;
   mOrbitalStateVectorValid  = true;
   mAccelerationValid        = false;
}

void OrbitalState::SetInertial(const UtCalendar& aEpoch, const Vector& aVector)
{
   SetReferenceFrame(ReferenceFrame::cECI);
   SetEpoch(aEpoch);
   SetOrbitalStateVector(aVector);
}

//! Set the state from a two-line element.
//! @note It is the responsibility of the caller to insure
//! that the reference frame and coordinate frame are consistent.
void OrbitalState::Set(const UtOrbitalElements& aElements)
{
   mOrbitalElementsPtr       = ut::clone(aElements);
   mEpoch                    = aElements.GetEpoch();
   mEpochValid               = true;
   mOrbitalElementsValid     = true;
   mOrbitalElementsTOD_Valid = false;
   mOrbitalStateVectorValid  = false;
   mAccelerationValid        = false;
}

//! Set the orbital elements as referenced to TOD
void OrbitalState::SetTOD(const UtOrbitalElements& aElements)
{
   if (mReferenceFrame == ReferenceFrame::cTRUE_OF_DATE)
   {
      Set(aElements);
   }
   else
   {
      UtVec3d locTOD, velTOD;
      aElements.ComputeState(locTOD, velTOD);
      SetEpoch(aElements.GetEpoch());
      UtECI_Conversion conv = GetInertialConversion();
      conv.SetLocationTOD(locTOD);
      conv.SetVelocityTOD(velTOD);

      switch (mReferenceFrame)
      {
      case ReferenceFrame::cECI:
         mOrbitalStateVector.Set(conv.GetLocationECI(), conv.GetVelocityECI());
         break;
      case ReferenceFrame::cTEME:
         mOrbitalStateVector.Set(conv.GetLocationTEME(), conv.GetVelocityTEME());
         break;
      case ReferenceFrame::cJ2000:
         mOrbitalStateVector.Set(conv.GetLocationJ2000(), conv.GetVelocityJ2000());
         break;
      default:
         throw UtException("ut::OrbitalState::SetTOD: Unsupported reference frame");
      }

      mOrbitalElementsValid     = false;
      mOrbitalElementsTOD_Valid = false;
      mOrbitalStateVectorValid  = true;
      mAccelerationValid        = false;
   }
}

//! Given a source orbital state, set the state in the target, but convert between reference frames if the two are different.
void OrbitalState::Set(const OrbitalState& aState)
{
   auto vector         = aState.GetOrbitalStateVector();
   auto referenceFrame = aState.GetReferenceFrame();

   if (mEpoch != aState.mEpoch)
   {
      mEpoch = aState.mEpoch;
   }
   mEpochValid = true;

   if (mCoordinateSystem != aState.mCoordinateSystem)
   {
      // Currently assuming central bodies are the same.
      if ((aState.mCoordinateSystem == CoordinateSystem::cEQUATORIAL) && (mCoordinateSystem == CoordinateSystem::cECLIPTIC))
      {
         // convert Equatorial->Ecliptic
         // For simplicity, always use J2000 to do the conversion.
         referenceFrame = ReferenceFrame::cECI;
         UtCalendar epoch;
         epoch.SetJ2000_Date(0.0);
         vector = ut::CentralBody::ConvertEquatorialToEcliptic(epoch, aState.GetOrbitalStateVectorInertial());

         // TODO support conversions from one central body to another (earth->sun, etc.)
         if (mCentralPointPtr->GetName() != aState.mCentralPointPtr->GetName())
         {
            auto warning = ut::log::warning() << "ut::OrbitalState::Set: Equatorial->Ecliptic conversions not "
                                                 "supported between two different central bodies";
            warning.AddNote() << mCentralPointPtr->GetName() << "->" << aState.mCentralPointPtr->GetName();
         }
      }
      else if ((aState.mCoordinateSystem == CoordinateSystem::cECLIPTIC) &&
               (mCoordinateSystem == CoordinateSystem::cEQUATORIAL))
      {
         // Convert ecliptic->equatorial
         // For simplicity, always use J2000 to do the conversion.
         referenceFrame = ReferenceFrame::cECI;
         vector         = aState.GetOrbitalStateVectorInertial();
         UtCalendar epoch;
         epoch.SetJ2000_Date(0.0);

         vector = ut::CentralBody::ConvertEclipticToEquatorial(epoch, aState.GetOrbitalStateVectorInertial());

         // TODO support conversions from one central body to another (earth->sun, etc.)
         if (mCentralPointPtr->GetName() != aState.mCentralPointPtr->GetName())
         {
            auto warning = ut::log::warning() << "ut::OrbitalState::Set: Ecliptic->Equatorial conversions not "
                                                 "supported between two different central bodies";
            warning.AddNote() << mCentralPointPtr->GetName() << "->" << aState.mCentralPointPtr->GetName();
         }
      }
   }

   if (mReferenceFrame != referenceFrame)
   {
      UtVec3d           newLocation;
      UtVec3d           newVelocity;
      UtECI_Conversion& conv = GetInertialConversion();
      mOrbitalElementsValid  = false;

      switch (referenceFrame)
      {
      case ReferenceFrame::cECI:
         conv.SetLocationECI(vector.GetLocation());
         conv.SetVelocityECI(vector.GetVelocity());
         if (aState.mAccelerationValid)
         {
            conv.SetAccelerationECI(aState.mAcceleration);
         }
         break;
      case ReferenceFrame::cTRUE_OF_DATE:
         conv.SetLocationTOD(vector.GetLocation());
         conv.SetVelocityTOD(vector.GetVelocity());
         if (aState.mAccelerationValid)
         {
            conv.SetAccelerationTOD(aState.mAcceleration);
         }
         break;
      case ReferenceFrame::cTEME:
         conv.SetLocationTEME(vector.GetLocation());
         conv.SetVelocityTEME(vector.GetVelocity());
         if (aState.mAccelerationValid)
         {
            conv.SetAccelerationTOD(aState.mAcceleration); // TODO acceleration TEME
         }
         break;
      case ReferenceFrame::cJ2000:
         conv.SetLocationJ2000(vector.GetLocation());
         conv.SetVelocityJ2000(vector.GetVelocity());
         if (aState.mAccelerationValid)
         {
            conv.SetAccelerationECI(aState.mAcceleration); // TODO acceleration J2000
         }
         break;
      default:
         throw UtException("ut::OrbitalState::Set: Unsupported reference frame");
      }
      switch (mReferenceFrame)
      {
      case ReferenceFrame::cECI:
         newLocation = conv.GetLocationECI();
         newVelocity = conv.GetVelocityECI();
         if (aState.mAccelerationValid)
         {
            mAcceleration = conv.GetAccelerationECI();
         }
         break;
      case ReferenceFrame::cTRUE_OF_DATE:
         newLocation = conv.GetLocationTOD();
         newVelocity = conv.GetVelocityTOD();
         if (aState.mAccelerationValid)
         {
            mAcceleration = conv.GetAccelerationTOD();
         }
         break;
      case ReferenceFrame::cTEME:
         newLocation = conv.GetLocationTEME();
         newVelocity = conv.GetVelocityTEME();
         if (aState.mAccelerationValid)
         {
            mAcceleration = conv.GetAccelerationTOD(); // TODO acceleration TEME
         }
         break;
      case ReferenceFrame::cJ2000:
         newLocation = conv.GetLocationJ2000();
         newVelocity = conv.GetVelocityJ2000();
         if (aState.mAccelerationValid)
         {
            mAcceleration = conv.GetAccelerationECI(); // TODO acceleration J2000
         }
         break;
      default:
         throw UtException("ut::OrbitalState::Set: Unsupported reference frame");
      }

      mOrbitalStateVector.Set(newLocation, newVelocity);
   }
   else // Same reference frame
   {
      mOrbitalStateVector = vector;
      if (aState.mAccelerationValid)
      {
         mAcceleration = aState.mAcceleration;
      }
   }

   if ((mCoordinateSystem == CoordinateSystem::cEQUATORIAL) &&
       (mCentralPointPtr->GetName() != aState.mCentralPointPtr->GetName()))
   {
      auto         osvi = aState.GetOrbitalStateVectorInertial(*mCentralPointPtr);
      OrbitalState inertialState(*mCentralPointPtr, CoordinateSystem::cEQUATORIAL, ReferenceFrame::cECI);
      inertialState.Set(aState.mEpoch, osvi);
      Set(inertialState);
   }

   if ((aState.mOrbitalElementsPtr->GetDesignator() != UtOrbitalElements::GetDefaultDesignator()) &&
       (aState.mOrbitalElementsPtr->GetDesignator() != mOrbitalElementsPtr->GetDesignator()))
   {
      // Copy the TLE as we may need the non-orbital element data later
      mOrbitalElementsPtr = aState.mOrbitalElementsPtr;
   }

   mOrbitalElementsValid     = false;
   mOrbitalStateVectorValid  = true;
   mOrbitalElementsTOD_Valid = false;
   mAccelerationValid        = aState.mAccelerationValid;
}

// private
//! Initializes the reference frame if it wasn't set explicitly by the user.
void OrbitalState::InitializeReferenceFrame()
{
   if (mReferenceFrame == ReferenceFrame::cUNDEFINED_REFERENCE_FRAME)
   {
      // Orbital elements processed from input implies a TEME or TOD reference frame.
      if (mOrbitalElementsPtr->HasMeanElements())
      {
         mReferenceFrame = ReferenceFrame::cTEME;
      }
      else
      {
         mReferenceFrame = ReferenceFrame::cTRUE_OF_DATE;
      }
      // Both of these imply an equatorial coordinate system
      SetCoordinateSystem(CoordinateSystem::cEQUATORIAL);
   }
}

// private
//! Get the inertial conversion object.  The returned reference will automatically be updated based on the current orbital state.
UtECI_Conversion& OrbitalState::GetInertialConversion() const
{
   if (mInertialConversionPtr == nullptr)
   {
      mInertialConversionPtr = ut::make_unique<UtECI_Conversion>(mEpoch, *mCentralPointPtr);
   }
   UpdateInertialConversion();

   return *mInertialConversionPtr;
}

// private
//! Update the inertial conversion such that it can provide conversions based on the latest state information.
void OrbitalState::UpdateInertialConversion() const
{
#undef GetCurrentTime // Avoid conflict with Windows macro
   double                  dt = mEpoch.GetJ2000_Date() - mInertialConversionPtr->GetCurrentTime().GetJ2000_Date();
   static constexpr double cTIME_TOLERANCE = 1.0e-9 / UtCalendar::cSEC_IN_DAY;
   // Use absolute value here to handle updating the state both forward and backward in time.
   if (std::abs(dt) > cTIME_TOLERANCE)
   {
      mInertialConversionPtr->SetTime(mEpoch.GetTimeSince(mInertialConversionPtr->GetEpoch()));
   }

   if (IsValid())
   {
      const Vector& osv = GetOrbitalStateVector();

      switch (mReferenceFrame)
      {
      case ReferenceFrame::cECI:
         mInertialConversionPtr->SetLocationECI(osv.GetLocation());
         mInertialConversionPtr->SetVelocityECI(osv.GetVelocity());
         break;
      case ReferenceFrame::cTRUE_OF_DATE:
         mInertialConversionPtr->SetLocationTOD(osv.GetLocation());
         mInertialConversionPtr->SetVelocityTOD(osv.GetVelocity());
         break;
      case ReferenceFrame::cTEME:
         mInertialConversionPtr->SetLocationTEME(osv.GetLocation());
         mInertialConversionPtr->SetVelocityTEME(osv.GetVelocity());
         break;
      case ReferenceFrame::cJ2000:
         mInertialConversionPtr->SetLocationJ2000(osv.GetLocation());
         mInertialConversionPtr->SetVelocityJ2000(osv.GetVelocity());
         break;
      default:
         throw UtException("ut::OrbitalState::UpdateInertialConversion: Unsupported reference frame");
      }
   }
}

} // namespace ut
