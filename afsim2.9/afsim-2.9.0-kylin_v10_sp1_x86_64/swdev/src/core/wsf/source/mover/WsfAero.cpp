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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfAero.hpp"

#include <memory>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
WsfAero::WsfAero(const UtAtmosphere& aAtmosphere)
   : WsfObject()
   , mEntityPtr(nullptr)
   , mAtm(aAtmosphere)
   , mRefArea(1.0)
   , mAltitude(0.0)
   , mDynPressure(0.0)
   , mSpeed(0.0)
   , mMach(0.0)
   , mDrag(0.0)
   , mZForce(0.0)
   , mYForce(0.0)
   , mMaximumForce(0.0)
   , mLiftCurveSlope(0.0)
   , mDebug(false)
   , mInitialized(false)
{
}

// =================================================================================================
//! Copy constructor, used by Clone().
WsfAero::WsfAero(const WsfAero& aSrc)
   : WsfObject(aSrc)
   , mEntityPtr(nullptr)
   , mAtm(aSrc.mAtm)
   , mRefArea(aSrc.mRefArea)
   , mAltitude(aSrc.mAltitude)
   , mDynPressure(aSrc.mDynPressure)
   , mSpeed(aSrc.mSpeed)
   , mMach(aSrc.mMach)
   , mDrag(aSrc.mDrag)
   , mZForce(aSrc.mZForce)
   , mYForce(aSrc.mYForce)
   , mMaximumForce(aSrc.mMaximumForce)
   , mLiftCurveSlope(aSrc.mLiftCurveSlope)
   , mDebug(aSrc.mDebug)
   , mInitialized(aSrc.mInitialized)
{
}

// =================================================================================================
WsfAero::~WsfAero() {}

// =================================================================================================
//! Initialize the aerodynamics object.
//! @return 'True' if object successfully initialized, or 'false' if an error occurred.
// virtual
bool WsfAero::Initialize(double aSimTime, UtEntity* aPlatformPtr)
{
   mInitialized = true;

   mEntityPtr = aPlatformPtr;
   if (mEntityPtr == nullptr)
   {
      auto out = ut::log::error() << "'aero' was not supplied a WsfPlatform during Initialize().";
      out.AddNote() << "T = " << aSimTime;
      mInitialized = false;
   }

   if (mRefArea == 0.0)
   {
      auto out = ut::log::error() << "'aero' 'reference_area' was Zero.";
      out.AddNote() << "T = " << aSimTime;
      if (mInitialized)
      {
         out.AddNote() << "Platform: " << GetPlatformName();
      }
      mInitialized = false;
   }

   return mInitialized;
}

// =================================================================================================
//! Recognize and process aerodynamics input.
//! @return 'True' if it is a command I recognize, 'false' if not.
// virtual
bool WsfAero::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "lift_curve_slope")
   {
      aInput.ReadValue(mLiftCurveSlope);
      aInput.ValueGreaterOrEqual(mLiftCurveSlope, 0.0);
   }
   else if (command == "reference_area")
   {
      aInput.ReadValueOfType(mRefArea, UtInput::cAREA);
      aInput.ValueGreater(mRefArea, 0.0);
   }
   else if (mAtm.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
//! Update the aerodynamics objects.
//! Update() is the primary method of the class.  Using the internal
//! platform pointer, calculate aerodynamic data in the body frame for
//! this motion state (as appropriate):  The input (desired) body-frame
//! forces may not be physically attainable under the current conditions,
//! so the input values are merely requests, and may not be satisfied.
//!
//! @param aSimTime:          Current simulation time.
//! @param aRequestedYForce:  Desired body-frame lateral force (positive to the right).
//! @param aRequestedZForce:  Desired body-frame vertical force (positive down).
//! @param aTVC_Force         Thrust vector control force
//! @note In the base class implementation, roll angles are zero!
// virtual
void WsfAero::Update(double aSimTime, double aRequestedYForce, double aRequestedZForce, double aTVC_Force)
{
   mYForce = 0.0;
   mZForce = 0.0;

   // Need to get the current altitude for atmospheric properties:
   mAltitude = mEntityPtr->GetAltitude();
   mSpeed    = mEntityPtr->GetSpeedWCS();

   mDynPressure = GetDynamicPressure(mAltitude, mSpeed);
   mMach        = GetMach(mAltitude, mSpeed);
   // This value must be calculated in derived classes:
   // mDrag = mDynPressure * mRefArea * mCd;
}

std::string WsfAero::GetPlatformName() const
{
   WsfPlatform* platPtr = dynamic_cast<WsfPlatform*>(mEntityPtr);
   std::string  name    = "<none>";
   if (platPtr != nullptr)
   {
      name = platPtr->GetName();
   }
   return name;
}

double WsfAero::GetDynamicPressure(double aAltitude_m, double aSpeed_mps) const
{
   return 0.5 * mAtm.Density(aAltitude_m) * aSpeed_mps * aSpeed_mps;
}

double WsfAero::GetMach(double aAltitude_m, double aSpeed_mps) const
{
   return aSpeed_mps / mAtm.SonicVelocity(aAltitude_m);
}

double WsfAero::GetSpeed(double aAltitude_m, double aMach) const
{
   return aMach * mAtm.SonicVelocity(aAltitude_m);
}
