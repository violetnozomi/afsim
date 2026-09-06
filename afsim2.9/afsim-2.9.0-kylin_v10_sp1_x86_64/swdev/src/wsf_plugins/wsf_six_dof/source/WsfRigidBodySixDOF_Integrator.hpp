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

#ifndef WSFRIGIDBODYSIXDOFINTEGRATOR_HPP
#define WSFRIGIDBODYSIXDOFINTEGRATOR_HPP

#include "WsfSixDOF_Integrator.hpp"

namespace wsf
{
namespace six_dof
{
class ForceAndMomentsObject;
class RigidBodyMover;

// RigidBodyIntegrator integrates the state of RigidBodySixDOF_Objects.
class WSF_SIX_DOF_EXPORT RigidBodyIntegrator : public Integrator
{
public:
   explicit RigidBodyIntegrator(RigidBodyMover&);
   RigidBodyIntegrator()        = default;
   ~RigidBodyIntegrator()       = default;
   RigidBodyIntegrator& operator=(const RigidBodyIntegrator& other) = delete;

   RigidBodyIntegrator* Clone() const;

   void SetParentVehicle(RigidBodyMover* aVehicle);

   void Update(int64_t aSimTime_nanosec, double aDeltaT_sec) const override;

protected:
   RigidBodyIntegrator(const RigidBodyIntegrator& other) = default;

   Mover* GetParentVehicle() const override;

   // This is called to calculate forces and moments
   void CalculateFM(KinematicState&        aState,
                    int64_t                aSimTime_nanosec,
                    double                 aDeltaT_sec,
                    ForceAndMomentsObject& aForcesMomentsAtRP,
                    ForceAndMomentsObject& aForcesMomentsAtCM) const;

   // This adds support for the simple yaw damper to the base rotation propagation
   void PropagateRotation(KinematicState& aState, UtVec3dX aRotationalAccel_rps2, double aDeltaT_sec) const override;

   // This performs calculations (not updates) to propagate through the specified delta t.
   // It is used in the Update() function to obtain an intermediate step in the integration
   // process. It does not perform calculations on all state parameters, rather it is limited
   // to those parameters that will impact changes in aero, propulsion, and/or gravity F&M.
   void PropagateUsingFM(KinematicState&       aState,
                         const MassProperties& aMassProperties,
                         double                aDeltaT_sec,
                         ForceAndMomentsObject aForcesMomentsAtRP,
                         ForceAndMomentsObject aForcesMomentsAtCM) const;

   // This performs a full integration "update" (not just calculate) including changing state
   // as well as updating all state parameters, including the additional "secondary" data,
   // as required.
   void UpdateUsingFM(KinematicState&              aState,
                      const MassProperties&        aMassProperties,
                      int64_t                      aSimTime_nanosec,
                      double                       aDeltaT_sec,
                      const ForceAndMomentsObject& aForcesMomentsAtRP,
                      const ForceAndMomentsObject& aForcesMomentsAtCM) const;

private:
   RigidBodyMover* mVehicle = nullptr;
};
} // namespace six_dof
} // namespace wsf


#endif
