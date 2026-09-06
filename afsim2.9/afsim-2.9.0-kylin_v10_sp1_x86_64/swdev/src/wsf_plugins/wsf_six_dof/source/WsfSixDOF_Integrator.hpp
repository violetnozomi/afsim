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

#ifndef WSFSIXDOFINTEGRATOR_HPP
#define WSFSIXDOFINTEGRATOR_HPP

#include "wsf_six_dof_export.h"

#include <cstdint>

#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
class KinematicState;
class MassProperties;
class Mover;

// Integrator integrates the state of Objects.
class WSF_SIX_DOF_EXPORT Integrator
{
public:
   Integrator()        = default;
   ~Integrator()       = default;
   Integrator& operator=(const Integrator& other) = delete;

   virtual void Update(int64_t aSimTime_nanosec, double aDeltaT_sec) const = 0;

protected:
   Integrator(const Integrator& other) = default;

   virtual Mover* GetParentVehicle() const = 0;

   // This is called to update fuel burn
   virtual void UpdateFuelBurn(KinematicState& aState, int64_t aSimTime_nanosec, double aDeltaT_sec) const;

   // This is called to propagate translation
   virtual void PropagateTranslation(KinematicState& aState, const UtVec3dX& aBodyAccel_mps2, double aDeltaT_sec) const;

   // This is called to propagate rotation
   virtual void PropagateRotation(KinematicState& aState, UtVec3dX aRotationalAccel_rps2, double aDeltaT_sec) const;
};
} // namespace six_dof
} // namespace wsf

#endif
