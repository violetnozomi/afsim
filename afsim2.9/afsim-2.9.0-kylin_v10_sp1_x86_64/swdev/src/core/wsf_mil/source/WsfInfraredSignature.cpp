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

#include "WsfInfraredSignature.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "WsfEM_Util.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfInfraredSignatureTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
WsfInfraredSignature::WsfInfraredSignature()
   : WsfSignature()
{
}

// =================================================================================================
// Definition of the default signature to be used if a signature is not defined on a platform
// and a sensor is present that requires the signature.
// =================================================================================================

namespace
{
class DefaultInfraredSignature : public WsfInfraredSignature
{
public:
   DefaultInfraredSignature()
      : WsfInfraredSignature()
   {
   }
   WsfInfraredSignature* Clone() const override { return new DefaultInfraredSignature(); }
   float                 GetSignature(double aSimTime, WsfStringId aStateId, WsfEM_Types::InfraredBand aBand, double aAzimuth, double aElevation) override
   {
      return 1000.0F;
   } // w/sr
};
} // namespace

// =================================================================================================
// Definition of the signature-specific interface class
// =================================================================================================

namespace
{
std::string sInfraredSignatureClassName("infrared_signature");
std::string sShortName("infrared");

class InfraredInterface : public WsfSignatureInterface
{
public:
   InfraredInterface()
      : WsfSignatureInterface()
   {
   }
   InfraredInterface(const InfraredInterface& aSrc) = default;
   std::string            GetClassName() const override { return sInfraredSignatureClassName; }
   std::string            GetShortName() const override { return sShortName; }
   WsfSignatureInterface* Clone() const override { return new InfraredInterface(*this); }
   WsfSignature*          CloneSignature(const WsfScenario& aScenario) const override
   {
      return WsfInfraredSignatureTypes::Get(aScenario).Clone(mInputType);
   }
};
} // namespace

// =================================================================================================
// Static methods to support access to the signature on a platform.
// =================================================================================================

// =================================================================================================
//! Get the input type of the signature.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfInfraredSignature::GetInputType(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetInputType(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Get the current signature state.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfInfraredSignature::GetState(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetState(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signatures state
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aState       [input] The new signatures state.
//! @returns true if successful, false if and error occurred.
bool WsfInfraredSignature::SetState(WsfPlatform* aPlatformPtr, WsfStringId aState)
{
   return aPlatformPtr->GetSignatureList().SetState(cSIGNATURE_INDEX, aState);
}

// =================================================================================================
//! Get the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The signature scale factor.
float WsfInfraredSignature::GetScaleFactor(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetScaleFactor(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aScaleFactor [input] The new signature scale factor.
//! @returns true if successful, false if and error occurred.
bool WsfInfraredSignature::SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor)
{
   return aPlatformPtr->GetSignatureList().SetScaleFactor(cSIGNATURE_INDEX, aScaleFactor);
}

// =================================================================================================
//! Get the signature value for a set of conditions.
//! @param aSimTime   [input] The current simulation time.
//! @param aPlatformPtr [input] The platform from which the signature is to be retrieved.
//! @param aBand        [input] infrared band.
//! @param aAzimuth     [input] azimuth of the observer with respect to this platform (radians).
//! @param aElevation   [input] elevation of the observer with respect to this platform (radians).
//! @returns The infrared signature.
// static
float WsfInfraredSignature::GetValue(double                    aSimTime,
                                     WsfPlatform*              aPlatformPtr,
                                     WsfEM_Types::InfraredBand aBand,
                                     double                    aAzimuth,
                                     double                    aElevation)
{
   InfraredInterface* interfacePtr =
      static_cast<InfraredInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfInfraredSignature* signaturePtr = static_cast<WsfInfraredSignature*>(interfacePtr->GetSignature());
   if (signaturePtr == nullptr)
   {
      // Signature not defined... use the default signature.
      { // RAII block
         auto out = ut::log::warning() << "Platform's Infrared Signature not defined. Using default.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Infrared Signature: " << aPlatformPtr->GetType();
      }
      signaturePtr = new DefaultInfraredSignature();
      interfacePtr->SetSignature(signaturePtr);
   }
   return signaturePtr->GetSignature(aSimTime, interfacePtr->GetState(), aBand, aAzimuth, aElevation) *
          interfacePtr->GetScaleFactor();
}

// =================================================================================================
// Script Interface
// =================================================================================================

namespace
{

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, InfraredSigState, 0, "string", "")
{
   aReturnVal.SetString(WsfInfraredSignature::GetState(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, SetInfraredSigState, 1, "void", "string")
{
   WsfStringId stateId = WsfStringId(aVarArgs[0].GetString());
   WsfInfraredSignature::SetState(aObjectPtr, stateId);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, InfraredSigScaleFactor, 0, "double", "")
{
   aReturnVal.SetDouble(WsfInfraredSignature::GetScaleFactor(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, SetInfraredSigScaleFactor, 1, "void", "double")
{
   float scaleFactor = static_cast<float>(aVarArgs[0].GetDouble());
   WsfInfraredSignature::SetScaleFactor(aObjectPtr, scaleFactor);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, InfraredRadiantIntensity, 3, "double", "WsfGeoPoint, double, string")
{
   WsfGeoPoint* viewPointPtr     = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   earthRadiusScale              = (earthRadiusScale > 0.0) ? earthRadiusScale : 1.0; // Use default if requested
   WsfEM_Types::InfraredBand waveBand;
   if (!WsfEM_Util::StringToEnum(waveBand, aVarArgs[2].GetString()))
   {
      waveBand = WsfEM_Types::cIRBAND_DEFAULT;
   }

   // Compute the azimuth and elevation of the viewpoint with respect to the target (this platform).
   double viewerLocWCS[3];
   viewPointPtr->GetLocationWCS(viewerLocWCS);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Update(simTime);
   double targetToViewerAz;
   double targetToViewerEl;
   WsfEM_Util::ComputeViewerAspect(aObjectPtr, viewerLocWCS, earthRadiusScale, targetToViewerAz, targetToViewerEl);

   double value = WsfInfraredSignature::GetValue(simTime, aObjectPtr, waveBand, targetToViewerAz, targetToViewerEl);
   aReturnVal.SetDouble(value);
}

} // End anonymous namespace

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfInfraredSignature::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<InfraredSigState>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<SetInfraredSigState>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<InfraredSigScaleFactor>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<SetInfraredSigScaleFactor>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<InfraredRadiantIntensity>());
}

// =================================================================================================
//! Register the platform interface object on the prototype signature list in the specified scenario.
// static
void WsfInfraredSignature::RegisterInterface(WsfScenario& aScenario)
{
   aScenario.GetSignatureListPrototype().AddInterface(new InfraredInterface(), cSIGNATURE_INDEX);
}
