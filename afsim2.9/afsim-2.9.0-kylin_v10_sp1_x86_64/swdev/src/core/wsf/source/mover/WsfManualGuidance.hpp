// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFMANUALGUIDANCE_HPP
#define WSFMANUALGUIDANCE_HPP

#include "wsf_export.h"

class UtInput;

//! A guidance base abstract class (for interface) used by WsfMovers that
//! allows for manual controls (such as joystick values).  It is intended
//! that all deriving classes override the "begin" and "end" methods.
class WSF_EXPORT WsfManualGuidance
{
public:
   class ManualControls
   {
   public:
      ManualControls()
         : mStickX(0.0)
         , mStickY(0.0)
         , mPedals(0.0)
         , mThrottle(0.0)
      {
      }
      double mStickX;   // Lateral stick input      (-1 to +1)
      double mStickY;   // Longitudinal stick input (-1 to +1)
      double mPedals;   // Rudder pedal input       (-1 to +1)
      double mThrottle; // Throttle                 ( 0 to +1)
   };

   WsfManualGuidance()
      : mManualControls()
   {
   }

   virtual bool Initialize(double aSimTime) { return true; }
   virtual bool ProcessInput(UtInput& aInput) { return false; }
   virtual void Update(double aSimTime){};

   //! Any derived class needs to override these abstract methods.
   //! On "begin" perhaps it would save previous control state so it can
   //! return to it on "end".
   virtual void BeginManualControl() = 0;
   virtual void EndManualControl()   = 0;

   void            SetManualInputs(ManualControls& aControl) { mManualControls = aControl; };
   ManualControls& GetManualInputs(void) { return mManualControls; };

private:
   ManualControls mManualControls;
};

#endif
