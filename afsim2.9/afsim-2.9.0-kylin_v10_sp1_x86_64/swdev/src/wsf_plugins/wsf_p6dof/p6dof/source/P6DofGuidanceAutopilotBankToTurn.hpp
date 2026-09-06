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

#include "P6DofGuidanceAutopilot.hpp"

#ifndef P6DOFGUIDANCEAUTOPILOTBANKTOTURN_H
#define P6DOFGUIDANCEAUTOPILOTBANKTOTURN_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofGuidanceAutopilotBankToTurn : public P6DofGuidanceAutopilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofGuidanceAutopilotBankToTurn(P6DofScenario* aScenario);
   ~P6DofGuidanceAutopilotBankToTurn() override = default;
   P6DofGuidanceAutopilotBankToTurn& operator=(const P6DofGuidanceAutopilotBankToTurn& other) = delete;

   P6DofGuidanceAutopilotBankToTurn* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Guidance_BTT"; }

protected:
   explicit P6DofGuidanceAutopilotBankToTurn(P6DofPilotObject* aPilotObject);

   P6DofGuidanceAutopilotBankToTurn(const P6DofGuidanceAutopilotBankToTurn& aSrc) = default;
};

#endif // P6DOFGUIDANCEAUTOPILOTBANKTOTURN_H
