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

#ifndef WSFRIGIDBODYSIXDOFMANUALPILOT_HPP
#define WSFRIGIDBODYSIXDOFMANUALPILOT_HPP

#include "WsfRigidBodySixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class RigidBodyPilotManager;

class WSF_SIX_DOF_EXPORT RigidBodyManualPilot : public RigidBodyPilotObject
{
   friend class RigidBodyPilotManager;

public:
   RigidBodyManualPilot()        = default;
   ~RigidBodyManualPilot()       = default;
   RigidBodyManualPilot& operator=(const RigidBodyManualPilot& other) = delete;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "ManualPilot"; }

protected:
   explicit RigidBodyManualPilot(RigidBodyPilotObject* aPilotObject);

   RigidBodyManualPilot(const RigidBodyManualPilot& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf


#endif // WSFRIGIDBODYSIXDOFMANUALPILOT_H
