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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfNavigationErrors.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
//! Component factory to process platform input.
class NavigationErrorsComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      bool myCommand(false);
      if (aInput.GetCommand() == "navigation_errors")
      {
         myCommand                        = true;
         WsfNavigationErrors* navErrorPtr = WsfNavigationErrors::Find(aPlatform);
         if (navErrorPtr == nullptr)
         {
            navErrorPtr = new WsfNavigationErrors;
            aPlatform.AddComponent(navErrorPtr); // Shouldn't fail because Find didn't return one.
         }
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            if (!navErrorPtr->ProcessInput(aInput))
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      return myCommand;
   }
};
} // namespace

// =================================================================================================
// static
void WsfNavigationErrors::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<NavigationErrorsComponentFactory>());
}

// =================================================================================================
// static
WsfNavigationErrors* WsfNavigationErrors::Find(const WsfPlatform& aPlatform)
{
   return static_cast<WsfNavigationErrors*>(aPlatform.FindComponentByRole(cWSF_COMPONENT_NAVIGATION_ERRORS));
}

// =================================================================================================
WsfNavigationErrors::WsfNavigationErrors()
   : WsfSimplePlatformComponent(cWSF_COMPONENT_NAVIGATION_ERRORS, "#navigation_errors")
   , mPlatformPtr(nullptr)
   , mGPS_Status(cGPS_PERFECT)
   , mGPS_StatusVar(static_cast<int>(cGPS_PERFECT))
   , mGPS_InTrackError(0.0)
   , mGPS_CrossTrackError(0.0)
   , mGPS_VerticalError(0.0)
   , mGPS_DegradedMultiplier(1.0)
   , mINS_GyroscopeBiasError(0.0)
   , mINS_AccelerometerBiasError(0.0)
   , mINS_RandomWalkError(0.0)
   , mINS_VerticalError(0.0)
   , mLastUpdateTime(-1.0)
   , mINS_StartTime(-1.0)
   , mRandomnessEnabled(true)
   , mShowStatusChanges(false)
   , mPerceivedEntityValid(false)
   , mINS_ErrorsInitialized(false)
   , mPerceivedEntity()
   , mTruthEntity()
   , mRandom()
   , mTimeHistoryPath()
   , mTimeHistoryOfs()
{
   for (size_t ixyz = 0; ixyz < 3; ++ixyz)
   {
      mINS_LocError[ixyz] = Polynomial();
      mINS_VelError[ixyz] = Polynomial();
      mINS_AclError[ixyz] = Polynomial();
   }
   UtVec3d::Set(mINS_ScaleFactor, 1.0);
}

// =================================================================================================
WsfNavigationErrors::WsfNavigationErrors(const WsfNavigationErrors& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , mPlatformPtr(nullptr)
   , mGPS_Status(aSrc.mGPS_Status)
   , mGPS_StatusVar(aSrc.mGPS_StatusVar)
   , mGPS_InTrackError(aSrc.mGPS_InTrackError)
   , mGPS_CrossTrackError(aSrc.mGPS_CrossTrackError)
   , mGPS_VerticalError(aSrc.mGPS_VerticalError)
   , mGPS_DegradedMultiplier(aSrc.mGPS_DegradedMultiplier)
   , mINS_GyroscopeBiasError(aSrc.mINS_GyroscopeBiasError)
   , mINS_AccelerometerBiasError(aSrc.mINS_AccelerometerBiasError)
   , mINS_RandomWalkError(aSrc.mINS_RandomWalkError)
   , mINS_VerticalError(aSrc.mINS_VerticalError)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mINS_StartTime(aSrc.mINS_StartTime)
   , mRandomnessEnabled(aSrc.mRandomnessEnabled)
   , mShowStatusChanges(aSrc.mShowStatusChanges)
   , mPerceivedEntityValid(aSrc.mPerceivedEntityValid)
   , mINS_ErrorsInitialized(aSrc.mINS_ErrorsInitialized)
   , mPerceivedEntity(aSrc.mPerceivedEntity)
   , mTruthEntity(aSrc.mTruthEntity)
   , mRandom(aSrc.mRandom)
   , mTimeHistoryPath(aSrc.mTimeHistoryPath)
   , mTimeHistoryOfs()
{
   for (size_t ixyz = 0; ixyz < 3; ++ixyz)
   {
      mINS_LocError[ixyz] = aSrc.mINS_LocError[ixyz];
      mINS_VelError[ixyz] = aSrc.mINS_VelError[ixyz];
      mINS_AclError[ixyz] = aSrc.mINS_AclError[ixyz];
   }
   UtVec3d::Set(mINS_ScaleFactor, aSrc.mINS_ScaleFactor);
}

WsfNavigationErrors& WsfNavigationErrors::operator=(const WsfNavigationErrors& aRhs)
{
   if (this != &aRhs)
   {
      WsfSimplePlatformComponent::operator=(aRhs);
      mPlatformPtr                        = nullptr;
      mGPS_Status                         = aRhs.mGPS_Status;
      mGPS_StatusVar                      = aRhs.mGPS_StatusVar;
      mGPS_InTrackError                   = aRhs.mGPS_InTrackError;
      mGPS_CrossTrackError                = aRhs.mGPS_CrossTrackError;
      mGPS_VerticalError                  = aRhs.mGPS_VerticalError;
      mGPS_DegradedMultiplier             = aRhs.mGPS_DegradedMultiplier;
      mINS_GyroscopeBiasError             = aRhs.mINS_GyroscopeBiasError;
      mINS_AccelerometerBiasError         = aRhs.mINS_AccelerometerBiasError;
      mINS_RandomWalkError                = aRhs.mINS_RandomWalkError;
      mINS_VerticalError                  = aRhs.mINS_VerticalError;
      mLastUpdateTime                     = aRhs.mLastUpdateTime;
      mINS_StartTime                      = aRhs.mINS_StartTime;
      mRandomnessEnabled                  = aRhs.mRandomnessEnabled;
      mShowStatusChanges                  = aRhs.mShowStatusChanges;
      mPerceivedEntityValid               = aRhs.mPerceivedEntityValid;
      mINS_ErrorsInitialized              = aRhs.mINS_ErrorsInitialized;
      mPerceivedEntity                    = aRhs.mPerceivedEntity;
      mTruthEntity                        = aRhs.mTruthEntity;
      mRandom                             = aRhs.mRandom;
      mTimeHistoryPath                    = aRhs.mTimeHistoryPath;
      mTimeHistoryOfs.clear();


      for (size_t ixyz = 0; ixyz < 3; ++ixyz)
      {
         mINS_LocError[ixyz] = aRhs.mINS_LocError[ixyz];
         mINS_VelError[ixyz] = aRhs.mINS_VelError[ixyz];
         mINS_AclError[ixyz] = aRhs.mINS_AclError[ixyz];
      }
      UtVec3d::Set(mINS_ScaleFactor, aRhs.mINS_ScaleFactor);
   }
   return *this;
}

// =================================================================================================
// virtual
WsfNavigationErrors::~WsfNavigationErrors()
{
   if (mPlatformPtr != nullptr)
   {
      mPlatformPtr->DetachObserver(this);

      if ((mGPS_Status == cGPS_INACTIVE) && mShowStatusChanges)
      {
         auto locErrorRSCS = GetLocationErrorRSCS();
         auto out          = ut::log::error() << "WsfNavigationError() Report:";
         out.AddNote() << "T = " << mLastUpdateTime;
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Final INS location error: [ " << locErrorRSCS[0] << ' ' << locErrorRSCS[1] << ' '
                       << locErrorRSCS[2] << " ]";
         out.AddNote() << "Error Magnitude: " << locErrorRSCS.Magnitude();
         out.AddNote() << "Elapsed INS time: " << mLastUpdateTime - mINS_StartTime;
      }
   }
}

// =================================================================================================
bool WsfNavigationErrors::Initialize(double aSimTime)
{
   bool ok = true;
   mPlatformPtr->AttachObserver(this); // Attach the OnPlatformUpdated observer

   // Initialize the random number stream.  Each instance has a different seed.
   unsigned int seed = GetSimulation()->GetRandom().Uniform<unsigned>();
   mRandom.SetSeed(seed);

   // Initialize variables than can reference script variables.
   WsfScriptContext& globalContext = GetSimulation()->GetScriptContext();
   WsfScriptContext* contextPtr    = &mPlatformPtr->GetScriptContext();
   ok &= mGPS_StatusVar.Initialize("gps_status", this, contextPtr, globalContext);
   ok &= mGPS_InTrackError.Initialize("gps_in_track_error", this, contextPtr, globalContext);
   ok &= mGPS_CrossTrackError.Initialize("gps_cross_track_error", this, contextPtr, globalContext);
   ok &= mGPS_VerticalError.Initialize("gps_vertical_error", this, contextPtr, globalContext);
   ok &= mGPS_DegradedMultiplier.Initialize("gps_degraded_multiplier", this, contextPtr, globalContext);
   ok &= mINS_GyroscopeBiasError.Initialize("ins_gyroscope_bias_error", this, contextPtr, globalContext);
   ok &= mINS_AccelerometerBiasError.Initialize("ins_accelerometer_bias_error", this, contextPtr, globalContext);
   ok &= mINS_RandomWalkError.Initialize("ins_random_walk_error", this, contextPtr, globalContext);
   ok &= mINS_VerticalError.Initialize("ins_vertical_error", this, contextPtr, globalContext);

   mGPS_Status = GetGPS_Status(mGPS_StatusVar);
   return ok;
}

// =================================================================================================
bool WsfNavigationErrors::Initialize2(double aSimTime)
{
   mLastUpdateTime = aSimTime;

   if (!mTimeHistoryPath.empty())
   {
      std::string fileName(mTimeHistoryPath + '/' + mPlatformPtr->GetName() + ".neh");
      mTimeHistoryOfs.open(fileName);
      if (mTimeHistoryOfs.is_open())
      {
         mTimeHistoryOfs << "#--time-- stat -----lat----- -----lon------ ----alt--- --hdg--"
                         << " -it-error-- -xt-error-- --v-error-- ----rss----\n";
      }
      if (!mTimeHistoryOfs.is_open())
      {
         auto out = ut::log::warning() << "Unable to open navigation time history file for platform.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "File: " << fileName;
      }
   }

   InitializeGPS_Status(aSimTime);

   return true;
}

// =================================================================================================
bool WsfNavigationErrors::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "gps_in_track_error")
   {
      mGPS_InTrackError.ReadValueOfType(aInput, UtInput::cLENGTH);
      mGPS_InTrackError.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "gps_cross_track_error")
   {
      mGPS_CrossTrackError.ReadValueOfType(aInput, UtInput::cLENGTH);
      mGPS_CrossTrackError.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "gps_vertical_error")
   {
      mGPS_VerticalError.ReadValueOfType(aInput, UtInput::cLENGTH);
      mGPS_VerticalError.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "gps_degraded_multiplier")
   {
      double GPS_DegradedMultiplier = 1.0;
      aInput.ReadValue(GPS_DegradedMultiplier);
      mGPS_DegradedMultiplier = GPS_DegradedMultiplier;
      mGPS_VerticalError.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "ins_x_errors")
   {
      ReadPolynomial(aInput, mINS_LocError[0]);
      // This supersedes ins_accelerometer_bias_error, ins_gyroscope_error and ins_random_walk_error
      mINS_AccelerometerBiasError = 0.0;
      mINS_GyroscopeBiasError     = 0.0;
      mINS_RandomWalkError        = 0.0;
   }
   else if (command == "ins_y_errors")
   {
      ReadPolynomial(aInput, mINS_LocError[1]);
      // This supersedes ins_accelerometer_bias_error, ins_gyroscope_error and ins_random_walk_error
      mINS_AccelerometerBiasError = 0.0;
      mINS_GyroscopeBiasError     = 0.0;
      mINS_RandomWalkError        = 0.0;
   }
   else if (command == "ins_z_errors")
   {
      ReadPolynomial(aInput, mINS_LocError[2]);
      // This supersedes the ins_vertical_error
      mINS_VerticalError = 0.0;
   }
   else if (command == "ins_xy_errors")
   {
      ReadPolynomial(aInput, mINS_LocError[0]);
      mINS_LocError[1] = mINS_LocError[0];
      // This supersedes ins_accelerometer_bias_error, ins_gyroscope_error and ins_random_walk_error
      mINS_AccelerometerBiasError = 0.0;
      mINS_GyroscopeBiasError     = 0.0;
      mINS_RandomWalkError        = 0.0;
   }
   else if (command == "ins_accelerometer_bias_error")
   {
      mINS_AccelerometerBiasError.ReadValueOfType(aInput, UtInput::cACCELERATION);
      mINS_AccelerometerBiasError.ValueGreaterOrEqual(aInput, 0.0);
      // This supersedes the ins_x_error and ins_y_error
      mINS_LocError[0].clear();
      mINS_LocError[1].clear();
   }
   else if (command == "ins_gyroscope_bias_error")
   {
      mINS_GyroscopeBiasError.ReadValueOfType(aInput, UtInput::cANGULAR_RATE);
      mINS_GyroscopeBiasError.ValueGreaterOrEqual(aInput, 0.0);
      // This supersedes the ins_x_error and ins_y_error
      mINS_LocError[0].clear();
      mINS_LocError[1].clear();
   }
   else if (command == "ins_random_walk_error")
   {
      std::string dummy;
      aInput.ReadValue(dummy);
      aInput.PushBack(dummy);
      if (dummy == "/variable")
      {
         mINS_RandomWalkError.ReadValue(aInput);
      }
      else
      {
         double      value;
         std::string units;
         aInput.ReadValue(value);
         aInput.ReadValue(units);
         aInput.StringEqual(units, "deg/sqrt-hr");
         aInput.ValueGreaterOrEqual(value, 0.0);
         mINS_RandomWalkError = value * UtMath::cRAD_PER_DEG / 60.0;
      }
      // This supersedes the ins_x_error and ins_y_error
      mINS_LocError[0].clear();
      mINS_LocError[1].clear();
   }
   else if (command == "ins_vertical_error")
   {
      mINS_VerticalError.ReadValueOfType(aInput, UtInput::cLENGTH);
      mINS_VerticalError.ValueGreaterOrEqual(aInput, 0.0);
      // This supersedes the ins_z_error
      mINS_LocError[2].clear();
   }
   else if (command == "gps_status")
   {
      mGPS_StatusVar.ReadValue(aInput);
   }
   else if (command == "randomness")
   {
      aInput.ReadValue(mRandomnessEnabled);
   }
   else if (command == "ins_scale_factors")
   {
      aInput.ReadValue(mINS_ScaleFactor[0]);
      aInput.ReadValue(mINS_ScaleFactor[1]);
      aInput.ReadValue(mINS_ScaleFactor[2]);
   }
   else if (command == "show_status_changes")
   {
      aInput.ReadValue(mShowStatusChanges);
   }
   else if (command == "time_history_path")
   {
      aInput.ReadValue(mTimeHistoryPath);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Callback from platform when the mover is updated.
void WsfNavigationErrors::OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr)
{
   Update(aSimTime);
}

// =================================================================================================
void WsfNavigationErrors::Update(double aSimTime)
{
   if (aSimTime <= mLastUpdateTime)
   {
      return;
   }

   if (mGPS_Status != cGPS_PERFECT)
   {
      if (mGPS_Status != cGPS_INACTIVE)
      {
         GPS_Update(aSimTime);
      }
      else // INS active,  no GPS
      {
         INS_Update(aSimTime);
      }
   }
   WriteTimeHistory(aSimTime);
   mLastUpdateTime = aSimTime;
}

// =================================================================================================
//! Return difference between where the platform thinks it is located and where it is actual located.
//! @return The WCS error vector (P_perceived - P_true).
ut::coords::WCS WsfNavigationErrors::GetLocationErrorWCS() const
{
   ut::coords::WCS locationErrorWCS;
   if (mGPS_Status != cGPS_PERFECT)
   {
      if (mGPS_Status != cGPS_EXTERNAL)
      {
         double trueLocWCS[3];
         mPlatformPtr->GetLocationWCS(trueLocWCS);
         double perceivedLocWCS[3];
         mPerceivedEntity.GetLocationWCS(perceivedLocWCS);
         locationErrorWCS.Subtract(perceivedLocWCS, trueLocWCS);
      }
      else
      {
         locationErrorWCS = mExternalLocationErrorWCS;
      }
   }
   return locationErrorWCS;
}

// =================================================================================================
//! Set the location error from an external source.  This will only affect the parent platform
//! if the GPS_Status (mGPS_Status) is set to GPS_Status::cGPS_EXTERNAL.
//! @param aLocationErrorWCS The location error in WCS coordinates to be applied.
void WsfNavigationErrors::SetLocationErrorWCS(const ut::coords::WCS& aLocationErrorWCS)
{
   if (mGPS_Status == cGPS_EXTERNAL)
   {
      mExternalLocationErrorWCS = aLocationErrorWCS;
   }
}

// =================================================================================================
//! Return the difference between where the platform thinks it is located and where it is actually
//! located, as a vector in the Roll Stabilized Coordinate System (RSCS).
//! @return The RSCS error vector (P_perceived - P_true).
ut::coords::RSCS WsfNavigationErrors::GetLocationErrorRSCS() const
{
   ut::coords::RSCS locationErrorRSCS;
   if (mGPS_Status != cGPS_PERFECT)
   {
      auto locErrorWCS  = GetLocationErrorWCS();
      auto locErrorNED  = mPlatformPtr->ConvertWCSVectorToNED(locErrorWCS);
      locationErrorRSCS = ConvertNED_ToRSCS(mPlatformPtr, locErrorNED);
   }
   return locationErrorRSCS;
}

// =================================================================================================
//! Get the perceived latitude, longitude and altitude of the platform as of the last update.
void WsfNavigationErrors::GetPerceivedLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   if (mGPS_Status == cGPS_PERFECT)
   {
      mPlatformPtr->GetLocationLLA(aLat, aLon, aAlt);
   }
   else
   {
      mPerceivedEntity.GetLocationLLA(aLat, aLon, aAlt);
   }
}

// =================================================================================================
//! Get the perceived WCS location of the platform as of the last update.
ut::coords::WCS WsfNavigationErrors::GetPerceivedLocationWCS() const
{
   if (mGPS_Status == cGPS_PERFECT)
   {
      return mPlatformPtr->GetLocationWCS();
   }
   else
   {
      return mPerceivedEntity.GetLocationWCS();
   }
}

// =================================================================================================
//! Hand-off of navigation error status from one platform to a another platform.
void WsfNavigationErrors::Handoff(double aSimTime, WsfNavigationErrors* aSourcePtr)
{
   mPerceivedEntityValid = false;
   if ((aSourcePtr != nullptr) && aSourcePtr->mPerceivedEntityValid)
   {
      mPerceivedEntityValid = true;
      mPerceivedEntity      = aSourcePtr->mPerceivedEntity;
   }
}

// =================================================================================================
//! Set the GPS status
//! @param aSimTime The current simulation time
//! @param aGPS_Status The new GPS status.
//! @returns 'true' if the status was changed. The return value is 'false' if the requested
//! status was the same as the current status.
bool WsfNavigationErrors::SetGPS_Status(double aSimTime, GPS_Status aGPS_Status)
{
   bool statusChanged = false;
   if (aGPS_Status != mGPS_Status)
   {
      statusChanged     = true;
      int oldGPS_Status = mGPS_Status;
      mGPS_Status       = aGPS_Status;
      if (mLastUpdateTime >= 0.0) // Initialization is complete
      {
         InitializeGPS_Status(aSimTime);
         mLastUpdateTime = aSimTime;
         WsfObserver::NavigationStatusChanged(GetSimulation())(aSimTime, mPlatformPtr, StatusString(mGPS_Status));
         if (mShowStatusChanges)
         {
            auto out = ut::log::info() << "Navigation status changed.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            out.AddNote() << "Old Status: " << StatusString(oldGPS_Status);
            out.AddNote() << "New Status: " << StatusString(mGPS_Status);
         }
         WriteTimeHistory(aSimTime);
      }
   }
   return statusChanged;
}

// protected
void WsfNavigationErrors::InitializeGPS_Status(double aSimTime)
{
   if (mGPS_Status == cGPS_PERFECT)
   {
      // No errors.
      mINS_StartTime        = -1.0;
      mPerceivedEntityValid = false;
   }
   else if (mGPS_Status != cGPS_INACTIVE)
   {
      // GPS active with GPS error model.
      GPS_Update(aSimTime);
      mINS_StartTime = -1.0;
   }
   else
   {
      // INS active, no GPS.
      // If this is the first time INS has been used, initialize the INS equations.
      if (!mINS_ErrorsInitialized)
      {
         INS_Initialize(aSimTime);
      }
      // If not currently using INS, set the baseline for computing INS errors.
      if (mINS_StartTime < 0.0)
      {
         INS_Start(aSimTime);
      }
      INS_Update(aSimTime);
   }
}

// static
//! This method is used to copy the value of the GPS_StatusVar variable, used for ProcessInput and referencing a
//! script variable if it exists, to the GPS_Status variable, and to honor the legacy interface in the script method.
//! @param aStatus The (legacy) integer status, defined to be cINACTIVE if aStatus < 0, cGPS_PERFECT if aStatus == 0,
//! cGPS_DEGRADED if aStatus == 2, and cGPS_ACTIVE otherwise.
//! @return the corresponding WsfNavigationErrors::GPS_Status.
WsfNavigationErrors::GPS_Status WsfNavigationErrors::GetGPS_Status(int aStatus)
{
   GPS_Status status;
   if (aStatus < 0)
   {
      status = cGPS_INACTIVE;
   }
   else if (aStatus == 0)
   {
      status = cGPS_PERFECT;
   }
   else if (aStatus == 2)
   {
      status = cGPS_DEGRADED;
   }
   else if (aStatus == 3)
   {
      status = cGPS_EXTERNAL;
   }
   else
   {
      status = cGPS_ACTIVE;
   }
   return status;
}

// =================================================================================================
//! Convert an error vector in the NED frame to the RSCS frame.
//!
//! This transforms a error vector in the NED frame to local RSCS frame (X axis aligned with the
//! heading vector, and the XY plane tangent to the Earth's surface).
// protected
ut::coords::RSCS WsfNavigationErrors::ConvertNED_ToRSCS(UtEntity* aEntityPtr, const ut::coords::NED& aLocNED) const
{
   double heading;
   double pitch;
   double roll;
   aEntityPtr->GetOrientationNED(heading, pitch, roll);
   double           sinHeading = sin(heading);
   double           cosHeading = cos(heading);
   ut::coords::RSCS locRSCS(aLocNED[0] * cosHeading + aLocNED[1] * sinHeading,
                           -aLocNED[0] * sinHeading + aLocNED[1] * cosHeading,
                            aLocNED[2]);
   return locRSCS;
}

// =================================================================================================
//! Convert an RSCS error vector to an NED coordinate.
//!
//! This transforms a error vector in the local RSCS frame (X axis aligned with the heading vector,
//! and the XY plane tangent to the Earth's surface) into a coordinate in the standard NED frame.
// protected
ut::coords::NED WsfNavigationErrors::ConvertRSCS_ToNED(UtEntity* aEntityPtr, const ut::coords::RSCS& aLocRSCS) const
{
   double heading;
   double pitch;
   double roll;
   aEntityPtr->GetOrientationNED(heading, pitch, roll);
   double          sinHeading = sin(heading);
   double          cosHeading = cos(heading);
   ut::coords::NED locNED(aLocRSCS[0] * cosHeading - aLocRSCS[1] * sinHeading,
                          aLocRSCS[0] * sinHeading + aLocRSCS[1] * cosHeading,
                          aLocRSCS[2]);
   return locNED;
}

// =================================================================================================
// protected
double WsfNavigationErrors::GlobalGaussian()
{
   return (mRandomnessEnabled) ? GetSimulation()->GetRandom().Gaussian() : 1.0;
}

// =================================================================================================
// protected
double WsfNavigationErrors::LocalGaussian()
{
   return (mRandomnessEnabled) ? mRandom.Gaussian() : 1.0;
}

// =================================================================================================
//! Compress a polynomial to remove terms with a zero coefficient.
// protected
void WsfNavigationErrors::CompressPolynomial(Polynomial& aTerms)
{
   Polynomial output;
   for (auto& term : aTerms)
   {
      if (term.mCoefficient != 0.0)
      {
         output.push_back(term);
      }
   }

   if (output.size() != aTerms.size())
   {
      aTerms = output;
   }
}

// =================================================================================================
//! Evaluate a polynomial for at the specified point.
//! This assumes terms with zero coefficients have been removed.
// protected
double WsfNavigationErrors::EvaluatePolynomial(double aT, const Polynomial& aTerms) const
{
   double value = 0.0;
   double t     = aT;
   double t2    = t * t;
   for (const auto& term : aTerms)
   {
      double coef = term.mCoefficient;
      switch (term.mIntExponent)
      {
      case 0:
         value += coef;
         break;
      case 1:
         value += coef * t;
         break;
      case 2:
         value += coef * t2;
         break;
      case 3:
         value += coef * t * t2;
         break;
      case 4:
         value += coef * t2 * t2;
         break;
      case 5:
         value += coef * t * t2 * t2;
         break;
      default:
         value += coef * pow(t, term.mExponent);
         break;
      }
   }
   return value;
}

// =================================================================================================
// protected
void WsfNavigationErrors::ReadPolynomial(UtInput& aInput, Polynomial& aTerms)
{
   aTerms.clear();

   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "coefficient")
      {
         double coefficient;
         double exponent = 0.0;
         aInput.ReadValue(coefficient);
         aInput.ReadValue(command);
         if (command == "exponent")
         {
            aInput.ReadValue(exponent);
            aInput.ValueGreaterOrEqual(exponent, 0.0);
         }
         else
         {
            aInput.PushBack(command);
         }
         aTerms.emplace_back(coefficient, exponent);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
// protected
std::string WsfNavigationErrors::StatusString(int aStatus) const
{
   std::string statusString("PERFECT");
   if (aStatus < 0)
   {
      statusString = "INS" + UtStringUtil::ToString(-aStatus);
   }
   else if (aStatus > 0)
   {
      statusString = "GPS" + UtStringUtil::ToString(aStatus);
   }
   return statusString;
}

// =================================================================================================
// protected
void WsfNavigationErrors::GPS_Update(double aSimTime)
{
   ut::coords::WCS perceivedLocWCS;

   if (mGPS_Status != cGPS_EXTERNAL)
   {
      ut::coords::RSCS locErrorRSCS(LocalGaussian() * mGPS_InTrackError,
                                    LocalGaussian() * mGPS_CrossTrackError,
                                    LocalGaussian() * mGPS_VerticalError);

      if (mGPS_Status == cGPS_DEGRADED)
      {
         locErrorRSCS[0] *= mGPS_DegradedMultiplier;
         locErrorRSCS[1] *= mGPS_DegradedMultiplier;
         locErrorRSCS[2] *= mGPS_DegradedMultiplier;
      }

      // Convert the error from local RSCS frame to the NED frame.
      auto locErrorNED = ConvertRSCS_ToNED(mPlatformPtr, locErrorRSCS);

      // Update the perceived location.
      perceivedLocWCS = mPlatformPtr->ConvertNEDToWCS(locErrorNED);
   }
   else // mStatus == cGPS_External
   {
      perceivedLocWCS = mPlatformPtr->GetLocationWCS() + mExternalLocationErrorWCS;
   }

   mPerceivedEntity.SetLocationWCS(perceivedLocWCS);
   mPerceivedEntityValid = true;
}

// =================================================================================================
//! Initialize the INS error equations.
//!
//! This is called the FIRST time INS is enabled for a given platform. It initializes the error
//! equation coefficients based on input parameters and random draws.
//!
//! @param aSimTime The current simulation time.
// protected
void WsfNavigationErrors::INS_Initialize(double aSimTime)
{
   mINS_ErrorsInitialized = true;

   // If the user specified a simple ins_vertical_error, convert it to the ins_z_error polynomial

   if (mINS_VerticalError != 0.0)
   {
      mINS_LocError[2].clear();
      mINS_LocError[2].emplace_back(mINS_VerticalError, 0.0);
   }

   // If the user specified the simple INS/IMU errors, convert the ins_x/y/z_error polynomial.

   if ((mINS_GyroscopeBiasError != 0.0) || (mINS_AccelerometerBiasError != 0.0) || (mINS_RandomWalkError != 0.0))
   {
      // Errors defined by fixed IMU parameters.

      double gyroscopeBias     = (1.0 / 6.0) * mINS_GyroscopeBiasError * UtEarth::cACCEL_OF_GRAVITY;
      double accelerometerBias = (1.0 / 2.0) * mINS_AccelerometerBiasError;
      double randomWalkBias    = (1.0 / sqrt(20.0)) * mINS_RandomWalkError * UtEarth::cACCEL_OF_GRAVITY;

      // If the user defined a z error polynomial then don't override it with the IMU parameters.

      size_t xyzCount = mINS_LocError[2].empty() ? 3U : 2U;

      for (size_t ixyz = 0; ixyz < xyzCount; ++ixyz)
      {
         mINS_LocError[ixyz].clear();
         mINS_LocError[ixyz].emplace_back(gyroscopeBias * GlobalGaussian() * mINS_ScaleFactor[ixyz], 3.0);
         mINS_LocError[ixyz].emplace_back(accelerometerBias * GlobalGaussian() * mINS_ScaleFactor[ixyz], 2.0);
         mINS_LocError[ixyz].emplace_back(randomWalkBias * GlobalGaussian() * mINS_ScaleFactor[ixyz], 2.5);
      }
      if (xyzCount == 2)
      {
         double zDraw = GlobalGaussian() * mINS_ScaleFactor[2];
         for (auto& iterm : mINS_LocError[2])
         {
            iterm.mCoefficient *= zDraw;
         }
      }
   }
   else
   {
      // Use user-defined polynomials.
      for (size_t ixyz = 0; ixyz < 3; ++ixyz)
      {
         double scale = GlobalGaussian() * mINS_ScaleFactor[ixyz];
         for (auto& iterm : mINS_LocError[ixyz])
         {
            iterm.mCoefficient *= scale;
         }
      }
   }

   // Compress the position error polynomial (squeeze out zero terms).

   CompressPolynomial(mINS_LocError[0]);
   CompressPolynomial(mINS_LocError[1]);
   CompressPolynomial(mINS_LocError[2]);

   // Generate the velocity and acceleration error polynomials.
   // (The first and second derivatives of the position error polynomial).

   for (size_t ixyz = 0; ixyz < 3; ++ixyz)
   {
      for (size_t iterm = 0; iterm < mINS_LocError[ixyz].size(); ++iterm)
      {
         double coefficient = mINS_LocError[ixyz][iterm].mCoefficient;
         double exponent    = mINS_LocError[ixyz][iterm].mExponent;
         if (exponent >= 1.0)
         {
            coefficient *= exponent;
            exponent = exponent - 1.0;
            mINS_VelError[ixyz].emplace_back(coefficient, exponent);
            if (exponent >= 1.0)
            {
               coefficient *= exponent;
               exponent = exponent - 1.0;
               mINS_AclError[ixyz].emplace_back(coefficient, exponent);
            }
         }
      }
      CompressPolynomial(mINS_VelError[ixyz]);
      CompressPolynomial(mINS_AclError[ixyz]);
   }

   if (mShowStatusChanges)
   {
      auto out = ut::log::info() << "INS Equations:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mPlatformPtr->GetName();
      INS_PrintEquations("Position", mINS_LocError, out);
      INS_PrintEquations("Velocity", mINS_VelError, out);
      INS_PrintEquations("Acceleration", mINS_AclError, out);
   }
}

// =================================================================================================
void WsfNavigationErrors::INS_PrintEquations(const std::string&      aString,
                                             const Polynomial        aTerms[3],
                                             ut::log::MessageStream& aStream) const
{
   if ((!aTerms[0].empty()) || (!aTerms[1].empty()) || (!aTerms[2].empty()))
   {
      auto out = aStream.AddNote() << aString << " Equation:";

      for (size_t ixyz = 0; ixyz < 3; ++ixyz)
      {
         auto note = out.AddNote() << "Coordinate " << ixyz + 1 << ":";
         for (size_t iterm = 0; iterm < aTerms[ixyz].size(); ++iterm)
         {
            if (aTerms[ixyz][iterm].mCoefficient != 0.0)
            {
               note << " " << aTerms[ixyz][iterm].mCoefficient;
               if (mINS_LocError[ixyz][iterm].mExponent != 0.0)
               {
                  note << "*t^" << aTerms[ixyz][iterm].mExponent;
               }
            }
         }
      }
   }
}

// =================================================================================================
// protected
void WsfNavigationErrors::INS_Start(double aSimTime)
{
   mINS_StartTime = aSimTime;
   mTruthEntity   = *mPlatformPtr;
   if (!mPerceivedEntityValid)
   {
      mPerceivedEntity = mTruthEntity;
   }

   // Determine the current INS errors. Because the time since the start of the INS segment is zero,
   // the results should be only the constant terms.

   ut::coords::RSCS locErrorRSCS(EvaluatePolynomial(0.0, mINS_LocError[0]),
                                 EvaluatePolynomial(0.0, mINS_LocError[1]),
                                 EvaluatePolynomial(0.0, mINS_LocError[2]));

   // Convert the local RSCS error delta to NED.
   auto locErrorNED = ConvertRSCS_ToNED(&mPerceivedEntity, locErrorRSCS); // Check this...

   // Get the perceived orientation in the local NED frame prior to applying the error to the position.
   // This way it retains the directions relative to new NED frame.

   double velNED[3];
   double aclNED[3];
   double yaw, pitch, roll;
   mPerceivedEntity.GetVelocityNED(velNED);
   mPerceivedEntity.GetAccelerationNED(aclNED);
   mPerceivedEntity.GetOrientationNED(yaw, pitch, roll);

   // Update the perceived entity.
   auto perceivedLocWCS = mPerceivedEntity.ConvertNEDToWCS(locErrorNED);
   mPerceivedEntity.SetLocationWCS(perceivedLocWCS);
   mPerceivedEntity.SetVelocityNED(velNED);
   mPerceivedEntity.SetAccelerationNED(aclNED);
   mPerceivedEntity.SetOrientationNED(yaw, pitch, roll);
   mPerceivedEntityValid = true;
}

// =================================================================================================
// protected
void WsfNavigationErrors::INS_Update(double aSimTime)
{
   double dt = aSimTime - mLastUpdateTime;
   if (dt <= 1.0E-6)
   {
      return;
   }

   // Get the velocity error (Verror) and acceleration error (Aerror) vectors in the local RSCS frame.

   ut::coords::RSCS velErrorRSCS;
   ut::coords::RSCS aclErrorRSCS;
   double           timeSinceINS_Start = aSimTime - mINS_StartTime;
   for (size_t ixyz = 0; ixyz < 3; ++ixyz)
   {
      velErrorRSCS[ixyz] = EvaluatePolynomial(timeSinceINS_Start, mINS_VelError[ixyz]);
      aclErrorRSCS[ixyz] = EvaluatePolynomial(timeSinceINS_Start, mINS_AclError[ixyz]);
   }

   // Convert the error vectors from RSCS to NED

   auto velErrorNED = ConvertRSCS_ToNED(mPlatformPtr, velErrorRSCS);
   auto aclErrorNED = ConvertRSCS_ToNED(mPlatformPtr, aclErrorRSCS);

   // Convert the error vectors from NED to WCS.

   auto velErrorWCS = mPerceivedEntity.ConvertNEDVectorToWCS(velErrorNED);
   auto aclErrorWCS = mPerceivedEntity.ConvertNEDVectorToWCS(aclErrorNED);

   // Compute the change in truth position.

   auto newTruthLocWCS   = mPlatformPtr->GetLocationWCS();
   auto oldTruthLocWCS   = mTruthEntity.GetLocationWCS();
   auto deltaTruthLocWCS = newTruthLocWCS - oldTruthLocWCS;

   // Compute the new perceived position (EADSIM, top of page 5.4-10 in the methods manual)
   //
   // Pp(new) = Pp(old) + deltaP(truth) + Verror * dt + 1/2 * Aerror * dt^2

   auto perceivedLocWCS =
      mPerceivedEntity.GetLocationWCS() + deltaTruthLocWCS + dt * velErrorWCS + 0.5 * dt * dt * aclErrorWCS;

   // Compute the change in truth velocity.

   auto newTruthVelWCS   = mPlatformPtr->GetVelocityWCS();
   auto oldTruthVelWCS   = mTruthEntity.GetVelocityWCS();
   auto deltaTruthVelWCS = newTruthVelWCS - oldTruthVelWCS;

   // Compute the new perceived velocity.
   //
   // Vp(new) = Vp(old) + deltaV(truth) + Aerror * dt
   auto perceivedVelWCS = mPerceivedEntity.GetVelocityWCS() + deltaTruthVelWCS + dt * aclErrorWCS;

   // Compute the new perceived acceleration.
   //
   // Ap(new) = Atruth + Aerror.
   auto perceivedAclWCS = mPlatformPtr->GetAccelerationWCS() + aclErrorWCS;

   // Update the perceived entity state. Note that the truth NED orientation is copied.

   double yaw, pitch, roll;
   mPlatformPtr->GetOrientationNED(yaw, pitch, roll);
   mPerceivedEntity.SetLocationWCS(perceivedLocWCS);
   mPerceivedEntity.SetVelocityWCS(static_cast<UtVec3d>(perceivedVelWCS));
   mPerceivedEntity.SetAccelerationWCS(static_cast<UtVec3d>(perceivedAclWCS));
   mPerceivedEntity.SetOrientationNED(yaw, pitch, roll);
   mTruthEntity = *mPlatformPtr;
}

// =================================================================================================
// protected
void WsfNavigationErrors::WriteTimeHistory(double aSimTime)
{
   if (mTimeHistoryOfs.is_open())
   {
      double lat;
      double lon;
      double alt;
      mPlatformPtr->GetLocationLLA(lat, lon, alt);
      double heading;
      double pitch;
      double roll;
      mPlatformPtr->GetOrientationNED(heading, pitch, roll);
      heading           = UtMath::NormalizeAngle0_360(heading * UtMath::cDEG_PER_RAD);
      auto locErrorRSCS = GetLocationErrorRSCS();
      mTimeHistoryOfs.setf(std::ios_base::fixed, std::ios_base::floatfield);
      // clang-format off
      mTimeHistoryOfs << std::setprecision(4) << std::setw(12) << aSimTime
                      << ' ' << std::setw(4) << StatusString(mGPS_Status)
                      << ' ' << UtLatPos(lat, UtAngle::FmtDMS + 3 + UtAngle::FmtDegIncr * 2)
                      << ' ' << UtLonPos(lon, UtAngle::FmtDMS + 3 + UtAngle::FmtDegIncr * 3)
                      << ' ' << std::setprecision(3) << std::setw(10) << alt
                      << ' ' << std::setprecision(3) << std::setw(7)  << heading
                      << ' ' << std::setprecision(3) << std::setw(11) << locErrorRSCS[0]
                      << ' ' << std::setprecision(3) << std::setw(11) << locErrorRSCS[1]
                      << ' ' << std::setprecision(3) << std::setw(11) << locErrorRSCS[2]
                      << ' ' << std::setprecision(3) << std::setw(11) << locErrorRSCS.Magnitude()
                      << '\n';
      // clang-format on
   }
}

WsfSimulation* WsfNavigationErrors::GetSimulation() const
{
   return mPlatformPtr->GetSimulation();
}

// =================================================================================================
// Deprecated Methods

//! Return difference between where the platform thinks it is located and where it is actual located.
//! @param aLocationErrorWCS The WCS error vector (P_perceived - P_true).
void WsfNavigationErrors::GetLocationErrorWCS(double aLocationErrorWCS[3]) const
{
   auto locationErrorWCS = GetLocationErrorWCS();
   UtVec3d::Set(aLocationErrorWCS, locationErrorWCS.GetData());
}

//! Return difference between where the platform thinks it is located and where it is actual located.
//! @param aLocationErrorXYZ The RSCS error vector (P_perceived - P_true).
void WsfNavigationErrors::GetLocationErrorXYZ(double aLocationErrorXYZ[3]) const
{
   auto locationErrorRSCS = GetLocationErrorRSCS();
   UtVec3d::Set(aLocationErrorXYZ, locationErrorRSCS.GetData());
}

//! Get the perceived WCS location of the platform as of the last update.
void WsfNavigationErrors::GetPerceivedLocationWCS(double aLocationWCS[3]) const
{
   auto perceivedLocationWCS = GetPerceivedLocationWCS();
   UtVec3d::Set(aLocationWCS, perceivedLocationWCS.GetData());
}
