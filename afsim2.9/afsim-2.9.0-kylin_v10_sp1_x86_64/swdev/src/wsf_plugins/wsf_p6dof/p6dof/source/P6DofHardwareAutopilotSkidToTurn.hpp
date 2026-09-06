// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <cinttypes>

#include "P6DofHardwareAutopilot.hpp"

#ifndef P6DOFHARDWAREAUTOPILOTSKIDTOTURN_H
#define P6DOFHARDWAREAUTOPILOTSKIDTOTURN_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofHardwareAutopilotSkidToTurn : public P6DofHardwareAutopilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofHardwareAutopilotSkidToTurn(P6DofScenario* aScenario);
   ~P6DofHardwareAutopilotSkidToTurn() override = default;
   P6DofHardwareAutopilotSkidToTurn& operator=(const P6DofHardwareAutopilotSkidToTurn& other) = delete;

   P6DofHardwareAutopilotSkidToTurn* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Autopilot_STT"; }

protected:
   explicit P6DofHardwareAutopilotSkidToTurn(P6DofPilotObject* aPilotObject);

   P6DofHardwareAutopilotSkidToTurn(const P6DofHardwareAutopilotSkidToTurn& aSrc) = default;
};

#endif // P6DOFHARDWAREAUTOPILOTSKIDTOTURN_H
