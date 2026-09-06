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

#ifndef WSFINSTANTATTITUDECONTROLLER_HPP
#define WSFINSTANTATTITUDECONTROLLER_HPP

#include "WsfAttitudeController.hpp"

class UtInput;
class WsfSpaceMoverBase;

//! An attitude controller that makes instantaneous changes in platform orientation
class WsfInstantAttitudeController : public WsfAttitudeController
{
public:
   WsfInstantAttitudeController();
   WsfInstantAttitudeController(const WsfInstantAttitudeController& aOther) = default;
   ~WsfInstantAttitudeController() override                                 = default;

   void SetTargetOrientationECI(double aPsiECI, double aThetaECI, double aPhiECI) override;
   void GetTargetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const override;
   void UpdateOrientation(double aDeltaT) override;
   bool AtTargetOrientation() const override;

   WsfAttitudeController* Clone() const override { return new WsfInstantAttitudeController(*this); }

private:
   double mTargetPsiECI;   //! The target psi in ECI frame [radians]
   double mTargetThetaECI; //! The target theta in ECI frame [radians]
   double mTargetPhiECI;   //! The target phi in ECI frame [radians]
};

#endif // WSFINSTANTATTITUDECONTROLLER_HPP