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

#include "WsfCompositeOpticalSignature.hpp"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>

#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtBlackBody.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtSun.hpp"
#include "WsfDateTime.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfOpticalEnvironment.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"

// static std::ostream& operator<<(std::ostream&  aOut,
//                                 const UtVec3d& aVec)
//{
//    aOut << "[ " << aVec[0] << ' ' << aVec[1] << ' ' << aVec[2] << " ]";
//    return aOut;
// }

// =================================================================================================
// This is a facade that acts as the platforms 'infrared_signature' if one wasn't specified.
// It simply redirects to this signature which has all of information necessary.
namespace
{
class InfraredSignatureFacade : public WsfInfraredSignature
{
public:
   explicit InfraredSignatureFacade(WsfCompositeOpticalSignature* aSignaturePtr)
      : WsfInfraredSignature()
      , mSignaturePtr(aSignaturePtr)
   {
   }

   // This should never be called.
   WsfInfraredSignature* Clone() const override
   {
      return nullptr; // return new InfraredSignatureFacade(*this);
   }

   std::vector<WsfStringId> GetStateNames() const override { return mSignaturePtr->GetStateNames(); }

   float GetSignature(double aSimTime, WsfStringId aStateId, WsfEM_Types::InfraredBand aBand, double aAzimuth, double aElevation) override
   {
      double                        radiantIntensity;
      WsfEM_Types::OpticalBand      optBand = static_cast<WsfEM_Types::OpticalBand>(aBand);
      WsfOpticalSignature::BandList bands;
      bands.push_back(optBand);
      mSignaturePtr->GetRadiantIntensity(aSimTime, aStateId, bands, aAzimuth, aElevation, radiantIntensity);
      return static_cast<float>(radiantIntensity);
   }

   WsfCompositeOpticalSignature* mSignaturePtr;
};
} // namespace

// =================================================================================================
//! Factory method called by WsfOpticalSignatureTypes.
// static
WsfOpticalSignature* WsfCompositeOpticalSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfOpticalSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_COMPOSITE_OPTICAL_SIGNATURE")
   {
      instancePtr = new WsfCompositeOpticalSignature();
   }
   return instancePtr;
}

// =================================================================================================
WsfCompositeOpticalSignature::WsfCompositeOpticalSignature()
   : WsfOpticalSignature()
{
   mRequiresPlatform = true;
}

// =================================================================================================
WsfCompositeOpticalSignature::WsfCompositeOpticalSignature(const WsfCompositeOpticalSignature& aSrc)
   : WsfOpticalSignature(aSrc)
   , mStates()
   , mDebugLevel(aSrc.mDebugLevel)
   , mDefaultSurface(aSrc.mDefaultSurface)
   , mCurrentStateId(aSrc.mCurrentStateId)
   , mGeometryUpdateMutex()
   , mGeometryUpdateTime(-1.0)
   //, mCurrentTime()
   //, mLoc_s()
   //, mLoc_p()
   //, mUnit_vel()
   //, mUnit_ps()
   //, mUnit_pe()
   //, mUnit_es()
   //, mDist_ps(0.0)
   //, mDist_pe(0.0)
   //, mDist_es(0.0)
   , mTempUpdateMutex()
   , mTempUpdateTime(-1.0)
   , mAtmosphere(aSrc.mAtmosphere)
{
   std::transform(aSrc.mStates.begin(),
                  aSrc.mStates.end(),
                  std::back_inserter(mStates),
                  [](const State* srcState) { return new State(*srcState); });
}

// =================================================================================================
// override
WsfCompositeOpticalSignature::~WsfCompositeOpticalSignature()
{
   for (auto& statePtr : mStates)
   {
      delete statePtr;
   }
}

// =================================================================================================
// override
WsfOpticalSignature* WsfCompositeOpticalSignature::Clone() const
{
   return new WsfCompositeOpticalSignature(*this);
}

// =================================================================================================
// override
bool WsfCompositeOpticalSignature::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok = WsfOpticalSignature::Initialize(aSimTime, aPlatformPtr);

   // If the user did not define an infrared_signature item then supply an interface to this signature that
   // will provide the infrared signature...

   if (ok)
   {
      // Install our facades for the infrared_signature and inherent_contrast, and warn the user if they explicitly
      // provided these items that they will be ignored and instead provided by this class.

      WsfSignatureInterface* interfacePtr =
         mPlatformPtr->GetSignatureList().GetInterface(WsfInfraredSignature::cSIGNATURE_INDEX);
      if (interfacePtr != nullptr)
      {
         if (!interfacePtr->GetInputType().IsNull())
         {
            auto out = ut::log::warning() << "The infrared signature will be provided by the optical signature model "
                                             "and not the Infrared Signature.";
            out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            out.AddNote() << "Infrared Signature: " << interfacePtr->GetInputType();
         }
         InfraredSignatureFacade* facadePtr = new InfraredSignatureFacade(this);
         interfacePtr->SetSignature(facadePtr);
      }
   }

   // (JAJ) - this is kind of a kludge, but the atmosphere that gets constructed in the constructor
   // may differ from the one created by the scenario. This makes them the same...

   // TODO ... but it doesn't work. Causes a crash in debug
   // TODO mAtmosphere = mPlatformPtr->GetScenario().GetAtmosphere();
   return ok;
}

// =================================================================================================
bool WsfCompositeOpticalSignature::InitializeType()
{
   bool ok = WsfOpticalSignature::InitializeType();

   if (mStates.empty())
   {
      ut::log::error() << "At least one state must implicitly or explicitly defined.";
      return false;
   }

   // Make sure the state 'default' exists.
   bool foundDefault =
      std::any_of(mStates.begin(),
                  mStates.end(),
                  [](const State* aStatePtr) { return aStatePtr->mStateId == UtStringIdLiteral("default"); });
   if (!foundDefault)
   {
      ok = false;
      ut::log::error() << "The state 'default' does not exist.";
   }

   // Make sure each state has at least one surface, and determine the maximum surface count in any state.
   for (const auto& statePtr : mStates)
   {
      if (statePtr->mSurfaces.empty())
      {
         ok       = false;
         auto out = ut::log::error() << "No surfaces defined for state.";
         out.AddNote() << "State: " << statePtr->mStateId;
      }

      // Ensure each surface has the required data...
      // And set the default location if not provided.
      UtVec3d defaultLocationECS(0.0, 0.0, 0.0);
      size_t  surfaceIndex = 0;
      for (auto& surfacePtr : statePtr->mSurfaces)
      {
         surfacePtr->mDebugLevel = mDebugLevel;
         if (!surfacePtr->InitializeType(defaultLocationECS))
         {
            auto out = ut::log::error() << "Error initializing surface.";
            out.AddNote() << "State: " << statePtr->mStateId;
            out.AddNote() << "Surface: " << surfaceIndex + 1;
            ok = false;
         }
         ++surfaceIndex;
      }

      // Check for direct adjacency between surfaces... exclude abutting surfaces...
      if (ok)
      {
         for (auto& surface1Ptr : statePtr->mSurfaces)
         {
            for (auto& surface2Ptr : statePtr->mSurfaces)
            {
               if (surface2Ptr != surface1Ptr)
               {
                  surface1Ptr->CheckAdjacency(*surface2Ptr);
               }
            }
         }
      }
   }
   return ok;
}

// =================================================================================================
// override
std::vector<WsfStringId> WsfCompositeOpticalSignature::GetStateNames() const
{
   std::vector<WsfStringId> names;
   names.reserve(mStates.size());
   for (const auto& state : mStates)
   {
      names.emplace_back(state->mStateId);
   }
   return names;
}

// =================================================================================================
// override
bool WsfCompositeOpticalSignature::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);
      mCurrentStateId = stateName;
      // Make sure the new state has not been used
      if (std::any_of(mStates.begin(),
                      mStates.end(),
                      [=](const State* aStatePtr) { return aStatePtr->mStateId == mCurrentStateId; }))
      {
         throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
      }
   }
   else if (mDefaultSurface.ProcessInput(aInput))
   {
   }
   else if (command == "surface")
   {
      std::string type;
      aInput.ReadValue(type);
      std::unique_ptr<Surface> surfacePtr(SurfaceFactory(type, mDefaultSurface));
      if (surfacePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Invalid surface type: " + type);
      }
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!surfacePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // If necessary create a new state to hold the surface...
      if (mStates.empty() ||                           // First surface in the "default" state
          mStates.back()->mStateId != mCurrentStateId) // First surface in a state that is different than the last one
      {
         mStates.push_back(new State(mCurrentStateId));
      }

      // ... and add the surface to the state
      mStates.back()->mSurfaces.push_back(surfacePtr.get());

      surfacePtr.release();
   }
   else if (command == "debug")
   {
      mDebugLevel = 1;
   }
   else if (command == "debug_level")
   {
      aInput.ReadValue(mDebugLevel);
      aInput.ValueGreaterOrEqual(mDebugLevel, 0U);
   }
   else
   {
      myCommand = WsfOpticalSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// override
float WsfCompositeOpticalSignature::GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation)
{
   double projectedArea = 0.0;
   GetProjectedArea(aSimTime, aStateId, aAzimuth, aElevation, projectedArea);
   return static_cast<float>(projectedArea);
}

// =================================================================================================
bool WsfCompositeOpticalSignature::GetProjectedArea(double      aSimTime,
                                                    WsfStringId aStateId,
                                                    double      aAzimuth,
                                                    double      aElevation,
                                                    double&     aProjectedArea)
{
   // Make sure our state member variables reflect the platform state at the current time.
   State* statePtr = SelectState(aStateId);
   if (aSimTime != mGeometryUpdateTime)
   {
      UpdateGeometry(aSimTime, *statePtr);
   }

   UtVec3d unit_po; // Unit vector from the platform to the observer
   mPlatformPtr->GetRelativeLocationWCS(aAzimuth, aElevation, 1.0, unit_po.GetData());

   double area = 0.0;
   for (const auto& surfacePtr : statePtr->mSurfaces)
   {
      double surfaceArea = surfacePtr->ProjectedArea(unit_po);
      area += surfaceArea;
      if (mDebugLevel > 0)
      {
         auto out = ut::log::debug() << "Projected Area:";
         out.AddNote() << "Area: " << surfaceArea;
         out.AddNote() << "Sum: " << area;
      }
   }
   aProjectedArea = area;
   return true;
}

// =================================================================================================
// override
bool WsfCompositeOpticalSignature::GetRadiantIntensity(double          aSimTime,
                                                       WsfStringId     aStateId,
                                                       const BandList& aBands,
                                                       double          aAzimuth,
                                                       double          aElevation,
                                                       double&         aRadiantIntensity)
{
   // Make sure our geometry member variables reflect the platform state at the current time.
   State* statePtr = SelectState(aStateId);
   if (aSimTime != mGeometryUpdateTime)
   {
      UpdateGeometry(aSimTime, *statePtr);
   }

   // Make sure our temperature member variables reflect the platform state at the current time
   if (aSimTime != mTempUpdateTime)
   {
      UpdateTemperature(aSimTime, *statePtr);
   }

   // Build the input structure to the radiant intensity computation. Lots of potential arguments, so this is
   // a structure to avoid having to potentially change multiple virtual function definitions.
   Surface::RI_Input input;
   input.mPlatformPtr = mPlatformPtr;
   mPlatformPtr->GetRelativeLocationWCS(aAzimuth, aElevation, 1.0, input.mUnit_po.GetData());
   input.mBands = aBands;

   aRadiantIntensity = std::accumulate(statePtr->mSurfaces.begin(),
                                       statePtr->mSurfaces.end(),
                                       0.0,
                                       [input](double sum, const Surface* aSurfacePtr)
                                       { return sum + aSurfacePtr->RadiantIntensity(input); });
   return true;
}

// =================================================================================================
WsfCompositeOpticalSignature::State* WsfCompositeOpticalSignature::SelectState(WsfStringId aId) const
{
   auto it =
      std::find_if(mStates.begin(), mStates.end(), [aId](const State* aStatePtr) { return aStatePtr->mStateId == aId; });

   if (it == std::end(mStates)) // use the default
   {
      it = std::find_if(mStates.begin(),
                        mStates.end(),
                        [](const State* aStatePtr) { return aStatePtr->mStateId == UtStringIdLiteral("default"); });
   }

   return *it;
}

// =================================================================================================
// Update the member variables that reflect the current platform state.
void WsfCompositeOpticalSignature::UpdateGeometry(double aSimTime, State& aState)
{
   std::lock_guard<std::recursive_mutex> lock(mGeometryUpdateMutex);
   if (aSimTime == mGeometryUpdateTime)
   {
      return; // Previous concurrent update has already occurred
   }

   // Update the vectors and distance between the platform, Sun and Earth.

   // Note: The Sun/Earth positions are not used in the current iteration of this model.
   //       If at some time we decide to implement reflections or time-of-day-dependent
   //       temperatures, then they will be needed...

   // Advance the calendar up to the current simulation time...
   // mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime, mCurrentTime);
   // UtSun::GetSunLocationWCS(mCurrentTime, mLoc_s);    // Location of the Sun
   // mPlatformPtr->GetLocationWCS(mLoc_p.GetData());    // Location of the platform
   // mPlatformPtr->GetVelocityWCS(mUnit_vel.GetData()); // Will be unit vector of platform velocity
   // mUnit_ps = mLoc_s - mLoc_p;                        // Will be unit vector from the platform to the Sun
   // mUnit_pe = - mLoc_p;                               // Will be unit vector from the platform to the Earth
   // mUnit_es = mLoc_s;                                 // Will be unit vector from the Earth to the Sun
   // mUnit_vel.Normalize();
   // mDist_ps = mUnit_ps.Normalize();                   // Distance from the platform to the Sun
   // mDist_pe = mUnit_pe.Normalize();                   // Distance from the platform to the Earth
   // mDist_es = mUnit_es.Normalize();                   // Distance from the Earth to the Sun

   Surface::UG_Input input;
   input.mPlatformPtr = mPlatformPtr;
   mPlatformPtr->GetWCSToECSTransform(input.mWCS_ToECS_Transform.mMat);

   // input.mUnit_vel = mUnit_vel;
   // input.mUnit_ps = mUnit_ps;
   // input.mUnit_pe = mUnit_pe;

   // Update the current thrust for the plume model...
   input.mCurrentThrust  = -1.0;
   WsfGuidedMover* gmPtr = dynamic_cast<WsfGuidedMover*>(mPlatformPtr->GetMover());
   if (gmPtr != nullptr)
   {
      input.mCurrentThrust = gmPtr->GetCurrentThrust();
   }

   input.mPrevSurfacePtr = nullptr;
   for (auto& surfacePtr : aState.mSurfaces)
   {
      surfacePtr->UpdateGeometry(input);
      input.mPrevSurfacePtr = surfacePtr;
   }

   mGeometryUpdateTime = aSimTime; // This must be done LAST before unlocking the mutex.
}

// =================================================================================================
// Update the member variables that reflect the current surface temperatures
void WsfCompositeOpticalSignature::UpdateTemperature(double aSimTime, State& aState)
{
   std::lock_guard<std::recursive_mutex> lock(mTempUpdateMutex);
   if (aSimTime == mTempUpdateTime)
   {
      return; // Previous concurrent update has already occurred
   }

   // Update data for temperature table lookup or adiabatic wall computation
   double altitude      = mPlatformPtr->GetAltitude();
   double speed         = mPlatformPtr->GetSpeed();
   double temperature   = mAtmosphere.Temperature(altitude);
   double sonicVelocity = mAtmosphere.SonicVelocity(altitude);

   // NOTE There is an issue with UtAtmosphere such that it generates a zero temperature
   // NOTE at altitudes > 61 KM. If we get a zero temperature we will compute it ourselves
   // NOTE using the same algorithm as UtAtmosphereData::CalcStdTemperature.
   if (temperature <= 0.0)
   {
      // From UtAtmosphere::CalcStdTemperature.
      if ((51000 <= altitude) && (altitude < 71000))
      {
         temperature = 270.65 - 2.8 * (altitude - 51000) / 1000;
      }
      else if ((71000 <= altitude) && (altitude < 84852))
      {
         temperature = 214.65 - 2.0 * (altitude - 71000) / 1000;
      }
      else if (84852 <= altitude)
      {
         temperature = 186.946;
      }
   }

   Surface::UT_Input input;
   input.mPlatformPtr        = mPlatformPtr;
   input.mAmbientTemperature = temperature;

   // The 'adiabatic adjustment factor' is used to compensate for the fact that the adiabatic wall approximation
   // really isn't effective for high altitudes and Mach numbers. Density, and hence skin friction, decreases with
   // altitude. A rocket flying at Mach 10+ at 60km probably doesn't glow :)
   //
   // The adjustment starts at 1.0 at lower altitudes and becomes zero as the density approaches zero.

   input.mAdiabaticAdjustmentFactor       = 1.0;
   static const double cAC_BEGIN_ALTITUDE = 30000.0; // Altitude at which the correction starts
   // static const double cAC_END_ALTITUDE = 84000.0;   // Altitude at which the density is effectively zero
   if (altitude > cAC_BEGIN_ALTITUDE)
   {
      double rho_beg                   = mAtmosphere.Density(cAC_BEGIN_ALTITUDE);
      double rho                       = mAtmosphere.Density(altitude);
      input.mAdiabaticAdjustmentFactor = 1.0 - ((rho_beg - rho) / rho_beg);
   }

   input.mLookupArgs[cIV_TIME]     = aSimTime - mPlatformPtr->GetCreationTime();
   input.mLookupArgs[cIV_THROTTLE] = 0.0; // TODO - throttle not yet implemented
   input.mLookupArgs[cIV_ALTITUDE] = altitude;
   input.mLookupArgs[cIV_SPEED]    = speed;
   input.mLookupArgs[cIV_MACH]     = speed / sonicVelocity;

   for (auto& surfacePtr : aState.mSurfaces)
   {
      surfacePtr->UpdateTemperature(input);
   }

   mTempUpdateTime = aSimTime; // This must be done LAST before unlocking the mutex.
}

// =================================================================================================
// Nested class State
// =================================================================================================

// =================================================================================================
WsfCompositeOpticalSignature::State::State(const State& aSrc)
   : mStateId(aSrc.mStateId)
   , mSurfaces()
{
   std::transform(aSrc.mSurfaces.begin(),
                  aSrc.mSurfaces.end(),
                  std::back_inserter(mSurfaces),
                  [](const Surface* srcSurface) { return srcSurface->Clone(); });
}

// =================================================================================================
WsfCompositeOpticalSignature::State::~State()
{
   for (auto& surfacePtr : mSurfaces)
   {
      delete surfacePtr;
   }
}

// =================================================================================================
// Nested class Surface
// =================================================================================================

// =================================================================================================
WsfCompositeOpticalSignature::Surface::Surface()
   : mTemperatureType(cTT_UNDEFINED)
   , mTemperaturePtr(nullptr)
   , mTemperatureOffset(0.0)
   , mRecoveryFactor(0.85)
   , mGamma(1.4)
   , mLocationECS(0.0, 0.0, 0.0)
   , mUseDefaultLocation(true)
   , mECS_ToSCS_Transform()
   , mTemperature(0.0)
   , mWCS_ToSCS_Transform()
   , mAftRadius(0.0)
   , mPlumeRadius(0.0)
   , mDebugLevel(0)
{
   mWCS_ToSCS_Transform.Identity();
}

// =================================================================================================
WsfCompositeOpticalSignature::Surface* WsfCompositeOpticalSignature::Surface::Clone() const
{
   return new Surface(*this);
}

// =================================================================================================
bool WsfCompositeOpticalSignature::Surface::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = true;
   if (mTemperatureType == cTT_UNDEFINED)
   {
      ut::log::info() << "Surface temperature must be defined.";
      ok = false;
   }
   return ok;
}

// =================================================================================================
bool WsfCompositeOpticalSignature::Surface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "location")
   {
      std::string units;
      double      x;
      double      y;
      double      z;
      aInput.ReadValue(x);
      aInput.ReadValue(y);
      aInput.ReadValue(z);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
      mLocationECS.Set(x * multiplier, y * multiplier, z * multiplier);
      mUseDefaultLocation = false;
   }
   else if (command == "temperature")
   {
      std::string nextWord;
      aInput.ReadValue(nextWord);
      mTemperaturePtr = nullptr; // delete existing definition before reading new one...
      // mTemperatureType = cTT_UNDEFINED;
      if (nextWord == "ambient")
      {
         mTemperatureType = cTT_AMBIENT;
      }
      else if (nextWord == "adiabatic_wall")
      {
         mTemperatureType = cTT_ADIABATIC_WALL;
      }
      else
      {
         mTemperatureType = cTT_TABLE;
         aInput.PushBack(nextWord);
         // Register allowable independent variables.
         UtTable::Parameters parameters;
         parameters.AddRealParameter("time", UtInput::cTIME, UtTable::ValueGE(0.0), cIV_TIME);
         parameters.AddRealParameter("throttle", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), cIV_THROTTLE);
         parameters.AddRealParameter("altitude", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE);
         parameters.AddRealParameter("speed", UtInput::cSPEED, UtTable::ValueGE(0.0), cIV_SPEED);
         parameters.AddRealParameter("mach", UtInput::cLENGTH, UtTable::ValueGE(0.0), cIV_MACH);
         mTemperaturePtr = UtTable::LoadInstance(aInput, UtInput::cTEMPERATURE, UtTable::ValueGE(0.0), parameters);
      }
   }
   else if (command == "temperature_offset")
   {
      aInput.ReadValueOfType(mTemperatureOffset, UtInput::cTEMPERATURE);
   }
   else if (command == "recovery_factor")
   {
      aInput.ReadValue(mRecoveryFactor);
      aInput.ValueGreater(mRecoveryFactor, 0.0);
      aInput.ValueLessOrEqual(mRecoveryFactor, 1.0);
   }
   else if (command == "gamma")
   {
      aInput.ReadValue(mGamma);
      aInput.ValueGreater(mGamma, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Compute the projected area of the surface in the direction of the viewer.
//! @param aUnit_v  The WCS unit vector pointing from the platform towards the viewer
//! @returns The projected area (m^2)
double WsfCompositeOpticalSignature::Surface::ProjectedArea(const UtVec3d& aUnit_v) const
{
   return 0.0;
}

// =================================================================================================
double WsfCompositeOpticalSignature::Surface::RadiantIntensity(const RI_Input& aInput) const
{
   double L    = WsfOpticalEnvironment::GetBlackbodyRadiance(aInput.mBands, mTemperature);
   double A_po = ProjectedArea(aInput.mUnit_po);
   double I    = L * A_po; // W/sr <- W/m^2/sr * m^2
   if (mDebugLevel > 0)
   {
      auto out = ut::log::debug() << "Radiant Intensity Data Report.";
      out.AddNote() << "Temperature: " << mTemperature;
      out.AddNote() << "A_po: " << A_po;
      out.AddNote() << "L: " << L;
      out.AddNote() << "I: " << I;
   }
   return I;
}

// =================================================================================================
void WsfCompositeOpticalSignature::Surface::CheckAdjacency(const Surface& aOther) {}

// =================================================================================================
double WsfCompositeOpticalSignature::Surface::AdjacentArea(const UtVec3d& aOtherFaceLocECS,
                                                           const UtVec3d& aOtherFaceNormalECS) const
{
   return 0.0;
}

// =================================================================================================
//! Update the geometry of the surface.
//! This updates the WCS->SCS transform. Derived classes may also update their shape definition,
//! but they also have to make sure the WCS->SCS transform is updated.
//! @param aInput The reference data for computing the WCS->SCS transform.
void WsfCompositeOpticalSignature::Surface::UpdateGeometry(const UG_Input& aInput)
{
   // Update the WCS-to-SCS (surface coordinate system) transform.
   UtMat3d::Set(mWCS_ToSCS_Transform.mMat, aInput.mWCS_ToECS_Transform.mMat);
}

// =================================================================================================
// Update the current temperature of the surface based on current conditions.
// @param aData Structure of input arguments
// @param aLookupArgs [input] the arguments for a possible table lookup.
void WsfCompositeOpticalSignature::Surface::UpdateTemperature(const UT_Input& aInput)
{
   // Assume the temperature is the ambient temperature, plus/minus the specified temperature offset.
   // Don't let it get below the temperature of deep space... (~3 deg-K)

   mTemperature = std::max(3.0, aInput.mAmbientTemperature + mTemperatureOffset);

   if (mTemperaturePtr != nullptr)
   {
      mTemperature = mTemperaturePtr->Lookup(aInput.mLookupArgs);
   }
   else if (mTemperatureType == cTT_ADIABATIC_WALL)
   {
      // Compute temperature based on adiabatic wall assumption.
      double mach = aInput.mLookupArgs[cIV_MACH];

      // Scaling factor to convert ambient to stagnation temperature.
      //
      // NOTE: See WsfCompositeOpticalSignature::UpdateTemperature to see the description of 'mAdiabaticAdjustment'.
      double scaleFactor =
         1.0F + aInput.mAdiabaticAdjustmentFactor * (mRecoveryFactor * 0.5 * (mGamma - 1.0F) * mach * mach);

      mTemperature = aInput.mAmbientTemperature * scaleFactor;
   }
}

// =================================================================================================
// Two faces are adjacent if they have the same location and their normal vectors point in opposite direction.
bool WsfCompositeOpticalSignature::Surface::AreAdjacent(const UtVec3d& aOtherFaceLocECS,
                                                        const UtVec3d& aOtherFaceNormalECS,
                                                        const UtVec3d& aMyFaceLocPCS,
                                                        const UtVec3d& aMyFaceNormalPCS) const
{
   // Translate my face origin from SCS/PCS to ECS
   UtVec3d myFaceLocECS(mLocationECS + aMyFaceLocPCS);

   // Transform my face normal from SCS/PCS to ECS
   UtVec3d myFaceNormalECS(mECS_ToSCS_Transform.InverseMultiply(aMyFaceNormalPCS));

   // The faces are adjacent if their origin is the same and the normals are in the opposite direction...
   bool    areAdjacent = false;
   UtVec3d locDelta(aOtherFaceLocECS - myFaceLocECS);
   if ((locDelta.MagnitudeSquared() < 0.01) && (aOtherFaceNormalECS.DotProduct(myFaceNormalECS) < -0.99999))
   {
      areAdjacent = true;
   }
   return areAdjacent;
}

// =================================================================================================
bool WsfCompositeOpticalSignature::Surface::UpdateSharedFaceArea(const Surface& aOther,
                                                                 const UtVec3d& aMyFaceLocPCS,
                                                                 const UtVec3d& aMyFaceNormalPCS,
                                                                 double         aMaxSharedArea,
                                                                 double&        aSharedArea)
{
   // Translate my face origin from SCS/PCS to ECS
   UtVec3d myFaceLocECS(mLocationECS + aMyFaceLocPCS);

   // Transform my face normal from SCS/PCS to ECS
   UtVec3d myFaceNormalECS(mECS_ToSCS_Transform.InverseMultiply(aMyFaceNormalPCS));

   bool   isShared(false);
   double otherFaceArea = aOther.AdjacentArea(myFaceLocECS, myFaceNormalECS);
   if (otherFaceArea > 0.0)
   {
      isShared      = true;
      otherFaceArea = std::min(otherFaceArea, aMaxSharedArea);
      aSharedArea   = std::max(aSharedArea, otherFaceArea);
   }
   return isShared;
}

// =================================================================================================
//! Update the default location.
//! If the surface did not have a location defined, this will set it to a default that uses the previous
//! surface. It also updates the default location for the next surface.
//!
//! The default works under the assumption that surfaces stack from +X to -X.
//!
//! @param aDefaultLocationECS On input this is end location (most aft) of the previous surface.
//! On output it will be updated with the end location of this surface.
//! @param aSizeSCS The size of the bounding box in the SCS/PCS frame.
//! @param aOffsetSCS The offset of the location from the centroid in the SCS/PCS frame.
void WsfCompositeOpticalSignature::Surface::UpdateLocation(UtVec3d&       aDefaultLocationECS,
                                                           const UtVec3d& aSizeSCS,
                                                           const UtVec3d& aOffsetSCS)
{
   // First rotate the SCS box minima/maxima into the untranslated ECS frame.
   UtVec3d sizeECS(mECS_ToSCS_Transform.InverseMultiply(aSizeSCS));
   UtVec3d offsetECS(mECS_ToSCS_Transform.InverseMultiply(aOffsetSCS));

   // If no location was specified use the incoming default, with the X coordinate shifted
   // aft by half the ECS X size;
   if (mUseDefaultLocation)
   {
      mLocationECS = aDefaultLocationECS;
      mLocationECS[0] -= (0.5 * sizeECS[0]); // Location of the centroid
      mLocationECS[0] += offsetECS[0];       // Offset of the surface origin from the centroid
   }

   // Update the X coordinate for the next surface by moving it aft (-X)
   aDefaultLocationECS[0] -= sizeECS[0];

   // Also update the real or effective radius of the aft surface.
   // This assumes the rear face is circular, but if it is rectangular then the error won't be much.
   mAftRadius = 0.5 * std::max(sizeECS[1], sizeECS[2]);

   if (mDebugLevel > 0)
   {
      auto out = ut::log::debug() << "WsfCompositeOpticalSignature::Surface::UpdateLocation Report:";
      out.AddNote() << "Surface Location: " << mLocationECS;
      out.AddNote() << "Next Surface Starts At: " << aDefaultLocationECS;
      out.AddNote() << "Aft Radius: " << mAftRadius;
   }
}

// =================================================================================================
//! Update the projected area of this surface to include blockage by the plume.
//!
//! If the viewing angle is from the rear hemisphere then the plume blocks some of the surface before.
//! Typically a plume surface is preceded by a cylinder surface. If the plume is active then the rear
//! surface of the cylinder should not be included in the area. If the viewing angle is from the rear
//! then the projected area will be updated to exclude the plume area.
//!
//! Note that the plume may be of bigger or smaller radius than the rear of this surface.
//!
//! @param aArea current projected area from this surface.
//! @param aCosTheta The cosine of the viewing angle.
//! @returns The updated area.
double WsfCompositeOpticalSignature::Surface::UpdatePlumeBlockage(double aArea, double aCosTheta) const
{
   double area = aArea;
   if (aCosTheta < 0.0)
   {
      double blockedRadius = std::min(mAftRadius, mPlumeRadius);
      double blockedArea   = UtMath::cPI * blockedRadius * blockedRadius * fabs(aCosTheta);
      area -= blockedArea;
      area = std::max(0.0, area);
   }
   return area;
}

// =================================================================================================
// Begin anonymous namespace for specific surface implementations.
// =================================================================================================

// Note that many of the surface definitions also include an alternative method to compute projected
// area and a 'test' command to print a table of projected area for various viewing angles using
// the actual method and the numerical integration.

namespace
{

// A helper method used to determine the projected area of a facet.
// Used by the test drivers...
double FacetProjectedArea(double         aArea,   // m^2
                          const UtVec3d& aUnit_n, // Unit vector normal to the area
                          const UtVec3d& aUnit_v) // Unit vector from the surface to the viewer
{
   double area_p = 0.0;
   double cos_nv = aUnit_n.DotProduct(aUnit_v); // Cosine of angle between surface normal and viewer
   if (cos_nv > 0.0)                            // Surface is visible
   {
      area_p = aArea * cos_nv;
   }
   return area_p;
}

// Compute the projected area of a sphere (or part of a sphere)
// The equatorial plane is the XY plane. The angle phi is the angle from the +X-axis [-pi..pi].
// The angle theta is the angle is from the +Z axis [0..pi].
double SphericalProjectedArea(double         aRadius, // m
                              double         aPhiBegDeg,
                              double         aPhiEndDeg,
                              const UtVec3d& aUnit_v) // Unit vector from the surface to the viewer
{
   const double cANGLE_STEP = 0.1;

   double area     = 0.0;
   double thetaDeg = 0.0;
   while (thetaDeg < 180.0)
   {
      // Vertical midpoint angle of the facet
      double theta       = (thetaDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
      double facetHeight = aRadius * (cANGLE_STEP * UtMath::cRAD_PER_DEG); // height of the facet
      double theta1      = thetaDeg * UtMath::cRAD_PER_DEG;
      double theta2      = (thetaDeg + cANGLE_STEP) * UtMath::cRAD_PER_DEG;
      double width1      = aRadius * (cANGLE_STEP * UtMath::cRAD_PER_DEG) * sin(theta1); // width of the facet
      double width2      = aRadius * (cANGLE_STEP * UtMath::cRAD_PER_DEG) * sin(theta2); // width of the facet
      double facetWidth  = 0.5 * (width1 + width2);
      double facetArea   = facetHeight * facetWidth;
      double phiDeg      = aPhiBegDeg;
      while (phiDeg < aPhiEndDeg)
      {
         // Horizontal midpoint of the facet
         double  phi = (phiDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
         double  z   = cos(theta);
         double  xy  = sin(theta);
         double  x   = xy * cos(phi);
         double  y   = xy * sin(phi);
         UtVec3d unit_n(x, y, z); // Normal to the facet
         area += FacetProjectedArea(facetArea, unit_n, aUnit_v);
         phiDeg += cANGLE_STEP;
      }
      thetaDeg += cANGLE_STEP;
   }
   return area;
}

// =================================================================================================
// Base orientable surface definition.
// This is used as a base class for non-spherical surfaces. For the most part this is a reduced
// implementation of WsfArticulatedPart/UtEntityPart.
// =================================================================================================

class Orientable : public WsfCompositeOpticalSignature::Surface
{
public:
   explicit Orientable(const Surface& aDefault)
      : Surface(aDefault)
      , mYaw(0.0)
      , mPitch(0.0)
      , mRoll(0.0)
      , mIdentityTransform(true)
   {
      UpdateECS_ToSCS_Transform();
   }
   Orientable(const Orientable& aSrc) = default;

   Surface* Clone() const override { return new Orientable(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   void     UpdateGeometry(const UG_Input& aInput) override;
   void     UpdateECS_ToSCS_Transform();

   double mYaw;
   double mPitch;
   double mRoll;
   bool   mIdentityTransform;
};

bool Orientable::InitializeType(UtVec3d& aDefaultLocationECS)
{
   return Surface::InitializeType(aDefaultLocationECS);
}

bool Orientable::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "yaw")
   {
      aInput.ReadValueOfType(mYaw, UtInput::cANGLE);
      aInput.ValueInClosedRange(mYaw, -UtMath::cPI, UtMath::cPI);
      UpdateECS_ToSCS_Transform();
   }
   else if (command == "pitch")
   {
      aInput.ReadValueOfType(mPitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(mPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      UpdateECS_ToSCS_Transform();
   }
   else if (command == "roll")
   {
      aInput.ReadValueOfType(mRoll, UtInput::cANGLE);
      aInput.ValueInClosedRange(mRoll, -UtMath::cPI, UtMath::cPI);
      UpdateECS_ToSCS_Transform();
   }
   else
   {
      myCommand = Surface::ProcessInput(aInput);
   }
   return myCommand;
}

void Orientable::UpdateGeometry(const UG_Input& aInput)
{
   if (mIdentityTransform)
   {
      // Yaw, pitch, roll not specified - so just use the WCS->ECS transform
      mWCS_ToSCS_Transform = aInput.mWCS_ToECS_Transform;
   }
   else
   {
      // Yaw, pitch or roll specified. [WCS->SCS] = [ECS->SCS] x [WCS->ECS]
      UtMat3d::Multiply(mWCS_ToSCS_Transform.mMat, mECS_ToSCS_Transform.mMat, aInput.mWCS_ToECS_Transform.mMat);
   }

   if (mDebugLevel > 2)
   {
      auto out = ut::log::info() << "Surface Transform:" << std::fixed << std::setprecision(8);
      out.AddNote() << "ECS->SCS X: " << UtVec3d(mECS_ToSCS_Transform.mMat[0]);
      out.AddNote() << "ECS->SCS Y: " << UtVec3d(mECS_ToSCS_Transform.mMat[1]);
      out.AddNote() << "ECS->SCS Z: " << UtVec3d(mECS_ToSCS_Transform.mMat[2]);
      out.AddNote() << "WCS->SCS X: " << UtVec3d(mWCS_ToSCS_Transform.mMat[0]);
      out.AddNote() << "WCS->SCS Y: " << UtVec3d(mWCS_ToSCS_Transform.mMat[1]);
      out.AddNote() << "WCS->SCS Z: " << UtVec3d(mWCS_ToSCS_Transform.mMat[2]);
   }
}

// This is called only during construction and input.
// The ECS->SCS transform is static once input processing is complete.
void Orientable::UpdateECS_ToSCS_Transform()
{
   if ((mYaw != 0.0) || (mPitch != 0.0) || (mRoll != 0.0))
   {
      UtEntity::ComputeRotationalTransform(mYaw, mPitch, mRoll, mECS_ToSCS_Transform.mMat);
      mIdentityTransform = false;
   }
   else
   {
      mECS_ToSCS_Transform.Identity();
      mIdentityTransform = true;
   }
}

// =================================================================================================
// 'box' surface definition
// =================================================================================================

class Box : public Orientable
{
public:
   explicit Box(const Surface& aDefaults)
      : Orientable(aDefaults)
      , mSize(0.0, 0.0, 0.0)
      , mFaceArea(0.0, 0.0, 0.0)
      , mSharedFaceArea{0.0}
   {
   }

   Box(const Box& aSrc) = default;
   Surface* Clone() const override { return new Box(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   double   AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override;
   void     CheckAdjacency(const Surface& aOther) override;

   UtVec3d mSize;
   UtVec3d mFaceArea;
   double  mSharedFaceArea[6];
};

// =================================================================================================
bool Box::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Orientable::InitializeType(aDefaultLocationECS);
   if ((mSize[0] == 0.0) || (mSize[1] == 0.0) || (mSize[2] == 0.0))
   {
      ut::log::error() << "'size' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  mSize,                   // Size
                  UtVec3d(0.0, 0.0, 0.0)); // Offset
   return ok;
}

// =================================================================================================
bool Box::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "size")
   {
      aInput.ReadValueOfType(mSize[0], UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSize[0], 0.0);
      aInput.ReadValueOfType(mSize[1], UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSize[1], 0.0);
      aInput.ReadValueOfType(mSize[2], UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSize[2], 0.0);
      // Make sure that a maximum of one dimension is zero.
      bool foundZero = false;
      for (size_t i = 0; i < 3; ++i)
      {
         if (mSize[i] == 0.0)
         {
            if (foundZero)
            {
               throw UtInput::BadValue(aInput, "only one 'size' value can be zero");
            }
            foundZero = true;
         }
      }
      // Update the area of each face
      mFaceArea[0] = mSize[1] * mSize[2]; // Constant X (Y-Z) plane
      mFaceArea[1] = mSize[2] * mSize[0]; // Constant Y (Z-X) plane
      mFaceArea[2] = mSize[0] * mSize[1]; // Constant Z (X-Y) plane
   }
   else
   {
      myCommand = Orientable::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Box::ProjectedArea(const UtVec3d& aUnit_v) const
{
   // Note that while the box has six sides, we only do 3 sides. For a given opposing pair of faces one will be facing
   // the observer and the other will face away.
   double  area = 0.0;
   UtVec3d unit_n_SCS(0.0, 0.0, 0.0);
   UtVec3d unit_n; // normal to the current face..
   for (size_t axis = 0; axis < 3; ++axis)
   {
      unit_n_SCS[axis] = 1.0; // Form the normal vector for the face
      unit_n           = mWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
      unit_n_SCS[axis] = 0.0; // Reset for next pass
      double cos_nv    = unit_n.DotProduct(aUnit_v);
      if (cos_nv > 0.0)
      {
         // +X, +Y or +Z face
         area += (mFaceArea[axis] - mSharedFaceArea[axis]) * cos_nv;
      }
      else if (cos_nv < 0.0)
      {
         // -X, -Y or -Z face
         area += (mFaceArea[axis] - mSharedFaceArea[axis + 3]) * fabs(cos_nv);
      }
   }
   // TODO We don't yet consider the plume attached to this shape
   return area;
}

// =================================================================================================
double Box::AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const
{
   // Check to see if any of my faces are adjacent to the specified face.
   UtVec3d myFaceLocPCS(0.0, 0.0, 0.0);
   UtVec3d myFaceNormalPCS(0.0, 0.0, 0.0);
   for (size_t axis = 0; axis < 3; ++axis)
   {
      myFaceLocPCS[axis]    = 0.5 * mSize[axis];
      myFaceNormalPCS[axis] = 1.0;
      if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, myFaceLocPCS, myFaceNormalPCS))
      {
         return mFaceArea[axis];
      }
      myFaceLocPCS[axis]    = -0.5 * mSize[axis];
      myFaceNormalPCS[axis] = -1.0;
      if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, myFaceLocPCS, myFaceNormalPCS))
      {
         return mFaceArea[axis];
      }
      myFaceLocPCS[axis]    = 0.0;
      myFaceNormalPCS[axis] = 0.0;
   }
   return 0.0;
}

// =================================================================================================
void Box::CheckAdjacency(const Surface& aOther)
{
   // Check to see if any of my faces area adjacent to the other surface
   UtVec3d myFaceLocPCS(0.0, 0.0, 0.0);
   UtVec3d myFaceNormalPCS(0.0, 0.0, 0.0);
   for (size_t axis = 0; axis < 3; ++axis)
   {
      myFaceLocPCS[axis]    = 0.5 * mSize[axis];
      myFaceNormalPCS[axis] = 1.0;
      if (UpdateSharedFaceArea(aOther, myFaceLocPCS, myFaceNormalPCS, mFaceArea[axis], mSharedFaceArea[axis]))
      {
         return;
      }
      myFaceLocPCS[axis]    = -0.5 * mSize[axis];
      myFaceNormalPCS[axis] = -1.0;
      if (UpdateSharedFaceArea(aOther, myFaceLocPCS, myFaceNormalPCS, mFaceArea[axis], mSharedFaceArea[axis + 3]))
      {
         return;
      }
      myFaceLocPCS[axis]    = 0.0;
      myFaceNormalPCS[axis] = 0.0;
   }
}

// =================================================================================================
// 'cone' surface definition
// =================================================================================================

class Cone : public Orientable
{
public:
   explicit Cone(const Surface& aDefaults)
      : Orientable(aDefaults)
      , mLength(0.0)
      , mRadius(0.0)
      , mSharedMinusX_Area(0.0)
   {
   }
   Cone(const Cone& aSrc) = default;
   Surface* Clone() const override { return new Cone(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   double   AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override;
   void     CheckAdjacency(const Surface& aOther) override;

   double ExcessArea(double aCosTheta) const;

   void   Test();
   double IntegratedProjectedArea(const UtVec3d& aUnit_v) const;

   double mLength;
   double mRadius;
   double mSharedMinusX_Area; // Base area that is shared with another face.
};

// =================================================================================================
bool Cone::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Orientable::InitializeType(aDefaultLocationECS);
   if (mLength == 0.0)
   {
      ut::log::error() << "'length' must be provided.";
      ok = false;
   }
   if (mRadius == 0.0)
   {
      ut::log::error() << "'radius' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  UtVec3d(mLength, 2.0 * mRadius, 2.0 * mRadius), // Size
                  UtVec3d(-0.5 * mLength, 0.0, 0.0));             // Offset
   return ok;
}

// =================================================================================================
bool Cone::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "length")
   {
      aInput.ReadValueOfType(mLength, UtInput::cLENGTH);
      aInput.ValueGreater(mLength, 0.0);
   }
   else if (command == "radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mRadius, 0.0);
   }
   else if (command == "test")
   {
      Test();
   }
   else
   {
      myCommand = Orientable::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Cone::ProjectedArea(const UtVec3d& aUnit_v) const
{
   // The axis of the cone is aligned with the SCS X axis
   UtVec3d unit_x_SCS(1.0, 0.0, 0.0);
   UtVec3d unit_x   = mWCS_ToSCS_Transform.InverseMultiply(unit_x_SCS);
   double  cosTheta = unit_x.DotProduct(aUnit_v);
   double  area     = 0.0;
   if (cosTheta >= 0.0)
   {
      // Viewing from front hemisphere
      area = UtMath::cPI * mRadius * mRadius * cosTheta; // Area of the projected base
      area += ExcessArea(cosTheta);
   }
   else
   {
      // Viewing from rear hemisphere
      area = ExcessArea(cosTheta);
   }

   // Add in the projected area of the base
   if (cosTheta < 0.0)
   {
      area -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
   }
   area = UpdatePlumeBlockage(area, cosTheta); // Account for blockage by plume...
   return area;
}

// =================================================================================================
double Cone::AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const
{
   double adjacentFaceArea = 0.0;
   if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, UtVec3d(0.0, 0.0, 0.0), UtVec3d(-1.0, 0.0, 0.0)))
   {
      adjacentFaceArea = UtMath::cPI * mRadius * mRadius;
   }
   return adjacentFaceArea;
}

// =================================================================================================
void Cone::CheckAdjacency(const Surface& aOther)
{
   UpdateSharedFaceArea(aOther,
                        UtVec3d(0.0, 0.0, 0.0),
                        UtVec3d(-1.0, 0.0, 0.0),
                        UtMath::cPI * mRadius * mRadius,
                        mSharedMinusX_Area);
}

// =================================================================================================
// Compute projected area of the cone outside of the projected area of the base.
double Cone::ExcessArea(double aCosTheta) const
{
   double cosTheta = UtMath::Limit(aCosTheta, 1.0); // aCosTheta may be slightly outside [-1..1]
   cosTheta        = fabs(cosTheta);                // May be in the rear hemisphere....
   double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

   // We treat the 2D projection plane as a conventional X-Y plane.
   // The projected cone base is an ellipse that is centered about the origin...
   //
   // Note that is our case 'a' is less than 'b', so the length of the major axis is less than the minor axis.

   double a = mRadius * cosTheta; // Major (X) axis of the projected cone base
   double b = mRadius;            // Minor (Y) axis of the projected cone base
   double c = mLength * sinTheta; // X coordinate of the cone tip in the projection plane.

   double a_excess = 0.0;
   if (c > a)
   {
      // Projected cone tip is outside the projected base. Compute the area of the cone outside
      // the projected base...

      // First determine the angle (phi) to the point on the projected ellipse such that a line drawn from
      // the projected cone point to that point is on the ellipse tangent to the ellipse.
      double cosPhi = (mRadius * cosTheta) / (mLength * sinTheta);
      double sinPhi = sqrt(1.0 - cosPhi * cosPhi);

      // Area of the triangle formed by the origin, projected cone point and the tangent point
      double a_triangle = 0.5 * (mLength * sinTheta) * (mRadius * sinPhi); // 1/2 * base * height

      // Need to derive the area of the elliptical sector between the X axis and the tangent point.
      // Map the ellipse to a circle through a change of coordinates. The radius of the mapped circle
      // is 'b', which for us is the same as the input radius.
      // double x_t = a * cosPhi;
      // double y_t = b * sinPhi;
      // double x_tp = x_t * b / a;    // = a * cosPhi * b / a = b * cosPhi;
      // double y_tp = y_t;            // = b * sinPhi;
      // tan phi' = y_t' / x_t' = (b * sinPhi) / (b * cosPhi) = sinPhi / cosPhi;
      // double tanPhi_p = sinPhi / cosPhi;
      // double phi_p = atan2(sinPhi, cosPhi);
      // double a_sector = UtMath::cPI * mRadius * mRadius * (phi_p / UtMath::cTWO_PI);

      double phi_p    = atan2(sinPhi, cosPhi);
      double a_sector = 0.5 * phi_p * mRadius * mRadius;
      // The area above is in scaled area (x' = (a/b) * a). Convert to unscaled area...
      a_sector *= a / b; // 1 / cosTheta

      a_excess = 2.0 * (a_triangle - a_sector);
   }
   return a_excess;
}

// =================================================================================================
// The cone is assumed to be pointing along the +X axis
double Cone::IntegratedProjectedArea(const UtVec3d& aUnit_v) const
{
   const double cANGLE_STEP  = 0.05;
   double       cHEIGHT_STEP = mLength / 1000.0;

   double area     = 0.0;
   double l        = 0.0;
   double cosAngle = mLength / sqrt(mRadius * mRadius + mLength * mLength);
   double sinAngle = mRadius / sqrt(mRadius * mRadius + mLength * mLength);

   while (l < (mLength - (0.1 * cHEIGHT_STEP)))
   {
      double facetHeight = cHEIGHT_STEP / cosAngle;
      double radius1     = mRadius * (l / mLength);
      double radius2     = mRadius * ((l + cHEIGHT_STEP) / mLength);
      double width1      = radius1 * (cANGLE_STEP * UtMath::cRAD_PER_DEG);
      double width2      = radius2 * (cANGLE_STEP * UtMath::cRAD_PER_DEG);
      double facetWidth  = 0.5 * (width1 + width2);
      double facetArea   = facetWidth * facetHeight;
      double phiDeg      = 0.0;
      while (phiDeg < (360.0 - 0.1 * cANGLE_STEP))
      {
         // Angular midpoint of the facet
         double  phi = (phiDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
         double  x   = sinAngle;
         double  yz  = cosAngle;
         double  y   = yz * cos(phi);
         double  z   = yz * sin(phi);
         UtVec3d unit_n(x, y, z); // Normal to the facet
         area += FacetProjectedArea(facetArea, unit_n, aUnit_v);
         phiDeg += cANGLE_STEP;
      }
      l += cHEIGHT_STEP;
   }
   return area;
}

// =================================================================================================
void Cone::Test()
{
   double coneAngle = atan2(mRadius, mLength);
   auto   out       = ut::log::info() << "Cone Test Data Report:";
   out.AddNote() << "Radius: " << mRadius << " m";
   out.AddNote() << "Length: " << mLength << " m";
   out.AddNote() << "Cone Angle: " << coneAngle * UtMath::cDEG_PER_RAD << " deg";
   UtMat3d::Identity(mWCS_ToSCS_Transform.mMat);
   for (int intAngle = 0; intAngle <= 180; intAngle += 5)
   {
      double  angleRad = intAngle * UtMath::cRAD_PER_DEG;
      UtVec3d unit_v(cos(angleRad), sin(angleRad), 0.0);
      double  area = ProjectedArea(unit_v);
      // IntegrateProjectedArea does not include the optional faces that may be included by ProjectArea.
      // These are handled separately after the base call.
      double areaInt  = IntegratedProjectedArea(unit_v);
      double cosTheta = cos(angleRad);
      // Add in the projected area of the base
      if (cosTheta < 0.0)
      {
         areaInt -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
      }
      auto note = out.AddNote() << "Angle Radius: " << std::fixed << std::setprecision(6)
                                << angleRad * UtMath::cDEG_PER_RAD << " deg";
      note.AddNote() << "Projected Area: " << std::setprecision(6) << area << " m^2";
      note.AddNote() << "Integrated Projected Area: " << std::setprecision(6) << areaInt << " m^2";
   }
}

// =================================================================================================
// 'cylinder' surface definition
// =================================================================================================

class Cylinder : public Orientable
{
public:
   explicit Cylinder(const Surface& aDefaults)
      : Orientable(aDefaults)
      , mLength(0.0)
      , mRadius(0.0)
      , mSharedPlusX_Area(0.0)
      , mSharedMinusX_Area(0.0)
   {
   }
   Cylinder(const Cylinder& aSrc) = default;
   Surface* Clone() const override { return new Cylinder(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   double   AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override;
   void     CheckAdjacency(const Surface& aOther) override;

   void   Test();
   double IntegratedProjectedArea(const UtVec3d& aUnit_v) const;

   double mLength;
   double mRadius;
   double mSharedPlusX_Area;
   double mSharedMinusX_Area;
};

// =================================================================================================
bool Cylinder::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Orientable::InitializeType(aDefaultLocationECS);
   if (mLength == 0.0)
   {
      ut::log::error() << "'length' must be provided.";
      ok = false;
   }
   if (mRadius == 0.0)
   {
      ut::log::error() << "'radius' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  UtVec3d(mLength, 2.0 * mRadius, 2.0 * mRadius), // Size
                  UtVec3d(0.0, 0.0, 0.0));                        // Offset
   return ok;
}

// =================================================================================================
bool Cylinder::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "length")
   {
      aInput.ReadValueOfType(mLength, UtInput::cLENGTH);
      aInput.ValueGreater(mLength, 0.0);
   }
   else if (command == "radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mRadius, 0.0);
   }
   else if (command == "test")
   {
      Test();
   }
   else
   {
      myCommand = Orientable::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Cylinder::ProjectedArea(const UtVec3d& aUnit_v) const
{
   // The axis of the cylinder is aligned with the SCS X axis
   UtVec3d unit_x_SCS(1.0, 0.0, 0.0);
   UtVec3d unit_x   = mWCS_ToSCS_Transform.InverseMultiply(unit_x_SCS);
   double  cosTheta = unit_x.DotProduct(aUnit_v);
   cosTheta         = UtMath::Limit(cosTheta, 1.0); // Magnitude could be *slightly* greater than 1
   double sinTheta  = sqrt(1.0 - cosTheta * cosTheta);

   double area = 2.0 * mRadius * mLength * sinTheta;

   // Add the projected area of the end cap
   if (cosTheta > 0.0)
   {
      area += (UtMath::cPI * mRadius * mRadius - mSharedPlusX_Area) * cosTheta;
   }
   else if (cosTheta < 0.0)
   {
      area -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
   }
   area = UpdatePlumeBlockage(area, cosTheta);
   return area;
}

// =================================================================================================
double Cylinder::AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const
{
   double adjacentFaceArea = 0.0;
   if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, UtVec3d(0.5 * mLength, 0.0, 0.0), UtVec3d(1.0, 0.0, 0.0)))
   {
      adjacentFaceArea = UtMath::cPI * mRadius * mRadius;
   }
   else if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, UtVec3d(-0.5 * mLength, 0.0, 0.0), UtVec3d(-1.0, 0.0, 0.0)))
   {
      adjacentFaceArea = UtMath::cPI * mRadius * mRadius;
   }

   return adjacentFaceArea;
}

// =================================================================================================
void Cylinder::CheckAdjacency(const Surface& aOther)
{
   if (UpdateSharedFaceArea(aOther,
                            UtVec3d(0.5 * mLength, 0.0, 0.0),
                            UtVec3d(1.0, 0.0, 0.0),
                            UtMath::cPI * mRadius * mRadius,
                            mSharedPlusX_Area))
   {
      return;
   }
   else if (UpdateSharedFaceArea(aOther,
                                 UtVec3d(-0.5 * mLength, 0.0, 0.0),
                                 UtVec3d(-1.0, 0.0, 0.0),
                                 UtMath::cPI * mRadius * mRadius,
                                 mSharedMinusX_Area))
   {
      return;
   }
}

// =================================================================================================
double Cylinder::IntegratedProjectedArea(const UtVec3d& aUnit_v) const
{
   const double cANGLE_STEP  = 0.05;
   double       cHEIGHT_STEP = mLength;

   double area = 0.0;
   double l    = 0.0;
   while (l < mLength - (0.1 * cHEIGHT_STEP))
   {
      double facetHeight = cHEIGHT_STEP;
      double facetWidth  = mRadius * cANGLE_STEP * UtMath::cRAD_PER_DEG;
      double facetArea   = facetWidth * facetHeight;
      double phiDeg      = 0.0;
      while (phiDeg < 360.0)
      {
         // Angular midpoint of the facet
         double  phi = (phiDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
         double  x   = 0.0;
         double  y   = cos(phi);
         double  z   = sin(phi);
         UtVec3d unit_n(x, y, z); // Normal to the facet
         area += FacetProjectedArea(facetArea, unit_n, aUnit_v);
         phiDeg += cANGLE_STEP;
      }
      l += cHEIGHT_STEP;
   }
   return area;
}

// =================================================================================================
void Cylinder::Test()
{
   auto out = ut::log::info() << "Cylinder Data Report:";
   out.AddNote() << "Radius: " << mRadius << " m";
   out.AddNote() << "Length: " << mLength << " m";
   UtMat3d::Identity(mWCS_ToSCS_Transform.mMat);
   for (int intAngle = 0; intAngle <= 180; intAngle += 5)
   {
      double  angleRad = intAngle * UtMath::cRAD_PER_DEG;
      UtVec3d unit_v(cos(angleRad), sin(angleRad), 0.0);
      double  area = ProjectedArea(unit_v);
      // IntegrateProjectedArea does not include the optional faces that may be included by ProjectArea.
      // These are handled separately after the base call.
      double areaInt  = IntegratedProjectedArea(unit_v);
      double cosTheta = cos(angleRad);
      // Add the projected area of the end caps if requested
      if (cosTheta > 0.0)
      {
         areaInt += (UtMath::cPI * mRadius * mRadius - mSharedPlusX_Area) * cosTheta;
      }
      if (cosTheta < 0.0)
      {
         areaInt -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
      }
      auto note = out.AddNote() << "Angle Radius: " << std::fixed << std::setprecision(6)
                                << angleRad * UtMath::cDEG_PER_RAD << " deg";
      note.AddNote() << "Projected Area: " << std::setprecision(6) << area << " m^2";
      note.AddNote() << "Integrated Projected Area: " << std::setprecision(6) << areaInt << " m^2";
   }
}

// =================================================================================================
// 'hemisphere' surface definition
// =================================================================================================

class Hemisphere : public Orientable
{
public:
   explicit Hemisphere(const Surface& aDefaults)
      : Orientable(aDefaults)
      , mRadius(1.0)
      , mSharedMinusX_Area(0.0)
   {
   }
   Hemisphere(const Hemisphere& aSrc) = default;
   Surface* Clone() const override { return new Hemisphere(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   double   AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override;
   void     CheckAdjacency(const Surface& aOther) override;

   void Test();

   double mRadius;
   double mSharedMinusX_Area;
};

// =================================================================================================
bool Hemisphere::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Surface::InitializeType(aDefaultLocationECS);
   if (mRadius == 0.0)
   {
      ut::log::error() << "'radius' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  UtVec3d(mRadius, 2.0 * mRadius, 2.0 * mRadius), // Size
                  UtVec3d(-0.5 * mRadius, 0.0, 0.0));             // Offset
   return ok;
}

// =================================================================================================
bool Hemisphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mRadius, 0.0);
   }
   else if (command == "test")
   {
      Test();
   }
   else
   {
      myCommand = Orientable::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Hemisphere::ProjectedArea(const UtVec3d& aUnit_v) const
{
   // The normal of the 'surface' is the axis of rotation of the hemisphere which is aligned with the SCS X axis
   UtVec3d unit_n_SCS(1.0, 0.0, 0.0);
   UtVec3d unit_n   = mWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
   double  cosTheta = unit_n.DotProduct(aUnit_v);
   cosTheta         = UtMath::Limit(cosTheta, 1.0); // Magnitude could be *slightly* greater than 1

   // This is analogous the phases of the moon. Theta has the follow meanings...
   //
   // Theta:      0     Full moon
   //                   Waning gibbous
   //            90     Quarter
   //                   Waning crescent
   //           180     New moon

   double area = UtMath::cPI_OVER_2 * mRadius * mRadius * (1.0 + cosTheta);

   // Add the projected area of the base
   if (cosTheta < 0.0)
   {
      area -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
   }
   area = UpdatePlumeBlockage(area, cosTheta); // Account for blockage by plume...
   return area;
}

// =================================================================================================
double Hemisphere::AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const
{
   double adjacentFaceArea = 0.0;
   if (AreAdjacent(aOtherFaceLocECS, aOtherFaceNormalECS, UtVec3d(0.0, 0.0, 0.0), UtVec3d(-1.0, 0.0, 0.0)))
   {
      adjacentFaceArea = UtMath::cPI * mRadius * mRadius;
   }
   return adjacentFaceArea;
}

// =================================================================================================
void Hemisphere::CheckAdjacency(const Surface& aOther)
{
   UpdateSharedFaceArea(aOther,
                        UtVec3d(0.0, 0.0, 0.0),
                        UtVec3d(-1.0, 0.0, 0.0),
                        UtMath::cPI * mRadius * mRadius,
                        mSharedMinusX_Area);
}

// =================================================================================================
void Hemisphere::Test()
{
   auto out = ut::log::info() << "Hemisphere Data Report:";
   out.AddNote() << "Radius: " << mRadius << " m";
   UtMat3d::Identity(mWCS_ToSCS_Transform.mMat);
   for (int intAngle = 0; intAngle <= 180; intAngle += 5)
   {
      double  angleRad = intAngle * UtMath::cRAD_PER_DEG;
      UtVec3d unit_v(cos(angleRad), sin(angleRad), 0.0);
      double  area = ProjectedArea(unit_v);
      // SphericalProjectArea does not include the optional faces that may be included by ProjectArea.
      // These are handled separately after the base call.
      double areaInt  = SphericalProjectedArea(mRadius, -90.0, 90.0, unit_v);
      double cosTheta = cos(angleRad);
      // Add in the projected area of the base
      if (cosTheta < 0.0)
      {
         areaInt -= (UtMath::cPI * mRadius * mRadius - mSharedMinusX_Area) * cosTheta; // '-=' used because cosTheta < 0.0
      }
      auto note = out.AddNote() << "Angle Radius: " << std::fixed << std::setprecision(6)
                                << angleRad * UtMath::cDEG_PER_RAD << " deg";
      note.AddNote() << "Projected Area: " << std::setprecision(6) << area << " m^2";
      note.AddNote() << "Integrated Projected Area: " << std::setprecision(6) << areaInt << " m^2";
   }
}

// =================================================================================================
// 'plume' surface definition
// =================================================================================================

class Plume : public WsfCompositeOpticalSignature::Surface
{
public:
   explicit Plume(const Surface& aDefaults)
      : Surface(aDefaults)
      , mLength(0.0)
      , mRadius(0.0)
      , mCurLength(0.0)
      , mCurRadius(0.0)
      , mBlockedArea(0.0)
      , mSizeSpecified(false)
   {
   }
   Plume(const Plume& aSrc) = default;
   Surface* Clone() const override { return new Plume(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   // Adjacency with the plume is not considered. It is handled by special case because it is dynamic.
   double AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override
   {
      return 0.0;
   }
   void CheckAdjacency(const Surface& aOther) override {}
   void UpdateGeometry(const UG_Input& aInput) override;
   void UpdateTemperature(const UT_Input& aInput) override;

   double mLength;
   double mRadius;
   double mCurLength;
   double mCurRadius;
   double mBlockedArea;
   bool   mSizeSpecified;
};

// =================================================================================================
bool Plume::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Surface::InitializeType(aDefaultLocationECS);

   // TODO-must provide length/radius for now... no auto-sizing yet...
   if (mLength == 0.0)
   {
      ut::log::error() << "'length' must be provided.";
      ok = false;
   }
   if (mRadius == 0.0)
   {
      ut::log::error() << "'radius' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  UtVec3d(mLength, 2.0 * mRadius, 2.0 * mRadius), // Size
                  UtVec3d(0.5 * mLength, 0.0, 0.0));              // Offset
   return ok;
}

// =================================================================================================
bool Plume::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "length")
   {
      aInput.ReadValueOfType(mLength, UtInput::cLENGTH);
      aInput.ValueGreater(mLength, 0.0);
   }
   else if (command == "radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mRadius, 0.0);
   }
   else if (command == "location")
   {
      // disable the location command in the base class
      throw UtInput::UnknownCommand(aInput);
   }
   else
   {
      myCommand = Surface::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Plume::ProjectedArea(const UtVec3d& aUnit_v) const
{
   UtVec3d unit_x_SCS(1.0, 0.0, 0.0);
   UtVec3d unit_x   = mWCS_ToSCS_Transform.InverseMultiply(unit_x_SCS);
   double  cosTheta = unit_x.DotProduct(aUnit_v);
   cosTheta         = UtMath::Limit(cosTheta, 1.0); // Magnitude could be *slightly* greater than 1
   double sinTheta  = sqrt(1.0 - cosTheta * cosTheta);

   double area = 2.0 * mCurRadius * mCurLength * sinTheta; // Projected area of the plume main body
   if (cosTheta >= 0.0)
   {
      // Viewed from the front hemisphere... Some or all of the front cap may be obscured by the previous surface...
      double capArea = (UtMath::cPI * mCurRadius * mCurRadius - mBlockedArea) * cosTheta;
      capArea        = std::max(capArea, 0.0);
      area += capArea;
   }
   else
   {
      // Viewed from the rear hemisphere. Add in the end cap of the plume cylinder.
      area -= UtMath::cPI * mCurRadius * mCurRadius * cosTheta; // '-=' used because cosTheta < 0.0
   }
   return area;
}

// =================================================================================================
void Plume::UpdateGeometry(const UG_Input& aInput)
{
   Surface::UpdateGeometry(aInput);

   // If no thrust is currently being produced then set the length and radius to zero.
   // The thrust detection logic only works for the guided mover...

   double curThrust = 10000.0;
   if (aInput.mCurrentThrust >= 0.0) // Use thrust from WsfGuidedMover....
   {
      curThrust = aInput.mCurrentThrust;
   }
   if (curThrust == 0.0)
   {
      mCurLength = 0.0;
      mCurRadius = 0.0;
   }
   else
   {
      mCurLength = mLength;
      mCurRadius = mRadius;
      if ((mLength != 0.0) && (mRadius != 0.0))
      {
         // TODO Determine the plume size based on thrust...
      }
   }
   // Tell the surface to which I am attached the radius of the plume...
   // ... and get the area of the aft face from that surface...
   mBlockedArea = 0.0;
   if (aInput.mPrevSurfacePtr != nullptr)
   {
      aInput.mPrevSurfacePtr->UpdatePlumeRadius(mCurRadius);
      double r     = aInput.mPrevSurfacePtr->mAftRadius;
      mBlockedArea = UtMath::cPI * r * r;
   }
}

// =================================================================================================
void Plume::UpdateTemperature(const UT_Input& aInput)
{
   // TODO Automatically determine plume temperature. For now just use base class..
   mTemperature = aInput.mAmbientTemperature; //  Assume no plume (engine off)
   if (mCurRadius > 0.0)
   {
      Surface::UpdateTemperature(aInput);
   }
}

// =================================================================================================
// 'sphere' surface definition
// =================================================================================================

class Sphere : public WsfCompositeOpticalSignature::Surface
{
public:
   explicit Sphere(const Surface& aDefaults)
      : Surface(aDefaults)
      , mRadius(1.0)
   {
   }
   Sphere(const Sphere& aSrc) = default;
   Surface* Clone() const override { return new Sphere(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   // No adjacency possible.
   double AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const override
   {
      return 0.0;
   }
   void CheckAdjacency(const Surface& aOther) override {}

   double mRadius;
};

// =================================================================================================
bool Sphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mRadius, 0.0);
   }
   else
   {
      myCommand = Surface::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool Sphere::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Surface::InitializeType(aDefaultLocationECS);
   if (mRadius == 0.0)
   {
      ut::log::error() << "'radius' must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS,
                  UtVec3d(2.0 * mRadius, 2.0 * mRadius, 2.0 * mRadius), // Size
                  UtVec3d(0.0, 0.0, 0.0));                              // Offset
   return ok;
}

// =================================================================================================
double Sphere::ProjectedArea(const UtVec3d& aUnit_v) const
{
   // TODO we don't consider plume blockage here
   return UtMath::cPI * mRadius * mRadius;
}

// =================================================================================================
// 'tabular' surface definition
// =================================================================================================

class Tabular : public WsfCompositeOpticalSignature::Surface
{
public:
   explicit Tabular(const Surface& aDefaults)
      : Surface(aDefaults)
      , mAreaPtr(nullptr)
   {
   }
   Tabular(const Tabular& aSrc) = default;
   Surface* Clone() const override { return new Tabular(*this); }
   bool     InitializeType(UtVec3d& aDefaultLocationECS) override;
   bool     ProcessInput(UtInput& aInput) override;
   double   ProjectedArea(const UtVec3d& aUnit_v) const override;
   // No adjacency checked...
   double AdjacentArea(const UtVec3d& aOtherFaceLoc, const UtVec3d& aOtherFaceNormalECS) const override { return 0.0; }
   void   CheckAdjacency(const Surface& aOther) override {}

   std::shared_ptr<UtAzElTable> mAreaPtr;
};

// =================================================================================================
bool Tabular::InitializeType(UtVec3d& aDefaultLocationECS)
{
   bool ok = Surface::InitializeType(aDefaultLocationECS);
   if (mAreaPtr == nullptr)
   {
      ut::log::error() << "'projected_area' definition must be provided.";
      ok = false;
   }
   UpdateLocation(aDefaultLocationECS, UtVec3d(0.0, 0.0, 0.0), UtVec3d(0.0, 0.0, 0.0));
   return ok;
}

// =================================================================================================
bool Tabular::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "projected_area")
   {
      aInput.ReadCommand(command); // swallow the 'projected_area' string...
      UtAzElTable* tablePtr = nullptr;
      std::string  tableUnits("m^2");
      myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
      if (myCommand && tablePtr != nullptr)
      {
         UtStringUtil::ToLower(tableUnits);
         if (tableUnits == "dbsm")
         {
            // Input values are in dBsm
            tablePtr->ConvertValuesFromDB(1.0F);
         }
         else
         {
            // Input values were dimensional
            try
            {
               double multiplier = aInput.ConvertValue(1.0, tableUnits, UtInput::cAREA);
               tablePtr->MultiplyValues(static_cast<float>(multiplier));
            }
            catch (...)
            {
               delete tablePtr;
               throw;
            }
         }
         mAreaPtr.reset(tablePtr);
      }
   }
   else if (command == "location")
   {
      // Disallow 'location' input in the base class.
      throw UtInput::UnknownCommand(aInput);
   }
   else
   {
      myCommand = Surface::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double Tabular::ProjectedArea(const UtVec3d& aUnit_v) const
{
   double area = 0.0;
   if (mAreaPtr != nullptr)
   {
      // Transform the WCS unit vector pointing to the viewer to the SCS frame.
      UtVec3d unit_v_SCS = mWCS_ToSCS_Transform.Multiply(aUnit_v);
      // Compute the azimuth and elevation and lookup the projected area.
      double az;
      double el;
      UtEntity::ComputeAzimuthAndElevation(unit_v_SCS.GetData(), az, el);
      UtAzElLookup lookup;
      area = lookup.Lookup(az, el);
   }
   // Plume blockage is not considered here. This *should* be a standalone surface....
   return area;
}

} // end namespace

// =================================================================================================
// End anonymous namespace for specific surface implementations.
// =================================================================================================

// =================================================================================================
//! Factory for surfaces.
//! @param aType The type of surface to be created.
//! @param aDefaultData The default surface data (reflectance, temperature, etc.)
//! @return Pointer to the new instance or nullptr if the type was not valid.
// This must appear after the above anonymous namespace that defines the surfaces
// static
WsfCompositeOpticalSignature::Surface* WsfCompositeOpticalSignature::SurfaceFactory(const std::string& aType,
                                                                                    const Surface&     aDefaultData)
{
   Surface* surfacePtr = nullptr;
   if (aType == "box")
   {
      surfacePtr = new Box(aDefaultData);
   }
   else if (aType == "cone")
   {
      surfacePtr = new Cone(aDefaultData);
   }
   else if (aType == "cylinder")
   {
      surfacePtr = new Cylinder(aDefaultData);
   }
   else if (aType == "hemisphere")
   {
      surfacePtr = new Hemisphere(aDefaultData);
   }
   else if (aType == "plume")
   {
      surfacePtr = new Plume(aDefaultData);
   }
   else if (aType == "sphere")
   {
      surfacePtr = new Sphere(aDefaultData);
   }
   else if (aType == "tabular")
   {
      surfacePtr = new Tabular(aDefaultData);
   }
   return surfacePtr;
}
