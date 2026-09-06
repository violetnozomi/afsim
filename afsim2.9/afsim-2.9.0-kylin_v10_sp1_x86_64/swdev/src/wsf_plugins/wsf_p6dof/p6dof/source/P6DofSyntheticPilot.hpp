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

#include "P6DofPilotObject.hpp"

#ifndef P6DOFSYNTHETICPILOT_H
#define P6DOFSYNTHETICPILOT_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofSyntheticPilot : public P6DofPilotObject
{
   friend class P6DofPilotManager;

public:
   explicit P6DofSyntheticPilot(P6DofScenario* aScenario);
   ~P6DofSyntheticPilot() override = default;
   P6DofSyntheticPilot& operator=(const P6DofSyntheticPilot& other) = delete;

   P6DofSyntheticPilot* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "SyntheticPilot"; }

protected:
   explicit P6DofSyntheticPilot(P6DofPilotObject* aPilotObject);

   P6DofSyntheticPilot(const P6DofSyntheticPilot& aSrc) = default;
};

#endif // P6DOFSYNTHETICPILOT_H
