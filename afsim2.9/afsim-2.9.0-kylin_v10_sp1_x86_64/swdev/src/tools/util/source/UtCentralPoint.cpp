// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtCentralPoint.hpp"

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace ut
{

//! Return the Inertial velocity of this central point.
//! @note The default implementation is to return the difference between the central
//! point at aEpoch, and the central body at aEpoch + 1 second (vector difference / 1 s).
UtVec3d CentralPoint::GetVelocityECI(const UtCalendar& aEpoch) const
{
   UtCalendar epoch{aEpoch};
   auto       locationAtEpoch = GetLocationECI(epoch);
   epoch.AdvanceTimeBy(1.0); // seconds
   auto locationAtNextSec = GetLocationECI(epoch);
   return locationAtNextSec.Subtract(locationAtEpoch);
}

//! @brief Compute the rotating to inertial transform.
//! @param aEpoch The epoch at which the transform is valid.
//! @return The rotating to inertial transform.
UtMat3d CentralPoint::ComputeSiderealMotion(const UtCalendar& aEpoch) const
{
   auto   siderealRotationAngle    = ComputeSiderealAngle(aEpoch);
   double cosSiderealRotationAngle = cos(siderealRotationAngle);
   double sinSiderealRotationAngle = sin(siderealRotationAngle);

   UtMat3d rotatingToInertialTransform;
   rotatingToInertialTransform[0] = cosSiderealRotationAngle;
   rotatingToInertialTransform[1] = -sinSiderealRotationAngle;
   rotatingToInertialTransform[3] = sinSiderealRotationAngle;
   rotatingToInertialTransform[4] = cosSiderealRotationAngle;
   rotatingToInertialTransform[8] = 1.0;
   return rotatingToInertialTransform;
}

//! Return the gravitational parameter associated with this central point.
//! @note An error message will be output if the object is not a ut::CentralBody.
double CentralPoint::GetGravitationalParameter() const
{
   ut::log::error() << "CentralPoint::Ellipsoid: There is no defined gravitational parameter for Central Point: " +
                          std::string(GetName());
   return 0.0;
}

//! Return the mean radius associated with this central point.
//! @note If the object is not a ut::CentralBody, 0.0 will be returned.
double CentralPoint::GetMeanRadius() const
{
   return 0.0; // Not associated with an central body ellipsoid.
}

//! Compute the central point inertial to Earth-Centered Inertial (ECI) transform for this CentralPoint.
//! @param aEpoch The epoch at which the transform is valid.
//! @return The central point inertial to Earth-Centered Inertial (ECI) transform.
UtMat3d CentralPoint::ComputeCentralPointInertialToECI_Transform(const UtCalendar& aEpoch) const
{
   auto xform = ComputeECI_ToCentralPointInertialTransform(aEpoch);
   xform.Transpose();
   return xform;
}

//! Compute the sidereal angle to be used in computation of sidereal motion.
//! @param aTime The time at which the angle is computed.
double CentralPoint::ComputeSiderealAngle(const UtCalendar& aTime) const
{
   return (UtMath::NormalizeAngle0_TwoPi(GetRotationRate() * UtCalendar::cSEC_IN_DAY * aTime.GetJ2000TT_Date() +
                                         GetSiderealOffsetAngle(aTime)));
}

//! Process input from a common source.
//! @return Whether any input was processed.
bool CentralPoint::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = false;

   if ((command == "rotation_rate") || (command == "mean_motion"))
   {
      aInput.ReadValueOfType(mRotationRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mRotationRate, 0.0);
      myCommand = true;
   }

   return myCommand;
}

//! A simple factory method to create an instance of the named central body.
//! Other central points could be created in addition to the ones enumerated in CentralBody::Create.
//! @param aName The name of the CentralPoint type to create.
//! @return The desired CentralPoint type, or nullptr if aName is not one of the recognized central point types.
std::unique_ptr<CentralPoint> CentralPoint::Create(const std::string& aName)
{
   std::unique_ptr<CentralPoint> centralPointPtr = CentralBody::Create(aName);
   return centralPointPtr;
}

//! @brief Return the CentralPoint as a CentralBody.
//! If the CentralPoint is not a CentralBody, an exception will be thrown.
//! @return The CentralBody object associated with this CentralPoint.
const CentralBody& CentralPoint::GetAsCentralBody() const
{
   if (IsCentralBody())
   {
      return static_cast<const ut::CentralBody&>(*this);
   }
   else
   {
      std::string what  = "ut::CentralPoint::GetAsCentralBody: Central point is not a central body.";
      auto        fatal = ut::log::fatal() << what;
      fatal.AddNote() << GetName();
      throw UtException(what);
   }
}

//! @brief Return the CentralPoint as a CentralBody (non-const version).
//! If the CentralPoint is not a CentralBody, an exception will be thrown.
//! @return The CentralBody object associated with this CentralPoint.
CentralBody& CentralPoint::GetAsCentralBody()
{
   if (IsCentralBody())
   {
      return static_cast<ut::CentralBody&>(*this);
   }
   else
   {
      std::string what  = "ut::CentralPoint::GetAsCentralBody: Central point is not a central body.";
      auto        fatal = ut::log::fatal() << what;
      fatal.AddNote() << GetName();
      throw UtException(what);
   }
}

} // namespace ut
