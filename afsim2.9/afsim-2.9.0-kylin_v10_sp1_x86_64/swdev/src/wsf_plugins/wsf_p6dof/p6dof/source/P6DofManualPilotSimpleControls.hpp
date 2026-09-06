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

#ifndef P6DOFMANUALPILOTSIMPLECONTROLS_H
#define P6DOFMANUALPILOTSIMPLECONTROLS_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofManualPilotSimpleControls : public P6DofManualPilot
{
   friend class P6DofPilotManager;

public:
   explicit P6DofManualPilotSimpleControls(P6DofScenario* aScenario);
   ~P6DofManualPilotSimpleControls() override = default;
   P6DofManualPilotSimpleControls& operator=(const P6DofManualPilotSimpleControls& other) = delete;

   P6DofManualPilotSimpleControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-Simple"; }

   bool UseSimpleYawDamper() { return mSimpleYawDamper; }

protected:
   explicit P6DofManualPilotSimpleControls(P6DofPilotObject* aPilotObject);

   P6DofManualPilotSimpleControls(const P6DofManualPilotSimpleControls& aSrc) = default;

   bool mSimpleYawDamper = false;
};

#endif // P6DOFMANUALPILOTSIMPLECONTROLS_H
