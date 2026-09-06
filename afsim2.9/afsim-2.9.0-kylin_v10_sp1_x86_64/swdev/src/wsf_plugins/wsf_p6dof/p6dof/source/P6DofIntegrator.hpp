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

#ifndef P6DOFINTEGRATOR_H
#define P6DOFINTEGRATOR_H

#include "p6dof_export.h"

#include <cstdint>

#include "UtVec3dX.hpp"

class P6DofForceAndMomentsObject;
class P6DofKinematicState;
class P6DofMassProperties;
class P6DofVehicle;

// P6DofIntegrator integrates the state of P6DofObjects.
class P6DOF_EXPORT P6DofIntegrator
{
public:
   P6DofIntegrator()        = default;
   ~P6DofIntegrator()       = default;
   P6DofIntegrator& operator=(const P6DofIntegrator& other) = delete;

   P6DofIntegrator* Clone() const;

   static void Update(P6DofVehicle* aObject, int64_t aSimTime_nanosec, double aDeltaT_sec);

protected:
   P6DofIntegrator(const P6DofIntegrator& other) = default;

   // This is called to calculate forces and moments
   static void CalculateFM(P6DofVehicle*               aObject,
                           P6DofKinematicState&        aState,
                           int64_t                     aSimTime_nanosec,
                           double                      aDeltaT_sec,
                           P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                           P6DofForceAndMomentsObject& aForcesMomentsAtCM);

   // This is called to update fuel burn
   static void UpdateFuelBurn(P6DofVehicle* aObject, P6DofKinematicState& aState, int64_t aSimTime_nanosec, double aDeltaT_sec);

   // This is called to propagate translation
   static void PropagateTranslation(P6DofVehicle*        aObject,
                                    P6DofKinematicState& aState,
                                    const UtVec3dX&      aInertialAccel_mps2,
                                    double               aDeltaT_sec);

   // This is called by PropagateTranslation for a spherical earth model
   static void PropagateTranslationSphericalEarth(P6DofVehicle*        aObject,
                                                  P6DofKinematicState& aState,
                                                  UtVec3dX             aInertialAccel_mps2,
                                                  double               aDeltaT_sec);

   // This is called by PropagateTranslation for a WGS earth model
   static void PropagateTranslationWGSEarth(P6DofVehicle*        aObject,
                                            P6DofKinematicState& aState,
                                            UtVec3dX             aInertialAccel_mps2,
                                            double               aDeltaT_sec);

   // This is called to propagate rotation
   static void PropagateRotation(P6DofVehicle*        aObject,
                                 P6DofKinematicState& aState,
                                 UtVec3dX             aRotationalAccel_rps2,
                                 double               aDeltaT_sec);

   // This performs calculations (not updates) to propagate through the specified delta t.
   // It is used in the Update() function to obtain an intermediate step in the integration
   // process. It does not perform calculations on all state parameters, rather it is limited
   // to those parameters that will impact changes in aero, propulsion, and/or gravity F&M.
   static void PropagateUsingFM(P6DofVehicle*               aObject,
                                P6DofKinematicState&        aState,
                                const P6DofMassProperties&  aMassProperties,
                                double                      aDeltaT_sec,
                                P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                                P6DofForceAndMomentsObject& aForcesMomentsAtCM);

   // This performs a full integration "update" (not just calculate) including changing state
   // as well as updating all state parameters, including the additional "secondary" data,
   // as required.
   static void UpdateUsingFM(P6DofVehicle*               aObject,
                             P6DofKinematicState&        aState,
                             const P6DofMassProperties&  aMassProperties,
                             int64_t                     aSimTime_nanosec,
                             double                      aDeltaT_sec,
                             P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                             P6DofForceAndMomentsObject& aForcesMomentsAtCM);
};

#endif
