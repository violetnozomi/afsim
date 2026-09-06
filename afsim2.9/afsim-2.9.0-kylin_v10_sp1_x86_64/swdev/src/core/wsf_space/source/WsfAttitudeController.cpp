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

#include "WsfAttitudeController.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtMat3.hpp"
#include "UtMemory.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceMoverBase.hpp"

WsfAttitudeController::WsfAttitudeController()
   : WsfObject()
   , mMover(nullptr)
   , mOrientationPtr(wsf::space::Orientation::Factory(wsf::space::OrientationNadirECI_Velocity::GetTypeName()))
{
   mOrientationPtr->SetAttitudeController(*this);
}

WsfAttitudeController::WsfAttitudeController(const WsfAttitudeController& aSrc)
   : WsfObject(aSrc)
   , mMover(aSrc.mMover)
   , mOrientationPtr(aSrc.mOrientationPtr->Clone())
{
   mOrientationPtr->SetAttitudeController(*this);
}

void WsfAttitudeController::SetTargetOrientation(double         aYawECS,
                                                 double         aPitchECS,
                                                 double         aRollECS,
                                                 const UtVec3d& aX_AxisOrientationECI,
                                                 const UtVec3d& aZ_AxisConstraintECI)
{
   double psi, theta, phi;
   mOrientationPtr->CalculateGeneral(aYawECS, aPitchECS, aRollECS, aX_AxisOrientationECI, aZ_AxisConstraintECI, psi, theta, phi);
   SetTargetOrientationECI(psi, theta, phi);
}

// override
bool WsfAttitudeController::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command = aInput.GetCommand();
   if (command == "orientation")
   {
      std::string orientation;
      aInput.ReadValue(orientation);
      mOrientationPtr = wsf::space::Orientation::Factory(orientation);
      if (mOrientationPtr != nullptr)
      {
         mOrientationPtr->SetAttitudeController(*this);
         mOrientationPtr->ProcessInput(aInput);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
      myCommand = true;
   }
   else if (command == "swap_axes")
   {
      mOrientationPtr->SetDefaultX_Aligned(!mOrientationPtr->GetDefaultX_Aligned());
      myCommand = true;
   }

   return myCommand;
}

//! (Deprecated) Sets the orientation to one of the standard orientation types.
//! @param aOrientation The desired orientation type.
//! @return Whether the operation was successful (returns true if aOrientation is a recognized type name).
bool WsfAttitudeController::SetOrientation(const std::string& aOrientation)
{
   std::unique_ptr<wsf::space::Orientation> orientationPtr = wsf::space::Orientation::Factory(aOrientation);
   bool                                     created        = (orientationPtr != nullptr);
   if (created)
   {
      mOrientationPtr = std::move(orientationPtr);
      mOrientationPtr->SetAttitudeController(*this);
   }
   return created;
}

//! Sets the orientation to one of the standard orientation types.
//! @param aOrientationPtr The desired orientation type.
void WsfAttitudeController::SetOrientation(std::unique_ptr<wsf::space::Orientation> aOrientationPtr)
{
   mOrientationPtr = std::move(aOrientationPtr);
   mOrientationPtr->SetAttitudeController(*this);
}

//! Update the platform's attitude.
//! @param aDeltaT the change in time to integrate angular rotation.
void WsfAttitudeController::Update(double aDeltaT)
{
   // First, update the target orientation as needed
   double psi, theta, phi;
   if (CalculateOrientation(psi, theta, phi))
   {
      SetTargetOrientationECI(psi, theta, phi);
   }

   // Second, use the new target to update the platform's orientation
   UpdateOrientation(aDeltaT);
}

// virtual
//! a Pass-through method to calculate the satellite's orientation.
//! @param aPsi The calculated ECI 'heading' angle.
//! @param aTheta The calculated ECI 'pitch' angle.
//! @param aPhi The calculated ECI 'roll' angle.
//! @note See UtOrbitalPropagatorBase::Orientation.
bool WsfAttitudeController::CalculateOrientation(double& aPsi, double& aTheta, double& aPhi) const
{
   return mOrientationPtr->Calculate(aPsi, aTheta, aPhi);
}
