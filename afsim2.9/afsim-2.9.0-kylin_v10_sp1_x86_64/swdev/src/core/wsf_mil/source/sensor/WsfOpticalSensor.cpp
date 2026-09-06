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

// WsfOpticalSensor is a simple EO sensor implementation.

#include "WsfOpticalSensor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "TblLookup.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Util.hpp"
#include "WsfInherentContrast.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfUtil.hpp"

WsfOpticalSensor::WsfOpticalSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mOpticalModeList()
{
   SetClass(cPASSIVE | cVISUAL); // This is a passive sensor
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new OpticalMode(aScenario)));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

WsfOpticalSensor::WsfOpticalSensor(const WsfOpticalSensor& aSrc)
   : WsfSensor(aSrc)
   , mOpticalModeList()
{
}

// virtual
WsfSensor* WsfOpticalSensor::Clone() const
{
   return new WsfOpticalSensor(*this);
}

// virtual
bool WsfOpticalSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mOpticalModeList);

   return ok;
}

// virtual
void WsfOpticalSensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// virtual
size_t WsfOpticalSensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mOpticalModeList.empty() ? 0U : 1U;
}

// virtual
WsfEM_Rcvr& WsfOpticalSensor::GetEM_Rcvr(size_t aIndex) const
{
   return *mOpticalModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

//! virtual
void WsfOpticalSensor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // go through each mode and clean-up the PdMap
   for (auto& i : mOpticalModeList)
   {
      i->PlatformDeleted(aPlatformPtr);
   }

   // call base class method
   WsfSensor::PlatformDeleted(aPlatformPtr);
}


// ****************************************************************************
// Definition for the nested class that implements the mode of the sensor.

WsfOpticalSensor::OpticalMode::OpticalMode(WsfScenario& aScenario)
   : WsfSensorMode()
   , mAntenna(new WsfEM_Antenna)
   , mRcvr(new WsfEM_Rcvr(WsfEM_Rcvr::cRF_SENSOR, mAntenna.get()))
   , mGlimpseTime(1.0 / 3.0)
   , mReacquireTime(3.0)
   , mAtmosphericAttenuation(0.0)
   , mBackgroundRadiance(0.0)
   , mPathRadiance(0.0)
   , mVerbose(false)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mRhoSeaLevel(UtEarth::cSSL_AIR_DENSITY)
   , mPdMap()
   , mSearch()
   , mReacquire()
   , mTrack()
{
   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL);
}

WsfOpticalSensor::OpticalMode::OpticalMode(const OpticalMode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(new WsfEM_Antenna(*aSrc.mAntenna))
   , mRcvr(new WsfEM_Rcvr(*aSrc.mRcvr, mAntenna.get()))
   , mGlimpseTime(aSrc.mGlimpseTime)
   , mReacquireTime(aSrc.mReacquireTime)
   , mAtmosphericAttenuation(aSrc.mAtmosphericAttenuation)
   , mBackgroundRadiance(aSrc.mBackgroundRadiance)
   , mPathRadiance(aSrc.mPathRadiance)
   , mVerbose(aSrc.mVerbose)
   , mAtmosphere(aSrc.mAtmosphere)
   , mRhoSeaLevel(aSrc.mRhoSeaLevel)
   , mPdMap(aSrc.mPdMap)
   , mSearch(aSrc.mSearch)
   , mReacquire(aSrc.mReacquire)
   , mTrack(aSrc.mTrack)
{
}

// virtual
WsfMode* WsfOpticalSensor::OpticalMode::Clone() const
{
   return new OpticalMode(*this);
}

// virtual
bool WsfOpticalSensor::OpticalMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);
   ok &= mSearch.Initialize();
   ok &= mReacquire.Initialize();
   ok &= mTrack.Initialize();
   ok &= mAntenna->Initialize(mSensorPtr);

   // We don't yet initialize the receiver because we don't deal in frequency and
   // frequency is a required input for a receiver.  At some point when we want
   // to do optical jamming or passive detection then we'll have to add it in.
   // ok &= mRcvr->Initialize();

   // Set the debug flag
   mRcvr->SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// virtual
bool WsfOpticalSensor::OpticalMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "search_glimpse_data")
   {
      UtInputBlock block(aInput, "end_search_glimpse_data");
      while (block.ReadCommand())
      {
         if (!mSearch.ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   else if (command == "reacquire_glimpse_data")
   {
      UtInputBlock block(aInput, "end_reacquire_glimpse_data");
      while (block.ReadCommand())
      {
         if (!mReacquire.ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   else if (command == "track_glimpse_data")
   {
      UtInputBlock block(aInput, "end_track_glimpse_data");
      while (block.ReadCommand())
      {
         if (!mTrack.ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   else if (command == "atmospheric_attenuation")
   {
      std::string per;
      std::string units;
      aInput.ReadValue(mAtmosphericAttenuation);
      aInput.ValueGreater(mAtmosphericAttenuation, 0.0);
      aInput.ReadValue(per);
      aInput.StringEqual(per, "per");
      aInput.ReadValue(units);
      mAtmosphericAttenuation /= UtInput::ConvertValueFrom(1.0, units, UtInput::cLENGTH);
   }
   else if (command == "background_radiance")
   {
      std::string units;
      aInput.ReadValue(mBackgroundRadiance);
      aInput.ValueGreater(mBackgroundRadiance, 0.0);
      aInput.ReadValue(units);
      std::string::size_type slashPos1 = units.find('/');
      std::string::size_type slashPos2 = units.rfind('/');
      if ((slashPos1 != 0) && (slashPos1 != std::string::npos) && (slashPos2 != std::string::npos) &&
          ((slashPos2 - slashPos1) > 1) && ((units.size() - slashPos2) > 1))
      {
         std::string powerUnits(units.substr(0, slashPos1));
         std::string angleUnits(units.substr(slashPos1 + 1, slashPos2 - slashPos1 - 1));
         std::string areaUnits(units.substr(slashPos2 + 1));
         double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
         double      angleFactor = aInput.ConvertValue(1.0, angleUnits, UtInput::cSOLID_ANGLE);
         double      areaFactor  = aInput.ConvertValue(1.0, areaUnits, UtInput::cAREA);
         double      multiplier  = powerFactor / angleFactor / areaFactor;
         mBackgroundRadiance *= multiplier;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown radiance units " + units);
      }
   }
   else if (command == "path_radiance")
   {
      std::string units;
      aInput.ReadValue(mPathRadiance);
      aInput.ValueGreater(mPathRadiance, 0.0);
      aInput.ReadValue(units);
      std::string::size_type slashPos1 = units.find('/');
      std::string::size_type slashPos2 = units.rfind('/');
      if ((slashPos1 != 0) && (slashPos1 != std::string::npos) && (slashPos2 != std::string::npos) &&
          ((slashPos2 - slashPos1) > 1) && ((units.size() - slashPos2) > 1))
      {
         std::string powerUnits(units.substr(0, slashPos1));
         std::string angleUnits(units.substr(slashPos1 + 1, slashPos2 - slashPos1 - 1));
         std::string areaUnits(units.substr(slashPos2 + 1));
         double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
         double      angleFactor = aInput.ConvertValue(1.0, angleUnits, UtInput::cSOLID_ANGLE);
         double      areaFactor  = aInput.ConvertValue(1.0, areaUnits, UtInput::cAREA);
         double      multiplier  = powerFactor / angleFactor / areaFactor;
         mPathRadiance *= multiplier;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown radiance units " + units);
      }
   }
   else if (command == "reacquisition_time")
   {
      aInput.ReadValueOfType(mReacquireTime, UtInput::cTIME);
      aInput.ValueGreater(mReacquireTime, 0.0);
   }
   else if (command == "verbose")
   {
      mVerbose = true;
   }
   else if (mAntenna->ProcessInput(aInput) || mRcvr->ProcessInput(aInput) || mRcvr->ProcessInputBlock(aInput))
   {
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else if (command == "test_transmittance")
   {
      double z1;
      double z2;
      double rg;
      std::cout << "Enter z1, z2, rg (in meters): ";
      while (std::cin >> z1 >> z2 >> rg)
      {
         double t = ComputeSimpleTransmittance(mAtmosphericAttenuation, z1, z2, rg);
         std::cout << "z1=" << z1 << " z2=" << z2 << " Rg=" << rg << " t=" << t << std::endl;
         std::cout << "Enter z1, z2, rg (in meters): ";
      }
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOpticalSensor::OpticalMode::AttemptToDetect(double           aSimTime,
                                                    WsfPlatform*     aTargetPtr,
                                                    Settings&        aSettings,
                                                    WsfSensorResult& aResult)
{
   bool detected = false;
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   // check to see if this is the first detection attempt against this target
   // if it is, add the target to the PdMap
   auto it = mPdMap.find(aTargetPtr->GetIndex());
   if (it == mPdMap.end())
   {
      // add target to map
      CumPd initialPds(1.0, 0.0, 0.0);
      mPdMap[aTargetPtr->GetIndex()] = initialPds;
   }

   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current
   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, mRcvr.get());
      }
   }

   if ((aResult.mFailedStatus == 0) && (aResult.BeginOneWayInteraction(mRcvr.get(), aTargetPtr) == 0))
   {
      // Set the position of the antenna.
      aResult.SetReceiverBeamPosition();

      // Determine the optical radiant intensity of the target.
      // (i.e.: The source contrast... I_s)

      // Determine the projected area of the target. (A_proj)
      aResult.ComputeOpticalSigAzEl();
      aResult.mOpticalSig =
         WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);

      double inherentContrast = WsfInherentContrast::GetValue(aTargetPtr,
                                                              aResult.mOpticalSigAz,
                                                              aResult.mOpticalSigEl,
                                                              WsfOpticalSignature::UsesInherentContrast(aTargetPtr));

      // Compute the atmospheric transmittance.
      aResult.mAbsorptionFactor = ComputeAtmosphericTransmittance(aResult);

      // Compute the background radiance at the sensor
      double backgroundRadianceAtSensor = mBackgroundRadiance * aResult.mAbsorptionFactor + mPathRadiance;

      // Compute the contrast at the sensor
      double contrastAtSensor = inherentContrast * aResult.mAbsorptionFactor; // worst case without background radiance
      if (backgroundRadianceAtSensor > 0.0)
      {
         contrastAtSensor *= mBackgroundRadiance / backgroundRadianceAtSensor; // scale it with the background if non-zero
      }

      // Account for structural masking.
      contrastAtSensor *= aResult.mMaskingFactor;

      // Compute solid angle of target at sensor
      double range      = aResult.mRcvrToTgt.mRange;
      double targetSize = aResult.mOpticalSig / (range * range);

      // Determine the probability of detection
      aResult.mPd = ComputeProbabilityOfDetection(targetSize, contrastAtSensor, aTargetPtr->GetIndex());
      if (aResult.CategoryIsSet())
      {
         aResult.mPd = std::max(0.0, aResult.mPd - aResult.mZoneAttenuationValue);
      }

      // see if the resulting Pd results in detection
      if (aResult.mFailedStatus == 0)
      {
         aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         if (aResult.mPd >= aSettings.mRequiredPd)
         {
            // Check terrain masking only if all other checks succeed
            detected = !aResult.MaskedByTerrain();
         }
         else
         {
            aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         }
      }
   }

   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   // Determine the impact (if any) that an external script should have on detection status
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);

   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Debug Info.";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// virtual
void WsfOpticalSensor::OpticalMode::Deselect(double aSimTime)
{
   mRcvr->Deactivate();
}

// virtual
void WsfOpticalSensor::OpticalMode::Select(double aSimTime)
{
   mRcvr->Activate();
}

//! Compute the atmospheric transmittance for the current interaction.
//!
//! @param aResult The current interaction data.
//! @returns The atmospheric transmittance in the range [0, 1].
double WsfOpticalSensor::OpticalMode::ComputeAtmosphericTransmittance(WsfSensorResult& aResult)
{
   double transmittance = 1.0;
   if (mAtmosphericAttenuation > 0.0)
   {
      // Simple attenuation model.

      // Compute the approximate ground range by computing the angle between the
      // position vectors and using the definition of the dot product.

      double dot         = UtVec3d::DotProduct(aResult.mRcvrLoc.mLocWCS, aResult.mTgtLoc.mLocWCS);
      double magA        = UtVec3d::Magnitude(aResult.mRcvrLoc.mLocWCS);
      double magB        = UtVec3d::Magnitude(aResult.mTgtLoc.mLocWCS);
      double cosTheta    = dot / (magA * magB);
      double theta       = acos(std::min(std::max(cosTheta, -1.0), 1.0));
      double groundRange = UtSphericalEarth::cEARTH_RADIUS * theta;

      transmittance =
         ComputeSimpleTransmittance(mAtmosphericAttenuation, aResult.mRcvrLoc.mAlt, aResult.mTgtLoc.mAlt, groundRange);
   }
   return transmittance;
}


//! Determine the simplistic transmittance along a path.
//!
//! @param aAttenuationPerMeter [input] The attenuation per meter.
//! @param aHeightMSL_1         [input] The height MSL of the first  object (meters)
//! @param aHeightMSL_2         [input] The height MSL of the second object (meters)
//! @param aGroundRange         [input] The ground range between the objects (meters)
//!
//! @returns The transmittance in the range [0, 1]

// protected
double WsfOpticalSensor::OpticalMode::ComputeSimpleTransmittance(double aAttenuationPerMeter,
                                                                 double aHeightMSL_1,
                                                                 double aHeightMSL_2,
                                                                 double aGroundRange)
{
   // Sort the heights so that the first point is lower than the second point.

   double zMin = std::max(std::min(aHeightMSL_1, aHeightMSL_2), 0.0);
   double zMax = std::max(std::max(aHeightMSL_1, aHeightMSL_2), 0.0);

   double const1 = (0.5 / mRhoSeaLevel) * aAttenuationPerMeter;

   // Determine the elevation angle of the path.

   double dz       = zMax - zMin;
   double dg       = aGroundRange;
   double ds       = sqrt(dg * dg + dz * dz);
   double sinTheta = dz / ds;

   // Integrate through the layers.

   static const double cDELTA_Z = 1000.0;
   double              z2       = zMin;
   double              rho2     = mAtmosphere.Density(z2);
   double              t        = 1.0;
   bool                done     = false;
   while (!done)
   {
      // Calculate the altitude at the top of the current layer and get the density.

      double z1   = z2;
      double rho1 = rho2;
      z2          = z1 + cDELTA_Z;
      if (z2 >= zMax)
      {
         z2   = zMax;
         done = true;
      }
      rho2 = mAtmosphere.Density(z2);

      // Calculate the path length in the current layer.

      dz = z2 - z1;
      ds = aGroundRange; // Assume horizontal path...
      if (sinTheta != 0.0)
      {
         ds = dz / sinTheta;
      }

      // Calculate the attenuation along the path segment.

      double a = const1 * (rho1 + rho2) * ds;
      t *= (1.0 - a);
      // cout << "z1=" << z1 << " ds=" << ds << " a12=" << a12 << " t=" << t << '\n';
   }
   return t;
}

//! Compute the probability of detection.
//!
//! @param aSize [input] The target solid angle at the sensor
//! @param aContrast  [input] The contrast of the target at the sensor
//! @param aIndex [input] The Index of the target for a Pd map lookup
//!
//! @returns The probability of detection in the range [0, 1]
double WsfOpticalSensor::OpticalMode::ComputeProbabilityOfDetection(double aSize, double aContrast, size_t aIndex)
{
   // compute search, reacquire and track probabilities
   double searchProb    = mSearch.ProbabilityOfDetection(aSize, aContrast);
   double reacquireProb = mReacquire.ProbabilityOfDetection(aSize, aContrast);
   double trackProb     = mTrack.ProbabilityOfDetection(aSize, aContrast);

   // determine number of glimpses in reacquisition interval
   unsigned int numReacqGlimpses = std::max(static_cast<unsigned int>(1.0 / mGlimpseTime * mReacquireTime + 0.5), 1U);

   // determine number of glimpses in sensor period
   unsigned int numGlimpses = std::max(static_cast<unsigned int>(1.0 / mGlimpseTime * mFrameTime + 0.5), 1U);

   // get cumulative Pds against this target
   CumPd oldPd = mPdMap[aIndex];

   // calculate probability of search, reacquire and track
   double pSearch    = oldPd.mSearchPd;
   double pReacquire = oldPd.mReacquirePd;
   double pTrack     = oldPd.mTrackPd;
   for (unsigned int i = 0; i < numGlimpses; ++i)
   {
      pTrack     = trackProb * pTrack + reacquireProb * pReacquire + searchProb * pSearch;
      pSearch    = (1.0 - searchProb) * pSearch + (1.0 - reacquireProb) * pReacquire / numReacqGlimpses;
      pReacquire = 1.0 - pTrack - pSearch;
   }

   oldPd.mSearchPd    = pSearch;
   oldPd.mReacquirePd = pReacquire;
   oldPd.mTrackPd     = pTrack;

   // store cumulative probabilities in map
   mPdMap[aIndex] = oldPd;

   return pTrack;
}

//! call each glimpse probability to clean-up PdMap
//! virtual
void WsfOpticalSensor::OpticalMode::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   mPdMap.erase(aPlatformPtr->GetIndex());
}

// ****************************************************************************
// Definition for the nested class that implements the glimpse probabilities
WsfOpticalSensor::OpticalMode::GlimpseProbability::GlimpseProbability()
   : mAzimuthFOV(5.0)
   , mMinElevation(0.0)
   , mMaxElevation(5.0)
   , mNumIterations(1000)
   , mMagnification(1.0)
   , mApparentHalfAngle(45.0)
   , mResolution(0.001)
   , mGain(1.0)
   , mOcularIntegration(0.05)
   , mProbabilityDistribution()
{
}

// virtual
WsfOpticalSensor::OpticalMode::GlimpseProbability* WsfOpticalSensor::OpticalMode::GlimpseProbability::Clone() const
{
   return new GlimpseProbability(*this);
}

// virtual
bool WsfOpticalSensor::OpticalMode::GlimpseProbability::Initialize()
{
   ComputeProbabilityDistribution();
   return true;
}

//! computes the probability distribution of the angular separation between a randomly
//! distributed target and a randomly distributed LOS points in a given FOV
//! virtual protected
void WsfOpticalSensor::OpticalMode::GlimpseProbability::ComputeProbabilityDistribution()
{
   // resize and initialize probability vector
   mProbabilityDistribution.resize(181);

   std::vector<unsigned int> iStats(180, 0);

   double theta0 = mAzimuthFOV * UtMath::cRAD_PER_DEG;

   double sinPhiMax = sin(mMaxElevation * UtMath::cRAD_PER_DEG);
   double sinPhiMin = sin(mMinElevation * UtMath::cRAD_PER_DEG);
   double delSinPhi = sinPhiMax - sinPhiMin;

   // random number generator
   ut::Random sRandom = ut::Random();

   // iterate random occurrences (Monte Carlo)
   for (int i = 0; i < mNumIterations; ++i)
   {
      // let U = random(iseed);
      //
      // probability distribution for azimuth is uniform:
      // p(theta) = 1 / theta0 for 0 <= theta <= theta0 <= 2 * PI
      // and the cumulative is
      // p(theta) = theta / theta0
      // therefore we generate angles
      // theta = theta0 * U
      //
      // probability distribution for elevation (phi) is not uniform:
      // cos(phi) / (sin(phiMax) - sin(phiMin))
      // for -PI/2 <= elMin <= el <= elMax <= PI/2
      // and the cumulative is
      // p(phi) = (sin(phi) + sin(phiMin)) / ((sin(phiMax) - sin(phiMin))
      // therefore we generate angles
      // phi = asin(sin(phiMin) + (sin(phiMax) - sin(phiMin)) * U)

      // generate random LOS unit vector
      double theta  = sRandom.Uniform(0.0, theta0);
      double sinPhi = sinPhiMin + sRandom.Uniform(0.0, delSinPhi);
      double cosPhi = sqrt(1.0 - sinPhi * sinPhi);
      double x1     = cosPhi * cos(theta);
      double y1     = cosPhi * sin(theta);
      double z1     = sinPhi;

      // generate random target unit vector
      theta     = sRandom.Uniform(0.0, theta0);
      sinPhi    = sinPhiMin + sRandom.Uniform(0.0, delSinPhi);
      cosPhi    = sqrt(1.0 - sinPhi * sinPhi);
      double x2 = cosPhi * cos(theta);
      double y2 = cosPhi * sin(theta);
      double z2 = sinPhi;

      // calculate and record angular separation (dot product)
      double temp  = x1 * x2 + y1 * y2 + z1 * z2;
      double alpha = 0.0;
      if (temp < 1.0)
      {
         alpha = acos(temp) * UtMath::cDEG_PER_RAD;
      }

      // increment histogram
      int ialpha = static_cast<int>(alpha);
      ialpha     = std::min(std::max(ialpha, 0), 179);
      ++iStats[ialpha];
   }

   // compute cumulative probability distribution
   mProbabilityDistribution[0] = 0.0;
   for (unsigned int j = 1; j < 181; ++j)
   {
      mProbabilityDistribution[j] = mProbabilityDistribution[j - 1] + iStats[j - 1] / static_cast<float>(mNumIterations);
   }
}

//! calculates the glimpse probability of detection based on the ratio of the apparent
//! contrast of divided by the threshold contrast based on Army contrast model
//! assumption of a standard normal distribution with mean of unity and sigma of 0.32
//! probability of glimpse if the integral up to the deviate of (C/CT - 1) / sigma
//! of the normal standard curve
//! virtual protected
double WsfOpticalSensor::OpticalMode::GlimpseProbability::ContrastRatioToPd(double aRatio)
{
   static TblIndVarE<double> RatioIndex;
   RatioIndex.SetValues(0.0, 1.9, 20);

   static TblDepVar1<double> PdData;
   PdData.Resize(20);
   PdData.Set(0.0000, 0);
   PdData.Set(0.0000, 1);
   PdData.Set(0.0002, 2);
   PdData.Set(0.0010, 3);
   PdData.Set(0.0040, 4);
   PdData.Set(0.0136, 5);
   PdData.Set(0.0385, 6);
   PdData.Set(0.0924, 7);
   PdData.Set(0.1884, 8);
   PdData.Set(0.3293, 9);
   PdData.Set(0.5000, 10);
   PdData.Set(0.6707, 11);
   PdData.Set(0.8116, 12);
   PdData.Set(0.9075, 13);
   PdData.Set(0.9614, 14);
   PdData.Set(0.9864, 15);
   PdData.Set(0.9960, 16);
   PdData.Set(0.9990, 17);
   PdData.Set(0.9998, 18);
   PdData.Set(1.0000, 19);

   // interpolate to find proper Pd value
   TblLookupLE<double> lookup;
   lookup.Lookup(RatioIndex, aRatio);

   return TblEvaluate(PdData, lookup);
}

//! models contrast threshold with data from
//! Poe, Arthur C. III
//! "A Model for Visual Detection by Ground Observers"
//! US Army Missile Command, AD-A017-599, Report RD-75-30
//! October, 1974
//! virtual protected
double WsfOpticalSensor::OpticalMode::GlimpseProbability::ContrastThreshold(double aOmega, double aLambda)
{
   // error check on input angles
   if ((aOmega < 0.0) || (aLambda < 0.0) || (aLambda > 90.0))
   {
      ut::log::fatal() << "In Glimpse Probability:: ContrastThreshold().";
      exit(1);
   }

   // alpha = angular subtence (minutes arc)
   double alpha = 3879.082 * sqrt(aOmega);

   if ((aLambda >= 0.0) && (aLambda <= 0.6))
   {
      return ContrastThreshold0(alpha);
   }

   if ((aLambda > 0.6) && (aLambda <= 15.0))
   {
      double value = ContrastThreshold0(alpha) + (aLambda - 0.6) * ContrastThreshold1(alpha);
      return value;
   }

   if (alpha <= 9.1)
   {
      double value =
         (ContrastThreshold0(alpha) + 14.4 * ContrastThreshold1(alpha)) * exp(0.000643 * (aLambda * aLambda - 225.0));
      return value;
   }
   else
   {
      double value = (ContrastThreshold0(alpha) + 14.4 * ContrastThreshold1(alpha)) *
                     pow((690.0 / alpha), (0.0001486 * (aLambda * aLambda - 225.0)));
      return value;
   }
}

//! part of Army model for contrast threshold
//! virtual protected
double WsfOpticalSensor::OpticalMode::GlimpseProbability::ContrastThreshold0(double aAlpha)
{
   // define static data
   static const double a0[8] = {0.240, 0.240, 0.155, 0.108, 0.0524, 0.0327, 0.0214, 0.0128};

   static const double b0[8] = {2.000, 1.982, 1.507, 1.220, 0.736, 0.473, 0.289, 0.117};

   static const double limit[8] = {1.0, 2.5, 3.5, 4.5, 6.0, 10.0, 20.0, 120.0};

   // error check
   if (aAlpha < 0.0)
   {
      ut::log::fatal() << "In Glimpse Probability::ContrastThreshold0().";
      exit(1);
   }

   if (aAlpha == 0.0)
   {
      return 1.0e9;
   }

   for (unsigned int i = 0; i < 8; ++i)
   {
      if (aAlpha <= limit[i])
      {
         double value = a0[i] / pow(aAlpha, b0[i]);
         return value;
      }
   }

   return a0[7] / pow(limit[7], b0[7]);
}

//! part of Army model for contrast threshold
//! virtual protected
double WsfOpticalSensor::OpticalMode::GlimpseProbability::ContrastThreshold1(double aAlpha)
{
   // define static data
   static const double a0[7] = {0.525, 0.525, 0.431, 0.301, 0.164, 0.0728, 0.0506};

   static const double b0[7] = {2.000, 2.106, 1.821, 1.561, 1.269, 0.977, 0.878};

   static const double limit[7] = {1.0, 2.0, 4.0, 8.0, 16.0, 40.0, 120.0};

   // error check
   if (aAlpha < 0.0)
   {
      ut::log::fatal() << "In Glimpse Probability::ContrastThreshold1().";
      exit(1);
   }

   if (aAlpha == 0.0)
   {
      return 1.0e9;
   }

   for (unsigned int i = 0; i < 7; ++i)
   {
      if (aAlpha <= limit[i])
      {
         double value = a0[i] / pow(aAlpha, b0[i]);
         return value;
      }
   }

   return a0[6] / pow(limit[6], b0[6]);
}

//! piecewise linear interpolation of the cumulative probability distribution
//! PDist over the interval thetaMin, thetaMax
//! virtual protected
double WsfOpticalSensor::OpticalMode::GlimpseProbability::InterpolateDistribution(double aThetaMin, double aThetaMax)
{
   int    ix     = static_cast<int>(aThetaMin);
   double slope  = mProbabilityDistribution[ix + 1] - mProbabilityDistribution[ix];
   double yInter = mProbabilityDistribution[ix] - slope * ix;
   double y1     = slope * aThetaMin + yInter;

   ix        = static_cast<int>(aThetaMax);
   slope     = mProbabilityDistribution[ix + 1] - mProbabilityDistribution[ix];
   yInter    = mProbabilityDistribution[ix] - slope * ix;
   double y2 = slope * aThetaMax + yInter;

   return y2 - y1;
}

//! compute single glimpse (1/3 second) probability of detection versus target size
//! and apparent contrast
//! virtual
double WsfOpticalSensor::OpticalMode::GlimpseProbability::ProbabilityOfDetection(double aSize, double aContrast)
{
   // in an optically-aided situation, mMagnification > 1, two processes are occuring
   // involving both the actual and apparent angular parameters

   // ocular integration interval (deg)
   double delta  = mOcularIntegration;
   double delta2 = delta * 0.5;

   // magnify size (sr)
   double apparentSize = aSize * mMagnification * mMagnification;

   // apply contrast gain/loss
   double apparentContrast = aContrast * mGain;

   // account for blurring / low resolution display devices
   if (apparentSize < mResolution)
   {
      apparentContrast *= apparentSize / mResolution;
      apparentSize = mResolution;
   }

   // ocular integration
   int    nStep = static_cast<int>(mApparentHalfAngle / delta + 0.5);
   double pd    = 0.0;
   for (int i = 0; i < nStep; ++i)
   {
      double theta            = delta * i;
      double apparentThetaAvg = theta + delta2;
      double thetaMin         = theta / mMagnification;
      double thetaMax         = (theta + delta) / mMagnification;

      pd += ContrastRatioToPd(apparentContrast / ContrastThreshold(apparentSize, apparentThetaAvg)) *
            InterpolateDistribution(thetaMin, thetaMax);
   }

   pd = std::min(pd, 1.0);

   return pd;
}

//! virtual
bool WsfOpticalSensor::OpticalMode::GlimpseProbability::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "azimuth_fov")
   {
      aInput.ReadValueOfType(mAzimuthFOV, UtInput::cANGLE);
      aInput.ValueInClosedRange(mAzimuthFOV, 0.0, UtMath::cTWO_PI);
      // store value as degrees
      mAzimuthFOV *= UtMath::cDEG_PER_RAD;
   }
   else if (command == "minimum_elevation")
   {
      aInput.ReadValueOfType(mMinElevation, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMinElevation, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      // store value as degrees
      mMinElevation *= UtMath::cDEG_PER_RAD;
   }
   else if (command == "maximum_elevation")
   {
      aInput.ReadValueOfType(mMaxElevation, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMaxElevation, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      aInput.ValueGreaterOrEqual(mMaxElevation, mMinElevation);
      // store value as degrees
      mMaxElevation *= UtMath::cDEG_PER_RAD;
   }
   else if (command == "number_of_iterations")
   {
      aInput.ReadValue(mNumIterations);
      aInput.ValueGreater(mNumIterations, 0);
   }
   else if (command == "magnification")
   {
      aInput.ReadValue(mMagnification);
      aInput.ValueGreaterOrEqual(mMagnification, 1.0);
   }
   else if (command == "apparent_half_angle_FOV")
   {
      aInput.ReadValueOfType(mApparentHalfAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mApparentHalfAngle, 0.0, UtMath::cPI_OVER_2);
      // store value as degrees
      mApparentHalfAngle *= UtMath::cDEG_PER_RAD;
   }
   else if (command == "minimum_resolution")
   {
      aInput.ReadValueOfType(mResolution, UtInput::cANGLE);
      aInput.ValueInClosedRange(mResolution, 0.0, 5.0 * UtMath::cRAD_PER_DEG);
      // convert resolution from a circular diameter (deg) to
      // angular size (sr)
      mResolution *= UtMath::cDEG_PER_RAD;
      mResolution = 2.39246e-4 * mResolution * mResolution;
   }
   else if (command == "contrast_gain")
   {
      aInput.ReadValue(mGain);
      aInput.ValueGreaterOrEqual(mGain, 0.0);
   }
   else if (command == "ocular_integration_interval")
   {
      aInput.ReadValueOfType(mOcularIntegration, UtInput::cANGLE);
      aInput.ValueInClosedRange(mOcularIntegration, 0.01 * UtMath::cRAD_PER_DEG, UtMath::cRAD_PER_DEG);
      // store value as degrees
      mOcularIntegration *= UtMath::cDEG_PER_RAD;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
