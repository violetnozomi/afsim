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

#ifndef P6DOFHARDWAREAUTOPILOT_H
#define P6DOFHARDWAREAUTOPILOT_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofHardwareAutopilot : public P6DofPilotObject
{
   friend class P6DofPilotManager;

public:
   explicit P6DofHardwareAutopilot(P6DofScenario* aScenario);
   ~P6DofHardwareAutopilot() override = default;
   P6DofHardwareAutopilot& operator=(const P6DofHardwareAutopilot& other) = delete;

   P6DofHardwareAutopilot* Clone() const;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Autopilot"; }

   virtual void HoldAltitude();
   virtual void HoldVerticalSpeed();
   virtual void HoldPitchAngle();
   virtual void HoldBankAngle();
   virtual void HoldHeading();
   virtual void HoldSpeedKCAS();
   virtual void HoldSpeedKTAS();
   virtual void HoldSpeedMach();

protected:
   explicit P6DofHardwareAutopilot(P6DofPilotObject* aPilotObject);

   P6DofHardwareAutopilot(const P6DofHardwareAutopilot& aSrc) = default;
};

#endif // P6DOFHARDWAREAUTOPILOT_H
