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

#ifndef P6DOFGUIDANCEAUTOPILOT_H
#define P6DOFGUIDANCEAUTOPILOT_H

class P6DofPilotManager;

class P6DOF_EXPORT P6DofGuidanceAutopilot : public P6DofPilotObject
{
   friend class P6DofPilotManager;

public:
   explicit P6DofGuidanceAutopilot(P6DofScenario* aScenario);
   ~P6DofGuidanceAutopilot() override = default;
   P6DofGuidanceAutopilot& operator=(const P6DofGuidanceAutopilot& other) = delete;

   P6DofGuidanceAutopilot* Clone() const;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Guidance"; }

   virtual void HoldAltitude();
   virtual void HoldVerticalSpeed();
   virtual void HoldPitchAngle();
   virtual void HoldBankAngle();
   virtual void HoldHeading();
   virtual void HoldSpeedKCAS();
   virtual void HoldSpeedKTAS();
   virtual void HoldSpeedMach();

protected:
   explicit P6DofGuidanceAutopilot(P6DofPilotObject* aPilotObject);

   P6DofGuidanceAutopilot(const P6DofGuidanceAutopilot& aSrc) = default;
};

#endif // P6DOFGUIDANCEAUTOPILOT_H
