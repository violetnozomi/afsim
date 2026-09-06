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

#include "WsfSpaceOpticalSignature.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "UtBlackBody.hpp"
#include "UtCalendar.hpp"
#include "UtEclipse.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "WsfDateTime.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfOpticalEnvironment.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace
{

// The following three variables are from the energy balance diagram in the design white paper. They are used in

// The approximate reflected flux density from Sun
const double cAVG_EARTH_REFLECTED_FLUX_DENSITY = 102.0; // W/m^2

// The approximate emitted flux density of the Earth
const double cAVG_EARTH_EMITTED_FLUX_DENSITY = 239.0; // W/m^2

// The approximate total flux density of the Earth (reflected + emitted)
const double cAVG_EARTH_TOTAL_FLUX_DENSITY = 341.0; // W/m^2

double GetAngleBetween(const UtVec3d& aUnit1, const UtVec3d& aUnit2)
{
   double cosTheta = UtVec3d::DotProduct(aUnit1, aUnit2);
   cosTheta        = std::min(std::max(cosTheta, -1.0), 1.0);
   return acos(cosTheta);
}
} // namespace

namespace
{
// The BRDF for a Lambertian sphere, for every one degree of difference between the illumination and the viewer.
//
// This was accomplished using the commented out routine at the end of this file. It models a Lambertian sphere
// as a collection of small Lambertian surfaces.
//
// It should be notes that for a simple Lambertian surface the BRDF is 1/pi. For a 'head-on' sphere it was found
// to be (2/3)*(1/pi). For an off-axis viewer the result is the following, but I did find that it could be approximated
// by (2/3)*(1/pi)*P*P where P is the 'phase' defined as (1+cos(PA))/2 and PA is the angle between the illuminator
// and the viewer. The approximation was very good at the ends, but 10% or so off in the middle. Hence, this table.

const double cSPHERICAL_BRDF[182] = {
   // ====== BEGIN GENERATED DATA
   0.212207,
   0.212174,
   0.212078,
   0.211919,
   0.211698,
   0.211414,
   0.211070,
   0.210666,
   0.210203,
   0.209682, // 0-9 deg
   0.209103,
   0.208467,
   0.207776,
   0.207030,
   0.206230,
   0.205378,
   0.204474,
   0.203518,
   0.202513,
   0.201458, // 10-19 deg
   0.200356,
   0.199207,
   0.198012,
   0.196772,
   0.195488,
   0.194161,
   0.192792,
   0.191383,
   0.189934,
   0.188447, // 20-29 deg
   0.186922,
   0.185361,
   0.183765,
   0.182134,
   0.180471,
   0.178775,
   0.177048,
   0.175292,
   0.173507,
   0.171695, // 30-39 deg
   0.169856,
   0.167992,
   0.166104,
   0.164193,
   0.162259,
   0.160305,
   0.158331,
   0.156339,
   0.154329,
   0.152303, // 40-49 deg
   0.150261,
   0.148205,
   0.146136,
   0.144054,
   0.141962,
   0.139860,
   0.137749,
   0.135630,
   0.133504,
   0.131373, // 50-59 deg
   0.129236,
   0.127096,
   0.124954,
   0.122809,
   0.120664,
   0.118519,
   0.116375,
   0.114234,
   0.112095,
   0.109960, // 60-69 deg
   0.107831,
   0.105707,
   0.103590,
   0.101480,
   0.099379,
   0.097288,
   0.095206,
   0.093135,
   0.091076,
   0.089030, // 70-79 deg
   0.086996,
   0.084977,
   0.082973,
   0.080984,
   0.079011,
   0.077055,
   0.075117,
   0.073196,
   0.071295,
   0.069413, // 80-89 deg
   0.067551,
   0.065709,
   0.063889,
   0.062090,
   0.060314,
   0.058560,
   0.056829,
   0.055122,
   0.053439,
   0.051781, // 90-99 deg
   0.050147,
   0.048539,
   0.046956,
   0.045399,
   0.043869,
   0.042365,
   0.040887,
   0.039437,
   0.038015,
   0.036619, // 100-109 deg
   0.035252,
   0.033912,
   0.032601,
   0.031318,
   0.030063,
   0.028837,
   0.027639,
   0.026469,
   0.025329,
   0.024217, // 110-119 deg
   0.023133,
   0.022078,
   0.021052,
   0.020054,
   0.019084,
   0.018143,
   0.017230,
   0.016345,
   0.015488,
   0.014659, // 120-129 deg
   0.013857,
   0.013083,
   0.012335,
   0.011614,
   0.010920,
   0.010253,
   0.009611,
   0.008994,
   0.008404,
   0.007838, // 130-139 deg
   0.007296,
   0.006779,
   0.006286,
   0.005816,
   0.005370,
   0.004945,
   0.004543,
   0.004163,
   0.003803,
   0.003465, // 140-149 deg
   0.003146,
   0.002847,
   0.002567,
   0.002306,
   0.002062,
   0.001837,
   0.001627,
   0.001434,
   0.001257,
   0.001095, // 150-159 deg
   0.000947,
   0.000813,
   0.000692,
   0.000584,
   0.000487,
   0.000402,
   0.000327,
   0.000262,
   0.000207,
   0.000159, // 160-169 deg
   0.000120,
   0.000088,
   0.000062,
   0.000041,
   0.000026,
   0.000015,
   0.000008,
   0.000003,
   0.000001,
   0.000000, // 170-179 deg
   // ====== END   GENERATED DATA
   // The following two values for angles of 180 and 181 were not part of the generation process.
   // They were added for safety just in case an angle of 180 degrees appears.
   0.000000,
   0.0000000};

// Returns the BRDF for a Lambertian sphere given an angle (in radians) between the illuminator and the viewer
double LambertianSphereBRDF(double aAngle)
{
   // The incoming angle should AWAYS be in the range [0..180], but don't make assumptions
   double angleRad = fabs(UtMath::NormalizeAngleMinus180_180(aAngle));
   double angleDeg = fabs(angleRad) * UtMath::cDEG_PER_RAD;
   double angleMin = floor(angleDeg);
   int    i1       = static_cast<int>(angleMin);
   double f        = angleDeg - angleMin; // Assuming 1 degree increments in table
   double BRDF     = cSPHERICAL_BRDF[i1] + f * (cSPHERICAL_BRDF[i1 + 1] - cSPHERICAL_BRDF[i1]);
   return BRDF;
}
} // namespace

// =================================================================================================
// This is a facade that acts as the platforms 'infrared_signature' if one wasn't specified.
// It simply redirects to this signature which has all of information necessary.
namespace
{
class InfraredSignatureFacade : public WsfInfraredSignature
{
public:
   explicit InfraredSignatureFacade(WsfSpaceOpticalSignature* aSignaturePtr)
      : WsfInfraredSignature()
      , mSignaturePtr(aSignaturePtr)
   {
   }

   // This should never be called.
   WsfInfraredSignature* Clone() const override
   {
      return nullptr; // TODO_JAJ return new InfraredSignatureFacade(*this);
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

   WsfSpaceOpticalSignature* mSignaturePtr;
};
} // namespace

// =================================================================================================
//! Factory method called by WsfOpticalSignatureTypes.
// static
WsfOpticalSignature* WsfSpaceOpticalSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfOpticalSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_SPACE_OPTICAL_SIGNATURE")
   {
      instancePtr = new WsfSpaceOpticalSignature();
   }
   return instancePtr;
}

// =================================================================================================
WsfSpaceOpticalSignature::WsfSpaceOpticalSignature()
   : WsfOpticalSignature()
   , mSharedDataPtr(new SharedData())
   , mPlotFile()
   , mPlotFieldDelimiter(" ")
   , mDebugLevel(0)
   , mHighResolutionEclipse(false)
   , mStateUpdateMutex()
   , mStateUpdateTime(-1.0)
   , mCurrentTime()
   , mLoc_s()
   , mLoc_p()
   , mUnit_vel()
   , mUnit_ps()
   , mUnit_pe()
   , mUnit_es()
   , mDist_ps(0.0)
   , mDist_pe(0.0)
   , mDist_es(0.0)
   , mTransforms()
   , mEclipseEntryExitTime(0.0)
   , mIsEclipsed(false)
   , mEclipseUpdateMutex()
   , mNextEclipseUpdateTime(-1.0)
   , mEclipseEntryTime(-1.0)
   , mEclipseExitTime(-1.0)
   , mOrbitalPeriod(-1.0)
   , mLastEclipseUpdateTime(-1.0)
   , mLastEclipseState(false)
   , mPlotOfs()
{
   mRequiresPlatform = true;
   SetInherentContrast(false);
}

// =================================================================================================
WsfSpaceOpticalSignature::WsfSpaceOpticalSignature(const WsfSpaceOpticalSignature& aSrc)
   : WsfOpticalSignature(aSrc)
   , mSharedDataPtr(aSrc.mSharedDataPtr)
   , mPlotFile(aSrc.mPlotFile)
   , mPlotFieldDelimiter(aSrc.mPlotFieldDelimiter)
   , mDebugLevel(aSrc.mDebugLevel)
   , mHighResolutionEclipse(aSrc.mHighResolutionEclipse)
   , mStateUpdateMutex()
   , mStateUpdateTime(-1.0)
   , mCurrentTime()
   , mLoc_s()
   , mLoc_p()
   , mUnit_vel()
   , mUnit_ps()
   , mUnit_pe()
   , mUnit_es()
   , mDist_ps(0.0)
   , mDist_pe(0.0)
   , mDist_es(0.0)
   , mTransforms()
   , mEclipseEntryExitTime(0.0)
   , mIsEclipsed(false)
   , mEclipseUpdateMutex()
   , mNextEclipseUpdateTime(-1.0)
   , mEclipseEntryTime(-1.0)
   , mEclipseExitTime(-1.0)
   , mOrbitalPeriod(-1.0)
   , mLastEclipseUpdateTime(-1.0)
   , mLastEclipseState(false)
   , mPlotOfs()
{
}

// =================================================================================================
WsfOpticalSignature* WsfSpaceOpticalSignature::Clone() const // override
{
   return new WsfSpaceOpticalSignature(*this);
}

// =================================================================================================
bool WsfSpaceOpticalSignature::Initialize(double       aSimTime,
                                          WsfPlatform* aPlatformPtr) // override
{
   bool ok = WsfOpticalSignature::Initialize(aSimTime, aPlatformPtr);
   mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime, mCurrentTime);
   if (!mPlotFile.empty())
   {
      mPlotOfs.open(mPlotFile);
      if (!mPlotOfs.is_open())
      {
         auto logger = ut::log::error() << "Unable to open output file.";
         logger.AddNote() << mPlotFile;
         logger.AddNote() << mPlatformPtr->GetName() << ".infrared_signature";
         logger.AddNote() << "File: " << mPlotFile;
         ok = false;
      }
   }

   // If the user did not define an optical_signature item then supply an interface to this signature that
   // will provide the presented area...

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
            auto logger = ut::log::warning() << "The infrared signature will be provided by the optical signature "
                                                "model and not the infrared_signature";
            logger.AddNote() << "Platform: " << mPlatformPtr->GetName();
            logger.AddNote() << "infrared_signature: " << interfacePtr->GetInputType();
         }
         InfraredSignatureFacade* facadePtr = new InfraredSignatureFacade(this);
         interfacePtr->SetSignature(facadePtr);
      }
   }
   return ok;
}

// =================================================================================================
bool WsfSpaceOpticalSignature::InitializeType()
{
   bool ok = WsfOpticalSignature::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
std::vector<WsfStringId> WsfSpaceOpticalSignature::GetStateNames() const
{
   const auto& states = mSharedDataPtr->mStates;
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
bool WsfSpaceOpticalSignature::ProcessInput(UtInput& aInput) // override
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else if (command == "plot_file")
   {
      aInput.ReadValueQuoted(mPlotFile);
      if (mPlotFile == "NULL")
      {
         mPlotFile = "";
      }
   }
   else if (command == "high_resolution_eclipse")
   {
      aInput.ReadValue(mHighResolutionEclipse);
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
   else if (command == "print_computed_data")
   {
      WsfOpticalEnvironment::PrintComputedData();
   }
   else
   {
      myCommand = WsfOpticalSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
float WsfSpaceOpticalSignature::GetSignature(double      aSimTime,
                                             WsfStringId aStateId,
                                             double      aAzimuth,
                                             double      aElevation) // override
{
   double projectedArea = 0.0;
   GetProjectedArea(aSimTime, aStateId, aAzimuth, aElevation, projectedArea);
   return static_cast<float>(projectedArea);
}

// =================================================================================================
bool WsfSpaceOpticalSignature::GetProjectedArea(double      aSimTime,
                                                WsfStringId aStateId,
                                                double      aAzimuth,
                                                double      aElevation,
                                                double&     aProjectedArea)
{
   const State& state = mSharedDataPtr->SelectState(aStateId);
   if (aSimTime != mStateUpdateTime)
   {
      UpdateState(aSimTime, state);
   }

   UtVec3d unit_po; // Unit vector from the platform to the observer
   mPlatformPtr->GetRelativeLocationWCS(aAzimuth, aElevation, 1.0, unit_po.GetData());

   double area = 0.0;
   for (const auto& surfacePtr : state.mSurfaces)
   {
      area += surfacePtr->ProjectedArea(mTransforms[surfacePtr->mListIndex], unit_po);
   }
   aProjectedArea = area;
   return true;
}

// =================================================================================================
bool WsfSpaceOpticalSignature::GetRadiantIntensity(double          aSimTime,
                                                   WsfStringId     aStateId,
                                                   const BandList& aBands,
                                                   double          aAzimuth,
                                                   double          aElevation,
                                                   double&         aRadiantIntensity) // override
{
   const State& state = mSharedDataPtr->SelectState(aStateId);

   // Make sure our state member variables reflect the platform state at the current time.
   if (aSimTime != mStateUpdateTime)
   {
      UpdateState(aSimTime, state);
   }

   Data data; // Engagement data shared between the various routines....
   mPlatformPtr->GetRelativeLocationWCS(aAzimuth, aElevation, 1.0, data.mUnit_po.GetData());
   if (mDebugLevel > 1)
   {
      std::cout << std::fixed << std::setprecision(8);
      double WCS_ToECS_Transform[3][3];
      mPlatformPtr->GetWCSToECSTransform(WCS_ToECS_Transform);
      auto logger = ut::log::debug() << std::setprecision(8) << "Aspect of Viewer";
      logger.AddNote() << "Az: " << aAzimuth * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "El: " << aElevation * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "Total: "
                       << acos(UtVec3d::DotProduct(WCS_ToECS_Transform[0], data.mUnit_po.GetData())) * UtMath::cDEG_PER_RAD;
   }

   // Determine the current eclipse state and time since state last changed.
   GetEclipseState(aSimTime, data);

   double sigma_s = ComputeSolarReflectionSignature(aSimTime, aBands, state, data);
   double sigma_e = ComputeEarthReflectionSignature(aSimTime, aBands, state, data);
   double sigma_t = ComputeThermalSignature(aSimTime, aBands, state, data);

   if (mPlotOfs.is_open())
   {
      std::string del = mPlotFieldDelimiter;
      mPlotOfs << aSimTime << del << sigma_s + sigma_e + sigma_t << del << sigma_s << del << sigma_e << del << sigma_t
               << del << data.mTemp_p << del << mIsEclipsed << std::endl;
   }

   if (mDebugLevel > 0)
   {
      auto logger = ut::log::debug() << "Eclipse State";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Name: " << mPlatformPtr->GetName();
      logger.AddNote() << "Eclipsed: " << mIsEclipsed;
      logger.AddNote() << "Entry/Exit Time: " << mEclipseEntryExitTime;
      logger.AddNote() << "sigma: " << sigma_s + sigma_e + sigma_t;
      logger.AddNote() << "sigma_s: " << sigma_s;
      logger.AddNote() << "sigma_e: " << sigma_e;
      logger.AddNote() << "sigma_t: " << sigma_t;
      logger.AddNote() << "T_p: " << data.mTemp_p;

      // Display Sun and platform locations
      double lat, lon, alt;
      UtEntity::ConvertWCSToLLA(mLoc_p.GetData(), lat, lon, alt);
      logger.AddNote() << "Sat LL: " << UtLatPos(lat) << ' ' << UtLonPos(lon);
      UtEntity::ConvertWCSToLLA(mLoc_s.GetData(), lat, lon, alt);
      logger.AddNote() << "Sun LL: " << UtLatPos(lat) << ' ' << UtLonPos(lon);

      // Display the angles between the various vectors..
      logger.AddNote() << "Platform-relative angles between:";
      logger.AddNote() << "Sun-Observer: " << GetAngleBetween(mUnit_ps, data.mUnit_po) * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "Earth-Observer: " << GetAngleBetween(mUnit_pe, data.mUnit_po) * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "Earth-Sun: " << GetAngleBetween(mUnit_pe, mUnit_ps) * UtMath::cDEG_PER_RAD;
   }
   aRadiantIntensity = sigma_s + sigma_e + sigma_t;
   return true;
}

// =================================================================================================
//! Compute the Earth reflection signature component of the platforms optical signature.
//! The Earth reflection component consists of two parts:
//! - The direction reflection of the solar radiation off the top of the atmosphere. This will occur
//!   only when the portion of the Earth under the platform is illuminated by the Sun.
//! - The natural emittance of the Earth. This occurs at all times.
//! @param aSimTime The current simulation time.
//! @param aBands   The list of bands of interest.
//! @param aState   The current signature state.
//! @param aData    The current interaction data.
//! @returns The Earth reflection signature component (W/sr)
double WsfSpaceOpticalSignature::ComputeEarthReflectionSignature(double          aSimTime,
                                                                 const BandList& aBands,
                                                                 const State&    aState,
                                                                 Data&           aData)
{
   // Compute the contribution due to emission. This occurs even if the Earth part is in shadow.

   // The outgoing radiation in the design white paper is ~ 239.0 W/m^2 and is classified as long-wave radiation.
   double M_emit = cAVG_EARTH_EMITTED_FLUX_DENSITY;    // Radiant exitance (W/m^2)
   double L_emit = M_emit / UtMath::cPI;               // Radiance (W/m^2/sr)
   double T_emit = ut::BlackBody::Temperature(L_emit); // Equivalent black body temperature for the emitted portion
   // T_emit should be very close to cEARTH_EMITTED_BB_TEMP

   // Determine the in-band radiance for a black body of the temperature determined above (W/m^2/sr)
   L_emit = WsfOpticalEnvironment::GetBlackbodyRadiance(aBands, T_emit);

   // Assume the whole Earth is radiating at this temperature and compute the radiant intensity (W/sr).
   // This is simply the radiance times the projected area of the radiation (the area of the Earth's disc.
   double R_e    = UtSphericalEarth::cEARTH_MEAN_RADIUS;
   double I_emit = L_emit * UtMath::cPI * R_e * R_e;

   // Now compute the contribution due to reflection by the Earth of the solar radiation. This will occur only if the
   // portion of the Earth under the platform is illuminated (|theta| < 90)
   // The contribution to due reflection will occur only if the portion of the Earth that is visible from the platform
   // is actually illuminated by the Sun.
   //
   // If the angle is < 90 then it is assumed the entire disc under the satellite is illuminated. No attempt is being
   // made to handle partial illumination.

   // Compute the angle between the line-of-sight vector from the Earth to the platform and from the Earth to the Sun.
   UtVec3d unit_ep(-mUnit_pe);
   double  cosTheta = UtVec3d::DotProduct(unit_ep, mUnit_es);
   cosTheta         = std::min(std::max(cosTheta, -1.0), 1.0);
   double I_refl    = 0.0;
   if (cosTheta > 0.0) // -90 < theta < 90
   {
      // In-band radiant intensity of the Sun (W/sr)
      double I_sun = WsfOpticalEnvironment::GetSolarRadiantIntensity(aBands);

      // Compute the amount of the Sun's radiant intensity captured and re-reflected back into a hemisphere
      double d_es = mDist_es; // Distant from the Earth to the Sun
      double sr   = UtMath::cPI * R_e * R_e / (d_es * d_es);
      I_refl      = I_sun * sr / UtMath::cPI;

      // We are really interested in the EFFECTIVE radiant intensity at the point of interest, so decrease by
      // the cosine of the angle between the line of sight from the Earth to the Sun and the line-of-sight from
      // the Earth to the platform.
      I_refl *= cosTheta;

      // The Earth and atmosphere only reflect some of the incoming solar energy. The rest is absorbed.
      // The reflection factor is the Earth's albedo and is often quoted as 0.3.
      double albedo = cAVG_EARTH_REFLECTED_FLUX_DENSITY / cAVG_EARTH_TOTAL_FLUX_DENSITY;
      I_refl *= albedo;
   }

   // Now combine the emitted and reflected components.
   double I       = I_emit + I_refl; // W/sr
   double sigma_e = ComputeReflection(aSimTime, aBands, aState, aData, I, mUnit_pe, mDist_pe);
   return sigma_e;
}

// =================================================================================================
//! Compute the solar reflection signature component of the platforms optical signature.
//! @param aSimTime The current simulation time.
//! @param aBands   The list of bands of interest.
//! @param aState   The current signature state.
//! @param aData    The current interaction data.
//! @returns The solar reflection signature component (W/sr)
double WsfSpaceOpticalSignature::ComputeSolarReflectionSignature(double          aSimTime,
                                                                 const BandList& aBands,
                                                                 const State&    aState,
                                                                 Data&           aData)
{
   if (mIsEclipsed && !mHighResolutionEclipse)
   {
      return 0.0;
   }

   // In-band radiant intensity of the Sun (W/sr)
   double I_sun = WsfOpticalEnvironment::GetSolarRadiantIntensity(aBands);

   if (mHighResolutionEclipse)
   {
      UtVec3d viewerECI;
      mPlatformPtr->GetLocationECI(viewerECI.GetData());

      // The calendar time now
      UtCalendar now = mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);

      // Sun data
      UtVec3d sunLocECI;
      UtSun::GetSunLocationECI(now, sunLocECI.GetData());
      double sunRadius = UtSun::cMEAN_RADIUS;

      // Earth data
      UtVec3d earthLocECI{};
      double  earthRadius = UtSphericalEarth::cEARTH_MEAN_RADIUS;

      double fraction = UtEclipse::GetUmbralFraction(earthLocECI, earthRadius, sunLocECI, sunRadius, viewerECI);
      I_sun *= 1.0 - fraction;
   }

   // Compute the reflection of the Sun from the platform towards to the observer
   double sigma_s = ComputeReflection(aSimTime, aBands, aState, aData, I_sun, mUnit_ps, mDist_ps);
   return sigma_s;
}

// =================================================================================================
//! Compute the thermal component of the platforms optical signature.
//! This routine computes the temperature of the satellite by using the time since the platform last
//! changed its eclipse state and using the rate of temperature change.
//! @param aSimTime The current simulation time.
//! @param aBands   The list of bands of interest.
//! @param aState   The current signature state.
//! @param aData    The current interaction data.
//! @returns The thermal signature component (W/sr)
double WsfSpaceOpticalSignature::ComputeThermalSignature(double          aSimTime,
                                                         const BandList& aBands,
                                                         const State&    aState,
                                                         Data&           aData)
{
   double sigma_t       = 0.0;
   double area_sum      = 0.0;
   double temp_area_sum = 0.0;
   double last_T_p      = -1.0;
   double L_p           = 0.0;
   for (const auto& surfacePtr : aState.mSurfaces)
   {
      double T_min   = surfacePtr->mMinimumTemperature;
      double T_max   = surfacePtr->mMaximumTemperature;
      double T_delta = T_max - T_min;

      // The time it takes to transition between T_max and T_min
      double t_trans = T_delta / surfacePtr->mTemperatureChangeRate;

      // Determine the current temperature.
      double T_p = 0.0;
      if (mIsEclipsed)
      {
         // mEclipseEntryExitTime is when the platform last entered eclipse
         double f = (aSimTime - mEclipseEntryExitTime) / t_trans;
         assert(f >= 0.0); // will assert if entry/exit times not computed properly
         f   = std::min(std::max(f, 0.0), 1.0);
         T_p = T_max - f * T_delta;
      }
      else
      {
         // mEclipseEntryExitTime is when the platform last exited eclipse
         double f = (aSimTime - mEclipseEntryExitTime) / t_trans;
         assert(f >= 0.0); // will assert if entry/exit times not computed properly
         f   = std::min(std::max(f, 0.0), 1.0);
         T_p = T_min + f * T_delta;
      }

      // Assume the platform is a black body at the temperature computed above. Compute the in-band radiance (W/m^2/sr)
      if (T_p != last_T_p)
      {
         L_p      = WsfOpticalEnvironment::GetBlackbodyRadiance(aBands, T_p);
         last_T_p = T_p;
      }

      // Multiply the radiance times the projected area of the surface in the direction of the observer to get the
      // signature contribution from this surface
      double A_po = surfacePtr->ProjectedArea(mTransforms[surfacePtr->mListIndex], aData.mUnit_po);
      sigma_t += L_p * A_po; // W/sr <- W/m^2/sr * m^2

      area_sum += A_po; // Sum for area weighted average of the temperatures
      temp_area_sum += (T_p * A_po);
   }
   aData.mTemp_p = temp_area_sum / area_sum; // Area weighted average of the temperatures
   return sigma_t;
}

// =================================================================================================
//! Compute the reflected radiant intensity towards the observer due to the radiant intensity of some source.
//!
//! Given the illumination and the unit vector that points to the source of the illumination,
//! compute the reflected flux in the direction of the observer.
//!
//! @param aSimTime    The current simulation time.
//! @param aState      The shared data for the current signature state.
//! @param aData       The interaction data, which contains the direction of the observer from the platform
//! @param aIntensity  The radiant intensity (W/sr) of the illumination.
//! @param aUnit_ps    Unit vector that points from the platform to the source of the illumination.
//! @param aDist_ps    Distance from the platform to the source of the illumination.
//! @returns The reflected radiant intensity (W/sr)
double WsfSpaceOpticalSignature::ComputeReflection(double          aSimTime,
                                                   const BandList& aBands,
                                                   const State&    aState,
                                                   Data&           aData,
                                                   double          aIntensity,
                                                   const UtVec3d&  aUnit_ps,
                                                   double          aDist_ps)
{
   double I_sum = 0.0;
   for (const auto& surfacePtr : aState.mSurfaces)
   {
      I_sum +=
         surfacePtr->Reflect(mTransforms[surfacePtr->mListIndex], aIntensity, aUnit_ps, aDist_ps, aData.mUnit_po, mDebugLevel);
   }
   return I_sum;
}

// =================================================================================================
//! Determine the current eclipse state and when it started.
//! The solar contribution is computed only if the platform is not eclipsed.  Also, ComputeThermalSignature
//! will use the eclipse state AND the time when the current state started in order to compute the temperature
//! of the platform.
//! @param aSimTime The current simulation time.
//! @param aData The updated interaction data.
void WsfSpaceOpticalSignature::GetEclipseState(double aSimTime, Data& aData)
{
   // If it is time, update the times when the platform enters and exits being eclipsed.
   // This is only done once in a while because it is a relatively compute-intensive operation.
   if (aSimTime > mNextEclipseUpdateTime)
   {
      UpdateEclipseTimes(aSimTime, aData);
   }

   // Note that if two threads do pass through here at the same time, it will be for the same simulation
   // time and they will compute the same data. There is no need for a mutex because the data is first
   // computed locally and then the object is updated - and each thread will produce the same result.
   bool   isEclipsed           = false;
   double eclipseEntryExitTime = 0.0;

   // Determine the current eclipse state and determine when the platform entered that state.
   if (mEclipseEntryTime == mEclipseExitTime)
   {
      // This is a special case where there was no eclipse in the current orbit (GetEclipseTimes returned
      // 'false', so no entry or exit times were computed.) The state change time is set way in the past
      // so ComputeThermalSignature will treat it as though it has been illuminated for a long time (which
      // it probably has!)
      isEclipsed           = false;
      eclipseEntryExitTime = aSimTime - mOrbitalPeriod;
   }
   else if (mEclipseExitTime > mEclipseEntryTime)
   {
      // Object was NOT in eclipse at the time of the state calculations
      // The state at evaluation in UpdateEclipseTimes was [ evalTime, entryTime, exitTime],
      // so we ARE NOT eclipsed before 'mEclipseEntryTime' AND after 'mEclipseExitTime'.
      if (aSimTime < mEclipseEntryTime)
      {
         isEclipsed           = false;
         eclipseEntryExitTime = mEclipseExitTime - mOrbitalPeriod;
      }
      else if (aSimTime > mEclipseExitTime)
      {
         isEclipsed           = false;
         eclipseEntryExitTime = mEclipseExitTime;
      }
      else
      {
         isEclipsed           = true;
         eclipseEntryExitTime = mEclipseEntryTime;
      }
   }
   else
   {
      // Object WAS in eclipse at the time of the state calculations.
      // The state at evaluation in UpdateEclipseTimes was [ evalTime, exitTime, entryTime ],
      // so we ARE eclipsed before 'mEclipseExitTime' AND after 'mEclipseEntryTime'.
      if (aSimTime < mEclipseExitTime)
      {
         isEclipsed           = true;
         eclipseEntryExitTime = mEclipseEntryTime - mOrbitalPeriod;
      }
      else if (aSimTime > mEclipseEntryTime)
      {
         isEclipsed           = true;
         eclipseEntryExitTime = mEclipseEntryTime;
      }
      else
      {
         isEclipsed           = false;
         eclipseEntryExitTime = mEclipseExitTime;
      }
   }

   mLastEclipseState      = isEclipsed;
   mLastEclipseUpdateTime = aSimTime;

   // Lastly, update the member variables. (This must be done LAST for multi-threading)
   mIsEclipsed           = isEclipsed;
   mEclipseEntryExitTime = eclipseEntryExitTime;
}

// =================================================================================================
//! Determine the times when the platform enters and exits being eclipsed.
//! This routine is relatively expensive, so it is done only a few times per orbit for space movers.
//! For non-space movers it is done every interaction.
//! @returns true if the Earth is eclipsing the Sun, false if not.
//! @param aData The interaction data containing the geometry information (used only if not a space mover.)
void WsfSpaceOpticalSignature::UpdateEclipseTimes(double aSimTime, Data& aData)
{
   // It has been determined by GetEclipseState that the eclipse times need to be updated, but if there are concurrent
   // updates we have to make sure only one update of the eclipse times is done at a time (the others are redundant as
   // all concurrent updaters are at the same simulation and thus should generate the same results).
   //
   // So first get the lock and then check the time AGAIN. This handles the case where two callers hit this
   // simultaneously. The first gets the lock and updates and the second has to wait. When the first releases the lock
   // and the second gets it, it sees the times have been updated so no need to update again.
   //
   // The initial check in GetEclipseState does not need to be locked. The first concurrent caller that requires an
   // update call this routine and it will lock and do the update, BUT IT WILL NOT UPDATE 'mNextEclipseUpdateTime' UNTIL
   // ALL OTHER DATA HAS BEEN UPDATED!

   std::lock_guard<std::recursive_mutex> lock(mEclipseUpdateMutex);
   if (aSimTime <= mNextEclipseUpdateTime)
   {
      return; // Previous concurrent update has already occurred
   }
   double nextEclipseUpdateTime = 0.0; // See comments above!

   WsfSpaceMoverBase* moverPtr = dynamic_cast<WsfSpaceMoverBase*>(mPlatformPtr->GetMover());
   if (moverPtr != nullptr && !mHighResolutionEclipse)
   {
      // The computed time-to-entry and time-to-exit variables returned by GetEclipseTimes are RELATIVE to the
      // current simulation time (the time passed as the calendar as the first argument) and are ALWAYS POSITIVE and
      // so reflect times in the FUTURE. The current state of being eclipsed or not eclipsed is given by the
      // relationship between the returned values.

      double                   timeToEntry = -1.0;
      double                   timeToExit  = -1.0;
      UtOrbitalPropagatorBase& propagator(moverPtr->GetPropagator());
      mOrbitalPeriod = UtMath::cTWO_PI / propagator.GetOrbitalState().GetOrbitalElements().GetMeanMotion();

      bool ok = propagator.GetEclipseTimes(mCurrentTime, timeToEntry, timeToExit);
      if (ok)
      {
         // Note: If the next transition is not 'close' to the current time, force an update at a time closer to the
         //       transition. It shouldn't be REAL close, and we have to avoid cascading updates. The threshold for
         //       'close' should be larger than the amount subtracted for the update.
         if (timeToEntry < timeToExit)
         {
            // We are currently not eclipsed.
            //   T=aSimTime          -> T=mEclipseEntryTime:   not eclipsed
            //   T=mEclipseEntryTime -> T=mEclipseExitTime:    eclipsed
            //   T=mEclipseExitTime  -> ...                    not eclipsed
            mEclipseEntryTime = aSimTime + timeToEntry;
            mEclipseExitTime  = aSimTime + timeToExit;
            if (timeToEntry > 100.0)
            {
               nextEclipseUpdateTime = mEclipseEntryTime - 50.0;
            }
            else
            {
               nextEclipseUpdateTime = mEclipseExitTime - 50.0;
            }
         }
         else
         {
            // We are currently eclipsed. The entry time is for the start of the NEXT eclipse interval.
            //   T=aSimTime          -> T=mEclipseExitTime:    eclipsed
            //   T=mEclipseExitTime  -> T=mEclipseEntryTime:   not eclipsed
            //   T=mEclipseEntryTime -> ...                    eclipsed
            mEclipseExitTime      = aSimTime + timeToExit;
            mEclipseEntryTime     = aSimTime + timeToEntry;
            nextEclipseUpdateTime = mEclipseExitTime - 10.0;
            if (timeToExit > 100.0)
            {
               nextEclipseUpdateTime = mEclipseExitTime - 50.0;
            }
            else
            {
               nextEclipseUpdateTime = mEclipseEntryTime - 50.0;
            }
         }
      }
      else
      {
         // Entry and exit times were not computed because an eclipse does not occur within the orbit time.
         // We don't know exactly when the platform was last eclipsed - we just have to assume it as always
         // been in the Sun. Setting the following values will get a reasonably high exposure time in
         // ComputeThermalSignature, while at the same time not forcing this routine to be called too often.
         mEclipseEntryTime     = aSimTime + mOrbitalPeriod;
         mEclipseExitTime      = mEclipseEntryTime;
         nextEclipseUpdateTime = aSimTime + 0.25 * mOrbitalPeriod; // Force update once in awhile
      }
      if (mDebugLevel > 0)
      {
         auto logger = ut::log::debug() << "Eclipse Times";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << mPlatformPtr->GetName();
         logger.AddNote() << "ok: " << ok;
         logger.AddNote() << "TimeToEntry: " << timeToEntry;
         logger.AddNote() << "TimeToExit: " << timeToExit;
         logger.AddNote() << "Period: " << mOrbitalPeriod;
         if (mEclipseExitTime >= mEclipseEntryTime)
         {
            auto note = logger.AddNote() << "NOT Eclipsed.";
            note.AddNote() << "Entry Time: " << mEclipseEntryTime;
            note.AddNote() << "Exit Time: " << mEclipseExitTime;
         }
         else
         {
            auto note = logger.AddNote() << "IS Eclipsed.";
            note.AddNote() << "Entry Time: " << mEclipseEntryTime;
            note.AddNote() << "Exit Time: " << mEclipseExitTime;
         }
      }
   }
   else
   {
      // The mover for this platform is not a space mover, or it is and we have selected high resolution solar component
      // computations.
      UtVec3d locationECI;
      mPlatformPtr->GetLocationECI(locationECI.GetData()); // (The ECI location is needed to check the eclipsed state)
      mIsEclipsed = (UtEclipse::GetEclipsedState(mCurrentTime, locationECI) != UtEclipse::cECLIPSED_NONE);
      static constexpr double cLARGE_TIME = 9.0E99;
      if (mLastEclipseUpdateTime < 0.0) // Initializing...
      {
         if (mIsEclipsed)
         {
            mEclipseEntryTime = std::max(aSimTime - 10.0, 0.0);
            mEclipseExitTime  = cLARGE_TIME;
         }
         else
         {
            mEclipseEntryTime = cLARGE_TIME;
            mEclipseExitTime  = std::max(aSimTime - 10.0, 0.0);
         }
      }
      else if (mIsEclipsed != mLastEclipseState)
      {
         if (mIsEclipsed)
         {
            mEclipseEntryTime = aSimTime;
            mEclipseExitTime  = cLARGE_TIME;
         }
         else
         {
            mEclipseExitTime  = aSimTime;
            mEclipseEntryTime = cLARGE_TIME;
         }
      }
      mOrbitalPeriod        = 20.0;
      mLastEclipseState     = mIsEclipsed;
      nextEclipseUpdateTime = aSimTime; // Force this routine to be called when time increases
   }
   // This must be done last so concurrent callers are ensured that all data is up-to-date.
   mNextEclipseUpdateTime = nextEclipseUpdateTime;
}

// =================================================================================================
// Update the member variables that reflect the current platform state.
void WsfSpaceOpticalSignature::UpdateState(double aSimTime, const State& aState)
{
   std::lock_guard<std::recursive_mutex> lock(mStateUpdateMutex);
   if (aSimTime == mStateUpdateTime)
   {
      return; // Previous concurrent update has already occurred
   }

   // Advance the calendar up to the current simulation time...
   mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime, mCurrentTime);

   // Update the vectors and distance between the platform, Sun and Earth.
   UtSun::GetSunLocationWCS(mCurrentTime, mLoc_s);    // Location of the Sun
   mPlatformPtr->GetLocationWCS(mLoc_p.GetData());    // Location of the platform
   mPlatformPtr->GetVelocityWCS(mUnit_vel.GetData()); // Will be unit vector of platform velocity
   mUnit_ps = mLoc_s - mLoc_p;                        // Will be unit vector from the platform to the Sun
   mUnit_pe = -mLoc_p;                                // Will be unit vector from the platform to the Earth
   mUnit_es = mLoc_s;                                 // Will be unit vector from the Earth to the Sun
   mUnit_vel.Normalize();
   mDist_ps = mUnit_ps.Normalize(); // Distance from the platform to the Sun
   mDist_pe = mUnit_pe.Normalize(); // Distance from the platform to the Earth
   mDist_es = mUnit_es.Normalize(); // Distance from the Earth to the Sun

   // Update the WCS->SCS transform for each surface.
   Surface::RefData refData;
   refData.mPlatformPtr = mPlatformPtr;
   mPlatformPtr->GetWCSToECSTransform(refData.mWCS_ToECS_Transform);
   refData.mUnit_vel   = mUnit_vel;
   refData.mUnit_ps    = mUnit_ps;
   refData.mUnit_pe    = mUnit_pe;
   refData.mDebugLevel = mDebugLevel;

   if (mDebugLevel > 1)
   {
      auto logger = ut::log::debug() << "Platform State";
      logger.AddNote() << std::fixed << std::setprecision(8);
      logger.AddNote() << "Platform Vel: " << mUnit_vel;
      logger.AddNote() << "Platform->Sun: " << mUnit_ps;
      logger.AddNote() << "Platform->Earth: " << mUnit_pe;
      logger.AddNote() << "WCS->ECS X: " << UtVec3d(refData.mWCS_ToECS_Transform[0]);
      logger.AddNote() << "WCS->ECS Y: " << UtVec3d(refData.mWCS_ToECS_Transform[1]);
      logger.AddNote() << "WCS->ECS Z: " << UtVec3d(refData.mWCS_ToECS_Transform[2]);
      double az, el;
      mPlatformPtr->ComputeAspect(mUnit_ps.GetData(), az, el);
      logger.AddNote() << "Aspect of Sun: Az: " << az * UtMath::cDEG_PER_RAD << " El: " << el * UtMath::cDEG_PER_RAD
                       << " Total: "
                       << acos(UtVec3d::DotProduct(refData.mWCS_ToECS_Transform[0], mUnit_ps.GetData())) *
                             UtMath::cDEG_PER_RAD;
      mPlatformPtr->ComputeAspect(mUnit_pe.GetData(), az, el);
      logger.AddNote() << "Aspect of Earth: Az: " << az * UtMath::cDEG_PER_RAD << " El: " << el * UtMath::cDEG_PER_RAD
                       << " Total: "
                       << acos(UtVec3d::DotProduct(refData.mWCS_ToECS_Transform[0], mUnit_pe.GetData())) *
                             UtMath::cDEG_PER_RAD;
   }

   if (mTransforms.size() < mSharedDataPtr->mMaxSurfaceCount)
   {
      mTransforms.resize(aState.mSurfaces.size());
   }
   for (const auto& surfacePtr : aState.mSurfaces)
   {
      surfacePtr->UpdateTransform(refData, mTransforms[surfacePtr->mListIndex]);
   }

   mStateUpdateTime = aSimTime; // This must be done LAST before unlocking the mutex.
}

// =================================================================================================
// Nested class 'SharedData'
// =================================================================================================

WsfSpaceOpticalSignature::SharedData::SharedData()
   : mDefaultSurface()
   , mCurrentStateId("default")
   , mDefaultStatePtr(nullptr)
   , mStates()
   , mMaxSurfaceCount(0)
{
}

// =================================================================================================
bool WsfSpaceOpticalSignature::SharedData::InitializeType(WsfObject& aBase)
{
   bool ok = true;

   if (mStates.empty())
   {
      ut::log::error() << "At least one state must be implicitly or explicitly defined.";
      return false;
   }

   // Make sure the state 'default' exists.
   mDefaultStatePtr = nullptr;
   for (const auto& state : mStates)
   {
      if (state.mStateId == "default")
      {
         mDefaultStatePtr = &state;
         break;
      }
   }

   if (mDefaultStatePtr == nullptr)
   {
      ok = false;
      ut::log::error() << "The state 'default' does not exist.";
   }

   // Make sure each state has at least one surface, and determine the maximum surface count in any state.
   mMaxSurfaceCount = 0;
   for (const auto& state : mStates)
   {
      if (state.mSurfaces.empty())
      {
         ok          = false;
         auto logger = ut::log::error() << "No surfaces where defined for state";
         logger.AddNote() << "State: " << state.mStateId;
      }
      mMaxSurfaceCount = std::max(mMaxSurfaceCount, state.mSurfaces.size());
   }
   return ok;
}

// =================================================================================================
bool WsfSpaceOpticalSignature::SharedData::IsA_ValidState(WsfStringId aId) const
{
   if (aId == "default") // "default" is always true
   {
      return true;
   }
   for (const auto& state : mStates)
   {
      if (state.mStateId == aId)
      {
         return true;
      }
   }
   return false;
}

// =================================================================================================
bool WsfSpaceOpticalSignature::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);
      mCurrentStateId = stateName;
      // Make sure the new state has not been used
      for (const auto& state : mStates)
      {
         if (mCurrentStateId == state.mStateId)
         {
            throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
         }
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
      if (mStates.empty() ||                          // First surface in the "default" state
          mStates.back().mStateId != mCurrentStateId) // First surface in a state that is different than the last one
      {
         mStates.emplace_back(mCurrentStateId);
      }

      // Update the index of the surface in the list of surfaces
      surfacePtr->mListIndex = mStates.back().mSurfaces.size();

      // ... and add the surface to hold the state
      mStates.back().mSurfaces.push_back(std::move(surfacePtr));
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
const WsfSpaceOpticalSignature::State& WsfSpaceOpticalSignature::SharedData::SelectState(WsfStringId aId) const
{
   for (const auto& state : mStates)
   {
      if (aId == state.mStateId)
      {
         return state;
      }
   }
   return *mDefaultStatePtr;
}

// =================================================================================================
// Nested class Surface
// =================================================================================================

bool WsfSpaceOpticalSignature::Surface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "reflectance")
   {
      aInput.ReadValue(mReflectance);
      aInput.ValueInClosedRange(mReflectance, 0.0, 1.0);
      // TODO_NOTE: For now we allow a reflectance of greater than one to allow for tweaking the model.
      aInput.ValueGreaterOrEqual(mReflectance, 0.0);
   }
   else if (command == "minimum_temperature")
   {
      aInput.ReadValueOfType(mMinimumTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreater(mMinimumTemperature, 0.0);
      aInput.ValueLessOrEqual(mMinimumTemperature, mMaximumTemperature);
   }
   else if (command == "maximum_temperature")
   {
      aInput.ReadValueOfType(mMaximumTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreaterOrEqual(mMaximumTemperature, mMinimumTemperature);
   }
   else if (command == "temperature_change_rate")
   {
      aInput.ReadValue(mTemperatureChangeRate);
      aInput.ValueGreater(mTemperatureChangeRate, 0.0);
      std::string units;
      aInput.ReadValue(units);
      std::string::size_type slashPos = units.find('/');
      if ((slashPos != 0) && (slashPos != std::string::npos) && ((units.size() - slashPos) > 1))
      {
         std::string tempUnits(units.substr(0, slashPos));
         std::string timeUnits(units.substr(slashPos + 1));
         double      tempFactor = aInput.ConvertValue(1.0, tempUnits, UtInput::cTEMPERATURE);
         double      timeFactor = aInput.ConvertValue(1.0, timeUnits, UtInput::cTIME);
         double      multiplier = tempFactor / timeFactor;
         mTemperatureChangeRate *= multiplier;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown temperature_change_rate units " + units);
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Compute the projected area of the surface in the direction of the viewer.
//! @param aWCS_ToSCS_Transform The WCS->SCS transform.
//! @param aUnit_v  The unit vector pointing from the surface (platform) towards the viewer
//! @returns The projected area (m^2)
double WsfSpaceOpticalSignature::Surface::ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const
{
   return 0.0;
}

// =================================================================================================
//! Compute the reflected radiant intensity of some incident illumination.
//! @param aWCS_ToSCS_Transform The WCS->SCS transform.
//! @param aIntensity The radiant intensity of the illumination (W/sr)
//! @param aUnit_i  Unit vector pointing from the surface (platform) towards the illuminator.
//! @param aDist_i  Distance from the illuminator to the surface (platform) (m).
//! @param aUnit_v  The unit vector pointing from the surface (platform) towards the viewer.
//! @param aDebugLevel Debug flag from the caller.
//! @returns The reflected radiant intensity (W/sr)
double WsfSpaceOpticalSignature::Surface::Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                                                  double         aIntensity,
                                                  const UtVec3d& aUnit_i,
                                                  double         aDist_i,
                                                  const UtVec3d& aUnit_v,
                                                  unsigned int   aDebugLevel) const
{
   return 0.0;
}

// =================================================================================================
//! Update the WCS-to-SCS (surface coordinate system) transform.
//! @param aRefData The reference data for computing the WCS->SCS transform.
//! @param aWCS_ToSCS_Transform The computed WCS->SCS transform.
void WsfSpaceOpticalSignature::Surface::UpdateTransform(const RefData& aRefData, MyMat3d& aWCS_ToSCS_Transform) const
{
   UtMat3d::Set(aWCS_ToSCS_Transform.mMat, aRefData.mWCS_ToECS_Transform);
}

// =================================================================================================
// Begin anonymous namespace for specific surface implementations.
// =================================================================================================
namespace
{

// TODO_JAJ NOTE:
//  These models for specular reflectance are not being included in the initial release.
//  They are left here for future reference, but they need some work! They should probably be defined
//  as members of the 'Surface' class, and whatever parameters they need can be defined there.
//  Phong and Blinn-Phong are reasonably complete, but Cook-Torrance needs work.

#ifdef TODO_JAJ
//! Blinn-Phong model for specular highlights
//! @param aL Unit vector to the light
//! @param aN Unit vector normal to the surface
//! @param aV Unit vector to the viewer
double BlinnPhong(const UtVec3d& aL, const UtVec3d& aN, const UtVec3d& aV)
{
   double n = 2.0; // The Phong exponent

   // Compute the half way vector between the light and viewer vectors, H
   UtVec3d H(aL + aV);
   H.Normalize();

   double cosTheta = UtVec3d::DotProduct(aN, H);
   cosTheta        = UtMath::Limit(cosTheta, 0.0, 1.0); // Must clamp to positive
   return pow(cosTheta, n);
}


//! Phong model for specular highlights
//! @param aL Unit vector to the light
//! @param aN Unit vector normal to the surface
//! @param aV Unit vector to the viewer
double Phong(const UtVec3d& aL, const UtVec3d& aN, const UtVec3d& aV)
{
   double n = 2.0; // The Phong exponent

   // Compute the reflection vector.
   UtVec3d R = aL - (2.0 * UtVec3d::DotProduct(aL, aN)) * aN; // R = V - 2(R dot N)N
   R.Normalize();

   double cosTheta = UtVec3d::DotProduct(R, aV);
   cosTheta        = UtMath::Limit(cosTheta, 0.0, 1.0); // Must clamp to positive
   return pow(cosTheta, n);
}

//! Cook-Torrance model for specular highlights
//! @param aL Unit vector to the light
//! @param aN Unit vector normal to the surface
//! @param aV Unit vector to the viewer
double CookTorrance(const UtVec3d& aL, const UtVec3d& aN, const UtVec3d& aV)
{
   // Compute the half way vector (H) between the light and viewer vectors
   UtVec3d H(aL + aV);
   H.Normalize();

   double cos_VN = UtVec3d::DotProduct(aV, aN);
   double cos_LN = UtVec3d::DotProduct(aL, aN);
   if ((cos_VN <= 0.0) || (cos_LN <= 0.0))
   {
      return 0.0;
   }
   double cos_VH = UtVec3d::DotProduct(aV, H);
   double cos_HN = UtVec3d::DotProduct(H, aN);

   double G1 = 2.0 * cos_HN * cos_VN / cos_VH;
   double G2 = 2.0 * cos_HN * cos_LN / cos_VH;
   double G  = std::min(std::min(1.0, G1), G2); // min(1,G1,G2)

   // Compute the Beckman distribution function, D

   double m          = 0.3;
   double mSq        = m * m;
   double cosAlpha   = cos_HN;
   double cosAlphaSq = cosAlpha * cosAlpha;
   double t1         = (1.0 - cosAlphaSq) / (mSq * cosAlphaSq);
   double D          = exp(-t1) / (UtMath::cPI * mSq * cosAlpha * cosAlpha);


   // Compute the Fresnel term, F, using Sclick's approximation
   double F = 1.0; // TODO_JAJ

   double k_spec = (D * F * G) / (4.0 * cos_VN * cos_LN);
   return k_spec;
}
#endif

// =================================================================================================
// Base orientable surface definition.
// This is used as a base class for non-spherical surfaces. For the most part this is a reduced
// implementation of WsfArticulatedPart/UtEntityPart.
// =================================================================================================

class OrientableSurface : public WsfSpaceOpticalSignature::Surface
{
public:
   enum CueTo
   {
      cCUE_TO_NOTHING,
      cCUE_TO_SUN
   };

   explicit OrientableSurface(const Surface& aDefault)
      : Surface(aDefault)
      , mYaw(0.0)
      , mPitch(0.0)
      , mRoll(0.0)
      , mMinAzCue(0.0)
      , mMaxAzCue(0.0)
      , mMinElCue(0.0)
      , mMaxElCue(0.0)
      , mCueTo(cCUE_TO_NOTHING)
      , mIdentityTransform(true)

   {
      UpdateECS_ToSCS_Transform();
   }

   bool ProcessInput(UtInput& aInput) override;

   void UpdateTransform(const RefData& aRefData, MyMat3d& aWCS_ToSCS_Transform) const override;

   void UpdateECS_ToSCS_Transform();

   double mECS_ToSCS_Transform[3][3];
   double mYaw;
   double mPitch;
   double mRoll;
   double mMinAzCue;
   double mMaxAzCue;
   double mMinElCue;
   double mMaxElCue;
   CueTo  mCueTo;
   bool   mIdentityTransform;
};

bool OrientableSurface::ProcessInput(UtInput& aInput)
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
   else if (command == "azimuth_cue_limits")
   {
      aInput.ReadValueOfType(mMinAzCue, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxAzCue, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinAzCue, -UtMath::cPI);
      aInput.ValueLessOrEqual(mMaxAzCue, UtMath::cPI);
      aInput.ValueLessOrEqual(mMinAzCue, mMaxAzCue);
   }
   else if (command == "elevation_cue_limits")
   {
      aInput.ReadValueOfType(mMinElCue, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxElCue, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinElCue, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMaxElCue, UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMinElCue, mMaxElCue);
   }
   else if (command == "cue_to")
   {
      std::string cueType;
      aInput.ReadValue(cueType);
      if (cueType == "sun")
      {
         mCueTo = cCUE_TO_SUN;
      }
   }
   else
   {
      myCommand = Surface::ProcessInput(aInput);
   }
   return myCommand;
}

void OrientableSurface::UpdateTransform(const RefData& aRefData, MyMat3d& aWCS_ToSCS_Transform) const
{
   // First construct the uncued surface coordinate system (SCS) transform
   if (mIdentityTransform)
   {
      // Yaw, pitch, roll not specified - so just use the WCS->ECS transform
      UtMat3d::Set(aWCS_ToSCS_Transform.mMat, aRefData.mWCS_ToECS_Transform);
   }
   else
   {
      // Yaw, pitch or roll specified. [WCS->SCS] = [ECS->SCS] x [WCS->ECS]
      UtMat3d::Multiply(aWCS_ToSCS_Transform.mMat, mECS_ToSCS_Transform, aRefData.mWCS_ToECS_Transform);
   }

   // If a cue was specified then apply it.
   double  cueAz_r = 0.0; // Requested cue
   double  cueEl_r = 0.0;
   double  cueAz_a = 0.0; // Actual cue
   double  cueEl_a = 0.0;
   UtVec3d cueVecWCS(0.0, 0.0, 0.0);
   if (mCueTo != cCUE_TO_NOTHING)
   {
      switch (mCueTo)
      {
      case cCUE_TO_SUN:
         cueVecWCS = aRefData.mUnit_ps;
         break;
      default:
         assert(false); // should never happen
         break;
      }

      // Compute the pointing vector in the uncued SCS frame and then extract the az/el.

      double cueVecSCS[3];
      UtMat3d::Transform(cueVecSCS, aWCS_ToSCS_Transform.mMat, cueVecWCS.GetData()); // [SCS] = [WCS->SCS] x [WCS]
      UtEntity::ComputeAzimuthAndElevation(cueVecSCS, cueAz_r, cueEl_r);
      cueAz_a = cueAz_r;
      cueEl_a = cueEl_r;
      // Make sure the cue is within the limits...
      if (mMinAzCue != mMaxAzCue) // Surface can be cued in azimuth
      {
         if ((cueAz_a < mMinAzCue) || (cueAz_a > mMaxAzCue))
         {
            // Cue is outside the azimuth limit. Set position to the closest limit
            double deltaMin = UtMath::NormalizeAngle0_TwoPi(mMinAzCue - cueAz_a);
            double deltaMax = UtMath::NormalizeAngle0_TwoPi(cueAz_a - mMaxAzCue);
            if (deltaMin <= deltaMax)
            {
               cueAz_a = mMinAzCue; // Set position to the min limit
            }
            else
            {
               cueAz_a = mMaxAzCue; // Set position to the max limit
            }
         }
      }
      else
      {
         cueAz_a = 0.0;
      }

      if (mMinElCue != mMaxElCue) // Surface can be cued in elevation
      {
         cueEl_a = UtMath::Limit(cueEl_a, mMinElCue, mMaxElCue);
      }
      else
      {
         cueEl_a = 0.0;
      }

      // Compute the transform from the cued to uncued surface coordinate system.
      double Cued_ToUncued_Transform[3][3];
      UtEntity::ComputeRotationalTransform(cueAz_a, cueEl_a, 0.0, Cued_ToUncued_Transform);

      // And now compute the final WCS to cued surface coordinate system transform.
      double newWCS_ToSCS_Transform[3][3];
      UtMat3d::Multiply(newWCS_ToSCS_Transform, Cued_ToUncued_Transform, aWCS_ToSCS_Transform.mMat);
      UtMat3d::Set(aWCS_ToSCS_Transform.mMat, newWCS_ToSCS_Transform);
   }

   if (aRefData.mDebugLevel > 1)
   {
      auto logger = ut::log::debug() << "Transform";
      logger.AddNote() << std::fixed << std::setprecision(8);
      logger.AddNote() << "Surface Index: " << mListIndex;
      logger.AddNote() << "ECS->SCS X: " << UtVec3d(mECS_ToSCS_Transform[0]);
      logger.AddNote() << "ECS->SCS Y: " << UtVec3d(mECS_ToSCS_Transform[1]);
      logger.AddNote() << "ECS->SCS Z: " << UtVec3d(mECS_ToSCS_Transform[2]);
      logger.AddNote() << "Cue: " << cueVecWCS;
      logger.AddNote() << "Before Limiting: Az: " << cueAz_r * UtMath::cDEG_PER_RAD
                       << " El: " << cueEl_r * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "After Limiting: Az: " << cueAz_a * UtMath::cDEG_PER_RAD
                       << " El: " << cueEl_a * UtMath::cDEG_PER_RAD;
      logger.AddNote() << "WCS->SCS X: " << UtVec3d(aWCS_ToSCS_Transform.mMat[0]);
      logger.AddNote() << "WCS->SCS Y: " << UtVec3d(aWCS_ToSCS_Transform.mMat[1]);
      logger.AddNote() << "WCS->SCS Z: " << UtVec3d(aWCS_ToSCS_Transform.mMat[2]);
   }
}

// This is called only during construction and input. This ECS->SCS transform is static once input processing is complete.
void OrientableSurface::UpdateECS_ToSCS_Transform()
{
   if ((mYaw != 0.0) || (mPitch != 0.0) || (mRoll != 0.0))
   {
      UtEntity::ComputeRotationalTransform(mYaw, mPitch, mRoll, mECS_ToSCS_Transform);
      mIdentityTransform = false;
   }
   else
   {
      UtMat3d::Identity(mECS_ToSCS_Transform);
      mIdentityTransform = true;
   }
}

// =================================================================================================
// 'box' surface definition
// =================================================================================================

class BoxSurface : public OrientableSurface
{
public:
   explicit BoxSurface(const Surface& aDefaults)
      : OrientableSurface(aDefaults)
      , mSize(1.0, 1.0, 1.0)
      , mFaceArea(1.0, 1.0, 1.0)
   {
   }
   bool   ProcessInput(UtInput& aInput) override;
   double ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const override;
   double Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                  double         aIntensity,
                  const UtVec3d& aUnit_i,
                  double         aDist_i,
                  const UtVec3d& aUnit_v,
                  unsigned int   aDebugLevel) const override;

   UtVec3d mSize;
   UtVec3d mFaceArea;
};

// =================================================================================================
bool BoxSurface::ProcessInput(UtInput& aInput)
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
      myCommand = OrientableSurface::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double BoxSurface::ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const
{
   // Note that while the box has six sides, we only do 3 sides. For a given opposing pair of faces one will be facing
   // the observer and the other will face away.
   double  area_sum = 0.0;
   UtVec3d unit_n_SCS(0.0, 0.0, 0.0);
   UtVec3d unit_n; // normal to the current face..
   for (size_t axis = 0; axis < 3; ++axis)
   {
      unit_n_SCS[axis] = 1.0; // Form the normal vector for the face
      unit_n           = aWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
      unit_n_SCS[axis] = 0.0; // Reset for next pass
      double cos_nv    = unit_n.DotProduct(aUnit_v);
      cos_nv           = fabs(cos_nv); // Takes only the face with an area visible to the observer
      area_sum += mFaceArea[axis] * cos_nv;
   }
   return area_sum;
}

// =================================================================================================
double BoxSurface::Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                           double         aIntensity,
                           const UtVec3d& aUnit_i,
                           double         aDist_i,
                           const UtVec3d& aUnit_v,
                           unsigned int   aDebugLevel) const
{
   if (aDebugLevel > 1)
   {
      auto logger = ut::log::debug() << "Reflect, Surface" << mListIndex << std::setprecision(8);
      logger.AddNote() << "I_i: " << aIntensity << " w/sr";
      logger.AddNote() << "I_0:" << std::scientific << aIntensity << " w/sr";
      logger.AddNote() << "Unit I: " << std::fixed << aUnit_i;
      logger.AddNote() << "Unit V: " << std::fixed << aUnit_v;
   }

   double  I_ref_sum = 0.0; // Total reflected radiant intensity (W/sr)
   UtVec3d unit_n_SCS(0.0, 0.0, 0.0);
   UtVec3d unit_n; // normal to the current face..

   auto logger = ut::log::debug();
   if (aDebugLevel > 1)
   {
      logger << "Axis Measurements";
   }
   for (size_t axis = 0; axis < 3; ++axis)
   {
      // Zero-area faces are typically used for things like solar panels - just ignore them.
      if (mFaceArea[axis] > 0.0)
      {
         unit_n_SCS[axis] = 1.0; // Form the normal vector for the face
         unit_n           = aWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
         unit_n_SCS[axis] = 0.0; // Reset for next pass

         // Compute the angle of the illumination with respect to the surface.
         // If the angle is greater than 90 degrees switch and use the opposing surface.
         double cos_ni    = unit_n.DotProduct(aUnit_i);
         size_t faceIndex = axis;
         if (cos_ni < 0.0)
         {
            cos_ni = -cos_ni;
            unit_n = -unit_n;
            faceIndex += 3;
         }

         // Compute the angle of the viewer with respect to the surface.
         // If the angle is greater than or equal to 90 degrees there is no reflection towards the viewer
         double cos_nv = unit_n.DotProduct(aUnit_v);
         double I_ref  = 0.0;
         if (cos_nv > 0.0)
         {
            // Compute the irradiance of the surface at the specified distance from the illuminator.
            //
            // The intensity (I) is in W/sr and we need the irradiance (E) in W/m2.  The total radiant flux (Phi) in
            // Watts through the surface is the intensity (I) times the solid angle (omega) subtended by the surface.
            // The radiant flux is then divided by the TOTAL area of the surface to give the irradiance of the surface.
            //
            // omega = area * cos(alpha_i) / (d_s * d_s)
            // Phi   = I * omega
            // E     = Phi / area
            //
            // This can be reduced as follows:
            //
            // E = Phi / area = (I * omega) / area = I * (area * cos(alpha_i) / (d_s * d_s)) / area
            //   = I * cos(theta_i) / (d_s * d_s)
            double E    = aIntensity * cos_ni / (aDist_i * aDist_i);
            double BRDF = 1.0 / UtMath::cPI; // BRDF is a constant for a Lambertian surface
            double L    = E * BRDF;          // So therefore, is the radiance
            L *= mReflectance;               // TODO_JAJ should this be band-specific?

            // The radiant intensity is the radiance times the projected area in the direction of the viewer.
            I_ref = L * mFaceArea[axis] * cos_nv;
            I_ref_sum += I_ref;
         }
         if (aDebugLevel > 1)
         {
            auto note = logger.AddNote() << "Face " << faceIndex << ":";
            note.AddNote() << "Area: " << mFaceArea[axis] << std::fixed;
            note.AddNote() << "Unit N: " << unit_n;
            note.AddNote() << "I dot N: " << std::setprecision(8) << cos_ni << " (" << std::setprecision(2)
                           << acos(cos_ni) * UtMath::cDEG_PER_RAD << " deg)";
            note.AddNote() << "V dot N: " << std::setprecision(8) << cos_nv << " (" << std::setprecision(2)
                           << acos(cos_nv) * UtMath::cDEG_PER_RAD << " deg)";
            double cos_iv = aUnit_i.DotProduct(aUnit_v);
            note.AddNote() << "I dot V: " << std::setprecision(8) << cos_iv << " (" << std::setprecision(2)
                           << acos(cos_iv) * UtMath::cDEG_PER_RAD << " deg)";
            note.AddNote() << "I_ref: " << std::scientific << std::setprecision(6) << I_ref << " w/sr";
         }
      }
   }
   return I_ref_sum;
}
// =================================================================================================
// 'plane' surface definition
// =================================================================================================

class PlaneSurface : public OrientableSurface
{
public:
   explicit PlaneSurface(const Surface& aDefaults)
      : OrientableSurface(aDefaults)
      , mArea(1.0)
   {
   }
   bool   ProcessInput(UtInput& aInput) override;
   double ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const override;
   double Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                  double         aIntensity,
                  const UtVec3d& aUnit_i,
                  double         aDist_i,
                  const UtVec3d& aUnit_v,
                  unsigned int   aDebugLevel) const override;

   double mArea;
};

// =================================================================================================
bool PlaneSurface::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "area")
   {
      aInput.ReadValueOfType(mArea, UtInput::cAREA);
      aInput.ValueGreater(mArea, 0.0);
   }
   else
   {
      myCommand = OrientableSurface::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
double PlaneSurface::ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const
{
   UtVec3d unit_n_SCS(1.0, 0.0, 0.0); // normal to the face in SCS
   UtVec3d unit_n = aWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
   double  cos_nv = unit_n.DotProduct(aUnit_v);
   cos_nv         = fabs(cos_nv); // Only take the front of the plane
   return mArea * cos_nv;
}

// =================================================================================================
double PlaneSurface::Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                             double         aIntensity,
                             const UtVec3d& aUnit_i,
                             double         aDist_i,
                             const UtVec3d& aUnit_v,
                             unsigned int   aDebugLevel) const
{
   double  I_ref = 0;
   UtVec3d unit_n_SCS(1.0, 0.0, 0.0); // normal to the face in SCS
   UtVec3d unit_n = aWCS_ToSCS_Transform.InverseMultiply(unit_n_SCS);
   double  cos_ni = unit_n.DotProduct(aUnit_i);
   if (cos_ni > 0.0) // Not facing away from illumination
   {
      double cos_nv = unit_n.DotProduct(aUnit_v);
      if (cos_nv > 0.0) // Not facing away from the viewer
      {
         double E    = aIntensity * cos_ni / (aDist_i * aDist_i);
         double BRDF = 1.0 / UtMath::cPI; // BRDF is a constant for a Lambertian surface
         double L    = E * BRDF;          // So therefore, is the radiance
         L *= mReflectance;               // TODO_JAJ should this be band-specific?
         // The radiant intensity is the radiance times the projected area in the direction of the viewer.
         I_ref = L * mArea * cos_nv;
      }
   }
   if (aDebugLevel > 1)
   {
      auto logger = ut::log::debug() << "Reflect";
      logger.AddNote() << std::setprecision(8);
      logger.AddNote() << "Reflect, Surface " << mListIndex << " I_i: " << aIntensity << " w/sr";
      logger.AddNote() << std::scientific << "  I0: " << aIntensity << " w/sr" << std::fixed;
      logger.AddNote() << "  Unit I: " << aUnit_i;
      logger.AddNote() << "  Unit V: " << aUnit_v;
      logger.AddNote() << "  Unit N: " << unit_n;
      logger.AddNote() << "  I dot N: " << std::setprecision(8) << cos_ni << " (" << std::setprecision(2)
                       << acos(cos_ni) * UtMath::cDEG_PER_RAD << " deg)";
      double cos_nv = unit_n.DotProduct(aUnit_v);
      logger.AddNote() << "  V dot N: " << std::setprecision(8) << cos_nv << " (" << std::setprecision(2)
                       << acos(cos_nv) * UtMath::cDEG_PER_RAD << " deg)";
      double cos_iv = aUnit_i.DotProduct(aUnit_v);
      logger.AddNote() << "  I dot V: " << std::setprecision(8) << cos_iv << " (" << std::setprecision(2)
                       << acos(cos_iv) * UtMath::cDEG_PER_RAD << " deg)";
      logger.AddNote() << "  I_ref: " << std::scientific << std::setprecision(6) << I_ref << " w/sr";
   }
   return I_ref;
}

// =================================================================================================
// 'sphere' surface definition
// =================================================================================================

class SphereSurface : public WsfSpaceOpticalSignature::Surface
{
public:
   explicit SphereSurface(const Surface& aDefaults)
      : Surface(aDefaults)
      , mRadius(1.0)
   {
   }
   bool   ProcessInput(UtInput& aInput) override;
   double ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const override;
   double Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                  double         aIntensity,
                  const UtVec3d& aUnit_i,
                  double         aDist_i,
                  const UtVec3d& aUnit_v,
                  unsigned int   aDebugLevel) const override;

   double mRadius;
};

// =================================================================================================
bool SphereSurface::ProcessInput(UtInput& aInput)
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
double SphereSurface::ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const
{
   return UtMath::cPI * mRadius * mRadius;
}

// =================================================================================================
double SphereSurface::Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                              double         aIntensity,
                              const UtVec3d& aUnit_i,
                              double         aDist_i,
                              const UtVec3d& aUnit_v,
                              unsigned int   aDebugLevel) const
{
   // Compute the phase angle - the angle between the illuminator and the viewer
   double cosPA = aUnit_i.DotProduct(aUnit_v);
   cosPA        = std::min(std::max(cosPA, -1.0), 1.0);

   // If one considers the sphere to be Lambertian, the resultant reflectance can be derived by considering it as a
   // Lambertian disc oriented normal to the illumination and applying a BRDF to account for the angle between the
   // viewer and the illuminator.
   double angle = acos(cosPA);
   double BRDF  = LambertianSphereBRDF(angle);

   // Now compute the radiant intensity of the reflection. (See the simplification in BoxSurface). The illumination
   // is always considered normal to the sphere so the cosine term is not needed.
   double E = aIntensity / (aDist_i * aDist_i);
   double L = E * BRDF;
   L *= mReflectance; // TODO_JAJ should this be band-specific?

   // The radiant intensity is the radiance times the projected area in the direction of the viewing.
   // The projected area of a sphere in any direction is a constant...
   double I_ref = L * UtMath::cPI * mRadius * mRadius;
   return I_ref;
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
WsfSpaceOpticalSignature::Surface* WsfSpaceOpticalSignature::SurfaceFactory(const std::string& aType,
                                                                            const Surface&     aDefaultData)
{
   Surface* surfacePtr = nullptr;
   if (aType == "box")
   {
      surfacePtr = new BoxSurface(aDefaultData);
   }
   else if (aType == "plane")
   {
      surfacePtr = new PlaneSurface(aDefaultData);
   }
   else if (aType == "sphere")
   {
      surfacePtr = new SphereSurface(aDefaultData);
   }
   return surfacePtr;
}

// =================================================================================================
// The following was the code used to generate the Lambertian Sphere BRDFs.
// =================================================================================================

#if 0

double LambertianSurface(double         aIrradiance,     // W/m^2
                         double         aArea,           // m^2
                         const UtVec3d& aUnit_n,         // Unit vector normal to the area
                         const UtVec3d& aUnit_i,         // Unit vector from the surface to the illuminator
                         const UtVec3d& aUnit_v)         // Unit vector from the surface to the viewer
{
   double I_ref = 0.0;
   double cos_ni = aUnit_n.DotProduct(aUnit_i);          // Cosine of angle between surface normal and illumination
   if (cos_ni > 0.0)                                     // Surface is illuminated
   {
      double cos_nv = aUnit_n.DotProduct(aUnit_v);       // Cosine of angle between surface normal and viewer
      if (cos_nv > 0.0)                                  // Surface is visible
      {
         double E = aIrradiance * cos_ni;                // Decrease the incoming irradiance by the cosine of the incidence angle
         double BRDF = 1.0 / UtMath::cPI;                // BRDF is a constant for a Lambertian surface
         double L = E * BRDF;                            // So therefore, is the radiance

         // The radiant intensity is the radiance times the projected area in the direction of the observer.
         I_ref = L * aArea * cos_nv;
      }
   }
   return I_ref;
}

double LambertianDisc(double         aIrradiance,        // W/m2
                      double         aRadius,            // m^2
                      const UtVec3d& aUnit_n,            // Unit vector normal to the area
                      const UtVec3d& aUnit_i,            // Unit vector from the surface to the illuminator
                      const UtVec3d& aUnit_v)            // Unit vector from the surface to the viewer
{
   double area = UtMath::cPI * aRadius * aRadius;
   return LambertianSurface(aIrradiance, area, aUnit_n, aUnit_i, aUnit_v);
}

// Compute the reflectance of a Lambertian sphere...
double LambertianSphere(double         aIrradiance,      // W/m^2
                        double         aRadius,          // m
                        const UtVec3d& aUnit_i,          // Unit vector from the surface to the illuminator
                        const UtVec3d& aUnit_v)          // Unit vector from the surface to the viewer
{
   // Phi is the angle from the North pole [0..pi]
   // Theta goes around the sphere horizontally [-pi..pi], with theta=0 pointing to the source of the illumination.
   //
   // A single facet in the surface occupies 1 degree by 1 degree

   const double cANGLE_STEP = 1.0;
   double I_ref_sum = 0.0;
   double thetaDeg = 0.0;
   while (thetaDeg < 180.0)
   {
      // Vertical midpoint angle of the facet
      double theta = (thetaDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
      double facetHeight = aRadius * (cANGLE_STEP * UtMath::cRAD_PER_DEG);              // height of the facet
      double facetWidth  = aRadius * (cANGLE_STEP * UtMath::cRAD_PER_DEG) * sin(theta); // width of the facet
      double facetArea   = facetHeight * facetWidth;
      double phiDeg = -180.0;
      while (phiDeg < 180.0)
      {
         // Horizontal midpoint of the facet
         double phi = (phiDeg + 0.5 * cANGLE_STEP) * UtMath::cRAD_PER_DEG;
         double z = cos(theta);
         double xy = sin(theta);
         double x = xy * cos(phi);
         double y = xy * sin(phi);
         UtVec3d unit_n(x, y, z);                             // Normal to the facet
         I_ref_sum += LambertianSurface(aIrradiance, facetArea, unit_n, aUnit_i, aUnit_v);
         phiDeg += cANGLE_STEP;
      }
      thetaDeg += cANGLE_STEP;
   }
   return I_ref_sum;
}

void GenerateLambertianSphereBRDF()
{
   ofstream ofs("lambertian_sphere_brdf.txt");
   ofs << fixed << setprecision(6);

   // An irradiance of 1 w/m2 and an area of 1 m2 will result the returned intensity to have the same value as the BRDF.
   double irradiance = 1.0;
   double radius = sqrt(1.0 / UtMath::cPI);
   UtVec3d unit_i(1.0, 0.0, 0.0);                        // Unit vector to illuminator
   ofs << "      ";
   for (int angleDeg = 0; angleDeg < 180; ++angleDeg)
   {
      double angle = angleDeg * UtMath::cRAD_PER_DEG;
      UtVec3d unit_v(cos(angle), sin(angle), 0.0);       // Unit vector to viewer
      double I_ref_sphere = LambertianSphere(irradiance, radius, unit_i, unit_v);
      ofs << I_ref_sphere << ", ";
      if ((angleDeg + 1) % 10 == 0)
      {
         ofs << "    // " << angleDeg - 9 << "-" << angleDeg << " deg\n      ";
      }
   }
}

int main(int argc, char* argv[])
{
   GenerateLambertianSphereBRDF();
}

#endif
