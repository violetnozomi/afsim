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

#ifndef WSFATTITUDECONTROLLER_HPP
#define WSFATTITUDECONTROLLER_HPP

#include "wsf_space_export.h"

#include <memory>
#include <string>

class UtInput;
#include "UtVec3.hpp"
#include "WsfObject.hpp"
class WsfSpaceMoverBase;
#include "WsfSpaceOrientation.hpp"

//! A base class for attitude controllers
//! The attitude controller models the ability of the platform with the space mover to reorient
//! itself. It operates by setting a target orientation to which the platform will try to
//! orient.
class WSF_SPACE_EXPORT WsfAttitudeController : public WsfObject
{
public:
   WsfAttitudeController();
   WsfAttitudeController(const WsfAttitudeController& aOther);
   ~WsfAttitudeController() override = default;

   WsfAttitudeController& operator=(const WsfAttitudeController& aSrc) = delete;

   //! Set the orientation target for this attitude controller.
   virtual void SetTargetOrientationECI(double aPsiECI, double aThetaECI, double aPhiECI) = 0;

   //! Set a general target orientation based on an ECS-referenced yaw, pitch, roll,
   //! a PCS x-axis alignment vector in ECI coordinates, and a PCS z-axis constraint vector.
   void SetTargetOrientation(double         aYawECS,
                             double         aPitchECS,
                             double         aRollECS,
                             const UtVec3d& aX_AxisOrientationECI,
                             const UtVec3d& aZ_AxisConstraintECI);

   //! Get the current orientation target for this attitude controller
   virtual void GetTargetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const = 0;

   //! Perform update to orientation.
   //! This is responsible for updating the actual orientation of the platform
   virtual void UpdateOrientation(double aDeltaT) = 0;

   //! Calculate the updated orientation target
   virtual bool CalculateOrientation(double& aPsi, double& aTheta, double& aPhi) const;

   //! Update attitude controller.
   void Update(double aDeltaT);

   //! Has the mover reached the target orientation
   virtual bool AtTargetOrientation() const = 0;

   //! Clone this object
   WsfAttitudeController* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   //! Set the mover to which this is attached
   void SetMover(WsfSpaceMoverBase* aMover) { mMover = aMover; }

   //! Return the mover to which this attitude controller is attached
   WsfSpaceMoverBase* GetMover() const { return mMover; }

   void SetOrientation(std::unique_ptr<wsf::space::Orientation> aOrientationPtr);

   WSF_SPACE_DEPRECATED bool SetOrientation(const std::string& aOrientation);

private:
   WsfSpaceMoverBase*                       mMover;          //! This mover to which this controller is attached
   std::unique_ptr<wsf::space::Orientation> mOrientationPtr; //! An orientation for the controller; gives updating
                                                             //! target orientations.
   std::string mGeoPoint;                                    //! Name of GeoPoint to use with point stare modes.
};

#endif // WSFATTITUDECONTROLLER_HPP
