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

#ifndef WSFNAVIGATIONERRORS_HPP
#define WSFNAVIGATIONERRORS_HPP

#include "wsf_export.h"

#include <fstream>
#include <string>
#include <vector>

#include "UtCoords.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "UtLog.hpp"
#include "UtRandom.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfSimpleComponent.hpp"
class WsfSimulation;
#include "WsfSinglePlatformObserver.hpp"
#include "WsfVariable.hpp"

//! A simple navigation error model.
//! This is an optional object that can be instantiated on a platform which is
//! responsible for modeling where the platform *THINKS* it is located. This is
//! used when producing reports from the platform that include the location of
//! the reporting platform.
class WSF_EXPORT WsfNavigationErrors : public WsfSimplePlatformComponent, public WsfSinglePlatformObserver
{
public:
   static void                 RegisterComponentFactory(WsfScenario& aScenario);
   static WsfNavigationErrors* Find(const WsfPlatform& aPlatform);

   WsfNavigationErrors();
   WsfNavigationErrors(const WsfNavigationErrors& aSrc);
   WsfNavigationErrors& operator=(const WsfNavigationErrors& aRhs);
   ~WsfNavigationErrors() override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return new WsfNavigationErrors(*this); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_NAVIGATION_ERRORS; }
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime);
   //@}

   //! @name Overrides of WsfSinglePlatformObserver.
   //@{
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;
   //@}

   //! Get the time when the last call was made.
   double GetLastUpdateTime() const { return mLastUpdateTime; }

   ut::coords::RSCS GetLocationErrorRSCS() const;
   ut::coords::WCS  GetLocationErrorWCS() const;
   void             SetLocationErrorWCS(const ut::coords::WCS& aLocationErrorWCS);

   ut::coords::WCS GetPerceivedLocationWCS() const;
   void            GetPerceivedLocationLLA(double& aLat, double& aLon, double& aAlt) const;

   UtEntity& GetPerceivedEntity() { return mPerceivedEntity; }

   void Handoff(double aSimTime, WsfNavigationErrors* aSourcePtr);

   enum GPS_Status
   {
      cGPS_INACTIVE = -1,
      cGPS_PERFECT  = 0,
      cGPS_ACTIVE   = 1,
      cGPS_DEGRADED = 2,
      cGPS_EXTERNAL = 3
   };

   virtual bool SetGPS_Status(double aSimTime, GPS_Status aStatus);
   GPS_Status   GetGPS_Status() const { return mGPS_Status; }

   static GPS_Status GetGPS_Status(int aStatus);

   WsfSimulation* GetSimulation() const;

   //! A single term in an error polynomial
   class Term
   {
   public:
      Term()
         : mCoefficient(0.0)
         , mExponent(0.0)
         , mIntExponent(-0)
      {
      }

      Term(double aCoefficient, double aExponent)
         : mCoefficient(aCoefficient)
         , mExponent(aExponent)
         , mIntExponent(-1)
      {
         int intExponent = static_cast<int>(mExponent);
         if (static_cast<double>(intExponent) == mExponent)
         {
            mIntExponent = intExponent;
         }
      }
      double mCoefficient;
      double mExponent;
      int    mIntExponent;
   };

   // Deprecated methods
   WSF_DEPRECATED void GetLocationErrorWCS(double aLocationErrorWCS[3]) const;

   WSF_DEPRECATED void GetLocationErrorXYZ(double aLocationErrorXYZ[3]) const;

   WSF_DEPRECATED void GetPerceivedLocationWCS(double aLocationWCS[3]) const;

protected:
   typedef std::vector<Term> Polynomial;

   virtual void InitializeGPS_Status(double aSimTime);

   ut::coords::RSCS ConvertNED_ToRSCS(UtEntity* aEntityPtr, const ut::coords::NED& aLocNED) const;

   ut::coords::NED ConvertRSCS_ToNED(UtEntity* aEntityPtr, const ut::coords::RSCS& aLocRSCS) const;

   double GlobalGaussian();
   double LocalGaussian();

   void          CompressPolynomial(Polynomial& aTerms);
   inline double EvaluatePolynomial(double aT, const Polynomial& aTerms) const;
   void          ReadPolynomial(UtInput& aInput, Polynomial& aTerms);
   std::string   StatusString(int aStatus) const;

   virtual void GPS_Update(double aSimTime);

   void INS_Initialize(double aSimTime);
   void INS_PrintEquations(const std::string& aString, const Polynomial aTerms[3], ut::log::MessageStream& aStream) const;
   void INS_Start(double aSimTime);
   void INS_Update(double aSimTime);

   void WriteTimeHistory(double aSimTime);

   //! Pointer to the platform for which errors are being computed.
   WsfPlatform* mPlatformPtr;

   //! GPS status. -1: no GPS (use INS), 0 = perfect (no errors), 1 : GPS Active; 2 GPS Degraded
   GPS_Status       mGPS_Status;
   WsfVariable<int> mGPS_StatusVar;

   //! Standard deviation of a Gaussian distribution for the 'in-track' errors for GPS.
   WsfVariable<double> mGPS_InTrackError;

   //! Standard deviation of a Gaussian distribution for the 'cross-track' errors for GPS.
   WsfVariable<double> mGPS_CrossTrackError;

   //! Standard deviation of a Gaussian distribution for vertical errors for GPS.
   WsfVariable<double> mGPS_VerticalError;

   //! When GPS_Status is cGPS_Degraded, multiply all errors computed from mGPS_InTrackError,
   //! mGPS_CrossTrackError, and mGPS_VerticalError by this number.
   WsfVariable<double> mGPS_DegradedMultiplier;

   //! Standard deviation of a Gaussian distribution for the INS gyroscope bias error.
   WsfVariable<double> mINS_GyroscopeBiasError;

   //! Standard deviation of a Gaussian distribution for the INS accelerometer bias error.
   WsfVariable<double> mINS_AccelerometerBiasError;

   //! Standard deviation of a Gaussian distribution for the INS angular random walk error.
   WsfVariable<double> mINS_RandomWalkError;

   //! Standard deviation of a Gaussian distribution for vertical error for INS.
   WsfVariable<double> mINS_VerticalError;

   //! @name INS Error polynomial.
   //!
   //! The INS error polynomials are user-defined 10-term polynomials that is a function of time
   //! (since the start of INS-navigation). Separate polynomials can be defined for the horizontal (XY)
   //! and vertical (Z) directions.
   //!
   //! Prior to initialization the user-defined XY terms are stored in the X terms arrays.
   //! During initialization the X terms are copied to the Y terms and then the X/Y/Z arrays
   //! a multiplied by a separate random draw.
   //@{
   Polynomial mINS_LocError[3];
   Polynomial mINS_VelError[3];
   Polynomial mINS_AclError[3];
   //@}

   //! The location error used when mGPS_Status == GPS_Status::cGPS_EXTERNAL.
   ut::coords::WCS mExternalLocationErrorWCS;

   //! A simple scale factor for the INS errors.
   double mINS_ScaleFactor[3];

   //! The time when the GPS error/INS drift direction was last computed.
   //! This is less than zero if initialization has not completed.
   double mLastUpdateTime;

   //! The time when INS mode was initiated.
   double mINS_StartTime;

   //! true if randomness is enabled (default true).
   bool mRandomnessEnabled;

   //! true if status change messages should be written to stdout.
   bool mShowStatusChanges;

   //! True if the perceived entity is valid.
   //! This will be true if GPS/INS is active or a hand-off was received.
   bool mPerceivedEntityValid;

   //! true if INS errors have been initialized.
   bool mINS_ErrorsInitialized;

   //! The entity that represents the perceived state.
   UtEntity mPerceivedEntity;

   //! The truth entity state
   UtEntity mTruthEntity;

   //! Random number stream.
   ut::Random mRandom;

   //! The directory name (without a trailing '/') indicating where time history output
   //! is to be written.
   std::string mTimeHistoryPath;

   //! Stream for writing time history output.
   std::ofstream mTimeHistoryOfs;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfNavigationErrors, cWSF_COMPONENT_NAVIGATION_ERRORS)

#endif
