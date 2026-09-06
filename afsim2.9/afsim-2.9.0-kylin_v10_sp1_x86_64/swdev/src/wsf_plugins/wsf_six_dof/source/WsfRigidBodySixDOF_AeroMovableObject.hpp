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

#ifndef WSFRIGIDBODYSIXDOFAEROMOVABLEOBJECT_HPP
#define WSFRIGIDBODYSIXDOFAEROMOVABLEOBJECT_HPP

#include <string>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtTable.hpp"
class UtVec3dX;

namespace wsf
{
namespace six_dof
{
// The RigidBodyAeroMovableObject provides supplemental aerodynamics for an object,
// beyond the 'core' aerodynamics of RigidBodyAeroCoreObject. It is used for
// aerodynamics for components, landing gear, flaps, speed brakes, etc.

class RigidBodyAeroMovableObject
{
public:
   RigidBodyAeroMovableObject()  = default;
   ~RigidBodyAeroMovableObject() = default;

   RigidBodyAeroMovableObject& operator=(const RigidBodyAeroMovableObject& other) = delete;

   RigidBodyAeroMovableObject* Clone() const;

   // The SetExtendedValue function determines the extended amount of the item (0.0-1.0)
   void SetExtendedValue(double aExtendedValue);

   // The SetAngle_deg function determines the angle the item (typically, retracted = 0.0)
   void SetAngle_deg(double aAngle_deg);

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);
   bool Initialize();

   // This sets the handle for the control surface (or other aero part)
   void SetControlSurfaceHandle(size_t aHandle);

   // This gets the handle for the control surface (or other aero part)
   size_t GetControlSurfaceHandle() const;

   // Returns the name string
   std::string GetName() const;

   // Either SetReferenceArea or SetReferenceAreas should be called during initialization
   void SetReferenceArea(double aRefArea_sqft);

   // Either SetReferenceArea or SetReferenceAreas should be called during initialization
   void SetReferenceAreas(double aWingChord_ft, double aWingSpan_ft, double aWingArea_sqft);

   // The CalculateMovableAeroFM function is called to calculate the aerodynamic
   // forces and moments. The aLiftFactor is used for spoilers, which have the
   // potential to spoil lift for the entire vehicle.
   void CalculateMovableAeroFM(double          aDynPress_lbsqft,
                               double          aMach,
                               double          aSpeed_fps,
                               double          aAlpha_rad,
                               double          aBeta_rad,
                               const UtVec3dX& aAngularRates_rps,
                               UtVec3dX&       aMoment_ftlbs,
                               double&         aLift_lbs,
                               double&         aDrag_lbs,
                               double&         aSideForce_lbs,
                               double&         aLiftFactor,
                               double          aAngle_deg);

   // This calculates the pitching moment (Cm) including reference area effects
   // but not including dynamic pressure effects.
   double CalculateMovableAeroCmArea(double aMach, double aAlpha_rad, double aAngle_deg) const;

   // This calculates the lift coefficient (CL) including reference area effects
   // but not including dynamic pressure effects.
   double CalculateMovableAeroCLArea(double aMach, double aAlpha_rad, double aAngle_deg) const;

   // This calculates the lift coefficient (CL) without considering reference
   // area effects.
   double CalculateMovableAeroCL(double aMach, double aAlpha_rad, double aAngle_deg) const;

   // This calculates the drag coefficient (Cd) including reference area effects
   // but not including dynamic pressure effects.
   double CalculateMovableAeroCdArea(double aMach, double aAlpha_rad, double aAngle_deg) const;

protected:
   RigidBodyAeroMovableObject(const RigidBodyAeroMovableObject& aSrc) = default;

   // Lift
   double CL_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const;

   // Drag
   double Cd_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const; // This should be used when drag
                                                                                       // is only a function of alpha
   double Cd_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const; // This should be used when drag is
                                                                                     // only a function of beta
   double Cd_AngleMach(double aMach,
                       double aAngle_deg) const; // This should be used when drag is a function of alpha and beta

   // Side force
   double CY_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const;

   // Pitching moments
   double Cm_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const;
   double Cmq_AngleMach(double aMach, double aAngle_deg) const;

   // Yawing moments
   double Cn_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const;
   double Cnr_AngleMach(double aMach, double aAngle_deg) const;

   // Rolling moments
   double Cl_AngleAlphaBeta(double aAlpha_rad, double aBeta_rad, double aAngle_deg) const;
   double Clp_AngleMach(double aMach, double aAngle_deg) const;
   double Clq_AngleMach(double aMach, double aAngle_deg) const;
   double Clr_AngleMach(double aMach, double aAngle_deg) const;

   std::string mName;

   bool   mUseExternalRefArea   = false;
   bool   mUseInternalRefArea   = false;
   double mRefExternalArea_sqft = 0.0;
   double mRefInternalArea_sqft = 0.0;
   double mRefWingChord_ft      = 0.0;
   double mRefWingSpan_ft       = 0.0;
   double mRefWingArea_sqft     = 0.0;
   double mExtendedValue        = 0.0;
   double mAngle_deg            = 0.0;

   size_t mControlSurfaceHandle = 0;

   // This "reduced frequency" flag allows the use of reduced frequency rather than angular rates to compute aerodynamic derivatives
   bool mUseReducedFrequency = true;

   // Lift
   UtCloneablePtr<UtTable::Table> mCL_AngleAlphaMachTablePtr{nullptr};

   // Drag - Only one of these should be used
   UtCloneablePtr<UtTable::Table> mCd_AngleAlphaMachTablePtr{
      nullptr}; // This should be used when drag is only a function of alpha
   UtCloneablePtr<UtTable::Table> mCd_AngleBetaMachTablePtr{
      nullptr}; // This should be used when drag is only a function of beta
   UtCloneablePtr<UtTable::Table> mCd_AngleMachTablePtr{
      nullptr}; // This should be used when drag is a function of alpha and beta

   // Side force
   UtCloneablePtr<UtTable::Table> mCY_AngleBetaMachTablePtr{nullptr};

   // Pitching moments
   UtCloneablePtr<UtTable::Table> mCm_AngleAlphaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCmq_AngleMachTablePtr{nullptr};

   // Yawing moments
   UtCloneablePtr<UtTable::Table> mCn_AngleBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCnr_AngleMachTablePtr{nullptr};

   // Rolling moments
   UtCloneablePtr<UtTable::Table> mCl_AngleAlphaBetaTablePtr{
      nullptr}; // This replaces the Mach-based version, instead using alpha/beta
   UtCloneablePtr<UtTable::Table> mClp_AngleMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mClq_AngleMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mClr_AngleMachTablePtr{nullptr};
};
} // namespace six_dof
} // namespace wsf

#endif
