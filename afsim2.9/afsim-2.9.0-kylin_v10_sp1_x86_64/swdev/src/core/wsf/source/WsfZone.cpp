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

#include "WsfZone.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
WsfZone::WsfZone()
   : WsfObject()
   , WsfPlatformComponent()
   , mPlatformPtr(nullptr)
   , mModifierList()
   , mMinAlt(cUNSPECIFIED)
   , mMaxAlt(cUNSPECIFIED)
   , mMinX(cHUGE)
   , mMaxX(cSMALL)
   , mMinY(cHUGE)
   , mMaxY(cSMALL)
   , mArea(0.0)
   , mCentroid(0.0, 0.0)
   , mIsInitialized(false)
   , mGlobalName(nullptr)
{
}

// =================================================================================================
WsfZone::WsfZone(const WsfZone& aSrc)
   : WsfObject(aSrc)
   , WsfPlatformComponent(aSrc)
   , WsfAuxDataEnabled(aSrc)
   , mPlatformPtr(nullptr)
   , mModifierList(aSrc.mModifierList)
   , mMinAlt(aSrc.mMinAlt)
   , mMaxAlt(aSrc.mMaxAlt)
   , mMinX(aSrc.mMinX)
   , mMaxX(aSrc.mMaxX)
   , mMinY(aSrc.mMinY)
   , mMaxY(aSrc.mMaxY)
   , mArea(aSrc.mArea)
   , mCentroid(aSrc.mCentroid)
   , mIsInitialized(aSrc.mIsInitialized)
   , mGlobalName(aSrc.mGlobalName)
   , mFillColor(aSrc.mFillColor)
   , mLineColor(aSrc.mLineColor)
{
}

// ================================================================================================
const int* WsfZone::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_ZONE, cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfZone::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_ZONE)
   {
      return this;
   }
   return nullptr;
}

bool WsfZone::ProcessInput(UtInput& aInput)
{
   auto command   = aInput.GetCommand();
   bool myCommand = true;
   if (command == "fill_color")
   {
      // Special handling if a named color is provided, to set a default fill alpha
      std::string color;
      aInput.ReadValue(color);
      if (UtColor::ColorExists(color))
      {
         SetFillColor(color);
      }
      else
      {
         aInput.PushBack(color);
         aInput.ReadValue(mFillColor.emplace());
      }
   }
   else if (command == "line_color")
   {
      aInput.ReadValue(mLineColor.emplace());
   }
   else
   {
      myCommand = WsfAuxDataEnabled::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! This method is called to initialize the zone for use.
//! This form is called from scenario initialization for globally defined zones.
// virtual
void WsfZone::Initialize(const WsfScenario& aScenario)
{
   mIsInitialized = true;
}

// =================================================================================================
//! Implementation of WsfComponent::Initialize.
//! This is called by when initializing components attached to a platform (WsfPlatform::Initialize)
//! @param aSimTime The current simulation time.
//! @returns true;
// virtual
bool WsfZone::Initialize(double aSimTime)
{
   assert(mPlatformPtr != nullptr);
   Initialize(mPlatformPtr->GetScenario());
   return true;
}

// =================================================================================================
// virtual
bool WsfZone::GetLatLonExtrema(double& aSouthernLat, double& aWesternLon, double& aNorthernLat, double& aEasternLon) const
{
   return false;
}

void WsfZone::SetFillColor(const std::string& aColor)
{
   UtColor fillColor(UtColor::GetColorMapping(aColor));
   fillColor[3] = 63.0F / 255.0F; // default fill alpha
   SetFillColor(fillColor);
}

void WsfZone::SetLineColor(const std::string& aColor)
{
   SetLineColor(UtColor::GetColorMapping(aColor));
}

// =================================================================================================
//! Is the point inside the zone?
//!
//! @param aSimulationPtr  the simulation
//! @param aViewedPointWCS the point which is being tested for containment in the zone.
//! @param aEyePointWCS is the potential reference point for a relative Zone.
//! @param aLookHeading    the current heading (for relative references)
//! @param aDeltaDownRange is an (optional) delta to slide the Zone away from the eyepoint.
//!                         (Used to adjust the Zone location for off-design conditions.)
//! @returns 'true' if the point is inside the Zone or 'false' otherwise.
// virtual
bool WsfZone::PointIsInside(WsfSimulation* aSimulationPtr,
                            const double   aViewedPointWCS[3],
                            const double   aEyePointWCS[3],
                            const double   aLookHeading,
                            const double   aDeltaDownRange)
{
   return false;
}

// =================================================================================================
// virtual
double WsfZone::Area()
{
   return mArea;
}

// =================================================================================================
// virtual
WsfGeoPoint WsfZone::Centroid()
{
   return WsfGeoPoint(mCentroid.mX, mCentroid.mY, 0);
}

// =================================================================================================
// virtual
void WsfZone::MaxPoint(double& aX, double& aY, double& aAlt)
{
   aX   = mMaxX;
   aY   = mMaxY;
   aAlt = mMaxAlt;
}

// =================================================================================================
// virtual
void WsfZone::MinPoint(double& aX, double& aY, double& aAlt)
{
   aX   = mMinX;
   aY   = mMinY;
   aAlt = mMinAlt;
}

// =================================================================================================
double WsfZone::ModifierValue(const std::string& aModifierName) const
{
   double result = -1.0;

   WsfStringId                                   modifierNameId(aModifierName);
   std::map<WsfStringId, double>::const_iterator mli(mModifierList.find(modifierNameId));
   if (mli != mModifierList.end())
   {
      result = (*mli).second;
   }

   return result;
}
