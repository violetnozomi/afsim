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

#include "WsfSensorResult.hpp"

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"

// =================================================================================================
//! Check to see if the target was detected based on the following:
//! @returns ((mFailedStatus == 0) && (mCheckedStatus != 0) && (mPd >= mRequiredPd))
// virtual
bool WsfSensorResult::Detected() const
{
   // A successful detection (mFailedStatus == 0) is valid only if something was
   // actually checked (mCheckedStatus != 0).
   return ((mCheckedStatus != 0) && (mFailedStatus == 0) && (mPd >= mRequiredPd));
}

// =================================================================================================
//! Print the status data.
//! Called by the base class Print() method and sensor-specific printing implementations.
void WsfSensorResult::PrintStatus(ut::log::MessageStream& aStream) const
{
   WsfEM_Interaction::PrintStatus(aStream);
   bool detected = ((mFailedStatus == 0) && (mPd >= mRequiredPd));
   aStream.AddNote() << "PD: " << mPd;
   aStream.AddNote() << "Required PD: " << mRequiredPd;
   aStream.AddNote() << "Detected: " << std::boolalpha << detected;
}

// =================================================================================================
//! Print the status flags.
//! Called by the base class PrintStatus() method.
void WsfSensorResult::PrintStatusP(ut::log::MessageStream& aStream) const
{
   WsfEM_Interaction::PrintStatusP(aStream);
   if ((mFailedStatus & cCONCEALMENT) != 0)
   {
      aStream.AddNote() << "Concealed";
   }
   if ((mFailedStatus & cDOPPLER_LIMITS) != 0)
   {
      aStream.AddNote() << "Exceeded_Doppler_Limits";
   }
   if ((mFailedStatus & cVELOCITY_LIMITS) != 0)
   {
      aStream.AddNote() << "Exceeded_Velocity_Limits";
   }
   if ((mFailedStatus & cTARGET_DELETED) != 0)
   {
      aStream.AddNote() << "Target_Deleted";
   }
   if ((mFailedStatus & cOTH_LIMITS) != 0)
   {
      aStream.AddNote() << "Exceeded_OTH_Limits";
   }
   if ((mFailedStatus & cEXCLUSION_SOLAR) != 0)
   {
      aStream.AddNote() << "Solar_Exclusion";
   }
   if ((mFailedStatus & cEXCLUSION_LUNAR) != 0)
   {
      aStream.AddNote() << "Lunar_Exclusion";
   }
   if ((mFailedStatus & cMOON_BLOCKED) != 0)
   {
      aStream.AddNote() << "Blocked_By_Moon";
   }
   if ((mFailedStatus & cDETECTION_STOP) != 0)
   {
      aStream.AddNote() << "Detection_Stopped";
   }
   if ((mFailedStatus & cSCRIPT_CONSTRAINT) != 0)
   {
      aStream.AddNote() << "Blocked_By_Script";
   }
}

// =================================================================================================
//! Reset the detection data for a new detection chance.
//!
//! This only resets those values that are absolutely necessary. Values whose
//! validity can be determined by looking at other values are NOT reset.
// virtual
void WsfSensorResult::Reset()
{
   WsfEM_Interaction::Reset();
   mMeasurement.Reset();
   mModeIndex  = 0;
   mBeamIndex  = 0;
   mPd         = 0.0;
   mRequiredPd = 0.0;
}

// =================================================================================================
//! Reset the detection data for a new detection chance.
//! This is the typical form.
//! @param aSettings The Settings object for the detection chance.
void WsfSensorResult::Reset(const WsfSensor::Settings& aSettings)
{
   Reset();
   mModeIndex  = aSettings.mModeIndex;
   mRequiredPd = aSettings.mRequiredPd;
}

// =================================================================================================
//! Reset the detection data for a new detection chance.
//! This is an alternative form which first initializes the object and then copies
//! common data from an already initialized Result object. This is typically used
//! when a sensor needs to make multiple internal detection chances in a single user
//! invoked detection chance (such as multi-beam, etc.)
//! @param aResult An existing Result from which to copy common data.
void WsfSensorResult::Reset(const WsfSensorResult& aResult)
{
   Reset();
   mModeIndex  = aResult.mModeIndex;
   mRequiredPd = aResult.mRequiredPd;
}

// =================================================================================================
unsigned int WsfSensorResult::BeginOneWayInteraction(WsfEM_Rcvr* aRcvrPtr, WsfPlatform* aTgtPtr)
{
   unsigned int returnCode = WsfEM_Interaction::BeginOneWayInteraction(aRcvrPtr, aTgtPtr);
   if (returnCode == 0)
   {
      ComputeWCS_ToBCS_Transform(*aRcvrPtr, mRcvrToTgt.mAz, mRcvrToTgt.mEl);
      double originatorLocWCS[3];
      aRcvrPtr->GetAntenna()->GetLocationWCS(originatorLocWCS);
      mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
   }
   return returnCode;
}

// =================================================================================================
unsigned int WsfSensorResult::BeginOneWayInteraction(WsfEM_Xmtr* aXmtrPtr,
                                                     WsfEM_Rcvr* aRcvrPtr,
                                                     bool        aCheckXmtrLimits,
                                                     bool        aCheckRcvrLimits,
                                                     bool        aCheckMaskingFactor) // = true
{
   unsigned int returnCode =
      WsfEM_Interaction::BeginOneWayInteraction(aXmtrPtr, aRcvrPtr, aCheckXmtrLimits, aCheckRcvrLimits, aCheckMaskingFactor);
   if (returnCode == 0)
   {
      ComputeWCS_ToBCS_Transform(*aRcvrPtr, mRcvrToXmtr.mAz, mRcvrToXmtr.mEl);
      double originatorLocWCS[3];
      aRcvrPtr->GetAntenna()->GetLocationWCS(originatorLocWCS);
      mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
   }
   return returnCode;
}

// =================================================================================================
unsigned int WsfSensorResult::BeginTwoWayInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr)
{
   unsigned int returnCode = WsfEM_Interaction::BeginTwoWayInteraction(aXmtrPtr, aTgtPtr, aRcvrPtr);
   if (returnCode == 0)
   {
      ComputeWCS_ToBCS_Transform(*aRcvrPtr, mRcvrToTgt.mAz, mRcvrToTgt.mEl);
      double originatorLocWCS[3];
      aRcvrPtr->GetAntenna()->GetLocationWCS(originatorLocWCS);
      mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
   }
   return returnCode;
}

// =================================================================================================
unsigned int WsfSensorResult::BeginTwoWayOTH_Interaction(WsfEM_Xmtr*  aXmtrPtr,
                                                         WsfPlatform* aTgtPtr,
                                                         WsfEM_Rcvr*  aRcvrPtr,
                                                         double       aReflectionLocWCS[3])
{
   unsigned int returnCode = WsfEM_Interaction::BeginTwoWayOTH_Interaction(aXmtrPtr, aTgtPtr, aRcvrPtr, aReflectionLocWCS);
   if (returnCode == 0)
   {
      ComputeWCS_ToBCS_Transform(*aRcvrPtr, mRcvrToTgt.mAz, mRcvrToTgt.mEl);
      double originatorLocWCS[3];
      aRcvrPtr->GetAntenna()->GetLocationWCS(originatorLocWCS);
      mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
   }
   return returnCode;
}

// =================================================================================================
unsigned int WsfSensorResult::BeginGenericInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr)
{
   unsigned int returnCode = WsfEM_Interaction::BeginGenericInteraction(aXmtrPtr, aTgtPtr, aRcvrPtr);
   if (returnCode == 0)
   {
      WsfEM_XmtrRcvr* xmtrRcvr = aRcvrPtr;
      if (xmtrRcvr == nullptr)
      {
         assert(aXmtrPtr != nullptr);
         xmtrRcvr = aXmtrPtr;
      }

      double xmtrRcvrLocWCS[3];
      xmtrRcvr->GetAntenna()->GetLocationWCS(xmtrRcvrLocWCS);
      mMeasurement.SetOriginatorLocationWCS(xmtrRcvrLocWCS);

      // If we choose a generic interaction, we didn't want to pay the computational
      // price of computing rcvrToTgt variables, so we similarly don't compute azimuth, elevation
      // receiver values here.
      ComputeWCS_ToBCS_Transform(*aRcvrPtr, 0.0, 0.0); // use zero azimuth, elevation pointing angle
   }
   return returnCode;
}

void WsfSensorResult::ComputeUndefinedGeometry()
{
   WsfEM_Interaction::ComputeUndefinedGeometry();
   WsfEM_Rcvr* rcvrPtr = GetReceiver();
   if (rcvrPtr != nullptr)
   {
      if (GetTarget() == nullptr)
      {
         if (mRcvrToXmtr.mRange > 0.0)
         {
            ComputeWCS_ToBCS_Transform(*rcvrPtr, mRcvrToXmtr.mAz, mRcvrToXmtr.mEl);
         }
      }
      else if (mRcvrToTgt.mRange > 0.0)
      {
         ComputeWCS_ToBCS_Transform(*rcvrPtr, mRcvrToTgt.mAz, mRcvrToTgt.mEl);
      }
   }
}

// =================================================================================================
// virtual
void WsfSensorResult::ComputeWCS_ToBCS_Transform(const WsfEM_Rcvr& aRcvr, double aRcvrPointingAz, double aRcvrPointingEl)
{
   // Set the rotation of the antenna wrt the WCS frame
   double wcsToPCS_Transform[3][3];
   aRcvr.GetAntenna()->GetArticulatedPart()->GetWCSToPCSTransform(wcsToPCS_Transform);
   if ((aRcvrPointingAz != 0.0) && (aRcvrPointingEl != 0.0))
   {
      double pcsToBCS_Transform[3][3];
      UtEntity::ComputeRotationalTransform(aRcvrPointingAz, aRcvrPointingEl, 0.0, pcsToBCS_Transform);
      double wcsToBCS_Transform[3][3];
      UtMat3d::Multiply(wcsToBCS_Transform, pcsToBCS_Transform, wcsToPCS_Transform);
      mMeasurement.SetOriginatorTransformWCS(wcsToBCS_Transform);
   }
   else
   {
      mMeasurement.SetOriginatorTransformWCS(wcsToPCS_Transform);
   }
}

// ***************************************************************************

//! The script interface 'class'

WsfScriptSensorInteractionClass::WsfScriptSensorInteractionClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptEM_InteractionClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSensorInteraction");
   RegisterMethods(this);
}

void WsfScriptSensorInteractionClass::RegisterMethods(UtScriptClass* aClassPtr)
{
   aClassPtr->AddMethod(ut::make_unique<Detected>());
   aClassPtr->AddMethod(ut::make_unique<ModeName>());
   aClassPtr->AddMethod(ut::make_unique<BeamNumber>());
   aClassPtr->AddMethod(ut::make_unique<Pd>());
   aClassPtr->AddMethod(ut::make_unique<RequiredPd>());
}

// bool detected = <x>.Detected();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorInteractionClass, WsfSensorResult, Detected, 0, "bool", "")
{
   // bool detected = ((aObjectPtr->mCheckedStatus != 0) &&
   //    (aObjectPtr->mFailedStatus  == 0) &&
   //    (aObjectPtr->mPd >= aObjectPtr->mRequiredPd));

   aReturnVal.SetBool(aObjectPtr->Detected());
}

// string modeName = <x>.ModeName();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorInteractionClass, WsfSensorResult, ModeName, 0, "string", "")
{
   WsfStringId modeNameId;
   // This is the hard way, but the most reliable way. The mode pointer in the receiver may be 0.
   WsfEM_Rcvr* rcvrPtr = aObjectPtr->GetReceiver();
   if ((rcvrPtr != nullptr) && (rcvrPtr->GetArticulatedPart() != nullptr))
   {
      WsfSensor* sensorPtr = dynamic_cast<WsfSensor*>(rcvrPtr->GetArticulatedPart());
      if (sensorPtr != nullptr)
      {
         modeNameId = sensorPtr->GetModeName(aObjectPtr->mModeIndex);
      }
   }
   aReturnVal.SetString(modeNameId);
}

// =================================================================================================
//! Reset the detection data for a new detection chance.
//!
//! This only resets those values that are absolutely necessary. Values whose
//! validity can be determined by looking at other values are NOT reset.
// virtual
void WsfLocalSensorResult::Reset()
{
   WsfSensorResult::Reset();
   mMeasurementCount = 0;
}

// int beamNumber = <x>.BeamNumber();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorInteractionClass, WsfSensorResult, BeamNumber, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->mBeamIndex + 1));
}

// double Pd = <x>.Pd();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorInteractionClass, WsfSensorResult, Pd, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mPd);
}

// double requiredPd = <x>.RequiredPd();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorInteractionClass, WsfSensorResult, RequiredPd, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mRequiredPd);
}
