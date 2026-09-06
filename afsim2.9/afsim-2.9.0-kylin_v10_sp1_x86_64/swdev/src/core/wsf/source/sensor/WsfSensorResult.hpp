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

#ifndef WSFSENSORRESULT_HPP
#define WSFSENSORRESULT_HPP

#include "wsf_export.h"

#include "WsfEM_Interaction.hpp"
#include "WsfMeasurement.hpp"
#include "WsfSensor.hpp"

//! Result class is supplied to the AttemptToDetect method.
//! On output it contains detailed data about the detection attempt.
//! Not every sensor will update every member. Each member documents under
//! what conditions it is valid. It is the responsibility of the caller to
//! ensure that the member they are using contains valid data.
class WSF_EXPORT WsfSensorResult : public WsfEM_Interaction
{
public:
   //! Status flag values used in mCheckedStatus and mFailedStatus
   enum
   {
      cCONCEALMENT       = 0x00010000, //!< Concealment checked/failed
      cDOPPLER_LIMITS    = 0x00020000, //!< Doppler limits checked/failed
      cVELOCITY_LIMITS   = 0x00040000, //!< Velocity limits checked/failed
      cTARGET_DELETED    = 0x00080000, //!< Target deleted
      cOTH_LIMITS        = 0x00100000, //!< OTH Bounce Constraints
      cEXCLUSION_SOLAR   = 0x00200000, //!< Solar exclusion is blocking sensor
      cEXCLUSION_LUNAR   = 0x00400000, //!< Lunar exclusion is blocking sensor
      cMOON_BLOCKED      = 0x00800000, //!< Moon is blocking the sensor from target
      cDETECTION_STOP    = 0x01000000, //!< Sensor stops detecting
      cSCRIPT_CONSTRAINT = 0x02000000  //!< Externally defined script check
   };

   WsfSensorResult()           = default;
   ~WsfSensorResult() override = default;

   bool Detected() const;

   //! WsfEM_Interaction interface:
   void PrintStatus(ut::log::MessageStream& aStream) const override;
   void PrintStatusP(ut::log::MessageStream& aStream) const override;

   unsigned int BeginOneWayInteraction(WsfEM_Rcvr* aRcvrPtr, WsfPlatform* aTgtPtr) override;

   unsigned int BeginOneWayInteraction(WsfEM_Xmtr* aXmtrPtr,
                                       WsfEM_Rcvr* aRcvrPtr,
                                       bool        aCheckXmtrLimits,
                                       bool        aCheckRcvrLimits,
                                       bool        aCheckMaskingFactor = true) override;

   unsigned int BeginTwoWayInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr) override;

   unsigned int BeginTwoWayOTH_Interaction(WsfEM_Xmtr*  aXmtrPtr,
                                           WsfPlatform* aTgtPtr,
                                           WsfEM_Rcvr*  aRcvrPtr,
                                           double       aReflectionLocWCS[3]) override;

   unsigned int BeginGenericInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr) override;

   void ComputeUndefinedGeometry() override;

   void Reset() override;
   void Reset(const WsfSensor::Settings& aSettings);
   void Reset(const WsfSensorResult& aResult);

   //! Get the name of the script class associated with this class.
   //! This is necessary for proper downcasts in the scripting language.
   const char* GetScriptClassName() const override;

   //! For multi-mode systems this is the index of the mode within the mode list
   //! that resulted in the best probability of detection.
   //!
   //! This will be zero for single-mode systems.
   size_t mModeIndex{0};

   //! The transmitter index.
   size_t mXmtrIndex{0};

   //! The receiver index.
   size_t mRcvrIndex{0};

   //! For multi-beam systems this is the index of the beam within the beam list
   //! that resulted in the best probability of detection.
   //!
   //! This field will be zero for single-beam systems.
   size_t mBeamIndex{0};

   //! Measured information, which may include any of the following:
   //! <ul>
   //! <li> The reported spherical location (range, az, el) of the detected object, including  measurement errors and
   //! filtering. <li> The standard deviations in range, az, and el of the spherical location of the detected object.
   //! <li> The range rate and range rate error. <li> The reported side Id. <li> The reported type Id. <li> The reported
   //! WCS location. <li> The reported WCS velocity. <li> The transform from WCS to measurement coordinate system.
   //! </ul>
   WsfMeasurement mMeasurement;

   //! The probability of detection.
   //! For a binary detector the value will be 1.0 if the signal exceeds the
   //! detection threshold and 0.0 if it did not.
   //!
   //! @note By itself, this field should NOT be used for determining if the detection
   //! succeeded. Only the boolean return value from AttemptToDetect() should be used
   //! as the determining factor. If this value is computed then it will be non-zero
   //! even if something other later check causes the detection attempt to fail (such
   //! as terrain masking).
   double mPd{0.0};

   //! The probability of detection required for detection.
   //! This field will be zero for a non-probabilistic sensor.
   double mRequiredPd{0.0};

private:
   //! Used to set the transform from beam coordinates to wcs in the Sensor::Result derived class.
   //! Base class version does nothing.
   void ComputeWCS_ToBCS_Transform(const WsfEM_Rcvr& aRcvr, double aRcvrPointingAz, double aRcvrPointingEl);
};

class WSF_EXPORT WsfLocalSensorResult : public WsfSensorResult
{
public:
   WsfLocalSensorResult()           = default;
   ~WsfLocalSensorResult() override = default;

   void Reset() override;

   WsfLocalSensorResult& operator=(const WsfSensorResult& aRhs)
   {
      WsfSensorResult::operator=(aRhs);
      return *this;
   }

   // Number of local measurements correlated/fused in result
   unsigned int mMeasurementCount{0};
};

#include "script/WsfScriptEM_InteractionClass.hpp"

class WSF_EXPORT WsfScriptSensorInteractionClass : public WsfScriptEM_InteractionClass
{
public:
   WsfScriptSensorInteractionClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   static void RegisterMethods(UtScriptClass* aClassPtr);

   UT_DECLARE_SCRIPT_METHOD(Detected);
   UT_DECLARE_SCRIPT_METHOD(ModeName);
   UT_DECLARE_SCRIPT_METHOD(BeamNumber);
   UT_DECLARE_SCRIPT_METHOD(Pd);
   UT_DECLARE_SCRIPT_METHOD(RequiredPd);
};

UT_MAP_CLASS_TO_SCRIPT_NAME(WsfSensorResult, "WsfSensorInteraction")

#endif
