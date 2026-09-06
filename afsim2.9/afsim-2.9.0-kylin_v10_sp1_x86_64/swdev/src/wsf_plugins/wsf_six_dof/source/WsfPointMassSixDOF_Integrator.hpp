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

#ifndef WSFPOINTMASSSIXDOFINTEGRATOR_HPP
#define WSFPOINTMASSSIXDOFINTEGRATOR_HPP

#include "WsfSixDOF_Integrator.hpp"

namespace wsf
{
namespace six_dof
{
class PointMassMover;
class ForceAndRotationObject;

// PointMassIntegrator integrates the state of PointMassSixDOF_Objects.
class WSF_SIX_DOF_EXPORT PointMassIntegrator : public Integrator
{
public:
   PointMassIntegrator() = default;
   explicit PointMassIntegrator(PointMassMover&);
   ~PointMassIntegrator()       = default;
   PointMassIntegrator& operator=(const PointMassIntegrator& other) = delete;

   PointMassIntegrator* Clone() const;

   void SetParentVehicle(PointMassMover* aVehicle);

   void Update(int64_t aSimTime_nanosec, double aDeltaT_sec) const override;

protected:
   PointMassIntegrator(const PointMassIntegrator& other) = default;

   Mover* GetParentVehicle() const override;

   // This is called to calculate forces and moments
   void CalculateAcceleration(KinematicState& aState,
                              int64_t         aSimTime_nanosec,
                              double          aDeltaT_sec,
                              UtVec3dX&       aGravitationalAccel_g,
                              UtVec3dX&       aTranslationalAccel_mps2,
                              UtVec3dX&       aRotationalAccel_mps2) const;

   // This performs calculations (not updates) to propagate through the specified delta t.
   // It is used in the Update() function to obtain an intermediate step in the integration
   // process. It does not perform calculations on all state parameters, rather it is limited
   // to those parameters that will impact changes in aero, propulsion, and/or gravity F&M.
   void PropagateUsingAcceleration(KinematicState& aState,
                                   double          aDeltaT_sec,
                                   const UtVec3dX& aGravitationalAccel_g,
                                   const UtVec3dX& aTranslationalAccel_mps2,
                                   const UtVec3dX& aRotationalAccel_rps2) const;

   // This performs a full integration "update" (not just calculate) including changing state
   // as well as updating all state parameters, including the additional "secondary" data,
   // as required.
   void UpdateUsingAcceleration(KinematicState& aState,
                                int64_t         aSimTime_nanosec,
                                double          aDeltaT_sec,
                                const UtVec3dX& aGravitationalAccel_g,
                                const UtVec3dX& aTranslationalAccel_mps2,
                                const UtVec3dX& aRotationalAccel_rps2) const;

private:
   PointMassMover* mVehicle = nullptr;
};
} // namespace six_dof
} // namespace wsf


#endif
