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

#ifndef WSFSCRIPTRIGIDBODYSIXDOFMOVER_HPP
#define WSFSCRIPTRIGIDBODYSIXDOFMOVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_Mover.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
//! WsfScriptRigidBodySixDOF_Mover is a WsfScriptObjectClass that defines the methods in
//! WsfRigidBodySixDOF_Mover.  This makes the methods available for use in script.
class WSF_SIX_DOF_EXPORT RigidBodyMover : public Mover
{
public:
   RigidBodyMover(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // -----------------------------------------------------------------------

   // LANDING GEAR AND BRAKING SUPPORT

   // Landing Gear
   UT_DECLARE_SCRIPT_METHOD(RetractLandingGear);
   UT_DECLARE_SCRIPT_METHOD(LowerLandingGear);
   UT_DECLARE_SCRIPT_METHOD(GetLandingGearPosition); // Angle in degrees

   // Weight on Wheels
   UT_DECLARE_SCRIPT_METHOD(WeightOnWheels);
   UT_DECLARE_SCRIPT_METHOD(WeightOnNoseWheel);

   // Nose Wheel Steering (NWS)
   UT_DECLARE_SCRIPT_METHOD(SetEnableNWS);
   UT_DECLARE_SCRIPT_METHOD(GetNoseWheelAngle); // Angle in degrees

   // Brake Controls
   UT_DECLARE_SCRIPT_METHOD(EnableDirectBraking);
   UT_DECLARE_SCRIPT_METHOD(SetParkingBrake);
   UT_DECLARE_SCRIPT_METHOD(ApplyLeftGearBrake);
   UT_DECLARE_SCRIPT_METHOD(ApplyRightGearBrake);
   UT_DECLARE_SCRIPT_METHOD(ReleaseWheelBrakes);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectBraking);

   // -----------------------------------------------------------------------

   // EXTERNAL FORCES SUPPORT

   // This allows an external force (such as a ground tug or aircraft carrier
   // catapult) to apply a force to the nose gear. The angle is measured as
   // zero being forward and positive angles being to the right.
   UT_DECLARE_SCRIPT_METHOD(ApplyExternalForce); // Force in Newtons

   // This removes any and all external forces on the nose gear
   UT_DECLARE_SCRIPT_METHOD(RemoveExternalForce);

   // -----------------------------------------------------------------------

   // AUTOPILOT SUPPORT

   // Autopilot commands
   UT_DECLARE_SCRIPT_METHOD(SetTaxiMode);
   UT_DECLARE_SCRIPT_METHOD(SetTaxiRadius);

   // Current autopilot limits
   UT_DECLARE_SCRIPT_METHOD(GetTaxiSpeedMax);
   UT_DECLARE_SCRIPT_METHOD(GetTaxiYawRateMax);

   // Set autopilot limits
   UT_DECLARE_SCRIPT_METHOD(SetTaxiSpeedMax);
   UT_DECLARE_SCRIPT_METHOD(SetTaxiYawRateMax);

   // -----------------------------------------------------------------------

   // CONTROL INPUTS SUPPORT

   // Thrust Reverser Control
   UT_DECLARE_SCRIPT_METHOD(SetThrustReverserPosition);

   // Flight control inputs
   UT_DECLARE_SCRIPT_METHOD(GetThrustReverserPosition);   // Range of 0 to 1
   UT_DECLARE_SCRIPT_METHOD(GetLandingGearLeverPosition); // Range of 0 to 1

   // -----------------------------------------------------------------------

   // CONTROL SURFACE POSITIONS SUPPORT

   // This provides the angle (deg) of the specified control surface (string)
   UT_DECLARE_SCRIPT_METHOD(GetAngleOfControlSurface);

   // This returns an array of control surface names (strings)
   UT_DECLARE_SCRIPT_METHOD(GetListOfControlSurfaceNames);

   // These provide the angle (deg) of common control surfaces
   UT_DECLARE_SCRIPT_METHOD(GetAileronsBothPosition);
   UT_DECLARE_SCRIPT_METHOD(GetStabilatorLeftPosition);
   UT_DECLARE_SCRIPT_METHOD(GetStabilatorRightPosition);
   UT_DECLARE_SCRIPT_METHOD(GetRudderPosition);

   // -----------------------------------------------------------------------

   // FORCES AND MOMENTS (F&M) SUPPORT

   // Forces and Moments (F&M)
   UT_DECLARE_SCRIPT_METHOD(GetYawMoment);   // Yaw moment in Newton-meters
   UT_DECLARE_SCRIPT_METHOD(GetPitchMoment); // Pitch moment in Newton-meters
   UT_DECLARE_SCRIPT_METHOD(GetRollMoment);  // Roll moment in Newton-meters

   // -----------------------------------------------------------------------

   // TESTING SUPPORT

   // The following functions are for Testing and should not be documented,
   //  to hide them from normal users
   UT_DECLARE_SCRIPT_METHOD(GetRigidBodyTestSupportObject);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif
