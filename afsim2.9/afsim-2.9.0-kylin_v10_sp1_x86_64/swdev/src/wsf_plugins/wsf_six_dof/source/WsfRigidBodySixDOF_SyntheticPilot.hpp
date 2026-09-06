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

#ifndef WSFRIGIDBODYSIXDOFSYNTHETICPILOT_HPP
#define WSFRIGIDBODYSIXDOFSYNTHETICPILOT_HPP

#include <cinttypes>

#include "WsfRigidBodySixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class RigidBodyPilotManager;

class WSF_SIX_DOF_EXPORT RigidBodySyntheticPilot : public RigidBodyPilotObject
{
   friend class RigidBodyPilotManager;

public:
   RigidBodySyntheticPilot()        = default;
   ~RigidBodySyntheticPilot()       = default;
   RigidBodySyntheticPilot& operator=(const RigidBodySyntheticPilot& other) = delete;

   RigidBodySyntheticPilot* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "SyntheticPilot"; }

   void HoldAltitude() override;
   void HoldVerticalSpeed() override;
   void HoldPitchAngle() override;
   void HoldBankAngle() override;
   void HoldHeading() override;
   void HoldSpeedKCAS() override;
   void HoldSpeedKTAS() override;
   void HoldSpeedMach() override;

protected:
   explicit RigidBodySyntheticPilot(RigidBodyPilotObject* aPilotObject);

   RigidBodySyntheticPilot(const RigidBodySyntheticPilot& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf

#endif // WSFRIGIDBODYSIXDOFSYNTHETICPILOT_H
