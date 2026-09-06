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

#include "P6DofManualPilot.hpp"

#ifndef P6DOFMANUALPILOTAUGMENTEDCONTROLS_H
#define P6DOFMANUALPILOTAUGMENTEDCONTROLS_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofManualPilotAugmentedControls : public P6DofManualPilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofManualPilotAugmentedControls(P6DofScenario* aScenario);
   ~P6DofManualPilotAugmentedControls() override = default;
   P6DofManualPilotAugmentedControls& operator=(const P6DofManualPilotAugmentedControls& other) = delete;

   P6DofManualPilotAugmentedControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-CAS"; }

protected:
   explicit P6DofManualPilotAugmentedControls(P6DofPilotObject* aPilotObject);

   P6DofManualPilotAugmentedControls(const P6DofManualPilotAugmentedControls& aSrc) = default;
};

#endif // P6DOFMANUALPILOTAUGMENTEDCONTROLS_H
