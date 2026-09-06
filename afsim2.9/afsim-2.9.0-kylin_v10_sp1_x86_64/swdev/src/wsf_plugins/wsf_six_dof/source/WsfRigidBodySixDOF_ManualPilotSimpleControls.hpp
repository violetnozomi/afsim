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

#ifndef WSFRIGIDBODYSIXDOFMANUALPILOTSIMPLECONTROLS_HPP
#define WSFRIGIDBODYSIXDOFMANUALPILOTSIMPLECONTROLS_HPP

#include "WsfRigidBodySixDOF_ManualPilot.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RigidBodyManualPilotSimpleControls : public RigidBodyManualPilot
{
   friend class RigidBodyPilotManager;

public:
   RigidBodyManualPilotSimpleControls()        = default;
   ~RigidBodyManualPilotSimpleControls()       = default;
   RigidBodyManualPilotSimpleControls& operator=(const RigidBodyManualPilotSimpleControls& other) = delete;

   RigidBodyManualPilotSimpleControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-Simple"; }

   bool UseSimpleYawDamper() { return mSimpleYawDamper; }

protected:
   explicit RigidBodyManualPilotSimpleControls(RigidBodyPilotObject* aPilotObject);

   RigidBodyManualPilotSimpleControls(const RigidBodyManualPilotSimpleControls& aSrc) = default;

   bool mSimpleYawDamper = false;
};
} // namespace six_dof
} // namespace wsf


#endif // WSFRIGIDBODYSIXDOFMANUALPILOTSIMPLECONTROLS_H
