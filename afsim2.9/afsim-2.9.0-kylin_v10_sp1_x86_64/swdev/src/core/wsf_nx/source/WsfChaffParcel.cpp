// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfChaffParcel.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"

WsfChaffParcel::MaxRcsCache WsfChaffParcel::sMaxRcsCache;

WsfChaffParcel::WsfChaffParcel()
   : WsfObject()
   , WsfSimplePlatformComponent(cCOMPONENT_ROLE<WsfChaffParcel>(), "#chaff_parcel")
   , mCloudPtr(nullptr)
   , mBloomDiameter(10.0)
   , mExpansionTimeConstant(0.75)
   , mTerminalVelocity(1.0) // m/s
   , mDecelerationRate(100.0)
   , mExpirationTime(60.0)
   , mDebug(false)
   , mFreqMaxRCS_Table(ut::make_unique<UtTable::Curve>())
   , mNumberDipoles(1000000U)
   , mDropLocationWCS()
   , mDropVelocityWCS()
   , mDropAccelerationWCS()
   , mFreeFallVelocityWCS()
   , mDropTime()
   , mRadialSize(0.0)
{
}

WsfChaffParcel::WsfChaffParcel(const WsfChaffParcel& aSrc)
   : WsfObject(aSrc)
   , WsfSimplePlatformComponent(aSrc)
   , mCloudPtr(aSrc.mCloudPtr)
   , mBloomDiameter(aSrc.mBloomDiameter)
   , mExpansionTimeConstant(aSrc.mExpansionTimeConstant)
   , mTerminalVelocity(aSrc.mTerminalVelocity)
   , mDecelerationRate(aSrc.mDecelerationRate)
   , mExpirationTime(aSrc.mExpirationTime)
   , mDebug(aSrc.mDebug)
   , mFreqMaxRCS_Table(aSrc.mFreqMaxRCS_Table)
   , mNumberDipoles(aSrc.mNumberDipoles)
   , mDropLocationWCS{aSrc.mDropLocationWCS[0], aSrc.mDropLocationWCS[1], aSrc.mDropLocationWCS[2]}
   , mDropVelocityWCS{aSrc.mDropVelocityWCS[0], aSrc.mDropVelocityWCS[1], aSrc.mDropVelocityWCS[2]}
   , mDropAccelerationWCS{aSrc.mDropAccelerationWCS[0], aSrc.mDropAccelerationWCS[1], aSrc.mDropAccelerationWCS[2]}
   , mFreeFallVelocityWCS{aSrc.mFreeFallVelocityWCS[0], aSrc.mFreeFallVelocityWCS[1], aSrc.mFreeFallVelocityWCS[2]}
   , mDropTime(aSrc.mDropTime)
   , mRadialSize(aSrc.mRadialSize)
{
}

WsfObject* WsfChaffParcel::Clone() const
{
   return new WsfChaffParcel(*this);
}

double WsfChaffParcel::GetRadarCrossSection(double       aFrequency,
                                            const double aXmtrLocationWCS[3],
                                            const double aRcvrLocationWCS[3]) const
{
   assert(mCloudPtr);

   // Get presented area of the cloud parcel belongs to, accounts for blooming and aspect angle dependency
   size_t numParcels;
   double pArea = mCloudPtr->GetBistaticPresentedArea(aXmtrLocationWCS, aRcvrLocationWCS, numParcels);

   double rcs = 0.0;
   if (pArea > 0.0)
   {
      double maxRcs = 0.0;
      // Look in max rcs cache for frequency first. If not found, compute using GetMaximumRCS and add to cache.
      auto pair = std::make_pair(GetTypeId(), aFrequency);
      auto it   = sMaxRcsCache.find(pair);
      if (it != sMaxRcsCache.end())
      {
         maxRcs = it->second;
      }
      else
      {
         maxRcs = GetMaximumRCS(aFrequency);
         sMaxRcsCache.emplace(pair, maxRcs);
      }

      double rcsPerUnitArea = numParcels * maxRcs / pArea;
      rcs                   = pArea * (1.0 - exp(-rcsPerUnitArea)) / numParcels;
      // Limit to max rcs
      rcs = std::min(rcs, maxRcs);
   }
   return rcs;
}

// Maximum parcel rcs for a dispersed cloud (widely spaced dipoles with no shielding effects). If no lookup table was
// defined, assume average RCS for randomly oriented half-wave dipoles at resonance given as σ = 0.15Nλ²
// (Ref: Schleher, Electronic Warfare in the Information Age)
double WsfChaffParcel::GetMaximumRCS(double aFrequency) const
{
   return mFreqMaxRCS_Table->IsDefined() ? mFreqMaxRCS_Table->Lookup(aFrequency) :
                                           0.15 * mNumberDipoles * std::pow(UtMath::cLIGHT_SPEED / aFrequency, 2.0);
}

// Maximum parcel radar cross section for the dispersed cloud for any frequency
double WsfChaffParcel::GetMaximumRCS() const
{
   if (mFreqMaxRCS_Table->IsDefined())
   {
      double minVal, maxVal;
      if (mFreqMaxRCS_Table->GetVariableLimits("", minVal, maxVal))
      {
         return maxVal;
      }
   }
   // pretending lowest practical frequencies for radar is 900 MHz.
   return .15 * mNumberDipoles * std::pow(UtMath::cLIGHT_SPEED / 9e8, 2.0);
}

void WsfChaffParcel::GetDropLocationWCS(double aDropLocationWCS[3]) const
{
   UtVec3d::Set(aDropLocationWCS, mDropLocationWCS);
}

bool WsfChaffParcel::Find(WsfChaffParcel*& aParcelPtr, const WsfPlatform& aPlatform)
{
   return aPlatform.FindByRole(aParcelPtr);
}

// WsfChaffParcel does not support Clone()
WsfComponent* WsfChaffParcel::CloneComponent() const
{
   return nullptr;
}

void* WsfChaffParcel::QueryInterface(int aRole)
{
   return (aRole == cCOMPONENT_ROLE<WsfChaffParcel>()) ? this : nullptr;
}

bool WsfChaffParcel::InitializeType() const
{
   if (!mFreqMaxRCS_Table->IsDefined())
   {
      auto logger = ut::log::warning() << "Chaff parcel type has not defined maximum RCS vs. frequency table.";
      logger.AddNote() << "Parcel Type: " << GetType();
      logger.AddNote() << "Assuming spot chaff with " << mNumberDipoles << " half-wave dipoles.";
   }
   return true;
}

// Save off initial state of the ejected parcel for kinematic calculations
bool WsfChaffParcel::Initialize(double aSimTime)
{
   // Save location where parcel was dropped
   GetPlatform()->GetLocationWCS(mDropLocationWCS);

   // Save velocity vector and initial speed at drop
   GetPlatform()->GetVelocityWCS(mDropVelocityWCS);

   // Get velocity vector of free fall
   double freeFallNED[3] = {0.0, 0.0, mTerminalVelocity};
   GetPlatform()->ConvertNEDVectorToWCS(mFreeFallVelocityWCS, freeFallNED);

   // Parcel acceleration is aligned opposite velocity vector and has magnitude mDecelerationRate
   UtVec3d::Normalize(mDropAccelerationWCS, mDropVelocityWCS);
   UtVec3d::Multiply(mDropAccelerationWCS, -mDecelerationRate);
   GetPlatform()->SetAccelerationWCS(mDropAccelerationWCS);

   // Save time of parcel drop
   mDropTime = aSimTime;

   return WsfSimplePlatformComponent::Initialize(aSimTime);
}

bool WsfChaffParcel::ProcessInput(UtInput& aInput)
{
   bool myCommand(true);

   std::string command(aInput.GetCommand());
   if (command == "debug")
   {
      aInput.ReadValue(mDebug);
   }
   else if (command == "terminal_velocity")
   {
      aInput.ReadValueOfType(mTerminalVelocity, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mTerminalVelocity, 0.0);
   }
   else if (command == "bloom_diameter")
   {
      aInput.ReadValueOfType(mBloomDiameter, UtInput::cLENGTH);
      aInput.ValueGreater(mBloomDiameter, 0.0);
   }
   else if ((command == "expansion_time_constant") || (command == "bloom_time_constant"))
   {
      aInput.ReadValueOfType(mExpansionTimeConstant, UtInput::cTIME);
      aInput.ValueGreater(mExpansionTimeConstant, 0.0);
   }
   else if (command == "deceleration_rate")
   {
      aInput.ReadValueOfType(mDecelerationRate, UtInput::cACCELERATION);
      aInput.ValueGreater(mDecelerationRate, 0.0);
   }
   else if (command == "expiration_time")
   {
      aInput.ReadValueOfType(mExpirationTime, UtInput::cTIME);
      aInput.ValueGreater(mExpirationTime, 0.0);
   }
   else if (command == "frequency_maximum_rcs_table")
   {
      mFreqMaxRCS_Table->ProcessInput(aInput,
                                      "frequency",
                                      UtInput::cFREQUENCY,
                                      "hz",
                                      UtTable::ValueGT(0.0),
                                      UtInput::cAREA_DB,
                                      "dBsm",
                                      UtTable::ValueGT(0.0));
   }
   else if (command == "number_dipoles")
   {
      aInput.ReadValue(mNumberDipoles);
      aInput.ValueGreater(mNumberDipoles, 0U);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

//! Chaff physical parameters are computed as follows:
//! 1. The chaff mean radial size is computed as an exponential growth to the value of DMAX/2, where DMAX is the
//!    the characteristic diameter of scatters in parcel at time infinity. Chaff parcels are treated as spherical.
//! 2. The mean radial speed is calculated as an exponential decay from the value DMAX/2/TAUD, where TAUD is the
//!    parcel's radial size expansion time constant (i.e., it is the first derivative of the mean radial size)
//! 3. Chaff decelerates with a constant acceleration value until it comes to rest
void WsfChaffParcel::Update(double aSimTime)
{
   // Length of time this parcel has been falling
   double tfall = aSimTime - mDropTime;
   // Compute mean radial size of this parcel
   mRadialSize = 0.5 * mBloomDiameter * (1.0 - exp(-tfall / mExpansionTimeConstant));
   // Compute mean radial speed of this parcel
   // double radialSpeed = (0.5 * mBloomDiameter / mExpansionTimeConstant) * exp(-tfall / mExpansionTimeConstant);
   // Set time for position calculation as time since parcel drop, limited to time at which parcel stops decelerating
   double tloc = std::min(tfall, UtVec3d::Magnitude(mDropVelocityWCS) / mDecelerationRate);

   UtVec3d dropLocWCS(mDropLocationWCS);
   UtVec3d dropVelWCS(mDropVelocityWCS);
   UtVec3d dropAclWCS(mDropAccelerationWCS);
   UtVec3d ffVelWCS(mFreeFallVelocityWCS);
   UtVec3d locationWCS = dropLocWCS + dropVelWCS * tloc + 0.5 * dropAclWCS * pow(tloc, 2) + ffVelWCS * tfall;
   UtVec3d velocityWCS = dropVelWCS + dropAclWCS * tloc + ffVelWCS;

   GetPlatform()->SetLocationWCS(locationWCS.GetData());
   GetPlatform()->SetVelocityWCS(velocityWCS.GetData());
   if (tloc < tfall) // Parcel has stopped decelerating (and is just in free fall)
   {
      double aclWCS[3] = {0.0, 0.0, 0.0};
      GetPlatform()->SetAccelerationWCS(aclWCS);
   }
}

//! To improve run time performance, chaff parcels are removed from the simulation once they are no longer needed.
//! Parcels are removed if they hit the ground. Otherwise, they are retained for a maximum time given by mExpirationTime
bool WsfChaffParcel::IsExpired(double aSimTime)
{
   bool expired = false;
   // Check expiration time first, least expensive
   if (aSimTime - mDropTime >= mExpirationTime)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Chaff parcel type has expired and will be removed from the simulation.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Parcel Type: " << GetType();
         logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      expired = true;
   }
   else if (GetPlatform()->GetHeightAboveTerrain() <= 0) // Then check for hitting the ground
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Chaff parcel type hit the ground and will be removed from the simulation.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Parcel Type: " << GetType();
         logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      expired = true;
   }

   return expired;
}

void WsfChaffParcel::AddToCloud(std::shared_ptr<WsfChaffCloudInterface>& aCloudPtr)
{
   if (!mCloudPtr)
   {
      mCloudPtr = aCloudPtr;
   }
}

void WsfChaffParcel::ClearMaxRcsCache()
{
   sMaxRcsCache.clear();
}
