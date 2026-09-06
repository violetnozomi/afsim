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

#ifndef WSFRIGIDBODYSIXDOFMANUALPILOTAUGMENTEDCONTROLS_HPP
#define WSFRIGIDBODYSIXDOFMANUALPILOTAUGMENTEDCONTROLS_HPP

#include "WsfRigidBodySixDOF_ManualPilot.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RigidBodyManualPilotAugmentedControls : public RigidBodyManualPilot
{
   friend class RigidBodyPilotManager;

public:
   RigidBodyManualPilotAugmentedControls()        = default;
   ~RigidBodyManualPilotAugmentedControls()       = default;
   RigidBodyManualPilotAugmentedControls& operator=(const RigidBodyManualPilotAugmentedControls& other) = delete;

   RigidBodyManualPilotAugmentedControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-CAS"; }

protected:
   explicit RigidBodyManualPilotAugmentedControls(RigidBodyPilotObject* aPilotObject);

   RigidBodyManualPilotAugmentedControls(const RigidBodyManualPilotAugmentedControls& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf

#endif // WSFRIGIDBODYSIXDOFMANUALPILOTAUGMENTEDCONTROLS_H
