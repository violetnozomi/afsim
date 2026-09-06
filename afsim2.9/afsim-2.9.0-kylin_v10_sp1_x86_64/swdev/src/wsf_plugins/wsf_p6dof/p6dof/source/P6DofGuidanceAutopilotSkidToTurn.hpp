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

#ifndef P6DOFGUIDANCEAUTOPILOTSKIDTOTURN_H
#define P6DOFGUIDANCEAUTOPILOTSKIDTOTURN_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofGuidanceAutopilotSkidToTurn : public P6DofGuidanceAutopilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofGuidanceAutopilotSkidToTurn(P6DofScenario* aScenario);
   ~P6DofGuidanceAutopilotSkidToTurn() override = default;
   P6DofGuidanceAutopilotSkidToTurn& operator=(const P6DofGuidanceAutopilotSkidToTurn& other) = delete;

   P6DofGuidanceAutopilotSkidToTurn* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Guidance_STT"; }

protected:
   explicit P6DofGuidanceAutopilotSkidToTurn(P6DofPilotObject* aPilotObject);

   P6DofGuidanceAutopilotSkidToTurn(const P6DofGuidanceAutopilotSkidToTurn& aSrc) = default;
};

#endif // P6DOFGUIDANCEAUTOPILOTSKIDTOTURN_H
