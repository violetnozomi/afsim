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

#include "P6DofPilotObject.hpp"

#ifndef P6DOFMANUALPILOT_H
#define P6DOFMANUALPILOT_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofManualPilot : public P6DofPilotObject
{
   friend class P6DofPilotManager;

public:
   explicit P6DofManualPilot(P6DofScenario* aScenario);
   ~P6DofManualPilot() override = default;
   P6DofManualPilot& operator=(const P6DofManualPilot& other) = delete;

   P6DofManualPilot* Clone() const;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "ManualPilot"; }

protected:
   explicit P6DofManualPilot(P6DofPilotObject* aPilotObject);

   P6DofManualPilot(const P6DofManualPilot& aSrc) = default;
};

#endif // P6DOFMANUALPILOT_H
