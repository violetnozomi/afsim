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

#ifndef P6DOFHARDWAREAUTOPILOTBANKTOTURN_H
#define P6DOFHARDWAREAUTOPILOTBANKTOTURN_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofHardwareAutopilotBankToTurn : public P6DofHardwareAutopilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofHardwareAutopilotBankToTurn(P6DofScenario* aScenario);
   ~P6DofHardwareAutopilotBankToTurn() override = default;
   P6DofHardwareAutopilotBankToTurn& operator=(const P6DofHardwareAutopilotBankToTurn& other) = delete;

   P6DofHardwareAutopilotBankToTurn* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Autopilot_BTT"; }

protected:
   explicit P6DofHardwareAutopilotBankToTurn(P6DofPilotObject* aPilotObject);

   P6DofHardwareAutopilotBankToTurn(const P6DofHardwareAutopilotBankToTurn& aSrc) = default;
};

#endif // P6DOFHARDWAREAUTOPILOTBANKTOTURN_H
