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

#ifndef WSFRATELIMITEDATTITUDECONTROLLER_HPP
#define WSFRATELIMITEDATTITUDECONTROLLER_HPP

#include "wsf_space_export.h"

#include "WsfAttitudeController.hpp"

class UtInput;
class WsfSpaceMoverBase;

//! An attitude controller than can only turn its platform at a specified rate.
//! It is worth noting that this attitude controller will not necessarily find the
//! optimal path between the current and target orientations.
class WSF_SPACE_EXPORT WsfRateLimitedAttitudeController : public WsfAttitudeController
{
public:
   WsfRateLimitedAttitudeController();
   WsfRateLimitedAttitudeController(const WsfRateLimitedAttitudeController& aOther) = default;
   ~WsfRateLimitedAttitudeController() override                                     = default;

   void SetTargetOrientationECI(double aPsi, double aTheta, double aPhi) override;
   void GetTargetOrientationECI(double& aPsi, double& aTheta, double& aPhi) const override;
   void UpdateOrientation(double aDeltaT) override;
   bool AtTargetOrientation() const override;

   WsfAttitudeController* Clone() const override { return new WsfRateLimitedAttitudeController(*this); }
   bool                   ProcessInput(UtInput& aInput) override;

private:
   static double UpdateAngle(double aCurr, double aTarget, double aMaxChange);

   double mTargetPsiECI;   //! The target psi in ECI frame   [radians]
   double mTargetThetaECI; //! The target theta in ECI frame [radians]
   double mTargetPhiECI;   //! The target phi in ECI frame   [radians]
   double mYawRate;        //! Maximum rate of change of platform yaw   [radians / sec]
   double mPitchRate;      //! Maximum rate of change of platform pitch [radians / sec]
   double mRollRate;       //! Maximum rate of change of platform roll  [radians / sec]
};

#endif // WSFRATELIMITEDATTITUDECONTROLLER_HPP
